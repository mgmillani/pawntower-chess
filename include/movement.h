#ifndef MOVEMENT_H
#define MOVEMENT_H

/**
  * dada uma posicao no tabuleiro, calcula em quais posicoes a peca pode se mover
  * escreve essas posicoes no array movimentos, e o numero de posicoes em numMov
  * escreve as posicoes de captura em capturas, e o numero de capturas em numCapt
  * cada array deve ter, pelo menos, 14 posicoes
  */
void movimentosPossiveis(const unsigned char *tabuleiro,unsigned int pos, unsigned int *movimentos, unsigned int *numMov, unsigned int *capturas, unsigned int *numCapt);

#endif // MOVEMENT_H

