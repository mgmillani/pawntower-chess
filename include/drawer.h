#ifndef DRAWER_H
#define DRAWER_H

#include "definitions.h"

/**
  * desenha o tabuleiro na regiao informada
  */
void drawBoard(t_rect *region, char board[8][8], GLfloat evenColor[3], GLfloat oddColor[3]);

#endif // DRAWER_H
