#include <GL/gl.h>
#include "curvas.h"

// avalia para ver se a curva de bezier cubica é valida
// usa polinomial de bernstein
// b(t) = (1-t)^3*p0 + 3*(1-t)^2*t*p1 + 3*(1-t)*t^2*p2 + t^3*p3
Vetor3 curva_avaliar(CurvaBezier curva, float t) {
    // t é parametro da curva que indica onde esta o ponto no trajeto, entro 0(começa) e 1(fim)
    // u é só pra não repetir 1-t varias vezes
    float u = 1.0f - t;
    
    // coeficientes de bernstein de bézier cúbica
    float b0 = u * u * u;
    float b1 = 3.0f * u * u * t;
    float b2 = 3.0f * u * t * t;
    float b3 = t * t * t;

    // calculo dos pontos no x y z, aplicando a fórmula
    Vetor3 ponto;
    ponto.x = b0 * curva.p0.x + b1 * curva.p1.x + b2 * curva.p2.x + b3 * curva.p3.x;
    ponto.y = b0 * curva.p0.y + b1 * curva.p1.y + b2 * curva.p2.y + b3 * curva.p3.y;
    ponto.z = b0 * curva.p0.z + b1 * curva.p1.z + b2 * curva.p2.z + b3 * curva.p3.z;

    return ponto;
}

void curva_desenhar(CurvaBezier curva, int num_segmentos) {
    // empacota os pontos de controle no layout contiguo que o glMap1f espera
    GLfloat pontos_controle[4][3] = {
        {curva.p0.x, curva.p0.y, curva.p0.z},
        {curva.p1.x, curva.p1.y, curva.p1.z},
        {curva.p2.x, curva.p2.y, curva.p2.z},
        {curva.p3.x, curva.p3.y, curva.p3.z}
    };

    // mapeia pra desnehar a curva
    glMap1f(GL_MAP1_VERTEX_3, 0.0f, 1.0f, 3, 4, &pontos_controle[0][0]);
    glEnable(GL_MAP1_VERTEX_3);

    // gera os vertices da curva pra conectar os segmentos
    glBegin(GL_LINE_STRIP);
    for(int i = 0; i <= num_segmentos; i++){
        glEvalCoord1f((float)i / (float)num_segmentos);
    }
    glEnd();
    
    // desativa o evaluator no fim pra não mexer no estado do opengl
    glDisable(GL_MAP1_VERTEX_3);
}