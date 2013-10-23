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
#include "game_menu.h"

#define ITEM_BOX_HEIGHT	50
#define LOGOBTN_SIZE Size(116,116)

#define TEXT_SINGLE_GAME	L"单人游戏"
#define TEXT_NETWORK_BATTLE	L"网络对战"
#define TEXT_OPTIONS		L"选项"
#define TEXT_HELP		L"帮助"
#define TEXT_QUIT		L"退出"

#define TEXT_USAGE		L"操作方法"
#define TEXT_ABOUT		L"关于本游戏"
#define TEXT_LICENSE		L"许可协议"

#define SIZE_MAIN_MENU_BOX	Size(150,174)
#define SIZE_HELP_MENU_BOX	Size(150,110)

#define COLOR_RED	{255,78,0}
#define COLOR_ORANGE	{255,197,2}
#define COLOR_GREEN	{76,133,2}
#define COLOR_BLUE	{0,156,255}
#define COLOR_PURPLE	{161,159,234}

#define TOTAL_COLOR_NUM	5

static LCUI_RGB color_set[TOTAL_COLOR_NUM]={ 
	COLOR_RED, COLOR_ORANGE, 
	COLOR_GREEN, COLOR_BLUE, 
	COLOR_PURPLE
};

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

static LCUI_Widget *main_ui_box;
static LCUI_Widget *main_menu_box, *help_menu_box;
static LCUI_Widget *btn_single, *btn_battle, *btn_options, *btn_help, *btn_quit;
static LCUI_Widget *btn_usage, *btn_about, *btn_license;
static LCUI_Widget *footer_box;
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

static void Game_ShowHelpMenuButton(void)
{
	Widget_Show( help_menu_box );
}

/** 初始化主菜单按钮 */
void Game_InitMenuButton(void)
{
	main_menu_box = GameMenu_New();
	
	Widget_SetAlign( main_menu_box, ALIGN_MIDDLE_LEFT, Pos(50,0) );
	GameMenu_SetColorScheme( main_menu_box, color_set, TOTAL_COLOR_NUM );
	GameMenu_NewButtonW( main_menu_box, TEXT_SINGLE_GAME );
	GameMenu_NewButtonW( main_menu_box, TEXT_NETWORK_BATTLE );
	GameMenu_NewButtonW( main_menu_box, TEXT_OPTIONS );
	GameMenu_NewButtonW( main_menu_box, TEXT_HELP );
	GameMenu_NewButtonW( main_menu_box, TEXT_QUIT );
	Widget_Resize( main_menu_box, SIZE_MAIN_MENU_BOX );
}

static void Game_ShowMenuButton(void)
{
	Widget_Show( main_menu_box );
}

/** 初始化主菜单 */
void Game_InitMainMenu(void)
{
	Game_LoadMainMenuRes();
	/* 创建所需的部件 */
	main_ui_box = Widget_New(NULL);

	footer_box = Widget_New(NULL);
	front_wave[0] = Widget_New(NULL);
	front_wave[1] = Widget_New(NULL);
	back_wave[0] = Widget_New(NULL);
	back_wave[1] = Widget_New(NULL);
	copyright_text = Widget_New("label");

	Widget_SetSize( main_ui_box, "100%", "100%" );
	Widget_SetAlign( main_ui_box, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	Widget_SetAlpha( main_ui_box, 0 );
	Widget_SetBackgroundColor( main_ui_box, RGB(240,240,240) );
	Widget_SetBackgroundTransparent( main_ui_box, FALSE );
	
	/* 主界面显示在普通部件的底层 */
	Widget_SetZIndex( main_ui_box, -1 );
	Widget_Container_Add( main_ui_box, footer_box );
	Widget_SetHeight( footer_box, ITEM_BOX_HEIGHT );
	Widget_SetSize( footer_box, "100%", NULL );
	Widget_SetAlign( footer_box, ALIGN_BOTTOM_CENTER, Pos(0,0) );
	Widget_SetBackgroundColor( footer_box, RGB(255,255,255) );
	Widget_SetBackgroundTransparent ( footer_box, FALSE );

	Widget_Container_Add( main_ui_box, front_wave[0] );
	Widget_Container_Add( main_ui_box, front_wave[1] );
	Widget_Resize( front_wave[0], Graph_GetSize(&img_res[RES_FRONT_WAVE]) );
	Widget_Resize( front_wave[1], Graph_GetSize(&img_res[RES_FRONT_WAVE]) );
	Widget_SetBackgroundImage( front_wave[0], &img_res[RES_FRONT_WAVE] );
	Widget_SetBackgroundImage( front_wave[1], &img_res[RES_FRONT_WAVE] );
	Widget_SetBackgroundLayout( front_wave[0], LAYOUT_CENTER );
	Widget_SetBackgroundLayout( front_wave[1], LAYOUT_CENTER );
	Widget_SetAlign( front_wave[0], ALIGN_BOTTOM_LEFT, Pos(0,-ITEM_BOX_HEIGHT) );
	Widget_SetAlign( front_wave[1], ALIGN_BOTTOM_LEFT, Pos(Graph_GetSize(&img_res[RES_FRONT_WAVE]).w,-ITEM_BOX_HEIGHT) );
	
	Widget_Container_Add( main_ui_box, back_wave[0] );
	Widget_Container_Add( main_ui_box, back_wave[1] );
	Widget_Resize( back_wave[0], Graph_GetSize(&img_res[RES_BACK_WAVE]) );
	Widget_Resize( back_wave[1], Graph_GetSize(&img_res[RES_BACK_WAVE]) );
	Widget_SetBackgroundImage( back_wave[0], &img_res[RES_BACK_WAVE] );
	Widget_SetBackgroundImage( back_wave[1], &img_res[RES_BACK_WAVE] );
	Widget_SetBackgroundLayout( back_wave[0], LAYOUT_CENTER );
	Widget_SetBackgroundLayout( back_wave[1], LAYOUT_CENTER );
	Widget_SetAlign( back_wave[0], ALIGN_BOTTOM_LEFT, Pos(0,-ITEM_BOX_HEIGHT) );
	Widget_SetAlign( back_wave[1], ALIGN_BOTTOM_LEFT, Pos(Graph_GetSize(&img_res[RES_BACK_WAVE]).w,-ITEM_BOX_HEIGHT) );
	
	Widget_Container_Add( main_ui_box, copyright_text );
	Label_TextW( copyright_text, L"<size=16px>Powered By LCUI</size>" );
	Widget_SetAlign( copyright_text, ALIGN_BOTTOM_CENTER, Pos(0,-30) );
	
	Game_InitMenuButton();
	Game_ShowMenuButton();

	Widget_Show( front_wave[0] );
	Widget_Show( front_wave[1] );
	Widget_Show( back_wave[0] );
	Widget_Show( back_wave[1] );
	Widget_Show( footer_box );
	Widget_Show( copyright_text );
	Widget_Show( main_ui_box );
	/* 设置两个定时器，用于定时改变波浪的位置，以实现波浪的移动效果 */
	LCUITimer_Set( 20, UIEffect_MoveWave1, NULL, TRUE );
	LCUITimer_Set( 60, UIEffect_MoveWave2, NULL, TRUE );
}

/** 显示主菜单 */
void Game_ShowMainMenu(void)
{
	uchar_t alpha;

	Widget_Show( main_ui_box );
	/* 以淡入的效果显示菜单 */
	for( alpha=0; alpha<240; alpha+=20 ) {
		Widget_SetAlpha( main_ui_box, alpha );
		LCUI_MSleep(25);
	}
	Widget_SetAlpha( main_ui_box, 255 );
}

LCUI_Widget* Game_GetMainMenuBox(void)
{
	return main_ui_box;
}

/** 隐藏主菜单 */
void Game_HideMainMenu(void)
{
	uchar_t alpha;
	/* 以淡出的效果显示菜单 */
	for( alpha=240; alpha>0; alpha-=20 ) {
		Widget_SetAlpha( main_ui_box, alpha );
		LCUI_MSleep(25);
	}
	Widget_SetAlpha( main_ui_box, 0 );
	Widget_Hide( main_ui_box );
}

/** 销毁主菜单 */
void Game_DestroyMainMenu(void)
{

}
