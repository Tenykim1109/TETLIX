#include<stdio.h>
#include"tetlix.h"
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<curses.h>
#include<sys/time.h>
#include<string.h>
#include<signal.h>
#include<time.h>
struct player *p1;
struct player *p2;
int pattern_num[7][4];
char temp_score[5];
int delay=300;
int fd_alpha;
int fd_number;
char buf_alpha[5][5];
char buf_number[5][3];
char PLAYER[6]="player";    // print player
char WIN[3]="win";	    // print when the player wins
char DRAW[4]="draw";	    // print when players draw

void init_player(struct player *p){	// doubleplay 시, player 값 초기화 및 map 생성
	
	int i;	
	int fd_alpha = open("alpha.txt", O_RDONLY);	     // alphabet이 기록된 파일 open
	int fd_pattern = open("pattern.txt", O_RDONLY);	     // block pattern들이 기록된 파일 open
	int fd_number = open("number.txt", O_RDONLY);	     // score 기록을 위한 number 파일 open
	p->info_p->row_times = 0;
	p->info_p->col_times = 0;
	p->info_p->now_pattern = 0;
	p->info_p->game_over = 0;
	p->info_p->score = 0;
	p->times=1200;		    // block 초기 속도 설정
	p->cnt=0;
		
	strcpy(p->record_p->user_score,"00000");
	memset(p->chk_map, 0, sizeof(p->chk_map));
	lseek(fd_pattern,p->info_p->now_pattern*16 ,SEEK_SET);
	read(fd_pattern, p->buf_pattern, sizeof(char)*16);
	
	for(i=0; i<5; ++i){
		read(fd_number, p->buf_score[i], sizeof(char)*15);
		lseek(fd_number, 0, SEEK_SET);
	}
	close(fd_alpha);
	close(fd_pattern);
	close(fd_number);
}
void init_bound(){	    // block 모양 초기화
	
	p1->bound_p->start_row = 0;
	p1->bound_p->start_col = 57;
	p1->bound_p->left_edge = 37;
	p1->bound_p->right_edge = 76;
	p1->bound_p->bot_row = 25;
	p1->bound_p->score_row = 4;
	p1->bound_p->score_col = 5;

	p2->bound_p->start_row = 0;
	p2->bound_p->start_col = 111;
	p2->bound_p->left_edge = 91;
	p2->bound_p->right_edge = 130;
	p2->bound_p->bot_row = 25;
	p2->bound_p->score_row = 4;
	p2->bound_p->score_col = 134;
}

int set_ticker(int n_msecs){		// block 이동을 위한 set_ticker
        struct itimerval new_timeset;
        long n_sec, n_usecs;

        n_sec = n_msecs/1000;
        n_usecs = (n_msecs%1000)*1000L;

        new_timeset.it_interval.tv_sec = n_sec;
        new_timeset.it_interval.tv_usec = n_usecs;
        new_timeset.it_value.tv_sec = n_sec;
        new_timeset.it_value.tv_usec = n_usecs;

        return setitimer(ITIMER_REAL, &new_timeset, NULL);

}
void time_handle(int signum){	    // cnt가 4가 되면 밑으로 한칸 이동하는 move_by_signal을 호출

        fflush(stdin);
        int i,j;
	p1->cnt++;
	p2->cnt++;
	if(p1->cnt*delay == p1->times){
		p1->cnt=0;
                move_by_signal(p1);
        }
	if(p2->cnt*delay == p2->times){
		p2->cnt=0;
		move_by_signal(p2);
	}
}
int main(){

	char c;
	struct sigaction time_handler;
	sigset_t blocked;
	p1 = malloc(sizeof(struct player));		// player1 allocation
	p1->info_p = malloc(sizeof(struct now_info));	// player1 info allocation
	p1->bound_p = malloc(sizeof(struct bound));	// player1 map bound allocation
	p1->record_p = malloc(sizeof(struct record));	// player1 score record allocation
	init_player(p1);
	
	p2 = malloc(sizeof(struct player));		// player2 allocation
	p2->info_p = malloc(sizeof(struct now_info));	// player2 info allocation
	p2->bound_p = malloc(sizeof(struct bound));	// player2 map bound allocation
	p2->record_p = malloc(sizeof(struct record));	// player2 score record allocation

	init_player(p2);
	
	init_bound();

        initscr();
        crmode();
        nodelay(stdscr, TRUE);
	noecho();
        clear();
        
	init_map(p1);
	init_map(p2);

        refresh();
	

        time_handler.sa_sigaction = time_handle;	// 블록 이동에 대한 timer 설정 
        time_handler.sa_flags = SA_RESTART | SA_SIGINFO;
        sigemptyset(&blocked);
        time_handler.sa_mask = blocked;
        sigaction(SIGALRM, &time_handler, NULL);
        set_ticker(delay);
        generate_block(p1);	    // block을 생성하는 함수 호출
	generate_block(p2);	    // block을 생성하는 함수 호출
	signal(SIGINT, SIG_IGN);    // SIGINT ignore
	signal(SIGQUIT, SIG_IGN);   // SIGQUIT ignore
	while(1){		    // player의 keyboard input을 받음
		move(0,0);
		addstr("Press Q to quit");
		move(100,0);
		refresh();

		if(p1->info_p->game_over==1||p2->info_p->game_over==1){	    // game over 시 실행
			signal(SIGALRM, SIG_IGN);
			if(p1->info_p->game_over==1){
				game_over_map(p1);
				winner_map(p2);
			}else{
				game_over_map(p2);
				winner_map(p1);
			}
			sleep(3);
			nocrmode();
			echo();
			exit(0);
		}
                c=getch();
		if(c=='Q'){	// Q 입력 시 종료
			nocrmode();
			echo();
			exit(1);
		}	
                if(c==27||c==0){
                        c=getch();
                        if(c==91){
                                c=getch();
                                if(c==68){
                                        move_by_input(0,-2,p2);	    // player 2 move left
                                }else if(c==67){
                                        move_by_input(0,2,p2);	    // player 2 move right
                                }else if(c==66){
                                        move_by_input(1,0,p2);	    // player2 move down
                                }else if(c==65){
                                        change_pattern(p2);	    // player2 change pattern
                                }
                        }
                }else if(c=='w'){		    // player 1 change pattern
			change_pattern(p1);
		}else if(c=='a'){		    // player 1 move left
			move_by_input(0,-2,p1);
		}else if(c=='d'){		    // player 1 move right
			move_by_input(0,2,p1);
		}else if(c=='s'){		    // player 1 move down
			move_by_input(1,0,p1);
		}
        }

        endwin();

	free(p1->info_p);
	free(p1->bound_p);
	free(p1->record_p);
	free(p1);
	
	free(p2->info_p);
	free(p2->bound_p);
	free(p2->record_p);
	free(p2);
}
