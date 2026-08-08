// g++ src/main.c -o museu -lGL -lGLU -lglut
#include <GL/glut.h>
#include <stdlib.h>

#include "common.h"
#include "camera.h"
#include "cena.h"
#include "exibicoes.h"
#include "iluminacao.h"

// dimensoes atuais da janela que são atulizadas no reshape
static int largura_janela = LARGURA_JANELA;
static int altura_janela = ALTURA_JANELA;

// tempo do frame anterior em milissegundos pra calcular o delta tempo dt
static int tempo_anterior = 0;

// variaveis para controle do modo tela cheia
static int tela_cheia = 0;

// guarda tamanho e posicao da janela antes de entrar em tela cheia
static int janela_largura_anterior = LARGURA_JANELA;
static int janela_altura_anterior = ALTURA_JANELA;
static int janela_pos_x_anterior = 0;
static int janela_pos_y_anterior = 0;

// --- FUNÇÃO DE INICIALIZAÇÃO
void init(void) {
    // cor de fundo da janela ((0.1, 0.1, 0.1, 1.0)cinza escuro)
    glClearColor(0.1, 0.1, 0.1, 1.0); 
    
    glEnable(GL_DEPTH_TEST); // ativa o z-buffer
    glShadeModel(GL_SMOOTH); // sombreamento suave
    glEnable(GL_NORMALIZE); // normaliza normais apos glScalef

    // inicia cada modulo
    cena_iniciar();
    exponatos_iniciar();
    iluminacao_iniciar();
    camera_iniciar();

    // esconde o cursor e centraliza o mouse pro modo primeira pessoa
    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(largura_janela / 2, altura_janela / 2);

    tempo_anterior = glutGet(GLUT_ELAPSED_TIME);
}

// --- FUNÇÃO DE DESENHO (DISPLAY)
void display(void) {
    // limpa os buffers de cor e de profundidade
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // - CÂMERA
    // aplica a vizualização da camera antes de tudo
    camera_aplicar_visualizacao();

    // ILUMINAÇÃO, atualiaz a posição das luzes
    iluminacao_atualizar();

       // - AMBIENTE / CHÃO
    cena_desenhar();

    // - ACERVO / OBJETOS
    exponatos_desenhar();

    // troca os buffers pra animação
    glutSwapBuffers();
}

// --- FUNÇÃO DE REDIMENSIONAMENTO DA JANELA
void reshape(int w, int h) {
    // não deixa dividir por 0 se a janela for muito pequena
    if (h == 0) h = 1; 
    largura_janela = w;
    altura_janela = h;

    float ratio = (float)w / (float)h;

    glMatrixMode(GL_PROJECTION); // entra no modo de projeção
    glLoadIdentity();
    glViewport(0, 0, w, h);      // define a área de desenho
    
    // define a perspectiva: (angulo de visão, proporção tela, corte perto, corte longe)
    gluPerspective(45.0, ratio, 0.1, 100.0); 
    
    glMatrixMode(GL_MODELVIEW);  // retorna pro modo de visualização
}

// --- FUNÇÃO PARA TELA CHEIA NO F11
void alternar_tela_cheia() {
    // se estiver em modo janela, salva o estado atual da janela pra poder restaurar depois
    // e então coloca em tela cheia
    if (!tela_cheia) {
        janela_largura_anterior = glutGet(GLUT_WINDOW_WIDTH);
        janela_altura_anterior = glutGet(GLUT_WINDOW_HEIGHT);
        janela_pos_x_anterior = glutGet(GLUT_WINDOW_X);
        janela_pos_y_anterior = glutGet(GLUT_WINDOW_Y);

        glutFullScreen();
        tela_cheia = 1;
    }
    else {
        // restaura o tamanho e a posicao que a janela tinha antes para sair da tela cheia
        glutReshapeWindow(janela_largura_anterior, janela_altura_anterior);
        glutPositionWindow(janela_pos_x_anterior, janela_pos_y_anterior);
        tela_cheia = 0;
    }
}

// --- FUNÇÕES DE TECLADO
void tecla_pressionada(unsigned char key, int x, int y) {
    switch (key) {
        case 27:    // código ASCII pra tecla ESC fechar o programa
            exit(0);  
            break;
        case 't':
        case 'T':
            camera_alternar_modo();
            break;
        default:
            camera_tecla_pressionada(key);
    }
}

void tecla_solta(unsigned char key, int x, int y) {
    camera_tecla_solta(key);
}

// -- MOVIMENTAÇÃO DO MOUSE
void movimento_mouse(int x, int y){
    camera_processar_mouse(x, y, largura_janela, altura_janela);
}

// -- LOOP DE ATUALIZAÇÃO (manter 60 fps)
void atualizar(int valor) {
    int agora = glutGet(GLUT_ELAPSED_TIME);
    float dt = (agora - tempo_anterior) / 1000.0f; // converte pra segundos
    tempo_anterior = agora;

    // trava o dt pra evitar saltos gigantes se a janela ficar suspensa
    if (dt > 0.1f) 
        dt = 0.1f;

    camera_atualizar(dt);

    glutPostRedisplay();
    glutTimerFunc(16, atualizar, 0);
}

// --- FUNÇÃO MAIN
int main(int argc, char** argv) {
    // inicialização do GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(LARGURA_JANELA, ALTURA_JANELA); // tamanho inicial da janela
    glutCreateWindow("Projeto CG - Museu Virtual");

    init();

    // registro dos callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(tecla_pressionada);
    glutKeyboardUpFunc(tecla_solta);
    glutPassiveMotionFunc(movimento_mouse);

    glutTimerFunc(16, atualizar, 0);

    // entra no loop infinito do OpenGL
    glutMainLoop();
    
    return 0;
}
