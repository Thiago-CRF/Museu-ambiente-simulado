#include <GL/glut.h>
#include "exibicoes.h"
#include "common.h"
#include "texture.h"

#define NUM_ESTATUAS 4
#define NUM_QUADROS  7
#define NUM_OBJETOS  2

// medidas da fonte do meio da sala de quadros
#define FONTE_RAIO_BACIA    0.85f
#define FONTE_ALTURA_BACIA  0.34f
#define FONTE_BORDA_BACIA   0.07f  // parede fina, de peçaa decorativa

// tipos de escultura da sala 1, cada um com sua funcao de desenho
typedef enum {
    ESTATUA_GENERICA,
    ESTATUA_T_POSE,
    ESTATUA_LIBERDADE,
    ESTATUA_ESPADA
} TipoEstatua;

typedef struct {
    Vetor3 posicao;
    float corR, corG, corB;
    TipoEstatua tipo;
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
    estatuas[0] = (Estatua){ { -19.0f, 0.0f, -4.0f }, 0.80f, 0.75f, 0.65f, ESTATUA_ESPADA };
    estatuas[1] = (Estatua){ { -11.0f, 0.0f, -4.0f }, 0.70f, 0.55f, 0.35f, ESTATUA_GENERICA };
    estatuas[2] = (Estatua){ { -19.0f, 0.0f,  4.0f }, 0.40f, 0.70f, 0.62f, ESTATUA_LIBERDADE };
    estatuas[3] = (Estatua){ { -11.0f, 0.0f,  4.0f }, 0.77f, 0.80f, 0.74f, ESTATUA_T_POSE };

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

// escultura no formato da estatua da liberdade
// o pedestal que ja tinha termina em y = 1.0, entao o corpo comeca dali
static void desenhar_estatua_liberdade(Estatua e) {
    glPushMatrix();
        glTranslatef(e.posicao.x, e.posicao.y, e.posicao.z);

        desenhar_pedestal();

        glColor3f(e.corR, e.corG, e.corB);

        // tunica: cone com a base no topo do pedestal e a ponta na altura do pescoco
        glPushMatrix();
            glTranslatef(0.0f, 1.0f, 0.0f);
            // o glutSolidCone cresce no eixo z, entao gira pra ele subir no y
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glutSolidCone(0.45, 1.35, 20, 4);
        glPopMatrix();

        // cabeca
        glPushMatrix();
            glTranslatef(0.0f, 2.48f, 0.0f);
            glutSolidSphere(0.17, 16, 16);
        glPopMatrix();

        // coroa: sete espinhos distribuidos em meia volta em torno da cabeca
        glPushMatrix();
            glTranslatef(0.0f, 2.58f, 0.0f);
            for (int i = 0; i < 7; i++) {
                glPushMatrix();
                    glRotatef(i * 30.0f - 90.0f, 0.0f, 1.0f, 0.0f);
                    glRotatef(-55.0f, 1.0f, 0.0f, 0.0f); // inclina o espinho pra cima
                    glutSolidCone(0.045, 0.33, 8, 1);
                glPopMatrix();
            }
        glPopMatrix();

        // braco direito pra cima, com uma tocha na ponta
        glPushMatrix();
            glTranslatef(0.28f, 2.00f, 0.0f);
            glRotatef(-14.0f, 0.0f, 0.0f, 1.0f); // afasta o braco do corpo

            glPushMatrix();
                glTranslatef(0.0f, 0.45f, 0.0f);
                glScalef(0.13f, 0.9f, 0.13f);
                glutSolidCube(1.0); // cubo da mão
            glPopMatrix();

            // a origem local sobe pra ponta do braco
            glTranslatef(0.0f, 0.95f, 0.0f);

            // taca da tocha
            glPushMatrix();
                glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
                glutSolidCone(0.13, 0.20, 12, 1);
            glPopMatrix();

            // chama: emissao faz ela brilhar sozinha, sem depender das luzes
            GLfloat emissao[] = { 1.0f, 0.80f, 0.35f, 1.0f };
            glMaterialfv(GL_FRONT, GL_EMISSION, emissao);

            glColor3f(1.0f, 0.85f, 0.45f);
            glPushMatrix();
                glTranslatef(0.0f, 0.26f, 0.0f);
                glScalef(1.0f, 1.7f, 1.0f);
                glutSolidSphere(0.10, 12, 12);
            glPopMatrix();

            // reset pra o não glMaterialfv persistir e deixar tudo brilhando depois
            GLfloat sem_emissao[] = { 0.0f, 0.0f, 0.0f, 1.0f };
            glMaterialfv(GL_FRONT, GL_EMISSION, sem_emissao);
        glPopMatrix();

        // volta a cor do corpo, que a chama tinha trocado
        glColor3f(e.corR, e.corG, e.corB);

        // braco esquerdo dobrado, segurando a tabua junto ao peito
        glPushMatrix();
            glTranslatef(-0.30f, 1.90f, 0.10f);
            glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
            glScalef(0.12f, 0.62f, 0.12f);
            glutSolidCube(1.0);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(-0.34f, 1.80f, 0.26f);
            glRotatef(-18.0f, 1.0f, 0.0f, 0.0f);
            glScalef(0.30f, 0.46f, 0.07f);
            glutSolidCube(1.0);
        glPopMatrix();

    glPopMatrix();
}

// escultura de uma espada cravada no pedestal, com a lamina apontando pra baixo
static void desenhar_escultura_espada(Estatua e) {
    glPushMatrix();
        glTranslatef(e.posicao.x, e.posicao.y, e.posicao.z);

        desenhar_pedestal();

        // lamina: cubo achatado no eixo z pra virar uma chapa fina
        glColor3f(0.75f, 0.78f, 0.82f);
        glPushMatrix();
            glTranslatef(0.0f, 1.75f, 0.0f);
            glScalef(0.22f, 1.5f, 0.05f);
            glutSolidCube(1.0);
        glPopMatrix();

        // ponta da lamina, entrando no pedestal
        glPushMatrix();
            glTranslatef(0.0f, 1.05f, 0.0f);
            // a escala vem antes da rotacao na pilha, entao ela achata o cone
            // depois que ele ja esta virado pra baixo, acompanhando a lamina
            glScalef(1.0f, 1.0f, 0.23f);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glutSolidCone(0.11, 0.30, 12, 1);
        glPopMatrix();

        // guarda, cabo e pomo usam a cor da escultura
        glColor3f(e.corR, e.corG, e.corB);

        // guarda transversal
        glPushMatrix();
            glTranslatef(0.0f, 2.55f, 0.0f);
            glScalef(0.75f, 0.10f, 0.12f);
            glutSolidCube(1.0);
        glPopMatrix();

        // cabo
        glPushMatrix();
            glTranslatef(0.0f, 2.82f, 0.0f);
            glScalef(0.10f, 0.45f, 0.10f);
            glutSolidCube(1.0);
        glPopMatrix();

        // pomo na ponta do cabo
        glPushMatrix();
            glTranslatef(0.0f, 3.10f, 0.0f);
            glutSolidSphere(0.11, 14, 14);
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
        glColor3f(0.52f, 0.52f, 0.50f); // cinza
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
        glColor3f(0.52f, 0.52f, 0.50f);
        glPushMatrix();
            glTranslatef(0.0f, alt_tronco / 2.0f, 0.0f);
            glScalef(largura_retangulo, alt_tronco, diam_cilindro); 
            glutSolidCube(1.0);
        glPopMatrix();

        glTranslatef(0.0f, alt_tronco, 0.0f);

        // Desenhando os cilindros dos braços
        glColor3f(0.52f, 0.52f, 0.50f);
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
        glColor3f(0.52f, 0.52f, 0.50f);
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
        //GLfloat dirLuz[] = { 0.0f, 0.0f, 1.0f };       
        
        // Define a luz na matriz atual do cone
        //glLightfv(luzAtual, GL_POSITION, posLuz);
        //glLightfv(luzAtual, GL_SPOT_DIRECTION, dirLuz);

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

// desenha uma fonte decorativa seca, em pedra clara: base baixa, bacia rasa,
// coluna fina e uma taca no topo
static void desenhar_fonte(float x, float z) {
    GLUquadric *q = gluNewQuadric();

    float raio_interno = FONTE_RAIO_BACIA - FONTE_BORDA_BACIA;

    glPushMatrix();
        glTranslatef(x, 0.0f, z);

        // marmore claro, levemente azulado
        glColor3f(0.90f, 0.91f, 0.93f);

        // degrau da base, um pouco mais largo que a bacia
        // o gluCylinder cresce no eixo z, entao gira pra ele subir no y
        glPushMatrix();
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            gluCylinder(q, 0.98, 0.94, 0.12, 28, 1);
        glPopMatrix();

        // disco de baixo da fonte
        glPushMatrix();
            glTranslatef(0.0f, 0.12f, 0.0f);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            gluDisk(q, 0.0, 0.94, 28, 2);
        glPopMatrix();

        // parede externa da bacia
        glPushMatrix();
            glTranslatef(0.0f, 0.12f, 0.0f);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            gluCylinder(q, FONTE_RAIO_BACIA, FONTE_RAIO_BACIA, FONTE_ALTURA_BACIA, 28, 2);
        glPopMatrix();

        // parede interna, vista por dentro da bacia
        // o GLU_INSIDE inverte as normais, senao essa face fica escura
        glPushMatrix();
            glTranslatef(0.0f, 0.12f, 0.0f);
            gluQuadricOrientation(q, GLU_INSIDE);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            gluCylinder(q, raio_interno, raio_interno, FONTE_ALTURA_BACIA, 28, 2);
            gluQuadricOrientation(q, GLU_OUTSIDE); // volta ao padrao
        glPopMatrix();

        // borda de cima: anel entre o raio interno e o externo
        glPushMatrix();
            glTranslatef(0.0f, 0.12f + FONTE_ALTURA_BACIA, 0.0f);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            gluDisk(q, raio_interno, FONTE_RAIO_BACIA, 28, 1);
        glPopMatrix();

        // fundo da bacia, agora aparente porque nao tem agua
        glPushMatrix();
            glTranslatef(0.0f, 0.18f, 0.0f);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            gluDisk(q, 0.0, raio_interno, 28, 4);
        glPopMatrix();

        // coluna fina, subindo do fundo da bacia
        glPushMatrix();
            glTranslatef(0.0f, 0.18f, 0.0f);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            gluCylinder(q, 0.15, 0.11, 1.25, 20, 4);
        glPopMatrix();

        // taça do topo: abre pra cima, tambem de parede fina
        glPushMatrix();
            glTranslatef(0.0f, 1.43f, 0.0f);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            gluCylinder(q, 0.11, 0.46, 0.24, 24, 2);
        glPopMatrix();

        // interior da taça, com as normais invertidas pelo mesmo motivo da bacia
        glPushMatrix();
            glTranslatef(0.0f, 1.45f, 0.0f);
            gluQuadricOrientation(q, GLU_INSIDE);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            gluCylinder(q, 0.09, 0.42, 0.22, 24, 2);
            gluQuadricOrientation(q, GLU_OUTSIDE);
        glPopMatrix();

        // fundo da taça
        glPushMatrix();
            glTranslatef(0.0f, 1.45f, 0.0f);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            gluDisk(q, 0.0, 0.11, 20, 1);
        glPopMatrix();

        // pilar central da fonte
        glPushMatrix();
            glTranslatef(0.0f, 1.60f, 0.0f);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            gluCylinder(q, 0.06, 0.05, 0.22, 14, 1);
        glPopMatrix();

        // esfera no fim do pila, entao ela engole a ponta da haste
        glPushMatrix();
            glTranslatef(0.0f, 1.82f, 0.0f);
            glutSolidSphere(0.08, 14, 14);
        glPopMatrix();

    glPopMatrix();

    gluDeleteQuadric(q);
}

void exponatos_desenhar(void) {
    // cada estatua diz qual funcao a desenha, entao adicionar um modelo novo
    // é so criar a funcao e mais um caso aqui se colocar outra escultura diferente
    for (int i = 0; i < NUM_ESTATUAS; i++) {
        switch (estatuas[i].tipo) {
            case ESTATUA_GENERICA:
                desenhar_estatua(estatuas[i]);
                break;
            case ESTATUA_T_POSE:
                desenhar_estatua_T_pose(estatuas[i].posicao.x, estatuas[i].posicao.y,
                                        estatuas[i].posicao.z, 2.0f);
                break;
            case ESTATUA_LIBERDADE:
                desenhar_estatua_liberdade(estatuas[i]);
                break;
            case ESTATUA_ESPADA:
                desenhar_escultura_espada(estatuas[i]);
                break;
        }
    }

    // Desenha os quadros enviando um ID de luz progressivo
    for (int i = 0; i < NUM_QUADROS; i++) {
        // Envia GL_LIGHT1 pro primeiro quadro, GL_LIGHT2 pro segundo, etc.
        desenhar_quadro(quadros[i], GL_LIGHT1 + i); 
    }

    for (int i = 0; i < NUM_OBJETOS; i++) {
        desenhar_objeto(objetos[i]);
    }

    // fonte no meio da sala dos quadros
    desenhar_fonte(15.0f, 0.0f);
};