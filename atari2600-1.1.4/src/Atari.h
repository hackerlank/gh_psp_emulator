#ifndef _ATARI_H_
#define _ATARI_H_

# ifdef __cplusplus
extern "C" {
# endif

//LUDO:
# define ATARI_RENDER_NORMAL   0
# define ATARI_RENDER_X125     1
# define ATARI_RENDER_MAX      2
# define ATARI_LAST_RENDER     2

# define ATARI_FLICKER_NONE      0
# define ATARI_FLICKER_SIMPLE    1
# define ATARI_FLICKER_AVERAGE   2
# define ATARI_FLICKER_PHOSPHOR  3
# define ATARI_LAST_FLICKER      3

# define MAX_PATH           256
# define ATARI_MAX_SAVE_STATE 5

#include <psptypes.h>

  typedef struct Atari_save_t {

    SDL_Surface    *surface;
    char            used;
    char            thumb;
    ScePspDateTime  date;

  } Atari_save_t;

  typedef struct Atari_t {
 
    Atari_save_t atari_save_state[ATARI_MAX_SAVE_STATE];

    char atari_save_name[MAX_PATH];
    char atari_home_dir[MAX_PATH];
    int  psp_screenshot_id;
    int  psp_cpu_clock;
    int  psp_reverse_analog;
    int  psp_display_lr;
    int  atari_view_fps;
    int  atari_current_fps;
    int  psp_active_joystick;
    int  atari_flicker_mode;
    int  atari_snd_enable;
    int  atari_render_mode;
    int  atari_vsync;
    int  atari_speed_limiter;
    int  psp_skip_max_frame;
    int  psp_skip_cur_frame;
    int  atari_slow_down_max;
    int  atari_paddle_enable;
    int  atari_paddle_speed;
    int  atari_auto_fire;
    int  atari_auto_fire_pressed;
    int  atari_auto_fire_period;

  } Atari_t;

  extern Atari_t ATARI;

  extern void main_atari_send_key_event(int atari_idx, int key_press);
  extern int  main_atari_load_state(char *filename);
  extern void main_atari_force_draw_blit();
  extern int  main_atari_save_state(char *filename);
  extern int main_atari_load_rom(char *filename);
  extern void main_atari_emulator_reset();

# ifdef __cplusplus
}
# endif

#endif
