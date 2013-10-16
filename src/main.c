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
#include "skills/game_skill.h"
#include "game_role_select.h"

#define TEXT_COPYRIGHT		L"Developed by LC-Games."
#define TEXT_STATEMENT		L"<size=20px>"\
				L"本程序仅用于展示：\n\n"\
				L" ● 游戏角色的控制\n"\
				L" ● 技能动作效果\n"\
				L" ● 血条的动态效果\n"\
				L" ● 人工智能\n\n"\
				L"游戏中大部分BUG是已知的，请无视本程序出现的一些细节上的BUG。\n"\
				L"如果遇到较为严重的BUG，请关闭并重新运行本程序。</size>"

static void Game_ShowBootScreen(void)
{
	uchar_t alpha;
	LCUI_Graph img_game_logo;
	LCUI_Widget *wdg_img_logo, *wdg_text, *wdg_copyright_text;

	wdg_img_logo = Widget_New(NULL);
	wdg_copyright_text = Widget_New("label");
	wdg_text = Widget_New("label");
	
	Graph_Init( &img_game_logo );
	GameGraphRes_GetGraph(MAIN_RES, "main-logo", &img_game_logo );
	
	Label_TextW( wdg_copyright_text, TEXT_COPYRIGHT );
	Label_TextW( wdg_text, TEXT_STATEMENT );

	Widget_Resize( wdg_img_logo, Graph_GetSize(&img_game_logo) );
	
	Widget_SetAlign( wdg_copyright_text, ALIGN_BOTTOM_CENTER, Pos(0,-20) );
	Widget_SetAlign( wdg_img_logo, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	Widget_SetAlign( wdg_text, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	
	Widget_SetBackgroundImage( wdg_img_logo, &img_game_logo );
	Widget_SetBackgroundLayout( wdg_img_logo, LAYOUT_CENTER );
	
	Widget_SetAlpha( wdg_img_logo, 0 );
	Widget_SetAlpha( wdg_copyright_text, 0 );
	Widget_SetAlpha( wdg_text, 0 );

	Widget_Show( wdg_img_logo );
	Widget_Show( wdg_copyright_text );
#ifndef SKIP_BOOT_SCREEN
	/* 以淡入淡出的效果显示LOGO */
	for( alpha=0; alpha<250; alpha+=10 ) {
		Widget_SetAlpha( wdg_img_logo, alpha );
		Widget_SetAlpha( wdg_copyright_text, alpha );
		LCUI_MSleep(25);
	}
	Widget_SetAlpha( wdg_img_logo, 255 );
	Widget_SetAlpha( wdg_copyright_text, 255 );
	LCUI_Sleep(2);
	for( alpha=255; alpha>5; alpha-=10 ) {
		Widget_SetAlpha( wdg_img_logo, alpha );
		Widget_SetAlpha( wdg_copyright_text, alpha );
		LCUI_MSleep(25);
	}
#endif
	Widget_SetAlpha( wdg_img_logo, 0 );
	Widget_SetAlpha( wdg_copyright_text, 0 );
	Widget_Hide( wdg_img_logo );
	Widget_Hide( wdg_copyright_text );
	Widget_Show( wdg_text );
#ifndef SKIP_BOOT_SCREEN
	/* 以淡入淡出的效果显示LOGO */
	for( alpha=0; alpha<250; alpha+=10 ) {
		Widget_SetAlpha( wdg_text, alpha );
		LCUI_MSleep(25);
	}
	Widget_SetAlpha( wdg_text, 255 );
	LCUI_Sleep(4);
	for( alpha=255; alpha>5; alpha-=10 ) {
		Widget_SetAlpha( wdg_text, alpha );
		LCUI_MSleep(25);
	}
#endif
	Widget_Hide( wdg_text );
	Widget_Destroy( wdg_img_logo );
	Widget_Destroy( wdg_text );
	Widget_Destroy( wdg_copyright_text );
}

static void Game_AddCPUPlayer(void)
{
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
}

static void Game_MainThread( void *arg )
{
	int role_id;
	ControlKey ctrlkey;

	srand((unsigned int)time(NULL));
	GameObject_Register();
	LCUICursor_Hide();		/* 隐藏鼠标游标 */
	Game_ShowBootScreen();		/* 显示启动画面 */
	LCUICursor_Show();		/* 显示鼠标游标 */
	Game_InitMainMenu();
	Game_ShowMainMenu();
	Game_InitRoleSelectBox();
	Game_ShowRoleSelectBox();
	role_id = Game_GetSelectedRole();
	Game_HideMainMenu();

	ctrlkey.a_attack = LCUIKEY_J;
	ctrlkey.b_attack = LCUIKEY_K;
	ctrlkey.jump = LCUIKEY_SPACE;
	ctrlkey.defense = LCUIKEY_L;
	ctrlkey.left = LCUIKEY_A;
	ctrlkey.right = LCUIKEY_D;
	ctrlkey.up = LCUIKEY_W;
	ctrlkey.down = LCUIKEY_S;

	Game_InitBattle();
	Game_EnableGamePlayer( 1 );
	Game_SetGamePlayer( 1, role_id, TRUE );
	Game_SetGamePlayerControlKey( 1, &ctrlkey );
	Game_AddCPUPlayer();
	Game_StartBattle();
	Game_Loop();

	LCUIThread_Exit(NULL);
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

int main( int argc, char **argv )
{
	int ret;
	LCUI_Thread t;
//#define DEBUG
#if defined (LCUI_BUILD_IN_WIN32) && defined (DEBUG)
	InitConsoleWindow();
#endif
	LCUI_Init(800,600,0);

	/* 初始化游戏资源 */
	GameGraphRes_Init();
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
		return -1;
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
		return -2;
	}
	/* 创建一个线程，用于进行游戏的初始化 */
	LCUIThread_Create( &t, Game_MainThread, NULL );
	return LCUI_Main();
}
