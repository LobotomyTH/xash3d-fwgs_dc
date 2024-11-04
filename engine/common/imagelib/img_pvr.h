/*
img_pvr.h - pvr format reference
Copyright (C) 2024 maximqad

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/
#ifndef IMG_PVR_H
#define IMG_PVR_H
/*
========================================================================

.PVR image format

========================================================================
*/
#define GBIXHEADER	(('X'<<24)+('I'<<16)+('B'<<8)+'G') // little-endian "GBIX"
#define PVRTSIGN	(('T'<<24)+('R'<<16)+('V'<<8)+'P') // little-endian "PVRT"



#define PVR_TWIDDLE	0x01
#define PVR_VQ		0x03
#define PVR_RECT	0x09



#pragma pack(push,1)
typedef struct gbix_s
{
    uint32_t version;           // "GBIX" in ASCII
    uint32_t nextTagOffset;     // bytes number to next tag
    unsigned long long globalIndex;
} gbix_t;

typedef struct pvrt_s 
{
    uint32_t version;          // "PVRT" in ASCII
    uint32_t textureDataSize;  // Size of rest of the file
    uint8_t colorFormat;       // 0x01 for RGB565
    uint8_t imageFormat;       // 0x01=twiddled, 0x03=VQ, 0x09=rectangle
    uint16_t zeroes;          // Always 0
    uint16_t width;
    uint16_t height;
} pvrt_t;
#pragma pack(pop)


#endif // IMG_PVR_H

