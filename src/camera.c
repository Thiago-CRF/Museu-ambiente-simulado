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

