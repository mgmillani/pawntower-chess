#ifndef CONTROL_H
#define CONTROL_H

#include <SDL/SDL_thread.h>

#include "movement.h"
#include "drawer.h"

typedef enum {Poder,Jogar,TotalMutex} e_mutex;

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
	SDL_sem *comecoP1;
	SDL_sem *comecoP2;
	SDL_mutex *turnoP1[TotalMutex];
	SDL_mutex *turnoP2[TotalMutex];
	SDL_mutex *inicioJogo;
	SDL_mutex *iniciaPartida;
	int estadoJogo;
}t_controle;

typedef struct s_controleJogador
{

	t_jogo *jogo;
	t_jogada *jogada;

	char time; //time do jogador

	SDL_sem *turnoComeco;
	SDL_sem *pronto;
	SDL_mutex *turno[TotalMutex];
	int *estadoJogo;	//o estado do jogo (ver funcao fimDeJogo)

}t_controleJogador;

typedef struct s_controleHumano
{
	//variaveis para realce
	t_realce *realce;
	SDL_sem *entradaRecebida;
	SDL_mutex *escreveEntrada;
	int *celulaX;
	int *celulaY;
}t_controleHumano;

typedef struct s_jogadorData
{
	t_controleJogador *controle;
	void (*joga)(t_jogador*,char,t_jogada*,t_jogo*,void*);
	void *data;

}t_jogadorData;


void iniciaControleJogador(t_controleJogador *controleJogador, t_controle *controleMestre, char time);

/**
  * inicia o controle de um jogador humano
  */
void iniciaControleHumano(t_controleHumano *controleHumano,t_realce *realce, int *celulaX, int *celulaY, SDL_sem *entradaRecebida,SDL_mutex *escreveEntrada);

/**
  * inicia o controle do jogo
  */
void iniciaControle(t_controle *controle,t_jogo *jogo,SDL_mutex *jogoPronto,SDL_mutex *iniciaPartida);

/**
	* thread principal do jogo, a qual deve-se mandar os comandos de cada jogador
	*/
int mestreDeJogo(t_controle *controle);

/**
  * algoritmo para uma jogada aleatoria
  */
void iaRandom(t_jogador* jogador,char time,t_jogada *jogada,t_jogo *jogo,void *data);

/**
  * wrapper para a criacao da thread
  */
int threadJogador(t_jogadorData *data);

/**
  * thread para um jogador qualquer que usa a funcao joga para determinar sua jogada
  */
int jogador(t_controleJogador *controle,void (*)(t_jogador*,char,t_jogada*,t_jogo*,void*) ,void *data);

/**
  * thread para uma ia randomica
  */
//int jogadorIaRandom(t_controleIA *controle);

/**
  * thread para um jogador humano
  */
void jogaHumano(t_jogador* jogador, char time, t_jogada *jogada, t_jogo *jogo,t_controleHumano *controle);

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
void executaJogada(t_jogo *jogo,t_jogada *jogada);

void printTabuleiro(unsigned char *tabuleiro);

#endif // CONTROL_H
