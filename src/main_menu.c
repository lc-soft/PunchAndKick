#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_CURSOR_H
#include LC_WIDGET_H
#include LC_LABEL_H
#include LC_INPUT_H
#include LC_DISPLAY_H

#include "game.h"
#include "game_role_select.h"
#include "game_menubtn.h"
#include "game_menu.h"
#include "game_config.h"
#include "gamewnd_usage.h"
#include "gamewnd_about.h"
#include "gamewnd_joinus.h"
#include "gamewnd_license.h"

#define ITEM_BOX_HEIGHT	50
#define LOGOBTN_SIZE Size(116,116)

#define TEXT_SINGLE_GAME	L"单人游戏"
#define TEXT_NETWORK_BATTLE	L"网络对战"
#define TEXT_OPTIONS		L"选项"
#define TEXT_HELP		L"帮助"
#define TEXT_QUIT		L"退出"

#define TEXT_2V2_GAME		L"2v2对战"
#define TEXT_SCUFFLE_GAME	L"4人混战"

#define TEXT_SET_KEYBOARD	L"键位设置"
#define TEXT_WINDOWED		L"窗口模式"
#define TEXT_ON			L"开"
#define TEXT_OFF		L"关"

#define TEXT_USAGE		L"操作方法"
#define TEXT_ABOUT		L"关于本游戏"
#define TEXT_LICENSE		L"许可协议"
#define TEXT_JOINUS		L"加入我们"

#define TEXT_QUIT_TIP_TEXT	L"你确定要退出游戏？"
#define TEXT_QUIT_TIP_TITLE	L"提示"

#define SIZE_MAIN_MENU_BOX	Size(150,180)
#define SIZE_GAME_MENU_BOX	Size(150,75)
#define SIZE_HELP_MENU_BOX	Size(150,145)
#define SIZE_OPTIONS_MENU_BOX	Size(150,75)

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
static LCUI_Widget *main_menu_box, *game_menu_box, *help_menu_box, *options_menu_box;
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

void Game_LoadMainUIRes(void)
{
	GameGraphRes_GetGraph( MAIN_RES, "main-menu-bg", &img_res[RES_MAIN_BG] );
	GameGraphRes_GetGraph( MAIN_RES, "front-wave-img", &img_res[RES_FRONT_WAVE] );
	GameGraphRes_GetGraph( MAIN_RES, "back-wave-img", &img_res[RES_BACK_WAVE] );
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

/** 在退出按钮被点击时 */
static void btn_quit_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{
	int ret;
	ret = LCUI_MessageBoxW( MB_ICON_WARNING, TEXT_QUIT_TIP_TEXT, 
				TEXT_QUIT_TIP_TITLE, MB_BTN_YESNO );
	if( ret == MB_BTN_IS_YES ) {
		LCUI_MainLoop_Quit(NULL);
	}
}

static void btn_usage_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{
	GameWindow_InitUsageWindow();
	GameWindow_ShowUsageWindow();
}

static void btn_about_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{
	GameWindow_InitAboutWindow();
	GameWindow_ShowAboutWindow();
}

static void btn_license_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{
	GameWindow_InitLicenseWindow();
	GameWindow_ShowLicenseWindow();
}

static void btn_joinus_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{
	GameWindow_InitJoinUsWindow();
	GameWindow_ShowJoinUsWindow();
}

static void btn_2v2_mode_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{
	LCUI_MessageBoxW( MB_ICON_WARNING, L"此对战模式未加入。", L"提示", MB_BTN_OK );
}

static void btn_network_battle_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{
	LCUI_MessageBoxW( MB_ICON_WARNING, 
	L"此功能暂未开发，若你有一定的windows和GNU/Linux环境下的网络编\n"
	L"程经验，并且有时间有兴趣，可以协助我们为本游戏开发此功能。"
	, L"提示", MB_BTN_OK );
}

static void scuffle_game_btn_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{
	Game_InitRoleSelectBox();
	Game_ShowRoleSelectBox();
}

static void btn_switch_video_mode_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{
	int mode;
	wchar_t str_buff[32];
	if( GameConfig_IsWindowed() ) {
		GameConfig_SetWindowed(FALSE);
		GameConfig_Save();
		mode = LCUI_INIT_MODE_FULLSCREEN;
		wcscpy( str_buff, TEXT_WINDOWED L": " TEXT_OFF );
	} else {
		GameConfig_SetWindowed(TRUE);
		GameConfig_Save();
		mode = LCUI_INIT_MODE_WINDOW;
		wcscpy( str_buff, TEXT_WINDOWED L": " TEXT_ON );
	}

	/* 如果该部件类型不是菜单按钮，则查找是菜单按钮类型的父级部件 */
	if( 0 != _LCUIString_Cmp(&widget->type_name, WIDGET_TYPE_GAME_MENU_BUTTON ) ) {
		widget = Widget_GetParent( widget, WIDGET_TYPE_GAME_MENU_BUTTON );
	}
	if( widget ) {
		GameMenuBtn_SetTextW( widget, str_buff );
	}
	LCUIScreen_SetMode( GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, mode );
}

/** 初始化主菜单 */
static void Game_InitMainMenu(void)
{
	wchar_t str_buff[32];
	LCUI_Widget	*btn_single_game,
			*btn_help,
			*btn_network_battle, 
			*btn_options, 
			*btn_quit,
			*btn_2v2_mode, 
			*btn_scuffle_mode,
			*btn_switch_video_mode, 
			*btn_usage, 
			*btn_about, 
			*btn_license, 
			*btn_joinus;

	/* 创建几个游戏菜单 */
	main_menu_box = GameMenu_New();
	game_menu_box = GameMenu_New();
	help_menu_box = GameMenu_New();
	options_menu_box = GameMenu_New();
	/* 将这些菜单放入主界面框里 */
	Widget_Container_Add( main_ui_box, main_menu_box );
	Widget_Container_Add( main_ui_box, game_menu_box );
	Widget_Container_Add( main_ui_box, help_menu_box );
	Widget_Container_Add( main_ui_box, options_menu_box );
	/* 设置主菜单的位置 */
	Widget_SetAlign( main_menu_box, ALIGN_MIDDLE_LEFT, Pos(50,0) );
	/* 为这些菜单设置配色方案 */
	GameMenu_SetColorScheme( main_menu_box, color_set, TOTAL_COLOR_NUM );
	GameMenu_SetColorScheme( game_menu_box, color_set, TOTAL_COLOR_NUM );
	GameMenu_SetColorScheme( help_menu_box, color_set, TOTAL_COLOR_NUM );
	GameMenu_SetColorScheme( options_menu_box, color_set, TOTAL_COLOR_NUM );
	/* 为菜单创建按钮 */
	btn_single_game = GameMenu_NewButtonW( main_menu_box, TEXT_SINGLE_GAME );
	btn_network_battle = GameMenu_NewButtonW( main_menu_box, TEXT_NETWORK_BATTLE );

	btn_2v2_mode = GameMenu_NewButtonW( game_menu_box, TEXT_2V2_GAME );
	btn_scuffle_mode = GameMenu_NewButtonW( game_menu_box, TEXT_SCUFFLE_GAME );

	btn_options = GameMenu_NewButtonW( main_menu_box, TEXT_OPTIONS );
	btn_help = GameMenu_NewButtonW( main_menu_box, TEXT_HELP );
	btn_quit = GameMenu_NewButtonW( main_menu_box, TEXT_QUIT );

	btn_usage = GameMenu_NewButtonW( help_menu_box, TEXT_USAGE );
	btn_about = GameMenu_NewButtonW( help_menu_box, TEXT_ABOUT );
	btn_license = GameMenu_NewButtonW( help_menu_box, TEXT_LICENSE );
	btn_joinus = GameMenu_NewButtonW( help_menu_box, TEXT_JOINUS );

	GameMenu_NewButtonW( options_menu_box, TEXT_SET_KEYBOARD );
	/* 根据游戏的配置数据，判断是否启用窗口模式，并让按钮上显示相应文字 */
	if( GameConfig_IsWindowed() ) {
		wcscpy( str_buff, TEXT_WINDOWED L": " TEXT_ON );
	} else {
		wcscpy( str_buff, TEXT_WINDOWED L": " TEXT_OFF );
	}
	btn_switch_video_mode = GameMenu_NewButtonW( options_menu_box, str_buff );
	/* 为菜单按钮关联CLICKED事件，以在按钮被点击时进行相应 */
	Widget_Event_Connect( btn_2v2_mode, EVENT_CLICKED, btn_2v2_mode_clicked );
	Widget_Event_Connect( btn_scuffle_mode, EVENT_CLICKED, scuffle_game_btn_clicked );
	Widget_Event_Connect( btn_network_battle, EVENT_CLICKED, btn_network_battle_clicked );
	Widget_Event_Connect( btn_switch_video_mode, EVENT_CLICKED, btn_switch_video_mode_clicked );
	Widget_Event_Connect( btn_quit, EVENT_CLICKED, btn_quit_clicked );
	Widget_Event_Connect( btn_usage, EVENT_CLICKED, btn_usage_clicked );
	Widget_Event_Connect( btn_about, EVENT_CLICKED, btn_about_clicked );
	Widget_Event_Connect( btn_license, EVENT_CLICKED, btn_license_clicked );
	Widget_Event_Connect( btn_joinus, EVENT_CLICKED, btn_joinus_clicked );
	/* 设置一些子菜单 */
	GameMenu_SetChildMenu( main_menu_box, btn_single_game, game_menu_box );
	GameMenu_SetChildMenu( main_menu_box, btn_help, help_menu_box );
	GameMenu_SetChildMenu( main_menu_box, btn_options, options_menu_box );
	/* 调整各个菜单的尺寸 */
	Widget_Resize( main_menu_box, SIZE_MAIN_MENU_BOX );
	Widget_Resize( game_menu_box, SIZE_GAME_MENU_BOX );
	Widget_Resize( help_menu_box, SIZE_HELP_MENU_BOX );
	Widget_Resize( options_menu_box, SIZE_OPTIONS_MENU_BOX );
}

static void Game_ShowMainMenu(void)
{
	Widget_Show( main_menu_box );
}

/** 初始化主界面 */
void Game_InitMainUI(void)
{
	Game_LoadMainUIRes();
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
	
	Game_InitMainMenu();
	Game_ShowMainMenu();

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
void Game_ShowMainUI(void)
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
void Game_HideMainUI(void)
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
