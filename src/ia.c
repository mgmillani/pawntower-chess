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
	int i = 1;
	jogada->time = time;
	int profundidade = *((int *)data);
	for(i=0 ; i<profundidade && (SDL_GetTicks() - t0 < 5000); i++)
	{
		ERR("Profundidade: %d\n",i);
		miniMax(time,jogada,jogo,-2,2,1,i,1,t0);
	}

	ERR("O:%u\tD:%u\n",jogada->posOrigem,jogada->posDestino);
}

double miniMax(char time,t_jogada *jogada,t_jogo *jogoOrig,double alfa, double beta, int cor, unsigned int profundidade,char primeiro,unsigned int t0)
{
	t_jogador *jogador;
	if(profundidade == 0)
	{
		if(time == P1)
			jogador = &jogoOrig->p1;
		else
			jogador = &jogoOrig->p2;
		return cor * funcaoHeuristica(jogador,time,jogoOrig);
	}
	else
	{
		if(SDL_GetTicks() - t0 > 5000)
			return -2*cor;
		//int filhos = 1;
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
							//ERR("EMPATE\n");
							val = 0;
						}
						else if(estado == time)
						{
							//ERR("VITORIA %d\n",(int)time);
							//ERR("Val = %d\n",cor);
							val = cor;
						}
						else if(estado == outroTime)
						{
							//ERR("DERROTA %d\n",(int)time);
							//ERR("Val = %d\n",-cor);
							val = -cor;
						}
						else
							val = miniMax(outroTime,jogada,&jogo,-beta,-alfa,-cor,profundidade-1,0,t0);
						if(val <= beta)
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
  * determina quao bom eh o jogo para o jogador
  * retorna um valor em [-1,1]
  */
double funcaoHeuristica(t_jogador* jogador,char time, t_jogo* jogo)
{

	unsigned int partida;
	int sinal;
	if(time == P1)
	{
		partida = 2;
		sinal = -1;
	}
	else
	{
		partida = 5;
		sinal = 1;
	}

	//soma o numero de pecas
	int valor = 0;
	int pecas = 0;
	pecas += jogador->numPeoes;
	pecas += jogador->numTorres;

	//verifica quao perto estao os peoes da vitoria
	int i;
	int maisProximo = 8;
	for(i=0 ; i<jogador->numPeoes ; i++)
	{
		int proximidade = ((jogador->peaoPos[i]/7)-partida)*sinal;
		valor += proximidade;
		if(proximidade < maisProximo)
			maisProximo = proximidade;
	}

	return ((valor/8*7) + ((double)pecas/10) + ((double)maisProximo/7))/4.0;

}

