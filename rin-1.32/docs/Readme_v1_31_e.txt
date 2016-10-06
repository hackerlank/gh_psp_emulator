RIN v1.31 - OFFICIAL English Readme (by Smiths, Emuholic [www.emuholic.com])

------[About]------
A GB/GBC for the PSP based off the Windows TGB Dual source.
In order to use this, a v1.0/v1.5 PSP is required

---[Usage]---
Make a folder on your memory stick called /PSP/GAME/RIN
Place the EBOOT.PBP file into the same folder
Please place ROMs in any folder you like
(ZIP, GZIP supported)

After that, from your HOME screen navigate to GAME→MEMORY STICK→RIN
Select RIN to run the emulator

Game's save files can be made in a subfolder called "SAVE".  This folder will be created automatically if it doesn't exist.

Default Controls:
O: A Button
X: B Button
Triangle: A Button (rapid fire)
Square: B Button (rapid fire)
L: In-Game Menu
START: Start Button
SELECT: Select Button
R+START: Quick Load (Save State)
R+SELECT: Quick Save (Save State)

---[File Browser Wallpaper Information (feature added by kwn)]---
Place a file called RINMENU.BMP in the same working directory as RIN
The file MUST obey the following format rules:
　+ 480x272x24bit Windows BMP format
　+ Have a header size of 0x36byte (Making the file size 391,734 bytes)*
*If you use a standard image editing program, the above rules should automatically be followed, creating a proper BMP file

---[Cheat Codes]---
You can use Gameshark (Pro Action Replay) cheat codes.
Place a cheat file (extension of .tch) in the CHEAT folder.
After loading a game, load the cheat file from the menu.
The cheat file should have the same name as the ROM image.
Note: You cannot use TGB Dual codes

---[On Memory State Save]---
Save State information is stored to a temporary slot in the system's RAM.
This allows saving and loading without having to access the memory stick. (Faster)
However, if power is cut, or another ROM is loaded, this memory is erased.
(It is not erased in Sleep Mode)

---[Version History]---
v1.31 (2005/08/06)
+ Sped up save compression
+ On Memory State Save
+ Added low battery level warning
+ Others

v1.3 (2005/07/25)
+ Pro Action Replay (Gameshark) cheat code support
+ Fixed sound bug
+ Others

v1.29 (2005/07/20)
+ Performance increases
+ Altered sound buffer relationships
+ Others

v1.28 (2005/07/16)
+ GZIP support for ROM files added
+ Small bug fixes
+ Others

v1.27 (2005/07/14)
+ Changed thumbnail format to PNG
+ PNG Wallpaper Support
+ Sped up State Saves
+ Added GZIP support for State Saves and SRAM files
+ Increased Save State slots to 10
+ Fixed a bug in Waitstep when VSYNC was off
+ Others

v1.26 (2005/06/26)
+ Fixed rare freezing bug
+ Changed compilers

v1.25 (2005/06/21)
+ Sped up file sorting (ruka)
+ Added ability to view individual files inside an archive
+ Improvements/fixes when returning from sleep mode
+ Infrared input support (Chicchai Alien, etc. use it)
+ Battery Life indicator (in build 2005/06/22)
+ Others

v1.24 (2005/06/14)
+ Added ability to change clock speed (USE AT YOUR OWN RISK)
+ Added ability to delete ROMs
+ Added folder parsing
+ Removed unused screen size modes and unnecessary GB COLOR routines
   - (Shortcut keys updated to reflect this)
+ Working on existing code for when simultaneous key presses are used in shortcuts
+ Small bug fixes
+ Others

v1.23 (2005/06/12)
+ Fixed a bug in Save State thumbnail creations
+ Added ability to delete Save State files
+ Good stuff

v1.22 (2005/06/12)
+ Put in Super Gameboy Border support (screen must be in 1x mode)
+ Super Gameboy screen rendering bug fixed
+ Placed a fix for screen distortion when switching to TURBO mode
+ Has State Save thumbnail creation support
+ And others

v1.21 (2005/06/07)
+ Crushed freezes/bugs with Super Gameboy support

v1.2 (2005/06/07)
+ Knows Super Gameboy Palette support
+ Enabled "Turbo Mode"
+ Ruka's latest Unzip library for PSP is now used
+ Such and such

v1.19 (2005/06/02)
+ Uses times stamps in save state files
+ Changes to the Save State interface
+ Kinds of Performance Enhancements
+ Supports Auto Frame Skipping
+ A lot...

v1.18 (2005/05/30)
+ Speed optimizations to Fullscreen mode (YASIT)
+ Supports pseudo-coloring of GBMono games in GBC/GBA mode
+ Has changes to the menu's interface
+ Others

v1.17 (2005/05/27)
+ Improved Japanese file sorting algorithm (AC)
+ Performance enhancements (syn-z)
+ Sound Buffer Length selection (syn-z)
+ Pausing of sound during quicksave / quickload (kmg)
+ Added "reset" option
+ Altered timing of setting saving
+ "SAVE" folder automatic creation
+ Ability to change equipment type added 
    > IMPORTANT: Even if you select GBA, you will NOT be able to run GBA software
    > This option is designed for GBC games that detected/offered special items if played on a GBA (Zelda, etc.)
+ Others

v1.16 (2005/05/26)
+ Added full screen mode (panda)
+ x2(doublesize) mode became x2(fit) mode (panda)
+ SRAM SAVE is perfectly automated
+ Fixed bug when VSYNC was OFF and you were unable to exit the program properly
+ Others

v1.15 (2005/05/24)
+ Multiple Save States (selectable via menu)
+ Fixed a bug in the Timer Function
+ Hitting a shortcut key combination will output the message associated with the command
+ Others

v1.141 (2005/05/23)
+ Fixed error accessing memory stick if sleep function is used

v1.14 (2005/05/23)
+ Analog pad sensitivity adjustments (kmg)
+ Control Motion Sensor via analog pad (Tilt N Tumble, etc.)
+ Adjusted key configuration's shortcut key input
+ Added "AnalogPad to D-Pad" option in key config

v1.13 (2005/05/22)
+ Performance Improvements (LCK & ruka)
+ ZIP File support (henoheno & ruka)
+ Analog pad support (kmg)
+ Various items added to key config
+ Ability to define simultaneous button presses in key config
+ Others

v1.12 (2005/05/20)
+ Timer functions added
+ Wait Stepping On/Off via button
+ Performance Improvements (LCK & ruka)
+ 3 Screen Modes added (z-wrt)
   > x2 (scanline) - Scanline draw every 4 lines
   > x2 (uncropped without top) - doublesized display with top 16 pixels omitted
   > x2 (uncropped without bottom) - doublesized displaye with bottom 16 pixels omitted
+ Others

v1.11 (2005/05/19)
+ Wait Step
+ Wallpaper brightness adjustment (by Natakanin [??])
+ Speed Up adjustments (syn-z & ruka)
+ Others

v1.1 (2005/05/17)
+ Sound Support
+ GUI Color Setting (NOT GB Palette editing)
+ Automatic save file creation
+ Auto-SRAM saving (by Smiths)
+ 1.5x (filtered) screen size　(by kwn)
+ Key Config　(by kwn)
+ Wallpaper Support (by kwn)
+ AB Button Rapid Fire Support (by kwn)
+ Configuration Saving　(by kwn)
+ HOME Button Exit Support (by mh_)
+ Other changes (can't remember)

v1.0 (2005/05/14)
+ Manual SRAM Saving
+ 2x Stretch mode

v0.9 (2005/05/13)
+ ROM Image selection
+ 2x (double-size) support
+ VSYNC ON/OFF Selection
+ Button configuration changes

---[Disclaimer]---
Use at your own responsibility. Distribute freely.


/* ------------------------------------------------
　by Mirakichi
　http://mirakichi.hp.infoseek.co.jp/software/
------------------------------------------------- */

