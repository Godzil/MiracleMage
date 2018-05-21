#include "wwbios.h"

extern "C" {
#include "../wonx/wonx_include/text.h"
}

enum {
    TEXT_SCREEN_INIT = 0,
    TEXT_WINDOW_INIT,
    TEXT_SET_MODE,
    TEXT_GET_MODE,
    TEXT_PUT_CHAR,
    TEXT_PUT_STRING,
    TEXT_PUT_SUBSTRING,
    TEXT_PUT_NUMERIC,
    TEXT_FILL_CHAR,
    TEXT_SET_PALETTE,
    TEXT_GET_PALETTE,
    TEXT_SET_ANK_FONT,
    TEXT_SET_SJIS_FONT,
    TEXT_GET_FONTDATA,
    TEXT_SET_SCREEN,
    TEXT_GET_SCREEN,
    CURSOR_DISPLAY,
    CURSOR_STATUS,
    CURSOR_SET_LOCATION,
    CURSOR_GET_LOCATION,
    CURSOR_SET_TYPE,
    CURSOR_GET_TYPE,
};

void text_handler(int func_no) {
	int tmp;

	switch(func_no) {
		case TEXT_SCREEN_INIT:
			text_screen_init();
			break;
		case TEXT_WINDOW_INIT:
			text_window_init(*bregs[BL], *bregs[BH], *bregs[CL], *bregs[CH], wregs[DX]);
			break;
		case TEXT_SET_MODE:
			text_set_mode(wregs[BX]);
			break;
		case TEXT_GET_MODE:
			wregs[AX] = text_get_mode();
			break;
		case TEXT_PUT_CHAR:
			text_put_char(*bregs[BL], *bregs[BH], wregs[CX]);
			break;
		case TEXT_PUT_STRING:
			wregs[AX] = text_put_string(*bregs[BL], *bregs[BH], (char *)(c_ds + wregs[DX]));
			break;
		case TEXT_PUT_SUBSTRING:
			text_put_substring(*bregs[BL], *bregs[BH], (char *)(c_ds + wregs[DX]), wregs[CX]);
			break;
		case TEXT_PUT_NUMERIC:
			if((*bregs[CH]) & NUM_STORE) {
				text_store_numeric((char *)(c_ds + wregs[SI]), *bregs[CL], *bregs[CH], wregs[DX]);
			} else {
				text_put_numeric(*bregs[BL], *bregs[BH], *bregs[CL], *bregs[CH], wregs[DX]);
			}
			break;
		case TEXT_FILL_CHAR:
			text_fill_char(*bregs[BL], *bregs[BH], wregs[CX], wregs[DX]);
			break;
		case TEXT_SET_PALETTE:
			text_set_palette(wregs[BX]);
			break;
		case TEXT_GET_PALETTE:
			wregs[AX] = text_get_palette();
			break;
		case TEXT_SET_ANK_FONT:
			text_set_ank_font(*bregs[BL], *bregs[BH], wregs[CX], c_ds + wregs[DX]);
			break;
		case TEXT_SET_SJIS_FONT:
			text_set_sjis_font((void *)(wregs[BX] << 4 | wregs[DX]));
			break;
		case TEXT_GET_FONTDATA:
			wregs[AX] = text_get_fontdata(wregs[CX], c_ds + wregs[DX]);
			break;
		case TEXT_SET_SCREEN:
//			text_set_screen(wregs[BX]);  // マニュアル第 1 版は間違い
			text_set_screen(*bregs[AL]);
			break;
		case TEXT_GET_SCREEN:
			wregs[AX] = text_get_screen();
			break;
		case CURSOR_DISPLAY:
			cursor_display(*bregs[AL]);
			break;
		case CURSOR_STATUS:
			wregs[AX] = cursor_status();
			break;
		case CURSOR_SET_LOCATION:
			cursor_set_location(*bregs[BL], *bregs[BH], *bregs[CL], *bregs[CH]);
			break;
		case CURSOR_GET_LOCATION:
			tmp = cursor_get_location();
			wregs[DX] = (tmp >> 16) & 0xffff;
			wregs[AX] = tmp & 0xffff;
			break;
		case CURSOR_SET_TYPE:
			cursor_set_type(*bregs[BL], *bregs[CL]);
			break;
		case CURSOR_GET_TYPE:
			tmp = cursor_get_type();
			wregs[DX] = (tmp >> 16) & 0xffff;
			wregs[AX] = tmp & 0xffff;
			break;

		default:
			break;
	}


}