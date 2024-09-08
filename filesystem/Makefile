TARGET = libfilesystem_stdio.a
DEFINES = -D_KOS_=1 -D_SH4_=1 -DXASH_GAMEDIR="valve" -DXASH_REDUCE_FD=0 -DXASH_LOW_MEMORY=2 -DALLOCA_H=<malloc.h> -DSTDINT_H=<stdint.h> -DHAVE_TGMATH_H=0 -DHAVE_STRNICMP=0 -DHAVE_STRICMP=0 -DXASH_STATIC_LIBS=1
INCLUDES = -I../filesystem -I../public -I../common -I../pm_shared -I../engine 

KOS_CFLAGS += $(INCLUDES) $(DEFINES) -nostartfiles -Ofast -Wl,-q -Wfatal-errors -Wno-write-strings -fsigned-char -fno-short-enums -fno-lto 
KOS_CPPFLAGS += $(KOS_CFLAGS) -fno-exceptions -fpermissive

OBJS = android.o dir.o filesystem.o pak.o VFileSystem009.o wad.o link_helper.o
include ../Makefile.library