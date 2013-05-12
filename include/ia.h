#ifndef IA_H
#define IA_H

#include "control.h"
#include "movement.h"
#include "definitions.h"

typedef struct s_miniMax
{

	double alfa;
	double beta;
	char color;
	unsigned int depth;

}t_miniMax;

void iaRandom(t_jogador *jogador,char time,t_jogada *jogada,t_jogo *jogo,void *data);

void iaMinMax(t_jogador *jogador,char time,t_jogada *jogada,t_jogo *jogo,void *data);

double miniMax(char time,t_jogada *jogada,t_jogo *jogoOrig,double alfa, double beta, int cor, unsigned int profundidade,char primeiro,unsigned int t0);

/**
  * determina quao bom eh o jogo para o jogador
  * retorna um valor em [-1,1]
  */
double funcaoHeuristica(t_jogador*,char time, t_jogo*);

#endif // IA_H

