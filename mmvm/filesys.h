#ifndef _MMVM_FILESYS_H
#define _MMVM_FILESYS_H

#include "mmvm.h"
#include "wwstruct.h"

void InitFilesys(LPMMVM mmvm);
void CloseFilesys();
void HandleFsIL(LPMMVM mmvm);

#endif // #ifdef _MMVM_FILESYS_H