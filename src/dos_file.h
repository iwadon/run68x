// run68x - Human68k CUI Emulator based on run68
// Copyright (C) 2025 TcbnErik
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

#ifndef DOS_FILE_H
#define DOS_FILE_H

#include "run68.h"

// ファイルハンドル管理(2層構造: ハンドルテーブル htable と実体テーブル ftable)。
void InitFileHandleTable(void);            // 全ハンドルを未使用に初期化
bool IsOpened(Long fileno);                // ハンドルがオープン中か
FILEINFO* GetFinfo(Long fileno);           // ハンドル→実体(未使用ならNULL)
void ShareHandle(Long to, Long from);      // fromの実体をtoにも参照させる(DUP用)
FILEINFO* UnbindHandle(Long fileno);       // 紐付け解除。実体解放時のみ実体を返す

Long FindFreeFileNo(void);
Long CreateNewfile(ULong file, UWord atr, bool newfile);
Long OpenExistingFile(ULong file, UWord mode);

Long DosMkdir(ULong param);
Long DosRmdir(ULong param);
Long DosChdir(ULong param);
Long DosCreate(ULong param);
Long DosOpen(ULong param);
Long DosRead(ULong param);
Long DosSeek(ULong param);
Long DosChmod(ULong param);
Long DosCurdir(ULong param);
Long DosFiledate(ULong param);
Long DosMaketmp(ULong param);
Long DosNewfile(ULong param);

FILEINFO* SetFinfo(Long fileno, HostFileInfoMember hostfile, FileOpenMode mode,
                   unsigned int nest);
void FreeOnmemoryFile(FILEINFO* finfop);
void ReadOnmemoryFile(FILEINFO* finfop, FileOpenMode openMode);

#endif
