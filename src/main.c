//#define SKIP_BOOT_SCREEN
#define I_NEED_WINMAIN
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_CURSOR_H
#include LC_WIDGET_H
#include LC_TEXTSTYLE_H
#include LC_DISPLAY_H
#include LC_LABEL_H
#include "game.h"

#define TEXT_COPYRIGHT		L"Developed by LC-Games."

static void Game_ShowBootScreen(void)
{
	uchar_t alpha;
	LCUI_Widget *wdg_img_logo, *wdg_text;
	LCUI_Graph img_game_logo;

	wdg_img_logo = Widget_New(NULL);
	wdg_text = Widget_New("label");
	
	Graph_Init( &img_game_logo );
	GameGraphRes_GetGraph(MAIN_RES, "main-logo", &img_game_logo );
	Label_TextW( wdg_text, TEXT_COPYRIGHT );
	Widget_Resize( wdg_img_logo, Graph_GetSize(&img_game_logo) );
	Widget_SetAlign( wdg_text, ALIGN_BOTTOM_CENTER, Pos(0,-20) );
	Widget_SetAlign( wdg_img_logo, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	Widget_SetBackgroundImage( wdg_img_logo, &img_game_logo );
	Widget_SetBackgroundLayout( wdg_img_logo, LAYOUT_CENTER );
	Widget_SetAlpha( wdg_img_logo, 0 );
	Widget_SetAlpha( wdg_text, 0 );
	Widget_Show( wdg_img_logo );
	Widget_Show( wdg_text );
#ifndef SKIP_BOOT_SCREEN
	/* 以淡入淡出的效果显示LOGO */
	for( alpha=0; alpha<250; alpha+=10 ) {
		Widget_SetAlpha( wdg_img_logo, alpha );
		Widget_SetAlpha( wdg_text, alpha );
		LCUI_MSleep(25);
	}
	Widget_SetAlpha( wdg_img_logo, 255 );
	Widget_SetAlpha( wdg_text, 255 );
	LCUI_Sleep(2);
	for( alpha=255; alpha>5; alpha-=10 ) {
		Widget_SetAlpha( wdg_img_logo, alpha );
		Widget_SetAlpha( wdg_text, alpha );
		LCUI_MSleep(25);
	}
#endif
	Widget_SetAlpha( wdg_img_logo, 0 );
	Widget_SetAlpha( wdg_text, 0 );
	Widget_Hide( wdg_img_logo );
	Widget_Hide( wdg_text );
	Widget_Destroy( wdg_img_logo );
	Widget_Destroy( wdg_text );
}

static void UpdateViewFPS( void *arg )
{
	char str[10];
	LCUI_Widget *label = (LCUI_Widget*)arg;
	sprintf( str, "FPS: %d", LCUIScreen_GetFPS() );
	Label_Text( label, str );
}

static void InitViewFPS( void )
{
	LCUI_Widget *widget;
	LCUI_TextStyle style;

	widget = Widget_New("label");
	TextStyle_Init( &style );
	TextStyle_FontSize( &style, 14 );
	Label_TextStyle( widget, style );
	Widget_SetAlign( widget, ALIGN_TOP_RIGHT, Pos(-20,20) );
	LCUITimer_Set( 500, UpdateViewFPS, widget, TRUE );
	Widget_Show( widget );
}

static void Game_MainThread( void *arg )
{
	int ret;
	ret = GameGraphRes_LoadFromFile("main.data");
	if( ret != 0 ) {
		LCUI_MessageBoxW(
			MB_ICON_ERROR,
			L"游戏资源载入出错，请检查程序的完整性！",
			L"错误", MB_BTN_OK );
		LCUIThread_Exit(NULL);
		return;
	}
	LCUICursor_Hide();		/* 隐藏鼠标游标 */
	Game_ShowBootScreen();		/* 显示启动画面 */
	LCUICursor_Show();		/* 显示鼠标游标 */
	InitViewFPS();
	ret = Game_Init();
	if( ret != 0 ) {
		LCUI_MainLoop_Quit(NULL);
		LCUIThread_Exit(NULL);
	}
	Game_Start();
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
	LCUI_Thread t;
	
#ifdef LCUI_BUILD_IN_WIN32
	//InitConsoleWindow();
#endif
	LCUI_Init(640,480,0);
	
	/* 初始化游戏资源 */
	GameGraphRes_Init();
	/* 创建一个线程，用于进行游戏的初始化 */
	LCUIThread_Create( &t, Game_MainThread, NULL );
	return LCUI_Main();
}
