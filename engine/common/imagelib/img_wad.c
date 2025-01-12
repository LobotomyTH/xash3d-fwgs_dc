/*
img_mip.c - hl1 and q1 image mips
Copyright (C) 2007 Uncle Mike

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
#include "wadfile.h"
#include "studio.h"
#include "sprite.h"
#include "qfont.h"
#if XASH_DREAMCAST
#include "img_pvr.h"
#endif
/*
============
Image_LoadPAL
============
*/
qboolean Image_LoadPAL( const char *name, const byte *buffer, fs_offset_t filesize )
{
	int	rendermode = LUMP_NORMAL;

	if( filesize != 768 )
	{
		Con_DPrintf( S_ERROR "%s: (%s) have invalid size (%li should be %d)\n", __func__, name, (long)filesize, 768 );
		return false;
	}

	if( name[0] == '#' )
	{
		// using palette name as rendermode
		if( Q_stristr( name, "normal" ))
			rendermode = LUMP_NORMAL;
		else if( Q_stristr( name, "masked" ))
			rendermode = LUMP_MASKED;
		else if( Q_stristr( name, "gradient" ))
			rendermode = LUMP_GRADIENT;
		else if( Q_stristr( name, "valve" ))
		{
			rendermode = LUMP_HALFLIFE;
			buffer = NULL; // force to get HL palette
		}
		else if( Q_stristr( name, "id" ))
		{
			rendermode = LUMP_QUAKE1;
			buffer = NULL; // force to get Q1 palette
		}
	}

	// NOTE: image.d_currentpal not cleared with Image_Reset()
	// and stay valid any time before new call of Image_SetPalette
	Image_GetPaletteLMP( buffer, rendermode );
	Image_CopyPalette32bit();

	image.rgba = NULL;	// only palette, not real image
	image.size = 1024;	// expanded palette
	image.width = image.height = 0;
	image.depth = 1;

	return true;
}

/*
============
Image_LoadFNT
============
*/
qboolean Image_LoadFNT( const char *name, const byte *buffer, fs_offset_t filesize )
{
	qfont_t		font;
	const byte	*pal, *fin;
	size_t		size;
	int		numcolors;

	if( image.hint == IL_HINT_Q1 )
		return false; // Quake1 doesn't have qfonts

	if( filesize < sizeof( font ))
		return false;

	memcpy( &font, buffer, sizeof( font ));

	// last sixty four bytes - what the hell ????
	size = sizeof( qfont_t ) - 4 + ( font.height * font.width * QCHAR_WIDTH ) + sizeof( short ) + 768 + 64;

	if( size != filesize )
	{
		// oldstyle font: "conchars" or "creditsfont"
		image.width = 256;		// hardcoded
		image.height = font.height;
	}
	else
	{
		// Half-Life 1.1.0.0 font style (qfont_t)
		image.width = font.width * QCHAR_WIDTH;
		image.height = font.height;
	}

	if( !Image_LumpValidSize( name ))
		return false;

	fin = buffer + sizeof( font ) - 4;
	pal = fin + (image.width * image.height);
	numcolors = *(short *)pal, pal += sizeof( short );

	if( numcolors == 768 || numcolors == 256 )
	{
		// g-cont. make sure that is didn't hit anything
		Image_GetPaletteLMP( pal, LUMP_MASKED );
		image.flags |= IMAGE_HAS_ALPHA; // fonts always have transparency
	}
	else
	{
		return false;
	}

	image.type = PF_INDEXED_32;	// 32-bit palette
	image.depth = 1;

	return Image_AddIndexedImageToPack( fin, image.width, image.height );
}

/*
======================
Image_SetMDLPointer

Transfer buffer pointer before Image_LoadMDL
======================
*/
static void *g_mdltexdata;
void Image_SetMDLPointer( byte *p )
{
	g_mdltexdata = p;
}


/*
============
Image_LoadMDL
============
*/
qboolean Image_LoadMDL( const char *name, const byte *buffer, fs_offset_t filesize )
{
 	byte        *fin;
    size_t      pixels;
    mstudiotexture_t    *pin;
    int         flags;
    uint32_t    format;

    pin = (mstudiotexture_t *)buffer;
    flags = pin->flags;

    image.width = pin->width;
    image.height = pin->height;
    pixels = image.width * image.height;
    fin = (byte *)g_mdltexdata;
    ASSERT(fin);
    g_mdltexdata = NULL;

    if (!Image_ValidSize(name))
        return false;

	if (*(uint32_t*)fin == GBIXHEADER || *(uint32_t*)fin == PVRTSIGN)
	{
		byte *texture_data;
		pvrt_t *pvrt;
		
		if (*(uint32_t*)fin == GBIXHEADER)
        {
            gbix_t *gbix = (gbix_t*)fin;
            texture_data = fin + sizeof(gbix_t);
            pvrt = (pvrt_t*)texture_data;
            
            // Verify we have a PVRT signature
            if (*(uint32_t*)texture_data != PVRTSIGN)
            {
                Con_DPrintf("%s: Invalid PVRT signature after GBIX in %s\n", __func__, name);
                return false;
            }
            texture_data += sizeof(pvrt_t);  
        }
		else  // standalone PVRT
		{
			pvrt = (pvrt_t*)fin;
			texture_data = fin + sizeof(pvrt_t);
		}

		image.width = pvrt->width;
		image.height = pvrt->height;

		switch(pvrt->imageFormat)
		{
			case PVR_VQ: 
				image.type = PF_VQ_RGB_5650;
				const int codebook_size = 2048;  // 1024 entries * 2 bytes each
				const int indices_size = (image.width * image.height) / 4;  // Each index covers 2x2 pixels
				image.size = codebook_size + indices_size;
				break;
			case PVR_RECT:
				image.type = PF_RGB_5650;
				image.size = image.width * image.height * 2;
				SetBits(image.flags, TF_KEEP_SOURCE);
				break;
			default:
				Con_DPrintf("Unsupported PVR image format: 0x%X\n", pvrt->imageFormat);
				return false;
		}
		Image_GetPaletteLMP(NULL, LUMP_VQ);
		image.rgba = Mem_Malloc(host.imagepool, image.size);
		memcpy(image.rgba, texture_data, image.size);
		return true;
	} 	 
	else if (image.hint == IL_HINT_HL)
    {
            size_t pixels = image.width * image.height;
            if (filesize < (sizeof(*pin) + pixels + 768))
                return false;

			Con_DPrintf("%s: loading IL_HINT_HL texture %s\n", __func__, name);

            if (FBitSet(pin->flags, STUDIO_NF_MASKED))
            {
                byte *pal = fin + pixels;
                Image_GetPaletteLMP(pal, LUMP_MASKED);
                image.flags |= IMAGE_HAS_ALPHA|IMAGE_ONEBIT_ALPHA;
            }
            else Image_GetPaletteLMP(fin + pixels, LUMP_NORMAL);

        image.type = PF_INDEXED_32;
        image.depth = 1;

        return Image_AddIndexedImageToPack(fin, image.width, image.height);
    }
	else 
 	{
		Con_DPrintf("%s: unsupported texture %s should be PVR or indexed\n", __func__, name);
		return false;
	}
}
	
/*
============
Image_LoadSPR
============
*/
qboolean Image_LoadSPR( const char *name, const byte *buffer, fs_offset_t filesize )
{
    dspriteframe_t pin;    // identical for q1\hl sprites
    qboolean truecolor = false;
    byte *fin;
    
    if( image.hint == IL_HINT_HL )
    {
        if( !image.d_currentpal )
            return false;
    }
    else if( image.hint == IL_HINT_Q1 )
    {
        Image_GetPaletteQ1();
    }
    else
    {
        // unknown mode rejected
        return false;
    }

    memcpy( &pin, buffer, sizeof(dspriteframe_t) );
    image.width = pin.width;
    image.height = pin.height;

    if( filesize < image.width * image.height )
        return false;

    fin = (byte *)(buffer + sizeof(dspriteframe_t));

    uint32_t sign;
    memcpy(&sign, fin, sizeof(uint32_t)); 
    if (sign == PVRTSIGN)
    {
        uint32_t format;
        memcpy(&format, fin + 8, sizeof(uint32_t));
        uint8_t texture_format = (format >> 8) & 0xFF;
        
        image.width = pin.width;
        image.height = pin.height;
        
        if (texture_format == PVR_RECT || texture_format == PVR_VQ)
        {  
            if (texture_format == PVR_VQ)
            {
                image.type = PF_VQ_RGB_5650;
                const int codebook_size = 2048;
                const int indices_size = (image.width * image.height) / 4;
                image.size = codebook_size + indices_size;
				fin += sizeof(dspriteframe_t);
                Image_GetPaletteLMP(NULL, LUMP_VQ);
                image.rgba = Mem_Malloc(host.imagepool, image.size);
                memcpy(image.rgba, fin, image.size);
                return true;
            }
            else
            {
                image.type = PF_RGB_5650;
                image.size = image.width * image.height * 2;
                SetBits(image.flags, TF_KEEP_SOURCE);
                return true;
            }
        }
    }


    if( filesize == ( image.width * image.height * 4 ))
        truecolor = true;

    if( !Image_LumpValidSize( name )) 
        return false;

    image.type = (truecolor) ? PF_RGBA_32 : PF_INDEXED_32;
    image.depth = 1;

    switch( image.d_rendermode )
    {
    case LUMP_MASKED:
        SetBits( image.flags, IMAGE_ONEBIT_ALPHA );
        // intentionally fallthrough
    case LUMP_GRADIENT:
    case LUMP_QUAKE1:
        SetBits( image.flags, IMAGE_HAS_ALPHA );
        break;
    }

    if( truecolor )
    {
        image.size = image.width * image.height * 4;
        image.rgba = Mem_Malloc( host.imagepool, image.size );
        memcpy( image.rgba, fin, image.size );
        SetBits( image.flags, IMAGE_HAS_COLOR );
        return true;
    }

    return Image_AddIndexedImageToPack( fin, image.width, image.height );
}
/*
============
Image_LoadLMP
============
*/
qboolean Image_LoadLMP( const char *name, const byte *buffer, fs_offset_t filesize )
{
	lmp_t	lmp;
	byte	*fin, *pal;
	int	rendermode;
	int	i, pixels;

	if( filesize < sizeof( lmp ))
		return false;

	// valve software trick (particle palette)
	if( Q_stristr( name, "palette.lmp" ))
		return Image_LoadPAL( name, buffer, filesize );

	// id software trick (image without header)
	if( Q_stristr( name, "conchars" ) && filesize == 16384 )
	{
		image.width = image.height = 128;
		rendermode = LUMP_QUAKE1;
		filesize += sizeof( lmp );
		fin = (byte *)buffer;

		// need to remap transparent color from first to last entry
		for( i = 0; i < 16384; i++ ) if( !fin[i] ) fin[i] = 0xFF;
	}
	else
	{
		fin = (byte *)buffer;
		memcpy( &lmp, fin, sizeof( lmp ));
		image.width = lmp.width;
		image.height = lmp.height;
		rendermode = LUMP_NORMAL;
		fin += sizeof( lmp );
	}

	pixels = image.width * image.height;

	if( filesize < sizeof( lmp ) + pixels )
		return false;

	if( !Image_ValidSize( name ))
		return false;

	if( image.hint != IL_HINT_Q1 && filesize > (int)sizeof(lmp) + pixels )
	{
		int	numcolors;

		// HACKHACK: console background image shouldn't be transparent
		if( !Q_stristr( name, "conback" ))
		{
			for( i = 0; i < pixels; i++ )
			{
				if( fin[i] == 255 )
				{
					image.flags |= IMAGE_HAS_ALPHA;
					rendermode = LUMP_MASKED;
					break;
				}
			}
		}
		pal = fin + pixels;
		numcolors = *(short *)pal;
		if( numcolors != 256 ) pal = NULL; // corrupted lump ?
		else pal += sizeof( short );
	}
	else if( image.hint != IL_HINT_HL )
	{
		image.flags |= IMAGE_HAS_ALPHA;
		rendermode = LUMP_QUAKE1;
		pal = NULL;
	}
	else
	{
		// unknown mode rejected
		return false;
	}

	Image_GetPaletteLMP( pal, rendermode );
	image.type = PF_INDEXED_32; // 32-bit palete
	image.depth = 1;

	return Image_AddIndexedImageToPack( fin, image.width, image.height );
}
/*
=============
Image_LoadMIP
=============
*/
qboolean Image_LoadMIP( const char *name, const byte *buffer, fs_offset_t filesize )
{
	mip_t	mip;
	qboolean	hl_texture;
	byte	*fin, *pal;
	int	ofs[4], rendermode;
	int	i, pixels, numcolors;
	uint	reflectivity[3] = { 0, 0, 0 };

	if( filesize < sizeof( mip ))
		return false;

	memcpy( &mip, buffer, sizeof( mip ));
	
	fin = (byte *)buffer;

	if (*(uint32_t*)buffer == PVRTSIGN)  
	{
		char basename[MAX_QPATH];
		pvrt_t *pvrt = (pvrt_t*)buffer;
		byte *texture_data = fin;
	

		// Set dimensions from PVRT header
		image.width = pvrt->width;
		image.height = pvrt->height;
		
		// Set up MIP info
		mip.width = image.width;
		mip.height = image.height;
		COM_FileBase(name, basename, sizeof(basename));
		COM_StripExtension(basename);
		Q_strncpy(mip.name, basename, sizeof(mip.name));

		switch(pvrt->imageFormat)
		{
			case PVR_VQ: 
				image.type = PF_VQ_RGB_5650;
				const int codebook_size = 2048;  
				const int indices_size = (image.width * image.height) / 4;  
				image.size = codebook_size + indices_size;
				break;
			case PVR_RECT:
				image.type = PF_RGB_5650;
				image.size = image.width * image.height * 2;
				SetBits(image.flags, TF_KEEP_SOURCE);
				break;
			default:
				Con_DPrintf("Unsupported PVR image format: 0x%X\n", pvrt->imageFormat);
				return false;
		}
		Image_GetPaletteLMP(NULL, LUMP_VQ);
		image.rgba = Mem_Malloc(host.imagepool, image.size);
		memcpy(image.rgba, texture_data, image.size);
		return true;
	}
   	else if(!Q_strncmp(mip.name, "GBIX", 4))
    {
		char basename[MAX_QPATH];
        gbix_t *gbix = (gbix_t*)fin;
    
        byte *texture_data = fin + sizeof(gbix_t) + gbix->nextTagOffset;

        uint8_t color_format = texture_data[0];
        uint8_t image_format = texture_data[1];
        image.width = texture_data[6] | (texture_data[7] << 8);
        image.height = texture_data[4] | (texture_data[5] << 8);

		mip.width = image.width;
		mip.height = image.height;
		COM_FileBase(name, basename, sizeof(basename));
		COM_StripExtension(basename);
		Q_strncpy(mip.name, basename, sizeof(mip.name));

        switch(image_format)
        {
            case PVR_VQ:
                image.type = PF_VQ_RGB_5650;
                image.size = 2048 + ((image.width * image.height) / 4);
                texture_data += 8;

                break;
			case PVR_VQ_MIPMAP:
				 // Base size for codebook (2048) + main texture size + mipmap sizes
				int main_size = (image.width * image.height) / 4;
				int mip1_size = (main_size) / 4;  // 1/4 of main size
				int mip2_size = mip1_size / 4;    // 1/16 of main size
				int mip3_size = mip2_size / 4;    // 1/64 of main size
				image.type = PF_VQ_MIPMAP_RGB_5650;
				image.size = 2048 + main_size + mip1_size + mip2_size + mip3_size;
				texture_data += 8;
			//	Con_Printf("VQ_MIPMAP texture detected, size: %d\n", image.size);
                break;
            case PVR_RECT:
                image.type = PF_RGB_5650;
				#if 0
				image.size = ((image.width + 3) & ~3) * ((image.height + 3) & ~3) * 2;
				#else
                image.size = image.width * image.height * 2;
				#endif
				break;
            case PVR_TWIDDLE:
                return false;
            default:
                return false;
        }

        image.rgba = Mem_Malloc(host.imagepool, image.size);
        memcpy(image.rgba, texture_data, image.size);
       
    } 
	else
	{

	image.width = mip.width;
	image.height = mip.height;

	if( !Image_ValidSize( name ))
		return false;

	memcpy( ofs, mip.offsets, sizeof( ofs ));
	pixels = image.width * image.height;

	if( image.hint != IL_HINT_Q1 && filesize >= (int)sizeof(mip) + ((pixels * 85)>>6) + sizeof(short) + 768)
	{
		// half-life 1.0.0.1 mip version with palette
		fin = (byte *)buffer + mip.offsets[0];
		pal = (byte *)buffer + mip.offsets[0] + (((image.width * image.height) * 85)>>6);
		numcolors = *(short *)pal;
		if( numcolors != 256 ) pal = NULL; // corrupted mip ?
		else pal += sizeof( short ); // skip colorsize
		hl_texture = true;


		// setup rendermode
		if( Q_strrchr( name, '{' ))
		{
			// NOTE: decals with 'blue base' can be interpret as colored decals
			if( !Image_CheckFlag( IL_LOAD_DECAL ) || ( pal && pal[765] == 0 && pal[766] == 0 && pal[767] == 255 ))
			{
				SetBits( image.flags, IMAGE_ONEBIT_ALPHA );
				rendermode = LUMP_MASKED;
			}
			else
			{
				// classic gradient decals
				SetBits( image.flags, IMAGE_COLORINDEX );
				rendermode = LUMP_GRADIENT;
			}

			SetBits( image.flags, IMAGE_HAS_ALPHA );
		}
		else
		{
			int	pal_type;

			// NOTE: we can have luma-pixels if quake1 texture
			// converted into the hl texture but palette leave unchanged
			// this is a good reason for using fullbright pixels
			pal_type = Image_ComparePalette( pal );

			// check for luma pixels (but ignore liquid textures because they have no lightmap)
			if( mip.name[0] != '*' && mip.name[0] != '!' && pal_type == PAL_QUAKE1 )
			{
				for( i = 0; i < image.width * image.height; i++ )
				{
					if( fin[i] > 224 )
					{
						image.flags |= IMAGE_HAS_LUMA;
						break;
					}
				}
			}

			if( pal_type == PAL_QUAKE1 )
			{
				SetBits( image.flags, IMAGE_QUAKEPAL );

				// if texture was converted from quake to half-life with no palette changes
				// then applying texgamma might make it too dark or even outright broken
				rendermode = LUMP_NORMAL;
			}
			else
			{
				// half-life mips need texgamma applied
				rendermode = LUMP_TEXGAMMA;
			}
		}

		Image_GetPaletteLMP( pal, rendermode );
		image.d_currentpal[255] &= 0xFFFFFF;
	}
	else if( image.hint != IL_HINT_HL && filesize >= (int)sizeof(mip) + ((pixels * 85)>>6))
	{
		// quake1 1.01 mip version without palette
		fin = (byte *)buffer + mip.offsets[0];
		pal = NULL; // clear palette
		rendermode = LUMP_NORMAL;

		hl_texture = false;

		// check for luma and alpha pixels
		if( !image.custom_palette )
		{
			for( i = 0; i < image.width * image.height; i++ )
			{
				if( fin[i] > 224 && fin[i] != 255 )
				{
					// don't apply luma to water surfaces because they have no lightmap
					if( mip.name[0] != '*' && mip.name[0] != '!' )
						image.flags |= IMAGE_HAS_LUMA;
					break;
				}
			}
		}

		// Arcane Dimensions has the transparent textures
		if( Q_strrchr( name, '{' ))
		{
			for( i = 0; i < image.width * image.height; i++ )
			{
				if( fin[i] == 255 )
				{
					// don't set ONEBIT_ALPHA flag for some reasons
					image.flags |= IMAGE_HAS_ALPHA;
					break;
				}
			}
		}

		SetBits( image.flags, IMAGE_QUAKEPAL );
		Image_GetPaletteQ1();
	}
	else
	{
		return false; // unknown or unsupported mode rejected
	}

	// check for quake-sky texture
	if( !Q_strncmp( mip.name, "sky", 3 ) && image.width == ( image.height * 2 ))
	{
		// g-cont: we need to run additional checks for palette type and colors ?
		image.flags |= IMAGE_QUAKESKY;
	}

	// check for half-life water texture
	if( pal != NULL )
	{
		if( hl_texture && ( mip.name[0] == '!' || !Q_strnicmp( mip.name, "water", 5 )))
		{
			// grab the fog color
			image.fogParams[0] = pal[3*3+0];
			image.fogParams[1] = pal[3*3+1];
			image.fogParams[2] = pal[3*3+2];

			// grab the fog density
			image.fogParams[3] = pal[4*3+0];
		}
		else if( hl_texture && ( rendermode == LUMP_GRADIENT ))
		{
			// grab the decal color
			image.fogParams[0] = pal[255*3+0];
			image.fogParams[1] = pal[255*3+1];
			image.fogParams[2] = pal[255*3+2];

			// calc the decal reflectivity
			image.fogParams[3] = VectorAvg( image.fogParams );
		}
		else
		{
			// calc texture reflectivity
			for( i = 0; i < 256; i++ )
			{
				reflectivity[0] += pal[i*3+0];
				reflectivity[1] += pal[i*3+1];
				reflectivity[2] += pal[i*3+2];
			}

			VectorDivide( reflectivity, 256, image.fogParams );
		}
	}
	
	image.type = PF_INDEXED_32;	// 32-bit palete

	image.depth = 1;

	return Image_AddIndexedImageToPack( fin, image.width, image.height );

	}
}
