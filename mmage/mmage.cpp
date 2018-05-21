// WonW32.cpp : DLL アプリケーション用のエントリ ポイントを定義します。
//

//#define SHOW_RENDERTIME
#define INITGUID

#include "mmage.h"

#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <ctype.h>

#define DIRECTDRAW_VERSION 0x0500
#define DIRECTINPUT_VERSION 0x0500

#include <ddraw.h>
#include <dinput.h>

#include "../resource/resource.h"
#include "preference.h"
#include "../mmvm/mmvm.h"

extern "C" {
#include "../wonx/wonx.h"
#include "../wonx/wonxdisplay.h"
#include "../wonx/xdisplay.h"
#include "../wonx/xdisplayp.h"
#include "../wonx/wwlcdpanel.h"

#include "../cpu/mytypes.h"
#include "../cpu/global.h"
#include "../cpu/cpu.h"
}

typedef struct {
	WonXDisplay wonx_display;
	XDisplay x_display;
	WWLCDPanel ww_lcd_panel;
	WWScreen ww_screen[2];
	WWDisplay ww_display;
} WONXOBJECT, *LPWONXOBJECT;


#define MAX_LOADSTRING 100

static WONW32CONTEXT context;
static WONXOBJECT wonxobject;
LPWONW32CONTEXT wonw32ctx = &context;
static LPWONXOBJECT wonxobj = &wonxobject;
static HBITMAP titlebmp;
static HWND statusbar;
#define LCD_SLEEP 0
#define LCD_VERTICAL 1
#define LCD_HORIZONTAL 2
#define LCD_AUX1 3
#define LCD_AUX2 4
#define LCD_AUX3 5
static HICON lcdseg[6][2];

BOOL ddInit;
static LPDIRECTDRAW lpDD;
static LPDIRECTDRAWSURFACE lpDDSPrimary;
static LPDIRECTDRAWSURFACE screen[2];
static LPDIRECTDRAWCLIPPER lpClip;
#define COLORKEY (RGB(0x00, 0x00, 0x00))
#define SafeRelease(x) if (x) { x->Release(); x=NULL;}

extern "C" {
extern LPDIRECTINPUTDEVICE lpDIDevice;
}
LPDIRECTINPUT lpDI;
LPDIRECTINPUTDEVICE lpDIDevice;

static COLORREF w32colors[] = {
	COLORKEY,
	RGB(0xff, 0xff, 0xff),
	RGB(0xef, 0xef, 0xef),
	RGB(0xdf, 0xdf, 0xdf),
	RGB(0xcf, 0xcf, 0xcf),
	RGB(0xbf, 0xbf, 0xbf),
	RGB(0xaf, 0xaf, 0xaf),
	RGB(0x9f, 0x9f, 0x9f),
	RGB(0x8f, 0x8f, 0x8f),
	RGB(0x7f, 0x7f, 0x7f),
	RGB(0x6f, 0x6f, 0x6f),
	RGB(0x5f, 0x5f, 0x5f),
	RGB(0x4f, 0x4f, 0x4f),
	RGB(0x3f, 0x3f, 0x3f),
	RGB(0x2f, 0x2f, 0x2f),
	RGB(0x1f, 0x1f, 0x1f),
	RGB(0x0f, 0x0f, 0x0f),
};

static BYTE ddclrs[17][4];
static BYTE ddclrkey[4];
static int bytesPerPixel;
static LONG pitch[2];

static TCHAR szTitle[MAX_LOADSTRING];				// タイトル バー テキスト
static TCHAR szWindowClass[MAX_LOADSTRING];			// タイトル バー テキスト

static ATOM			MyRegisterClass( HINSTANCE hInstance );
static BOOL			InitInstance( HINSTANCE, int );
static LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
static LRESULT CALLBACK	About( HWND, UINT, WPARAM, LPARAM );
static LRESULT CALLBACK	License( HWND, UINT, WPARAM, LPARAM );


static void W32Display_DrawLCDWindow(HWND hWnd, WonXDisplay display);
static void DrawScreen(LPBYTE surfm, int screenno, LONG pitch);
static void DrawSprite(LPBYTE surfm, WWSprite sprite, LONG pitch);
static BOOL InitDDraw();
static BOOL InitDInput();
static int GetMaskInfo(DWORD bitmask, int *lpShift);
static void SetWindowSize();

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	MSG msg;
	HACCEL hAccelTable;
	int i, skip = 0;
	int drawline;
	DDSURFACEDESC lockedSurface[2];
	RECT dr;
	RECT sr[2];
	DDBLTFX ddbltfx;

	GetCurrentDirectory(MAX_PATH, mmconfig->workingdir);
	LoadConfig();

	if(!CreateVM())
		return FALSE;

	if (!WonX_IsCreated())
		WonX_Create();

	wonxobj->wonx_display = WonX_GetWonXDisplay();
	wonxobj->x_display = WonXDisplay_GetXDisplay(wonxobj->wonx_display);
	wonxobj->ww_display = WonXDisplay_GetWWDisplay(wonxobj->wonx_display);
	wonxobj->ww_lcd_panel = WWDisplay_GetLCDPanel(wonxobj->ww_display);
	wonxobj->ww_screen[0] = WWDisplay_GetScreen(wonxobj->ww_display, 0);
	wonxobj->ww_screen[1] = WWDisplay_GetScreen(wonxobj->ww_display, 1);

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass( hInstance );
	if( !InitInstance( hInstance, nCmdShow ) ) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_MMAGE);

	ddInit = InitDDraw();
	if(!ddInit) {
		MessageBox(wonw32ctx->hWnd, "DirectDraw の初期化に失敗しました。", NULL, MB_OK);
		return FALSE;
	}
	if(!InitDInput()) {
		MessageBox(wonw32ctx->hWnd, "DirectInput の初期化に失敗しました。", NULL, MB_OK);
		return TRUE;
	}
 
	wonw32ctx->syncperiod = 1000/75;
	wonw32ctx->syncevent = CreateEvent(NULL, TRUE, FALSE, NULL);
	wonw32ctx->running = FALSE;
	wonw32ctx->loaded = FALSE;

	XDisplay_SetColorMapPrint(wonxobj->x_display, 0);
	XDisplay_SetPalettePrint(wonxobj->x_display, 0);
	XDisplay_SetCharacterPrint(wonxobj->x_display, 0);
	XDisplay_SetSpritePrint(wonxobj->x_display, 0);

	wonw32ctx->title = CreateCompatibleDC(GetDC(wonw32ctx->hWnd));
	titlebmp = (HBITMAP)LoadBitmap(wonw32ctx->hInst, MAKEINTRESOURCE(IDB_TITLE));
	SelectObject(wonw32ctx->title, titlebmp);

	InvalidateRect(wonw32ctx->hWnd, NULL, FALSE);
	UpdateWindow(wonw32ctx->hWnd);

	lockedSurface[0].dwSize = sizeof(lockedSurface[0]);
	lockedSurface[1].dwSize = sizeof(lockedSurface[1]);

	if(lstrlen(lpCmdLine)) {
		LPTSTR str;
		TCHAR filename[MAX_PATH + 1];
		BOOL quote = FALSE;

		str = lpCmdLine;
		i = 0;
		while(*str) {
			if(_istspace(*str) && !quote) {
				*str = TEXT('\0');
				str++;
				break;
			} else if(quote && (*str == TEXT('\"') || *str == TEXT('\''))) {
				quote = FALSE;
			} else if(!quote && (*str == TEXT('\"') || *str == TEXT('\''))) {
				quote = TRUE;
			} else {
				filename[i] = *str;
				i++;
			}
			str++;
		}
		filename[i] = TEXT('\0');
		OpenAndRun(filename, str);
	}

	DragAcceptFiles(wonw32ctx->hWnd,TRUE);

	drawline = 0;
	ZeroMemory(&ddbltfx, sizeof(ddbltfx));
	ddbltfx.dwSize = sizeof(ddbltfx);
	sr[MMCONFIG_HORIZONTAL].top = sr[MMCONFIG_HORIZONTAL].left = 0;
	sr[MMCONFIG_HORIZONTAL].right = wonw32ctx->ww_lcd_width;
	sr[MMCONFIG_HORIZONTAL].bottom = wonw32ctx->ww_lcd_height;
	sr[MMCONFIG_VERTICAL].top = sr[MMCONFIG_VERTICAL].left = 0;
	sr[MMCONFIG_VERTICAL].right = wonw32ctx->ww_lcd_height;
	sr[MMCONFIG_VERTICAL].bottom = wonw32ctx->ww_lcd_width;

#ifdef SHOW_RENDERTIME
	DWORD oldt, t;
	BOOL showrt = TRUE;
	TCHAR rtmsg[256];
#endif

	// メイン メッセージ ループ:
	while(TRUE) {
		WWSprite sprite;
		POINT p;
		LPBYTE border;

		// ウィンドウがアクティブでないとき
		if(!wonw32ctx->active || (wonw32ctx->active && !wonw32ctx->running)) {
			if(GetMessage(&msg, NULL, 0, 0)) {
				if( !TranslateAccelerator (msg.hwnd, hAccelTable, &msg) ) {
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
			}
			if(msg.message == WM_QUIT)
				break;

			continue;
		}

		// ウィンドウがアクティブなとき
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT)
				break;

			if( !TranslateAccelerator (msg.hwnd, hAccelTable, &msg) ) {
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}

		} else {
#ifdef SHOW_RENDERTIME
			oldt = timeGetTime();
#endif
			screen[1]->Lock(NULL, &lockedSurface[0], DDLOCK_WAIT|DDLOCK_NOSYSLOCK, NULL);
			pitch[0] = lockedSurface[0].lPitch;

			screen[2]->Lock(NULL, &lockedSurface[1], DDLOCK_WAIT|DDLOCK_NOSYSLOCK, NULL);
			pitch[1] = lockedSurface[1].lPitch;

			DrawScreen((BYTE *)lockedSurface[0].lpSurface, 0, pitch[0]);
			DrawScreen((BYTE *)lockedSurface[1].lpSurface, 1, pitch[1]);

			if(WWDisplay_GetSpriteEnable(wonxobj->ww_display)) {
				for(i = WWDisplay_GetSpriteCount(wonxobj->ww_display) - 1; i >= 0; i--) {
					sprite = WWDisplay_GetSprite(wonxobj->ww_display, i + WWDisplay_GetSpriteStart(wonxobj->ww_display));
					if(!WWSprite_GetPriority(sprite)) {
						DrawSprite((BYTE *)lockedSurface[0].lpSurface, sprite, pitch[0]);
					} else {
						DrawSprite((BYTE *)lockedSurface[1].lpSurface, sprite, pitch[1]);
					}
				}
			}

			screen[1]->Unlock(lockedSurface[0].lpSurface);
			screen[2]->Unlock(lockedSurface[1].lpSurface);

			border = (LPBYTE)&ddclrs[WWColorMap_GetLCDColor(WWDisplay_GetColorMap(wonxobj->ww_display), WWDisplay_GetBorder(wonxobj->ww_display)) + 1];

			ddbltfx.dwFillColor =   (border[0] << 24)
				                  | (border[1] << 16)
								  | (border[2] <<  8)
								  |  border[3];

			screen[0]->Blt(NULL, NULL, NULL, DDBLT_WAIT|DDBLT_COLORFILL, &ddbltfx);
			screen[0]->BltFast(0, 0, screen[1], NULL, DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY);
			screen[0]->BltFast(0, 0, screen[2], NULL, DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY);

			ZeroMemory(&p, sizeof(p));
			ClientToScreen(wonw32ctx->hWnd, &p);
			dr.left = p.x;
			dr.top = p.y;
			dr.right = dr.left + wonw32ctx->winwidth + 1;
			dr.bottom = dr.top + wonw32ctx->winheight + 1;
			lpDDSPrimary->Blt(&dr, screen[0], &sr[mmconfig->horizontal], DDBLT_WAIT, NULL);
			PulseEvent(wonw32ctx->syncevent);
#ifdef SHOW_RENDERTIME
			if(showrt) {
				t = timeGetTime();
				wsprintf(rtmsg, TEXT("1 フレーム描画にかかった時間は %d msec"), t - oldt);
				MessageBox(wonw32ctx->hWnd, rtmsg, TEXT("描画時間"), MB_OK);
				showrt = FALSE;
			}
#endif
		}
	}

	if(wonw32ctx->running)
		StopExecution(1000);

	DeleteObject(titlebmp);
	DeleteDC(wonw32ctx->title);

	SafeRelease(lpDI);
	SafeRelease(lpDIDevice);
	for(i = 2; i >= 0; i--) {
		SafeRelease(screen[i]);
	}

	SafeRelease(lpDDSPrimary);
	SafeRelease(lpDD);

	CloseHandle(wonw32ctx->syncevent);
	
	if(wonw32ctx->hWnd != INVALID_HANDLE_VALUE)
		DestroyWindow(wonw32ctx->hWnd);
	free(memory);

	SaveConfig();

	return msg.wParam;	
	
}

static BOOL InitDDraw() {
	DDSURFACEDESC ddsd;
	DDPIXELFORMAT ddpf;
	DDCOLORKEY key;

	int rsz, gsz, bsz;
	int rsh, gsh, bsh;
	int r, g, b;
	DWORD compiledPixel;
	int i;

	HINSTANCE dddll;
	typedef HRESULT (WINAPI *DDC)(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter); 

	DDC ddc;

	dddll = LoadLibrary(TEXT("ddraw.dll"));
	ddc = (DDC)GetProcAddress(dddll, TEXT("DirectDrawCreate"));
	if(!ddc)
		return FALSE;

	if(ddc(NULL, &lpDD, NULL) != DD_OK) {
		return FALSE;
	}
	if(lpDD->SetCooperativeLevel(wonw32ctx->hWnd, DDSCL_NORMAL) != DD_OK) {
		return FALSE;
	}
	if(lpDD->CreateClipper(NULL, &lpClip, NULL) != DD_OK) {
		return FALSE;
	}
	lpClip->SetHWnd(0, wonw32ctx->hWnd);
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	if(lpDD->CreateSurface(&ddsd, &lpDDSPrimary, NULL) != DD_OK) {
		return FALSE;
	}
	lpDDSPrimary->SetClipper(lpClip);

	ZeroMemory(&ddsd, sizeof(ddsd));

	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = ddsd.dwHeight = max(wonw32ctx->ww_lcd_width, wonw32ctx->ww_lcd_height);

	for(i = 0; i < 3; i++) {
		if(lpDD->CreateSurface(&ddsd, &screen[i], NULL) != DD_OK) {
			return FALSE;
		}
	}

	// Caliculate compiled pixel value for surface
	ZeroMemory(&ddpf, sizeof(ddpf));
	ddpf.dwSize = sizeof(ddpf);
	lpDDSPrimary->GetPixelFormat(&ddpf);

	bytesPerPixel = ddpf.dwRGBBitCount / 8;
	if(bytesPerPixel == 1) {
		MessageBox(wonw32ctx->hWnd, "256 色モードでは動作しません。", NULL, MB_OK);
		return FALSE;
	}

	rsz = GetMaskInfo(ddpf.dwRBitMask, &rsh);
	gsz = GetMaskInfo(ddpf.dwGBitMask, &gsh);
	bsz = GetMaskInfo(ddpf.dwBBitMask, &bsh);

	for(i = 0; i < 17; i++) {
		r = GetRValue(w32colors[i]);
		g = GetGValue(w32colors[i]);
		b = GetBValue(w32colors[i]);

		r >>= (8-rsz);
		g >>= (8-gsz);
		b >>= (8-bsz);
		r <<= rsh;
		g <<= gsh;
		b <<= bsh;

		compiledPixel = (DWORD)(r | g | b);
		ddclrs[i][3] = (BYTE) compiledPixel;
		ddclrs[i][2] = (BYTE)(compiledPixel >>= 8);
		ddclrs[i][1] = (BYTE)(compiledPixel >>= 8);
		ddclrs[i][0] = (BYTE)(compiledPixel >>= 8);
	}
	r = GetRValue(COLORKEY);
	g = GetGValue(COLORKEY);
	b = GetBValue(COLORKEY);
	r >>= (8-rsz);
	g >>= (8-gsz);
	b >>= (8-bsz);
	r <<= rsh;
	g <<= gsh;
	b <<= bsh;
	compiledPixel = (DWORD)(r | g | b);
	for(i = 0; i < 3; i++) {
		key.dwColorSpaceHighValue = key.dwColorSpaceLowValue = compiledPixel;
		screen[i]->SetColorKey(DDCKEY_SRCBLT, &key);
	}
	ddclrkey[3] = (BYTE) compiledPixel;
	ddclrkey[2] = (BYTE)(compiledPixel >>= 8);
	ddclrkey[1] = (BYTE)(compiledPixel >>= 8);
	ddclrkey[0] = (BYTE)(compiledPixel >>= 8);

	return TRUE;
}

static BOOL InitDInput() {
	HINSTANCE didll;
	typedef HRESULT (WINAPI *DICA)(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUT * lplpDirectInput, LPUNKNOWN punkOuter);
	DICA dica;

	didll = LoadLibrary(TEXT("dinput.dll"));
	dica = (DICA)GetProcAddress(didll, TEXT("DirectInputCreateA"));

	if(!dica)
		return FALSE;

	if(FAILED((dica)(wonw32ctx->hInst, DIRECTINPUT_VERSION, &lpDI, NULL))) {
		return FALSE;
	}		 
	if(FAILED(lpDI->CreateDevice(GUID_SysKeyboard, &lpDIDevice, NULL))) { 
		return FALSE;
	}
	if(FAILED(lpDIDevice->SetDataFormat(&c_dfDIKeyboard))) {
		return FALSE;
	}
	if(FAILED(lpDIDevice->SetCooperativeLevel(wonw32ctx->hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) {
		return FALSE;
	}
	lpDIDevice->Acquire();

	return TRUE;
}

static int GetMaskInfo(DWORD bitmask, int *lpShift) {
	int precision, shift;

	precision = shift = 0;
	// count the zeros on right hand side
	while(!(bitmask & 0x01L)) {
		bitmask >>= 1;
		shift++;
	}

	// count the ones on right hand side
	while(bitmask & 0x01L) {
		bitmask >>= 1;
		precision++;
	}
	*lpShift = shift;
	return precision;
}

//
//  関数: MyRegisterClass()
//
//  用途: ウィンドウ クラスの登録
//
//  コメント:
//
//    この関数およびその使用はこのコードを Windows 95 で先に追加された
//    'RegisterClassEx' 関数と Win32 システムの互換性を保持したい場合に
//    のみ必要となります。アプリケーションが、アプリケーションに関連付け
//    られたスモール アイコンを取得できるよう、この関数を呼び出すことは
//    重要です。
//
ATOM MyRegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style		= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon		= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MMAGE));
	wcex.hCursor		= LoadCursor((HINSTANCE)NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)NULL;
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MMAGE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon((HINSTANCE)wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx( &wcex );
}

//
//   関数: InitInstance(HANDLE, int)
//
//   用途: インスタンス ハンドルの保存とメイン ウィンドウの作成
//
//   コメント:
//
//        この関数では、インスタンス ハンドルをグローバル変数に保存し、プログラムの
//        メイン ウィンドウを作成し表示します。
//
BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
	WWLCDPanel ww_lcd_panel;
	INITCOMMONCONTROLSEX icce;

	ZeroMemory(&icce, sizeof(icce));
	icce.dwSize = sizeof(icce);
	icce.dwICC = ICC_DATE_CLASSES | ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icce);
	wonw32ctx->hInst = hInstance; // グローバル変数にインスタンス ハンドルを保存します

	ww_lcd_panel = WWDisplay_GetLCDPanel(WonXDisplay_GetWWDisplay(WonX_GetWonXDisplay()));

	wonw32ctx->ww_lcd_width = WWLCDPanel_GetWidth(ww_lcd_panel);
	wonw32ctx->ww_lcd_height = WWLCDPanel_GetHeight(ww_lcd_panel);

	wonw32ctx->hWnd = CreateWindow(szWindowClass, THIS_APP_TITLE, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX, CW_USEDEFAULT, 0, 
		CW_USEDEFAULT, 0,	
		NULL, NULL, hInstance, NULL);

	if( !wonw32ctx->hWnd ) {
		return FALSE;
	}

	lcdseg[LCD_SLEEP][0] = LoadIcon(wonw32ctx->hInst, MAKEINTRESOURCE(IDI_SLEEPOFF));
	lcdseg[LCD_VERTICAL][0]= LoadIcon(wonw32ctx->hInst, MAKEINTRESOURCE(IDI_VERTICALOFF));
	lcdseg[LCD_HORIZONTAL][0] = LoadIcon(wonw32ctx->hInst, MAKEINTRESOURCE(IDI_HORIZONTALOFF));
	lcdseg[LCD_AUX1][0] = LoadIcon(wonw32ctx->hInst, MAKEINTRESOURCE(IDI_AUX1OFF));
	lcdseg[LCD_AUX2][0] = LoadIcon(wonw32ctx->hInst, MAKEINTRESOURCE(IDI_AUX2OFF));
	lcdseg[LCD_AUX3][0] = LoadIcon(wonw32ctx->hInst, MAKEINTRESOURCE(IDI_AUX3OFF));

	lcdseg[LCD_SLEEP][1] = LoadIcon(wonw32ctx->hInst, MAKEINTRESOURCE(IDI_SLEEP));
	lcdseg[LCD_VERTICAL][1]= LoadIcon(wonw32ctx->hInst, MAKEINTRESOURCE(IDI_VERTICAL));
	lcdseg[LCD_HORIZONTAL][1] = LoadIcon(wonw32ctx->hInst, MAKEINTRESOURCE(IDI_HORIZONTAL));
	lcdseg[LCD_AUX1][1] = LoadIcon(wonw32ctx->hInst, MAKEINTRESOURCE(IDI_AUX1));
	lcdseg[LCD_AUX2][1] = LoadIcon(wonw32ctx->hInst, MAKEINTRESOURCE(IDI_AUX2));
	lcdseg[LCD_AUX3][1] = LoadIcon(wonw32ctx->hInst, MAKEINTRESOURCE(IDI_AUX3));

	statusbar = CreateStatusWindow(WS_CHILD | WS_VISIBLE | CCS_BOTTOM,
					NULL, wonw32ctx->hWnd, 0);
	SendMessage(statusbar, SB_SETMINHEIGHT, (WPARAM)16, 0);

	if(!statusbar) {
		return FALSE;
	}

	SetWindowSize();
	switch(mmconfig->viewsize) {
		case 1:
			CheckMenuRadioItem(GetMenu(wonw32ctx->hWnd), IDM_X1, IDM_X4, IDM_X1, MF_BYCOMMAND);
			break;
		case 2:
			CheckMenuRadioItem(GetMenu(wonw32ctx->hWnd), IDM_X1, IDM_X4, IDM_X2, MF_BYCOMMAND);
			break;
		case 3:
			CheckMenuRadioItem(GetMenu(wonw32ctx->hWnd), IDM_X1, IDM_X4, IDM_X3, MF_BYCOMMAND);
			break;
		case 4:
			CheckMenuRadioItem(GetMenu(wonw32ctx->hWnd), IDM_X1, IDM_X4, IDM_X4, MF_BYCOMMAND);
			break;
	}
	if(mmconfig->horizontal)
		CheckMenuRadioItem(GetMenu(wonw32ctx->hWnd), IDM_HORIZONTAL, IDM_VERTICAL, IDM_HORIZONTAL, MF_BYCOMMAND);
	else
		CheckMenuRadioItem(GetMenu(wonw32ctx->hWnd), IDM_HORIZONTAL, IDM_VERTICAL, IDM_VERTICAL, MF_BYCOMMAND);

	ShowWindow( wonw32ctx->hWnd, nCmdShow );
	UpdateWindow( wonw32ctx->hWnd );

	return TRUE;
}

void SetWindowSize() {
	int width, height;
	RECT r;

	wonw32ctx->winwidth = (mmconfig->horizontal ? wonw32ctx->ww_lcd_width : wonw32ctx->ww_lcd_height) * mmconfig->viewsize;
	wonw32ctx->winheight = (mmconfig->horizontal ? wonw32ctx->ww_lcd_height : wonw32ctx->ww_lcd_width) * mmconfig->viewsize;

	GetWindowRect(statusbar, &r);
	width = wonw32ctx->winwidth + GetSystemMetrics(SM_CXFIXEDFRAME) * 2;
	height = wonw32ctx->winheight + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYFIXEDFRAME) * 2
		+ (r.bottom - r.top);

	SetWindowPos(wonw32ctx->hWnd, 0, 0, 0, width, height, SWP_NOMOVE|SWP_NOZORDER);
}

//
//  関数: WndProc(HWND, unsigned, WORD, LONG)
//
//  用途: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 終了メッセージの通知とリターン
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	BOOL handled = TRUE;
	HMENU menu;
	HDROP hdrop;
	TCHAR drpfile[MAX_PATH + 1];
	int sbsize[2];
	LPDRAWITEMSTRUCT lpdi;

	menu = GetMenu(hWnd);

	switch( message ) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// メニュー選択の解析:
			switch( wmId ) 
			{
				case IDM_ABOUT:
				   DialogBox(wonw32ctx->hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_LICENSE:
				   DialogBox(wonw32ctx->hInst, (LPCTSTR)IDD_LICENSE, hWnd, (DLGPROC)License);
				   break;
				case IDM_EXIT:
					PostQuitMessage( 0 );
				   break;

				case IDM_OPEN:
					OpenDlgAndRun();
					break;

				case IDM_RESTART:
					if(!wonw32ctx->running && wonw32ctx->loaded)
						Restart();
					break;

				case IDM_STOP:
					if(wonw32ctx->running)
						StopExecution(5000);
					break;

				case IDM_OPENWEB:
					ShellExecute(NULL, NULL, THIS_APP_URL, NULL, NULL, SW_SHOWNORMAL);
					break;

				case IDM_X1:
					mmconfig->viewsize = 1;
					SetWindowSize();
					CheckMenuRadioItem(menu, IDM_X1, IDM_X4, IDM_X1, MF_BYCOMMAND);
					break;
				case IDM_X2:
					mmconfig->viewsize = 2;
					SetWindowSize();
					CheckMenuRadioItem(menu, IDM_X1, IDM_X4, IDM_X2, MF_BYCOMMAND);
					break;
				case IDM_X3:
					mmconfig->viewsize = 3;
					SetWindowSize();
					CheckMenuRadioItem(menu, IDM_X1, IDM_X4, IDM_X3, MF_BYCOMMAND);
					break;
				case IDM_X4:
					mmconfig->viewsize = 4;
					SetWindowSize();
					CheckMenuRadioItem(menu, IDM_X1, IDM_X4, IDM_X4, MF_BYCOMMAND);
					break;

				case IDM_HORIZONTAL:
					mmconfig->horizontal = MMCONFIG_HORIZONTAL;
					SetWindowSize();
					CheckMenuRadioItem(menu, IDM_HORIZONTAL, IDM_VERTICAL, IDM_HORIZONTAL, MF_BYCOMMAND);
					break;
				case IDM_VERTICAL:
					mmconfig->horizontal = MMCONFIG_VERTICAL;
					SetWindowSize();
					CheckMenuRadioItem(menu, IDM_HORIZONTAL, IDM_VERTICAL, IDM_VERTICAL, MF_BYCOMMAND);
					break;

				case IDM_PREFERENCE:
					OpenPreferenceDlg();
					break;

				default:
					handled = FALSE;
			}
			break;

		case WM_PAINT:
			if(wonw32ctx->running) {
				SendMessage(statusbar, SB_SETTEXT, (WPARAM)0, (LPARAM)TEXT("実行中"));
			} else {
				SendMessage(statusbar, SB_SETTEXT, (WPARAM)0, (LPARAM)TEXT(""));
				W32Display_DrawLCDWindow(hWnd, WonX_GetWonXDisplay());
			}
			handled = FALSE;
			break;

		case WM_SIZE:
			sbsize[0] = LOWORD(lParam) - 100;
			sbsize[1] = LOWORD(lParam);
			SendMessage(statusbar, SB_SETPARTS, 2, (LPARAM)sbsize);
			SendMessage(statusbar, WM_SIZE, wParam, lParam);
			SendMessage(statusbar, SB_SETTEXT, (WPARAM)1 | SBT_OWNERDRAW, (LPARAM)NULL);
			break;

		case WM_DRAWITEM:
			lpdi = (LPDRAWITEMSTRUCT)lParam;
			DrawIconEx(lpdi->hDC, lpdi->rcItem.left     , lpdi->rcItem.top,
				lcdseg[LCD_SLEEP][mmvm->lcdseg & (1 << LCD_SLEEP) ? 1 : 0] , 16, 16, 0, NULL, DI_IMAGE);
			DrawIconEx(lpdi->hDC, lpdi->rcItem.left + 16, lpdi->rcItem.top,
				lcdseg[LCD_VERTICAL][mmvm->lcdseg & (1 << LCD_VERTICAL) ? 1 : 0], 16, 16, 0, NULL, DI_IMAGE);
			DrawIconEx(lpdi->hDC, lpdi->rcItem.left + 32, lpdi->rcItem.top,
				lcdseg[LCD_HORIZONTAL][mmvm->lcdseg & (1 << LCD_HORIZONTAL) ? 1 : 0], 16, 16, 0, NULL, DI_IMAGE);
			DrawIconEx(lpdi->hDC, lpdi->rcItem.left + 48, lpdi->rcItem.top,
				lcdseg[LCD_AUX1][mmvm->lcdseg & (1 << LCD_AUX1) ? 1 : 0], 16, 16, 0, NULL, DI_IMAGE);
			DrawIconEx(lpdi->hDC, lpdi->rcItem.left + 64, lpdi->rcItem.top,
				lcdseg[LCD_AUX2][mmvm->lcdseg & (1 << LCD_AUX2) ? 1 : 0], 16, 16, 0, NULL, DI_IMAGE);
			DrawIconEx(lpdi->hDC, lpdi->rcItem.left + 80, lpdi->rcItem.top,
				lcdseg[LCD_AUX3][mmvm->lcdseg & (1 << LCD_AUX3) ? 1 : 0], 16, 16, 0, NULL, DI_IMAGE);
			break;

		case WM_ENTERMENULOOP:
			if(wonw32ctx->running) {
				EnableMenuItem(menu, IDM_STOP, MF_BYCOMMAND | MF_ENABLED);
			} else {
				EnableMenuItem(menu, IDM_STOP, MF_BYCOMMAND | MF_GRAYED);
			}
			if(wonw32ctx->loaded) {
				EnableMenuItem(menu, IDM_RESTART, MF_BYCOMMAND | MF_ENABLED);
			} else {
				EnableMenuItem(menu, IDM_RESTART, MF_BYCOMMAND | MF_GRAYED);
			}
			break;

		case WM_DROPFILES:
			hdrop=(HDROP) wParam;

			DragQueryFile(hdrop, 0, drpfile, MAX_PATH);
			OpenAndRun(drpfile, TEXT(""));

			DragFinish(hdrop);
			break;

		case WM_NCACTIVATE:
			wonw32ctx->active = (BOOL) wParam;
			if(wonw32ctx->running) {
				if(wonw32ctx->active) {
					ResumeThread(wonw32ctx->cputhread);
				} else {
#ifndef _DEBUG
					SuspendThread(wonw32ctx->cputhread);
#endif
				}
			}
			handled = FALSE;
			break;

		case WM_CLOSE:
			PostQuitMessage( 0 );
			break;

		default:
			handled = FALSE;
			break;
   }

	if(handled)
		return FALSE;
	else
		return DefWindowProc( hWnd, message, wParam, lParam );
}

// バージョン情報ボックス用メッセージ ハンドラ
LRESULT CALLBACK About( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if( LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL ) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

// ライセンス情報ボックス用メッセージ ハンドラ
LRESULT CALLBACK License( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	HRSRC rsc;
	LPCTSTR msg;
	HWND msgwnd;

	switch( message )
	{
		case WM_INITDIALOG:
			rsc = FindResource(NULL, MAKEINTRESOURCE(IDR_LICENSE), TEXT("TEXT"));
			msg = (LPCTSTR)LoadResource(NULL, rsc);
			msgwnd = GetDlgItem(hDlg, IDC_LICENSE);
			SetWindowText(msgwnd, msg);
			return TRUE;

		case WM_COMMAND:
			if( LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL ) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

static void W32Display_DrawLCDWindow(HWND hWnd, WonXDisplay display) {
	HDC hdc;
	RECT dr, sr;
	POINT p;

	if(!ddInit)
		return;

	p.x = p.y = 0;
	ClientToScreen(wonw32ctx->hWnd, &p);
	dr.left = p.x;
	dr.top = p.y;
	dr.right = dr.left + wonw32ctx->winwidth;
	dr.bottom = dr.top + wonw32ctx->winheight;
	sr.left = sr.top = 0;
	sr.right = wonw32ctx->ww_lcd_width;
	sr.bottom = wonw32ctx->ww_lcd_height;

	screen[0]->GetDC(&hdc);
	BitBlt(hdc, 0, 0, wonw32ctx->ww_lcd_width, wonw32ctx->ww_lcd_height, wonw32ctx->title, 0, 0, SRCCOPY);
	screen[0]->ReleaseDC(hdc);

	lpDDSPrimary->Blt(&dr, screen[0], &sr, DDBLT_WAIT, NULL);
}

static void DrawScreen(LPBYTE surfm, int screenno, LONG pitch) {
	int surfa;
	WWScreen screen = wonxobj->ww_screen[screenno];
	int plt[9];
	int *palette = &plt[1];

	int pixel;
	int x, y;
	int sx = 0, sy = 0, ex = 0, ey = 0;
	int mode = 0;
	int i;
	int rx, ry;
	BOOL enable;

	plt[0] = -1;
	WWColorMap_GetLCDColors(WWDisplay_GetColorMap(wonxobj->ww_display), plt + 1);

	if(screenno)
		mode = WWScreen_GetMode(screen);

	rx = WWScreen_GetRollX(screen);
	ry = WWScreen_GetRollY(screen);

	if ( (mode == WW_SCREEN_INSIDE_ONLY) || (mode == WW_SCREEN_OUTSIDE_ONLY) ) {
		sx = WWScreen_GetDrawX(screen);
		sy = WWScreen_GetDrawX(screen);

		ex = sx + WWScreen_GetDrawWidth( screen);
		ey = sy + WWScreen_GetDrawHeight(screen);
	}

	enable = WWScreen_GetEnable(screen);

	surfa = 0;

	if(mmconfig->horizontal) {
		for(y = 0; y < wonw32ctx->ww_lcd_height; y++) {
			surfa = y * pitch;
			for (x = 0; x < wonw32ctx->ww_lcd_width; x++) {
				pixel = enable ? WWScreen_GetPixel(screen, x + rx, y + ry) : -1;
				if (mode == WW_SCREEN_INSIDE_ONLY) {
					if( (x < sx) || (x > ex) || (y < sy) || (y > ey) ) {
						pixel = -1;
					}
				} else if (mode == WW_SCREEN_OUTSIDE_ONLY) {
					if ( (x >= sx) && (x <= ex) && (y >= sy) && (y <= ey) ) {
						pixel = -1;
					}
				}
				for(i = 3; i >= (4 - bytesPerPixel); i--) {
					surfm[surfa++] = ddclrs[palette[pixel] + 1][i];
				}
			}
		}
	} else {
		for(y = 0; y < wonw32ctx->ww_lcd_width; y++) {
			surfa = y * pitch;
			for (x = 0; x < wonw32ctx->ww_lcd_height; x++) {
				pixel = enable ? WWScreen_GetPixel(screen, (wonw32ctx->ww_lcd_width - y) + rx, x + ry) : -1;
				if (mode == WW_SCREEN_INSIDE_ONLY) {
					if( (x < sx) || (x > ex) || (y < sy) || (y > ey) ) {
						pixel = -1;
					}
				} else if (mode == WW_SCREEN_OUTSIDE_ONLY) {
					if ( (x >= sx) && (x <= ex) && (y >= sy) && (y <= ey) ) {
						pixel = -1;
					}
				}
				for(i = 3; i >= (4 - bytesPerPixel); i--) {
					surfm[surfa++] = ddclrs[palette[pixel] + 1][i];
				}
			}
		}
	}
}

static void DrawSprite(LPBYTE surfm, WWSprite sprite, LONG pitch) {
	int surfa;
	int x, y, lcd_x, lcd_y;
	int sx, sy, ex, ey;
	int pixel, outside;
	WWDisplay display = wonxobj->ww_display;
	int palette[8];
	int i;

	WWColorMap_GetLCDColors(WWDisplay_GetColorMap(display), palette);

	sx = WWDisplay_GetSpriteWindowX(display) - 1;
	sy = WWDisplay_GetSpriteWindowY(display) - 1;

	ex = sx + WWDisplay_GetSpriteWindowWidth(display);
	ey = sy + WWDisplay_GetSpriteWindowHeight(display);

	for (y = 0; y < 8; y++) {
		for (x = 0; x < 8; x++) {
			pixel = WWSprite_GetPixel(sprite, x, y);

			/* 透明色の場合 */
			if (pixel == -1) continue;

			lcd_x = WWSprite_GetX(sprite) + x;
			lcd_y = WWSprite_GetY(sprite) + y;

			if (WWDisplay_GetSpriteWindowEnable(display)) {
				if ( (lcd_x < sx) || (lcd_y < sy) || (lcd_x > ex) || (lcd_y > ey) )
					outside = 1;
				else
					outside = 0;

				if (WWSprite_GetClipping(sprite)) { /* ウインドウの外側部分を表示 */
					if (!outside) continue;
				} else {                            /* ウインドウの内側部分を表示 */
					if (outside) continue;
				}
			}

			if(mmconfig->horizontal)
				surfa = (lcd_x * bytesPerPixel) + (pitch * lcd_y);
			else
				surfa = (lcd_y * bytesPerPixel) + (pitch * (wonw32ctx->ww_lcd_width - lcd_x));

			for(i = 3; i >= (4 - bytesPerPixel); i--) {
				surfm[surfa++] = ddclrs[palette[pixel] + 1][i];
			}
		}
	}

	return;
}

#ifdef _DEBUG
void ShowError(int code) {
	LPVOID lpMsgBuf;

	FormatMessage( 
	    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
	    FORMAT_MESSAGE_FROM_SYSTEM | 
	    FORMAT_MESSAGE_IGNORE_INSERTS,
	    NULL,
	    code,
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト言語
	    (LPTSTR) &lpMsgBuf,
	    0,
	    NULL 
	);

	MessageBox( NULL, (LPTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );

	LocalFree( lpMsgBuf );
}
#endif

