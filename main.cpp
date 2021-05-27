#include "RED2Launcher.h"
#include "ini.h"
#include "config_ini_writer.h"

#define IMAGECLASS LauncherImages
#define IMAGEFILE <Launcher/images.iml>
#include <Draw/iml.h>

const char* LanguageList[] = {
	"English",
	"Italian",
	"German",
	"French",
	"Spanish",
};

ConfigWindow::ConfigWindow()
{
	CtrlLayout(*this, "Configuration");
	
	for(int i = 0; i < sizeof(LanguageList) / sizeof(char*); i++)
		languageList.Add(LanguageList[i]);
	
	finishBtn << [=] {
			if(StoreConfig())
				Close();
			else
				Exclamation("Unable to save 'config.ini'!");
		};
		
	imageChooseBtn << [=] {
			if(!fs.ExecuteOpen("Select ISO/BIN image"))
				return;
			String filename = fs;
			imageFilenameEdit <<= filename;
		};
		
	useImageCheck << [=] {
		imageChooseBtn.Enable(useImageCheck.Get());
		imageFilenameEdit.Enable(useImageCheck.Get());
	};
	
	fs.ActiveDir(".");
	fs.Type("Compatible ISO 9660 files (.bin, .iso, .ccd)", "*.bin *.iso *.ccd");
	
	useImageCheck.Set(0);
	imageChooseBtn.Enable(0);
	imageFilenameEdit.Enable(0);
	
	// temporarily disabled
	keyboardCtrlsBtn.Enable(0);
	joy1CtrlsBtn.Enable(0);

	// load config.ini
	LoadConfig();
}

void ConfigWindow::LoadConfig()
{
	config_data_t cfgData;
	ini_t* config;

	cfgData.fullScreen = 0;
	cfgData.pgxpTextureCorrection = 1;
	cfgData.pgxpZBuffer = 1;
	cfgData.bilinearFiltering = 1;
	cfgData.disableChicagoBridges = 0;
	cfgData.fieldOfView = 256;
	cfgData.widescreenOverlayAlign = 0;
	cfgData.fastLoadingScreens = 0;
	cfgData.languageId = 0;

	config = ini_load("config.ini");
	
	if(config)
	{
		const char* cdImageFileName = ini_get(config, "cdfs", "image");
		
		if(cdImageFileName)
		{
			useImageCheck.Set(1);
			imageChooseBtn.Enable(1);
			imageFilenameEdit.Enable(1);
		}
		imageFilenameEdit <<= cdImageFileName;
		
		// configure Psy-X pads
		//ini_sget(config, "pad", "pad1device", "%d", &g_controllerToSlotMapping[0]);
		//ini_sget(config, "pad", "pad2device", "%d", &g_controllerToSlotMapping[1]);

		// configure Psy-X renderer
		//ini_sget(config, "render", "windowWidth", "%d", &windowWidth);
		//ini_sget(config, "render", "windowHeight", "%d", &windowHeight);
		ini_sget(config, "render", "fullscreen", "%d", &cfgData.fullScreen);
		ini_sget(config, "render", "pgxpTextureMapping", "%d", &cfgData.pgxpTextureCorrection);
		ini_sget(config, "render", "pgxpZbuffer", "%d", &cfgData.pgxpZBuffer);
		ini_sget(config, "render", "bilinearFiltering", "%d", &cfgData.bilinearFiltering);

		// configure host game
		//ini_sget(config, "game", "drawDistance", "%d", &gDrawDistance);
		ini_sget(config, "game", "disableChicagoBridges", "%d", &cfgData.disableChicagoBridges);
		ini_sget(config, "game", "fieldOfView", "%d", &cfgData.fieldOfView);
		//ini_sget(config, "game", "freeCamera", "%d", &enableFreecamera);
		//ini_sget(config, "game", "driver1music", "%d", &gDriver1Music);
		ini_sget(config, "game", "widescreenOverlays", "%d", &cfgData.widescreenOverlayAlign);
		ini_sget(config, "game", "fastLoadingScreens", "%d", &cfgData.fastLoadingScreens);
		ini_sget(config, "game", "languageId", "%d", &cfgData.languageId);
		//ini_sget(config, "game", "overrideContent", "%d", &gContentOverride);
		
		ini_free(config);
	}

	fullscreenCheck.Set(cfgData.fullScreen);
	pgxpTextureMappingCheck.Set(cfgData.pgxpTextureCorrection);
	pgxpZbufferCheck.Set(cfgData.pgxpZBuffer);
	bilinearFilteringCheck.Set(cfgData.bilinearFiltering);
	disableChicagoBridgesCheck.Set(cfgData.disableChicagoBridges);
	fieldOfViewSpin <<= cfgData.fieldOfView;
	widescreenOverlaysCheck.Set(cfgData.widescreenOverlayAlign);
	fastLoadingScreensCheck.Set(cfgData.fastLoadingScreens);
	languageList.SetIndex(cfgData.languageId);
}

bool ConfigWindow::StoreConfig()
{
	config_data_t cfgData;
	
	std::string imageFilenameStr = (~imageFilenameEdit).ToStd();

	cfgData.imageFilename = (char*)imageFilenameStr.c_str();
	cfgData.fullScreen = fullscreenCheck.Get();
	cfgData.pgxpTextureCorrection = pgxpTextureMappingCheck.Get();
	cfgData.pgxpZBuffer = pgxpZbufferCheck.Get();
	cfgData.bilinearFiltering = bilinearFilteringCheck.Get();
	cfgData.disableChicagoBridges = disableChicagoBridgesCheck.Get();
	cfgData.fieldOfView = ~fieldOfViewSpin;
	cfgData.widescreenOverlayAlign = widescreenOverlaysCheck.Get();
	cfgData.fastLoadingScreens = fastLoadingScreensCheck.Get();
	cfgData.languageId = languageList.GetIndex();
	
	return SaveNewConfigFile(&cfgData);
}

//-----------------------------------------------

SetupWizardWindow::SetupWizardWindow()
{
	CtrlLayout(*this, "Setup wizard");
	
	fs.ActiveDir(".");
	fs.Type("Compatible ISO 9660 files (.bin, .iso, .ccd)", "*.bin *.iso *.ccd");
	
	selCDImageBtn << [=] {
			if(!fs.ExecuteOpen("Select ISO/BIN image"))
				return;
			
			int result = PromptYesNo("Do you want to unpack selected image? Choose NO to use directly");
			
			if(result == 1)
			{
				Exclamation("oofs");
			}
			
			Close();
		};
		
	selFolderBtn << [=] {
			if(!fs.ExecuteSelectDir("Select DRIVER2 folder"))
				return;

			Close();
		};
}

//-----------------------------------------------

RED2Launcher::RED2Launcher()
{
	CtrlLayout(*this, "Driver 2 PC");

	logoImg.SetImage(LauncherImages::Logo());
	launchBtn << [=] {
			LaunchGame();
			Exit(0);
		};
		
	configureBtn << [=] {
			configWindow.Open(this);
		};
		
	setupBtn << [=] {
			wizardWindow.Open(this);
		};
}

void RED2Launcher::LaunchGame()
{
#ifdef PLATFORM_WIN32
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOWDEFAULT;

	if(!CreateProcess("REDRIVER2.EXE", NULL, NULL, NULL, FALSE,
	                  CREATE_NO_WINDOW|NORMAL_PRIORITY_CLASS, NULL, NULL,
	                  &si, &pi))
	{
		Exclamation("Unable to launch game!");
	}
#endif
}

GUI_APP_MAIN
{
	// TODO: check if needed to run Setup Wizard
	//SetupWizardWindow().Run();
	
	RED2Launcher().Run();
}
