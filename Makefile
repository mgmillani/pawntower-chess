CC = gcc
CFLAGS = -Wall -Wextra -I$(INC)
SRC = src
OBJ = obj
BIN = bin
INC = include
LIBS = -lGL -lSDLmain -lSDL
OBJECTS = $(OBJ)/main.o $(OBJ)/drawer.o $(OBJ)/frameControl.o
NAME = pawntower-chess

all: release

release: CFLAGS += -s -O3
release: main

debug: CFLAGS += -g
debug: main

main: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LIBS) -o $(BIN)/$(NAME)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(OBJ)/*.o
	rm -f $(BIN)/$(NAME)
