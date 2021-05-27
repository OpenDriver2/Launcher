#ifndef _RED2Launcher_config_ini_writer_h_
#define _RED2Launcher_config_ini_writer_h_

struct config_data_t
{
	// settings
	char* imageFilename;
	int fullScreen;
	int pgxpTextureCorrection;
	int pgxpZBuffer;
	int bilinearFiltering;
	int disableChicagoBridges;
	int fieldOfView;
	int widescreenOverlayAlign;
	int fastLoadingScreens;
	int languageId;
};

bool SaveNewConfigFile(config_data_t* data);

#endif
