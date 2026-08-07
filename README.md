# Museu-ambiente-simulado
Ambiente simulado de um museu feito em OpenGL 2.1 usando C. Projeto final da matéria de introdução à copmutação gráfica - UFPB

## Como compilar e executar
#### No linux:
- Instalar dependências
    `sudo apt-get install build-essential freeglut3-dev libglew-dev libglfw3-dev`

- Compilar:
    Abra o terminal na pasta do projeto e execute:
    ´gcc main.c -o museu -lGL -lGLU -lglut`
- Executar:
    `./museu`