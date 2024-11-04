/*
img_pvr.c - pvr format load
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

#include "imagelib.h"
#include "xash3d_mathlib.h"
#include "img_pvr.h"

/*
=============
Image_LoadPVR
=============
*/
qboolean Image_LoadPVR(const char *name, const byte *buffer, fs_offset_t filesize) {
    
    if (filesize < sizeof(gbix_t) + sizeof(pvrt_t)) {
        Con_DPrintf(S_ERROR "%s: (%s) file too small\n", __func__, name);
        return false;
    }

    gbix_t *gbix = ( gbix_t *)buffer;
    uint32_t gbix_identifier = gbix->version;
    if (gbix_identifier != GBIXHEADER) {
        Con_DPrintf(S_ERROR "%s: (%s) missing GBIX header. Got: 0x%08x, Expected: 0x%08x\n", 
                    __func__, name, gbix_identifier, GBIXHEADER);
        return false;
    }

    byte *texture_data = buffer + sizeof(gbix_t) + gbix->nextTagOffset;
    
    Con_DPrintf("GBIX nextTagOffset: %d\n", gbix->nextTagOffset);
    
    // Hacky stuff incoming
    // Direct byte access for format information
    uint8_t color_format = texture_data[0];    // First byte is color format
    uint8_t image_format = texture_data[1];    // Second byte is image format
    
    // Get dimensions from correct offsets
    image.width = texture_data[6] | (texture_data[7] << 8);
    image.height = texture_data[4] | (texture_data[5] << 8);

    Con_DPrintf("PVRT Format Debug:\n");
    Con_DPrintf("Color Format: 0x%02x\n", color_format);
    Con_DPrintf("Image Format: 0x%02x\n", image_format);
    Con_DPrintf("Dimensions: %dx%d\n", image.width, image.height);


    switch (image_format)
    {
        case PVR_VQ:
            image.type = PF_VQ_RGB_5650;
            image.size = 2048 + ((image.width * image.height) / 4);
            texture_data += 8; // Skip format header for VQ

			Con_DPrintf("PVR_VQ Texture: %s\n", name);
            Con_DPrintf("Dimensions: %dx%d\n", image.width, image.height);
            Con_DPrintf("Calculated size: %d\n", image.size);
            Con_DPrintf("First codebook bytes: %02x %02x %02x %02x\n",
                       texture_data[0], texture_data[1], texture_data[2], texture_data[3]);
            break;
        case PVR_RECT:
			image.type = PF_RGB_5650;
            image.size = image.width * image.height * 2;

			Con_DPrintf("PVR_RECT Texture: %s\n", name);
            Con_DPrintf("Dimensions: %dx%d\n", image.width, image.height);
            Con_DPrintf("Calculated size: %d\n", image.size);
            Con_DPrintf("First 8 bytes of texture data: %02x %02x %02x %02x %02x %02x %02x %02x\n",
                       texture_data[0], texture_data[1], texture_data[2], texture_data[3],
                       texture_data[4], texture_data[5], texture_data[6], texture_data[7]);
            break;
        case PVR_TWIDDLE:
            Con_DPrintf("%:s Twiddle format is not supported yet\n.", __func__);
            return false;
        default:
            Con_DPrintf(S_ERROR "%s: (%s) unsupported PVR format %02x\n", __func__, name, image_format);
            return false;
    }

    image.rgba = Mem_Malloc(host.imagepool, image.size);
    memcpy(image.rgba, texture_data, image.size);

    return true;
}
