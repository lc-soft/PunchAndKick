//角色选择界面

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_WINDOW_H
#include LC_BUTTON_H
#include LC_LABEL_H
#include LC_PROGBAR_H

#include "game_control.h"
#include "game_action.h"
#include "game_object.h"
#include "skills/game_skill.h"
#include "game_role_select.h"

#define WINDOWS_SIZE		Size(320,300)
#define ROLE_IMAGE_BOX_SIZE	Size(100,120)
#define BTN_SIZE		Size(80,30)
#define INFO_AREA_SIZE		Size(295,135)
#define SKILL_AREA_SIZE		Size(295,70)

#define TEXT_NEXT_ROLE		L"下个角色"
#define TEXT_PREV_ROLE		L"上个角色"
#define TEXT_SELECT_ROLE	L"选择该角色"

static LCUI_Widget *window;
static LCUI_Widget *role_image, *role_image_box;
static LCUI_Widget *label_role_name, *label_hp_value;
static LCUI_Widget *btn_prev_role, *btn_next_role, *btn_select_role;
static LCUI_Widget *label_punch, *label_kick, *label_defense;
static LCUI_Widget *label_hp, *label_throw, *label_speed;
static LCUI_Widget *progbar_punch, *progbar_kick, *progbar_defense;
static LCUI_Widget *progbar_hp, *progbar_throw, *progbar_speed;
static LCUI_Widget *label_skilllist;
static LCUI_Widget *info_area, *skill_area;

static LCUI_Sleeper sleeper;
static LCUI_BOOL window_is_inited = FALSE;

static int current_select_role = 0;

static void closebtn_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{
	Game_HideRoleSelectBox();
	Game_DestroyRoleSelectBox();
	/* 赋值为-1，表示当前并未选中角色 */
	current_select_role = -1;
	LCUISleeper_BreakSleep( &sleeper );
}

static void GetSkillName( const char *in_skillname, wchar_t *out_skillname )
{
	if(strcmp(in_skillname, SKILLNAME_MACH_A_ATTACK) == 0) {
		wcscpy( out_skillname, L"高速拳" );
	}
	else if(strcmp(in_skillname, SKILLNAME_MACH_B_ATTACK) == 0) {
		wcscpy( out_skillname, L"高速踢" );
	}
	else if(strcmp(in_skillname, SKILLNAME_BIG_ELBOW) == 0) {
		wcscpy( out_skillname, L"肘压" );
	}
	else if(strcmp(in_skillname, SKILLNAME_GUILLOTINE) == 0) {
		wcscpy( out_skillname, L"断头台" );
	}
	else if(strcmp(in_skillname, SKILLNAME_TORNADO_ATTACK) == 0) {
		wcscpy( out_skillname, L"龙卷攻击" );
	}
	else if(strcmp(in_skillname, SKILLNAME_JUMP_SPINKICK) == 0) {
		wcscpy( out_skillname, L"高跳旋转落踢" );
	}
	else if(strcmp(in_skillname, SKILLNAME_BOMBKICK) == 0) {
		wcscpy( out_skillname, L"爆裂踢" );
	}
	else if(strcmp(in_skillname, SKILLNAME_SOLID_DEFENSE) == 0) {
		wcscpy( out_skillname, L"磐石防御" );
	}
	else if(strcmp(in_skillname, SKILLNAME_SPINHIT) == 0) {
		wcscpy( out_skillname, L"自旋击" );
	}
	else if(strcmp(in_skillname, SKILLNAME_MACH_STOMP) == 0) {
		wcscpy( out_skillname, L"自旋击" );
	} else {
		wcscpy( out_skillname, L"(未知)" );
	}
}

/** 获取已选择的角色 */
int Game_GetSelectedRole(void)
{
	if( !window_is_inited ) {
		return -1;
	}
	LCUISleeper_StartSleep( &sleeper, MAXINT32 );
	return current_select_role;
}

/** 选择角色 */
void RoleSelectBox_SetRole( int role_id )
{
	int i;
	RoleInfo *p_info;
	wchar_t str[256], tmp_str[256], skillname[32];
	
	p_info = Game_GetRoleInfo( role_id );
	Label_TextW( label_role_name, p_info->name );
	swprintf( str, sizeof(str), L"%d", p_info->property.max_hp );
	Label_TextW( label_hp_value, str );
	ProgressBar_SetValue( progbar_punch, p_info->property.punch );
	ProgressBar_SetValue( progbar_kick, p_info->property.kick );
	ProgressBar_SetValue( progbar_defense, p_info->property.defense );
	ProgressBar_SetValue( progbar_throw, p_info->property.throw );
	ProgressBar_SetValue( progbar_speed, p_info->property.speed );
	GameObject_SwitchAction( role_image, role_id );

	wcscpy( str, L"技能列表：\n" );
	for(i=0; i<p_info->total_skill; ++i) {
		GetSkillName( p_info->skills[i], skillname );
		memcpy( tmp_str, str, sizeof(str) );
		swprintf( str, sizeof(str), L"%s  %s", tmp_str, skillname );
		if(i>0 && (i+1)%4 == 0) {
			wcscat( str, L"\n" );
		}
	}
	Label_TextW( label_skilllist, str );
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
	Game_HideRoleSelectBox();
	LCUISleeper_BreakSleep( &sleeper );
	Game_DestroyRoleSelectBox();
}

/** 初始化角色选择框 */
void Game_InitRoleSelectBox(void)
{
	int i;
	ActionData *p_action;
	
	if( window_is_inited ) {
		return;
	}
	window = Widget_New("window");
	role_image_box = Widget_New(NULL);
	role_image = NULL;//GameObject_New();
	btn_prev_role = Widget_New("button");
	btn_next_role = Widget_New("button");
	btn_select_role = Widget_New("button");

	info_area = Widget_New(NULL);
	skill_area = Widget_New(NULL);
	label_hp_value = Widget_New("label");
	label_role_name = Widget_New("label");
	label_punch = Widget_New("label");
	label_kick = Widget_New("label");
	label_defense = Widget_New("label");
	label_hp = Widget_New("label");
	label_throw = Widget_New("label");
	label_speed = Widget_New("label");

	progbar_punch = Widget_New("progress_bar");
	progbar_kick = Widget_New("progress_bar");
	progbar_defense = Widget_New("progress_bar");;
	progbar_throw = Widget_New("progress_bar");
	progbar_speed = Widget_New("progress_bar");
	
	label_skilllist = Widget_New("label");
	
	Window_ClientArea_Add( window, info_area );
	Widget_Resize( info_area, INFO_AREA_SIZE );
	Widget_SetAlign( info_area, ALIGN_TOP_CENTER, Pos(0,5) );
	Widget_SetBorder( info_area, Border(1,BORDER_STYLE_SOLID, RGB(200,200,200)) );

	Widget_Container_Add( info_area, role_image_box );
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
	Widget_SetAlign( role_image_box, ALIGN_MIDDLE_LEFT, Pos(5,0) );

	/* 载入各个角色的START动作 */
	for(i=0; i<TOTAL_ROLE_NUM; ++i) {
		p_action = ActionRes_Load( i, ACTION_START );
		GameObject_AddAction( role_image, p_action, i );
	}
	current_select_role = ROLE_KUNI;
	/* 播放角色的动作动画 */
	GameObject_PlayAction( role_image );
	/* 将部件加入至角色形象框内 */
	GameObject_AddToContainer( role_image, role_image_box );
	/* 设置坐标 */
	GameObject_SetX( role_image, 45 );
	GameObject_SetY( role_image, 90 );
	Widget_Container_Add( role_image_box, label_role_name );
	Widget_SetAlign( label_role_name, ALIGN_BOTTOM_CENTER, Pos(0,-5) );

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
	
	Widget_Container_Add( info_area, label_hp );
	Widget_Container_Add( info_area, label_hp_value );
	Widget_Container_Add( info_area, label_punch );
	Widget_Container_Add( info_area, label_kick );
	Widget_Container_Add( info_area, label_defense );
	Widget_Container_Add( info_area, label_throw );
	Widget_Container_Add( info_area, label_speed );
	Widget_Container_Add( info_area, progbar_punch );
	Widget_Container_Add( info_area, progbar_kick );
	Widget_Container_Add( info_area, progbar_defense );
	Widget_Container_Add( info_area, progbar_throw );
	Widget_Container_Add( info_area, progbar_speed );
	/* 设置文本 */
	Label_TextW( label_hp, L"生命" );
	Label_TextW( label_punch, L"拳力" );
	Label_TextW( label_kick, L"腿力" );
	Label_TextW( label_defense, L"防御" );
	Label_TextW( label_throw, L"投掷" );
	Label_TextW( label_speed, L"移速" );
	Widget_SetAlign( label_hp, ALIGN_MIDDLE_LEFT, Pos(115,-54) );
	Widget_SetAlign( label_punch, ALIGN_MIDDLE_LEFT, Pos(115,-32) );
	Widget_SetAlign( label_kick, ALIGN_MIDDLE_LEFT, Pos(115,-10) );
	Widget_SetAlign( label_defense, ALIGN_MIDDLE_LEFT, Pos(115,10) );
	Widget_SetAlign( label_throw, ALIGN_MIDDLE_LEFT, Pos(115,30) );
	Widget_SetAlign( label_speed, ALIGN_MIDDLE_LEFT, Pos(115,52) );
	/* 设置进度条 */
	ProgressBar_SetMaxValue( progbar_punch, 300 );
	ProgressBar_SetMaxValue( progbar_kick, 300 );
	ProgressBar_SetMaxValue( progbar_defense, 300 );
	ProgressBar_SetMaxValue( progbar_throw, 300 );
	ProgressBar_SetMaxValue( progbar_speed, 300 );
	Widget_Resize( progbar_punch, Size(145,18) );
	Widget_Resize( progbar_kick, Size(145,18) );
	Widget_Resize( progbar_defense, Size(145,18) );
	Widget_Resize( progbar_throw, Size(145,18) );
	Widget_Resize( progbar_speed, Size(145,18) );
	Widget_SetAlign( label_hp_value, ALIGN_MIDDLE_LEFT, Pos(145,-54) );
	Widget_SetAlign( progbar_punch, ALIGN_MIDDLE_LEFT, Pos(145,-31) );
	Widget_SetAlign( progbar_kick, ALIGN_MIDDLE_LEFT, Pos(145,-10) );
	Widget_SetAlign( progbar_defense, ALIGN_MIDDLE_LEFT, Pos(145,11) );
	Widget_SetAlign( progbar_throw, ALIGN_MIDDLE_LEFT, Pos(145,32) );
	Widget_SetAlign( progbar_speed, ALIGN_MIDDLE_LEFT, Pos(145,53) );

	Window_ClientArea_Add( window, skill_area );
	Widget_Container_Add( skill_area, label_skilllist );
	Widget_Resize( skill_area, SKILL_AREA_SIZE );
	Widget_SetBorder( skill_area, Border(1,BORDER_STYLE_SOLID, RGB(200,200,200)) );
	Widget_SetAlign( skill_area, ALIGN_BOTTOM_CENTER, Pos(0,-50) );
	Widget_SetAlign( label_skilllist, ALIGN_TOP_LEFT, Pos(5,5) );

	/* 显示这些部件 */
	Widget_Show( label_role_name );
	Widget_Show( label_hp );
	Widget_Show( label_hp_value );
	Widget_Show( label_punch );
	Widget_Show( label_kick );
	Widget_Show( label_defense );
	Widget_Show( label_throw );
	Widget_Show( label_speed );
	Widget_Show( label_skilllist );
	Widget_Show( progbar_punch );
	Widget_Show( progbar_kick );
	Widget_Show( progbar_defense );
	Widget_Show( progbar_throw );
	Widget_Show( progbar_speed );
	Widget_Show( btn_next_role );
	Widget_Show( btn_prev_role );
	Widget_Show( btn_select_role );
	Widget_Show( role_image );
	Widget_Show( role_image_box );
	Widget_Show( info_area );
	Widget_Show( skill_area );

	RoleSelectBox_SetRole( ROLE_KUNI );
	LCUISleeper_Create( &sleeper );
	window_is_inited = TRUE;
}

/** 显示角色选择框 */
void Game_ShowRoleSelectBox(void)
{
	uchar_t alpha;
	
	if( !window_is_inited ) {
		return;
	}
	Widget_Show( window );
	Widget_SetModal( window, TRUE );
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
	if( !window_is_inited ) {
		return;
	}
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
	if( !window_is_inited ) {
		return;
	}
	Widget_Destroy( window );
	window_is_inited = FALSE;
}
