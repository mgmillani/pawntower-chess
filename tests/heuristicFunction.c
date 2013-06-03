#include <stdio.h>

#include "ia.h"
#include "control.h"

#include "definitions.h"

void testFunction(t_jogo *jogo)
{
	printf("Tabuleiro:\n");
	printTabuleiro(jogo->tabuleiro);
	printf("----------\n");

	double val = funcaoHeuristica(P1,jogo);
	printf("Utilidade para P1: %lf\n",val);
	val = funcaoHeuristica(P2,jogo);
	printf("Utilidade para P2: %lf\n",val);
}

void test0()
{
	t_jogo jogo;
	int i;
	for(i=0 ; i<64 ; i++)
		jogo.tabuleiro[i] = VAZIO;

	//um peao moveu uma casa, outro moveu duas
	jogo.tabuleiro[16] = P2PEAO;
	jogo.tabuleiro[63-24] = P1PEAO;

	t_jogador p1;
	p1.numTorres = 0;
	p1.numPeoes = 1;
	p1.peaoPos[0] = 63-21;
	t_jogador p2;
	p2.numTorres = 0;
	p2.numPeoes = 1;
	p2.peaoPos[0] = 14;
	jogo.p1 = p1;
	jogo.p2 = p2;

	testFunction(&jogo);
}

int main()
{
	int i;

	printf("\nTESTE 0\n\n");
	test0();
	printf("\n##################################\n\n");

	return 0;
}
