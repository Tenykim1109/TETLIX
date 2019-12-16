struct record{
	char user_name[3];
	char user_score[5];
}; //플레이어 스코어 기록

struct now_info{
	int row_times;
	int col_times;
	int now_pattern;
	int game_over;
	int score;
	int now_step;
}; //현재 게임 진행 정보 기록

struct bound{
	int start_row;
	int start_col;
	int left_edge;
	int right_edge;
	int bot_row;

	int score_row;
	int score_col;
	int row_chk[26];
}; //게임 화면의 테두리 설정을 위한 구조체

struct player{
	struct now_info *info_p;
	struct bound *bound_p;
	struct record *record_p;

	char buf_score[5][3];
	char buf_pattern[4][8];
	int chk_map[26][300];
	int cnt;
	int times;
}; //플레이어 정보를 담고 있는 구조체
