#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "frameControl.h"
#include "movement.h"
#include "drawer.h"

#include "definitions.h"

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
	glOrtho(0,1,0,1,0,1);
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
	//frame controller
	t_frameController ctrl;
	initFrameController(&ctrl,FPS);

	t_jogo jogo;
	iniciaEstadoJogo(&jogo);

	t_
	SDL_Thread *mestre = SDL_CreateThread((int (*)(void*))mestreDeJogo,);

	char quit = 0;
	while(!quit)
	{
		//verifica se o usuario fez algo
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
				quit = 1;
		}

		desenhaJogo(&camera,&jogo);
		SDL_GL_SwapBuffers();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		controlFramerate(ctrl);
	}

	return 0;
}
