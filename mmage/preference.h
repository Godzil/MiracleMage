#ifndef _PREFERENCE_H
#define _PREFERENCE_H

#include "mmage.h"
#include "../mmvm/wwstruct.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	MMCONFIG_VERTICAL = 0,
	MMCONFIG_HORIZONTAL,
};

typedef struct {
	TCHAR workingdir[MAX_PATH + 1];
	int viewsize;
	int horizontal;
	BOOL queryarg;
	TCHAR argument[MAX_WW_ARG];
	TCHAR rom0dir[MAX_PATH + 1];
	TCHAR ram0dir[MAX_PATH + 1];
	WW_OWNERINFO ownerinfo;
} MMAGE_CONFIG, *LPMMAGE_CONFIG;

extern LPMMAGE_CONFIG mmconfig;

void LoadConfig();
void OpenPreferenceDlg();
void SaveConfig();

#ifdef __cplusplus
}
#endif

#endif // #ifdef _PREFERENCE_H