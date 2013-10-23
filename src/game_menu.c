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
	LCUI_Queue button_list;
	LCUI_Queue childmenu_data;
} GameMenuData;

static void GameMenu_ExecInit( LCUI_Widget *widget )
{

}

static void GameMenu_ExecDestroy( LCUI_Widget *widget )
{

}

/** 创建一个游戏菜单 */
LCUI_Widget *GameMenu_New(void)
{
	return NULL;
}

/** 为菜单添加一个按钮 */
LCUI_Widget *GameMenu_NewButtonW( const wchar_t *unicode_text )
{
	return NULL;
}

/** 为菜单设置配色方案，以让菜单按钮拥有相应的颜色 */
void GameMenu_SetColorScheme(	LCUI_Widget *menu,
				LCUI_RGB *color_set, 
				int n_color )
{

}

/** 为菜单中的按钮设置子菜单，当该按钮被点击后，会显示子菜单 */
int GameMenu_SetChildMenu( LCUI_Widget *menu_button, LCUI_Widget *menu )
{
	return 0;
}

void GameMenu_Register(void)
{
	WidgetType_Add( WIDGET_TYPE_NAME );
	WidgetFunc_Add( WIDGET_TYPE_NAME, GameMenu_ExecInit, FUNC_TYPE_INIT );
	WidgetFunc_Add( WIDGET_TYPE_NAME, GameMenu_ExecDestroy, FUNC_TYPE_DESTROY );
}
