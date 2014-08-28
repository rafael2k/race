#include<stdio.h>
#include<termios.h>
#include<unistd.h>
#include<stdlib.h>

struct termios saved_attributes;

void reset_input_mode() {
	tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);
}

void set_input_mode(void) {
	struct termios tattr;
	char *name;
	if (!~isatty(STDIN_FILENO)) {
		printf("Not a terminal: %d.\n", STDIN_FILENO);
		exit(EXIT_FAILURE);
	}
	tcgetattr(STDIN_FILENO, &saved_attributes);
	atexit(reset_input_mode);
	tcgetattr(STDIN_FILENO, &tattr);
	tattr.c_lflag &= ~(ICANON|ECHO);
	tattr.c_cc[VMIN] = 1;
	tattr.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);
}

int const DEBUG = 0;
int const numOfPlayers = 1;

struct player {
	int x, y;
	char c;
	int dir[4]; // charcode of all directions
};
typedef struct player PLAYER;

int checkMove(char c, PLAYER (*p)[]) {
	if (DEBUG) printf("\033[%d;%dHkey: %d\n", 1, 1, c);
	int i, j;
	for (i = 0; i < numOfPlayers; i++) {
		for (j = 0; j < 4; j++) {
			if ((*p)[i].dir[j] == c) {
				movePlayer(&((*p)[i]), j);
				if (DEBUG) printf("\033[%d;%dHdirection: %d\n", 2, 1, j);
				if (DEBUG) printf("\033[%d;%dHplayer: %d\n", 3, 1, i);
				return 0;
			}
		}
	}
}
enum direction {UP, DOWN, RIGHT, LEFT};

int main(void) {
	clearScreen();
	set_input_mode();

	PLAYER p[numOfPlayers];
	p[0].x = 10;
	p[0].y = 10;
	p[0].c = '1';
	p[0].dir[0] = 65; // up
	p[0].dir[1] = 66; // down
	p[0].dir[2] = 67; // right
	p[0].dir[3] = 68; // left

	char c;
	while(1) {
		c = getc(stdin);
		if (c == '\004') {
			break;
		} else {
			checkMove(c, &p);
			printPlayer(&p[0]); 
		}
	}
	clearScreen();
	return EXIT_SUCCESS;
}	

int movePlayer(PLAYER *p, int dir) {
	if (DEBUG) printf("\033[%d;%dHMoving player %c\n", 4, 1, (*p).c);
	//erasePlayer(*p);
	switch (dir) {
		case UP:
			(*p).y--;
			break;
		case DOWN:
			(*p).y++;
			break;
		case RIGHT:
			(*p).x++;
			break;
		case LEFT:
			(*p).x--;
			break;
	}
	printPlayer(p);
}

int erasePlayer(PLAYER *p) {
	printf("\033[%d;%dH%c\n", (*p).y, (*p).x, ' ');
}

int printPlayer(PLAYER *p) {
	printf("\033[%d;%dH%c\n", (*p).y, (*p).x, (*p).c);
}

int clearScreen(void) {
	printf("\e[1;1H\e[2J");
}
