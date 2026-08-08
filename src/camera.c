#include <GL/glut.h>
#include <math.h>

#include "camera.h"
#include "curvas.h"

#define VELOCIDADE_MOVIMENTO 4.0f // unidades por segundo para movimento
#define SENSIBILIDADE_MOUSE 0.15f
#define ALTURA_OLHOS 1.7f
#define VELOCIDADE_TOUR 0.2f // fração de trecho percorrida por segundo

#define NUM_TRECHO 4

static Camera cam;
static int teclas[256]; // estado das teclas (1: pressionada)
static int ignorar_prox_mouse = 0; // evita loop quando recentraliza o cursor

// caminho do tour, com 4 curvas de bezier que formam o circuito
static CurvaBezier caminho_tour[NUM_TRECHO];
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