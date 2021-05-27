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
#include <Core/Core.h>

#if defined (__unix__)
#include <sys/stat.h>
#define _mkdir(str)				mkdir(str, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#else
#include <direct.h>
#endif

struct CDImage_t
{
	FILE* fp;		// any poitner
	int sectorSize; // CD_SECTOR_SIZE_MODE2
};

// dumps ISO 9660 compatible image file into target folder
void DumpFiles_r(CDImage_t* im, Sector* sector, const char* curPath)
{
	char nameStr[256];
	TOC* toc;
	int tocLocalOffset;

	toc = (TOC*)&sector->data[0];
	tocLocalOffset = 0;

	if(*curPath == '\\')
		curPath++;

	// create directory structure
	_mkdir(curPath);

	while (toc->tocEntryLength != 0)
	{
		char* itemNameStr = (char*)&sector->data[tocLocalOffset + sizeof(TOC)];
		
		// skip . and ..
		if (*itemNameStr == 0 || *itemNameStr == 1)
		{
			tocLocalOffset += toc->tocEntryLength;
			toc = (TOC*)&sector->data[tocLocalOffset];

			continue;
		}

		if (toc->flags & 2) // is directory
		{
			// read a sector
			Sector nsector;
			sprintf(nameStr, "%s\\%s", curPath, itemNameStr);

			fseek(im->fp, toc->sectorPosition[0] * im->sectorSize, SEEK_SET);
			fread(&nsector, sizeof(Sector), 1, im->fp);

			DumpFiles_r(im, &nsector, nameStr);
		}
		else
		{
			Sector nsector;

			int remainingBytes = toc->fileSize[0];

			// seek to start
			fseek(im->fp, toc->sectorPosition[0] * im->sectorSize, SEEK_SET);
			fread(&nsector, sizeof(Sector), 1, im->fp);

			int interleaved = nsector.subHead[0] == 1;

			// [A] don't extract form 2
			if(!interleaved)
			{
				sprintf(nameStr, "%s\\%s", curPath, itemNameStr);

				char* verStart = strchr(nameStr, ';');
				if (verStart)
					*verStart = '\0';

				// save file
				FILE* fp = fopen(nameStr, "wb");
				if (fp)
				{
					do
					{
						fread(&nsector, sizeof(Sector), 1, im->fp);

						if (!interleaved)
						{
							Sector* pSector = &nsector;
							remainingBytes -= sizeof(pSector->data);

							fwrite(pSector->data, 1, sizeof(pSector->data), fp);
						}
						else
						{
							// extract whole sectors?
							// FIXME: wtf with file sizes?
							AudioSector* pSector = (AudioSector*)&nsector;
							remainingBytes -= sizeof(pSector->data);

							fwrite(pSector, 1, sizeof(AudioSector), fp);
						}

						fread(&nsector, sizeof(Sector), 1, im->fp);

					} while (remainingBytes > 0);
				}

				// XA flag: (nsector.subHead[2] & (1 << 5)) ? 2 : 1
				// form 2 flag: nsector.subHead[0] & 1

				/*printf("%s\\%s - form %d - %x %x %x %x %x %x %x %x\n", curPath, itemNameStr, (nsector.subHead[0] & 1) ? 2 : 1,
					nsector.subHead[0],
					nsector.subHead[1],
					nsector.subHead[2],
					nsector.subHead[3],
					nsector.subHead[4],
					nsector.subHead[5],
					nsector.subHead[6],
					nsector.subHead[7]);*/
			}
		}

		// next file
		tocLocalOffset += toc->tocEntryLength;
		toc = (TOC*)&sector->data[tocLocalOffset];
	}
}

bool DumpImageFile(const char* filename, const char* targetFolder)
{
	CDImage_t im;
	
	im.sectorSize = CD_SECTOR_SIZE_MODE2;
	im.fp = fopen(filename, "rb");
	
	if(!im.fp)
		return false;
	
	// read first sector and proceed
	Sector sector;
	fseek(im.fp, CD_ROOT_DIRECTORY_SECTOR * im.sectorSize, SEEK_SET);
	fread(&sector, sizeof(Sector), 1, im.fp);
	
	DumpFiles_r(&im, &sector, "");
	
	fclose(im.fp);
	
	return true;
}