/*
PhyLoCopy32 (C) 1995-2000,2021 Tatsuhiko Shoji
The sources for PhyLoCopy32 are distributed under the MIT open source license
*/
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winioctl.h>

class DiskOperationNT : public DiskOperation {
private:

	/* �f�B�X�N�{�����[������ */
	LRESULT IoctlDiskVolume(HANDLE hDisk, DWORD dwControlCode);

public:
	/* �t���b�s�[�f�B�X�N���I�[�v������B */
	HANDLE OpenFloppy(UINT uDrive, DWORD dwMode);
	/* �t���b�s�[���N���[�Y����B */
	void CloseFloppy(HANDLE hDisk);
	/* �Z�N�^���������� */
	BOOL writeSector(HANDLE hDisk,int drive,int sectorNo, char *buf);
	/* �Z�N�^��ǂݍ��� */
	BOOL readSector(HANDLE hDisk,int drive,int sectorNo,char *buf);
	
	DiskOperationNT();
};
