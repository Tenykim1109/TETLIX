## Makefile for tetlix

start : start.c doubleplay singleplay regist help rank
	gcc -w -g start.c -o start -lcurses
regist : regist.c
	gcc -w -o regist regist.c -lcurses
doubleplay : doubleplay.c tetlixlib.c tetlix.h
	gcc -w -o doubleplay doubleplay.c tetlixlib.c -lcurses
rank : rank.c
	gcc -w -o rank rank.c -lcurses
singleplay : singleplay.c tetlixlib.c tetlix.h
	gcc -w -o singleplay singleplay.c tetlixlib.c -lcurses
help : help.c
	gcc -w -g help.c -o help -lcurses
tetlixlib.o : tetlixlib.c tetlix.h
	gcc -w -g -c tetlixlib.c -lcurses

