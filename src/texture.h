// usa do stb_image (wrapper)

#ifndef MUSEU_TEXTURE_H
#define MUSEU_TEXTURE_H

#include <GL/glut.h>

// Variável que armazena o ID da textura está declarada em src/textura.c
//extern GLuint texturaChao;

GLuint textura_carregar(const char *caminhoArquivo);    // Função para carregar uma textura a partir de um arquivo de imagem
//void texturizarChao();                                  // Função para definir o chão com a textura carregada

#endif