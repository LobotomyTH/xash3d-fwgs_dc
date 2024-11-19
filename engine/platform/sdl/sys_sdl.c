/*
sys_sdl.c - SDL2 system utils
Copyright (C) 2018 a1batross

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include <SDL.h>
#include "platform/platform.h"
#include "events.h"
#if XASH_DREAMCAST
#include <dc/video.h>

static void drawtext(int x, int y, char *string);
extern void getRamStatus(void);
extern char *menu(int *argc, char **argv, char **basedir, int num_dirs);
extern void snd_bump_poll(void);
extern void dreamcast_sound_init(void);
extern void setSystemRam(void);

extern void arch_stk_trace(int n);
void *__stack_chk_guard = (void *)0x69420A55;
#if !XASH_DREAMCAST
void __stack_chk_fail(void)
{
  char strbuffer[1024];
  uint32_t pr = arch_get_ret_addr();

  /* Reset video mode, clear screen */
  vid_set_mode(DM_640x480, PM_RGB565);
  vid_empty();

  /* Display the error message on screen */
  drawtext(32, 64, "Xash3D - Stack failure");
  sprintf(strbuffer, "PR = 0x%016X\n", pr);
  drawtext(32, 96, strbuffer);
  arch_stk_trace(2);


#ifdef FRAME_POINTERS
/* Lifted from Kallistios: kernel/arch/dreamcast/kernel/stack.c
   stack.c
   (c)2002 Dan Potter
*/
  int y=96+32;
  uint32_t fp = arch_get_fptr();
  int n = 3;
  drawtext(32, (y+=32), "-------- Stack Trace (innermost first) ---------");
#if 1
  while((fp > 0x100) && (fp != 0xffffffff)) {
      if((fp & 3) || (fp < 0x8c000000) || (fp > 0x8d000000)) {
          drawtext(32, (y+=32),"   (invalid frame pointer)\n");
          break;
      }

      if(n <= 0) {
          sprintf(strbuffer, "   %08lx\n", arch_fptr_ret_addr(fp));
          drawtext(32, (y+=32), strbuffer);
      }
      else n--;

      fp = arch_fptr_next(fp);
  }
#endif
  drawtext(32, (y+=32), "-------------- End Stack Trace -----------------\n");
#else
  drawtext(32, 128, "Stack Trace: frame pointers not enabled!\n");
#endif

}
#endif
//-----------------------------------------------------------------------------
extern void bfont_draw_str(uint16_t *buffer, int bufwidth, int opaque, char *str);
static void drawtext(int x, int y, char *string) {
  printf("%s\n", string);
  fflush(stdout);
  int offset = ((y * 640) + x);
  bfont_draw_str(vram_s + offset, 640, 1, string);
}

static void assert_hnd(const char *file, int line, const char *expr, const char *msg, const char *func) {
  char strbuffer[1024];

  /* Reset video mode, clear screen */
  vid_set_mode(DM_640x480, PM_RGB565);
  vid_empty();

  /* Display the error message on screen */
  drawtext(32, 64, "Xash3D - Assertion failure");

  sprintf(strbuffer, " Location: %s, line %d (%s)", file, line, func);
  drawtext(32, 96, strbuffer);

  sprintf(strbuffer, "Assertion: %s", expr);
  drawtext(32, 128, strbuffer);

  sprintf(strbuffer, "  Message: %s", msg);
  drawtext(32, 160, strbuffer);
  arch_exit();
}
#endif

#if XASH_TIMER == TIMER_SDL
double Platform_DoubleTime( void )
{
	static longtime_t g_PerformanceFrequency;
	static longtime_t g_ClockStart;
	longtime_t CurrentTime;

	if( !g_PerformanceFrequency )
	{
		g_PerformanceFrequency = SDL_GetPerformanceFrequency();
		g_ClockStart = SDL_GetPerformanceCounter();
	}
	CurrentTime = SDL_GetPerformanceCounter();
	return (double)( CurrentTime - g_ClockStart ) / (double)( g_PerformanceFrequency );
}

void Platform_Sleep( int msec )
{
	SDL_Delay( msec );
}
#endif // XASH_TIMER == TIMER_SDL

#if XASH_DREAMCAST

#define MAX_LINE_LENGTH 40
#define Y_SPACING 24

void Platform_MessageBox(const char *title, const char *message, qboolean parentMainWindow)
{
    char line[MAX_LINE_LENGTH + 1];
    const char *msg = message;
    int y = 96;
    int len = 0;
    int i;

    drawtext(32, 64, title);

    while (*msg)
    {
        // Copy characters until we hit max length or end of string
        for (i = 0; i < MAX_LINE_LENGTH && msg[i] && msg[i] != '\n'; i++)
            line[i] = msg[i];
        
        line[i] = '\0';
        
        // Draw current line
        drawtext(32, y, line);
        y += Y_SPACING;
        
        // Move pointer forward
        msg += i;
        if (*msg == '\n') 
            msg++;
    }
}
#endif

void SDLash_Init( void )
{
#ifndef SDL_INIT_EVENTS
#define SDL_INIT_EVENTS 0
#endif
	if( SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS ) )
	{
		Sys_Warn( "SDL_Init failed: %s", SDL_GetError() );
		host.type = HOST_DEDICATED;
	}

	assert_set_handler(assert_hnd);
#if XASH_SDL == 2
	SDL_SetHint(SDL_HINT_ACCELEROMETER_AS_JOYSTICK, "0");
	SDL_StopTextInput();
#endif // XASH_SDL == 2
	SDLash_InitCursors();
}

void SDLash_Shutdown( void )
{
	SDLash_FreeCursors();
}
