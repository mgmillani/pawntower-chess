#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "drawer.h"
#include "movement.h"

#include "definitions.h"

#include "debug.h"

/**
  * desenha as pecas e o tabuleiro na regiao dada da tela
  */
void desenhaJogo(t_rect *regiao, t_jogo *jogo)
{
	desenhaTabuleiro(regiao);
	desenhaPecas(regiao,&(jogo->p1),P1);
	desenhaPecas(regiao,&(jogo->p2),P2);
}

/**
  * desenha o tabuleiro na regiao dada
  */
void desenhaTabuleiro(t_rect *regiao)
{
   int coluna,linha;
   double minx,miny,maxx,maxy;
   double cellW = regiao->w/8;
   double cellH = regiao->h/8;
   glColor3f(1.0,1.0,1.0);
   glBegin(GL_QUADS);
   glVertex2d(regiao->x,regiao->y);
   glVertex2d(regiao->x,regiao->y+regiao->h);
   glVertex2d(regiao->x+regiao->w,regiao->y+regiao->h);
   glVertex2d(regiao->x+regiao->w,regiao->y);
   glEnd();
   glColor3f(0.0,0.0,0.0);
   for(coluna=0;coluna<8;coluna=coluna+2)
   {
      for(linha=0;linha<8;linha++)
      {
	 minx = coluna*cellW + cellW*(linha%2) + regiao->x ;
	 maxx = (coluna+1)*cellW+ cellW*(linha%2) + regiao->x;
	 miny = linha*cellH + regiao->y;
	 maxy = (linha+1)*cellH  + regiao->y;
	 glBegin(GL_QUADS);
	    glVertex2d(minx,miny);
	    glVertex2d(minx,maxy);
	    glVertex2d(maxx,maxy);
	    glVertex2d(maxx,miny);
	 glEnd();
      }
   }
   return;
}

/**
  * desenha as pecas do jogador na regiao dada
  */
void desenhaPecas(t_rect *regiao,t_jogador *jogador, char time)
{
	unsigned int i;
	//desenha os peoes
	for(i=0 ; i<jogador->numPeoes ; i++)
		desenhaPeao(regiao,jogador->peaoPos[i]/8,jogador->peaoPos[i]%8,time);
	//desenha as torres
	for(i=0 ; i<jogador->numTorres ; i++)
		desenhaTorre(regiao,jogador->torrePos[i]/8,jogador->torrePos[i]%8,time);
}

/**
  * desenha um peao na linha e coluna dadas do tabuleiro (que esta na regiao passada)
  */
void desenhaPeao(t_rect *regiao,int linha, int coluna,char time)
{
  float cor;
  if(time==P1)
    cor = 1;
  else
    cor = 0;

  double cellW = regiao->w/8;
  double cellH = regiao->h/8;

  double minx = coluna*cellW  + regiao->x;
  //double maxx = coluna*cellW  + regiao->x;
  double miny = linha*cellH + regiao->y;
  //double maxy = linha*cellH + regiao->y;
  glColor3f (cor, cor, cor);
  glTranslatef(minx,miny,0.0);
   //base peao
  glBegin (GL_POLYGON);
    glVertex2f (0.1*cellW,0.9*cellH);
    glVertex2f (0.1*cellW,0.8*cellH);
    glVertex2f (0.3*cellW,0.75*cellH);
    glVertex2f (0.7*cellW,0.75*cellH);
    glVertex2f (0.9*cellW,0.8*cellH);
    glVertex2f (0.9*cellW,0.9*cellH);
  glEnd();

  //corpo peao
   glBegin (GL_POLYGON);
     glVertex2f (0.7*cellW  ,0.75*cellH);
     glVertex2f (0.525*cellW,0.25*cellH);
     glVertex2f (0.475*cellW,0.25*cellH);
     glVertex2f (0.3*cellW  ,0.75*cellH);
   glEnd();
  //esfera topo peao *cellH
  glBegin (GL_POLYGON);
    glVertex2f (0.525*cellW,0.25*cellH);
    glVertex2f (0.6*cellW,0.2*cellH);
    glVertex2f (0.6*cellW,0.15*cellH);
    glVertex2f (0.55*cellW,0.1*cellH);
    glVertex2f (0.45*cellW,0.1*cellH);
    glVertex2f (0.4*cellW,0.15*cellH);
    glVertex2f (0.4*cellW,0.2*cellH);
    glVertex2f (0.475*cellW,0.25*cellH);
  glEnd();
  //linhas *cell
  glLineWidth(3);
  glBegin (GL_LINE_LOOP);
  glColor3f (1-cor, 1-cor, 1-cor);
  glVertex2f (0.9*cellW,0.9*cellH);
  glVertex2f (0.9*cellW,0.8*cellH);
  glVertex2f (0.7*cellW,0.75*cellH);
  //linha esfera topo peao
  glVertex2f (0.525*cellW ,0.25*cellH);
  glVertex2f (0.6*cellW   ,0.2*cellH);

  glVertex2f (0.6*cellW   ,0.15*cellH);
  glVertex2f (0.55*cellW  ,0.1*cellH);

  glVertex2f (0.45*cellW  ,0.1*cellH);
  glVertex2f (0.4*cellW   ,0.15*cellH);

  glVertex2f (0.4*cellW   ,0.2*cellH);
  glVertex2f (0.475*cellW ,0.25*cellH);
  //fim esfera topo


  glVertex2f (0.3*cellW ,0.75*cellH);
  glVertex2f (0.1*cellW,0.8*cellH);
  glVertex2f (0.1*cellW,0.9*cellH);
  glEnd();
  glTranslatef(-minx,-miny,0.0);
/*
	glBegin(GL_QUADS);

	glVertex2d(minx,miny);
	glVertex2d(maxx,miny);
	glVertex2d(maxx,maxy);
	glVertex2d(minx,maxy);

	glEnd();
*/
	return;
}

/**
  * desenha um peao na linha e coluna dadas do tabuleiro (que esta na regiao passada)
  */
void desenhaTorre(t_rect *regiao,int linha, int coluna,char time)
{
   float cor=0.5;
   if(time==P1)
     glColor3f(1.0,0.0,1.0);
   else
     glColor3f(1.0,1.0,0.0);

   double cellW = regiao->w/8;
   double cellH = regiao->h/8;

   double minx = coluna*cellW  + regiao->x;
   //double maxx = (coluna+1)*cellW  + regiao->x;
   double miny = linha*cellH + regiao->y;
   // double maxy = (linha+1)*cellH + regiao->y;
   glTranslatef(minx,miny,0.0);
   glColor3f (cor, cor, cor);
   //base torre
   glBegin (GL_POLYGON);
      glVertex2f (0.9*cellW,0.9*cellH);
      glVertex2f (0.9*cellW,0.75*cellH);
      glVertex2f (0.1*cellW,0.75*cellH);
      glVertex2f (0.1*cellW,0.9*cellH);
   glEnd();
   //corpo torre
   glBegin (GL_POLYGON);
      glVertex2f (0.7*cellW,0.8*cellH);
      glVertex2f (0.7*cellW,0.3*cellH);
      glVertex2f (0.3*cellW,0.3*cellH);
      glVertex2f (0.3*cellW,0.8*cellH);
   glEnd();
   //topo torre
   glBegin (GL_QUADS);
      glVertex2f (0.8*cellW,0.3*cellH);
      glVertex2f (0.2*cellW,0.3*cellH);
      glVertex2f (0.2*cellW,0.2*cellH);
      glVertex2f (0.8*cellW,0.2*cellH);
   glEnd();
   glBegin (GL_QUADS);
      glVertex2f (0.8*cellW,0.2*cellH);
      glVertex2f (0.65*cellW,0.2*cellH);
      glVertex2f (0.65*cellW,0.1*cellH);
      glVertex2f (0.8*cellW,0.1*cellH);
   glEnd();
   glBegin (GL_QUADS);
      glVertex2f (0.575*cellW,0.2*cellH);
      glVertex2f (0.425*cellW,0.2*cellH);
      glVertex2f (0.425*cellW,0.1*cellH);
      glVertex2f (0.575*cellW,0.1*cellH);
   glEnd();
   glBegin (GL_QUADS);
      glVertex2f (0.35*cellW,0.2*cellH);
      glVertex2f (0.2*cellW,0.2*cellH);
      glVertex2f (0.2*cellW,0.1*cellH);
      glVertex2f (0.35*cellW,0.1*cellH);
   glEnd();
   glColor3f(1.0, 0.0, 1.0);
   //contorno
   glLineWidth(3);
   glBegin (GL_LINE_LOOP);
   //glColor3f (1-cor, 1-cor, 1-cor);
      glVertex2f (0.9*cellW,0.9*cellH);
      glVertex2f (0.9*cellW,0.75*cellH);
      glVertex2f (0.7*cellW,0.75*cellH);
      glVertex2f (0.7*cellW,0.3*cellH);
      //estrutura topo torre
      glVertex2f (0.8*cellW,0.3*cellH);
      glVertex2f (0.8*cellW,0.1*cellH);
      glVertex2f (0.65*cellW,0.1*cellH);
      glVertex2f (0.65*cellW,0.2*cellH);
      glVertex2f (0.575*cellW,0.2*cellH);
      glVertex2f (0.575*cellW,0.1*cellH);
      glVertex2f (0.425*cellW,0.1*cellH);
      glVertex2f (0.425*cellW,0.2*cellH);
      glVertex2f (0.35*cellW,0.2*cellH);
      glVertex2f (0.35*cellW,0.1*cellH);
      glVertex2f (0.2*cellW,0.1*cellH);
      glVertex2f (0.2*cellW,0.3*cellH);
      //fim  topo
      glVertex2f (0.3*cellW,0.3*cellH);
      glVertex2f (0.3*cellW,0.75*cellH);
      glVertex2f (0.1*cellW,0.75*cellH);
      glVertex2f (0.1*cellW,0.9*cellH);
   glEnd();
   glTranslatef(-minx,-miny,0.0);
/*
	glBegin(GL_QUADS);

	glVertex2d(minx,miny);
	glVertex2d(maxx,miny);
	glVertex2d(maxx,maxy);
	glVertex2d(minx,maxy);

	glEnd();
*/
   return;
}
