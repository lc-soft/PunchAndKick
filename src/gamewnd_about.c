// 游戏窗口-关于
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_WINDOW_H
#include LC_LABEL_H
#include LC_BUTTON_H
#include LC_TEXTBOX_H

#include "game_resource.h"
#include "gamewnd_about.h"

#define LOGOBTN_SIZE	Size(116,116)
#define WINDOW_SIZE	Size(350,300)
#define BTN_SIZE	Size(80,30)
#define LOGO_SIZE	Size(64,64)

#define TEXT_OK		L"确定"
#define TEXT_TITLE	L"关于本游戏"
#define TEXT_MEMBER	L" 制作人员 "
const wchar_t member_list_text[]={
	L"神马"
};

const wchar_t about_text[]={
	L"2D格斗游戏 (非正式名称)\n"
	L"版本 1.1.0\n"
	L"版权所有 (C) 2013 LC-Games 开发组。\n"
	L"保留所有权利。"
};

enum RES_ID {
	RES_LOGO_NORMAL,
	RES_LOGOBTN_NORMAL,
	RES_LOGOBTN_HOVER,
	RES_LOGOBTN_PRESSED
};

static LCUI_Widget *window;
static LCUI_Widget *wdg_img_logo, *label_about_text;
static LCUI_Widget *label_member;
static LCUI_Widget *textbox_member_list;
static LCUI_Widget *box;
static LCUI_Widget *btn_ok;

static LCUI_Graph img_game_logo;

static void btn_ok_on_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *event )
{
	GameWindow_HideAboutWindow();
	GameWindow_DestroyAboutWindow();
}

void GameWindow_InitAboutWindow(void)
{
	window = Widget_New("window");
	label_about_text = Widget_New("label");
	label_member = Widget_New("label");
	textbox_member_list = Widget_New("text_box");
	wdg_img_logo = Widget_New(NULL);
	box = Widget_New(NULL);
	btn_ok = Widget_New("button");

	Graph_Init( &img_game_logo );
	GameGraphRes_GetGraph( MAIN_RES, "game-logo", &img_game_logo );

	Window_ClientArea_Add( window, label_about_text );
	Window_ClientArea_Add( window, label_member );
	Window_ClientArea_Add( window, wdg_img_logo );
	Window_ClientArea_Add( window, box );
	Window_ClientArea_Add( window, btn_ok );
	Widget_Container_Add( box, textbox_member_list );
	
	Widget_SetBackgroundColor( label_member, RGB(255,255,255) );
	Widget_SetBackgroundTransparent( label_member, FALSE );
	
	Widget_SetAlign( wdg_img_logo, ALIGN_TOP_LEFT, Pos(0,0) );
	Widget_SetAlign( label_about_text, ALIGN_TOP_LEFT, Pos(76,0) );
	Widget_SetAlign( label_member, ALIGN_TOP_CENTER, Pos(0,72) );
	Widget_SetAlign( box, ALIGN_TOP_CENTER, Pos(0,80) );
	Widget_SetAlign( btn_ok, ALIGN_BOTTOM_CENTER, Pos(0,-5) );

	Widget_SetBackgroundImage( wdg_img_logo, &img_game_logo );

	Label_TextW( label_about_text, about_text );
	Label_TextW( label_member, TEXT_MEMBER );
	Button_TextW( btn_ok, TEXT_OK );
	TextBox_SetMultiline( textbox_member_list, TRUE );
	TextBox_SetReadOnly( textbox_member_list, TRUE );
	TextBox_TextW( textbox_member_list, member_list_text );
	Window_SetTitleTextW( window, TEXT_TITLE );

	Widget_SetStyleID( window, WINDOW_STYLE_LINE );
	Widget_SetPadding( Window_GetClientArea(window), Padding(10,10,10,10) );
	Widget_SetAutoSize( btn_ok, FALSE, 0 );

	Widget_SetBorder( box, Border(1,BORDER_STYLE_SOLID,RGB(200,200,200)) );
	Widget_SetPadding( box, Padding(10,10,10,10) );
	Widget_SetSize( box, "100%", "150px" );
	Widget_SetSize( textbox_member_list, "100%", "100%" );

	Widget_Resize( window, WINDOW_SIZE );
	Widget_Resize( btn_ok, BTN_SIZE );
	Widget_Resize( wdg_img_logo, LOGO_SIZE );
	Widget_Event_Connect( btn_ok, EVENT_CLICKED, btn_ok_on_clicked );

	Widget_Show( wdg_img_logo );
	Widget_Show( label_about_text );
	Widget_Show( textbox_member_list );
	Widget_Show( box );
	Widget_Show( label_member );
	Widget_Show( btn_ok );
}

void GameWindow_ShowAboutWindow(void)
{
	/* 设置为模态部件 */
	Widget_SetModal( window, TRUE );
	Widget_Show( window );
}

void GameWindow_HideAboutWindow(void)
{
	Widget_Hide( window );
}

void GameWindow_DestroyAboutWindow(void)
{
	Widget_Destroy( window );
}

