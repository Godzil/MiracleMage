#ifndef _KANJIFONT_H
#define _KANJIFONT_H

#ifdef __cplusplus
extern "C" {
#endif

int InitKanjiFont(char *fontfile);
unsigned char *GetKanjiFont(int u, int d);
void GetKanjiFont16(int code, unsigned char *font);

#ifdef __cplusplus
}
#endif

#endif