#include "RED2Launcher.h"
#include "ini.h"
#include "config_ini_writer.h"
#include "iso9660_dump.h"

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
	BestDefaultConfig(&cfgData);

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

void RunJPSXDec(const char* imageFilename)
{
	const char* indexesFile = "idxfile.tmp";
	
#ifdef PLATFORM_WIN32
	system(Format("java -jar install\\jpsxdec.jar -f %s -x %s", imageFilename, indexesFile));
	system(Format("java -jar install\\jpsxdec.jar -x %s -a video -quality psx -vf avi:mjpg -up Lanczos3", indexesFile));
	system(Format("java -jar install\\jpsxdec.jar -x %s -a audio -quality psx -af wav", indexesFile));
#endif
}

SetupWizardWindow::SetupWizardWindow()
{
	CtrlLayout(*this, "Setup wizard");
	
	fs.ActiveDir(".");
	fs.Type("Compatible ISO 9660 files (.bin, .iso, .ccd)", "*.bin *.iso *.ccd");
	
	unpackFMV_XACheck.Set(1);
	fullUnpackCheck.Set(1);
	
	imageCD1ChooseBtn << [=] {
		if(!fs.ExecuteOpen("Select ISO/BIN image"))
			return;
		
		imageCD1FilenameEdit = (~fs);
	};
	
	imageCD2ChooseBtn << [=] {
		if(!fs.ExecuteOpen("Select ISO/BIN image"))
			return;
		
		imageCD2FilenameEdit = (~fs);
	};
	
	installBtn << [=] {
		
			std::string cd1Filename = (~imageCD1FilenameEdit).ToStd();
			std::string cd2Filename = (~imageCD2FilenameEdit).ToStd();
			
			if(cd1Filename.length() < 2 && cd2Filename.length() < 2)
			{
				Exclamation("Please select CD1 or CD2 to continue!");
				return;
			}
			
			bool dumped = false;
			
			std::string cdImageFiles[2] = {
				cd1Filename,
				cd2Filename
			};
			
			if(cd1Filename.length() < 2 || cd2Filename.length() < 2)
			{
				if(!PromptYesNo("Not all CD images selected. You will still be able to run the game. Continue?"))
					return;
			}
			
			for(int i = 0; i < 2; i++)
			{
				if(cdImageFiles[i].length() < 2)
					continue;
				
				const char* imageFilename = cdImageFiles[i].c_str();

				if(CheckIsDriver2GameImage(imageFilename))
				{
					if(!dumped) // this will allow to dump any of CDs
					{
						bool dumpImage = fullUnpackCheck.Get();
					
						if(dumpImage)
						{
							if(!DumpImageFile((char*)imageFilename))
							{
								Exclamation("Unable to unpack the game!");
								return;
							}
						}
						
						int languageId = DetectDriver2Language((char*)imageFilename);
						
						if(languageId != -1)
						{
							PromptOK(String("Language detected: ") + LanguageList[languageId]);
						}
						else
							Exclamation("Unable to detect language - custom CDs?");
						
						// make default best game settings
						config_data_t cfgData;
						BestDefaultConfig(&cfgData);
		
						cfgData.languageId = languageId;
						cfgData.imageFilename = dumpImage ? NULL : (char*)imageFilename;
					
						if(!SaveNewConfigFile(&cfgData))
							Exclamation("Unable to save 'config.ini'!");
						
						PromptOK(String("Detected game language: ") + LanguageList[languageId]);
						
						dumped = true;
					}
					
					// also run FMV and XA conversion
					if(unpackFMV_XACheck.Get())
					{
						RunJPSXDec(imageFilename);
					}
				}
				else
				{
					Exclamation(Format("CD%d is not valid game CD image or it is corrupted!", i + 1));
				}
			}
			
			if(!dumped)
			{
				Exclamation("Installation failed!");
			}
			else
			{
				PromptOK("Installation completed!");
			}
			
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
