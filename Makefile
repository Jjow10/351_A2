FOLDER_PATH := $(shell if [ -d $(HOME)/ensc351/public/myApps/ ]; then echo "$(HOME)/ensc351/public/myApps/"; else echo "$(HOME)/cmpt433/public/myApps/"; fi)

CC := arm-linux-gnueabihf-gcc
CFLAGS := -Wall -g -std=c99 -D_POSIX_C_SOURCE=200809L -Werror
LDFLAGS := -pthread -lm

# List of source files for each module
NOWORKY_SRC := noworky.c
POT_DRIVER_SRC := potDriver.c timeFunctions.c A2DReadings.c buttonState.c displayLED.c buffer.c dataProcessing.c cleanup.c

# Output executables
NOWORKY := noworky
POT_DRIVER := potDriver

all: $(NOWORKY) $(POT_DRIVER)

$(NOWORKY): $(NOWORKY_SRC)
	$(CC) $(CFLAGS) $(NOWORKY_SRC) -static -o $@
	cp $@ $(FOLDER_PATH)

$(POT_DRIVER): $(POT_DRIVER_SRC)
	$(CC) $(CFLAGS) $(POT_DRIVER_SRC) $(LDFLAGS) -static -o $@
	cp $@ $(FOLDER_PATH)

clean:
	rm -rf $(NOWORKY) $(POT_DRIVER)
