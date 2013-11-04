// 游戏窗口-加入我们
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_WINDOW_H
#include LC_LABEL_H
#include LC_BUTTON_H
#include LC_TEXTBOX_H

#include "game_resource.h"
#include "gamewnd_joinus.h"

#define WINDOW_SIZE	Size(500,350)
#define BTN_SIZE	Size(80,30)
#define BOX_SIZE	Size(300,270)
#define RQCODE_SIZE	Size(162,162)

#define TEXT_OK		L"确定"
#define TEXT_LICENSE	L" 相关说明 "

static const wchar_t contact_us_text[]={
	L"有什么问题可以通过官网留言、电子邮件与我们进行交流，如果你实在是需要进QQ群里交流，请扫描上面的二维码。"
	L"官网：<color=#00f>lcui.org</color>\n"
	L"邮箱：<color=#00f>lc-soft@live.cn</color>"
};


static const wchar_t directions_text[]={
L"<size=13px>"
L"    本游戏是一个开源项目，项目源代码托管在GitHub上：<color=#00f>https://github.com/lc-soft/LCUI-2DFightGame</color>\n\n"
L"    如果你有一定的windows或GNU/Linux环境下的编程经验，并对本游戏感兴趣，欢迎参与本游戏的开发。\n\n"
L"    游戏中大部分BUG是已知的，要解决它们只是个时间问题，如果你在运行本游戏的过程中出现了BUG，请重新运行游戏。\n\n"
L"    计划为本游戏添加2v2对战模式和网络对战功能，以下是大致的实现方案。\n\n"
L"2v2对战模式：\n"
L"    将4人分成两组，让同一组的人可以使用合体技能。实现该对战模式需要合体技能的效果动画，加入动画后，需要添加合体技能发动条件，当判定满足发动条件时，显示一个指示器，用来指示发动时机，合体技能成功发动后，需要有跟踪目标的功能，若想体验具体效果，你可以用模拟器试玩FC版《热血格斗》。\n\n"
L"网络对战功能：\n"
L"    让多个玩家在不同的计算机上进行同一场对战，多个玩家控制各自的游戏角色，每个玩家的控制操作都能同步至其它玩家的计算机上。\n"
L"    进行网络对战前需要让每个客户端得到其它客户端的IP地址，以让每个客户端相互进行通讯，这个可以通过\"游戏房间\"来实现，房主创建房间后，该房间信息就会在局域网中广播，其它计算机上的客户端也就能够得到该房间信息，当玩家选择进入该房间后，房主的房间信息会更新，而当人数满4人后，房主可以确定开始游戏。\n\n"
L"    还有个较为独立的功能模块：音效系统，主要用来播放游戏中的各种音效以及背景音乐，开发该功能模块不需要对本游戏内部原理有多少了解。\n"
L"</size>"
};

static LCUI_Widget *window;
static LCUI_Widget *label_title;
static LCUI_Widget *textbox;
static LCUI_Widget *box;
static LCUI_Widget *btn_ok, *wdg_qrcode, *label_contact_us;

static void btn_ok_on_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *event )
{
	GameWindow_HideJoinUsWindow();
	GameWindow_DestroyJoinUsWindow();
}

void GameWindow_InitJoinUsWindow(void)
{
	LCUI_Graph img_qrcode;

	window = Widget_New("window");
	label_title = Widget_New("label");
	label_contact_us = Widget_New("label");
	textbox = Widget_New("text_box");
	box = Widget_New(NULL);
	btn_ok = Widget_New("button");
	wdg_qrcode = Widget_New(NULL);

	Graph_Init( &img_qrcode );
	GameGraphRes_GetGraph( MAIN_RES, "qrcode", &img_qrcode );

	Window_ClientArea_Add( window, label_title );
	Window_ClientArea_Add( window, label_contact_us );
	Window_ClientArea_Add( window, box );
	Window_ClientArea_Add( window, btn_ok );
	Window_ClientArea_Add( window, wdg_qrcode );
	Widget_Container_Add( box, textbox );
	
	Widget_SetBackgroundColor( label_title, RGB(255,255,255) );
	Widget_SetBackgroundTransparent( label_title, FALSE );
	Widget_SetBackgroundImage( wdg_qrcode, &img_qrcode );
	Widget_SetBackgroundTransparent( wdg_qrcode, FALSE );
	
	Widget_SetAlign( label_title, ALIGN_TOP_LEFT, Pos(120,0) );
	Widget_SetAlign( wdg_qrcode, ALIGN_TOP_RIGHT, Pos(-5,10) );
	Widget_SetAlign( label_contact_us, ALIGN_TOP_RIGHT, Pos(-5,185) );
	Widget_SetAlign( box, ALIGN_TOP_LEFT, Pos(0,8) );
	Widget_SetAlign( btn_ok, ALIGN_BOTTOM_CENTER, Pos(0,-5) );

	Label_TextW( label_title, TEXT_LICENSE );
	Label_TextW( label_contact_us, contact_us_text );
	Button_TextW( btn_ok, TEXT_OK );
	TextBox_SetAutoWrap( textbox, TRUE );
	TextBox_SetMultiline( textbox, TRUE );
	TextBox_SetReadOnly( textbox, TRUE );
	TextBox_SetUsingStyleTags( textbox, TRUE );
	TextBox_TextW( textbox, directions_text );
	TextBox_ShowCursor( textbox, FALSE );
	
	Label_AutoSize( label_contact_us, FALSE, 0 );
	Label_SetAutoWrap( label_contact_us, TRUE );
	Widget_Resize( label_contact_us, Size(162,100) );

	Widget_SetStyleID( window, WINDOW_STYLE_LINE );
	Widget_SetPadding( Window_GetClientArea(window), Padding(10,10,10,10) );
	Widget_SetAutoSize( btn_ok, FALSE, 0 );

	Widget_SetBorder( box, Border(1,BORDER_STYLE_SOLID,RGB(200,200,200)) );
	Widget_SetPadding( box, Padding(10,10,10,10) );
	Widget_Resize( box, BOX_SIZE );
	Widget_SetSize( textbox, "100%", "100%" );

	Widget_Resize( window, WINDOW_SIZE );
	Widget_Resize( btn_ok, BTN_SIZE );
	Widget_Resize( wdg_qrcode, RQCODE_SIZE );

	Widget_Event_Connect( btn_ok, EVENT_CLICKED, btn_ok_on_clicked );

	Widget_Show( textbox );
	Widget_Show( box );
	Widget_Show( label_title );
	Widget_Show( label_contact_us );
	Widget_Show( wdg_qrcode );
	Widget_Show( btn_ok );
}

void GameWindow_ShowJoinUsWindow(void)
{
	/* 设置为模态部件 */
	Widget_SetModal( window, TRUE );
	Widget_Show( window );
}

void GameWindow_HideJoinUsWindow(void)
{
	Widget_Hide( window );
}

void GameWindow_DestroyJoinUsWindow(void)
{
	Widget_Destroy( window );
}

