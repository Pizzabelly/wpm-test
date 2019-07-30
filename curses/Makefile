# the compiler: gcc or what ever compiler is in CC env
all: CC=gcc
win: CC=x86_64-w64-mingw32-gcc

# compiler flags:
#  -O3       optimization level 3
#  -Wall     turns on most, but not all, compiler warnings
#  -std=c99  c99 standard
#  ncursesw args from pkg-config
CFLAGS  = -O3 -Wall -std=c99 $(shell pkg-config --cflags ncursesw)

all: BIN = wpm
win: BIN = wpm.exe

all: executable
win: executable

debug: CFLAGS += -DDEBUG -g
debug: executable

# linker flags
#  -lncurses link ncurses for the terminal interface
all: LDFLAGS  = -lncursesw -ltinfow
win: LDFLAGS  = ./win/pdcurses.a

executable: wpm_curses.c 
	$(CC) $(CFLAGS) -o $(BIN) wpm_curses.c $(LDFLAGS)

clean:
	$(RM) wpm
