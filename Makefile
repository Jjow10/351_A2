nowork:
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror noworky.c -static -o noworky 
	cp noworky $(HOME)/ensc351/public/myApps/
