/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_image.h>
#include <pspctrl.h>
#include <psptypes.h>
#include <png.h>
# ifndef LINUX_MODE
#include <pspgu.h>
#include <pspdisplay.h>
#include <psprtc.h>
# endif

#include "global.h"
#include "psp_sdl.h"
#include "psp_danzeff.h"
#include "psp_dragon.h"

int psp_screenshot_mode = 0;
int dragon_in_menu = 0;

static void
dragon_render_normal()
{
  SDL_Rect srcRect;
  SDL_Rect dstRect;

  srcRect.x = 32;
  srcRect.y = 24;
  srcRect.w = REAL_DRAGON_W;
  srcRect.h = REAL_DRAGON_H;
  dstRect.x = (480 - REAL_DRAGON_W) / 2;
  dstRect.y = (272 - REAL_DRAGON_H) / 2;
  dstRect.w = REAL_DRAGON_W;
  dstRect.h = REAL_DRAGON_H;

  psp_sdl_gu_stretch(&srcRect, &dstRect);
}

static void
dragon_render_fit_height()
{
  SDL_Rect srcRect;
  SDL_Rect dstRect;

  srcRect.x = 32;
  srcRect.y = 24;
  srcRect.w = REAL_DRAGON_W;
  srcRect.h = REAL_DRAGON_H;
  dstRect.x = (480 - 360) / 2;
  dstRect.y = 0;
  dstRect.w = 360;
  dstRect.h = 272;

  psp_sdl_gu_stretch(&srcRect, &dstRect);
}

static void
dragon_render_fit()
{
  SDL_Rect srcRect;
  SDL_Rect dstRect;

  srcRect.x = 32;
  srcRect.y = 24;
  srcRect.w = REAL_DRAGON_W;
  srcRect.h = REAL_DRAGON_H;
  dstRect.x = 0;
  dstRect.y = 0;
  dstRect.w = 480;
  dstRect.h = 272;

  psp_sdl_gu_stretch(&srcRect, &dstRect);
}

static void
dragon_synchronize(void)
{
	static u32 nextclock = 1;

  if (DRAGON.dragon_speed_limiter) {

	  if (nextclock) {
		  u32 curclock;
		  do {
        curclock = SDL_GetTicks();
		  } while (curclock < nextclock);
  
      nextclock = curclock + (u32)( 1000 / DRAGON.dragon_speed_limiter);
    }
  }
}

void
psp_dragon_wait_vsync()
{
#ifndef LINUX_MODE
  static int loc_pv = 0;
  int cv = sceDisplayGetVcount();
  if (loc_pv == cv) {
    sceDisplayWaitVblankCB();
  }
  loc_pv = sceDisplayGetVcount();
#endif
}

void
dragon_update_fps()
{
  static u32 next_sec_clock = 0;
  static u32 cur_num_frame = 0;
  cur_num_frame++;
  u32 curclock = SDL_GetTicks();
  if (curclock > next_sec_clock) {
    next_sec_clock = curclock + 1000;
    DRAGON.dragon_current_fps = cur_num_frame;
    cur_num_frame = 0;
  }
}

extern int psp_in_dragon_menu;

void
psp_sdl_render()
{
  if (dragon_in_menu) return;

  if (DRAGON.psp_skip_cur_frame <= 0) {

    DRAGON.psp_skip_cur_frame = DRAGON.psp_skip_max_frame;

    if (DRAGON.dragon_render_mode == DRAGON_RENDER_NORMAL) dragon_render_normal();
    else                          
    if (DRAGON.dragon_render_mode == DRAGON_RENDER_FIT_HEIGHT) dragon_render_fit_height();
    else                          
    if (DRAGON.dragon_render_mode == DRAGON_RENDER_FIT) dragon_render_fit();

    if (psp_kbd_is_danzeff_mode()) {
      sceDisplayWaitVblankStart();
      danzeff_moveTo(-165, -50);
      danzeff_render();
    }

    if (DRAGON.dragon_view_fps) {
      char buffer[32];
      sprintf(buffer, "%3d", (int)DRAGON.dragon_current_fps);
      psp_sdl_fill_print(0, 0, buffer, 0xffffff, 0 );
    }

    if (DRAGON.psp_display_lr) {
      psp_kbd_display_active_mapping();
    }
    if (DRAGON.dragon_vsync) {
      psp_dragon_wait_vsync();
    }
    psp_sdl_flip();
  
    if (psp_screenshot_mode) {
      psp_screenshot_mode--;
      if (psp_screenshot_mode <= 0) {
        psp_sdl_save_screenshot();
        psp_screenshot_mode = 0;
      }
    }

  } else if (DRAGON.psp_skip_max_frame) {
    DRAGON.psp_skip_cur_frame--;
  }

  if (DRAGON.dragon_speed_limiter) {
    dragon_synchronize();
  }

  if (DRAGON.dragon_view_fps) {
    dragon_update_fps();
  }
}
