#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_GRAPH_H
#include "game_object.h"
#include "game_resource.h"

#define MAIN_RES_NUM 8

#define KUNI_ACTION_FILE_NUM	70
#define RIKI_ACTION_FILE_NUM	75
#define MIKE_ACTION_FILE_NUM	72
#define BEN_ACTION_FILE_NUM	76
#define TORAJI_ACTION_FILE_NUM	75
#define MAIN_FILE_NUM		2
#define FONT_FILE_NUM		31

struct fileinfo {
	char *filepath;
	char *name;
};

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

const struct fileinfo font_file_info[FONT_FILE_NUM]={
	{"font-0.png","0"},
	{"font-1.png","1"},
	{"font-2.png","2"},
	{"font-3.png","3"},
	{"font-4.png","4"},
	{"font-5.png","5"},
	{"font-6.png","6"},
	{"font-7.png","7"},
	{"font-8.png","8"},
	{"font-9.png","9"},
	{"font-x.png","x"},
	{"red-font-digital-0.png","red-0"},
	{"red-font-digital-1.png","red-1"},
	{"red-font-digital-2.png","red-2"},
	{"red-font-digital-3.png","red-3"},
	{"red-font-digital-4.png","red-4"},
	{"red-font-digital-5.png","red-5"},
	{"red-font-digital-6.png","red-6"},
	{"red-font-digital-7.png","red-7"},
	{"red-font-digital-8.png","red-8"},
	{"red-font-digital-9.png","red-9"},
	{"green-font-digital-0.png","red-0"},
	{"green-font-digital-1.png","red-1"},
	{"green-font-digital-2.png","red-2"},
	{"green-font-digital-3.png","red-3"},
	{"green-font-digital-4.png","red-4"},
	{"green-font-digital-5.png","red-5"},
	{"green-font-digital-6.png","red-6"},
	{"green-font-digital-7.png","red-7"},
	{"green-font-digital-8.png","red-8"},
	{"green-font-digital-9.png","red-9"}
};

const struct fileinfo main_file_info[MAIN_RES_NUM]={
	{"wave1.png","front-wave-img"},
	{"wave2.png","back-wave-img"},
	{"LC-GAMES-175x128.png","main-logo"},
	{"game-icon-64x64.png", "game-logo"},
	{"qrcode.png","qrcode"},
	{"bg.png", "background-image"},
	{"avatar-unknown.png","avatar-unknown"},
	{"shadow.png", "shadow"}
};

const struct fileinfo scenes_file_info={
	"default-scene.png","default"
};

const struct fileinfo kuni_action_file_info[KUNI_ACTION_FILE_NUM]={
	{"start-01.png", "start-01"},
	{"start-02.png", "start-02"},
	{"ready.png","ready"},
	{"stance.png","stance"},
	{"A-attack-01.png","A-attack-01"},
	{"A-attack-02.png","A-attack-02"},
	{"A-sprint-attack.png","A-sprint-attack"},
	{"A-sprint-jump-attack.png","A-sprint-jump-attack"},
	{"B-attack-01.png","B-attack-01"},
	{"B-attack-02.png","B-attack-02"},
	{"B-sprint-attack.png","B-sprint-attack"},
	{"back-be-catch.png","back-be-catch"},
	{"kick.png","kick"},
	{"catch.png","catch"},
	{"defense.png","defense"},
	{"defense++.png","defense++"},
	{"fall.png","fall"},
	{"final-blow-01.png","final-blow-01"},
	{"final-blow-02.png","final-blow-02"},
	{"final-blow-03.png","final-blow-03"},
	{"final-blow-04.png","final-blow-04"},
	{"hit.png","hit"},
	{"hit-fly.png","hit-fly"},
	{"hit-fly-fall.png","hit-fly-fall"},
	{"jump-elbow.png","jump-elbow"},
	{"squat.png","squat"},
	{"jump.png","jump"},
	{"lift-fall.png","lift-fall"},
	{"lift-jump.png","lift-jump"},
	{"lift-run.png","lift-run"},
	{"lift-walk-01.png","lift-walk-01"},
	{"lift-walk-02.png","lift-walk-02"},
	{"lift.png","lift"},
	{"tummy.png","tummy"},
	{"tummy-hit.png","tummy-hit"},
	{"tummy-hit-fly.png","tummy-hit-fly"},
	{"lying.png","lying"},
	{"lying-hit.png","lying-hit"},
	{"half-lying.png","half-lying"},
	{"pull.png","pull"},
	{"push.png","push"},
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
	{"walk-01.png","walk-01"},
	{"walk-03.png","walk-03"},
	{"weak-walk.png","weak-walk"},
	{"ride.png","ride"},
	{"ride-attack-01.png","ride-attack-01"},
	{"ride-attack-02.png","ride-attack-02"},
	{"be-push.png", "be-push"},
	{"hug-front-put-01.png", "hug-front-put-01"},
	{"hug-front-put-02.png", "hug-front-put-02"},
	{"hug-front-put-03.png", "hug-front-put-03"},
	{"hug-front-put-04.png", "hug-front-put-04"},
	{"half-stance.png", "half-stance"},
	{"sit.png", "sit"}
};

const struct fileinfo riki_action_file_info[RIKI_ACTION_FILE_NUM]={
	{"start-01.png", "start-01"},
	{"start-02.png", "start-02"},
	{"A-attack-01.png","A-attack-01"},
	{"A-attack-02.png","A-attack-02"},
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
	{"half-lying.png","half-lying"},
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
	{"tummy-hit-fly.png","tummy-hit-fly"},
	{"tummy.png","tummy"},
	{"walk-01.png","walk-01"},
	{"walk-02.png","walk-02"},
	{"walk-03.png","walk-03"},
	{"walk-04.png","walk-04"},
	{"weak-walk-01.png","weak-walk-01"},
	{"weak-walk-02.png","weak-walk-02"},
	{"ride.png","ride"},
	{"ride-attack-01.png","ride-attack-01"},
	{"ride-attack-02.png","ride-attack-02"},
	{"be-push.png", "be-push"},
	{"defense++.png","defense++"},
	{"half-stance.png", "half-stance"},
	{"sit.png", "sit"}
};

const struct fileinfo mike_action_file_info[MIKE_ACTION_FILE_NUM]={
	{"start-01.png", "start-01"},
	{"start-02.png", "start-02"},
	{"ready.png","ready"},
	{"stance.png","stance"},
	{"A-attack-01.png","A-attack-01"},
	{"A-attack-02.png","A-attack-02"},
	{"A-sprint-attack-01.png","A-sprint-attack-01"},
	{"A-sprint-attack-02.png","A-sprint-attack-02"},
	{"B-attack-01.png","B-attack-01"},
	{"B-attack-02.png","B-attack-02"},
	{"B-sprint-attack-01.png","B-sprint-attack-01"},
	{"B-sprint-attack-02.png","B-sprint-attack-02"},
	{"back-be-catch.png","back-be-catch"},
	{"kick.png","kick"},
	{"catch.png","catch"},
	{"defense.png","defense"},
	{"defense++.png","defense++"},
	{"fall.png","fall"},
	{"final-blow-01.png","final-blow-01"},
	{"final-blow-02.png","final-blow-02"},
	{"final-blow-03.png","final-blow-03"},
	{"final-blow-04.png","final-blow-04"},
	{"final-blow-05.png","final-blow-05"},
	{"hit.png","hit"},
	{"hit-fly.png","hit-fly"},
	{"hit-fly-fall.png","hit-fly-fall"},
	{"jump-elbow.png","jump-elbow"},
	{"squat.png","squat"},
	{"jump.png","jump"},
	{"lift-fall.png","lift-fall"},
	{"lift-jump.png","lift-jump"},
	{"lift-run.png","lift-run"},
	{"lift-walk-01.png","lift-walk-01"},
	{"lift-walk-02.png","lift-walk-02"},
	{"lift.png","lift"},
	{"tummy.png","tummy"},
	{"tummy-hit.png","tummy-hit"},
	{"tummy-hit-fly.png","tummy-hit-fly"},
	{"lying.png","lying"},
	{"lying-hit.png","lying-hit"},
	{"half-lying.png","half-lying"},
	{"pull.png","pull"},
	{"push.png","push"},
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
	{"walk-01.png","walk-01"},
	{"walk-02.png","walk-02"},
	{"weak-walk.png","weak-walk"},
	{"ride.png","ride"},
	{"ride-attack-01.png","ride-attack-01"},
	{"ride-attack-02.png","ride-attack-02"},
	{"be-push.png", "be-push"},
	{"half-stance.png", "half-stance"},
	{"hug-jump-01.png", "hug-jump-01"},
	{"hug-jump-02.png", "hug-jump-02"},
	{"hug-jump-03.png", "hug-jump-03"},
	{"hug-jump-04.png", "hug-jump-04"},
	{"sit.png", "sit"}
};

const struct fileinfo toraji_action_file_info[TORAJI_ACTION_FILE_NUM]={
	{"start-01.png", "start-01"},
	{"start-02.png", "start-02"},
	{"ready.png","ready"},
	{"stance.png","stance"},
	{"half-lying.png","half-lying"},
	{"walk-01.png","walk-01"},
	{"walk-02.png","walk-02"},
	{"walk-03.png","walk-03"},
	{"walk-04.png","walk-04"},
	{"run-01.png","run-01"},
	{"run-02.png","run-02"},
	{"run-03.png","run-03"},
	{"run-04.png","run-04"},
	{"run-05.png","run-05"},
	{"run-06.png","run-06"},
	{"A-attack-01.png","A-attack-01"},
	{"A-attack-02.png","A-attack-02"},
	{"B-attack-01.png","B-attack-01"},
	{"B-attack-02.png","B-attack-02"},
	{"A-sprint-attack-01.png","A-sprint-attack-01"},
	{"A-sprint-attack-02.png","A-sprint-attack-02"},
	{"A-sprint-attack-03.png","A-sprint-attack-03"},
	{"A-sprint-attack-04.png","A-sprint-attack-04"},
	{"A-sprint-attack-05.png","A-sprint-attack-05"},
	{"A-sprint-attack-06.png","A-sprint-attack-06"},
	{"jump.png","jump"},
	{"squat.png","squat"},
	{"defense.png","defense"},
	{"hit-fly-fall.png","hit-fly-fall"},
	{"final-blow-01.png","final-blow-01"},
	{"final-blow-02.png","final-blow-02"},
	{"hit-fly.png","hit-fly"},
	{"hit.png","hit"},
	{"lying-hit.png","lying-hit"},
	{"lying.png","lying"},
	{"tummy.png","tummy"},
	{"tummy-hit.png","tummy-hit"},
	{"tummy-hit-fly.png","tummy-hit-fly"},
	{"stomp.png","stomp"},
	{"kick.png","kick"},
	{"roll-01.png","roll-01"},
	{"roll-02.png","roll-02"},
	{"roll-03.png","roll-03"},
	{"roll-04.png","roll-04"},
	{"roll-05.png","roll-05"},
	{"roll-06.png","roll-06"},
	{"roll-07.png","roll-07"},
	{"roll-08.png","roll-08"},
	{"spin-up-01.png","spin-up-01"},
	{"spin-up-02.png","spin-up-02"},
	{"spin-up-03.png","spin-up-03"},
	{"catch.png","catch"},
	{"B-sprint-attack.png","B-sprint-attack"},
	{"rest-01.png","rest-01"},
	{"rest-02.png","rest-02"},
	{"push.png","push"},
	{"pull.png","pull"},
	{"back-be-catch.png","back-be-catch"},
	{"weak-walk.png","weak-walk"},
	{"fall.png","fall"},
	{"defense++.png","defense++"},
	{"ride.png","ride"},
	{"ride-attack-01.png","ride-attack-01"},
	{"ride-attack-02.png","ride-attack-02"},
	{"lift-fall.png","lift-fall"},
	{"lift.png","lift"},
	{"lift-jump.png","lift-jump"},
	{"lift-run-01.png","lift-run-01"},
	{"lift-walk-01.png","lift-walk-01"},
	{"lift-walk-02.png","lift-walk-02"},
	{"lift-walk-03.png","lift-walk-03"},
	{"sit.png","sit"},
	{"jump-elbow.png","jump-elbow"},
	{"half-stance.png", "half-stance"},
	{"be-push.png","be-push"}
};

const struct fileinfo ben_action_file_info[BEN_ACTION_FILE_NUM]={
	{"start-01.png", "start-01"},
	{"start-02.png", "start-02"},
	{"ready.png","ready"},
	{"stance.png","stance"},
	{"A-attack-01.png","A-attack-01"},
	{"A-attack-02.png","A-attack-02"},
	{"B-attack-01.png","B-attack-01"},
	{"B-attack-02.png","B-attack-02"},
	{"B-attack-03.png","B-attack-03"},
	{"back-be-catch-01.png","back-be-catch-01"},
	{"back-be-catch-02.png","back-be-catch-02"},
	{"kick.png","kick"},
	{"catch.png","catch"},
	{"defense.png","defense"},
	{"defense++.png","defense++"},
	{"final-blow-01.png","final-blow-01"},
	{"final-blow-02.png","final-blow-02"},
	{"final-blow-03.png","final-blow-03"},
	{"final-blow-04.png","final-blow-04"},
	{"final-blow-05.png","final-blow-05"},
	{"final-blow-06.png","final-blow-06"},
	{"hit.png","hit"},
	{"hit-fly.png","hit-fly"},
	{"hit-fly-fall.png","hit-fly-fall"},
	{"jump-elbow.png","jump-elbow"},
	{"squat.png","squat"},
	{"jump.png","jump"},
	{"fall.png","fall"},
	{"lift-fall.png","lift-fall"},
	{"lift-jump.png","lift-jump"},
	{"lift-run.png","lift-run"},
	{"lift-walk-01.png","lift-walk-01"},
	{"lift-walk-02.png","lift-walk-02"},
	{"lift.png","lift"},
	{"tummy.png","tummy"},
	{"tummy-hit.png","tummy-hit"},
	{"tummy-hit-fly.png","tummy-hit-fly"},
	{"lying.png","lying"},
	{"lying-hit.png","lying-hit"},
	{"half-lying.png","half-lying"},
	{"half-stance.png", "half-stance"},
	{"pull.png","pull"},
	{"push.png","push"},
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
	{"walk-01.png","walk-01"},
	{"walk-02.png","walk-02"},
	{"weak-walk.png","weak-walk"},
	{"ride.png","ride"},
	{"ride-attack-01.png","ride-attack-01"},
	{"ride-attack-02.png","ride-attack-02"},
	{"be-push.png", "be-push"},
	{"sit.png", "sit"},
	{"back-judo-01.png","back-judo-01"},
	{"back-judo-02.png","back-judo-02"},
	{"back-judo-03.png","back-judo-03"},
	{"back-judo-04.png","back-judo-04"},
	{"back-judo-05.png","back-judo-05"},
	{"judo-01.png","judo-01"},
	{"judo-02.png","judo-02"},
	{"judo-03.png","judo-03"},
	{"judo-04.png","judo-04"}
};

static void GraphRes_WirteToFile(	const char *class_name,
					const struct fileinfo *filelist,
					int n_file,
					const char *outputfile )
{
	int i, class_id;
	LCUI_Graph graph_buff;

	GameGraphRes_Init();
	class_id = GameGraphRes_AddClass(class_name);
	for(i=0; i<n_file; ++i) {
		Graph_Init( &graph_buff );
		Graph_LoadImage( filelist[i].filepath, &graph_buff );
		GameGraphRes_AddGraph( class_id, filelist[i].name, &graph_buff );
	}
	GameGraphRes_WriteToFile( outputfile, class_name );
	GameGraphRes_FreeAll();
}

static void ActionRes_Toraji_ReadFromFile(void)
{
	int i;
	LCUI_Graph graph_buff;

	GameGraphRes_Init();
	GameGraphRes_LoadFromFile( "action-toraji.data" );
	for(i=0; i<TORAJI_ACTION_FILE_NUM; ++i) {
		Graph_Init( &graph_buff );
		GameGraphRes_GetGraph( 
			ACTION_RES_CLASS_TORAJI, 
			toraji_action_file_info[i].name,
			&graph_buff 
		);
		Graph_WritePNG( toraji_action_file_info[i].filepath, &graph_buff );
		_DEBUG_MSG("[%d/%d]: %s\n", i, TORAJI_ACTION_FILE_NUM, toraji_action_file_info[i].filepath);
	}
	GameGraphRes_FreeAll();
}

static void MainGraphRes_ReadFromFile(void)
{
	int i;
	LCUI_Graph graph_buff;

	GameGraphRes_Init();
	GameGraphRes_LoadFromFile( "main.data" );
	for(i=0; i<5; ++i) {
		Graph_Init( &graph_buff );
		GameGraphRes_GetGraph( 
			MAIN_RES, 
			main_file_info[i].name,
			&graph_buff 
		);
		Graph_WritePNG( main_file_info[i].filepath, &graph_buff );
	}
	GameGraphRes_FreeAll();
}

/** 将战斗场景的图形资源写入至文件 */
static void ScenesGraphRes_WirteToFile( void )
{
	int class_id;
	LCUI_Graph graph_buff;

	GameGraphRes_Init();
	class_id = GameGraphRes_AddClass( SCENES_RES );
	Graph_Init( &graph_buff );
	Graph_LoadImage( scenes_file_info.filepath, &graph_buff );
	GameGraphRes_AddGraph( class_id, scenes_file_info.name, &graph_buff );
	GameGraphRes_WriteToFile( "scenes.data", SCENES_RES );
	GameGraphRes_FreeAll();
}

/** 将字体的图形资源写入至文件 */
static void FontGraphRes_WirteToFile( void )
{
	int class_id, i;
	LCUI_Graph graph_buff;

	GameGraphRes_Init();
	class_id = GameGraphRes_AddClass( FONT_RES );
	for( i=0; i<FONT_FILE_NUM; ++i ) {
		Graph_Init( &graph_buff );
		Graph_LoadImage( font_file_info[i].filepath, &graph_buff );
		GameGraphRes_AddGraph( class_id, font_file_info[i].name, &graph_buff );
	}
	GameGraphRes_WriteToFile( "font.data", FONT_RES );
	GameGraphRes_FreeAll();
}

static void ActionRes_Riki_ReadFromFile(void)
{
	int i;
	LCUI_Graph graph_buff;

	GameGraphRes_Init();
	GameGraphRes_LoadFromFile( "action-riki.data" );
	for(i=0; i<RIKI_ACTION_FILE_NUM; ++i) {
		Graph_Init( &graph_buff );
		GameGraphRes_GetGraph( 
			ACTION_RES_CLASS_RIKI, 
			riki_action_file_info[i].name,
			&graph_buff 
		);
		Graph_WritePNG( riki_action_file_info[i].filepath, &graph_buff );
		_DEBUG_MSG("[%d/%d]: %s\n", i, RIKI_ACTION_FILE_NUM, riki_action_file_info[i].filepath);
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
		//ActionRes_Kuni_ReadFromFile();
		//ActionRes_Riki_ReadFromFile();
		//ActionRes_Toraji_ReadFromFile();
		return 0;
	}
	//FontGraphRes_WirteToFile();
	//ScenesGraphRes_WirteToFile();
	//MainGraphRes_WirteToFile();
#define need_main_res
#ifdef need_font
	GraphRes_WirteToFile(	FONT_RES,
				font_file_info,
				FONT_FILE_NUM, 
				"font.data" );
#elif defined(need_main_res)
	//MainGraphRes_ReadFromFile();
	GraphRes_WirteToFile(	MAIN_RES,
				main_file_info,
				MAIN_RES_NUM, 
				"main.data" );
#elif defined(need_kuni)
	GraphRes_WirteToFile(	ACTION_RES_CLASS_KUNI,
				kuni_action_file_info,
				KUNI_ACTION_FILE_NUM, 
				"action-kuni.data" );
#elif defined(need_riki)
	GraphRes_WirteToFile(	ACTION_RES_CLASS_RIKI,
				riki_action_file_info,
				RIKI_ACTION_FILE_NUM, 
				"action-riki.data" );
#elif defined(need_toraji)
	GraphRes_WirteToFile(	ACTION_RES_CLASS_TORAJI,
				toraji_action_file_info,
				TORAJI_ACTION_FILE_NUM, 
				"action-toraji.data" );
#elif defined(need_mike)
	GraphRes_WirteToFile(	ACTION_RES_CLASS_MIKE,
				mike_action_file_info,
				MIKE_ACTION_FILE_NUM, 
				"action-mike.data" );
#elif defined(need_ben)
	GraphRes_WirteToFile(	ACTION_RES_CLASS_BEN,
				ben_action_file_info,
				BEN_ACTION_FILE_NUM, 
				"action-ben.data" );
#endif
	return 0;
}
