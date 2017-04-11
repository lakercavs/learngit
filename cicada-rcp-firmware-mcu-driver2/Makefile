# written by lu yongjiang  on 08/27/11
# modified by liu jirang on 20140228

NAME    = app
CC      = csky-abiv2-elf-gcc 
AS      = csky-abiv2-elf-as
AR      = csky-abiv2-elf-ar
LD      = csky-abiv2-elf-ld
DUMP    = csky-abiv2-elf-objdump
OBJCOPY = csky-abiv2-elf-objcopy
ROOTDIR  = $(shell pwd)
DRIVERDIR = $(ROOTDIR)/driver

INCLUDEDIRS += -I$(ROOTDIR)/include  
INCLUDEDIRS += -I$(ROOTDIR)
INCLUDEDIRS += -I$(DRIVERDIR)/dma
INCLUDEDIRS += -I$(DRIVERDIR)/intc
INCLUDEDIRS += -I$(DRIVERDIR)/i2c
INCLUDEDIRS += -I$(DRIVERDIR)/itr
INCLUDEDIRS += -I$(DRIVERDIR)/pcie
INCLUDEDIRS += -I$(DRIVERDIR)/rtc
INCLUDEDIRS += -I$(DRIVERDIR)/timer
INCLUDEDIRS += -I$(DRIVERDIR)/gpio
INCLUDEDIRS += -I$(DRIVERDIR)/uart
INCLUDEDIRS += -I$(DRIVERDIR)/wdt
INCLUDEDIRS += -I$(DRIVERDIR)/atl
INCLUDEDIRS += -I$(DRIVERDIR)/pea
INCLUDEDIRS += -I$(DRIVERDIR)/replaycache
INCLUDEDIRS += -I$(DRIVERDIR)/cmdresq
INCLUDEDIRS += -I$(DRIVERDIR)/spi
INCLUDEDIRS += -I./

CFLAGS += -mcpu=ck810f -g3 -c -O0 -mlittle-endian -mhard-float -mno-multiple-stld 
CFLAGS += -Wall
CFLAGS += -fno-strict-aliasing -fno-builtin -fomit-frame-pointer  
CFLAGS += -frename-registers 
CFLAGS += $(INCLUDEDIRS)

LDFLAGS = -EL 
ASFLAGS = $(CFLAGS) -Wa,--gstabs

export CC AS AR LD DUMP OBJCOPY CFLAGS LDFLAGS ASFLAGS INCLUDEDIRS ROOTDIR

LDLIBS	=  -lm -lc -lgcc -lstdc++ -Wl,--gc-sections
DEPEND	= .dep

.PHONY : all

all    : childdir $(NAME).elf 

childdir:	 
	@for dir in $(SUB_DIRS); do\
		$(MAKE) -C $$dir || exit 1;\
	done

SUB_DIRS = $(shell ls  -F | grep /$ | grep -v Debug | grep -v Release | \
		  grep -v CVS | grep -v arch | grep -v include | grep -v obj \
		| sed "s/\///g")

SUBDIR_FILES = $(foreach dir, $(SUB_DIRS), $(dir)/$(dir).o)

SSRC = $(wildcard *.S) arch/entry.S arch/debug.S

CSRC = $(wildcard *.c)

OBJECTS =  $(SSRC:%.S=%.o) $(CSRC:%.c=%.o) 


arch/crt0.o: arch/crt0.S
	$(CC)  $(ASFLAGS) -o $@ $<
%.o:%.c  
	$(CC)  $(CFLAGS)  -o $@ $<
%.o:%.S 
	$(CC)  $(ASFLAGS) -o $@ $<

$(NAME).elf: arch/crt0.o $(OBJECTS)  $(SUBDIR_FILES) ckcpu.ld 
	$(CC)  -mcpu=ck810f -mno-multiple-stld -mlittle-endian -mhard-float -nostartfiles -nostdlib \
	       "`csky-abiv2-elf-gcc -mcpu=ck810f -mlittle-endian -mhard-float -print-file-name=crti.o`" \
	       "`csky-abiv2-elf-gcc -mcpu=ck810f -mlittle-endian -mhard-float -print-file-name=crtbegin.o`" \
	       -T./ckcpu.ld -o $(NAME).elf arch/crt0.o  $(OBJECTS) $(SUBDIR_FILES) $(LDLIBS) \
	       "`csky-abiv2-elf-gcc -mcpu=ck810f -mlittle-endian -mhard-float -print-file-name=crtend.o`" \
	       "`csky-abiv2-elf-gcc -mcpu=ck810f -mlittle-endian -mhard-float -print-file-name=crtn.o`"	
	$(DUMP) -S $(NAME).elf > $(NAME).asm
	$(OBJCOPY) -O binary -S  $(NAME).elf $(NAME).bin

ifneq ($(wildcard .dep),)
include $(DEPEND)
endif

dep: 
	$(SHELL) -ec '$(CC) -MM $(CFLAGS) $(INCLUDEDIRS) $(CSRC) $(SSRC) \
        | sed '\''s@\(.*\)\.o[ :]@\1.o:@g'\'' > $(DEPEND)'
	@for dir in $(SUB_DIRS); do\
                $(MAKE) -C $$dir dep;\
        done


clean:
	rm -f arch/crt0.o
	rm -f $(OBJECTS)
	rm -f $(NAME).elf
	rm -f $(NAME).asm
	@for dir in $(SUB_DIRS); do\
		$(MAKE) -C $$dir clean;\
	done

.PHONY : debug-make

DEBUG_VARS = 	SUB_DIRS \
		SUBDIR_FILES\
		INCLUDEDIRS \
		CSRC \
		SSRC \
		OBJECTS \
		ASFLAGS \
		CFLAGS
#:
#: debug-make -- Print a list of Makefile variables

debug-make:
	@$(foreach var, $(DEBUG_VARS), echo $(var)=$($(var)) ; )

