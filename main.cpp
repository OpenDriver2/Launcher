#include "RED2Launcher.h"

#define IMAGECLASS LauncherImages
#define IMAGEFILE <Launcher/images.iml>
#include <Draw/iml.h>

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
