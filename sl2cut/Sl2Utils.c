//
//  Sl2Utils.c
//  sl2cut
//
//  Created by Vladimir Borisov on 17/05/16.
//  Copyright © 2016. All rights reserved.
//

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
