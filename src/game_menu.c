#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include "game_menubtn.h"

#define WIDGET_TYPE_NAME	"GameMenu"

typedef struct GameChildMenuData_ {
	LCUI_Widget *target_button;
	LCUI_Widget *menu;
} GameChildMenuData;

typedef struct GameMenuData_ {
	int button_height;
	int font_size;
	LCUI_Queue color_set;
	LCUI_Queue button_list;
	LCUI_Queue childmenu_data;
} GameMenuData;

static void GameMenu_ExecInit( LCUI_Widget *widget )
{
	GameMenuData *p_data;
	p_data = (GameMenuData*)Widget_NewPrivData( widget, sizeof(GameMenuData) );
	Queue_Init( &p_data->color_set, sizeof(LCUI_RGB), NULL );
	Queue_Init( &p_data->button_list, 0, NULL );
	Queue_Init( &p_data->childmenu_data, sizeof(GameChildMenuData), NULL );
	Queue_UsingPointer( &p_data->button_list );
	p_data->button_height = 34;
	p_data->font_size = 16;
}

static void GameMenu_ExecDestroy( LCUI_Widget *widget )
{

}

static void GameMenu_ExecUpdate( LCUI_Widget *widget )
{
	LCUI_RGB *p_color;
	GameMenuData *p_data;
	LCUI_Widget *wdg_btn;
	LCUI_Pos btn_pos;
	int btn_i, color_i, total_btn, total_color;

	p_data = (GameMenuData*)Widget_GetPrivData( widget );
	total_btn = Queue_GetTotal( &p_data->button_list );
	total_color = Queue_GetTotal( &p_data->color_set );
	color_i = 0;
	btn_pos.x = 0;
	btn_pos.y = 0;
	for(btn_i=0; btn_i<total_btn; ++btn_i) {
		wdg_btn = (LCUI_Widget*)Queue_Get( &p_data->button_list, btn_i );
		if( !wdg_btn ) {
			continue;
		}
		/* 移动按钮位置 */
		Widget_Move( wdg_btn, btn_pos );
		/* 计算下个按钮的位置 */
		btn_pos.y += p_data->button_height;
		btn_pos.y += 2;
		/* 获取本按钮的配色 */
		p_color = (LCUI_RGB*)Queue_Get( &p_data->color_set, color_i );
		if( p_color ) {
			/* 设置按钮前景部件的颜色 */
			GameMenuBtn_SetForeWidgetColor( wdg_btn, *p_color );
			++color_i;
			if( color_i >= total_color ) {
				color_i = 0;
			}
		} else {
			GameMenuBtn_SetForeWidgetColor( wdg_btn, RGB(0,0,0) );
		}
	}
}

/** 创建一个游戏菜单 */
LCUI_Widget *GameMenu_New(void)
{
	return Widget_New( WIDGET_TYPE_NAME );
}

/** 为菜单添加一个按钮 */
LCUI_Widget *GameMenu_NewButtonW( LCUI_Widget *menu, const wchar_t *unicode_text )
{
	char height_str[12];
	LCUI_Widget *btn;
	GameMenuData *p_data;
	
	btn = GameMenuBtn_New();
	if( !btn ) {
		return NULL;
	}

	p_data = (GameMenuData*)Widget_GetPrivData( menu );
	Queue_AddPointer( &p_data->button_list, btn );
	Widget_Container_Add( menu, btn );
	sprintf( height_str, "%dpx", p_data->button_height );
	Widget_SetSize( btn, "100%", height_str );
	GameMenuBtn_SetTextW( btn, unicode_text );
	GameMenuBtn_SetFontSize( btn, p_data->font_size );
	GameMenuBtn_SetForeWidgetColor( btn, RGB(0,0,0) );
	Widget_Show( btn );
	Widget_Update( menu );
	return btn;
}

/** 为菜单设置配色方案，以让菜单按钮拥有相应的颜色 */
void GameMenu_SetColorScheme(	LCUI_Widget *menu,
				LCUI_RGB *color_set, 
				int n_color )
{
	int i;
	GameMenuData *p_data;
	p_data = (GameMenuData*)Widget_GetPrivData( menu );
	for(i=0; i<n_color; ++i) {
		Queue_Add( &p_data->color_set, &color_set[i] );
	}
}

/** 为菜单中的按钮设置子菜单，当该按钮被点击后，会显示子菜单 */
int GameMenu_SetChildMenu(	LCUI_Widget *menu,
				LCUI_Widget *menu_button, 
				LCUI_Widget *child_menu )
{
	return 0;
}

void GameMenu_Register(void)
{
	WidgetType_Add( WIDGET_TYPE_NAME );
	WidgetFunc_Add( WIDGET_TYPE_NAME, GameMenu_ExecInit, FUNC_TYPE_INIT );
	WidgetFunc_Add( WIDGET_TYPE_NAME, GameMenu_ExecUpdate, FUNC_TYPE_UPDATE );
	WidgetFunc_Add( WIDGET_TYPE_NAME, GameMenu_ExecDestroy, FUNC_TYPE_DESTROY );
}
