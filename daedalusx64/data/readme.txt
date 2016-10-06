Daedalusx64 Pre-Beta 3 Readme File
Copyright (C) 2006-2009 StrmnNrmn
Copyright (C) 2008-2010 DaedalusX64 Team

This document last edited 20 May 2010 by Wally

What is Daedalus?
*****************

Daedalus is a Nintendo 64 emulator for Windows and PSP. Daedalus
is named after the craftsman at King Minos's court who designed
the labyrinth for the Minotaur.

Getting the Latest Version
**************************
Visit http://sourceforge.net/projects/daedalusx64/ and if you can compile you may compile the latest alpha otherwise source it off the net elsewhere.


About this Release
******************

This release of DaedalusX64 is currently in beta stages
of development. 

As for the past releases of Daedalus PSP, this project is intended to show the potential for a N64 emulator on the PSP, but the current release has many missing
features that make it far from usable:

* Many roms won't boot.
* Some roms have serious graphical glitches.
* Various roms have random lockups and crashes.
* Many roms run at a low framerate.

Having said all that, We believe most of these problems can be
overcome in the near future.


Compatibility List / Forums 
http:// compat.daedalusx64.com / http://www.daedalusx64.com
***************************

Please check the compatibility list frequently as changes may be made to specific versions which could mean your favourite game might start working.

We would also like you to register on the forums which would give you many benefits such as:
a) ability to update the compatibility list and report bugs.
b) ability to possibly join the team as a tester / developer if interested.
c) tell us any ideas you may have for the emulator.
d) join a great community.



Using Daedalusx64
**************

Installation
------------

Copy the Daedalus folder to the \PSP\GAME\ folder on your PSP.

Daedalus requires Custom Firmware 4.00 or higher, failing to install CFW 4.00 or higher will mean Daedalus does not load past the firmware check screen.


Upgrade Procedure
------------

Get the latest build from subversion / compiled in mirror
Remove all HLE files from the SaveGames folder
Remove rom.db and preferences.ini 

This will ensure that Daedalus is fresh.

Roms
----

Roms can be placed in two locations, either the Daedalus Folder Rom directory or you can create a directory in the root of the memory stick (eg: F:\N64\).

Daedalus supports most rom formats (.v64, .z64, .rom etc)

We support zipped roms, however if zipped the rom will extract itself onto the memory stick anyway so it defeats the purpose of having a zipped rom. If you are running low on space, it is recommended that you extract the rom you want into the Rom Directory.


Preview Pictures
----------------

All previews are included in a stable release or on subversion.

Preview pictures can be found in Resources\Preview which you can modify to your liking.
The Pictures are required to be in .png format, in a 4:3 aspect ratio.

To let Daedalusx64 know which picture to use for each rom, you need to
add a line to the corresponding entry in the main roms.ini file,
with this format:

Preview=<filename.png>

Save Games
----------

All the save game types are currently supported, if you have issues saving any game,
check the roms.ini and make sure the save type is correct.

Save games are created with the same name as the rom file, in the
Daedalusx64/SaveGames/ directory.

IMPORTANT NOTE: For performance reasons Daedalus only saves out
modified save game files when the Pause menu is accessed (by pressing
the 'home' button while the emulator is running). I'll look at removing
this restriction ASAP.

Save States
----------

All the save states are created using this name format : SaveSlotXX.ss ( XX can be from 0 to 15 )
Save States are saved by default at Daedalusx64/SaveStates/<gamename>/
But you can also create this directory ms0:/n64/SaveSates and Daedalus will use this directory
for save state creation and loading. 

In total there's 16 slots for save states, each game has its own 16 slots to choose.
You can delete any save state easily, just choose the slot you want to delete and press square and then press triangle to confirm.

Main Menu
---------

When you first run DaedalusX64 you will be presented with a nice coverflow menu (Devs will get a listing)
to select a game you will press DPad left or right then X to select a title, 
there's various options you can choose from in the menu, every option is explained.
To cycle through options screens use the left and right shoulder buttons

Pause Menu
----------

When a rom is running, you can access the Pause Menu by pressing the 
'Home' button.

From the Pause Menu you can use the left and right shoulder buttons to
access various option screens. You can use the Pause Menu to take screenshots
and reset the emulator to the main menu. Screenshots are saved under
the Dumps/<gamename>/ScreenShots/ directory in the Daedalus folder on your
memory stick.

You can press the 'Home' button again to quickly return to the emulator.

Controls
--------

When and indeed if :) the rom runs, the following controls are mapped
by default:

N64					PSP
Start				Start
Analogue Stick		Analogue Stick
Dpad				O (Circle) + Dpad
A					X (Cross)
B					[] (Square)
Z					^ (Triangle)
L Trigger			L Trigger
R Trigger			R Trigger
C buttons			Dpad (Circle unpressed)
Menu 				Home

Daedalusx allows user-configurable controls to be specified.
The desired controls can be chosen from the Rom Settings screen.

In order to define your own controller configuration you need to add a 
new .ini file to the Daedalus/ControllerConfigs directory. There are a
few examples provided which should give an overview of what is possible.


Helping us Develop Daedalusx64
******************************
Join us on the forums, post detailed logs for your title of choice, its recommended you include screenshots, save states and if possible debug logs.

To be rewarded official tester Status, you must remain active and post useful, clear and precise information for everyone to read then elected by staff.


Support, Comments, Chat 
***********************

www.DaedalusX64.com

irc.freenode.net #daedalusx64

Credits
*******

A big thanks to all our testers, developers and the believers for giving us a hand when we need it.

A huge thanks to the pspdev guys for all their work.

A huge thanks to StrmnNrmn for getting the ball rolling with this excellent emulator and for providing tips to our developers when he can. StrmnNrmn leads a busy life and due to that he sadly can not contribute to the emulator himself.

Another huge thanks to Rice and the Glide2X Napalm team for giving us access to source code and their brains.



StrmnNrmns Credits:
The Audio HLE code used in Daedalus was adapted from Azimer's great
plugin for PC-based emulators. Thanks Azimer! Drop me a line!

A special thanks to everyone who was involved with Daedalus on the PC in 
the past. Sorry I was so rubbish at keeping the project ticking along. You
know who you are.

Hello to everyone I used to know in the N64 emulation scene back in 2001 -
hope you are all doing well!


Many thanks to 71M for giving me the inspiration to get this ported over
and lots of pointers along the way.

A big hello to the Super Zamoyski Bros :)

Many thanks to all the people who suggested using the Circle button to 
toggle between the Dpad and C buttons. I received over two dozen emails and
comments suggesting this approach, so thanks for that :)

Many thanks to hlide and Raphael in the PS2Dev forums for advice on
various VFPU issues.

Many thanks to Exophase and laxer3A for their continued input.

Thanks to Lkb for his various improvements to the PC build. The current 
savestate support is derived from his work.

Copying
*******

Daedalus is released under the GNU General Public License,
and the sourcecode is available from the sourceforge site. 

Disclaimer
**********

The Daedalus distribution comes with absolutely no warranty of any kind.

