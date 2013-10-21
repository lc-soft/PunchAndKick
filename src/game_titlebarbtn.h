// 游戏主界面上的标题栏里的按钮
#ifndef __GAME_TITLEBARBTN_H__

void TitleBarBtn_Text( LCUI_Widget *widget, const char *text );

void TitleBarBtn_TextW( LCUI_Widget *widget, const wchar_t *text );

LCUI_Widget* TitleBarBtn_New(void);

void TitleBarBtn_Register(void);

#endif
