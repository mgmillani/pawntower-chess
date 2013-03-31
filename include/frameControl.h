#ifndef FRAMECONTROL_H
#define FRAMECONTROL_H

typedef struct s_frameController
{
	int duration;		//the duration, in milliseconds, of each frame
	unsigned int framesPerSec;
	unsigned int frameStart;	//the millisecond in which the current frame started

}t_frameController;

/**
	*	sleeps the required amount to maintan the desired framerate
	* returns the number of milliseconds slept
	*/
int controlFramerate(t_frameController frameCtrl);

/**
  * inicializa a estrutura para o dado numero de frames por segundo
  */
t_frameController *initFrameController(t_frameController *ctrl, unsigned int fps);

#endif
