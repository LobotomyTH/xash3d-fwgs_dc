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

#include "common.h"
#include "sound.h"
#include "platform/platform.h"

#if XASH_SOUND == SOUND_KOS

qboolean SNDDMA_Init(void)
{
    // TODO: implement sound
    Con_DPrintf("%s: DC audio component is not implemented yet\n", __func__);
    return false;
}

void SNDDMA_Shutdown(void)
{
    // TODO: implement sound
}

void SNDDMA_Submit(void)
{
    // TODO: implement sound
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
	return false;
}

#endif // XASH_SOUND == SOUND_KOS