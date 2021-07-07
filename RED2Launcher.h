#ifndef _RED2Launcher_RED2Launcher_h
#define _RED2Launcher_RED2Launcher_h

#include <CtrlLib/CtrlLib.h>
#include "config_ini_writer.h"

using namespace Upp;

extern const char* LanguageList[];

#define LAYOUTFILE <Launcher/RED2Launcher.lay>
#include <CtrlCore/lay.h>

//------------------------

class ControlsWindow : public WithKeyControlsWindow<TopWindow> {
public:
	ControlsWindow();
		
protected:
	
	typedef WithKeyControlsWindow<TopWindow> BaseCtrl;
	
	virtual void Close();
	
	bool InitSDL2();
	void CloseSDL2();
	void UpdateSDL2();
};

struct ConfigWindow : public WithConfigWindowLayout<TopWindow> {
public:
	ConfigWindow();

	void LoadConfig();
	bool StoreConfig();

protected:
	FileSel fs;
	ControlsWindow ctrlWindow;
};

//------------------------

class SetupWizardWindow : public WithSetupWizardWindow<TopWindow> {
public:
	SetupWizardWindow();
protected:
	FileSel fs;
};

//------------------------

class RED2Launcher : public WithRED2LauncherLayout<TopWindow> {
public:
	RED2Launcher();
	
protected:
	void LaunchGame();
	ConfigWindow configWindow;
	SetupWizardWindow wizardWindow;
};

#endif
