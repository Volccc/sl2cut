//
//  Sl2Utils.c
//  sl2cut
//
//  Created by Vladimir Borisov on 17/05/16.
//  Copyright © 2016. All rights reserved.
//

static const double POLAR_EARTH_RADIUS = 6356752.3142;

#include <math.h>
#include "Sl2Utils.h"

const char* freq(BYTE frequency) {
	switch (frequency) {
		case 1:
			return "50";
		case 2:
			return "83";
		case 3:
			return "455";
		case 4:
			return "800";
		case 5:
			return "38";
		case 6:
			return "28";
		case 7:
			return "130 - 210";
		case 8:
			return "90 - 150";
		case 9:
			return "40 - 60";
		case 10:
			return "25 - 45";
		default:
			return "200";
	}
}

const char* chan(WORD channel) {
	switch (channel) {
  case 0:
			return "Primary (Tranditional Sonar)";
  case 1:
			return "Secondary (Tranditional Sonar)";
  case 2:
			return "DSI (DownScan Imaging)";
  case 3:
			return "Left (Sidescan)";
  case 4:
			return "Right (Sidescan)";
  case 5:
			return "Composite (Sidescan)";
  default:
			return "Invalid";
	}
}

//function y2lat(a) { return 180/Math.PI * (2 * Math.atan(Math.exp(a*Math.PI/180)) - Math.PI/2); }
//function lat2y(a) { return 180/Math.PI * Math.log(Math.tan(Math.PI/4+a*(Math.PI/180)/2)); }

double lat(DWORD northing) {
    double temp = (double)northing / POLAR_EARTH_RADIUS;
    temp = exp(temp);
    temp = (2 * atan(temp)) - (M_PI / 2);
    
    return (temp * (180 / M_PI));
}

DWORD lowlat(double dlat) {
	
	double temp = dlat / (180 / M_PI);
	temp = log(tan(M_PI/4 + temp * (M_PI / 180) / 2));
	
//	temp = (180 / M_PI) * temp;
	
	DWORD ll = temp * POLAR_EARTH_RADIUS;
	return ll;
}

double lon(DWORD easting) {
    return ((double)easting / POLAR_EARTH_RADIUS * (180 / M_PI));
}

DWORD lowlon(double dlon) {
	double temp = (dlon / (180 / M_PI)) * POLAR_EARTH_RADIUS;
	return (DWORD)temp;
}

float meter(float foots) {
	return (foots/3.2808399);
}

float kph(float knot) {
	return (knot * 1.852);
}

char* strbin(char* buf, WORD flags) {
	buf[16] = '\0';
	WORD bit = 0x8000;
	for (int i = 0; i < 16; i++) {
		if (flags & bit) {
			buf[i] = '1';
		}
		else {
			buf[i] = '0';
		}
		bit >>= 1;
	}
	return buf;
}
