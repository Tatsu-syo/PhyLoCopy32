/*
PhyLoCopy32 (C) 1995-2000,2021 Tatsuhiko Shoji
The sources for PhyLoCopy32 are distributed under the MIT open source license
*/
/* 変数 */
/* オプション */
extern char deffn[64];
extern int media,machine;
extern int dtl; /* データ長 */

extern char *er_info; /* BIOS使用時のエラー情報を格納する。 */

/* 関数 */
/* 汎用関数 */
void far *nptofp(void *);

/* 定義のたぐい */
#define SECSIZE 512
