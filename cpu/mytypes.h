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

/* This is MYTYPES.H  It contains definitions for the basic types to ease
   portability */


#ifndef MYTYPES_H
#define MYTYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef signed char INT8;
typedef unsigned char UINT8;
typedef signed short INT16;
typedef unsigned short UINT16;

#ifndef _WINDOWS

typedef signed long INT32;
typedef unsigned long UINT32;


typedef UINT8 BYTE;
typedef UINT16 WORD;
typedef UINT32 DWORD;

#else

#include <windows.h>
#define LITTLE_ENDIAN
#define ALIGNED_ACCESS
//#define BIGCASE

#endif


#ifdef __cplusplus
}
#endif

#endif
