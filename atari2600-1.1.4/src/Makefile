#
# Atari 2600 / Stella port on PSP 
#
#
TARGET = "psp2600"
PSPSDK = $(shell psp-config --pspsdk-path)
PSPBIN = $(shell psp-config --psp-prefix)/bin
SDL_CONFIG = $(PSPBIN)/sdl-config

PSP_OBJS = \
psp_main.o \
psp_sdl.o \
psp_gu.o \
psp_font.o \
psp_battery.o \
psp_kbd.o \
psp_menu.o \
psp_danzeff.o \
Atari.o  \
psp_menu_set.o \
psp_menu_help.o \
psp_menu_kbd.o \
psp_irkeyb.o \
psp_fmgr.o

EMU_OBJS= \
emucore/Joystick.o \
emucore/TIA.o \
emucore/Settings.o \
emucore/CartE0.o \
emucore/Paddles.o \
emucore/CartF8.o \
emucore/Props.o \
emucore/Switches.o \
emucore/Event.o \
emucore/Random.o \
emucore/Device.o \
emucore/Driving.o \
emucore/Cart2K.o \
emucore/Keyboard.o \
emucore/Console.o \
emucore/CartF6.o \
emucore/M6532.o \
emucore/PropsSet.o \
emucore/CartUA.o \
emucore/CartMC.o \
emucore/CartFE.o \
emucore/CartMB.o \
emucore/CartF8SC.o \
emucore/CartF6SC.o \
emucore/Cart.o \
emucore/CartF4SC.o \
emucore/CartF4.o \
emucore/CartE7.o \
emucore/CartCV.o \
emucore/FSNode.o \
emucore/FrameBuffer.o \
emucore/MD5.o \
emucore/MediaSrc.o \
emucore/MediaFactory.o \
emucore/CartAR.o \
emucore/CartFASC.o \
emucore/Cart4K.o \
emucore/Control.o \
emucore/Cart3F.o \
emucore/Cart3E.o \
emucore/Booster.o \
emucore/CartDPC.o \
emucore/Deserializer.o \
emucore/EventHandler.o \
emucore/EventStreamer.o \
emucore/Serializer.o \
emucore/M6502.o \
emucore/M6502Hi.o \
emucore/M6502Low.o \
emucore/NullDev.o \
emucore/System.o \
emucore/OSystem.o \
\
emucore/unzip.o \
emucore/TIASnd.o \
\
common/FrameBufferSoft.o \
common/mainSDL.o \
common/SoundSDL.o \
\
psp/OSystemPSP.o \
psp/SettingsPSP.o \
psp/FSNodePSP.o

GUI_OBJS= \
gui/AboutDialog.o \
gui/AudioDialog.o \
gui/BrowserDialog.o \
gui/CheatCodeDialog.o \
gui/CommandDialog.o \
gui/CommandMenu.o \
gui/DialogContainer.o \
gui/Dialog.o \
gui/EditableWidget.o \
gui/EditTextWidget.o \
gui/EventMappingDialog.o \
gui/Font.o \
gui/GameInfoDialog.o \
gui/GameList.o \
gui/HelpDialog.o \
gui/InputTextDialog.o \
gui/Launcher.o \
gui/LauncherDialog.o \
gui/LauncherOptionsDialog.o \
gui/ListWidget.o \
gui/Menu.o \
gui/OptionsDialog.o \
gui/PopUpWidget.o \
gui/ProgressDialog.o \
gui/ScrollBarWidget.o \
gui/CheckListWidget.o \
gui/StringListWidget.o \
gui/TabWidget.o \
gui/VideoDialog.o \
gui/Widget.o

OBJS= $(EMU_OBJS) $(PSP_OBJS)
# $(GUI_OBJS) 

DEFAULT_CFLAGS = $(shell $(SDL_CONFIG) --cflags)

PSP_FW_VERSION=401
PSP2600_VERSION=1.1.4

MORE_CFLAGS = -G0 -Iemucore -Icommon -Ipsp \
 -fexceptions \
 -funroll-loops -ffast-math -fomit-frame-pointer \
 -fno-strength-reduce -finline-functions \
 -fsingle-precision-constant \
 -DPSPFW30X \
 -DPSP -DBSPF_PSP \
 -DSOUND_SUPPORT \
 -DPSP2600_VERSION=\"$(PSP2600_VERSION)\" \
 -DUSE_PSP_IRKEYB \
 -DNO_STDIO_REDIRECT
# -DJOYSTICK_SUPPORT
# -DPSP_DEBUG 

OPTIM_CFLAGS= -O2

CFLAGS = $(DEFAULT_CFLAGS) $(MORE_CFLAGS)
CXXFLAGS = $(DEFAULT_CFLAGS) $(MORE_CFLAGS) -fno-exceptions -fno-rtti

LIBS += -lstdc++ -lpspirkeyb -lpsppower -lSDL_image -lSDL -lpng -ljpeg -lpspaudio -lpspgu -lpsphprm -lz -lm -lg -lGLU -lGL -lglut -lpspvfpu -lpsprtc

BUILD_PRX=1

EXTRA_TARGETS   = EBOOT.PBP
PSP_EBOOT_TITLE = PSP2600-v$(PSP2600_VERSION)
PSP_EBOOT_ICON  = psp2600.png
PSP_EBOOT_SND0  = psp2600.at3
PSP_EBOOT_PIC1 = psp2600-pic.png

include $(PSPSDK)/lib/build.mak

all: EBOOT.PBP

.c.o:
	$(CC) $(CFLAGS) $(OPTIM_CFLAGS) -c -o $@ $<

.cpp.o:
	$(CXX) $(CXXFLAGS) $(OPTIM_CFLAGS) -c -o $@ $<

install: all
	@mkdir -p /media/disk/psp/game4XX/psp2600/
	cp EBOOT.PBP /media/disk/psp/game4XX/psp2600/

psp_gu.o : psp_gu.c
	$(CC) -c $< $(CFLAGS) -O0

realclean:
	/bin/rm -f $(OBJS) EBOOT.PBP PARAM.SFO

ctags :
	ctags *[chxp] gui/*[chxp] common/*[chxp] emucore/*[chxp] psp/*[chxp]
