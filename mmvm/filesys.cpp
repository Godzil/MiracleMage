#ifdef _DEBUG
#include "../mmage/mmage.h" // for ShowError()
#endif

#include "../mmage/preference.h"
#include "filesys.h"
#include <stdio.h>
#include <stdlib.h>
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

extern LPMMVM mmvm;

typedef struct {
	LPTSTR rom0names[ROM0FS_NUM_ENTRIES];
	LPTSTR ram0names[RAM0FS_NUM_ENTRIES];
	HANDLE fd[OPEN_MAX];
	TCHAR rom0dirname[MAX_PATH];
	TCHAR ram0dirname[MAX_PATH];
} FILETABLE, *LPFILETABLE;

static TCHAR rom0names[ROM0FS_NUM_ENTRIES][MAX_PATH + 1];
static TCHAR ram0names[RAM0FS_NUM_ENTRIES][MAX_PATH + 1];

static FILETABLE ftable_struct;
static LPFILETABLE ftable = &ftable_struct;

static void InitFent(WW_FS fs);
static WW_FARPTR wwfs_entries(WW_FS fs);
static WW_INT wwfs_n_entries(WW_FS fs);
static WW_INT wwfs_getent(WW_FS fs, WW_INT n, WW_FENT_T *fep);
static WW_INT wwfs_findent(WW_FS fs, char *fname, WW_FENT_T *fep);
static WW_FARPTR wwfs_mmap(WW_FS fs, char *fname);
static WW_INT wwfs_open(WW_FS fs, char *fname, WW_INT mode, WW_INT perms);
static WW_INT wwfs_close(WW_INT fd);
static WW_INT wwfs_read(WW_INT fd, char *buf, WW_INT len);
static WW_INT wwfs_write(WW_INT fd, char *buf, WW_INT len);
static WW_LONG wwfs_lseek(WW_INT fd, WW_LONG offset, WW_INT origin);
static WW_INT wwfs_chmod(WW_FS fs, char *fname, WW_INT mode);
static WW_INT wwfs_freeze(WW_FS fs, char *fname);
static WW_INT wwfs_melt(WW_FS fs, char *fname);
static WW_INT wwfs_creat(WW_FS fs, WW_FENT_T *fep);
static WW_INT wwfs_unlink(WW_FS fs, char *fname);
static WW_INT wwfs_newfs(WW_FS fs);
static WW_INT wwfs_defrag(WW_FS fs);
static WW_LONG wwfs_space(WW_FS fs);

// fs からファイル名 fname のエントリを取得(内部関数)
static WW_FENT_T *findent(WW_FS fs, char *fname, WW_INT *n);

void InitFilesys(LPMMVM mmvm) {
	HANDLE ffile;
	TCHAR ffind[MAX_PATH + 1];
	TCHAR w32path[MAX_PATH + 1];
    WIN32_FIND_DATA fdata;
 	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR];
	int i;

	// initialize file entries
	for(i = 0; i < ROOTFS_NUM_ENTRIES; i++) {
		InitFent(&(mmvm->swork->_root_fs_entries[i]));
	}
	for(i = 0; i < ROM0FS_NUM_ENTRIES; i++) {
		InitFent(&(mmvm->swork->_rom0_fs_entries[i]));
		ftable->rom0names[i] = &rom0names[i][0];
	}
	for(i = 0; i < RAM0FS_NUM_ENTRIES; i++) {
		InitFent(&(mmvm->swork->_ram0_fs_entries[i]));
		ftable->ram0names[i] = &ram0names[i][0];
	}

	// setup root fs
	for(i = 0; i < DIRENT_NUM; i++) {
		mmvm->swork->_root_fs_entries[i].mode = FMODE_DIR|FMODE_R|FMODE_W|FMODE_X;
		mmvm->swork->_root_fs_entries[i].handler.il = GETFARPTR(mmvm->il->fs);
	}
	strcpy(mmvm->swork->_root_fs_entries[DIRENT_ROOT].name, ".");
	mmvm->swork->_root_fs_entries[DIRENT_ROOT].count = DIRENT_NUM;
	strcpy(mmvm->swork->_root_fs_entries[DIRENT_KERN].name, "kern");
	mmvm->swork->_root_fs_entries[DIRENT_KERN].count = 0;
	strcpy(mmvm->swork->_root_fs_entries[DIRENT_ROM0].name, "rom0");
	mmvm->swork->_root_fs_entries[DIRENT_ROM0].count = ROM0FS_NUM_ENTRIES;
	strcpy(mmvm->swork->_root_fs_entries[DIRENT_RAM0].name, "ram0");
	mmvm->swork->_root_fs_entries[DIRENT_RAM0].count = RAM0FS_NUM_ENTRIES;

	// initialize file handles
	for(i = 0; i < OPEN_MAX; i++) {
		if(ftable->fd[i] != INVALID_HANDLE_VALUE) {
			CloseHandle(ftable->fd[i]);
			ftable->fd[i] = INVALID_HANDLE_VALUE;
		}
		mmvm->swork->_openfiles[i].omode = 0;
	}

	// initialize /rom0 file entries
	i = 0;
	if(_tcslen(mmconfig->rom0dir)) {
		_stprintf(ffind, TEXT("%s\\*"), mmconfig->rom0dir);
		_tcscpy(ftable->rom0dirname, mmconfig->rom0dir);
	} else {
		_stprintf(ffind, TEXT("%s\\rom0\\*"), mmconfig->workingdir);
		_stprintf(ftable->rom0dirname, TEXT("%s\\rom0\\"), mmconfig->workingdir);
	}
	_tsplitpath(ffind, drive, dir, NULL, NULL);
	ffile = FindFirstFile(ffind, &fdata);
	if(ffile != INVALID_HANDLE_VALUE) {
		do {
			if(!(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				strncpy(mmvm->swork->_rom0_fs_entries[i].name, fdata.cFileName, MAXFNAME);
				mmvm->swork->_rom0_fs_entries[i].count = 0;
				mmvm->swork->_rom0_fs_entries[i].mode = FMODE_R;
				mmvm->swork->_rom0_fs_entries[i].count = (WW_INT)fdata.nFileSizeLow / 128;
				mmvm->swork->_rom0_fs_entries[i].len = fdata.nFileSizeLow;
				_tmakepath(w32path, drive, dir, fdata.cFileName, NULL);
				_tcscpy(ftable->rom0names[i], w32path);
				i++;
				if(i > ROM0FS_NUM_ENTRIES)
					break;
			}
		} while(FindNextFile(ffile, &fdata));
		FindClose(ffile);
	}

	// initialize /ram0 file entries
	i = 0;
	if(_tcslen(mmconfig->ram0dir)) {
		_stprintf(ffind, TEXT("%s\\*"), mmconfig->ram0dir);
		_tcscpy(ftable->ram0dirname, mmconfig->ram0dir);
	} else {
		_stprintf(ffind, TEXT("%s\\ram0\\*"), mmconfig->workingdir);
		_stprintf(ftable->ram0dirname, TEXT("%s\\ram0\\"), mmconfig->workingdir);
	}
	_tsplitpath(ffind, drive, dir, NULL, NULL);
	ffile = FindFirstFile(ffind, &fdata);
	if(ffile != INVALID_HANDLE_VALUE) {
		do {
			if(!(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				strncpy(mmvm->swork->_ram0_fs_entries[i].name, fdata.cFileName, MAXFNAME);
				mmvm->swork->_ram0_fs_entries[i].count = 0;
				mmvm->swork->_ram0_fs_entries[i].mode = FMODE_R|FMODE_W;
				mmvm->swork->_ram0_fs_entries[i].count = (WW_INT)fdata.nFileSizeLow / 128;
				mmvm->swork->_ram0_fs_entries[i].len = fdata.nFileSizeLow;
				_tmakepath(w32path, drive, dir, fdata.cFileName, NULL);
				_tcscpy(ftable->ram0names[i], w32path);
				i++;
				if(i > RAM0FS_NUM_ENTRIES)
					break;
			}
		} while(FindNextFile(ffile, &fdata));
		FindClose(ffile);
	}

}

void CloseFilesys() {
	int i;

	for(i = 0; i < OPEN_MAX; i++) {
		if(ftable->fd[i] != INVALID_HANDLE_VALUE) {
			CloseHandle(ftable->fd[i]);
			ftable->fd[i] = INVALID_HANDLE_VALUE;
		}
		mmvm->swork->_openfiles[i].omode = 0;
	}
}

void HandleFsIL(LPMMVM mmvm) {
	WW_FARPTR p, ret;
	WW_INT n, mode, perms, fd, len, origin;
	WW_FS fs, fep;
	char *fname, *buf;
	WW_LONG offset;

	switch(ip) {
 	case 0: // ILInfo far *super._get_info();
		wregs[DX] = GETSEG(mmvm->il->fs);
		wregs[AX] = 0x0100;
		break;

	case 1: // fent_t far *(far *_entries)(FS fs);
		p = *((WW_FARPTR *)GetArgAddr(0));
		fs = (WW_FS)&memory[FARPTR2ADDR(p)];
		ret = wwfs_entries(fs);
		wregs[AX] = ret.segoff.off;
		wregs[DX] = ret.segoff.seg;

		break;

	case 2: //  int (far *_n_entries)(FS fs);
		p = *((WW_FARPTR *)GetArgAddr(0));
		fs = (WW_FS)&memory[FARPTR2ADDR(p)];
		wregs[AX] = wwfs_n_entries(fs);

		break;

    case 3: // int (far *_getent)(FS fs, int n, fent_t far *fep);
		p = *((WW_FARPTR *)GetArgAddr(0));
		fs = (WW_FS)&memory[FARPTR2ADDR(p)];
		n = *((WW_INT *)GetArgAddr(4));
		p = *((WW_FARPTR *)GetArgAddr(6));
		fep = (WW_FENT_T *)&memory[FARPTR2ADDR(p)];
		wregs[AX] = wwfs_getent(fs, n, fep);

		break;

    case 4: // int (far *_findent)(FS fs, char far *fname, fent_t far *fep);
		p = *((WW_FARPTR *)GetArgAddr(0));
		fs = (WW_FS)&memory[FARPTR2ADDR(p)];
		p = *((WW_FARPTR *)GetArgAddr(4));
		fname = (char *)&memory[FARPTR2ADDR(p)];
		p = *((WW_FARPTR *)GetArgAddr(8));
		fep = (WW_FENT_T *)&memory[FARPTR2ADDR(p)];
		wregs[AX] = wwfs_findent(fs, fname, fep);

		break;

    case 5: // void far *(far *_mmap)(FS fs, char far *fname);
		p = *((WW_FARPTR *)GetArgAddr(0));
		fs = (WW_FS)&memory[FARPTR2ADDR(p)];
		p = *((WW_FARPTR *)GetArgAddr(4));
		fname = (char *)&memory[FARPTR2ADDR(p)];
		ret = wwfs_mmap(fs, fname);
		wregs[AX] = ret.segoff.off;
		wregs[DX] = ret.segoff.seg;
		
		break;

    case 6: // int (far *_open)(FS fs, char far *fname, int mode, int perms);
		p = *((WW_FARPTR *)GetArgAddr(0));
		fs = (WW_FS)&memory[FARPTR2ADDR(p)];
		p = *((WW_FARPTR *)GetArgAddr(4));
		fname = (char *)&memory[FARPTR2ADDR(p)];
		mode = *((WW_INT *)GetArgAddr(8));
		perms = *((WW_INT *)GetArgAddr(10));
		wregs[AX] = wwfs_open(fs, fname, mode, perms);

		break;

    case 7:  // int (far *_close)(int fd);
		fd = *((WW_INT *)GetArgAddr(0));
		wregs[AX] = wwfs_close(fd);

		break;

    case 8: // int (far *_read)(int fd, char far *buf, int len);
		fd = *((WW_INT *)GetArgAddr(0));
		p = *((WW_FARPTR *)GetArgAddr(2));
		buf = (char *)&memory[FARPTR2ADDR(p)];
		len = *((WW_INT *)GetArgAddr(6));
		wregs[AX] = wwfs_read(fd, buf, len);

		break;

    case 9: // int (far *_write)(int fd, char far *buf, int len);
		fd = *((WW_INT *)GetArgAddr(0));
		p = *((WW_FARPTR *)GetArgAddr(2));
		buf = (char *)&memory[FARPTR2ADDR(p)];
		len = *((WW_INT *)GetArgAddr(6));
		wregs[AX] = wwfs_write(fd, buf, len);

		break;

    case 10: // long (far *_lseek)(int fd, long offset, int origin);
		fd = *((WW_INT *)GetArgAddr(0));
		offset = *((WW_LONG *)GetArgAddr(2));
		origin = *((WW_INT *)GetArgAddr(6));
		ret.farptr = wwfs_lseek(fd, offset, origin);
		wregs[AX] = ret.segoff.off;
		wregs[DX] = ret.segoff.seg;

		break;

    case 11: // int (far *_chmod)(FS fs, char far *fname, int mode);
		p = *((WW_FARPTR *)GetArgAddr(0));
		fs = (WW_FS)&memory[FARPTR2ADDR(p)];
		p = *((WW_FARPTR *)GetArgAddr(4));
		fname = (char *)&memory[FARPTR2ADDR(p)];
		mode = *((WW_INT *)GetArgAddr(8));
		wregs[AX] = wwfs_chmod(fs, fname, mode);

		break;

    case 12: // int (far *_freeze)(FS fs, char far *fname);
		p = *((WW_FARPTR *)GetArgAddr(0));
		fs = (WW_FS)&memory[FARPTR2ADDR(p)];
		p = *((WW_FARPTR *)GetArgAddr(4));
		fname = (char *)&memory[FARPTR2ADDR(p)];
		wregs[AX] = wwfs_freeze(fs, fname);

		break;

    case 13: // int (far *_melt)(FS fs, char far *fname);
		p = *((WW_FARPTR *)GetArgAddr(0));
		fs = (WW_FS)&memory[FARPTR2ADDR(p)];
		p = *((WW_FARPTR *)GetArgAddr(4));
		fname = (char *)&memory[FARPTR2ADDR(p)];
		wregs[AX] = wwfs_melt(fs, fname);

		break;

    case 14: // int (far *_creat)(FS fs, fent_t far *fep);
		p = *((WW_FARPTR *)GetArgAddr(0));
		fs = (WW_FS)&memory[FARPTR2ADDR(p)];
		p = *((WW_FARPTR *)GetArgAddr(4));
		fep = (WW_FENT_T *)&memory[FARPTR2ADDR(p)];
		wregs[AX] = wwfs_creat(fs, fep);

		break;

    case 15: // int (far *_unlink)(FS fs, char *fname);
		p = *((WW_FARPTR *)GetArgAddr(0));
		fs = (WW_FS)&memory[FARPTR2ADDR(p)];
		p = *((WW_FARPTR *)GetArgAddr(4));
		fname = (char *)&memory[FARPTR2ADDR(p)];
		wregs[AX] = wwfs_unlink(fs, fname);

		break;

    case 16: // int (far *_newfs)(FS fs);
		p = *((WW_FARPTR *)GetArgAddr(0));
		fs = (WW_FS)&memory[FARPTR2ADDR(p)];
		wregs[AX] = wwfs_newfs(fs);

		break;

    case 17: // int (far *_defrag)(FS fs);
		p = *((WW_FARPTR *)GetArgAddr(0));
		fs = (WW_FS)&memory[FARPTR2ADDR(p)];
		wregs[AX] = wwfs_defrag(fs);

		break;

    case 18: // unsigned long (far *_space)(FS fs);
		p = *((WW_FARPTR *)GetArgAddr(0));
		fs = (WW_FS)&memory[FARPTR2ADDR(p)];
		ret.farptr = wwfs_space(fs);
		wregs[AX] = ret.segoff.off;
		wregs[DX] = ret.segoff.seg;

		break;

	default:
		break;
	}

	ILReturn();
}

static WW_FARPTR wwfs_entries(WW_FS fs) {
	WW_FARPTR ret;

	if(fs->mode & FMODE_DIR && !strncmp(fs->name, ".", MAXPATHLEN)) {
		ret.segoff.seg = SRAMWORK_SEG;
		ret.segoff.off = (LPBYTE)(mmvm->swork->_root_fs_entries) - (LPBYTE)(mmvm->swork);
	} else if(fs->mode & FMODE_DIR && !strncmp(fs->name, "rom0", MAXPATHLEN)) {
		ret.segoff.seg = SRAMWORK_SEG;
		ret.segoff.off = (LPBYTE)(mmvm->swork->_rom0_fs_entries) - (LPBYTE)(mmvm->swork);
	} else if(fs->mode & FMODE_DIR && !strncmp(fs->name, "ram0", MAXPATHLEN)) {
		ret.segoff.seg = SRAMWORK_SEG;
		ret.segoff.off = (LPBYTE)(mmvm->swork->_ram0_fs_entries) - (LPBYTE)(mmvm->swork);
	} else {
		ret.farptr = NULL;
	}

	return ret;
}

static WW_INT wwfs_n_entries(WW_FS fs) {
	WW_INT ret;

	if(fs->mode & FMODE_DIR && !strncmp(fs->name, ".", MAXPATHLEN)) {
		ret = ROOTFS_NUM_ENTRIES;
	} else if(fs->mode & FMODE_DIR && !strncmp(fs->name, "rom0", MAXPATHLEN)) {
		ret = ROM0FS_NUM_ENTRIES;
	} else if(fs->mode & FMODE_DIR && !strncmp(fs->name, "ram0", MAXPATHLEN)) {
		ret = RAM0FS_NUM_ENTRIES;
	} else {
		ret = E_FS_FILE_NOT_FOUND;
	}

	return ret;
}

static WW_INT wwfs_getent(WW_FS fs, WW_INT n, WW_FENT_T *fep) {
	WW_INT ret;

	if(fs->mode & FMODE_DIR && !strncmp(fs->name, ".", MAXPATHLEN)) {
		if(n >= 0 && n < ROM0FS_NUM_ENTRIES) {
			memcpy(fep, &(mmvm->swork->_root_fs_entries[n]), sizeof(WW_FENT_T));
			ret = E_FS_SUCCESS;
		} else {
			ret = E_FS_OUT_OF_BOUNDS;
		}
	} else if(fs->mode & FMODE_DIR && !strncmp(fs->name, "rom0", MAXPATHLEN)) {
		if(n >= 0 && n < ROM0FS_NUM_ENTRIES) {
			memcpy(fep, &(mmvm->swork->_rom0_fs_entries[n]), sizeof(WW_FENT_T));
			ret = E_FS_SUCCESS;
		} else {
			ret = E_FS_OUT_OF_BOUNDS;
		}
	} else if(fs->mode & FMODE_DIR && !strncmp(fs->name, "ram0", MAXPATHLEN)) {
		if(n >= 0 && n < RAM0FS_NUM_ENTRIES) {
			memcpy(fep, &(mmvm->swork->_ram0_fs_entries[n]), sizeof(WW_FENT_T));
			ret = E_FS_SUCCESS;
		} else {
			ret = E_FS_OUT_OF_BOUNDS;
		}
	} else {
		ret = E_FS_FILE_NOT_FOUND;
	}

	return ret;
}

static WW_INT wwfs_findent(WW_FS fs, char *fname, WW_FENT_T *fep) {
	WW_INT ret;
	WW_FENT_T *fents;
	WW_INT n;

	fents = findent(fs, fname, &n);
	if(fents) {
		memcpy(fep, &fents, sizeof(WW_FENT_T));
		ret = E_FS_SUCCESS;
	} else {
		ret = E_FS_FILE_NOT_FOUND;
	}

	return ret;
}

static WW_FARPTR wwfs_mmap(WW_FS fs, char *fname) {
	WW_FARPTR ret;

	// not supported
	ret.farptr = 0;

	return ret;
}

static WW_INT wwfs_open(WW_FS fs, char *fname, WW_INT mode, WW_INT perms) {
	WW_INT ret;
	WW_FENT_T *fents;
	LPTSTR *fntable;
	DWORD w32mode;
	LPTSTR w32fname;
	WW_INT nfent;
	WW_FENT_T newfile;

	switch(mode) {
	case FMODE_R:
		w32mode = GENERIC_READ;
		break;
	case FMODE_W:
		w32mode = GENERIC_WRITE;
		break;
	case (FMODE_R|FMODE_W):
		w32mode = (GENERIC_READ|GENERIC_WRITE);
		break;
	default:
		return E_FS_PERMISSION_DENIED;
		break;
	}

	if(fs->mode & FMODE_DIR && !strncmp(fs->name, "rom0", MAXPATHLEN)) {
		fntable = ftable->rom0names;
		if(mode & FMODE_W)
			return E_FS_PERMISSION_DENIED;
	} else if(fs->mode & FMODE_DIR && !strncmp(fs->name, "ram0", MAXPATHLEN)) {
		fntable = ftable->ram0names;
	} else {
		return E_FS_FILE_NOT_FOUND;
	}

	fents = findent(fs, fname, &nfent);
	if(!fents) {
		// ファイルが見つからなかったら新しいファイルを作る
		InitFent(&newfile);
		strncpy(newfile.name, fname, MAXPATHLEN);
		newfile.mode = mode;

		if(wwfs_creat(fs, &newfile) != E_FS_SUCCESS)
			return E_FS_FILE_NOT_FOUND;

		fents = findent(fs, fname, &nfent);
		if(!fents)
			return E_FS_FILE_NOT_FOUND;
	}

	w32fname = fntable[nfent];
	for(ret = 0; ret < OPEN_MAX; ret++) {
		if(ftable->fd[ret] == INVALID_HANDLE_VALUE)
			break;
	}

	if(ret == OPEN_MAX) {
		ret = E_FS_ERROR;
	} else {
		ftable->fd[ret] = CreateFile(w32fname, w32mode, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(ftable->fd[ret] == INVALID_HANDLE_VALUE) {
			ret = E_FS_PERMISSION_DENIED;
		} else {
			mmvm->swork->_openfiles[ret].fs = (WW_FS)mmvm->cwfs.farptr;
			mmvm->swork->_openfiles[ret].fentp.segoff.seg = SRAMWORK_SEG;
			mmvm->swork->_openfiles[ret].fentp.segoff.off = (LPBYTE)&fents - (LPBYTE)mmvm->swork;
			mmvm->swork->_openfiles[ret].omode = mode;
			mmvm->swork->_openfiles[ret].fpos = 0;
			mmvm->swork->_openfiles[ret].flen = 0;
			mmvm->swork->_openfiles[ret].floc.farptr = 0;
			mmvm->swork->_openfiles[ret].count = 0;
			mmvm->swork->_openfiles[ret].pcb = 1;
			mmvm->swork->_openfiles[ret].driver.farptr = 0;
		}
	}

	return ret;
}

static WW_INT wwfs_close(WW_INT fd) {
	WW_INT ret;

	if(!(mmvm->swork->_openfiles[fd].omode))
		return E_FS_ERROR;

	if(ftable->fd[fd] != INVALID_HANDLE_VALUE) {
		if(CloseHandle(ftable->fd[fd]))
			ret = E_FS_SUCCESS;
		else
			ret = E_FS_FILE_NOT_OPEN;
	} else {
		ret = E_FS_FILE_NOT_OPEN;
	}
	ftable->fd[fd] = INVALID_HANDLE_VALUE;
	mmvm->swork->_openfiles[fd].omode = 0;

	return ret;
}

static WW_INT wwfs_read(WW_INT fd, char *buf, WW_INT len) {
	WW_INT ret;
	DWORD nbytes;

	if(!(mmvm->swork->_openfiles[fd].omode))
		return E_FS_ERROR;

	if(!(mmvm->swork->_openfiles[fd].omode & FMODE_R))
		return E_FS_PERMISSION_DENIED;

	if(ReadFile(ftable->fd[fd], buf, len, &nbytes, NULL)) {
		ret = (WW_INT)nbytes;
	} else {
		ret = -1;
	}

	return ret;
}

static WW_INT wwfs_write(WW_INT fd, char *buf, WW_INT len) {
	WW_INT ret;
	DWORD nbytes;

	if(!(mmvm->swork->_openfiles[fd].omode))
		return E_FS_ERROR;

	if(!(mmvm->swork->_openfiles[fd].omode & FMODE_W))
		return E_FS_PERMISSION_DENIED;

	if(WriteFile(ftable->fd[fd], buf, len, &nbytes, NULL)) {
		ret = (WW_INT)nbytes;
	} else {
		ret = -1;
	}

	return ret;
}

static WW_LONG wwfs_lseek(WW_INT fd, WW_LONG offset, WW_INT origin) {
	WW_LONG ret;
	DWORD w32off, w32org;

	if(!(mmvm->swork->_openfiles[fd].omode))
		return E_FS_ERROR;

	switch(origin) {
	case 0:
		w32org = FILE_BEGIN;
		break;
	case 1:
		w32org = FILE_CURRENT;
		break;
	case 2:
		w32org = FILE_END;
		break;
	default:
		return E_FS_OUT_OF_BOUNDS;
		break;
	}

	if(ftable->fd[fd] == INVALID_HANDLE_VALUE) {
		ret = E_FS_FILE_NOT_OPEN;
	} else {
		w32off = SetFilePointer(ftable->fd[fd], offset, NULL, w32org);
		if(w32off != 0xFFFFFFFF)
			ret = w32off;
		else
			ret = E_FS_OUT_OF_BOUNDS;
	}

	return ret;

}

// not yet.
static WW_INT wwfs_chmod(WW_FS fs, char *fname, WW_INT mode) {
	return E_FS_ERROR;
}

static WW_INT wwfs_freeze(WW_FS fs, char *fname) {
	return E_FS_ERROR;
}

static WW_INT wwfs_melt(WW_FS fs, char *fname) {
	return E_FS_ERROR;
}

static WW_INT wwfs_creat(WW_FS fs, WW_FENT_T *fep) {
	WW_FENT_T *fent;
	WW_INT n;
	HANDLE newfile;
	int i;

	if(fs->mode & FMODE_DIR && strncmp(fs->name, "ram0", MAXPATHLEN))
		return E_FS_PERMISSION_DENIED;

	if(findent(fs, fep->name, &n))
		return E_FS_ERROR;

	fent = NULL;
	for(i = 0; i < RAM0FS_NUM_ENTRIES; i++) {
		if(mmvm->swork->_ram0_fs_entries[i].count == (WW_INT)-1) {
			fent = &(mmvm->swork->_ram0_fs_entries[i]);
			n = i;
			break;
		}
	}

	if(!fent)
		return E_FS_ERROR;

	strcat(ftable->ram0names[n], ftable->ram0dirname);
	strcat(ftable->ram0names[n], fep->name);

	newfile = CreateFile(ftable->ram0names[n], GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(newfile == INVALID_HANDLE_VALUE)
		return E_FS_ERROR;

	CloseHandle(newfile);
	memcpy(fent, fep, sizeof(WW_FENT_T));
	fent->count = 0;

	return E_FS_SUCCESS;
}

static WW_INT wwfs_unlink(WW_FS fs, char *fname) {
	WW_FENT_T *fent;
	WW_INT n;

	if(fs->mode & FMODE_DIR && strncmp(fs->name, "ram0", MAXPATHLEN))
		return E_FS_PERMISSION_DENIED;

	fent = findent(fs, fname, &n);
	if(!fent)
		return E_FS_FILE_NOT_FOUND;

	if(!DeleteFile(ftable->ram0names[n]))
		return E_FS_ERROR;

	fent->count = (WW_INT)-1;

	return E_FS_SUCCESS;
}

static WW_INT wwfs_newfs(WW_FS fs) {
	return E_FS_ERROR;
}

static WW_INT wwfs_defrag(WW_FS fs) {
	return E_FS_SUCCESS;
}

static WW_LONG wwfs_space(WW_FS fs) {
	return 0x7fffffff;
}

static WW_FENT_T *findent(WW_FS fs, char *fname, WW_INT *n) {
	int n_entries;
	WW_FENT_T *fents;
	int i;

	if(fs->mode & FMODE_DIR && !strncmp(fs->name, ".", MAXPATHLEN)) {
		n_entries = ROOTFS_NUM_ENTRIES;
		fents = mmvm->swork->_root_fs_entries;
	} else if(fs->mode & FMODE_DIR && !strncmp(fs->name, "rom0", MAXPATHLEN)) {
		n_entries = ROM0FS_NUM_ENTRIES;
		fents = mmvm->swork->_rom0_fs_entries;
	} else if(fs->mode & FMODE_DIR && !strncmp(fs->name, "ram0", MAXPATHLEN)) {
		n_entries = RAM0FS_NUM_ENTRIES;
		fents = mmvm->swork->_ram0_fs_entries;
	} else {
		return NULL;
	}

	for(i = 0; i < n_entries; i++) {
		if(fents[i].count != -1 && !strncmp(fname, fents[i].name, MAXPATHLEN)) {
			*n = i;
			return &fents[i];
		}
	}

	return NULL;
}

static void InitFent(WW_FS fent) {
	strcpy(fent->name, "");
	strcpy(fent->info, "");
	fent->loc = 0;
	fent->len = 0;
	fent->count = (WW_INT)-1;
	fent->mode = 0;
	fent->mtime = 0;
	fent->handler.appid = 0;
	fent->resource = 0xffffffff;
}

