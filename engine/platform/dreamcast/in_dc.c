/*
in_dc.c - DC input component
Copyright (C) 2024 maximqad
some code are borrowed from Xash3D PSP Port Copyright (C) 2021 Sergey Galushko

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/
#if !XASH_DEDICATED
#if XASH_INPUT == INPUT_KOS
#include <dc/maple/controller.h>
#include <dc/maple/keyboard.h>
#include <dc/maple/mouse.h>

#include "common.h"
#include "keydefs.h"
#include "input.h"
#include "client.h"
#include "vgui_draw.h"
#include "sound.h"
#include "vid_common.h"

#define DC_MAX_KEYS	sizeof(dc_keymap) / sizeof(struct dc_keymap_s)

static struct dc_keymap_s {
    int srckey;     
    int dstkey;      
    qboolean stdpressed; 
    qboolean shiftpressed; 
} dc_keymap[] = {
	{ CONT_START,    K_START_BUTTON, false, false }, // start
	{ CONT_B,        K_B_BUTTON,    false, false }, // Jump
    { CONT_A,        K_A_BUTTON,    false, false }, // Attack
    { CONT_DPAD_DOWN, K_DPAD_DOWN,  false, false }, // Back
	{ CONT_DPAD_UP,  K_DPAD_UP,     false, false },
    { CONT_DPAD_LEFT, K_DPAD_LEFT,  false, false }, // Left
    { CONT_DPAD_RIGHT, K_DPAD_RIGHT, false, false }, // Right
	{ CONT_Y,        K_Y_BUTTON,    false, false },
    { CONT_X,        K_X_BUTTON,    false, false }, // Invprev
};


/*
=============
Platform_GetMousePos

=============
*/
void GAME_EXPORT Platform_GetMousePos(int *x, int *y) 
{
   // TODO: mouse
}

/*
=============
Platform_SetMousePos

============
*/
void GAME_EXPORT Platform_SetMousePos(int x, int y) 
{
    // TODO: mouse
}

/*
========================
Platform_MouseMove

========================
*/
void Platform_MouseMove( float *x, float *y )
{
	// TODO: mouse
}

/*
=============
Platform_GetClipobardText

=============
*/
int Platform_GetClipboardText( char *buffer, size_t size )
{
	// stub
	return 0;
}

/*
=============
Platform_SetClipobardText

=============
*/
void Platform_SetClipboardText( const char *buffer )
{
	// stub
}

/*
=============
Platform_Vibrate

=============
*/
void Platform_Vibrate( float time, char flags )
{
	// stub
}

/*
=============
Platform_EnableTextInput

=============
*/
void Platform_EnableTextInput(qboolean enable) 
{
    // TODO: keyboard
}
    

/*
=============
Platform_JoyInit

=============
*/
int Platform_JoyInit( int numjoy )
{
    return 1; // Return success
}

/*
========================
SDLash_InitCursors

========================
*/
void SDLash_InitCursors( void )
{
    // stub
}

/*
========================
SDLash_FreeCursors

========================
*/
void SDLash_FreeCursors( void )
{
    // stub
}

/*
========================
Platform_SetCursorType

========================
*/
void Platform_SetCursorType( VGUI_DefaultCursor type )
{
    // stub
}

/*
========================
Platform_GetKeyModifiers

========================
*/
key_modifier_t Platform_GetKeyModifiers( void )
{
	return KeyModifier_None;
}

/*
=============
Platform_RunEvents

Processes input events from the keyboard, mouse, and joystick.
=============
*/
void Platform_RunEvents(void)
{
    int i;
    signed short curr_X, curr_Y;
    static unsigned int last_buttons;
    static signed short last_X, last_Y;
    maple_device_t *joy = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
    cont_state_t *state;

    // Check if the joystick device is available
    if (joy) 
    {
        state = (cont_state_t *)maple_dev_status(joy);

        if (state) 
        {
            // Handle button presses
            for (i = 0; i < DC_MAX_KEYS; i++) 
            {
                if (state->buttons & dc_keymap[i].srckey)
                 {
                    if (!(last_buttons & dc_keymap[i].srckey)) 
                    {
                        Key_Event(dc_keymap[i].dstkey, true); 
                    }
                } 
                else
                {
                    if (last_buttons & dc_keymap[i].srckey) 
                    {
                        Key_Event(dc_keymap[i].dstkey, false); 
                    }
                }
            }

			//Special case for triggers
            if (state->ltrig > 0) 
			{
                Key_Event(K_JOY1, true); 
            }
            else 
			{
                Key_Event(K_JOY1, false); 
            }

            if (state->rtrig > 0)
			{
                Key_Event(K_JOY2, true); 
            } 
            else
			{
                Key_Event(K_JOY2, false); 
            }

            last_buttons = state->buttons;

			signed short curr_X = state->joyx; 
			signed short curr_Y = state->joyy; 
            float sensitivity = 99.0f; // HACK HACK FIX ME 

			if (last_X != curr_X) {
				Joy_AxisMotionEvent(2, -curr_X * sensitivity); 
			}

			if (last_Y != curr_Y) {
				Joy_AxisMotionEvent(3, -curr_Y * sensitivity); 
			}

			last_X = curr_X;
			last_Y = curr_Y;
		}
	}

}
/*
========================
Platform_PreCreateMove

this should disable mouse look on client when m_ignore enabled
TODO: kill mouse in win32 clients too
========================
*/
void Platform_PreCreateMove(void)
{
    if( m_ignore.value )
	{
		Platform_GetMousePos( NULL, NULL );
		
	}
}

#endif // XASH_INPUT == INPUT_KOS
#endif // !XASH_DEDICATED