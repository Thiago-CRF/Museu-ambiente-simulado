CC = gcc
CFLAGS = -Wall -Wextra -Isrc -Iexternal
LDFLAGS = -lGL -lGLU -lglut -lm
ALVO = museu

FONTES = $(wildcard src/*.c)
OBJETOS = $(FONTES:.c=.o)

all: $(ALVO)

$(ALVO): $(OBJETOS)
	$(CC) $(OBJETOS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(ALVO)

clean:
	rm -f src/*.o $(ALVO)

.PHONY: all run clean