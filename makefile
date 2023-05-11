ifeq (,$(CROSS_COMPILE))
$(error missing CROSS_COMPILE for this toolchain)
endif

TARGET = audiobridge

CC = $(CROSS_COMPILE)gcc
CFLAGS   = -marm -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7ve 
LDFLAGS	 = -lSDL

all:
	$(CC) main.c -o $(TARGET) $(CFLAGS) $(LDFLAGS)
clean:
	rm -f $(TARGET)