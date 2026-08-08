#include "curvas.h"

// avalia para ver se a curva de bezier cubica é valida
// usa polinomial de bernstein
// b(t) = (1-t)^3*p0 + 3*(1-t)^2*t*p1 + 3*(1-t)*t^2*p2 + t^3*p3
Vetor3 curva_avaliar(CurvaBezier curva, float t) {
    float u = 1.0f - t;
}