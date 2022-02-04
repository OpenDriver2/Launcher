#include "config_ini_writer.h"
#include <stdio.h>
#include <Core/Core.h>
using namespace Upp;


const char config_ini_kbcontrols[] = \
"# Game keyboard controls (Psy-X layer PSX mapping)\r\n"
"[kbcontrols_game]\r\n"
"cross=%s\r\n"
"square=%s\r\n"
"circle=%s\r\n"
"triangle=%s\r\n"
"up=%s\r\n"
"down=%s\r\n"
"left=%s\r\n"
"right=%s\r\n"
"start=%s\r\n"
"select=%s\r\n"
"l1=%s\r\n"
"r1=%s\r\n"
"l2=%s\r\n"
"r2=%s\r\n"
"l3=%s\r\n"
"r3=%s";

const char config_ini_controls[] = \
"[controls_game]\r\n"
"cross=%s\r\n"
"square=%s\r\n"
"circle=%s\r\n"
"triangle=%s\r\n"
"up=%s\r\n"
"down=%s\r\n"
"left=%s\r\n"
"right=%s\r\n"
"start=%s\r\n"
"select=%s\r\n"
"l1=%s\r\n"
"r1=%s\r\n"
"l2=%s\r\n"
"r2=%s\r\n"
"l3=%s\r\n"
"r3=%s\r\n"
"axis_left_x=%s\r\n"
"axis_left_y=%s\r\n"
"axis_right_x=%s\r\n"
"axis_right_y=%s";

const char config_ini_settings[] = \
"[cdfs]\r\n"
"%s=%s\r\n"
"mode=1,2,2352				# track, mode, sector size. DO NOT MODIFY\r\n"
"\r\n"
"[pad]\r\n"
"pad1device=-1				# player 1 controller device; -1 for automatic assignment\r\n"
"pad2device=-1				# player 2 controller device; -1 for automatic assignment\r\n"
"\r\n"
"[render]\r\n"
"windowWidth=1280\r\n"
"windowHeight=720\r\n"
"screenWidth=1280\r\n"
"screenHeight=720\r\n"
"fullscreen=%d				# enable full screen mode; it takes screen resolution\r\n"
"vsync=1					# Prevents screen tearing in Full screen. Turn it off if you have framerate problems.\r\n"
"pgxpTextureMapping=%d\r\n"
"bilinearFiltering=%d		# \"smooth\" textures\r\n"
"pgxpZbuffer=%d				# full Z-buffer on PSX polygons\r\n"
"\r\n"
"[game]\r\n"
"languageId=%d				# 0 = ENGLISH; 1 = ITALIAN; 2 = GERMAN; 3 = FRENCH; 4 = SPANISH;\r\n"
"drawDistance=1800			# 441..1800\r\n"
"fieldOfView=%d				# 128..384, 256 is default\r\n"
"disableChicagoBridges=%d	# Experimental: also activate AI roads\r\n"
"freeCamera=1				# Press F7 in game to enable\r\n"
"fastLoadingScreens=%d\r\n"
"widescreenOverlays=%d		# set 1 to see map, bars and stats aligned to screen corners\r\n"
"driver1music=0				# put Driver 1's MUSIC.BIN as D1MUSIC.BIN to DRIVER2\\SOUNDS folder\r\n"
"overrideContent=%d			# this enables texture and car model modding\r\n";

const char config_ini_fmt[] = \
"[fs]\r\n"
"# dataFolder=REDRIVER2\r\n"
"# dataFolder=.\r\n"
"\r\n"
"# Available controller binds (refer to SDL2 game controller)\r\n"
"#\r\n"
"# Axes:\r\n"
"#	leftx lefty\r\n"
"#	rightx righty\r\n"
"#	lefttrigger righttrigger\r\n"
"#\r\n"
"# NOTE: adding `-` before axis names makes it inverse, so -leftx inverse left stick X axis\r\n"
"#\r\n"
"# Buttons:\r\n"
"# 	a, b, x, y\r\n"
"# 	back guide start\r\n"
"# 	leftstick rightstick\r\n"
"# 	leftshoulder rightshoulder\r\n"
"# 	dpup dpdown dpleft dpright\r\n"
"\r\n"
"%s\r\n"
"\r\n"
"%s\r\n"
"\r\n"
"[kbcontrols_menu]\r\n"
"cross=return\r\n"
"square=q\r\n"
"circle=right shift\r\n"
"triangle=escape\r\n"
"start=escape\r\n"
"up=up\r\n"
"down=down\r\n"
"left=left\r\n"
"right=right\r\n"
"start=w\r\n"
"select=H\r\n"
"\r\n"
"%s";

//--------------------------------------------------------------------

void BestDefaultConfig(config_data_t* cfgData)
{
	cfgData->imageFilename = NULL;
	cfgData->fullScreen = 0;
	cfgData->pgxpTextureCorrection = 1;
	cfgData->pgxpZBuffer = 1;
	cfgData->bilinearFiltering = 1;
	cfgData->disableChicagoBridges = 0;
	cfgData->fieldOfView = 256;
	cfgData->widescreenOverlayAlign = 1;
	cfgData->fastLoadingScreens = 1;
	cfgData->languageId = 0;
	DefaultMappings(cfgData->keyboardCtrls);
	DefaultMappings(cfgData->controllerCtrls);
}

// stores config.ini from template above
bool SaveNewConfigFile(config_data_t* data)
{
	// keyboard controls config
	String kbControlsIniData = Format(config_ini_kbcontrols,
		KeyboardScanToName(data->keyboardCtrls.kc_cross),
		KeyboardScanToName(data->keyboardCtrls.kc_square),
		KeyboardScanToName(data->keyboardCtrls.kc_circle),
		KeyboardScanToName(data->keyboardCtrls.kc_triangle),
		KeyboardScanToName(data->keyboardCtrls.kc_dpad_up),
		KeyboardScanToName(data->keyboardCtrls.kc_dpad_down),
		KeyboardScanToName(data->keyboardCtrls.kc_dpad_left),
		KeyboardScanToName(data->keyboardCtrls.kc_dpad_right),
		KeyboardScanToName(data->keyboardCtrls.kc_start),
		KeyboardScanToName(data->keyboardCtrls.kc_select),
		KeyboardScanToName(data->keyboardCtrls.kc_l1),
		KeyboardScanToName(data->keyboardCtrls.kc_r1),
		KeyboardScanToName(data->keyboardCtrls.kc_l2),
		KeyboardScanToName(data->keyboardCtrls.kc_r2),
		KeyboardScanToName(data->keyboardCtrls.kc_l3),
		KeyboardScanToName(data->keyboardCtrls.kc_r3)
		);
	
	// controls config
	String controlsIniData = Format(config_ini_controls,
		ControllerMapToName(data->controllerCtrls.gc_cross),
		ControllerMapToName(data->controllerCtrls.gc_square),
		ControllerMapToName(data->controllerCtrls.gc_circle),
		ControllerMapToName(data->controllerCtrls.gc_triangle),
		ControllerMapToName(data->controllerCtrls.gc_dpad_up),
		ControllerMapToName(data->controllerCtrls.gc_dpad_down),
		ControllerMapToName(data->controllerCtrls.gc_dpad_left),
		ControllerMapToName(data->controllerCtrls.gc_dpad_right),
		ControllerMapToName(data->controllerCtrls.gc_start),
		ControllerMapToName(data->controllerCtrls.gc_select),
		ControllerMapToName(data->controllerCtrls.gc_l1),
		ControllerMapToName(data->controllerCtrls.gc_r1),
		ControllerMapToName(data->controllerCtrls.gc_l2),
		ControllerMapToName(data->controllerCtrls.gc_r2),
		ControllerMapToName(data->controllerCtrls.gc_l3),
		ControllerMapToName(data->controllerCtrls.gc_r3),
		ControllerMapToName(data->controllerCtrls.gc_axis_left_x),
		ControllerMapToName(data->controllerCtrls.gc_axis_left_y),
		ControllerMapToName(data->controllerCtrls.gc_axis_right_x),
		ControllerMapToName(data->controllerCtrls.gc_axis_right_y)
		);
		
	// make actual config ini data
	String configIniData = Format(config_ini_settings,
		data->imageFilename ? "image" : "# image=install/Driver2CD1.bin",
		data->imageFilename,
		data->fullScreen,
		data->pgxpTextureCorrection,
		data->bilinearFiltering,
		data->pgxpZBuffer,
		data->languageId,
		data->fieldOfView,
		data->disableChicagoBridges,
		data->fastLoadingScreens,
		data->widescreenOverlayAlign,
		data->imageFilename == NULL ? 1 : 0);
	

	FILE* fp = fopen("config.ini", "wb");
	if(!fp)
	{
		return false;
	}
	
	// write final INI file contents
	fprintf(fp, config_ini_fmt, (const char*)kbControlsIniData, (const char*)controlsIniData, (const char*)configIniData);
	
	fclose(fp);
	
	return true;
}