//
//  Sl2Utils.h
//  sl2cut
//
//  Created by Vladimir Borisov on 17/05/16.
//  Copyright © 2016. All rights reserved.
//

#ifndef Sl2Utils_h
#define Sl2Utils_h

#include <stdio.h>
#include "sl2def.h"

const char* freq(BYTE frequency);
const char* chan(WORD channel);
double meter(float foots);
double kph(float knot);
char* strbin(char* buf, WORD flags);

double lat(int northing);
double lon(int easting);
int lowlat(double dlat);
int lowlon(double dlon);

typedef struct {
	WORD chan;
	DWORD count;
	DWORD nulCount;
	WORD blockSize;
	float minDepth;
	float maxDepth;
	float minSpeed;
	float maxSpeed;
	BYTE freq;
} channelStat;

#endif /* Sl2Utils_h */
