#include <GL/glut.h>
#include "cena.h"
#include "common.h"
#include "texture.h"

// limites fisicos da sala
#define SALA_X  10.0f
#define SALA_Z  10.0f
#define ALTURA_PAREDE 6.0f

static GLuint texturaPiso = 0;
static GLuint texturaParede = 0;

void cena_iniciar(void) {
    // se o arquivo nao existir, textura_carregar retorna 0 e a cena usa so cor
    texturaPiso = textura_carregar("assets/textures/piso.jpg");
    texturaParede = textura_carregar("assets/textures/parede.jpg");
}

// desenha um quad com textura opcional e coordenadas de textura repetidas
static void desenhar_quad(GLuint textura, float repeticao,
                          float nx, float ny, float nz,
                          float x1, float y1, float z1,
                          float x2, float y2, float z2,
                          float x3, float y3, float z3,
                          float x4, float y4, float z4) {
    if (textura != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textura);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

    glBegin(GL_QUADS);
        glNormal3f(nx, ny, nz);
        glTexCoord2f(0.0f, 0.0f);            glVertex3f(x1, y1, z1);
        glTexCoord2f(repeticao, 0.0f);       glVertex3f(x2, y2, z2);
        glTexCoord2f(repeticao, repeticao);  glVertex3f(x3, y3, z3);
        glTexCoord2f(0.0f, repeticao);       glVertex3f(x4, y4, z4);
    glEnd();

    if (textura != 0) {
        glDisable(GL_TEXTURE_2D);
    }
}

void cena_desenhar(void) {
    // piso
    glColor3f(0.6f, 0.6f, 0.6f);
    desenhar_quad(texturaPiso, 8.0f,
                  0.0f, 1.0f, 0.0f,
                  -SALA_X, 0.0f, -SALA_Z,
                  -SALA_X, 0.0f,  SALA_Z,
                   SALA_X, 0.0f,  SALA_Z,
                   SALA_X, 0.0f, -SALA_Z);

    // teto
    glColor3f(0.35f, 0.35f, 0.38f);
    desenhar_quad(0, 1.0f,
                  0.0f, -1.0f, 0.0f,
                  -SALA_X, ALTURA_PAREDE, -SALA_Z,
                   SALA_X, ALTURA_PAREDE, -SALA_Z,
                   SALA_X, ALTURA_PAREDE,  SALA_Z,
                  -SALA_X, ALTURA_PAREDE,  SALA_Z);

    // paredes (normais apontando pra dentro da sala)
    glColor3f(0.85f, 0.84f, 0.80f);

    // parede do fundo (-z)
    desenhar_quad(texturaParede, 4.0f,
                  0.0f, 0.0f, 1.0f,
                  -SALA_X, 0.0f, -SALA_Z,
                   SALA_X, 0.0f, -SALA_Z,
                   SALA_X, ALTURA_PAREDE, -SALA_Z,
                  -SALA_X, ALTURA_PAREDE, -SALA_Z);

    // parede da frente (+z)
    desenhar_quad(texturaParede, 4.0f,
                  0.0f, 0.0f, -1.0f,
                   SALA_X, 0.0f, SALA_Z,
                  -SALA_X, 0.0f, SALA_Z,
                  -SALA_X, ALTURA_PAREDE, SALA_Z,
                   SALA_X, ALTURA_PAREDE, SALA_Z);

    // parede da esquerda (-x)
    desenhar_quad(texturaParede, 4.0f,
                  1.0f, 0.0f, 0.0f,
                  -SALA_X, 0.0f,  SALA_Z,
                  -SALA_X, 0.0f, -SALA_Z,
                  -SALA_X, ALTURA_PAREDE, -SALA_Z,
                  -SALA_X, ALTURA_PAREDE,  SALA_Z);

    // parede da direita (+x)
    desenhar_quad(texturaParede, 4.0f,
                  -1.0f, 0.0f, 0.0f,
                   SALA_X, 0.0f, -SALA_Z,
                   SALA_X, 0.0f,  SALA_Z,
                   SALA_X, ALTURA_PAREDE,  SALA_Z,
                   SALA_X, ALTURA_PAREDE, -SALA_Z);
}