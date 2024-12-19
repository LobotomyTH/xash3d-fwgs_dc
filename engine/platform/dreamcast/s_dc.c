/*
s_dc.c - DC sound component
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

#include "kos.h"
#include "common.h"
#include "sound.h"
#include "platform/platform.h"
#include <dc/sound/sound.h>
#include <dc/sound/sfxmgr.h>
#include <dc/sound/stream.h>

#if XASH_SOUND == SOUND_KOS

static snd_stream_hnd_t stream = SND_STREAM_INVALID;
static char pcm_buffer[2*1024] __attribute__((aligned(32)));

static void* sound_callback(snd_stream_hnd_t hnd, int len, int *actual)
{
	const int size = dma.samples << 1;
	int pos;
	int wrapped;
	
	pos = dma.samplepos << 1;
	
	if( pos >= size )
		pos = dma.samplepos = 0;

	wrapped = pos + len - size;

	if( wrapped < 0 )
	{
		memcpy( pcm_buffer, dma.buffer + pos, len );
		dma.samplepos += len >> 1;
		*actual = len;
	}
	else
	{
		int remaining = size - pos;

		memcpy( pcm_buffer, dma.buffer + pos, remaining );
		memcpy( pcm_buffer + remaining, dma.buffer, wrapped );
		dma.samplepos = wrapped >> 1;
		*actual = wrapped;
	}

	if( dma.samplepos >= size )
		dma.samplepos = 0;
	
	return pcm_buffer;
}

qboolean SNDDMA_Init(void)
{
    // TODO: implement sound
	//Con_DPrintf("%s: DC audio component is not implemented yet\n", __func__);
	snd_init();
	snd_stream_init();
	stream = snd_stream_alloc(sound_callback, 4096);
	assert(stream != FILEHND_INVALID);
	
	dma.format.speed    = SOUND_DMA_SPEED;
	dma.format.channels = 2;
	dma.format.width    = 2;
	dma.samples         = 2048 * 2;
	dma.buffer          = Z_Calloc( dma.samples * 2 );
	dma.samplepos       = 0;
	dma.initialized = true;
	dma.backendName = "AICA SPU";
	snd_stream_volume(stream, 240);
	snd_stream_start(stream, SOUND_DMA_SPEED, 0);
	return true;
}

void SNDDMA_Shutdown(void)
{
    // TODO: implement sound
    snd_stream_stop(stream);
	snd_stream_destroy(stream);
	snd_stream_shutdown();
    snd_shutdown();
}

void SNDDMA_Submit(void)
{
    // TODO: implement sound
    snd_stream_poll(stream);
}

void SNDDMA_BeginPainting(void)
{
    // TODO: implement sound
}

void S_Activate(qboolean active)
{
   // TODO: implement sound
}

/*
===========
VoiceCapture_Init
===========
*/
qboolean VoiceCapture_Init( void )
{
    // stub
	return false;
}

/*
===========
VoiceCapture_Activate
===========
*/
qboolean VoiceCapture_Activate( qboolean activate )
{
    // stub
	return false;
}

/*
===========
VoiceCapture_Lock
===========
*/
qboolean VoiceCapture_Lock( qboolean lock )
{
    // stub
	return false;
}

/*
==========
VoiceCapture_Shutdown
==========
*/
void VoiceCapture_Shutdown( void )
{
    // stub
}

#endif // XASH_SOUND == SOUND_KOS
