#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_opengl.h>

#include "frameControl.h"
#include "movement.h"
#include "interface.h"
#include "control.h"
#include "drawer.h"
#include "init.h"
#include "ia.h"
#include "oldIa.h"

#include "definitions.h"
#include "debug.h"

#define WIDTH 800
#define HEIGHT 600
#define BPP 32
#define FPS 50

const char gProgName[] = "Pawntower Chess";
const char gConfFile[] = "config";

void carregaConfig(const char *fname, int *width, int *height,unsigned int *maxDepth, unsigned int *maxTime, int *tipoP1, int *tipoP2)
{

	typedef enum {State_NewLine,State_Rvalue,State_Equal,State_Lvalue} t_state;

	t_state estado = State_NewLine;

	FILE *fl = fopen(fname,"rb");
	unsigned int size;
	fseek(fl,0,SEEK_END);
	size = ftell(fl);
	fseek(fl,0,SEEK_SET);

	char *data = malloc(size+1);
	fread(data,1,size,fl);
	data[size] = '\0';
	fclose(fl);
	int i;
	int bl,br;
	int el,er;

	for(i=0 ; data[i]!='\0' ; i++)
	{
		char c = data[i];
		switch(estado)
		{
			case State_NewLine:
				bl = i;
				estado = State_Lvalue;
				break;
			case State_Lvalue:
				if(c=='=')
				{
					//terminou a palavra esquerda
					el = i;
					br = i+1;
					estado = State_Equal;
				}
				else if(c==' ')
				{
					el = i;
					estado = State_Equal;
				}
				break;
			case State_Equal:
				if(c != ' ' && c != '=')
				{
					br = i;
					estado = State_Rvalue;
				}
				break;
			case State_Rvalue:
				if(c=='\r' || c=='\n')
				{
					estado = State_NewLine;
					er = i;
					char aux = data[el];
					char aux2 = data[er];
					data[el] = '\0';
					data[er] = '\0';
					if(strcmp("width",data+bl)==0)
						sscanf(data+br,"%d",width);
					else if(strcmp("height",data+bl)==0)
						sscanf(data+br,"%d",height);
					else if(strcmp("depth",data+bl)==0)
						sscanf(data+br,"%u",maxDepth);
					else if(strcmp("time",data+bl)==0)
						sscanf(data+br,"%u",maxTime);
					else if(strcmp("black",data+bl)==0)
					{
						if(strcmp("human",data+br)==0)
							*tipoP2 = HUMANO;
						else
							*tipoP2 = MAQUINA;
					}
					else if(strcmp("white",data+bl)==0)
					{
						if(strcmp("human",data+br)==0)
							*tipoP1 = HUMANO;
						else
							*tipoP1 = MAQUINA;
					}

					data[el] = aux;
					data[er] = aux2;
				}
				break;
		}
	}

	if(estado != State_NewLine)
	{
		er = i;
		char aux = data[el];
		char aux2 = data[er];
		data[el] = '\0';
		data[er] = '\0';
		if(strcmp("width",data+bl)==0)
			sscanf(data+br,"%d",width);
		else if(strcmp("height",data+bl)==0)
			sscanf(data+br,"%d",height);
		else if(strcmp("depth",data+bl)==0)
			sscanf(data+br,"%u",maxDepth);
		else if(strcmp("time",data+bl)==0)
			sscanf(data+br,"%u",maxTime);
		else if(strcmp("black",data+bl)==0)
		{
			if(strcmp("human",data+br)==0)
				*tipoP2 = HUMANO;
			else
				*tipoP2 = MAQUINA;
		}
		else if(strcmp("black",data+bl)==0)
		{
			if(strcmp("human",data+br)==0)
				*tipoP1 = HUMANO;
			else
				*tipoP1 = MAQUINA;
		}

		data[el] = aux;
		data[er] = aux2;
	}

}

//inits stuff and the screen
int init(SDL_Surface **screen,int width,int height,int bpp,int options)
{
	SDL_putenv("SDL_VIDEO_CENTERED=center");
	if(SDL_Init(SDL_INIT_VIDEO) == -1)
			return -1;

	SDL_WM_SetCaption(gProgName,NULL);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

	*screen = SDL_SetVideoMode(width,height,bpp,options);
	if(*screen == NULL)
		return -1;

	//opengl stuff
	glDisable(GL_DEPTH_TEST);

	glClearColor(0.7f,1.0f,0.7f,0.0f);
	glViewport(0,0,width,height);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glOrtho(0.0f,width,height,0.0f,0.0f,1.0f);
	glOrtho(0,1,1,0,0,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return 0;
}

int main(int argc, char *argv[])
{

	time_t seed;
	seed = time(NULL);
	srand(seed);

	int tipoP1 = HUMANO;
	int tipoP2 = MAQUINA;
	unsigned int profundidade = 100;
	unsigned int tempoMax = 5000;
	int width = WIDTH;
	int height = HEIGHT;

	carregaConfig(gConfFile,&width,&height,&profundidade,&tempoMax,&tipoP1,&tipoP2);

	ERR("%d vs %d\n",tipoP1,tipoP2);

	//inicializacao
	SDL_Surface *screen;
	SDL_Event event;
	init(&screen,width,height,BPP,SDL_OPENGL | SDL_SWSURFACE);
	//camera
	t_rect camera;
	camera.x = (width - height)/(2.0*width);
	camera.y = 0;
	camera.w = height/(double)width;
	camera.h = 1;
	double cellW = camera.w/8;
	double cellH = camera.h/8;
	//frame controller
	t_frameController ctrl;
	initFrameController(&ctrl,FPS);

	//menuPrincipal(&tipoP1,&tipoP2,width,height);

	t_jogo jogo;
	iniciaEstadoJogo(&jogo);

	t_controle controle;
	t_jogada ultimaJogada;
	ultimaJogada.posOrigem = 100;
	ultimaJogada.posDestino = 100;

	SDL_mutex *leUltimaJogada = SDL_CreateMutex();
	SDL_sem *jogoPronto = SDL_CreateSemaphore(1);
	SDL_sem *iniciaPartida = SDL_CreateSemaphore(1);
	SDL_SemWait(jogoPronto);
	SDL_SemWait(iniciaPartida);

	ERR("Inicia mestre\n");
	iniciaControle(&controle,&jogo,&ultimaJogada,jogoPronto,iniciaPartida,leUltimaJogada);

	SDL_Thread *mestre = SDL_CreateThread((int (*)(void*))mestreDeJogo,&controle);
	//espera ate o mestre ter terminado a inicializacao
	ERR("Espera o mestre\n");
	SDL_SemWait(jogoPronto);
	//cria os jogadores humanos
	int celulaX;
	int celulaY;
	t_realce realce;
	iniciaRealce(&realce);
	SDL_sem *houveEntrada = SDL_CreateSemaphore(1);
	SDL_SemWait(houveEntrada);
	SDL_mutex *escreveEntrada = SDL_CreateMutex();
	t_controleHumano controleP1;
	iniciaControleHumano(&controleP1,&realce,&celulaX,&celulaY,houveEntrada,escreveEntrada);
	t_controleHumano controleP2;
	iniciaControleHumano(&controleP2,&realce,&celulaX,&celulaY,houveEntrada,escreveEntrada);

	t_controleJogador jogador1;
	iniciaControleJogador(&jogador1,&controle,P1);
	t_jogadorData jogador1Data;
	jogador1Data.controle = &jogador1;

	t_controleJogador jogador2;
	iniciaControleJogador(&jogador2,&controle,P2);
	t_jogadorData jogador2Data;
	jogador2Data.controle = &jogador2;

	//cria as threads dos jogadores

	SDL_Thread *p1;
	SDL_Thread *p2;

	t_controleHumano humano1;
	t_controleHumano humano2;

	//espera as threads estarem prontas
	SDL_SemWait(jogador1.pronto);
	SDL_SemWait(jogador2.pronto);

	t_miniMax parametrosIa;
	parametrosIa.profundidadeMaxima = profundidade;
	parametrosIa.tempoMaximo = tempoMax;

	//P1 = Brancas
	if(tipoP1 == HUMANO)
	{
		iniciaControleHumano(&humano1,&realce,&celulaX,&celulaY,houveEntrada,escreveEntrada);
		jogador1Data.joga = jogaHumano;
		jogador1Data.data = &humano1;
		p1 = SDL_CreateThread((int (*)(void *))threadJogador,&jogador1Data);
	}
	else
	{
		jogador1Data.joga = iaMinMax;
		jogador1Data.data = &parametrosIa;
		p1 = SDL_CreateThread((int (*)(void *))threadJogador,&jogador1Data);
	}
	//P2 = Pretas
	if(tipoP2 == HUMANO)
	{
		iniciaControleHumano(&humano2,&realce,&celulaX,&celulaY,houveEntrada,escreveEntrada);
		jogador2Data.joga = jogaHumano;
		jogador2Data.data = &humano2;
		p2 = SDL_CreateThread((int (*)(void *))threadJogador,&jogador2Data);
	}
	else
	{
		//jogador2Data.joga = iaRandom;
		jogador2Data.joga = iaMinMax;
		jogador2Data.data = &parametrosIa;
		p2 = SDL_CreateThread((int (*)(void *))threadJogador,&jogador2Data);
	}

	//espera as threads estarem prontas
	SDL_SemWait(jogador1.pronto);
	SDL_SemWait(jogador2.pronto);

	SDL_SemPost(iniciaPartida);

	ERR("Go!\n");

	char quit = 0;
	char invert = 0;
	while(!quit)
	{
		//verifica se o usuario fez algo
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
				quit = 1;
			else if(event.type == SDL_MOUSEBUTTONDOWN)
			{
				SDL_LockMutex(escreveEntrada);
				//calcula em qual celula ocorreu o clique
				double x;
				double y;

				if(invert)
				{
					y = 1 - event.motion.y/(double)height - camera.y;
					x = 1 -event.motion.x/(double)width - camera.x;
				}
				else
				{
					y = event.motion.y/(double)height - camera.y;
					x = event.motion.x/(double)width - camera.x;
				}

				celulaX = x/cellW;
				celulaY = y/cellH;
				//avisa as threads que o usuario fez algo
				SDL_SemPost(houveEntrada);
				SDL_UnlockMutex(escreveEntrada);
			}
			else if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				else if(event.key.keysym.sym == SDLK_r)
					invert ^= 1;
			}
		}

		//desenha o tabuleiro e a ultima jogada
		SDL_LockMutex(leUltimaJogada);
		if(invert)
		{
			glPushMatrix();
			glTranslatef(0.5,0.5,0);
			glRotatef(180,0,0,1);
			glTranslatef(-0.5,-0.5,0);
			desenhaJogo(&camera,&jogo,&ultimaJogada);
			glPopMatrix();
		}
		else
			desenhaJogo(&camera,&jogo,&ultimaJogada);
		SDL_UnlockMutex(leUltimaJogada);
		SDL_GL_SwapBuffers();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		controlFramerate(ctrl);
	}

	SDL_Quit();

	return 0;
}
