/*
 ===================================================================================
		Cygne WIN  v 2.1a      (c)Dox 2002     dox@space.pl
 ===================================================================================

   NEC cpu core  by Bryan McPhail,Oliver Bergmann, Fabrice Frances and David Hedley
   Zlib  by  Jean-loup Gailly and Mark Adler

 ===================================================================================
*/

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <windows.h>
#include <commctrl.h>
#include <math.h>
#include <mmsystem.h>
#include "resource.h"
#include "globals.h"
#include "gfx.h"
#include "memory.h"
#include "start.h"


HWND		window;
HINSTANCE	instance;
char		classnm[]="cygne class";

int 		wsc;			/*color/mono*/
BOOL		f_load;			/*rom load request*/	
BOOL		f_stopped;		/*emulation stopped*/
DWORD		flipd;			/*screen flip*/
int		    wsShades;		/*colorset for mono games*/
DWORD		wsCycles;		/*executed cpu cycles(not machine)*/
BYTE		wsLine;			/*current scanline*/
DWORD		wsSkip;			/*if executed cycles > cycles per line (eg big MOVSW) then redraw wsSkip lines( should be wsSkipped ;))*/
DWORD		l_f,c_f;		/*lastframe time , current frame time*/
DWORD 		cycles_line=677; /*670*/
DWORD		rom_size;
BYTE 		vsync;
const DWORD tabkey[47]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,VK_SPACE,VK_UP,VK_DOWN,VK_LEFT,VK_RIGHT,VK_TAB,VK_SHIFT,VK_DELETE,VK_INSERT,VK_HOME,VK_END};

DWORD k_up1,k_down1,k_left1,k_right1,k_up2,k_down2,k_left2,k_right2,k_a,k_b,k_start,k_up1i,k_down1i,k_left1i,k_right1i;
DWORD k_up2i,k_down2i,k_left2i,k_right2i,k_ai,k_bi,k_starti,k_flipi,link_controls;


#define mMENUCOLORSET(OPTION1,OPTION2,OPTION3,OPTION4)               \
		CheckMenuItem(hMenu,ID_OPTIONS_VIDEO_COLORSET_GREY,OPTION1); \
		CheckMenuItem(hMenu,ID_OPTIONS_VIDEO_COLORSET_RED,OPTION2);  \
		CheckMenuItem(hMenu,ID_OPTIONS_VIDEO_COLORSET_BLUE,OPTION3); \
		CheckMenuItem(hMenu,ID_OPTIONS_VIDEO_COLORSET_GREEN,OPTION4);
 
#define mMENUFRAMES(OPTION1,OPTION2,OPTION3,OPTION4,OPTION5,OPTION6)  \
		CheckMenuItem(hMenu,ID_OPTIONS_VIDEO_FRAMESKIP_0,OPTION1); \
		CheckMenuItem(hMenu,ID_OPTIONS_VIDEO_FRAMESKIP_1,OPTION2);    \
		CheckMenuItem(hMenu,ID_OPTIONS_VIDEO_FRAMESKIP_2,OPTION3);    \
		CheckMenuItem(hMenu,ID_OPTIONS_VIDEO_FRAMESKIP_3,OPTION4);    \
		CheckMenuItem(hMenu,ID_OPTIONS_VIDEO_FRAMESKIP_4,OPTION5);    \
		CheckMenuItem(hMenu,ID_OPTIONS_VIDEO_FRAMESKIP_5,OPTION6);

#define mMENUSIZE(OPTION1,OPTION2,OPTION3,OPTION4)			   \
		CheckMenuItem(hMenu,ID_OPTIONS_VIDEO_SIZE_X1,OPTION1); \
		CheckMenuItem(hMenu,ID_OPTIONS_VIDEO_SIZE_X2,OPTION2); \
		CheckMenuItem(hMenu,ID_OPTIONS_VIDEO_SIZE_X3,OPTION3); \
		CheckMenuItem(hMenu,ID_OPTIONS_VIDEO_SIZE_X4,OPTION4); 
	


void refresh_menu_colors(void)
{
	 HMENU hMenu=GetMenu(window);
	  switch (wsShades)
	  {
		case 0: mMENUCOLORSET(MF_CHECKED,MF_UNCHECKED,MF_UNCHECKED,MF_UNCHECKED); break;
		case 1: mMENUCOLORSET(MF_UNCHECKED,MF_CHECKED,MF_UNCHECKED,MF_UNCHECKED); break;
		case 2: mMENUCOLORSET(MF_UNCHECKED,MF_UNCHECKED,MF_CHECKED,MF_UNCHECKED); break;
		case 3: mMENUCOLORSET(MF_UNCHECKED,MF_UNCHECKED,MF_UNCHECKED,MF_CHECKED); break;
	 }
}

void frames(int n)
{
	HMENU hMenu=GetMenu(window);	 
	frameskip=n+1;
	switch (n)
	{
		case 0: mMENUFRAMES(MF_CHECKED,MF_UNCHECKED,MF_UNCHECKED,MF_UNCHECKED,MF_UNCHECKED,MF_UNCHECKED);break;
		case 1: mMENUFRAMES(MF_UNCHECKED,MF_CHECKED,MF_UNCHECKED,MF_UNCHECKED,MF_UNCHECKED,MF_UNCHECKED);break;
		case 2: mMENUFRAMES(MF_UNCHECKED,MF_UNCHECKED,MF_CHECKED,MF_UNCHECKED,MF_UNCHECKED,MF_UNCHECKED);break;
		case 3: mMENUFRAMES(MF_UNCHECKED,MF_UNCHECKED,MF_UNCHECKED,MF_CHECKED,MF_UNCHECKED,MF_UNCHECKED);break;
		case 4: mMENUFRAMES(MF_UNCHECKED,MF_UNCHECKED,MF_UNCHECKED,MF_UNCHECKED,MF_CHECKED,MF_UNCHECKED);break;
		case 5: mMENUFRAMES(MF_UNCHECKED,MF_UNCHECKED,MF_UNCHECKED,MF_UNCHECKED,MF_UNCHECKED,MF_CHECKED);break;
	}
}


void set_size(int refresh)
{
 HMENU	hMenu=GetMenu(window);	 
 RECT	wind;
 switch (screen_size)
 {
	case 1:	mMENUSIZE(MF_CHECKED,MF_UNCHECKED,MF_UNCHECKED,MF_UNCHECKED);break;
	case 2: mMENUSIZE(MF_UNCHECKED,MF_CHECKED,MF_UNCHECKED,MF_UNCHECKED);break;
	case 3: mMENUSIZE(MF_UNCHECKED,MF_UNCHECKED,MF_CHECKED,MF_UNCHECKED);break;
	case 4: mMENUSIZE(MF_UNCHECKED,MF_UNCHECKED,MF_UNCHECKED,MF_CHECKED);break;
 }
 

 if(!refresh) return;


 if(!flipd)
 {
	 wind.top=0;wind.left=0;wind.right=224*screen_size;wind.bottom=144*screen_size;
	 AdjustWindowRect(&wind,WS_SIZEBOX|WS_OVERLAPPEDWINDOW|WS_VISIBLE,TRUE);
	 SetWindowPos(window,NULL,0,0,wind.right-wind.left,wind.bottom-wind.top,SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);
 }
 else
 {
	wind.top = 0;wind.left = 0;wind.right = 144*screen_size;wind.bottom = 224*screen_size;
	AdjustWindowRect(&wind,WS_SIZEBOX|WS_OVERLAPPEDWINDOW|WS_VISIBLE,TRUE);
	SetWindowPos(window,NULL,0,0,wind.right-wind.left,wind.bottom-wind.top,SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);

 }
}










void wsReset(void)
{
	int		u0,u1;

	nec_reset(NULL);				/* Reset CPU */
	memset(&wsRAM,0,65536);			/* Clear  RAM  (it's filled with 0 after reset)*/

	for(u0=0;u0<256;u0++)wsIO[u0]=startio[u0];
	for(u0=0;u0<16;u0++)for(u1=0;u1<16;u1++)wsCols[u0][u1]=0;
	for(u0=0;u0<0xc9;u0++)cpu_writeport(u0,startio[u0]);


    wsCycles=0;
	wsLine=0;
	
	wsSetVideo(0,TRUE);
	l_f=0;
	nec_set_reg(NEC_SS,0);
	nec_set_reg(NEC_SP,0x2000);
}



void wsExecuteLine()
{
/* Should be
   Line $00  - <Line  interrupt check>.........<HBL Interrupt check>....	
    ...
   Line $8F  - <Line  interrupt check>.........<HBL Interrupt check>....	
   Line $90  - <VBL Start Interrupt check>.<<Line  interrupt check>.........<HBL Interrupt check>....	
   Line $90  - <<Line  interrupt check>.........<HBL Interrupt check>....	
    ...
   Last Line - <<Line  interrupt check>.........<HBL Interrupt check>....<VBL End Interrupt check>
*/
	
	
	unsigned		uI;
	if(f_load){f_load=0;wsROMLoad(window);}
	if(!f_stopped)
	{
		wsIO[2]=wsLine;
		wsCycles=nec_execute(cycles_line+(rand()&7));
		if(wsCycles>=cycles_line+cycles_line)
			wsSkip=wsCycles/cycles_line;
		else
			wsSkip=1;
		wsCycles%=cycles_line;
	    for(uI=0;uI<wsSkip;uI++)
		{
		   wsScanline();
		   wsLine++;
		   if(wsLine==144)wsMakeScr=1;

		}
		if(wsLine>158)
		{
			wsLine=0;
			{
				if((wsIO[0xb2]&32))/*VBLANK END INT*/ 
				{
					if(wsIO[0xa7]!=0x35)/*Beatmania Fix*/
					{
						wsIO[0xb6]&=~32;
						nec_int((wsIO[0xb0]+5)*4);
					}
				}
			}
		}
		wsIO[2]=wsLine;
		if(wsMakeScr)
		{
		
			if(wsIO[0xb2]&64) /*VBLANK INT*/
			{
				wsIO[0xb6]&=~64;
				nec_int((wsIO[0xb0]+6)*4);
			}
			fr=(fr+1)%frameskip;
			if(!fr)	
			{
				UpdateFrame(window,flipd);
			}
			if(vsync)
			{
				if(!l_f)l_f=timeGetTime();
				c_f=timeGetTime();
				while((c_f-l_f)<(12))c_f=timeGetTime();
				l_f=c_f;
			}
			wsMakeScr=0;
		}
       if(wsIO[0xa4]&&(wsIO[0xb2]&128)) /*HBLANK INT*/
	   {
		  
		  if(!wsIO[0xa5])wsIO[0xa5]=wsIO[0xa4];
		  if(wsIO[0xa5]) wsIO[0xa5]--;
		  if((!wsIO[0xa5])&&(wsIO[0xb2]&128))
		  {
		
			  wsIO[0xb6]&=~128;
			  nec_int((wsIO[0xb0]+7)*4);
	
		  }
	  }
	  if((wsIO[0x2]==wsIO[0x3])&&(wsIO[0xb2]&16)) /*SCANLINE INT*/
	  {	
			wsIO[0xb6]&=~16;
			nec_int((wsIO[0xb0]+4)*4);
	  }	 
	}
}



LRESULT CALLBACK aboutproc(HWND window1,UINT Message,WPARAM wparam,LPARAM lparam){if((Message==WM_COMMAND)&&(HIWORD(wparam)==BN_CLICKED)&&((int)LOWORD(wparam)==IDOK)){EndDialog(window1,TRUE);return TRUE;}return FALSE;}

LRESULT CALLBACK wndProc(HWND window1,UINT Message,WPARAM wparam,LPARAM lparam)
{
	HMENU	menu;
	switch(Message)
       {
		case WM_COMMAND:
			switch(LOWORD(wparam))
			{
				case ID_FILE_EXIT:saveSRAM();PostMessage(window1, WM_DESTROY, 0, 0);return 0;
				case ID_FILE_SAVESTATE:wsSaveState();break;
				case ID_FILE_LOADSTATE:wsLoadState();break;
				case ID_OPTIONS_CONTROLS:DialogBox(instance,MAKEINTRESOURCE(IDD_DIALOG4),window,cproc);break;
				case ID_OPTIONS_WSCEMULATION:
					menu=GetMenu(window);wsc^=1;
					if(wsc)
						CheckMenuItem(menu,ID_OPTIONS_WSCEMULATION,MF_CHECKED);
					else
						CheckMenuItem(menu,ID_OPTIONS_WSCEMULATION,MF_UNCHECKED);
					saveSRAM();
					wsReset();
					wsSetVideo(0,TRUE);
					break;
				case ID_OPTIONS_VIDEO_SYNCHRONIZE:
					menu=GetMenu(window);
					vsync^=1;
					if (vsync)
						CheckMenuItem(menu,ID_OPTIONS_VIDEO_SYNCHRONIZE,MF_CHECKED);
					else
						CheckMenuItem(menu,ID_OPTIONS_VIDEO_SYNCHRONIZE,MF_UNCHECKED);
					break;
				case ID_OPTIONS_VIDEO_SIZE_X1:screen_size=1;set_size(1);break;
				case ID_OPTIONS_VIDEO_SIZE_X2:screen_size=2;set_size(1);break;
				case ID_OPTIONS_VIDEO_SIZE_X3:screen_size=3;set_size(1);break;
				case ID_OPTIONS_VIDEO_SIZE_X4:screen_size=4;set_size(1);break;
				case ID_FILE_RESET:wsReset();break;
				case ID_FILE_OPENROM:saveSRAM();f_load=TRUE;break;
				case ID_HELP_ABOUT:DialogBox(instance,MAKEINTRESOURCE(IDD_DIALOG3),window,(DLGPROC)aboutproc);break;
				case ID_OPTIONS_VIDEO_COLORSET_GREY:wsShades=0;set_shades();break;
				case ID_OPTIONS_VIDEO_COLORSET_RED:wsShades=1;set_shades();break;
				case ID_OPTIONS_VIDEO_COLORSET_BLUE:wsShades=2;set_shades();break;
				case ID_OPTIONS_VIDEO_COLORSET_GREEN:wsShades=3;set_shades();break;
				case ID_OPTIONS_VIDEO_FRAMESKIP_0:frames(0);break;
				case ID_OPTIONS_VIDEO_FRAMESKIP_1:frames(1);break;
				case ID_OPTIONS_VIDEO_FRAMESKIP_2:frames(2);break;
				case ID_OPTIONS_VIDEO_FRAMESKIP_3:frames(3);break;
				case ID_OPTIONS_VIDEO_FRAMESKIP_4:frames(4);break;
				case ID_OPTIONS_VIDEO_FRAMESKIP_5:frames(5);break;
				case ID_OPTIONS_VIDEO_ORIENTATION_HORIZONTAL:if(flipd)flip_screen();break;
				case ID_OPTIONS_VIDEO_ORIENTATION_VERTICAL:if(!flipd)flip_screen();break;
				case ID_OPTIONS_VIDEO_SAVESCREEN: screenshot();break;
			}
			break;
				     
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_KEYDOWN:
			if(wparam=='1'){cycles_line=391;char ttt[256];sprintf(ttt,"Cycles/line -> 391");SetWindowText(window,ttt);return 0;}
			if(wparam=='2'){cycles_line=574;char ttt[256];sprintf(ttt,"Cycles/line -> 574");SetWindowText(window,ttt);return 0;}
			if(wparam=='3'){cycles_line=837;char ttt[256];sprintf(ttt,"Cycles/line -> 837");SetWindowText(window,ttt);return 0;}
			if(wparam=='4'){cycles_line=677;char ttt[256];sprintf(ttt,"Cycles/line -> 677");SetWindowText(window,ttt);return 0;}
			if(wparam=='5'){if(cycles_line>30)cycles_line--;char ttt[256];sprintf(ttt,"Cycles/line -> %i",cycles_line);SetWindowText(window,ttt);return 0;}
			if(wparam=='6'){if(cycles_line<=12000)cycles_line++;char ttt[256];sprintf(ttt,"Cycles/line -> %i",cycles_line);SetWindowText(window,ttt);return 0;}
			if(wparam==tabkey[k_up1i]){k_up1=1;return 0;}
			if(wparam==tabkey[k_up2i]){k_up2=1;return 0;}
			if(wparam==tabkey[k_down1i]){k_down1=1;return 0;}
			if(wparam==tabkey[k_down2i]){k_down2=1;return 0;}
			if(wparam==tabkey[k_left1i]){k_left1=1;return 0;}
			if(wparam==tabkey[k_left2i]){k_left2=1;return 0;}
			if(wparam==tabkey[k_right1i]){k_right1=1;return 0;}	
			if(wparam==tabkey[k_right2i]){k_right2=1;return 0;}
			if(wparam==tabkey[k_ai]){k_a=1;return 0;}
			if(wparam==tabkey[k_bi]){k_b=1;return 0;}
			if(wparam==tabkey[k_starti]){k_start=1;return 0;}
			if(wparam==tabkey[k_flipi]){flip_screen();return 0;}
			if(wparam==VK_ESCAPE){saveSRAM();PostMessage(window1, WM_DESTROY, 0, 0);return 0;}
			break;

		case WM_KEYUP:
			if((wparam==112)||(wparam==80))f_stopped^=1;
		  	if(wparam==tabkey[k_up1i]){k_up1=0;return 0;}
			if(wparam==tabkey[k_up2i]){k_up2=0;return 0;}
			if(wparam==tabkey[k_down1i]){k_down1=0;return 0;}
			if(wparam==tabkey[k_down2i]){k_down2=0;return 0;}
			if(wparam==tabkey[k_left1i]){k_left1=0;return 0;}
			if(wparam==tabkey[k_left2i]){k_left2=0;return 0;}
			if(wparam==tabkey[k_right1i]){k_right1=0;return 0;}	
			if(wparam==tabkey[k_right2i]){k_right2=0;return 0;}
			if(wparam==tabkey[k_ai]){k_a=0;return 0;}
			if(wparam==tabkey[k_bi]){k_b=0;return 0;}
			if(wparam==tabkey[k_starti]){k_start=0;return 0;}
			if(wparam=='0')screenshot();
			break;
		default: return DefWindowProc(window1,Message,wparam,lparam);
        }
	return 0;
}

int WINAPI WinMain(HINSTANCE inst,HINSTANCE,LPSTR,int trybokna)
{
	
	wsc=1;
	screen_size=2;
	flipd=0;
	wsShades=0;
	wsCycles=0;
	wsLine=0;
	rom_size=128000000;
	wsMakeScr=0;
	fr=0;
	frameskip=1;
	wsVMode=-1;
	vsync=1;
	wsMakeTiles();

	InitCommonControls();
	MSG Message;
	instance=inst;
	WNDCLASS kokna;
	kokna.style=0;
	kokna.hInstance=instance;
	kokna.lpszClassName=classnm;
	kokna.lpfnWndProc=wndProc;
	kokna.hIcon=LoadIcon(instance,MAKEINTRESOURCE(IDI_ICON1));
	kokna.hCursor=LoadCursor(NULL,IDC_ARROW);
	kokna.lpszMenuName=MAKEINTRESOURCE(menu1);
	kokna.cbClsExtra=0;
	kokna.cbWndExtra=0;
	kokna.hbrBackground=(HBRUSH)(COLOR_WINDOW+2);
	if(!RegisterClass(&kokna)) return(-1);
	
	RECT	wind;
	wind.top = 0;
    wind.left = 0;
    wind.right = 224*2;
    wind.bottom = 144*2;
	AdjustWindowRect(&wind,WS_SIZEBOX|WS_OVERLAPPEDWINDOW|WS_VISIBLE,TRUE);
	window=CreateWindowEx(0,classnm,"Cygne",WS_SIZEBOX|WS_OVERLAPPEDWINDOW|WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,wind.right,wind.bottom,NULL,NULL,instance,NULL);
	ShowWindow(window,trybokna);
	UpdateWindow(window);
	
	GetModuleFileName(NULL,path_app,MAX_PATH);
	for(int j=strlen(path_app);j>0;j--)if((BYTE)(path_app[j])==92){	path_app[j+1]=0;break;}
	strcpy(path_save,path_app);
	strcat(path_save,"SRAM");
	SECURITY_ATTRIBUTES seca;
	seca.nLength=sizeof(SECURITY_ATTRIBUTES);
	seca.lpSecurityDescriptor=NULL;
	seca.bInheritHandle=TRUE;
	CreateDirectory(path_save,&seca);

	ReadRegistry();

	set_size(0);
	HMENU menu=GetMenu(window);	 

	if (wsc)CheckMenuItem(menu,ID_OPTIONS_WSCEMULATION,MF_CHECKED);else	CheckMenuItem(menu,ID_OPTIONS_WSCEMULATION,MF_UNCHECKED);
	if (vsync)CheckMenuItem(menu,ID_OPTIONS_VIDEO_SYNCHRONIZE,MF_CHECKED);else CheckMenuItem(menu,ID_OPTIONS_VIDEO_SYNCHRONIZE,MF_UNCHECKED);
	
	frames(frameskip-1);
	start_dx();
	set_shades();
	f_load=TRUE;

    while (TRUE) 
	 {
     if (PeekMessage(&Message,NULL,0,0,PM_NOREMOVE)) 
	 {
       if (!GetMessage(&Message,NULL,0,0))break;
       TranslateMessage(&Message);
       DispatchMessage(&Message);
     }
     else
       wsExecuteLine();
     }
	saveSRAM();
	Sleep(100);
	closedx();
	return 0;
}







