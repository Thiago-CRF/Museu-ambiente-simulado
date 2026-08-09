#include <GL/glut.h>
#include "exibicoes.h"
#include "common.h"
#include "texture.h"

#define NUM_ESTATUAS 4
#define NUM_QUADROS  7
#define NUM_OBJETOS  2

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

typedef struct {
    Vetor3 posicao;   // centro do quadro na parede
    float rotacaoY;   // giro pra encostar o quadro na parede certa
    float largura, altura;
    GLuint texturaObjeto;
} Objetos;

static Estatua estatuas[NUM_ESTATUAS];
static Quadro quadros[NUM_QUADROS];
static Objetos objetos[NUM_OBJETOS];

void exponatos_iniciar(void) {

    // SALA 1 (Esquerda): Apenas Esculturas
    // Centro da sala é em X = -15.0
    estatuas[0] = (Estatua){ { -19.0f, 0.0f, -4.0f }, 0.80f, 0.75f, 0.65f };
    estatuas[1] = (Estatua){ { -11.0f, 0.0f, -4.0f }, 0.70f, 0.55f, 0.35f };
    estatuas[2] = (Estatua){ { -19.0f, 0.0f,  4.0f }, 0.60f, 0.65f, 0.70f };
    estatuas[3] = (Estatua){ { -11.0f, 0.0f,  4.0f }, 0.85f, 0.80f, 0.55f };

    // SALA 2 (Direita): Apenas Quadros
    // Centro da sala é em X = 15.0

    // Quadro 1: Parede da esquerda (-X)
    // Como o corredor fica no meio (Z de -3 a 3), vamos colocar este quadro
    // no pilar que fica na parte de trás da parede (Z = -6.5).
    quadros[0] = (Quadro){ {   5.1f, 3.0f, -6.5f },  90.0f, 3.0f, 2.18f,
                        textura_carregar("assets/textures/Salvador_Dali.jpg") };

    // Quadro 2: Parede da esquerda (-X)
    // Sofre do mesmo problema que o quadro 1
    quadros[1] = (Quadro){ {   5.1f, 3.0f,   6.5f }, 90.0f,  2.94f,  3.78f,
                        textura_carregar("assets/textures/Salvador_Dali2.jpg") };

    // Quadro 3, 4 e 5: Parede da frente (+Z)
    quadros[2] = (Quadro){ { 10.0f, 3.0f,  9.9f }, 180.0f, 3.02f, 3.78f,
                        textura_carregar("assets/textures/Os_Girassois.jpg") };
    
    quadros[3] = (Quadro){ { 15.0f, 3.0f,  9.9f }, 180.0f, 3.11f, 3.78f,
                        textura_carregar("assets/textures/Van_Gogh.jpg") };

    quadros[4] = (Quadro){ { 20.5f, 3.0f,  9.9f }, 180.0f, 4.77f, 3.78f,
                        textura_carregar("assets/textures/Noite_Estrelada.jpg") };
    
    // Quadro 6: Parede da direita (+X), que é totalmente fechada
    quadros[5] = (Quadro){ {  24.9f, 3.0f,  0.0f }, -90.0f, 6.54f, 4.36f,
                        textura_carregar("assets/textures/Tito_Lobo.jpg") };
    
    // Quadro 7: Parede do fundo (-Z)
    quadros[6] = (Quadro){ {  15.0f, 3.0f, -9.9f },   0.0f, 3.0f, 4.36f,
                        textura_carregar("assets/textures/Mona_Lisa.PNG") };

    // Objetos 1, 2:
    objetos[0] = (Objetos){ {  5.8f, 0.0f,  3.0f },   0.0f, 2.0f, 2.0f,
                        textura_carregar("assets/textures/planta.png") };

    objetos[1] = (Objetos){ {  5.5f, 0.0f, -3.0f },   0.0f, 1.0f, 1.2f,
                        textura_carregar("assets/textures/planta.png") };
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
    float alt_pedestal = altura_total * 0.05f; // 5%
    float alt_pernas    = altura_total * 0.50f; // 50%
    float alt_tronco    = altura_total * 0.30f; // 30%
    float diam_cabeca   = altura_total * 0.15f; // 15%
    float raio_cabeca   = diam_cabeca / 2.0f;

    // Configuração dos cilindros das pernas
    float raio_cilindro = 0.12f;
    float diam_cilindro = raio_cilindro * 2.0f;
    // Configuração dos cilindros dos braços
    float compr_braco   = altura_total * 0.35f;
    float raio_braco    = 0.08f;
    
    float largura_retangulo = diam_cilindro * 2.0f; 

    GLUquadric* q = gluNewQuadric();

    glPushMatrix();
        glTranslatef(x, y, z);

        // Criando um pedestal baixo
        glColor3f(0.9f, 0.9f, 0.9f); // um branco um pouco mais escuro
        glPushMatrix();
            glTranslatef(0.0f, alt_pedestal / 2.0f, 0.0f);
            glScalef(largura_retangulo * 1.5f, alt_pedestal, largura_retangulo * 1.5f);
            glutSolidCube(1.0);
        glPopMatrix();

        glTranslatef(0.0f, alt_pedestal, 0.0f);
        
        // A criação dos cilindros das pernas
        glColor3f(0.3f, 0.4f, 0.6f); // azul
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

        // Desenhando o retangulo do tronco
        glColor3f(0.3f, 0.4f, 0.6f);
        glPushMatrix();
            glTranslatef(0.0f, alt_tronco / 2.0f, 0.0f);
            glScalef(largura_retangulo, alt_tronco, diam_cilindro); 
            glutSolidCube(1.0);
        glPopMatrix();

        glTranslatef(0.0f, alt_tronco, 0.0f);

        // Desenhando os cilindros dos braços
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

        // Desenhando a esfere que representa a cabeça
        glColor3f(0.3f, 0.4f, 0.6f);
        glPushMatrix();
            glTranslatef(0.0f, raio_cabeca, 0.0f);
            glutSolidSphere(raio_cabeca, 32, 32);
        glPopMatrix();

    glPopMatrix();

    gluDeleteQuadric(q);
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
    glPopMatrix();
}

void desenhar_objeto(Objetos obj) {
    float meioL = obj.largura / 2.0f;

    glPushMatrix();
        glTranslatef(obj.posicao.x, obj.posicao.y, obj.posicao.z);

        // Ativa a transparencia para onde não houver cor
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, obj.texturaObjeto);
        
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // mantem a cor original da textura

        glBegin(GL_QUADS);
            // Eixo X, frente
            glNormal3f(0.0f, 0.0f, 1.0f); // Normal apontando para frente
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-meioL, 0.0f,   0.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f( meioL, 0.0f,   0.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex3f( meioL, obj.altura, 0.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-meioL, obj.altura, 0.0f);

            // Eixo Z, virado para o lado
            glNormal3f(1.0f, 0.0f, 0.0f); // Normal apontando para o lado
            glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f,  -meioL);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.0f,   meioL);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, obj.altura,  meioL);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, obj.altura, -meioL);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);

    glPopMatrix();
}

void exponatos_desenhar(void) {
    for (int i = 0; i < NUM_ESTATUAS-1; i++) {
        desenhar_estatua(estatuas[i]);
    }

    desenhar_estatua_T_pose(estatuas[3].posicao.x, estatuas[3].posicao.y, estatuas[3].posicao.z, 2.0f);

    for (int i = 0; i < NUM_QUADROS; i++) {
        desenhar_quadro(quadros[i]);
    }

    for (int i = 0; i < NUM_OBJETOS; i++) {
        desenhar_objeto(objetos[i]);
    }
};