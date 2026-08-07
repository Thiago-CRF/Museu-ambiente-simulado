#include <GL/glut.h>
#include <stdlib.h>
// gcc main.c -o museu -lGL -lGLU -lglut

// --- FUNÇÃO DE INICIALIZAÇÃO
void init(void) {
    // define a cor de fundo da janela ((0.1, 0.1, 0.1, 1.0)cinza escuro)
    glClearColor(0.1, 0.1, 0.1, 1.0); 
    
    glEnable(GL_DEPTH_TEST); // ativa o z-buffer
}

// --- FUNÇÃO DE DESENHO (DISPLAY)
void display(void) {
    // limpa os buffers de cor e de profundidade
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // - CÂMERA
    // atualmente a camera é estática e olhando pro centro, um pouco de cima
    gluLookAt(0.0, 2.0, 5.0,   // posição da câmera (X, Y, Z)
              0.0, 0.0, 0.0,   // para onde a câmera está olhando
              0.0, 1.0, 0.0);  // qual eixo é o "cima" (Vetor UP)

       // - AMBIENTE / CHÃO
    glColor3f(0.3, 0.3, 0.3); // define a cor do chão ((0.3, 0.3, 0.3)cinza medio)
    glBegin(GL_QUADS);
        glVertex3f(-10.0, 0.0, -10.0);
        glVertex3f(-10.0, 0.0,  10.0);
        glVertex3f( 10.0, 0.0,  10.0);
        glVertex3f( 10.0, 0.0, -10.0);
    glEnd();

    // troca os buffers pra animação
    glutSwapBuffers();
}

// --- FUNÇÃO DE REDIMENSIONAMENTO DA JANELA
void reshape(int w, int h) {
    // não deixa dividir por 0 se a janela for muito pequena
    if (h == 0) h = 1; 
    float ratio = w * 1.0 / h;

    glMatrixMode(GL_PROJECTION); // entra no modo de projeção
    glLoadIdentity();
    glViewport(0, 0, w, h);      // define a área de desenho
    
    // define a perspectiva: (angulo de visão, proporção tela, corte perto, corte longe)
    gluPerspective(45.0, ratio, 0.1, 100.0); 
    
    glMatrixMode(GL_MODELVIEW);  // retorna pro modo de visualização
}

// --- FUNÇÃO DE TECLADO
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:    // código ASCII pra tecla ESC fechar o programa
            exit(0);  
            break;
    }
    glutPostRedisplay(); // redesenha a janela
}

// --- FUNÇÃO MAIN
int main(int argc, char** argv) {
    // inicialização do GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600); // tamanho inicial da janela
    glutCreateWindow("Projeto CG - Museu Virtual");

    init();

    // registro dos callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    // entra no loop infinito do OpenGL
    glutMainLoop();
    
    return 0;
}