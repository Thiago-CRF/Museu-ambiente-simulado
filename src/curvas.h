// curvas de bézier pra movimentação automatica

#ifndef MUSEU_CURVAS_H
#define MUSEU_CURVAS_H

#include "common.h"

typedef struct {
    Vetor3 p0, p1, p2, p3; // pontos de controle
} CurvaBezier;

// avalia a curva de Bézier cúbica no parâmetro t (0.0 a 1.0)
Vetor3 curva_avaliar(CurvaBezier curva, float t);

#endif