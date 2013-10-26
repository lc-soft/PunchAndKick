#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_WINDOW_H

#define WINDOW_SIZE		Size(400,400)
#define TEXT_WINDOW_TILE	L"许可协议"

static LCUI_Widget *window;
static LCUI_Widget *license_textbox;

void GameWindow_InitLicenseWindow(void)
{
	window = Widget_New("window");
	license_textbox = Widget_New("text_box");

	Window_ClientArea_Add( window, license_textbox );
	Widget_Resize( window, WINDOW_SIZE );
	Window_SetTitleTextW( window, TEXT_WINDOW_TILE );
	Widget_SetDock( license_textbox, DOCK_TYPE_FILL );
	Widget_Show( license_textbox );
}

void GameWindow_ShowLicenseWindow(void)
{
	Widget_SetModal( window, TRUE );
	Widget_Show( window );
}

void GameWindow_HideLicenseWindow(void)
{

}

void GameWindow_DestroyLicenseWindow(void)
{

}
