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

// Constante PI definido para testar se não da bug visual na camera
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// limites físicos da planta do museu, 
// usadas para colisão e para desenhar a cena

// Sala 1 (Esculturas) - Fica à esquerda
#define S1_XMIN -25.0f
#define S1_XMAX -5.0f
#define S1_ZMIN -10.0f
#define S1_ZMAX  10.0f

// Corredor - Fica no centro
#define C_XMIN  -5.0f
#define C_XMAX   5.0f
#define C_ZMIN  -3.0f // Mais estreito que as salas
#define C_ZMAX   3.0f

// Sala 2 (Quadros) - Fica à direita
#define S2_XMIN  5.0f
#define S2_XMAX  25.0f
#define S2_ZMIN -10.0f
#define S2_ZMAX  10.0f

// --- ALTURAS (PÉ-DIREITO) ---
#define ALTURA_SALA     7.0f  // Salas bem altas
#define ALTURA_CORREDOR 4.0f  // Corredor mais baixo

// distancia para camera não bater na parede
#define MARGEM_COLISAO 0.5f

#endif