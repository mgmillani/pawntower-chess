#include <SDL/SDL.h>

#include "control.h"

/**
	* thread principal do jogo, a qual deve-se mandar os comandos de cada jogador
	*/
int mestreDeJogo(void *data);

/**
	* informa a thread do jogo de que um movimento deve ser feito
	* time corresponde ao time do jogador que fez o movimento
	*/
void executaMovimento(unsigned char time, unsigned char pecaOrigem, unsigned int posOrigem, unsigned int posDestino);
