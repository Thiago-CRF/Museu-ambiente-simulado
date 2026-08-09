#include <GL/glut.h>

#include "iluminacao.h"
#include "common.h"
#include "camera.h"

#define NUM_LUZES_MUSEU 9   // 4 spots de estatua + 3 do corredor + 2 lustres
#define MAX_LUZES_GL 8  // o opengl 2.1 so tem o GL_LIGHT0 ate GL_LIGHT7

// medidas do objeto que representa cada spot (apenas para desenhar)
// as luzes ficam 0.1 abaixo do teto, entao a caixa preenche essa folga
#define SPOT_LADO_CAIXA    0.5f // tamanho do quadrado do spot
#define SPOT_ALTURA_CAIXA  0.12f // altura do quadrado do spot
#define SPOT_RAIO_LAMPADA  0.16f // raio da lampada do spot

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

static LuzMuseu luzes[NUM_LUZES_MUSEU];

// define onde fica cada luz do museu a ordem importa:
// quando um ambiente nao couber inteiro nos slots restantes,
// as luzes do fim da lista dele sao as primeiras a ficarem de fora
static void montar_luzes(void) {
    // sala 1: o lustre vem antes das outras pra nunca ser cortado
    luzes[0] = (LuzMuseu){ { -15.0f, 6.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, 0, AMBIENTE_SALA1 };

    // spots de cada estatua, nas mesmas posicoes x/z usadas em exibicoes.c
    luzes[1] = (LuzMuseu){ {-19.0f, 6.9f, -4.0f}, {0.0f, -1.0f, 0.0f}, 1, AMBIENTE_SALA1 };
    luzes[2] = (LuzMuseu){ {-11.0f, 6.9f, -4.0f}, {0.0f, -1.0f, 0.0f}, 1, AMBIENTE_SALA1 };
    luzes[3] = (LuzMuseu){ {-19.0f, 6.9f, 4.0f}, {0.0f, -1.0f, 0.0f}, 1, AMBIENTE_SALA1 };
    luzes[4] = (LuzMuseu){ {-11.0f, 6.9f, 4.0f}, {0.0f, -1.0f, 0.0f}, 1, AMBIENTE_SALA1 };

    // spots do corredor, mais baixos porque o pe-direito ali e menor
    luzes[5] = (LuzMuseu){ {-3.3f, 3.9f, 0.0f}, {0.0f, -1.0f, 0.0f}, 1, AMBIENTE_CORREDOR };
    luzes[6] = (LuzMuseu){ {0.0f, 3.9f, 0.0f}, {0.0f, -1.0f, 0.0f}, 1, AMBIENTE_CORREDOR };
    luzes[7] = (LuzMuseu){ {3.3f, 3.9f, 0.0f}, {0.0f, -1.0f, 0.0f}, 1, AMBIENTE_CORREDOR };

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
        GLfloat difusa[]    = { 1.20f, 1.0f, 0.70f, 1.0f };
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
        // lustre: luz pontual, fria e mais fraca e espalhada (luz principal da sala)
        GLfloat difusa[]    = { 0.50f, 0.50f, 0.50f, 1.0f };
        GLfloat especular[] = { 0.30f, 0.30f, 0.30f, 1.0f };

        glLightfv(slot, GL_DIFFUSE, difusa);
        glLightfv(slot, GL_SPECULAR, especular);
        glLightf(slot, GL_SPOT_CUTOFF, 180.0f);  // 180 desliga o cone e vira pontual
        glLightf(slot, GL_SPOT_EXPONENT, 0.0f);
        glLightf(slot, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(slot, GL_LINEAR_ATTENUATION, 0.05f);
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

// desenha o corpo de um spot: uma caixa preta com uma lampada redonda embaixo
static void desenhar_corpo_spot(Vetor3 posicao) {
    GLUquadric *q = gluNewQuadric();

    glPushMatrix();
        glTranslatef(posicao.x, posicao.y, posicao.z);

        // caixa preta do suporte, deslocada pra cima pra encostar no teto
        glColor3f(0.08f, 0.08f, 0.08f);
        glPushMatrix();
            glTranslatef(0.0f, SPOT_ALTURA_CAIXA / 2.0f - 0.02f, 0.0f);
            glScalef(SPOT_LADO_CAIXA, SPOT_ALTURA_CAIXA, SPOT_LADO_CAIXA);
            glutSolidCube(1.0);
        glPopMatrix();

        // lampada é só um disco com emissao, logo abaixo da face de baixo da caixa
        GLfloat emissao[] = { 1.0f, 0.95f, 0.85f, 1.0f };
        glMaterialfv(GL_FRONT, GL_EMISSION, emissao);

        glColor3f(1.0f, 1.0f, 1.0f);
        glPushMatrix();
            glTranslatef(0.0f, -0.03f, 0.0f);
            // o gluDisk fica no plano z = 0 com a normal em +z, entao gira pra
            // essa normal apontar pra baixo e o disco ser visto de baixo
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            gluDisk(q, 0.0, SPOT_RAIO_LAMPADA, 24, 1);
        glPopMatrix();

        // reset pois o glMaterialfv persiste e deixaria tudo brilhando depois
        GLfloat sem_emissao[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glMaterialfv(GL_FRONT, GL_EMISSION, sem_emissao);

    glPopMatrix();

    gluDeleteQuadric(q);
}

// desenha o corpo de todos os spots, nas mesmas posicoes das luzes
void iluminacao_desenhar_spots(void) {
    for (int i = 0; i < NUM_LUZES_MUSEU; i++) {
        if (luzes[i].e_spot) {
            desenhar_corpo_spot(luzes[i].posicao);
        }
    }
}

void iluminacao_iniciar(){
    glEnable(GL_LIGHTING);

    // faz com que glColor3f continue definindo a cor do material
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // brilho especular padrao dos materiais (no momento esta o mesmo brilho pra todos materiais)
    // mudar isso dentro de exibicoes.c, chamando glMaterialfv individualmente pra cada item de exibição
    GLfloat especular_material[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, especular_material);
    glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);

    // luz ambiente global: fica no modelo de iluminacao, e nao numa luz especifica,
    // pra a cena nao escurecer toda quando os slots trocam de dono entre um ambiente e outro
    GLfloat ambiente_global[] = { 0.20f, 0.19f, 0.18f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambiente_global);

    montar_luzes();
}

void iluminacao_atualizar(){
    // as posições sao definidas todo frame porque depende da matriz modelview
    // precisa chamar depois de camera_aplicar_visualizacao()

    // pega a posição da camera e define o ambiente atual dela
    Vetor3 posicao_camera = camera_obter_posicao();
    Ambiente amb_atual = ambiente_da_camera(posicao_camera);

    // com base na ordem de ambientes (contando o atual), define a ordem de prioridade das luzes
    Ambiente amb_ordem[3];
    ordem_de_prioridade(amb_atual, amb_ordem);

    int slots_usados = 0;


    // os lustres entram primeiro, independente do ambiente: como o opengl nao
    // projeta sombras, eles iluminam o museu inteiro e servem de preenchimento
    // geral, entao deixar um de fora escurece a sala que perdeu o slot
    for (int i = 0; i < NUM_LUZES_MUSEU && slots_usados < MAX_LUZES_GL; i++) {
        if (!luzes[i].e_spot) {
            aplicar_luz(GL_LIGHT0 + slots_usados, luzes[i]);
            slots_usados++;
        }
    }

    // os spots ocupam o que sobrou, comecando pelo ambiente onde a camera esta
    for (int a = 0; a < 3 && slots_usados < MAX_LUZES_GL; a++) {
        for (int i = 0; i < NUM_LUZES_MUSEU && slots_usados < MAX_LUZES_GL; i++) {
            if (luzes[i].e_spot && luzes[i].ambiente == amb_ordem[a]) {
                aplicar_luz(GL_LIGHT0 + slots_usados, luzes[i]);
                slots_usados++;
            }
        }
    }
}