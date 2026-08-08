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

// joga os parametros de uma luz do museu num slot do opengl
// todos os parametros sao setados aqui e nao no iniciar, porque o mesmo slot
// pode ser um spot de estatua num frame e um spot de corredor no seguinte
static void aplicar_luz(GLenum slot, LuzMuseu luz) {
    // o w = 1.0 no fim indica que a luz é posicional, e nao direcional
    GLfloat posicao[] = { luz.posicao.x, luz.posicao.y, luz.posicao.z, 1.0f };
    glLightfv(slot, GL_POSITION, posicao);

    if (luz.e_spot) {
        // luz quente dos holofotes
        GLfloat difusa[]    = { 1.00f, 0.85f, 0.60f, 1.0f };
        GLfloat especular[] = { 1.00f, 0.95f, 0.85f, 1.0f };
        GLfloat direcao[]   = { luz.direcao.x, luz.direcao.y, luz.direcao.z };

        glLightfv(slot, GL_DIFFUSE, difusa);
        glLightfv(slot, GL_SPECULAR, especular);
        glLightfv(slot, GL_SPOT_DIRECTION, direcao);
        glLightf(slot, GL_SPOT_CUTOFF, 30.0f);  // abertura do cone em graus
        glLightf(slot, GL_SPOT_EXPONENT, 10.0f); // concentracao no centro do cone
        glLightf(slot, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(slot, GL_LINEAR_ATTENUATION, 0.08f);
    }
    else {
        // lustre: luz pontual, quente mas mais fraca e espalhada
        GLfloat difusa[]    = { 0.90f, 0.85f, 0.72f, 1.0f };
        GLfloat especular[] = { 0.40f, 0.40f, 0.35f, 1.0f };

        glLightfv(slot, GL_DIFFUSE, difusa);
        glLightfv(slot, GL_SPECULAR, especular);
        glLightf(slot, GL_SPOT_CUTOFF, 180.0f);  // 180 desliga o cone e vira pontual
        glLightf(slot, GL_SPOT_EXPONENT, 0.0f);
        glLightf(slot, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(slot, GL_LINEAR_ATTENUATION, 0.02f);
    }

    glEnable(slot);
}

// descobre em qual ambiente a camera esta, so pelo x (ja que todo o ambiente ta no mesmo eixo x)
static Ambiente ambiente_da_camera(Vetor3 posicao) {
    if (posicao.x < C_XMIN) return AMBIENTE_SALA1;
    if (posicao.x > C_XMAX) return AMBIENTE_SALA2;
    return AMBIENTE_CORREDOR;
}

// monta a ordem de prioridade dos ambientes: o atual primeiro, depois os vizinhos
// assim as luzes que da pra ver pela porta continuam acesas se ainda houver slot
static void ordem_de_prioridade(Ambiente atual, Ambiente ordem[3]) {
    if (atual == AMBIENTE_SALA1) {
        ordem[0] = AMBIENTE_SALA1;
        ordem[1] = AMBIENTE_CORREDOR;
        ordem[2] = AMBIENTE_SALA2;
    }
    else if (atual == AMBIENTE_SALA2) {
        ordem[0] = AMBIENTE_SALA2;
        ordem[1] = AMBIENTE_CORREDOR;
        ordem[2] = AMBIENTE_SALA1;
    }
    else {
        ordem[0] = AMBIENTE_CORREDOR;
        ordem[1] = AMBIENTE_SALA2;
        ordem[2] = AMBIENTE_SALA1;
    }
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