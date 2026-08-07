// usa do stb_image (wrapper)

#ifndef MUSEU_TEXTURE_H
#define MUSEU_TEXTURE_H

#include <GL/gl.h>

// carrega uma imagem do disco e retorna o ID da textura OpenGL gerada
// retorna 0 caso falhe
GLuint textura_carregar(const char *caminhoArquivo);

#endif 