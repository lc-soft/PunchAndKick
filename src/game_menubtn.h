// 游戏菜单按钮
#ifndef __GAME_MENU_BUTTON_H__
#define __GAME_MENU_BUTTON_H__

void GameMenuBtn_SetFontSize( LCUI_Widget *widget, int pixel_size );

void GameMenuBtn_SetTextW( LCUI_Widget *widget, const wchar_t *unicode_text );

void GameMenuBtn_SetForeWidgetColor( LCUI_Widget *widget, LCUI_RGB color );

LCUI_Widget *GameMenuBtn_New(void);

void GameMenuBtn_Register(void);

#endif
