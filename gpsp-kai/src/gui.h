/* gameplaySP
 *
 * Copyright (C) 2006 Exophase <exophase@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef GUI_H
#define GUI_H

#define GPSP_CONFIG_FILENAME "gpsp.cfg"

s32 load_file(u8 **wildcards, u8 *result, u8 *default_dir_name);
u32 adjust_frameskip(u32 button_id);
s32 load_game_config_file();
s32 load_config_file();
s32 save_game_config_file();
s32 save_config_file();
u32 menu(u16 *original_screen);

u32 load_dircfg(char *file_name);
u32 load_fontcfg(char *file_name);
u32 load_msgcfg(char *file_name);
u32 parse_line(u8 *current_line, u8 *current_str);
u32 load_font();

void print_status();
void get_timestamp_string(char *buffer, u16 msg_id, u16 year, u16 mon, u16 day, u16 wday, u16 hour, u16 min, u16 sec, u32 msec);
void save_ss_bmp(u16 *image);

extern u32 savestate_slot;

void get_savestate_filename_noshot(u32 slot, u8 *name_buffer);
void get_savestate_filename(u32 slot, u8 *name_buffer);
void get_savestate_snapshot(u8 *savestate_filename);

extern char default_rom_dir[512];
extern char default_save_dir[512];
extern char default_cfg_dir[512];
extern char default_ss_dir[512];
extern char default_cheat_dir[512];

#endif

