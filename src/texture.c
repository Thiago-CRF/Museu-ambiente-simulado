#include <stdio.h>
#include <stdbool.h>
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"

GLuint texturaChao = 0; // Variável global para armazenar o ID da textura do chão

GLuint textura_carregar(const char *caminhoArquivo) {
    int largura, altura, canais;

    stbi_set_flip_vertically_on_load(true); // Inverte a imagem verticalmente ao carregar

    unsigned char *dados = stbi_load(caminhoArquivo, &largura, &altura, &canais, 0);
    if (!dados) {
        fprintf(stderr, "falha ao carregar a textura: %s\n", caminhoArquivo);
        return 0;
    }

    GLuint idTextura;
    glGenTextures(1, &idTextura);
    glBindTexture(GL_TEXTURE_2D, idTextura);

    // parametros de repeticao e filtragem
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (canais == 3) {
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, largura, altura,
                          GL_RGB, GL_UNSIGNED_BYTE, dados);
    } else if (canais == 4) {
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, largura, altura,
                          GL_RGBA, GL_UNSIGNED_BYTE, dados);
    }

    stbi_image_free(dados);
    return idTextura;
}

/* void texturizarChao() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaChao);
    glColor3f(1.0f, 1.0f, 1.0f);    // Define a máscara de cor para branco, para não mudar na textura

    float tamanho = 30.0f;          // Tamanho do plano
    float repeticoes = 15.0f;       // Quantas vezes a imagem de textura se repete

    glBegin(GL_QUADS);              // Desenha o plano em que será aplicada a textura
        glTexCoord2f(0.0f, repeticoes);       glVertex3f(-tamanho, -0.0f, -tamanho);
        glTexCoord2f(0.0f, 0.0f);             glVertex3f(-tamanho, -0.0f,  tamanho);
        glTexCoord2f(repeticoes, 0.0f);       glVertex3f( tamanho, -0.0f,  tamanho);
        glTexCoord2f(repeticoes, repeticoes); glVertex3f( tamanho, -0.0f, -tamanho);
    glEnd();

    glDisable(GL_TEXTURE_2D);
} */