//#define SKIP_BOOT_SCREEN
#define I_NEED_WINMAIN
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_CURSOR_H
#include LC_WIDGET_H
#include LC_LABEL_H

#include "game.h"

#define IMG_PATH_GAME_LOGO	"drawable/lcsoft-logo.png"
#define IMG_PATH_BG		"drawable/bg.png"
#define IMG_PATH_RING		"drawable/ring.png"
#define IMG_PATH_LC_LOGO	"drawable/logo.png"

static LCUI_Graph img_game_logo, img_bg, img_ring, img_lc_logo;

static void GraphRes_Load(void)
{
	Graph_Init( &img_game_logo );
	Graph_Init( &img_bg );
	Graph_Init( &img_lc_logo );
	Graph_Init( &img_ring );
	Graph_LoadImage( IMG_PATH_GAME_LOGO, &img_game_logo );
	Graph_LoadImage( IMG_PATH_LC_LOGO, &img_lc_logo );
	Graph_LoadImage( IMG_PATH_BG, &img_bg );
	Graph_LoadImage( IMG_PATH_RING, &img_ring );
}

static void Game_ShowBootScreen(void)
{
	uchar_t alpha;
	LCUI_Widget *wdg_img_logo;
	
	wdg_img_logo = Widget_New(NULL);
	Widget_Resize( wdg_img_logo, Graph_GetSize(&img_game_logo) );
	Widget_SetAlign( wdg_img_logo, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	Widget_SetBackgroundImage( wdg_img_logo, &img_game_logo );
	Widget_SetBackgroundLayout( wdg_img_logo, LAYOUT_CENTER );
	Widget_SetAlpha( wdg_img_logo, 0 );
	Widget_Show( wdg_img_logo );
#ifndef SKIP_BOOT_SCREEN
	/* 以淡入淡出的效果显示LOGO */
	for( alpha=0; alpha<250; alpha+=10 ) {
		Widget_SetAlpha( wdg_img_logo, alpha );
		LCUI_MSleep(25);
	}
	Widget_SetAlpha( wdg_img_logo, 255 );
	LCUI_Sleep(1);
	for( alpha=255; alpha>5; alpha-=10 ) {
		Widget_SetAlpha( wdg_img_logo, alpha );
		LCUI_MSleep(25);
	}
#endif
	Widget_SetAlpha( wdg_img_logo, 0 );
	Widget_Hide( wdg_img_logo );
	Widget_Destroy( wdg_img_logo );
}

static LCUI_BOOL res_load_state = 0;

/* 载入游戏数据 */
static void Game_LoadData( void *arg )
{
	LCUI_Widget *wdg_label;

	wdg_label = (LCUI_Widget*)arg;
	Label_TextW( wdg_label, L"载入主菜单资源..." );
	LCUI_MSleep(100);
	if( Game_LoadMenuRes() != 0 ) {
		Label_TextW( wdg_label, L"主菜单资源载入失败！" );
		res_load_state = -1;
		LCUIThread_Exit(NULL);
	}
	Label_TextW( wdg_label, L"载入角色数据..." );
	LCUI_MSleep(500);
	Label_TextW( wdg_label, L"已载入完成" );
	res_load_state = 1;
	LCUIThread_Exit(NULL);
}

static int Game_ShowLoadingScreen(void)
{
	int angle = 0;
	LCUI_Thread t;
	uchar_t alpha;
	LCUI_TextStyle style;
	LCUI_Graph img_buff;
	LCUI_Widget *wdg_logo, *wdg_ring, *wdg_text;
	
	Graph_Init( &img_buff );
	TextStyle_Init( &style );
	TextStyle_FontSize( &style, 18 );

	wdg_logo = Widget_New(NULL);
	wdg_ring = Widget_New(NULL);
	wdg_text = Widget_New("label");
	Widget_Container_Add( wdg_logo, wdg_ring );
	Widget_Resize( wdg_logo, Size(128,128) );
	Widget_Resize( wdg_ring, Size(128,128) );
	Widget_SetBackgroundImage( wdg_ring, &img_ring );
	Widget_SetBackgroundImage( wdg_logo, &img_lc_logo );
	Widget_SetBackgroundLayout( wdg_logo, LAYOUT_CENTER );
	Widget_SetBackgroundLayout( wdg_ring, LAYOUT_CENTER );
	Widget_SetAlign( wdg_logo, ALIGN_MIDDLE_CENTER, Pos(0,-20) );
	Widget_SetAlign( wdg_ring, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	Widget_SetAlign( wdg_text, ALIGN_MIDDLE_CENTER, Pos(0,80) );
	Widget_SetAlpha( wdg_logo, 0 );
	Widget_SetAlpha( wdg_ring, 0 );
	Widget_SetAlpha( wdg_text, 0 );
	Widget_Show( wdg_logo );
	Widget_Show( wdg_ring );
	Widget_Show( wdg_text );

	Label_TextStyle( wdg_text, style );
	Label_TextW( wdg_text, L"正初始化..." );
#ifndef SKIP_BOOT_SCREEN
	/* 以淡入的效果显示LOGO */
	for( alpha=0; alpha<250; alpha+=10 ) {
		Widget_SetAlpha( wdg_logo, alpha );
		Widget_SetAlpha( wdg_text, alpha );
		LCUI_MSleep(25);
	}
#endif
	Widget_SetAlpha( wdg_logo, 255 );
	Widget_SetAlpha( wdg_text, 255 );
	
#ifndef SKIP_BOOT_SCREEN
	for( angle=0,alpha=0; alpha<250; alpha+=10, angle+=10 ) {
		Widget_SetAlpha( wdg_ring, alpha );
		Graph_Rotate( &img_ring, angle, &img_buff );
		Widget_SetBackgroundImage( wdg_ring, &img_buff );
		LCUI_MSleep(25);
	}
#endif
	Widget_SetAlpha( wdg_ring, 255 );
	LCUIThread_Create( &t, Game_LoadData, (void*)wdg_text );
	while(1) {
		Graph_Rotate( &img_ring, angle, &img_buff );
		Widget_SetBackgroundImage( wdg_ring, &img_buff );
		LCUI_MSleep(25);
		angle+=10;
		if( angle >= 360 ) {
			angle = 0;
		}
		if( res_load_state == 1 ) {
			break;
		} else if( res_load_state == -1 ) {
			return -1;
		}
	}
#ifndef SKIP_BOOT_SCREEN
	for( alpha=250; alpha>10; alpha-=10 ) {
		Widget_SetAlpha( wdg_ring, alpha );
		Graph_Rotate( &img_ring, angle, &img_buff );
		Widget_SetBackgroundImage( wdg_ring, &img_buff );
		if( angle >= 360 ) {
			angle = 0;
		}
		angle+=10;
		if( angle >= 360 ) {
			angle = 0;
		}
		LCUI_MSleep(25);
	}
#endif
	Widget_SetAlpha( wdg_ring, 0 );
	Widget_Hide( wdg_ring );
#ifndef SKIP_BOOT_SCREEN
	/* 以淡出的效果显示LOGO */
	for( alpha=250; alpha>10; alpha-=20 ) {
		Widget_SetAlpha( wdg_logo, alpha );
		Widget_SetAlpha( wdg_text, alpha );
		LCUI_MSleep(25);
	}
#endif
	Widget_SetAlpha( wdg_logo, 0 );
	Widget_SetAlpha( wdg_text, 0 );
	Widget_Hide( wdg_logo );
	Widget_Hide( wdg_text );
	Widget_Destroy( wdg_text );
	Widget_Destroy( wdg_logo );
	return 0;
}

static void Game_MainThread( void *arg )
{
	int ret;
	LCUICursor_Hide();		/* 隐藏鼠标游标 */
	Game_ShowBootScreen();		/* 显示启动画面 */
	LCUICursor_Show();		/* 显示鼠标游标 */
	ret = Game_ShowLoadingScreen();	/* 显示载入画面 */
	if( ret == 0 ) {
		Game_MainMenu();	/* 进入游戏主菜单 */
	}
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
	LCUI_Widget *widget;
	
#ifdef LCUI_BUILD_IN_WIN32
	//InitConsoleWindow();
#endif
	LCUI_Init(640,480,0);
	/* 预先载入图像资源 */
	GraphRes_Load();
	/* 修改根部件的背景图 */
	widget = RootWidget_GetSelf();
	Widget_SetBackgroundImage( widget, &img_bg );
	Widget_SetBackgroundLayout( widget, LAYOUT_TILE );
	/* 创建一个线程，用于进行游戏的初始化 */
	LCUIThread_Create( &t, Game_MainThread, NULL );
	return LCUI_Main();
}
