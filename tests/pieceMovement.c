#include <stdio.h>

#include "movement.h"
#include "init.h"

#include "definitions.h"

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("usage: pieceMovement.test <pos>\n");
		return 1;
	}

	unsigned int pos;
	sscanf(argv[1],"%d",&pos);

	unsigned char board[8*8];
	iniciaTabuleiro(board);
	unsigned int movimentos[14];
	unsigned int capturas[14];
	unsigned int numMov;
	unsigned int numCapt;

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


	return 0;
}
