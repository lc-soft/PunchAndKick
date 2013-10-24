#ifndef __GAME_MENU_H__
#define __GAME_MENU_H__

#define WIDGET_TYPE_GAME_MENU	"GameMenu"

/** 创建一个游戏菜单 */
LCUI_Widget *GameMenu_New(void);

/** 为菜单添加一个按钮 */
LCUI_Widget *GameMenu_NewButtonW( LCUI_Widget *menu, const wchar_t *unicode_text );

/** 为菜单设置配色方案，以让菜单按钮拥有相应的颜色 */
void GameMenu_SetColorScheme(	LCUI_Widget *menu,
				LCUI_RGB *color_set, 
				int n_color );

/** 为菜单中的按钮设置子菜单，当该按钮被点击后，会显示子菜单 */
int GameMenu_SetChildMenu(	LCUI_Widget *menu,
				LCUI_Widget *menu_button, 
				LCUI_Widget *child_menu );

void GameMenu_Register(void);

#endif
