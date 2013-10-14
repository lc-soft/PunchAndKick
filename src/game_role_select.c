#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_WINDOW_H
#include LC_BUTTON_H

#include "game_control.h"
#include "game_action.h"
#include "game_object.h"
#include "game_role_select.h"

#define WINDOWS_SIZE		Size(320,240)
#define ROLE_IMAGE_BOX_SIZE	Size(100,120)
#define BTN_SIZE		Size(80,30)

#define TEXT_NEXT_ROLE		L"下个角色"
#define TEXT_PREV_ROLE		L"上个角色"
#define TEXT_SELECT_ROLE	L"选择该角色"

static LCUI_Widget *window;
static LCUI_Widget *role_image, *role_image_box;
static LCUI_Widget *btn_prev_role, *btn_next_role, *btn_select_role;
static LCUI_Widget *role_info_text;

static int current_select_role = 0;

static void closebtn_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{
	int ret;
	ret = LCUI_MessageBoxW(	MB_ICON_WARNING, 
				L"你确定要退出游戏？", 
				L"提示", MB_BTN_YESNO );
	if( ret == MB_BTN_IS_YES ) {
		Game_HideRoleSelectBox();
		Widget_Destroy( window );
		LCUI_MainLoop_Quit(NULL);
	}
}

/** 选择角色 */
void RoleSelectBox_SetRole( int role_id )
{
	GameObject_SwitchAction( role_image, role_id );
}

static void btn_prev_role_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{
	--current_select_role;
	if( current_select_role < 0 ) {
		current_select_role = TOTAL_ROLE_NUM-1;
	}
	RoleSelectBox_SetRole( current_select_role );
}

static void btn_next_role_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{
	++current_select_role;
	if( current_select_role >= TOTAL_ROLE_NUM ) {
		current_select_role = 0;
	}
	RoleSelectBox_SetRole( current_select_role );
}

static void btn_select_role_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{

}

/** 初始化角色选择框 */
void Game_InitRoleSelectBox(void)
{
	int i;
	ActionData *p_action;

	window = Widget_New("window");
	role_image_box = Widget_New(NULL);
	role_image = GameObject_New();
	btn_prev_role = Widget_New("button");
	btn_next_role = Widget_New("button");
	btn_select_role = Widget_New("button");

	Window_ClientArea_Add( window, role_image_box );
	Widget_Resize( window, WINDOWS_SIZE );
	Window_SetTitleTextW( window, L"选择游戏角色" );
	Widget_SetAlpha( window, 0 );
	Widget_Event_Connect(	Window_GetCloseButton(window), 
				EVENT_CLICKED, closebtn_clicked );
	
	/* 配置角色形象框部件的样式 */
	Widget_Resize( role_image_box, ROLE_IMAGE_BOX_SIZE );
	Widget_SetBorder( role_image_box, Border(1,BORDER_STYLE_SOLID, RGB(200,200,200)) );
	Widget_SetBackgroundColor( role_image_box, RGB(230,230,230) );
	Widget_SetBackgroundTransparent( role_image_box, FALSE );
	Widget_SetAlign( role_image_box, ALIGN_MIDDLE_LEFT, Pos(10,-10) );

	/* 载入各个角色的START动作 */
	for(i=0; i<TOTAL_ROLE_NUM; ++i) {
		p_action = ActionRes_Load( i, ACTION_START );
		GameObject_AddAction( role_image, p_action, i );
	}
	current_select_role = ROLE_KUNI;
	/* 默认显示ROLE_KUNI角色的动作 */
	GameObject_SwitchAction( role_image, ROLE_KUNI );
	/* 播放角色的动作动画 */
	GameObject_PlayAction( role_image );
	/* 将部件加入至角色形象框内 */
	GameObject_AddToContainer( role_image, role_image_box );
	/* 设置坐标 */
	GameObject_SetX( role_image, 45 );
	GameObject_SetY( role_image, 100 );

	/* 设置按钮上显示的文本 */
	Button_TextW( btn_next_role, TEXT_NEXT_ROLE );
	Button_TextW( btn_prev_role, TEXT_PREV_ROLE );
	Button_TextW( btn_select_role, TEXT_SELECT_ROLE );
	/* 禁止自动调整尺寸 */
	Widget_SetAutoSize( btn_next_role, FALSE, 0 );
	Widget_SetAutoSize( btn_prev_role, FALSE, 0 );
	Widget_SetAutoSize( btn_select_role, FALSE, 0 );
	/* 调整按钮尺寸 */
	Widget_Resize( btn_next_role, BTN_SIZE );
	Widget_Resize( btn_prev_role, BTN_SIZE );
	Widget_Resize( btn_select_role, BTN_SIZE );
	/* 将按钮加入至窗口客户区 */
	Window_ClientArea_Add( window, btn_next_role );
	Window_ClientArea_Add( window, btn_prev_role );
	Window_ClientArea_Add( window, btn_select_role );
	/* 设置位置 */
	Widget_SetAlign( btn_prev_role, ALIGN_BOTTOM_LEFT, Pos(10,-10) );
	Widget_SetAlign( btn_select_role, ALIGN_BOTTOM_CENTER, Pos(0,-10) );
	Widget_SetAlign( btn_next_role, ALIGN_BOTTOM_RIGHT, Pos(-10,-10) );
	/* 关联按钮点击事件 */
	Widget_Event_Connect( btn_prev_role, EVENT_CLICKED, btn_prev_role_clicked );
	Widget_Event_Connect( btn_next_role, EVENT_CLICKED, btn_next_role_clicked );
	Widget_Event_Connect( btn_select_role, EVENT_CLICKED, btn_select_role_clicked );

	/* 显示这些部件 */
	Widget_Show( btn_next_role );
	Widget_Show( btn_prev_role );
	Widget_Show( btn_select_role );
	Widget_Show( role_image );
	Widget_Show( role_image_box );
}

/** 显示角色选择框 */
void Game_ShowRoleSelectBox(void)
{
	uchar_t alpha;

	Widget_Show( window );
	/* 以淡入的效果显示菜单 */
	for( alpha=0; alpha<240; alpha+=20 ) {
		Widget_SetAlpha( window, alpha );
		LCUI_MSleep(25);
	}
	Widget_SetAlpha( window, 255 );
}

/** 隐藏角色选择框 */
void Game_HideRoleSelectBox(void)
{
	uchar_t alpha;
	/* 以淡出的效果显示菜单 */
	for( alpha=240; alpha>0; alpha-=20 ) {
		Widget_SetAlpha( window, alpha );
		LCUI_MSleep(25);
	}
	Widget_SetAlpha( window, 0 );
	Widget_Hide( window );
}

void Game_DestroyRoleSelectBox(void)
{

}
