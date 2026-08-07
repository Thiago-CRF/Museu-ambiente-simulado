// contantes e tipos compartilhados

#ifndef MUSEU_COMMON_H
#define MUSEU_COMMON_H

// TIPOS COMPARTILHADOS
typedef struct
{
    float x, y, z;
} Vetor3;

// CONSTANTES GLOBAIS
#define LARGURA_JANELA 800
#define ALTURA_JANELA 600

// limites da sala pra colisão simples da camera
#define SALA_MIN_X -9.5f
#define SALA_MAX_X 9.5f
#define SALA_MIN_Z -9.5f
#define SALA_MAX_Z 9.5f

#endif