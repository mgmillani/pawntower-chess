#include <stdlib.h>
/**
  * gera um numero aleatorio em [0,max]
  */
unsigned int randrange(unsigned int max)
{
    return ((max+1) * ((double) rand())/((unsigned int)RAND_MAX+1));
}
