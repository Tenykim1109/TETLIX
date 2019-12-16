#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<curses.h>
#include<signal.h>
#include<sys/wait.h>
#include<string.h>
#include<stdlib.h>
int now_position=0;
int rank_position=0;
char temp[2];
char TETLIX[6]="tetlix";
char SINGLE[6]="single";
char DOUBLE[6]="double";
char RANK[4]="rank";
char HELP[4]="help";
char EXIT[4]="exit";
char CHANGE_MAP[6][6]={"single","double","rank","help","exit"};
char buf_alpha[5][5];
int fd_alpha;
int change_case[5]={6,6,4,4,4};

//화면 구성
void init_map(){
	clear();
	refresh();
	standout();
	fd_alpha = open("alpha.txt", O_RDONLY); //문자열 변환을 위한 fd
	int i,j,k;
	for(i=0; i<6; ++i){
		lseek(fd_alpha,25*(SINGLE[i]-'a'),SEEK_SET);
		read(fd_alpha, buf_alpha, sizeof(buf_alpha));
		for(j=0; j<5; ++j){
			for(k=0; k<5; ++k){
				if(buf_alpha[j][k]=='1'){
					move(4+j,20+6*i+k);
					addch(' ');
				}
			}
		}
	}
	standout();
	for(i=0; i<6; ++i){
		lseek(fd_alpha, 25*(DOUBLE[i]-'a'),SEEK_SET);
		read(fd_alpha, buf_alpha, sizeof(buf_alpha));
		for(j=0; j<5; ++j){
			for(k=0; k<5; ++k){
				if(buf_alpha[j][k]=='1'){
					move(11+j,20+6*i+k);
					addch(' ');
        }
			}
		}
	}
	standout();
	for(i=0; i<4; ++i){
		lseek(fd_alpha, 25*(RANK[i]-'a'), SEEK_SET);
		read(fd_alpha, buf_alpha, sizeof(buf_alpha));
		for(j=0; j<5; ++j){
			for(k=0; k<5; ++k){
				if(buf_alpha[j][k]=='1'){
		 			move(18+j,27+6*i+k);
	 				addch(' ');
				}
			}
		}
	}
	standout();
	for(i=0; i<4; ++i){
		lseek(fd_alpha, 25*(HELP[i]-'a'), SEEK_SET);
		read(fd_alpha, buf_alpha, sizeof(buf_alpha));
		for(j=0; j<5; ++j){
			for(k=0; k<5; ++k){
				if(buf_alpha[j][k]=='1'){
					move(25+j,27+6*i+k);
					addch(' ');
        }
			}
		}
	}
	standout();
	for(i=0; i<4; ++i){
		lseek(fd_alpha, 25*(EXIT[i]-'a'), SEEK_SET);
		read(fd_alpha, buf_alpha, sizeof(buf_alpha));
		for(j=0; j<5; ++j){
			for(k=0; k<5; ++k){
				if(buf_alpha[j][k]=='1'){
					move(32+j,27+6*i+k);
					addch(' ');
				}
			}
		}
	}
	standend();
}
void int_to_string(int x){
       	temp[0]=x+'0';
}
void reset_screen(int pos){
	int i,j,k;
	standout();
	for(i=0; i<7; ++i){
		for(j=0; j<37; ++j){
			move(i+pos*7+3, 19+j);
			addch(' ');
		}
	}
	standend();
	for(i=0; i<change_case[pos]; ++i){
		lseek(fd_alpha, 25*(CHANGE_MAP[pos][i]-'a'), SEEK_SET);
		read(fd_alpha, buf_alpha, sizeof(buf_alpha));
		for(j=0; j<5; ++j){
			for(k=0; k<5; ++k){
				if(buf_alpha[j][k]=='1'&&pos<=1){
					move(4+7*pos+j, 20+i*6+k);
					addch(' ');
				}else if(buf_alpha[j][k]=='1'&&pos>=2){
					move(4+7*pos+j, 27+i*6+k);
					addch(' ');
				}
			}
		}
	}
	refresh();
}
void start_screen(){ //시작 화면 출력
	clear();
	int i,j,k;
	fd_alpha = open("alpha.txt", O_RDONLY);
	standout();
	for(i=0; i<6; ++i){ //형식에 맞게 문자열 변환
		lseek(fd_alpha, 25*(TETLIX[i]-'a'),SEEK_SET);
		read(fd_alpha, buf_alpha, sizeof(buf_alpha));
		for(j=0; j<5; ++j){
			for(k=0; k<5; ++k){
				move(j+20, k+i*6+40);
				if(buf_alpha[j][k]=='1'){
					addch(' ');
				}
			}
		}
	}
	move(27, 48);
	addstr("Press S to start game");
	standend();
	refresh();
}
int main(){
	char c;
	int status=0;
	int pid,pid2;
	int pipe1[2];
	char score_buf[5];
	initscr();
	crmode();
	noecho();
	clear();
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	start_screen();
	while(1){
		c=getch();
		if(c=='S'){ //게임 모드 선택 화면 진입
			break;
		}
	}
	clear();
	init_map();
	refresh();
	reset_screen(now_position);
	move(0,0);
	refresh();
	while(1){
		c=getch();
		if(c==27){
			c=getch();
			if(c==91){
				c=getch();
				if(c==66){
					now_position++;
					now_position%=5;
					init_map();
					reset_screen(now_position);
					move(0,0);
					refresh();
				}
				else if(c==65){
					now_position--;
					now_position+=5;
					now_position%=5;
					init_map();
					reset_screen(now_position);
					move(0,0);
					refresh();
				}
			}
		}
		else if(c=='\n'){
			if(now_position==4){
				close(fd_alpha);
				nocrmode();
				echo();
				clear();
				refresh();
				endwin();
				exit(0);
			}
			pipe(pipe1);

//게임 모드를 선택하여 그에 맞는 모드 실행
			if((pid=fork())==-1)
				perror("fork");
			else if(pid==0){
				if(now_position==0){
					int_to_string(pipe1[1]);
					execlp("./singleplay", "./singleplay",temp,NULL);
				}
				else if(now_position==1){
					execlp("./doubleplay", "./doubleplay",NULL);
				}
				else if(now_position==2){
					execlp("./rank","./rank",NULL);
				}
				else if(now_position==3) {
					execlp("./help", "./help", NULL);
				}
			}else{
				if(wait(&status)==-1)
					perror("wait");
				if(status>>8==1){
					crmode();
					noecho();
					refresh();
					init_map();
					reset_screen(now_position);
					continue;
				}
				if(now_position==0){
					if((pid2=fork())==-1){
						perror("fork");
					}
					if(pid2==0){
						if(read(pipe1[0], score_buf,sizeof(score_buf))==-1)
							perror("read");
						execlp("./regist", "./regist",score_buf,NULL);
					}else{
						wait(NULL);
						crmode();
						noecho();
						init_map();
						reset_screen(now_position);
						continue;
					}
				}else{
					crmode();
					noecho();
					init_map();
					reset_screen(now_position);
					continue;
				}
			}
		}
	}
}
