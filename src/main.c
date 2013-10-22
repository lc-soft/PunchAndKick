#define SKIP_BOOT_SCREEN
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
#include "game_logobtn.h"
#include "game_titlebarbtn.h"

#define TEXT_COPYRIGHT_EN	L"Developed by LC-Games, Copyright © 2013 LC-Games, All Rights Reserved."
#define TEXT_COPYRIGHT_CN	L"本游戏由 LC-Games 开发 , LC-Games 保留所有权利。"
#define TEXT_STATEMENT		L"<size=20px>"\
				L"此版本的游戏仅用于展示：\n\n"\
				L" ● 游戏主菜单界面\n"\
				L" ● 游戏角色的控制\n"\
				L" ● 技能动作效果\n"\
				L" ● 血条的动态效果\n"\
				L" ● 人工智能\n\n"\
				L"游戏中大部分BUG是已知的，请无视本程序出现的一些细节上的BUG。\n"\
				L"如果遇到较为严重的BUG，请关闭并重新运行本程序。</size>"

/** 在屏幕上显示游戏启动时的画面 */
static void Game_ShowBootScreen(void)
{
	uchar_t alpha;
	LCUI_Graph img_game_logo;
	LCUI_Widget *wdg_img_logo, *wdg_text;
	LCUI_Widget *wdg_copyright_cn_text, *wdg_copyright_en_text;

	wdg_img_logo = Widget_New(NULL);
	wdg_copyright_cn_text = Widget_New("label");
	wdg_copyright_en_text = Widget_New("label");
	wdg_text = Widget_New("label");
	
	Graph_Init( &img_game_logo );
	GameGraphRes_GetGraph(MAIN_RES, "main-logo", &img_game_logo );
	
	Label_TextW( wdg_copyright_en_text, TEXT_COPYRIGHT_EN );
	Label_TextW( wdg_copyright_cn_text, TEXT_COPYRIGHT_CN );
	Label_TextW( wdg_text, TEXT_STATEMENT );

	Widget_Resize( wdg_img_logo, Graph_GetSize(&img_game_logo) );
	
	Widget_SetAlign( wdg_copyright_cn_text, ALIGN_BOTTOM_CENTER, Pos(0,-20) );
	Widget_SetAlign( wdg_copyright_en_text, ALIGN_BOTTOM_CENTER, Pos(0,-40) );
	Widget_SetAlign( wdg_img_logo, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	Widget_SetAlign( wdg_text, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	
	Widget_SetBackgroundImage( wdg_img_logo, &img_game_logo );
	Widget_SetBackgroundLayout( wdg_img_logo, LAYOUT_CENTER );
	
	Widget_SetAlpha( wdg_img_logo, 0 );
	Widget_SetAlpha( wdg_copyright_cn_text, 0 );
	Widget_SetAlpha( wdg_copyright_en_text, 0 );
	Widget_SetAlpha( wdg_text, 0 );

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
	Widget_Show( wdg_text );
#ifndef SKIP_BOOT_SCREEN
	/* 以淡入淡出的效果显示文本内容 */
	for( alpha=0; alpha<250; alpha+=10 ) {
		Widget_SetAlpha( wdg_text, alpha );
		LCUI_MSleep(25);
	}
	Widget_SetAlpha( wdg_text, 255 );
	LCUI_Sleep(2);
	for( alpha=255; alpha>5; alpha-=10 ) {
		Widget_SetAlpha( wdg_text, alpha );
		LCUI_MSleep(25);
	}
#endif
	Widget_Hide( wdg_text );
	Widget_Destroy( wdg_img_logo );
	Widget_Destroy( wdg_text );
	Widget_Destroy( wdg_copyright_en_text );
	Widget_Destroy( wdg_copyright_cn_text );
}

/** 加入CPU玩家 */
static void Game_AddCPUPlayer(void)
{
#ifdef aaaa
	int i, j, role_id;
	GamePlayer *player, *other_player;
	for(j=2; j<5; ++j) {
		player = GamePlayer_GetByID(j);
		while(1) {
			/* 随机选择一个角色 */
			role_id = rand()%TOTAL_ROLE_NUM;
			/* 检查是否与前面几个角色的ID相同 */
			for(i=1; i<j; ++i) {
				other_player = GamePlayer_GetByID(i);
				if( !other_player ) {
					continue;	
				}
				if( role_id == other_player->role_id ) {
					break;
				}
			}
			if( i>=j ) {
				break;
			}
		}
		Game_EnableGamePlayer( j );
		Game_SetGamePlayer( j, role_id, FALSE );
	}
#endif
}

static void GameDemoScene_PutPlayer( int battle_id )
{
	int i;
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
	battle_id = GameBattle_New();
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

/** 游戏线程 */
static void Game_MainThread( void *arg )
{
	int role_id;
	ControlKey ctrlkey;
	LCUI_Thread t;

	srand((unsigned int)time(NULL));
	TitleBarBtn_Register();
	LogoBtn_Register();
	GameObject_Register();

	Game_InitAttackRecord();
	Game_InitStateActionMap();
	SkillLibrary_Init();

	LCUICursor_Hide();			/* 隐藏鼠标游标 */
	Game_ShowBootScreen();			/* 显示启动画面 */
	LCUICursor_Show();			/* 显示鼠标游标 */
	Game_InitMainMenu();			/* 初始化主菜单界面 */
	Game_ShowMainMenu();			/* 显示主菜单界面 */
	
	GameMsgLoopStart();
	LCUIThread_Create( &t, Game_DemoThread, NULL );
	//LCUIThread_Exit(NULL);
	return;
#ifdef asdasdasd
	while(1) {
		role_id = Game_GetSelectedRole();	/* 获取选择的角色的ID */
		if( role_id == -1 ) {
			LCUI_MSleep(100);
			continue;
		}
		Game_HideMainMenu();			/* 隐藏主菜单界面 */
		/* 设置1号玩家的控制键 */
		ctrlkey.a_attack = LCUIKEY_J;
		ctrlkey.b_attack = LCUIKEY_K;
		ctrlkey.jump = LCUIKEY_SPACE;
		ctrlkey.defense = LCUIKEY_L;
		ctrlkey.left = LCUIKEY_A;
		ctrlkey.right = LCUIKEY_D;
		ctrlkey.up = LCUIKEY_W;
		ctrlkey.down = LCUIKEY_S;

		Game_InitBattle();				/* 初始化对战 */
		Game_EnableGamePlayer( 1 );			/* 启用1号玩家 */
		Game_SetGamePlayer( 1, role_id, TRUE );		/* 设置1号玩家信息 */
		Game_SetGamePlayerControlKey( 1, &ctrlkey );	/* 设置1号玩家的控制键 */
		Game_AddCPUPlayer();				/* 补全剩余玩家信息 */
		Game_StartBattle();				/* 开始对战 */
		Game_Loop();					/* 进入游戏主循环 */
	}
#endif
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
	LCUI_Thread t;
//#define DEBUG
#if defined (LCUI_BUILD_IN_WIN32) && defined (DEBUG)
	InitConsoleWindow();
#endif
	/* 初始化LCUI */
	LCUI_Init(	GAME_SCREEN_WIDTH, 
			GAME_SCREEN_HEIGHT, 
			LCUI_INIT_MODE_WINDOW );

	GameGraphRes_Init();		/* 初始化游戏资源 */
	ret = Game_LoadResource();	/* 载入游戏资源 */
	if( ret != 0 ) {
		return -1;
	}
	LCUIThread_Create( &t, Game_MainThread, NULL );
	return LCUI_Main();
}
