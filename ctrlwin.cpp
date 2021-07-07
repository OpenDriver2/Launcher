#include "RED2Launcher.h"

// for controller binder
#ifdef PLATFORM_POSIX
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

bool initsdl = 0;
int waitingAction = 0;
String waitingVal;
EditString* waitingEdit = nullptr;
SDL_Window*	waitingHWND = nullptr;

ControlsWindow::ControlsWindow()
{
	CtrlLayout(*this, "Controls");
	
	SetTimeCallback(-1, [=] {
		UpdateSDL2();
	});
	
	crossBtn << [=] {
		if(InitSDL2())
		{
			waitingAction = 0x10;
			waitingEdit = &crossVal;
			waitingVal = ~(*waitingEdit);

			(*waitingEdit) <<= "Press a key...";
		}
	};
}

void ControlsWindow::Close()
{
	if(waitingAction)
	{
		(*waitingEdit) <<= waitingVal;
		waitingEdit = nullptr;
		waitingAction = 0;
		
		CloseSDL2();
	}
	BaseCtrl::Close();
}

bool ControlsWindow::InitSDL2()
{
	if(!initsdl && SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS) < 0)
	{
		Exclamation(Format("Could not create window: %s\n", SDL_GetError()));
		return false;
	}
	
	initsdl = true;

	waitingHWND = SDL_CreateWindow("Press a key...", -1, -1, 1, 1, SDL_WINDOW_TOOLTIP | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS);

	if(!waitingHWND)
	{
		Exclamation(Format("Could not create window: %s\n", SDL_GetError()));
		return false;
	}
	
	return true;
}

void ControlsWindow::CloseSDL2()
{
	SDL_DestroyWindow(waitingHWND);

	waitingHWND = nullptr;

	SDL_Quit();
	initsdl = false;
}

void ControlsWindow::UpdateSDL2()
{
	if(waitingAction == 0)
		return;
	
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				int nKey = event.key.keysym.scancode;
				
				if(nKey == SDL_SCANCODE_DELETE)
				{
					CloseSDL2();
					break;
				}
				
				waitingVal = SDL_GetScancodeName((SDL_Scancode)nKey);
				
				// done?
				if(event.type == SDL_KEYUP)
				{
					
					(*waitingEdit) <<= waitingVal;
					waitingEdit = nullptr;
					waitingAction = 0;
					
					CloseSDL2();
				}
				
				break;
			}
		}
	}
}