#include "config_ini_writer.h"
#include <stdio.h>
#include <string.h>

const char config_ini_kbcontrols[] = \
"# Game keyboard controls (Psy-X layer PSX mapping)\r\n"
"[kbcontrols_game]\r\n"
"cross=up\r\n"
"square=down\r\n"
"circle=right shift\r\n"
"triangle=space\r\n"
"up=NONE\r\n"
"down=NONE\r\n"
"left=left\r\n"
"right=right\r\n"
"start=escape\r\n"
"select=c\r\n"
"l1=right ctrl\r\n"
"r1=H\r\n"
"l2=A\r\n"
"r2=D\r\n"
"l3=S\r\n"
"r3=return";

const char config_ini_controls[] = \
"[controls_game]\r\n"
"# cross=a\r\n"
"# square=x\r\n"
"# circle=b\r\n"
"# triangle=y\r\n"
"# up=dpup\r\n"
"# down=dpdown\r\n"
"# left=dpleft\r\n"
"# right=dpright\r\n"
"# start=start\r\n"
"# select=back\r\n"
"# l1=leftshoulder\r\n"
"# r1=rightshoulder\r\n"
"# l2=lefttrigger\r\n"
"# r2=righttrigger\r\n"
"# l3=leftstick\r\n"
"# r3=rightstick\r\n"
"# axis_left_x=leftx\r\n"
"# axis_left_y=lefty\r\n"
"# axis_right_x=rightx\r\n"
"# axis_right_y=righty";

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
"fullscreen=%d				# enable full screen mode; it takes screen resolution\r\n"
"pgxpTextureMapping=%d\r\n"
"bilinearFiltering=%d			# \"smooth\" textures\r\n"
"pgxpZbuffer=%d				# full Z-buffer on PSX polygons\r\n"
"\r\n"
"[game]\r\n"
"languageId=%d				# 0 = ENGLISH; 1 = ITALIAN; 2 = GERMAN; 3 = FRENCH; 4 = SPANISH;\r\n"
"drawDistance=1800			# 441..1800\r\n"
"fieldOfView=%d				# 128..384, 256 is default\r\n"
"disableChicagoBridges=%d		# Experimental: also activate AI roads\r\n"
"freeCamera=1				# Press F7 in game to enable\r\n"
"fastLoadingScreens=%d\r\n"
"widescreenOverlays=%d		# set 1 to see map, bars and stats aligned to screen corners\r\n"
"driver1music=0				# put Driver 1's MUSIC.BIN as D1MUSIC.BIN to DRIVER2\\SOUNDS folder\r\n"
"overrideContent=%d			# this enables texture and car model modding\r\n"
"userChases=RacingFreak,Snoopi,Olanov,Vortex,Fireboyd78";

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
}

// stores config.ini from template above
bool SaveNewConfigFile(config_data_t* data)
{
	static char buffer[sizeof(config_ini_settings) + 2048];
	memset(buffer, 0, sizeof(buffer));
	
	// make actual config ini data
	sprintf(buffer, config_ini_settings,
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
	fprintf(fp, config_ini_fmt, config_ini_kbcontrols, config_ini_controls, buffer);
	
	fclose(fp);
	
	return true;
}