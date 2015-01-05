/* ****************************************************************************
* Punch And Kick -- a simple 2D Fighting Game.
*
* Copyright (C) 2013 by
* Liu Chao
*
* This file is part of the Punch And Kick project, and may only be used,
* modified, and distributed under the terms of the GPLv2.
*
* (GPLv2 is abbreviation of GNU General Public License Version 2)
*
* By continuing to use, modify, or distribute this file you indicate that you
* have read the license and understand and accept it fully.
*
* The Punch And Kick project is distributed in the hope that it will be
* useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GPL v2 for
* more details.
*
* You should have received a copy of the GPLv2 along with this file. It is
* usually in the LICENSE.TXT file, If not, see <http://www.gnu.org/licenses/>.
* ***************************************************************************/

/* ****************************************************************************
* Punch And Kick -- 一个普通的2D格斗游戏
*
* 版权所有 (C) 2013 归属于
* 刘超
*
* 这个文件是 Punch And Kick 项目的一部分，并且只可以根据GPLv2许可协议来使用、
* 更改和发布。
*
* (GPLv2 是 GNU通用公共许可证第二版 的英文缩写)
*
* 继续使用、修改或发布本文件，表明您已经阅读并完全理解和接受这个许可协议。
*
* Punch And Kick 项目是基于技术交流目的而加以散布的，但不负任何担保责任，例
* 如：稳定性、可玩性、趣味性等的担保，甚至没有适销性或特定用途的隐含担保，详
* 情请参照GPLv2许可协议。
*
* 您应已收到附随于本文件的GPLv2许可协议的副本，它通常在LICENSE.TXT文件中，如果
* 没有，请查看：<http://www.gnu.org/licenses/>.
* ***************************************************************************/

// 资源打包/解包工具

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_GRAPH_H
#include "game_object.h"
#include "game_resource.h"

#define MAIN_RES_NUM 7

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

/** 测试写入 .data 文件 */
static void test_write(void)
{
	int i,class_id;
	LCUI_Graph graph[5];
	
	GameGraphRes_Init();
	srand( (unsigned int)time(NULL) );
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

/** 测试读取 .data 文件 */
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

/** 一堆资源文件信息，格式为：文件名  资源名 */
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

/** 打包资源至 .data 文件 */
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

/** 解包 .data 文件中的资源 */
static void ResourceTool_UnPackDataFile(	const char *data_file, 
						const char *class_name,
						const struct fileinfo *list,
						int n_files )
{
	int i, dir_code;
	LCUI_Graph graph_buff;
	char filepath[512], cmd[256];

	GameGraphRes_Init();
	GameGraphRes_LoadFromFile( data_file );
	/* 
	 * 类名中会包含一些非法字符，如： : / \ * " ' ? 等符号，目录/文件名不能有这些
	 * 符号，为了能够正常创建目录，将类名字符串内容转换为数字字符
	 */
	dir_code = BKDRHash( class_name );
	/* 生成所需命令行，以创建文件目录 */
	sprintf(cmd, "mkdir %d", dir_code);
	/* 执行命令行 */
	system(cmd);
	/* 遍历每个图像资源 */
	for(i=0; i<n_files; ++i) {
		Graph_Init( &graph_buff );
		/* 获取相应名称的图像资源 */
		GameGraphRes_GetGraph( class_name, list[i].name, &graph_buff );
		/* 生成图像输出文件路径 */
		sprintf( filepath, "%d/%s", dir_code, list[i].filepath );
		/* 写入至输出文件 */
		Graph_WritePNG( filepath, &graph_buff );
		_DEBUG_MSG("[%d/%d]: %s\n", i, n_files, filepath);
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
	/* 当有两个参数时，解包所有.data文件 */
	if( argc >= 2 ) {
		ResourceTool_UnPackDataFile(
			"font.data", FONT_RES, font_file_info, FONT_FILE_NUM
		);
		ResourceTool_UnPackDataFile(
			"scenes.data", SCENES_RES, &scenes_file_info, 1 
		);
		ResourceTool_UnPackDataFile(
			"main.data", MAIN_RES, main_file_info, MAIN_FILE_NUM
		);
		ResourceTool_UnPackDataFile(
			"action-riki.data", ACTION_RES_CLASS_RIKI, 
			riki_action_file_info, RIKI_ACTION_FILE_NUM
		);
		ResourceTool_UnPackDataFile(
			"action-ben.data", ACTION_RES_CLASS_BEN, 
			ben_action_file_info, BEN_ACTION_FILE_NUM
		);
		ResourceTool_UnPackDataFile(
			"action-toraji.data", ACTION_RES_CLASS_TORAJI,
			toraji_action_file_info, TORAJI_ACTION_FILE_NUM 
		);
		ResourceTool_UnPackDataFile(
			"action-kuni.data", ACTION_RES_CLASS_KUNI, 
			kuni_action_file_info, KUNI_ACTION_FILE_NUM 
		);
		ResourceTool_UnPackDataFile(
			"action-mike.data", ACTION_RES_CLASS_MIKE,
			mike_action_file_info, MIKE_ACTION_FILE_NUM
		);
		return 0;
	}
	/* 不带参数运行本程序时，重新将当前目录下的相应文件打包成.data文件 */
#ifdef need_font
	GraphRes_WirteToFile( 
		FONT_RES,font_file_info,FONT_FILE_NUM, "font.data" 
	);
#elif defined(need_main_res)
	GraphRes_WirteToFile(
		MAIN_RES, main_file_info, MAIN_RES_NUM, "main.data"
	);
#elif defined(need_kuni)
	GraphRes_WirteToFile(
		ACTION_RES_CLASS_KUNI, kuni_action_file_info,
		KUNI_ACTION_FILE_NUM, "action-kuni.data"
	);
#elif defined(need_riki)
	GraphRes_WirteToFile(
		ACTION_RES_CLASS_RIKI, riki_action_file_info,
		RIKI_ACTION_FILE_NUM, "action-riki.data"
	);
#elif defined(need_toraji)
	GraphRes_WirteToFile(
		ACTION_RES_CLASS_TORAJI, toraji_action_file_info,
		TORAJI_ACTION_FILE_NUM, "action-toraji.data" 
	);
#elif defined(need_mike)
	GraphRes_WirteToFile(
		ACTION_RES_CLASS_MIKE, mike_action_file_info,
		MIKE_ACTION_FILE_NUM, "action-mike.data"
	);
#elif defined(need_ben)
	GraphRes_WirteToFile(
		ACTION_RES_CLASS_BEN, ben_action_file_info,
		BEN_ACTION_FILE_NUM, "action-ben.data"
	);
#endif
	return 0;
}
