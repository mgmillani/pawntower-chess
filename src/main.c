#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_opengl.h>

#include "frameControl.h"
#include "movement.h"
#include "control.h"
#include "drawer.h"
#include "init.h"

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

	//inicializacao
	SDL_Surface *screen;
	SDL_Event event;
	init(&screen,WIDTH,HEIGHT,BPP,SDL_OPENGL | SDL_SWSURFACE);
	//camera
	t_rect camera;
	camera.x = (WIDTH - HEIGHT)/(2.0*WIDTH);
	camera.y = 0;
	camera.w = HEIGHT/(double)WIDTH;
	camera.h = 1;
	double cellW = camera.w/8;
	double cellH = camera.h/8;
	//frame controller
	t_frameController ctrl;
	initFrameController(&ctrl,FPS);

	t_jogo jogo;
	iniciaEstadoJogo(&jogo);

	t_controle controle;
	iniciaControle(&controle,&jogo);
	SDL_mutex *jogoPronto = SDL_CreateMutex();
	SDL_LockMutex(jogoPronto);
	SDL_Thread *mestre = SDL_CreateThread((int (*)(void*))mestreDeJogo,&controle);
	//espera ate o mestre ter terminado a inicializacao
	SDL_CondWait(controle.inicioJogo,jogoPronto);
	ERR("Mestre criado.\n");
	//cria os jogadores humanos
	int celulaX;
	int celulaY;
	t_realce realce;
	iniciaRealce(&realce);
	SDL_cond *houveEntrada = SDL_CreateCond();
	t_controleHumano controleP1;
	iniciaControleHumano(&controleP1,&controle,P1,&realce,&celulaX,&celulaY,houveEntrada);
	t_controleHumano controleP2;
	iniciaControleHumano(&controleP2,&controle,P2,&realce,&celulaX,&celulaY,houveEntrada);

	t_controleIA ia2;
	iniciaControleIA(&ia2,&controle,P2);
	t_iaData iadata2;
	iadata2.controle = &ia2;
	iadata2.joga = iaRandom;
	iadata2.data = NULL;

	t_controleIA ia1;
	iniciaControleIA(&ia1,&controle,P1);
	t_iaData iadata1;
	iadata1.controle = &ia1;
	iadata1.joga = iaRandom;
	iadata1.data = NULL;
	ERR("Go!\n");
	SDL_CondBroadcast(controle.inicioJogo);

	//SDL_Thread *p1 = SDL_CreateThread((int (*)(void *))threadIa,&iadata1 );
	SDL_Thread *p1 = SDL_CreateThread((int (*)(void *))jogadorHumano,&controleP1 );
	SDL_Thread *p2 = SDL_CreateThread((int (*)(void *))threadIa,&iadata2 );
	//SDL_Thread *p2 = SDL_CreateThread((int (*)(void *))jogadorHumano,&controleP2 );

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
				//calcula em qual celula ocorreu o clique
				double x = event.motion.x/(double)WIDTH - camera.x;
				double y = event.motion.y/(double)HEIGHT - camera.y;

				celulaX = x/cellW;
				celulaY = y/cellH;
				//avisa as threads que o usuario fez algo
				/*ERR("Entrada:\n");
				ERR("X:%d\tY:%d\n",celulaX,celulaY);
				ERR("sX:%lf\tsY:%lf\n",x,y);*/
				SDL_CondBroadcast(houveEntrada);
				//printTabuleiro(jogo.tabuleiro);
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
