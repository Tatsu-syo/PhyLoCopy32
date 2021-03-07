/*
PhyLoCopy32 (C) 1995-2000,2021 Tatsuhiko Shoji
The sources for PhyLoCopy32 are distributed under the MIT open source license
*/
#include	<windows.h>

class DiskOperation {
public:
	/* フロッピーディスクをオープンする。 */
	virtual HANDLE OpenFloppy(UINT uDrive, DWORD dwMode) = 0;
	/* フロッピーをクローズする。 */
	virtual void CloseFloppy(HANDLE hDisk) = 0;
	/* セクタを書き込む */
	virtual BOOL writeSector(HANDLE hDisk,int drive,int sectorNo, char *buf) = 0;
	/* セクタを読み込む */
	virtual BOOL readSector(HANDLE hDisk,int drive,int sectorNo,char *buf) = 0;

};
