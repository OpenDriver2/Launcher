// Basic types are here because i'm lazy
#ifndef _UCHAR_T
#define _UCHAR_T
typedef	unsigned char	u_char;
#endif
#ifndef _USHORT_T
#define _USHORT_T
typedef	unsigned short	u_short;
#endif
#ifndef _UINT_T
#define _UINT_T
typedef	unsigned int	u_int;
#endif
#ifndef _ULONG_T
#define _ULONG_T
typedef	unsigned long	u_long;
#endif

#include "iso9660.h"
#include <stdio.h>
#include <string.h>
#include <Core/Core.h>

#if defined (__unix__)
#	include <sys/stat.h>
#	define _mkdir(str)				mkdir(str, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#	define PATH_SEPARATOR '/'
#else
#	include <direct.h>
#	define PATH_SEPARATOR '\\'
#endif

struct CDImage_t
{
	FILE* fp;		// any poitner
	int sectorSize; // CD_SECTOR_SIZE_MODE2
};

typedef bool (*fileFunc_t)(CDImage_t* im, TOC* toc, const char* filename, void* userData);
typedef bool (*folderFunc_t)(CDImage_t* im, TOC* toc, const char* foldername, void* userData);

// walks through all files inside ISO 9660 filesystem
bool WalkFiles_r(CDImage_t* im, const char* curPath, fileFunc_t fileFunc, folderFunc_t folderFunc = NULL, void* userData = NULL)
{
	Sector sector;
	char nameStr[512];
	TOC* toc;
	int tocLocalOffset;

	// read TOC from sector
	fread(&sector, sizeof(Sector), 1, im->fp);
	toc = (TOC*)&sector.data[0];
	tocLocalOffset = 0;
	
	// useful for mkdir
	if(folderFunc)
		folderFunc(im, toc, curPath, userData);

	while (toc->tocEntryLength != 0)
	{
		char* itemNameStr = (char*)&sector.data[tocLocalOffset + sizeof(TOC)];
		
		// skip . and ..
		if (*itemNameStr == 0 || *itemNameStr == 1)
		{
			tocLocalOffset += toc->tocEntryLength;
			toc = (TOC*)&sector.data[tocLocalOffset];

			continue;
		}

		if (toc->flags & 2) // is directory
		{
			// read a sector
			fseek(im->fp, toc->sectorPosition[0] * im->sectorSize, SEEK_SET);

			// append directory
			sprintf(nameStr, "%s%c%s", curPath, PATH_SEPARATOR, itemNameStr);
			
			// enter directory and walk here
			if(!WalkFiles_r(im, nameStr, fileFunc, folderFunc, userData))
				return false;
		}
		else
		{
			// seek to file start
			fseek(im->fp, toc->sectorPosition[0] * im->sectorSize, SEEK_SET);
			
			// append directory
			sprintf(nameStr, "%s%c%s", curPath, PATH_SEPARATOR, itemNameStr);
			
			// exec callback
			if(!fileFunc( im, toc, nameStr, userData ))
				return false;
		}
		
		tocLocalOffset += toc->tocEntryLength;
		toc = (TOC*)&sector.data[tocLocalOffset];
	}
	
	return true;
}

// callback function for creating folder
bool CreateFolderFunc(CDImage_t* im, TOC* toc, const char* folderName, void* userData)
{
	if(*folderName == PATH_SEPARATOR)
		folderName++;
	
	_mkdir(folderName);
	return true;
}

// callback function for dumping a file
bool DumpFileFunc(CDImage_t* im, TOC* toc, const char* filename, void* userData)
{
	Sector sector;
	int interleaved;
	
	// read first sector
	fread(&sector, sizeof(Sector), 1, im->fp);

	interleaved = sector.subHead[0] == 1;
	u_int remainingBytes = toc->fileSize[0];
	
	if(*filename == PATH_SEPARATOR)
		filename++;
	
	// skip few files
	// TODO: detect language here!!!!
	if( strstr(filename, "SYSTEM.CNF") ||
		strstr(filename, "SLES_") ||
		strstr(filename, "SLUS_") ||
		strstr(filename, "FMV.EXE") ||
		strstr(filename, "FRONTEND.BIN") ||
		strstr(filename, "LEAD.BIN") ||
		strstr(filename, "PATH.BIN") ||
		strstr(filename, "MCARD.BIN") ||
		strstr(filename, "ATTRACT.40") )
	{
		return true;
	}
	
	// don't extract form 2
	if(!interleaved)
	{
		char* verStart = (char*)strchr(filename, ';');
		if (verStart)
			*verStart = '\0';

		// save file
		FILE* fp = fopen(filename, "wb");
		if (fp)
		{
			do
			{
				if (!interleaved)
				{
					Sector* pSector = &sector;
					
					int writeCnt = remainingBytes;
					if(writeCnt > sizeof(pSector->data))
						writeCnt = sizeof(pSector->data);
					
					fwrite(pSector->data, 1, writeCnt, fp);
					
					remainingBytes -= sizeof(pSector->data);
				}
#if 0
				else
				{
					// extract whole sectors?
					// FIXME: wtf with file sizes?
					AudioSector* pSector = (AudioSector*)&sector;

					int writeCnt = remainingBytes;
					if(writeCnt > sizeof(pSector->data))
						writeCnt = sizeof(pSector->data);

					fwrite(pSector, 1, writeCnt, fp);
					
					remainingBytes -= sizeof(AudioSector); //sizeof(pSector->data);

				}
#endif
				
				// read next sector
				fread(&sector, sizeof(Sector), 1, im->fp);

			} while (remainingBytes > 0);
			
			fclose(fp);
		}

		// XA flag: (sector.subHead[2] & (1 << 5)) ? 2 : 1
		// form 2 flag: sector.subHead[0] & 1

		/*printf("%s\\%s - form %d - %x %x %x %x %x %x %x %x\n", curPath, itemNameStr, (sector.subHead[0] & 1) ? 2 : 1,
			sector.subHead[0],
			sector.subHead[1],
			sector.subHead[2],
			sector.subHead[3],
			sector.subHead[4],
			sector.subHead[5],
			sector.subHead[6],
			sector.subHead[7]);*/
	}
	
	return true;
}

// callback function for validating game files
bool ValidateFileFunc(CDImage_t* im, TOC* toc, const char* filename, void* userData)
{
	const char* sub = strstr(filename, "FONT2.FNT");
	return (sub == 0);
}

// callback function for detecting game language by executable file
bool DetectGameLanguageFunc(CDImage_t* im, TOC* toc, const char* filename, void* userData)
{
	int* languageId = (int*)userData;
	if(strstr(filename, "SLUS_"))
	{
		*languageId = 0;	// ENGLISH, US
		return false;
	}
	else
	{
		const char* sub = strstr(filename, "SLES");
		
		if(sub)
		{
			sub = strchr(sub, '.');
			if(sub)
			{
				switch(sub[2])
				{
					case '3':
						*languageId = 0;	// ENGLISH, UK
						break;
					case '4':
						*languageId = 3;	// FRENCH
						break;
					case '5':
						*languageId = 2;	// GERMAN
						break;
					case '6':
						*languageId = 1;	// ITALIAN
						break;
					case '7':
						*languageId = 4;	// SPANISH
						break;
				}
				return false;
			}
		}
	}
	return true;
}

int DetectDriver2Language(const char* filename)
{
	int languageId = -1;
	CDImage_t im;
	
	im.sectorSize = CD_SECTOR_SIZE_MODE2;
	im.fp = fopen(filename, "rb");
	
	if(!im.fp)
		return false;
	
	// goto first sector and proceed
	fseek(im.fp, CD_ROOT_DIRECTORY_SECTOR * im.sectorSize, SEEK_SET);
	if(!WalkFiles_r(&im, "", DetectGameLanguageFunc, NULL, &languageId))
	{
		fclose(im.fp);
		return languageId;
	}
	
	fclose(im.fp);
	return languageId;
}

// validates file
bool CheckIsDriver2GameImage(const char* filename)
{
	CDImage_t im;
	
	im.sectorSize = CD_SECTOR_SIZE_MODE2;
	im.fp = fopen(filename, "rb");
	
	if(!im.fp)
		return false;
	
	// goto first sector and proceed
	fseek(im.fp, CD_ROOT_DIRECTORY_SECTOR * im.sectorSize, SEEK_SET);
	if(!WalkFiles_r(&im, "", ValidateFileFunc))
	{
		fclose(im.fp);
		return true;
	}
	
	fclose(im.fp);
	return false;
}

// dumps entire ISO-9660 filesystem to current working folder
bool DumpImageFile(const char* filename)
{
	CDImage_t im;
	
	im.sectorSize = CD_SECTOR_SIZE_MODE2;
	im.fp = fopen(filename, "rb");
	
	if(!im.fp)
		return false;
	
	// goto first sector and proceed
	fseek(im.fp, CD_ROOT_DIRECTORY_SECTOR * im.sectorSize, SEEK_SET);
	WalkFiles_r(&im, "", DumpFileFunc, CreateFolderFunc);
	
	fclose(im.fp);
	
	return true;
}