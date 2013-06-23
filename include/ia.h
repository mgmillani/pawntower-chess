#ifndef IA_H
#define IA_H

#include "control.h"
#include "movement.h"
#include "definitions.h"

typedef struct s_miniMax
{
	unsigned int profundidadeMaxima;
	unsigned int tempoMaximo;
}t_miniMax;

typedef struct s_miniMaxData
{
	char time;
	t_jogada *jogada;
	t_jogo *jogoOrig;
	double alfa;
	double beta;
	int cor;
	unsigned int profundidade;
	unsigned int tempoMax;
	char primeiro;
	unsigned int t0;
	double val;
}t_miniMaxData;

void iaRandom(t_jogador *jogador,char time,t_jogada *jogada,t_jogo *jogo,void *data);

void iaMinMax(t_jogador *jogador,char time,t_jogada *jogada,t_jogo *jogo,void *data);
void iaMinMaxB(t_jogador *jogador,char time,t_jogada *jogada,t_jogo *jogo,void *data);

double miniMax(char time,t_jogada *jogada,t_jogo *jogoOrig,double alfa, double beta, int cor, unsigned int profundidade,unsigned int tempoMax,char primeiro,unsigned int t0);

double miniMaxB(char time,t_jogada *jogada,t_jogo *jogoOrig,double alfa, double beta, int cor, unsigned int profundidade,unsigned int tempoMax,char primeiro,unsigned int t0);

double parallelMiniMax(char time,t_jogada *jogada,t_jogo *jogoOrig,double alfa, double beta, int cor, unsigned int profundidade,unsigned int tempoMax,char primeiro,unsigned int t0);

/**
  * thread para aplicar o minimax em paralelo
  */
int miniMaxThread(void *minmaxData);

double funcaoHeuristica(char time, t_jogo* jogo);

/**
  * determina quao bom eh o jogo para o jogador
  * retorna um valor em [-1,1]
  */
double funcaoHeuristica(char time, t_jogo*);

double funcaoHeuristicaJogador(char time, t_jogo* jogo);

#endif // IA_H

