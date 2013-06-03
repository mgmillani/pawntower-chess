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

}

int main()
{
	int i;

	return 0;
}
