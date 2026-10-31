CC = gcc
FLAGS = -O2 -Wall -Wextra -pedantic -std=c99

SDL_WIN = -lsdl3 -lsdl3_image -lsdl3_mixer -lsdl3_ttf
SDL_LIN = ./extern/lin/libSDL3.so.0 ./extern/lin/libSDL3_image.so.0 \
           ./extern/lin/libSDL3_mixer.so.0 ./extern/lin/libSDL3_ttf.so.0

all:
ifeq ($(OS), Windows_NT)
	$(CC) $(FLAGS) -shared engine.c -I"extern" -L. -L"extern/win" $(SDL_WIN) -o tabby.dll
	$(CC) $(FLAGS) launcher.c utils.c -I"extern" -L. -L"extern/win" $(SDL_WIN) -o launcher
	$(CC) $(FLAGS) tabby.c utils.c -o tabby

else
	$(CC) $(FLAGS) -fPIC -shared engine.c -I"extern" -L"extern" -Wl,-rpath,'$$ORIGIN' \
		$(SDL_LIN) -o libtabby.so

	$(CC) $(FLAGS) launcher.c utils.c -I"extern" -L"extern" -Wl,-rpath,'$$ORIGIN' \
		$(SDL_LIN) -o launcher
	$(CC) $(FLAGS) tabby.c utils.c -o tabby
endif
