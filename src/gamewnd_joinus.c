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
L"    游戏中大部分BUG是已知的，要解决它们只是个时间问题，如果你在运行本游戏的过程中出现了BUG，请重新运行游戏。\n"
L"\n"
L"    此款游戏和FC游戏《热血格斗》类似，现在的想法是先模仿《热血格斗》，把应有的功能、特性及效果都实现，以后再在此基础上对游戏进行改进、扩展，添加些新特性、功能，例如：\n"
L"     ●多人联机对战\n"
L"     ●角色形象自定义\n"
L"     ●各种效果的必杀技\n"
L"     ●各种对战场景\n"
L"     ●能够使用各种武器、道具\n"
L"     ●故事模式\n"
L"     ●(等等)\n"
L"\n"
L"    上述的大部分功能都需要美工的支持，但现阶段不用考虑美工，因为游戏还未成形，其核心引擎也未达到支持这些功能的程度。以下是各个功能的大致说明。\n"
L"\n"
L"<color=#2c91bc>多人联机对战</color>\n"
L"    主要需要解决的是多个玩家在各自计算机上的数据同步问题，在联机前，需要知道每个玩家的计算机的IP地址，供联机时让每个玩家的计算机相互进行通讯，这个可以通过\"游戏房间\"来实现，房主创建房间后，该房间信息就会在局域网中广播，其它计算机上的客户端也就能够得到该房间信息，当玩家选择进入该房间后，房主的房间信息会更新。\n"
L"    在联机后，若本机的玩家控制的游戏角色改变了状态或切换了动作，则会发消息给其它计算机，让本机玩家在其他玩家计算机里的游戏数据得到更新。各种技能的效果可由各个玩家的计算机进行演算，例如：击飞效果，只需要在本机玩家控制的游戏角色被击飞时，通知其它玩家的客户端，让其它玩家的客户端在本地演算击飞效果，这样就无需让本机玩家的客户端时刻将演算出的结果发给其它玩家的客户端。\n"
L"    考虑到在不同的计算机里游戏的演算速度可能会不一样，演算结果也可能会有误差，因此需要在联机时定时将本机玩家的游戏角色的坐标、当前动作和状态等数据与其他玩家的计算机同步。\n"
L"    这个是局域网的实现方法，要与互联网上的其他玩家进行联机对战话，在房主创建房间后，可将本机的IP地址告诉其他玩家，让他们在游戏客户端中直接通过IP地址来进入游戏房间。\n"
L"    还有一种方法，开发一个游戏服务器端的程序，玩家在游戏客户端中创建房间时，可以选择将房间信息发送至服务器，让服务器端程序记录该房间信息，这样，其他已设置从该服务器中同步游戏房间列表的玩家，就能看到该玩家创建的房间。\n"
L"\n"
L"<color=#2c91bc>2v2对战模式</color>\n"
L"    将4人分成两组，让同一组的人可以使用合体技能。实现该对战模式需要合体技能的效果动画，加入动画后，需要添加合体技能发动条件，当判定满足发动条件时，显示一个指示器，用来指示发动时机，合体技能成功发动后，需要有跟踪目标的功能，如果你想体验具体效果，你可以用模拟器试玩FC版《热血格斗》。\n"
L"\n"
L"<color=#2c91bc>角色形象自定义</color>\n"
L"    这个功能估计是在遥远的未来才会实现，目前的想法是让该功能能够对角色的发型、眼睛、衣服、裤子、鞋以及装饰等进行自定义，你可以将这些能被自定义的东西称为“部件”。本版本的游戏中共有4种角色类型/职业，分别是格斗家、拳击家、功夫家、柔道家，若按照这个设定，不同职业的游戏角色有不同的动作，也就需要与该动作对应版本的“部件”，以让自定义的形象在各种动作中都能保持一致，例如：衣服，游戏角色在“站立”动作中能看见衣服，那么在“行走”动作中也必须有衣服，可别切到其它动作时就没衣服了。\n"
L"    这样搞有些复杂，需要的“部件”很多，而且“部件”版本数会随动作的增加而增加。如果让每个职业的角色形象都有一套专属“部件”，那么可能会减少“部件”的版本数，进而降低成本，就像男性角色和女性角色都有专属服装一样。\n"
L"    或者，不区分职业，各个游戏角色共用一套动作模板，这样各种游戏角色的动作及技能是通用的，仅仅是外观不同而已。\n"
L"    感觉把游戏做成3D版的应该好些，这样就不用让2D游戏美术设计师为每一帧动作去画对应版本的“部件”，只需要让3D游戏建模师做好各个“部件”的3D模型，游戏角色在使用各种动作时，直接将与该动作对应版本的“部件”实时演算出来，但问题是，如何让这些“部件”的位置、角度等参数随游戏角色的肢体运动的改变而改变？例如：手套，不管拳头怎么移动，朝什么方向移动，手套都一直是包住拳头的，不可能让手套在拳头挥出去了还保持原来的位置和角度。\n"
L"\n"
L"<color=#2c91bc>音效系统</color>\n"
L"    主要用来播放游戏中的各种音效以及背景音乐。\n"
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

