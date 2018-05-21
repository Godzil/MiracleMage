#ifndef _WWSTRUCT_H
#define _WWSTRUCT_H

#include <windows.h>

#define MAX_WW_ARG 256

#define MAXFNAME 16
#define MAXPATHLEN 64
#define MAXFINFO 24

#define OPEN_MAX		16		/* max open fils per process */

#define BLOCKSZ			128		/* size of file block */

#define	FMODE_X			(0x0001)	/* executable */
#define	FMODE_W			(0x0002)	/* writable */
#define	FMODE_R			(0x0004)	/* readable */
#define	FMODE_MMAP		(0x0008)	/* disallow mmap */
#define	FMODE_STREAM	(0x0010)	/* StreamIL instance */
#define	FMODE_ILIB		(0x0020)	/* IL instance */
#define	FMODE_LINK		(0x0040)	/* symbolic link */
#define	FMODE_DIR		(0x0080)	/* directory */

#define E_FS_SUCCESS		0
#define E_FS_ERROR		0x8000
#define E_FS_FILE_NOT_FOUND	0x8001
#define E_FS_PERMISSION_DENIED	0x8002
#define E_FS_OUT_OF_BOUNDS	0x8003
#define E_FS_NO_SPACE_LEFT	0x8004
#define E_FS_FILE_NOT_OPEN	0x8005

typedef union {
	DWORD farptr;
	struct {
		WORD off;
		WORD seg;
	} segoff;
} WW_FARPTR;

#define FARPTR2ADDR(X) ((X.segoff.seg << 4) | X.segoff.off)
#define SEGOFF2FARPTR(SEG, OFF) ((SEG << 16 | OFF))
#define GETSEG(X) (X.segoff.seg)
#define GETOFF(X) (X.segoff.off)
#define GETFARPTR(X) (X.farptr)

typedef WORD WW_NEARPTR;
typedef WORD WW_SHORT; 
typedef WORD WW_INT;
typedef DWORD WW_LONG;

#pragma pack( push, beforewwstruct )
#pragma pack(1)

typedef struct {
/* from startup routine
__id		db	'TCC', 0
__pid		dw	?
__ppid		dw	?
__pcbid		dw	?
__ppcbid	dw	?
__ilib		dd	?
__proc		dd	?
__cwfs		dd	?
__currentdir	db	MAXPATHLEN dup (?)
__argv		dw	?
__resource	dd	?
__heap		dw	?
*/
	BYTE id[4];
	WW_INT pid;
	WW_INT ppid;
	WW_INT pcbid;
	WW_INT ppcbid;
	WW_FARPTR ilib;
	WW_FARPTR proc;
	WW_FARPTR cwfs;
	char currentdir[MAXPATHLEN];
	WW_NEARPTR argv;
	WW_FARPTR resource;
	WW_NEARPTR heap;
} WW_PCB, *LPWW_PCB;

typedef struct {
    char	name[MAXFNAME];
    char	info[MAXFINFO];
    WW_LONG	loc;
    WW_LONG	len;
    WW_INT	count;
    WW_INT	mode;
    WW_LONG	mtime;
    union {
        WW_LONG	appid;
		WW_LONG	il;	
    } handler;
    WW_LONG	resource;
} WW_FENT_T, *WW_FS;


typedef struct {
	BYTE magic[4];
	BYTE padding[60];
	WW_FENT_T fent;
} WW_FXHEADER, *LPWW_FXHEADER;


typedef struct {
	WW_FARPTR className;   // char far *className;
	WW_FARPTR name;        // char far *name;
	WW_FARPTR version;     // char far *version;
	WW_FARPTR description; // char far *description;
	WW_FARPTR depends;     // char far * far *depends;
} WW_ILInfo, *LPWW_ILInfo;

typedef struct { // IL struct (see sys/indirect.h)
	WW_FARPTR link_pos;  // void far *link_pos;
	WW_INT n_methods;  // int n_methods;
	WW_FARPTR _get_info; // ILInfo far *(far *_get_info)(void);
} WW_IL, *LPWW_IL;

typedef struct { // IlibIL struct (see sys/indeirect.h)
    WW_IL super;
    WW_FARPTR _open;        // int (far *_open)(char far *ilname, IL far *ilbuf);
    WW_FARPTR _open_system; //int (far *_open_system)(char far *ilname, IL far *ilbuf);
} WW_IlibIL, *LPWW_IlibIL;

typedef struct { // ProcIL struct (see sys/indeirect.h)
    WW_IL super;
    WW_FARPTR _load;    // void far *(far *_load)(char far *command);
    WW_FARPTR _run;     // int (far *_run)(void far *entry, int argc, char far * far *argv);
    WW_FARPTR _exec;    // int (far *_exec)(char far *command, int argc, char far * far *argv);
    WW_FARPTR _exit;    // void (far *_exit)(int exitcode);
    WW_FARPTR _yield;   // void (far *_yield)(void);
    WW_FARPTR _suspend; // int (far *_suspend)(int pcbid);
    WW_FARPTR _resume;  // void (far *_resume)(int pcbid);
    WW_FARPTR _swap;    // int (far *_swap)(int pcbid);
/*
    WW_FARPTR _kill;    // int (far *_kill)(child);
 */
} WW_ProcIL, *LPWW_ProcIL;

typedef struct { // FsIL struct (see sys/indeirect.h)
    WW_IL super;
    WW_FARPTR _entries;   // fent_t far *(far *_entries)(FS fs);
    WW_FARPTR _n_entries; // int (far *_n_entries)(FS fs);
    WW_FARPTR _getent;    // int (far *_getent)(FS fs, int n, fent_t far *fep);
    WW_FARPTR _findent;   // int (far *_findent)(FS fs, char far *fname, fent_t far *fep);
    WW_FARPTR _mmap;      // void far *(far *_mmap)(FS fs, char far *fname);
    WW_FARPTR _open;      // int (far *_open)(FS fs, char far *fname, int mode, int perms);
    WW_FARPTR _close;     // int (far *_close)(int fd);
    WW_FARPTR _read;      // int (far *_read)(int fd, char far *buf, int len);
    WW_FARPTR _write;     // int (far *_write)(int fd, char far *buf, int len);
    WW_FARPTR _lseek;     // long (far *_lseek)(int fd, long offset, int origin);
    WW_FARPTR _chmod;     // int (far *_chmod)(FS fs, char far *fname, int mode);
    WW_FARPTR _freeze;    // int (far *_freeze)(FS fs, char far *fname);
    WW_FARPTR _melt;      // int (far *_melt)(FS fs, char far *fname);
    WW_FARPTR _creat;     // int (far *_creat)(FS fs, fent_t far *fep);
    WW_FARPTR _unlink;    // int (far *_unlink)(FS fs, char far *fname);
    WW_FARPTR _newfs;     // int (far *_newfs)(FS fs);
    WW_FARPTR _defrag;    // int (far *_defrag)(FS fs);
    WW_FARPTR _space;     // unsigned long (far *_space)(FS fs);
} WW_FsIL, *LPWW_FsIL;

typedef struct {
    WW_INT _status;		// int _status;			/* status of process */
    WW_INT _exit_code;	// int _exit_code;		/* process exit code */
} WW_ProcControl, *LPWW_ProcControl;

typedef struct {
    WW_FS fs;			// FS		fs;		/* FS which contains the fent */
    WW_FARPTR fentp;	// fent_t far *fentp;		/* original file entry */
    WW_SHORT omode;		// fmode_t	omode;		/* open mode: 0 indicates free handle */
    WW_FARPTR floc;		// floc_t 	floc;		/* file location */
    WW_LONG flen;		// flen_t 	flen;		/* file length */
    WW_LONG fpos;		// fpos_t	fpos;		/* current seek position */
    WW_INT count;		// int 	count;		/* total file block count */
    WW_FARPTR driver;	// StreamIL_p  driver;		/* stream driver for the file */
    WW_INT pcb;			// int		pcb;		/* opening process's pcb */
    WW_INT _reserved;	// int		_reserved;	/* reserved */
} WW_fhandle_t, *LPWW_fhandle_t;

#define MAXPROCESSES 3
#define MAXFILES 16
#define ROOTFS_NUM_ENTRIES 16
#define RAM0FS_NUM_ENTRIES 24
#define ROM0FS_NUM_ENTRIES 128

typedef struct { // SRAMWork struct (see sys/oswork.h)
    WW_PCB _opsc;						// ProcContext	_ospc;
    WW_ProcControl _pcb[MAXPROCESSES]; // ProcControl	_pcb[MAXPROCESSES];
    WW_INT _os_version;					// unsigned	_os_version;
    WW_INT _last_pcb;					// unsigned	_last_pcb;
    WW_INT _freefd;						// int		_freefd;
    WW_fhandle_t _openfiles[MAXFILES];	// fhandle_t	_openfiles[MAXFILES];
    char	_shell_work[128];
    WW_FENT_T _root_fs_entries[ROOTFS_NUM_ENTRIES]; // fent_t	_root_fs_entries[ROOTFS_NUM_ENTRIES];
    WW_FENT_T _ram0_fs_entries[RAM0FS_NUM_ENTRIES]; // fent_t	_ram0_fs_entries[RAM0FS_NUM_ENTRIES];
    WW_FENT_T _rom0_fs_entries[ROM0FS_NUM_ENTRIES]; // fent_t	_rom0_fs_entries[ROM0FS_NUM_ENTRIES];
    WW_FARPTR _saveports;				// unsigned char far *	_saveports;
} WW_SRAMWork, *LPWW_SRAMWork;


typedef struct { // 
	char name[16];
	WW_INT birth_year;
	char birth_month;
	char birth_day;
	char sex;
	char bloodtype;
} WW_OWNERINFO, *LPWW_OWNERINFO;

#pragma pack( pop, beforewwstruct )

#endif // #ifndef _WWSTRUCT_H