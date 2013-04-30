#ifndef CONTROL_H
#define CONTROL_H

#include "movement.h"
#include "drawer.h"

typedef struct s_jogada
{
	unsigned char time;
	unsigned char pecaOrigem;
	unsigned int posOrigem;
	unsigned int posDestino;
}t_jogada;

typedef struct s_controle
{
	t_jogo *jogo;
	t_jogada *jogada;
	SDL_mutex *proximo;
	SDL_mutex *turnoP1;
	SDL_mutex *turnoP2;
	SDL_cond *fimTurno;
	SDL_cond *inicioJogo;
	int *estadoJogo;
}t_controle;

typedef struct s_controleHumano
{
	t_jogo *jogo;
	t_jogada *jogada;
	int *celulaX;  //qual celula foi ativada
	int *celulaY;
	char time; //time do jogador
	//variaveis para realce
	t_realce *realce;

	SDL_mutex *proximo;
	SDL_mutex *turno;
	SDL_cond *houveEntrada;
	SDL_cond *fimTurno;
	int *estadoJogo;	//o estado do jogo (ver funcao fimDeJogo)
}t_controleHumano;

/**
  * inicia o controle de um jogador humano
  */
void iniciaControleHumano(t_controleHumano *controleHumano,t_controle *controleMestre,char time,t_realce *realce, int *celulaX, int *celulaY, SDL_cond *houveEntrada);

/**
  * inicia o controle do jogo
  */
void iniciaControle(t_controle *controle,t_jogo *jogo);

/**
	* thread principal do jogo, a qual deve-se mandar os comandos de cada jogador
	*/
int mestreDeJogo(t_controle *controle);

/**
  * thread para um jogador humano
  */
int jogadorHumano(t_controleHumano *controle);

/**
  * verifica se o jogo acabou
  * retorna 0 caso nao tenha terminado
  * retorna o time do jogador que ganhou ou
  * P1 | P2 caso tenha ocorrido empate
  */
int fimDeJogo(t_jogo *jogo);

/**
	* executa a jogada. Deve ser chamada pela thread do mestreDeJogo
	* time corresponde ao time do jogador que fez o movimento
	*/
void executaJogada(t_jogada *jogada);

#endif // CONTROL_H
