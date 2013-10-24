// 游戏菜单按钮
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_LABEL_H

#include "game_menubtn.h"

#define	FORE_WIDGET_TYPE_NAME	"GameMenuButtonForeWidget"

/** 游戏菜单按钮的前景部件数据 */
typedef struct ForeWidgetData_ {
	LCUI_Widget *label;
} ForeWidgetData;

/** 游戏菜单按钮数据 */
typedef struct GameMenuBtnData_ {
	LCUI_BOOL need_fill_fore;
	int timer_id;
	int fore_widget_width;
	LCUI_RGB color;
	LCUI_Widget *fore_widget;
	LCUI_Widget *label;
} GameMenuBtnData;

static void ForeWidget_ExecInit( LCUI_Widget *widget )
{
	ForeWidgetData *p_data;
	LCUI_TextStyle style;

	p_data = (ForeWidgetData*)Widget_NewPrivData( widget, sizeof(ForeWidgetData) );
	p_data->label = Widget_New("label");
	Widget_Container_Add( widget, p_data->label );
	Widget_SetAlign( p_data->label, ALIGN_MIDDLE_LEFT, Pos(35,0) );
	style = Label_GetTextStyle( p_data->label );
	TextStyle_FontColor( &style, RGB(255,255,255) );
	Label_TextStyle( p_data->label, style );
	Widget_Show( p_data->label );
}

static void ForeWidget_SetFontSize( LCUI_Widget *widget, int pixel_size )
{
	ForeWidgetData *p_data;
	LCUI_TextStyle style;

	p_data = (ForeWidgetData*)Widget_GetPrivData( widget );
	style = Label_GetTextStyle( p_data->label );
	TextStyle_FontColor( &style, RGB(255,255,255) );
	TextStyle_FontSize( &style, pixel_size );
	Label_TextStyle( p_data->label, style );
}

static void ForeWidget_SetTextW( LCUI_Widget *widget, const wchar_t *unicode_text )
{
	ForeWidgetData *p_data;
	p_data = (ForeWidgetData*)Widget_GetPrivData( widget );
	Label_TextW( p_data->label, unicode_text );
}

static void GameMenuBtn_UpdateForeWidgetWidth( void *arg )
{
	int width;
	char width_str[20];
	LCUI_Widget *widget;
	GameMenuBtnData *p_data;

	widget = (LCUI_Widget*)arg;
	p_data = (GameMenuBtnData*)Widget_GetPrivData( widget );
	if( p_data->need_fill_fore ) {
		width = Widget_GetWidth(widget);
		/* 如果已经达到预定宽度，则不再调整宽度 */
		if( p_data->fore_widget_width == width ) {
			return;
		}
		p_data->fore_widget_width += width/10;
		if( p_data->fore_widget_width > width ) {
			p_data->fore_widget_width = width;
		}
		sprintf( width_str, "%dpx", p_data->fore_widget_width );
		Widget_SetSize( p_data->fore_widget, width_str, "100%" );
		return;
	}
	if( p_data->fore_widget_width == 3 ) {
		return;
	}
	if( p_data->fore_widget_width > 3 ) {
		width = Widget_GetWidth(widget);
		p_data->fore_widget_width -= width/10;
	}
	if( p_data->fore_widget_width < 3 ) {
		p_data->fore_widget_width = 3;
	}
	sprintf( width_str, "%dpx", p_data->fore_widget_width );
	Widget_SetSize( p_data->fore_widget, width_str, "100%" );
}

static void GameMenuBtn_OnFocusEvent( LCUI_Widget *widget, LCUI_WidgetEvent *event )
{
	Widget_Update( widget );
}

static void GameMenuBtn_ExecInit( LCUI_Widget *widget )
{
	int valid_state;
	GameMenuBtnData *p_data;
	
	p_data = (GameMenuBtnData*)Widget_NewPrivData( widget, sizeof(GameMenuBtnData) );
	
	valid_state = WIDGET_STATE_ACTIVE | WIDGET_STATE_OVERLAY;
	valid_state |= WIDGET_STATE_NORMAL;

	p_data->fore_widget = Widget_New(FORE_WIDGET_TYPE_NAME);
	p_data->label = Widget_New("label");
	p_data->need_fill_fore = FALSE;
	p_data->fore_widget_width = 3;

	Widget_Container_Add( widget, p_data->label );
	Widget_Container_Add( widget, p_data->fore_widget );
	Widget_SetAlign( p_data->label, ALIGN_MIDDLE_LEFT, Pos(35,0) );
	Widget_SetAlign( p_data->fore_widget, ALIGN_MIDDLE_LEFT, Pos(0,0) );
	Widget_SetSize( p_data->fore_widget, "3px", "100%" );
	Widget_SetValidState( widget, valid_state );
	
	Widget_Show( p_data->label );
	Widget_Show( p_data->fore_widget );

	Widget_SetBackgroundColor( widget, RGB(255,255,255));
	Widget_SetBackgroundTransparent( widget, FALSE );
	Widget_SetBorder( widget, Border(1,BORDER_STYLE_SOLID,RGB(220,220,220)) );
	Widget_SetPadding( widget, Padding(1,1,1,1) );

	Widget_Event_Connect( widget, EVENT_FOCUSIN, GameMenuBtn_OnFocusEvent );
	Widget_Event_Connect( widget, EVENT_FOCUSOUT, GameMenuBtn_OnFocusEvent );
	/* 设置定时器，用于动态更新前景部件的宽度 */
	p_data->timer_id = LCUITimer_Set( 20, GameMenuBtn_UpdateForeWidgetWidth, widget, TRUE );
}

static void GameMenuBtn_ExecDraw( LCUI_Widget *widget )
{
	LCUI_Graph *graph;
	graph = Widget_GetSelfGraph( widget );
	Graph_FillAlpha( graph, 150 );
}

static void GameMenuBtn_ExecUpdate( LCUI_Widget *widget )
{
	GameMenuBtnData *p_data;
	LCUI_RGB color;
	
	p_data = (GameMenuBtnData*)Widget_GetPrivData( widget );
	switch( widget->state ) {
	case WIDGET_STATE_NORMAL:
		if( Widget_GetFocus(widget) ) {
			p_data->need_fill_fore = TRUE;
			Widget_SetBackgroundColor( p_data->fore_widget, p_data->color );
			break;
		}
		p_data->need_fill_fore = FALSE;
		Widget_SetBackgroundColor( p_data->fore_widget, p_data->color );
		break;
	case WIDGET_STATE_OVERLAY:
		p_data->need_fill_fore = TRUE;
		Widget_SetBackgroundColor( p_data->fore_widget, p_data->color );
		break;
	case WIDGET_STATE_ACTIVE:
		p_data->need_fill_fore = TRUE;
		p_data->fore_widget_width = Widget_GetWidth( widget );
		/* 让前景部件长度直接达到最大 */
		Widget_SetSize( p_data->fore_widget, "100%", "100%" );
		/* 让背景色变暗 */
		color.red = (int)(p_data->color.red * 0.7);
		color.green = (int)(p_data->color.green * 0.7);
		color.blue = (int)(p_data->color.blue * 0.7);
		Widget_SetBackgroundColor( p_data->fore_widget, color );
	default:
		break;
	}
	Widget_Draw( p_data->fore_widget );
}

void GameMenuBtn_SetFontSize( LCUI_Widget *widget, int pixel_size )
{
	GameMenuBtnData *p_data;
	LCUI_TextStyle style;

	p_data = (GameMenuBtnData*)Widget_GetPrivData( widget );
	style = Label_GetTextStyle( p_data->label );
	TextStyle_FontSize( &style, pixel_size );
	TextStyle_FontColor( &style, RGB(0,0,0) );
	Label_TextStyle( p_data->label, style );
	ForeWidget_SetFontSize( p_data->fore_widget, pixel_size );
}

void GameMenuBtn_SetTextW( LCUI_Widget *widget, const wchar_t *unicode_text )
{
	GameMenuBtnData *p_data;
	p_data = (GameMenuBtnData*)Widget_GetPrivData( widget );
	Label_TextW( p_data->label, unicode_text );
	ForeWidget_SetTextW( p_data->fore_widget, unicode_text );
}

void GameMenuBtn_SetForeWidgetColor( LCUI_Widget *widget, LCUI_RGB color )
{
	GameMenuBtnData *p_data;
	p_data = (GameMenuBtnData*)Widget_GetPrivData( widget );
	p_data->color = color;
	Widget_SetBackgroundColor( p_data->fore_widget, color );
	Widget_SetBackgroundTransparent( p_data->fore_widget, FALSE );
}

LCUI_Widget *GameMenuBtn_New(void)
{
	return Widget_New( WIDGET_TYPE_GAME_MENU_BUTTON );
}

void GameMenuBtn_Register(void)
{
	WidgetType_Add( WIDGET_TYPE_GAME_MENU_BUTTON );
	WidgetType_Add( FORE_WIDGET_TYPE_NAME );
	WidgetFunc_Add( WIDGET_TYPE_GAME_MENU_BUTTON, GameMenuBtn_ExecInit, FUNC_TYPE_INIT );
	WidgetFunc_Add( WIDGET_TYPE_GAME_MENU_BUTTON, GameMenuBtn_ExecUpdate, FUNC_TYPE_UPDATE );
	WidgetFunc_Add( WIDGET_TYPE_GAME_MENU_BUTTON, GameMenuBtn_ExecDraw, FUNC_TYPE_DRAW );
	WidgetFunc_Add( FORE_WIDGET_TYPE_NAME, ForeWidget_ExecInit, FUNC_TYPE_INIT );
}
