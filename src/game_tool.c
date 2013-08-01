#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_GRAPH_H
#include "game_resource.h"

static void test_write(void)
{
	int i,class_id;
	LCUI_Graph graph[5];
	
	GameGraphRes_Init();
	srand( time(NULL) );
	for(i=0; i<5; ++i) {
		Graph_Init( &graph[i] );
		Graph_Create( &graph[i], 320, 240 );
		Graph_FillColor( &graph[i], RGB(rand()%255,rand()%255,rand()%255));
	}
	class_id = GameGraphRes_AddClass("main-resource");
	GameGraphRes_AddGraph( class_id, "test-01", &graph[0] );
	GameGraphRes_AddGraph( class_id, "test-02", &graph[1] );
	GameGraphRes_AddGraph( class_id, "test-03", &graph[2] );
	GameGraphRes_AddGraph( class_id, "test-04", &graph[3] );
	GameGraphRes_AddGraph( class_id, "test-05", &graph[4] );
	GameGraphRes_WriteToFile( "test.data", "main-resource" );
	GameGraphRes_FreeAll();
}

static void test_read(void)
{
	int i;
	LCUI_Graph graph[5];
	char filename[256];

	GameGraphRes_Init();
	GameGraphRes_LoadFromFile( "test.data" );
	GameGraphRes_GetGraph( "main-resource", "test-01", &graph[0] );
	GameGraphRes_GetGraph( "main-resource", "test-02", &graph[1] );
	GameGraphRes_GetGraph( "main-resource", "test-03", &graph[2] );
	GameGraphRes_GetGraph( "main-resource", "test-04", &graph[3] );
	GameGraphRes_GetGraph( "main-resource", "test-05", &graph[4] );
	for(i=0; i<5; ++i) {
		sprintf( filename, "test-%02d.png", i );
		Graph_WritePNG( filename, &graph[i] );
	}
}

#define ACTION_FILE_NUM 65
#define MAIN_FILE_NUM	2

struct fileinfo {
	char *filepath;
	char *name;
};

const struct fileinfo main_file_info[2]={
	{"LC-GAMES-175x128.png","main-logo"},
	{"shadow.png", "shadow"}
};

const struct fileinfo action_file_info[ACTION_FILE_NUM]={
	{"A-attack-01.png","A-attack-01"},
	{"A-attack-02.png","A-attack-02"},
	{"A-attack-03.png","A-attack-03"},
	{"A-sprint-attack-01.png","A-sprint-attack-01"},
	{"A-sprint-attack-02.png","A-sprint-attack-02"},
	{"B-attack-01.png","B-attack-01"},
	{"B-attack-02.png","B-attack-02"},
	{"B-sprint-attack-01.png","B-sprint-attack-01"},
	{"B-sprint-attack-02.png","B-sprint-attack-02"},
	{"back-be-catch.png","back-be-catch"},
	{"catch.png","catch"},
	{"defense.png","defense"},
	{"elbow-01.png","elbow-01"},
	{"elbow-02.png","elbow-02"},
	{"elbow-03.png","elbow-03"},
	{"elbow-04.png","elbow-04"},
	{"fall.png","fall"},
	{"final-blow-01.png","final-blow-01"},
	{"final-blow-02.png","final-blow-02"},
	{"final-blow-03.png","final-blow-03"},
	{"hit-fly.png","hit-fly"},
	{"hit.png","hit"},
	{"jump-elbow.png","jump-elbow"},
	{"jump-stomp.png","jump-stomp"},
	{"jump.png","jump"},
	{"lift-fall.png","lift-fall"},
	{"lift-jump.png","lift-jump"},
	{"lift-run-01.png","lift-run-01"},
	{"lift-run-02.png","lift-run-02"},
	{"lift-walk-01.png","lift-walk-01"},
	{"lift-walk-02.png","lift-walk-02"},
	{"lift-walk-03.png","lift-walk-03"},
	{"lift-walk-04.png","lift-walk-04"},
	{"lift.png","lift"},
	{"lying-hit.png","lying-hit"},
	{"lying.png","lying"},
	{"pull.png","pull"},
	{"push.png","push"},
	{"ready-01.png","ready-01"},
	{"rest-01.png","rest-01"},
	{"rest-02.png","rest-02"},
	{"roll-01.png","roll-01"},
	{"roll-02.png","roll-02"},
	{"roll-03.png","roll-03"},
	{"roll-04.png","roll-04"},
	{"roll-05.png","roll-05"},
	{"roll-06.png","roll-06"},
	{"roll-07.png","roll-07"},
	{"roll-08.png","roll-08"},
	{"run-01.png","run-01"},
	{"run-02.png","run-02"},
	{"run-03.png","run-03"},
	{"run-04.png","run-04"},
	{"run-05.png","run-05"},
	{"run-06.png","run-06"},
	{"squat.png","squat"},
	{"stance-01.png","stance-01"},
	{"tummy-hit.png","tummy-hit"},
	{"tummy.png","tummy"},
	{"walk-01.png","walk-01"},
	{"walk-02.png","walk-02"},
	{"walk-03.png","walk-03"},
	{"walk-04.png","walk-04"},
	{"weak-walk-01.png","weak-walk-01"},
	{"weak-walk-02.png","weak-walk-02"}
};

static void ActionRes_Riki_WirteToFile( void )
{
	int i, class_id;
	LCUI_Graph graph_buff;

	GameGraphRes_Init();
	class_id = GameGraphRes_AddClass(ACTION_RES_CLASS_RIKI);
	for(i=0; i<ACTION_FILE_NUM; ++i) {
		Graph_Init( &graph_buff );
		Graph_LoadImage( action_file_info[i].filepath, &graph_buff );
		GameGraphRes_AddGraph( class_id, action_file_info[i].name, &graph_buff );
	}
	GameGraphRes_WriteToFile( "action-riki.data", ACTION_RES_CLASS_RIKI );
	GameGraphRes_FreeAll();
}

static void MainGraphRes_WirteToFile( void )
{
	int i, class_id;
	LCUI_Graph graph_buff;

	GameGraphRes_Init();
	class_id = GameGraphRes_AddClass( MAIN_RES );
	for(i=0; i<MAIN_FILE_NUM; ++i) {
		Graph_Init( &graph_buff );
		Graph_LoadImage( main_file_info[i].filepath, &graph_buff );
		GameGraphRes_AddGraph( class_id, main_file_info[i].name, &graph_buff );
	}
	GameGraphRes_WriteToFile( "main.data", MAIN_RES );
	GameGraphRes_FreeAll();
}

static void ActionRes_Riki_ReadFromFile(void)
{
	int i;
	LCUI_Graph graph_buff;

	GameGraphRes_Init();
	GameGraphRes_LoadFromFile( "action-riki.data" );
	for(i=0; i<ACTION_FILE_NUM; ++i) {
		Graph_Init( &graph_buff );
		GameGraphRes_GetGraph( 
			ACTION_RES_CLASS_RIKI, 
			action_file_info[i].name,
			&graph_buff 
		);
		Graph_WritePNG( action_file_info[i].filepath, &graph_buff );
		_DEBUG_MSG("[%d/%d]: %s\n", i, ACTION_FILE_NUM, action_file_info[i].filepath);
	}
	GameGraphRes_FreeAll();
}

int main(int argc, char** argv)
{
#ifdef TEST
	if( argc == 2 ) {
		test_read();
	} else {
		test_write();
	}
#endif
	if( argc == 2 ) {
		ActionRes_Riki_ReadFromFile();
	} else {
		//ActionRes_Riki_WirteToFile();
		MainGraphRes_WirteToFile();
	}
	return 0;
}
