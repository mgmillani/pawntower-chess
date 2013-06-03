CC = gcc
CFLAGS = -Wall -Wextra -g -I$(INC)
SRC = ../src
OBJ = obj
BIN = .
INC = ../include
LIBS = -lGL -lSDLmain -lSDL
#OBJECTS = $(OBJ)/main.o $(OBJ)/drawer.o $(OBJ)/frameControl.o $(OBJ)/movement.o
#NAME = pawntower-chess

all: $(OBJ) pieceMovement heuristicFunction
$(OBJ):
	mkdir $(OBJ)

pieceMovement: $(OBJ)/movement.o $(OBJ)/init.o
	$(CC) $(CFLAGS) pieceMovement.c $(OBJ)/movement.o $(OBJ)/init.o $(LIBS) -o $(BIN)/pieceMovement.test

heuristicFunction: $(OBJ)/ia.o $(OBJ)/control.o $(OBJ)/movement.o $(OBJ)/random.o
	$(CC) $(CFLAGS) $@.c $^  $(LIBS) -o $(BIN)/$@.test

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(OBJ)/*.o
	rm -f $(BIN)/*.test
