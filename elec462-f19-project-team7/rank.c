#include<signal.h>
#include<stdio.h>
#include<unistd.h>
#include<curses.h>
#include<string.h>
#include"tetlix.h"
#include<fcntl.h>
#include<stdlib.h>
int now_position=0;
char NAME[4]="name";
char SCORE[5]="score";
char score_buf[5][3];
char name_buf[5][5];
struct record user_record[10]; //1등부터 10등까지의 순위를 보여주기 위한 구조체 배열
int fd_rank,fd_number,fd_alpha;

//화면 구성
void init_map(int x){
	clear();
	int i,j,k,l;
	standout();
	for(i=0; i<4; ++i){ //문자열 변환
		lseek(fd_alpha,25*(NAME[i]-'a') ,SEEK_SET);
		read(fd_alpha,name_buf, sizeof(name_buf));
		for(j=0; j<5; ++j){
			for(k=0; k<5; ++k){
				if(name_buf[j][k]=='1'){
					move(4+j, 10+k+i*6);
					addch(' ');
				}
			}
		}
	}
	for(i=0; i<5; ++i){
		lseek(fd_alpha, 25*(SCORE[i]-'a'), SEEK_SET);
		read(fd_alpha, name_buf, sizeof(name_buf));
		for(j=0; j<5; ++j){
			for(k=0; k<5; ++k){
				if(name_buf[j][k]=='1'){
					move(4+j, 40+k+i*6);
					addch(' ');
				}
			}
		}
	}
	refresh();

	if(x==0){
		for(i=0; i<5; ++i){ //숫자 변환
			lseek(fd_number,15*(i+1),SEEK_SET);
			read(fd_number, score_buf, sizeof(score_buf));
			for(j=0; j<5; ++j){
				for(k=0; k<3; ++k){
					if(score_buf[j][k]=='1'){
						move(10+i*6+j,5+k);
						addch(' ');
					}
				}
			}
			for(j=0; j<3; ++j){
				lseek(fd_alpha,25*(user_record[i].user_name[j]-'a'),SEEK_SET);
				read(fd_alpha, name_buf, sizeof(name_buf));
				for(k=0; k<5; ++k){
					for(l=0; l<5; ++l){
						if(name_buf[k][l]=='1'){
							move(10+i*6+k,13+6*j+l);
							addch(' ');
						}
					}
				}
			}
			for(j=0; j<5; ++j){
				lseek(fd_number, 15*(user_record[i].user_score[j]-'0'),SEEK_SET);
				read(fd_number, score_buf, sizeof(score_buf));
				for(k=0; k<5; ++k){
					for(l=0; l<3; ++l){
						if(score_buf[k][l]=='1'){
							move(10+i*6+k, 43+4*j+l);
							addch(' ');
						}
					}
				}
			}
		}
	}else{
		for(i=5; i<10; ++i){
                        lseek(fd_number,15*(i+1),SEEK_SET);
                        read(fd_number, score_buf, sizeof(score_buf));
                        if(i!=9){
				for(j=0; j<5; ++j){
					for(k=0; k<3; ++k){
       						if(score_buf[j][k]=='1'){
	       						move(10+(i-5)*6+j,5+k);
							addch(' ');
          	}
        	}
      	}
			}
      for(j=0; j<3; ++j){
      	lseek(fd_alpha,25*(user_record[i].user_name[j]-'a'),SEEK_SET);
        	read(fd_alpha, name_buf, sizeof(name_buf));
          for(k=0; k<5; ++k){
            for(l=0; l<5; ++l){
              if(name_buf[k][l]=='1'){
                move(10+(i-5)*6+k,13+6*j+l);
                addch(' ');
            	}
            }
					}
        }
        for(j=0; j<5; ++j){
        	lseek(fd_number, 15*(user_record[i].user_score[j]-'0'),SEEK_SET);
            read(fd_number, score_buf, sizeof(score_buf));
            	for(k=0; k<5; ++k){
                for(l=0; l<3; ++l){
                  if(score_buf[k][l]=='1'){
                    move(10+(i-5)*6+k, 43+4*j+l);
                    addch(' ');
                  }
                }
              }
            }
        }
		lseek(fd_number, 15, SEEK_SET);
		read(fd_number, score_buf, sizeof(score_buf));
		for(j=0; j<5; ++j){
			for(k=0; k<3; ++k){
				if(score_buf[j][k]=='1'){
					move(10+24+j,1+k);
					addch(' ');
				}
			}
		}
		lseek(fd_number, 0, SEEK_SET);
		read(fd_number, score_buf, sizeof(score_buf));
		for(j=0; j<5; ++j){
			for(k=0; k<3; ++k){
				if(score_buf[j][k]=='1'){
					move(10+24+j,5+k);
					addch(' ');
				}
			}
		}
	}
	refresh();
}

int main(){
	initscr();
	crmode();
	noecho();
	clear();
	fd_rank=open("rank.txt",O_RDONLY);
	read(fd_rank, user_record, sizeof(struct record)*10); //기록된 점수 읽어옴
	fd_alpha=open("alpha.txt",O_RDONLY);
	fd_number=open("number.txt",O_RDONLY);
	init_map(0); //점수 출력
	int position=0;
	char c;
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	while(1){
		move(0,0);
		addstr("Press Q to exit");
		move(100,0);
		refresh();
		c=getch();
		if(c==27){
			c=getch();
			if(c==91){
				c=getch();
				if(c==68||c==67){ //점수기록 페이지 이동
					init_map((++position)%2);
				}
			}
		}else if(c=='Q'){ //실행 종료
			close(fd_rank);
			close(fd_alpha);
			close(fd_number);
			nocrmode();
			endwin();
			exit(0);
		}
	}
}
