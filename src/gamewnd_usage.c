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

// 游戏窗口-操作方法

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_WINDOW_H
#include LC_LABEL_H
#include LC_BUTTON_H
#include LC_TEXTBOX_H

#include "gamewnd_usage.h"

#define WINDOW_SIZE	Size(350,300)
#define BTN_SIZE	Size(80,30)
#define TEXT_OK		L"确定"
#define TEXT_LICENSE	L" 操作方法 "

const wchar_t usage_text[]={
L"<size=13px>"
L"<color=#ff0000><size=14px>基本操作</size></color>\n"
L"<color=#0000ff>人物移动</color>\n"
L"通过按W、S、A、D键控制人物向上、下、左、右方向移动\n\n"
L"<color=#0000ff>普通攻击</color>\n"
L"按J、K键可进行普通攻击\n\n"
L"<color=#0000ff>跳跃</color>\n"
L"按空格键进行跳跃\n\n"
L"<color=#0000ff>防御</color>\n"
L"按L键进行防御\n\n"
L"<color=#ff0000><size=14px>特殊技能</size></color>\n"
L"<color=#0000ff>磐石防御</color>\n"
L"站立按L键\n\n"
L"<color=#0000ff>音速拳</color>\n"
L"站立按J键\n\n"
L"<color=#0000ff>音速踢</color>\n"
L"站立按K键\n\n"
L"<color=#0000ff>音速踩踏</color>\n"
L"敌人倒地后，站在敌人头部或脚部，按J键\n\n"
L"<color=#0000ff>肘压</color>\n"
L"跳起后，下落过程中按J键\n\n"
L"<color=#0000ff>断头台</color>\n"
L"跳起后，下落过程中按K键\n\n"
L"<color=#0000ff>自旋击</color>\n"
L"跳起后，落地瞬间按J键（可连续发动）\n\n"
L"<color=#0000ff>爆裂踢</color>\n"
L"跳起后，落地瞬间按K键（可连续发动）\n\n"
L"<color=#0000ff>龙卷攻击</color>\n"
L"跑动中跳起后按K键，击中敌人前按住A键或D键（选择方向）\n\n"
L"<color=#0000ff>高跳旋转落踢</color>\n"
L"跑动中跳起后按S键"
L"</size>"
};

static LCUI_Widget *window;
static LCUI_Widget *label_usage;
static LCUI_Widget *textbox_usage;
static LCUI_Widget *box;
static LCUI_Widget *btn_ok;

static void btn_ok_on_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *event )
{
	GameWindow_HideUsageWindow();
	GameWindow_DestroyUsageWindow();
}

void GameWindow_InitUsageWindow(void)
{
	window = Widget_New("window");
	label_usage = Widget_New("label");
	textbox_usage = Widget_New("text_box");
	box = Widget_New(NULL);
	btn_ok = Widget_New("button");

	Window_ClientArea_Add( window, label_usage );
	Window_ClientArea_Add( window, box );
	Window_ClientArea_Add( window, btn_ok );
	Widget_Container_Add( box, textbox_usage );
	
	Widget_SetBackgroundColor( label_usage, RGB(255,255,255) );
	Widget_SetBackgroundTransparent( label_usage, FALSE );
	
	Widget_SetAlign( label_usage, ALIGN_TOP_CENTER, Pos(0,0) );
	Widget_SetAlign( box, ALIGN_TOP_CENTER, Pos(0,8) );
	Widget_SetAlign( btn_ok, ALIGN_BOTTOM_CENTER, Pos(0,-5) );

	Label_TextW( label_usage, TEXT_LICENSE );
	Button_TextW( btn_ok, TEXT_OK );
	TextBox_SetMultiline( textbox_usage, TRUE );
	TextBox_SetAutoWrap( textbox_usage, TRUE );
	TextBox_SetReadOnly( textbox_usage, TRUE );
	TextBox_SetUsingStyleTags( textbox_usage, TRUE );
	TextBox_SetTextBlockSize( textbox_usage, 1024 );
	TextBox_TextW( textbox_usage, usage_text );
	TextBox_ShowCursor( textbox_usage, FALSE );

	Widget_SetStyleID( window, WINDOW_STYLE_LINE );
	Widget_SetPadding( Window_GetClientArea(window), Padding(10,10,10,10) );
	Widget_SetAutoSize( btn_ok, FALSE, 0 );

	Widget_SetBorder( box, Border(1,BORDER_STYLE_SOLID,RGB(200,200,200)) );
	Widget_SetPadding( box, Padding(10,10,10,10) );
	Widget_SetSize( box, "100%", "220px" );
	Widget_SetSize( textbox_usage, "100%", "100%" );

	Widget_Resize( window, WINDOW_SIZE );
	Widget_Resize( btn_ok, BTN_SIZE );

	Widget_ConnectEvent( btn_ok, EVENT_CLICKED, btn_ok_on_clicked );

	Widget_Show( textbox_usage );
	Widget_Show( box );
	Widget_Show( label_usage );
	Widget_Show( btn_ok );
}

void GameWindow_ShowUsageWindow(void)
{
	/* 设置为模态部件 */
	Widget_SetModal( window, TRUE );
	Widget_Show( window );
}

void GameWindow_HideUsageWindow(void)
{
	Widget_Hide( window );
}

void GameWindow_DestroyUsageWindow(void)
{
	Widget_Destroy( window );
}

