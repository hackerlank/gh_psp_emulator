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


#define save_reg()            \
  asm( "\n                    \
  sw $3, 0(%0)\n              \
  sw $7, 4(%0)\n              \
  sw $8, 8(%0)\n              \
  sw $9, 12(%0)\n             \
  sw $10, 16(%0)\n            \
  sw $11, 20(%0)\n            \
  sw $12, 24(%0)\n            \
  sw $13, 28(%0)\n            \
  sw $14, 32(%0)\n            \
  sw $15, 36(%0)\n            \
  sw $24, 44(%0)\n            \
  sw $25, 48(%0)\n            \
  \n                          \
  sw $18, 40(%0)\n            \
  sw $28, 52(%0)\n            \
  sw $30, 64(%0)\n            \
  "::"r"(reg));               \

#define load_reg()            \
  asm( "\n                    \
  lw $3, 0(%0)\n              \
  lw $7, 4(%0)\n              \
  lw $8, 8(%0)\n              \
  lw $9, 12(%0)\n             \
  lw $10, 16(%0)\n            \
  lw $11, 20(%0)\n            \
  lw $12, 24(%0)\n            \
  lw $13, 28(%0)\n            \
  lw $14, 32(%0)\n            \
  lw $15, 36(%0)\n            \
  lw $24, 44(%0)\n            \
  lw $25, 48(%0)\n            \
  \n                          \
  lw $18, 40(%0)\n            \
  lw $28, 52(%0)\n            \
  lw $30, 64(%0)\n            \
  "::"r"(reg));               \

void bios_halt();
int bios_sqrt(int value);
void bios_cpuset(u32 source, u32 dest, u32 cnt);
void bios_cpufastset(u32 source, u32 dest, u32 cnt);
void bios_bgaffineset(u32 source, u32 dest, u32 num);
void bios_objaffineset(u32 source, u32 dest, u32 num, u32 offset);
