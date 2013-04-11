#include <stdio.h>

#include "movement.h"
#include "init.h"

#include "definitions.h"

void movePiece(unsigned char *board, unsigned int src,unsigned int dst)
{



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

	unsigned char board[8*8];
	iniciaTabuleiro(board);

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

		movePiece(board,src,dst);

	}

	return 0;
}
