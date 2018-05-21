#include "../mmage/mmage.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "wwbios.h"

void int_handler(int no) {
	int func_no;
	TCHAR msg[256];

	func_no = *bregs[AH];

	switch(no) {
		case INT_BIOS_EXIT:
			wonw32ctx->running = FALSE;
			wsprintf(msg, TEXT("アプリケーションは終了しました"));
			MessageBox(wonw32ctx->hWnd, msg, TEXT("終了"), MB_OK);
			break;

		case INT_KEY:
			key_handler(func_no);
			break;
		case INT_DISP:
			disp_handler(func_no);
			break;
		case INT_TEXT:
			text_handler(func_no);
			break;
		case INT_SERIAL:
//			serial_handler(func_no);
			break;
		case INT_SOUND:
//			sound_handler(func_no);
			break;
		case INT_TIMER:
			timer_handler(func_no);
			break;
		case INT_SYSTEM:
			system_handler(func_no);
			break;
		case INT_BANK:
//			bank_handler(func_no);
			break;
		default:
			break;
	}

}

