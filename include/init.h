#ifndef INIT_H
#define INIT_H

#include "movement.h"
#include "drawer.h"

/**
  * inicia a estrutura para realce de celulas
  */
void iniciaRealce(t_realce *realce);

/**
  * coloca as pecas em suas posicoes originais
  */
void iniciaTabuleiro(unsigned char *tabuleiro);

/**
  * coloca o jogo em seu estado inicial
  * se a estrutura passada for NULL, aloca uma nova
  * retorna o ponteiro para o jogo
  */
t_jogo *iniciaEstadoJogo(t_jogo *jogo);

#endif // INIT_H
