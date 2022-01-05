#include "RED2Launcher.h"

#define IMAGECLASS LauncherImages
#define IMAGEFILE <Launcher/images.iml>
#include <Draw/iml.h>

//-----------------------------------------------

RED2Launcher::RED2Launcher()
{
	// Change working directory in case someone start the launcher from another directory.
	#ifdef PLATFORM_LINUX
		char path_buff[PATH_MAX];
    	ssize_t len = readlink("/proc/self/exe", path_buff, sizeof(path_buff) - 1);
		if (len != -1 ) {
    		std::string path_with_exe = std::string(path_buff);
      		size_t last_slash =  path_with_exe.find_last_of("/");
	  		chdir(path_with_exe.substr(0, last_slash).c_str());
    	} else {
    		Exclamation("Cannot change working directory of the process");
    		Exit(-1);
    	}
	#endif
	
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
#ifdef PLATFORM_LINUX
	execl("REDRIVER2", (char *) NULL);
#endif
}

GUI_APP_MAIN
{
	// TODO: check if needed to run Setup Wizard
	//SetupWizardWindow().Run();
	
	RED2Launcher().Run();
}
