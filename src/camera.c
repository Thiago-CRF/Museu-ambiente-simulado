#include <GL/glut.h>
#include <math.h>

#include "camera.h"
#include "curvas.h"

#define VELOCIDADE_MOVIMENTO 4.0f // unidades por segundo para movimento
#define SENSIBILIDADE_MOUSE 0.15f
#define ALTURA_OLHOS 1.7f
#define VELOCIDADE_TOUR 0.2f // fração de trecho percorrida por segundo

#define NUM_TRECHOS 4

// struct com os valores das regioes navegaveis do museu, plano XZ
typedef struct {
    float xmin, xmax, zmin, zmax;
} RegiaoNavegavel;

#define NUM_REGIOES 3   // 3 pois tem duas salas e um corredor

static const RegiaoNavegavel regioes[NUM_REGIOES] = {
    // sala 1, encolhida pela margem pra camera nao encostar na parede
    { S1_XMIN + MARGEM_COLISAO, S1_XMAX - MARGEM_COLISAO,
      S1_ZMIN + MARGEM_COLISAO, S1_ZMAX - MARGEM_COLISAO },

    // corredor: avanca 1 unidade pra dentro das duas salas pras
    // portas nao ficarem bloqueadas pela margem das regioes vizinhas
    { C_XMIN - 1.0f, C_XMAX + 1.0f,
      C_ZMIN + MARGEM_COLISAO, C_ZMAX - MARGEM_COLISAO },

    // sala 2
    { S2_XMIN + MARGEM_COLISAO, S2_XMAX - MARGEM_COLISAO,
      S2_ZMIN + MARGEM_COLISAO, S2_ZMAX - MARGEM_COLISAO }
};

static Camera cam;
static int teclas[256]; // estado das teclas (1: pressionada)
static int ignorar_prox_mouse = 0; // evita loop quando recentraliza o cursor

// caminho do tour, com 4 curvas de bezier que formam o circuito
static CurvaBezier caminho_tour[NUM_TRECHOS];
static int trecho_atual = 0;
static float t_atual = 0.0f;

// verifica se a posição esta dentro de alguma regiao navegavel
static int posicao_valida(float x, float z) {
    for (int i = 0; i < NUM_REGIOES; i++) {
        if (x >= regioes[i].xmin && x <= regioes[i].xmax &&
            z >= regioes[i].zmin && z <= regioes[i].zmax) {
            return 1;
        }
    }
    return 0;
}

// monta os pontos de controle do circuito do tour
static void montar_caminho_tour(void) {
    float y = ALTURA_OLHOS;
    float r = 6.0f; // raio do circuito
    float k = 3.31f;    // deslocamento dos pontos de controle (aproxima um circulo)

    caminho_tour[0] = (CurvaBezier){{ r, y, 0}, { r, y, k}, { k, y, r}, { 0, y, r}};
    caminho_tour[1] = (CurvaBezier){{ 0, y, r}, {-k, y, r}, {-r, y, k}, {-r, y, 0}};
    caminho_tour[2] = (CurvaBezier){{-r, y, 0}, {-r, y,-k}, {-k, y,-r}, { 0, y,-r}};
    caminho_tour[3] = (CurvaBezier){{ 0, y,-r}, { k, y,-r}, { r, y,-k}, { r, y, 0}};
}

// calcula o vetor de direcao a partir dos angulos da camera
static void direcao_camera(float *dx, float *dy, float *dz) {
    float radYaw = cam.yaw * M_PI / 180.0f; // M_PI da biblioteca math
    float radPitch = cam.pitch * M_PI / 180.0f;

    *dx = cosf(radYaw) * cosf(radPitch);
    *dy = sinf(radPitch);
    *dz = sinf(radYaw) * cosf(radPitch);
}

void camera_iniciar(void) {
    // comeca no centro da sala 1, olhando na direcao do corredor
    cam.x = -15.0f;
    cam.y = ALTURA_OLHOS;
    cam.z = 0.0f;
    cam.yaw = 0.0f;   // olhando pra +x
    cam.pitch = 0.0f;
    cam.modo = CAMERA_MODO_LIVRE;

    // zera estado das tecla
    for (int i = 0; i < 256; i++) teclas[i] = 0;

    montar_caminho_tour();
    trecho_atual = 0;
    t_atual = 0.0f;
}

// função de movimentação livre com WASD, e colisão caso chegue na parede
// float dt: delta tempo
static void atualizar_modo_livre(float dt) {
    float dx, dy, dz;   // variaveis q apontam pra onde a camera ta olhando, forward, 
    // definindo os valores com direcao_camera()
    direcao_camera(&dx, &dy, &dz);

    // projeta a direcao no plano xz, pra não considerar o y pois se eu andar pra frente olhando pra cima eu voo
    float fx = dx, fz = dz;
    // pega o comprimento do vetor
    float comprimento = sqrtf(fx * fx + fz * fz);
    // evita divisão por 0 quando ta olhando com pitch 90 graus
    if (comprimento > 0.0001f) { 
        // normaliza o vetor, pra o quanto que movimenta depender da velocidade
        fx /= comprimento; fz /= comprimento; }

    // vetor perpendicular (pra direita, pra mover pro lados com A e D)
    float rx = -fz, rz = fx;
    
    // variaveis pra atualizar os valores no final
    float novoX = cam.x;
    float novoZ = cam.z;
    float passo = VELOCIDADE_MOVIMENTO * dt;

    if (teclas['w'] || teclas['W']) { 
        novoX += fx * passo; novoZ += fz * passo; 
    }
    if (teclas['s'] || teclas['S']) { 
        novoX -= fx * passo; novoZ -= fz * passo; 
    }
    if (teclas['a'] || teclas['A']) { 
        novoX -= rx * passo; novoZ -= rz * passo; 
    }
    if (teclas['d'] || teclas['D']) { 
        novoX += rx * passo; novoZ += rz * passo; 
    }

    // seção de teste de colisão da camera
    // testa cada eixo separadamente pra poder deslizar ao encostar na parede em vez de travar o movimento
    if (posicao_valida(novoX, cam.z)) {
        cam.x = novoX;
    }
    if (posicao_valida(cam.x, novoZ)) {
        cam.z = novoZ;
    }

    cam.y = ALTURA_OLHOS;
}

// movimentacao automatica seguindo as curvas de bezier
static void atualizar_modo_tour(float dt) {
    t_atual += VELOCIDADE_TOUR * dt;

    // qauando terminar um trecho, passa pro proximo (voltando ao inicio no fim)
    while (t_atual >= 1.0f) {
        t_atual -= 1.0f;
        trecho_atual = (trecho_atual + 1) % NUM_TRECHOS;
    }

    Vetor3 posicao = curva_avaliar(caminho_tour[trecho_atual], t_atual);

    // calcula um ponto um pouco a frente pra saber pra onde olhar
    float t_frente = t_atual + 0.02f;
    int trecho_frente = trecho_atual;
    if (t_frente >= 1.0f) {
        t_frente -= 1.0f;
        trecho_frente = (trecho_atual + 1) % NUM_TRECHOS;
    }

    Vetor3 alvo = curva_avaliar(caminho_tour[trecho_frente], t_frente);

    cam.x = posicao.x;
    cam.y = posicao.y;
    cam.z = posicao.z;
}

void camera_atualizar(float dt){
    if(cam.modo == CAMERA_MODO_TOUR){
        // se o usuario mexer no WASD ele assume o controle 
        if(teclas['w'] || teclas['a'] || teclas['s'] || teclas['d'] ||
           teclas['W'] || teclas['A'] || teclas['S'] || teclas['D']) {
            cam.modo = CAMERA_MODO_LIVRE;
        }
        else {
            atualizar_modo_tour(dt);
            return;
        }
    }

    atualizar_modo_livre(dt);
}

void camera_aplicar_visualizacao(void) {
    // pega pra onde a camera esta olhando e aplica no gluLookAt
    float dx, dy, dz;
    direcao_camera(&dx, &dy, &dz);

    gluLookAt(cam.x, cam.y, cam.z,
              cam.x + dx, cam.y + dy, cam.z + dz,
              0.0, 1.0, 0.0);
}

void camera_alternar_modo(void) {
    if (cam.modo == CAMERA_MODO_LIVRE) {
        cam.modo = CAMERA_MODO_TOUR;
        trecho_atual = 0;
        t_atual = 0.0f;
    } 
    else {
        cam.modo = CAMERA_MODO_LIVRE;
    }
}

void camera_processar_mouse(int x, int y, int largura_janela, int altura_janela) {
    // mouse não controla visao no modo tour
    if(cam.modo == CAMERA_MODO_TOUR)
        return;


    // processa o ignorar mouse pra quando o modo de camera mudar não ter bug visual
    if(ignorar_prox_mouse){
        ignorar_prox_mouse = 0;
        return;
    }

    int centro_x = largura_janela / 2;
    int centro_y = altura_janela / 2;

    int desloc_x = x - centro_x;
    int desloc_y = y - centro_y;

    if (desloc_x == 0 && desloc_y == 0) 
        return;

    cam.yaw += desloc_x * SENSIBILIDADE_MOUSE;
    cam.pitch -= desloc_y * SENSIBILIDADE_MOUSE;

    // trava a inclinacao pra nao virar a camera de cabeca pra baixo
    if (cam.pitch > 89.0f) {
        cam.pitch = 89.0f;
    }
    if (cam.pitch < -89.0f) {
        cam.pitch = -89.0f;
    }

    ignorar_prox_mouse = 1;
    glutWarpPointer(centro_x, centro_y);
}

// registram as teclas sendo clicadas e soltas

void camera_tecla_pressionada(unsigned char tecla) {
    teclas[tecla] = 1;
}

void camera_tecla_solta(unsigned char tecla) {
    teclas[tecla] = 0;
}