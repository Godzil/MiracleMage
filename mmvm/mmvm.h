#ifndef _MMVM_H
#define _MMVM_H

#include "wwstruct.h"
extern "C" {
#include "../cpu/mytypes.h"
#include "../cpu/global.h"
#include "../cpu/cpu.h"
}

#ifdef __cplusplus
extern "C" {
#endif

#define MEMORYSIZE (1 * 1024 * 1024)

#define ELISA_FONTFILE TEXT("elisa100.fnt")

#define SRAMWORK_SEG 0x1000
#define APP_CSEG 0x3000
#define FIRST_DSEG 0x1000
#define FINAL_DSEG 0x2000
#define STACK_SEG 0xF000

#define DIRENT_NUM 4
#define DIRENT_ROOT 0
#define DIRENT_KERN 1
#define DIRENT_ROM0 2
#define DIRENT_RAM0 3

typedef struct {
	char className[32];
	char name[32];
	char version[32];
	char description[32];
	char **depends;
} MMVM_ILInfo, *LPMMVM_ILInfo;

typedef struct {
	WW_FARPTR il;
	MMVM_ILInfo ilinfo;
	WW_FARPTR proc;
	MMVM_ILInfo procinfo;
	WW_FARPTR fs;
	MMVM_ILInfo fsinfo;
} MMVM_IL, *LPMMVM_IL;

typedef struct {
	WW_FENT_T fent;
	TCHAR commandline[MAX_WW_ARG];
	BOOL loadended;
	BOOL maincalled;
} MMVM_APPINFO, *LPMMVM_APPINFO;

typedef struct {
	LPBYTE memory;
	LPMMVM_IL il;
	WW_FARPTR cwfs;
	LPWW_SRAMWork swork;
	LPMMVM_APPINFO appinfo;
	int lcdseg;
} MMVM, *LPMMVM;

extern LPMMVM mmvm;

BOOL CreateVM();
void InitVM();
void UpdateVM();
void OpenDlgAndRun();
void OpenAndRun(LPCTSTR filename, LPCTSTR cmdline);
void LoadAndRun();
BOOL LoadFXFile(LPCTSTR filename, WORD addr, WW_FENT_T *fentp);
void RunWWApp();
void Restart();
void StopExecution(DWORD wait);
LPBYTE GetArgAddr(int pos);
void ILReturn();

#ifdef __cplusplus
}
#endif

#endif // #ifndef _MMVM_H