//
// mainwin.cpp - Qt-based GUI for Virtual Jaguar: Main Application Window
// by James Hammons
// (C) 2009 Underground Software
//
// JLH = James Hammons <jlhamm@acm.org>
//
// Who  When        What
// ---  ----------  ------------------------------------------------------------
// JLH  12/23/2009  Created this file
// JLH  12/20/2010  Added settings, menus & toolbars
// JLH  07/05/2011  Added CD BIOS functionality to GUI
//

// FIXED:
//
// - Add dbl click/enter to select in cart list, ESC to dimiss [DONE]
// - Autoscan/autoload all available BIOS from 'software' folder [DONE]
// - Add 1 key jumping in cartridge list (press 'R', jumps to carts starting
//   with 'R', etc) [DONE]
// - Controller configuration [DONE]
//
// STILL TO BE DONE:
//
// - Fix bug in switching between PAL & NTSC in fullscreen mode.
// - Remove SDL dependencies (sound, mainly) from Jaguar core lib
// - Fix inconsistency with trailing slashes in paths (eeproms needs one,
//   software doesn't)
//
// SFDX CODE: S1E9T8H5M23YS

// Uncomment this for debugging...
//#define DEBUG
//#define DEBUGFOO			// Various tool debugging...
//#define DEBUGTP				// Toolpalette debugging...

#include "mainwin.h"

#include "SDL.h"
#include "app.h"
#include "about.h"
#include "configdialog.h"
#include "controllertab.h"
#include "filepicker.h"
#include "gamepad.h"
#include "generaltab.h"
#include "glwidget.h"
#include "help.h"
#include "profile.h"
#include "settings.h"
#include "version.h"
#include "debug/cpubrowser.h"
#include "debug/m68kdasmbrowser.h"
#include "debug/memorybrowser.h"
#include "debug/opbrowser.h"
#include "debug/riscdasmbrowser.h"

#include "dac.h"
#include "jaguar.h"
#include "log.h"
#include "file.h"
#include "jagbios.h"
#include "jagbios2.h"
#include "jagcdbios.h"
#include "jagstub2bios.h"
#include "joystick.h"
#include "m68000/m68kinterface.h"

// According to SebRmv, this header isn't seen on Arch Linux either... :-/
//#ifdef __GCCWIN32__
// Apparently on win32, usleep() is not pulled in by the usual suspects.
#include <unistd.h>
//#endif

// The way BSNES controls things is by setting a timer with a zero
// timeout, sleeping if not emulating anything. Seems there has to be a
// better way.

// It has a novel approach to plugging-in/using different video/audio/input
// methods, can we do something similar or should we just use the built-in
// QOpenGL?

// We're going to try to use the built-in OpenGL support and see how it goes.
// We'll make the VJ core modular so that it doesn't matter what GUI is in
// use, we can drop it in anywhere and use it as-is.

MainWin::MainWin(bool autoRun): running(true), powerButtonOn(false),
	showUntunedTankCircuit(true), cartridgeLoaded(false), CDActive(false),
	pauseForFileSelector(false), loadAndGo(autoRun), scannedSoftwareFolder(false), plzDontKillMyComputer(false)
{
	debugbar = NULL;

	for(int i=0; i<8; i++)
		keyHeld[i] = false;

	// FPS management
	for(int i=0; i<RING_BUFFER_SIZE; i++)
		ringBuffer[i] = 0;

	ringBufferPointer = RING_BUFFER_SIZE - 1;

	videoWidget = new GLWidget(this);
	setCentralWidget(videoWidget);
	setWindowIcon(QIcon(":/res/vj-icon.png"));

	QString title = QString(tr("Virtual Jaguar " VJ_RELEASE_VERSION ));

	if (vjs.hardwareTypeAlpine)
		title += QString(tr(" - Alpine Mode"));

	setWindowTitle(title);

	aboutWin = new AboutWindow(this);
	helpWin = new HelpWindow(this);
	filePickWin = new FilePickerWindow(this);
	memBrowseWin = new MemoryBrowserWindow(this);
	cpuBrowseWin = new CPUBrowserWindow(this);
	opBrowseWin = new OPBrowserWindow(this);
	m68kDasmBrowseWin = new M68KDasmBrowserWindow(this);
	riscDasmBrowseWin = new RISCDasmBrowserWindow(this);

	videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	setUnifiedTitleAndToolBarOnMac(true);

	// Create actions

	quitAppAct = new QAction(tr("E&xit"), this);
//	quitAppAct->setShortcuts(QKeySequence::Quit);
//	quitAppAct->setShortcut(QKeySequence(tr("Alt+x")));
	quitAppAct->setShortcut(QKeySequence(tr("Ctrl+q")));
	quitAppAct->setShortcutContext(Qt::ApplicationShortcut);
	quitAppAct->setStatusTip(tr("Quit Virtual Jaguar"));
	connect(quitAppAct, SIGNAL(triggered()), this, SLOT(close()));

	powerGreen.addFile(":/res/power-off.png", QSize(), QIcon::Normal, QIcon::Off);
	powerGreen.addFile(":/res/power-on-green.png", QSize(), QIcon::Normal, QIcon::On);
	powerRed.addFile(":/res/power-off.png", QSize(), QIcon::Normal, QIcon::Off);
	powerRed.addFile(":/res/power-on-red.png", QSize(), QIcon::Normal, QIcon::On);

//	powerAct = new QAction(QIcon(":/res/power.png"), tr("&Power"), this);
	powerAct = new QAction(powerGreen, tr("&Power"), this);
	powerAct->setStatusTip(tr("Powers Jaguar on/off"));
	powerAct->setCheckable(true);
	powerAct->setChecked(false);
//	powerAct->setDisabled(true);
	connect(powerAct, SIGNAL(triggered()), this, SLOT(TogglePowerState()));

	QIcon pauseIcon;
	pauseIcon.addFile(":/res/pause-off", QSize(), QIcon::Normal, QIcon::Off);
	pauseIcon.addFile(":/res/pause-on", QSize(), QIcon::Normal, QIcon::On);
//	pauseAct = new QAction(QIcon(":/res/pause.png"), tr("Pause"), this);
	pauseAct = new QAction(pauseIcon, tr("Pause"), this);
	pauseAct->setStatusTip(tr("Toggles the running state"));
	pauseAct->setCheckable(true);
	pauseAct->setDisabled(true);
	pauseAct->setShortcut(QKeySequence(tr("Esc")));
	pauseAct->setShortcutContext(Qt::ApplicationShortcut);
	connect(pauseAct, SIGNAL(triggered()), this, SLOT(ToggleRunState()));

	zoomActs = new QActionGroup(this);

	x1Act = new QAction(QIcon(":/res/zoom100.png"), tr("Zoom 100%"), zoomActs);
	x1Act->setStatusTip(tr("Set window zoom to 100%"));
	x1Act->setCheckable(true);
	connect(x1Act, SIGNAL(triggered()), this, SLOT(SetZoom100()));

	x2Act = new QAction(QIcon(":/res/zoom200.png"), tr("Zoom 200%"), zoomActs);
	x2Act->setStatusTip(tr("Set window zoom to 200%"));
	x2Act->setCheckable(true);
	connect(x2Act, SIGNAL(triggered()), this, SLOT(SetZoom200()));

	x3Act = new QAction(QIcon(":/res/zoom300.png"), tr("Zoom 300%"), zoomActs);
	x3Act->setStatusTip(tr("Set window zoom to 300%"));
	x3Act->setCheckable(true);
	connect(x3Act, SIGNAL(triggered()), this, SLOT(SetZoom300()));

	tvTypeActs = new QActionGroup(this);

	ntscAct = new QAction(QIcon(":/res/ntsc.png"), tr("NTSC"), tvTypeActs);
	ntscAct->setStatusTip(tr("Sets Jaguar to NTSC mode"));
	ntscAct->setCheckable(true);
	connect(ntscAct, SIGNAL(triggered()), this, SLOT(SetNTSC()));

	palAct = new QAction(QIcon(":/res/pal.png"), tr("PAL"), tvTypeActs);
	palAct->setStatusTip(tr("Sets Jaguar to PAL mode"));
	palAct->setCheckable(true);
	connect(palAct, SIGNAL(triggered()), this, SLOT(SetPAL()));

	blurAct = new QAction(QIcon(":/res/blur.png"), tr("Blur"), this);
	blurAct->setStatusTip(tr("Sets OpenGL rendering to GL_NEAREST"));
	blurAct->setCheckable(true);
	connect(blurAct, SIGNAL(triggered()), this, SLOT(ToggleBlur()));

	aboutAct = new QAction(QIcon(":/res/vj-icon.png"), tr("&About..."), this);
	aboutAct->setStatusTip(tr("Blatant self-promotion"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(ShowAboutWin()));

	helpAct = new QAction(QIcon(":/res/vj-icon.png"), tr("&Contents..."), this);
	helpAct->setStatusTip(tr("Help is available, if you should need it"));
	connect(helpAct, SIGNAL(triggered()), this, SLOT(ShowHelpWin()));

	filePickAct = new QAction(QIcon(":/res/software.png"), tr("&Insert Cartridge..."), this);
	filePickAct->setStatusTip(tr("Insert a cartridge into Virtual Jaguar"));
	filePickAct->setShortcut(QKeySequence(tr("Ctrl+i")));
	filePickAct->setShortcutContext(Qt::ApplicationShortcut);
	connect(filePickAct, SIGNAL(triggered()), this, SLOT(InsertCart()));

	configAct = new QAction(QIcon(":/res/wrench.png"), tr("&Configure"), this);
	configAct->setStatusTip(tr("Configure options for Virtual Jaguar"));
	configAct->setShortcut(QKeySequence(tr("Ctrl+c")));
	configAct->setShortcutContext(Qt::ApplicationShortcut);
	connect(configAct, SIGNAL(triggered()), this, SLOT(Configure()));

	useCDAct = new QAction(QIcon(":/res/compact-disc.png"), tr("&Use CD Unit"), this);
	useCDAct->setStatusTip(tr("Use Jaguar Virtual CD unit"));
//	useCDAct->setShortcut(QKeySequence(tr("Ctrl+c")));
	useCDAct->setCheckable(true);
	connect(useCDAct, SIGNAL(triggered()), this, SLOT(ToggleCDUsage()));

	frameAdvanceAct = new QAction(QIcon(":/res/frame-advance.png"), tr("&Frame Advance"), this);
	frameAdvanceAct->setShortcut(QKeySequence(tr("F7")));
	frameAdvanceAct->setShortcutContext(Qt::ApplicationShortcut);
	frameAdvanceAct->setDisabled(true);
	connect(frameAdvanceAct, SIGNAL(triggered()), this, SLOT(FrameAdvance()));

	fullScreenAct = new QAction(QIcon(":/res/fullscreen.png"), tr("F&ull Screen"), this);
	fullScreenAct->setShortcut(QKeySequence(tr("F9")));
	fullScreenAct->setShortcutContext(Qt::ApplicationShortcut);
	fullScreenAct->setCheckable(true);
	connect(fullScreenAct, SIGNAL(triggered()), this, SLOT(ToggleFullScreen()));

	// Debugger Actions
	memBrowseAct = new QAction(QIcon(":/res/tool-memory.png"), tr("Memory Browser"), this);
	memBrowseAct->setStatusTip(tr("Shows the Jaguar memory browser window"));
//	memBrowseAct->setCheckable(true);
	connect(memBrowseAct, SIGNAL(triggered()), this, SLOT(ShowMemoryBrowserWin()));

	cpuBrowseAct = new QAction(QIcon(":/res/tool-cpu.png"), tr("CPU Browser"), this);
	cpuBrowseAct->setStatusTip(tr("Shows the Jaguar CPU browser window"));
//	memBrowseAct->setCheckable(true);
	connect(cpuBrowseAct, SIGNAL(triggered()), this, SLOT(ShowCPUBrowserWin()));

	opBrowseAct = new QAction(QIcon(":/res/tool-op.png"), tr("OP Browser"), this);
	opBrowseAct->setStatusTip(tr("Shows the Jaguar OP browser window"));
//	memBrowseAct->setCheckable(true);
	connect(opBrowseAct, SIGNAL(triggered()), this, SLOT(ShowOPBrowserWin()));

	m68kDasmBrowseAct = new QAction(QIcon(":/res/tool-68k-dis.png"), tr("68K Listing Browser"), this);
	m68kDasmBrowseAct->setStatusTip(tr("Shows the 68K disassembly browser window"));
//	memBrowseAct->setCheckable(true);
	connect(m68kDasmBrowseAct, SIGNAL(triggered()), this, SLOT(ShowM68KDasmBrowserWin()));

	riscDasmBrowseAct = new QAction(QIcon(":/res/tool-risc-dis.png"), tr("RISC Listing Browser"), this);
	riscDasmBrowseAct->setStatusTip(tr("Shows the RISC disassembly browser window"));
//	memBrowseAct->setCheckable(true);
	connect(riscDasmBrowseAct, SIGNAL(triggered()), this, SLOT(ShowRISCDasmBrowserWin()));

	// Misc. connections...
	connect(filePickWin, SIGNAL(RequestLoad(QString)), this, SLOT(LoadSoftware(QString)));
	connect(filePickWin, SIGNAL(FilePickerHiding()), this, SLOT(Unpause()));

	// Create menus & toolbars

	fileMenu = menuBar()->addMenu(tr("&Jaguar"));
	fileMenu->addAction(powerAct);
	fileMenu->addAction(pauseAct);
//	fileMenu->addAction(frameAdvanceAct);
	fileMenu->addAction(filePickAct);
	fileMenu->addAction(useCDAct);
	fileMenu->addAction(configAct);
	fileMenu->addAction(quitAppAct);

	if (vjs.hardwareTypeAlpine)
	{
		debugMenu = menuBar()->addMenu(tr("&Debug"));
		debugMenu->addAction(memBrowseAct);
		debugMenu->addAction(cpuBrowseAct);
		debugMenu->addAction(opBrowseAct);
		debugMenu->addAction(m68kDasmBrowseAct);
		debugMenu->addAction(riscDasmBrowseAct);
	}

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(helpAct);
	helpMenu->addAction(aboutAct);

	toolbar = addToolBar(tr("Stuff"));
	toolbar->addAction(powerAct);
	toolbar->addAction(pauseAct);
	toolbar->addAction(frameAdvanceAct);
	toolbar->addAction(filePickAct);
	toolbar->addAction(useCDAct);
	toolbar->addSeparator();
	toolbar->addAction(x1Act);
	toolbar->addAction(x2Act);
	toolbar->addAction(x3Act);
	toolbar->addSeparator();
	toolbar->addAction(ntscAct);
	toolbar->addAction(palAct);
	toolbar->addSeparator();
	toolbar->addAction(blurAct);
	toolbar->addAction(fullScreenAct);

	if (vjs.hardwareTypeAlpine)
	{
		debugbar = addToolBar(tr("&Debug"));
		debugbar->addAction(memBrowseAct);
		debugbar->addAction(cpuBrowseAct);
		debugbar->addAction(opBrowseAct);
		debugbar->addAction(m68kDasmBrowseAct);
		debugbar->addAction(riscDasmBrowseAct);
	}

	// Add actions to the main window, as hiding widgets with them
	// disables them :-P
	addAction(fullScreenAct);
	addAction(quitAppAct);
	addAction(configAct);
	addAction(pauseAct);
	addAction(filePickAct);
	addAction(frameAdvanceAct);

	//	Create status bar
	statusBar()->showMessage(tr("Ready"));

	ReadSettings();

	// Do this in case original size isn't correct (mostly for the first-run case)
	ResizeMainWindow();

	// Create our test pattern bitmaps
	QImage tempImg(":/res/test-pattern.jpg");
	QImage tempImgScaled = tempImg.scaled(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT_PAL, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

	for(uint32_t y=0; y<VIRTUAL_SCREEN_HEIGHT_PAL; y++)
	{
		const QRgb * scanline = (QRgb *)tempImgScaled.constScanLine(y);

		for(uint32_t x=0; x<VIRTUAL_SCREEN_WIDTH; x++)
		{
			uint32_t pixel = (qRed(scanline[x]) << 24) | (qGreen(scanline[x]) << 16) | (qBlue(scanline[x]) << 8) | 0xFF;
			testPattern[(y * VIRTUAL_SCREEN_WIDTH) + x] = pixel;
		}
	}

	QImage tempImg2(":/res/test-pattern-pal");
	QImage tempImgScaled2 = tempImg2.scaled(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT_PAL, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

	for(uint32_t y=0; y<VIRTUAL_SCREEN_HEIGHT_PAL; y++)
	{
		const QRgb * scanline = (QRgb *)tempImgScaled2.constScanLine(y);

		for(uint32_t x=0; x<VIRTUAL_SCREEN_WIDTH; x++)
		{
			uint32_t pixel = (qRed(scanline[x]) << 24) | (qGreen(scanline[x]) << 16) | (qBlue(scanline[x]) << 8) | 0xFF;
			testPattern2[(y * VIRTUAL_SCREEN_WIDTH) + x] = pixel;
		}
	}

	// Set up timer based loop for animation...
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(Timer()));

	// This isn't very accurate for NTSC: This is early by 40 msec per frame.
	// This is because it's discarding the 0.6666... on the end of the fraction.
	// Alas, 6 doesn't divide cleanly into 10. :-P
//Should we defer this until SyncUI? Probably.
//No, it doesn't work, because it uses setInterval() instead of start()...
//	timer->start(vjs.hardwareTypeNTSC ? 16 : 20);

	// We set this initially, to make VJ behave somewhat as it would if no
	// cart were inserted and the BIOS was set as active...
	jaguarCartInserted = true;
	WriteLog("Virtual Jaguar %s (Last full build was on %s %s)\n", VJ_RELEASE_VERSION, __DATE__, __TIME__);
	WriteLog("VJ: Initializing jaguar subsystem...\n");
	JaguarInit();
//	memcpy(jagMemSpace + 0xE00000, jaguarBootROM, 0x20000);	// Use the stock BIOS
	memcpy(jagMemSpace + 0xE00000, (vjs.biosType == BT_K_SERIES ? jaguarBootROM : jaguarBootROM2), 0x20000);	// Use the stock BIOS

	// Prevent the file scanner from running if filename passed
	// in on the command line...
	if (autoRun)
		return;

	// Load up the default ROM if in Alpine mode:
	if (vjs.hardwareTypeAlpine)
	{
		bool romLoaded = JaguarLoadFile(vjs.alpineROMPath);

		// If regular load failed, try just a straight file load
		// (Dev only! I don't want people to start getting lazy with their releases again! :-P)
		if (!romLoaded)
			romLoaded = AlpineLoadFile(vjs.alpineROMPath);

		if (romLoaded)
			WriteLog("Alpine Mode: Successfully loaded file \"%s\".\n", vjs.alpineROMPath);
		else
			WriteLog("Alpine Mode: Unable to load file \"%s\"!\n", vjs.alpineROMPath);

		// Attempt to load/run the ABS file...
		LoadSoftware(vjs.absROMPath);
		memcpy(jagMemSpace + 0xE00000, jaguarDevBootROM2, 0x20000);	// Use the stub BIOS
		// Prevent the scanner from running...
		return;
	}

	// Run the scanner if nothing passed in and *not* Alpine mode...
	// NB: Really need to look into caching the info scanned in here...
	filePickWin->ScanSoftwareFolder(allowUnknownSoftware);
	scannedSoftwareFolder = true;
}


void MainWin::LoadFile(QString file)
{
	LoadSoftware(file);
}


void MainWin::SyncUI(void)
{
	// Set toolbar buttons/menus based on settings read in (sync the UI)...
	// (Really, this is to sync command line options passed in)
	blurAct->setChecked(vjs.glFilter);
	x1Act->setChecked(zoomLevel == 1);
	x2Act->setChecked(zoomLevel == 2);
	x3Act->setChecked(zoomLevel == 3);
//	running = powerAct->isChecked();
	ntscAct->setChecked(vjs.hardwareTypeNTSC);
	palAct->setChecked(!vjs.hardwareTypeNTSC);
	powerAct->setIcon(vjs.hardwareTypeNTSC ? powerRed : powerGreen);

	fullScreenAct->setChecked(vjs.fullscreen);
	fullScreen = vjs.fullscreen;
	SetFullScreen(fullScreen);

	// Reset the timer to be what was set in the command line (if any):
//	timer->setInterval(vjs.hardwareTypeNTSC ? 16 : 20);
	timer->start(vjs.hardwareTypeNTSC ? 16 : 20);
}


void MainWin::closeEvent(QCloseEvent * event)
{
	JaguarDone();
// This should only be done by the config dialog
//	WriteSettings();
	WriteUISettings();
	event->accept(); // ignore() if can't close for some reason
}


void MainWin::keyPressEvent(QKeyEvent * e)
{
#ifndef VJ_REMOVE_DEV_CODE
	// From jaguar.cpp
	extern bool startM68KTracing;
	// From joystick.cpp
	extern int blit_start_log;
	// From blitter.cpp
	extern bool startConciseBlitLogging;
#endif

	// We ignore the Alt key for now, since it causes problems with the GUI
	if (e->key() == Qt::Key_Alt)
	{
		e->accept();
		return;
	}
// Bar this shite from release versions kthxbai
#ifndef VJ_REMOVE_DEV_CODE
	else if (e->key() == Qt::Key_F11)
	{
		startM68KTracing = true;
		e->accept();
		return;
	}
	else if (e->key() == Qt::Key_F12)
	{
		blit_start_log = true;
		e->accept();
		return;
	}
	else if (e->key() == Qt::Key_F10)
	{
		startConciseBlitLogging = true;
		e->accept();
		return;
	}
#endif
	else if (e->key() == Qt::Key_F8)
	{
		// ggn: For extra NYAN pleasure...
		// ggn: There you go James :P
		// Shamus: Thanks for the patch! :-D
		WriteLog("    o  +           +        +\n");
		WriteLog("+        o     o       +        o\n");
		WriteLog("-_-_-_-_-_-_-_,------,      o \n");
		WriteLog("_-_-_-_-_-_-_-|   /\\_/\\  \n");
		WriteLog("-_-_-_-_-_-_-~|__( ^ .^)  +     +  \n");
		WriteLog("_-_-_-_-_-_-_-\"\"  \"\"      \n");
		WriteLog("+      o         o   +       o\n");
		WriteLog("    +         +\n");
		e->accept();
		return;
	}

/*
This is done now by a QAction...
	if (e->key() == Qt::Key_F9)
	{
		ToggleFullScreen();
		return;
	}
*/
	HandleKeys(e, true);
}


void MainWin::keyReleaseEvent(QKeyEvent * e)
{
	// We ignore the Alt key for now, since it causes problems with the GUI
	if (e->key() == Qt::Key_Alt)
	{
		e->accept();
		return;
	}

	HandleKeys(e, false);
}


void MainWin::HandleKeys(QKeyEvent * e, bool state)
{
	enum { P1LEFT = 0, P1RIGHT, P1UP, P1DOWN, P2LEFT, P2RIGHT, P2UP, P2DOWN };
	// We kill bad key combos here, before they can get to the emulator...
	// This also kills the illegal instruction problem that cropped up in
	// Rayman!

	// First, settle key states...
	if (e->key() == (int)vjs.p1KeyBindings[BUTTON_L])
		keyHeld[P1LEFT] = state;
	else if (e->key() == (int)vjs.p1KeyBindings[BUTTON_R])
		keyHeld[P1RIGHT] = state;
	else if (e->key() == (int)vjs.p1KeyBindings[BUTTON_U])
		keyHeld[P1UP] = state;
	else if (e->key() == (int)vjs.p1KeyBindings[BUTTON_D])
		keyHeld[P1DOWN] = state;
	else if (e->key() == (int)vjs.p2KeyBindings[BUTTON_L])
		keyHeld[P2LEFT] = state;
	else if (e->key() == (int)vjs.p2KeyBindings[BUTTON_R])
		keyHeld[P2RIGHT] = state;
	else if (e->key() == (int)vjs.p2KeyBindings[BUTTON_U])
		keyHeld[P2UP] = state;
	else if (e->key() == (int)vjs.p2KeyBindings[BUTTON_D])
		keyHeld[P2DOWN] = state;

	// Next, check for conflicts and kill 'em if there are any...
	if (keyHeld[P1LEFT] && keyHeld[P1RIGHT])
		keyHeld[P1LEFT] = keyHeld[P1RIGHT] = false;

	if (keyHeld[P1UP] && keyHeld[P1DOWN])
		keyHeld[P1UP] = keyHeld[P1DOWN] = false;

	if (keyHeld[P2LEFT] && keyHeld[P2RIGHT])
		keyHeld[P2LEFT] = keyHeld[P2RIGHT] = false;

	if (keyHeld[P2UP] && keyHeld[P2DOWN])
		keyHeld[P2UP] = keyHeld[P2DOWN] = false;

	// No bad combos exist now, let's stuff the emulator key buffers...!
	for(int i=BUTTON_FIRST; i<=BUTTON_LAST; i++)
	{
		if (e->key() == (int)vjs.p1KeyBindings[i])
			joypad0Buttons[i] = (state ? 0x01 : 0x00);

		if (e->key() == (int)vjs.p2KeyBindings[i])
			joypad1Buttons[i] = (state ? 0x01 : 0x00);
	}
}


//
// N.B.: The profile system AutoConnect functionality sets the gamepad IDs here.
//
void MainWin::HandleGamepads(void)
{
	Gamepad::Update();

	for(int i=BUTTON_FIRST; i<=BUTTON_LAST; i++)
	{
		if (vjs.p1KeyBindings[i] & (JOY_BUTTON | JOY_HAT | JOY_AXIS))
			joypad0Buttons[i] = (Gamepad::GetState(gamepadIDSlot1, vjs.p1KeyBindings[i]) ? 0x01 : 0x00);

		if (vjs.p2KeyBindings[i] & (JOY_BUTTON | JOY_HAT | JOY_AXIS))
			joypad1Buttons[i] = (Gamepad::GetState(gamepadIDSlot2, vjs.p2KeyBindings[i]) ? 0x01 : 0x00);
	}
}


void MainWin::Open(void)
{
}


void MainWin::Configure(void)
{
	// Call the configuration dialog and update settings
	ConfigDialog dlg(this);
	//ick.
	dlg.generalTab->useUnknownSoftware->setChecked(allowUnknownSoftware);
	dlg.controllerTab1->profileNum = lastEditedProfile;
	dlg.controllerTab1->SetupLastUsedProfile();
// maybe instead of this, we tell the controller tab to work on a copy that gets
// written if the user hits 'OK'.
	SaveProfiles();		// Just in case user cancels

	if (dlg.exec() == false)
	{
		RestoreProfiles();
		return;
	}

	QString before = vjs.ROMPath;
	QString alpineBefore = vjs.alpineROMPath;
	QString absBefore = vjs.absROMPath;
//	bool audioBefore = vjs.audioEnabled;
	bool audioBefore = vjs.DSPEnabled;
	dlg.UpdateVJSettings();
	QString after = vjs.ROMPath;
	QString alpineAfter = vjs.alpineROMPath;
	QString absAfter = vjs.absROMPath;
//	bool audioAfter = vjs.audioEnabled;
	bool audioAfter = vjs.DSPEnabled;

	bool allowOld = allowUnknownSoftware;
	//ick.
	allowUnknownSoftware = dlg.generalTab->useUnknownSoftware->isChecked();
	lastEditedProfile = dlg.controllerTab1->profileNum;
	AutoConnectProfiles();

	// We rescan the "software" folder if the user either changed the path or
	// checked/unchecked the "Allow unknown files" option in the config dialog.
	if ((before != after) || (allowOld != allowUnknownSoftware))
		filePickWin->ScanSoftwareFolder(allowUnknownSoftware);

	// If the "Alpine" ROM is changed, then let's load it...
	if (alpineBefore != alpineAfter)
	{
		if (!JaguarLoadFile(vjs.alpineROMPath) && !AlpineLoadFile(vjs.alpineROMPath))
		{
			// Oh crap, we couldn't get the file! Alert the media!
			QMessageBox msg;
			msg.setText(QString(tr("Could not load file \"%1\"!")).arg(vjs.alpineROMPath));
			msg.setIcon(QMessageBox::Warning);
			msg.exec();
		}
	}

	// If the "ABS" ROM is changed, then let's load it...
	if (absBefore != absAfter)
	{
		if (!JaguarLoadFile(vjs.absROMPath))
		{
			// Oh crap, we couldn't get the file! Alert the media!
			QMessageBox msg;
			msg.setText(QString(tr("Could not load file \"%1\"!")).arg(vjs.absROMPath));
			msg.setIcon(QMessageBox::Warning);
			msg.exec();
		}
	}

	// If the "Enable DSP" checkbox changed, then we have to re-init the DAC,
	// since it's running in the host audio IRQ...
	if (audioBefore != audioAfter)
	{
		DACDone();
		DACInit();
	}

	// Just in case we crash before a clean exit...
	WriteSettings();
}


//
// Here's the main emulator loop
//
void MainWin::Timer(void)
{
#if 0
static uint32_t ntscTickCount;
	if (vjs.hardwareTypeNTSC)
	{
		ntscTickCount++;
		ntscTickCount %= 3;
		timer->start(16 + (ntscTickCount == 0 ? 1 : 0));
	}
#endif

	if (!running)
		return;

	if (showUntunedTankCircuit)
	{
		// Some machines can't handle this, so we give them the option to disable it. :-)
		if (!plzDontKillMyComputer)
		{
			// Random hash & trash
			// We try to simulate an untuned tank circuit here... :-)
			for(uint32_t x=0; x<videoWidget->rasterWidth; x++)
			{
				for(uint32_t y=0; y<videoWidget->rasterHeight; y++)
				{
					videoWidget->buffer[(y * videoWidget->textureWidth) + x]
						= (rand() & 0xFF) << 8 | (rand() & 0xFF) << 16 | (rand() & 0xFF) << 24;
				}
			}
		}
	}
	else
	{
		// Otherwise, run the Jaguar simulation
		HandleGamepads();
		JaguarExecuteNew();
		videoWidget->HandleMouseHiding();
	}

	videoWidget->updateGL();

	// FPS handling
	// Approach: We use a ring buffer to store times (in ms) over a given
	// amount of frames, then sum them to figure out the FPS.
	uint32_t timestamp = SDL_GetTicks();
	// This assumes the ring buffer size is a power of 2
//	ringBufferPointer = (ringBufferPointer + 1) & (RING_BUFFER_SIZE - 1);
	// Doing it this way is better. Ring buffer size can be arbitrary then.
	ringBufferPointer = (ringBufferPointer + 1) % RING_BUFFER_SIZE;
	ringBuffer[ringBufferPointer] = timestamp - oldTimestamp;
	uint32_t elapsedTime = 0;

	for(uint32_t i=0; i<RING_BUFFER_SIZE; i++)
		elapsedTime += ringBuffer[i];

	// elapsedTime must be non-zero
	if (elapsedTime == 0)
		elapsedTime = 1;

	// This is in frames per 10 seconds, so we can have 1 decimal
	uint32_t framesPerSecond = (uint32_t)(((float)RING_BUFFER_SIZE / (float)elapsedTime) * 10000.0);
	uint32_t fpsIntegerPart = framesPerSecond / 10;
	uint32_t fpsDecimalPart = framesPerSecond % 10;
	// If this is updated too frequently to be useful, we can throttle it down
	// so that it only updates every 10th frame or so
	statusBar()->showMessage(QString("%1.%2 FPS").arg(fpsIntegerPart).arg(fpsDecimalPart));
	oldTimestamp = timestamp;
}


void MainWin::TogglePowerState(void)
{
	powerButtonOn = !powerButtonOn;
	running = true;

	// With the power off, we simulate white noise on the screen. :-)
	if (!powerButtonOn)
	{
		// Restore the mouse pointer, if hidden:
		videoWidget->CheckAndRestoreMouseCursor();
		useCDAct->setDisabled(false);
		palAct->setDisabled(false);
		ntscAct->setDisabled(false);
		pauseAct->setChecked(false);
		pauseAct->setDisabled(true);
		showUntunedTankCircuit = true;
		DACPauseAudioThread();
		// This is just in case the ROM we were playing was in a narrow or wide
		// field mode, so the untuned tank sim doesn't look wrong. :-)
		TOMReset();

		if (plzDontKillMyComputer)
		{
			// We have to do it line by line, because the texture pitch is not
			// the same as the picture buffer's pitch.
			for(uint32_t y=0; y<videoWidget->rasterHeight; y++)
			{
				if (vjs.hardwareTypeNTSC)
					memcpy(videoWidget->buffer + (y * videoWidget->textureWidth), testPattern + (y * VIRTUAL_SCREEN_WIDTH), VIRTUAL_SCREEN_WIDTH * sizeof(uint32_t));
				else
					memcpy(videoWidget->buffer + (y * videoWidget->textureWidth), testPattern2 + (y * VIRTUAL_SCREEN_WIDTH), VIRTUAL_SCREEN_WIDTH * sizeof(uint32_t));
			}
		}
	}
	else
	{
		useCDAct->setDisabled(true);
		palAct->setDisabled(true);
		ntscAct->setDisabled(true);
		pauseAct->setChecked(false);
		pauseAct->setDisabled(false);
		showUntunedTankCircuit = false;

		// Otherwise, we prepare for running regular software...
		if (CDActive)
		{
// Should check for cartridgeLoaded here as well...!
// We can clear it when toggling CDActive on, so that when we power cycle it
// does the expected thing. Otherwise, if we use the file picker to insert a
// cart, we expect to run the cart! Maybe have a RemoveCart function that only
// works if the CD unit is active?
			setWindowTitle(QString("Virtual Jaguar " VJ_RELEASE_VERSION
				" - Now playing: Jaguar CD"));
		}

		WriteLog("GUI: Resetting Jaguar...\n");
		JaguarReset();
		DACPauseAudioThread(false);
	}
}


void MainWin::ToggleRunState(void)
{
	running = !running;

	if (!running)
	{
		// Restore the mouse pointer, if hidden:
		videoWidget->CheckAndRestoreMouseCursor();
		frameAdvanceAct->setDisabled(false);

		for(uint32_t i=0; i<(uint32_t)(videoWidget->textureWidth * 256); i++)
		{
			uint32_t pixel = videoWidget->buffer[i];
			uint8_t r = (pixel >> 24) & 0xFF, g = (pixel >> 16) & 0xFF, b = (pixel >> 8) & 0xFF;
			pixel = ((r + g + b) / 3) & 0x00FF;
			videoWidget->buffer[i] = 0x000000FF | (pixel << 16) | (pixel << 8);
		}

		videoWidget->updateGL();
	}
	else
		frameAdvanceAct->setDisabled(true);

	// Pause/unpause any running/non-running threads...
	DACPauseAudioThread(!running);
}


void MainWin::SetZoom100(void)
{
	zoomLevel = 1;
	ResizeMainWindow();
}


void MainWin::SetZoom200(void)
{
	zoomLevel = 2;
	ResizeMainWindow();
}


void MainWin::SetZoom300(void)
{
	zoomLevel = 3;
	ResizeMainWindow();
}


void MainWin::SetNTSC(void)
{
	powerAct->setIcon(powerRed);
	timer->setInterval(16);
	vjs.hardwareTypeNTSC = true;
	ResizeMainWindow();
	WriteSettings();
}


void MainWin::SetPAL(void)
{
	powerAct->setIcon(powerGreen);
	timer->setInterval(20);
	vjs.hardwareTypeNTSC = false;
	ResizeMainWindow();
	WriteSettings();
}


void MainWin::ToggleBlur(void)
{
	vjs.glFilter = !vjs.glFilter;
	WriteSettings();
}


void MainWin::ShowAboutWin(void)
{
	aboutWin->show();
}


void MainWin::ShowHelpWin(void)
{
	helpWin->show();
}


void MainWin::InsertCart(void)
{
	// Check to see if we did autorun, 'cause we didn't load anything in that
	// case
	if (!scannedSoftwareFolder)
	{
		filePickWin->ScanSoftwareFolder(allowUnknownSoftware);
		scannedSoftwareFolder = true;
	}

	// If the emulator is running, we pause it here and unpause it later
	// if we dismiss the file selector without choosing anything
	if (running && powerButtonOn)
	{
		ToggleRunState();
		pauseForFileSelector = true;
	}

	filePickWin->show();
}


void MainWin::Unpause(void)
{
	// Here we unpause the emulator if it was paused when we went into the file selector
	if (pauseForFileSelector)
	{
		pauseForFileSelector = false;

		// Some nutter might have unpaused while in the file selector, so check for that
		if (!running)
			ToggleRunState();
	}
}


void MainWin::LoadSoftware(QString file)
{
	running = false;							// Prevent bad things(TM) from happening...
	pauseForFileSelector = false;				// Reset the file selector pause flag

	char * biosPointer = jaguarBootROM;

	if (vjs.hardwareTypeAlpine)
		biosPointer = jaguarDevBootROM2;

	memcpy(jagMemSpace + 0xE00000, biosPointer, 0x20000);

	powerAct->setDisabled(false);
	powerAct->setChecked(true);
	powerButtonOn = false;
	TogglePowerState();
	// We have to load our software *after* the Jaguar RESET
	cartridgeLoaded = JaguarLoadFile(file.toAscii().data());
	SET32(jaguarMainRAM, 0, 0x00200000);		// Set top of stack...

	// This is icky because we've already done it
// it gets worse :-P
if (!vjs.useJaguarBIOS)
	SET32(jaguarMainRAM, 4, jaguarRunAddress);

	m68k_pulse_reset();

	if (!vjs.hardwareTypeAlpine && !loadAndGo)
	{
		QString newTitle = QString("Virtual Jaguar " VJ_RELEASE_VERSION " - Now playing: %1")
			.arg(filePickWin->GetSelectedPrettyName());
		setWindowTitle(newTitle);
	}
}


void MainWin::ToggleCDUsage(void)
{
	CDActive = !CDActive;

	// Set up the Jaguar CD for execution, otherwise, clear memory
	if (CDActive)
		memcpy(jagMemSpace + 0x800000, jaguarCDBootROM, 0x40000);
	else
		memset(jagMemSpace + 0x800000, 0xFF, 0x40000);
}


void MainWin::FrameAdvance(void)
{
//printf("Frame Advance...\n");
	// Execute 1 frame, then exit (only useful in Pause mode)
	JaguarExecuteNew();
	videoWidget->updateGL();
	// Need to execute 1 frames' worth of DSP thread as well :-/
#warning "!!! Need to execute the DSP thread for 1 frame too !!!"
}


void MainWin::SetFullScreen(bool state/*= true*/)
{
	if (state)
	{
		mainWinPosition = pos();
		menuBar()->hide();
		statusBar()->hide();
		toolbar->hide();

		if (debugbar)
			debugbar->hide();

		showFullScreen();
		// This is needed because the fullscreen may happen on a different
		// screen than screen 0:
		int screenNum = QApplication::desktop()->screenNumber(videoWidget);
//		QRect r = QApplication::desktop()->availableGeometry(screenNum);
		QRect r = QApplication::desktop()->screenGeometry(screenNum);
		double targetWidth = (double)VIRTUAL_SCREEN_WIDTH,
			targetHeight = (double)(vjs.hardwareTypeNTSC ? VIRTUAL_SCREEN_HEIGHT_NTSC : VIRTUAL_SCREEN_HEIGHT_PAL);
		double aspectRatio = targetWidth / targetHeight;
		// NOTE: Really should check here to see which dimension constrains the
		//       other. Right now, we assume that height is the constraint.
		int newWidth = (int)(aspectRatio * (double)r.height());
		videoWidget->offset = (r.width() - newWidth) / 2;
		videoWidget->fullscreen = true;
		videoWidget->outputWidth = newWidth;
		videoWidget->setFixedSize(r.width(), r.height());
		showFullScreen();
	}
	else
	{
		// Reset the video widget to windowed mode
		videoWidget->offset = 0;
		videoWidget->fullscreen = false;
		menuBar()->show();
		statusBar()->show();
		toolbar->show();

		if (debugbar)
			debugbar->show();

		showNormal();
		ResizeMainWindow();
		move(mainWinPosition);
	}
}


void MainWin::ToggleFullScreen(void)
{
	fullScreen = !fullScreen;
	SetFullScreen(fullScreen);
}


void MainWin::ShowMemoryBrowserWin(void)
{
	memBrowseWin->show();
	memBrowseWin->RefreshContents();
}


void MainWin::ShowCPUBrowserWin(void)
{
	cpuBrowseWin->show();
	cpuBrowseWin->RefreshContents();
}


void MainWin::ShowOPBrowserWin(void)
{
	opBrowseWin->show();
	opBrowseWin->RefreshContents();
}


void MainWin::ShowM68KDasmBrowserWin(void)
{
	m68kDasmBrowseWin->show();
	m68kDasmBrowseWin->RefreshContents();
}


void MainWin::ShowRISCDasmBrowserWin(void)
{
	riscDasmBrowseWin->show();
	riscDasmBrowseWin->RefreshContents();
}


void MainWin::ResizeMainWindow(void)
{
	videoWidget->setFixedSize(zoomLevel * VIRTUAL_SCREEN_WIDTH,
		zoomLevel * (vjs.hardwareTypeNTSC ? VIRTUAL_SCREEN_HEIGHT_NTSC : VIRTUAL_SCREEN_HEIGHT_PAL));

	// Show the test pattern if user requested plzDontKillMyComputer mode
	if (!powerButtonOn && plzDontKillMyComputer)
	{
		for(uint32_t y=0; y<videoWidget->rasterHeight; y++)
		{
			if (vjs.hardwareTypeNTSC)
				memcpy(videoWidget->buffer + (y * videoWidget->textureWidth), testPattern + (y * VIRTUAL_SCREEN_WIDTH), VIRTUAL_SCREEN_WIDTH * sizeof(uint32_t));
			else
				memcpy(videoWidget->buffer + (y * videoWidget->textureWidth), testPattern2 + (y * VIRTUAL_SCREEN_WIDTH), VIRTUAL_SCREEN_WIDTH * sizeof(uint32_t));
		}
	}

	show();

	for(int i=0; i<2; i++)
	{
		resize(0, 0);
		usleep(2000);
		QApplication::processEvents();
	}
}


#warning "!!! Need to check the window geometry to see if the positions are legal !!!"
// i.e., someone could drag it to another screen, close it, then disconnect that screen
void MainWin::ReadSettings(void)
{
	QSettings settings("Underground Software", "Virtual Jaguar");
	mainWinPosition = settings.value("pos", QPoint(200, 200)).toPoint();
	QSize size = settings.value("size", QSize(400, 400)).toSize();
	resize(size);
	move(mainWinPosition);
	QPoint pos = settings.value("cartLoadPos", QPoint(200, 200)).toPoint();
	filePickWin->move(pos);

	zoomLevel = settings.value("zoom", 2).toInt();
	allowUnknownSoftware = settings.value("showUnknownSoftware", false).toBool();
	lastEditedProfile = settings.value("lastEditedProfile", 0).toInt();

	vjs.useJoystick      = settings.value("useJoystick", false).toBool();
	vjs.joyport          = settings.value("joyport", 0).toInt();
	vjs.hardwareTypeNTSC = settings.value("hardwareTypeNTSC", true).toBool();
	vjs.frameSkip        = settings.value("frameSkip", 0).toInt();
	vjs.useJaguarBIOS    = settings.value("useJaguarBIOS", false).toBool();
	vjs.GPUEnabled       = settings.value("GPUEnabled", true).toBool();
	vjs.DSPEnabled       = settings.value("DSPEnabled", true).toBool();
	vjs.audioEnabled     = settings.value("audioEnabled", true).toBool();
	vjs.usePipelinedDSP  = settings.value("usePipelinedDSP", false).toBool();
	vjs.fullscreen       = settings.value("fullscreen", false).toBool();
	vjs.useOpenGL        = settings.value("useOpenGL", true).toBool();
	vjs.glFilter         = settings.value("glFilterType", 1).toInt();
	vjs.renderType       = settings.value("renderType", 0).toInt();
	vjs.allowWritesToROM = settings.value("writeROM", false).toBool();
	vjs.biosType         = settings.value("biosType", BT_M_SERIES).toInt();
	vjs.useFastBlitter   = settings.value("useFastBlitter", false).toBool();
	strcpy(vjs.EEPROMPath, settings.value("EEPROMs", QDesktopServices::storageLocation(QDesktopServices::DataLocation).append("/eeproms/")).toString().toAscii().data());
	strcpy(vjs.ROMPath, settings.value("ROMs", QDesktopServices::storageLocation(QDesktopServices::DataLocation).append("/software/")).toString().toAscii().data());
	strcpy(vjs.alpineROMPath, settings.value("DefaultROM", "").toString().toAscii().data());
	strcpy(vjs.absROMPath, settings.value("DefaultABS", "").toString().toAscii().data());

WriteLog("MainWin: Paths\n");
WriteLog("   EEPROMPath = \"%s\"\n", vjs.EEPROMPath);
WriteLog("      ROMPath = \"%s\"\n", vjs.ROMPath);
WriteLog("AlpineROMPath = \"%s\"\n", vjs.alpineROMPath);
WriteLog("   absROMPath = \"%s\"\n", vjs.absROMPath);
WriteLog("Pipelined DSP = %s\n", (vjs.usePipelinedDSP ? "ON" : "off"));

	// Keybindings in order of U, D, L, R, C, B, A, Op, Pa, 0-9, #, *
	vjs.p1KeyBindings[BUTTON_U] = settings.value("p1k_up", Qt::Key_S).toInt();
	vjs.p1KeyBindings[BUTTON_D] = settings.value("p1k_down", Qt::Key_X).toInt();
	vjs.p1KeyBindings[BUTTON_L] = settings.value("p1k_left", Qt::Key_A).toInt();
	vjs.p1KeyBindings[BUTTON_R] = settings.value("p1k_right", Qt::Key_D).toInt();
	vjs.p1KeyBindings[BUTTON_C] = settings.value("p1k_c", Qt::Key_J).toInt();
	vjs.p1KeyBindings[BUTTON_B] = settings.value("p1k_b", Qt::Key_K).toInt();
	vjs.p1KeyBindings[BUTTON_A] = settings.value("p1k_a", Qt::Key_L).toInt();
	vjs.p1KeyBindings[BUTTON_OPTION] = settings.value("p1k_option", Qt::Key_O).toInt();
	vjs.p1KeyBindings[BUTTON_PAUSE] = settings.value("p1k_pause", Qt::Key_P).toInt();
	vjs.p1KeyBindings[BUTTON_0] = settings.value("p1k_0", Qt::Key_0).toInt();
	vjs.p1KeyBindings[BUTTON_1] = settings.value("p1k_1", Qt::Key_1).toInt();
	vjs.p1KeyBindings[BUTTON_2] = settings.value("p1k_2", Qt::Key_2).toInt();
	vjs.p1KeyBindings[BUTTON_3] = settings.value("p1k_3", Qt::Key_3).toInt();
	vjs.p1KeyBindings[BUTTON_4] = settings.value("p1k_4", Qt::Key_4).toInt();
	vjs.p1KeyBindings[BUTTON_5] = settings.value("p1k_5", Qt::Key_5).toInt();
	vjs.p1KeyBindings[BUTTON_6] = settings.value("p1k_6", Qt::Key_6).toInt();
	vjs.p1KeyBindings[BUTTON_7] = settings.value("p1k_7", Qt::Key_7).toInt();
	vjs.p1KeyBindings[BUTTON_8] = settings.value("p1k_8", Qt::Key_8).toInt();
	vjs.p1KeyBindings[BUTTON_9] = settings.value("p1k_9", Qt::Key_9).toInt();
	vjs.p1KeyBindings[BUTTON_d] = settings.value("p1k_pound", Qt::Key_Minus).toInt();
	vjs.p1KeyBindings[BUTTON_s] = settings.value("p1k_star", Qt::Key_Equal).toInt();

	vjs.p2KeyBindings[BUTTON_U] = settings.value("p2k_up", Qt::Key_Up).toInt();
	vjs.p2KeyBindings[BUTTON_D] = settings.value("p2k_down", Qt::Key_Down).toInt();
	vjs.p2KeyBindings[BUTTON_L] = settings.value("p2k_left", Qt::Key_Left).toInt();
	vjs.p2KeyBindings[BUTTON_R] = settings.value("p2k_right", Qt::Key_Right).toInt();
	vjs.p2KeyBindings[BUTTON_C] = settings.value("p2k_c", Qt::Key_Z).toInt();
	vjs.p2KeyBindings[BUTTON_B] = settings.value("p2k_b", Qt::Key_X).toInt();
	vjs.p2KeyBindings[BUTTON_A] = settings.value("p2k_a", Qt::Key_C).toInt();
	vjs.p2KeyBindings[BUTTON_OPTION] = settings.value("p2k_option", Qt::Key_Apostrophe).toInt();
	vjs.p2KeyBindings[BUTTON_PAUSE] = settings.value("p2k_pause", Qt::Key_Return).toInt();
	vjs.p2KeyBindings[BUTTON_0] = settings.value("p2k_0", Qt::Key_0).toInt();
	vjs.p2KeyBindings[BUTTON_1] = settings.value("p2k_1", Qt::Key_1).toInt();
	vjs.p2KeyBindings[BUTTON_2] = settings.value("p2k_2", Qt::Key_2).toInt();
	vjs.p2KeyBindings[BUTTON_3] = settings.value("p2k_3", Qt::Key_3).toInt();
	vjs.p2KeyBindings[BUTTON_4] = settings.value("p2k_4", Qt::Key_4).toInt();
	vjs.p2KeyBindings[BUTTON_5] = settings.value("p2k_5", Qt::Key_5).toInt();
	vjs.p2KeyBindings[BUTTON_6] = settings.value("p2k_6", Qt::Key_6).toInt();
	vjs.p2KeyBindings[BUTTON_7] = settings.value("p2k_7", Qt::Key_7).toInt();
	vjs.p2KeyBindings[BUTTON_8] = settings.value("p2k_8", Qt::Key_8).toInt();
	vjs.p2KeyBindings[BUTTON_9] = settings.value("p2k_9", Qt::Key_9).toInt();
	vjs.p2KeyBindings[BUTTON_d] = settings.value("p2k_pound", Qt::Key_Slash).toInt();
	vjs.p2KeyBindings[BUTTON_s] = settings.value("p2k_star", Qt::Key_Asterisk).toInt();

	ReadProfiles(&settings);
}

  
void MainWin::WriteSettings(void)
{
	QSettings settings("Underground Software", "Virtual Jaguar");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
	settings.setValue("cartLoadPos", filePickWin->pos());

	settings.setValue("zoom", zoomLevel);
	settings.setValue("showUnknownSoftware", allowUnknownSoftware);
	settings.setValue("lastEditedProfile", lastEditedProfile);

	settings.setValue("useJoystick", vjs.useJoystick);
	settings.setValue("joyport", vjs.joyport);
	settings.setValue("hardwareTypeNTSC", vjs.hardwareTypeNTSC);
	settings.setValue("frameSkip", vjs.frameSkip);
	settings.setValue("useJaguarBIOS", vjs.useJaguarBIOS);
	settings.setValue("GPUEnabled", vjs.GPUEnabled);
	settings.setValue("DSPEnabled", vjs.DSPEnabled);
	settings.setValue("audioEnabled", vjs.audioEnabled);
	settings.setValue("usePipelinedDSP", vjs.usePipelinedDSP);
	settings.setValue("fullscreen", vjs.fullscreen);
	settings.setValue("useOpenGL", vjs.useOpenGL);
	settings.setValue("glFilterType", vjs.glFilter);
	settings.setValue("renderType", vjs.renderType);
	settings.setValue("writeROM", vjs.allowWritesToROM);
	settings.setValue("biosType", vjs.biosType);
	settings.setValue("useFastBlitter", vjs.useFastBlitter);
	settings.setValue("JagBootROM", vjs.jagBootPath);
	settings.setValue("CDBootROM", vjs.CDBootPath);
	settings.setValue("EEPROMs", vjs.EEPROMPath);
	settings.setValue("ROMs", vjs.ROMPath);
	settings.setValue("DefaultROM", vjs.alpineROMPath);
	settings.setValue("DefaultABS", vjs.absROMPath);

	settings.setValue("p1k_up", vjs.p1KeyBindings[BUTTON_U]);
	settings.setValue("p1k_down", vjs.p1KeyBindings[BUTTON_D]);
	settings.setValue("p1k_left", vjs.p1KeyBindings[BUTTON_L]);
	settings.setValue("p1k_right", vjs.p1KeyBindings[BUTTON_R]);
	settings.setValue("p1k_c", vjs.p1KeyBindings[BUTTON_C]);
	settings.setValue("p1k_b", vjs.p1KeyBindings[BUTTON_B]);
	settings.setValue("p1k_a", vjs.p1KeyBindings[BUTTON_A]);
	settings.setValue("p1k_option", vjs.p1KeyBindings[BUTTON_OPTION]);
	settings.setValue("p1k_pause", vjs.p1KeyBindings[BUTTON_PAUSE]);
	settings.setValue("p1k_0", vjs.p1KeyBindings[BUTTON_0]);
	settings.setValue("p1k_1", vjs.p1KeyBindings[BUTTON_1]);
	settings.setValue("p1k_2", vjs.p1KeyBindings[BUTTON_2]);
	settings.setValue("p1k_3", vjs.p1KeyBindings[BUTTON_3]);
	settings.setValue("p1k_4", vjs.p1KeyBindings[BUTTON_4]);
	settings.setValue("p1k_5", vjs.p1KeyBindings[BUTTON_5]);
	settings.setValue("p1k_6", vjs.p1KeyBindings[BUTTON_6]);
	settings.setValue("p1k_7", vjs.p1KeyBindings[BUTTON_7]);
	settings.setValue("p1k_8", vjs.p1KeyBindings[BUTTON_8]);
	settings.setValue("p1k_9", vjs.p1KeyBindings[BUTTON_9]);
	settings.setValue("p1k_pound", vjs.p1KeyBindings[BUTTON_d]);
	settings.setValue("p1k_star", vjs.p1KeyBindings[BUTTON_s]);

	settings.setValue("p2k_up", vjs.p2KeyBindings[BUTTON_U]);
	settings.setValue("p2k_down", vjs.p2KeyBindings[BUTTON_D]);
	settings.setValue("p2k_left", vjs.p2KeyBindings[BUTTON_L]);
	settings.setValue("p2k_right", vjs.p2KeyBindings[BUTTON_R]);
	settings.setValue("p2k_c", vjs.p2KeyBindings[BUTTON_C]);
	settings.setValue("p2k_b", vjs.p2KeyBindings[BUTTON_B]);
	settings.setValue("p2k_a", vjs.p2KeyBindings[BUTTON_A]);
	settings.setValue("p2k_option", vjs.p2KeyBindings[BUTTON_OPTION]);
	settings.setValue("p2k_pause", vjs.p2KeyBindings[BUTTON_PAUSE]);
	settings.setValue("p2k_0", vjs.p2KeyBindings[BUTTON_0]);
	settings.setValue("p2k_1", vjs.p2KeyBindings[BUTTON_1]);
	settings.setValue("p2k_2", vjs.p2KeyBindings[BUTTON_2]);
	settings.setValue("p2k_3", vjs.p2KeyBindings[BUTTON_3]);
	settings.setValue("p2k_4", vjs.p2KeyBindings[BUTTON_4]);
	settings.setValue("p2k_5", vjs.p2KeyBindings[BUTTON_5]);
	settings.setValue("p2k_6", vjs.p2KeyBindings[BUTTON_6]);
	settings.setValue("p2k_7", vjs.p2KeyBindings[BUTTON_7]);
	settings.setValue("p2k_8", vjs.p2KeyBindings[BUTTON_8]);
	settings.setValue("p2k_9", vjs.p2KeyBindings[BUTTON_9]);
	settings.setValue("p2k_pound", vjs.p2KeyBindings[BUTTON_d]);
	settings.setValue("p2k_star", vjs.p2KeyBindings[BUTTON_s]);

	WriteProfiles(&settings);
}


void MainWin::WriteUISettings(void)
{
	QSettings settings("Underground Software", "Virtual Jaguar");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
	settings.setValue("cartLoadPos", filePickWin->pos());

	settings.setValue("zoom", zoomLevel);
}

