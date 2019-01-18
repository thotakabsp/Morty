# Sample Makefile to compile individual .c files to applications

#CC = arm-poky-linux-gnueabi-gcc
#INCLUDES = "-I/home/medical/var-som-mx6-yocto-jethro/build-fb/tmp/sysroots/var-som-mx6/usr/include"
#LIB_PATH = "-L/home/medical/var-som-mx6-yocto-jethro/build-fb/tmp/sysroots/var-som-mx6/usr/lib"
LFLAGS += $(LIB_PATH) -ljpeg #-lcairo
#CFLAGS = -O2 -g -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=/home/medical/var-som-mx6-yocto-jethro/build-fb/tmp/sysroots/var-som-mx6 -Wall

SRCS = $(wildcard *.c)

PROGS = $(patsubst %.c,%,$(SRCS))

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS)
clean: 
	rm -f $(PROGS)
