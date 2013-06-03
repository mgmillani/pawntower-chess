#ifndef OLDIA_H
#define OLDIA_H

#include "control.h"
#include "movement.h"
#include "definitions.h"

void iaMinMaxOld(t_jogador *jogador,char time,t_jogada *jogada,t_jogo *jogo,void *data);

double miniMaxOld(char time,t_jogada *jogada,t_jogo *jogoOrig,double alfa, double beta, int cor, unsigned int profundidade,unsigned int tempoMax,char primeiro,unsigned int t0);

double funcaoHeuristicaOld(t_jogador*,char time, t_jogo* jogo);

#endif // OLDIA_H


