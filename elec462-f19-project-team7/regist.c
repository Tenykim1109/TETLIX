#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<curses.h>
#include<sys/time.h>
#include<time.h>
#include<signal.h>
#include<fcntl.h>
#include"tetlix.h"
#define map_row		3
#define map_col		20
#define name_row	10
#define name_col	23
int position;
char temp[5][5];
char name[4]="name";
char score[5]="score";
struct record user_record; //플레이어 기록 저장을 위한 구조체
struct record temp_record;
int fd;
char temp_score[5][3]={0};
void init_map(){

	int i,j,k;
	int tempfd;

	fd = open("alpha.txt",O_RDONLY); //알파벳 변환을 위한 파일
	standout();
	for(i=0; i<4; ++i){
		lseek(fd, (name[i]-'a')*25, SEEK_SET);
		read(fd, temp, sizeof(char)*25);
		for(j=0; j<5; ++j){
			for(k=0; k<5; ++k){
				move(map_row+j, map_col+k+i*6);
				if(temp[j][k]=='1'){
					addch(' ');
				}
			}
		}
	}
	for(i=0; i<5; ++i){
		lseek(fd, (score[i]-'a')*25, SEEK_SET);
		read(fd, temp, sizeof(temp));
		for(j=0; j<5; ++j){
			for(k=0; k<5; ++k){
				move(map_row+j, map_col+k+i*6+35);
				if(temp[j][k]=='1'){
					addch(' ');
				}
			}
		}
	}
	tempfd=open("number.txt", O_RDONLY); //숫자 변환을 위한 파일
	for(i=0; i<5; ++i){
		lseek(tempfd, (user_record.user_score[i]-'0')*15, SEEK_SET);
		read(tempfd, temp_score,sizeof(temp_score));
		for(j=0; j<5; ++j){
			for(k=0; k<3; ++k){
				move(name_row+j, name_col+k+i*4+37);
				if(temp_score[j][k]=='1'){
					addch(' ');
				}
			}
		}
	}
	close(tempfd);
	standend();
	refresh();
}

void add(){ //입력받은 글자 변환
	int i,j;
	standout();
	for(i=0; i<5; ++i){
		for(j=0; j<5; ++j){
			move(name_row+i, name_col+j+position*6);
			if(temp[i][j]=='1'){
				addch(' ');
			}
		}
	}
	position++;
	move(0,0);
	refresh();
	standend();
}

void delete(){ //백스페이스 키 입력시 글자 삭제
	int i,j;
	standend();
	for(i=0; i<5; ++i){
		for(j=0; j<5; ++j){
			move(name_row+i, name_col+j+(position-1)*6);
			addch(' ');
		}
	}
	move(0,0);
	position--;
	refresh();
}

int main(int argc, char* argv[]){
	char c;
	int i;
	initscr();
	crmode();
	noecho();
	clear();
	refresh();
	strcpy(temp_record.user_name,"___");
	strcpy(temp_record.user_score,"00000");
	strcpy(user_record.user_score, argv[1]);
	init_map();
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	int chk=0;
	while(1){
		chk++;
		c=getch();
		if('a'<=c&&c<='z'){ //소문자 알파벳을 형식에 맞게 변환하여 화면에 출력
			lseek(fd, (c-'a')*25, SEEK_SET);
			read(fd, temp, sizeof(temp));
			user_record.user_name[position]=c;
			add();
		}else if(c==127){
			if(position>=0){
				delete();
				user_record.user_name[position+1]=0;
			}
		}else if(c=='\n'){
			if(position>=1)
			break;
		}

	}
	close(fd);
	int cnt=0;
	nocrmode();
	endwin();
	fd=open("rank.txt", O_RDWR);
	int test_fd = open("test.txt", O_WRONLY|O_CREAT, 0644);
	int cntt=0;
	strcpy(user_record.user_score, argv[1]);
	while(read(fd, &temp_record, sizeof(temp_record))!=0){
		cnt++;
		if(atoi(temp_record.user_score)<atoi(user_record.user_score)){ //내림차순 점수 정렬
			for(i=2; i<=11-cnt; ++i){
				lseek(fd, -sizeof(temp_record)*i-1,SEEK_END);
				read(fd, &temp_record, sizeof(temp_record));
				lseek(fd, -sizeof(temp_record)*(i-1)-1, SEEK_END);
				write(fd, &temp_record, sizeof(temp_record));
			}
			lseek(fd, -sizeof(temp_record)*(11-cnt)-1,SEEK_END);
			write(fd, &user_record, sizeof(user_record));
			cntt=1;
		}
		if(cnt==10||cntt==1)
			break;
	}

	close(fd);
	nocrmode();
	exit(1);
}
