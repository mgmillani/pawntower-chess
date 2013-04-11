#include <stdio.h>

#include "movement.h"
#include "init.h"

#include "definitions.h"

void movePiece(t_jogo *jogo, unsigned int src,unsigned int dst)
{

	unsigned char time = jogo->tabuleiro[src] & MASCARATIME;
	unsigned char pecaOrigem = jogo->tabuleiro[src];

	printf("SRC: %d\n",src);
	printf("POrigem: %d\n",(int)pecaOrigem);
	printf("Time: %d\n",(int)time);

	movePeca(jogo,time,pecaOrigem,src,dst);
	//void movePeca(t_jogo *jogo, unsigned char time,unsigned char pecaOrigem, unsigned int posOrigem, unsigned int posDestino);

}

void moves(unsigned char *board, unsigned int pos)
{
	unsigned int movimentos[14];
	unsigned int capturas[14];
	unsigned int numMov;
	unsigned int numCapt;
	unsigned int i;

	movimentosPossiveis(board,pos,movimentos,&numMov,capturas,&numCapt);

	printf("\nNumero de Movimentos: %d\n",numMov);
	printf("Numero de Capturas: %d\n",numCapt);
	printf("Movimentos:\n");
	for(i=0 ; i<numMov ; i++)
		printf("%d ",movimentos[i]);
	printf("\nCapturas:\n");
	for(i=0 ; i<numCapt ; i++)
		printf("%d ",capturas[i]);
	printf("\n");

}

int main(int argc, char *argv[])
{

	unsigned char *board;
	t_jogo jogo;
	iniciaEstadoJogo(&jogo);
	//iniciaTabuleiro(board);
	board = jogo.tabuleiro;

	if(argc != 2 && argc!=3)
	{
		printf("usage: pieceMovement.test <pos>\n");
		printf("or:\tpieceMovement.test <from> <to>\n");
		return 1;
	}

	//mostra o tabuleiro
	unsigned int i,j,k=0;
	for(i=0 ; i<8 ; i++)
	{
		for(j=0 ; j<8 ; j++,k++)
		{
			printf("%d(%02d) ",(int)board[k],k);
		}
		printf("\n");
	}

	//mostra o estado da estrutura t_jogo
	printf("\n#Jogador 1:\n");
	printf("Peoes:");
	for(i=0;i<jogo.p1.numPeoes;i++)
		printf(" %u",jogo.p1.peaoPos[i]);
	printf("\n");

	//mostrar os movimentos
	if(argc==2)
	{
		unsigned int pos;
		sscanf(argv[1],"%u",&pos);
		moves(board,pos);
	}
	else if(argc==3)
	{
		unsigned int src;
		sscanf(argv[1],"%u",&src);
		unsigned int dst;
		sscanf(argv[2],"%u",&dst);

		movePiece(&jogo,src,dst);

		printf("\nAfter movement:\n");
		k=0;
		for(i=0 ; i<8 ; i++)
		{
			for(j=0 ; j<8 ; j++,k++)
			{
				printf("%d(%02d) ",(int)board[k],k);
			}
			printf("\n");
		}

	}



	return 0;
}
