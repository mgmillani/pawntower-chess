#include <math.h>

#include <SDL/SDL.h>

#include "movement.h"
#include "control.h"
#include "random.h"
#include "oldIa.h"
#include "ia.h"

#include "debug.h"

void iaMinMaxOld(t_jogador *jogador,char time,t_jogada *jogada,t_jogo *jogo,void *data)
{
	unsigned int t0 = SDL_GetTicks();
	unsigned int i;
	jogada->time = time;
	t_miniMax *parametros = data;
	unsigned int profundidade = parametros->profundidadeMaxima;
	unsigned int tempo = parametros->tempoMaximo;
	ERR("Tempo: %u\n",tempo);
	ERR("Profundidade: %u\n",profundidade);
	for(i=1 ; i<profundidade && (SDL_GetTicks() - t0 < tempo); i++)
	{
		ERR("Profundidade: %d\n",i);
		double val = miniMaxOld(time,jogada,jogo,-2,2,1,i,tempo,1,t0);
		ERR("Valor :%lf\n",val);
		ERR("O:%u\tD:%u\n",jogada->posOrigem,jogada->posDestino);
	}

	ERR("O:%u\tD:%u\n",jogada->posOrigem,jogada->posDestino);
}

double miniMaxOld(char time,t_jogada *jogada,t_jogo *jogoOrig,double alfa, double beta, int cor, unsigned int profundidade,unsigned int tempoMax,char primeiro,unsigned int t0)
{
	t_jogador *jogador;
	int filhos = 0;
	if(profundidade == 0)
	{
		if(time == P1)
			jogador = &jogoOrig->p1;
		else
			jogador = &jogoOrig->p2;

		return funcaoHeuristicaOld(jogador,time,jogoOrig);
	}
	else
	{
		if(SDL_GetTicks() - t0 > tempoMax)
		{
			return -2*cor;
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
				//aplica o negamax para cafuncaoHeuristicaOldda jogada possivel
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
							//ERR("EMPATE\n");
							val = 0;
						}
						else if(estado == time)
						{
							ERR("VITORIA %d\n",(int)time);
							//ERR("Val = %d\n",cor);
							val = 1;
						}
						else if(estado == outroTime)
						{
							ERR("DERROTA %d\n",(int)time);
							//ERR("Val = %d\n",-cor);
							val = -1;
						}
						else
							val = -miniMaxOld(outroTime,jogada,&jogo,-beta,-alfa,-cor,profundidade-1,tempoMax,0,t0);
						if(val >= beta)
						{
							if(primeiro)
								memcpy(jogada,&bakJogada,sizeof(*jogada));
							//ERR("(PRUNED)(%d) filhos: %d\n",profundidade,filhos);
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
	//ERR("(%d) filhos: %d\n",profundidade,filhos);
	return alfa;
}

/**
  * determina quao bom eh o jogo para o jogador
  * retorna um valor em [-1,1]
  */
double funcaoHeuristicaOld(t_jogador* jogador,char time, t_jogo* jogo)
{
	unsigned int partida;
	int sinal;
	if(time == P1)
	{
		partida = 1;
		sinal = 1;
	}
	else
	{
		partida = 6;
		sinal = -1;
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


