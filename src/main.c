//#define SKIP_BOOT_SCREEN
#define I_NEED_WINMAIN
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_CURSOR_H
#include LC_WIDGET_H
#include LC_DISPLAY_H
#include LC_LABEL_H
#include LC_INPUT_H

#include <time.h>
#include <stdlib.h>

#include "game.h"
#include "main_menu.h"
#include "skills/game_skill.h"
#include "game_role_select.h"
#include "game_menubtn.h"
#include "game_menu.h"
#include "game_config.h"

#define TEXT_COPYRIGHT_EN	L"Developed by LC-Games, Copyright © 2013 LC-Games, All Rights Reserved."
#define TEXT_COPYRIGHT_CN	L"本游戏由 LC-Games 开发 , LC-Games 保留所有权利。"

static int main_battle_id;
static LCUI_Thread main_thread_id;
static LCUI_Widget *player_status_area;

/** 在屏幕上显示游戏启动时的画面 */
static void Game_ShowBootScreen(void)
{
	uchar_t alpha;
	LCUI_Graph img_game_logo;
	LCUI_Widget *wdg_img_logo;
	LCUI_Widget *wdg_copyright_cn_text, *wdg_copyright_en_text;

	wdg_img_logo = Widget_New(NULL);
	wdg_copyright_cn_text = Widget_New("label");
	wdg_copyright_en_text = Widget_New("label");
	
	Graph_Init( &img_game_logo );
	GameGraphRes_GetGraph(MAIN_RES, "main-logo", &img_game_logo );
	
	Label_TextW( wdg_copyright_en_text, TEXT_COPYRIGHT_EN );
	Label_TextW( wdg_copyright_cn_text, TEXT_COPYRIGHT_CN );

	Widget_Resize( wdg_img_logo, Graph_GetSize(&img_game_logo) );
	
	Widget_SetAlign( wdg_copyright_cn_text, ALIGN_BOTTOM_CENTER, Pos(0,-20) );
	Widget_SetAlign( wdg_copyright_en_text, ALIGN_BOTTOM_CENTER, Pos(0,-40) );
	Widget_SetAlign( wdg_img_logo, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	
	Widget_SetBackgroundImage( wdg_img_logo, &img_game_logo );
	Widget_SetBackgroundLayout( wdg_img_logo, LAYOUT_CENTER );
	
	Widget_SetAlpha( wdg_img_logo, 0 );
	Widget_SetAlpha( wdg_copyright_cn_text, 0 );
	Widget_SetAlpha( wdg_copyright_en_text, 0 );

	Widget_Show( wdg_img_logo );
	Widget_Show( wdg_copyright_en_text );
	Widget_Show( wdg_copyright_cn_text );
#ifndef SKIP_BOOT_SCREEN
	/* 以淡入淡出的效果显示LOGO */
	for( alpha=0; alpha<250; alpha+=10 ) {
		Widget_SetAlpha( wdg_img_logo, alpha );
		Widget_SetAlpha( wdg_copyright_en_text, alpha );
		Widget_SetAlpha( wdg_copyright_cn_text, alpha );
		LCUI_MSleep(25);
	}
	Widget_SetAlpha( wdg_img_logo, 255 );
	Widget_SetAlpha( wdg_copyright_cn_text, 255 );
	Widget_SetAlpha( wdg_copyright_en_text, 255 );
	LCUI_Sleep(2);
	for( alpha=255; alpha>5; alpha-=10 ) {
		Widget_SetAlpha( wdg_img_logo, alpha );
		Widget_SetAlpha( wdg_copyright_en_text, alpha );
		Widget_SetAlpha( wdg_copyright_cn_text, alpha );
		LCUI_MSleep(25);
	}
#endif
	Widget_SetAlpha( wdg_img_logo, 0 );
	Widget_SetAlpha( wdg_copyright_cn_text, 0 );
	Widget_SetAlpha( wdg_copyright_en_text, 0 );
	Widget_Hide( wdg_img_logo );
	Widget_Hide( wdg_copyright_en_text );
	Widget_Hide( wdg_copyright_cn_text );

	Widget_Destroy( wdg_img_logo );
	Widget_Destroy( wdg_copyright_en_text );
	Widget_Destroy( wdg_copyright_cn_text );
}

/** 加入CPU玩家角色 */
static void Game_AddCPUPlayerRole( int battle_id, int role_id[4] )
{
	int i, j;

	for(j=2; j<5; ++j) {
		while(1) {
			/* 随机选择一个角色 */
			role_id[j-1] = rand()%TOTAL_ROLE_NUM;
			/* 检查是否与前面几个角色的ID相同 */
			for(i=1; i<j; ++i) {
				if( role_id[j-1] == role_id[i-1] ) {
					break;
				}
			}
			if( i>=j ) {
				break;
			}
		}
	}
}

static void GameDemoScene_PutPlayer( int battle_id )
{
	int x, y;
	LCUI_Pos start_pos;
	LCUI_Size scene_size;
	GamePlayer *player[2];
	
	player[0] = GameBattle_GetPlayer( battle_id, 1 );
	player[1] = GameBattle_GetPlayer( battle_id, 2 );
	//GameObject_SetFillColor( player[0]->object, TRUE, RGB(0,0,0) );
	//GameObject_SetFillColor( player[1]->object, TRUE, RGB(0,0,0) );
	//GameObject_SetShadowVisible( player[0]->object, FALSE );
	//GameObject_SetShadowVisible( player[1]->object, FALSE );
	GameBattle_GetSceneLand( battle_id, &start_pos, &scene_size );
	x = scene_size.w/2 - 150;
	y = scene_size.h/2;
	GameObject_SetX( player[0]->object, start_pos.x+x );
	GameObject_SetX( player[1]->object, start_pos.x+x+300 );
	GameObject_SetY( player[0]->object, start_pos.y+y );
	GameObject_SetY( player[1]->object, start_pos.y+y );
	GamePlayer_SetRightOriented( player[0] );
	GamePlayer_SetLeftOriented( player[1] );
	GamePlayer_SetStart( player[0] );
	GamePlayer_SetStart( player[1] );
}

static void Game_DemoThread( void *arg )
{
	int battle_id;
	LCUI_Size scene_size;
	LCUI_Widget *demo_scene;

	scene_size.w = 800;
	scene_size.h = 600;
	battle_id = (int)arg;
	GameBattle_SetSceneLand( battle_id, Pos(50,400), Size(700,10) );
	GameBattle_SetSceneSize( battle_id, scene_size );
	demo_scene = GameBattle_GetScene( battle_id );
	Widget_SetZIndex( demo_scene, -10 );
	Widget_SetClickableAlpha( demo_scene, 0, 0 );
	Widget_Container_Add( Game_GetMainMenuBox(), demo_scene );
	GameBattle_SetPlayer( battle_id, 1, ROLE_KUNI, FALSE );
	GameBattle_SetPlayer( battle_id, 2, ROLE_RIKI, FALSE );
	GameBattle_EnablePlayer( battle_id, 1, TRUE );
	GameBattle_EnablePlayer( battle_id, 2, TRUE );
	GameBattlePlayer_SetInvincible( battle_id, 1, TRUE );
	GameBattlePlayer_SetInvincible( battle_id, 2, TRUE );
	GameDemoScene_PutPlayer( battle_id );
	GameBattle_Start( battle_id );
	Widget_Show( demo_scene );
	GameBattle_Loop( battle_id );
}

/** 初始化游戏角色的位置 */
static void Game_InitPlayerPos( int battle_id )
{
	int x, y;
	GamePlayer *p_player[4];
	LCUI_Pos land_pos;
	LCUI_Size land_size;

	p_player[0] = GameBattle_GetPlayer( battle_id, 1 );
	p_player[1] = GameBattle_GetPlayer( battle_id, 2 );
	p_player[2] = GameBattle_GetPlayer( battle_id, 3 );
	p_player[3] = GameBattle_GetPlayer( battle_id, 4 );

	GameBattle_GetSceneLand( battle_id, &land_pos, &land_size );
	/* 计算并设置游戏角色的位置 */
	x = land_size.w/2 - 150;
	GameObject_SetX( p_player[0]->object, land_pos.x+x );
	GameObject_SetX( p_player[1]->object, land_pos.x+x-50 );
	x = land_size.w/2 + 150;
	GameObject_SetX( p_player[2]->object, land_pos.x+x );
	GameObject_SetX( p_player[3]->object, land_pos.x+x+50 );
	y = land_size.h/2;
	GameObject_SetY( p_player[0]->object, land_pos.y+y-50 );
	GameObject_SetY( p_player[1]->object, land_pos.y+y+50 );
	GameObject_SetY( p_player[2]->object, land_pos.y+y-50 );
	GameObject_SetY( p_player[3]->object, land_pos.y+y+50 );
	/* 改变游戏角色的朝向 */
	GamePlayer_SetRightOriented( p_player[0] );
	GamePlayer_SetRightOriented( p_player[1] );
	GamePlayer_SetLeftOriented( p_player[2] );
	GamePlayer_SetLeftOriented( p_player[3] );
}

static void Game_UpdatePlayerStatusBar( int battle_id )
{
	int i;
	RoleInfo *p_role_info;
	wchar_t player_type_name[5];
	GamePlayer *p_player[4];

	p_player[0] = GameBattle_GetPlayer( battle_id, 1 );
	p_player[1] = GameBattle_GetPlayer( battle_id, 2 );
	p_player[2] = GameBattle_GetPlayer( battle_id, 3 );
	p_player[3] = GameBattle_GetPlayer( battle_id, 4 );

	for(i=0; i<4; ++i) {
		if( p_player[i]->human_control ) {
			player_type_name[0] = L'1'+i;
			player_type_name[1] = L'P';
			player_type_name[2] = 0;
		} else {
			player_type_name[0] = L'C';
			player_type_name[1] = L'P';
			player_type_name[2] = L'U';
			player_type_name[3] = 0;
		}
		p_role_info = Game_GetRoleInfo( p_player[i]->role_id );
		StatusBar_SetPlayerNameW( p_player[i]->statusbar, p_role_info->name );
		/* 设置角色类型名 */
		StatusBar_SetPlayerTypeNameW( p_player[i]->statusbar, player_type_name );
		/* 设置血量 */
		StatusBar_SetHealth( p_player[i]->statusbar, p_player[i]->property.cur_hp );
		StatusBar_SetMaxHealth( p_player[i]->statusbar, p_player[i]->property.max_hp );
		/* 显示状态栏 */
		Widget_Show( p_player[i]->statusbar );
	}
}

static void UpdateViewFPS( void *arg )
{
	char str[10];
	LCUI_Widget *label = (LCUI_Widget*)arg;
	sprintf( str, "FPS: %d", LCUIScreen_GetFPS() );
	Label_Text( label, str );
}

static void InitSceneText( LCUI_Widget *scene )
{
	LCUI_Widget *text, *fps_text;
	LCUI_TextStyle style;

	text = Widget_New("label");
	fps_text = Widget_New("label");
	Widget_Container_Add( scene, text );
	Widget_Container_Add( scene, fps_text );
	TextStyle_Init( &style );
	TextStyle_FontSize( &style, 18 );
	Label_TextStyle( fps_text, style );
	Widget_SetAlign( text, ALIGN_TOP_CENTER, Pos(0,40) );
	Widget_SetAlign( fps_text, ALIGN_TOP_CENTER, Pos(0,100) );
	Label_TextW( text, L"<size=42px>游戏测试</size>");
	Widget_SetZIndex( fps_text, -5000 );
	Widget_SetZIndex( text, -5000 );
	Widget_Show( fps_text );
	Widget_Show( text );
	LCUITimer_Set( 500, UpdateViewFPS, fps_text, TRUE );
}

static void Game_ShowPlayerStatusArea(void)
{
	Widget_Show( player_status_area );
}

static int Game_DestroyPlayerStatusArea( int battle_id )
{

}

static int Game_InitPlayerStatusArea( int battle_id )
{
	GamePlayer *p_player[4];

	p_player[0] = GameBattle_GetPlayer( battle_id, 1 );
	p_player[1] = GameBattle_GetPlayer( battle_id, 2 );
	p_player[2] = GameBattle_GetPlayer( battle_id, 3 );
	p_player[3] = GameBattle_GetPlayer( battle_id, 4 );

	player_status_area = Widget_New(NULL);
	Widget_SetBackgroundColor( player_status_area, RGB(240,240,240) );
	Widget_SetBackgroundTransparent( player_status_area, FALSE );
	Widget_SetBorder( player_status_area, Border(1,BORDER_STYLE_SOLID,RGB(150,150,150)));
	Widget_Resize( player_status_area, Size(800,STATUS_BAR_HEIGHT) );
	Widget_SetAlign( player_status_area, ALIGN_BOTTOM_CENTER, Pos(0,0) );
	/* 创建状态栏 */
	p_player[0]->statusbar = StatusBar_New();
	p_player[1]->statusbar = StatusBar_New();
	p_player[2]->statusbar = StatusBar_New();
	p_player[3]->statusbar = StatusBar_New();

	Widget_Container_Add( player_status_area, p_player[0]->statusbar );
	Widget_Container_Add( player_status_area, p_player[1]->statusbar );
	Widget_Container_Add( player_status_area, p_player[2]->statusbar );
	Widget_Container_Add( player_status_area, p_player[3]->statusbar );
	Widget_SetAlign( p_player[0]->statusbar, ALIGN_TOP_LEFT, Pos(5,5) );
	Widget_SetAlign( p_player[1]->statusbar, ALIGN_TOP_LEFT, Pos(5+200,5) );
	Widget_SetAlign( p_player[2]->statusbar, ALIGN_TOP_LEFT, Pos(5+400,5) );
	Widget_SetAlign( p_player[3]->statusbar, ALIGN_TOP_LEFT, Pos(5+600,5) );
	return 0;
}

static int Game_InitFight( int role_id[4] )
{
	int ret, battle_id;
	LCUI_Graph stage_img;
	LCUI_Pos land_pos;
	ControlKey ctrlkey;
	LCUI_Size land_size, scene_size;
	
	ret = GameGraphRes_LoadFromFile("scenes.data");
	if( ret != 0 ) {
		LCUI_MessageBoxW(
			MB_ICON_ERROR,
			L"场景资源载入出错，请检查程序的完整性！",
			L"错误", MB_BTN_OK );
		return ret;
	}
	
	Graph_Init( &stage_img );
	/* 载入战斗场景的图形资源 */
	GameGraphRes_GetGraph( SCENES_RES, "default", &stage_img );

	battle_id = GameBattle_New();
	scene_size = Graph_GetSize( &stage_img );
	land_pos.x = 20;
	land_pos.y = 268;
	land_size.w = scene_size.w - 2*land_pos.x;
	land_size.h = scene_size.h - land_pos.y;

	GameBattle_SetSceneBackground( battle_id, &stage_img );
	GameBattle_SetSceneSize( battle_id, scene_size );
	GameBattle_SetSceneLand( battle_id, land_pos, land_size );

	Game_AddCPUPlayerRole( battle_id, role_id );
	GameBattle_SetPlayer( battle_id, 1, role_id[0], TRUE );
	GameBattle_SetPlayer( battle_id, 2, role_id[1], FALSE );
	GameBattle_SetPlayer( battle_id, 3, role_id[2], FALSE );
	GameBattle_SetPlayer( battle_id, 4, role_id[3], FALSE );
	GameBattle_EnablePlayer( battle_id, 1, TRUE );
	GameBattle_EnablePlayer( battle_id, 2, TRUE );
	GameBattle_EnablePlayer( battle_id, 3, TRUE );
	GameBattle_EnablePlayer( battle_id, 4, TRUE );

	/* 设置战斗场景显示的位置 */
	Widget_SetAlign( GameBattle_GetScene(battle_id), 
	ALIGN_BOTTOM_CENTER, Pos(0,-STATUS_BAR_HEIGHT) );
	/* 初始化在场景上显示的文本 */
	InitSceneText( GameBattle_GetScene(battle_id) );
	
	ctrlkey.a_attack = LCUIKEY_J;
	ctrlkey.b_attack = LCUIKEY_K;
	ctrlkey.jump = LCUIKEY_SPACE;
	ctrlkey.defense = LCUIKEY_L;
	ctrlkey.left = LCUIKEY_A;
	ctrlkey.right = LCUIKEY_D;
	ctrlkey.up = LCUIKEY_W;
	ctrlkey.down = LCUIKEY_S;
	/* 设置1号玩家的控制键 */
	GameBattle_SetPlayerControlKey( battle_id, 1, &ctrlkey );

	Game_InitPlayerStatusArea( battle_id );
	Game_InitPlayerPos( battle_id );
	Game_UpdatePlayerStatusBar( battle_id );
	GameBattle_SetAutoSyncCamera( battle_id, TRUE );
	return battle_id;
}

/** 显示退出提示框 */
static void Game_ShowQuitTipBox( void *arg1, void *arg2 )
{
	int ret;
	GameBattle_Pause( main_battle_id, TRUE );
	ret = LCUI_MessageBoxW(	MB_ICON_WARNING, 
				L"是否退出本场对战？", 
				L"提示", MB_BTN_OKCANCEL );
	GameBattle_Pause( main_battle_id, FALSE );
	if( ret == MB_BTN_IS_OK ) {
		GameBattle_Quit( main_battle_id );
	}
}

/** 在游戏的对战场景中对键盘进行响应 */
static void Game_ProcKeyboardInBattle( LCUI_KeyboardEvent *event, void *arg )
{
	LCUI_Task task;
	if( event->type != LCUI_KEYDOWN ) {
		return;
	}
	if( event->key_code != LCUIKEY_ESC ) {
		return;
	}
	/** 准备程序任务 */
	task.id = main_thread_id;
	task.destroy_arg[0] = FALSE;
	task.destroy_arg[1] = FALSE;
	task.func = Game_ShowQuitTipBox;
	task.arg[0] = NULL;
	task.arg[1] = NULL;
	/* 投递至程序任务队列 */
	AppTasks_Add( &task );
}

static void Game_StartFight( int battle_id )
{
	GamePlayer *p_player[4];
	LCUI_Widget *game_scene;

	p_player[0] = GameBattle_GetPlayer( battle_id, 1 );
	p_player[1] = GameBattle_GetPlayer( battle_id, 2 );
	p_player[2] = GameBattle_GetPlayer( battle_id, 3 );
	p_player[3] = GameBattle_GetPlayer( battle_id, 4 );
	
	/* 设置游戏角色的初始状态 */
	GamePlayer_SetStart( p_player[0] );
	GamePlayer_SetStart( p_player[1] );
	GamePlayer_SetStart( p_player[2] );
	GamePlayer_SetStart( p_player[3] );

	game_scene = GameBattle_GetScene(battle_id);
	Widget_Show( game_scene );
	Game_ShowPlayerStatusArea();
	GameBattle_InitKeyboardControl();
	GameBattle_Start( battle_id );
	LCUI_KeyboardEvent_Connect( Game_ProcKeyboardInBattle, NULL );
	GameBattle_Loop( battle_id );
}

/** 游戏线程 */
static void Game_MainThread( void *arg )
{
	int role_id[4];
	int demo_battle;
	LCUI_Thread t;

	srand((unsigned int)time(NULL));
	/* 注册游戏所需的部件类型 */
	GameObject_Register();
	GameMenuBtn_Register();
	GameMenu_Register();
	StatusBar_Register();
	LifeBar_Regiser();

	Game_InitAttackLibrary();
	Game_InitStateActionMap();
	SkillLibrary_Init();

	LCUICursor_Hide();			/* 隐藏鼠标游标 */
	Game_ShowBootScreen();			/* 显示启动画面 */
	LCUICursor_Show();			/* 显示鼠标游标 */
	Game_InitMainUI();			/* 初始化主菜单界面 */
	Game_ShowMainUI();			/* 显示主菜单界面 */
	
	GameMsgLoopStart();
	
	demo_battle = GameBattle_New();
	LCUIThread_Create( &t, Game_DemoThread, (void*)demo_battle );
	while(1) {
		role_id[0] = Game_GetSelectedRole();
		if( role_id[0] == -1 ) {
			LCUI_MSleep(100);
			continue;
		}
		/* 暂停演示对战 */
		GameBattle_Pause( demo_battle, TRUE );
		Game_HideMainUI();
		main_battle_id = Game_InitFight( role_id );
		Game_StartFight( main_battle_id );
		/* 继续演示对战 */
		GameBattle_Pause( demo_battle, FALSE );
		Game_ShowMainUI();
		break;
	}
}

#ifdef LCUI_BUILD_IN_WIN32
#include <io.h>
#include <fcntl.h>

/* 在运行程序时会打开控制台，以查看打印的调试信息 */
static void InitConsoleWindow(void)
{
	int hCrt;
	FILE *hf;
	AllocConsole();
	hCrt=_open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE),_O_TEXT );
	hf=_fdopen( hCrt, "w" );
	*stdout=*hf;
	setvbuf (stdout, NULL, _IONBF, 0);
	// test code
	printf ("InitConsoleWindow OK!\n");
}
#endif

static int Game_LoadResource(void)
{
	int ret;
	ret = GameGraphRes_LoadFromFile("main.data");
	if( ret != 0 ) {
		LCUI_MessageBoxW(
			MB_ICON_ERROR,
			L"游戏资源载入出错，请检查程序的完整性！",
			L"错误", MB_BTN_OK );
		return -1;
	}
	ret = GameGraphRes_LoadFromFile( "font.data" );
	if( ret != 0 ) {
		LCUI_MessageBoxW(
			MB_ICON_ERROR,
			L"字体资源载入出错，请检查程序的完整性！",
			L"错误", MB_BTN_OK );
		return -2;
	}
	ret = GameGraphRes_LoadFromFile("action-riki.data");
	ret |= GameGraphRes_LoadFromFile("action-kuni.data");
	ret |= GameGraphRes_LoadFromFile("action-mike.data");
	ret |= GameGraphRes_LoadFromFile("action-ben.data");
	ret |= GameGraphRes_LoadFromFile("action-toraji.data");
	if( ret != 0 ) {
		LCUI_MessageBoxW(
			MB_ICON_ERROR,
			L"角色资源载入出错，请检查程序的完整性！",
			L"错误", MB_BTN_OK );
		return -3;
	}
	return 0;
}

int main( int argc, char **argv )
{
	int ret;
	int mode;
	LCUI_Thread t;
//#define DEBUG
#if defined (LCUI_BUILD_IN_WIN32) && defined (DEBUG)
	InitConsoleWindow();
#endif
	GameConfig_Load();
	if( GameConfig_IsWindowed() ) {
		mode = LCUI_INIT_MODE_WINDOW;
	} else {
		mode = LCUI_INIT_MODE_FULLSCREEN;
	}
	/* 初始化LCUI */
	LCUI_Init( GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, mode );
	GameGraphRes_Init();		/* 初始化游戏资源 */
	ret = Game_LoadResource();	/* 载入游戏资源 */
	if( ret != 0 ) {
		return -1;
	}
	/* 保存主线程ID */
	main_thread_id = LCUIThread_SelfID();
	LCUIThread_Create( &t, Game_MainThread, NULL );
	return LCUI_Main();
}
