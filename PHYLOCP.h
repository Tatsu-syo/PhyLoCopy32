/*
PhyLoCopy32 (C) 1995-2000,2021 Tatsuhiko Shoji
The sources for PhyLoCopy32 are distributed under the MIT open source license
*/
/* �ϐ� */
/* �I�v�V���� */
extern char deffn[64];
extern int media,machine;
extern int dtl; /* �f�[�^�� */

extern char *er_info; /* BIOS�g�p���̃G���[�����i�[����B */

/* �֐� */
/* �ėp�֐� */
void far *nptofp(void *);

/* ��`�̂����� */
#define SECSIZE 512
