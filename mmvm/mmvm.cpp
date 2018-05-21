#include "../mmage/mmage.h"
#include "../mmage/preference.h"

#include "../resource/resource.h"
#include "wwstruct.h"
#include "mmvm.h"
#include "filesys.h"
#include "kanjifont.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <process.h>
#include <tchar.h>

#ifdef __GNUC__
#if defined(_MBCS)
#define _tmakepath _makepath
#define _tsplitpath _splitpath
#elif defined(_UNICODE)
#define _tmakepath _wmakepath
#define _tsplitpath _wplitpath
#else
#define _tmakepath _makepath
#define _tsplitpath _splitpath
#endif
#endif

/* 
  IL は CS:ip が特定アドレスになったところでトラップすることで実現する

  IlibIL は 0100:0000 に配置。
  IlibIL の関数は 0100:0000 から 1byte ずつ順に配置されているように見せる。
    _get_info() は 0100:0000
	_open() は 0100:0001
	_open_system() は 0100:0002
  という具合に。
  Ilib の ILInfo は 0100:0100 に配置。
  Ilib の ILInfo が指す文字列は 0100:(0100 + ILInfo のサイズ) に配置。
    なので IL の関数の数は(_get_info() を含めて) 0x0100 個まで。

  ProcIL は 0200:0000 から、FsIL は 0300:0000 から同じように配置。
  こんなんでええんか？
*/

#define MMVM_ILIBILADDR	0x01000000
#define MMVM_PROCILADDR	0x02000000
#define MMVM_FSILADDR	0x03000000

static MMVM_IL mmvmil_struct;
static MMVM mmvm_struct;
static MMVM_APPINFO appinfo_struct;

LPMMVM mmvm;
BYTE *memory;

static void HandleIlibIL();
static void HandleProcIL();

static LRESULT CALLBACK GetArgument(HWND, UINT, WPARAM, LPARAM);
static void StartThread(LPVOID arg);

BOOL CreateVM() {
	LPBYTE rom0;
	TCHAR fontfile[MAX_PATH + 1];
   char drive[_MAX_DRIVE + 1];
   char dir[_MAX_DIR + 1];

	mmvm =  &mmvm_struct;
	mmvm->memory = memory = (BYTE *)malloc(MEMORYSIZE);
	if(!memory) {
		return FALSE;
	}
	memset(memory, 0, MEMORYSIZE);

	mmvm->il = &mmvmil_struct;
	mmvm->appinfo = &appinfo_struct;

	mmvm->il->il.farptr = MMVM_ILIBILADDR;
	strcpy(mmvm->il->ilinfo.className, "ILibIL");
	strcpy(mmvm->il->ilinfo.name, "ILibIL");
	strcpy(mmvm->il->ilinfo.version, "1.0");
	strcpy(mmvm->il->ilinfo.description, "MMVM default ILibIL");
	mmvm->il->ilinfo.depends = NULL;

	mmvm->il->proc.farptr = MMVM_PROCILADDR;
	strcpy(mmvm->il->procinfo.className, "ProcIL");
	strcpy(mmvm->il->procinfo.name, "ProcIL");
	strcpy(mmvm->il->procinfo.version, "1.0");
	strcpy(mmvm->il->procinfo.description, "MMVM default ProcIL");
	mmvm->il->procinfo.depends = NULL;
	
	mmvm->il->fs.farptr = MMVM_FSILADDR;
	strcpy(mmvm->il->fsinfo.className, "FsIL");
	strcpy(mmvm->il->fsinfo.name, "FsIL");
	strcpy(mmvm->il->fsinfo.version, "1.0");
	strcpy(mmvm->il->fsinfo.description, "MMVM default FsIL");
	mmvm->il->fsinfo.depends = NULL;

	mmvm->swork = (LPWW_SRAMWork)&memory[SRAMWORK_SEG << 4];
	rom0 = (LPBYTE)&(mmvm->swork->_root_fs_entries[DIRENT_ROM0]);
	mmvm->cwfs.segoff.seg = SRAMWORK_SEG;
	mmvm->cwfs.segoff.off = rom0 - (LPBYTE)(mmvm->swork);

	mmvm->lcdseg = 0;

	GetModuleFileName(NULL, fontfile, MAX_PATH);
	_tsplitpath(fontfile, drive, dir, NULL, NULL);
	_tmakepath(fontfile, drive, dir, ELISA_FONTFILE, NULL);
	InitKanjiFont(fontfile);

	return TRUE;
}

void InitVM() {
	LPWW_IlibIL ilil;
	LPWW_ProcIL procil;
	LPWW_FsIL fsil;
	LPWW_ILInfo ilinfo, procinfo, fsinfo;
	unsigned int tmp1;

	init_cpu();
    sregs[CS] = APP_CSEG;
    sregs[DS] = APP_CSEG; // ここでは何に設定しても同じ
    sregs[ES] = APP_CSEG; // ここでは何に設定しても同じ
    sregs[SS] = STACK_SEG;
	ip = 0;
	c_cs = SegToMemPtr(CS);
	c_ds = SegToMemPtr(DS);
	c_es = SegToMemPtr(ES);
	c_stack = c_ss = SegToMemPtr(SS);

	// set return address
	tmp1 = (WORD)(ReadWord(&wregs[SP]) - 2);
	PutMemW(c_stack, tmp1, 0x0010);
	tmp1 = (WORD)(tmp1 - 2);
	PutMemW(c_stack, tmp1, 0x0000);
	WriteWord(&wregs[SP], tmp1);

	ilil = (LPWW_IlibIL)&memory[FARPTR2ADDR(mmvm->il->il)];
	ilil->super.n_methods = 2;
	ilil->super.link_pos.farptr = GETFARPTR(mmvm->il->il);
	ilil->super._get_info.farptr = SEGOFF2FARPTR(GETSEG(mmvm->il->il), 0);
	ilil->_open.farptr =           SEGOFF2FARPTR(GETSEG(mmvm->il->il), 1);
	ilil->_open_system.farptr =    SEGOFF2FARPTR(GETSEG(mmvm->il->il), 2);
	ilinfo = (LPWW_ILInfo)&memory[GETSEG(mmvm->il->il) << 4 | 0x0100];
	memcpy((LPBYTE)ilinfo + sizeof(WW_ILInfo), &mmvm->il->ilinfo, sizeof(mmvm->il->ilinfo));
	ilinfo->className.farptr =   SEGOFF2FARPTR(GETSEG(mmvm->il->il), 0x0100 + sizeof(WW_ILInfo)     );
	ilinfo->name.farptr =        SEGOFF2FARPTR(GETSEG(mmvm->il->il), 0x0100 + sizeof(WW_ILInfo) + 32);
	ilinfo->version.farptr =     SEGOFF2FARPTR(GETSEG(mmvm->il->il), 0x0100 + sizeof(WW_ILInfo) + 64);
	ilinfo->description.farptr = SEGOFF2FARPTR(GETSEG(mmvm->il->il), 0x0100 + sizeof(WW_ILInfo) + 96);
	ilinfo->depends.farptr =     SEGOFF2FARPTR(0, 0);

	procil = (LPWW_ProcIL)&memory[FARPTR2ADDR(mmvm->il->proc)];
	procil->super.n_methods = 8;
	procil->super.link_pos.farptr = GETFARPTR(mmvm->il->proc);
	procil->super._get_info.farptr = SEGOFF2FARPTR(GETSEG(mmvm->il->proc), 0);
	procil->_load.farptr =           SEGOFF2FARPTR(GETSEG(mmvm->il->proc), 1);
	procil->_run.farptr =            SEGOFF2FARPTR(GETSEG(mmvm->il->proc), 2);
	procil->_exec.farptr =           SEGOFF2FARPTR(GETSEG(mmvm->il->proc), 3);
	procil->_exit.farptr =           SEGOFF2FARPTR(GETSEG(mmvm->il->proc), 4);
	procil->_yield.farptr =          SEGOFF2FARPTR(GETSEG(mmvm->il->proc), 5);
	procil->_suspend.farptr =        SEGOFF2FARPTR(GETSEG(mmvm->il->proc), 6);
	procil->_resume.farptr =         SEGOFF2FARPTR(GETSEG(mmvm->il->proc), 7);
	procil->_swap.farptr =           SEGOFF2FARPTR(GETSEG(mmvm->il->proc), 8);
	procinfo = (LPWW_ILInfo)&memory[GETSEG(mmvm->il->proc) << 4 | 0x0100];
	memcpy((LPBYTE)procinfo + sizeof(WW_ILInfo), &mmvm->il->procinfo, sizeof(mmvm->il->procinfo));
	procinfo->className.farptr =   SEGOFF2FARPTR(GETSEG(mmvm->il->proc), 0x0100 + sizeof(WW_ILInfo)     );
	procinfo->name.farptr =        SEGOFF2FARPTR(GETSEG(mmvm->il->proc), 0x0100 + sizeof(WW_ILInfo) + 32);
	procinfo->version.farptr =     SEGOFF2FARPTR(GETSEG(mmvm->il->proc), 0x0100 + sizeof(WW_ILInfo) + 64);
	procinfo->description.farptr = SEGOFF2FARPTR(GETSEG(mmvm->il->proc), 0x0100 + sizeof(WW_ILInfo) + 96);
	procinfo->depends.farptr =     SEGOFF2FARPTR(0, 0);

	fsil = (LPWW_FsIL)&memory[FARPTR2ADDR(mmvm->il->fs)];
	fsil->super.link_pos.farptr = GETFARPTR(mmvm->il->il);
	fsil->super.n_methods = 18;
	fsil->super._get_info.farptr = SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 0);
	fsil->_entries.farptr =        SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 1);
	fsil->_n_entries.farptr =      SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 2);
	fsil->_getent.farptr =         SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 3);
	fsil->_findent.farptr =        SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 4);
	fsil->_mmap.farptr =           SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 5);
	fsil->_open.farptr =           SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 6);
	fsil->_close.farptr =          SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 7);
	fsil->_read.farptr =           SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 8);
	fsil->_write.farptr =          SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 9);
	fsil->_lseek.farptr =          SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 10);
	fsil->_chmod.farptr =          SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 11);
	fsil->_freeze.farptr =         SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 12);
	fsil->_melt.farptr =           SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 13);
	fsil->_creat.farptr =          SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 14);
	fsil->_unlink.farptr =         SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 15);
	fsil->_newfs.farptr =          SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 16);
	fsil->_defrag.farptr =         SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 17);
	fsil->_space.farptr =          SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 18);
	fsinfo = (LPWW_ILInfo)&memory[GETSEG(mmvm->il->fs) << 4 | 0x0100];
	memcpy((LPBYTE)fsinfo + sizeof(WW_ILInfo), &mmvm->il->fsinfo, sizeof(mmvm->il->fsinfo));
	fsinfo->className.farptr =   SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 0x0100 + sizeof(WW_ILInfo)     );
	fsinfo->name.farptr =        SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 0x0100 + sizeof(WW_ILInfo) + 32);
	fsinfo->version.farptr =     SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 0x0100 + sizeof(WW_ILInfo) + 64);
	fsinfo->description.farptr = SEGOFF2FARPTR(GETSEG(mmvm->il->fs), 0x0100 + sizeof(WW_ILInfo) + 96);
	fsinfo->depends.farptr =     SEGOFF2FARPTR(0, 0);

	mmvm->lcdseg = 0;

	mmvm->appinfo->loadended = FALSE;
	mmvm->appinfo->maincalled = FALSE;
}

void UpdateVM() {
	LPWW_PCB pcb;
	unsigned int tmp1;
	WORD argptr;
	WORD argc;
	WORD args[256];
	int i;
	BOOL quote;

	if(sregs[CS] == GETSEG(mmvm->il->il)) {
		HandleIlibIL();
		return;
	}
	if(sregs[CS] == GETSEG(mmvm->il->proc)) {
		HandleProcIL();
		return;
	}
	if(sregs[CS] == GETSEG(mmvm->il->fs)) {
		HandleFsIL(mmvm);
		return;
	}

	if(sregs[CS] == 0x0010 && ip == 0x0000) { // if load routine returned
		mmvm->appinfo->loadended = TRUE;
		ip = wregs[AX];
		sregs[CS] = wregs[DX];
	    c_cs = SegToMemPtr(CS);
		c_ds = SegToMemPtr(DS);
		c_es = SegToMemPtr(ES);
		c_stack = c_ss = SegToMemPtr(SS);

		pcb = (LPWW_PCB)SegToMemPtr(ES);
		// init dseg_start
		pcb->pid = 1;
		pcb->ppid = 0;
		pcb->pcbid = 1;
		pcb->ppcbid = 0;
		pcb->cwfs.farptr = GETFARPTR(mmvm->cwfs);
		pcb->ilib.farptr = GETFARPTR(mmvm->il->il);
		pcb->proc.farptr = GETFARPTR(mmvm->il->proc);
		pcb->resource.farptr = (sregs[CS] << 16) | mmvm->appinfo->fent.resource;
		strcpy(pcb->currentdir, "/rom0");

		argptr = pcb->argv;
		argc = 0;
		args[0] = argptr;
		quote = FALSE;
		for(i = 0; i < (int)strlen(mmvm->appinfo->commandline) + 1; i++) {
			if(mmvm->appinfo->commandline[i] == TEXT('\\')) {
				if(i < (int)strlen(mmvm->appinfo->commandline)) {
					i++;
					memory[(FIRST_DSEG << 4) + argptr++] = mmvm->appinfo->commandline[i];
				}
			} else if(_istspace(mmvm->appinfo->commandline[i]) && !quote) {
				argc++;
				memory[(FIRST_DSEG << 4) + argptr++] = TEXT('\0');
				args[argc] = argptr;
				while(isspace(mmvm->appinfo->commandline[i]))
					i++;
				if(!mmvm->appinfo->commandline[i])
					argc--;
				i--;
			} else if(quote && (mmvm->appinfo->commandline[i] == TEXT('\"') || mmvm->appinfo->commandline[i] == TEXT('\''))) {
				quote = FALSE;
			} else if(!quote && (mmvm->appinfo->commandline[i] == TEXT('\"') || mmvm->appinfo->commandline[i] == TEXT('\''))) {
				quote = TRUE;
			} else {
				memory[(FIRST_DSEG << 4) + argptr++] = mmvm->appinfo->commandline[i];
			}
		}
		argc++;

		// push argv
		tmp1 = (WORD)(ReadWord(&wregs[SP]) - 2);
		PutMemW(c_stack, tmp1, argptr);
		pcb->argv = argptr;

		// set argv
		for(i = 0; i < argc; i++) {
			memory[(FIRST_DSEG << 4) + argptr++] =  args[i]       & 0xff;
			memory[(FIRST_DSEG << 4) + argptr++] = (args[i] >> 8) & 0xff;
		}

		// push argc
		tmp1 = (WORD)(tmp1 - 2);
		PutMemW(c_stack, tmp1, argc);

		// set _heap address
		pcb->heap = argptr; 

		// push return address
		tmp1 = (WORD)(tmp1 - 2);
		PutMemW(c_stack, tmp1, 0x0010);
		tmp1 = (WORD)(tmp1 - 2);
		PutMemW(c_stack, tmp1, 0x0000);
		WriteWord(&wregs[SP], tmp1);

		memcpy(&memory[FINAL_DSEG << 4], &memory[FIRST_DSEG << 4], 0x10000);
		sregs[DS] = FINAL_DSEG; // どうせ run ルーチンで変更されるけど
		sregs[ES] = FINAL_DSEG; // どうせ run ルーチンで変更されるけど
		c_ds = SegToMemPtr(DS);
		c_es = SegToMemPtr(ES);

		InitFilesys(mmvm);
	}

	// main が呼ばれる直前に無理矢理 DS、ES を 0x2000 にする
	if(mmvm->appinfo->loadended && !mmvm->appinfo->maincalled && *(c_cs + ip) == 0xe8) {
		sregs[DS] = FINAL_DSEG;
		sregs[ES] = FINAL_DSEG;
		c_ds = SegToMemPtr(DS);
		c_es = SegToMemPtr(ES);
		mmvm->appinfo->maincalled = TRUE;
	}

}


void OpenDlgAndRun() {
	OPENFILENAME ofn;
	TCHAR filename[MAX_PATH + 1];
	TCHAR wndName[1024];

	filename[0] = TEXT('\0');
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = wonw32ctx->hWnd;
	ofn.lpstrFilter = TEXT("WonderWitch 転送形式ファイル (*.fx)\0*.fx\0すべてのファイル (*.*)\0*.*\0\0");
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	ofn.Flags = OFN_FILEMUSTEXIST;
	if(GetOpenFileName(&ofn)) {
		if(wonw32ctx->running) {
			StopExecution(5000);
		}

		wonw32ctx->loaded = LoadFXFile(filename, APP_CSEG, &(mmvm->appinfo->fent));

		if(!wonw32ctx->loaded) {
			SetWindowText(wonw32ctx->hWnd, THIS_APP_TITLE);
			MessageBox(wonw32ctx->hWnd, "ファイルの読み込みに失敗しました", NULL, MB_OK);
			wonw32ctx->loaded = FALSE;
			return;
		}

		wsprintf(wndName, "%s - %s", (LPCTSTR)mmvm->appinfo->fent.info, THIS_APP_TITLE);
		SetWindowText(wonw32ctx->hWnd, wndName);

		sprintf(mmvm->appinfo->commandline, "%s ", mmvm->appinfo->fent.name);
		if(mmconfig->queryarg) {
			DialogBoxParam(wonw32ctx->hInst, MAKEINTRESOURCE(IDD_ARGUMENT), wonw32ctx->hWnd, 
				(DLGPROC)GetArgument, (LPARAM)mmvm->appinfo->commandline);
		} else {
			sprintf(mmvm->appinfo->commandline, "%s %s", mmvm->appinfo->fent.name, mmconfig->argument);
		}
		RunWWApp();
	}

	return;
}

void OpenAndRun(LPCTSTR filename, LPCTSTR cmdline) {
	TCHAR wndName[1024];

	if(wonw32ctx->running) {
		StopExecution(5000);
	}

	wonw32ctx->loaded = LoadFXFile(filename, APP_CSEG, &(mmvm->appinfo->fent));

	if(!wonw32ctx->loaded) {
		SetWindowText(wonw32ctx->hWnd, THIS_APP_TITLE);
		MessageBox(wonw32ctx->hWnd, "ファイルの読み込みに失敗しました", NULL, MB_OK);
		wonw32ctx->loaded = FALSE;
		return;
	}

	wsprintf(wndName, "%s - %s", (LPCTSTR)mmvm->appinfo->fent.info, THIS_APP_TITLE);
	SetWindowText(wonw32ctx->hWnd, wndName);

	sprintf(mmvm->appinfo->commandline, "%s %s", mmvm->appinfo->fent.name, cmdline);

	RunWWApp();

	return;
}

void RunWWApp() {
	wonw32ctx->cputhread = (HANDLE)_beginthread(StartThread, 0, NULL);
}

BOOL LoadFXFile(LPCTSTR filename, WORD seg, WW_FENT_T *fentp) {
	LPBYTE b;
	int c;
	FILE *fp;
	int i;

	b = (LPBYTE)fentp;
	fp = fopen(filename, "rb");
	if(!fp)
		goto error;
	for(i = 0; i < 64; i++) {
		if((c = fgetc(fp)) == EOF)
			goto error;
		b[i] = (BYTE)(c & 0xff);
		if((i == 0) && (b[i] != 0x23)) goto error;
		if((i == 1) && (b[i] != 0x21)) goto error;
		if((i == 2) && (b[i] != 0x77)) goto error;
		if((i == 3) && (b[i] != 0x73)) goto error;
	}
	for(i = 0; i < 64; i++) {
		if((c = fgetc(fp)) == EOF)
			goto error;
		b[i] = (BYTE)(c & 0xff);
	}

	i = seg << 4;
	while((c = fgetc(fp)) != EOF)
		memory[i++] = (BYTE)(c & 0xff);

	return TRUE;

error:
	if(fp)
		fclose(fp);
	return FALSE;
}

static LRESULT CALLBACK GetArgument( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	static LPTSTR arg;
	HWND argwnd;
	HWND ok;

	switch( message )
	{
		case WM_INITDIALOG:
			arg = (LPTSTR)lParam;
			argwnd = GetDlgItem(hDlg, IDC_ARGUMENT);
			SetWindowText(argwnd, arg);
			SendMessage(argwnd, EM_SETSEL, -1, 0);
			return TRUE;

		case WM_COMMAND:
			argwnd = GetDlgItem(hDlg, IDC_ARGUMENT);
			GetWindowText(argwnd, arg, 1024);
			if( LOWORD(wParam) == IDOK)  {
				if(lstrlen(arg) > 0) 
				{
					EndDialog(hDlg, 0);
					return TRUE;
				}
			} else if(LOWORD(wParam) == IDC_ARGUMENT && HIWORD(wParam) == EN_UPDATE) {
				ok = GetDlgItem(hDlg, IDOK);
				if(lstrlen(arg) > 0) {
					EnableWindow(ok, TRUE);
				} else {
					EnableWindow(ok, FALSE);
				}
				return TRUE;
			}
			break;
	}
    return FALSE;
}

void Restart() {
	if(wonw32ctx->running) {
		StopExecution(5000);
	}
	RunWWApp();
}

void StopExecution(DWORD wait) {
	wonw32ctx->running = FALSE;
	PulseEvent(wonw32ctx->syncevent);
	TerminateThread(wonw32ctx->cputhread, 0);
	if(WaitForSingleObject(wonw32ctx->cputhread, wait) != WAIT_OBJECT_0) {
		MessageBox(wonw32ctx->hWnd, TEXT("MiracleMage 内部エラーです\nMiracleMage を再起動してください"), NULL, MB_OK);
	}
}

void StartThread(LPVOID arg) {
	wonw32ctx->running = TRUE;
	InitVM();

	InvalidateRect(wonw32ctx->hWnd, NULL, FALSE);
	UpdateWindow(wonw32ctx->hWnd);

	while(wonw32ctx->running) {
		execute();
		UpdateVM();
	}

	CloseFilesys();
	mmvm->lcdseg = 0;

	InvalidateRect(wonw32ctx->hWnd, NULL, FALSE);
	UpdateWindow(wonw32ctx->hWnd);

	_endthread();
}


static void HandleIlibIL() {
	WW_FARPTR ilname;
	WW_FARPTR ilbuf;

	switch(ip) {
	case 0: // ILInfo far *super._get_info();
		wregs[DX] = GETSEG(mmvm->il->il);
		wregs[AX] = 0x0100;
		break;

	case 1: // int _open(char far *ilname, IL far *ilbuf);
	case 2: // int _open_system(char far *ilname, IL far *ilbuf);
		ilname = *((WW_FARPTR *)GetArgAddr(0));
		ilbuf = *((WW_FARPTR *)GetArgAddr(4));

		memory[FARPTR2ADDR(ilbuf)    ] = 0;
		memory[FARPTR2ADDR(ilbuf) + 1] = 0;
		memory[FARPTR2ADDR(ilbuf) + 2] = 0;
		memory[FARPTR2ADDR(ilbuf) + 3] = 0;

		wregs[AX] = 0xffff;
		break;

	default:
		wregs[AX] = 0xffff;
		break;
	}

	ILReturn();
}

static void HandleProcIL() {
	switch(ip) {
	case 0: // ILInfo far *super._get_info();
		wregs[DX] = GETSEG(mmvm->il->proc);
		wregs[AX] = 0x0100;
		break;

	default:
		wregs[AX] = 0xffff;
		break;
	}

	ILReturn();
}

void ILReturn() {
	unsigned tmp = ReadWord(&wregs[SP]);
    ip = GetMemW(c_stack,tmp);
    tmp = (WORD)(tmp+2);
    sregs[CS] = GetMemW(c_stack,tmp);
    c_cs = SegToMemPtr(CS);
    tmp += 2;
    WriteWord(&wregs[SP],tmp);
}

LPBYTE GetArgAddr(int pos) {
	return c_stack + ReadWord(&wregs[SP]) + 4 + pos;
}

