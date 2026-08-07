// luzes e materiais

#ifndef MUSEU_ILUMINACAO_H
#define MUSEU_ILUMINACAO_H

// configura e habilita as fontes de luz (GL_LIGHT0, GL_LIGHT1, ...)
void iluminacao_iniciar(void);

// atualiza posição/parâmetros das luzes a cada frame (se tiver luzes móveis, talvez não tenha)
void iluminacao_atualizar(void);

#endif 