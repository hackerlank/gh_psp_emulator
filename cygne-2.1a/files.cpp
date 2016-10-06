/*
 ===================================================================================
		Cygne WIN  v 2.1a     (c) Dox 2002     dox@space.pl
 ===================================================================================

   NEC cpu core  by Bryan McPhail,Oliver Bergmann, Fabrice Frances and David Hedley
   Zlib  by  Jean-loup Gailly and Mark Adler

 ===================================================================================
  Files  - ROM Load, Save/Load state
 ===================================================================================
*/



#include <windows.h>
#include <stdio.h>
#include "globals.h"
#include "zlib/unzip.h"
#include "gfx.h"
#include "memory.h"

#define mSTOREREG(REGNAME)        \
	    uReg=nec_get_reg(REGNAME); \
		fwrite(&uReg,sizeof(uReg),1,fState);

#define mMENUFRAMES(OPTION1,OPTION2,OPTION3,OPTION4,OPTION5,OPTION6)  \
		CheckMenuItem(hMenu,ID_OPTIONS_VIDEO_FRAMESKIP_0,OPTION1); \


char	path_app[1024];
char	path_save[1024];
char	path_sram[1024];
char	path_rom[1024];
char	dirname[2048];


extern DWORD cycles_line;

DWORD ws_maxbank;

enum {
	I86_IP=1, I86_AX, I86_CX, I86_DX, I86_BX, I86_SP, I86_BP, I86_SI, I86_DI,
	I86_FLAGS, I86_ES, I86_CS, I86_SS, I86_DS,
	I86_VECTOR, I86_PENDING, I86_NMI_STATE, I86_IRQ_STATE};


void saveSRAM(void)
{
	FILE* file;
	if(!sram_size)return;
	if(!(sram_size&1)){file=fopen(path_sram,"wb");if(file!=NULL){ fwrite(wsSRAM,1,sram_size,file);fclose(file);}}
	else {file=fopen(path_sram,"wb");if(file!=NULL){ fwrite(wsEEPROM,1,sram_size-1,file);fclose(file);}}

}



DWORD unziprom(char* RomPath)
{
    unzFile fp;
    unsigned long gROMLength; //size in bytes of the ROM
   
  if(fp = unzOpen(RomPath))
  {
    char szFileName[256];
    if(unzGoToFirstFile(fp) == UNZ_OK)
    {
      do
      {
        unz_file_info file_info;
        if(unzGetCurrentFileInfo(fp,&file_info, szFileName,256,NULL,0,NULL,0) == UNZ_OK)
        {
          if(stricmp(&szFileName[strlen(szFileName) - 3], ".ws") == 0
            || stricmp(&szFileName[strlen(szFileName) - 4], ".wsc") == 0)
           {
            gROMLength = file_info.uncompressed_size; 
            if(unzOpenCurrentFile(fp)== UNZ_OK)
            {
                if(unzReadCurrentFile(fp,wsmem ,gROMLength) == (int)( gROMLength))
				{
					unzClose(fp);
					return gROMLength;
				}
				else
				{
					unzClose(fp);
					return 0;
				}
				unzCloseCurrentFile(fp);
            }
            else
            {
                
                unzClose(fp);
                return 0;
            }
          }
        }
        else
        {
              unzClose(fp);
              return 0;
        }
      }
      while(unzGoToNextFile(fp) == UNZ_OK);
    }
    unzClose(fp);
  }
	return 0;
}


void wsROMLoad(HWND dialog)
{
	FILE*			file;
	char			buf[256];
	char			romname[1024];
	char			fil[]="ws files\0*.ws;*.wsc;*.zip\0";
	OPENFILENAME	ofn;
	char			dirname[256];
	int				hh,index;

	f_load=FALSE;
	buf[0]=0;
	GetCurrentDirectory(sizeof(dirname),dirname);
	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.hwndOwner=dialog;
	ofn.lpstrFilter=fil;
	ofn.nFilterIndex=1;
	ofn.lpstrFile=buf;
	ofn.lpstrInitialDir=dirname;
	ofn.nMaxFile=256;
	ofn.Flags=OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
	GetOpenFileName(&ofn);
	if(buf[0])
	{
		 strcpy(romname,"Cygne - ");
		 strcat(romname,buf);
		 SetWindowText(window,romname);
		 if(!stricmp(&buf[strlen(buf) - 4], ".zip"))
			rom_size=unziprom(buf);
		 else
		 {
			file=fopen(buf,"rb");
			fseek(file,0,SEEK_END);
			rom_size=ftell(file);
			fseek(file,0,SEEK_SET);
			fread(wsmem,1,rom_size,file);
			fclose(file);
		}
		index=0;
	
		sram_size=0;
		if(wsmem[rom_size-5]==01)sram_size=8*1024;
		if(wsmem[rom_size-5]==02)sram_size=32*1024;
		if(wsmem[rom_size-5]==0x10)sram_size=128+1;/* Bit 1 set = EEPROM */
		if(wsmem[rom_size-5]==0x20)sram_size=2*1024+1;
		if(wsmem[rom_size-5]==0x50)sram_size=1024+1;


	    for(int x=strlen(buf);x>0;x--)
		{
			if(buf[x]==92)
			{
				for(hh=x;hh<strlen(buf);hh++)
					path_rom[index++]=buf[hh];
				if(path_rom[index-4]!=46)
					index++;
				
				if(sram_size&1)
				{
					path_rom[index]=0;
					path_rom[index-1]='p';
					path_rom[index-2]='e';
					path_rom[index-3]='e';
				}
				else
				{
					path_rom[index]=0;
					path_rom[index-1]='v';
					path_rom[index-2]='a';
					path_rom[index-3]='s';
				}
			
				strcpy(path_sram,path_save);
				strcat(path_sram,path_rom);
				break;
			}
		}


		crc=wsmem[rom_size-2]|(((DWORD)(wsmem[rom_size-1]))<<8);
	
		if((wsmem[rom_size-10]==0x01)&&(wsmem[rom_size-8]==0x27)) /* Detective Conan */
		{
		    /* WS cpu is using cache/pipeline or
			   there's protected ROM bank where 
			   pointing CS 
			*/
			wsmem[0xfffe8]=0xea;
			wsmem[0xfffe9]=0x00;
			wsmem[0xfffea]=0x00;
			wsmem[0xfffeb]=0x00;
			wsmem[0xfffec]=0x20;
			
		}

		cycles_line=677;

		if((( wsmem[rom_size-10]==0x01)&&(wsmem[rom_size-8]==0x23))|| //Final Lap special
			((wsmem[rom_size-10]==0x00)&&(wsmem[rom_size-8]==0x17))|| //turn tablis
			((wsmem[rom_size-10]==0x01)&&(wsmem[rom_size-8]==0x08))|| //klonoa
			((wsmem[rom_size-10]==0x26)&&(wsmem[rom_size-8]==0x01))|| //ring infinity
			((wsmem[rom_size-10]==0x01)&&(wsmem[rom_size-8]==0x04))|| //puyo puyo 2
			((wsmem[rom_size-10]==0x1b)&&(wsmem[rom_size-8]==0x03))|| //rainbow islands
			((wsmem[rom_size-10]==0x28)&&(wsmem[rom_size-8]==0x01))|| //FF1
			((wsmem[rom_size-10]==0x28)&&(wsmem[rom_size-8]==0x02)))  //FF2
				cycles_line=837;


		if(((wsmem[rom_size-10]==0x01)&&(wsmem[rom_size-8]==0x3)))//digimon tamers
			cycles_line=574;
		
	    if(wsmem[rom_size-4]&1)flipd=0;else flipd=1;
		flip_screen();
		wsReset();
			
		memset(&wsSRAM,0,65536);
		if(sram_size)
		{
			if(sram_size&1)
			{
				file=fopen(path_sram,"rb");
				if(file!=NULL)
				{
					fread(wsEEPROM,1,sram_size-1,file);
					fclose(file);
				}
			}
			else
			{
				file=fopen(path_sram,"rb");
				if(file!=NULL)
				{
					fread(wsSRAM,1,sram_size,file);
					fclose(file);
				}
			}
		}	
	}
}






void wsLoadState(void)
{
	char			buffer[1024];
	char			filter[]="Cygne save states\0*.wss\0";
	char			dirname[1024];
	char			text1[1024];
	char			cx[1024];
	OPENFILENAME	ofn;
	FILE*			file;
	unsigned		reg;
	WORD			newcrc;
	int				i0;
	long			size;

	buffer[0]=0;
	GetCurrentDirectory(sizeof(dirname),dirname);
	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.hwndOwner=window;
	ofn.lpstrFilter=filter;
	ofn.nFilterIndex=1;
	ofn.lpstrFile=buffer;
	ofn.lpstrInitialDir=dirname;
	ofn.nMaxFile=1024;
	ofn.Flags=OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
	GetOpenFileName(&ofn);
	if(buffer[0])
	{
		file=fopen(buffer,"rb");
		fseek(file,0,SEEK_END);
		size=ftell(file);
		fseek(file,0,SEEK_SET);
		
		fread(&newcrc,1,2,file);
		if(crc!=newcrc)
		{
					
			fread(&cx[0],1,1024,file);
			for(i0=0;i0<1024;i0++)if(cx[i0]=='.')cx[i0]=0;
			sprintf(text1,"Please load correct rom first - %s ",cx);
			MessageBox(window,text1,"Error",MB_OK|MB_ICONERROR);
			fclose(file);
			return;
		}
		fread(&path_rom[0],1,1024,file);
		fread(&reg,sizeof(reg),1,file);
		nec_set_reg(NEC_IP,reg);
		fread(&reg,sizeof(reg),1,file);
		nec_set_reg(NEC_AW,reg);
		fread(&reg,sizeof(reg),1,file);
		nec_set_reg(NEC_BW,reg);
		fread(&reg,sizeof(reg),1,file);
		nec_set_reg(NEC_CW,reg);
		fread(&reg,sizeof(reg),1,file);
		nec_set_reg(NEC_DW,reg);
		fread(&reg,sizeof(reg),1,file);
		nec_set_reg(NEC_CS,reg);
		fread(&reg,sizeof(reg),1,file);
		nec_set_reg(NEC_DS,reg);
		fread(&reg,sizeof(reg),1,file);
		nec_set_reg(NEC_ES,reg);
		fread(&reg,sizeof(reg),1,file);
		nec_set_reg(NEC_SS,reg);
		fread(&reg,sizeof(reg),1,file);
		nec_set_reg(NEC_IX,reg);
		fread(&reg,sizeof(reg),1,file);
		nec_set_reg(NEC_IY,reg);
		fread(&reg,sizeof(reg),1,file);
		nec_set_reg(NEC_BP,reg);
		fread(&reg,sizeof(reg),1,file);
		nec_set_reg(NEC_SP,reg);
		fread(&reg,sizeof(reg),1,file);
		nec_set_reg(NEC_FLAGS,reg);
		fread(&wsRAM[0],1,65536,file);
		fread(&wsSRAM[0],1,65536,file);
		fread(&wsIO[0],1,256,file);
		fread(&wsColors[0],4,8,file);
		fread(&wsMonoPal[0],4,64*4,file);
		fread(&wsCols[0][0],1,16*16*4,file);
		fread(&rom_size,1,4,file);
		fread(&wsLine,1,1,file);
		fread(&wsVMode,1,4,file);
		fread(&sram_size,1,4,file);
		fread(&flipd,1,4,file);
		fclose(file);
		wsSetVideo(wsVMode,true);
		strcpy(path_sram,path_save);
		strcat(path_sram,path_rom);
		wsCycles=0;
		wsMakeScr=0;
	}
}


void wsSaveState(void)
{
	char			cBuffer[1024];
	char			cFilter[]="Cygne save states\0*.wss\0";
	char			cDirname[1024];
	OPENFILENAME	ofn;
	FILE*			fState;
	unsigned		uReg;
	
	cBuffer[0]=0;
	GetCurrentDirectory(sizeof(dirname),dirname);
	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.hwndOwner=window;
	ofn.lpstrFilter=cFilter;
	ofn.nFilterIndex=1;
	ofn.lpstrFile=cBuffer;
	ofn.lpstrInitialDir=cDirname;
	ofn.nMaxFile=1024;
	ofn.Flags=OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
	GetSaveFileName(&ofn);
	if(cBuffer[0])
	{
		strtok(cBuffer,".");
		strcat(cBuffer,".wss");
		fState=fopen(cBuffer,"wb");
		fwrite(&crc,1,2,fState);
		fwrite(&path_rom[0],1,1024,fState);

		mSTOREREG(NEC_IP);
		mSTOREREG(NEC_AW);
		mSTOREREG(NEC_BW);
		mSTOREREG(NEC_CW);
		mSTOREREG(NEC_DW);
		mSTOREREG(NEC_CS);
		mSTOREREG(NEC_DS);
		mSTOREREG(NEC_ES);
		mSTOREREG(NEC_SS);
		mSTOREREG(NEC_IX);
		mSTOREREG(NEC_IY);
		mSTOREREG(NEC_BP);
		mSTOREREG(NEC_SP);
		mSTOREREG(NEC_FLAGS);
		
		fwrite(&wsRAM[0],1,65536,fState);
		fwrite(&wsSRAM[0],1,65536,fState);
		fwrite(&wsIO[0],1,256,fState);
		fwrite(&wsColors[0],4,8,fState);
		fwrite(&wsMonoPal[0],4,64*4,fState);
		fwrite(&wsCols[0][0],1,16*16*4,fState);
		fwrite(&rom_size,1,4,fState);
		fwrite(&wsLine,1,1,fState);
		fwrite(&wsVMode,1,4,fState);
		fwrite(&sram_size,1,4,fState);
		fwrite(&flipd,1,4,fState);

		fwrite(&wsEEPROM,1,131072,fState);	

		fclose(fState);
	}
}
