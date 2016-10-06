/*
 ===================================================================================
		Cygne WIN  v 2.1a      (c) Dox 2002     dox@space.pl
 ===================================================================================

   NEC cpu core  by Bryan McPhail,Oliver Bergmann, Fabrice Frances and David Hedley
   Zlib  by  Jean-loup Gailly and Mark Adler

 ===================================================================================
*/


#define MAIN_KEY	  "Software\\Cygne"
#define KEY_UP1		  "H_UP"
#define KEY_DOWN1     "H_DOWN"
#define KEY_LEFT1     "H_LEFT"
#define KEY_RIGHT1    "H_RIGHT"
#define KEY_UP2		  "V_UP"
#define KEY_DOWN2     "V_DOWN"
#define KEY_LEFT2     "V_LEFT"
#define KEY_RIGHT2    "V_RIGHT"
#define KEY_START     "START"
#define KEY_A		  "A"
#define KEY_B	      "B"
#define KEY_ROTATE    "ROTATE"
#define KEY_LINK	  "LINK CONTROLS"


#define wREGISTRY(name,value)\
		if (RegConnectRegistry(NULL,HKEY_CURRENT_USER, &hKey1) == ERROR_SUCCESS)\
        {\
          strcpy(Buffer, MAIN_KEY);\
          rc = RegOpenKey(hKey1,Buffer,&hKey2);\
          if(rc != ERROR_SUCCESS) rc = RegCreateKey(hKey1,Buffer,&hKey2);\
          if(rc == ERROR_SUCCESS){ RegSetValueEx( hKey2, name, 0, REG_DWORD, (LPBYTE)&value, cbData);RegCloseKey(hKey2);}\
          RegCloseKey(hKey1);\
        }



#include <windows.h>
#include "globals.h"


int ReadRegistryStrVal(char* MainKey, char* Field);

void ReadRegistry(void)
{
   
	int wart;
	wart=ReadRegistryStrVal(MAIN_KEY, KEY_UP1);
	if(wart==255)
		wsDefaultKeys();
	else
	{
		k_up1i=wart;
		k_up2i=ReadRegistryStrVal(MAIN_KEY, KEY_UP2);
		k_down1i=ReadRegistryStrVal(MAIN_KEY, KEY_DOWN1);
		k_down2i=ReadRegistryStrVal(MAIN_KEY, KEY_DOWN2);
		k_left1i=ReadRegistryStrVal(MAIN_KEY, KEY_LEFT1);
		k_left2i=ReadRegistryStrVal(MAIN_KEY, KEY_LEFT2);
		k_right1i=ReadRegistryStrVal(MAIN_KEY, KEY_RIGHT1);
		k_right2i=ReadRegistryStrVal(MAIN_KEY, KEY_RIGHT2);
		k_ai=ReadRegistryStrVal(MAIN_KEY, KEY_A);
		k_bi=ReadRegistryStrVal(MAIN_KEY, KEY_B);
		k_starti=ReadRegistryStrVal(MAIN_KEY, KEY_START);
		k_flipi=ReadRegistryStrVal(MAIN_KEY, KEY_ROTATE);
		link_controls=ReadRegistryStrVal(MAIN_KEY, KEY_LINK);
	}


	WriteRegistry();
}


char Data[256];


int ReadRegistryStrVal(char* MainKey, char* Field)
{
	HKEY  hKey1, hKey2;
	DWORD rc;

	DWORD cbData, dwType;
	DWORD value;
  
	if(RegConnectRegistry(NULL,HKEY_CURRENT_USER, &hKey1) == ERROR_SUCCESS)
	{
		char    Buffer[260];
    
		strcpy(Buffer, MainKey);
    
		rc = RegOpenKey(hKey1, Buffer, &hKey2);
		if(rc == ERROR_SUCCESS)
		{
			cbData = sizeof(Data);
			rc = RegQueryValueEx( hKey2, Field, NULL, &dwType, (LPBYTE)&value, &cbData);
			RegCloseKey(hKey2);
		}
		RegCloseKey(hKey1);
	}

	if (rc==ERROR_SUCCESS && cbData!=0)
		return value;
	else
		return 255;
	
}


void WriteRegistry(void)
{
	    HKEY    hKey1, hKey2;
        DWORD   rc;
        DWORD	cbData=4;
		char	Buffer[300];

        wREGISTRY(KEY_UP1,k_up1i);			  
		wREGISTRY(KEY_UP2,k_up2i);			  
		wREGISTRY(KEY_DOWN1,k_down1i);		  
		wREGISTRY(KEY_DOWN2,k_down2i);	
		wREGISTRY(KEY_LEFT1,k_left1i);	
		wREGISTRY(KEY_LEFT2,k_left2i);	
		wREGISTRY(KEY_RIGHT1,k_right1i);	  
		wREGISTRY(KEY_RIGHT2,k_right2i);	
		wREGISTRY(KEY_A,k_ai);	
		wREGISTRY(KEY_B,k_bi);	
		wREGISTRY(KEY_START,k_starti);			
		wREGISTRY(KEY_ROTATE,k_flipi);	
		wREGISTRY(KEY_LINK,link_controls);
}