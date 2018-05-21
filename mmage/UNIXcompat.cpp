#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "../wonx/UNIXTimerP.h"
#include "../wonx/UNIXTimer.h"

#ifdef __cplusplus
}
#endif

static _UNIXTimer ut;

UNIXTimer UNIXTimer_Create(int auto_preset, int interval, void *parameter, UNIXTimerCallBack callback) {
	return &ut;
}

UNIXTimer UNIXTimer_Destroy(UNIXTimer unix_timer) {
	return NULL;
}

int UNIXTimer_Pause(UNIXTimer unix_timer) {
	return 0;
}

int UNIXTimer_Unpause(UNIXTimer unix_timer) {
	return 0;
}

int UNIXTimer_ON(UNIXTimer unix_timer) {
	return 0;	
}

int UNIXTimer_OFF(UNIXTimer unix_timer) {
	return 0;
}
