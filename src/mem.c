// run68x - Human68k CUI Emulator based on run68
// Copyright (C) 2023 TcbnErik
//
// This program is free software; you can redistribute it and /or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110 - 1301 USA.

#include "mem.h"

#include <stdbool.h>
#include <stdio.h>

#include "run68.h"

/*
 　機能：PCの指すメモリからインデックスレジスタ＋8ビットディスプレースメント
 　　　　の値を得る
 戻り値：その値
*/
Long idx_get(void) {
  char *mem = prog_ptr + pc;

  // Brief Extension Word Format
  //   D/A | REG | REG | REG | W/L | SCALE | SCALE | 0
  //   M68000ではSCALEは無効、最下位ビットが1でもBrief Formatとして解釈される。
  UByte ext = *mem++;
  Byte disp8 = *mem;
  pc += 2;

  int idx_reg = ((ext >> 4) & 0x07);
  Long idx = (ext & 0x80) ? ra[idx_reg] : rd[idx_reg];
  if ((ext & 0x08) == 0) idx = extl((Word)idx);  // Sign-Extended Word

  return idx + extbl(disp8);
}

/*
 　機能：PCの指すメモリから指定されたサイズのイミディエイトデータをゲットし、
 　　　　サイズに応じてPCを進める
 戻り値：データの値
*/
Long imi_get(char size) {
  Long d;
  UByte *mem = (UByte *)prog_ptr + pc;

  switch (size) {
    case S_BYTE:
      pc += 2;
      return (*(mem + 1));
    case S_WORD:
      pc += 2;
      d = *(mem++);
      d = ((d << 8) | *mem);
      return (d);
    default: /* S_LONG */
      pc += 4;
      d = *(mem++);
      d = ((d << 8) | *(mem++));
      d = ((d << 8) | *(mem++));
      d = ((d << 8) | *mem);
      return (d);
  }
}

/*
 　機能：読み込みアドレスのチェック
 戻り値： true = OK
         false = NGだが、0を読み込んだとみなす
*/
bool mem_red_chk(Long adr) {
  char message[256];

  adr &= 0x00FFFFFF;
  if (adr >= 0xC00000) {
    if (ini_info.io_through) return false;
    sprintf(message, "I/OポートorROM($%06X)から読み込もうとしました。", adr);
    err68(message);
  }
  if (SR_S_REF() == 0 || (ULong)adr >= mem_aloc) {
    sprintf(message, "不正アドレス($%06X)からの読み込みです。", adr);
    err68(message);
  }
  return true;
}

/*
 　機能：書き込みアドレスのチェック
 戻り値： true = OK
         false = NGだが、何も書き込まずにOKとみなす
*/
bool mem_wrt_chk(Long adr) {
  char message[256];

  adr &= 0x00FFFFFF;
  if (adr >= 0xC00000) {
    if (ini_info.io_through) return false;
    sprintf(message, "I/OポートorROM($%06X)に書き込もうとしました。", adr);
    err68(message);
  }
  if (SR_S_REF() == 0 || (ULong)adr >= mem_aloc) {
    sprintf(message, "不正アドレスへの書き込みです($%06X)", adr);
    err68(message);
  }
  return true;
}

/*
 　機能：メモリから指定されたサイズのデータをゲットする
 戻り値：データの値
*/
Long mem_get(Long adr, char size) {
  Long d;

  if (adr < ENV_TOP || (ULong)adr >= mem_aloc) {
    if (!mem_red_chk(adr)) return (0);
  }
  UByte *mem = (UByte *)prog_ptr + adr;

  switch (size) {
    case S_BYTE:
      return (*mem);
    case S_WORD:
      d = *(mem++);
      d = ((d << 8) | *mem);
      return (d);
    default: /* S_LONG */
      d = *(mem++);
      d = ((d << 8) | *(mem++));
      d = ((d << 8) | *(mem++));
      d = ((d << 8) | *mem);
      return (d);
  }
}

/*
 　機能：メモリに指定されたサイズのデータをセットする
 戻り値：なし
*/
void mem_set(Long adr, Long d, char size) {
  if (adr < ENV_TOP || (ULong)adr >= mem_aloc) {
    if (!mem_wrt_chk(adr)) return;
  }

  UByte *mem = (UByte *)prog_ptr + adr;

  switch (size) {
    case S_BYTE:
      *mem = (d & 0xFF);
      return;
    case S_WORD:
      *(mem++) = ((d >> 8) & 0xFF);
      *mem = (d & 0xFF);
      return;
    default: /* S_LONG */
      *(mem++) = ((d >> 24) & 0xFF);
      *(mem++) = ((d >> 16) & 0xFF);
      *(mem++) = ((d >> 8) & 0xFF);
      *mem = (d & 0xFF);
      return;
  }
}

/*
 機能：異常終了する
*/
void run68_abort(Long adr) {
  fprintf(stderr, "アドレス：%08X\n", adr);

  close_all_files();

#ifdef TRACE
  int i;
  printf("d0-7=%08lx", rd[0]);
  for (i = 1; i < 8; i++) {
    printf(",%08lx", rd[i]);
  }
  printf("\n");
  printf("a0-7=%08lx", ra[0]);
  for (i = 1; i < 8; i++) {
    printf(",%08lx", ra[i]);
  }
  printf("\n");
  printf("  pc=%08lx    sr=%04x\n", pc, sr);
#endif
  longjmp(jmp_when_abort, 2);
}

/* $Id: mem.c,v 1.2 2009-08-08 06:49:44 masamic Exp $ */

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2001/05/23 11:22:08  masamic
 * First imported source code and docs
 *
 * Revision 1.4  1999/12/07  12:47:22  yfujii
 * *** empty log message ***
 *
 * Revision 1.4  1999/11/29  06:18:06  yfujii
 * Calling CloseHandle instead of fclose when abort().
 *
 * Revision 1.3  1999/11/01  06:23:33  yfujii
 * Some debugging functions are introduced.
 *
 * Revision 1.2  1999/10/18  03:24:40  yfujii
 * Added RCS keywords and modified for WIN/32 a little.
 *
 */
