#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "drawer.h"
#include "movement.h"

#include "definitions.h"

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
		desenhaPeao(regiao,jogador->torrePos[i]/8,jogador->torrePos[i]%8,time);
}

/**
  * desenha um peao na linha e coluna dadas do tabuleiro (que esta na regiao passada)
  */
void desenhaPeao(t_rect *regiao,int linha, int coluna,char time)
{
	return;
}

/**
  * desenha um peao na linha e coluna dadas do tabuleiro (que esta na regiao passada)
  */
void desenhaTorre(t_rect *regiao,int linha, int coluna,char time)
{
	return;
}
