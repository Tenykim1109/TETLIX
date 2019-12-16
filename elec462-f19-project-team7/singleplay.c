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
int pattern_num[7][4];
char temp_score[5];
int delay=300;

void init_player(struct player *p){	    // singleplay 시, player 값 초기화 및 map 형성
	
	int i;
	int fd_alpha = open("alpha.txt", O_RDONLY);	    // alphabet이 기록된 파일 open
	int fd_pattern = open("pattern.txt", O_RDONLY);	    // block pattern들이 기록된 파일 open
	int fd_number = open("number.txt", O_RDONLY);	    // score 기록을 위한 number 파일 open
	p->info_p->row_times = 0;
	p->info_p->col_times = 0;
	p->info_p->now_pattern = 0;
	p->info_p->game_over = 0;
	p->info_p->score = 0;
	
	p->bound_p->start_row = 0;			    // block 모양 초기화
	p->bound_p->start_col = 49;
	p->bound_p->left_edge = 31;
	p->bound_p->right_edge = 70;
	p->bound_p->bot_row = 25;
	p->bound_p->score_row = 4;
	p->bound_p->score_col = 90;
	p->info_p->now_step=0;
	p->times = 1200;				    // block 초기 속도 설정
	p->cnt = 0;
	for(i=0; i<=p->bound_p->bot_row; ++i){
		p->bound_p->row_chk[i]=0;
	}
	strcpy(p->record_p->user_score,"00000");	    // score 초기화
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

int set_ticker(int n_msecs){	    // block 이동을 위한 set_ticker
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

void move_pattern(int signum){	    // cnt가 4가 되면 밑으로 한칸 이동하는 move_by_signal을 호출

        fflush(stdin);
        int i,j;
	
	p1->cnt++;
	if(p1->cnt*delay == p1->times){
		p1->cnt=0;
		move_by_signal(p1);
	}
}
int main(int argc, char *argv[]){

	char c;
        struct sigaction time_handler;
        sigset_t blocked;
	int write_fd=atoi(argv[1]);
	p1 = malloc(sizeof(struct player));		// player allocation
	p1->info_p = malloc(sizeof(struct now_info));	// player info allocation
	p1->bound_p = malloc(sizeof(struct bound));	// map bound allocation
	p1->record_p = malloc(sizeof(struct record));	// score record allocation
	init_player(p1);

        initscr();
        crmode();
	nodelay(stdscr, TRUE);
        noecho();
        clear();
        init_map(p1);
        refresh();
        time_handler.sa_sigaction = move_pattern;	// timer 설정    
        time_handler.sa_flags = SA_RESTART | SA_SIGINFO;
        sigemptyset(&blocked);
        time_handler.sa_mask = blocked;
        sigaction(SIGALRM, &time_handler, NULL);
        set_ticker(delay);
        generate_block(p1);		// block을 생성하는 함수 호출
	signal(SIGQUIT, SIG_IGN);	// SIGQUIT ignore
	signal(SIGINT, SIG_IGN);	// SIGINT ignore
	
	while(1){			// player의 keyboard input을 받음
		standend();
		move(0,0);
		addstr("Press Q to quit");
		move(100,0);
		refresh();
		rewind(stdin);

		if(p1->info_p->game_over==1){	// game over 시 실행
			move(30,0);
			addstr("asd");
			refresh();
			break;
			signal(SIGALRM, SIG_IGN);
		}
                c=getch();
                if(c==27||c==0){
                        c=getch();
                        if(c==91){
                                c=getch();
                                if(c==68){
                                        move_by_input(0,-2,p1);	// move left
                                }else if(c==67){
                                        move_by_input(0,2,p1);	// move right
                                }else if(c==66){
                                        move_by_input(1,0,p1);	// move down
                                }else if(c==65){
                                        change_pattern(p1);	// change pattern
                                }
                        }
                }else if(c=='Q'){		// Q 입력 시 종료
			echo();
			nocrmode();
			clear();
			refresh();
			exit(1);
		}
        }
	signal(SIGALRM, SIG_IGN);
	game_over_map(p1);		    // game over
	sleep(5);
	write(write_fd, p1->record_p->user_score, sizeof(char)*5);  // user score를 기록
	free(p1->info_p);
	free(p1->bound_p);
	free(p1->record_p);
	free(p1);
	echo();
	nocrmode();
	clear();
	refresh();
	exit(0);
        endwin();
}
