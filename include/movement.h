#ifndef MOVEMENT_H
#define MOVEMENT_H

typedef struct s_jogo
{

	unsigned char tabuleiro[64];
	t_jogador p1;
	t_jogador p2;

}t_jogo;

typedef struct s_jogador
{
	unsigned char numTorres;
	unsigned char numPeoes;

	unsigned char peaoPos[8];
	unsigned char torrePos[2];

}t_jogador;

/**
  * dada uma posicao no tabuleiro, calcula em quais posicoes a peca pode se mover
  * escreve essas posicoes no array movimentos, e o numero de posicoes em numMov
  * escreve as posicoes de captura em capturas, e o numero de capturas em numCapt
  * cada array deve ter, pelo menos, 14 posicoes
  */
void movimentosPossiveis(const unsigned char *tabuleiro,unsigned int pos, unsigned int *movimentos, unsigned int *numMov, unsigned int *capturas, unsigned int *numCapt);

/**
  * move a peca que esta em posOrigem no tabuleiro para posDestino
  * atualiza os dados relevantes da estrutura jogo
  * assume que o movimento eh valido
  */
void movePeca(t_jogo *jogo, unsigned int posOrigem, unsigned int posDestino);

#endif // MOVEMENT_H

