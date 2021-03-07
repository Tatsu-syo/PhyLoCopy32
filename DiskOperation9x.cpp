/*
PhyLoCopy32 (C) 1995-2000,2021 Tatsuhiko Shoji
The sources for PhyLoCopy32 are distributed under the MIT open source license
*/
#include "DiskOperation.h"
#include "DiskOperation9x.h"
#include <time.h>

#define BUFSIZE 512 * 15

DiskOperation9x::DiskOperation9x()
{
}

/* フロッピーディスクをオープンする。 */
HANDLE DiskOperation9x::OpenFloppy(UINT uDrive, DWORD dwMode)
{
	HANDLE	hDisk;
	char	szFileName[16];
	LRESULT lResult = ERROR_SUCCESS;

	__try{
		wsprintf(szFileName, "\\\\.\\vwin32");
		hDisk = CreateFile(szFileName, 0, 0,NULL, 0,FILE_FLAG_DELETE_ON_CLOSE, NULL);
		if(hDisk == INVALID_HANDLE_VALUE){
			lResult = GetLastError();
			hDisk = NULL;
			__leave;
		}
	}
	__finally{
		if(lResult != ERROR_SUCCESS){
			if(hDisk){
				CloseHandle(hDisk);
				hDisk = NULL;
			}
		}
	}

	SetLastError(lResult);
	return hDisk;
}


/*----------------------------------------------------------------------------
	IOCTLを実行する
----------------------------------------------------------------------------*/
LRESULT DiskOperation9x::Ioctl(HANDLE hDisk,int iCtlCode,PDIOC_REGISTERS pRegs)
{
	DWORD			dwReturned;
	BOOL			fResult;

	fResult = DeviceIoControl(hDisk, iCtlCode,
			pRegs, sizeof(DIOC_REGISTERS),
			pRegs, sizeof(DIOC_REGISTERS),
			&dwReturned, 0);
	if(!fResult){
		return GetLastError();
	}

	if(pRegs->reg_Flags & 0x0001){
//		printf("IOCTLが異常終了した (code = %X)\n", pRegs->reg_EAX);
		return ERROR_IO_PENDING;
	}

	return ERROR_SUCCESS;
}

/*-----------------------------------------------------------------------------
	FDをクローズする
-----------------------------------------------------------------------------*/
void DiskOperation9x::CloseFloppy(HANDLE hDisk)
{
	if(hDisk){
		CloseHandle(hDisk);
	}
}

/**
 * セクタを書き込む
 *
 * @param hDisk ディスクのハンドル
 * @param sectorNo セクタ番号
 * @param buf 書き込む内容
 * @return TRUE:書き込み成功 FALSE:書き込み失敗
 */
BOOL DiskOperation9x::writeSector(HANDLE hDisk,int drive,int sectorNo, char *buf)
{
	DIOC_REGISTERS regs;
	BOOL result = TRUE;

	/* floppy disk lock */
	regs.reg_EAX   = (DWORD)0x440D;					/* ブロックデバイスIOCTL */
	regs.reg_EBX   = (DWORD)(0x0100 | (drive + 1));	/* ロックレベル,ドライブ */
	regs.reg_ECX   = (DWORD)(0x0800 | 0x4a);	/* マイナーコード */
	regs.reg_EDX   = (DWORD)0x0002;					/* アクセス権 */
	regs.reg_Flags = (DWORD)0x0001;					/* キャリーフラグを設定 */
	Ioctl(hDisk,VWIN32_DIOC_DOS_IOCTL, &regs);

	regs.reg_EAX   = (DWORD)drive;				/* Drive A: */
	regs.reg_EBX   = (DWORD)buf;				/* 転送アドレス */
	regs.reg_ECX   = (DWORD)1;					/* セクタ数 */
	regs.reg_EDX   = (DWORD)sectorNo;			/* 開始セクタ */
	regs.reg_Flags = (DWORD)0x0001;				/* キャリーフラグを設定 */
	LRESULT writeResult = Ioctl(hDisk,VWIN32_DIOC_DOS_INT26, &regs);
	if (writeResult != ERROR_SUCCESS) {
		result = FALSE;
/*
AH = 80h if attachment failed to respond
 40h if seek operation failed
 20h if controller failed
 10h if data error (bad CRC)
 08h if DMA failure
 04h if requested sector not found
 03h if write-protected disk
 02h if bad address mark
 01h if bad command
*/
	} else {
		result = TRUE;
	}

	/* floppy disk un lock */
	regs.reg_EAX   = (DWORD)0x440D;					/* ブロックデバイスIOCTL */
	regs.reg_EBX   = (DWORD)(0x0100 | (drive + 1));	/* ロックレベル,ドライブ */
	regs.reg_ECX   = (DWORD)(0x0800 | 0x6a);		/* マイナーコード */
	regs.reg_EDX   = (DWORD)0x0002;					/* アクセス権 */
	regs.reg_Flags = (DWORD)0x0001;					/* キャリーフラグを設定 */
	Ioctl(hDisk,VWIN32_DIOC_DOS_IOCTL, &regs);

	return result;
}

/**
 * セクタを読み込む
 *
 * @param hDisk ディスクのハンドル
 * @param sectorNo セクタ番号
 * @param buf 書き込む内容
 * @return TRUE:書き込み成功 FALSE:書き込み失敗
 */
BOOL DiskOperation9x::readSector(HANDLE hDisk,int drive,int sectorNo, char *buf)
{
	DIOC_REGISTERS regs;
	BOOL result = TRUE;

	/* floppy disk lock */
	regs.reg_EAX   = (DWORD)0x440D;					/* ブロックデバイスIOCTL */
	regs.reg_EBX   = (DWORD)(0x0100 | (drive + 1));	/* ロックレベル,ドライブ */
	regs.reg_ECX   = (DWORD)(0x0800 | 0x4a);	/* マイナーコード */
	regs.reg_EDX   = (DWORD)0x0002;					/* アクセス権 */
	regs.reg_Flags = (DWORD)0x0001;					/* キャリーフラグを設定 */
	Ioctl(hDisk,VWIN32_DIOC_DOS_IOCTL, &regs);

	regs.reg_EAX   = (DWORD)drive;				/* Drive A: */
	regs.reg_EBX   = (DWORD)buf;				/* 転送アドレス */
	regs.reg_ECX   = (DWORD)1;					/* セクタ数 */
	regs.reg_EDX   = (DWORD)sectorNo;			/* 開始セクタ */
	regs.reg_Flags = (DWORD)0x0001;				/* キャリーフラグを設定 */
	LRESULT readEesult = Ioctl(hDisk,VWIN32_DIOC_DOS_INT25, &regs);
	if (readEesult != ERROR_SUCCESS) {
		result = FALSE;
/*
AH = 80h if attachment failed to respond
 40h if seek operation failed
 20h if controller failed
 10h if data error (bad CRC)
 08h if DMA failure
 04h if requested sector not found
 03h if write-protected disk
 02h if bad address mark
 01h if bad command
*/
	} else {
		result = TRUE;
	}

	/* floppy disk un lock */
	regs.reg_EAX   = (DWORD)0x440D;					/* ブロックデバイスIOCTL */
	regs.reg_EBX   = (DWORD)(0x0100 | (drive + 1));	/* ロックレベル,ドライブ */
	regs.reg_ECX   = (DWORD)(0x0800 | 0x6a);		/* マイナーコード */
	regs.reg_EDX   = (DWORD)0x0002;					/* アクセス権 */
	regs.reg_Flags = (DWORD)0x0001;					/* キャリーフラグを設定 */
	Ioctl(hDisk,VWIN32_DIOC_DOS_IOCTL, &regs);

	return result;
}


