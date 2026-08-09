#include <GL/glut.h>
#include "cena.h"
#include "common.h"
#include "texture.h"

// divisoes por lado ao desenhar cada superficie. usado pra melhorar a iluminação
// a luz do opengl 2.1 é calculada por vertice, entao um quad unico nao tem
// pontos suficientes pra registrar o brilho embaixo das luzes
#define SUBDIVISOES_QUAD 12

static GLuint texturaPiso = 0;
static GLuint texturaParede = 0;
static GLuint texturaTeto = 0;

void cena_iniciar(void) {
    texturaPiso = textura_carregar("assets/textures/piso.jpg");
    texturaParede = textura_carregar("assets/textures/parede.jpg");
    texturaTeto = textura_carregar("assets/textures/teto.jpg");
}

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

        // percorre a superficie numa grade, gerando varios quads menores
        // u e v vao de 0 a 1 e indicam a posicao relativa dentro do quad original
        for (int i = 0; i < SUBDIVISOES_QUAD; i++) {
            for (int j = 0; j < SUBDIVISOES_QUAD; j++) {
                float u0 = (float)i / SUBDIVISOES_QUAD;
                float u1 = (float)(i + 1) / SUBDIVISOES_QUAD;
                float v0 = (float)j / SUBDIVISOES_QUAD;
                float v1 = (float)(j + 1) / SUBDIVISOES_QUAD;

                // interpolacao bilinear entre os 4 cantos originais
                // a borda de baixo vai de p1 a p2, e a de cima de p4 a p3
                float ax = x1 + (x2 - x1) * u0, az = z1 + (z2 - z1) * u0;
                float ay = y1 + (y2 - y1) * u0;
                float bx = x4 + (x3 - x4) * u0, bz = z4 + (z3 - z4) * u0;
                float by = y4 + (y3 - y4) * u0;

                float cx = x1 + (x2 - x1) * u1, cz = z1 + (z2 - z1) * u1;
                float cy = y1 + (y2 - y1) * u1;
                float dx = x4 + (x3 - x4) * u1, dz = z4 + (z3 - z4) * u1;
                float dy = y4 + (y3 - y4) * u1;

                glTexCoord2f(u0 * repeticao, v0 * repeticao);
                glVertex3f(ax + (bx - ax) * v0, ay + (by - ay) * v0, az + (bz - az) * v0);

                glTexCoord2f(u1 * repeticao, v0 * repeticao);
                glVertex3f(cx + (dx - cx) * v0, cy + (dy - cy) * v0, cz + (dz - cz) * v0);

                glTexCoord2f(u1 * repeticao, v1 * repeticao);
                glVertex3f(cx + (dx - cx) * v1, cy + (dy - cy) * v1, cz + (dz - cz) * v1);

                glTexCoord2f(u0 * repeticao, v1 * repeticao);
                glVertex3f(ax + (bx - ax) * v1, ay + (by - ay) * v1, az + (bz - az) * v1);
            }
        }
    glEnd();

    if (textura != 0) {
        glDisable(GL_TEXTURE_2D);
    }
}

void cena_desenhar(void) {
    // ==========================================
    // 1. PISO (Normais para cima: 0, 1, 0)
    // ==========================================
    glColor3f(0.6f, 0.6f, 0.6f);
    
    // Piso - Sala 1
    desenhar_quad(texturaPiso, 8.0f, 0.0f, 1.0f, 0.0f,
                  S1_XMIN, 0.0f, S1_ZMIN,  S1_XMIN, 0.0f, S1_ZMAX,
                  S1_XMAX, 0.0f, S1_ZMAX,  S1_XMAX, 0.0f, S1_ZMIN);
    
    // Piso - Corredor
    desenhar_quad(texturaPiso, 4.0f, 0.0f, 1.0f, 0.0f,
                  C_XMIN, 0.0f, C_ZMIN,  C_XMIN, 0.0f, C_ZMAX,
                  C_XMAX, 0.0f, C_ZMAX,  C_XMAX, 0.0f, C_ZMIN);

    // Piso - Sala 2
    desenhar_quad(texturaPiso, 8.0f, 0.0f, 1.0f, 0.0f,
                  S2_XMIN, 0.0f, S2_ZMIN,  S2_XMIN, 0.0f, S2_ZMAX,
                  S2_XMAX, 0.0f, S2_ZMAX,  S2_XMAX, 0.0f, S2_ZMIN);

    // ==========================================
    // 2. TETO (Normais para baixo: 0, -1, 0)
    // ==========================================
    glColor3f(0.85f, 0.85f, 0.85f); 
    
    // Teto - Sala 1 (Mais alto) - repetindo a textura 8 vezes
    desenhar_quad(texturaTeto, 8.0f, 0.0f, -1.0f, 0.0f,
                  S1_XMIN, ALTURA_SALA, S1_ZMIN,  S1_XMAX, ALTURA_SALA, S1_ZMIN,
                  S1_XMAX, ALTURA_SALA, S1_ZMAX,  S1_XMIN, ALTURA_SALA, S1_ZMAX);

    // Teto - Corredor (Mais baixo) - repetindo a textura 4 vezes
    desenhar_quad(texturaTeto, 4.0f, 0.0f, -1.0f, 0.0f,
                  C_XMIN, ALTURA_CORREDOR, C_ZMIN,  C_XMAX, ALTURA_CORREDOR, C_ZMIN,
                  C_XMAX, ALTURA_CORREDOR, C_ZMAX,  C_XMIN, ALTURA_CORREDOR, C_ZMAX);

    // Teto - Sala 2 (Mais alto) - repetindo a textura 8 vezes
    desenhar_quad(texturaTeto, 8.0f, 0.0f, -1.0f, 0.0f,
                  S2_XMIN, ALTURA_SALA, S2_ZMIN,  S2_XMAX, ALTURA_SALA, S2_ZMIN,
                  S2_XMAX, ALTURA_SALA, S2_ZMAX,  S2_XMIN, ALTURA_SALA, S2_ZMAX);
    // ==========================================
    // 3. PAREDES
    // ==========================================
    glColor3f(0.85f, 0.84f, 0.80f);

    // --- PAREDES DA SALA 1 (Esquerda) ---
    // Fundo (-Z)
    desenhar_quad(texturaParede, 4.0f, 0.0f, 0.0f, 1.0f,
                  S1_XMIN, 0.0f, S1_ZMIN,  S1_XMAX, 0.0f, S1_ZMIN,
                  S1_XMAX, ALTURA_SALA, S1_ZMIN,  S1_XMIN, ALTURA_SALA, S1_ZMIN);
    
    // Frente (+Z)
    desenhar_quad(texturaParede, 4.0f, 0.0f, 0.0f, -1.0f,
                  S1_XMAX, 0.0f, S1_ZMAX,  S1_XMIN, 0.0f, S1_ZMAX,
                  S1_XMIN, ALTURA_SALA, S1_ZMAX,  S1_XMAX, ALTURA_SALA, S1_ZMAX);

    // Esquerda (-X) - Parede fechada
    desenhar_quad(texturaParede, 4.0f, 1.0f, 0.0f, 0.0f,
                  S1_XMIN, 0.0f, S1_ZMAX,  S1_XMIN, 0.0f, S1_ZMIN,
                  S1_XMIN, ALTURA_SALA, S1_ZMIN,  S1_XMIN, ALTURA_SALA, S1_ZMAX);

    // Direita (+X) - Parede com a porta para o corredor
    // 1º Pilar (fundo)
    desenhar_quad(texturaParede, 2.0f, -1.0f, 0.0f, 0.0f,
                  S1_XMAX, 0.0f, S1_ZMIN,  S1_XMAX, 0.0f, C_ZMIN,
                  S1_XMAX, ALTURA_SALA, C_ZMIN,  S1_XMAX, ALTURA_SALA, S1_ZMIN);
    // 2º Pilar (frente)
    desenhar_quad(texturaParede, 2.0f, -1.0f, 0.0f, 0.0f,
                  S1_XMAX, 0.0f, C_ZMAX,  S1_XMAX, 0.0f, S1_ZMAX,
                  S1_XMAX, ALTURA_SALA, S1_ZMAX,  S1_XMAX, ALTURA_SALA, C_ZMAX);
    // Verga (parede acima da abertura do corredor)
    desenhar_quad(texturaParede, 1.0f, -1.0f, 0.0f, 0.0f,
                  S1_XMAX, ALTURA_CORREDOR, C_ZMIN,  S1_XMAX, ALTURA_CORREDOR, C_ZMAX,
                  S1_XMAX, ALTURA_SALA, C_ZMAX,  S1_XMAX, ALTURA_SALA, C_ZMIN);

    // --- PAREDES DO CORREDOR ---
    // Fundo (-Z)
    desenhar_quad(texturaParede, 2.0f, 0.0f, 0.0f, 1.0f,
                  C_XMIN, 0.0f, C_ZMIN,  C_XMAX, 0.0f, C_ZMIN,
                  C_XMAX, ALTURA_CORREDOR, C_ZMIN,  C_XMIN, ALTURA_CORREDOR, C_ZMIN);
    // Frente (+Z)
    desenhar_quad(texturaParede, 2.0f, 0.0f, 0.0f, -1.0f,
                  C_XMAX, 0.0f, C_ZMAX,  C_XMIN, 0.0f, C_ZMAX,
                  C_XMIN, ALTURA_CORREDOR, C_ZMAX,  C_XMAX, ALTURA_CORREDOR, C_ZMAX);

    // --- PAREDES DA SALA 2 (Direita) ---
    // Fundo (-Z)
    desenhar_quad(texturaParede, 4.0f, 0.0f, 0.0f, 1.0f,
                  S2_XMIN, 0.0f, S2_ZMIN,  S2_XMAX, 0.0f, S2_ZMIN,
                  S2_XMAX, ALTURA_SALA, S2_ZMIN,  S2_XMIN, ALTURA_SALA, S2_ZMIN);

    // Frente (+Z)
    desenhar_quad(texturaParede, 4.0f, 0.0f, 0.0f, -1.0f,
                  S2_XMAX, 0.0f, S2_ZMAX,  S2_XMIN, 0.0f, S2_ZMAX,
                  S2_XMIN, ALTURA_SALA, S2_ZMAX,  S2_XMAX, ALTURA_SALA, S2_ZMAX);

    // Direita (+X) - Parede fechada
    desenhar_quad(texturaParede, 4.0f, -1.0f, 0.0f, 0.0f,
                  S2_XMAX, 0.0f, S2_ZMIN,  S2_XMAX, 0.0f, S2_ZMAX,
                  S2_XMAX, ALTURA_SALA, S2_ZMAX,  S2_XMAX, ALTURA_SALA, S2_ZMIN);

    // Esquerda (-X) - Parede com a porta para o corredor
    // 1º Pilar (frente)
    desenhar_quad(texturaParede, 2.0f, 1.0f, 0.0f, 0.0f,
                  S2_XMIN, 0.0f, S2_ZMAX,  S2_XMIN, 0.0f, C_ZMAX,
                  S2_XMIN, ALTURA_SALA, C_ZMAX,  S2_XMIN, ALTURA_SALA, S2_ZMAX);
    // 2º Pilar (fundo)
    desenhar_quad(texturaParede, 2.0f, 1.0f, 0.0f, 0.0f,
                  S2_XMIN, 0.0f, C_ZMIN,  S2_XMIN, 0.0f, S2_ZMIN,
                  S2_XMIN, ALTURA_SALA, S2_ZMIN,  S2_XMIN, ALTURA_SALA, C_ZMIN);
    // Verga (parede acima da abertura do corredor)
    desenhar_quad(texturaParede, 1.0f, 1.0f, 0.0f, 0.0f,
                  S2_XMIN, ALTURA_CORREDOR, C_ZMAX,  S2_XMIN, ALTURA_CORREDOR, C_ZMIN,
                  S2_XMIN, ALTURA_SALA, C_ZMIN,  S2_XMIN, ALTURA_SALA, C_ZMAX);
}