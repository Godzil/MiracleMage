/****************************************************************************
*                                                                           *
*                            Third Year Project                             *
*                                                                           *
*                            An IBM PC Emulator                             *
*                          For Unix and X Windows                           *
*                                                                           *
*                             By David Hedley                               *
*                                                                           *
*                                                                           *
* This program is Copyrighted.  Consult the file COPYRIGHT for more details *
*                                                                           *
****************************************************************************/

/* This is CPU.H  it contains definitions for cpu.c */


#ifndef CPU_H
#define CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mytypes.h"

#define AX 0
#define CX 1
#define DX 2
#define BX 3
#define SP 4
#define BP 5
#define SI 6
#define DI 7

#define AL 0
#define CL 1
#define DL 2
#define BL 3
#define AH 4
#define CH 5
#define DH 6
#define BH 7

#define SPL 8
#define SPH 9
#define BPL 10
#define BPH 11
#define SIL 12
#define SIH 13
#define DIL 14
#define DIH 15


#define ES 0
#define CS 1
#define SS 2
#define DS 3


/* parameter x = result, y = source 1, z = source 2 */

#define SetCFB_Add(x,y) (CF = (BYTE)(x) < (BYTE)(y))
#define SetCFW_Add(x,y) (CF = (WORD)(x) < (WORD)(y))
#define SetCFB_Sub(y,z) (CF = (BYTE)(y) > (BYTE)(z))
#define SetCFW_Sub(y,z) (CF = (WORD)(y) > (WORD)(z))
#define SetZFB(x)	(ZF = !(BYTE)(x))
#define SetZFW(x)	(ZF = !(WORD)(x))
#define SetTF(x)	(TF = (x))
#define SetIF(x)	(IF = (x))
#define SetDF(x)	(DF = (x))
#define SetAF(x,y,z)	(AF = ((x) ^ ((y) ^ (z))) & 0x10)
#define SetPF(x)	    (PF = parity_table[(BYTE)(x)])
#define SetOFW_Add(x,y,z)   (OF = ((x) ^ (y)) & ((x) ^ (z)) & 0x8000)
#define SetOFB_Add(x,y,z)   (OF = ((x) ^ (y)) & ((x) ^ (z)) & 0x80)
#define SetOFW_Sub(x,y,z)   (OF = ((z) ^ (y)) & ((z) ^ (x)) & 0x8000)
#define SetOFB_Sub(x,y,z)   (OF = ((z) ^ (y)) & ((z) ^ (x)) & 0x80)
#define SetSFW(x)	   (SF = (x) & 0x8000)
#define SetSFB(x)	   (SF = (x) & 0x80)


#define GetMemInc(Seg,Off) ((Seg)[(Off)++])

#if defined(TEST) && defined(BSD386)
#   define SegToMemPtr(Seg) (sregs[Seg] == 0xa000 ? (BYTE *)0xe00a0000 : \
				sregs[Seg] == 0xb800 ? (BYTE *)0xe00b8000 : \
					    &memory[sregs[Seg] << 4])
#else
#   define SegToMemPtr(Seg) (&memory[sregs[Seg] << 4])
#endif


#define PutMemB(Seg,Off,x) ((Seg)[(WORD)(Off)] = (BYTE)(x))
#define GetMemB(Seg,Off) ((BYTE)(Seg)[(WORD)(Off)])

#define CalcAll()

#define CompressFlags() (WORD)(CF | (PF << 2) | (!(!AF) << 4) | (ZF << 6) \
			    | (!(!SF) << 7) | (TF << 8) | (IF << 9) \
			    | (DF << 10) | (!(!OF) << 11))

#define ExpandFlags(f) \
{ \
      CF = (f) & 1; \
      PF = ((f) & 4) == 4; \
      AF = (f) & 16; \
      ZF = ((f) & 64) == 64; \
      SF = (f) & 128; \
      TF = ((f) & 256) == 256; \
      IF = ((f) & 512) == 512; \
      DF = ((f) & 1024) == 1024; \
      OF = (f) & 2048; \
}


/* ChangeE(x) changes x to little endian from the machine's natural endian
    format and back again. Obviously there is nothing to do for little-endian
    machines... */

#if defined(LITTLE_ENDIAN)
#   define ChangeE(x) (WORD)(x)
#else
#   define ChangeE(x) (WORD)(((x) << 8) | ((BYTE)((x) >> 8)))
#endif

#if defined(LITTLE_ENDIAN) && !defined(ALIGNED_ACCESS)
#   define ReadWord(x) (*(x))
#   define WriteWord(x,y) (*(x) = (y))
#   define CopyWord(x,y) (*x = *y)
#   define PutMemW(Seg,Off,x) (*(WORD *)((Seg)+(WORD)(Off)) = (WORD)(x))
#   define GetMemW(Seg,Off) (*(WORD *)((Seg)+(WORD)(Off)))
#else
#   define ReadWord(x) ((WORD)(*((BYTE *)(x))) + ((WORD)(*((BYTE *)(x)+1)) << 8))
#   define WriteWord(x,y) (*(BYTE *)(x) = (BYTE)(y), *((BYTE *)(x)+1) = (BYTE)((y) >> 8))
#   define CopyWord(x,y) (*(BYTE *)(x) = *(BYTE *)(y), *((BYTE *)(x)+1) = *((BYTE *)(y)+1))
#   define PutMemW(Seg,Off,x) (Seg[Off] = (BYTE)(x), Seg[(WORD)(Off)+1] = (BYTE)((x) >> 8))
#   define GetMemW(Seg,Off) ((WORD)Seg[Off] + ((WORD)Seg[(WORD)(Off)+1] << 8))
#endif

extern WORD wregs[8];	    /* Always little-endian */
extern BYTE *bregs[16];     /* Points to bytes within wregs[] */
extern unsigned sregs[4];   /* Always native machine word order */

extern unsigned ip;	     /* Always native machine word order */

    /* All the byte flags will either be 1 or 0 */
extern BYTE CF, PF, ZF, TF, IF, DF;

    /* All the word flags may be either none-zero (true) or zero (false) */
extern unsigned AF, OF, SF;

extern BYTE *c_cs,*c_ds,*c_es,*c_ss,*c_stack;

extern volatile int int_pending;
extern volatile int int_blocked;

#ifdef __cplusplus
}
#endif

#endif
