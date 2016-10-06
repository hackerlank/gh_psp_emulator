#include "common.h"
#include "bios.h"
#include <fastmath.h>

s16 b_sinetable[256] = {
  (s16)0x0000, (s16)0x0192, (s16)0x0323, (s16)0x04B5, (s16)0x0645, (s16)0x07D5, (s16)0x0964, (s16)0x0AF1,
  (s16)0x0C7C, (s16)0x0E05, (s16)0x0F8C, (s16)0x1111, (s16)0x1294, (s16)0x1413, (s16)0x158F, (s16)0x1708,
  (s16)0x187D, (s16)0x19EF, (s16)0x1B5D, (s16)0x1CC6, (s16)0x1E2B, (s16)0x1F8B, (s16)0x20E7, (s16)0x223D,
  (s16)0x238E, (s16)0x24DA, (s16)0x261F, (s16)0x275F, (s16)0x2899, (s16)0x29CD, (s16)0x2AFA, (s16)0x2C21,
  (s16)0x2D41, (s16)0x2E5A, (s16)0x2F6B, (s16)0x3076, (s16)0x3179, (s16)0x3274, (s16)0x3367, (s16)0x3453,
  (s16)0x3536, (s16)0x3612, (s16)0x36E5, (s16)0x37AF, (s16)0x3871, (s16)0x392A, (s16)0x39DA, (s16)0x3A82,
  (s16)0x3B20, (s16)0x3BB6, (s16)0x3C42, (s16)0x3CC5, (s16)0x3D3E, (s16)0x3DAE, (s16)0x3E14, (s16)0x3E71,
  (s16)0x3EC5, (s16)0x3F0E, (s16)0x3F4E, (s16)0x3F84, (s16)0x3FB1, (s16)0x3FD3, (s16)0x3FEC, (s16)0x3FFB,
  (s16)0x4000, (s16)0x3FFB, (s16)0x3FEC, (s16)0x3FD3, (s16)0x3FB1, (s16)0x3F84, (s16)0x3F4E, (s16)0x3F0E,
  (s16)0x3EC5, (s16)0x3E71, (s16)0x3E14, (s16)0x3DAE, (s16)0x3D3E, (s16)0x3CC5, (s16)0x3C42, (s16)0x3BB6,
  (s16)0x3B20, (s16)0x3A82, (s16)0x39DA, (s16)0x392A, (s16)0x3871, (s16)0x37AF, (s16)0x36E5, (s16)0x3612,
  (s16)0x3536, (s16)0x3453, (s16)0x3367, (s16)0x3274, (s16)0x3179, (s16)0x3076, (s16)0x2F6B, (s16)0x2E5A,
  (s16)0x2D41, (s16)0x2C21, (s16)0x2AFA, (s16)0x29CD, (s16)0x2899, (s16)0x275F, (s16)0x261F, (s16)0x24DA,
  (s16)0x238E, (s16)0x223D, (s16)0x20E7, (s16)0x1F8B, (s16)0x1E2B, (s16)0x1CC6, (s16)0x1B5D, (s16)0x19EF,
  (s16)0x187D, (s16)0x1708, (s16)0x158F, (s16)0x1413, (s16)0x1294, (s16)0x1111, (s16)0x0F8C, (s16)0x0E05,
  (s16)0x0C7C, (s16)0x0AF1, (s16)0x0964, (s16)0x07D5, (s16)0x0645, (s16)0x04B5, (s16)0x0323, (s16)0x0192,
  (s16)0x0000, (s16)0xFE6E, (s16)0xFCDD, (s16)0xFB4B, (s16)0xF9BB, (s16)0xF82B, (s16)0xF69C, (s16)0xF50F,
  (s16)0xF384, (s16)0xF1FB, (s16)0xF074, (s16)0xEEEF, (s16)0xED6C, (s16)0xEBED, (s16)0xEA71, (s16)0xE8F8,
  (s16)0xE783, (s16)0xE611, (s16)0xE4A3, (s16)0xE33A, (s16)0xE1D5, (s16)0xE075, (s16)0xDF19, (s16)0xDDC3,
  (s16)0xDC72, (s16)0xDB26, (s16)0xD9E1, (s16)0xD8A1, (s16)0xD767, (s16)0xD633, (s16)0xD506, (s16)0xD3DF,
  (s16)0xD2BF, (s16)0xD1A6, (s16)0xD095, (s16)0xCF8A, (s16)0xCE87, (s16)0xCD8C, (s16)0xCC99, (s16)0xCBAD,
  (s16)0xCACA, (s16)0xC9EE, (s16)0xC91B, (s16)0xC851, (s16)0xC78F, (s16)0xC6D6, (s16)0xC626, (s16)0xC57E,
  (s16)0xC4E0, (s16)0xC44A, (s16)0xC3BE, (s16)0xC33B, (s16)0xC2C2, (s16)0xC252, (s16)0xC1EC, (s16)0xC18F,
  (s16)0xC13B, (s16)0xC0F2, (s16)0xC0B2, (s16)0xC07C, (s16)0xC04F, (s16)0xC02D, (s16)0xC014, (s16)0xC005,
  (s16)0xC000, (s16)0xC005, (s16)0xC014, (s16)0xC02D, (s16)0xC04F, (s16)0xC07C, (s16)0xC0B2, (s16)0xC0F2,
  (s16)0xC13B, (s16)0xC18F, (s16)0xC1EC, (s16)0xC252, (s16)0xC2C2, (s16)0xC33B, (s16)0xC3BE, (s16)0xC44A,
  (s16)0xC4E0, (s16)0xC57E, (s16)0xC626, (s16)0xC6D6, (s16)0xC78F, (s16)0xC851, (s16)0xC91B, (s16)0xC9EE,
  (s16)0xCACA, (s16)0xCBAD, (s16)0xCC99, (s16)0xCD8C, (s16)0xCE87, (s16)0xCF8A, (s16)0xD095, (s16)0xD1A6,
  (s16)0xD2BF, (s16)0xD3DF, (s16)0xD506, (s16)0xD633, (s16)0xD767, (s16)0xD8A1, (s16)0xD9E1, (s16)0xDB26,
  (s16)0xDC72, (s16)0xDDC3, (s16)0xDF19, (s16)0xE075, (s16)0xE1D5, (s16)0xE33A, (s16)0xE4A3, (s16)0xE611,
  (s16)0xE783, (s16)0xE8F8, (s16)0xEA71, (s16)0xEBED, (s16)0xED6C, (s16)0xEEEF, (s16)0xF074, (s16)0xF1FB,
  (s16)0xF384, (s16)0xF50F, (s16)0xF69C, (s16)0xF82B, (s16)0xF9BB, (s16)0xFB4B, (s16)0xFCDD, (s16)0xFE6E
};

void bios_halt()
{
}

int bios_sqrt(int b_value)
{
    return sqrtf(b_value);
}

void bios_cpuset(u32 b_source, u32 b_dest, u32 b_cnt) /* arm_r0, arm r1, arm_r2 */
{
  save_reg();
  if(((b_source & 0xe000000) == 0) ||
     ((b_source + (((b_cnt << 11)>>9) & 0x1FFFFF)) & 0xe000000) == 0)
    {
      load_reg();
      return;
    }
  
  u32 b_count = b_cnt & 0x1FFFFF;

  // 32-bit ?
  if((b_cnt >> 26) & 1) {
    // needed for 32-bit mode!
    b_source &= 0x0FFFFFFC;
    b_dest &= 0x0FFFFFFC;
    // fill ?
    if((b_cnt >> 24) & 1) {
      u32 b_value = read_memory32(b_source);
      while(b_count) {
        write_memory32(b_dest, b_value);
        b_dest += 4;
        b_count--;
      }
    } else {
      // copy
      while(b_count) {
        u32 b_value = read_memory32(b_source);
        write_memory32(b_dest, b_value);
        b_source += 4;
        b_dest += 4;
        b_count--;
      }
    }
  } else {
    // 16-bit fill?
    if((b_cnt >> 24) & 1) {
      u16 b_value = read_memory16(b_source);
      while(b_count) {
        write_memory16(b_dest, b_value);
        b_dest += 2;
        b_count--;
      }
    } else {
      // copy
      while(b_count) {
        u16 b_value = read_memory16(b_source);
        write_memory16(b_dest, b_value);
        b_source += 2;
        b_dest += 2;
        b_count--;
      }
    }
  }
  load_reg();
}

void bios_cpufastset(u32 b_source, u32 b_dest, u32 b_cnt)
{
  save_reg();
  if(((b_source & 0xe000000) == 0) ||
     ((b_source + (((b_cnt << 11)>>9) & 0x1FFFFF)) & 0xe000000) == 0)
    {
      load_reg();
      return;
    }
  // needed for 32-bit mode!
  b_source &= 0x0FFFFFFC;
  b_dest &= 0x0FFFFFFC;
  
  u32 b_count = b_cnt & 0x1FFFFF;
  u32 b_i;

  // fill?
  if((b_cnt >> 24) & 1) {
    while(b_count > 0) {
      // BIOS always transfers 32 bytes at a time
      u32 b_value = read_memory32(b_source);
      for(b_i = 0; b_i < 8; b_i++) {
        write_memory32(b_dest, b_value);
        b_dest += 4;
      }
      b_count -= 8;
    }
  } else {
    // copy
    while(b_count > 0) {
      // BIOS always transfers 32 bytes at a time
      for(b_i = 0; b_i < 8; b_i++) {
        u32 b_value = read_memory32(b_source);
        write_memory32(b_dest, b_value);
        b_source += 4;
        b_dest += 4;
      }
      b_count -= 8;
    }
  }
  load_reg();
}

void bios_bgaffineset(u32 b_src, u32 b_dest, u32 b_num)
{
  save_reg();
  u32 b_i;
  for(b_i = 0; b_i < b_num; b_i++) {
    s32 b_cx = read_memory32(b_src);
    b_src+=4;
    s32 b_cy = read_memory32(b_src);
    b_src+=4;
    s16 b_dispx = read_memory16(b_src);
    b_src+=2;
    s16 b_dispy = read_memory16(b_src);
    b_src+=2;
    s16 b_rx = read_memory16(b_src);
    b_src+=2;
    s16 b_ry = read_memory16(b_src);
    b_src+=2;
    u16 b_theta = read_memory16(b_src)>>8;
    b_src+=4; // keep structure alignment
    s32 b_a = b_sinetable[(b_theta+0x40)&255];
    s32 b_b = b_sinetable[b_theta];

    s16 b_dx =  (b_rx * b_a)>>14;
    s16 b_dmx = (b_rx * b_b)>>14;
    s16 b_dy =  (b_ry * b_b)>>14;
    s16 b_dmy = (b_ry * b_a)>>14;
    
    write_memory16(b_dest, b_dx);
    b_dest += 2;
    write_memory16(b_dest, -b_dmx);
    b_dest += 2;
    write_memory16(b_dest, b_dy);
    b_dest += 2;
    write_memory16(b_dest, b_dmy);
    b_dest += 2;

    s32 b_startx = b_cx - b_dx * b_dispx + b_dmx * b_dispy;
    s32 b_starty = b_cy - b_dy * b_dispx - b_dmy * b_dispy;
    
    write_memory32(b_dest, b_startx);
    b_dest += 4;
    write_memory32(b_dest, b_starty);
    b_dest += 4;
  }
  load_reg();
}  

void bios_objaffineset(u32 b_src, u32 b_dest, u32 b_num, u32 b_offset)
{
  save_reg();
  u32 b_i;
  for(b_i = 0; b_i < b_num; b_i++) {
    s16 b_rx = read_memory16(b_src);
    b_src+=2;
    s16 b_ry = read_memory16(b_src);
    b_src+=2;
    u16 b_theta = read_memory16(b_src)>>8;
    b_src+=4; // keep structure alignment

    s32 b_a = (s32)b_sinetable[(b_theta+0x40)&255];
    s32 b_b = (s32)b_sinetable[b_theta];

    s16 b_dx =  ((s32)b_rx * b_a)>>14;
    s16 b_dmx = ((s32)b_rx * b_b)>>14;
    s16 b_dy =  ((s32)b_ry * b_b)>>14;
    s16 b_dmy = ((s32)b_ry * b_a)>>14;
    
    write_memory16(b_dest, b_dx);
    b_dest += b_offset;
    write_memory16(b_dest, -b_dmx);
    b_dest += b_offset;
    write_memory16(b_dest, b_dy);
    b_dest += b_offset;
    write_memory16(b_dest, b_dmy);
    b_dest += b_offset;
  }
  load_reg();
}
