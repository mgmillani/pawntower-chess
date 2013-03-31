#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "drawer.h"

#include "definitions.h"

/**
  * desenha o tabuleiro na regiao informada
  */
void drawBoard(t_rect *region, char board[8][8], GLfloat evenColor[3], GLfloat oddColor[3])
{
	//calcula o tamanho de cada celula
	unsigned int cellW = region->w/8;
	unsigned int cellH = region->h/8;
	unsigned int x = region->x;
	unsigned int y = region->y;
	//desenha os retangulos pares
	unsigned int i;
	glColor3fv(evenColor);
	//para cada linha
	for(i=0 ; i<8 ; i++,y+=cellH)
	{
		//para cada coluna
		unsigned int j;
		x = region->x + (i&1)*cellW;
		for(j=i&1 ; j<8 ; j+=2,x+=cellW*2)
		{
			glBegin(GL_QUADS);
			glVertex2i(x,y);	//top-left
			glVertex2i(x+cellW,y);	//top-right
			glVertex2i(x+cellW,y+cellH);	//bottom-right
			glVertex2i(x,y+cellH);	//bottom-left
			glEnd();
		}
	}

	//desenha as celulas impares
	glColor3fv(oddColor);
	y = region->y;
	//para cada linha
	for(i=0 ; i<8 ; i++,y+=cellH)
	{
		//para cada coluna
		unsigned int j;
		x = region->x + ((i+1)&1)*cellW;
		for(j=(i+1)&1 ; j<8 ; j+=2,x+=cellW*2)
		{
			glBegin(GL_QUADS);
			glVertex2i(x,y);	//top-left
			glVertex2i(x+cellW,y);	//top-right
			glVertex2i(x+cellW,y+cellH);	//bottom-right
			glVertex2i(x,y+cellH);	//bottom-left
			glEnd();
		}
	}


}
