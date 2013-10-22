#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_CURSOR_H
#include LC_WIDGET_H
#include LC_LABEL_H
#include LC_INPUT_H

#include "game.h"
#include "game_logobtn.h"
#include "game_titlebarbtn.h"
#include "game_role_select.h"

#define ITEM_BOX_HEIGHT	50
#define LOGOBTN_SIZE Size(116,116)

#define TEXT_SINGLE_GAME	L"<size=18px>单人游戏</size>"
#define TEXT_NETWORK_BATTLE	L"<size=18px>网络对战</size>"
#define TEXT_OPTIONS		L"<size=18px>选项</size>"
#define TEXT_HELP		L"<size=18px>帮助</size>"

enum RES_ID {
	RES_MAIN_BG,
	RES_FRONT_WAVE,
	RES_BACK_WAVE,
	RES_KEY_TIP,
	RES_TITEBAR_BG,
	RES_LOGO_NORMAL,
	RES_LOGOBTN_NORMAL,
	RES_LOGOBTN_HOVER,
	RES_LOGOBTN_PRESSED
};

static LCUI_Widget *main_menu_box;
static LCUI_Widget *main_menu_titlebar;
static LCUI_Widget *btn_single, *btn_battle, *btn_options, *btn_help;
static LCUI_Widget *logobtn;
static LCUI_Widget *keyboard_tip_box;
static LCUI_Widget *main_menu_item_box;
static LCUI_Widget *front_wave[2], *back_wave[2];
static LCUI_Widget *copyright_text;
static LCUI_Graph img_res[9];

void Game_LoadMainMenuRes(void)
{
	GameGraphRes_GetGraph( MAIN_RES, "main-menu-bg", &img_res[RES_MAIN_BG] );
	GameGraphRes_GetGraph( MAIN_RES, "front-wave-img", &img_res[RES_FRONT_WAVE] );
	GameGraphRes_GetGraph( MAIN_RES, "back-wave-img", &img_res[RES_BACK_WAVE] );
	GameGraphRes_GetGraph( MAIN_RES, "keyboard-control-method", &img_res[RES_KEY_TIP] );
	GameGraphRes_GetGraph( MAIN_RES, "main-menu-titlebar", &img_res[RES_TITEBAR_BG] );
	GameGraphRes_GetGraph( MAIN_RES, "logo-btn-img", &img_res[RES_LOGO_NORMAL] );
	GameGraphRes_GetGraph( MAIN_RES, "logo-btn-hover", &img_res[RES_LOGOBTN_HOVER] );
	GameGraphRes_GetGraph( MAIN_RES, "logo-btn-normal", &img_res[RES_LOGOBTN_NORMAL] );
	GameGraphRes_GetGraph( MAIN_RES, "logo-btn-pressed", &img_res[RES_LOGOBTN_PRESSED] );
}

static void keyboard_tip_box_on_drag( LCUI_Widget *widget, LCUI_WidgetEvent *event )
{
	/* 在开始拖动时 */
	if( event->drag.first_click ) {
		Widget_SetAlign( widget, ALIGN_NONE, Pos(0,0) );
		Widget_Front( widget );		/* 前置部件显示 */
		Widget_Refresh( widget );	/* 刷新该部件区域的图形 */
	}
	if( event->drag.new_pos.x < 0 ) {
		event->drag.new_pos.x = 0;
	}
	if( event->drag.new_pos.x + widget->size.w > GAME_SCREEN_WIDTH ) {
		event->drag.new_pos.x = GAME_SCREEN_WIDTH - widget->size.w;
	}
	if( event->drag.new_pos.y < 0 ) {
		event->drag.new_pos.y = 0;
	}
	if( event->drag.new_pos.y + widget->size.h > GAME_SCREEN_HEIGHT ) {
		event->drag.new_pos.y = GAME_SCREEN_HEIGHT - widget->size.h;
	}
	Widget_Move( widget, event->drag.new_pos );
}

static void keyboard_tip_box_on_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *event )
{
	Widget_Front( widget );
	Widget_Refresh( widget );
}

static void btn_single_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{
	Game_InitRoleSelectBox();	/* 初始化角色选择框 */
	Game_ShowRoleSelectBox();	/* 显示角色选择框 */
}

static void Game_InitMenuBar(void)
{
	logobtn = LogoBtn_New();
	btn_single = TitleBarBtn_New();
	btn_help = TitleBarBtn_New();
	btn_battle = TitleBarBtn_New();
	btn_options = TitleBarBtn_New();
	main_menu_titlebar = Widget_New(NULL);
	/* 配置主菜单栏 */
	Widget_Container_Add( main_menu_box, main_menu_titlebar );
	Widget_SetBackgroundImage( main_menu_titlebar, &img_res[RES_TITEBAR_BG] );
	Widget_Resize( main_menu_titlebar, Graph_GetSize(&img_res[RES_TITEBAR_BG]) );
	Widget_SetAlign( main_menu_titlebar, ALIGN_TOP_LEFT, Pos(0,30) );
	/* 配置LOGO按钮 */
	Widget_Container_Add( main_menu_titlebar, logobtn );
	Widget_Resize( logobtn, LOGOBTN_SIZE );
	Widget_SetAlign( logobtn, ALIGN_MIDDLE_LEFT, Pos(10,0) );
	LogoBtn_SetStyle( logobtn, &img_res[RES_LOGO_NORMAL], &img_res[RES_LOGOBTN_NORMAL],
				&img_res[RES_LOGOBTN_HOVER], &img_res[RES_LOGOBTN_PRESSED] );
	/* 配置菜单栏上的按钮 */
	Widget_Container_Add( main_menu_titlebar, btn_single );
	Widget_Container_Add( main_menu_titlebar, btn_battle );
	Widget_Container_Add( main_menu_titlebar, btn_options );
	Widget_Container_Add( main_menu_titlebar, btn_help );
	
	Widget_SetAlign( btn_single, ALIGN_MIDDLE_LEFT, Pos(130,-2) );
	Widget_SetAlign( btn_battle, ALIGN_MIDDLE_LEFT, Pos(130+100,-2) );
	Widget_SetAlign( btn_options, ALIGN_MIDDLE_LEFT, Pos(130+200,-2) );
	Widget_SetAlign( btn_help, ALIGN_MIDDLE_LEFT, Pos(130+300,-2) );
	
	TitleBarBtn_TextW( btn_single, TEXT_SINGLE_GAME );
	TitleBarBtn_TextW( btn_battle, TEXT_NETWORK_BATTLE );
	TitleBarBtn_TextW( btn_options, TEXT_OPTIONS );
	TitleBarBtn_TextW( btn_help, TEXT_HELP );
	
	Widget_Resize( btn_single, Size(100,65) );
	Widget_Resize( btn_battle, Size(100,65) );
	Widget_Resize( btn_options, Size(100,65) );
	Widget_Resize( btn_help, Size(100,65) );

	Widget_Event_Connect( btn_single, EVENT_CLICKED, btn_single_clicked );

	Widget_Show( logobtn );
	Widget_Show( btn_single );
	Widget_Show( btn_battle );
	Widget_Show( btn_options );
	Widget_Show( btn_help );
	Widget_Show( main_menu_titlebar );
}

/** 初始化主菜单 */
void Game_InitMainMenu(void)
{
	Game_LoadMainMenuRes();
	/* 创建所需的部件 */
	main_menu_box = Widget_New(NULL);
	copyright_text = Widget_New("label");

	Widget_SetSize( main_menu_box, "100%", "100%" );
	Widget_SetAlign( main_menu_box, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	Widget_SetAlpha( main_menu_box, 0 );
	Widget_SetBackgroundColor( main_menu_box, RGB(240,240,240));
	Widget_SetBackgroundTransparent( main_menu_box, FALSE );
	/* 主菜单框显示在普通部件的底层 */
	Widget_SetZIndex( main_menu_box, -1 );

	Widget_Container_Add( main_menu_box, copyright_text );
	Label_TextW( copyright_text, L"<size=16px>Powered By LCUI</size>" );
	Widget_SetAlign( copyright_text, ALIGN_BOTTOM_CENTER, Pos(0,-30) );
	
	Widget_Show( copyright_text );
	Widget_Show( main_menu_box );
}

/** 显示主菜单 */
void Game_ShowMainMenu(void)
{
	uchar_t alpha;

	Widget_Show( main_menu_box );
	/* 以淡入的效果显示菜单 */
	for( alpha=0; alpha<240; alpha+=20 ) {
		Widget_SetAlpha( main_menu_box, alpha );
		LCUI_MSleep(25);
	}
	Widget_SetAlpha( main_menu_box, 255 );
}

LCUI_Widget* Game_GetMainMenuBox(void)
{
	return main_menu_box;
}

/** 隐藏主菜单 */
void Game_HideMainMenu(void)
{
	uchar_t alpha;
	/* 以淡出的效果显示菜单 */
	for( alpha=240; alpha>0; alpha-=20 ) {
		Widget_SetAlpha( main_menu_box, alpha );
		LCUI_MSleep(25);
	}
	Widget_SetAlpha( main_menu_box, 0 );
	Widget_Hide( main_menu_box );
}

/** 销毁主菜单 */
void Game_DestroyMainMenu(void)
{

}
