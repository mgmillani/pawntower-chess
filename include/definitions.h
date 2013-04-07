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

typedef struct s_rect
{
	unsigned int x,y;
	unsigned int w,h;
}t_rect;



#endif // DEFINITIONS_H

