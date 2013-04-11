#ifndef INIT_H
#define INIT_H

/**
  * coloca as pecas em suas posicoes originais
  */
void iniciaTabuleiro(unsigned char *tabuleiro);


/**
  * coloca o jogo em seu estado inicial
  * se a estrutura passada for NULL, aloca uma nova
  * retorna o ponteiro para o jogo
  */
t_jogo *iniciaEstadoJogo(t_jogo *jogo);



#endif // INIT_H

