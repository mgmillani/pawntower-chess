#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define MASCARATIME 6 //110
#define P1          0 //000
#define P2          2 //010

#define MASCARAPECA 5 //101
#define PEAO        0 //000
#define TORRE       1 //001

#define P1PEAO      0 //000
#define P1TORRE     1 //001
#define P2PEAO      2 //010
#define P2TORRE     3 //011
#define VAZIO       4 //100
#define ENPASSANT   8//1000

#define UNFINISHED -1
#define DRAW -2

//tipo do jogador
#define HUMANO 0
#define MAQUINA 1

//posicao dos botoes
#define HH 0
#define HM 1
#define MH 2
#define MM 3

typedef struct s_rect
{
	double x,y;
	double w,h;
}t_rect;



#endif // DEFINITIONS_H

