//
//  main.c
//  sl2cut
//
//  Created by Vladimir Borisov on 13/05/16.
//  Copyright © 2016. All rights reserved.
//

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#endif // _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>

#ifndef _WIN32
#include <utime.h>
#include <sys/stat.h>
#endif // _WIN32

#include "bool.h"

#include "sl2def.h"
#include "Sl2Utils.h"

#define MAX_CHAN 5
channelStat* stats[MAX_CHAN+1];

// For debug output
//#define VERBOSE 1

BOOL bInfoOnly;
DWORD cutStart;
DWORD cutEnd;

BOOL bPrimary;
BOOL bSecondary;
BOOL bDsi;
BOOL bSide;

BOOL bHideCoordinates;
double fakeN = 46.45;
double fakeE = 6.5;
double shiftN = 0;
double shiftE = 0;

void printStats(channelStat* ptr) {
	if (ptr == NULL) {
		return;
	}
	printf("=== %s, freq %s: %d sounds", (char*)chan(ptr->chan), (char*)freq(ptr->freq), ptr->count + 1);

	// Set end to primary chan count if needed
	//
	if (!bInfoOnly && cutEnd == 0 && ptr->chan == 0) {
		cutEnd = ptr->count;
	}
	if (bPrimary && ptr->chan == 0) {
		printf(" - be deleted");
	}
	if (bSecondary && ptr->chan == 1) {
		printf(" - be deleted");
	}
	if (bDsi && (ptr->chan == 2)) {
		printf(" - be deleted");
	}
	if (bSide && (ptr->chan == 3 || ptr->chan == 4 || ptr->chan == 5)) {
		printf(" - be deleted");
	}
	printf("\n");
	printf("Depth: from %5.1f m to %5.1f m; 0 depth count = %d\n", meter(ptr->minDepth), meter(ptr->maxDepth), ptr->nulCount);
	printf("Speed: from %5.1f to %5.1f\n", ptr->minSpeed, ptr->maxSpeed);
	free(ptr);
}

void proceedFrame(sl2Frame *buf) {
	WORD chIndex = buf->channel;
	if (chIndex > MAX_CHAN) {
		return;
	}
	if (stats[chIndex] == NULL) {
		channelStat* ptr = stats[chIndex] = malloc(sizeof(channelStat));
		
		ptr->chan = buf->channel;
		ptr->maxDepth = ptr->minDepth = buf->waterDepth;
		ptr->maxSpeed = ptr->minSpeed = buf->speed;
		ptr->count = buf->frameIndex;
		ptr->nulCount = 0;
		ptr->blockSize = buf->blockSize;
		ptr->freq = buf->frequency;
	}
	else {
		channelStat* ptr = stats[chIndex];
		
		if (buf->waterDepth > 0) {
			if (ptr->maxDepth < buf->waterDepth) {
				ptr->maxDepth = buf->waterDepth;
			}
			if (ptr->minDepth > buf->waterDepth) {
				ptr->minDepth = buf->waterDepth;
			}
		}
		else {
			ptr->nulCount += 1;
		}
		if (ptr->maxSpeed < buf->speed) {
			ptr->maxSpeed = buf->speed;
		}
		if (ptr->minSpeed > buf->speed) {
			ptr->minSpeed = buf->speed;
		}
		ptr->count = buf->frameIndex;
	}
}

void proceedBlocks(FILE *fd) {
	sl2Frame frameBuf;
	BYTE buf[10000];
    long filepos;
	
	int n = sizeof(frameBuf);
	if (n != 144) {
		printf("Wrong block\n");
	}
	
//	printf("Offset %lu\n", (void*)&frameBuf.time1 - (void*)&frameBuf);
    filepos = ftell(fd);
	while (fread(&frameBuf, sizeof(frameBuf), 1, fd) > 0) {
		proceedFrame(&frameBuf);
        
#ifdef VERBOSE
		double dlat = lat(frameBuf.latitude);
		double dlon = lon(frameBuf.longitude);
		DWORD ilat = lowlat(dlat);
		DWORD ilon = lowlon(dlon);

		printf("Latitude: %f N (%d) Longitude %f E(%d)\n", dlat, ilat, dlon, ilon);
#endif
        
#ifdef VERBOSE
		char bin[17];
		printf("Chn %d Freq: %s Idx %d blkSz %d prevBlkSz %d packetSz %d flags %s time %d\n", frameBuf.channel, freq(frameBuf.frequency), frameBuf.frameIndex, frameBuf.blockSize, frameBuf.prevBlockSize, frameBuf.packetSize, strbin(bin, frameBuf.flags), frameBuf.time1);

        printf("FilePos: %8lu dw0 %7d dw1 %7d dw2 %7d dw3 %7d dw4 %d dw5 %7d dw6 %d\n\n", filepos, frameBuf.dw_0, frameBuf.dw_1, frameBuf.dw_2, frameBuf.dw_3, frameBuf.dw_4, frameBuf.dw_5, frameBuf.dw_6);
#endif
        
		if (frameBuf.packetSize > 10000) {
			printf("\nsize error!\n");
		}
		fread(buf, frameBuf.packetSize, 1, fd);
        filepos = ftell(fd);
	};
	
	for (int i = 0; i <= MAX_CHAN; ++i) {
		printStats(stats[i]);
	}
}

BOOL proceedCut(FILE* fdin, FILE* fdout) {
	BOOL ok = 1;
	sl2Header headBuf;
	sl2Frame frameBuf;
	BYTE buf[10000];
	WORD prevSize;
	DWORD filepos;

	fread(&headBuf, sizeof(headBuf), 1, fdin);
	if (fwrite(&headBuf, sizeof(headBuf), 1, fdout) == 0) {
		fclose(fdout);
		printf("Write error!\n");
		return 0;
	}
	
	prevSize = 0;
	
	DWORD shift[MAX_CHAN+1];
	memset(shift, 0, (MAX_CHAN+1)*sizeof(DWORD));

	while (fread(&frameBuf, sizeof(frameBuf), 1, fdin) > 0) {
		fread(buf, frameBuf.packetSize, 1, fdin);
		if (frameBuf.frameIndex < cutStart) {
			continue;
		}
		if (frameBuf.frameIndex > cutEnd) {
			continue;
		}
		if (bPrimary) {
			if (frameBuf.channel == 0) {
				continue;
			}
		}
		if (bSecondary) {
			if (frameBuf.channel == 1) {
				continue;
			}
		}
		if (bDsi) {
			if (frameBuf.channel == 2) {
				continue;
			}
		}
		if (bSide) {
			if (frameBuf.channel == 3 || frameBuf.channel == 4 || frameBuf.channel == 5) {
				continue;
			}
		}
		
		if (bHideCoordinates) {
			double tempLat = lat(frameBuf.latitude);
			double tempLon = lon(frameBuf.longitude);
			if (shiftN == 0) {
				shiftN = fakeN - tempLat;
				shiftE = fakeE - tempLon;
			}
			frameBuf.latitude = lowlat(tempLat + shiftN);
			frameBuf.longitude = lowlon(tempLon + shiftE);
		}
		filepos = (DWORD)ftell(fdout);
		shift[frameBuf.channel] = filepos;
		frameBuf.prevBlockSize = prevSize;
		frameBuf.frameIndex = frameBuf.frameIndex - cutStart;
		
		// Update shifts to frame
		//
		frameBuf.dw_0 = filepos;
		frameBuf.dw_1 = shift[0];
		frameBuf.dw_2 = shift[1];
		frameBuf.dw_3 = shift[2];
		frameBuf.dw_4 = shift[3];
		frameBuf.dw_5 = shift[4];
		frameBuf.dw_6 = shift[5];
		
		fwrite(&frameBuf, sizeof(frameBuf), 1, fdout);
		prevSize = frameBuf.blockSize;
		fwrite(buf, frameBuf.packetSize, 1, fdout);
	}
	return ok;
}

BOOL proceed(const char* fname) {
	BOOL ok = 1;
	sl2Header headBuf;
	
	FILE *fd;
	fd = fopen(fname, "rb");
	if (!fd) {
		printf("Can't open file %s\n", fname);
		return EXIT_FAILURE;
	}
	
	fread(&headBuf, sizeof(headBuf), 1, fd);
	
#ifdef VERBOSE
	printf("Format: %d Version: %d block: %d w_6 %d\n", headBuf.format, headBuf.version, headBuf.blockSize, headBuf.w_6);
#endif
	
	proceedBlocks(fd);
	
	if (!bInfoOnly) {
		rewind(fd);
		char fnameout[1024];
		
		sprintf(fnameout, "%.*s_%d-%d.sl2", (int)(strlen(fname) - 4), fname, cutStart, cutEnd);
		FILE* fdout = fopen(fnameout, "wb");
		if (!fdout) {
			printf("Can't create file %s\n", fnameout);
			fclose(fd);
			return EXIT_FAILURE;
		}
		ok = proceedCut(fd, fdout);
		if (ok) {
#ifdef _WIN32
			FILETIME ftCreate, ftAccess, ftWrite;

			GetFileTime((HANDLE)_get_osfhandle(_fileno(fd)), &ftCreate, &ftAccess, &ftWrite);
			SetFileTime((HANDLE)_get_osfhandle(_fileno(fdout)), &ftCreate, &ftAccess, &ftWrite);
            fclose(fd);
            fclose(fdout);
#else
            struct stat statBuf;
            struct utimbuf utBuf;
            
            fclose(fd);
            fclose(fdout);
            stat(fname, &statBuf);
            utBuf.actime = statBuf.st_mtimespec.tv_sec;
            utBuf.modtime = statBuf.st_mtimespec.tv_sec;
			utime(fnameout, &utBuf);
#endif // WIN32
			printf("Output file: %s\n", fnameout);
		}
	}
	return ok;
}

int main(int argc, const char * argv[]) {
	BOOL ok = (argc > 1);
	if (ok) {
		if (argc == 2) {
			// No cut, only print info
			bInfoOnly = 1;
			ok = proceed(argv[1]);
			return ok ? EXIT_SUCCESS : EXIT_FAILURE;
		}
		else {
			bInfoOnly = 0;
			for (int i=1; i<argc; i++) {
				if (argv[i][0]=='-') {
					switch(tolower((unsigned char)argv[i][1])) {
						case 'c':
							if (argc > i+1) {
								++i;
								int n = sscanf(argv[i], "%d:%d", &cutStart, &cutEnd);
								if (n != 2 || cutStart >= cutEnd) {
									printf("Wrong cut argument\n");
									return EXIT_FAILURE;
								}
							}
							break;
						case 'd':
							if (argc > i+1) {
								char delarg[100];
								++i;
								sscanf(argv[i], "%s", delarg);
								for (WORD j = 0; j < strlen(delarg); ++j) {
									if (tolower(delarg[j]) == 'p') {
										bPrimary = 1;
									}
									if (tolower(delarg[j]) == 't') {
										bSecondary = 1;
									}
									else if (tolower(delarg[j]) == 'd') {
										bDsi = 1;
									}
									else if (tolower(delarg[j]) == 's') {
										bSide = 1;
									}
									else {
										goto delerr;
									}
								}
							}
							else {
							delerr:
								printf("Wrong delete channel argument\n");
								return EXIT_FAILURE;
							}
							break;
						case 'a':
							bHideCoordinates = 1;
							break;
						case 'n':
							bHideCoordinates = 1;
							if (argc > i+1) {
								float nn;
								++i;
								if (sscanf(argv[i], "%f", &nn) == 1) {
									fakeN = nn;
								}
							}
							break;
						case 'e':
							bHideCoordinates = 1;
							if (argc > i+1) {
								float nn;
								++i;
								if (sscanf(argv[i], "%f", &nn) == 1) {
									fakeE = nn;
								}
							}
							break;
						default:
							break;
					}
				}
				else {
					ok = proceed(argv[i]);
					return ok ? EXIT_SUCCESS : EXIT_FAILURE;
				}
			}
		}
	}
	
	printf("Usage: sl2cut [-c start:end] [-d [PTDS]] [-a] [-n coord] [-e coord] file\n");
}
