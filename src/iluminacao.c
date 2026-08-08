#include <GL/glut.h>

#include "iluminacao.h"
#include "common.h"
#include "camera.h"

#define NUM_LUZES_MUSEU 9   // 4 spots de estatua + 3 do corredor + 2 lustres
#define MAX_LUZES_GL 8  // o opengl 2.1 so tem o GL_LIGHT0 ate GL_LIGHT7

// ambientes do museu, usados pra decidir quais luzes ficam acesas
typedef enum {
    AMBIENTE_SALA1,
    AMBIENTE_CORREDOR,
    AMBIENTE_SALA2
} Ambiente;

// descricao de uma luz do museu, independente do slot do opengl que ela vai ocupar (pra ficar dinamico)
typedef struct {
    Vetor3 posicao;
    Vetor3 direcao;
    int e_spot;        // 1 = spot (cone), 0 = pontual (lustre)
    Ambiente ambiente;
} LuzMuseu;

// define onde fica cada luz do museu a ordem importa:
// quando um ambiente nao couber inteiro nos slots restantes,
// as luzes do fim da lista dele sao as primeiras a ficarem de fora
static void montar_luzes(void) {
    // sala 1: o lustre vem antes das outras pra nunca ser cortado
    luzes[0] = (LuzMuseu){ { -15.0f, 6.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, 0, AMBIENTE_SALA1 };

    // spots de cada estatua, nas mesmas posicoes x/z usadas em exibicoes.c
    luzes[1] = (LuzMuseu){ {-19.0f, 5.5f, -4.0f}, {0.0f, -1.0f, 0.0f}, 1, AMBIENTE_SALA1 };
    luzes[2] = (LuzMuseu){ {-11.0f, 5.5f, -4.0f}, {0.0f, -1.0f, 0.0f}, 1, AMBIENTE_SALA1 };
    luzes[3] = (LuzMuseu){ {-19.0f, 5.5f, 4.0f}, {0.0f, -1.0f, 0.0f}, 1, AMBIENTE_SALA1 };
    luzes[4] = (LuzMuseu){ {-11.0f, 5.5f, 4.0f}, {0.0f, -1.0f, 0.0f}, 1, AMBIENTE_SALA1 };

    // spots do corredor, mais baixos porque o pe-direito ali e menor
    luzes[5] = (LuzMuseu){ {-3.3f, 3.6f, 0.0f}, {0.0f, -1.0f, 0.0f}, 1, AMBIENTE_CORREDOR };
    luzes[6] = (LuzMuseu){ {0.0f, 3.6f, 0.0f}, {0.0f, -1.0f, 0.0f}, 1, AMBIENTE_CORREDOR };
    luzes[7] = (LuzMuseu){ {3.3f, 3.6f, 0.0f}, {0.0f, -1.0f, 0.0f}, 1, AMBIENTE_CORREDOR };

    // sala 2: por enquanto so o lustre, os spots dos quadros vão ser feitos depois (talvez não sejam individuais)
    luzes[8] = (LuzMuseu){ {15.0f, 6.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, 0, AMBIENTE_SALA2 };
}



static LuzMuseu luzes[NUM_LUZES_MUSEU];

void iluminacao_iniciar(){
    glEnable(GL_LIGHTING);

    // faz com que glColor3f contie definindo a cor do material
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // brilho especular padrao dos materiais (no momento esta o mesmo brilho pra todos materiais)
    // mudar isso dentro de exibicoes.c, chamando glMaterialfv individualmente pra cada item de exibição
    GLfloat especular_material[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, especular_material);
    glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);

    // --- PONTOS DE LUZ
    // luz 0: iluminaçao geral da sala, pontual no centro
    GLfloat luz0_ambiente[]  = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat luz0_difusa[]    = { 0.55f, 0.55f, 0.50f, 1.0f };
    GLfloat luz0_especular[] = { 0.3f, 0.3f, 0.3f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, luz0_ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luz0_difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luz0_especular);
    glEnable(GL_LIGHT0);

    // luz 1: spot quente sobre estatuas no fundo (apenas base, mudar quando mudar as posições)
    GLfloat luz1_difusa[]    = { 1.0f, 0.9f, 0.7f, 1.0f };
    GLfloat luz1_especular[] = { 1.0f, 1.0f, 0.9f, 1.0f };

    glLightfv(GL_LIGHT1, GL_DIFFUSE, luz1_difusa);
    glLightfv(GL_LIGHT1, GL_SPECULAR, luz1_especular);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 35.0f);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 8.0f);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.05f);
    glEnable(GL_LIGHT1);

    // luz 2: spot frio sobre estatuas da frente (tambem apenas exemplo base, mudar quando mudar os itens do museu)
    GLfloat luz2_difusa[]    = { 0.7f, 0.8f, 1.0f, 1.0f };
    GLfloat luz2_especular[] = { 0.9f, 0.9f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT2, GL_DIFFUSE, luz2_difusa);
    glLightfv(GL_LIGHT2, GL_SPECULAR, luz2_especular);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 35.0f);
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 8.0f);
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.05f);
    glEnable(GL_LIGHT2);
}

void iluminacao_atualizar(){
    // as posições sao definidas todo frame porque depende da matriz modelview
    // precisa chamar depois de camera_aplicar_visualizacao()

    // modificar as posições base e adicionar para cada ponto de luz novo

    GLfloat posicao_luz0[] = { 0.0f, 5.5f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz0);

    GLfloat posicao_luz1[]  = { 0.0f, 5.5f, -4.0f, 1.0f };
    GLfloat direcao_luz1[]  = { 0.0f, -1.0f, 0.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, posicao_luz1);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direcao_luz1);

    GLfloat posicao_luz2[]  = { 0.0f, 5.5f, 4.0f, 1.0f };
    GLfloat direcao_luz2[]  = { 0.0f, -1.0f, 0.0f };
    glLightfv(GL_LIGHT2, GL_POSITION, posicao_luz2);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, direcao_luz2);
}