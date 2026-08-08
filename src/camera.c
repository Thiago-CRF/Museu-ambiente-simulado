#include <GL/glut.h>
#include <math.h>

#include "camera.h"
#include "curvas.h"

#define VELOCIDADE_MOVIMENTO 4.0f // unidades por segundo para movimento
#define SENSIBILIDADE_MOUSE 0.15f
#define ALTURA_OLHOS 1.7f
#define VELOCIDADE_TOUR 0.2f // fração de trecho percorrida por segundo

#define NUM_TRECHOS 4

static Camera cam;
static int teclas[256]; // estado das teclas (1: pressionada)
static int ignorar_prox_mouse = 0; // evita loop quando recentraliza o cursor

// caminho do tour, com 4 curvas de bezier que formam o circuito
static CurvaBezier caminho_tour[NUM_TRECHOS];
static int trecho_atual = 0;
static float t_atual = 0.0f;

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
    cam.x = 0.0f;
    cam.y = ALTURA_OLHOS;
    cam.z = 8.0f;
    cam.yaw = -90.0f;   // olhando pra -z
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
    float rx = fz, rz = -fx;
    
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

    // colisao simples com as paredes
    if (novoX < SALA_MIN_X) novoX = SALA_MIN_X;
    if (novoX > SALA_MAX_X) novoX = SALA_MAX_X;
    if (novoZ < SALA_MIN_Z) novoZ = SALA_MIN_Z;
    if (novoZ > SALA_MAX_Z) novoZ = SALA_MAX_Z;

    cam.x = novoX;
    cam.z = novoZ;
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
}