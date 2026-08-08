#include <GL/glut.h>
#include <math.h>

#include "camera.h"
#include "curvas.h"

#define VELOCIDADE_MOVIMENTO 4.0f // unidades por segundo para movimento
#define SENSIBILIDADE_MOUSE 0.15f
#define ALTURA_OLHOS 1.7f
#define VELOCIDADE_TOUR 0.2f // fração de trecho percorrida por segundo

#define NUM_TRECHOS 11 // quantos pontos tem o caminho do tour
#define TENSAO_TOUR 0.5f // tensão da curva de bézier 

// define um ponto do tour; onde a camera passa e para onde olha
typedef struct
{
    Vetor3 posicao;
    Vetor3 alvo;
} PontoTour;

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

// caminho do tour, pontos definidos na mao e curvas geradas com base neles
static PontoTour pontos_tour[NUM_TRECHOS];
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

// gera os pontos de controle de curva de bézier a partir dos pontos do percurso definidos
// a tangente em cada ponto vem da direção entreo o proximo vizinho e anterior
// pra que os trechos se emendem sem um movimento brusco
void gerar_curvas_tour() {
    for(int i = 0; i< NUM_TRECHOS; i++){
        // indices dos vizinho, usando % pra curva fechar em circuito
        int anterior  = (i - 1 + NUM_TRECHOS) % NUM_TRECHOS;
        int seguinte  = (i + 1) % NUM_TRECHOS;
        int posterior = (i + 2) % NUM_TRECHOS;
        
        Vetor3 ponto_ant = pontos_tour[anterior].posicao;
        Vetor3 ponto_ini = pontos_tour[i].posicao;
        Vetor3 ponto_fim = pontos_tour[seguinte].posicao;
        Vetor3 ponto_post= pontos_tour[posterior].posicao;

        // o trecho começa e trermina nos pontos do circuito
        caminho_tour[i].p0 = ponto_ini;
        caminho_tour[i].p3 = ponto_fim;

        float fator = TENSAO_TOUR / 3.0f; // fator de curva da curva de bezier

        // p1 sai de ponto_ini na direcao (ponto_fim - ponto_ant)
        caminho_tour[i].p1.x = ponto_ini.x + (ponto_fim.x - ponto_ant.x) * fator;
        caminho_tour[i].p1.y = ponto_ini.y + (ponto_fim.y - ponto_ant.y) * fator;
        caminho_tour[i].p1.z = ponto_ini.z + (ponto_fim.z - ponto_ant.z) * fator;

        // p2 chega em ponto_fim na direcao (ponto_post - ponto_ini)
        caminho_tour[i].p2.x = ponto_fim.x - (ponto_post.x - ponto_ini.x) * fator;
        caminho_tour[i].p2.y = ponto_fim.y - (ponto_post.y - ponto_ini.y) * fator;
        caminho_tour[i].p2.z = ponto_fim.z - (ponto_post.z - ponto_ini.z) * fator;        
    }
}

// monta os pontos de controle do circuito do tour
static void montar_caminho_tour(void) {
    /*
    pontos do percurso:
    0-3: contorno da sala 1; olhando para o centro da sala
    4-5: corredor ida; olhando para o fundo/centro da sala 2
    6-9: contorno da sala 2 (losango); olhando para cada quadro por vez
    10: corredor volta; olhando pro fundo/centro da sala 1

    na sala 2 ficar 4 ou 5 unidades da parede
    */

    float y = ALTURA_OLHOS;
    
    // alvos que são reaproveitados em varios pontos
    Vetor3 centro_sala1 = { -15.0f, 2.0f, 0.0f }; // meio da sala das esculturas
    Vetor3 fundo_sala2  = {  25.0f, 2.0f, 0.0f }; // parede do fundo da sala 2
    Vetor3 fundo_sala1  = { -25.0f, 2.0f, 0.0f }; // parede do fundo da sala 1

    // posicoes dos quadros, as mesmas usadas em exibicoes.c 
    // MUDAR CASO QUEIRA COLOCAR EM MAIS QUADROS ESPECIFICOS, com base em exibicoes.c
    Vetor3 quadro_fundo    = { 15.0f, 3.0f, -9.9f };
    Vetor3 quadro_direita  = { 24.9f, 3.0f, 0.0f };
    Vetor3 quadro_frente   = { 15.0f, 3.0f, 9.9f };
    Vetor3 quadro_esquerda = { 5.1f, 3.0f, -6.5f };

    // sala 1: contorna perto das paredes sempre olhando pro centro das esculturas
    pontos_tour[0] = (PontoTour){ {-8.0f, y, 6.0f}, centro_sala1 };
    pontos_tour[1] = (PontoTour){ {-21.0f, y, 6.0f}, centro_sala1 };
    pontos_tour[2] = (PontoTour){ {-21.0f, y, -6.0f}, centro_sala1 };
    pontos_tour[3] = (PontoTour){ {-8.0f, y, -6.0f}, centro_sala1 };

    // corredor de ida: entra pela porta olhando pra sala 2
    pontos_tour[4] = (PontoTour){ {-7.0f, y, -1.0f}, fundo_sala2 };
    pontos_tour[5] = (PontoTour){ {8.0f, y, -1.0f}, fundo_sala2 };

    // sala 2: percorre o meio da sala virando pra cada parede com quadro
    pontos_tour[6] = (PontoTour){ {15.0f, y, -6.0f}, quadro_fundo };
    pontos_tour[7] = (PontoTour){ {20.0f, y, 0.0f}, quadro_direita };
    pontos_tour[8] = (PontoTour){ {15.0f, y, 6.0f}, quadro_frente };
    pontos_tour[9] = (PontoTour){ {10.0f, y, 1.0f}, quadro_esquerda };

    // corredor de volta: atravessa por outra faixa de z e olha pra sala 1
    pontos_tour[10] = (PontoTour){ {-7.0f, y, 1.0f}, fundo_sala1 };

    gerar_curvas_tour();
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

    // quaando terminar um trecho, passa pro proximo (voltando ao inicio no fim)
    while (t_atual >= 1.0f) {
        t_atual -= 1.0f;
        trecho_atual = (trecho_atual + 1) % NUM_TRECHOS;
    }

    Vetor3 posicao = curva_avaliar(caminho_tour[trecho_atual], t_atual);

    cam.x = posicao.x;
    cam.y = posicao.y;
    cam.z = posicao.z;

    // muda o alvo do ponto atual pro proximo ponto ao longo do caminho
    int seguinte = (trecho_atual + 1) % NUM_TRECHOS;
    Vetor3 alvo_ini = pontos_tour[trecho_atual].alvo;
    Vetor3 alvo_fim = pontos_tour[seguinte].alvo;

    // faz o giro começar e terminar devagar em vez de a mesma velocidade
    float smooth = t_atual * t_atual * (3.0f - 2.0f * t_atual);

    Vetor3 alvo;
    alvo.x = alvo_ini.x + (alvo_fim.x - alvo_ini.x) * smooth;
    alvo.y = alvo_ini.y + (alvo_fim.y - alvo_ini.y) * smooth;
    alvo.z = alvo_ini.z + (alvo_fim.z - alvo_ini.z) * smooth;

    // converte a direcao ate o alvo nos angulos que a camera usa
    // comprimento é a distancia entre a camera e o alvo
    float dx = alvo.x - cam.x;
    float dy = alvo.y - cam.y;
    float dz = alvo.z - cam.z;
    float comprimento = sqrtf(dx * dx + dy * dy + dz * dz);

    // define yaw e pitch com base no comprimento, normalizando os valores com o comprimento
    // ignora valores muito próximos de 0
    if (comprimento > 0.0001f) {
        cam.yaw = atan2f(dz, dx) * 180.0f / M_PI;
        cam.pitch = asinf(dy / comprimento) * 180.0f / M_PI;
    }
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