#include "../mmage/preference.h"
#include "wwbios.h"

extern "C" {
#include "../wonx/wonx_include/system.h"
}

enum {
    SYS_INTERRUPT_SET_HOOK = 0,
    SYS_INTERRUPT_RESET_HOOK,
    SYS_WAIT,
    SYS_GET_TICK_COUNT,
    SYS_SLEEP,
    SYS_SET_SLEEP_TIME,
    SYS_GET_SLEEP_TIME,
    SYS_SET_AWAKE_KEY,
    SYS_GET_AWAKE_KEY,
    SYS_SET_KEEPALIVE_INT,
    SYS_GET_OWNERINFO,
    SYS_SUSPEND,
    SYS_RESUME,
    SYS_SET_REMOTE,
    SYS_GET_REMOTE,
    SYS_ALLOC_IRAM,
    SYS_FREE_IRAM,
    SYS_GET_MY_IRAM,
    SYS_GET_VERSION,
    SYS_SWAP,
    SYS_SET_RESUME,
    SYS_GET_RESUME,
};

void system_handler(int func_no) {
	int tmp;

	switch(func_no) {
		case SYS_INTERRUPT_SET_HOOK:
			/*
			sys_interrupt_set_hook(*bregs[AL], (intvector_t *)(c_ds + wregs[BX]), (intvector_t *)(c_ds + wregs[DX]));
			*/
			break;
		case SYS_INTERRUPT_RESET_HOOK:
			/*
			sys_interrupt_reset_hook(*bregs[AL], (intvector_t *)(c_ds + wregs[BX]));
			*/
			break;
		case SYS_WAIT:
			sys_wait(wregs[CX]);
			break;
		case SYS_GET_TICK_COUNT:
			tmp = sys_get_tick_count();
			wregs[DX] = (tmp >> 16) & 0xffff;
			wregs[AX] = tmp & 0xffff;
			break;
		case SYS_SLEEP:
			sys_sleep();
			break;
		case SYS_SET_SLEEP_TIME:
			sys_set_sleep_time(*bregs[BL]);
			break;
		case SYS_GET_SLEEP_TIME:
			wregs[AX] = sys_get_sleep_time();
			break;
		case SYS_SET_AWAKE_KEY:
			sys_set_awake_key(wregs[BX]);
			break;
		case SYS_GET_AWAKE_KEY:
			wregs[AX] = sys_get_awake_key();
			break;
		case SYS_SET_KEEPALIVE_INT:
			sys_set_keepalive_int(*bregs[BL]);
			break;
		case SYS_GET_OWNERINFO:
			memcpy((void *)(c_ds + wregs[DX]), (void *)&mmconfig->ownerinfo,
				(wregs[CX] > sizeof(mmconfig->ownerinfo)) ? sizeof(mmconfig->ownerinfo) : wregs[CX]);
//			sys_get_ownerinfo(wregs[CX], (char *)(c_ds + wregs[DX]));
			break;
		case SYS_SUSPEND:
			wregs[AX] = sys_suspend(*bregs[AL]);
			break;
		case SYS_RESUME:
			sys_resume(*bregs[AL]);
			break;
		case SYS_SET_REMOTE:
			sys_set_remote(*bregs[AL]);
			break;
		case SYS_GET_REMOTE:
			wregs[AX] = sys_get_remote();
			break;
		case SYS_ALLOC_IRAM:
			wregs[AX] = (unsigned short)sys_alloc_iram((void *)wregs[BX], wregs[CX]);
			break;
		case SYS_FREE_IRAM:
			sys_free_iram((void *)wregs[BX]);
			break;
		case SYS_GET_MY_IRAM:
			wregs[AX] = (unsigned short)sys_get_my_iram();
			break;
		case SYS_GET_VERSION:
			wregs[AX] = sys_get_version();
			break;
		case SYS_SWAP:
			wregs[AX] = sys_swap(*bregs[AL]);
			break;
		case SYS_SET_RESUME:
			sys_set_resume(wregs[BX]);
			break;
		case SYS_GET_RESUME:
			wregs[AX] = sys_get_resume();
			break;

		default:
			break;

	}
}