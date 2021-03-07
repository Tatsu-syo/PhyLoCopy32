/*
PhyLoCopy32 (C) 1995-2000,2021 Tatsuhiko Shoji
The sources for PhyLoCopy32 are distributed under the MIT open source license
*/
#include	<windows.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>
#include	<string.h>
#include	"DiskOperation.h"
#include	"DiskOperation9x.h"
#include	"DiskOperationNT.h"
#include	"phylocp.h"

char *title = "@(#)Virturl disk file maker for MSX Emulator PhyLoCopy32 Version 1.0.0\nBy Tatsuhiko Shoji 1995-2000,2021\n";

/* Global (option etc.) */
/* オプション */
char deffn[64];
int media,machine;
/* システム共用 */
int dtl;
char *er_info;
DiskOperation *ioObj = NULL;
HANDLE hDisk;

int absread(void *buff,unsigned char drive, int sec, int len)
{
	BOOL result = ioObj->readSector(hDisk, drive, sec, (char *)buff);

	if (result) {
		return 0;
	} else {
		return 1;
	}
}


int abswrite(void *buff,unsigned char drive, int sec, int len)
{
	BOOL result = ioObj->writeSector(hDisk, drive, sec, (char *)buff);

	if (result) {
		return 0;
	} else {
		return 1;
	}
}


/* DOS汎用コピー機能 */
int chk2dd(unsigned char drive)
{
	// 2DD 9sec
	return 1;

	/* 2DD,8 Sec. */
	// return(2);
}


/**
 * 実ディスクから仮想ディスクファイルにコピー
 *
 * @param srcd コピー元ドライブ
 * @param file コピー先ファイル
 */
int copyrv(char *srcd,char *file)
{
	char *buff;
	char *bufn;
	int i,m2,end;
	FILE *fp;
	unsigned char drive;

	if (isalpha(srcd[0])){
		drive = toupper(srcd[0]) - 'A';
	}else{
		drive = 0;
	}

	hDisk = ioObj->OpenFloppy(drive, GENERIC_READ | GENERIC_WRITE);
	if (hDisk == NULL) {
		fprintf(stderr,"Can't open Real disk.\n");
		return 1;
	}

	m2 = chk2dd(drive);
	switch (m2){
		case 0:
			fprintf(stderr,"Source disk isn't 2DD 9/8 sector format.\n");
			ioObj->CloseFloppy(hDisk);
			return 1;
		break;
		case 1:
			end = 1440;
		break;
		case 2:
			end = 1280;
		break;
		default:
			fprintf(stderr,"Source disk isn't 2DD 9/8 sector format.\n");
			ioObj->CloseFloppy(hDisk);
			return 1;
		break;
	}

	if ((bufn = (char *)calloc(1,4096)) == NULL){
		fprintf(stderr,"No enough memory.\n");
		ioObj->CloseFloppy(hDisk);
		return 1;
	}

	buff = bufn;

	if ((fp = fopen(file,"wb")) == NULL){
		fprintf(stderr,"Can't open output Virtual disk file\n");
		free(bufn);
		ioObj->CloseFloppy(hDisk);
		return 1;
	}

	fprintf(stderr,"Copying Virturl disk file from real disk...\n");

	for (i = 0;i < end;i++){
		if (absread(buff,drive,i,1)){
			/* read8(bufn,drive,i*8); */
			fprintf(stderr,"Can't read Real disk sector %d\n", i);
		}
		if (fwrite(bufn,512,1,fp) == 0){
			fprintf(stderr,"Can't write Virtual disk file.\n");
		}
	}
	ioObj->CloseFloppy(hDisk);

	fclose(fp);
	free(bufn);

	return(0);
}

/* 仮想ディスクファイルから実ディスクにコピー */
int copyvr(char *file,char *srcd)
{
	char *buff;
	char *bufn;
	int i,m2,end;
	FILE *fp;
	unsigned char drive;

	if (isalpha(srcd[0])){
		drive = toupper(srcd[0]) - 'A';
	}else{
		drive = 0;
	}

	hDisk = ioObj->OpenFloppy(drive, GENERIC_READ | GENERIC_WRITE);
	if (hDisk == NULL) {
		fprintf(stderr,"Can't open Real disk.\n");
		return 1;
	}

	m2 = chk2dd(drive);
	switch (m2){
		case 0:
			fprintf(stderr,"Source disk isn't 2DD 9/8 sector format.\n");
			ioObj->CloseFloppy(hDisk);
			return 1;
		break;
		case 1:
			end = 1440;
		break;
		case 2:
			end = 1280;
		break;
		default:
			fprintf(stderr,"Source disk isn't 2DD 9/8 sector format.\n");
			ioObj->CloseFloppy(hDisk);
			return 1;
		break;
	}

	if ((bufn = (char *)calloc(1,4096)) == NULL){
		fprintf(stderr,"No enough memory.\n");
		ioObj->CloseFloppy(hDisk);
		return 1;
	}
	buff = bufn;

	if ((fp = fopen(file,"rb")) == NULL){
		fprintf(stderr,"Can't open input Virtual disk file\n");
		free(bufn);
		ioObj->CloseFloppy(hDisk);
		return 1;
	}

	fprintf(stderr,"Copying Real disk from virtual disk file...\n");

	for (i = 0;i < end;i++){
		if (fread(bufn, 512,1,fp) == 0){
			fprintf(stderr,"Can't read Virtual disk file.\n");
			fclose(fp);
			free(bufn);
			ioObj->CloseFloppy(hDisk);

			return 1;
		} else {
			if (abswrite(buff,drive,i,1)){
				/* write8(bufn,drive,i*8); */
				fprintf(stderr,"Can't write Real disk sector %d\n", i);
			}
		}
	}
	ioObj->CloseFloppy(hDisk);

	fclose(fp);
	free(bufn);

	return(0);
}

/* 仮想ディスクファイルを使い、実ディスクのブートセクタを更新 */
int copyvb(char *file,char *srcd)
{
	char *buff;
	char *bufn;
	FILE *fp;
	unsigned char drive;
	int m2;

	if (isalpha(srcd[0])){
		drive = toupper(srcd[0]) - 'A';
	}else{
		drive = 0;
	}

	hDisk = ioObj->OpenFloppy(drive, GENERIC_READ | GENERIC_WRITE);
	if (hDisk == NULL) {
		fprintf(stderr,"Can't open Real disk.\n");
		return 1;
	}

	m2 = chk2dd(drive);
	if (m2 == 0){
		fprintf(stderr,"Real disk isn't 2DD 9/8 sector format.\n");
		ioObj->CloseFloppy(hDisk);
		return 1;
	}

	if ((bufn = (char *)calloc(1,4096)) == NULL){
		fprintf(stderr,"No enough memory.\n");
		ioObj->CloseFloppy(hDisk);
		return 1;
	}
	buff = bufn;

	if ((fp = fopen(file,"rb")) == NULL){
		fprintf(stderr,"Can't open input Virtual disk file\n");
		free(bufn);
		ioObj->CloseFloppy(hDisk);
		return 1;
	}

	fprintf(stderr,"Copying real disk's Boot sector from virtual disk file's Boot sector...\n");

	if (fread(bufn,512,1,fp) == 0){
		fprintf(stderr,"Can't read Virtual disk file.\n");
		free(bufn);
		ioObj->CloseFloppy(hDisk);
		return 1;
	}

	if (abswrite(buff,drive,0,1)){
		fprintf(stderr,"Can't write Real disk.\n");
		free(bufn);
		ioObj->CloseFloppy(hDisk);
		return 1;
	}
	ioObj->CloseFloppy(hDisk);

	fclose(fp);
	free(bufn);

	return(0);
}

/* 仮想ディスクを作る */
int makenewdisk(char *bufn,FILE *fp,int media)
{
	int i,j,sec,fat;
	unsigned char id;

	switch (media){
		case 0: /* 2DD,9sec. */
			id = 0xf9;
			sec = 1426;
			fat = 3;
		break;
		case 1: /* 2DD,8sec. */
			id = 0xfb;
			sec = 1268;
			fat = 2;
		break;
		case 2: /* 1DD,9sec */
			id = 0xf8;
			sec = 708;
			fat = 2;
		break;
		case 3: /* 1DD,8sec */
			id = 0xfa;
			sec = 630;
			fat = 1;
		break;
		default:
			id = 0xf9;
			sec = 1426;
			fat = 3;
		break;
	}

	for (i = 0;i < 1024;i++){
		bufn[i] = 0;
	}
	/* FAT */
	bufn[0] = id;
	bufn[1] = -1;
	bufn[2] = -1;

	for (i = 0;i < 2;i++){
		if (fwrite(bufn,512,1,fp) == 0){
			fprintf(stderr,"Can't write Virtual disk file.\n");
			free(bufn);
			return 1;
		}
		for (j = 0;j < (fat-1);j++){
			if (fwrite(bufn+512,512,1,fp) == 0){
				fprintf(stderr,"Can't write Virtual disk file.\n");
				free(bufn);
				return 1;
			}
		}
	}

	/* dir & data */
	sec = sec + 7;
	for (i = 0;i < sec;i++){
		if (fwrite(bufn+512,512,1,fp) == 0){
			fprintf(stderr,"Can't write Virtual disk file.\n");
			free(bufn);
			return 1;
		}
	}

	fclose(fp);
	free(bufn);

	return(0);
}

/* 実ディスクから何もない仮想ディスクを作る */
int newr(char *srcd,char *file,int media)
{
	char *buff;
	char *bufn;
	int m2;
	FILE *fp;
	unsigned char drive;

	if (isalpha(srcd[0])){
		drive = toupper(srcd[0]) - 'A';
	}else{
		drive = 0;
	}

	hDisk = ioObj->OpenFloppy(drive, GENERIC_READ | GENERIC_WRITE);
	if (hDisk == NULL) {
		fprintf(stderr,"Can't open Real disk.\n");
		return 1;
	}

	m2 = chk2dd(drive);
	if (m2 == 0){
		fprintf(stderr,"Source disk isn't 2DD 9/8 sector format.\n");
		ioObj->CloseFloppy(hDisk);
		return 1;
	}

	if ((bufn = (char *)calloc(1,4096)) == NULL){
		fprintf(stderr,"No enough memory.\n");
		ioObj->CloseFloppy(hDisk);
		return 1;
	}
	buff = bufn;

	if ((fp = fopen(file,"wb")) == NULL){
		fprintf(stderr,"Can't open output Virtual disk file\n");
		free(bufn);
		ioObj->CloseFloppy(hDisk);
		return 1;
	}

	fprintf(stderr,"Making new virturl disk file from real disk.\n");

	// ブートセクターを読み込む
	/* Boot sector */
	if (absread(buff,drive,0,1)){
		fprintf(stderr,"Can't read Real disk.\n");
		free(bufn);
		ioObj->CloseFloppy(hDisk);

		return 1;
	}
	ioObj->CloseFloppy(hDisk);

	if (fwrite(bufn,512,1,fp) == 0){
		fprintf(stderr,"Can't write Virtual disk file.\n");
		free(bufn);
		return 1;
	}

	makenewdisk(bufn,fp,media);

	return(0);
}

/* 仮想ディスクから何もない仮想ディスクを作る */
int newv(char *srcd,char *file,int media)
{
	char *bufn;
	FILE *fp,*fp2;

	if ((bufn = (char *)calloc(1,4096)) == NULL){
		fprintf(stderr,"No enough memory.\n");
		return 1;
	}

	if ((fp = fopen(file,"wb")) == NULL){
		fprintf(stderr,"Can't open output Virtual disk file\n");
		free(bufn);
		return 1;
	}

	if ((fp2 = fopen(srcd,"rb")) == NULL){
		fprintf(stderr,"Can't open output Virtual disk file\n");
		free(bufn);
		return 1;
	}

	fprintf(stderr,"Making New virtual disk file from virturl disk file.\n");

	/* Boot sector */
	if (fread(bufn,512,1,fp2) == 0){
		fprintf(stderr,"Can't read Virtual disk file.\n");
		free(bufn);
		return 1;
	}
	if (fwrite(bufn,512,1,fp) == 0){
		fprintf(stderr,"Can't write Virtual disk file.\n");
		free(bufn);
		return 1;
	}
	fclose(fp2);

	makenewdisk(bufn,fp,media);

	return(0);
}

/* 実ディスクのブートセクタをセーブする */
int savebs(char *srcd,char *file)
{
	char *buff;
	char *bufn;
	int m2;
	FILE *fp;
	unsigned char drive;

	if (isalpha(srcd[0])){
		drive = toupper(srcd[0]) - 'A';
	}else{
		drive = 0;
	}

	hDisk = ioObj->OpenFloppy(drive, GENERIC_READ | GENERIC_WRITE);
	if (hDisk == NULL) {
		return 1;
	}

	m2 = chk2dd(drive);
	switch (m2){
		case 1:
		case 2:
		break;
		default:
			fprintf(stderr,"Source disk isn't 2DD 9/8 sector format.\n");
			ioObj->CloseFloppy(hDisk);
			return 1;
		break;
	}

	if ((bufn = (char *)calloc(1,4096)) == NULL){
		fprintf(stderr,"No enough memory.\n");
		ioObj->CloseFloppy(hDisk);
		return 1;
	}
	buff = bufn;

	if ((fp = fopen(file,"wb")) == NULL){
		fprintf(stderr,"Can't open output Virtual disk file\n");
		free(bufn);
		ioObj->CloseFloppy(hDisk);
		return 1;
	}

	fprintf(stderr,"Save real disk's boot sector to virturl disk file.\n");

	// 実ディスクのブートセクターを読む
	if (absread(buff,drive,0,1)){
		fprintf(stderr,"Can't read Real disk.\n");
		fclose(fp);
		free(bufn);
		ioObj->CloseFloppy(hDisk);
		return 1;
	}
	ioObj->CloseFloppy(hDisk);

	// ブートセクターを書き出す
	if (fwrite(bufn,512,1,fp) == 0){
		fprintf(stderr,"Can't write Virtual disk file.\n");
		fclose(fp);
		free(bufn);
		return 1;
	}

	fclose(fp);
	free(bufn);

	return(0);
}

void usage(void)
{
	fprintf(stderr,"Usage:PHYLOCP Command P1 P2 options\nCommand:\nV copy real disk to Virtual disk file. P1:Drive P2:File name\nR copy virtual disk file to Real disk. P1:File name P2:Drive\nB copy virtual disk file's Boot sector to real disk. P1:File name P2:Drive\nM Make new virtual disk file from real disk P1:Drive P2:File name\nN make New virtual disk file from virtual disk file P1:Source file name P2:New file name\nS Save real disk's boot sector to file. P1:Drive P2:File name\nOption:\n/M?? :new virtual disk Media type. 29:2DD,9sec. 28:2DD,8sec. 19:1DD,9sec 18:1DD,8sec (M and N command use it.).\n/B* Base boot sector file (B and N command use it.)\n\n");
	exit(1);
}

/* オプション解析関連 */
void exarg(int argc,char *argv[],int st)
{
	int i;
	char *op;

	for (i = st;i < argc;i++){
		if (strnicmp(argv[i],"/M",2) == 0){
			if (strlen(argv[i]) > 2){
				op = argv[i]+2;
			}else{
				i++;
				if (i < argc){
					op = argv[i];
				}else{
					break;
				}
			}
			if (strnicmp(op,"19",2) == 0){
				media = 2;
			}
			if (strnicmp(op,"18",2) == 0){
				media = 3;
			}
			if (strnicmp(op,"28",2) == 0){
				media = 1;
			}
			continue;
		}
		if (strnicmp(argv[i],"/B",2) == 0){
			if (strlen(argv[i]) > 2){
				op = argv[i]+2;
			}else{
				i++;
				if (i < argc){
					op = argv[i];
				}else{
					break;
				}
			}
			strcpy(deffn,op);
			continue;
		}
	}
}

int optstart(int argc,char *argv[])
{
	int i;

	for (i = 0;i < argc;i++){
		if (argv[i][0] == '/'){
			return(i);
		}
	}
	return(-1);
}

char *skipsp(char *s)
{
	while((*s == ' ') || (*s == '\t')){ /* スペースとタブを飛ばす */
		s++;
	}
	if (*s == '\0'){
		return(NULL);
	}else{
		return(s);
	}
}

int toklen(char *s)
{
	int l;

	l = 0;

	while((*s != ' ') && (*s != '\t') && (*s != '\0')){
		s++;
		l++;
	}
	return(l);
}

void exenv(char *env)
{
	char *e2,*op;
	int i;

	e2 = env;
	for (e2 = skipsp(env);e2 != NULL;e2 = e2 + toklen(e2),e2 = skipsp(e2)){
		if (strnicmp(e2,"/M",2) == 0){
			if (toklen(e2) > 2){
				op = e2 + 2;
			}else{
				e2 = e2 + toklen(e2);
				e2 = skipsp(e2);
				if (e2 == NULL){
					break;
				}else{
					op = e2;
				}
			}
			if (strnicmp(op,"19",2) == 0){
				media = 2;
			}
			if (strnicmp(op,"18",2) == 0){
				media = 3;
			}
			if (strnicmp(op,"28",2) == 0){
				media = 1;
			}
			continue;
		}
		if (strnicmp(e2,"/B",2) == 0){
			if (toklen(e2) > 2){
				op = e2 + 2;
			}else{
				e2 = e2 + toklen(e2);
				e2 = skipsp(e2);
				if (e2 == NULL){
					break;
				}else{
					op = e2;
				}
			}
			for (i = 0;i < toklen(op);i++){
				deffn[i] = op[i];
			}
			deffn[i] = '\0';
			continue;
		}
	}
}

/* メインルーチン */
int main(int argc,char *argv[])
{
	int optst,rargc;
	char *env;

	fprintf(stderr,"%s",title + 4);

	if (argc < 2){
		usage();
	}

	OSVERSIONINFO ver;
	ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&ver);
	if (ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS){
		//MessageBox("Windows 9x/Meで起動しました。","プラットフォーム",MB_OK);
		ioObj = new DiskOperation9x();
	}else{
		//MessageBox("Windows NT/2000/XPで起動しました。","プラットフォーム",MB_OK);
		ioObj = new DiskOperationNT();
	}

	/* デフォルトオプション設定 */
	strcpy(deffn,"default.bsc");
	media = 0; /* 2DD,9 sector */
	machine = 0;  /* Use MS-DOS system call */

	/* 環境変数オプション解析 */
	if ((env = getenv("PLCP")) != NULL){
		exenv(env);
	}

	/* コマンドラインオプション解析 */
	optst = optstart(argc,argv);
	if (optst != -1){
		exarg(argc,argv,optst);
		rargc = optst;
	}else{
		rargc = argc;
	}

	fprintf(stderr,"Default Base boot sector file is %s\n",deffn);

	switch (toupper(argv[1][0])){
		case 'V': /* copy real disk to Virtual disk file */
			if (rargc < 4){
				usage();
			}else{
				copyrv(argv[2],argv[3]);
			}
		break;
		case 'R': /* copy virtual disk file to Real disk */
			if (rargc < 4){
				usage();
			}else{
				copyvr(argv[2],argv[3]);
			}
		break;
		case 'B': /* copy virtual disk file's Boot sector to real disk */
			if (rargc > 3){
				copyvb(argv[2],argv[3]);
			}else{
				if (rargc == 3){
					copyvb(deffn,argv[2]);
				}else{
					usage();
				}
			}
		break;
		case 'M': /* Make new virtual disk file from Real disk */
			if (rargc < 4){
				usage();
			}else{
				fprintf(stderr,"New virtual disk MEDIA type is %d\n\n",media);
				newr(argv[2],argv[3],media);
			}
		break;
		case 'N': /* make New virtual disk file from Virtual disk file */
			if (rargc > 3){
				fprintf(stderr,"New virtual disk MEDIA type is %d\n\n",media);
				newv(argv[2],argv[3],media);
			}else{
				if (rargc == 3){
					fprintf(stderr,"New virtual disk MEDIA type is %d\n\n",media);
					newv(deffn,argv[2],media);
				}else{
					usage();
				}
			}
		break;
		case 'S': /* Save real disk's boot sector to file */
			if (rargc < 4){
				usage();
			}else{
				savebs(argv[2],argv[3]);
			}
		break;
		default:;
			usage();
		break;
	}

	delete ioObj;

	return(0);
}
