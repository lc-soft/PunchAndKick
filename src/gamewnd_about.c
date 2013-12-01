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

#define TOP_SPACE	85

#define LOGOBTN_SIZE	Size(116,116)
#define WINDOW_SIZE	Size(350,300+TOP_SPACE)
#define BTN_SIZE	Size(80,30)
#define LOGO_SIZE	Size(64,64)

#define TEXT_OK		L"确定"
#define TEXT_TITLE	L"关于本游戏"
#define TEXT_MEMBER	L" 制作人员 "

const wchar_t member_list_text[]={
	L"<size=13px>"
	L"<color=#ff4e00>主策划</color>\n"
	L"        刘超\n\n"
	L"<color=#ff4e00>系统设计</color>\n"
	L"        刘超\n\n"
	L"<color=#ff4e00>数值设计</color>\n"
	L"        刘超\n\n"
	L"<color=#ff4e00>UI设计</color>\n"
	L"        刘超\n\n"
	L"<color=#ff4e00>AI设计</color>\n"
	L"        刘超\n\n"
	L"<color=#ff4e00>主程序</color>\n"
	L"        刘超\n\n"
	L"</size>"
};

const wchar_t about_text[]={
	L"Punch & Kick\n"
	L"版本 1.1.1\n"
	L"版权所有 (C) 2013 LC-Games 开发组。\n"
	L"保留所有权利。"
};

const wchar_t about_support_text[]={
	L"本游戏由 LCUI 驱动，主要为游戏提供图形界面、图形处理、键盘驱动等的支持，文本渲染功能由 FreeType 字体引擎提供支持。\n"
	L"游戏中应用的人物素材资源来自FC格斗游戏《热血格斗》，素材著作权属于TECHNōS JAPAN公司。"
};

enum RES_ID {
	RES_LOGO_NORMAL,
	RES_LOGOBTN_NORMAL,
	RES_LOGOBTN_HOVER,
	RES_LOGOBTN_PRESSED
};

static LCUI_Widget *window;
static LCUI_Widget *wdg_img_logo, *label_about_text;
static LCUI_Widget *label_about_support_text;
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
	label_about_support_text = Widget_New("label");
	label_member = Widget_New("label");
	textbox_member_list = Widget_New("text_box");
	wdg_img_logo = Widget_New(NULL);
	box = Widget_New(NULL);
	btn_ok = Widget_New("button");

	Graph_Init( &img_game_logo );
	GameGraphRes_GetGraph( MAIN_RES, "game-logo", &img_game_logo );

	Window_ClientArea_Add( window, label_about_text );
	Window_ClientArea_Add( window, label_about_support_text );
	Window_ClientArea_Add( window, label_member );
	Window_ClientArea_Add( window, wdg_img_logo );
	Window_ClientArea_Add( window, box );
	Window_ClientArea_Add( window, btn_ok );
	Widget_Container_Add( box, textbox_member_list );
	
	Widget_SetBackgroundColor( label_member, RGB(255,255,255) );
	Widget_SetBackgroundTransparent( label_member, FALSE );
	
	Widget_SetAlign( wdg_img_logo, ALIGN_TOP_LEFT, Pos(0,0) );
	Widget_SetAlign( label_about_text, ALIGN_TOP_LEFT, Pos(76,0) );
	Widget_SetAlign( label_about_support_text, ALIGN_TOP_LEFT, Pos(0,70) );
	Widget_SetAlign( label_member, ALIGN_TOP_CENTER, Pos(0,72+TOP_SPACE) );
	Widget_SetAlign( box, ALIGN_TOP_CENTER, Pos(0,80+TOP_SPACE) );
	Widget_SetAlign( btn_ok, ALIGN_BOTTOM_CENTER, Pos(0,-5) );

	Widget_SetBackgroundImage( wdg_img_logo, &img_game_logo );
	Label_SetAutoWrap( label_about_support_text, TRUE );
	Label_TextW( label_about_text, about_text );
	Label_TextW( label_about_support_text, about_support_text );
	Label_TextW( label_member, TEXT_MEMBER );
	Button_TextW( btn_ok, TEXT_OK );
	TextBox_SetMultiline( textbox_member_list, TRUE );
	TextBox_SetReadOnly( textbox_member_list, TRUE );
	TextBox_SetUsingStyleTags( textbox_member_list, TRUE );
	TextBox_TextW( textbox_member_list, member_list_text );
	Window_SetTitleTextW( window, TEXT_TITLE );
	TextBox_ShowCursor( textbox_member_list, FALSE );

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
	Widget_ConnectEvent( btn_ok, EVENT_CLICKED, btn_ok_on_clicked );

	Widget_Show( wdg_img_logo );
	Widget_Show( label_about_text );
	Widget_Show( label_about_support_text );
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

