# -x assembler-with-cpp
# gpSP makefile
# Gilead Kutnick - Exophase

# Global definitions

PSPSDK          = ${shell psp-config --pspsdk-path}
PREFIX          = ${shell psp-config --psp-prefix}
COMMON_FILE     = common.h cpu.h memory.h video.h input.h sound.h main.h gui.h zip.h cheats.h fbm_print.h message.h

PSP_EBOOT_ICON  = icon0.png

OBJS            = main.o cpu.o video.o memory.o sound.o input.o \
 	          cpu_threaded.o gui.o zip.o cheats.o mips_stub.o fbm_print.o bios.o

TARGET          = UO_gpSP_kai

VPATH           += ..
CFLAGS          += -O3 -DPSP_BUILD -G0 -funsigned-char -ffast-math
CFLAGS          += ${shell ${PREFIX}/bin/sdl-config --cflags}
ASFLAGS         = ${CFLAGS}
PSP_EBOOT_TITLE = UnOfficial_gpSP_kai_3.0
EXTRA_TARGETS   = EBOOT.PBP

LIBS            += ${shell ${PREFIX}/bin/sdl-config --libs} -lpsppower -lz

include ${PSPSDK}/lib/build.mak

main.o         : ${COMMON_FILE} main.c

cpu.o          : ${COMMON_FILE} cpu.c

video.o        : ${COMMON_FILE} video.c

memory.o       : ${COMMON_FILE} memory.c

sound.o        : ${COMMON_FILE} sound.c

input.o        : ${COMMON_FILE} input.c

cpu_threaded.o : ${COMMON_FILE} cpu_threaded.c mips_emit.h bios.h

gui.o          : ${COMMON_FILE} gui.c

zip.o          : ${COMMON_FILE} zip.c

cheats.o       : ${COMMON_FILE} cheats.c

mips_stub.o    : mips_stub.S

fbm_print.o    : ${COMMON_FILE} fbm_print.c

bios.o         : ${COMMON_FILE} bios.c bios.h

kx: kxploit
	rm -rf __SCE__UO_gpSP_kai
	rm -rf %__SCE__UO_gpSP_kai
	mv UO_gpSP_kai __SCE__UO_gpSP_kai
	mv UO_gpSP_kai% %__SCE__UO_gpSP_kai

all: kx
