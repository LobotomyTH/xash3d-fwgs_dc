/*
sys_dc.c - DC system component
Copyright (C) 2024 maximqad
sys_win.c - posix system utils
Copyright (C) 2019 a1batross

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include <unistd.h> // fork
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "platform/platform.h"
#include "menu_int.h"


#define MAX_LINE_LENGTH 40
#define Y_SPACING 24
#include <dc/video.h>
#include <arch/arch.h>
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
}
#if XASH_MESSAGEBOX == MSGBOX_KOS
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
        
        drawtext(32, y, line);
        y += Y_SPACING;
        
        msg += i;
        if (*msg == '\n') 
            msg++;
    }
}
#endif // XASH_MESSAGEBOX == MSGBOX_KOS
static qboolean Sys_FindExecutable( const char *baseName, char *buf, size_t size )
{
	char *envPath;
	char *part;
	size_t length;
	size_t baseNameLength;
	size_t needTrailingSlash;

	if( !baseName || !baseName[0] )
		return false;

	envPath = getenv( "PATH" );
	if( !COM_CheckString( envPath ) )
		return false;

	baseNameLength = Q_strlen( baseName );
	while( *envPath )
	{
		part = Q_strchr( envPath, ':' );
		if( part )
			length = part - envPath;
		else
			length = Q_strlen( envPath );

		if( length > 0 )
		{
			needTrailingSlash = ( envPath[length - 1] == '/' ) ? 0 : 1;
			if( length + baseNameLength + needTrailingSlash < size )
			{
				string temp;

				Q_strncpy( temp, envPath, length + 1 );
				Q_snprintf( buf, size, "%s%s%s",
					temp, needTrailingSlash ? "/" : "", baseName );

				if( access( buf, X_OK ) == 0 )
					return true;
			}
		}

		envPath += length;
		if( *envPath == ':' )
			envPath++;
	}
	return false;
}

void Posix_Daemonize( void )
{
	if( Sys_CheckParm( "-daemonize" ))
	{
#if XASH_POSIX && defined(_POSIX_VERSION) && !defined(XASH_MOBILE_PLATFORM)
		pid_t daemon;

		daemon = fork();

		if( daemon < 0 )
		{
			Host_Error( "fork() failed: %s\n", strerror( errno ) );
		}

		if( daemon > 0 )
		{
			// parent
			Con_Reportf( "Child pid: %i\n", daemon );
			exit( 0 );
		}
		else
		{
			// don't be closed by parent
			if( setsid() < 0 )
			{
				Host_Error( "setsid() failed: %s\n", strerror( errno ) );
			}

			// set permissions
			umask( 0 );

			// engine will still use stdin/stdout,
			// so just redirect them to /dev/null
			close( STDIN_FILENO );
			close( STDOUT_FILENO );
			close( STDERR_FILENO );
			open("/dev/null", O_RDONLY); // becomes stdin
			open("/dev/null", O_RDWR); // stdout
			open("/dev/null", O_RDWR); // stderr

			// fallthrough
		}
#elif defined(XASH_MOBILE_PLATFORM)
		Sys_Error( "Can't run in background on mobile platforms!" );
#else
		Sys_Error( "Daemonize not supported on this platform!" );
#endif
	}

}

#if XASH_TIMER == TIMER_KOS
double Platform_DoubleTime( void )
{
	struct timespec ts;

	clock_gettime( CLOCK_MONOTONIC, &ts );
	return (double) ts.tv_sec + (double) ts.tv_nsec/1000000000.0;
}

#endif // XASH_TIMER == TIMER_POSIX