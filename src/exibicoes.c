#include <GL/glut.h>
#include "exibicoes.h"
#include "common.h"
#include "texture.h"

#define NUM_ESTATUAS 4
#define NUM_QUADROS  7

typedef struct {
    Vetor3 posicao;
    float corR, corG, corB;
} Estatua;

typedef struct {
    Vetor3 posicao;   // centro do quadro na parede
    float rotacaoY;   // giro pra encostar o quadro na parede certa
    float largura, altura;
    GLuint texturaQuadro;
} Quadro;

static Estatua estatuas[NUM_ESTATUAS];
static Quadro quadros[NUM_QUADROS];

void exponatos_iniciar(void) {
    // ==========================================
    // SALA 1 (Esquerda): Apenas Esculturas
    // Centro da sala é em X = -15.0
    // ==========================================
    estatuas[0] = (Estatua){ { -19.0f, 0.0f, -4.0f }, 0.80f, 0.75f, 0.65f };
    estatuas[1] = (Estatua){ { -11.0f, 0.0f, -4.0f }, 0.70f, 0.55f, 0.35f };
    estatuas[2] = (Estatua){ { -19.0f, 0.0f,  4.0f }, 0.60f, 0.65f, 0.70f };
    estatuas[3] = (Estatua){ { -11.0f, 0.0f,  4.0f }, 0.85f, 0.80f, 0.55f };

    // ==========================================
    // SALA 2 (Direita): Apenas Quadros
    // Centro da sala é em X = 15.0
    // ==========================================
    // Quadro 1: Parede da esquerda (-X)
    quadros[0] = (Quadro){ {   5.1f, 3.0f, -6.5f },  90.0f, 3.0f, 2.18f,
                        textura_carregar("assets/textures/Salvador_Dali.jpg") };

    // Quadro 2: Parede da esquerda (-X)
    quadros[1] = (Quadro){ {   5.1f, 3.0f,   6.5f }, 90.0f,  2.94f,  3.78f,
                        textura_carregar("assets/textures/Salvador_Dali2.jpg") };

    // Quadro 3, 4 e 5: Parede da frente (+Z)
    quadros[2] = (Quadro){ { 10.0f, 3.0f,  9.9f }, 180.0f, 3.02f, 3.78f,
                        textura_carregar("assets/textures/Os_Girassois.jpg") };
    
    quadros[3] = (Quadro){ { 15.0f, 3.0f,  9.9f }, 180.0f, 3.11f, 3.78f,
                        textura_carregar("assets/textures/Van_Gogh.jpg") };

    quadros[4] = (Quadro){ { 20.5f, 3.0f,  9.9f }, 180.0f, 4.77f, 3.78f,
                        textura_carregar("assets/textures/Noite_Estrelada.jpg") };
    
    // Quadro 6: Parede da direita (+X)
    quadros[5] = (Quadro){ {  24.9f, 3.0f,  0.0f }, -90.0f, 6.54f, 4.36f,
                        textura_carregar("assets/textures/Tito_Lobo.jpg") };
    
    // Quadro 7: Parede do fundo (-Z)
    quadros[6] = (Quadro){ {  15.0f, 3.0f, -9.9f },   0.0f, 3.0f, 4.36f,
                        textura_carregar("assets/textures/Mona_Lisa.PNG") };
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

void desenhar_estatua_T_pose(float x, float y, float z, float altura_total) {
    float alt_pedestal = altura_total * 0.05f; 
    float alt_pernas    = altura_total * 0.50f; 
    float alt_tronco    = altura_total * 0.30f; 
    float diam_cabeca   = altura_total * 0.15f; 
    float raio_cabeca   = diam_cabeca / 2.0f;

    float raio_cilindro = 0.12f;
    float diam_cilindro = raio_cilindro * 2.0f;
    float compr_braco   = altura_total * 0.35f;
    float raio_braco    = 0.08f;
    
    float largura_retangulo = diam_cilindro * 2.0f; 

    GLUquadric* q = gluNewQuadric();

    glPushMatrix();
        glTranslatef(x, y, z);

        glColor3f(0.9f, 0.9f, 0.9f);
        glPushMatrix();
            glTranslatef(0.0f, alt_pedestal / 2.0f, 0.0f);
            glScalef(largura_retangulo * 1.5f, alt_pedestal, largura_retangulo * 1.5f);
            glutSolidCube(1.0);
        glPopMatrix();

        glTranslatef(0.0f, alt_pedestal, 0.0f);
        
        glColor3f(0.3f, 0.4f, 0.6f); 
        glPushMatrix();
            glTranslatef(-raio_cilindro, 0.0f, 0.0f);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); 
            gluCylinder(q, raio_cilindro, raio_cilindro, alt_pernas, 32, 1);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(raio_cilindro, 0.0f, 0.0f);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); 
            gluCylinder(q, raio_cilindro, raio_cilindro, alt_pernas, 32, 1);
        glPopMatrix();

        glTranslatef(0.0f, alt_pernas, 0.0f);

        glColor3f(0.3f, 0.4f, 0.6f);
        glPushMatrix();
            glTranslatef(0.0f, alt_tronco / 2.0f, 0.0f);
            glScalef(largura_retangulo, alt_tronco, diam_cilindro); 
            glutSolidCube(1.0);
        glPopMatrix();

        glTranslatef(0.0f, alt_tronco, 0.0f);

        glColor3f(0.3f, 0.4f, 0.6f);
        glPushMatrix();
            glTranslatef(largura_retangulo / 2.0f, -raio_braco, 0.0f);
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
            gluCylinder(q, raio_braco, raio_braco, compr_braco, 32, 1);
        glPopMatrix();


        glPushMatrix();
            glTranslatef(-largura_retangulo / 2.0f, -raio_braco, 0.0f);
            glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
            gluCylinder(q, raio_braco, raio_braco, compr_braco, 32, 1);
        glPopMatrix();

        glColor3f(0.3f, 0.4f, 0.6f);
        glPushMatrix();
            glTranslatef(0.0f, raio_cabeca, 0.0f);
            glutSolidSphere(raio_cabeca, 32, 32);
        glPopMatrix();

    glPopMatrix();

    gluDeleteQuadric(q);
}

// desenha uma luminaria articulada de parede no estilo Pixar
static void desenhar_luminaria(GLenum luzAtual) {
    glColor3f(0.1f, 0.1f, 0.1f); // Preto fosco para o metal

    glPushMatrix();
        // 1. Base colada na parede
        glPushMatrix();
            glScalef(0.15f, 0.15f, 0.05f);
            glutSolidSphere(1.0, 15, 15);
        glPopMatrix();

        // 2. Primeiro Braço
        glRotatef(-45.0f, 1.0f, 0.0f, 0.0f); 
        
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.3f); 
            glScalef(0.04f, 0.04f, 0.6f);   
            glutSolidCube(1.0);
        glPopMatrix();

        glTranslatef(0.0f, 0.0f, 0.6f);

        // 3. Articulação (Cotovelo)
        glPushMatrix();
            glScalef(0.06f, 0.06f, 0.06f);
            glutSolidSphere(1.0, 10, 10);
        glPopMatrix();

        // 4. Segundo Braço
        glRotatef(100.0f, 1.0f, 0.0f, 0.0f); 
        
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.3f);
            glScalef(0.04f, 0.04f, 0.6f);
            glutSolidCube(1.0);
        glPopMatrix();

        glTranslatef(0.0f, 0.0f, 0.6f);

        // 5. Articulação do bocal
        glPushMatrix();
            glScalef(0.06f, 0.06f, 0.06f);
            glutSolidSphere(1.0, 10, 10);
        glPopMatrix();

        // 6. Cúpula (Cone invertido)
        glRotatef(60.0f, 1.0f, 0.0f, 0.0f); 
        
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.2f);
            glRotatef(180.0f, 1.0f, 0.0f, 0.0f); 
            glutSolidCone(0.18, 0.2, 15, 15);
        glPopMatrix();

        // 7. Lâmpada (Bolinha) e Luz Real (Holofote)
        GLfloat posLuz[] = { 0.0f, 0.0f, 0.2f, 1.0f }; 
        GLfloat dirLuz[] = { 0.0f, 0.0f, 1.0f };       
        
        // Define a luz na matriz atual do cone
        glLightfv(luzAtual, GL_POSITION, posLuz);
        glLightfv(luzAtual, GL_SPOT_DIRECTION, dirLuz);

        // Desenha a bolinha com brilho próprio (Emissão)
        glPushMatrix();
            glTranslatef(posLuz[0], posLuz[1], posLuz[2]); 
            
            GLfloat corAcesa[] = { 1.0f, 1.0f, 0.8f, 1.0f };   
            GLfloat corApagada[] = { 0.0f, 0.0f, 0.0f, 1.0f }; 
            
            glMaterialfv(GL_FRONT, GL_EMISSION, corAcesa);
            glutSolidSphere(0.07, 10, 10);
            glMaterialfv(GL_FRONT, GL_EMISSION, corApagada); // Desliga!
            
        glPopMatrix();

    glPopMatrix();
}

// desenha um quadro com moldura encostado na parede
static void desenhar_quadro(Quadro q, GLenum luzAtual) {
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
        if (q.texturaQuadro != 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, q.texturaQuadro);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        }

        glBegin(GL_QUADS);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-meiaL, -meiaA, 0.06f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f( meiaL, -meiaA, 0.06f);
            glTexCoord2f(1.0f, 1.0f); glVertex3f( meiaL,  meiaA, 0.06f);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-meiaL,  meiaA, 0.06f);
        glEnd();

        if (q.texturaQuadro != 0) {
            glDisable(GL_TEXTURE_2D);
        }

        // ==========================================
        // LUMINÁRIA ACIMA DO QUADRO
        // ==========================================
        glPushMatrix();
            // Desloca para cima baseando-se na altura da tela (meiaA) + uma margem
            glTranslatef(0.0f, meiaA + 0.35f, 0.0f); 
            desenhar_luminaria(luzAtual); // <-- Passa a luz para a luminária
        glPopMatrix();

    glPopMatrix();
}

void exponatos_desenhar(void) {
    // Desenha as estátuas
    for (int i = 0; i < NUM_ESTATUAS-1; i++) {
        desenhar_estatua(estatuas[i]);
    }

    // Desenha a estátua especial na última posição
    desenhar_estatua_T_pose(estatuas[3].posicao.x, estatuas[3].posicao.y, estatuas[3].posicao.z, 2.0f);

    // Desenha os quadros enviando um ID de luz progressivo
    for (int i = 0; i < NUM_QUADROS; i++) {
        // Envia GL_LIGHT1 pro primeiro quadro, GL_LIGHT2 pro segundo, etc.
        desenhar_quadro(quadros[i], GL_LIGHT1 + i); 
    }
}