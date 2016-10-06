/*
 ===================================================================================
		Cygne WIN  v 2.0      (c) Dox 2001     dox@space.pl
 ===================================================================================

   NEC cpu core  by Bryan McPhail,Oliver Bergmann, Fabrice Frances and David Hedley
   Zlib  by  Jean-loup Gailly and Mark Adler

 ===================================================================================
*/
#define		cpu_readop cpu_readmem20	
#define		cpu_readop_arg cpu_readmem20	
enum {
	NEC_IP=1, NEC_AW, NEC_CW, NEC_DW, NEC_BW, NEC_SP, NEC_BP, NEC_IX, NEC_IY,
	NEC_FLAGS, NEC_ES, NEC_CS, NEC_SS, NEC_DS,
	NEC_VECTOR, NEC_PENDING, NEC_NMI_STATE, NEC_IRQ_STATE};


extern		DWORD k_up1;
extern		DWORD k_down1;
extern		DWORD k_left1;
extern		DWORD k_right1;
extern		DWORD k_up2;
extern		DWORD k_down2;
extern		DWORD k_left2;
extern		DWORD k_right2;
extern		DWORD k_a;
extern		DWORD k_b;
extern		DWORD k_start;
extern		DWORD k_up1i;
extern		DWORD k_down1i;
extern		DWORD k_left1i;
extern		DWORD k_right1i;
extern		DWORD k_up2i;
extern		DWORD k_down2i;
extern		DWORD k_left2i;
extern		DWORD k_right2i;
extern		DWORD k_ai;
extern		DWORD k_bi;
extern		DWORD k_starti;
extern		DWORD k_flipi;
extern		DWORD link_controls;
extern		DWORD colors[16];
extern		DWORD fr;
extern		DWORD frameskip;
extern		char path_app[1024];
extern		char path_save[1024];
extern		char path_sram[1024];
extern		char path_rom[1024];
extern		DWORD l_f,c_f;
extern		DWORD wsMakeScr;
extern		BYTE vsync;
extern		HINSTANCE instance;
extern		BOOL f_load;
extern		BOOL f_stopped;
extern		DWORD flipd;
extern		int	wsShades;
extern		BYTE dx_buffer[224*144*4];
extern		HWND window;
extern		char dirname[2048];
extern		DWORD wsCycles;
extern		BYTE wsLine;
extern		DWORD rom_size;
extern		int screen_size;
extern		DWORD crc;
extern		int wsc;
extern		const DWORD tabkey[47];
	

void nec_set_reg(int,unsigned);
void wsReset(void);
void wsSaveState(void);
void wsLoadState(void);
void wsROMLoad(HWND);
void saveSRAM(void);
BOOL CALLBACK cproc(HWND,UINT,WPARAM,LPARAM);
void WriteRegistry(void);
void ReadRegistry(void);
void closedx(void);
BYTE  cpu_readmem20(DWORD);
void init_mem(void);
void deinit_mem(void);
void wsDefaultKeys();
void refresh_menu_colors(void);
void set_size(int);
void flip_screen(void);
void WriteRegistry(void);
int nec_execute(int cycles);	
unsigned nec_get_reg(int regnum);
void nec_reset (void *param);
void cpu_writemem20(DWORD adr,BYTE co);
BYTE cpu_readport(BYTE adr);
void cpu_writeport(DWORD adr,BYTE co);
void UpdateFrame(HWND,int);
int start_dx(void);
void set_shades(void);
void nec_int(DWORD wektor);
void make_screen(void);
void UpdateFrame(HWND,int);
int start_dx(void);
void set_shades(void);
void nec_int(DWORD wektor);
void make_screen(void);
