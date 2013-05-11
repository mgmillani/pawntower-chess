#include <SDL/SDL_opengl.h>
#include <SDL/SDL.h>

#include "interface.h"
#include "drawer.h"

#include "definitions.h"

/**
  * trata o menu principal onde o usuario decide quem joga contra quem
  * escreve em tipoP1 e tipoP2 se o jogador sera HUMANO ou MAQUINA
  */
void menuPrincipal(int *tipoP1,int *tipoP2,int screenWidth, int screenHeight)
{
	int escolha = -1;
	int numBotoes = 1;
	t_rect botoes[numBotoes];
	botoes[0].x = 0.25;
	botoes[0].y = 0.25;
	botoes[0].w = 0.1;
	botoes[0].h = 0.1;
	while(escolha < 0)
	{
		double x,y;
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
				escolha = 1;
			else if(event.type==SDL_MOUSEMOTION)
			{
				x = event.motion.x / (double)screenWidth;
				y = event.motion.y / (double)screenHeight;

			}
			else if(event.type == SDL_MOUSEBUTTONDOWN)
			{
				x = event.motion.x / (double)screenWidth;
				y = event.motion.y / (double)screenHeight;
				escolha = trataClique(botoes,numBotoes,x,y);
			}
			else if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym == SDLK_ESCAPE)
					escolha = 1;
			}
		}

		desenhaMenuPrincipal(botoes,numBotoes);
		SDL_GL_SwapBuffers();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		SDL_Delay(10);
	}

	switch(escolha)
	{
		case HH:
			*tipoP1 = HUMANO;
			*tipoP2 = HUMANO;
			break;
		case HM:
			*tipoP1 = HUMANO;
			*tipoP2 = MAQUINA;
			break;
		case MH:
			*tipoP1 = MAQUINA;
			*tipoP2 = HUMANO;
			break;
		case MM:
			*tipoP1 = MAQUINA;
			*tipoP2 = MAQUINA;
			break;
	}
}

/**
  * verifica se o usuario clicou em um dos botoes
  */
int trataClique(t_rect *botoes, int numBotoes, double x, double y)
{
	return -1;
}

/**
  * verifica se o usuario passou o mouse por cima de um dos botoes
  */
int trataMovimento(t_rect *botoes, int numBotoes, double x, double y)
{
	return -1;
}
