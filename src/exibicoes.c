#include <GL/glut.h>
#include "exibicoes.h"
#include "common.h"
#include "texture.h"

#define NUM_ESTATUAS 4
#define NUM_QUADROS  4

typedef struct {
    Vetor3 posicao;
    float corR, corG, corB;
} Estatua;

typedef struct {
    Vetor3 posicao;   // centro do quadro na parede
    float rotacaoY;   // giro pra encostar o quadro na parede certa
    float largura, altura;
} Quadro;

static Estatua estatuas[NUM_ESTATUAS];
static Quadro quadros[NUM_QUADROS];
static GLuint texturaQuadro = 0;

void exponatos_iniciar(void) {
    // estatuas sobre pedestais, espalhadas pela sala
    estatuas[0] = (Estatua){ { -4.0f, 0.0f, -4.0f }, 0.80f, 0.75f, 0.65f };
    estatuas[1] = (Estatua){ {  4.0f, 0.0f, -4.0f }, 0.70f, 0.55f, 0.35f };
    estatuas[2] = (Estatua){ { -4.0f, 0.0f,  4.0f }, 0.60f, 0.65f, 0.70f };
    estatuas[3] = (Estatua){ {  4.0f, 0.0f,  4.0f }, 0.85f, 0.80f, 0.55f };

    // quadros encostados nas quatro paredes
    quadros[0] = (Quadro){ {  0.0f, 3.0f, -9.9f },   0.0f, 3.0f, 2.0f };
    quadros[1] = (Quadro){ {  0.0f, 3.0f,  9.9f }, 180.0f, 3.0f, 2.0f };
    quadros[2] = (Quadro){ { -9.9f, 3.0f,  0.0f },  90.0f, 3.0f, 2.0f };
    quadros[3] = (Quadro){ {  9.9f, 3.0f,  0.0f }, -90.0f, 3.0f, 2.0f };

    // Tenta carregar a textura. Se o arquivo não existir ou o texture.h não estiver pronto, ele retorna 0 em segurança
    texturaQuadro = textura_carregar("assets/textures/quadro.jpg");
}

// desenha um pedestal simples na origem local
static void desenhar_pedestal(void) {
    glColor3f(0.25f, 0.25f, 0.28f);
    glPushMatrix();
        glTranslatef(0.0f, 0.5f, 0.0f);
        glScalef(1.2f, 1.0f, 1.2f);
        glutSolidCube(1.0);
    glPopMatrix();
}

// desenha a estatua usando hierarquia de transformacoes (atividade 02)
static void desenhar_estatua(Estatua e) {
    glPushMatrix();
        glTranslatef(e.posicao.x, e.posicao.y, e.posicao.z);

        desenhar_pedestal();

        glColor3f(e.corR, e.corG, e.corB);
        
        // torso: origem local sobe pro topo do pedestal
        glPushMatrix();
            glTranslatef(0.0f, 1.4f, 0.0f);
            glPushMatrix();
                glScalef(0.5f, 0.9f, 0.35f);
                glutSolidCube(1.0);
            glPopMatrix();
            
            // cabeca, relativa ao torso
            glPushMatrix();
                glTranslatef(0.0f, 0.65f, 0.0f);
                glutSolidSphere(0.22, 20, 20);
            glPopMatrix();
            
            // braco esquerdo, relativo ao torso
            glPushMatrix();
                glTranslatef(-0.35f, 0.2f, 0.0f);
                glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
                glScalef(0.15f, 0.7f, 0.15f);
                glutSolidCube(1.0);
            glPopMatrix();
            
            // braco direito, relativo ao torso
            glPushMatrix();
                glTranslatef(0.35f, 0.2f, 0.0f);
                glRotatef(-30.0f, 0.0f, 0.0f, 1.0f);
                glScalef(0.15f, 0.7f, 0.15f);
                glutSolidCube(1.0);
            glPopMatrix();
        glPopMatrix();

    glPopMatrix();
}

// desenha um quadro com moldura encostado na parede
static void desenhar_quadro(Quadro q) {
    glPushMatrix();
        glTranslatef(q.posicao.x, q.posicao.y, q.posicao.z);
        glRotatef(q.rotacaoY, 0.0f, 1.0f, 0.0f);

        float meiaL = q.largura / 2.0f;
        float meiaA = q.altura / 2.0f;
        float borda = 0.12f;
        
        // moldura
        glColor3f(0.35f, 0.22f, 0.10f);
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.02f);
            glScalef(q.largura + borda, q.altura + borda, 0.05f);
            glutSolidCube(1.0);
        glPopMatrix();
        
        // tela do quadro
        glColor3f(1.0f, 1.0f, 1.0f);
        if (texturaQuadro != 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texturaQuadro);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        }

        glBegin(GL_QUADS);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-meiaL, -meiaA, 0.06f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f( meiaL, -meiaA, 0.06f);
            glTexCoord2f(1.0f, 1.0f); glVertex3f( meiaL,  meiaA, 0.06f);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-meiaL,  meiaA, 0.06f);
        glEnd();

        if (texturaQuadro != 0) {
            glDisable(GL_TEXTURE_2D);
        }
    glPopMatrix();
}

void exponatos_desenhar(void) {
    for (int i = 0; i < NUM_ESTATUAS; i++) {
        desenhar_estatua(estatuas[i]);
    }

    for (int i = 0; i < NUM_QUADROS; i++) {
        desenhar_quadro(quadros[i]);
    }
}