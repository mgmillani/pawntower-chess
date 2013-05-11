#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

#include "definitions.h"
#include "control.h"
#include "drawer.h"
#include "movement.h"

#include "random.h"

#include "debug.h"

void iniciaControleJogador(t_controleJogador *controleJogador, t_controle *controleMestre, char time)
{

	controleJogador->jogo = controleMestre->jogo;
	controleJogador->jogada = controleMestre->jogada;
	controleJogador->time = time;

	SDL_mutex **turno;

	if(time == P1)
	{
		turno = controleMestre->turnoP1;
		controleJogador->turnoComeco = controleMestre->comecoP1;
	}
	else
	{
		turno = controleMestre->turnoP2;
		controleJogador->turnoComeco = controleMestre->comecoP2;
	}

	int i;
	for(i=0 ; i<TotalMutex ; i++)
		controleJogador->turno[i] = turno[i];

	controleJogador->estadoJogo = &(controleMestre->estadoJogo);

	controleJogador->pronto = SDL_CreateSemaphore(1);

}

/**
  * inicia o controle de um jogador humano
  */
void iniciaControleHumano(t_controleHumano *controleHumano,t_realce *realce, int *celulaX, int *celulaY, SDL_sem *entradaRecebida,SDL_mutex *escreveEntrada)
{

	controleHumano->celulaX = celulaX;
	controleHumano->celulaY = celulaY;

	controleHumano->entradaRecebida = entradaRecebida;
	controleHumano->escreveEntrada = escreveEntrada;

	controleHumano->realce = realce;

}

/**
  * inicia o controle do jogo
  */
void iniciaControle(t_controle *controle,t_jogo *jogo,SDL_mutex *jogoPronto,SDL_mutex *iniciaPartida)
{
	controle->jogo = jogo;
	controle->jogada = malloc(sizeof(*(controle->jogada)));
	//cria os mutexes
	int i;
	for(i=0 ; i< TotalMutex ; i++)
	{
		controle->turnoP1[i] = SDL_CreateMutex();
		controle->turnoP2[i] = SDL_CreateMutex();
	}

	controle->comecoP1 = SDL_CreateSemaphore(1);
	controle->comecoP2 = SDL_CreateSemaphore(1);

	controle->inicioJogo = jogoPronto;
	controle->iniciaPartida = iniciaPartida;

	controle->estadoJogo = UNFINISHED;

}

/**
	* thread principal do jogo, a qual deve-se mandar os comandos de cada jogador
	*/
int mestreDeJogo(t_controle *controle)
{
	int estadoJogo = UNFINISHED;
	//tranca ambos os jogadores ate o jogo comecar
	ERR("Mestre trancou :%p\n",controle->turnoP1[Poder]);
	SDL_LockMutex(controle->turnoP1[Poder]);
	ERR("Mestre trancou :%p\n",controle->turnoP2[Poder]);
	SDL_LockMutex(controle->turnoP2[Poder]);
	SDL_UnlockMutex(controle->inicioJogo);
	//espera ate o sinal de inicio da partida para liberar o P1
	SDL_LockMutex(controle->iniciaPartida);

	do
	{
		//libera o primeiro jogador
		//ERR("Mestre libera p1\n");
		SDL_UnlockMutex(controle->turnoP1[Poder]);
		//espera o primeiro terminar seu turno
		//ERR("Mestre espera p1\n");
		SDL_LockMutex(controle->turnoP1[Jogar]);
		SDL_UnlockMutex(controle->turnoP1[Jogar]);
		//informa que recebeu a mensagem de P1
		//ERR("Mestre confirma p1\n");
		SDL_SemPost(controle->comecoP1);
		//Impede-o de comecar outro turno
		//ERR("Mestre tranca p1\n");
		SDL_LockMutex(controle->turnoP1[Poder]);
		//executa a jogada de P1
		executaJogada(controle->jogo,controle->jogada);
		//verifica se o jogo acabou
		estadoJogo = fimDeJogo(controle->jogo,P2);
		if(estadoJogo!=UNFINISHED)
			break;
		//termina o turno do primeiro
		SDL_SemPost(controle->comecoP1);
		//libera o segundo jogador
		//ERR("Mestre libera p2\n");
		SDL_UnlockMutex(controle->turnoP2[Poder]);
		//espera o jogador terminar
		//ERR("Mestre espera p2\n");
		SDL_LockMutex(controle->turnoP2[Jogar]);
		SDL_UnlockMutex(controle->turnoP2[Jogar]);
		//informa que recebeu a mensagem
		//ERR("Mestre informa p2\n");
		SDL_SemPost(controle->comecoP2);
		//Impede-o de comecar outro turno
		//ERR("Mestre tranca p2\n");
		SDL_LockMutex(controle->turnoP2[Poder]);
		//executa a jogada de P2
		executaJogada(controle->jogo,controle->jogada);
		//verifica se o jogo acabou
		estadoJogo = fimDeJogo(controle->jogo,P1);
		if(estadoJogo!=UNFINISHED)
			break;
		//termina o turno do segundo
		SDL_SemPost(controle->comecoP2);
	}while(estadoJogo==UNFINISHED);

	ERR("GAME OVER!!!: %d\n",estadoJogo);
	controle->estadoJogo = estadoJogo;
	return estadoJogo;

}

/**
  * thread para um jogador qualquer que usa a funcao joga para determinar sua jogada
  */
int jogador(t_controleJogador *controle,void (*joga)(t_jogador*,char,t_jogada*,t_jogo*,void*) ,void *data)
{
	int time = controle->time;
	t_jogador *jogador;
	if(time == P1)
		jogador = &(controle->jogo->p1);
	else
		jogador = &(controle->jogo->p2);
	t_jogada *jogada = controle->jogada;

	int i = 1;

	while(*(controle->estadoJogo) == UNFINISHED)
	{
		//inicia o turno
		ERR("%d quer\n",(int)time);
		SDL_SemWait(controle->turnoComeco);
		ERR("%d jogar\n",(int)time);
		SDL_LockMutex(controle->turno[Jogar]);
		if(i>0)
		{
			SDL_SemPost(controle->pronto);
			i--;
		}
		ERR("%d poder\n",(int)time);
		SDL_LockMutex(controle->turno[Poder]);
		ERR("%d esta jogando\n",(int)time);

		jogada->time = time;
		joga(jogador,time,jogada,controle->jogo,data);

		//termina o turno
		//ERR("%d libera jogar\n",(int)time);
		SDL_UnlockMutex(controle->turno[Jogar]);
		//ERR("%d espera mestre\n",(int)time);
		SDL_SemWait(controle->turnoComeco);
		//ERR("%d tranca jogar\n",(int)time);
		SDL_LockMutex(controle->turno[Jogar]);
		//ERR("%d libera poder\n",(int)time);
		SDL_UnlockMutex(controle->turno[Poder]);
		//ERR("%d post comeco\n",(int)time);
		SDL_UnlockMutex(controle->turno[Jogar]);
		//ERR("%d terminou\n",(int)time);
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
int threadJogador(t_jogadorData *data)
{
	return jogador(data->controle,data->joga,data->data);
}

void jogaHumano(t_jogador* jogador, char time, t_jogada *jogada, t_jogo *jogo,t_controleHumano *controle)
{
	t_realce *realce = controle->realce;

	int celulaAtiva = -1; //qual celula do tabuleiro esta ativa. -1 indica nenhuma
	unsigned char peca;
	char fimTurno = 0;
	//inicia o turno
	while(!fimTurno)
	{
		ERR("Ativa:%d\n",celulaAtiva);
		//espera o usuario fazer algo
		SDL_SemWait(controle->entradaRecebida);
		SDL_LockMutex(controle->escreveEntrada);
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
				jogada->time = time;
				jogada->posOrigem = celulaAtiva;
				jogada->posDestino = pos;
				jogada->pecaOrigem = peca;
				//termina o turno
				fimTurno = 1;
			}
			//se nao for, desmarca a peca
			else
			{
				celulaAtiva = -1;
				ERR("Marcado: %d\n",jogo->tabuleiro[pos]);
			}
		}
		//se nenhuma celula estiver ativa
		else
		{
			//se existe uma peca do jogador na posicao selecionada
			if((jogo->tabuleiro[pos] & MASCARATIME) == time)
			{
				//marca a peca como selecionada
				celulaAtiva = pos;
				peca = jogo->tabuleiro[pos];
				//realca todas as celulas para as quais a peca pode se mover
				movimentosPossiveis(jogo->tabuleiro,pos,realce->movimentos,&realce->numMov,realce->capturas,&realce->numCapt);

			}
		}
		SDL_UnlockMutex(controle->escreveEntrada);
	}
}

/**
  * verifica se o jogo acabou
  * timeAtual eh o time que pode fazer a jogada (usado para determinar empates)
  * retorna UNFINISHED caso nao tenha terminado
  * retorna o time do jogador que ganhou ou
  * DRAW caso tenha ocorrido empate
  */
int fimDeJogo(t_jogo *jogo,char timeAtual)
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

	//se o time atual nao pode se mover, ocorreu um empate
	t_jogador *jogador;
	if(timeAtual == P1)
		jogador = p1;
	else
		jogador = p2;
	unsigned int mov[14];
	unsigned int accumMov = 0;
	unsigned int numMov = 0;
	unsigned int numCapt = 0;
	//peoes
	for(i=0 ; i<jogador->numPeoes && accumMov==0; i++)
	{
		movimentosPossiveis(jogo->tabuleiro,jogador->peaoPos[i],mov,&numMov,mov,&numCapt);
		accumMov += numMov + numCapt;
	}
	//torres
	for(i=0 ; i<jogador->numTorres && accumMov==0; i++)
	{
		movimentosPossiveis(jogo->tabuleiro,jogador->torrePos[i],mov,&numMov,mov,&numCapt);
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


