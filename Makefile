CC = gcc
CFLAGS = -Wall -Wextra -I$(INC)
SRC = src
OBJ = obj
BIN = bin
INC = include
LIBS = -lGL -lSDLmain -lSDL
OBJECTS = $(OBJ)/main.o $(OBJ)/ia.o $(OBJ)/drawer.o $(OBJ)/interface.o $(OBJ)/frameControl.o $(OBJ)/movement.o $(OBJ)/init.o $(OBJ)/control.o $(OBJ)/random.o
NAME = pawntower-chess

all: $(OBJ) $(BIN) release

$(OBJ):
	mkdir $(OBJ)
$(BIN):
	mkdir $(BIN)

release: CFLAGS += -s -O3
release: main

debug: CFLAGS += -g -O0
debug: main

main: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LIBS) -o $(BIN)/$(NAME)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(OBJ)/*.o
	rm -f $(BIN)/$(NAME)
