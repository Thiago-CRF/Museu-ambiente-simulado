#include <GL/glut.h>
#include "cena.h"
#include "common.h"
#include "texture.h"

// --- LIMITES FÍSICOS DA NOVA PLANTA ---
// Sala 1 (Esculturas) - Fica à esquerda
#define S1_XMIN -25.0f
#define S1_XMAX -5.0f
#define S1_ZMIN -10.0f
#define S1_ZMAX  10.0f

// Corredor - Fica no centro
#define C_XMIN  -5.0f
#define C_XMAX   5.0f
#define C_ZMIN  -3.0f // Mais estreito que as salas
#define C_ZMAX   3.0f

// Sala 2 (Quadros) - Fica à direita
#define S2_XMIN  5.0f
#define S2_XMAX  25.0f
#define S2_ZMIN -10.0f
#define S2_ZMAX  10.0f

// --- ALTURAS (PÉ-DIREITO) ---
#define ALTURA_SALA     7.0f  // Salas bem altas
#define ALTURA_CORREDOR 4.0f  // Corredor mais baixo

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