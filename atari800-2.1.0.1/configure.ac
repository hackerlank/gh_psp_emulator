dnl   configure.ac - Autoconf script for building configure
dnl
dnl   Copyright (C) 2001 Krzysztof Nikiel
dnl   Copyright (C) 2001-2009 Atari800 development team (see DOC/CREDITS)
dnl
dnl   This file is part of the Atari800 emulator project which emulates
dnl   the Atari 400, 800, 800XL, 130XE, and 5200 8-bit computers.
dnl
dnl   Atari800 is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation; either version 2 of the License, or
dnl   (at your option) any later version.
dnl
dnl   Atari800 is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details.
dnl
dnl   You should have received a copy of the GNU General Public License
dnl   along with Atari800; if not, write to the Free Software
dnl   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

AC_PREREQ(2.57)
AC_INIT(Atari800, 2.1.0, pstehlik@sophics.cz)
AC_CONFIG_SRCDIR(atari.c)
AC_CONFIG_HEADER(config.h)


dnl Set a8_target...

case "$target" in
    basic | curses | ncurses | pdcurses | dosvga | falcon | javanvm | javanvm-basic | ps2 | sdl | windx | x11 | x11-shm | x11-motif | x11-xview | x11-xview-shm)
        a8_target="$target"
        ;;
    shm | motif | xview | xview-shm)
        a8_target="x11-$target"
        ;;
    *)
        echo
        echo "Usage: configure --target=<target> <other options>"
        echo
        echo "The '--target' option is required.  Use '--help' to see other available"
        echo "options, including Atari800 specific '--enable' options.  Possible values"
        echo "for the '--target' option are:"
        echo "    basic            (text mode using standard I/O)"
        echo "    curses           (text mode using Curses library)"
        echo "    ncurses          (text mode using NCurses library)"
        echo "    pdcurses         (text mode using PDCurses library)"
        echo "    dosvga           (DOS only)"
        echo "    falcon           (Atari Falcon systems only)"
        echo "    javanvm          (Java NestedVM: set --host=mips-unknown-elf)"
        echo "    javanvm-basic    (Java NestedVM text-mode: set --host=mips-unknown-elf)"
        echo "    ps2              (Sony PlayStation 2)"
        echo "    sdl              (System with Simple Direct Media Layer library)"
        echo "    windx            (Windows with DirectX only)"
        echo "    x11              (Standard X11)"
        echo "    (x11-)motif      (Motif on X11)"
        echo "    (x11-)shm        (Standard X11 with shared memory extensions)"
        echo "    (x11-)xview      (XView on X11)"
        echo "    (x11-)xview-shm  (XView on X11, with shared memory extensions)"
        echo
        exit 1;
        ;;
esac

AC_CANONICAL_BUILD

dnl Set a8_host...

AC_CANONICAL_HOST

if [[ "$host_os" = "cygwin" ]]; then
    if [[ "$a8_target" = "windx" -o "$a8_target" = "sdl" ]]; then
        CC="gcc -mno-cygwin"
        echo
        echo "Using CC=\"$CC\" to disable cygwin library...";
        echo
        host_os="mingw32"
    fi
fi

case $host_os in
    i386-pc-os2-emx)
        a8_host="os/2"
        ;;
    mingw32)
        a8_host="win"
        AC_DEFINE(DIR_SEP_BACKSLASH,1,[Define to use back slash as directory separator.])
        AC_DEFINE(DOS_DRIVES,1,[Define to enable DOS style drives support.])
        ;;
    msdosdjgpp)
        a8_host="dos"
        AC_DEFINE(DIR_SEP_BACKSLASH,1,[Define to use back slash as directory separator.])
        AC_DEFINE(DEFAULT_CFG_NAME,"atari800.cfg",[Alternate config filename due to 8+3 fs limit.])
        AC_DEFINE(DOS_DRIVES,1,[Define to enable DOS style drives support.])
        AC_DEFINE(SYSTEM_WIDE_CFG_FILE,"c:\\atari800.cfg",[Alternate system-wide config file for non-Unix OS.])
        ;;
    linux | linux-gnu)
        a8_host="linux"
        ;;
    mint)
        a8_host="falcon"
        AC_DEFINE(DIR_SEP_BACKSLASH,1,[Define to use back slash as directory separator.])
        AC_DEFINE(DEFAULT_CFG_NAME,"atari800.cfg",[Alternate config filename due to 8+3 fs limit.])
        AC_DEFINE(DOS_DRIVES,1,[Define to enable DOS style drives support.])
        AC_DEFINE(SYSTEM_WIDE_CFG_FILE,"c:\\atari800.cfg",[Alternate system-wide config file for non-Unix OS.])
        ;;
    *unix*)
        a8_host="unix"
        ;;
    *)
        a8_host="$host_os"
        ;;
esac


dnl Set CFLAGS and LDFLAGS...

if [[ "X_$CFLAGS" = "X_" ]]; then
    CFLAGS="-O2 -Wall"
fi
if [[ "X_$LDFLAGS" = "X_" ]]; then
    LDFLAGS=""
fi
if [[ "$a8_target" = "ps2" ]]; then
    CC="ee-gcc"
    CFLAGS="$CFLAGS -D_EE -G0 -mno-crt0 -fno-builtin-printf -nostartfiles"
    CFLAGS="$CFLAGS -I${PS2SDK}/common/include -I${PS2SDK}/ee/include"
    CFLAGS="$CFLAGS -I${GSKIT}/ee/dma/include -I${GSKIT}/ee/gs/include"
    CFLAGS="$CFLAGS -I${PS2DEV}/ee/lib/gcc-lib/ee/3.2.2/include"
    CFLAGS="$CFLAGS -I${PS2SDK}/ports/include"
    LDFLAGS="$LDFLAGS -T${PS2SDK}/ee/startup/linkfile"
    LDFLAGS="$LDFLAGS -L${PS2SDK}/ee/lib -L${GSKITSRC}/lib -L${PS2DEV}/ee/lib/gcc-lib/ee/3.2.2"
    LDFLAGS="$LDFLAGS -L${PS2SDK}/ports/lib"
fi


dnl Check for programs...

AC_PROG_CC
AC_PROG_INSTALL


dnl Check for header files...

dnl Beware: AC_PATH_X must NOT be inside "case"!
AC_PATH_X
case "$a8_target" in
    sdl)
        sdlcheck="X_`sdl-config --cflags`"
        if [[ "$sdlcheck" != "X_" ]]; then
            CFLAGS="$CFLAGS `sdl-config --cflags`"
dnl Don't redefine main -- SDL.h will do it for us in atari_sdl.c  Otherwise
dnl the autoconf tests don't work.
            if [[ "$a8_host" = "win" ]]; then
                CFLAGS=${CFLAGS/-Dmain=SDL_main/}
            fi
        else
            AC_MSG_ERROR("No output from sdl-config!  Is sdl-config installed?")
        fi
        ;;
    x11*)
        if [[ "X_$x_includes" != "X_" ]]; then
            CFLAGS="$CFLAGS -I$x_includes"
        fi
        if [[ "X_$x_libraries" != "X_" ]]; then
            LDFLAGS="$LDFLAGS -L$x_libraries"
        fi
        ;;
esac
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS([direct.h errno.h file.h signal.h sys/time.h time.h unistd.h unixio.h])
SUPPORTS_SOUND_OSS=yes
AC_CHECK_HEADERS([fcntl.h sys/ioctl.h sys/soundcard.h],,SUPPORTS_SOUND_OSS=no)
SUPPORTS_RDEVICE=yes
if [[ "$a8_host" = "win" ]]; then
    AC_CHECK_HEADERS([winsock2.h],,SUPPORTS_RDEVICE=no)
else
    AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h sys/socket.h termios.h],,SUPPORTS_RDEVICE=no)
fi


dnl Check for libraries...

AC_DEFUN(A8_NEED_LIB,AC_CHECK_LIB($1,main,,AC_MSG_ERROR("$1 library not found!")))
AC_CHECK_LIB(z,gzopen)
AC_CHECK_LIB(png,png_get_libpng_ver)
AC_CHECK_LIB(m,cos,[LIBS="-lm $LIBS"])
AC_CHECK_LIB(ossaudio,_oss_ioctl,[LIBS="-lossaudio $LIBS"])

dnl Set OBJS and libraries depending on host and target...

dnl OBJS is not an AC "precious" variable but is used in the Makefile, so
dnl it must be explicitly AC_SUBST'ed at the end of this file.
OBJS=
if [[ "$a8_host" = "win" -a "$a8_target" != "sdl" ]]; then
    A8_NEED_LIB(gdi32)
    A8_NEED_LIB(winmm)
fi
case "$a8_target" in
    basic)
        AC_DEFINE(BASIC,1,[Target: standard I/O.])
        OBJS="atari_basic.o"
	WANT_EVENT_RECORDING=no
        ;;
    dosvga)
        if [[ "$a8_host" != "dos" ]]; then
            AC_MSG_ERROR("the dosvga target is valid only on a DOS system!")
        fi
        AC_DEFINE(DOSVGA,1,[Target: DOS VGA.])
        AC_DEFINE(SUPPORTS_PLATFORM_CONFIGURE,1,[Additional config file options.])
        AC_DEFINE(SUPPORTS_PLATFORM_PALETTEUPDATE,1,[Update the Palette if it changed.])
        OBJS="dos/atari_vga.o dos/vga_gfx.o dos/vga_asm.o"
        CFLAGS="$CFLAGS -Waggregate-return -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wstrict-prototypes -Winline"
        ;;
    curses)
        AC_DEFINE(USE_CURSES,1,[Target: Curses-compatible library.])
        A8_NEED_LIB(curses)
        OBJS="atari_curses.o"
	WANT_EVENT_RECORDING=no
        ;;
    ncurses)
        AC_DEFINE(USE_CURSES,1,[Target: Curses-compatible library.])
        AC_DEFINE(USE_NCURSES,1,[Target: Ncurses library.])
        A8_NEED_LIB(ncurses)
        OBJS="atari_curses.o"
	WANT_EVENT_RECORDING=no
        ;;
    pdcurses)
        AC_DEFINE(USE_CURSES,1,[Target: Curses-compatible library.])
        A8_NEED_LIB(pdcurses)
        OBJS="atari_curses.o"
	WANT_EVENT_RECORDING=no
        ;;
    falcon)
        AC_DEFINE(FALCON,1,[Target: Atari Falcon system.])
        A8_NEED_LIB(gem)
        OBJS="atari_falcon.o falcon/c2p_uni.o falcon/c2p_unid.o falcon/videl.o falcon/ikbd.o"
        ;;
    javanvm)
        AC_DEFINE(JAVANVM,1,[Target: Java NestedVM.])
        AC_DEFINE(SUPPORTS_PLATFORM_PALETTEUPDATE,1,[Update the Palette if it changed.])
        AC_DEFINE(SUPPORTS_PLATFORM_SLEEP,1,[Platform-specific sleep function.])
        AC_DEFINE(SUPPORTS_SOUND_REINIT,1,[Reinitialise the sound system.])
        OBJS="javanvm/atari_javanvm.o"
        EXEEXT=".mips"
        enable_unalignedwords=no
        if [[ "$build_os" = "cygwin" ]]; then
            JAVAFLAGS="-classpath '""`cygpath -wp $CLASSPATH`'"
            JAVACFLAGS="-source 1.4 -classpath '""`cygpath -wp $CLASSPATH`'"
        fi
        CFLAGS="$CFLAGS -ansi -pedantic -Waggregate-return -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wstrict-prototypes -Winline"
        ;;
    javanvm-basic)
        AC_DEFINE(BASIC,1,[Target: standard I/O.])
        OBJS="atari_basic.o"
        EXEEXT=".mips"
        enable_unalignedwords=no
        enable_sound=no
        if [[ "$build_os" = "cygwin" ]]; then
            JAVAFLAGS="-classpath '""`cygpath -wp $CLASSPATH`'"
            JAVACFLAGS="-classpath '""`cygpath -wp $CLASSPATH`'"
        fi
        ;;
    ps2)
        AC_DEFINE(PS2,1,[Target: Sony PlayStation 2.])
        AC_DEFINE(DEFAULT_CFG_NAME,"mc0:/ATARI/ATARI800.CFG",[Alternate config filename due to 8+3 fs limit.])
        A8_NEED_LIB(kernel)
	A8_NEED_LIB(audsrv)
        A8_NEED_LIB(debug)
        A8_NEED_LIB(syscall)
        A8_NEED_LIB(dmakit)
        A8_NEED_LIB(gskit)
        A8_NEED_LIB(pad)
        A8_NEED_LIB(kbd)
        A8_NEED_LIB(mc)
        A8_NEED_LIB(g)
        A8_NEED_LIB(z)
        A8_NEED_LIB(c)
        OBJS="atari_ps2.o ps2kbd.s usbd.s audsrv.s"
        ;;
    sdl)
        AC_DEFINE(SDL,1,[Target: SDL library.])
        AC_DEFINE(SUPPORTS_PLATFORM_CONFIGURE,1,[Additional config file options.])
        AC_DEFINE(SUPPORTS_PLATFORM_CONFIGSAVE,1,[Save additional config file options.])
        AC_DEFINE(SUPPORTS_PLATFORM_PALETTEUPDATE,1,[Update the Palette if it changed.])
        AC_DEFINE(SUPPORTS_SOUND_REINIT,1,[Reinitialise the sound system.])
	WANT_XEP80_EMULATION=yes
	WANT_NTSC_FILTER=yes
        LIBS="$LIBS `sdl-config --libs`"
        OBJS="atari_sdl.o"
        CFLAGS="$CFLAGS -ansi -pedantic -Waggregate-return -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wstrict-prototypes -Winline"
        if [[ "$a8_host" = "win" ]]; then
dnl Avoid making stdout.txt on Windows sdl target by compiling our own SDL_main
dnl Get SDL_win32_main.c from the SDL source and copy it to the src directory.
            LIBS=${LIBS/-lSDLmain/}
            OBJS="$OBJS SDL_win32_main.o"
            CFLAGS="$CFLAGS -DNO_STDIO_REDIRECT"
dnl Don't make this a windowed app, make it a console app.
            LIBS=${LIBS/-mwindows/}
        fi
        AC_DEFINE(PBI_PROTO80,1,[A prototype 80 column card for the 1090 expansion box.])
        OBJS="$OBJS pbi_proto80.o"
        ;;
    windx)
        AC_DEFINE(DIRECTX,1,[Target: Windows with DirectX.])
        AC_DEFINE(SUPPORTS_PLATFORM_PALETTEUPDATE,1,[Update the Palette if it changed.])
        AC_DEFINE(SUPPORTS_SOUND_REINIT,1,[Reinitialise the sound system.])
        A8_NEED_LIB(ddraw)
        A8_NEED_LIB(dinput)
        A8_NEED_LIB(dsound)
        A8_NEED_LIB(dxguid)
        A8_NEED_LIB(ws2_32)
        OBJS="win32/atari_win32.o win32/screen_win32.o win32/main.o win32/keyboard.o win32/joystick.o win32/atari.ro"
        CFLAGS="$CFLAGS -ansi -pedantic -Waggregate-return -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wstrict-prototypes -Winline"
        ;;
    x11*)
        AC_DEFINE(X11,1,[Target: Standard X11.])
        A8_NEED_LIB(X11)
        case "$a8_target" in
            *-shm)
                AC_DEFINE(SHM,1,[Target: X11 with shared memory extensions.])
                A8_NEED_LIB(Xext)
                ;;
        esac
        case "$a8_target" in
            x11-xview*)
                AC_DEFINE(XVIEW,1,[Target: X11 with XView.])
                dnl Dunno if this is the right way...
                CFLAGS="$CFLAGS -I/usr/openwin/include"
                LDFLAGS="$LDFLAGS -L/usr/openwin/lib"
                dnl -lxview must come before -lolgx, otherwise it does not link.
                dnl Note that AC_CHECK_LIB (and therefore A8_NEED_LIB) _prepends_
                dnl the found library to LIBS.
                A8_NEED_LIB(olgx)
                A8_NEED_LIB(xview)
                ;;
            x11-motif)
                AC_DEFINE(MOTIF,1,[Target: X11 with Motif.])
                dnl According to the information I was able to find,
                dnl "-lgen -lsocket" is necessary only with old Sun Motif libs.
                AC_CHECK_LIB(socket,main,[LIBS="-lsocket $LIBS"])
                AC_CHECK_LIB(gen,main,[LIBS="-lgen $LIBS"])
                dnl -lXm must come before -lXt, otherwise I get the following
                dnl fatal error at runtime, when calling XtVaCreateManagedWidget:
                dnl X Error of failed request:  BadWindow (invalid Window parameter)
                dnl See MOTIF FAQ "What order should the libraries be linked in?"
                dnl Note that AC_CHECK_LIB (and therefore A8_NEED_LIB) _prepends_
                dnl the found library to LIBS.
                A8_NEED_LIB(Xt)
                A8_NEED_LIB(Xm)
                ;;
        esac
        OBJS="atari_x11.o"
        ;;
esac
dnl All future references to OBJS should ADD to it, not (re)define it.


dnl Check for typedefs, structures, and compiler characteristics...

AC_C_BIGENDIAN
AC_C_CONST
AC_C_INLINE
AC_C_VOLATILE
AC_PROG_GCC_TRADITIONAL
AC_STRUCT_TM
AC_TYPE_SIGNAL
AC_TYPE_SIZE_T


dnl Set usage of unaligned word accesses...

AC_ARG_ENABLE(unalignedwords,AC_HELP_STRING(--enable-unalignedwords,[Override usage of unaligned words]))
if [[ "$enable_unalignedwords" != "yes" -a "$enable_unalignedwords" != "no" ]]; then
    case $host_cpu in
        alpha* | arm* | hppa* | ia64 | mips* | sparc*)
            enable_unalignedwords=no
            ;;
        i*86 | m68* | powerpc* | x86_64)
            enable_unalignedwords=yes
            ;;
        *)
            AC_MSG_WARN([$host_cpu architecture is unknown to this script.])
            AC_MSG_WARN([Performance may be sub-optimal. Please contact Atari800 developers.])
            enable_unalignedwords=no
            ;;
    esac
fi

if [[ "$enable_unalignedwords" = "yes" ]]; then
    dnl Make sure it is allowed
    AC_MSG_CHECKING([for unaligned word access validity])
    AC_RUN_IFELSE(
        [AC_LANG_PROGRAM([[#include <stdio.h>]],[[
            unsigned char test_data[] = "Hello, I test unaligned word access validity.";
            unsigned char *p;
            /* step through test_data as far as 4 bytes are available via p */
            for (p = test_data; p[2] != '\0'; p++) {
                unsigned int word_read = *(unsigned short *) p;
                if (word_read != (p[0] + (p[1] << 8))
                 && word_read != (p[1] + (p[0] << 8))) {
                    printf("16-bit access at address %p yields bad data!\n"
                           "Bytes: %02X %02X; Value read: %04X\n",
                           p, p[0], p[1], word_read);
                    return 1;
                }
                word_read = *(unsigned int *) p;
                if (word_read != (p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24))
                 && word_read != (p[3] + (p[2] << 8) + (p[1] << 16) + (p[0] << 24))) {
                    printf("32-bit access at address %p yields bad data!\n"
                           "Bytes: %02X %02X %02X %02X; Value read: %08X\n",
                           p, p[0], p[1], p[2], p[3], word_read);
                    return 1;
                }
            }
        ]])],
        AC_MSG_RESULT([yes]),
        [enable_unalignedwords=no; AC_MSG_RESULT([failed! disabling unaligned word access])],
        AC_MSG_RESULT([skipped because cross-compiling])
    )
    if [[ "$enable_unalignedwords" = "yes" ]]; then
        AC_DEFINE(WORDS_UNALIGNED_OK,1,[Define if unaligned word access is ok.])
    fi
fi


dnl Check for library functions...

AC_FUNC_CLOSEDIR_VOID
AC_FUNC_MEMCMP
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit chmod clock fdopen fflush floor fstat getcwd])
AC_CHECK_FUNCS([gettimeofday localtime memmove memset mkdir mkstemp mktemp])
AC_CHECK_FUNCS([modf nanosleep opendir rename rewind rmdir signal snprintf])
AC_CHECK_FUNCS([stat strcasecmp strchr strdup strerror strrchr strstr])
AC_CHECK_FUNCS([strtol system time tmpfile tmpnam uclock unlink vsnprintf])
dnl select usleep strncpy are broken on javanvm targets
if [[ "$a8_target" != "javanvm" -a "$a8_target" != "javanvm-basic" ]]; then
    AC_CHECK_FUNCS([select usleep strncpy])
fi
if [[ "$a8_host" != "win" ]]; then
    AC_CHECK_FUNCS([gethostbyaddr gethostbyname inet_ntoa socket],,SUPPORTS_RDEVICE=no)
fi


dnl Select optional Atari800 features...

dnl The A8_OPTION macro automates the calling of AC_ARG_ENABLE, AC_HELP_STRING and AC_DEFINE.
dnl         $1    = the name of the feature, what follows "--enable-".
dnl         $2    = "yes" or "no", determines if feature defaults to being active or not.
dnl         $3    = The right side of the help line.
dnl                 The left side is always "--enable-$1".
dnl         $4    = The symbol name which goes to config.h, the C #define symbol.
dnl                 A symbol called "WANT_$4" is defined for use in this configure.ac.
dnl         $5    = The help string which shows up in config.h for the $4 symbol.
AC_DEFUN([A8_OPTION],[
    AC_ARG_ENABLE($1,AC_HELP_STRING(--enable-$1,$3),WANT_$4=$enableval,WANT_$4=$2)
    if [[ "$WANT_$4" = "yes" ]]; then
        AC_DEFINE($4,1,$5)
    fi
])

if [[ "$a8_target" != "basic" -a "$a8_target" != "javanvm-basic" ]]; then

    dnl These objects are not compiled in BASIC target
    OBJS="$OBJS input.o statesav.o ui_basic.o ui.o"

    case "$a8_target" in
        *curses)
            A8_OPTION(cursesbasic,yes,
                      [No bitmap graphics emulation (curses targets only) (default=ON)],
                      CURSES_BASIC,[Define to disable bitmap graphics emulation in CURSES target.]
                     )
            ;;
    esac

    if [[ "$WANT_CURSES_BASIC" != "yes" ]]; then

        dnl These objects are not compiled in BASIC nor CURSES_BASIC target
        OBJS="$OBJS colours.o screen.o"

        A8_OPTION(newcycleexact,yes,
                  [Allow color changes inside a scanline (default=ON)],
                  NEW_CYCLE_EXACT,[Define to allow color changes inside a scanline.]
                 )
        if [[ "$WANT_NEW_CYCLE_EXACT" = "yes" ]]; then
            OBJS="$OBJS cycle_map.o"
        fi

        A8_OPTION(veryslow,no,
                  [Use very slow computer support (use with the -refresh option) (default=OFF)],
                  VERY_SLOW,[Define to use very slow computer support (faster -refresh).]
                 )

    fi

    A8_OPTION(crashmenu,yes,
              [Display a menu after a CIM instruction (default=ON)],
              CRASH_MENU,[Define to activate crash menu after CIM instruction.]
             )
fi

A8_OPTION(pagedattrib,no,
          [Use page-based attribute array (default=OFF)],
          PAGED_ATTRIB,[Define to use page-based attribute array.]
         )

A8_OPTION(bufferedlog,no,
          [Use buffered debug output (until the graphics mode switches back to text mode) (default=OFF)],
          BUFFERED_LOG,[Define to use buffered debug output.]
         )

A8_OPTION(monitorasm,yes,
          [Provide an assembler in the monitor (default=ON)],
          MONITOR_ASSEMBLER,[Define to activate assembler in monitor.]
         )

A8_OPTION(monitorbreak,yes,
          [Support code breakpoints and execution history (slower emulation) (default=ON)],
          MONITOR_BREAK,[Define to activate code breakpoints and execution history.]
         )

A8_OPTION(monitorbreakpoints,no,
          [Support user-defined breakpoints (default=OFF)],
          MONITOR_BREAKPOINTS,[Define to activate user-defined breakpoints.]
         )

A8_OPTION(monitorhints,yes,
          [Provide hints in the disassembler (human-readable address labels) (default=ON)],
          MONITOR_HINTS,[Define to activate hints in disassembler.]
         )

A8_OPTION(monitorprofile,no,
          [6502 opcode profiling (default=OFF)],
          MONITOR_PROFILE,[Define to activate 6502 opcode profiling.]
         )

A8_OPTION(monitortrace,no,
          [Support TRACE command in the monitor (default=OFF)],
          MONITOR_TRACE,[Define to activate TRACE command in monitor.]
         )

if [[ "$WANT_MONITOR_READLINE" != "no" ]]; then
	AC_CHECK_LIB(readline,readline,
		[A8_OPTION(readline,yes,
          		[Support readline in the monitor (default=ON)],
          		MONITOR_READLINE,[Define to activate readline support in monitor.]
			)],
		WANT_MONITOR_READLINE="no"
	)
fi

if [[ "$WANT_MONITOR_READLINE" != "no" ]]; then
	LIBS="-lreadline -ltermcap $LIBS"
fi

if [[ "$WANT_EVENT_RECORDING" != "no" ]]; then
dnl we already checked for libz and added it to LIBS
    AC_CHECK_LIB(z,adler32,
              [A8_OPTION(eventrecording,yes,
                    [Support event recording (default=ON)],
                    EVENT_RECORDING,[Define to enable event recording.]
                   )],
              WANT_EVENT_RECORDING="no"
             )
fi

A8_OPTION(pbi_mio,yes,
      [Emulate the MIO board (default=ON)],
      PBI_MIO,[Define to emulate the MIO board.]
     )

if [[ "$WANT_PBI_MIO" = "yes" ]]; then
    OBJS="$OBJS pbi_mio.o"
fi

A8_OPTION(pbi_bb,yes,
      [Emulate the Black Box (default=ON)],
      PBI_BB,[Define to emulate the Black Box.]
     )

if [[ "$WANT_PBI_BB" = "yes" ]]; then
    OBJS="$OBJS pbi_bb.o"
fi

if [[ "$WANT_PBI_BB" = "yes" -o "$WANT_PBI_MIO" = "yes" ]]; then
    OBJS="$OBJS pbi_scsi.o"
fi

dnl Set sound support options...

A8_OPTION(sound,yes,
          [Use sound support (default=ON)],
          SOUND,[Define to activate sound support.]
         )
if [[ "$WANT_SOUND" = "yes" ]]; then

    OBJS="$OBJS pokeysnd.o mzpokeysnd.o remez.o sndsave.o"

    if [[ "$a8_target" != "ps2" -a "$a8_target" != "sdl" -a "$a8_target" != "javanvm" ]]; then
        case "$a8_host" in
            dos)
                OBJS="$OBJS dos/sound_dos.o dos/dos_sb.o"
                ;;
            win)
                OBJS="$OBJS win32/sound.o"
                ;;
            falcon)
                OBJS="$OBJS sound_falcon.o falcon/sound_dma.o"
                ;;
            *)
                if [[ "$SUPPORTS_SOUND_OSS" = "yes" ]]; then
                    OBJS="$OBJS sound_oss.o"
                else
                    AC_MSG_ERROR([Sound support not available, re-run configure with --disable-sound])
                fi
                ;;
        esac
    fi

    A8_OPTION(nonlinear_mixing,no,
              [Use nonlinear POKEY mixing (default=OFF)],
              NONLINEAR_MIXING,[Define to use nonlinear POKEY mixing.]
             )

    A8_OPTION(interpolatesound,yes,
              [Use sound interpolation (default=ON)],
              INTERPOLATE_SOUND,[Define to allow sound interpolation.]
             )

    A8_OPTION(stereosound,yes,
              [Use stereo sound (default=ON)],
              STEREO_SOUND,[Define to allow stereo sound.]
             )

    A8_OPTION(volonlysound,yes,
              [Use volume only sound (digitized sound effects) (default=ON)],
              VOL_ONLY_SOUND,[Define to allow volume only sound.]
             )
    if [[ "$WANT_VOL_ONLY_SOUND" = "yes" ]]; then
        A8_OPTION(consolesound,yes,
                  [Use console sound (keyboard clicks) (default=ON)],
                  CONSOLE_SOUND,[Define to allow console sound (keyboard clicks).]
                 )
        A8_OPTION(seriosound,no,
                  [Use serial in/out sound (default=OFF)],
                  SERIO_SOUND,[Define to allow serial in/out sound.]
                 )
    else
        WANT_CONSOLE_SOUND="no"
        WANT_SERIO_SOUND="no"
    fi

    A8_OPTION(clipsound,no,
              [Use sound clipping (default=OFF)],
              CLIP_SOUND,[Define to allow sound clipping.]
             )
    A8_OPTION(pbi_xld,yes,
              [Emulate 1450XLD (default=ON)],
              PBI_XLD,[Define to emulate the 1400XL/1450XLD.]
             )
    if [[ "$WANT_PBI_XLD" = "yes" ]]; then
        OBJS="$OBJS pbi_xld.o votrax.o"
    fi
else
    WANT_NONLINEAR_MIXING="no"
    WANT_INTERPOLATE_SOUND="no"
    WANT_STEREO_SOUND="no"
    WANT_VOL_ONLY_SOUND="no"
    WANT_CONSOLE_SOUND="no"
    WANT_SERIO_SOUND="no"
    WANT_CLIP_SOUND="no"
    WANT_PBI_XLD_SOUND="no"
fi


dnl Select host/target specific features...

if [[ "$a8_host" = "falcon" -a "$a8_target" = "falcon" ]]; then
    A8_OPTION(falconcpuasm,no,
              [Use m68k assembler CPU core for Falcon target (default=OFF)],
              FALCON_CPUASM,[Define to use m68k assembler CPU core for Falcon target.]
             )
    if [[ "$WANT_FALCON_CPUASM" = "yes" ]]; then
        OBJS="$OBJS cpu_m68k.o"
    fi
fi

if [[ "$a8_host" = "linux" ]]; then
    case "$a8_target" in
        x11*)
            A8_OPTION(linuxjoystick,yes,
                      [Use LINUX joystick (linux x11 targets only) (default=ON)],
                      LINUX_JOYSTICK,[Define to use LINUX joystick.]
                      )
    esac
fi

if [[ "$a8_target" = "windx" ]]; then
    A8_OPTION(cursorblock,no,
              [Using cursor/ctrl keys for keyboard joystick (windx targets only) (default=OFF)],
              USE_CURSORBLOCK,[Define for using cursor/ctrl keys for keyboard joystick.]
             )
fi

if [[ "$WANT_XEP80_EMULATION" = "yes" ]]; then
    AC_DEFINE(XEP80_EMULATION,1,[Emulate the XEP80.])
    OBJS="$OBJS xep80.o xep80_fonts.o"
fi

if [[ "$WANT_NTSC_FILTER" = "yes" ]]; then
	AC_DEFINE(NTSC_FILTER,1,[Use NTSC video filter.])
    OBJS="$OBJS atari_ntsc.o"
fi

if [[ "$SUPPORTS_RDEVICE" = "yes" ]]; then
    A8_OPTION(riodevice,no,
              [Use the R: networking device (Linux/Unix/Win32) (default=OFF)],
              R_IO_DEVICE,[Define to use R: device.]
             )
    if [[ "$WANT_R_IO_DEVICE" = "yes" ]]; then
        OBJS="$OBJS rdevice.o"
        A8_OPTION(rnetwork,yes,
                 [Use IP network connection with the R: networking device (Linux/Unix/Win32) (default=ON)],
                 R_NETWORK,[Define to use IP network connection with the R: device.]
                )
        if [[ "$a8_host" != "win" ]]; then
            A8_OPTION(rserial,yes,
                     [Use the host serial port with the R: networking device (Linux/Unix only) (default=ON)],
                     R_SERIAL,[Define to use the host serial port with the R: device.]
                    )
        else
            A8_NEED_LIB(ws2_32)
            WANT_R_SERIAL="no"
        fi
    fi
fi


dnl Wrapup: export OBJS and write Makefile...

AC_SUBST(OBJS)
AC_SUBST(JAVAFLAGS)
AC_SUBST(JAVACFLAGS)
CONFIGURE_TARGET="$target"
AC_SUBST(CONFIGURE_TARGET)
AC_CONFIG_FILES(Makefile)
AC_OUTPUT


dnl Print results of configure's work...

echo "-------------------------------------------------------"
echo "                 CONFIGURATION RESULTS:"
echo "-------------------------------------------------------"
echo "Host OS...............................: $a8_host"
echo "Target ...............................: $a8_target"
echo
if [[ "$a8_target" != "basic" -a "$a8_target" != "javanvm-basic" ]]; then
    case "$a8_target" in
        *curses)
            echo "Using no bitmap graphics emulation?...: $WANT_CURSES_BASIC"
            ;;
    esac
    if [[ "$WANT_CURSES_BASIC" != "yes" ]]; then
        echo "Using cycle exact?....................: $WANT_NEW_CYCLE_EXACT"
        echo "Using the very slow computer support?.: $WANT_VERY_SLOW"
    fi
    echo "Using the crash menu?.................: $WANT_CRASH_MENU"
fi
echo "Using the paged attribute array?......: $WANT_PAGED_ATTRIB"
echo "Using the buffered log?...............: $WANT_BUFFERED_LOG"
echo "Using the monitor assembler?..........: $WANT_MONITOR_ASSEMBLER"
echo "Using code breakpoints and history?...: $WANT_MONITOR_BREAK"
echo "Using user-defined breakpoints?.......: $WANT_MONITOR_BREAKPOINTS"
echo "Using monitor hints?..................: $WANT_MONITOR_HINTS"
echo "Using 6502 opcode profiling?..........: $WANT_MONITOR_PROFILE"
echo "Using TRACE monitor command?..........: $WANT_MONITOR_TRACE"
echo "Using readline support in monitor?....: $WANT_MONITOR_READLINE"
echo "Using event recording?................: $WANT_EVENT_RECORDING"
echo "Using MIO emulation?..................: $WANT_PBI_MIO"
echo "Using Black Box emulation?............: $WANT_PBI_BB"
echo "Using sound?..........................: $WANT_SOUND"
if [[ "$WANT_SOUND" = "yes" ]]; then
    echo "    Using nonlinear mixing?...........: $WANT_NONLINEAR_MIXING"
    echo "    Using sound interpolation?........: $WANT_INTERPOLATE_SOUND"
    echo "    Using stereo sound?...............: $WANT_STEREO_SOUND"
    echo "    Using volume only sound?..........: $WANT_VOL_ONLY_SOUND"
    if [[ "$WANT_VOL_ONLY_SOUND" = "yes" ]]; then
        echo "        Using console sound?..........: $WANT_CONSOLE_SOUND"
        echo "        Using serial I/O sound?.......: $WANT_SERIO_SOUND"
    else
        echo "        (Volume only sound sub-options disabled)"
    fi
    echo "    Using 1400XL/1450XLD emulation?...: $WANT_PBI_XLD"
    echo "    Using sound clipping?.............: $WANT_CLIP_SOUND"
else
    echo "    (Sound sub-options disabled)"
fi
if [[ "$a8_target" = "falcon" ]]; then
    echo "Using M68K assembler CPU core?........: $WANT_FALCON_CPUASM"
fi
if [[ "$a8_host" = "linux" ]]; then
    case "$a8_target" in
        x11*)
            echo "Using Linux joystick?.................: $WANT_LINUX_JOYSTICK"
    esac
fi
if [[ "$a8_target" = "windx" ]]; then
    echo "Using cursor/ctrl keys?...............: $WANT_USE_CURSORBLOCK"
fi
if [[ "$SUPPORTS_RDEVICE" = "yes" ]]; then
    echo "Using R: device?......................: $WANT_R_IO_DEVICE"
        if [[ "$a8_host" != "win" -a "$WANT_R_IO_DEVICE" = "yes" ]]; then
            echo "Using R: with the host serial port?...: $WANT_R_SERIAL"
            echo "Using R: with IP network support......: $WANT_R_NETWORK"
        fi
fi
echo
echo "Main build variables:"
echo "    CC......: \"$CC\""
echo "    CFLAGS..: \"$CFLAGS\""
echo "    LDFLAGS.: \"$LDFLAGS\""
echo "    OBJS....: \"$OBJS\""
echo "    LIBS....: \"$LIBS\""
echo "-------------------------------------------------------"

if [[ "$WANT_VERY_SLOW" = "yes" ]]; then
	echo
	echo "IMPORTANT: --enable-veryslow provides a bit better performance"
	echo "only when the compiled atari800 is run with the -refresh option."
	if [[ "$WANT_MONITOR_BREAK" = "yes" ]]; then
		echo "It is better to use --disable-monitorbreak."
	fi
	if [[ "$WANT_SOUND" = "yes" ]]; then
		echo "Try --disable-sound, too."
	fi
	if [[ "$WANT_PAGED_ATTRIB" != "yes" ]]; then
		echo "Have you tried --enable-pagedattrib ?"
	fi
fi

echo
echo "Now run \"gmake\" or \"make\"."
echo
