#include <stdio.h>

#include "movement.h"

#include "definitions.h"
#include "debug.h"

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
	//ERR("MIN:%d\tMAX:%d\tposAtual=%u\tdpos=%d\n",min,max,posAtual,deltaPos);

	for(i=min ; i<=max ; i+=iDelta,posAtual+=deltaPos)
	{

		if((VAZIO & tabuleiro[posAtual]) != 0)
		{
			movimentos[*numMov] = posAtual;
			(*numMov)++;
		}
		//se for uma peca adversaria, adiciona o como ponto de captura
		else if((tabuleiro[posAtual] & MASCARATIME) != time)
		{
			capturas[*numCapt] = posAtual;
			(*numCapt)++;
			return;
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
	unsigned int move2;
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
			move2 = pos-16;
			diagDir = pos-7;
			diagEsq = pos-9;
		}
		else
		{
			move = pos+8;
			move2 = pos+16;
			diagDir = pos+9;
			diagEsq = pos+7;
		}
		//pode mover reto para cima, ou capturar na diagonal
		if((VAZIO & tabuleiro[move]) != 0)
		{
			movimentos[0] = move;
			(*numMov)++;
			//na primeira jogada, pode mover duas casas
			if((tabuleiro[pos] & ENPASSANT)!=0)
			{
				if((tabuleiro[move2] & VAZIO) !=0)
				{
					movimentos[1] = move2;
					(*numMov)++;
				}
			}
		}

		//verifica em qual coluna a peca esta
		unsigned int col = pos%8;
		//caso nao seja a mais da esquerda, verifica a diagonal esquerda
		if(col!=0)
		{
			if((tabuleiro[diagEsq] & MASCARATIME) != time && ((tabuleiro[diagEsq] & VAZIO) == 0))
			{
				capturas[0] = diagEsq;
				(*numCapt)++;
			}
			//verifica a possibilidade de en passant
			if((tabuleiro[diagEsq] & MASCARATIME) != time && ((tabuleiro[diagEsq] & ENPASSANT) != 0))
			{
				capturas[*numCapt] = diagEsq;
				(*numCapt)++;
			}
		}
		//se nao for a mais da direita, verifica a diagonal direita
		if(col<7)
		{
			if((tabuleiro[diagDir] & MASCARATIME) != time && ((tabuleiro[diagDir] & VAZIO) == 0))
			{
				capturas[*numCapt] = diagDir;
				(*numCapt)++;
			}
			//verifica a possibilidade de en passant
			if((tabuleiro[diagDir] & MASCARATIME) != time && ((tabuleiro[diagDir] & ENPASSANT) != 0))
			{
				capturas[*numCapt] = diagDir;
				(*numCapt)++;
			}
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
		linhaScan(tabuleiro,col+1,7,1,1,posAtual+1,movimentos,numMov,capturas,numCapt,time);
		//determina para quais posicoes a esquerda da peca ela pode se mover
		linhaScan(tabuleiro,0,col-1,-1,1,posAtual-1,movimentos,numMov,capturas,numCapt,time);
		//determina para quais posicoes acima da peca ela pode se mover
		unsigned int row = pos/8;
		linhaScan(tabuleiro,0,row-1,-8,1,posAtual-8,movimentos,numMov,capturas,numCapt,time);
		//determina para quais posicoes abaixo da peca ela pode se mover
		linhaScan(tabuleiro,row+1,7,8,1,posAtual+8,movimentos,numMov,capturas,numCapt,time);
	}
}

/**
  * move a peca que esta em posOrigem no tabuleiro para posDestino
  * atualiza os dados relevantes da estrutura jogo
  * assume que o movimento eh valido
  */
void movePeca(t_jogo *jogo, unsigned char time,unsigned char pecaOrigem, unsigned int posOrigem, unsigned int posDestino)
{

	unsigned char *tabuleiro = jogo->tabuleiro;
	t_jogador *jogadorA; //jogador que moveu a peca
	t_jogador *jogadorB; //outro jogador (pode ter perdido uma peca)
	if(time == P1)
	{
		jogadorA = &(jogo->p1);
		jogadorB = &(jogo->p2);
	}
	else
	{
		jogadorA = &(jogo->p2);
		jogadorB = &(jogo->p1);
	}

	unsigned char *array;
	unsigned char len;
	unsigned int enpassant = 100;
	int tipo;
	//determina qual array deve ter a posicao da peca atualizada
	if((pecaOrigem & MASCARAPECA)==PEAO)
	{
		tipo = PEAO;
		array = jogadorA->peaoPos;
		len = jogadorA->numPeoes;
	}
	else
	{
		tipo = TORRE;
		array = jogadorA->torrePos;
		len = jogadorA->numTorres;
	}

	//busca a posicao da peca no array e atualiza-a
	//sabemos que ela esta no array
	unsigned int i;

	for(i=0 ; array[i]!=posOrigem ; i++)
		;
	array[i] = posDestino;

	//verifica se uma peca foi capturada
	if((tabuleiro[posDestino] & VAZIO) ==0)
	{
		if((tabuleiro[posDestino] & MASCARAPECA) == PEAO)
		{
			array = jogadorB->peaoPos;
			jogadorB->numPeoes--;
			len = jogadorB->numPeoes; //valor da ultima posicao valida do array antes da remocao
		}
		else
		{
			array = jogadorB->torrePos;
			jogadorB->numTorres--;
			len = jogadorB->numTorres;	//valor da ultima posicao valida do array antes da remocao
		}

		//remove a peca do array
		for(i=0 ; array[i]!=posDestino ; i++)
			;
		array[i] = array[len];
	}
	//verifica se eh um en passant
	else if(((tabuleiro[posDestino] & VAZIO) != 0) && ((tabuleiro[posDestino] & ENPASSANT) != 0) && tipo==PEAO)
	{
		//determina onde esta a peca capturada
		int dir;
		if(time == P1)
			dir = 8;
		else
			dir = -8;
		unsigned int posCaptura = posDestino + dir;

		//remove a peca do adversario
		array = jogadorB->peaoPos;
		jogadorB->numPeoes--;
		len = jogadorB->numPeoes; //valor da ultima posicao valida do array antes da remocao

		//remove a peca do array
		for(i=0 ; array[i]!=posCaptura ; i++)
			;
		array[i] = array[len];
		//remove a peca do tabuleiro
		tabuleiro[posCaptura] = VAZIO;

	}
	//verifica se eh um movimento duplo de um peao
	else if(tipo==PEAO && ((tabuleiro[posOrigem] & ENPASSANT) != 0))
	{
		int dir;
		if(time == P1)
			dir = -8;
		else
			dir = 8;
		//se for um movimento duplo
		if(posDestino != posOrigem+dir)
		{
			//marca como uma posicao possivel de receber en passant
			tabuleiro[posOrigem+dir] = ENPASSANT|VAZIO;
			enpassant = posOrigem+dir;
		}
	}

	//tira a peca de sua posicao
	tabuleiro[posOrigem] = VAZIO;
	//coloca a peca em sua devida posicao
	tabuleiro[posDestino] = pecaOrigem & ~ENPASSANT;
	//limpa o en passant anterior
	if(jogo->enpassant<64)
		tabuleiro[jogo->enpassant] &= ~ENPASSANT;
	jogo->enpassant = enpassant;
}

