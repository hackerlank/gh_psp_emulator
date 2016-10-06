
    Welcome to PSPDragon

Original Author of XRoar 

  Ciaran Anscomb  xroar(at)6809.org.uk

  Homepage: http://www.6809.org.uk/dragon/xroar.shtml

Author of the PSP port version 

  Ludovic Jacomme alias Zx-81 zx81.zx81(at)gmail.com

  Homepage: http://zx81.zx81.free.fr


1. INTRODUCTION
   ------------

  Xroar is the best emulator of Dragon32/64 and Tandy Coco home computer
  running on many systems such as Linux, Unix, Mac OS X, GP32 and Windows32.
  See http://www.6809.org.uk/dragon/xroar.shtml for further informations.

  PSPDragon is a port on PSP of the version 0.19 of Xroar.

  Special thanks to Danzel and Jeff Chen for their virtual keyboard,
  Ciaran Anscomb for this great emulator and his help on sound issue, to
  Raven's for eboot icons and to all PSPSDK developpers.

  This package is under GPL Copyright, read COPYING file for
  more information about it.


2. INSTALLATION
   ------------

  Unzip the zip file, and copy the content of the directory fw3.x or fw1.5
  (depending of the version of your firmware) on the psp/game, psp/game150,
  or psp/game3xx if you use custom firmware 3.xx-OE.

  It has been developped on linux for Firmware 3.71-m33 and i hope it works
  also for other firmwares.

  For any comments or questions on this version, please visit 
  http://zx81.zx81.free.fr or http://zx81.dcemu.co.uk


3. CONTROL
   ------------

3.1 - Virtual keyboard

  In the DRAGON emulator window, there are three different mapping 
  (standard, left trigger, and right Trigger mappings). 
  You can toggle between while playing inside the emulator using 
  the two PSP trigger keys.

    -------------------------------------
    PSP        DRAGON            (standard)
  
    Square     2
    Triangle   ENTER
    Circle     1
    Cross      Joystick Fire
    Up         Up
    Down       Down
    Left       Left 
    Right      Right

    Analog     Joystick

    -------------------------------------
    PSP        DRAGON   (left trigger)
  
    Square     FPS  
    Triangle   LOAD Snapshot
    Circle     Swap digital / Analog
    Cross      SAVE Snapshot
    Up         Up
    Down       Down
    Left       Render mode
    Right      Render mode

    -------------------------------------
    PSP        DRAGON   (right trigger)
  
    Square     Space
    Triangle   ENTER
    Circle     4
    Cross      Joystick Fire
    Up         Up
    Down       Down
    Left       Left 
    Right      Right
  
  
    Analog     Joystick
    
    Press Start  + L + R   to exit and return to eloader.
    Press Select           to enter in emulator main menu.
    Press Start            open/close the On-Screen keyboard

  In the main menu

    RTrigger   Reset the emulator

    Triangle   Go Up directory
    Cross      Valid
    Circle     Valid
    Square     Go Back to the emulator window

  The On-Screen Keyboard of "Danzel" and "Jeff Chen"

    Use Analog stick to choose one of the 9 squares, and
    use Triangle, Square, Cross and Circle to choose one
    of the 4 letters of the highlighted square.

    Use LTrigger and RTrigger to see other 9 squares 
    figures.

3.2 - IR keyboard

  You can also use IR keyboard. Edit the pspirkeyb.ini file
  to specify your IR keyboard model, and modify eventually
  layout keyboard files in the keymap directory.

  The following mapping is done :

  IR-keyboard   PSP

  Cursor        Digital Pad

  Tab           Start
  Ctrl-W        Start

  Escape        Select
  Ctrl-Q        Select

  Ctrl-E        Triangle
  Ctrl-X        Cross
  Ctrl-S        Square
  Ctrl-F        Circle
  Ctrl-Z        L-trigger
  Ctrl-C        R-trigger

  In the emulator window you can use the IR keyboard to
  enter letters, special characters and digits.

  (NOTE) : If you encounter issue with the joystick, don't forget to set the
  proper joystick (left or right, analog or not) in the settings menu.

4. LOADING DISK FILES (DSK)
   ------------

  If you want to load disk image in the virtual disk drive of your emulator,
  you have to put your disk file (with .dsk file extension) on your PSP
  memory stick in the 'disk' directory. 

  Then, while inside emulator, just press SELECT to enter in the emulator 
  main menu, choose "Load Disc" and then using the file selector choose 
  one disc file to load in your emulator.

  To list the content of the disk, you have to use the virtual keyboard
  (press START key) and type the command DIR followed by ENTER.

  To run the game MYGAME.BIN on your disk, you have to use the virtual 
  keyboard and type the command LOADM"MYGAME" followed by ENTER.
  Once the game is loaded, enter the command EXEC followed by ENTER.

  To run a basic game (with .BAS extention) use RUN command instead 
  of LOADM.

  If you want to specify the command to run for given games then
  you can do it in the run.txt, using the following syntax :

    diskname=LOADM"RunName":EXEC
    diskname=RUN"RunName"


5. LOADING KEY MAPPING FILES
   ------------

  For given games, the default keyboard mapping between PSP Keys and DRAGON keys,
  is not suitable, and the game can't be played on PSPDRAGON.

  To overcome the issue, you can write your own mapping file. Using notepad for
  example you can edit a file with the .kbd extension and put it in the kbd 
  directory.

  For the exact syntax of those mapping files, have a look on sample files already
  presents in the kbd directory (default.kbd etc ...).

  After writting such keyboard mapping file, you can load them using 
  the main menu inside the emulator.

  If the keyboard filename is the same as the cartridge or disk file
  then when you load this file, the corresponding keyboard file is 
  automatically loaded !

  You can now use the Keyboard menu and edit, load and save your
  keyboard mapping files inside the emulator. The Save option save the .kbd
  file in the kbd directory using the "Game Name" as filename. The game name
  is displayed on the right corner in the emulator menu.

  
6. COMPILATION
   ------------

  It has been developped under Linux using gcc with PSPSDK. 
  To rebuild the homebrew run the Makefile in the src archive.

