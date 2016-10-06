/*
 ===================================================================================
		Cygne WIN  v 2.1a      (c) Dox 2002     dox@space.pl
 ===================================================================================

   NEC cpu core  by Bryan McPhail,Oliver Bergmann, Fabrice Frances and David Hedley
   Zlib  by  Jean-loup Gailly and Mark Adler

 ===================================================================================
  A bit slow ddraw code. Direct  use of primary surface (without backbuffer) would
  be faster, but less compatible with some strange  videocards.
 ===================================================================================
*/


#include <windows.h>
#include <ddraw.h>
#include <stdio.h>
#include "globals.h"
#include "resource.h"
#include "gfx.h"

#define k_m 13
#define k_w 15
#define k_s 29


LPDIRECTDRAW				g_pDD=NULL;       
LPDIRECTDRAWSURFACE			g_pDDSPrimary=NULL,g_pDDSOne=NULL;
LPDIRECTDRAWCLIPPER			g_pClipper=NULL;
LPDIRECTDRAWPALETTE			g_pDDPal=NULL;

DWORD						dx_bits,dx_pitch,cmov,dx_linewidth_blit,dx_buffer_line;
DWORD						dx_r,dx_g,dx_b,dx_sr,dx_sg,dx_sb;
DWORD						dx_r_old,dx_g_old,dx_b_old;
BYTE						dx_buffer[224*144*4];


BYTE header[54]={
66,77,54,122,1,0,0,0,0,0,54,0,
0,0,40,0,0,0,144,0,0,0,224,0,
0,0,1,0,24,0,0,0,0,0,0,122,
1,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0};


BYTE BMPTab[224*144*4];


DWORD filecount=0;
int vmodel=0;

void screenshot(void)
{
	BYTE br,bg,bb;
	BYTE d0,d1,d2,d3;
	DWORD add=0;
	DWORD mucho;
	
	unsigned long maxy,maxx;
	if(!vmodel)
	{
		header[0x12]=0xe0;
		header[0x16]=0x90;
		maxx=224;
		maxy=144;
	}
	else
	{
		header[0x12]=0x90;
		header[0x16]=0xe0;
		maxy=224;
		maxx=144;
	}
	char fname[256];
	sprintf(fname,"SCR%.5i.bmp",filecount++);
	FILE* fil=fopen(fname,"wb");
	fwrite(header,1,54,fil);
	
	unsigned long xx,yy;
	for(yy=0;yy<maxy;yy++)
	for(xx=0;xx<maxx;xx++)
	{
		
	
		if(dx_bits==2)
		{
			d0=dx_buffer[add++];
			d1=dx_buffer[add++];
			mucho=d0|(d1<<8);
		}
		else
		{
			d0=dx_buffer[add++];
			d1=dx_buffer[add++];
			d2=dx_buffer[add++];
			d3=dx_buffer[add++];
			mucho=d0|(d1<<8)|(d2<<16)|(d3<<24);
		}

		br=((mucho&dx_r_old)>>dx_sr)<<(8-dx_r);
		bg=((mucho&dx_g_old)>>dx_sg)<<(8-dx_g);
		bb=((mucho&dx_b_old)>>dx_sb)<<(8-dx_b);

		BMPTab[(xx+(maxy-yy-1)*maxx)*3]=bb;
		BMPTab[(xx+(maxy-yy-1)*maxx)*3+1]=bg;
		BMPTab[(xx+(maxy-yy-1)*maxx)*3+2]=br;
	}
	fwrite(BMPTab,1,224*144*3,fil);


	fclose(fil);


}



void set_shades (void)
{
	  DWORD		red,green,blue;
	  int		i;
		
	  refresh_menu_colors();
  	  switch(wsShades)
	  {
		case 0:
			for (i=0;i<16;i++)
			{
				red=(i*k_w+k_s)>>(8-dx_r);
				green=(i*k_w+k_s)>>(8-dx_g);
				blue=(i*k_w+k_s)>>(8-dx_b);
				colors[i]=(red<<dx_sr)|(green<<dx_sg)|(blue<<dx_sb);
			}
		break;
		case 1:
			for (i=0;i<16;i++)
			{
				red=(i*k_w+k_s)>>(8-dx_r);
				green=(i*k_m+k_s)>>(8-dx_g);
				blue=(i*k_m+k_s)>>(8-dx_b);
				colors[i]=(red<<dx_sr)|(green<<dx_sg)|(blue<<dx_sb);
			}
		break;
		case 2: 
			for (i=0;i<16;i++)
			{
				red=(i*k_m+k_s)>>(8-dx_r);
				green=(i*k_m+k_s)>>(8-dx_g);
				blue=(i*k_w+k_s)>>(8-dx_b);
				colors[i]=(red<<dx_sr)|(green<<dx_sg)|(blue<<dx_sb);
			}
		break;
		case 3:
			for (i=0;i<16;i++)
			{
				red=(i*k_m+k_s)>>(8-dx_r);
				green=(i*k_w+k_s)>>(8-dx_g);
				blue=(i*k_m+k_s)>>(8-dx_b);
				colors[i]=(red<<dx_sr)|(green<<dx_sg)|(blue<<dx_sb);
			}
		break;
	 }		
}


DWORD find1(DWORD data)
{
	DWORD	res=0;
	if(!data)  return 0;
	while(1) {  if(data&1)	return res;  data>>=1;  res++; }
}
				
DWORD count1(DWORD data)
{
	DWORD	res=0;
	for(int i=0;i<32;i++) {  if(data&1)   res++;  data>>=1; }
	return res;
}

void closedx(void)
{
	if(g_pDD!=NULL)
	{
		if(g_pDDSPrimary!=NULL) {  g_pDDSPrimary->Release();  g_pDDSPrimary=NULL; }
		if(g_pDDSOne!=NULL) {  g_pDDSOne->Release();  g_pDDSOne=NULL; }
		if(g_pDDPal!=NULL) {  g_pDDPal->Release();  g_pDDPal=NULL; }
		g_pDD->Release();
		g_pDD=NULL;
	}
}





HRESULT InitFail(HWND hWnd,HRESULT hRet,LPCTSTR szError,...)
{
	char szBuff[128];
	va_list vl;
	va_start(vl,szError);
	vsprintf(szBuff,szError,vl);
	closedx();
	MessageBox(hWnd,szBuff,"DDraw error",MB_OK);
	DestroyWindow(hWnd);
	va_end(vl);
	return hRet;
}


void UpdateFrame_h(HWND hWnd)
{
	RECT rcRect,destRect;
	HRESULT hRet;
	POINT pt;
	DDSURFACEDESC ddsd;

	rcRect.left =0;
	rcRect.top = 0;
	rcRect.right = 224;
	rcRect.bottom = 144;
	GetClientRect(hWnd, &destRect);
	pt.x=pt.y=0;
	ClientToScreen(hWnd,&pt);
	OffsetRect(&destRect,pt.x,pt.y);
	g_pClipper->SetHWnd(0,window);
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	g_pDDSOne->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL);
	if(ddsd.lpSurface!=NULL)
	 switch(dx_bits)
	 {
		 case 2:
	 	  _asm
			{
				lea		esi,dx_buffer
				mov		edi,ddsd.lpSurface
				mov		ecx,144
				mov		edx,dx_linewidth_blit
			loop2:
				mov		eax,edi
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
 				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
 				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]	
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		esi,[esi+32]
				mov		edi,eax
				lea		edi,[edi+edx]
				dec		ecx
				jnz		loop2
				emms
			 }
			break;

		 case 4:
			_asm
			{
				lea		esi,dx_buffer
				mov		edi,ddsd.lpSurface
				mov		ecx,144
				mov		edx,dx_linewidth_blit
loop1:
				mov		eax,edi
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
 				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
 				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]	
    			movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]	
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		esi,[esi+32]		
				mov		edi,eax
			    lea		edi,[edi+edx]
				dec		ecx
				jnz		loop1
				emms
			 }
	 }
	 g_pDDSOne->Unlock(NULL);
	 while(1)
	 {
		hRet=g_pDDSPrimary->Blt(&destRect,g_pDDSOne,&rcRect,0,NULL);
		if(hRet==DD_OK)	break;
		if(hRet==DDERR_SURFACELOST){ while(!(g_pDDSPrimary->Restore()==DD_OK&&g_pDDSOne->Restore()==DD_OK));return;}
		if(hRet != DDERR_WASSTILLDRAWING) return;
	 }
	if (hRet != DD_OK)
	return;
}

void UpdateFrame_v(HWND hWnd)
{
	RECT rcRect,destRect;
	HRESULT hRet;
	POINT pt;
	DDSURFACEDESC ddsd;
	
	rcRect.left=0;
	rcRect.top=0;
	rcRect.right=144;
	rcRect.bottom=224;
	GetClientRect(hWnd, &destRect);
	pt.x=pt.y=0;
	ClientToScreen(hWnd,&pt);
	OffsetRect(&destRect,pt.x,pt.y);
	g_pClipper->SetHWnd(0,window);
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	g_pDDSOne->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL);
	if(ddsd.lpSurface!=NULL)
	 switch(dx_bits)
		{
			case 2:
		 	  _asm
			  {
				lea		esi,dx_buffer
				mov		edi,ddsd.lpSurface
				mov		ecx,224
				mov		edx,dx_linewidth_blit
loop2:
				mov		eax,edi
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
 				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
 				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3	
				lea		esi,[esi+32]		
				mov		edi,eax
				lea		edi,[edi+edx]
				dec		ecx
				jnz		loop2
				emms
			}
			break;
		case	4:
			_asm
			{
				lea		esi,dx_buffer
				mov		edi,ddsd.lpSurface
				mov		ecx,224
				mov		edx,dx_linewidth_blit
loop1:
				mov		eax,edi
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
 				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
 				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
			 	movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
				movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3
				lea		edi,[edi+32]
				lea		esi,[esi+32]		
		 		movq	mm0,[esi]
				movq	mm1,[esi+8]
				movq	mm2,[esi+16]
				movq	mm3,[esi+24]
				movq	[edi],mm0
				movq	[edi+8],mm1
				movq	[edi+16],mm2
				movq	[edi+24],mm3	
				lea		esi,[esi+32]		
				mov		edi,eax
				lea		edi,[edi+edx]
				dec		ecx
				jnz		loop1
				emms
			 }
			 break;
		}
	 g_pDDSOne->Unlock(NULL);
	 while(1)
	 {
		hRet=g_pDDSPrimary->Blt(&destRect,g_pDDSOne,&rcRect,0,NULL);
		if(hRet==DD_OK) break;
		if(hRet==DDERR_SURFACELOST){while(!(g_pDDSPrimary->Restore()==DD_OK&&g_pDDSOne->Restore()==DD_OK)); return;}
		if(hRet != DDERR_WASSTILLDRAWING) return;
	 }
	if (hRet != DD_OK)
	return;
}

int start_dx_h(void)
{
	LPDIRECTDRAW pDD;
	DDSURFACEDESC ddsd3;
	DDSURFACEDESC ddsd;
	DDSURFACEDESC ddsd2;

	HRESULT hRet=DirectDrawCreate(NULL,&pDD,NULL);
	if(hRet!=DD_OK)
		return InitFail(window,hRet,"DirectDrawCreate FAILED");
	hRet=pDD->QueryInterface(IID_IDirectDraw,(LPVOID *)&g_pDD);
	if(hRet!=DD_OK)
	    return InitFail(window,hRet,"QueryInterface FAILED");
	hRet=g_pDD->SetCooperativeLevel(window,DDSCL_NORMAL);
	if(hRet!=DD_OK)
		return InitFail(window,hRet,"SetCooperativeLevel FAILED");
	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags=DDSD_CAPS;
	ddsd.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE;
	hRet=g_pDD->CreateSurface(&ddsd,&g_pDDSPrimary,NULL);
	if(hRet!=DD_OK)
		return InitFail(window,hRet,"CreateSurface FAILED");
	hRet=g_pDD->CreateClipper(0,&g_pClipper,NULL);
	if(hRet!=DD_OK)
		return InitFail(window,hRet,"CreateClipper FAILED");
	hRet=g_pClipper->SetHWnd(0,window);
	if(hRet!=DD_OK)
		return InitFail(window,hRet,"SetHWnd FAILED");
	hRet=g_pDDSPrimary->SetClipper(g_pClipper);
	if(hRet!=DD_OK)
		return InitFail(window,hRet,"SetClipper FAILED");
	ZeroMemory(&ddsd3, sizeof(ddsd));
	ddsd3.dwSize=sizeof(ddsd);
	ddsd3.dwFlags=DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH;
	ddsd3.ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN;
	ddsd3.dwWidth=224;
	ddsd3.dwHeight=144;
	if(g_pDD->CreateSurface(&ddsd3,&g_pDDSOne,NULL)!=DD_OK)
		return InitFail(window,hRet,"CBuffer FAILED");
	memset(&ddsd2,0,sizeof(ddsd2));
	ddsd2.dwSize =sizeof(ddsd2);
	ddsd2.dwFlags=DDSD_PIXELFORMAT ;
	Sleep(1); 
	g_pDDSOne->Lock(NULL,&ddsd2,DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT,NULL);
	dx_bits=ddsd2.ddpfPixelFormat.dwRGBBitCount; 
	dx_r=ddsd2.ddpfPixelFormat.dwRBitMask; 
	dx_g=ddsd2.ddpfPixelFormat.dwGBitMask; 
	dx_b=ddsd2.ddpfPixelFormat.dwBBitMask; 
	dx_pitch=ddsd2.lPitch;
	cmov=ddsd2.lPitch/4;
	dx_bits/=8;
	dx_sr=find1(dx_r);
	dx_sg=find1(dx_g);
	dx_sb=find1(dx_b);

	dx_r_old=dx_r;
	dx_g_old=dx_g;
	dx_b_old=dx_b;


	dx_r=count1(dx_r);
	dx_g=count1(dx_g);
	dx_b=count1(dx_b);
	g_pDDSOne->Unlock(NULL);
	dx_linewidth_blit=ddsd2.lPitch;
	dx_buffer_line=61*dx_bits;
	set_shades();
	return	0;
}




int start_dx_v(void)
{
	LPDIRECTDRAW pDD;
	DDSURFACEDESC ddsd3;
	DDSURFACEDESC ddsd;
	DDSURFACEDESC ddsd2;
	HRESULT hRet=DirectDrawCreate(NULL,&pDD,NULL);
	
	if(hRet!=DD_OK)
		return InitFail(window,hRet,"DirectDrawCreate FAILED");
	hRet=pDD->QueryInterface(IID_IDirectDraw,(LPVOID *)&g_pDD);
	if(hRet!=DD_OK)
	    return InitFail(window,hRet,"QueryInterface FAILED");
	hRet=g_pDD->SetCooperativeLevel(window,DDSCL_NORMAL);
	if(hRet!=DD_OK)
		return InitFail(window,hRet,"SetCooperativeLevel FAILED");
  
	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags=DDSD_CAPS;
	ddsd.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE;
	hRet=g_pDD->CreateSurface(&ddsd,&g_pDDSPrimary,NULL);
	if(hRet!=DD_OK)
		return InitFail(window,hRet,"CreateSurface FAILED");
	hRet=g_pDD->CreateClipper(0,&g_pClipper,NULL);
	if(hRet!=DD_OK)
		return InitFail(window,hRet,"CreateClipper FAILED");
	hRet=g_pClipper->SetHWnd(0,window);
	if(hRet!=DD_OK)
		return InitFail(window,hRet,"SetHWnd FAILED");
	hRet=g_pDDSPrimary->SetClipper(g_pClipper);
	if(hRet!=DD_OK)
		return InitFail(window,hRet,"SetClipper FAILED");
	ZeroMemory(&ddsd3, sizeof(ddsd));
	ddsd3.dwSize=sizeof(ddsd);
	ddsd3.dwFlags=DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH;
	ddsd3.ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN;
	ddsd3.dwWidth=144;
	ddsd3.dwHeight=224;
	if(g_pDD->CreateSurface(&ddsd3,&g_pDDSOne,NULL)!=DD_OK)
		return InitFail(window,hRet,"CBuffer FAILED");
    memset(&ddsd2,0,sizeof(ddsd2));
	ddsd2.dwSize =sizeof(ddsd2);
	ddsd2.dwFlags=DDSD_PIXELFORMAT ;
	Sleep(1); 
	g_pDDSOne->Lock(NULL,&ddsd2,DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT,NULL);
	dx_bits=ddsd2.ddpfPixelFormat.dwRGBBitCount; 
	dx_r=ddsd2.ddpfPixelFormat.dwRBitMask; 
	dx_g=ddsd2.ddpfPixelFormat.dwGBitMask; 
	dx_b=ddsd2.ddpfPixelFormat.dwBBitMask; 
	dx_pitch=ddsd2.lPitch;
	cmov=ddsd2.lPitch/4;
	dx_bits/=8;
	dx_sr=find1(dx_r);
	dx_sg=find1(dx_g);
	dx_sb=find1(dx_b);
	
	dx_r_old=dx_r;
	dx_g_old=dx_g;
	dx_b_old=dx_b;

	dx_r=count1(dx_r);
	dx_g=count1(dx_g);
	dx_b=count1(dx_b);
	g_pDDSOne->Unlock(NULL);
	dx_linewidth_blit=ddsd2.lPitch;
	dx_buffer_line=61*dx_bits;
	set_shades();
	return	0;
}





int start_dx(void)
{
	if(flipd)
	 return start_dx_v();
    else
	 return start_dx_h();
}


void UpdateFrame(HWND hWnd,int i)
{
	
	
	vmodel=i;
	
	
	if(i)
		UpdateFrame_v(hWnd);
	else
		UpdateFrame_h(hWnd);
}



void flip_screen(void)
{
	HMENU	menu=GetMenu(window);	 
	flipd^=1;
	if(flipd)
	{
		CheckMenuItem(menu,ID_OPTIONS_VIDEO_ORIENTATION_VERTICAL,MF_CHECKED);
		CheckMenuItem(menu,ID_OPTIONS_VIDEO_ORIENTATION_HORIZONTAL,MF_UNCHECKED);
	}
	else
	{
		CheckMenuItem(menu,ID_OPTIONS_VIDEO_ORIENTATION_VERTICAL,MF_UNCHECKED);
		CheckMenuItem(menu,ID_OPTIONS_VIDEO_ORIENTATION_HORIZONTAL,MF_CHECKED);
	}
	closedx();
	set_size(1);
	start_dx();
	fr=0;
}


