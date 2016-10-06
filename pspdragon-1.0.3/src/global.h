#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#ifdef __cplusplus
extern "C" {
#endif

# define DRAGON_RENDER_NORMAL      0
# define DRAGON_RENDER_FIT_HEIGHT  1
# define DRAGON_RENDER_FIT         2
# define DRAGON_LAST_RENDER        2

# define MAX_PATH               256
# define DRAGON_MAX_SAVE_STATE    5

# define DRAGON_LOAD_K7_MODE      0
# define DRAGON_LOAD_DISK_MODE    1
# define DRAGON_MAX_LOAD_MODE     1

# define DRAGON_SCREEN_W    320
# define DRAGON_SCREEN_H    240
#define REAL_DRAGON_W  256
#define REAL_DRAGON_H  192
# define DRAGON_HEIGHT      DRAGON_SCREEN_H
# define DRAGON_WIDTH       DRAGON_SCREEN_W
# define SNAP_WIDTH   (REAL_DRAGON_W/2)
# define SNAP_HEIGHT  (REAL_DRAGON_H/2)


# include <psptypes.h>
#include <SDL.h>

  typedef struct DRAGON_save_t {

    SDL_Surface    *surface;
    char            used;
    char            thumb;
    ScePspDateTime  date;

  } DRAGON_save_t;

  typedef struct DRAGON_t {
 
    DRAGON_save_t dragon_save_state[DRAGON_MAX_SAVE_STATE];
    char dragon_save_name[MAX_PATH];
    char dragon_home_dir[MAX_PATH];
    int  dragon_speed_limiter;
    int  psp_screenshot_id;
    int  psp_cpu_clock;
    int  psp_reverse_analog;
    int  dragon_snd_enable;
    int  dragon_render_mode;
    int  dragon_view_fps;
    int  dragon_current_fps;
    int  dragon_joystick;
    int  dragon_joystick_step;
    int  dragon_joystick_anal;
    int  dragon_model;
    int  psp_display_lr;
    int  dragon_vsync;
    int  psp_skip_max_frame;
    int  psp_skip_cur_frame;
    int  dragon_auto_fire;
    int  dragon_auto_fire_pressed;
    int  dragon_auto_fire_period;
    int  video_artifact_mode;
    int  emu_started;

  } DRAGON_t;

  extern int dragon_in_menu;

  extern DRAGON_t DRAGON;

  extern void dragon_global_init();

//END_LUDO:
#ifdef __cplusplus
}
#endif

# endif
