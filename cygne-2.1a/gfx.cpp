/*
 ===================================================================================
		Cygne WIN  v 2.1a      (c) Dox 2002     dox@space.pl
 ===================================================================================

   NEC cpu core  by Bryan McPhail,Oliver Bergmann, Fabrice Frances and David Hedley
   Zlib  by  Jean-loup Gailly and Mark Adler

 ===================================================================================
 Main GFX code. Not optimised.
 For better speed  replace memset/memcpy  with faster code..
 ===================================================================================
*/


#include <windows.h>
#include "globals.h"
#include "gfx.h"
#include "memory.h"
#include <stdio.h>

DWORD	wsMonoPal[16][4];
DWORD	wsColors[8];
DWORD	wsCols[16][16];
DWORD 	fr;
DWORD 	frameskip;
DWORD 	wsMakeScr;
DWORD   crc;
int 	screen_size;
DWORD	colors[16];

#define mPUTPIXEL(number)\
		 if(wsTileRow[number])\
		{\
			b_bg[adrbuf]=wsTileRow[number];\
			b_bg_pal[adrbuf]=palette;\
		}\
		adrbuf++;

#define mPUTPIXEL_m(number,mask)\
		if((!(b_bg[adrbuf]&mask))&&(wsTileRow[number]))\
		{\
			b_bg[adrbuf]=wsTileRow[number];\
			b_bg_pal[adrbuf]=palette;\
		}\
		adrbuf++;\

#define mPUTPIXEL_mono(number)\
		b_bg[adrbuf]=wsColors[wsMonoPal[palette][wsTileRow[number]]];\
		adrbuf++;

#define mPUTPIXEL_mono_m(number,mask)\
		if(!(b_bg[adrbuf]&mask))\
			b_bg[adrbuf]=wsColors[wsMonoPal[palette][wsTileRow[number]]];\
		adrbuf++;

#define mPUTPIXEL_mono_t(number)\
		if(wsTileRow[number])\
			b_bg[adrbuf]=wsColors[wsMonoPal[palette][wsTileRow[number]]];\
		adrbuf++;

#define mPUTPIXEL_mono_t_m(number,mask)\
		if((wsTileRow[number])&&(!(b_bg[adrbuf]&mask)))\
			b_bg[adrbuf]=wsColors[wsMonoPal[palette][wsTileRow[number]]];\
		adrbuf++;

#define mPUTPIXEL_mm(number,mask)\
         if(!(b_bg[adrbuf]&mask))\
		{\
			b_bg[adrbuf]=wsTileRow[number];\
			b_bg_pal[adrbuf]=palette;\
		}\
		adrbuf++;

#define mPUTPIXEL_2(number)\
	    if(wsTileRow[number])\
		{\
			b_bg[adrbuf]=wsTileRow[number]|16;\
			b_bg_pal[adrbuf]=palette;\
		}\
		adrbuf++;

#define mPUTPIXEL_m_2(number,mask)\
	    if((!(b_bg[adrbuf]&mask))&&(wsTileRow[number]))\
		{\
			b_bg[adrbuf]=wsTileRow[number]|16;\
			b_bg_pal[adrbuf]=palette;\
		}\
		adrbuf++;

#define mPUTPIXEL_mono_2(number)\
        b_bg[adrbuf]=wsColors[wsMonoPal[palette][wsTileRow[number]]]|16;\
		adrbuf++;

#define mPUTPIXEL_mono_m_2(number,mask)\
		if(!(b_bg[adrbuf]&mask))\
			b_bg[adrbuf]=wsColors[wsMonoPal[palette][wsTileRow[number]]]|16;\
		adrbuf++;

#define mPUTPIXEL_mono_t_2(number)\
		if(wsTileRow[number])\
			b_bg[adrbuf]=wsColors[wsMonoPal[palette][wsTileRow[number]]]|16;\
		adrbuf++;

#define mPUTPIXEL_mono_t_m_2(number,mask)\
		if((wsTileRow[number])&&(!(b_bg[adrbuf]&mask)))\
			b_bg[adrbuf]=wsColors[wsMonoPal[palette][wsTileRow[number]]]|16;\
		adrbuf++;

#define mPUTPIXEL_mm_2(number,mask)\
		if(!(b_bg[adrbuf]&mask))\
		{\
			b_bg[adrbuf]=wsTileRow[number]|16;\
			b_bg_pal[adrbuf]=palette;\
		}\
		adrbuf++;

#define mSPUTPIXEL(number)\
        if(wsTileRow[number])\
		{\
			b_bg[adrbuf]=wsTileRow[number]|(b_bg[adrbuf]&16);\
			b_bg_pal[adrbuf]=palette+8;\
		}\
		adrbuf++;

#define mSPUTPIXEL_m(number,mask)\
	    if((!(b_bg[adrbuf]&mask))&&(wsTileRow[number]))\
		{\
			b_bg[adrbuf]=wsTileRow[number]|(b_bg[adrbuf]&16);\
			b_bg_pal[adrbuf]=palette+8;\
		}\
		adrbuf++;

#define mSPUTPIXEL_mono(number)\
	    b_bg[adrbuf]=wsColors[wsMonoPal[palette+8][wsTileRow[number]]]|(b_bg[adrbuf]&16);\
		adrbuf++;

#define mSPUTPIXEL_mono_m(number,mask)\
	    if(!(b_bg[adrbuf]&mask))\
			b_bg[adrbuf]=wsColors[wsMonoPal[palette+8][wsTileRow[number]]]|(b_bg[adrbuf]&16);\
		adrbuf++;

#define mSPUTPIXEL_mono_t(number)\
	    if(wsTileRow[number])\
			b_bg[adrbuf]=wsColors[wsMonoPal[palette+8][wsTileRow[number]]]|(b_bg[adrbuf]&16);\
		adrbuf++;

#define mSPUTPIXEL_mono_t_m(number,mask)\
		if((wsTileRow[number])&&(!(b_bg[adrbuf]&mask)))\
			 b_bg[adrbuf]=wsColors[wsMonoPal[palette+8][wsTileRow[number]]]|(b_bg[adrbuf]&16);\
		adrbuf++;

#define mSPUTPIXEL_mm(number,mask)\
        if(!(b_bg[adrbuf]&mask))\
		{\
			b_bg[adrbuf]=wsTileRow[number]|(b_bg[adrbuf]&16);\
			b_bg_pal[adrbuf]=palette+8;\
		}\
		adrbuf++;

void wsScanline(void)
{
	DWORD		packedcolor,start_tile_n,map_a,startindex,adrbuf,b1,b2,palette,j,t,l,sprtab,start;
	char		ys2;
	int			clipping,xs,ts,as,ys,ysx,h;
	BYTE		b_bg[320];
	BYTE		b_bg_pal[320]; 

    if((wsLine>143)||fr) return;
	if(!wsVMode) 
		memset(b_bg,wsColors[wsIO[1]&15]&15,320);
	else
	{
		memset(&b_bg[0],wsIO[1]&15,320);
		memset(&b_bg_pal[0],(wsIO[1]>>4)&15,320);
	}
	start_tile_n=(wsLine+wsIO[0x11])&0xff;/*First line*/
	map_a=(((DWORD)(wsIO[7]&0xf))<<11)+((start_tile_n&0xfff8)<<3);
	startindex=wsIO[0x10]>>3; /*First tile in row*/
	adrbuf=7-(wsIO[0x10]&7); /*Pixel in tile*/
	
	if((wsIO[0]&1))/*BG layer*/
		for (t=0;t<29;t++)
		{
			b1=wsRAM[map_a+(startindex<<1)];
			b2=wsRAM[map_a+(startindex<<1)+1];
			palette=(b2>>1)&15;
			b2=(b2<<8)|b1;
			wsGetTile(b2&0x1ff,start_tile_n&7,b2&0x8000,b2&0x4000,b2&0x2000);
		   if(wsVMode)
			{
				if((!(palette&4))&&(!(wsVMode&2)))
				{
					memcpy(&b_bg[adrbuf],&wsTileRow[0],8);			
					memset(&b_bg_pal[adrbuf],palette,8);
					adrbuf+=8;
				}
				else
				{
					mPUTPIXEL(0);
					mPUTPIXEL(1);
					mPUTPIXEL(2);
					mPUTPIXEL(3);
					mPUTPIXEL(4);
					mPUTPIXEL(5);
					mPUTPIXEL(6);
					mPUTPIXEL(7);
				}
			}
			else
			{
				if(!(palette&4))/*Non transparent palette*/
				{
					mPUTPIXEL_mono(0);
					mPUTPIXEL_mono(1);
					mPUTPIXEL_mono(2);
					mPUTPIXEL_mono(3);
					mPUTPIXEL_mono(4);
					mPUTPIXEL_mono(5);
					mPUTPIXEL_mono(6);
					mPUTPIXEL_mono(7);
				}
				else
				{
					mPUTPIXEL_mono_t(0);
					mPUTPIXEL_mono_t(1);
					mPUTPIXEL_mono_t(2);
					mPUTPIXEL_mono_t(3);
					mPUTPIXEL_mono_t(4);
					mPUTPIXEL_mono_t(5);
					mPUTPIXEL_mono_t(6);
					mPUTPIXEL_mono_t(7);
				}
			}
			startindex=(++startindex)&31;
		}
	
	if(wsIO[0]&2)/*FG layer*/
	{

		BYTE windowtype=wsIO[0]&0x30;
		if((windowtype==0x20)&&((wsIO[0x8]==wsIO[0xa])||(wsIO[0x9]==wsIO[0xb]))) /*soccer yaro fix*/
			goto skip_this;

		if(!((wsIO[9]==wsIO[0xb])||(wsIO[0x8]==wsIO[0xa])||(wsIO[8]>225)||(wsIO[9]>145)))/*Masks for FG window*/
		{
			if(windowtype==0x20)/*Inside*/
			{
				if((wsLine>wsIO[0xb])||(wsLine<wsIO[0x9]))
					 for(j=0;j<300;j++)b_bg[j]|=128;
			    else 
				{
					for(j=0;j<wsIO[0x8];j++) b_bg[7+j]|=128;
					for(j=wsIO[0xa];j<244;j++)b_bg[7+j]|=128;
				}
			}
			else
				if(windowtype==0x30)/*Outside*/
				{
				   if((wsLine>=wsIO[0x9])&&(wsLine<=wsIO[0xb]))
						for(j=wsIO[0x8];j<=wsIO[0xa];j++)
							b_bg[7+j]|=128;
				}
		}
		start_tile_n=(wsLine+wsIO[0x13])&0xff;
		map_a=(((DWORD)(wsIO[7]>>4))<<11)+((start_tile_n>>3)<<6);
		startindex=wsIO[0x12]>>3;
		adrbuf=7-(wsIO[0x12]&7);
		if(!windowtype)/*No FG Window*/
		{
			for (t=0;t<29;t++)
			{
				b1=wsRAM[map_a+(startindex<<1)];
				b2=wsRAM[map_a+(startindex<<1)+1];
				palette=(b2>>1)&15;
				b2=(b2<<8)|b1;
				wsGetTile(b2&0x1ff,start_tile_n&7,b2&0x8000,b2&0x4000,b2&0x2000);
				if(wsVMode)
				{
					if((!(palette&4))&&(!(wsVMode&2)))
					{
						memset(&b_bg_pal[adrbuf],palette,8);
						for(j=0;j<8;j++)
							b_bg[adrbuf++]=wsTileRow[j]|16;
					}
					else
					{
						mPUTPIXEL_2(0);
						mPUTPIXEL_2(1);
						mPUTPIXEL_2(2);
						mPUTPIXEL_2(3);
						mPUTPIXEL_2(4);
						mPUTPIXEL_2(5);
						mPUTPIXEL_2(6);
						mPUTPIXEL_2(7);
					}
				}
				else
				{
					if(!(palette&4))
					{
						mPUTPIXEL_mono_2(0);
						mPUTPIXEL_mono_2(1);
						mPUTPIXEL_mono_2(2);
						mPUTPIXEL_mono_2(3);
						mPUTPIXEL_mono_2(4);
						mPUTPIXEL_mono_2(5);
						mPUTPIXEL_mono_2(6);
						mPUTPIXEL_mono_2(7);
					}
					else
					{
						mPUTPIXEL_mono_t_2(0);
						mPUTPIXEL_mono_t_2(1);
						mPUTPIXEL_mono_t_2(2);
						mPUTPIXEL_mono_t_2(3);
						mPUTPIXEL_mono_t_2(4);
						mPUTPIXEL_mono_t_2(5);
						mPUTPIXEL_mono_t_2(6);
						mPUTPIXEL_mono_t_2(7);
					}
				}
				startindex=(++startindex)&31;
			}
		}
		else
		{
			for (t=0;t<29;t++)
			{
				b1=wsRAM[map_a+(startindex<<1)];
				b2=wsRAM[map_a+(startindex<<1)+1];
				palette=(b2>>1)&15;
				b2=(b2<<8)|b1;
				wsGetTile(b2&0x1ff,start_tile_n&7,b2&0x8000,b2&0x4000,b2&0x2000);
				if(wsVMode)
				{
					if((!(palette&4))&&(!(wsVMode&2)))
					{
						mPUTPIXEL_mm_2(0,128);
						mPUTPIXEL_mm_2(1,128);
						mPUTPIXEL_mm_2(2,128);
						mPUTPIXEL_mm_2(3,128);
						mPUTPIXEL_mm_2(4,128);
						mPUTPIXEL_mm_2(5,128);
						mPUTPIXEL_mm_2(6,128);
						mPUTPIXEL_mm_2(7,128);
					}
					else
					{
						mPUTPIXEL_m_2(0,128);
						mPUTPIXEL_m_2(1,128);
						mPUTPIXEL_m_2(2,128);
						mPUTPIXEL_m_2(3,128);
						mPUTPIXEL_m_2(4,128);
						mPUTPIXEL_m_2(5,128);
						mPUTPIXEL_m_2(6,128);
						mPUTPIXEL_m_2(7,128);
					}
				}
				else
				{
					if(!(palette&4))
					{
						mPUTPIXEL_mono_m_2(0,128);
						mPUTPIXEL_mono_m_2(1,128);
						mPUTPIXEL_mono_m_2(2,128);
						mPUTPIXEL_mono_m_2(3,128);
						mPUTPIXEL_mono_m_2(4,128);
						mPUTPIXEL_mono_m_2(5,128);
						mPUTPIXEL_mono_m_2(6,128);
						mPUTPIXEL_mono_m_2(7,128);
					}
					else
					{
						mPUTPIXEL_mono_t_m_2(0,128);
						mPUTPIXEL_mono_t_m_2(1,128);
						mPUTPIXEL_mono_t_m_2(2,128);
						mPUTPIXEL_mono_t_m_2(3,128);
						mPUTPIXEL_mono_t_m_2(4,128);
						mPUTPIXEL_mono_t_m_2(5,128);
						mPUTPIXEL_mono_t_m_2(6,128);
						mPUTPIXEL_mono_t_m_2(7,128);
					}
				}
				startindex=(++startindex)&31;//%32;//=(startindex+1)%256;
			}
		}
	}

skip_this:;
	
	
	if(wsIO[0]&4)/*Sprites*/
	{
		if((wsIO[0]&8)&&(wsIO[2]>=wsIO[0xd])&&(wsIO[2]<=wsIO[0xf]))/*Sprite window*/
			for(j=wsIO[0xc];j<wsIO[0xe];j++) b_bg[7+j]|=64;
		sprtab=(wsIO[0x4]<<9)+((wsIO[0x5]+wsIO[0x6]-1)<<2);

		for(h=0;h<wsIO[0x6];h++)
		{
			ts=wsRAM[sprtab++];
			as=wsRAM[sprtab++];
			ysx=wsRAM[sprtab];
			ys2=(signed char)wsRAM[sprtab++];
			xs=wsRAM[sprtab];
			if(ysx>150)ys=ys2;else ys=ysx;
			sprtab-=7;
			if(!((ys+8<=wsLine)||(ys>wsLine)))
			{
				ys=wsLine-ys;
				ts|=(as&1)<<8;
				wsGetTile(ts,ys,as&128,as&64,0);
				adrbuf=(7+xs)&255;
				palette=(as>>1)&7;
				if(wsIO[0]&8)
					clipping=as&16;
				else
					clipping=1;
				if(as&32)
				{
					if(!clipping)
					{
						if(wsVMode)
						{
							if((!(palette&4))&&(!(wsVMode&2)))
							{
								memcpy(&b_bg[adrbuf],&wsTileRow[0],8);			
								memset(&b_bg_pal[adrbuf],palette,8);
								adrbuf+=8;
							}
							else
							{
								mSPUTPIXEL(0);
								mSPUTPIXEL(1);
								mSPUTPIXEL(2);
								mSPUTPIXEL(3);
								mSPUTPIXEL(4);
								mSPUTPIXEL(5);
								mSPUTPIXEL(6);
								mSPUTPIXEL(7);
							}
						}
						else
						{
							if(!(palette&4))
							{
								mSPUTPIXEL_mono(0);
								mSPUTPIXEL_mono(1);
								mSPUTPIXEL_mono(2);
								mSPUTPIXEL_mono(3);
								mSPUTPIXEL_mono(4);
								mSPUTPIXEL_mono(5);
								mSPUTPIXEL_mono(6);
								mSPUTPIXEL_mono(7);
							}
							else
							{
								mSPUTPIXEL_mono_t(0);
								mSPUTPIXEL_mono_t(1);
								mSPUTPIXEL_mono_t(2);
								mSPUTPIXEL_mono_t(3);
								mSPUTPIXEL_mono_t(4);
								mSPUTPIXEL_mono_t(5);
								mSPUTPIXEL_mono_t(6);
								mSPUTPIXEL_mono_t(7);
							}
						}

					}
					else
					{
						if(wsVMode)
						{
							if((!(palette&4))&&(!(wsVMode&2)))
							{
								mSPUTPIXEL_mm(0,64);
								mSPUTPIXEL_mm(1,64);
								mSPUTPIXEL_mm(2,64);
								mSPUTPIXEL_mm(3,64);
								mSPUTPIXEL_mm(4,64);
								mSPUTPIXEL_mm(5,64);
								mSPUTPIXEL_mm(6,64);
								mSPUTPIXEL_mm(7,64);
							}
							else
							{
								mSPUTPIXEL_m(0,64);
								mSPUTPIXEL_m(1,64);
								mSPUTPIXEL_m(2,64);
								mSPUTPIXEL_m(3,64);
								mSPUTPIXEL_m(4,64);
								mSPUTPIXEL_m(5,64);
								mSPUTPIXEL_m(6,64);
								mSPUTPIXEL_m(7,64);
							}
						}
						else
						{
							if(!(palette&4))
							{
								mSPUTPIXEL_mono_m(0,64);
								mSPUTPIXEL_mono_m(1,64);
								mSPUTPIXEL_mono_m(2,64);
								mSPUTPIXEL_mono_m(3,64);
								mSPUTPIXEL_mono_m(4,64);
								mSPUTPIXEL_mono_m(5,64);
								mSPUTPIXEL_mono_m(6,64);
								mSPUTPIXEL_mono_m(7,64);
							}
							else
							{
								mSPUTPIXEL_mono_t_m(0,64);
								mSPUTPIXEL_mono_t_m(1,64);
								mSPUTPIXEL_mono_t_m(2,64);
								mSPUTPIXEL_mono_t_m(3,64);
								mSPUTPIXEL_mono_t_m(4,64);
								mSPUTPIXEL_mono_t_m(5,64);
								mSPUTPIXEL_mono_t_m(6,64);
								mSPUTPIXEL_mono_t_m(7,64);
							}
						}
					}
				}
				else
				{
					if(!clipping)
					{
						if(wsVMode)
						{
							if((!(palette&4))&&(!(wsVMode&2)))
							{
								mSPUTPIXEL_mm(0,16);
								mSPUTPIXEL_mm(1,16);
								mSPUTPIXEL_mm(2,16);
								mSPUTPIXEL_mm(3,16);
								mSPUTPIXEL_mm(4,16);
								mSPUTPIXEL_mm(5,16);
								mSPUTPIXEL_mm(6,16);
								mSPUTPIXEL_mm(7,16);
							}
							else
							{
								mSPUTPIXEL_m(0,16);
								mSPUTPIXEL_m(1,16);
								mSPUTPIXEL_m(2,16);
								mSPUTPIXEL_m(3,16);
								mSPUTPIXEL_m(4,16);
								mSPUTPIXEL_m(5,16);
								mSPUTPIXEL_m(6,16);
								mSPUTPIXEL_m(7,16);
							}
						}
						else
						{
							if(!(palette&4))
							{
								mSPUTPIXEL_mono_m(0,16);
								mSPUTPIXEL_mono_m(1,16);
								mSPUTPIXEL_mono_m(2,16);
								mSPUTPIXEL_mono_m(3,16);
								mSPUTPIXEL_mono_m(4,16);
								mSPUTPIXEL_mono_m(5,16);
								mSPUTPIXEL_mono_m(6,16);
								mSPUTPIXEL_mono_m(7,16);
							}
							else
							{
								mSPUTPIXEL_mono_t_m(0,16);
								mSPUTPIXEL_mono_t_m(1,16);
								mSPUTPIXEL_mono_t_m(2,16);
								mSPUTPIXEL_mono_t_m(3,16);
								mSPUTPIXEL_mono_t_m(4,16);
								mSPUTPIXEL_mono_t_m(5,16);
								mSPUTPIXEL_mono_t_m(6,16);
								mSPUTPIXEL_mono_t_m(7,16);
							}
						}

					}
					else
					{
						if(wsVMode)
						{
							if((!(palette&4))&&(!(wsVMode&2)))
							{
								mSPUTPIXEL_mm(0,80);
								mSPUTPIXEL_mm(1,80);
								mSPUTPIXEL_mm(2,80);
								mSPUTPIXEL_mm(3,80);
								mSPUTPIXEL_mm(4,80);
								mSPUTPIXEL_mm(5,80);
								mSPUTPIXEL_mm(6,80);
								mSPUTPIXEL_mm(7,80);
							}
							else
							{
								mSPUTPIXEL_m(0,80);
								mSPUTPIXEL_m(1,80);
								mSPUTPIXEL_m(2,80);
								mSPUTPIXEL_m(3,80);
								mSPUTPIXEL_m(4,80);
								mSPUTPIXEL_m(5,80);
								mSPUTPIXEL_m(6,80);
								mSPUTPIXEL_m(7,80);
							}
						}
						else
						{
							if(!(palette&4))
							{
								mSPUTPIXEL_mono_m(0,80);
								mSPUTPIXEL_mono_m(1,80);
								mSPUTPIXEL_mono_m(2,80);
								mSPUTPIXEL_mono_m(3,80);
								mSPUTPIXEL_mono_m(4,80);
								mSPUTPIXEL_mono_m(5,80);
								mSPUTPIXEL_mono_m(6,80);
								mSPUTPIXEL_mono_m(7,80);
							}
							else
							{
								mSPUTPIXEL_mono_t_m(0,80);
								mSPUTPIXEL_mono_t_m(1,80);
								mSPUTPIXEL_mono_t_m(2,80);
								mSPUTPIXEL_mono_t_m(3,80);
								mSPUTPIXEL_mono_t_m(4,80);
								mSPUTPIXEL_mono_t_m(5,80);
								mSPUTPIXEL_mono_t_m(6,80);
								mSPUTPIXEL_mono_t_m(7,80);
							}
						}
					}
				}
			}
		}
	}

	if(wsVMode)
	{
		if(!flipd)
		{
			switch(dx_bits)
			{
				case 4: /* 32 bpp */
					start=((wsLine<<7)+(wsLine<<6)+(wsLine<<5))<<2;/* line*224*4 */
					for(l=0;l<224;l++)
					{
						packedcolor=wsCols[b_bg_pal[l+7]][b_bg[(l+7)]&0xf];
						dx_buffer[start+(l<<2)]=(BYTE)(packedcolor&0xff);
						packedcolor>>=8;
						dx_buffer[start+(l<<2)+1]=(BYTE)(packedcolor&0xff);
						packedcolor>>=8;
						dx_buffer[start+(l<<2)+2]=(BYTE)(packedcolor&0xff);
						packedcolor>>=8;
						dx_buffer[start+(l<<2)+3]=(BYTE)(packedcolor&0xff);
					}
				break;
				case 2: /* 16/15 bpp */
					start=((wsLine<<7)+(wsLine<<6)+(wsLine<<5))<<1;/* line*224*2 */
					for(l=0;l<224;l++)
					{
						packedcolor=wsCols[b_bg_pal[l+7]][b_bg[(l+7)]&0xf];
						dx_buffer[start+(l<<1)]=(BYTE)(packedcolor&0xff);
						packedcolor>>=8;
						dx_buffer[start+(l<<1)+1]=(BYTE)(packedcolor&0xff);
					}
				break;
			}
		}
		else
		{
			switch(dx_bits)
			{
				case 4:
					start=(wsLine<<2)+144*4*223;
					for(l=0;l<224;l++)
					{
						packedcolor=wsCols[b_bg_pal[l+7]][b_bg[(l+7)]&0xf];
						dx_buffer[start]=(BYTE)(packedcolor&0xff);
						packedcolor>>=8;
						dx_buffer[start+1]=(BYTE)(packedcolor&0xff);
						packedcolor>>=8;
						dx_buffer[start+2]=(BYTE)(packedcolor&0xff);
						packedcolor>>=8;
						dx_buffer[start+3]=(BYTE)(packedcolor&0xff);
						start-=144*4;
					}
				break;
				case 2:
					start=(wsLine<<1)+144*2*223;
					for(l=0;l<224;l++)
					{
						packedcolor=wsCols[b_bg_pal[l+7]][b_bg[(l+7)]&0xf];
						dx_buffer[start]=(BYTE)(packedcolor&0xff);
						packedcolor>>=8;
						dx_buffer[start+1]=(BYTE)(packedcolor&0xff);
						start-=144*2;
					}
				break;
			}
		}
	}
	else
	{
		if(!flipd)
		{
			switch(dx_bits)
			{
				case 4:
					start=((wsLine<<7)+(wsLine<<6)+(wsLine<<5))<<2;
					for(l=0;l<224;l++)
					{
						packedcolor=colors[(b_bg[l+7])&15];
						dx_buffer[start+(l<<2)]=(BYTE)(packedcolor&0xff);
						packedcolor>>=8;
						dx_buffer[start+(l<<2)+1]=(BYTE)(packedcolor&0xff);
						packedcolor>>=8;
						dx_buffer[start+(l<<2)+2]=(BYTE)(packedcolor&0xff);
						packedcolor>>=8;
						dx_buffer[start+(l<<2)+3]=(BYTE)(packedcolor&0xff);
					}
				break;
				case 2:
					start=((wsLine<<7)+(wsLine<<6)+(wsLine<<5))<<1;
					for(l=0;l<224;l++)
					{
						packedcolor=colors[(b_bg[l+7])&15];
						dx_buffer[start+(l<<1)]=(BYTE)(packedcolor&0xff);
						packedcolor>>=8;
						dx_buffer[start+(l<<1)+1]=(BYTE)(packedcolor&0xff);
					}
				break;
			}

		}
		else
		{
			switch(dx_bits)
			{
				case 4:
					start=(wsLine<<2)+144*4*223;
					for(l=0;l<224;l++)
					{
						packedcolor=colors[b_bg[l+7]&15];
						dx_buffer[start]=(BYTE)(packedcolor&0xff);
						packedcolor>>=8;
						dx_buffer[start+1]=(BYTE)(packedcolor&0xff);
						packedcolor>>=8;
						dx_buffer[start+2]=(BYTE)(packedcolor&0xff);
						packedcolor>>=8;
						dx_buffer[start+3]=(BYTE)(packedcolor&0xff);
						start-=576;
					}
				break;
				case 2:
					start=(wsLine<<1)+144*2*223;
					for(l=0;l<224;l++)
					{
						packedcolor=colors[(b_bg[l+7])&15];
						dx_buffer[start]=(BYTE)(packedcolor&0xff);
						packedcolor>>=8;
						dx_buffer[start+1]=(BYTE)(packedcolor&0xff);
						start-=288;
					}
				break;
			}
		}
	}
}







