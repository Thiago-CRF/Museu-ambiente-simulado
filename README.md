

# Museu ambiente simulado - Computação gráfica
Ambiente simulado de um museu feito em OpenGL 2.1 usando C. Onde a câmera percorre o espaço em modo livre (com WASD e mouse), ou um um modo tour automático usando curvas de bézier
Projeto final da matéria de introdução à copmutação gráfica - UFPB

## Demonstração
![Demonstração da navegação livre](media/demo.gif)

Modo tour automatico, usando curvas de bézier:

https://github.com/user-attachments/assets/8ebb57ad-df6e-4977-b4ee-fa430ba52740


## Integrantes do grupo
- Thiago César
- José Roberto
- Vanderley Ferreira

## Sobre o projeto
O programa renderiza uma sala de museu (paredes, piso e teto texturizados) com diferentes itens de exibição, e iluminados por diferentes fontes de luz.

O usuário pode:
- Navegar livremente pelo ambiente em primeira pessoa (modo livre);
- Ativar um tour automático da câmera, que percorre trajetórias suaves definidas por curvas de Bézier cúbicas (modo tour);
- Alternar entre os dois modos a qualquer momento pressionando `T` (o modo livre retoma o controle automaticamente ao usar WASD durante o tour).

Desenvolvido usando C puro, com OpenGL 2.1 e GLUT/freeglut

## Dependências
- `gcc` e GNU Make
- OpenGL 2.1
- `GLUT` ou `freeglut`
- [`stb_image.h`](https://github.com/nothings/stb) — biblioteca *header-only* para carregamento de imagens (usadas nas texturas), de domínio público / licença MIT dupla

No Linux:

```bash
sudo apt install build-essential freeglut3-dev
```

A biblioteca `stb_image.h` já vem internamente no projeto, dentro de `external/`, e não é preciso intalar nada por fora pra carregar as texturas.

## Como compilar e executar

```bash
make
./museu
```
ou somente
```bash
make run
```

O `Makefile` compila usando `gcc`, linkando `-lGL -lGLU -lglut -lm` do OpenGL e incluindo os diretórios `-Iexternal -Isrc`.

## Controles

| Tecla / ação | Efeito |
|---|---|
| `W A S D` | Movimento em modo livre |
| Mouse | Olhar ao redor (modo livre) |
| `T` | Alterna entre modo livre e modo tour |
| `W A S D` durante o tour | Retoma o controle manual da câmera |

## Principais problemas encontrados

- Limite de 8 luzes do OpenGL. O museu tem 14 fontes de luz posicionadas (2 lustres, 4 spots das estátuas, 3 do corredor e 5 spots da sala 2), mas o OpenGL 2.1 só tem 8 slots (`GL_LIGHT0` a `GL_LIGHT7`). Foi preciso criar um sistema de alocação dinâmica em `iluminacao_atualizar()`, que identifica em qual ambiente a câmera está e distribui os slots por prioridade: os dois lustres são sempre alocados primeiro, e os restantes vão para os spots do ambiente atual e dos ambientes vizinhos. Também foi necessário desabilitar explicitamente os slots que sobram, senão luzes de uma alocação anterior continuavam acesas em posições erradas.

- Artefatos de iluminação por vértice. O OpenGL 2.1 calcula a iluminação por vértice e interpola, então um piso ou parede feito de um único quad tinha apenas 4 vértices calcular a iluminação, o resultado era iluminação chapada e com faixas visíveis, com o círculo de luz dos spots sem aparecer. A solução foi dividir a geometria dos quads em uma grade 12×12 (`SUBDIVISOES_QUAD`), aumentando a densidade de vértices pra melhorar a amostragem da luz.

- Evaluators do OpenGL não retornam dados para a aplicação. Como as curvas de Bézier com `glMap1f` / `glEvalCoord1f` foram feitas para *desenhar* a curva dentro de um `glBegin/glEnd`, ela não devolve o ponto avaliado como valor pro código. Como a câmera precisa da posição em `t` como dado (para passar ao `gluLookAt`), foi preciso implementar `curva_avaliar()` manualmente com a forma polinomial de Bernstein.

- Colisão em ambiente não-retangular. O museu tem duas salas ligadas por um corredor, então uma colisão simples nos limites de uma caixa não funciona. Foi preciso definir regiões retangulares navegáveis e testar a colisão eixo a eixo (X e Z separadamente)..

- Houveram vários erros pequenos de iluminação e de cor quando implementado as estátuas e os quadros, que foram corrigidos com testes, mudando os parametros de luz e cor.

- Na movimentação da câmera no começo teve um pequeno bug de que a câmera voava quando olhava para cima, pois não tinhamos travado a movimentação no plano vertical.

- Quando foi implementado a normalização de vetores no modo livre da câmera, foi preciso um guarda contra divisão por zero, pois enquanto movimenta a camera o vetor direção chega a um valor módulo muito próximo de 0.

- Por conta de `stb_image.h` ser uma biblioteca header-only, foi preciso definir `STB_IMAGE_IMPLEMENTATION` somente em um arquivo `.c`. Pois quando definido em mais de um acontece erro de linkagem

## O que pode ser melhorado

- Adicionar fontes de luz dinâmicas e exibições dinâmicas, que se movem pela cena

- Sincronizar corpo do spot com a luz alocada. Como `iluminacao_desenhar_spots()` percorre o array inteiro sem checar quais luzes realmente receberam slot, aparecem lâmpadas com `GL_EMISSION` aceso no teto mesmo quando a luz dela está desligada. Seria fazer uma função que identifica isso e muda o `GL_EMISSION` quando desenhar a luz a cada quadro.

- Implementar sombras, os objetos são iluminados, mas não bloqueiam luz. Daria para fazer sombras projetadas por matriz de projeção no plano do chão, ou shadow mapping.

- Adicionar uma detecção de colisão mais completa e robusta, pois no momento a colisão é apenas com os limites da sala

- Fazer um sistema de interação com as exibições, que mostrasse nome e autor ao clicar na exibição

- Expandir a variedade de itens de exibição e as texturas



## Os elementos das atividades práticas
| Atividade prática | Onde está implementado | Descrição |
|---|---|---|
| Primitivas e cor |  |  |
| Visualização 3D / hierarquia | `exibicoes.c` (`exponatos_desenhar`) | Itens de exibição montados com transformações hierárquicas (pilha de matrizes) |
| Visibilidade / z-buffer | `main.c` (`GL_DEPTH_TEST`) | Teste de profundidade garante oclusão correta entre os objetos da cena |
| Iluminação | `iluminacao.c` | `GL_LIGHT0` configurada em `iluminacao_iniciar()`; posição atualizada a cada frame em `iluminacao_atualizar()` (depende da modelview, por isso é chamada após `camera_aplicar_visualizacao()`); materiais por objeto via `glMaterialfv` em `exibicoes.c` |
| Texturas | `texture.c` | Wrapper sobre a `stb_image.h`; usada para aplicar texturas de piso/parede em `cena.c` |
| Curvas paramétricas | `curvas.c` (`curva_avaliar`) | Curva de Bézier cúbica avaliada em `t ∈ [0,1]`, usada em `camera.c` (`atualizar_modo_tour`) pro deslocamento automático da câmera |


## O que cada integrante fez

#### Thiago César
- Base inicial do projeto e organização da estrutura dos arquivos
- Sistema de câmera completo (`camera.c`): modo livre em primeira pessoa com WASD e mouse, cálculo dos vetores de direção e perpendicular e alternância de modos com a tecla `T`
- Detecção de colisão por regiões navegáveis, com teste em cada eixo para ter deslizamento nas paredes
- Modo tour automático: percurso de 11 waypoints usando splines de Catmull-Rom, alvos de visão por waypoint e transição suave ao mudar alvo de visão.
- Curvas de Bézier (`curvas.c`): `curva_avaliar()` pela forma polinomial de Bernstein para posicionar a câmera.
- Sistema de iluminação (`iluminacao.c`): alocação dinâmica dos 8 slots do OpenGL por ambiente, com prioridade para os lustres; configuração de cor, atenuação e abertura de cone dos spots
- Geometria das fontes de luz: lustres de teto como semiesferas com `GL_EMISSION` e cabo, e spots como uma caixa preta e um disco de luz (para representar uma lâmpada led) feito com `gluDisk`
- Subdivisão da geometria dos quads em grade 12×12 pra corrigir os artefatos de iluminação por vértice
- Alternância de tela cheia com F11, preservando o estado da janela para voltar ao modo janela sem bugar
- Algumas das estatuas e fonte da sala dos quadros
- `Makefile`
- Escrita do README

### José Roberto
- [preencher]

### Vanderley Ferreira
- [preencher]
