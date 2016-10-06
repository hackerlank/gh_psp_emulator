/*
 ===================================================================================
		Cygne WIN  v 2.1a      (c) Dox 2002     dox@space.pl
 ===================================================================================

   NEC cpu core  by Bryan McPhail,Oliver Bergmann, Fabrice Frances and David Hedley
   Zlib  by  Jean-loup Gailly and Mark Adler

 ===================================================================================
   Memory  - read/write WS memory (RAM, SRAM, ROM) and I/O
 ===================================================================================*/

extern BYTE		wsIO[256];
extern BYTE		wsRAM[65536];
extern BYTE		wsSRAM[65536];
extern BYTE		wsmem[16*1024*1024];
extern DWORD	sram_size;
extern BYTE     wsEEPROM[131072];

#define  mBCD(value) ((value/10)<<4)|(value%10)




