/* gameplaySP
 *
 * Copyright (C) 2006 Exophase <exophase@gmail.com>
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licens e as
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

#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <psprtc.h>
#include <psputility_sysparam.h>
#include "common.h"

#define MAX_PATH 1024

// Blatantly stolen and trimmed from MZX (megazeux.sourceforge.net)

#define STATUS_ROWS 0
#define CURRENT_DIR_ROWS 1
#define FILE_LIST_ROWS 25
#define FILE_LIST_POSITION 5
#define DIR_LIST_POSITION 360
#define PAGE_SCROLL_NUM 5

#ifdef PSP_BUILD

#define color16(red, green, blue)                                             \
  (blue << 11) | (green << 5) | red                                           \

#else

#define color16(red, green, blue)                                             \
  (red << 11) | (green << 5) | blue                                           \

#endif

#define COLOR_BG            color16(2, 8, 10)
#define COLOR_ROM_INFO      color16(22, 36, 26)
#define COLOR_ACTIVE_ITEM   color16(31, 63, 31)
#define COLOR_INACTIVE_ITEM color16(13, 40, 18)
#define COLOR_FRAMESKIP_BAR color16(15, 31, 31)
#define COLOR_HELP_TEXT     color16(16, 40, 24)

char font8[512];
char font16[512];
char default_rom_dir[512];
char default_save_dir[512];
char default_cfg_dir[512];
char default_ss_dir[512];
char default_cheat_dir[512];

int sort_function(const void *dest_str_ptr, const void *src_str_ptr)
{
  char *dest_str = *((char **)dest_str_ptr);
  char *src_str = *((char **)src_str_ptr);

  if(src_str[0] == '.')
    return 1;

  if(dest_str[0] == '.')
    return -1;

  return strcasecmp(dest_str, src_str);
}

// 汎用ファイル読込み
s32 load_file(u8 **wildcards, u8 *result,u8 *default_dir_name)
{
  DIR *current_dir;
  struct dirent *current_file;
  struct stat file_info;
  u8 current_dir_name[MAX_PATH];
  u8 current_dir_short[81];
  u32 current_dir_length;
  u32 total_filenames_allocated;
  u32 total_dirnames_allocated;
  u8 **file_list;
  u8 **dir_list;
  u32 num_files;
  u32 num_dirs;
  u8 *file_name;
  u32 file_name_length;
  u32 ext_pos = -1;
  u32 chosen_file, chosen_dir;
  u32 dialog_result = 1;
  s32 return_value = 1;
  u32 current_file_selection;
  u32 current_file_scroll_value;
  u32 current_dir_selection;
  u32 current_dir_scroll_value;
  u32 current_file_in_scroll;
  u32 current_dir_in_scroll;
  u32 current_file_number, current_dir_number;
  u32 current_column = 0;
  u32 repeat;
  u32 i;
  gui_action_type gui_action;

  if (default_dir_name != NULL)
    chdir(default_dir_name);

  while(return_value == 1)
  {
    current_file_selection = 0;
    current_file_scroll_value = 0;
    current_dir_selection = 0;
    current_dir_scroll_value = 0;
    current_file_in_scroll = 0;
    current_dir_in_scroll = 0;

    total_filenames_allocated = 32;
    total_dirnames_allocated = 32;
    file_list = (u8 **)malloc(sizeof(u8 *) * 32);
    dir_list = (u8 **)malloc(sizeof(u8 *) * 32);
    memset(file_list, 0, sizeof(u8 *) * 32);
    memset(dir_list, 0, sizeof(u8 *) * 32);

    num_files = 0;
    num_dirs = 0;
    chosen_file = 0;
    chosen_dir = 0;

    getcwd(current_dir_name, MAX_PATH);
    current_dir = opendir(current_dir_name);
    
    do
    {
      if(current_dir)
        current_file = readdir(current_dir);
      else
        current_file = NULL;

      if(current_file)
      {
        file_name = current_file->d_name;
        file_name_length = strlen(file_name);

        if((stat(file_name, &file_info) >= 0) &&
         ((file_name[0] != '.') || (file_name[1] == '.')))
        {
          if(S_ISDIR(file_info.st_mode))
          {
            dir_list[num_dirs] =
             (u8 *)malloc(file_name_length + 1);
             sprintf(dir_list[num_dirs], "%s", file_name);

            num_dirs++;
          }
          else
          {
            // Must match one of the wildcards, also ignore the .
            if(file_name_length >= 4)
            {
              if(file_name[file_name_length - 4] == '.')
                ext_pos = file_name_length - 4;
              else

              if(file_name[file_name_length - 3] == '.')
                ext_pos = file_name_length - 3;

              else
                ext_pos = 0;

              for(i = 0; wildcards[i] != NULL; i++)
              {
                if(!strcasecmp((file_name + ext_pos),
                 wildcards[i]))
                {
                  file_list[num_files] =
                   (u8 *)malloc(file_name_length + 1);

                  sprintf(file_list[num_files], "%s", file_name);

                  num_files++;
                  break;
                }
              }
            }
          }
        }

        if(num_files == total_filenames_allocated)
        {
          file_list = (u8 **)realloc(file_list, sizeof(u8 *) *
           total_filenames_allocated * 2);
          memset(file_list + total_filenames_allocated, 0,
           sizeof(u8 *) * total_filenames_allocated);
          total_filenames_allocated *= 2;
        }

        if(num_dirs == total_dirnames_allocated)
        {
          dir_list = (u8 **)realloc(dir_list, sizeof(u8 *) *
           total_dirnames_allocated * 2);
          memset(dir_list + total_dirnames_allocated, 0,
           sizeof(u8 *) * total_dirnames_allocated);
          total_dirnames_allocated *= 2;
        }
      }
    } while(current_file);

    qsort((void *)file_list, num_files, sizeof(u8 *), sort_function);
    qsort((void *)dir_list, num_dirs, sizeof(u8 *), sort_function);

    closedir(current_dir);

    current_dir_length = strlen(current_dir_name);

    if(current_dir_length > 80)
    {
      memcpy(current_dir_short, "...", 3);
      memcpy(current_dir_short + 3,
       current_dir_name + current_dir_length - 77, 77);
      current_dir_short[80] = 0;
    }
    else
    {
      memcpy(current_dir_short, current_dir_name,
       current_dir_length + 1);
    }

    repeat = 1;

    if(num_files == 0)
      current_column = 1;

    clear_screen(COLOR_BG);
  {
    u8 print_buffer[81];

    while(repeat)
    {
      flip_screen();

      print_status();
      print_string(current_dir_short, COLOR_ACTIVE_ITEM, COLOR_BG, 0, (CURRENT_DIR_ROWS * 10));
      print_string(msg[MSG_RETURN_MENU], COLOR_HELP_TEXT, COLOR_BG, 20, 260);

      for(i = 0, current_file_number = i + current_file_scroll_value;
       i < (FILE_LIST_ROWS - CURRENT_DIR_ROWS); i++, current_file_number++)
      {
        if(current_file_number < num_files)
        {
          if((current_file_number == current_file_selection) &&
           (current_column == 0))
          {
            print_string(file_list[current_file_number], COLOR_ACTIVE_ITEM,
             COLOR_BG, FILE_LIST_POSITION, ((i + CURRENT_DIR_ROWS + 1) * 10));
          }
          else
          {
            print_string(file_list[current_file_number], COLOR_INACTIVE_ITEM,
             COLOR_BG, FILE_LIST_POSITION, ((i + CURRENT_DIR_ROWS + 1) * 10));
          }
        }
      }

      for(i = 0, current_dir_number = i + current_dir_scroll_value;
       i < (FILE_LIST_ROWS - CURRENT_DIR_ROWS); i++, current_dir_number++)
      {
        if(current_dir_number < num_dirs)
        {
          if((current_dir_number == current_dir_selection) &&
           (current_column == 1))
          {
            print_string(dir_list[current_dir_number], COLOR_ACTIVE_ITEM,
             COLOR_BG, DIR_LIST_POSITION, ((i + CURRENT_DIR_ROWS + 1) * 10));
          }
          else
          {
            print_string(dir_list[current_dir_number], COLOR_INACTIVE_ITEM,
             COLOR_BG, DIR_LIST_POSITION, ((i + CURRENT_DIR_ROWS + 1) * 10));
          }
        }
      }

      gui_action = get_gui_input();

      switch(gui_action)
      {
        case CURSOR_DOWN:
          if(current_column == 0)
          {
            if(current_file_selection < (num_files - 1))
            {
              current_file_selection++;
              if(current_file_in_scroll == (FILE_LIST_ROWS - CURRENT_DIR_ROWS - 1))
              {
                clear_screen(COLOR_BG);
                current_file_scroll_value++;
              }
              else
              {
                current_file_in_scroll++;
              }
            }
          }
          else
          {
            if(current_dir_selection < (num_dirs - 1))
            {
              current_dir_selection++;
              if(current_dir_in_scroll == (FILE_LIST_ROWS - CURRENT_DIR_ROWS - 1))
              {
                clear_screen(COLOR_BG);
                current_dir_scroll_value++;
              }
              else
              {
                current_dir_in_scroll++;
              }
            }
          }

          break;

        case CURSOR_RTRIGGER:
          if(current_column == 0)
          {
            if(num_files > PAGE_SCROLL_NUM)
            {
              if(current_file_selection < (num_files - PAGE_SCROLL_NUM))
              {
                current_file_selection += PAGE_SCROLL_NUM;
                if(current_file_in_scroll >= (FILE_LIST_ROWS - CURRENT_DIR_ROWS - PAGE_SCROLL_NUM))
                {
                  clear_screen(COLOR_BG);
                  current_file_scroll_value += PAGE_SCROLL_NUM;
                }
                else
                {
                  current_file_in_scroll += PAGE_SCROLL_NUM;
                }
              }
            }
          }
          else
          {
            if(num_dirs > PAGE_SCROLL_NUM)
            {
              if(current_dir_selection < (num_dirs - PAGE_SCROLL_NUM))
              {
                current_dir_selection += PAGE_SCROLL_NUM;
                if(current_dir_in_scroll >= (FILE_LIST_ROWS - CURRENT_DIR_ROWS - PAGE_SCROLL_NUM))
                {
                clear_screen(COLOR_BG);
                current_dir_scroll_value += PAGE_SCROLL_NUM;
                }
                else
                {
                  current_dir_in_scroll += PAGE_SCROLL_NUM;
                }
              }
            }
          }
          break;

        case CURSOR_UP:
          if(current_column == 0)
          {
            if(current_file_selection)
            {
              current_file_selection--;
              if(current_file_in_scroll == 0)
              {
                clear_screen(COLOR_BG);
                current_file_scroll_value--;
              }
              else
              {
                current_file_in_scroll--;
              }
            }
          }
          else
          {
            if(current_dir_selection)
            {
              current_dir_selection--;
              if(current_dir_in_scroll == 0)
              {
                clear_screen(COLOR_BG);
                current_dir_scroll_value--;
              }
              else
              {
                current_dir_in_scroll--;
              }
            }
          }
          break;

        case CURSOR_LTRIGGER:
          if(current_column == 0)
          {
            if(current_file_selection >= PAGE_SCROLL_NUM)
            {
              current_file_selection -= PAGE_SCROLL_NUM;
              if(current_file_in_scroll < PAGE_SCROLL_NUM)
              {
                clear_screen(COLOR_BG);
                current_file_scroll_value -= PAGE_SCROLL_NUM;
              }
              else
              {
                current_file_in_scroll -= PAGE_SCROLL_NUM;
              }
            }
          }
          else
          {
            if(current_dir_selection >= PAGE_SCROLL_NUM)
            {
              current_dir_selection -= PAGE_SCROLL_NUM;
              if(current_dir_in_scroll < PAGE_SCROLL_NUM)
              {
                clear_screen(COLOR_BG);
                current_dir_scroll_value -= PAGE_SCROLL_NUM;
              }
              else
              {
                current_dir_in_scroll -= PAGE_SCROLL_NUM;
              }
            }
          }
          break;

        case CURSOR_RIGHT:
          if(current_column == 0)
          {
            if(num_dirs != 0)
              current_column = 1;
          }
          break;

        case CURSOR_LEFT:
          if(current_column == 1)
          {
            if(num_files != 0)
              current_column = 0;
          }
          break;

        case CURSOR_SELECT:
          if(current_column == 1)
          {
            repeat = 0;
            chdir(dir_list[current_dir_selection]);
          }
          else
          {
            if(num_files != 0)
            {
              repeat = 0;
              return_value = 0;
              strcpy(result, file_list[current_file_selection]);
            }
          }
          break;

        case CURSOR_BACK:
#ifdef PSP_BUILD
          if(!strcmp(current_dir_name, "ms0:/PSP"))
            break;
#endif
          repeat = 0;
          chdir("..");
          break;

        case CURSOR_EXIT:
          return_value = -1;
          repeat = 0;
          break;
      }
    }
  }
    for(i = 0; i < num_files; i++)
    {
      free(file_list[i]);
    }
    free(file_list);

    for(i = 0; i < num_dirs; i++)
    {
      free(dir_list[i]);
    }
    free(dir_list);
  }

  clear_screen(COLOR_BG);

  return return_value;
}

typedef enum
{
  NUMBER_SELECTION_OPTION = 0x01,
  STRING_SELECTION_OPTION = 0x02,
  SUBMENU_OPTION          = 0x04,
  ACTION_OPTION           = 0x08
} menu_option_type_enum;

struct _menu_type
{
  void (* init_function)();
  void (* passive_function)();
  struct _menu_option_type *options;
  u32 num_options;
};

struct _menu_option_type
{
  void (* action_function)();
  void (* passive_function)();
  struct _menu_type *sub_menu;
  char *display_string;
  void *options;
  u32 *current_option;
  u32 num_options;
  char *help_string;
  u32 line_number;
  menu_option_type_enum option_type;
};

typedef struct _menu_option_type menu_option_type;
typedef struct _menu_type menu_type;

#define make_menu(name, init_function, passive_function)                      \
  menu_type name##_menu =                                                     \
  {                                                                           \
    init_function,                                                            \
    passive_function,                                                         \
    name##_options,                                                           \
    sizeof(name##_options) / sizeof(menu_option_type)                         \
  }                                                                           \

#define gamepad_config_option(display_string, number)                         \
{                                                                             \
  NULL,                                                                       \
  menu_fix_gamepad_help,                                                      \
  NULL,                                                                       \
  display_string,                                                             \
  gamepad_config_buttons,                                                     \
  gamepad_config_map + gamepad_config_line_to_button[number],             \
  sizeof(gamepad_config_buttons) / sizeof(gamepad_config_buttons[0]),         \
  gamepad_help[gamepad_config_map[                                            \
   gamepad_config_line_to_button[number]]],                               \
  number,                                                                     \
  STRING_SELECTION_OPTION                                                     \
}                                                                             \

#define analog_config_option(display_string, number)                          \
{                                                                             \
  NULL,                                                                       \
  menu_fix_gamepad_help,                                                      \
  NULL,                                                                       \
  display_string,                                                             \
  gamepad_config_buttons,                                                     \
  gamepad_config_map + number + 12,                                           \
  sizeof(gamepad_config_buttons) / sizeof(gamepad_config_buttons[0]),         \
  gamepad_help[gamepad_config_map[number + 12]],                              \
  number + 2,                                                                 \
  STRING_SELECTION_OPTION                                                     \
}                                                                             \

#define cheat_option(number)                                                  \
{                                                                             \
  NULL,                                                                       \
  NULL,                                                                       \
  NULL,                                                                       \
  cheat_format_str[number],                                                   \
  enable_disable_options,                                                     \
  &(cheats[number].cheat_active),                                             \
  2,                                                                          \
  msg[MSG_CHEAT_MENU_HELP_0],                                                 \
  number,                                                                     \
  STRING_SELECTION_OPTION                                                     \
}                                                                             \

#define action_option(action_function, passive_function, display_string,      \
 help_string, line_number)                                                    \
{                                                                             \
  action_function,                                                            \
  passive_function,                                                           \
  NULL,                                                                       \
  display_string,                                                             \
  NULL,                                                                       \
  NULL,                                                                       \
  0,                                                                          \
  help_string,                                                                \
  line_number,                                                                \
  ACTION_OPTION                                                               \
}                                                                             \

#define submenu_option(sub_menu, display_string, help_string, line_number)    \
{                                                                             \
  NULL,                                                                       \
  NULL,                                                                       \
  sub_menu,                                                                   \
  display_string,                                                             \
  NULL,                                                                       \
  NULL,                                                                       \
  sizeof(sub_menu) / sizeof(menu_option_type),                                \
  help_string,                                                                \
  line_number,                                                                \
  SUBMENU_OPTION                                                              \
}                                                                             \

#define selection_option(passive_function, display_string, options,           \
 option_ptr, num_options, help_string, line_number, type)                     \
{                                                                             \
  NULL,                                                                       \
  passive_function,                                                           \
  NULL,                                                                       \
  display_string,                                                             \
  options,                                                                    \
  option_ptr,                                                                 \
  num_options,                                                                \
  help_string,                                                                \
  line_number,                                                                \
  type                                                                        \
}                                                                             \

#define action_selection_option(action_function, passive_function,            \
 display_string, options, option_ptr, num_options, help_string, line_number,  \
 type)                                                                        \
{                                                                             \
  action_function,                                                            \
  passive_function,                                                           \
  NULL,                                                                       \
  display_string,                                                             \
  options,                                                                    \
  option_ptr,                                                                 \
  num_options,                                                                \
  help_string,                                                                \
  line_number,                                                                \
  type | ACTION_OPTION                                                        \
}                                                                             \


#define string_selection_option(passive_function, display_string, options,    \
 option_ptr, num_options, help_string, line_number)                           \
  selection_option(passive_function, display_string, options,                 \
   option_ptr, num_options, help_string, line_number, STRING_SELECTION_OPTION)\

#define numeric_selection_option(passive_function, display_string,            \
 option_ptr, num_options, help_string, line_number)                           \
  selection_option(passive_function, display_string, NULL, option_ptr,        \
   num_options, help_string, line_number, NUMBER_SELECTION_OPTION)            \

#define string_selection_action_option(action_function, passive_function,     \
 display_string, options, option_ptr, num_options, help_string, line_number)  \
  action_selection_option(action_function, passive_function,                  \
   display_string,  options, option_ptr, num_options, help_string,            \
   line_number, STRING_SELECTION_OPTION)                                      \

#define numeric_selection_action_option(action_function, passive_function,    \
 display_string, option_ptr, num_options, help_string, line_number)           \
  action_selection_option(action_function, passive_function,                  \
   display_string,  NULL, option_ptr, num_options, help_string,               \
   line_number, NUMBER_SELECTION_OPTION)                                      \

#define numeric_selection_action_hide_option(action_function,                 \
 passive_function, display_string, option_ptr, num_options, help_string,      \
 line_number)                                                                 \
  action_selection_option(action_function, passive_function,                  \
   display_string, NULL, option_ptr, num_options, help_string,                \
   line_number, NUMBER_SELECTION_OPTION)                                      \


#define GAMEPAD_MENU_WIDTH 15

u32 gamepad_config_line_to_button[] =
 { 8, 6, 7, 9, 1, 2, 3, 0, 4, 5, 11, 10 };

s32 load_game_config_file()
{
  u8 game_config_filename[512];
  u8 game_config_path[1024];
  u32 file_loaded = 0;
  u32 i;

  change_ext(gamepak_filename, game_config_filename, ".cfg");

  if (default_cfg_dir != NULL) {
    sprintf(game_config_path, "%s/%s", default_cfg_dir, game_config_filename);
  }
  else
  {
    strcpy(game_config_path, game_config_filename);
  }

  file_open(game_config_file, game_config_path, read);

  if(file_check_valid(game_config_file))
  {
    u32 file_size = file_length(game_config_path, game_config_file);

    // Sanity check: File size must be the right size
    if(file_size == 56)
    {
      u32 file_options[file_size / 4];

      file_read_array(game_config_file, file_options);
      current_frameskip_type = file_options[0] % 3;
      frameskip_value = file_options[1];
      random_skip = file_options[2] % 2;
      clock_speed = file_options[3];

      if(clock_speed > 333)
        clock_speed = 333;

      if(clock_speed < 33)
        clock_speed = 33;

      if(frameskip_value < 0)
        frameskip_value = 0;

      if(frameskip_value > 99)
        frameskip_value = 99;

      for(i = 0; i < 10; i++)
      {
        cheats[i].cheat_active = file_options[3 + i] % 2;
        cheats[i].cheat_name[0] = 0;
      }

      file_close(game_config_file);
      file_loaded = 1;
    }
  }

  if(file_loaded)
    return 0;

  current_frameskip_type = auto_frameskip;
  frameskip_value = 4;
  random_skip = 0;
  clock_speed = 333;

  for(i = 0; i < 10; i++)
  {
    cheats[i].cheat_active = 0;
    cheats[i].cheat_name[0] = 0;
  }

  return -1;
}

s32 load_config_file()
{
  u8 config_path[512];
  #ifdef PSP_BUILD
    sprintf(config_path, "%s/%s", main_path, GPSP_CONFIG_FILENAME);
  #else
    sprintf(config_path, "%s\\%s", main_path, GPSP_CONFIG_FILENAME);
  #endif

  file_open(config_file, config_path, read);

  if(file_check_valid(config_file))
  {
    u32 file_size = file_length(config_path, config_file);

    // Sanity check: File size must be the right size
    if(file_size == 92)
    {
      u32 file_options[file_size / 4];
      u32 i;
      s32 menu_button = -1;
      file_read_array(config_file, file_options);

      screen_scale = file_options[0] % 3;
      screen_filter = file_options[1] % 2;
      global_enable_audio = file_options[2] % 2;
      audio_buffer_size_number = file_options[3] % 10;
      update_backup_flag = file_options[4] % 2;
      global_enable_analog = file_options[5] % 2;
      analog_sensitivity_level = file_options[6] % 8;

#ifdef PSP_BUILD
    scePowerSetClockFrequency(clock_speed, clock_speed, clock_speed / 2);
#endif

      // Sanity check: Make sure there's a MENU or FRAMESKIP
      // key, if not assign to triangle

      for(i = 0; i < 16; i++)
      {
        gamepad_config_map[i] = file_options[7 + i] %
         (BUTTON_ID_NONE + 1);

        if(gamepad_config_map[i] == BUTTON_ID_MENU)
        {
          menu_button = i;
        }
      }

      if(menu_button == -1)
      {
        gamepad_config_map[0] = BUTTON_ID_MENU;
      }

      file_close(config_file);
    }

    return 0;
  }

  return -1;
}

s32 save_game_config_file()
{
  u8 game_config_filename[512];
  u8 game_config_path[1024];
  u32 i;

  if(gamepak_filename[0] == 0) return -1;

  change_ext(gamepak_filename, game_config_filename, ".cfg");

  if (default_cfg_dir != NULL) {
    sprintf(game_config_path, "%s/%s", default_cfg_dir, game_config_filename);
  }
  else
  {
    strcpy(game_config_path, game_config_filename);
  }

  file_open(game_config_file, game_config_path, write);

  if(file_check_valid(game_config_file))
  {
    u32 file_options[14];

    file_options[0] = current_frameskip_type;
    file_options[1] = frameskip_value;
    file_options[2] = random_skip;
    file_options[3] = clock_speed;

    for(i = 0; i < 10; i++)
    {
      file_options[4 + i] = cheats[i].cheat_active;
    }

    file_write_array(game_config_file, file_options);
    file_close(game_config_file);

    return 0;
  }

  return -1;
}

s32 save_config_file()
{
  u8 config_path[512];
  #ifdef PSP_BUILD
    sprintf(config_path, "%s/%s", main_path, GPSP_CONFIG_FILENAME);
  #else
    sprintf(config_path, "%s\\%s", main_path, GPSP_CONFIG_FILENAME);
  #endif

  file_open(config_file, config_path, write);

  save_game_config_file();

  if(file_check_valid(config_file))
  {
    u32 file_options[23];
    u32 i;

    file_options[0] = screen_scale;
    file_options[1] = screen_filter;
    file_options[2] = global_enable_audio;
    file_options[3] = audio_buffer_size_number;
    file_options[4] = update_backup_flag;
    file_options[5] = global_enable_analog;
    file_options[6] = analog_sensitivity_level;

    for(i = 0; i < 16; i++)
    {
      file_options[7 + i] = gamepad_config_map[i];
    }

    file_write_array(config_file, file_options);
    file_close(config_file);

    return 0;
  }

  return -1;
}

typedef enum
{
  MAIN_MENU,
  GAMEPAD_MENU,
  SAVESTATE_MENU,
  FRAMESKIP_MENU,
  CHEAT_MENU
} menu_enum;

u32 savestate_slot = 0;

void get_savestate_snapshot(u8 *savestate_filename)
{
  u16 snapshot_buffer[240 * 160];
  u8 savestate_timestamp_string[80];
  u8 savestate_path[1024];

  if (default_save_dir != NULL) {
    sprintf(savestate_path, "%s/%s", default_save_dir, savestate_filename);
  }
  else
  {
    strcpy(savestate_path, savestate_filename);
  }

  file_open(savestate_file, savestate_path, read);

  if(file_check_valid(savestate_file))
  {
    time_t savestate_time_flat;
    time_t local_time;
    u64 utc;
    u64 local;
    int time_diff;
    struct tm *current_time;
    file_read_array(savestate_file, snapshot_buffer);
    file_read_variable(savestate_file, savestate_time_flat);

    file_close(savestate_file);

    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_TIMEZONE,&time_diff);
    local_time = savestate_time_flat + (time_diff * 60);

    current_time = localtime(&local_time);

    get_timestamp_string(savestate_timestamp_string, MSG_STATE_MENU_DATE_FMT_0, current_time->tm_year + 1900, current_time->tm_mon + 1, current_time->tm_mday,
      current_time->tm_wday, current_time->tm_hour, current_time->tm_min, current_time->tm_sec, 0);

    savestate_timestamp_string[40] = 0;

    print_string(savestate_timestamp_string, COLOR_HELP_TEXT, COLOR_BG, 10, 40);
  }
  else
  {
    memset(snapshot_buffer, 0, 240 * 160 * 2);
    print_string_ext(msg[MSG_STATE_MENU_STATE_NONE], 0xFFFF, 0x0000, 15, 75, snapshot_buffer, 240, 0);
    get_timestamp_string(savestate_timestamp_string, MSG_STATE_MENU_DATE_NONE_0, 0, 0, 0, 0, 0, 0, 0, 0);
    print_string(savestate_timestamp_string, COLOR_HELP_TEXT, COLOR_BG, 10, 40);
  }
  blit_to_screen(snapshot_buffer, 240, 160, 230, 40);
}

void get_savestate_filename(u32 slot, u8 *name_buffer)
{
  u8 savestate_ext[16];

  sprintf(savestate_ext, "_%d.svs", slot);
  change_ext(gamepak_filename, name_buffer, savestate_ext);

  get_savestate_snapshot(name_buffer);
}

void get_savestate_filename_noshot(u32 slot, u8 *name_buffer)
{
  u8 savestate_ext[16];

  sprintf(savestate_ext, "_%d.svs", slot);
  change_ext(gamepak_filename, name_buffer, savestate_ext);
}

#ifdef PSP_BUILD
  void _flush_cache()
  {
//    sceKernelDcacheWritebackAll();
    invalidate_all_cache();
  }
#endif

u32 menu(u16 *original_screen)
{
  u32 clock_speed_number = (clock_speed / 33) - 1;
  u8 print_buffer[81];
  u32 _current_option = 0;
  gui_action_type gui_action;
  menu_enum _current_menu = MAIN_MENU;
  u32 i;
  u32 repeat = 1;
  u32 return_value = 0;
  u32 first_load = 0;
  u8 savestate_ext[16];
  u8 current_savestate_filename[512];
  u8 line_buffer[80];
  u8 cheat_format_str[10][41];

  menu_type *current_menu;
  menu_option_type *current_option;
  menu_option_type *display_option;
  u32 current_option_num;

  SceCtrlData ctrl_data;
  u32 buttons;

  auto void choose_menu();
  auto void clear_help();

  u8 *gamepad_help[] =
  {
    msg[MSG_PAD_MENU_CFG_HELP_0],
    msg[MSG_PAD_MENU_CFG_HELP_1],
    msg[MSG_PAD_MENU_CFG_HELP_2],
    msg[MSG_PAD_MENU_CFG_HELP_3],
    msg[MSG_PAD_MENU_CFG_HELP_4],
    msg[MSG_PAD_MENU_CFG_HELP_5],
    msg[MSG_PAD_MENU_CFG_HELP_6],
    msg[MSG_PAD_MENU_CFG_HELP_7],
    msg[MSG_PAD_MENU_CFG_HELP_8],
    msg[MSG_PAD_MENU_CFG_HELP_9],
    msg[MSG_PAD_MENU_CFG_HELP_10],
    msg[MSG_PAD_MENU_CFG_HELP_11],
    msg[MSG_PAD_MENU_CFG_HELP_12],
    msg[MSG_PAD_MENU_CFG_HELP_13],
    msg[MSG_PAD_MENU_CFG_HELP_14],
    msg[MSG_PAD_MENU_CFG_HELP_15],
    msg[MSG_PAD_MENU_CFG_HELP_16],
    msg[MSG_PAD_MENU_CFG_HELP_17],
    msg[MSG_PAD_MENU_CFG_HELP_18],
    msg[MSG_PAD_MENU_CFG_HELP_19],
    msg[MSG_PAD_MENU_CFG_HELP_20],
    msg[MSG_PAD_MENU_CFG_HELP_21]
  };

  void menu_exit()
  {
    if(!first_load)
      repeat = 0;
  }

  void menu_quit()
  {
//    clock_speed = (clock_speed_number + 1) * 33;
    save_config_file();
    quit();
  }

  void menu_load()
  {
    u8 *file_ext[] = { ".gba", ".bin", ".zip", NULL };
    u8 load_filename[512];
    save_game_config_file();

//    if(!update_backup_flag)
      update_backup_force();

    if(load_file(file_ext, load_filename, default_rom_dir) != -1)
    {
       if(load_gamepak(load_filename) == -1)
       {
         quit();
       }
       reset_gba();
       return_value = 1;
       repeat = 0;
       reg[CHANGED_PC_STATUS] = 1;
    }
    else
    {
      choose_menu(current_menu);
    }
  }

  void menu_restart()
  {
    if(!first_load)
    {
      reset_gba();
      reg[CHANGED_PC_STATUS] = 1;
      return_value = 1;
      repeat = 0;
    }
  }

  void menu_save_ss()
  {
    save_ss_bmp(original_screen);
  }

  void menu_change_state()
  {
    get_savestate_filename(savestate_slot, current_savestate_filename);
  }

  void menu_save_state()
  {
    if(!first_load)
    {
      get_savestate_filename_noshot(savestate_slot,
       current_savestate_filename);
      save_state(current_savestate_filename, original_screen);
    }
    menu_change_state();
  }

  void menu_load_state()
  {
    if(!first_load)
    {
      load_state(current_savestate_filename);
      return_value = 1;
      repeat = 0;
    }
  }

  void menu_load_state_file()
  {
    u8 *file_ext[] = { ".svs", NULL };
    u8 load_filename[512];
    if(load_file(file_ext, load_filename, default_save_dir) != -1)
    {
      load_state(load_filename);
      return_value = 1;
      repeat = 0;
    }
    else
    {
      choose_menu(current_menu);
    }
  }

  void menu_load_cheat_file()
  {
    u8 *file_ext[] = { ".cht", NULL };
    u8 load_filename[512];
    if(load_file(file_ext, load_filename, default_cheat_dir) != -1)
    {
      add_cheats(load_filename);
      return_value = 1;
      repeat = 0;
    }
    else
    {
      choose_menu(current_menu);
    }
  }

  void menu_fix_gamepad_help()
  {
    clear_help();
    current_option->help_string =
     gamepad_help[gamepad_config_map[
     gamepad_config_line_to_button[current_option_num]]];
  }

  void submenu_graphics_sound()
  {

  }

  void submenu_cheats_misc()
  {

  }

  void submenu_gamepad()
  {

  }

  void submenu_analog()
  {

  }

  void submenu_savestate()
  {
    print_string(msg[MSG_STATE_MENU_TITLE], COLOR_ACTIVE_ITEM, COLOR_BG, 10, 70);
    menu_change_state();
  }

  void submenu_main()
  {
    strncpy(print_buffer, gamepak_filename, 80);
    print_string(print_buffer, COLOR_ROM_INFO, COLOR_BG, 10, 10);
    sprintf(print_buffer, "%s  %s  %s", gamepak_title,
     gamepak_code, gamepak_maker);
    print_string(print_buffer, COLOR_ROM_INFO, COLOR_BG, 10, 20);

    get_savestate_filename_noshot(savestate_slot,
     current_savestate_filename);
  }

  u8 *yes_no_options[] = { msg[MSG_NO], msg[MSG_YES] };
  u8 *enable_disable_options[] = { msg[MSG_DISABLED], msg[MSG_ENABLED] };

  u8 *scale_options[] =
  {
    msg[MSG_SCN_UNSCALED], msg[MSG_SCN_SCALED], msg[MSG_SCN_FULL]
  };

  u8 *frameskip_options[] = { msg[MSG_FS_AUTO], msg[MSG_FS_MANUAL], msg[MSG_FS_OFF] };
  u8 *frameskip_variation_options[] = { msg[MSG_FS_UNIFORM], msg[MSG_FS_RANDOM] };

  u8 *audio_buffer_options[] =
  {
    /*msg[MSG_SB_2048],*/ msg[MSG_SB_3072], msg[MSG_SB_4096], msg[MSG_SB_5120], msg[MSG_SB_6144],
    msg[MSG_SB_7168], msg[MSG_SB_8192], msg[MSG_SB_9216], msg[MSG_SB_10240], msg[MSG_SB_11264], msg[MSG_SB_12288]
  };

  u8 *update_backup_options[] = { msg[MSG_BK_EXITONLY], msg[MSG_BK_AUTO] };

  u8 *clock_speed_options[] =
  {
    msg[MSG_CLK_33], msg[MSG_CLK_66], msg[MSG_CLK_100], msg[MSG_CLK_133], msg[MSG_CLK_166],
    msg[MSG_CLK_200], msg[MSG_CLK_233], msg[MSG_CLK_266], msg[MSG_CLK_300], msg[MSG_CLK_333]
  };

  u8 *gamepad_config_buttons[] =
  {
    msg[MSG_PAD_MENU_CFG_0],
    msg[MSG_PAD_MENU_CFG_1],
    msg[MSG_PAD_MENU_CFG_2],
    msg[MSG_PAD_MENU_CFG_3],
    msg[MSG_PAD_MENU_CFG_4],
    msg[MSG_PAD_MENU_CFG_5],
    msg[MSG_PAD_MENU_CFG_6],
    msg[MSG_PAD_MENU_CFG_7],
    msg[MSG_PAD_MENU_CFG_8],
    msg[MSG_PAD_MENU_CFG_9],
    msg[MSG_PAD_MENU_CFG_10],
    msg[MSG_PAD_MENU_CFG_11],
    msg[MSG_PAD_MENU_CFG_12],
    msg[MSG_PAD_MENU_CFG_13],
    msg[MSG_PAD_MENU_CFG_14],
    msg[MSG_PAD_MENU_CFG_15],
    msg[MSG_PAD_MENU_CFG_16],
    msg[MSG_PAD_MENU_CFG_17],
    msg[MSG_PAD_MENU_CFG_18],
    msg[MSG_PAD_MENU_CFG_19],
    msg[MSG_PAD_MENU_CFG_20],
    msg[MSG_PAD_MENU_CFG_21],
  };

  // Marker for help information, don't go past this mark (except \n)------*
  menu_option_type graphics_sound_options[] =
  {
    string_selection_option(NULL, msg[MSG_G_S_MENU_0], scale_options, (u32 *)(&screen_scale), 3, msg[MSG_G_S_MENU_HELP_0], 2),

    string_selection_option(NULL, msg[MSG_G_S_MENU_1], yes_no_options, (u32 *)(&screen_filter), 2, msg[MSG_G_S_MENU_HELP_1], 3),

    string_selection_option(NULL, msg[MSG_G_S_MENU_2], frameskip_options, (u32 *)(&current_frameskip_type), 3, msg[MSG_G_S_MENU_HELP_2], 5),

    numeric_selection_option(NULL, msg[MSG_G_S_MENU_3], &frameskip_value, 100, msg[MSG_G_S_MENU_HELP_3], 6),

    string_selection_option(NULL, msg[MSG_G_S_MENU_4], frameskip_variation_options, &random_skip, 2, msg[MSG_G_S_MENU_HELP_4], 7),

    string_selection_option(NULL, msg[MSG_G_S_MENU_5], yes_no_options, &global_enable_audio, 2, msg[MSG_G_S_MENU_HELP_5], 9),

    string_selection_option(NULL, msg[MSG_G_S_MENU_6], audio_buffer_options, &audio_buffer_size_number, 10, msg[MSG_G_S_MENU_HELP_6], 10),

    action_option(menu_save_ss, NULL, msg[MSG_G_S_MENU_7], msg[MSG_G_S_MENU_HELP_7], 12),

    submenu_option(NULL, msg[MSG_G_S_MENU_8], msg[MSG_G_S_MENU_HELP_8], 14)
  };

  make_menu(graphics_sound, submenu_graphics_sound, NULL);

  menu_option_type cheats_misc_options[] =
  {
    cheat_option(0),
    cheat_option(1),
    cheat_option(2),
    cheat_option(3),
    cheat_option(4),
    cheat_option(5),
    cheat_option(6),
    cheat_option(7),
    cheat_option(8),
    cheat_option(9),

    action_option(menu_load_cheat_file, NULL, msg[MSG_CHEAT_MENU_1], msg[MSG_CHEAT_MENU_HELP_1], 11), 

    string_selection_option(NULL, msg[MSG_CHEAT_MENU_2], clock_speed_options, &clock_speed_number, 10, msg[MSG_CHEAT_MENU_HELP_2], 13), 

    string_selection_option(NULL, msg[MSG_CHEAT_MENU_3], update_backup_options, &update_backup_flag, 2, msg[MSG_CHEAT_MENU_HELP_3], 14), 

    submenu_option(NULL, msg[MSG_CHEAT_MENU_4], msg[MSG_CHEAT_MENU_HELP_4], 16) 
  };

  make_menu(cheats_misc, submenu_cheats_misc, NULL);

  menu_option_type savestate_options[] =
  {
    numeric_selection_action_hide_option(menu_load_state, menu_change_state, msg[MSG_STATE_MENU_0], &savestate_slot, 10, msg[MSG_STATE_MENU_HELP_0], 6),

    numeric_selection_action_hide_option(menu_save_state, menu_change_state, msg[MSG_STATE_MENU_1], &savestate_slot, 10, msg[MSG_STATE_MENU_HELP_1], 7),

    numeric_selection_action_hide_option(menu_load_state_file, menu_change_state, msg[MSG_STATE_MENU_2], &savestate_slot, 10, msg[MSG_STATE_MENU_HELP_2], 9),

    numeric_selection_option(menu_change_state, msg[MSG_STATE_MENU_3], &savestate_slot, 10, msg[MSG_STATE_MENU_HELP_3], 11),

    submenu_option(NULL, msg[MSG_STATE_MENU_4], msg[MSG_STATE_MENU_HELP_4], 13)
  };

  make_menu(savestate, submenu_savestate, NULL);

  menu_option_type gamepad_config_options[] =
  {
    gamepad_config_option(msg[MSG_PAD_MENU_0], 0),
    gamepad_config_option(msg[MSG_PAD_MENU_1], 1),
    gamepad_config_option(msg[MSG_PAD_MENU_2], 2),
    gamepad_config_option(msg[MSG_PAD_MENU_3], 3),
    gamepad_config_option(msg[MSG_PAD_MENU_4], 4),
    gamepad_config_option(msg[MSG_PAD_MENU_5], 5),
    gamepad_config_option(msg[MSG_PAD_MENU_6], 6),
    gamepad_config_option(msg[MSG_PAD_MENU_7], 7),
    gamepad_config_option(msg[MSG_PAD_MENU_8], 8),
    gamepad_config_option(msg[MSG_PAD_MENU_9], 9),
    gamepad_config_option(msg[MSG_PAD_MENU_10], 10),
    gamepad_config_option(msg[MSG_PAD_MENU_11], 11),
    submenu_option(NULL, msg[MSG_PAD_MENU_12], msg[MSG_PAD_MENU_HELP_0], 13)
  };

  menu_option_type analog_config_options[] =
  {
    analog_config_option(msg[MSG_A_PAD_MENU_0], 0),
    analog_config_option(msg[MSG_A_PAD_MENU_1], 1),
    analog_config_option(msg[MSG_A_PAD_MENU_2], 2),
    analog_config_option(msg[MSG_A_PAD_MENU_3], 3),
    string_selection_option(NULL, msg[MSG_A_PAD_MENU_4], yes_no_options, &global_enable_analog, 2, msg[MSG_A_PAD_MENU_HELP_0], 7),
    numeric_selection_option(NULL, msg[MSG_A_PAD_MENU_5], &analog_sensitivity_level, 10, msg[MSG_A_PAD_MENU_HELP_1], 8),
    submenu_option(NULL, msg[MSG_A_PAD_MENU_6], msg[MSG_A_PAD_MENU_HELP_2], 11)
  };

  make_menu(gamepad_config, submenu_gamepad, NULL);
  make_menu(analog_config, submenu_analog, NULL);

  menu_option_type main_options[] =
  {
    submenu_option(&graphics_sound_menu, msg[MSG_MAIN_MENU_0], msg[MSG_MAIN_MENU_HELP_0], 0), 

    numeric_selection_action_option(menu_load_state, NULL, msg[MSG_MAIN_MENU_1], &savestate_slot, 10, msg[MSG_MAIN_MENU_HELP_1], 2),

    numeric_selection_action_option(menu_save_state, NULL, msg[MSG_MAIN_MENU_2], &savestate_slot, 10, msg[MSG_MAIN_MENU_HELP_2], 3),

    submenu_option(&savestate_menu, msg[MSG_MAIN_MENU_3], msg[MSG_MAIN_MENU_HELP_3], 4),

    submenu_option(&gamepad_config_menu, msg[MSG_MAIN_MENU_4], msg[MSG_MAIN_MENU_HELP_4], 6),

    submenu_option(&analog_config_menu, msg[MSG_MAIN_MENU_5], msg[MSG_MAIN_MENU_HELP_5], 7),

    submenu_option(&cheats_misc_menu, msg[MSG_MAIN_MENU_6], msg[MSG_MAIN_MENU_HELP_6], 9),

    action_option(menu_load, NULL, msg[MSG_MAIN_MENU_7], msg[MSG_MAIN_MENU_HELP_7], 11),

    action_option(menu_restart, NULL, msg[MSG_MAIN_MENU_8], msg[MSG_MAIN_MENU_HELP_8], 12), 

    action_option(menu_exit, NULL, msg[MSG_MAIN_MENU_9], msg[MSG_MAIN_MENU_HELP_9], 13), 

    action_option(menu_quit, NULL, msg[MSG_MAIN_MENU_10], msg[MSG_MAIN_MENU_HELP_10], 15) 
  };

  make_menu(main, submenu_main, NULL);

  void choose_menu(menu_type *new_menu)
  {
    if(new_menu == NULL)
      new_menu = &main_menu;

    clear_screen(COLOR_BG);
    blit_to_screen(original_screen, 240, 160, 230, 40);

    current_menu = new_menu;
    current_option = new_menu->options;
    current_option_num = 0;
    if(current_menu->init_function)
     current_menu->init_function();
  }

  void clear_help()
  {
    for(i = 0; i < 6; i++)
    {
      print_string_pad(" ", COLOR_BG, COLOR_BG, 30, 210 + (i * 10), 90);
    }
  }

  video_resolution_large();

  SDL_LockMutex(sound_mutex);
  SDL_PauseAudio(1);
  SDL_UnlockMutex(sound_mutex);

  if(gamepak_filename[0] == 0)
  {
    first_load = 1;
    memset(original_screen, 0x00, 240 * 160 * 2);
    print_string_ext(msg[MSG_NON_LOAD_GAME], 0xFFFF, 0x0000, 60, 75,original_screen, 240, 0);
  }

  choose_menu(&main_menu);

  for(i = 0; i < 10; i++)
  {
    if(i >= num_cheats)
    {
      sprintf(cheat_format_str[i], msg[MSG_CHEAT_MENU_NON_LOAD], i);
    }
    else
    {
      sprintf(cheat_format_str[i], msg[MSG_CHEAT_MENU_0], i, cheats[i].cheat_name);
    }
  }

  current_menu->init_function();

  while(repeat)
  {

    print_status();

    display_option = current_menu->options;

    for(i = 0; i < current_menu->num_options; i++, display_option++)
    {
      if(display_option->option_type & NUMBER_SELECTION_OPTION)
      {
        sprintf(line_buffer, display_option->display_string,
         *(display_option->current_option));
      }
      else

      if(display_option->option_type & STRING_SELECTION_OPTION)
      {
        sprintf(line_buffer, display_option->display_string,
         ((u32 *)display_option->options)[*(display_option->current_option)]);
      }
      else
      {
        strcpy(line_buffer, display_option->display_string);
      }

      if(display_option == current_option)
      {
        print_string_pad(line_buffer, COLOR_ACTIVE_ITEM, COLOR_BG, 10,
         (display_option->line_number * 10) + 40, 36);
      }
      else
      {
        print_string_pad(line_buffer, COLOR_INACTIVE_ITEM, COLOR_BG, 10,
         (display_option->line_number * 10) + 40, 36);
      }
    }

    print_string(current_option->help_string, COLOR_HELP_TEXT,
     COLOR_BG, 30, 210);

    flip_screen();

    gui_action = get_gui_input();

    switch(gui_action)
    {
      case CURSOR_DOWN:
        current_option_num = (current_option_num + 1) %
          current_menu->num_options;

        current_option = current_menu->options + current_option_num;
        clear_help();
        break;

      case CURSOR_UP:
        if(current_option_num)
          current_option_num--;
        else
          current_option_num = current_menu->num_options - 1;

        current_option = current_menu->options + current_option_num;
        clear_help();
        break;

      case CURSOR_RIGHT:
        if(current_option->option_type & (NUMBER_SELECTION_OPTION |
         STRING_SELECTION_OPTION))
        {
          *(current_option->current_option) =
           (*current_option->current_option + 1) %
           current_option->num_options;

          if(current_option->passive_function)
            current_option->passive_function();
        }
        break;

      case CURSOR_LEFT:
        if(current_option->option_type & (NUMBER_SELECTION_OPTION |
         STRING_SELECTION_OPTION))
        {
          u32 current_option_val = *(current_option->current_option);

          if(current_option_val)
            current_option_val--;
          else
            current_option_val = current_option->num_options - 1;

          *(current_option->current_option) = current_option_val;

          if(current_option->passive_function)
            current_option->passive_function();
        }
        break;

      case CURSOR_EXIT:
        if(current_menu == &main_menu)
          menu_exit();

        choose_menu(&main_menu);
        break;

      case CURSOR_SELECT:
        if(current_option->option_type & ACTION_OPTION)
          current_option->action_function();

        if(current_option->option_type & SUBMENU_OPTION)
          choose_menu(current_option->sub_menu);
        break;
    }
  }

// menu終了時の処理

  while(sceCtrlPeekBufferPositive(&ctrl_data, 1), ctrl_data.Buttons != 0);

  set_gba_resolution(screen_scale);
  video_resolution_small();

  clock_speed = (clock_speed_number + 1) * 33;

  #ifdef PSP_BUILD
    scePowerSetClockFrequency(clock_speed, clock_speed, clock_speed / 2);
  #endif

  SDL_PauseAudio(0);

  return return_value;
}

u32 load_dircfg(char *file_name)
{
  int loop;
  int next_line;
  u8 current_line[256];
  u8 current_str[256];
  FILE *msg_file;
  char msg_path[512];

  #ifdef PSP_BUILD
    sprintf(msg_path, "%s/%s", main_path, file_name);
  #else
    sprintf(msg_path, "%s\\%s", main_path, file_name);
  #endif

  msg_file = fopen(msg_path, "r");

  next_line = 0;
  if(msg_file)
  {
    loop = 0;
    while(fgets(current_line, 256, msg_file))
    {
      if(parse_line(current_line, current_str) != -1)
      {
        switch(loop)
        {
          case 0:
            if(opendir(current_str) != NULL)
              strcpy(default_rom_dir,current_str);
            else
            {
              *default_rom_dir = (char)NULL;
              printf("not open rom dir : %s\n",current_str);
              delay_us(500000);
            }
            loop++;
            break;

          case 1:
            if(opendir(current_str) != NULL)
              strcpy(default_save_dir,current_str);
            else
            {
              *default_save_dir = (char)NULL;
              printf("not open save dir : %s\n",current_str);
              delay_us(500000);
            }
            loop++;
            break;

          case 2:
            if(opendir(current_str) != NULL)
              strcpy(default_cfg_dir,current_str);
            else
            {
              *default_cfg_dir = (char)NULL;
              printf("not open cfg dir : %s\n",current_str);
              delay_us(500000);
            }
            loop++;
            break;

          case 3:
            if(opendir(current_str) != NULL)
              strcpy(default_ss_dir,current_str);
            else
            {
              *default_ss_dir = (char)NULL;
              printf("not open screen shot dir : %s\n",current_str);
              delay_us(500000);
            }
            loop++;
            break;

          case 4:
            if(opendir(current_str) != NULL)
              strcpy(default_cheat_dir,current_str);
            else
            {
              *default_cheat_dir = (char)NULL;
              printf("not open cheat dir : %s\n",current_str);
              delay_us(500000);
            }
            loop++;
            break;
        }
      }
    }
    
    fclose(msg_file);
    if (loop == 5)
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
  fclose(msg_file);
  return -1;
}

u32 load_fontcfg(char *file_name)
{
  int loop;
  int next_line;
  u8 current_line[256];
  u8 current_str[256];
  FILE *msg_file;
  char msg_path[512];

  #ifdef PSP_BUILD
    sprintf(msg_path, "%s/%s", main_path, file_name);
  #else
    sprintf(msg_path, "%s\\%s", main_path, file_name);
  #endif

  msg_file = fopen(msg_path, "r");

  next_line = 0;
  if(msg_file)
  {
    loop = 0;
    while(fgets(current_line, 256, msg_file))
    {
      if(parse_line(current_line, current_str) != -1)
      {
        switch(loop)
        {
          case 0:
            strcpy(font8,current_str);
            loop++;
            break;
          case 1:
            strcpy(font16,current_str);
            loop++;
            break;
        }
      }
    }
    
    fclose(msg_file);
    if (loop == 2)
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
  fclose(msg_file);
  return -1;
}

u32 load_msgcfg(char *file_name)
{
  int loop;
  int next_line;
  u8 current_line[256];
  u8 current_str[256];
  FILE *msg_file;
  char msg_path[512];

  #ifdef PSP_BUILD
    sprintf(msg_path, "%s/%s", main_path, file_name);
  #else
    sprintf(msg_path, "%s\\%s", main_path, file_name);
  #endif

  msg_file = fopen(msg_path, "r");

  next_line = 0;
  if(msg_file)
  {
    loop = 0;
    while(fgets(current_line, 256, msg_file))
    {
      if(parse_line(current_line, current_str) != -1)
      {
        if (loop <= (MSG_END + 1 + next_line)) {
          if (next_line == 1) {
            strcat(msg[loop - 1], current_str);
          }
          else
          {
            strcpy(msg[loop], current_str);
            loop++;
            next_line = 1;
          }
        }
      }
      else
      {
        next_line = 0;
      }
    }
    
    fclose(msg_file);
    if (loop == (MSG_END))
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
  fclose(msg_file);
  return -1;
}

u32 parse_line(u8 *current_line, u8 *current_str)
{
  u8 *line_ptr;
  u8 *line_ptr_new;

  line_ptr = current_line;
  /* NULL or comment or other */
  if((current_line[0] == 0) || (current_line[0] == '#') || (current_line[0] != '!'))
    return -1;

  line_ptr++;

  line_ptr_new = strchr(line_ptr, '\r');
  while (line_ptr_new != NULL)
  {
    *line_ptr_new = '\n';
    line_ptr_new = strchr(line_ptr, '\r');
  }

  line_ptr_new = strchr(line_ptr, '\n');
  if (line_ptr_new == NULL)
    return -1;

  *line_ptr_new = 0;

  // "\n" to '\n'
  line_ptr_new = strstr(line_ptr, "\\n");
  while (line_ptr_new != NULL)
  {
    *line_ptr_new = '\n';
    memmove((line_ptr_new + 1), (line_ptr_new + 2), (strlen(line_ptr_new + 2) + 1));
    line_ptr_new = strstr(line_ptr_new, "\\n");
  }

  strcpy(current_str, line_ptr);
  return 0;
}

u32 load_font()
{
    return fbm_init(font8,font16,1);
}

void print_status()
{
  char print_buffer_1[256];
  char print_buffer_2[256];
  pspTime current_time;

  sceRtcGetCurrentClockLocalTime(&current_time);
  int wday = sceRtcGetDayOfWeek(current_time.year, current_time.month , current_time.day);

  get_timestamp_string(print_buffer_1, MSG_MENU_DATE_FMT_0, current_time.year, current_time.month , current_time.day, wday,
    current_time.hour, current_time.minutes, current_time.seconds, 0);
  sprintf(print_buffer_2,"%s%s", msg[MSG_MENU_DATE], print_buffer_1);
  print_string(print_buffer_2, COLOR_HELP_TEXT, COLOR_BG, 0, 0);

  sprintf(print_buffer_1, msg[MSG_MENU_BATTERY], scePowerGetBatteryLifePercent(), scePowerGetBatteryLifeTime());
  print_string(print_buffer_1, COLOR_HELP_TEXT, COLOR_BG, 240, 0);

  sprintf(print_buffer_1, "TOTAL FREE:%04dkb  MAX FREE:%04dkb", sceKernelTotalFreeMemSize()/1024, sceKernelMaxFreeMemSize()/1024);
  print_string(print_buffer_1, COLOR_HELP_TEXT, COLOR_BG, 240, 10);

  sprintf(print_buffer_1, "ROM BUF:%02dmb", gamepak_ram_buffer_size/1024/1024);
  print_string(print_buffer_1, COLOR_HELP_TEXT, COLOR_BG, 240, 20);
}

void get_timestamp_string(char *buffer, u16 msg_id, u16 year, u16 mon, u16 day, u16 wday, u16 hour, u16 min, u16 sec, u32 msec)
{
  int id;
  u8 *weekday_strings[] =
  {
    msg[MSG_WDAY_0], msg[MSG_WDAY_1], msg[MSG_WDAY_2], msg[MSG_WDAY_3],
    msg[MSG_WDAY_4], msg[MSG_WDAY_5], msg[MSG_WDAY_6], ""
  };

  sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_DATE_FORMAT,&id);
  switch(id)
  {
    case PSP_SYSTEMPARAM_DATE_FORMAT_YYYYMMDD:
      sprintf(buffer, msg[msg_id    ], year, mon, day, weekday_strings[wday], hour, min, sec, msec / 1000);
      break;
    case PSP_SYSTEMPARAM_DATE_FORMAT_MMDDYYYY:
      sprintf(buffer, msg[msg_id + 1], weekday_strings[wday], mon, day, year, hour, min, sec, msec / 1000);
      break;
    case PSP_SYSTEMPARAM_DATE_FORMAT_DDMMYYYY:
      sprintf(buffer, msg[msg_id + 2], weekday_strings[wday], day, mon, year, hour, min, sec, msec / 1000);
      break;
  }
}

void save_ss_bmp(u16 *image)
{
  static unsigned char header[] ={ 'B',  'M',  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
                                   0x00, 0x00,  240, 0x00, 0x00, 0x00,  160, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  u8 ss_filename[512];
  u8 timestamp[512];
  u8 save_ss_path[1024];
  pspTime current_time;
  u8 rgb_data[160][240][3];
  u8 x,y;
  u16 col;
  u8 r,g,b;

  sceRtcGetCurrentClockLocalTime(&current_time);

  change_ext(gamepak_filename, ss_filename, "_");

  get_timestamp_string(timestamp, MSG_SS_FMT_0, current_time.year, current_time.month , current_time.day, 7,
    current_time.hour, current_time.minutes, current_time.seconds, current_time.microseconds);

  if (default_ss_dir != NULL) {
    sprintf(save_ss_path, "%s/%s%s.bmp", default_ss_dir, ss_filename, timestamp);
  }
  else
  {
    sprintf(save_ss_path, "%s_%s.bmp", ss_filename, timestamp);
  }

  for(y = 0; y < 160; y++)
  {
    for(x = 0; x < 240; x++)
    {
      col = image[x + y * 240];
      r = (col >> 11);
      g = (col >> 5) & 0x3F;
      b = (col) & 0x1F;

      rgb_data[159-y][x][2] = b * 255 / 31;
      rgb_data[159-y][x][1] = g * 255 / 63;
      rgb_data[159-y][x][0] = r * 255 / 31;
    }
  }

    FILE *ss = fopen( save_ss_path, "wb" );
    if( ss == NULL ) return;

    fwrite( header, sizeof(header), 1, ss );
    fwrite( rgb_data, 240*160*3, 1, ss);
    fclose( ss );

}
