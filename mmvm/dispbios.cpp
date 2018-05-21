#include "../mmage/mmage.h"
#include "mmvm.h"
#include "wwbios.h"

extern "C" {
#include "../wonx/wonx_include/disp.h"
}

enum {
    DISPLAY_CONTROL = 0,
    DISPLAY_STATUS,
    FONT_SET_MONODATA,
    FONT_SET_COLORDATA,
    FONT_GET_DATA,
    FONT_SET_COLOR,
    FONT_GET_COLOR,
    SCREEN_SET_CHAR,
    SCREEN_GET_CHAR,
    SCREEN_FILL_CHAR,
    SCREEN_FILL_ATTR,
    SPRITE_SET_RANGE,
    SPRITE_SET_CHAR,
    SPRITE_GET_CHAR,
    SPRITE_SET_LOCATION,
    SPRITE_GET_LOCATION,
    SPRITE_SET_CHAR_LOCATION,
    SPRITE_GET_CHAR_LOCATION,
    SPRITE_SET_DATA,
    SCREEN_SET_SCROLL,
    SCREEN_GET_SCROLL,
    SCREEN2_SET_WINDOW,
    SCREEN2_GET_WINDOW,
    SPRITE_SET_WINDOW,
    SPRITE_GET_WINDOW,
    PALETTE_SET_COLOR,
    PALETTE_GET_COLOR,
    LCD_SET_COLOR,
    LCD_GET_COLOR,
    LCD_SET_SEGMENTS,
    LCD_GET_SEGMENTS,
    LCD_SET_SLEEP,
    LCD_GET_SLEEP,
    SCREEN_SET_VRAM,
    SPRITE_SET_VRAM,
};

void disp_handler(int func_no) {
	int tmp;

	switch(func_no) {
		case DISPLAY_CONTROL:
			display_control(wregs[BX]);
			break;
		case DISPLAY_STATUS:
			wregs[AX] = display_status();
			break;
		case FONT_SET_MONODATA:
			font_set_monodata(wregs[BX], wregs[CX], c_ds + wregs[DX]);
			break;
		case FONT_SET_COLORDATA:
			font_set_colordata(wregs[BX], wregs[CX], c_ds + wregs[DX]);
			break;
		case FONT_GET_DATA:
			font_get_data(wregs[BX], wregs[CX], c_ds + wregs[DX]);
			break;
		case FONT_SET_COLOR:
			font_set_color(wregs[BX]);
			break;
		case FONT_GET_COLOR:
			wregs[AX] = font_get_color();
			break;
		case SCREEN_SET_CHAR:
			screen_set_char(*bregs[AL], *bregs[BL], *bregs[BH], *bregs[CL], *bregs[CH],
				(unsigned short *)(c_ds + wregs[DX]));
			break;
		case SCREEN_GET_CHAR:
			screen_get_char(*bregs[AL], *bregs[BL], *bregs[BH], *bregs[CL], *bregs[CH],
				(unsigned short *)(c_ds + wregs[DX]));
			break;
		case SCREEN_FILL_CHAR:
			screen_fill_char(*bregs[AL], *bregs[BL], *bregs[BH], *bregs[CL], *bregs[CH], wregs[DX]);
			break;
		case SCREEN_FILL_ATTR:
			screen_fill_attr(*bregs[AL], *bregs[BL], *bregs[BH], *bregs[CL], *bregs[CH], wregs[DX], wregs[SI]);
			break;
		case SPRITE_SET_RANGE:
			sprite_set_range(wregs[BX], wregs[CX]);
			break;
		case SPRITE_SET_CHAR:
			sprite_set_char(wregs[BX], wregs[CX]);
			break;
		case SPRITE_GET_CHAR:
			wregs[AX] = sprite_get_char(wregs[BX]);
			break;
		case SPRITE_SET_LOCATION:
			sprite_set_location(wregs[BX], *bregs[DL], *bregs[DH]);
			break;
		case SPRITE_GET_LOCATION:
			wregs[AX] = sprite_get_location(wregs[BX]);
			break;
		case SPRITE_SET_CHAR_LOCATION:
			sprite_set_char_location(wregs[BX], wregs[CX], *bregs[DL], *bregs[DH]);
			break;
		case SPRITE_GET_CHAR_LOCATION:
			tmp = sprite_get_char_location(wregs[BX]);
			wregs[DX] = (tmp >> 16) & 0xffff;
			wregs[AX] = tmp & 0xffff;
			break;
		case SPRITE_SET_DATA:
			sprite_set_data(wregs[BX], wregs[CX], (unsigned long *)(c_ds + wregs[DX]));
			break;
		case SCREEN_SET_SCROLL:
			screen_set_scroll(*bregs[AL], *bregs[BL], *bregs[BH]);
			break;
		case SCREEN_GET_SCROLL:
			wregs[AX] = screen_get_scroll(*bregs[AL]);
			break;
		case SCREEN2_SET_WINDOW:
			screen2_set_window(*bregs[BL], *bregs[BH], *bregs[CL], *bregs[CH]);
			break;
		case SCREEN2_GET_WINDOW:
			tmp = screen2_get_window();
			wregs[DX] = (tmp >> 16) & 0xffff;
			wregs[AX] = tmp & 0xffff;
			break;
		case SPRITE_SET_WINDOW:
			sprite_set_window(*bregs[BL], *bregs[BH], *bregs[CL], *bregs[CH]);
			break;
		case SPRITE_GET_WINDOW:
			tmp = sprite_get_window();
			wregs[DX] = (tmp >> 16) & 0xffff;
			wregs[AX] = tmp & 0xffff;
			break;
		case PALETTE_SET_COLOR:
			palette_set_color(wregs[BX], wregs[CX]);
			break;
		case PALETTE_GET_COLOR:
			wregs[AX] = palette_get_color(wregs[BX]);
			break;
		case LCD_SET_COLOR:
			lcd_set_color(wregs[BX], wregs[CX]);
			break;
		case LCD_GET_COLOR:
			tmp = lcd_get_color();
			wregs[DX] = (tmp >> 16) & 0xffff;
			wregs[AX] = tmp & 0xffff;
			break;
		case LCD_SET_SEGMENTS:
//			lcd_set_segments(wregs[BX]);
			mmvm->lcdseg = wregs[BX];
			InvalidateRect(wonw32ctx->hWnd, NULL, FALSE);
			UpdateWindow(wonw32ctx->hWnd);
			break;
		case LCD_GET_SEGMENTS:
//			wregs[AX] = lcd_get_segments();
			wregs[AX] = mmvm->lcdseg;
			break;
		case LCD_SET_SLEEP:
			lcd_set_sleep(wregs[BX]);
			break;
		case LCD_GET_SLEEP:
			wregs[AX] = lcd_get_sleep();
			break;
		case SCREEN_SET_VRAM:
			screen_set_vram(*bregs[AL], wregs[BX]);
			break;
		case SPRITE_SET_VRAM:
			sprite_set_vram(wregs[BX]);
			break;

		default:
			break;
	}
}