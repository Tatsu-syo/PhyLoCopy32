/*
PhyLoCopy32 (C) 1995-2000,2021 Tatsuhiko Shoji
The sources for PhyLoCopy32 are distributed under the MIT open source license
*/
#include	<windows.h>

class DiskOperation {
public:
	/* �t���b�s�[�f�B�X�N���I�[�v������B */
	virtual HANDLE OpenFloppy(UINT uDrive, DWORD dwMode) = 0;
	/* �t���b�s�[���N���[�Y����B */
	virtual void CloseFloppy(HANDLE hDisk) = 0;
	/* �Z�N�^���������� */
	virtual BOOL writeSector(HANDLE hDisk,int drive,int sectorNo, char *buf) = 0;
	/* �Z�N�^��ǂݍ��� */
	virtual BOOL readSector(HANDLE hDisk,int drive,int sectorNo,char *buf) = 0;

};
