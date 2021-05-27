#ifndef _RED2Launcher_RED2Launcher_h
#define _RED2Launcher_RED2Launcher_h

#include <CtrlLib/CtrlLib.h>
#include "config_ini_writer.h"

using namespace Upp;

#define LAYOUTFILE <Launcher/RED2Launcher.lay>
#include <CtrlCore/lay.h>

struct ConfigWindow : public WithConfigWindowLayout<TopWindow> {
public:
	ConfigWindow();

	void LoadConfig();
	bool StoreConfig();

protected:
	FileSel fs;
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
