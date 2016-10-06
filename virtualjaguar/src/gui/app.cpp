//
// app.cpp - Qt-based GUI for Virtual Jaguar
//
// by James Hammons
// (C) 2010 Underground Software
//
// JLH = James Hammons <jlhamm@acm.org>
//
// Who  When        What
// ---  ----------  -------------------------------------------------------------
// JLH  12/23/2009  Created this file
// JLH  01/21/2011  Added SDL initialization
// JLH  06/26/2011  Added fix to keep SDL from hijacking main() on win32
// JLH  05/24/2012  Added option switches
// JLH  03/05/2013  Fixed console redireciton on win32 platform  :-P
//

#include "app.h"

#include <SDL.h>
#include <QApplication>
#include "gamepad.h"
#include "log.h"
#include "mainwin.h"
#include "profile.h"
#include "settings.h"
#include "version.h"


// Apparently on win32, SDL is hijacking main from Qt. So let's do this:
#ifdef __GCCWIN32__
#undef main
#include <windows.h>	// Ick, but needed for console redirection on win32 :-O
#endif

// Function prototypes...
static bool ParseCommandLine(int argc, char * argv[]);
static void ParseOptions(int argc, char * argv[]);


//hm. :-/
// This is stuff we pass into the mainWindow...
// Also, these are defaults. :-)
bool noUntunedTankPlease = false;
bool loadAndGo = false;
bool useLogfile = false;
QString filename;

// Here's the main application loop--short and simple...
int main(int argc, char * argv[])
{
	// Win32 console redirection, because MS and their band of super geniuses
	// decided that nobody would ever launch an app from the command line. :-P
	// [Unfortunately, this doesn't seem to work on Vista/7. :-(]
#ifdef __GCCWIN32__
	BOOL (WINAPI * AttachConsole)(DWORD dwProcessId);

	AttachConsole = (BOOL (WINAPI *)(DWORD))
		GetProcAddress(LoadLibraryA("kernel32.dll"), "AttachConsole");

	if (AttachConsole != NULL && AttachConsole(((DWORD)-1)))
	{
		if (_fileno(stdout) == -1)
			freopen("CONOUT$","wb",stdout);
		if (_fileno(stderr) == -1)
			freopen("CONOUT$","wb",stderr);
		if (_fileno(stdin) == -1)
			freopen("CONIN$","rb",stdin);

		// Fix C++
		std::ios::sync_with_stdio();
	}
#endif

	// Normally, this would be read in from the settings module... :-P
	vjs.hardwareTypeAlpine = false;
	// This is stuff we pass into the mainWindow...
//	noUntunedTankPlease = false;

	// Check for options that must be in place be constructing the App object
	if (!ParseCommandLine(argc, argv))
	{
		return 0;
	}

	Q_INIT_RESOURCE(virtualjaguar);	// This must the same name as the exe filename
//or is it the .qrc filename???
	// This is so we can pass this stuff using signal/slot mechanism...
//this is left here to remind me not to try doing this again :-P
//ick	int id = qRegisterMetaType<uint32>();

	int retVal = -1;							// Default is failure

	if (useLogfile)
	{
		bool success = (bool)LogInit("./virtualjaguar.log");	// Init logfile

		if (!success)
			printf("Failed to open virtualjaguar.log for writing!\n");
	}

	// Set up SDL library
	if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0)
	{
		WriteLog("VJ: Could not initialize the SDL library: %s\n", SDL_GetError());
	}
	else
	{
		WriteLog("VJ: SDL (joystick, audio) successfully initialized.\n");
		App app(argc, argv);					// Declare an instance of the application
		Gamepad::AllocateJoysticks();
		AutoConnectProfiles();
		retVal = app.exec();					// And run it!
		Gamepad::DeallocateJoysticks();

		// Free SDL components last...!
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_AUDIO);
		SDL_Quit();
	}

#ifdef __GCCWIN32__
#if 0
	fclose(ctt);
#endif
#endif
	LogDone();									// Close logfile
	return retVal;
}

//
// Main app constructor--we stick globally accessible stuff here... (?)
//
App::App(int & argc, char * argv[]): QApplication(argc, argv)
{
	bool loadAndGo = !filename.isEmpty();

	mainWindow = new MainWin(loadAndGo);
	mainWindow->plzDontKillMyComputer = noUntunedTankPlease;
	ParseOptions(argc, argv);					// Override defaults with command line (if any)
	mainWindow->SyncUI();

	if (loadAndGo)
		mainWindow->LoadFile(filename);

	mainWindow->show();
}


//
// Here we parse out stuff that needs to be looked at *before* we construct the 
// App object.
//
bool ParseCommandLine(int argc, char * argv[])
{
	for(int i=1; i<argc; i++)
	{
		if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0)
			|| (strcmp(argv[i], "-?") == 0))
		{
			printf(
				"Virtual Jaguar " VJ_RELEASE_VERSION " (" VJ_RELEASE_SUBVERSION ")\n"
				"Based upon Virtual Jaguar core v1.0.0 by David Raingeard.\n"
				"Written by James Hammons (Linux/WIN32), Niels Wagenaar (Linux/WIN32),\n"
				"Carwin Jones (BeOS), and Adam Green (MacOS)\n"
				"Contact: http://sdlemu.ngemu.com/ | sdlemu@ngemu.com\n"
				"\n"
				"Usage:\n"
				"   virtualjaguar [<filename>] [switches]\n"
				"\n"
				"   Option            Description\n"
				"   ----------------  -----------------------------------\n"
				"   <filename>        Name of file to autoload\n"
				"   --alpine      -a  Put Virtual Jaguar into Alpine mode\n"
				"   --pal         -p  PAL mode\n"
				"   --ntsc        -n  NTSC mode\n"
				"   --bios        -b  Boot using Jagaur BIOS\n"
				"   --no-bios         Do not use Jaguar BIOS\n"
				"   --gpu         -g  Enable GPU\n"
				"   --no-gpu          Disable GPU\n"
				"   --dsp         -d  Enable DSP\n"
				"   --no-dsp          Disable DSP\n"
				"   --fullscreen  -f  Start in full screen mode\n"
				"   --blur        -B  Enable GL bilinear filter\n"
				"   --no-blur         Disable GL bilinear filtering\n"
				"   --log         -l  Create and use log file\n"
				"   --no-log          Do not use log file\n"
				"   --help        -h  Show this message\n"
				"   --please-dont-kill-my-computer\n"
				"                 -z  Run Virtual Jaguar without \"snow\"\n"
				"\n"
				"Invoking Virtual Jagaur with no filename will cause it to boot up\n"
				"with the VJ GUI. Using Alpine mode will enable log file.\n"
				"\n");
			return false;
		}

		if (strcmp(argv[i], "--yarrr") == 0)
		{
			printf("\n");
			printf("Shiver me timbers!\n");
			printf("\n");
			return false;
		}

		if ((strcmp(argv[i], "--alpine") == 0) || (strcmp(argv[i], "-a") == 0))
		{
			printf("Alpine Mode enabled.\n");
			vjs.hardwareTypeAlpine = true;
			// We also enable logging as well :-)
			useLogfile = true;
		}

		if ((strcmp(argv[i], "--please-dont-kill-my-computer") == 0) || (strcmp(argv[i], "-z") == 0))
		{
			noUntunedTankPlease = true;
		}

		if ((strcmp(argv[i], "--log") == 0) || (strcmp(argv[i], "-l") == 0))
		{
			useLogfile = true;
		}

		if (strcmp(argv[i], "--no-log") == 0)
		{
			useLogfile = false;
		}

		// Check for filename
		if (argv[i][0] != '-')
		{
			loadAndGo = true;
			filename = argv[i];
		}
	}

	return true;
}


//
// This is to override settings loaded from the config file.
// Note that settings set here will become the new defaults!
// (Not any more: Settings are only saved if the config dialog was OKed, or
// the toolbar buttons were pressed.)
//
void ParseOptions(int argc, char * argv[])
{
	for(int i=1; i<argc; i++)
	{
		if ((strcmp(argv[i], "--pal") == 0) || (strcmp(argv[i], "-p") == 0))
		{
			vjs.hardwareTypeNTSC = false;
		}

		if ((strcmp(argv[i], "--ntsc") == 0) || (strcmp(argv[i], "-n") == 0))
		{
			vjs.hardwareTypeNTSC = true;
		}

		if ((strcmp(argv[i], "--bios") == 0) || (strcmp(argv[i], "-b") == 0))
		{
			vjs.useJaguarBIOS = true;
		}

		if (strcmp(argv[i], "--no-bios") == 0)
		{
			vjs.useJaguarBIOS = false;
		}

		if ((strcmp(argv[i], "--gpu") == 0) || (strcmp(argv[i], "-g") == 0))
		{
			vjs.GPUEnabled = true;
		}

		if (strcmp(argv[i], "--no-gpu") == 0)
		{
			vjs.GPUEnabled = false;
		}

		if ((strcmp(argv[i], "--dsp") == 0) || (strcmp(argv[i], "-d") == 0))
		{
			vjs.DSPEnabled = true;
			vjs.audioEnabled = true;
		}

		if (strcmp(argv[i], "--no-dsp") == 0)
		{
			vjs.DSPEnabled = false;
			vjs.audioEnabled = false;
		}

		if ((strcmp(argv[i], "--fullscreen") == 0) || (strcmp(argv[i], "-f") == 0))
		{
			vjs.fullscreen = true;
		}

		if ((strcmp(argv[i], "--blur") == 0) || (strcmp(argv[i], "-B") == 0))
		{
			vjs.glFilter = 1;
		}

		if (strcmp(argv[i], "--no-blur") == 0)
		{
			vjs.glFilter = 0;
		}
	}
}

#if 0
	bool useJoystick;
	int32 joyport;								// Joystick port
	bool hardwareTypeNTSC;						// Set to false for PAL
	bool useJaguarBIOS;
	bool GPUEnabled;
	bool DSPEnabled;
	bool usePipelinedDSP;
	bool fullscreen;
	bool useOpenGL;
	uint32 glFilter;
	bool hardwareTypeAlpine;
	bool audioEnabled;
	uint32 frameSkip;
	uint32 renderType;
	bool allowWritesToROM;

	// Keybindings in order of U, D, L, R, C, B, A, Op, Pa, 0-9, #, *

	uint32 p1KeyBindings[21];
	uint32 p2KeyBindings[21];

	// Paths

	char ROMPath[MAX_PATH];
	char jagBootPath[MAX_PATH];
	char CDBootPath[MAX_PATH];
	char EEPROMPath[MAX_PATH];
	char alpineROMPath[MAX_PATH];
	char absROMPath[MAX_PATH];
#endif
