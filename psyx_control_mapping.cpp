#include "psyx_control_mapping.h"

#include <SDL.h>
#include "ini.h"
#include <string.h>

#ifdef __GNUC__
#define _stricmp(s1, s2) strcasecmp(s1, s2)
#endif

//-----------------------------------------------------------------------
// PsyX_main.cpp part

// Keyboard mapping lookup
int PsyX_LookupKeyboardMapping(const char* str, int default_value)
{
	const char* scancodeName;
	int i;

	if (str)
	{
		if (!_stricmp("NONE", str))
			return SDL_SCANCODE_UNKNOWN;

		for (i = 0; i < SDL_NUM_SCANCODES; i++)
		{
			scancodeName = SDL_GetScancodeName((SDL_Scancode)i);

			if (strlen(scancodeName) && !_stricmp(scancodeName, str))
			{
				return i;
			}
		}
	}

	return default_value;
}

// Game controller mapping lookup
// Available controller binds(refer to SDL2 game controller)
//
// Axes:
//	leftx lefty
//	rightx righty
//	lefttrigger righttrigger
//
// NOTE: adding `-` before axis names makes it inverse, so `-leftx` inverse left stick X axis
//
// Buttons:
// 	a, b, x, y
// 	back guide start
// 	leftstick rightstick
// 	leftshoulder rightshoulder
// 	dpup dpdown dpleft dpright

int PsyX_LookupGameControllerMapping(const char* str, int default_value)
{
	const char* axisStr;
	const char* buttonOrAxisName;
	int i, axisFlags;

	if (str)
	{
		axisFlags = CONTROLLER_MAP_FLAG_AXIS;
		axisStr = str;

		if (*axisStr == '-')
		{
			axisFlags |= CONTROLLER_MAP_FLAG_INVERSE;
			axisStr++;
		}

		if (!_stricmp("NONE", str))
			return SDL_CONTROLLER_BUTTON_INVALID;

		// check buttons
		for (i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++)
		{
			buttonOrAxisName = SDL_GameControllerGetStringForButton((SDL_GameControllerButton)i);

			if (strlen(buttonOrAxisName) && !_stricmp(buttonOrAxisName, str))
			{
				return i;
			}
		}

		// Check axes
		for (i = 0; i < SDL_CONTROLLER_AXIS_MAX; i++)
		{
			buttonOrAxisName = SDL_GameControllerGetStringForAxis((SDL_GameControllerAxis)i);

			if (strlen(buttonOrAxisName) && !_stricmp(buttonOrAxisName, axisStr))
			{
				return i | axisFlags;
			}
		}
	}

	return default_value;
}

// this is for saving
const char* KeyboardScanToName(int scan)
{
	return SDL_GetScancodeName((SDL_Scancode)scan);
}

const char* ControllerMapToName(int value)
{
	// parse axis name
	if(value & CONTROLLER_MAP_FLAG_AXIS)
	{
		static char name[128];
		bool inverse = (value & CONTROLLER_MAP_FLAG_INVERSE);
		
		value &= ~(CONTROLLER_MAP_FLAG_AXIS | CONTROLLER_MAP_FLAG_INVERSE);
		
		// add a minus prefix
		if(inverse)
		{
			sprintf(name, "-%s", SDL_GameControllerGetStringForAxis((SDL_GameControllerAxis)value));
			return name;
		}
			
		return SDL_GameControllerGetStringForAxis((SDL_GameControllerAxis)value);
	}
	
	return SDL_GameControllerGetStringForButton((SDL_GameControllerButton)value);
}

void DefaultMappings(PsyXKeyboardMapping& mapping)
{
	mapping.kc_square = SDL_SCANCODE_DOWN;
	mapping.kc_circle = SDL_SCANCODE_RSHIFT;
	mapping.kc_triangle = SDL_SCANCODE_SPACE;
	mapping.kc_cross = SDL_SCANCODE_UP;

	mapping.kc_l1 = SDL_SCANCODE_RCTRL;
	mapping.kc_l2 = SDL_SCANCODE_A;
	mapping.kc_l3 = SDL_SCANCODE_S;

	mapping.kc_r1 = SDL_SCANCODE_H;
	mapping.kc_r2 = SDL_SCANCODE_D;
	mapping.kc_r3 = SDL_SCANCODE_RETURN;

	mapping.kc_dpad_up = SDL_SCANCODE_UNKNOWN;
	mapping.kc_dpad_down = SDL_SCANCODE_UNKNOWN;
	mapping.kc_dpad_left = SDL_SCANCODE_LEFT;
	mapping.kc_dpad_right = SDL_SCANCODE_RIGHT;

	mapping.kc_select = SDL_SCANCODE_C;
	mapping.kc_start = SDL_SCANCODE_ESCAPE;

}

void DefaultMappings(PsyXControllerMapping& mapping)
{
	mapping.gc_square = SDL_CONTROLLER_BUTTON_X;
	mapping.gc_circle = SDL_CONTROLLER_BUTTON_B;
	mapping.gc_triangle = SDL_CONTROLLER_BUTTON_Y;
	mapping.gc_cross = SDL_CONTROLLER_BUTTON_A;

	mapping.gc_l1 = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	mapping.gc_l2 = SDL_CONTROLLER_AXIS_TRIGGERLEFT | CONTROLLER_MAP_FLAG_AXIS;
	mapping.gc_l3 = SDL_CONTROLLER_BUTTON_LEFTSTICK;

	mapping.gc_r1 = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
	mapping.gc_r2 = SDL_CONTROLLER_AXIS_TRIGGERRIGHT | CONTROLLER_MAP_FLAG_AXIS;
	mapping.gc_r3 = SDL_CONTROLLER_BUTTON_RIGHTSTICK;

	mapping.gc_dpad_up = SDL_CONTROLLER_BUTTON_DPAD_UP;
	mapping.gc_dpad_down = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
	mapping.gc_dpad_left = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
	mapping.gc_dpad_right = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;

	mapping.gc_select = SDL_CONTROLLER_BUTTON_BACK;
	mapping.gc_start = SDL_CONTROLLER_BUTTON_START;

	mapping.gc_axis_left_x = SDL_CONTROLLER_AXIS_LEFTX | CONTROLLER_MAP_FLAG_AXIS;
	mapping.gc_axis_left_y = SDL_CONTROLLER_AXIS_LEFTY | CONTROLLER_MAP_FLAG_AXIS;
	mapping.gc_axis_right_x = SDL_CONTROLLER_AXIS_RIGHTX | CONTROLLER_MAP_FLAG_AXIS;
	mapping.gc_axis_right_y = SDL_CONTROLLER_AXIS_RIGHTY | CONTROLLER_MAP_FLAG_AXIS;
}

//-------------------------------------------------------------------
// redriver2_main.cpp part


void ParseKeyboardMappings(ini_t* config, char* section, PsyXKeyboardMapping& outMapping)
{
	extern PsyXKeyboardMapping g_keyboard_mapping;

	const char* str;

	str = ini_get(config, section, "square");
	outMapping.kc_square = PsyX_LookupKeyboardMapping(str, g_keyboard_mapping.kc_square);

	str = ini_get(config, section, "circle");
	outMapping.kc_circle = PsyX_LookupKeyboardMapping(str, g_keyboard_mapping.kc_circle);

	str = ini_get(config, section, "triangle");
	outMapping.kc_triangle = PsyX_LookupKeyboardMapping(str, g_keyboard_mapping.kc_triangle);

	str = ini_get(config, section, "cross");
	outMapping.kc_cross = PsyX_LookupKeyboardMapping(str, g_keyboard_mapping.kc_cross);

	str = ini_get(config, section, "l1");
	outMapping.kc_l1 = PsyX_LookupKeyboardMapping(str, g_keyboard_mapping.kc_l1);

	str = ini_get(config, section, "l2");
	outMapping.kc_l2 = PsyX_LookupKeyboardMapping(str, g_keyboard_mapping.kc_l2);

	str = ini_get(config, section, "l3");
	outMapping.kc_l3 = PsyX_LookupKeyboardMapping(str, g_keyboard_mapping.kc_l3);

	str = ini_get(config, section, "r1");
	outMapping.kc_r1 = PsyX_LookupKeyboardMapping(str, g_keyboard_mapping.kc_r1);

	str = ini_get(config, section, "r2");
	outMapping.kc_r2 = PsyX_LookupKeyboardMapping(str, g_keyboard_mapping.kc_r2);

	str = ini_get(config, section, "r3");
	outMapping.kc_r3 = PsyX_LookupKeyboardMapping(str, g_keyboard_mapping.kc_r3);

	str = ini_get(config, section, "up");
	outMapping.kc_dpad_up = PsyX_LookupKeyboardMapping(str, g_keyboard_mapping.kc_dpad_up);

	str = ini_get(config, section, "down");
	outMapping.kc_dpad_down = PsyX_LookupKeyboardMapping(str, g_keyboard_mapping.kc_dpad_down);

	str = ini_get(config, section, "left");
	outMapping.kc_dpad_left = PsyX_LookupKeyboardMapping(str, g_keyboard_mapping.kc_dpad_left);

	str = ini_get(config, section, "right");
	outMapping.kc_dpad_right = PsyX_LookupKeyboardMapping(str, g_keyboard_mapping.kc_dpad_right);

	str = ini_get(config, section, "select");
	outMapping.kc_select = PsyX_LookupKeyboardMapping(str, g_keyboard_mapping.kc_select);

	str = ini_get(config, section, "start");
	outMapping.kc_start = PsyX_LookupKeyboardMapping(str, g_keyboard_mapping.kc_start);
}

void ParseControllerMappings(ini_t* config, char* section, PsyXControllerMapping& outMapping)
{
	extern PsyXControllerMapping g_controller_mapping;

	const char* str;

	str = ini_get(config, section, "square");
	outMapping.gc_square = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_square);

	str = ini_get(config, section, "circle");
	outMapping.gc_circle = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_circle);

	str = ini_get(config, section, "triangle");
	outMapping.gc_triangle = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_triangle);

	str = ini_get(config, section, "cross");
	outMapping.gc_cross = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_cross);

	str = ini_get(config, section, "l1");
	outMapping.gc_l1 = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_l1);

	str = ini_get(config, section, "l2");
	outMapping.gc_l2 = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_l2);

	str = ini_get(config, section, "l3");
	outMapping.gc_l3 = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_l3);

	str = ini_get(config, section, "r1");
	outMapping.gc_r1 = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_r1);

	str = ini_get(config, section, "r2");
	outMapping.gc_r2 = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_r2);

	str = ini_get(config, section, "r3");
	outMapping.gc_r3 = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_r3);

	str = ini_get(config, section, "up");
	outMapping.gc_dpad_up = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_dpad_up);

	str = ini_get(config, section, "down");
	outMapping.gc_dpad_down = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_dpad_down);

	str = ini_get(config, section, "left");
	outMapping.gc_dpad_left = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_dpad_left);

	str = ini_get(config, section, "right");
	outMapping.gc_dpad_right = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_dpad_right);

	str = ini_get(config, section, "select");
	outMapping.gc_select = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_select);

	str = ini_get(config, section, "start");
	outMapping.gc_start = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_start);

	str = ini_get(config, section, "axis_left_x");
	outMapping.gc_axis_left_x = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_axis_left_x);

	str = ini_get(config, section, "axis_left_y");
	outMapping.gc_axis_left_y = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_axis_left_y);

	str = ini_get(config, section, "axis_right_x");
	outMapping.gc_axis_right_x = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_axis_right_x);

	str = ini_get(config, section, "axis_right_y");
	outMapping.gc_axis_right_y = PsyX_LookupGameControllerMapping(str, g_controller_mapping.gc_axis_right_y);
}
