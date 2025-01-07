#
# Basic KallistiOS skeleton / test program
# Copyright (C)2001-2004 Megan Potter
#   


# Put the filename of the output binary here
PROJECT_NAME = xash
TARGET = xash.elf

include engine.mk

# Module paths and lib names
FILESYSTEM_DIR = filesystem
GLDC_DIR = ref/gldc
MAINUI_DIR = mainui_cpp
CS_DLL_DIR = ../fork/cs16-client/cl_dll

MAINUI_LIB = $(MAINUI_DIR)/libmenu.a
FILESYSTEM_LIB = $(FILESYSTEM_DIR)/libfilesystem_stdio.a
GLDC_LIB = $(GLDC_DIR)/libref_gldc.a
CS_DLL_LIB = $(CS_DLL_DIR)/libcs_client.a


OBJS =  $(XASH_CLIENT_OBJS) $(XASH_OBJS) $(XASH_SERVER_OBJS) $(XASH_PLATFORM_OBJS)
# Libraries
LIBS = -L$(CS_DLL_DIR) \
       -L$(KOS_BASE)/addons/lib/$(KOS_ARCH) \
       -L$(KOS_PORTS)/lib \
       -L$(FILESYSTEM_DIR) \
       -L$(GLDC_DIR) \
       -L$(MAINUI_DIR) \
       -lfilesystem_stdio \
       -lcs_client \
       -lref_gldc \
       -l:libGL.a \
       -lppp
	   

# Build module libraries
$(FILESYSTEM_LIB):
	$(MAKE) -C $(FILESYSTEM_DIR)

$(GLDC_LIB):
	$(MAKE) -C $(GLDC_DIR)

$(CS_DLL_LIB):
	$(MAKE) -C $(CS_DLL_DIR)

$(MAINUI_LIB):
	$(MAKE) -C $(MAINUI_DIR)


# -l:libGL.a 
#-l:libGL.a
# The rm-elf step is to remove the target before building, to force the
# re-creation of the rom disk.
all: $(FILESYSTEM_LIB) $(GLDC_LIB) $(CS_DLL_LIB) $(TARGET) 1ST_READ.BIN IP.BIN $(PROJECT_NAME).iso $(PROJECT_NAME).cdi

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(OBJS) 
	-rm -f $(TARGET)
	$(MAKE) -C $(FILESYSTEM_DIR) clean
	$(MAKE) -C $(GLDC_DIR) clean
	$(MAKE) -C $(CS_DLL_DIR) clean
	-rm -f $(TARGET).bin
	-rm -f $(PROJECT_NAME).cdi
	-rm -f 1ST_READ.BIN
	-rm -f build/IP.BIN
	-rm -f $(PROJECT_NAME).iso
	-rm -f $(PROJECT_NAME).cdi
	

$(TARGET): $(OBJS) $(FILESYSTEM_LIB) $(GLDC_LIB) $(CS_DLL_LIB)
	kos-c++ -o $(TARGET) $(OBJS) $(LIBS) -Wl,--gc-sections -fwhole-program -Wl,--build-id=none

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

1ST_READ.BIN: $(TARGET)
	kos-objcopy -R .stack -O binary $(TARGET) $(TARGET).bin 
	$(KOS_BASE)/utils/scramble/scramble $(TARGET).bin 1ST_READ.BIN
	cp 1ST_READ.BIN build

IP.BIN: ip.txt
	-rm -f build/IP.BIN
	$(KOS_BASE)/utils/makeip/makeip ip.txt build/IP.BIN
	
$(PROJECT_NAME).iso: 1ST_READ.BIN
	mkisofs -V XashDC -G build/IP.BIN -r -J -l -o xash.iso build

$(PROJECT_NAME).cdi: $(PROJECT_NAME).iso
	makedisc $(PROJECT_NAME).cdi build build/IP.BIN $(TARGET) 

.PHONY: all clean 1ST_READ.BIN IP.BIN cdi



