#ifndef _MMAGE_H
#define _MMAGE_H

#define THIS_APP_TITLE TEXT("MiracleMage")
#define THIS_APP_URL TEXT("http://www04.u-page.so-net.ne.jp/td5/zry/WonderWitch/MiracleMage/")

#ifdef __cplusplus
extern "C" {
#endif

#define WINVER        0x0400    // Win 9x / NT 4.0 or later
#define _WIN32_IE     0x0300    // IE 3.0 or later
#include <windows.h>

typedef struct {
	HINSTANCE hInst;
	HWND hWnd;
	HDC title;
	HANDLE syncevent;
	int ww_lcd_width;
	int winwidth;
	int ww_lcd_height;
	int winheight;
	int syncperiod;
	BOOL running;
	BOOL loaded;
	HANDLE cputhread;
	BOOL active;
	int key;
	int keyhit;
	int oldkey;
} WONW32CONTEXT, *LPWONW32CONTEXT;

extern LPWONW32CONTEXT wonw32ctx;

#ifdef _DEBUG
void ShowError(int code);
#endif

#ifdef __cplusplus
}
#endif

#endif // #ifndef _MMAGE_H
