#include "frameControl.h"
#include "SDL/SDL.h"
//#include "debug.h"

/**
	*	sleeps the required amount to maintan the desired framerate
	* returns the number of milliseconds slept
	*/
int controlFramerate(t_frameController frameCtrl)
{

	int delta = SDL_GetTicks() - frameCtrl.frameStart;	//how long this frame lasted
	if (delta<frameCtrl.duration)
	{
		SDL_Delay(frameCtrl.duration - delta);
		return frameCtrl.duration - delta;
	}

	return 0;
}

/**
  * inicializa a estrutura para o dado numero de frames por segundo
  */
t_frameController *initFrameController(t_frameController *ctrl, unsigned int fps)
{
	if(ctrl==NULL)
		ctrl=malloc(sizeof(*ctrl));

	ctrl->framesPerSec = fps;
	ctrl->frameStart = SDL_GetTicks();
	ctrl->duration = 1000/fps;

	return ctrl;


}
