// camera modo livre (WASD) e modo tour (curvas bézier)

#ifndef MUSEU_CAMERA_H
#define MUSEU_CAMERA_H

#include "common.h"

typedef enum {
    CAMERA_MODO_LIVRE,
    CAMERA_MODO_TOUR
} ModoCamera;

typedef struct {
    float x, y, z;        // posição
    float yaw;         // yaw, em graus
    float pitch;      // pitch, em graus
    ModoCamera modo;
} Camera;

// inicia a câmera com posição e orientação padrão
void camera_iniciar(void);

// atualiza a posição da câmera a cada frame, de acordo com o modo atual
// dt: delta de tempo decorrido desde o ultimo frame, em segundos
void camera_atualizar(float dt);

// aplica a transformação de visualização (chama gluLookAt internamente)
void camera_aplicar_visualizacao(void);

// alterna entre modo livre e modo tour
void camera_alternar_modo(void);

// trata o movimento do mouse (chamado pelo callback de passive motion)
void camera_processar_mouse(int x, int y, int larguraJanela, int alturaJanela);

// trata teclas pressionadas/soltas (WASD)
void camera_tecla_pressionada(unsigned char tecla);
void camera_tecla_solta(unsigned char tecla);

// retorna a posicao atual da camera (usado pra iluminacao)
Vetor3 camera_obter_posicao(void);

// desenha o traçado do percurso do tour, apenas quando o modo tour esta ativo
void camera_desenhar_caminho_tour(void);

#endif // MUSEU_CAMERA_H