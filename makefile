CFLAGS = -std=c17 -Wall -Werror -Wextra -g

all:
	gcc 6502.c -o 6502 $(CFLAGS) `sdl2-config --cflags --libs`

debug: 
	gcc chip8.c -o chip8 $(CFLAGS) `sdl2-config --cflags --libs` -DDEBUG
clean:
	rm -f  chip8