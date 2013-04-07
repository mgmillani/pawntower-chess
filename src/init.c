#include "init.h"

#include "definitions.h"

/**
  * coloca as pecas em suas posicoes originais
  */
void iniciaTabuleiro(unsigned char *tabuleiro)
{
	unsigned int pos;
	unsigned int i;
	//primerio, preenche tudo com vazio
	for(pos = 0; pos < 64 ; pos++)
		tabuleiro[pos] = VAZIO;

	//preenche as linhas dos peoes
	for(pos=8,i=0 ; i< 8; i++,pos++)
	{
		tabuleiro[pos] = P2PEAO;
		tabuleiro[pos+5*8] = P1PEAO;
	}
	//coloca as torres
	tabuleiro[0] = P2TORRE;
	tabuleiro[7] = P2TORRE;
	tabuleiro[8*7] = P1TORRE;
	tabuleiro[7+8*7] = P1TORRE;
}