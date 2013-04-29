#include <SDL/SDL.h>

#include "definitions.h"
#include "control.h"

/**
  * inicia o controle do jogo
  */
void iniciaControle(t_controle *controle,t_jogo *jogo)
{
	controle->jogo = jogo;
	controle->jogada = malloc(sizeof(*(controle->jogada)));
	//cria os mutexes
	controle->proximo = SDL_CreateMutex();
	controle->turnoP1 = SDL_CreateMutex();
	controle->turnoP2 = SDL_CreateMutex();
	//cria os conds
	controle->fimTurno = SDL_CreateCond();
	controle->inicioJogo = SDL_CreateCond();
}

/**
	* thread principal do jogo, a qual deve-se mandar os comandos de cada jogador
	*/
int mestreDeJogo(t_controle *controle)
{
	int estadoJogo = 0;
	SDL_mutex *trocaJogador = SDL_CreateMutex();
	SDL_LockMutex(trocaJogador);
	//tranca ambos os jogadores ate o jogo comecar
	SDL_LockMutex(controle->turnoP1);
	SDL_LockMutex(controle->turnoP2);
	SDL_CondSignal(controle->inicioJogo);
	//espera ate o sinal de inicio da partida para liberar o P1
	SDL_CondWait(controle->inicioJogo,controle->turnoP1);
	while(estadoJogo==0)
	{
		//espera o primeiro terminar seu turno
		SDL_CondWait(controle->fimTurno,trocaJogador);
		SDL_LockMutex(controle->turnoP1);
		//executa a jogada de P1
		executaJogada(controle->jogada);
		//verifica se o jogo acabou
		estadoJogo = fimDeJogo(controle->jogo);
		if(estadoJogo!=0)
			break;
		//libera o segundo jogador
		SDL_UnlockMutex(controle->turnoP2);
		//espera o jogador terminar
		SDL_CondWait(controle->fimTurno,controle->turnoP1);
		//executa a jogada de P2
		executaJogada(controle->jogada);
		//verifica se o jogo acabou
		estadoJogo = fimDeJogo(controle->jogo);
		if(estadoJogo!=0)
			break;
		SDL_LockMutex(controle->turnoP2);
		//libera o primeiro novamente
		SDL_UnlockMutex(controle->turnoP1);
		//e espera ele terminar
		SDL_LockMutex(trocaJogador);
		SDL_CondWait(controle->fimTurno,trocaJogador);
	}

	SDL_DestroyMutex(trocaJogador);

	return estadoJogo;

}

/**
  * thread para um jogador humano
  */
int jogadorHumano(t_controleHumano *controle)
{

	//se o jogo acabou, sai da funcao
	while(controle->estadoJogo != 0)
	{
		int celulaAtiva = -1; //qual celula do tabuleira esta ativa. -1 indica nenhuma
		unsigned char peca;
		char fimTurno = 0;
		//inicia o turno
		SDL_LockMutex(controle->turno);
		while(!fimTurno)
		{
			//espera o usuario fazer algo
			SDL_LockMutex(controle->entrada);
			SDL_CondWait(controle->houveEntrada,controle->entrada);
			unsigned int pos = controle->celulaX + 8*controle->celulaY;
			//se alguma celula estiver ativa
			if(celulaAtiva>=0)
			{
				//se for uma celula para a qual podemos mover, move a peca para la
				unsigned int i;
				char move = 0;
				for(i=0 ; i<*(controle->numMov) &&!move ; i++)
					if(controle->movimentos[i]==pos)
						move = 1;
				for(i=0 ; i<*(controle->numCapt) &&!move ; i++)
					if(controle->capturas[i]==pos)
						move = 1;
				if(move)
				{
					controle->jogada->time = controle->time;
					controle->jogada->posOrigem = celulaAtiva;
					controle->jogada->posDestino = pos;
					controle->jogada->pecaOrigem = peca;
					//termina o turno
					fimTurno = 1;
					SDL_UnlockMutex(controle->turno);
					SDL_LockMutex(controle->proximo);
					SDL_CondSignal(controle->fimTurno);
					SDL_CondWait(controle->fimTurno,controle->proximo);
				}
			}
			//se nenhuma celula estiver ativa
			else
			{
				//se existe uma peca do jogador na posicao selecionada
				if((controle->jogo->tabuleiro[pos] & MASCARATIME) == controle->time)
				{
					//marca a peca como selecionada
					celulaAtiva = pos;
					peca = controle->jogo->tabuleiro[pos] & MASCARAPECA;
					//realca todas as celulas para as quais a peca pode se mover
					movimentosPossiveis(controle->jogo->tabuleiro,pos,controle->movimentos,controle->numMov,controle->capturas,controle->numCapt);
				}
			}
		}
	}

	return controle->estadoJogo;

}

/**
  * verifica se o jogo acabou
  * retorna 0 caso nao tenha terminado
  * retorna o time do jogador que ganhou ou
  * P1 | P2 caso tenha ocorrido empate
  */
int fimDeJogo(t_jogo *jogo)
{
	t_jogador *p1 = &(jogo->p1);
	t_jogador *p2 = &(jogo->p2);
	//verifica se um jogador nao possui peoes
	if(p1->numPeoes == 0)
		return P1;
	if(p2->numPeoes == 0)
		return P2;

	//verifica se algum jogador possui 1 peao em posicao de vitoria
	unsigned int i;
	for(i=0 ; i<p1->numPeoes ; i++)
	{
		//se o peao de P1 chegou na linha 0, ele ganhou
		if(p1->peaoPos[i]/8 == 0)
			return P1;
	}
	for(i=0 ; i<p2->numPeoes ; i++)
	{
		//se o peao de P2 chegou na linha 7, ele ganhou
		if(p2->peaoPos[i]/8 == 7)
			return P2;
	}

	//se alguem nao pode se mover, ocorreu um empate
	unsigned int mov[14];
	unsigned int accumMov = 0;
	unsigned int numMov = 0;
	unsigned int numCapt = 0;
	//peoes p1
	for(i=0 ; i<p1->numPeoes ; i++)
	{
		movimentosPossiveis(jogo->tabuleiro,p1->peaoPos[i],mov,&numMov,mov,&numCapt);
		accumMov += numMov + numCapt;
	}
	//torres p1
	for(i=0 ; i<p1->numTorres ; i++)
	{
		movimentosPossiveis(jogo->tabuleiro,p1->torrePos[i],mov,&numMov,mov,&numCapt);
		accumMov += numMov + numCapt;
	}
	if(accumMov == 0)
		return P1 | P2;

	//peoes p2
	for(i=0 ; i<p2->numPeoes ; i++)
	{
		movimentosPossiveis(jogo->tabuleiro,p2->peaoPos[i],mov,&numMov,mov,&numCapt);
		accumMov += numMov + numCapt;
	}
	//torres p2
	for(i=0 ; i<p2->numTorres ; i++)
	{
		movimentosPossiveis(jogo->tabuleiro,p2->torrePos[i],mov,&numMov,mov,&numCapt);
		accumMov += numMov + numCapt;
	}
	if(accumMov == 0)
		return P1 | P2;

	//caso nada disso ocorra, o jogo continua
	return 0;

}

/**
	* executa a jogada. Deve ser chamada pela thread do mestreDeJogo
	* time corresponde ao time do jogador que fez o movimento
	*/
void executaJogada(t_jogada *jogada)
{



}
