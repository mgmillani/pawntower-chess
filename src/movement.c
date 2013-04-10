#include <stdio.h>

#include "movement.h"
#include "definitions.h"

//funcao para percorrer uma linha/coluna buscando possiveis movimentos/capturas
void linhaScan(const unsigned char *tabuleiro,
              int min, int max,
              int deltaPos, unsigned int iDelta,
              unsigned int posAtual,
              unsigned int *movimentos,
              unsigned int *numMov,
              unsigned int *capturas,
              unsigned int *numCapt,
              unsigned char time)
{
	int i;

	for(i=min ; i<=max ; i+=iDelta,posAtual+=deltaPos)
	{

		if(VAZIO == tabuleiro[posAtual])
		{
			movimentos[*numMov] = posAtual;
			(*numMov)++;
		}
		//se for uma peca adversaria, adiciona o como ponto de captura
		else if((tabuleiro[posAtual] & MASCARATIME) != time)
		{
			capturas[*numCapt] = posAtual;
			(*numCapt)++;
		}
		//se for uma peca do proprio jogador, entao a torre nao pode se mover mais
		else
			return;
	}
}


/**
  * dada uma posicao no tabuleiro, calcula em quais posicoes a peca pode se mover
  * escreve essas posicoes no array movimentos, e o numero de posicoes em numMov
  * escreve as posicoes de captura em capturas, e o numero de capturas em numCapt
  * cada array deve ter, pelo menos, 14 posicoes
  */
void movimentosPossiveis(const unsigned char *tabuleiro,unsigned int pos, unsigned int *movimentos, unsigned int *numMov, unsigned int *capturas, unsigned int *numCapt)
{

	*numMov = 0;
	*numCapt = 0;
	unsigned int move;
	unsigned int diagDir;
	unsigned int diagEsq;
	//determina o time
	unsigned char time = tabuleiro[pos] & MASCARATIME;
	//determina o tipo de peca a ser analisado
	if((tabuleiro[pos] & MASCARAPECA) == PEAO)
	{
		if(time == P1)
		{
			move = pos-8;
			diagDir = pos-7;
			diagEsq = pos-9;
		}
		else
		{
			move = pos+8;
			diagDir = pos+9;
			diagEsq = pos+7;
		}
		//pode mover reto para cima, ou capturar na diagonal
		if(VAZIO == tabuleiro[move])
		{
			movimentos[0] = move;
			(*numMov)++;
		}
		//verifica em qual coluna a peca esta
		unsigned int col = pos%8;
		//caso nao seja a mais da esquerda, verifica a diagonal esquerda
		if(col!=0)
			if((tabuleiro[diagEsq] & MASCARATIME) == time)
			{
				capturas[0] = diagEsq;
				(*numCapt)++;
			}
		//se nao for a mais da direita, verifica a diagonal direita
		if(col<7)
			if((tabuleiro[diagDir] & MASCARATIME) == time)
			{
				capturas[*numCapt] = diagDir;
				(*numCapt)++;
			}
	}
	//caso seja uma torre
	else
	{

		//a torre pode se mover e capturar na mesma coluna e na mesma linha
		//verifica em qual coluna a peca esta
		unsigned int col = pos%8;
		unsigned int posAtual = pos;
		//determina para quais posicoes a direita da peca ela pode se mover
		linhaScan(tabuleiro,col,8,1,1,posAtual+1,movimentos,numMov,capturas,numCapt,time);
		//determina para quais posicoes a esquerda da peca ela pode se mover
		linhaScan(tabuleiro,0,col-1,-1,1,posAtual-1,movimentos,numMov,capturas,numCapt,time);
		//determina para quais posicoes acima da peca ela pode se mover
		unsigned int row = pos/8;
		linhaScan(tabuleiro,0,row-1,-8,1,posAtual-8,movimentos,numMov,capturas,numCapt,time);
		//determina para quais posicoes abaixo da peca ela pode se mover
		linhaScan(tabuleiro,row+1,8,8,1,posAtual+8,movimentos,numMov,capturas,numCapt,time);
	}
}

