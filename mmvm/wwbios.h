#ifndef _WWBIOS_H
#define _WWBIOS_H

#include <stdlib.h>

#include "../cpu/mytypes.h"
#include "../cpu/global.h"
#include "../cpu/cpu.h"

#ifdef __cplusplus
extern "C" {
#endif


enum {
	INT_BIOS_EXIT=0x10,
	INT_KEY,
	INT_DISP,
	INT_TEXT,
	INT_SERIAL,
	INT_SOUND,
	INT_TIMER,
	INT_SYSTEM,
	INT_BANK,
};

void int_handler(int no);

void key_handler(int func_no);
void disp_handler(int func_no);
void text_handler(int func_no);
void serial_handler(int func_no);
void sound_handler(int func_no);
void timer_handler(int func_no);
void system_handler(int func_no);
void bank_handler(int func_no);


#ifdef __cplusplus
}
#endif


#endif