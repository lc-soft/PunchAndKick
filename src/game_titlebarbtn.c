// 游戏主界面上的标题栏里的按钮

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_LABEL_H
#include LC_TEXTSTYLE_H

#define WIDGET_TYPE_NAME	"TitleBarButton"

typedef struct TitleBarTitleBarBtn_ {
	LCUI_Widget *label;
} TitleBarButton;

static void TitleBarBtn_ExecUpdate( LCUI_Widget *widget )
{
	LCUI_TextStyle style;
	TitleBarButton *btn;
	
	btn = (TitleBarButton*)Widget_GetPrivData(widget);
	style = Label_GetTextStyle( btn->label );
	switch( widget->state ) {
	case WIDGET_STATE_NORMAL:
		Widget_SetBackgroundColor( widget, RGB(255,255,255) );
		Widget_SetBorder( widget, Border(0, BORDER_STYLE_SOLID, RGB(172,172,172)) );
		/* 设置文字缺省颜色为黑色 */
		TextStyle_FontColor( &style, RGB(0,0,0) );
		Label_TextStyle( btn->label, style );
		break;
	case WIDGET_STATE_OVERLAY :
		Widget_SetBackgroundColor( widget, RGB(120,200,230) );
		Widget_SetBorder( widget, Border(1, BORDER_STYLE_SOLID, RGB(120,190,230)) );
		/* 设置文字缺省颜色为白色 */
		TextStyle_FontColor( &style, RGB(255,255,255) );
		Label_TextStyle( btn->label, style );
		break;
	case WIDGET_STATE_ACTIVE :
		Widget_SetBackgroundColor( widget, RGB(37,153,207) );
		Widget_SetBorder( widget, Border(1, BORDER_STYLE_SOLID, RGB(50,170,220)) );
		TextStyle_FontColor( &style, RGB(255,255,255) );
		Label_TextStyle( btn->label, style );
		break;
		default : break;
	}
	Widget_SetBackgroundTransparent( widget, FALSE );
	Widget_Refresh( widget );
}

static void TitleBarBtn_ExecInit( LCUI_Widget *widget )
{
	int valid_state;
	TitleBarButton *button;
	
	button = (TitleBarButton*)Widget_NewPrivData(widget, sizeof(TitleBarButton));
	
	valid_state = (WIDGET_STATE_NORMAL | WIDGET_STATE_ACTIVE);
	valid_state |= (WIDGET_STATE_DISABLE | WIDGET_STATE_OVERLAY);
	Widget_SetValidState( widget, valid_state );
	button->label = Widget_New("label");
	Widget_Container_Add( widget, button->label );
	Widget_SetAlign(button->label, ALIGN_MIDDLE_CENTER, Pos(0,0));
	Widget_Show( button->label );
}

void TitleBarBtn_Text( LCUI_Widget *widget, const char *text )
{
	TitleBarButton *button;
	button = (TitleBarButton*)Widget_GetPrivData(widget);
	Label_Text( button->label, text );
}

void TitleBarBtn_TextW( LCUI_Widget *widget, const wchar_t *text )
{
	TitleBarButton *button;
	button = (TitleBarButton*)Widget_GetPrivData(widget);
	Label_TextW( button->label, text );
}

LCUI_Widget* TitleBarBtn_New(void)
{
	return Widget_New(WIDGET_TYPE_NAME);
}

void TitleBarBtn_Register(void)
{
	WidgetType_Add(WIDGET_TYPE_NAME);
	WidgetFunc_Add(WIDGET_TYPE_NAME, TitleBarBtn_ExecInit, FUNC_TYPE_INIT);
	WidgetFunc_Add(WIDGET_TYPE_NAME, TitleBarBtn_ExecUpdate, FUNC_TYPE_UPDATE);
}
