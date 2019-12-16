#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<curses.h>
#include<sys/time.h>
#include<string.h>
#include<signal.h>
#include<stdlib.h>
#include<time.h>
#include"tetlix.h"
int cnt_chk=0;
int score_table[5]={0, 10, 20, 40, 80};
int step_table[3]={10, 20, 30};
int game_over=0;
void game_over_map(struct player *p);
void move_by_input(int x, int y, struct player *p);
void change_pattern(struct player *p);
int cnt_times;
int reach_end_chk(int x, int y, int mode, struct player *p);
int reach_end_chk_by_move(int x, int y, struct player *p);
int reach_end_chk_by_change(struct player *p);
int reach_end_chk_by_xmove(struct player *p);
int reach_end_chk_by_ymove(int y, struct player *p);
void delete_line(struct player *p);
void chk_map_update(struct player *p);
void generate_block(struct player *p);
void init_map(struct player *p);
void score_to_string(int x, struct player *p);

//점수를 문자열로 변환
void score_to_string(int x, struct player *p){
	int i=0;char c=0;
	for(i=0; i<5; ++i){
		p->record_p->user_score[4-i]=x%10+'0';
		c=p->record_p->user_score[4-i];
		x/=10;
	}
}

//게임 화면 구성
void init_map(struct player *p){
	int i,j,k;
	char TETLIX[6]="tetlix";
	char SCORE[5]="score";

	char buf_alpha[5][5];
	char buf_pattern[4][4];
	char buf_number[5][3];

	/*textfile의 내용을 읽어서 1인 부분을 standout 해줌*/
	int fd_alpha=open("alpha.txt", O_RDONLY); //for converting string
	int fd_pattern=open("pattern.txt", O_RDONLY); //for generate_block
	int fd_number=open("number.txt", O_RDONLY); //for converting score

	standout();
	for(i=0; i<=p->bound_p->bot_row+1; ++i){
		move(i, p->bound_p->left_edge-1);
		addch(' ');
		move(i, p->bound_p->right_edge+1);
		addch(' ');
	}
	for(i=p->bound_p->left_edge; i<=p->bound_p->right_edge; ++i){
		move(p->bound_p->bot_row+1, i);
		addch(' ');
	}

	for(k=0; k<6; ++k){
		lseek(fd_alpha,(TETLIX[k]-'a')*25 ,SEEK_SET);
		read(fd_alpha, buf_alpha, sizeof(buf_alpha));
		for(i=0; i<5; ++i){
			for(j=0; j<5; ++j){
				move(p->bound_p->bot_row+i+3,p->bound_p->left_edge+4+6*k+j);
				if(buf_alpha[i][j]=='1'){
					addch(' ');
				}
			}
		}
	}

	for(k=0; k<5; ++k){
		lseek(fd_alpha, (SCORE[k]-'a')*25, SEEK_SET);
		read(fd_alpha, buf_alpha, sizeof(buf_alpha));
		for(i=0; i<5; ++i){
			for(j=0; j<5; ++j){
				move(p->bound_p->score_row+i, p->bound_p->score_col+j+k*6);
				if(buf_alpha[i][j]=='1'){
					addch(' ');
				}
			}
		}
	}

	lseek(fd_number, 0, SEEK_SET);
	read(fd_number, buf_number, sizeof(buf_number));

	for(i=0; i<5; ++i){
		for(j=0; j<5; ++j){
			for(k=0; k<3; ++k){
				move(p->bound_p->score_row+6+j, p->bound_p->score_col+5+k+i*4);
				if(buf_number[j][k]=='1'){
					addch(' ');
				}
			}
		}
	}
	close(fd_alpha);
	close(fd_pattern);
	close(fd_number);
	move(0,0);
	refresh();
	standend();
}

//블럭이 한 줄을 채우는 지 확인
void chk_map_update(struct player *p){
	fflush(stdin);
	int i,j,k;
	cnt_chk=0;
	int count=0;
	cnt_times++;
	int fd_number= open("number.txt", O_RDONLY);
	for(i=p->bound_p->bot_row; i>0; --i)
		p->bound_p->row_chk[i]=0;

	standout();
	for(i=0; i<4; ++i){
		for(j=0; j<8; ++j){
			if(p->buf_pattern[i][j]=='1'){
				p->chk_map[i+p->bound_p->start_row+p->info_p->row_times][j+p->bound_p->start_col+p->info_p->col_times]=1;
				move(i+p->bound_p->start_row+p->info_p->row_times, j+p->bound_p->start_col+p->info_p->col_times);
				addch(' ');
			}
		}
	}

	move(0,0);
	refresh();
	for(i=p->bound_p->bot_row; i>0; --i){
		cnt_chk=0;
		for(j=p->bound_p->left_edge; j<=p->bound_p->right_edge; ++j){
			if(p->chk_map[i][j]==1){
				cnt_chk++;
			}
		}
		if(cnt_chk==p->bound_p->right_edge-p->bound_p->left_edge+1){
			count++;
			p->bound_p->row_chk[i]=1;
		}
	}
	if(count!=0){ //한 줄이 꽉 찼을 경우
		delete_line(p); //해당하는 줄 삭제
		p->info_p->score+=score_table[count];
		count=0;
		score_to_string(p->info_p->score, p);
	}else{
		p->info_p->score++;
		score_to_string(p->info_p->score,p);
	}


	if(p->info_p->score>=step_table[p->info_p->now_step]){
		if(p->times>300){
			p->info_p->now_step++;
			p->times -=300;
		}
	}

	standend();
	for(i=0; i<5; ++i){
		for(j=0; j<5; ++j){
			for(k=0; k<3; ++k){
				move(p->bound_p->score_row+6+j, p->bound_p->score_col+5+k+i*4);
				addch(' ');
			}
		}
	}
	move(0,0);
	refresh();
	standout();

	for(i=0; i<5; ++i){
		lseek(fd_number, (p->record_p->user_score[i]-'0')*15, SEEK_SET);
		read(fd_number, p->buf_score, sizeof(char)*15);
		for(j=0; j<5; ++j){
			for(k=0; k<3; ++k){
				move(p->bound_p->score_row+6+j, p->bound_p->score_col+5+k+i*4);
                                if(p->buf_score[j][k]=='1'){
                                        addch(' ');
                                }
			}
		}
	}

	refresh();

	close(fd_number);
	move(0,0);
	refresh();
	standend();
	generate_block(p);
	return;
}

//한 줄이 쌓였을 경우 라인 삭제
void delete_line(struct player *p){
	int i,j,k,l;
	int cnt=0;
	int chk_for_end=0;
	int end=0;
	char c;
	fflush(stdin);
	for(j=0; j<3; ++j){
		standend();
		for(i=p->bound_p->bot_row; i>0; --i){
			if(p->bound_p->row_chk[i]==1){
				move(i, p->bound_p->left_edge);
				addstr("                                        ");
			}
		}
		refresh();
		usleep(100000);
		standout();
		for(i=p->bound_p->bot_row; i>0; --i){
			if(p->bound_p->row_chk[i]==1){
				move(i, p->bound_p->left_edge);
				addstr("                                        ");
			}
		}
		refresh();
		usleep(100000);
	}
	for(i=p->bound_p->bot_row; i>0; --i){
		end=chk_for_end=0;
		if(p->bound_p->row_chk[i]==1){
			cnt++;
			standend();
			for(j=p->bound_p->left_edge; j<=p->bound_p->right_edge; ++j){
				move(i,j);
				addch(' ');
				p->chk_map[i][j]=0;
			}
			p->bound_p->row_chk[i]=0;
		}else if (p->bound_p->row_chk[i]==0&&cnt!=0){
			for(j=p->bound_p->left_edge; j<=p->bound_p->right_edge; ++j){
				move(i,j);
				standend();
				addch(' ');
				p->chk_map[i+cnt][j]=p->chk_map[i][j];
				if(p->chk_map[i][j]!=0){
					standout();
					move(i+cnt,j);
					addch(' ');
					chk_for_end=1;
				}
				p->chk_map[i][j]=0;
			}
			if(chk_for_end==0)
				break;
		}
	}

	refresh();
	fflush(stdin);
	standend();

	return;
}

//랜덤한 모양으로 블럭 생성
void generate_block(struct player *p){

	fflush(stdin);
	srandom(time(NULL));
	int fd_pattern = open("pattern.txt", O_RDONLY); //텍스트 파일에 저장된 블럭 모양 읽어옴
	p->info_p->col_times=p->info_p->row_times=0;
	p->info_p->now_pattern = (random()%7)*4; //random하게 패턴 지정
	lseek(fd_pattern, p->info_p->now_pattern*32, SEEK_SET);
	read(fd_pattern, p->buf_pattern, sizeof(p->buf_pattern));
	int i=0;
	move(0,0);
	refresh();
	standend();
	close(fd_pattern);
	return;
}

//좌우키 입력을 통한 블럭 이동
void move_by_input(int x, int y, struct player *p){
	int i,j;
	fflush(stdin);
	if(reach_end_chk(x,y,0,p)==0){
		return;
	}
	standend();
	for(i=0; i<4; ++i){
		for(j=0; j<8; ++j){
			if(p->buf_pattern[i][j]=='1'){
				move(p->bound_p->start_row+p->info_p->row_times+i, p->bound_p->start_col+p->info_p->col_times+j);
				addch(' ');
				move(0,0);
			}
		}
	}
	refresh();
	p->info_p->row_times+=x; p->info_p->col_times+=y;
	standout();
	for(i=0; i<4; ++i){
		for(j=0; j<8; ++j){
			move(p->bound_p->start_row+p->info_p->row_times+i, p->bound_p->start_col+p->info_p->col_times+j);
			if(p->buf_pattern[i][j]=='1'){
				addch(' ');
				move(0,0);
			}
		}
	}
	move(0,0);
	refresh();
}

//alarm 시그널로 블럭이 내려오는 속도 조절
void move_by_signal(struct player *p){
	int i,j;

	if(reach_end_chk(1,0,0,p)==0){ //바닥이나 벽에 닿았을 경우 chk_map_update 호출
    if(p->info_p->row_times==0){
			game_over=1;
			p->info_p->game_over=1;
			return;
		}
		chk_map_update(p);
		return;
  }
  standend();
  for(i=0; i<4; ++i){
    for(j=0; j<8; ++j){
      if(p->buf_pattern[i][j]=='1'){
        move(p->bound_p->start_row+p->info_p->row_times+i, p->bound_p->start_col+p->info_p->col_times+j);
        addch(' ');
    	}
    }
  }
  refresh();
  p->info_p->row_times++;
  standout();
  for(i=0; i<4; ++i){
    for(j=0; j<8; ++j){
      move(p->bound_p->start_row+p->info_p->row_times+i, p->bound_p->start_col+p->info_p->col_times+j);
      if(p->buf_pattern[i][j]=='1'){
        addch(' ');
      	move(0,0);
      }
    }
  }
	move(0,0);
  refresh();
}

//doubleplay에서 이긴 사람한테 출력하는 화면
void winner_map(struct player *p){
	char YOUWIN[6]="youwin";
	int i,j,k;
	int fd_alpha=open("alpha.txt", O_RDONLY);
	char buf_alpha[5][5]={0};

	standout();
	for(i=p->bound_p->bot_row; i>=0; --i){
		for(j=p->bound_p->left_edge; j<=p->bound_p->right_edge; ++j){
			move(i,j);
			addch(' ');
		}
	}
	standend();
	for(i=0; i<6; ++i){
		lseek(fd_alpha, 25*(YOUWIN[i]-'a'), SEEK_SET);
		read(fd_alpha, buf_alpha, sizeof(buf_alpha));
		for(j=0; j<5; ++j){
			for(k=0; k<5; ++k){
				if(buf_alpha[j][k]=='1'){
					if(i<3){
						move(p->bound_p->start_row+3+j, p->bound_p->left_edge+4+i*6+k);
						addch(' ');
					}else{
						move(p->bound_p->start_row+10+j, p->bound_p->left_edge+4+i*6+k);
						addch(' ');
					}
				}
			}
		}
	}
	refresh();
	close(fd_alpha);
}

//특정 키를 입력받았을 때 블럭 회전
void change_pattern(struct player *p){
	int fd_pattern = open("pattern.txt", O_RDONLY);
	fflush(stdin);
	int i,j;
	if(reach_end_chk(0,0,1,p)==0){
		return;
	}
	standend();

	for(i=0; i<4; ++i){
		for(j=0; j<8; ++j){
			if(p->buf_pattern[i][j]=='1'){
				move(p->bound_p->start_row+p->info_p->row_times+i, p->bound_p->start_col+p->info_p->col_times+j);
				addch(' ');
				move(0,0);
			}
		}
	}
	refresh();
	standout();
	p->info_p->now_pattern = (p->info_p->now_pattern+1)%4+((p->info_p->now_pattern)/4)*4;

	lseek(fd_pattern, p->info_p->now_pattern*32, SEEK_SET);
	read(fd_pattern, p->buf_pattern, sizeof(p->buf_pattern)); //패턴 읽어옴

	for(i=0; i<4; ++i){
		for(j=0; j<8; ++j){
			if(p->buf_pattern[i][j]=='1'){
				move(p->bound_p->start_row+p->info_p->row_times+i, p->bound_p->start_col+p->info_p->col_times+j);
				addch(' ');
				move(0,0);
			}
		}
	}
	move(0,0);
	refresh();

}

int reach_end_chk(int x, int y, int mode, struct player *p){
	fflush(stdin);
	if(mode==0){
		return reach_end_chk_by_move(x, y,p);
	}
	else{
		return reach_end_chk_by_change(p);
	}

}

//블럭을 회전시켰을 때 테두리에 닿는지 체크
int reach_end_chk_by_change(struct player *p){
	fflush(stdin);
	int temp=0;
	int i,j;
	int not_touched=1;
	int fd_pattern = open("pattern.txt", O_RDONLY);
	temp = (p->info_p->now_pattern+1)%4+((p->info_p->now_pattern)/4)*4;
	lseek(fd_pattern, temp*32, SEEK_SET);
	read(fd_pattern, p->buf_pattern, sizeof(p->buf_pattern));
	for(i=0; i<4; ++i){
		for(j=0; j<8; ++j){
			if(p->buf_pattern[i][j]=='1')
				if(p->chk_map[i+p->bound_p->start_row+p->info_p->row_times][j+p->bound_p->start_col+p->info_p->col_times]==1||p->bound_p->bot_row<i+p->bound_p->start_row+p->info_p->row_times||p->bound_p->left_edge>j+p->bound_p->start_col+p->info_p->col_times||p->bound_p->right_edge<j+p->bound_p->start_col+p->info_p->col_times)
					not_touched=0;
			else
				continue;
		}
	}
	temp= p->info_p->now_pattern;
	lseek(fd_pattern, temp*32, SEEK_SET);
	read(fd_pattern, p->buf_pattern, sizeof(p->buf_pattern));
	close(fd_pattern);
	return not_touched;
}

//블럭을 좌우로 움직일 때 테두리에 닿는 지 체크
int reach_end_chk_by_move(int x, int y, struct player *p){
	fflush(stdin);
	if(x==0)
		return reach_end_chk_by_ymove(y,p);
	return reach_end_chk_by_xmove(p);
}

//바닥에 닿는 지 체크
int reach_end_chk_by_xmove(struct player *p){
	if(game_over==1)
		return 0;
	fflush(stdin);
	int not_touched=1;
	int i,j;

	for(i=0; i<4; ++i){
		for(j=0; j<8; ++j){
			if(p->buf_pattern[i][j]=='1'){
				if(p->chk_map[i+p->bound_p->start_row+p->info_p->row_times+1][j+p->bound_p->start_col+p->info_p->col_times]==1||p->bound_p->bot_row<i+p->bound_p->start_row+p->info_p->row_times+1||p->bound_p->left_edge>j+p->bound_p->start_col+p->info_p->col_times||p->bound_p->right_edge<j+p->bound_p->start_col+p->info_p->col_times)
				{
					not_touched=0;
				}
			}
		}
	}
	return not_touched;
}

//좌우 테두리에 닿는 지 체크
int reach_end_chk_by_ymove(int y, struct player *p){
	fflush(stdin);
	int not_touched=1;
	int i,j;
	for(i=0; i<4; ++i){
                for(j=0; j<8; ++j){
                        if(p->buf_pattern[i][j]=='1'){
                                if(p->chk_map[i+p->bound_p->start_row+p->info_p->row_times][j+p->bound_p->start_col+p->info_p->col_times+y]==1||p->bound_p->bot_row<i+p->bound_p->start_row+p->info_p->row_times||p->bound_p->left_edge>j+p->bound_p->start_col+p->info_p->col_times+y||p->bound_p->right_edge<j+p->bound_p->start_col+p->info_p->col_times+y)
                                {
                                        not_touched=0;
                                }
                        }
                }
        }
	return not_touched;
}

//게임이 끝났을 경우 출력하는 화면
void game_over_map(struct player *p){

	char GAME[4]="game";
	char OVER[4]="over";
	char buf_alpha[5][5];
	int fd_alpha= open("alpha.txt", O_RDONLY);
	int i,j,k;
	standout();
	char c=0;
	for(i=p->bound_p->bot_row; i>=0; --i){
		for(j=p->bound_p->left_edge; j<=p->bound_p->right_edge; ++j){
			move(i,j);
			addch(' ');
		}
	}
	refresh();
	standend();
	for(i=0; i<4; ++i){
		lseek(fd_alpha, (GAME[i]-'a')*25,SEEK_SET);
		read(fd_alpha, buf_alpha, sizeof(buf_alpha));
		for(j=0; j<5; ++j){
			for(k=0; k<5; ++k){
				if(buf_alpha[j][k]=='1'){
					move(p->bound_p->start_row+3+j, p->bound_p->left_edge+8+i*6+k);
					addch(' ');
				}
			}
		}
	}
	for(i=0; i<4; ++i){
                lseek(fd_alpha, (OVER[i]-'a')*25,SEEK_SET);
                read(fd_alpha, buf_alpha, sizeof(buf_alpha));
                for(j=0; j<5; ++j){
                        for(k=0; k<5; ++k){
                                if(buf_alpha[j][k]=='1'){
                                        move(p->bound_p->start_row+10+j, p->bound_p->left_edge+8+i*6+k);
                                        addch(' ');
                                }
                        }
                }
        }

	move(0,0);
	refresh();
	close(fd_alpha);
}
