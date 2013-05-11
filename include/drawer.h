#ifndef DRAWER_H
#define DRAWER_H

#include "definitions.h"
#include "movement.h"

typedef struct s_realce
{
	unsigned int movimentos[14];
	unsigned int capturas[14];
	unsigned int numMov;
	unsigned int numCapt;
}t_realce;

/**
  * desenha os botoes do menu
  */
void desenhaMenuPrincipal(t_rect *botoes, int numBotoes);

/**
  * desenha as pecas e o tabuleiro na regiao dada da tela
  */
void desenhaJogo(t_rect *regiao, t_jogo *jogo);

/**
  * desenha o tabuleiro na regiao dada
  */
void desenhaTabuleiro(t_rect *regiao);

/**
  * desenha as pecas do jogador na regiao dada
  */
void desenhaPecas(t_rect *regiao,t_jogador *jogador,char time);

/**
  * desenha um peao na linha e coluna dadas do tabuleiro (que esta na regiao passada)
  */
void desenhaPeao(t_rect *regiao,int linha, int coluna,char time);

/**
  * desenha um peao na linha e coluna dadas do tabuleiro (que esta na regiao passada)
  */
void desenhaTorre(t_rect *regiao,int linha, int coluna,char time);

#endif // DRAWER_H
