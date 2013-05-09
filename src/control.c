#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

#include "definitions.h"
#include "control.h"
#include "drawer.h"
#include "movement.h"

#include "random.h"

#include "debug.h"

void iniciaControleIA(t_controleIA *controleIA, t_controle *controleMestre, char time)
{

	controleIA->jogo = controleMestre->jogo;
	controleIA->jogada = controleMestre->jogada;
	controleIA->time = time;
	controleIA->proximo = controleMestre->proximo;

	if(time == P1)
		controleIA->turno = controleMestre->turnoP1;
	else
		controleIA->turno = controleMestre->turnoP2;

	controleIA->fimTurno = controleMestre->fimTurno;
	controleIA->fimTurno2 = controleMestre->fimTurno2;
	controleIA->estadoJogo = &(controleMestre->estadoJogo);

}

/**
  * inicia o controle de um jogador humano
  */
void iniciaControleHumano(t_controleHumano *controleHumano,t_controle *controleMestre,char time,t_realce *realce, int *celulaX, int *celulaY, SDL_cond *houveEntrada)
{

	controleHumano->jogo = controleMestre->jogo;
	controleHumano->jogada = controleMestre->jogada;
	controleHumano->proximo = controleMestre->proximo;
	controleHumano->time = time;
	if(time == P1)
		controleHumano->turno = controleMestre->turnoP1;
	else
		controleHumano->turno = controleMestre->turnoP2;

	controleHumano->celulaX = celulaX;
	controleHumano->celulaY = celulaY;

	controleHumano->houveEntrada = houveEntrada;

	controleHumano->estadoJogo = &(controleMestre->estadoJogo);
	controleHumano->fimTurno = controleMestre->fimTurno;
	controleHumano->fimTurno2 = controleMestre->fimTurno2;

	controleHumano->realce = realce;

}

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
	controle->fimTurno2 = SDL_CreateCond();
	controle->estadoJogo = UNFINISHED;
	controle->inicioJogo = SDL_CreateCond();
}

/**
	* thread principal do jogo, a qual deve-se mandar os comandos de cada jogador
	*/
int mestreDeJogo(t_controle *controle)
{
	int estadoJogo = UNFINISHED;
	SDL_mutex *trocaJogador = SDL_CreateMutex();
	SDL_LockMutex(trocaJogador);
	//tranca ambos os jogadores ate o jogo comecar
	SDL_LockMutex(controle->turnoP1);
	SDL_LockMutex(controle->turnoP2);
	SDL_CondBroadcast(controle->inicioJogo);
	//espera ate o sinal de inicio da partida para liberar o P1
	SDL_CondWait(controle->inicioJogo,controle->turnoP1);
	ERR("Mestre: received inicioJogo\n");
	SDL_UnlockMutex(controle->turnoP1);
	ERR("Mestre: unlocked P1\n");
	while(estadoJogo==UNFINISHED)
	{
		//espera o primeiro terminar seu turno
		SDL_CondWait(controle->fimTurno,trocaJogador);SDL_UnlockMutex(trocaJogador);
		ERR("Mestre: received fimTurno\n");
		ERR("Mestre: unlocked trocaJogador\n");
		SDL_LockMutex(controle->turnoP1);
		//executa a jogada de P1
		executaJogada(controle->jogo,controle->jogada);
		//verifica se o jogo acabou
		estadoJogo = fimDeJogo(controle->jogo);
		if(estadoJogo!=UNFINISHED)
			break;
		//libera o segundo jogador
		SDL_UnlockMutex(controle->turnoP2);
		ERR("Mestre: unlocked P2\n");
		//espera o jogador terminar
		SDL_LockMutex(controle->turnoP1);
		ERR("Mestre: locked P1\n");
		SDL_CondWait(controle->fimTurno,controle->turnoP1);
		ERR("Mestre: received fimTurno\n");
		//executa a jogada de P2
		SDL_UnlockMutex(controle->turnoP1);
		executaJogada(controle->jogo,controle->jogada);
		//verifica se o jogo acabou
		estadoJogo = fimDeJogo(controle->jogo);
		if(estadoJogo!=UNFINISHED)
			break;
		SDL_LockMutex(controle->turnoP2);
		//libera o primeiro novamente
		SDL_UnlockMutex(controle->turnoP1);

		//e espera ele terminar
		SDL_LockMutex(trocaJogador);

	}

	SDL_DestroyMutex(trocaJogador);
	ERR("GAME OVER!!!: %d\n",estadoJogo);
	controle->estadoJogo = estadoJogo;
	return estadoJogo;

}

int jogadorIa(t_controleIA *controle, void (*joga)(t_jogador*,char,t_jogada*,t_jogo*,void*),void *data)
{
	int time = controle->time;
	t_jogador *jogador;
	if(time == P1)
		jogador = &(controle->jogo->p1);
	else
		jogador = &(controle->jogo->p2);
	t_jogada *jogada = controle->jogada;

	while(*(controle->estadoJogo) == UNFINISHED)
	{
		//inicia o turno
		SDL_LockMutex(controle->turno);

		jogada->time = time;
		joga(jogador,time,jogada,controle->jogo,data);

		//termina o turno
		int itime = time;
		SDL_UnlockMutex(controle->turno);
		ERR("Ia %d unlocked turno\n",itime);
		SDL_LockMutex(controle->proximo);
		ERR("Ia %d locked proximo\n",itime);
		SDL_CondBroadcast(controle->fimTurno);
		ERR("Ia %d signaled fimTurno\n",itime);
		SDL_CondBroadcast(controle->fimTurno2);
		ERR("Ia %d signaled fimTurno2\n",itime);
		SDL_CondWait(controle->fimTurno2,controle->proximo);
		ERR("Ia %d received fimTurno2\n",itime);
		SDL_UnlockMutex(controle->proximo);
		ERR("Ia %d unlocked proximo\n",itime);
	}

	return *(controle->estadoJogo);
}

void iaRandom(t_jogador *jogador,char time,t_jogada *jogada,t_jogo *jogo,void *data)
{
	unsigned int movimentos[14];
	unsigned int numMov = 0;
	unsigned int capturas[14];
	unsigned int numCapt = 0;

	while(numMov == 0 && numCapt == 0)
	{
		//escolhe uma peca
		int peca = randrange(jogador->numTorres + jogador->numPeoes-1);
		//se for menor que numPeoes, eh um peao
		if(peca < jogador->numPeoes)
		{
			jogada->pecaOrigem = time | PEAO;
			jogada->posOrigem = jogador->peaoPos[peca];
		}
		//caso contrario, eh uma torre
		else
		{
			jogada->pecaOrigem = time | TORRE;
			jogada->posOrigem = jogador->torrePos[peca-jogador->numPeoes];
		}

		movimentosPossiveis(jogo->tabuleiro,jogada->posOrigem,movimentos,&numMov,capturas,&numCapt);
	}

	//se puder capturar, captura
	if(numCapt>0)
	{
		int pos = randrange(numCapt-1);
		jogada->posDestino = capturas[pos];
	}
	else
	{
		int pos = randrange(numMov-1);
		jogada->posDestino = movimentos[pos];
	}
}

/**
  * wrapper para a criacao da thread
  */
int threadIa(t_iaData *data)
{
	return jogadorIa(data->controle,data->joga,data->data);
}

/**
  * thread para um jogador humano
  */
int jogadorHumano(t_controleHumano *controle)
{
	ERR("Jogador criado\n");
	int time = controle->time;
	SDL_mutex *entrada = SDL_CreateMutex();
	t_realce *realce = controle->realce;
	//se o jogo acabou, sai da funcao
	while(*(controle->estadoJogo) == UNFINISHED)
	{
		int celulaAtiva = -1; //qual celula do tabuleiro esta ativa. -1 indica nenhuma
		unsigned char peca;
		char fimTurno = 0;
		//inicia o turno
		/*ERR("Try Lock turno %d\n",(int)controle->time);*/
		SDL_LockMutex(controle->turno);
		/*ERR("Lock turno %d\n",(int)controle->time);*/
		while(!fimTurno)
		{
			ERR("Jogador: %d\n",(int)controle->time);
			ERR("Ativa:%d\n",celulaAtiva);
			//espera o usuario fazer algo
			SDL_LockMutex(entrada);
			SDL_CondWait(controle->houveEntrada,entrada);
			SDL_UnlockMutex(entrada);
			unsigned int pos = *controle->celulaX + 8*(*controle->celulaY);
			//se alguma celula estiver ativa
			if(celulaAtiva>=0)
			{
				//se for uma celula para a qual podemos mover, move a peca para la
				unsigned int i;
				char move = 0;
				for(i=0 ; i<realce->numMov &&!move ; i++)
					if(realce->movimentos[i]==pos)
						move = 1;
				for(i=0 ; i<realce->numCapt &&!move ; i++)
					if(realce->capturas[i]==pos)
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
					SDL_CondBroadcast(controle->fimTurno);
					SDL_CondBroadcast(controle->fimTurno2);
					SDL_CondWait(controle->fimTurno2,controle->proximo);
					SDL_UnlockMutex(controle->proximo);
				}
				//se nao for, desmarca a peca
				else
				{
					celulaAtiva = -1;
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
					peca = controle->jogo->tabuleiro[pos];
					//realca todas as celulas para as quais a peca pode se mover
					movimentosPossiveis(controle->jogo->tabuleiro,pos,realce->movimentos,&realce->numMov,realce->capturas,&realce->numCapt);

				}
			}
		}
	}

	SDL_DestroyMutex(entrada);
	return *(controle->estadoJogo);

}

/**
  * verifica se o jogo acabou
  * retorna UNFINISHED caso nao tenha terminado
  * retorna o time do jogador que ganhou ou
  * DRAW caso tenha ocorrido empate
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
		{
			return P1;
		}
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
		return DRAW;

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
		return DRAW;

	//caso nada disso ocorra, o jogo continua
	return UNFINISHED;

}

/**
	* executa a jogada. Deve ser chamada pela thread do mestreDeJogo
	* time corresponde ao time do jogador que fez o movimento
	*/
void executaJogada(t_jogo *jogo,t_jogada *jogada)
{

	movePeca(jogo,jogada->time,jogada->pecaOrigem,jogada->posOrigem,jogada->posDestino);

}

void printTabuleiro(unsigned char *tabuleiro)
{
	int i;
	int k;
	for(i=k=0 ; i<8 ; i++)
	{
		int j;
		for(j=0 ; j<8 ; j++,k++)
		{
			ERR("%d ",(int)tabuleiro[k]);
		}
		ERR("\n");
	}

	ERR("\n");
}


