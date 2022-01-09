#include "RED2Launcher.h"
#include "ini.h"
#include "config_ini_writer.h"

#include "iso9660_dump.h"

void RunJPSXDec(const char* imageFilename)
{	
#ifdef PLATFORM_WIN32
	const char* indexesFile = "install\\idxfile.tmp";
	system(Format("java -jar install\\jpsxdec.jar -f \"%s\" -x \"%s\"\n", imageFilename, indexesFile));
	system(Format("java -jar install\\jpsxdec.jar -x \"%s\" -a video -quality psx -vf avi:mjpg -up Lanczos3", indexesFile));
	system(Format("java -jar install\\jpsxdec.jar -x \"%s\" -a audio -quality psx -af wav", indexesFile));
#endif
#ifdef PLATFORM_LINUX
	const char* indexesFile = "install/idxfile.tmp";
	system(Format("java -jar install/jpsxdec.jar -f \"%s\" -x \"%s\"\n", imageFilename, indexesFile));
	system(Format("java -jar install/jpsxdec.jar -x \"%s\" -a video -quality psx -vf avi:mjpg -up Lanczos3", indexesFile));
	system(Format("java -jar install/jpsxdec.jar -x \"%s\" -a audio -quality psx -af wav", indexesFile));
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