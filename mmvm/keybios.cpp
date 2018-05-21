#include "../mmage/mmage.h"
#include "wwbios.h"

extern "C" {
#include "../wonx/wonx_include/key.h"
}

enum {
    KEY_PRESS_CHECK = 0,
    KEY_HIT_CHECK,
    KEY_WAIT,
    KEY_SET_REPEAT,
    KEY_GET_REPEAT,
    KEY_HIT_CHECK_WITH_REPEAT,
};

void key_handler(int func_no) {
	WORD ret;

	switch(func_no) {
		case KEY_PRESS_CHECK:
			wregs[AX] = key_press_check();
			break;
		case KEY_HIT_CHECK:
			wregs[AX] = key_hit_check();
			break;
		case KEY_WAIT:
			// return 値が 0 のときは ip を -2 して見かけ上 wait してるようにみせかける
			ret = key_wait();
			if(ret) {
				wregs[AX] = ret;
			} else {
				WaitForSingleObject(wonw32ctx->syncevent, INFINITE);
				ip = ip - 2;
			}
			break;
		case KEY_SET_REPEAT:
			key_set_repeat(*bregs[BL], *bregs[BH]);
			break;
		case KEY_GET_REPEAT:
			wregs[AX] = key_get_repeat();
			break;
		case KEY_HIT_CHECK_WITH_REPEAT:
			wregs[AX] = key_hit_check_with_repeat();
			break;
		default:
			break;
	}
}
