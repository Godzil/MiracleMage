#include "wwbios.h"

extern "C" {
#include "../wonx/wonx_include/timer.h"
}

enum {
    RTC_RESET = 0,
    RTC_SET_DATETIME,
    RTC_GET_DATETIME,
    RTC_SET_DATETIME_STRUCT,
    RTC_GET_DATETIME_STRUCT,
    RTC_ENABLE_ALARM,
    RTC_DISABLE_ALARM,
    TIMER_ENABLE,
    TIMER_DISABLE,
    TIMER_GET_COUNT,
};

void timer_handler(int func_no) {

	switch(func_no) {
		case RTC_RESET:
			/* WonX で未実装
			rtc_reset();
			*/
			break;
		case RTC_SET_DATETIME:
			rtc_set_datetime(wregs[BX], wregs[CX]);
			break;
		case RTC_GET_DATETIME:
			wregs[AX] = rtc_get_datetime(wregs[BX]);
			break;
		case RTC_SET_DATETIME_STRUCT:
			rtc_set_datetime_struct((datetime_t *)(c_ds + wregs[DX]));
			break;
		case RTC_GET_DATETIME_STRUCT:
			rtc_get_datetime_struct((datetime_t *)(c_ds + wregs[DX]));
			break;
		case RTC_ENABLE_ALARM:
			rtc_enable_alarm(*bregs[BL], *bregs[BH]);
			break;
		case RTC_DISABLE_ALARM:
			rtc_disable_alarm();
			break;
		case TIMER_ENABLE:
			timer_enable(*bregs[AL], *bregs[BL], wregs[CX]);
			break;
		case TIMER_DISABLE:
			timer_disable(*bregs[AL]);
			break;
		case TIMER_GET_COUNT:
			wregs[AX] = timer_get_count(*bregs[AL]);
			break;

		default:
			break;
	}
}