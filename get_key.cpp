#include <ncurses.h>

int main() {
	// INIT
	int ch;
	initscr();
	raw();			// same as cbreak, disables line buffering
	noecho();			// don't echo user input
	keypad(stdscr, TRUE); 	// allow fn keys
    
    ch = getch();
	endwin();
    printf("KEY NAME : %s - %d\n", keyname(ch),ch);
	return 0;
}
