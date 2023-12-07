FOLDER_PATH := $(shell if [ -d $(HOME)/ensc351/public/myApps/ ]; then echo "$(HOME)/ensc351/public/myApps/"; else echo "$(HOME)/cmpt433/public/myApps/"; fi)
all:	noworky potDriver

noworky:
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror noworky.c -static -o noworky 
	cp noworky $(FOLDER_PATH)

potDriver:
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread potDriver.c joystickState.c displayLED.c A2DReadings.c buttonState.c timeFunctions.c -static -o potDriver 

	cp potDriver $(FOLDER_PATH)

clean:
	rm -rf potDriver
	rm -rf noworky
