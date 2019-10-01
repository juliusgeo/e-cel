#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <ncurses.h>
#define COL_CHAR L'\x2503';
#define ROW_CHAR L'\x2501';
#define CROSS L'\x254B';
#define SPACE L'\x2003';
#define TOP_RIGHT_CORNER L'\x2513';
#define TOP_LEFT_CORNER L'\x250F';
#define LOW_LEFT_CORNER L'\x2517';
#define LOW_RIGHT_CORNER L'\x251B';
#define LEFT_SIDE L'\x2523';
#define RIGHT_SIDE L'\x252B';
#define SELECTION L'\x2590'; 
int w_col;
int w_row;
int num_c = 4;
int num_r = 10;
int sel_y = 0;
int sel_x = 0;
typedef struct Cell {
	int x;
	int y;
	float *val;
	char* fun;
} Cell;
void init_screen(wchar_t** s){
	int i, n;
	for(i=0; i<sizeof(s)/sizeof(wchar_t*); i++){
		for(n=0; n<wcslen(s[i]); n++){
			s[i][n] = SPACE;
		}
	}
}
void update_screen(wchar_t** s, Cell*** cells){
	int c = floor(w_col/num_c);
	int r = floor(w_row/num_r);
	int row, col;
	wchar_t cur_char;
	for(row=0; row<w_row; row++){
		if(row%r== 0){
			cur_char = ROW_CHAR;
			s[row][0] = LEFT_SIDE; 
			s[row][w_col-2] = RIGHT_SIDE;

		}
		else{
			cur_char = SPACE;
			s[row][0] = COL_CHAR; 
			s[row][w_col-2] = COL_CHAR;
		};
		s[row][w_col-1] = L'\0';
		for(col=1; col<w_col-2; col++){
			if(col%c==0 && row%r==0){
				s[row][col] = CROSS;	
			}
			else if(col%c==0){
				s[row][col] = COL_CHAR;
			}
			else{
				s[row][col] = cur_char;
			}
		}
	}
	s[0][0] = TOP_LEFT_CORNER;
	s[0][w_col-2] = TOP_RIGHT_CORNER;
	int i, n, z;
	int buffer_width = c-2;
	//printf("%d\n", sizeof(cells)/sizeof(cells[0]));
	for(i=0; i<num_r; i++){
		for(n=0; n<num_c; n++){
			Cell* cur_cell = cells[i][n];
			if(cur_cell->val == NULL){
				continue;
			}
			int c_x = (cur_cell->x*c);
			int c_y = (cur_cell->y*r)+floor(r/2);
			char* buf = malloc(sizeof(char)*buffer_width);
			gcvt(*(cur_cell->val), buffer_width-1, buf);
			mbstowcs(&s[c_y][c_x+2], buf, buffer_width);
			
			for(z=0; z<w_col-1; z++){
				if(s[c_y][z] == L'\0') s[c_y][z] = L' '; 
			}
			free(buf);
		}	
	}
	s[sel_y*r+(int)(r/2)][sel_x*c+1] = SELECTION;
	
}
void draw_screen(wchar_t** s){
	int i, n;
	erase();
	for(i=0; i<w_row; i++){
		addwstr(s[i]);
		addwstr(L"\n");
	}
	refresh();
}
void move_vert(bool up){
	if(up){sel_y--;}
	else{
		sel_y++;
	}
	if(sel_y > num_r-1) sel_y = num_r;
	if(sel_y < 0) sel_y = 0;
}
void move_horz(bool left){
	if(left){sel_x--;}
	else{
		sel_x++;
	}
	if(sel_x > num_c-1) sel_x = num_c-1;
	if(sel_x < 0) sel_x = 0;
}
float parse_formula(Cell* cell, char* buf){

}
int main(int argc, char** argv){
	wchar_t** screen;
	setlocale(LC_ALL, "");
	initscr();
	getmaxyx(stdscr, w_row, w_col);
	w_row -= 2;
	screen = (wchar_t**)malloc(sizeof(wchar_t*)*w_row);
	int i, n;
	for(i=0; i<w_row; i++){
		screen[i] = malloc(sizeof(wchar_t)*(w_col));
	}
	init_screen(screen);
	Cell*** cells = malloc(sizeof(Cell**)*num_r);
	for(i=0; i<num_r; i++){
		cells[i] = malloc(sizeof(Cell*)*num_c);
		for(n=0; n<num_c; n++){
			cells[i][n] = (Cell*)malloc(sizeof(Cell));
			cells[i][n]->x = n;
			cells[i][n]->y = i;
			cells[i][n]->val = NULL;
		}
		
	}
	noecho();
	keypad(stdscr, TRUE);
	int c;
	typedef enum state {INPUT=0, SELECT=1} state;
	state s = SELECT;
	while(1){
		getmaxyx(stdscr, w_row, w_col);
		w_row -= 2;
		update_screen(screen, cells);
		draw_screen(screen);
		if(s == SELECT){
			if((c=getch()) != ERR){
				switch(c){
					case KEY_UP:
						move_vert(true);
						break;
					case KEY_DOWN:
						move_vert(false);
						break;
					case KEY_LEFT:
						move_horz(true);
						break;
					case KEY_RIGHT:
						move_horz(false);
						break;
					case 10:
						s = INPUT;
						break;
					default:
						break;
				}
			}
		}
		else if(s == INPUT){
			echo();
			char* input_buf = malloc(sizeof(char)*(w_col-1));
			getstr(input_buf);
			if(cells[sel_y][sel_x]->val == NULL){
				cells[sel_y][sel_x]->val = malloc(sizeof(float));
			}
			if(input_buf[0] == '='){
				parse_formula(cells[sel_y][sel_x], input_buf);
			}
			else{
				*(cells[sel_y][sel_x]->val) = atof(input_buf);
			}
			free(input_buf);	
			noecho();
			s = SELECT;
		}
	} 
}
