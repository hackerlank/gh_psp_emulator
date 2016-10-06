/*
 ===================================================================================
		Cygne WIN  v 2.1a      (c) Dox 2002     dox@space.pl
 ===================================================================================

   NEC cpu core  by Bryan McPhail,Oliver Bergmann, Fabrice Frances and David Hedley
   Zlib  by  Jean-loup Gailly and Mark Adler

 ===================================================================================
  Keyconfig  - keys configuration
 ===================================================================================
*/

#include <windows.h>
#include <stdio.h>
#include "globals.h"
#include "resource.h"


const DWORD dk_up1i=37;
const DWORD dk_down1i=38;
const DWORD dk_left1i=39;
const DWORD dk_right1i=40;

const DWORD dk_up2i=32;
const DWORD dk_down2i=28;
const DWORD dk_left2i=10;
const DWORD dk_right2i=13;

const DWORD dk_ai=35;
const DWORD dk_bi=33;
const DWORD dk_starti=36;
const DWORD dk_flipi=41;
const DWORD dlink_controls=1;


const char tabkeytext[47][8] = {"0","1","2","3","4","5","6","7","8","9","A","B","C",
"D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X",
"Y","Z","SPACE","UP","DOWN","LEFT","RIGHT","TAB","SHIFT","DEL","INSERT","HOME","END"};


void wsDefaultKeys(void)
{
	
	k_up1i=dk_up1i;
	k_up2i=dk_up2i;
	k_down1i=dk_down1i;
	k_down2i=dk_down2i;
	k_left1i=dk_left1i;
	k_left2i=dk_left2i;
	k_right1i=dk_right1i;
	k_right2i=dk_right2i;
	k_ai=dk_ai;
	k_bi=dk_bi;
	k_starti=dk_starti;
	k_flipi=dk_flipi;
	link_controls=dlink_controls;
}


DWORD key_combos[12]={
IDC_COMBO1,
IDC_COMBO2,
IDC_COMBO3,
IDC_COMBO4,
IDC_COMBO5,
IDC_COMBO6,
IDC_COMBO7,
IDC_COMBO8,
IDC_COMBO9,
IDC_COMBO10,
IDC_COMBO11,
IDC_COMBO12};


BOOL CALLBACK cproc(HWND dialog,UINT komunikat,WPARAM wparam,LPARAM lparam)
{
	int i,j;
	char tempstring[256];
	switch(komunikat)
	{
		case WM_INITDIALOG:
				for(i=0;i<47;i++)for(j=0;j<12;j++)SendDlgItemMessage(dialog,key_combos[j],CB_ADDSTRING,0,(LPARAM)&tabkeytext[i]);
				SendDlgItemMessage(dialog,IDC_COMBO1,CB_SETCURSEL,k_up1i,0);
				SendDlgItemMessage(dialog,IDC_COMBO2,CB_SETCURSEL,k_left1i,0);
				SendDlgItemMessage(dialog,IDC_COMBO3,CB_SETCURSEL,k_right1i,0);
				SendDlgItemMessage(dialog,IDC_COMBO4,CB_SETCURSEL,k_down1i,0);
				SendDlgItemMessage(dialog,IDC_COMBO5,CB_SETCURSEL,k_up2i,0);
				SendDlgItemMessage(dialog,IDC_COMBO6,CB_SETCURSEL,k_left2i,0);
				SendDlgItemMessage(dialog,IDC_COMBO7,CB_SETCURSEL,k_right2i,0);
				SendDlgItemMessage(dialog,IDC_COMBO8,CB_SETCURSEL,k_down2i,0);
				SendDlgItemMessage(dialog,IDC_COMBO9,CB_SETCURSEL,k_starti,0);
				SendDlgItemMessage(dialog,IDC_COMBO10,CB_SETCURSEL,k_ai,0);
				SendDlgItemMessage(dialog,IDC_COMBO11,CB_SETCURSEL,k_bi,0);
				SendDlgItemMessage(dialog,IDC_COMBO12,CB_SETCURSEL,k_flipi,0);
				sprintf(tempstring,"P");
				SendDlgItemMessage(dialog,IDC_EDIT1,WM_SETTEXT,0,(int)&tempstring);
				if(link_controls)
					SendDlgItemMessage(dialog,IDC_CHECK1,BM_SETCHECK,BST_CHECKED,0);
				else 
					SendDlgItemMessage(dialog,IDC_CHECK1,BM_SETCHECK,BST_UNCHECKED,0);
				break;
	
	case WM_COMMAND:
				if((HIWORD(wparam)==BN_CLICKED)&&(((int)LOWORD(wparam))==IDC_BUTTON1))
				{
					wsDefaultKeys();
					SendDlgItemMessage(dialog,IDC_COMBO1,CB_SETCURSEL,k_up1i,0);
					SendDlgItemMessage(dialog,IDC_COMBO2,CB_SETCURSEL,k_left1i,0);
					SendDlgItemMessage(dialog,IDC_COMBO3,CB_SETCURSEL,k_right1i,0);
					SendDlgItemMessage(dialog,IDC_COMBO4,CB_SETCURSEL,k_down1i,0);
					SendDlgItemMessage(dialog,IDC_COMBO5,CB_SETCURSEL,k_up2i,0);
					SendDlgItemMessage(dialog,IDC_COMBO6,CB_SETCURSEL,k_left2i,0);
					SendDlgItemMessage(dialog,IDC_COMBO7,CB_SETCURSEL,k_right2i,0);
					SendDlgItemMessage(dialog,IDC_COMBO8,CB_SETCURSEL,k_down2i,0);
					SendDlgItemMessage(dialog,IDC_COMBO9,CB_SETCURSEL,k_starti,0);
					SendDlgItemMessage(dialog,IDC_COMBO10,CB_SETCURSEL,k_ai,0);
					SendDlgItemMessage(dialog,IDC_COMBO11,CB_SETCURSEL,k_bi,0);
					SendDlgItemMessage(dialog,IDC_COMBO12,CB_SETCURSEL,k_flipi,0);
				}
				else
					if((HIWORD(wparam)==BN_CLICKED)&&(((int)LOWORD(wparam))==IDC_CHECK1))
					{
						link_controls^=1;
						if(link_controls)
							SendDlgItemMessage(dialog,IDC_CHECK1,BM_SETCHECK,BST_CHECKED,0);
						else 
							SendDlgItemMessage(dialog,IDC_CHECK1,BM_SETCHECK,BST_UNCHECKED,0);
					}
					else
						if((HIWORD(wparam)==BN_CLICKED)&&(((int)LOWORD(wparam))==IDOK))
						{
							k_up1i=SendDlgItemMessage(dialog,IDC_COMBO1,CB_GETCURSEL,0,0);
							k_up2i=SendDlgItemMessage(dialog,IDC_COMBO5,CB_GETCURSEL,0,0);
							k_down1i=SendDlgItemMessage(dialog,IDC_COMBO4,CB_GETCURSEL,0,0);
							k_down2i=SendDlgItemMessage(dialog,IDC_COMBO8,CB_GETCURSEL,0,0);
							k_left1i=SendDlgItemMessage(dialog,IDC_COMBO2,CB_GETCURSEL,0,0);
							k_left2i=SendDlgItemMessage(dialog,IDC_COMBO6,CB_GETCURSEL,0,0);
							k_right1i=SendDlgItemMessage(dialog,IDC_COMBO3,CB_GETCURSEL,0,0);
							k_right2i=SendDlgItemMessage(dialog,IDC_COMBO7,CB_GETCURSEL,0,0);
							k_ai=SendDlgItemMessage(dialog,IDC_COMBO10,CB_GETCURSEL,0,0);
							k_bi=SendDlgItemMessage(dialog,IDC_COMBO11,CB_GETCURSEL,0,0);
							k_starti=SendDlgItemMessage(dialog,IDC_COMBO9,CB_GETCURSEL,0,0);
							k_flipi=SendDlgItemMessage(dialog,IDC_COMBO12,CB_GETCURSEL,0,0);
							WriteRegistry();
							EndDialog(dialog,TRUE);
							return TRUE;
						}
						break;
	
	}
	return FALSE;
}
