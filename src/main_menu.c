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
#include "game_menubtn.h"

#define ITEM_BOX_HEIGHT	50
#define LOGOBTN_SIZE Size(116,116)

#define TEXT_SINGLE_GAME	L"单人游戏"
#define TEXT_NETWORK_BATTLE	L"网络对战"
#define TEXT_OPTIONS		L"选项"
#define TEXT_HELP		L"帮助"
#define TEXT_QUIT		L"退出"

#define COLOR_RED	RGB(255,78,0)
#define COLOR_ORANGE	RGB(255,197,2)
#define COLOR_GREEN	RGB(76,133,2)
#define COLOR_BLUE	RGB(0,156,255)
#define COLOR_PURPLE	RGB(161,159,234)

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
static LCUI_Widget *btn_single, *btn_battle, *btn_options, *btn_help, *btn_quit;
static LCUI_Widget *logobtn;
static LCUI_Widget *keyboard_tip_box;
static LCUI_Widget *main_menu_item_box;
static LCUI_Widget *front_wave[2], *back_wave[2];
static LCUI_Widget *copyright_text;
static LCUI_Graph img_res[9];

static void UIEffect_MoveWave1( void *arg )
{
	LCUI_Pos offset_pos;
	static LCUI_Pos wave_offset_pos[2] = {
		{0,-ITEM_BOX_HEIGHT},
		{0,-ITEM_BOX_HEIGHT}
	};

	if( wave_offset_pos[0].x == 0 ) {
		wave_offset_pos[1].x = Graph_GetSize(&img_res[1]).w;
	}
	--wave_offset_pos[0].x;
	--wave_offset_pos[1].x;
	if( wave_offset_pos[0].x <= -Graph_GetSize(&img_res[1]).w ) {
		wave_offset_pos[0].x = Graph_GetSize(&img_res[1]).w;
	}
	if( wave_offset_pos[1].x <= -Graph_GetSize(&img_res[1]).w ) {
		wave_offset_pos[1].x = Graph_GetSize(&img_res[1]).w;
	}
	offset_pos.x = wave_offset_pos[0].x;
	offset_pos.y = wave_offset_pos[0].y;
	Widget_SetAlign( front_wave[0], ALIGN_BOTTOM_LEFT, offset_pos );
	offset_pos.x = wave_offset_pos[1].x;
	offset_pos.y = wave_offset_pos[1].y;
	Widget_SetAlign( front_wave[1], ALIGN_BOTTOM_LEFT, offset_pos );
}

static void UIEffect_MoveWave2( void *arg )
{
	LCUI_Pos offset_pos;
	static LCUI_Pos wave_offset_pos[2] = {
		{0,-ITEM_BOX_HEIGHT},
		{0,-ITEM_BOX_HEIGHT}
	};

	if( wave_offset_pos[0].x == 0 ) {
		wave_offset_pos[1].x = Graph_GetSize(&img_res[2]).w;
	}
	--wave_offset_pos[0].x;
	--wave_offset_pos[1].x;
	if( wave_offset_pos[0].x <= -Graph_GetSize(&img_res[2]).w ) {
		wave_offset_pos[0].x = Graph_GetSize(&img_res[2]).w;
	}
	if( wave_offset_pos[1].x <= -Graph_GetSize(&img_res[2]).w ) {
		wave_offset_pos[1].x = Graph_GetSize(&img_res[2]).w;
	}
	offset_pos.x = wave_offset_pos[0].x;
	offset_pos.y = wave_offset_pos[0].y;
	Widget_SetAlign( back_wave[0], ALIGN_BOTTOM_LEFT, offset_pos );
	offset_pos.x = wave_offset_pos[1].x;
	offset_pos.y = wave_offset_pos[1].y;
	Widget_SetAlign( back_wave[1], ALIGN_BOTTOM_LEFT, offset_pos );
}

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

void Game_InitMenuButton(void)
{
	btn_single = GameMenuBtn_New();
	btn_battle = GameMenuBtn_New();
	btn_options = GameMenuBtn_New();
	btn_help = GameMenuBtn_New();
	btn_quit = GameMenuBtn_New();

	Widget_Container_Add( main_menu_box, btn_single );
	Widget_Container_Add( main_menu_box, btn_battle );
	Widget_Container_Add( main_menu_box, btn_options );
	Widget_Container_Add( main_menu_box, btn_help );
	Widget_Container_Add( main_menu_box, btn_quit );

	Widget_SetAlign( btn_single, ALIGN_MIDDLE_LEFT, Pos(50,-70) );
	Widget_SetAlign( btn_battle, ALIGN_MIDDLE_LEFT, Pos(50,-35) );
	Widget_SetAlign( btn_options, ALIGN_MIDDLE_LEFT, Pos(50,0) );
	Widget_SetAlign( btn_help, ALIGN_MIDDLE_LEFT, Pos(50,35) );
	Widget_SetAlign( btn_quit, ALIGN_MIDDLE_LEFT, Pos(50,70) );

	Widget_Resize( btn_single, Size(150,34) );
	Widget_Resize( btn_battle, Size(150,34) );
	Widget_Resize( btn_options, Size(150,34) );
	Widget_Resize( btn_help, Size(150,34) );
	Widget_Resize( btn_quit, Size(150,34) );

	GameMenuBtn_SetTextW( btn_single, TEXT_SINGLE_GAME );
	GameMenuBtn_SetTextW( btn_battle, TEXT_NETWORK_BATTLE );
	GameMenuBtn_SetTextW( btn_options, TEXT_OPTIONS );
	GameMenuBtn_SetTextW( btn_help, TEXT_HELP );
	GameMenuBtn_SetTextW( btn_quit, TEXT_QUIT );

	GameMenuBtn_SetFontSize( btn_single, 16 );
	GameMenuBtn_SetFontSize( btn_battle, 16 );
	GameMenuBtn_SetFontSize( btn_options, 16 );
	GameMenuBtn_SetFontSize( btn_help, 16 );
	GameMenuBtn_SetFontSize( btn_quit, 16 );

	GameMenuBtn_SetForeWidgetColor( btn_single, COLOR_RED );
	GameMenuBtn_SetForeWidgetColor( btn_battle, COLOR_ORANGE );
	GameMenuBtn_SetForeWidgetColor( btn_options, COLOR_GREEN );
	GameMenuBtn_SetForeWidgetColor( btn_help, COLOR_BLUE );
	GameMenuBtn_SetForeWidgetColor( btn_quit, COLOR_PURPLE );
	
	Widget_Show( btn_single );
	Widget_Show( btn_battle );
	Widget_Show( btn_options );
	Widget_Show( btn_help );
	Widget_Show( btn_quit );
}

/** 初始化主菜单 */
void Game_InitMainMenu(void)
{
	Game_LoadMainMenuRes();
	/* 创建所需的部件 */
	main_menu_box = Widget_New(NULL);

	main_menu_item_box = Widget_New(NULL);
	keyboard_tip_box = Widget_New(NULL);
	front_wave[0] = Widget_New(NULL);
	front_wave[1] = Widget_New(NULL);
	back_wave[0] = Widget_New(NULL);
	back_wave[1] = Widget_New(NULL);
	copyright_text = Widget_New("label");

	Widget_SetSize( main_menu_box, "100%", "100%" );
	Widget_SetAlign( main_menu_box, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	Widget_SetAlpha( main_menu_box, 0 );
	Widget_SetBackgroundColor( main_menu_box, RGB(240,240,240) );
	Widget_SetBackgroundTransparent( main_menu_box, FALSE );
	/* 主菜单框显示在普通部件的底层 */
	Widget_SetZIndex( main_menu_box, -1 );
	Widget_Container_Add( main_menu_box, main_menu_item_box );
	Widget_SetHeight( main_menu_item_box, ITEM_BOX_HEIGHT );
	Widget_SetSize( main_menu_item_box, "100%", NULL );
	Widget_SetAlign( main_menu_item_box, ALIGN_BOTTOM_CENTER, Pos(0,0) );
	Widget_SetBackgroundColor( main_menu_item_box, RGB(255,255,255) );
	Widget_SetBackgroundTransparent ( main_menu_item_box, FALSE );

	Widget_Container_Add( main_menu_box, front_wave[0] );
	Widget_Container_Add( main_menu_box, front_wave[1] );
	Widget_Resize( front_wave[0], Graph_GetSize(&img_res[RES_FRONT_WAVE]) );
	Widget_Resize( front_wave[1], Graph_GetSize(&img_res[RES_FRONT_WAVE]) );
	Widget_SetBackgroundImage( front_wave[0], &img_res[RES_FRONT_WAVE] );
	Widget_SetBackgroundImage( front_wave[1], &img_res[RES_FRONT_WAVE] );
	Widget_SetBackgroundLayout( front_wave[0], LAYOUT_CENTER );
	Widget_SetBackgroundLayout( front_wave[1], LAYOUT_CENTER );
	Widget_SetAlign( front_wave[0], ALIGN_BOTTOM_LEFT, Pos(0,-ITEM_BOX_HEIGHT) );
	Widget_SetAlign( front_wave[1], ALIGN_BOTTOM_LEFT, Pos(Graph_GetSize(&img_res[RES_FRONT_WAVE]).w,-ITEM_BOX_HEIGHT) );
	
	Widget_Container_Add( main_menu_box, back_wave[0] );
	Widget_Container_Add( main_menu_box, back_wave[1] );
	Widget_Resize( back_wave[0], Graph_GetSize(&img_res[RES_BACK_WAVE]) );
	Widget_Resize( back_wave[1], Graph_GetSize(&img_res[RES_BACK_WAVE]) );
	Widget_SetBackgroundImage( back_wave[0], &img_res[RES_BACK_WAVE] );
	Widget_SetBackgroundImage( back_wave[1], &img_res[RES_BACK_WAVE] );
	Widget_SetBackgroundLayout( back_wave[0], LAYOUT_CENTER );
	Widget_SetBackgroundLayout( back_wave[1], LAYOUT_CENTER );
	Widget_SetAlign( back_wave[0], ALIGN_BOTTOM_LEFT, Pos(0,-ITEM_BOX_HEIGHT) );
	Widget_SetAlign( back_wave[1], ALIGN_BOTTOM_LEFT, Pos(Graph_GetSize(&img_res[RES_BACK_WAVE]).w,-ITEM_BOX_HEIGHT) );
	
	Widget_Container_Add( main_menu_box, copyright_text );
	Label_TextW( copyright_text, L"<size=16px>Powered By LCUI</size>" );
	Widget_SetAlign( copyright_text, ALIGN_BOTTOM_CENTER, Pos(0,-30) );
	
	Widget_SetBackgroundImage( keyboard_tip_box, &img_res[RES_KEY_TIP] );
	Widget_Resize( keyboard_tip_box, Graph_GetSize(&img_res[RES_KEY_TIP]) );
	Widget_SetAlign( keyboard_tip_box, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	Widget_Event_Connect( keyboard_tip_box, EVENT_DRAG, keyboard_tip_box_on_drag );
	Widget_Event_Connect( keyboard_tip_box, EVENT_CLICKED, keyboard_tip_box_on_clicked );
	
	//Game_InitMenuBar();
	Game_InitMenuButton();

	Widget_Show( front_wave[0] );
	Widget_Show( front_wave[1] );
	Widget_Show( back_wave[0] );
	Widget_Show( back_wave[1] );
	Widget_Show( main_menu_item_box );
	Widget_Show( copyright_text );
	Widget_Show( main_menu_box );
	/* 设置两个定时器，用于定时改变波浪的位置，以实现波浪的移动效果 */
	LCUITimer_Set( 20, UIEffect_MoveWave1, NULL, TRUE );
	LCUITimer_Set( 60, UIEffect_MoveWave2, NULL, TRUE );
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
	/* 隐藏按键提示框 */
	Widget_Hide( keyboard_tip_box );
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
