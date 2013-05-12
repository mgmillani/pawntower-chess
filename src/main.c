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

#include "definitions.h"
#include "debug.h"

#define WIDTH 800
#define HEIGHT 600
#define BPP 32
#define FPS 50

const char gProgName[] = "Pawntower Chess";

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

	glClearColor(1.0f,1.0f,1.0f,0.0f);
	glViewport(0,0,width,height);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glOrtho(0.0f,width,height,0.0f,0.0f,1.0f);
	glOrtho(0,1,1,0,0,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//SDL_image
	//if(IMG_Init(IMG_INIT_PNG)==0)
	//	return -1;

	return 0;
}

int main(int argc, char *argv[])
{

	time_t seed;
	seed = time(NULL);
	srand(seed);


	int width = WIDTH;
	int height = HEIGHT;
	if(argc == 3)
	{
		sscanf(argv[1], "%d",&width);
		sscanf(argv[2], "%d",&height);
	}


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

	int tipoP1 = HUMANO;
	int tipoP2 = MAQUINA;
	int profundidade = 100;

	//menuPrincipal(&tipoP1,&tipoP2,width,height);

	t_jogo jogo;
	iniciaEstadoJogo(&jogo);

	t_controle controle;

	SDL_mutex *jogoPronto = SDL_CreateMutex();
	SDL_mutex *iniciaPartida = SDL_CreateMutex();
	SDL_LockMutex(jogoPronto);
	SDL_LockMutex(iniciaPartida);

	ERR("Inicia mestre\n");
	iniciaControle(&controle,&jogo,jogoPronto,iniciaPartida);

	SDL_Thread *mestre = SDL_CreateThread((int (*)(void*))mestreDeJogo,&controle);
	//espera ate o mestre ter terminado a inicializacao
	ERR("Espera o mestre\n");
	SDL_LockMutex(jogoPronto);
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

	if(tipoP1 == HUMANO)
	{
		iniciaControleHumano(&humano1,&realce,&celulaX,&celulaY,houveEntrada,escreveEntrada);
		jogador1Data.joga = jogaHumano;
		jogador1Data.data = &humano1;
		p1 = SDL_CreateThread((int (*)(void *))threadJogador,&jogador1Data);
	}
	else
	{
		//jogador1Data.joga = iaRandom;
		jogador1Data.joga = iaMinMax;
		jogador1Data.data = &profundidade;
		p1 = SDL_CreateThread((int (*)(void *))threadJogador,&jogador1Data);
	}
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
		jogador2Data.data = &profundidade;
		p2 = SDL_CreateThread((int (*)(void *))threadJogador,&jogador2Data);
	}

	//espera as threads estarem prontas
	SDL_SemWait(jogador1.pronto);
	SDL_SemWait(jogador2.pronto);

	SDL_UnlockMutex(iniciaPartida);

	ERR("Go!\n");

	char quit = 0;
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
				double x = event.motion.x/(double)width - camera.x;
				double y = event.motion.y/(double)height - camera.y;

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
			}
		}
		desenhaJogo(&camera,&jogo);
		SDL_GL_SwapBuffers();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		controlFramerate(ctrl);
	}

	SDL_Quit();

	return 0;
}
