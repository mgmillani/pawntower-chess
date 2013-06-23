#include <math.h>

#include <SDL/SDL.h>

#include "movement.h"
#include "control.h"
#include "random.h"
#include "ia.h"

#include "debug.h"

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

void iaMinMax(t_jogador *jogador,char time,t_jogada *jogada,t_jogo *jogo,void *data)
{
	unsigned int t0 = SDL_GetTicks();
	unsigned int i;
	jogada->time = time;
	t_miniMax *parametros = data;
	unsigned int profundidade = parametros->profundidadeMaxima;
	unsigned int tempo = parametros->tempoMaximo;

	//double melhorValor = -5;
	t_jogada melhorJogada;

	for(i=1 ; i<profundidade && (SDL_GetTicks() - t0 < tempo); i++)
	{
		ERR("Profundidade: %d\n",i);
		double val = fabs(miniMax(time,jogada,jogo,-2,2,1,i,tempo,1,t0));
		if(val < 2)
		{
			//melhorValor = val;
			memcpy(&melhorJogada,jogada,sizeof(melhorJogada));
		}
		ERR("Valor :%lf\n",val);
	}

	memcpy(jogada,&melhorJogada,sizeof(melhorJogada));
	ERR("O:%u\tD:%u\n",jogada->posOrigem,jogada->posDestino);
}

void iaMinMaxB(t_jogador *jogador,char time,t_jogada *jogada,t_jogo *jogo,void *data)
{
	unsigned int t0 = SDL_GetTicks();
	unsigned int i;
	jogada->time = time;
	t_miniMax *parametros = data;
	unsigned int profundidade = parametros->profundidadeMaxima;
	unsigned int tempo = parametros->tempoMaximo;

	//double melhorValor = -5;
	t_jogada melhorJogada;

	for(i=1 ; i<profundidade && (SDL_GetTicks() - t0 < tempo); i++)
	{
		ERR("Profundidade: %d\n",i);
		double val = fabs(parallelMiniMax(time,jogada,jogo,-2,2,1,i,tempo,1,t0));
		if(val < 2)
		{
			//melhorValor = val;
			memcpy(&melhorJogada,jogada,sizeof(melhorJogada));
		}
		ERR("Valor :%lf\n",val);
	}

	memcpy(jogada,&melhorJogada,sizeof(melhorJogada));
	ERR("O:%u\tD:%u\n",jogada->posOrigem,jogada->posDestino);
}

double miniMax(char time,t_jogada *jogada,t_jogo *jogoOrig,double alfa, double beta, int cor, unsigned int profundidade,unsigned int tempoMax,char primeiro,unsigned int t0)
{
	t_jogador *jogador;
	int filhos = 0;
	if(profundidade == 0)
	{
		return funcaoHeuristica(time,jogoOrig);
	}
	else
	{
		if(SDL_GetTicks() - t0 > tempoMax)
		{
			return -2;
		}
		char outroTime = (time==P1) ? P2 : P1;
		t_jogo jogo;
		memcpy(&jogo,jogoOrig,sizeof(*jogoOrig));
		t_jogada melhorJogada;

		if(time == P1)
			jogador = &jogo.p1;
		else
			jogador = &jogo.p2;

		unsigned int movimentos[14];
		unsigned int numMov = 0;
		unsigned int capturas[14];
		unsigned int numCapt = 0;
		//para cada peca do jogador, calcula seus movimentos possiveis
		int i;

		unsigned char *pecaPos[2] = {jogador->peaoPos,jogador->torrePos};
		unsigned char numPeca[2] = {jogador->numPeoes,jogador->numTorres};

		int t;
		t_jogo outroJogo;
		for(t=0 ; t<2 ; t++)
		{
			for(i=0 ; i<numPeca[t] ; i++ )
			{
				filhos++;
				//faz uma copia do jogo original
				memcpy(&jogo,jogoOrig,sizeof(*jogoOrig));
				memcpy(&outroJogo,jogoOrig,sizeof(*jogoOrig));
				unsigned int pos = pecaPos[t][i];
				movimentosPossiveis(jogo.tabuleiro,pos,movimentos,&numMov,capturas,&numCapt);
				unsigned int peca = jogo.tabuleiro[pos];

				unsigned int k;
				//aplica o negamax para cada jogada possivel
				unsigned int j;
				unsigned int *tipoMovimento[2] = {capturas,movimentos};
				unsigned int numMovimento[2] = {numCapt,numMov};
				int m;
				for(m=0 ; m<2 ; m++)
				{
					for(j=0 ; j<numMovimento[m] ; j++)
					{
						//faz uma copia do jogo original
						memcpy(&jogo,jogoOrig,sizeof(*jogoOrig));
						jogada->time = time;
						jogada->pecaOrigem = jogo.tabuleiro[pos];
						jogada->posOrigem = pos;
						jogada->posDestino = tipoMovimento[m][j];

						//copia a jogada
						t_jogada bakJogada;
						if(primeiro)
							memcpy(&bakJogada,jogada,sizeof(*jogada));
						executaJogada(&jogo,jogada);
						double val;
						//verifica se a jogada levara ao termino do jogo
						int estado = fimDeJogo(&jogo,outroTime);
						if(estado == DRAW)
						{

							val = 0;
						}
						else if(estado == time)
						{

							val = 1;
						}
						else if(estado == outroTime)
						{

							val = -1;
						}
						else
							val = -miniMax(outroTime,jogada,&jogo,-beta,-alfa,-cor,profundidade-1,tempoMax,0,t0);
						if(val >= beta)
						{
							if(primeiro)
								memcpy(jogada,&bakJogada,sizeof(*jogada));
							return val;
						}
						if (val > alfa)
						{
							if(primeiro)
								memcpy(&melhorJogada,&bakJogada,sizeof(*jogada));
							alfa = val;
						}
					}//end for movimento
				}//end for tipo de movimento
			}//end for peca
		}//end for t

		if(primeiro)
			memcpy(jogada,&melhorJogada,sizeof(*jogada));
	}
	return alfa;
}

double miniMaxB(char time,t_jogada *jogada,t_jogo *jogoOrig,double alfa, double beta, int cor, unsigned int profundidade,unsigned int tempoMax,char primeiro,unsigned int t0)
{
	t_jogador *jogador;
	int filhos = 0;
	if(profundidade == 0)
	{
		return funcaoHeuristica(time,jogoOrig);
	}
	else
	{
		if(SDL_GetTicks() - t0 > tempoMax)
		{
			return -2;
		}
		char outroTime = (time==P1) ? P2 : P1;
		t_jogo jogo;
		memcpy(&jogo,jogoOrig,sizeof(*jogoOrig));
		t_jogada melhorJogada;

		if(time == P1)
			jogador = &jogo.p1;
		else
			jogador = &jogo.p2;

		unsigned int movimentos[14];
		unsigned int numMov = 0;
		unsigned int capturas[14];
		unsigned int numCapt = 0;
		//para cada peca do jogador, calcula seus movimentos possiveis
		int i;

		unsigned char *pecaPos[2] = {jogador->peaoPos,jogador->torrePos};
		unsigned char numPeca[2] = {jogador->numPeoes,jogador->numTorres};

		int t;
		t_jogo outroJogo;
		for(t=0 ; t<2 ; t++)
		{
			for(i=0 ; i<numPeca[t] ; i++ )
			{
				filhos++;
				//faz uma copia do jogo original
				memcpy(&jogo,jogoOrig,sizeof(*jogoOrig));
				memcpy(&outroJogo,jogoOrig,sizeof(*jogoOrig));
				unsigned int pos = pecaPos[t][i];
				movimentosPossiveis(jogo.tabuleiro,pos,movimentos,&numMov,capturas,&numCapt);
				unsigned int peca = jogo.tabuleiro[pos];

				unsigned int k;
				//aplica o negamax para cada jogada possivel
				unsigned int j;
				unsigned int *tipoMovimento[2] = {capturas,movimentos};
				unsigned int numMovimento[2] = {numCapt,numMov};
				int m;
				for(m=0 ; m<2 ; m++)
				{
					for(j=0 ; j<numMovimento[m] ; j++)
					{
						//faz uma copia do jogo original
						memcpy(&jogo,jogoOrig,sizeof(*jogoOrig));
						jogada->time = time;
						jogada->pecaOrigem = jogo.tabuleiro[pos];
						jogada->posOrigem = pos;
						jogada->posDestino = tipoMovimento[m][j];

						//copia a jogada
						t_jogada bakJogada;
						if(primeiro)
							memcpy(&bakJogada,jogada,sizeof(*jogada));
						executaJogada(&jogo,jogada);
						double val;
						//verifica se a jogada levara ao termino do jogo
						int estado = fimDeJogo(&jogo,outroTime);
						if(estado == DRAW)
						{

							val = 0;
						}
						else if(estado == time)
						{

							val = 1;
						}
						else if(estado == outroTime)
						{

							val = -1;
						}
						else
							val = -miniMaxB(outroTime,jogada,&jogo,-beta,-alfa,-cor,profundidade-1,tempoMax,0,t0);
						if(val >= beta)
						{
							if(primeiro)
								memcpy(jogada,&bakJogada,sizeof(*jogada));
							return val;
						}
						if (val > alfa)
						{
							if(primeiro)
								memcpy(&melhorJogada,&bakJogada,sizeof(*jogada));
							alfa = val;
						}
					}//end for movimento
				}//end for tipo de movimento
			}//end for peca
		}//end for t

		if(primeiro)
			memcpy(jogada,&melhorJogada,sizeof(*jogada));
	}
	return alfa;
}

/**
  * aplica o minimax em paralelo
  */
double parallelMiniMax(char time,t_jogada *jogada,t_jogo *jogoOrig,double alfa, double beta, int cor, unsigned int profundidade,unsigned int tempoMax,char primeiro,unsigned int t0)
{
	#define MAXMOV 120

	t_jogador *jogador;
	double maxFound = -2;;
	int filhos = 0;
	if(profundidade == 0)
	{
		return funcaoHeuristica(time,jogoOrig);
	}
	else
	{
		if(SDL_GetTicks() - t0 > tempoMax)
		{
			return -2;
		}
		char outroTime = (time==P1) ? P2 : P1;
		t_jogo jogo[MAXMOV];
		t_jogada jogadaPossivel[MAXMOV];
		double valor[MAXMOV];
		t_miniMaxData parametros[MAXMOV];
		SDL_Thread *threads[MAXMOV];
		int c;
		for(c=0 ; c<MAXMOV ; c++)
		{
			memcpy(&jogo[c],jogoOrig,sizeof(*jogoOrig));
			memcpy(&jogadaPossivel[c],jogada,sizeof(*jogada));
			valor[c] = -3.0;
			parametros[c].time = outroTime;
			parametros[c].beta = -alfa;
			parametros[c].alfa = -beta;
			parametros[c].cor = -cor;
			parametros[c].profundidade = profundidade-1;
			parametros[c].tempoMax = tempoMax;
			parametros[c].primeiro = 0;
			parametros[c].t0 = t0;

		}

		if(time == P1)
			jogador = &jogoOrig->p1;
		else
			jogador = &jogoOrig->p2;

		unsigned int movimentos[14];
		unsigned int numMov = 0;
		unsigned int capturas[14];
		unsigned int numCapt = 0;
		//para cada peca do jogador, calcula seus movimentos possiveis
		int i;

		unsigned char *pecaPos[2] = {jogador->peaoPos,jogador->torrePos};
		unsigned char numPeca[2] = {jogador->numPeoes,jogador->numTorres};

		int t;
		for(c=0,t=0 ; t<2 ; t++)
		{
			for(i=0 ; i<numPeca[t] ; i++ )
			{
				filhos++;
				unsigned int pos = pecaPos[t][i];
				movimentosPossiveis(jogo[c].tabuleiro,pos,movimentos,&numMov,capturas,&numCapt);
				unsigned int peca = jogo[c].tabuleiro[pos];
				//executa todas as jogadas possiveis
				unsigned int j;
				unsigned int *tipoMovimento[2] = {capturas,movimentos};
				unsigned int numMovimento[2] = {numCapt,numMov};
				int m;
				for(m=0 ; m<2 ; m++)
				{
					for(j=0 ; j<numMovimento[m] ; j++,c++)
					{
						ERR("c:%d\t",c);
						ERR("Jogo: %p\n",&jogo[c]);
						jogadaPossivel[c].time = time;
						jogadaPossivel[c].pecaOrigem = jogo[c].tabuleiro[pos];
						jogadaPossivel[c].posOrigem = pos;
						jogadaPossivel[c].posDestino = tipoMovimento[m][j];

						executaJogada(&jogo[c],&jogadaPossivel[c]);
						double val;
						//verifica se a jogada levara ao termino do jogo
						int estado = fimDeJogo(&jogo[c],outroTime);
						if(estado == DRAW)
						{
							valor[c] = 0;
						}
						else if(estado == time)
						{

							valor[c] = 1;
						}
						else if(estado == outroTime)
						{
							valor[c] = -1;
						}
						else
						{
							//aplica negamax em outra thread
							parametros[c].jogada = &jogadaPossivel[c];
							parametros[c].jogoOrig = &jogo[c];
							threads[c] = SDL_CreateThread(miniMaxThread,(void* )(&parametros[c]));
						}
					}//end for movimento
				}//end for tipo de movimento
			}//end for peca
		}//end for tipo de peca

		int totalBranches = c;
		maxFound = valor[0];
		int maxIndex = 0;
		//espera todas as threads terminarem, selecionando a melhor jogada
		for(c=0 ; c<totalBranches ; c++)
		{
			//se nao eh um nodo folha
			if(valor[c] < -2.1)
			{
				int status;
				SDL_WaitThread(threads[c],&status);
				//copia o valor encontrado
				valor[c] = parametros[c].val;
			}
			if(valor[c] > maxFound)
			{
				maxFound = valor[c];
				maxIndex = c;
			}
		}

		//if(primeiro)
		ERR("Para O: %u\t D:%u\n",jogadaPossivel[maxIndex].posOrigem,jogadaPossivel[maxIndex].posDestino);
		memcpy(jogada,jogadaPossivel+maxIndex,sizeof(*jogada));
	}
	return maxFound;
}

int miniMaxThread(void *minmaxData)
{
	t_miniMaxData *d = minmaxData;
	d->val = miniMax(d->time,d->jogada,d->jogoOrig,d->alfa,d->beta,d->cor,d->profundidade,d->tempoMax,d->primeiro,d->t0);
	return 0;
}

/**
  * determina quao bom eh o jogo para o jogador
  * retorna um valor em [-1,1]
  */
double funcaoHeuristica(char time, t_jogo* jogo)
{
	char outroTime;
	if(time==P1)
		outroTime = P2;
	else
		outroTime = P1;

	return funcaoHeuristicaJogador(time,jogo) - funcaoHeuristicaJogador(outroTime,jogo);
}


double funcaoHeuristicaJogador(char time, t_jogo* jogo)
{
	t_jogador *jogador;
	unsigned int partida;
	int sinal;
	//OBS: P1 = Brancas = Baixo
	//OBS: P2 = Pretas  = Cima
	if(time == P1)
	{
		partida = 6;
		sinal = -1;
		jogador = &jogo->p1;
	}
	else
	{
		partida = 1;
		sinal = 1;
		jogador = &jogo->p2;
	}

	//soma o numero de pecas
	int valor = 0;
	int pecas = 0;
	pecas += jogador->numPeoes;
	pecas += jogador->numTorres*2;

	//verifica quao perto estao os peoes da vitoria
	int i;
	int maisProximo = 0;
	for(i=0 ; i<jogador->numPeoes ; i++)
	{
		int proximidade = ((jogador->peaoPos[i]/8)-partida)*sinal;
		valor += proximidade;
		if(proximidade > maisProximo)
			maisProximo = proximidade;
	}

	double valores[3];
	valores[0] = (double)valor/(8*5);
	valores[1] = (double)pecas/14;
	valores[2] = (double)maisProximo/5;

	double final = (valores[0] + valores[1] + valores[2])/3.5;

	return final;

}

