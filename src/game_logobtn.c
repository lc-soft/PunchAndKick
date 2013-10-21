// 主菜单栏上的LOGO按钮
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H

#include "game_logobtn.h"

#define WIDGET_TYPE_NAME	"GameLogoButton"

typedef struct LogoButtonData_ {
	LCUI_Widget *wdg_logo;
	LCUI_Graph *bg_normal, *bg_pressed, *bg_hover;
	LCUI_Graph *logo;
} LogoButtonData;

#define WIDGET_NEWPRIVEDATA(widget,data_type) (data_type*)Widget_NewPrivData(widget,sizeof(data_type))

static void LogoBtn_ExecInit( LCUI_Widget *widget )
{
	LogoButtonData *data;
	data = WIDGET_NEWPRIVEDATA(widget, LogoButtonData);
	data->bg_normal = NULL;
	data->bg_pressed = NULL;
	data->bg_hover = NULL;
	data->logo = NULL;
	data->wdg_logo = Widget_New(NULL);
	Widget_Container_Add( widget, data->wdg_logo );
	Widget_SetAlign( data->wdg_logo, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	Widget_Show( data->wdg_logo );
	Widget_SetValidState( widget, WIDGET_STATE_OVERLAY | WIDGET_STATE_ACTIVE | WIDGET_STATE_NORMAL );
	/* 设置部件只有不透明的图形区域上才能响应鼠标点击 */
	Widget_SetClickableAlpha( widget, 255, 0 );
}

static void LogoBtn_ExecUpdate( LCUI_Widget *widget )
{
	LogoButtonData *data;

	data = (LogoButtonData*)Widget_GetPrivData(widget);
	switch(widget->state) {
	case WIDGET_STATE_OVERLAY:
		Widget_SetBackgroundImage( widget, data->bg_hover );
		Widget_SetBackgroundImage( data->wdg_logo, data->logo );
		Widget_SetAlign( data->wdg_logo, ALIGN_MIDDLE_CENTER, Pos(0,0) );
		break;
	case WIDGET_STATE_NORMAL:
		Widget_SetBackgroundImage( widget, data->bg_normal );
		Widget_SetBackgroundImage( data->wdg_logo, data->logo );
		Widget_SetBackgroundLayout( widget, LAYOUT_CENTER );
		Widget_SetAlign( data->wdg_logo, ALIGN_MIDDLE_CENTER, Pos(0,0) );
		break;
	case WIDGET_STATE_ACTIVE:
		Widget_SetBackgroundImage( widget, data->bg_pressed );
		Widget_SetBackgroundImage( data->wdg_logo, data->logo );
		Widget_SetAlign( data->wdg_logo, ALIGN_MIDDLE_CENTER, Pos(1,1) );
		break;
	default:break;
	}
}

LCUI_Widget* LogoBtn_New(void)
{
	return Widget_New(WIDGET_TYPE_NAME);
}

void LogoBtn_SetStyle(	LCUI_Widget *widget, 
			LCUI_Graph* logo,
			LCUI_Graph *bg_normal, 
			LCUI_Graph *bg_hover,
			LCUI_Graph *bg_pressed )
{
	LCUI_Size size={0,0};
	LogoButtonData *data;

	data = (LogoButtonData*)Widget_GetPrivData(widget);
	data->bg_normal = bg_normal;
	data->bg_pressed = bg_pressed;
	data->bg_hover = bg_hover;
	data->logo = logo;
	Widget_Resize( data->wdg_logo, Graph_GetSize(logo) );
}

void LogoBtn_Register(void)
{
	WidgetType_Add( WIDGET_TYPE_NAME );
	WidgetFunc_Add( WIDGET_TYPE_NAME, LogoBtn_ExecInit, FUNC_TYPE_INIT );
	WidgetFunc_Add( WIDGET_TYPE_NAME, LogoBtn_ExecUpdate, FUNC_TYPE_UPDATE );
}