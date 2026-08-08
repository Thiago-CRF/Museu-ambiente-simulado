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

