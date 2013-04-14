#ifndef CONTROL_H
#define CONTROL_H

#include "movement.h"

typedef struct s_jogada
{
	unsigned char time;
	unsigned char pecaOrigem;
	unsigned int posOrigem;
	unsigned int posDestino;
}t_jogada;

typedef struct s_controle
{
	t_jogo *jogo;
	t_jogada *jogada;
	SDL_mutex *efetuaJogada;
}t_controle;


/**
	* thread principal do jogo, a qual deve-se mandar os comandos de cada jogador
	*/
int mestreDeJogo(t_controle *controle);

/**
	* informa a thread do jogo de que um movimento deve ser feito
	* time corresponde ao time do jogador que fez o movimento
	*/
void executaMovimento(unsigned char time, unsigned char pecaOrigem, unsigned int posOrigem, unsigned int posDestino);

#endif // CONTROL_H
