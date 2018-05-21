#include <stdio.h>
#include "kanjifont.h"

#define NUM_OF_KANJIFONT 6877
#define KANJIFONT_SIZE 8

static unsigned char kfonts[NUM_OF_KANJIFONT * KANJIFONT_SIZE];
static unsigned char notfound[KANJIFONT_SIZE] = {0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55};
static int font_loaded;

int InitKanjiFont(char *fontfile) {
	FILE *fp;
	int c, i;

	font_loaded = 0;
	fp = fopen(fontfile, "rb");

	if(fp != NULL) {
		i = 0;
		while((c = fgetc(fp)) >= 0) {
			kfonts[i++] = (unsigned char)c;
		}
		if(i == NUM_OF_KANJIFONT * KANJIFONT_SIZE)
			font_loaded = 1;
	}

	return font_loaded;
}

unsigned char *GetKanjiFont(int u, int d) {
	int c;
	if (u > 0x9f) u -= 0x40;
	if (d > 0x7f) d--;
	d -= 0x40;
	u -= 0x81;
	c = u * 0xbc + d;

	if(!font_loaded)
		return notfound;

	if (c >= 7806) {
		c = -1;
	} else if(c >= 4418) {  /* 第二水準漢字 */
		c = c + (0xda1 - 4418);
	} else if (c >= 4375) { /* なし */
		c = -1;
	} else if (c >= 1410) { /* 第一水準漢字 */
		c = c + (0x20c - 1410);
	} else if (c >= 690) {  /* 禁止領域の文字 */
		c = -1;
	} else if (c >= 658) {  /* 罫線素 */
		c = c + (0x1ec - 658);
	} else if (c >= 612) {  /* ロシア小 */
		c = c + (0x1cb - 612);
	} else if (c >= 564) {  /* ロシア大 */
		c = c + (0x1aa - 564);
	} else if (c >= 502) {  /* ギリシャ小 */
		c = c + (0x192 - 502);
	} else if (c >= 470) {  /* ギリシャ大*/
		c = c + (0x17a - 470);
	} else if (c >= 376) {  /* カタカナ */
		c = c + (0x124 - 376);
	} else if (c >= 282) {  /* ひらがな */
		c = c + (0xd1 - 282);
	} else if (c >= 252) {  /* 英小文字 */
		c = c + (0xb7 - 252);
	} else if (c >= 220) {  /* 英大文字 */
		c = c + (0x9d - 220);
	} else if (c >= 203) {  /* 数字 */
		c = c + (0x93 - 203);
	} else if (c >= 187) { /* 記号(◯) */
		c = 0x92;
	} else if (c >= 175) { /* 記号(Å‰♯♭♪†‡¶) */
		c = c + (0x8a - 203);
	} else if (c >= 153) { /* 記号(∠⊥⌒∂∇≡≒≪≫√∽∝∵∫∬) */
		c = c + (0x7b - 153);
	} else if (c >= 135) { /* 記号(∧∨￢⇒⇔∀∃) */
		c = c + (0x74 - 135);
	} else if (c >= 119) { /* 記号(∈∋⊆⊇⊂⊃∪∩) */
		c = c + (0x6c - 119);
	} // else {} /* 記号(その他) */

	if(c == -1)
		return notfound;
	else
		return kfonts + (c * KANJIFONT_SIZE);

}

void GetKanjiFont16(int code, unsigned char *font) {
	unsigned char *font8;
	int i, j;

	font8 = GetKanjiFont((code >> 8) & 0xff, code & 0xff);
	for(i = 0; i < 8; i++) {
		font[i * 2 + 1] = 0;
		for(j = 0; j < 4; j++) {
			font[i * 2 + 1] <<= 2;
			if((font8[i] >> j) & 1)
				font[i * 2 + 1] |= 3; // (11)2
		}
		font[i * 2] = 0;
		for(j = 4; j < 8; j++) {
			font[i * 2] <<= 2;
			if((font8[i] >> j) & 1)
				font[i * 2] |= 3; // (11)2
		}
	}
}
