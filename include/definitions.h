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

#define UNFINISHED -1
#define DRAW -2

typedef struct s_rect
{
	double x,y;
	double w,h;
}t_rect;



#endif // DEFINITIONS_H

