#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_CURSOR_H
#include LC_WIDGET_H
#include LC_LABEL_H
#include LC_INPUT_H

#include "game.h"
#include "game_logobtn.h"

#define ITEM_BOX_HEIGHT	50
#define LOGOBTN_SIZE Size(116,116)

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

/** 初始化主菜单 */
void Game_InitMainMenu(void)
{
	Game_LoadMainMenuRes();
	/* 创建所需的部件 */
	main_menu_box = Widget_New(NULL);
	main_menu_titlebar = Widget_New(NULL);
	logobtn = LogoBtn_New();
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
	Widget_SetBackgroundImage( main_menu_box, &img_res[RES_MAIN_BG] );
	Widget_SetBackgroundLayout( main_menu_box, LAYOUT_TILE );
	Widget_SetBackgroundTransparent( main_menu_box, FALSE );
	/* 主菜单框显示在普通部件的底层 */
	Widget_SetZIndex( main_menu_box, -1 );
	
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

	Widget_Show( front_wave[0] );
	Widget_Show( front_wave[1] );
	Widget_Show( back_wave[0] );
	Widget_Show( back_wave[1] );
	Widget_Show( logobtn );
	Widget_Show( main_menu_titlebar );
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
	/* 显示按键提示框 */
	//Widget_Show( keyboard_tip_box );
}

/** 隐藏主菜单 */
void Game_HideMainMenu(void)
{
	uchar_t alpha;
	/* 隐藏按键提示框 */
	//Widget_Hide( keyboard_tip_box );
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
