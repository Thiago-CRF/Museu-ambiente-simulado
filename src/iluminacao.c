#include <GL/glut.h>

#include "iluminacao.h"

void iluminacao_iniciar(){
    glEnable(GL_LIGHTING);

    // faz com que glColor3f contie definindo a cor do material
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // brilho especular padrao dos materiais (no momento esta o mesmo brilho pra todos materiais)
    // mudar isso dentro de exibicoes.c, chamando glMaterialfv individualmente pra cada item de exibição
    GLfloat especular_material[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, especular_material);
    glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);

    // --- PONTOS DE LUZ
    // luz 0: iluminaçao geral da sala, pontual no centro
    GLfloat luz0_ambiente[]  = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat luz0_difusa[]    = { 0.55f, 0.55f, 0.50f, 1.0f };
    GLfloat luz0_especular[] = { 0.3f, 0.3f, 0.3f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, luz0_ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luz0_difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luz0_especular);
    glEnable(GL_LIGHT0);

    // luz 1: spot quente sobre estatuas no fundo (apenas base, mudar quando mudar as posições)

}