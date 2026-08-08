CC = gcc
CFLAGS = -Wall -Wextra -O2 -Isrc -Iexternal
LDFLAGS = -lGL -lGLU -lglut -lm
ALVO = museu

DIR_BUILD = build

FONTES = $(wildcard src/*.c)
OBJETOS = $(patsubst src/%.c,$(DIR_BUILD)/%.o,$(FONTES))

all: $(ALVO)

$(ALVO): $(OBJETOS)
	$(CC) $(OBJETOS) -o $@ $(LDFLAGS)
 
$(DIR_BUILD)/%.o: src/%.c | $(DIR_BUILD)
	$(CC) $(CFLAGS) -c $< -o $@
 
$(DIR_BUILD):
	mkdir -p $(DIR_BUILD)


run: all
	./$(ALVO)

clean:
	rm -f src/*.o $(ALVO)

.PHONY: all run clean