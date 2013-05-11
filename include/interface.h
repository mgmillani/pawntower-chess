#ifndef INTERFACE_H
#define INTERFACE_H

#include "definitions.h"

/**
  * trata o menu principal onde o usuario decide quem joga contra quem
  * escreve em tipoP1 e tipoP2 se o jogador sera HUMANO ou MAQUINA
  */
void menuPrincipal(int *tipoP1,int *tipoP2,int screenWidth, int screenHeight);

/**
  * verifica se o usuario clicou em um dos botoes
  */
int trataClique(t_rect *botoes, int numBotoes, double x, double y);

/**
  * verifica se o usuario passou o mouse por cima de um dos botoes
  */
int trataMovimento(t_rect *botoes, int numBotoes, double x, double y);

#endif // INTERFACE_H

