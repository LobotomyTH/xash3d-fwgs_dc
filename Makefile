#
# Basic KallistiOS skeleton / test program
# Copyright (C)2001-2004 Megan Potter
#   

# Put the filename of the output binary here
PROJECT_NAME = xash
TARGET = xash.elf

include engine.mk

OBJS =  $(XASH_CLIENT_OBJS) $(XASH_OBJS) $(XASH_SERVER_OBJS) $(XASH_PLATFORM_OBJS)
LIBS = -L../hlsdk-portable_dc/dlls -L../hlsdk-portable_dc/cl_dll -Lfilesystem -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) -L$(KOS_PORTS)/lib -Lref/gldc -lSDL_gl -lm  -lfilesystem_stdio -lhl -lcl_dll -lref_gldc -l:libGL.a 
#-l:libGL.a
# The rm-elf step is to remove the target before building, to force the
# re-creation of the rom disk.
all: $(TARGET) 

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(OBJS) 
	-rm -f $(TARGET)
	-rm -f $(TARGET).bin
	-rm -f $(PROJECT_NAME).cdi
	-rm -f 1ST_READ.BIN
	-rm -f IP.BIN
	-rm -f $(PROJECT_NAME).iso
	-rm -f $(PROJECT_NAME).cdi

$(TARGET): $(OBJS)
	kos-c++ -o $(TARGET) $(OBJS) $(LIBS) -Wl,--allow-multiple-definition

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

1ST_READ.BIN: $(TARGET)
	kos-objcopy -R .stack -O binary $(TARGET) $(TARGET).bin 
	scramble $(TARGET).bin 1ST_READ.BIN
	cp 1ST_READ.BIN build
	

IP.BIN:
	rm -f build/IP.BIN
	makeip -v build/IP.BIN

$(PROJECT_NAME).iso: IP.BIN 1ST_READ.BIN
	mkisofs -C 0,11702 -V $(PROJECT_NAME) -G IP.BIN -r -J -l -o $(PROJECT_NAME).iso build

$(PROJECT_NAME).cdi: $(PROJECT_NAME).iso
	cdi4dc $(PROJECT_NAME).iso $(PROJECT_NAME).cdi > cdi.log

cdi: $(PROJECT_NAME).cdi
