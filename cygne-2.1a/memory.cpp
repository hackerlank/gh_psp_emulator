/*
 ===================================================================================
		Cygne WIN  v 2.1a      (c) Dox 2002     dox@space.pl
 ===================================================================================

   NEC cpu core  by Bryan McPhail,Oliver Bergmann, Fabrice Frances and David Hedley
   Zlib  by  Jean-loup Gailly and Mark Adler

 ===================================================================================
*/

#include <windows.h>
#include "globals.h"
#include "gfx.h"
#include "memory.h"
#include <time.h>
#include <stdio.h>

BYTE		wsIO[256];
BYTE		wsRAM[65536];
BYTE		wsSRAM[65536];
BYTE		wsmem[16*1024*1024]; /*Static Array (16 megabytes)*/
DWORD		sram_size;
BYTE	    wsCA15=0;
BYTE        wsEEPROM[131072];


#include "eeprom.h"

void cpu_writemem20(DWORD address,BYTE data)
{
 DWORD ws_offset,ws_bank,v1,v2,v3;

 ws_offset=address&0xffff;
 ws_bank=address>>16;

 if(!ws_bank) /*RAM*/
 {
    wsRAM[ws_offset]=data;
	if((wsVMode&2)&&(ws_offset>=0x4000)&&(ws_offset<0x8000))
	{
		wsTCacheUpdate[(ws_offset-0x4000)>>5]=FALSE; /*invalidate tile*/
		return;
	}
	else
		if((ws_offset>=0x2000)&&(ws_offset<0x4000))
		{
			wsTCacheUpdate[(ws_offset-0x2000)>>4]=FALSE; /*invalidate tile*/
			return;
		}

    if((wsVMode&2)&&(ws_offset>=0x8000)&&(ws_offset<0xc000))
	{
		wsTCacheUpdate2[(ws_offset-0x8000)>>5]=FALSE; /*invalidate tile*/
		return;
	}
	else
		if((ws_offset>=0x4000)&&(ws_offset<0x6000))
		{
			wsTCacheUpdate2[(ws_offset-0x4000)>>4]=FALSE; /*invalidate tile*/
			return;
		}
	if(ws_offset>=0xfe00) /*WSC palettes*/
	{
	  ws_offset=(ws_offset&0xfffe)-0xfe00;
	  v1=((((wsRAM[ws_offset+0xfe00]&0xf))<<4)>>(8-dx_b))<<dx_sb; /*convert and store colors in screen pixel format*/
	  v2=(((((wsRAM[ws_offset+0xfe00]>>4)&0xf))<<4)>>(8-dx_g))<<dx_sg;
	  v3=((((wsRAM[ws_offset+0xfe01]&0xf))<<4)>>(8-dx_r))<<dx_sr;
	  wsCols[(ws_offset>>1)>>4][(ws_offset>>1)&15]=v1|v2|v3;
	 }
  }
 else
  if((ws_bank==1)) /*SRAM and EEPROM*/
  {
	 
      wsSRAM[ws_offset&(sram_size-1)]=data;
  }
}



BYTE cpu_readmem20(DWORD address)
{
 DWORD	ws_offset,ws_bank;

 ws_offset=address&0xffff;
 ws_bank=(address>>16)&0xf;
  
 switch(ws_bank)
 {
	case 0:  return wsRAM[ws_offset];
	case 1:  return wsSRAM[ws_offset&(sram_size-1)];
	case 2:
	case 3:  return wsmem[ws_offset+((wsIO[0xc0+ws_bank]&((rom_size>>16)-1))<<16)];
	default: return wsmem[ws_offset+rom_size-((256-(((wsIO[0xc0]&0xf)<<4)|(ws_bank&0xf)))<<16)];
 }
}


BYTE cpu_readport(BYTE number)
{
  DWORD		w1,w2;
  
  switch(number)
  {
	  case 0xa0: if(wsc) return wsIO[0xa0]|2; else return wsIO[0xa0]; /*WS/WSC*/
	  case 0x4e: return 0;
	  case 0x4f: return 0;
	  case 0xaa: return 255; /*FIX*/
	  case 0xb3: if(wsIO[0xb3]<0x80)return 0;if(wsIO[0xb3]<0xc0)return 0x84;return 0xc4;
      case 0xb5: /*keypad*/
				 w1=wsIO[0xb5];
				 if(w1&0x40) /*buttons*/
				 {
				   if(flipd&&link_controls)
					 w2=(k_start<<1)|(k_left2<<2)|(k_right2<<3);
				   else	
					 w2=(k_start<<1)|(k_a<<2)|(k_b<<3);
				   return (BYTE)((w1&0xf0)|w2);
				 }
				 if(w1&0x20) /*H cursors*/
				 {
					if(flipd&&link_controls)
						w2=k_a|(k_up2<<1)|(k_b<<2)|(k_down2<<3);
					else
						w2=k_up1|(k_right1<<1)|(k_down1<<2)|(k_left1<<3);
					return (BYTE)((w1&0xf0)|w2);
				 }
				 if(w1&0x10) /*V cursors*/
				 {
	   				if(flipd&&link_controls)
						w2=k_left1|(k_up1<<1)|(k_right1<<2)|(k_down1<<3);
					else	
						w2=k_up2|(k_right2<<1)|(k_down2<<2)|(k_left2<<3);
					return (BYTE)((w1&0xf0)|w2);
				 }
				 break;
	  case 0xbe: 
				 if(wsIO[0xbe]&0x20)return wsIO[0xbe]|2;
				 if(wsIO[0xbe]&0x10)return wsIO[0xbe]|1;
				 return wsIO[0xbe]|3;
 
	  case 0xba:
			w1=((((WORD)wsIO[0xbd])<<8)|((WORD)wsIO[0xbc]));
			w1=(w1<<1)&0x3ff;
			return iEEPROM[w1];

		   
	  case 0xbb:
		  	w1=((((WORD)wsIO[0xbd])<<8)|((WORD)wsIO[0xbc]));
			w1=((w1<<1)+1)&0x3ff;
			return iEEPROM[w1];

	  case 0xc4: w1=(((((WORD)wsIO[0xc7])<<8)|((WORD)wsIO[0xc6]))<<1)&(sram_size-2);
				 return wsEEPROM[w1];
	  
	  case 0xc5: w1=(((((WORD)wsIO[0xc7])<<8)|((WORD)wsIO[0xc6]))<<1)&(sram_size-2);
				 return wsEEPROM[w1+1];

	  case 0xc8: if(wsIO[0xc8]&0x20)return wsIO[0xc8]|2;
				 if(wsIO[0xc8]&0x10)return wsIO[0xc8]|1;
				 return wsIO[0xc8]|3;
	  
	  case 0xca : return (wsIO[0xca])|0x80;
	  case 0xc0 : return (wsIO[0xc0]&0xf)|0x20;
	  case 0xcb : 
		 
		  if(wsIO[0xca]==0x15)
		  {
				 
	        struct tm *newtime;
		    time_t long_time;
		    time( &long_time );                
			newtime = localtime( &long_time ); 
		
			  switch(wsCA15)
			  {
				  case 0: wsCA15++;return mBCD(newtime->tm_year-100);
				  case 1: wsCA15++;return mBCD(newtime->tm_mon);
				  case 2: wsCA15++;return mBCD(newtime->tm_mday);
				  case 3: wsCA15++;return mBCD(newtime->tm_wday);
				  case 4: wsCA15++;return mBCD(newtime->tm_hour);
				  case 5: wsCA15++;return mBCD(newtime->tm_min);
				  case 6: wsCA15=0;return mBCD(newtime->tm_sec);

			  }
			  return 0;
		  }
		  else
		  return wsIO[0xcb]|0x80;

 }
 return wsIO[number];
}


void cpu_writeport(DWORD IOPort,BYTE data)
{
 	DWORD	dma_start,dma_end,dma_size,ix,w1;

	wsIO[IOPort]=data;
	switch(IOPort)
	{

		case 0x14:	if((!(wsIO[0x14]&1))&&(data&1)) { wsLine=0; wsIO[2]=0;}break; /* LCD off ??*/
		
		case 0x48: /*DMA copy , initial version (immediate)*/
			   if(data&128)
			   {
					dma_start=(((DWORD)wsIO[0x41])<<8)|(((DWORD)wsIO[0x40]))|(((DWORD)wsIO[0x42])<<16);
					dma_end=(((DWORD)wsIO[0x45])<<8)|(((DWORD)wsIO[0x44]))|(((DWORD)wsIO[0x43])<<16);
					dma_size=(((DWORD)wsIO[0x47])<<8)|(((DWORD)wsIO[0x46]));
				    for(ix=0;ix<dma_size;ix++)
				 		cpu_writemem20(dma_end++,cpu_readmem20(dma_start++));/*The slowest way ;) */
	    			wsIO[0x47]=0;
					wsIO[0x46]=0;
					wsIO[0x41]=(BYTE)(dma_start>>8);
					wsIO[0x40]=(BYTE)(dma_start&0xff);
					wsIO[0x45]=(BYTE)(dma_end>>8);
					wsIO[0x44]=(BYTE)(dma_end&0xff);
					wsIO[0x48]=0; /* ?? */
				}
			   break;
	case 0x1c: wsColors[0]=15-(data&0xf);wsColors[1]=15-(data>>4);break; /*colors*/
	case 0x1d: wsColors[2]=15-(data&0xf);wsColors[3]=15-(data>>4);break;
	case 0x1e: wsColors[4]=15-(data&0xf);wsColors[5]=15-(data>>4);break;
	case 0x1f: wsColors[6]=15-(data&0xf);wsColors[7]=15-(data>>4);break;
  
	case 0x20: wsMonoPal[0][0]=data&7;wsMonoPal[0][1]=(data>>4)&7;break; /*palettes*/
	case 0x21: wsMonoPal[0][2]=data&7;wsMonoPal[0][3]=(data>>4)&7;break;
    case 0x22: wsMonoPal[1][0]=data&7;wsMonoPal[1][1]=(data>>4)&7;break;
    case 0x23: wsMonoPal[1][2]=data&7;wsMonoPal[1][3]=(data>>4)&7;break;
    case 0x24: wsMonoPal[2][0]=data&7;wsMonoPal[2][1]=(data>>4)&7;break;
    case 0x25: wsMonoPal[2][2]=data&7;wsMonoPal[2][3]=(data>>4)&7;break;
    case 0x26: wsMonoPal[3][0]=data&7;wsMonoPal[3][1]=(data>>4)&7;break;
    case 0x27: wsMonoPal[3][2]=data&7;wsMonoPal[3][3]=(data>>4)&7;break;
    case 0x28: wsMonoPal[4][0]=data&7;wsMonoPal[4][1]=(data>>4)&7;break;
    case 0x29: wsMonoPal[4][2]=data&7;wsMonoPal[4][3]=(data>>4)&7;break;
    case 0x2a: wsMonoPal[5][0]=data&7;wsMonoPal[5][1]=(data>>4)&7;break;
    case 0x2b: wsMonoPal[5][2]=data&7;wsMonoPal[5][3]=(data>>4)&7;break;
    case 0x2c: wsMonoPal[6][0]=data&7;wsMonoPal[6][1]=(data>>4)&7;break;
    case 0x2d: wsMonoPal[6][2]=data&7;wsMonoPal[6][3]=(data>>4)&7;break;
    case 0x2e: wsMonoPal[7][0]=data&7;wsMonoPal[7][1]=(data>>4)&7;break;
    case 0x2f: wsMonoPal[7][2]=data&7;wsMonoPal[7][3]=(data>>4)&7;break;
    case 0x30: wsMonoPal[8][0]=data&7;wsMonoPal[8][1]=(data>>4)&7;break;
    case 0x31: wsMonoPal[8][2]=data&7;wsMonoPal[8][3]=(data>>4)&7;break;
    case 0x32: wsMonoPal[9][0]=data&7;wsMonoPal[9][1]=(data>>4)&7;break;
    case 0x33: wsMonoPal[9][2]=data&7;wsMonoPal[9][3]=(data>>4)&7;break;
	case 0x34: wsMonoPal[10][0]=data&7;wsMonoPal[10][1]=(data>>4)&7;break;
	case 0x35: wsMonoPal[10][2]=data&7;wsMonoPal[10][3]=(data>>4)&7;break;
	case 0x36: wsMonoPal[11][0]=data&7;wsMonoPal[11][1]=(data>>4)&7;break;
	case 0x37: wsMonoPal[11][2]=data&7;wsMonoPal[11][3]=(data>>4)&7;break;
	case 0x38: wsMonoPal[12][0]=data&7;wsMonoPal[12][1]=(data>>4)&7;break;
	case 0x39: wsMonoPal[12][2]=data&7;wsMonoPal[12][3]=(data>>4)&7;break;
	case 0x3a: wsMonoPal[13][0]=data&7;wsMonoPal[13][1]=(data>>4)&7;break;
	case 0x3b: wsMonoPal[13][2]=data&7;wsMonoPal[13][3]=(data>>4)&7;break;
	case 0x3c: wsMonoPal[14][0]=data&7;wsMonoPal[14][1]=(data>>4)&7;break;
	case 0x3d: wsMonoPal[14][2]=data&7;wsMonoPal[14][3]=(data>>4)&7;break;
	case 0x3e: wsMonoPal[15][0]=data&7;wsMonoPal[15][1]=(data>>4)&7;break;
	case 0x3f: wsMonoPal[15][2]=data&7;wsMonoPal[15][3]=(data>>4)&7;break;
	
   case 0xba: w1=(((((WORD)wsIO[0xbd])<<8)|((WORD)wsIO[0xbc])));
			  w1=(w1<<1)&0x3ff;
			  iEEPROM[w1]=data;
			  return;
	  
	case 0xbb: w1=(((((WORD)wsIO[0xbd])<<8)|((WORD)wsIO[0xbc])));
			   w1=((w1<<1)+1)&0x3ff;
			   iEEPROM[w1]=data;
			   return;

	case 0xc4: w1=(((((WORD)wsIO[0xc7])<<8)|((WORD)wsIO[0xc6]))<<1)&(sram_size-2);
			   wsEEPROM[w1]=data;
			   return;
	  
	case 0xc5: w1=(((((WORD)wsIO[0xc7])<<8)|((WORD)wsIO[0xc6]))<<1)&(sram_size-2);
			   wsEEPROM[w1+1]=data;
			   return;

	case 0xca: if(data==0x15)wsCA15=0; break;
	case 0x60: wsSetVideo(data>>5,false); break; 
 }
}
