#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <string.h>

int main(){
	int c;

	initscr();
	crmode();
	noecho();
	clear();

//도움말 출력
	printw("\n\n");
	printw("\t\t\t< H E L P >\n\n\n");

	printw("\t\t<Single play>\n\n");
	printw("\t\tleft arrow key: move left\n", 224);
	printw("\t\tright arrow key: move right\n");
	printw("\t\tup arrow key: change block shape\n");
	printw("\t\tdown arrow key: increase speed\n\n\n");

	printw("\t\t<Double play>\n\n");
	printw("\t\tfor PLAYER 1\n");
	printw("\t\ta: move left\n");
	printw("\t\td: move right\n");
	printw("\t\tw: change block shape\n");
	printw("\t\ts: increase speed\n\n");

	printw("\t\tfor PLAYER 2\n");
	printw("\t\tleft arrow key: move left\n", 224);
	printw("\t\tright arrow key: move right\n");
	printw("\t\tup arrow key: change block shape\n");
	printw("\t\tdown arrow key: increse speed\n\n");

	printw("\t\tpress Q to quit.\n");
	refresh();
	while(1) {
		c=getch();
		if(c=='q' || c=='Q')	break; //뒤로가기
	}
	endwin();
	return 0;
}
