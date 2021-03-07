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

	/* ディスクボリューム制御 */
	LRESULT IoctlDiskVolume(HANDLE hDisk, DWORD dwControlCode);

public:
	/* フロッピーディスクをオープンする。 */
	HANDLE OpenFloppy(UINT uDrive, DWORD dwMode);
	/* フロッピーをクローズする。 */
	void CloseFloppy(HANDLE hDisk);
	/* セクタを書き込む */
	BOOL writeSector(HANDLE hDisk,int drive,int sectorNo, char *buf);
	/* セクタを読み込む */
	BOOL readSector(HANDLE hDisk,int drive,int sectorNo,char *buf);
	
	DiskOperationNT();
};
