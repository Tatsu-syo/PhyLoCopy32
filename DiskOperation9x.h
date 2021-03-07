/*
PhyLoCopy32 (C) 1995-2000,2021 Tatsuhiko Shoji
The sources for PhyLoCopy32 are distributed under the MIT open source license
*/
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>
#include <windows.h>
#include <winioctl.h>

/* VWIN32.VXD Control Code */
#define VWIN32_DIOC_DOS_IOCTL		1			/* DOS Int21 0x44xxh IOCTL */
#define VWIN32_DIOC_DOS_INT25		2			/* DOS Int25(Absolute Read) */
#define VWIN32_DIOC_DOS_INT26		3			/* DOS Int26(Absolute Write) */
#define VWIN32_DIOC_DOS_INT13		4			/* DOS Int13 */

/* ���W�X�^�i�[�̈� */
typedef struct {
	DWORD reg_EBX;
	DWORD reg_EDX;
	DWORD reg_ECX;
	DWORD reg_EAX;
	DWORD reg_EDI;
	DWORD reg_ESI;
	DWORD reg_Flags;
} DIOC_REGISTERS, *PDIOC_REGISTERS;

class DiskOperation9x : public DiskOperation {
private:
	/* IOCTL�����s����B */
	LRESULT Ioctl(HANDLE hDisk,int iCtlCode, PDIOC_REGISTERS pRegs);

public:
	/* �t���b�s�[�f�B�X�N���I�[�v������B */
	HANDLE OpenFloppy(UINT uDrive, DWORD dwMode);
	/* �t���b�s�[���N���[�Y����B */
	void CloseFloppy(HANDLE hDisk);
	/* �Z�N�^���������� */
	BOOL writeSector(HANDLE hDisk,int drive,int sectorNo, char *buf);
	/* �Z�N�^��ǂݍ��� */
	BOOL readSector(HANDLE hDisk,int drive,int sectorNo, char *buf);


	DiskOperation9x();
};
