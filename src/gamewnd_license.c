// 游戏窗口-许可协议
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_WINDOW_H
#include LC_LABEL_H
#include LC_BUTTON_H
#include LC_TEXTBOX_H

#include "gamewnd_license.h"

#define WINDOW_SIZE	Size(350,360)
#define BTN_SIZE	Size(80,30)
#define TEXT_OK		L"确定"
#define TEXT_LICENSE	L" 许可协议 "

static const wchar_t license_text[]={
L"<size=13px>"
L"本游戏是自由软件，并且只可以根据 GNU通用公共许可协议 来使用、更改和发布，该许可协议要求本游戏及游戏衍生软件无论以何种形式发布，都必须确保收受者能收到或得到源代码。\n\n"
L"本游戏是基于技术交流目的而加以散布的，但不负任何担保责任，例如：稳定性、可玩性、趣味性等的担保，甚至没有适销性或特定用途的隐含担保。\n\n"
L"关于 GNU通用公共许可协议 的详细内容，请访问：<color=#0000ff>http://www.gnu.org/licenses/</color>\n"
L"\n自由软件是什么？\n"
L"自由软件（Free Software）是权利问题，不是价格问题。\n\n"
L"要理解这个概念，自由应该是“言论自由”中的“自由”，而不是“免费啤酒”中的“免费”。（因为英文中的自由和免费是同一个字：free）\n\n"
L"自由软件关乎使用者运行、复制、发布、研究、修改和改进该软件的自由。更精确地说，自由软件赋予软件使用者四种自由：\n"
L" (0) 不论目的为何，有运行该软件的自由（自由之零）。\n"
L" (1) 有研究该软件如何运行，以及按需改写该软件的自由（自由之一）。取得该软件源代码为达成此目的之前提。\n"
L" (2) 有重新发布拷贝的自由，这样你可以借此来敦亲睦邻（自由之二）。\n"
L" (3) 有改进该软件，以及向公众发布改进的自由，这样整个社群都可受惠（自由之三）。取得该软件源码为达成此目的之前提。\n"
L"</size>"
};

static LCUI_Widget *window;
static LCUI_Widget *label_license;
static LCUI_Widget *textbox_license;
static LCUI_Widget *box;
static LCUI_Widget *btn_ok;

static void btn_ok_on_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *event )
{
	GameWindow_HideLicenseWindow();
	GameWindow_DestroyLicenseWindow();
}

void GameWindow_InitLicenseWindow(void)
{
	window = Widget_New("window");
	label_license = Widget_New("label");
	textbox_license = Widget_New("text_box");
	box = Widget_New(NULL);
	btn_ok = Widget_New("button");

	Window_ClientArea_Add( window, label_license );
	Window_ClientArea_Add( window, box );
	Window_ClientArea_Add( window, btn_ok );
	Widget_Container_Add( box, textbox_license );
	
	Widget_SetBackgroundColor( label_license, RGB(255,255,255) );
	Widget_SetBackgroundTransparent( label_license, FALSE );
	
	Widget_SetAlign( label_license, ALIGN_TOP_CENTER, Pos(0,0) );
	Widget_SetAlign( box, ALIGN_TOP_CENTER, Pos(0,8) );
	Widget_SetAlign( btn_ok, ALIGN_BOTTOM_CENTER, Pos(0,-5) );

	Label_TextW( label_license, TEXT_LICENSE );
	Button_TextW( btn_ok, TEXT_OK );
	TextBox_SetAutoWrap( textbox_license, TRUE );
	TextBox_SetMultiline( textbox_license, TRUE );
	TextBox_SetReadOnly( textbox_license, TRUE );
	TextBox_SetUsingStyleTags( textbox_license, TRUE );
	TextBox_TextW( textbox_license, license_text );
	TextBox_ShowCursor( textbox_license, FALSE );

	Widget_SetStyleID( window, WINDOW_STYLE_LINE );
	Widget_SetPadding( Window_GetClientArea(window), Padding(10,10,10,10) );
	Widget_SetAutoSize( btn_ok, FALSE, 0 );

	Widget_SetBorder( box, Border(1,BORDER_STYLE_SOLID,RGB(200,200,200)) );
	Widget_SetPadding( box, Padding(10,10,10,10) );
	Widget_SetSize( box, "100%", "280px" );
	Widget_SetSize( textbox_license, "100%", "100%" );

	Widget_Resize( window, WINDOW_SIZE );
	Widget_Resize( btn_ok, BTN_SIZE );

	Widget_ConnectEvent( btn_ok, EVENT_CLICKED, btn_ok_on_clicked );

	Widget_Show( textbox_license );
	Widget_Show( box );
	Widget_Show( label_license );
	Widget_Show( btn_ok );
}

void GameWindow_ShowLicenseWindow(void)
{
	/* 设置为模态部件 */
	Widget_SetModal( window, TRUE );
	Widget_Show( window );
}

void GameWindow_HideLicenseWindow(void)
{
	Widget_Hide( window );
}

void GameWindow_DestroyLicenseWindow(void)
{
	Widget_Destroy( window );
}

