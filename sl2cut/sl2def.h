//
//  sl2def.h
//  sl2cut
//
//  Created by Vladimir Borisov on 13/05/16.
//  Copyright © 2016. All rights reserved.
//

#ifndef sl2def_h
#define sl2def_h

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int

#pragma pack(push,1)

// 8 byte header, then a series of blocks/frames as described below.
//
typedef struct {
	WORD format;		// 1 = slg, 2 = sl2
	WORD version;		// ?
	WORD blockSize;
	WORD w_6;
} sl2Header;

typedef struct {
	DWORD dw_0;
	DWORD dw_1;
	DWORD dw_2;
	DWORD dw_3;
	DWORD dw_4;
	DWORD dw_5;
	DWORD dw_6;

	WORD blockSize;		// 28, blockSize in bytes
	WORD prevBlockSize;	// 30, lastBlocksize of block (b - 1) in bytes
	WORD channel;		// 32, __channel*__
						//  0 = Primary (Tranditional Sonar)
						//  1 = Secondary (Traditional Sonar)
						//  2 = DSI (DownScan Imaging)
						//  3 = Left (Sidescan)
						//  4 = Right (Sidescan)
						//  5 = Composite (Sidescan)
						// Any other value is treated as invalid
	WORD packetSize;	// 34, Packet size: Size of sounding/bounce data in bytes.
	DWORD frameIndex;	// 36
	float upperLimit;	// 40, UpperLimit, feet.
	float lowerLimit;	// 44, LowerLimit, feet.
	
	BYTE bb0[5];
	// 53 ++
	BYTE frequency;		//__frequency*__
						//* 0 = 200 KHz
						//* 1 = 50 KHz
						//* 2 = 83 KHz
						//* 3 = 455 KHz
						//* 4 = 800 KHz
						//* 5 = 38 KHz
						//* 6 = 28 KHz
						//* 7 = 130 - 210 KHz
						//* 8 = 90 - 150 KHz
						//* 9 = 40 - 60 KHz
						//* 10 = 25 - 45 KHz
						//* Any other value is treaded like 200 KHz
	BYTE b3[10];
	float waterDepth;	// 64
	float keelDepth;
	float d3;
	float d4;
	float f1;
	float f2;
	float f3;
	float f4;
	float f5;
	float speed;		// 100
	float temperature;	// 104
	int longitude;
	int latitude;
	float speedWater;
	float courseOverGround;
	float altitude;
	float heading;
	short flags;		// 132 Bitmask. From the left most bit:
						// 0 = TrackValid
						// 1 = WaterSpeedValid
						// 2 = Unknown
						// 3 = PositionValid
						// 4 = Unknown
						// 5 = WaterTempValid
						// 6 = SpeedValid
						// 7 to 13 = Unknown
						// 14 = AltitudeValid
						// 15 = HeadingValid
	BYTE b5[6];
	DWORD time1;		// 140
} sl2Frame;

#pragma pack(pop)

#endif /* sl2def_h */
