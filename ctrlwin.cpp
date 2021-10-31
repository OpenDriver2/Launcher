#include "RED2Launcher.h"

// for controller binder
#ifdef PLATFORM_POSIX
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

bool initsdl = 0;
SDL_Window*	waitingHWND = nullptr;
SDL_GameController* currentController = nullptr;

#define CONTROL_BUTTON_CHANGER(name) \
	name ## Btn << [=] { \
		if(waitingEdit) return; \
		if(initsdl || !initsdl && InitSDL2()) { \
			waitingAction = waitingController ? &controllerCtrls.gc_ ## name : &keyboardCtrls.kc_ ## name; \
			waitingEdit = &name ## Val;\
			waitingVal = ~(*waitingEdit);\
			(*waitingEdit) <<= "Press a key...";\
		}\
	};

ControlsWindow::ControlsWindow()
{
	CtrlLayout(*this, "Controls");
	
	SetTimeCallback(-1, [=] {
		UpdateSDL2();
	});
	
	CONTROL_BUTTON_CHANGER(cross)
	CONTROL_BUTTON_CHANGER(square)
	CONTROL_BUTTON_CHANGER(circle)
	CONTROL_BUTTON_CHANGER(triangle)
	CONTROL_BUTTON_CHANGER(dpad_left)
	CONTROL_BUTTON_CHANGER(dpad_right)
	CONTROL_BUTTON_CHANGER(select)
	
	CONTROL_BUTTON_CHANGER(l1)
	CONTROL_BUTTON_CHANGER(r1)
	CONTROL_BUTTON_CHANGER(l2)
	CONTROL_BUTTON_CHANGER(r2)
	CONTROL_BUTTON_CHANGER(l3)
	CONTROL_BUTTON_CHANGER(r3)
	
	revertBtn << [=] {
		if(waitingController)
		{
			DefaultMappings(controllerCtrls);
		}
		else
		{
			DefaultMappings(keyboardCtrls);
		}
		InitButtons(waitingController);
	};
	
	closeBtn << [=] {
		Close();
	};
}

void ControlsWindow::InitButtons(bool controller)
{
	waitingController = controller;
	
	if(controller)
	{
		crossVal <<= ControllerMapToName(controllerCtrls.gc_cross);
		squareVal <<= ControllerMapToName(controllerCtrls.gc_square);
		circleVal <<= ControllerMapToName(controllerCtrls.gc_circle);
		triangleVal <<= ControllerMapToName(controllerCtrls.gc_triangle);
		//crossVal <<= ControllerMapToName(controllerCtrls.gc_dpad_up);
		//crossVal <<= ControllerMapToName(controllerCtrls.gc_dpad_down);
		dpad_leftVal <<= ControllerMapToName(controllerCtrls.gc_dpad_left);
		dpad_rightVal <<= ControllerMapToName(controllerCtrls.gc_dpad_right);
		//crossVal <<= ControllerMapToName(controllerCtrls.gc_start);
		selectVal <<= ControllerMapToName(controllerCtrls.gc_select);
		l1Val <<= ControllerMapToName(controllerCtrls.gc_l1);
		r1Val <<= ControllerMapToName(controllerCtrls.gc_r1);
		l2Val <<= ControllerMapToName(controllerCtrls.gc_l2);
		r2Val <<= ControllerMapToName(controllerCtrls.gc_r2);
		l3Val <<= ControllerMapToName(controllerCtrls.gc_l3);
		r3Val <<= ControllerMapToName(controllerCtrls.gc_r3);
	}
	else
	{
		crossVal <<= KeyboardScanToName(keyboardCtrls.kc_cross);
		squareVal <<= KeyboardScanToName(keyboardCtrls.kc_square);
		circleVal <<= KeyboardScanToName(keyboardCtrls.kc_circle);
		triangleVal <<= KeyboardScanToName(keyboardCtrls.kc_triangle);
		//crossVal <<= KeyboardScanToName(keyboardCtrls.kc_dpad_up);
		//crossVal <<= KeyboardScanToName(keyboardCtrls.kc_dpad_down);
		dpad_leftVal <<= KeyboardScanToName(keyboardCtrls.kc_dpad_left);
		dpad_rightVal <<= KeyboardScanToName(keyboardCtrls.kc_dpad_right);
		//crossVal <<= KeyboardScanToName(keyboardCtrls.kc_start);
		selectVal <<= KeyboardScanToName(keyboardCtrls.kc_select);
		l1Val <<= KeyboardScanToName(keyboardCtrls.kc_l1);
		r1Val <<= KeyboardScanToName(keyboardCtrls.kc_r1);
		l2Val <<= KeyboardScanToName(keyboardCtrls.kc_l2);
		r2Val <<= KeyboardScanToName(keyboardCtrls.kc_r2);
		l3Val <<= KeyboardScanToName(keyboardCtrls.kc_l3);
		r3Val <<= KeyboardScanToName(keyboardCtrls.kc_r3);
	}
}

void ControlsWindow::Close()
{
	if(waitingAction)
	{
		(*waitingEdit) <<= waitingVal;
		waitingEdit = nullptr;
		waitingAction = nullptr;
		
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
	
	if(waitingController)
	{
		if(SDL_NumJoysticks() == 0)
		{
			Exclamation("At least one controller needs to be attached!");
			return false;
		}
		
		currentController = SDL_GameControllerOpen(0);
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
	SDL_GameControllerClose(currentController);
	SDL_DestroyWindow(waitingHWND);

	waitingHWND = nullptr;
	currentController = nullptr;

	SDL_Quit();
	initsdl = false;
}

void ControlsWindow::UpdateSDL2()
{
	if(!waitingAction)
		return;
	
	// focus to SDL window - only when this window has focus. Also deep check ensures that this
	// window will not flicker.
	if(HasFocusDeep())
	{
		SDL_RaiseWindow(waitingHWND);
		SDL_SetWindowInputFocus(waitingHWND);
	}

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		// cancel or delete
		switch (event.type)
		{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				int nKey = event.key.keysym.scancode;
				
				if(nKey == SDL_SCANCODE_ESCAPE)
				{
					(*waitingEdit) <<= waitingVal;
					waitingEdit = nullptr;
					waitingAction = nullptr;
					
					CloseSDL2();
					return;
				}
				
				if(nKey == SDL_SCANCODE_DELETE)
				{
					if(waitingController)
						(*waitingAction) = SDL_CONTROLLER_BUTTON_INVALID;
					else
						(*waitingAction) = SDL_SCANCODE_UNKNOWN;
					
					(*waitingEdit) <<= "";
					waitingEdit = nullptr;
					waitingAction = nullptr;
					
					CloseSDL2();
					return;
				}
				
				break;
			}
		}
		
		if(waitingController)
		{
			switch (event.type)
			{
				case SDL_CONTROLLERAXISMOTION:
				{
					SDL_GameControllerAxis axis = (SDL_GameControllerAxis)event.caxis.axis;
					
					Sint16 value = event.caxis.value;
					
					bool negateFlag = false;
					
					if(value < 0)
					{
						negateFlag = true;
					}
					
					if(abs(value) >= 16384)
					{
						int axisIdx = axis | CONTROLLER_MAP_FLAG_AXIS;
						
						if(negateFlag)
							axisIdx |= CONTROLLER_MAP_FLAG_INVERSE;
						
						waitingVal = ControllerMapToName(axisIdx);
						
						(*waitingAction) = axisIdx;
						(*waitingEdit) <<= waitingVal;
						waitingEdit = nullptr;
						waitingAction = nullptr;
						
						CloseSDL2();
					}
					
					break;
				}
				case SDL_CONTROLLERBUTTONDOWN:
				case SDL_CONTROLLERBUTTONUP:
				{
					SDL_GameControllerButton nKey = (SDL_GameControllerButton)event.cbutton.button;

					waitingVal = ControllerMapToName(nKey);

					// done?
					if(event.cbutton.state == SDL_RELEASED)
					{
						(*waitingAction) = nKey;
						(*waitingEdit) <<= waitingVal;
						waitingEdit = nullptr;
						waitingAction = nullptr;
						
						CloseSDL2();
					}
					break;
				}
			}
		}
		else
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
				case SDL_KEYUP:
				{
					SDL_Scancode nKey = (SDL_Scancode)event.key.keysym.scancode;
	
					waitingVal = SDL_GetScancodeName(nKey);
					
					// done?
					if(event.type == SDL_KEYUP)
					{
						(*waitingAction) = nKey;
						(*waitingEdit) <<= waitingVal;
						waitingEdit = nullptr;
						waitingAction = nullptr;
						
						CloseSDL2();
					}
					break;
				}
			}
		}
	}
}