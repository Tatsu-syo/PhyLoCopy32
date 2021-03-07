/*
PhyLoCopy32 (C) 1995-2000,2021 Tatsuhiko Shoji
The sources for PhyLoCopy32 are distributed under the MIT open source license
*/
#include "DiskOperation.h"
#include "DiskOperationNT.h"
#include <time.h>

#define FsctlLockVolume(h)		IoctlDiskVolume(h, FSCTL_LOCK_VOLUME)
#define FsctlUnlockVolume(h)	IoctlDiskVolume(h, FSCTL_UNLOCK_VOLUME)
#define FsctlDismountVolume(h)	IoctlDiskVolume(h, FSCTL_DISMOUNT_VOLUME)

#define BUFSIZE 512 * 15

DiskOperationNT::DiskOperationNT()
{
}

/*-----------------------------------------------------------------------------
	ディスクボリューム制御
-----------------------------------------------------------------------------*/
LRESULT DiskOperationNT::IoctlDiskVolume(HANDLE hDisk, DWORD dwControlCode)
{
	DWORD	dwRetByteCount;
	BOOL	fResult;
	LRESULT lResult = ERROR_SUCCESS;

	fResult = DeviceIoControl(hDisk,
			dwControlCode, NULL, 0, NULL, 0, &dwRetByteCount, NULL);
	if(fResult == FALSE){
		lResult = GetLastError();
	}
	return lResult;
}

/* フロッピーディスクをオープンする。 */
// TODO:自分の言葉でコメントを整理しよう。
HANDLE DiskOperationNT::OpenFloppy(UINT uDrive, DWORD dwMode)
{
	HANDLE	hDisk;
	char	szFileName[16];
	LRESULT lResult = ERROR_SUCCESS;

	__try{
		wsprintf(szFileName, "\\\\.\\%c:", uDrive + 'A');
		hDisk = CreateFile(szFileName, dwMode, FILE_SHARE_READ | FILE_SHARE_WRITE ,NULL, OPEN_EXISTING, 0,NULL);
		if(hDisk == INVALID_HANDLE_VALUE){
			lResult = GetLastError();
			hDisk = NULL;
			__leave;
		}
		if((lResult = FsctlLockVolume(hDisk)) != ERROR_SUCCESS){
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

/*-----------------------------------------------------------------------------
	FDをクローズする
-----------------------------------------------------------------------------*/
void DiskOperationNT::CloseFloppy(HANDLE hDisk)
{
	if(hDisk){
		FsctlDismountVolume(hDisk);
		FsctlUnlockVolume(hDisk);
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
BOOL DiskOperationNT::writeSector(HANDLE hDisk,int drive,int sectorNo, char *buf)
{
	DWORD written;
	BOOL result;

	SetFilePointer(hDisk, sectorNo * 512, NULL, 0);
	result = WriteFile(hDisk,buf, 512, &written, NULL);

	return result;
}

/**
 * セクタを読み込む
 *
 * @param hDisk ディスクのハンドル
 * @param sectorNo セクタ番号
 * @param buf 読み込んだ内容の格納先
 * @return TRUE:読み込み成功 FALSE:読み込み失敗
 */
BOOL DiskOperationNT::readSector(HANDLE hDisk,int drive,int sectorNo,char *buf)
{
	DWORD written;
	BOOL result;
	
	SetFilePointer(hDisk, sectorNo * 512, NULL, 0);
	result = ReadFile(hDisk, buf, 512, &written, NULL);

	return result;
}

