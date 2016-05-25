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
float meter(float foots);
float kph(float knot);
char* strbin(char* buf, WORD flags);

typedef struct {
	WORD chan;
	DWORD count;
	DWORD nulCount;
	WORD blockSize;
	float minDepth;
	float maxDepth;
	float minSpeed;
	float maxSpeed;
} channelStat;

#endif /* Sl2Utils_h */
