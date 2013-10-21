#ifndef __GAME_LOGO_BUTTON__
#define __GAME_LOGO_BUTTON__

LCUI_Widget* LogoBtn_New(void);

void LogoBtn_SetStyle(	LCUI_Widget *widget, 
			LCUI_Graph* logo, 
			LCUI_Graph *bg_normal, 
			LCUI_Graph *bg_hover,
			LCUI_Graph *bg_pressed );

void LogoBtn_Register(void);

#endif
