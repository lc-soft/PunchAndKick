// LifeBar：生命条，血条

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H

typedef struct LifeBarData_ {
	int current_life_point;		/**< 当前生命点 */
	int old_life_point;		/**< 上次的生命点 */
	int full_life_point;		/**< 满血条的生命点数 */
	int max_life_point;		/**< 能够拥有的最大生命点 */

	int flashbar_timer;		/**< 闪光条的定时器 */
	LCUI_BOOL need_show_flash;	/**< 指示是否需要显示闪光 */

	LCUI_Widget *lifebar;		/**< 当前生命条 */
	LCUI_Widget *flashbar[2];	/**< 扣血时显示的闪光条 */
	LCUI_Widget *oldlifebar;	/**< 扣血前显示的生命条 */
} LifeBarData;

static LCUI_RGB color_set[5]={
	{220,0,0},
	{255,130,0},
	{140,205,0},
	{10,110,210},
	{140,135,250}
};

#define TYPE_NAME	"GamePlayerLifeBar"

static void LifeBar_UpdateFlashBar( void *arg )
{
	double scale;
	int new_n, old_n;
	char scale_str[10];
	unsigned char alpha;
	LifeBarData *data;

	data = (LifeBarData*)Widget_GetPrivData((LCUI_Widget*)arg);
	if( data->need_show_flash ) {
		data->need_show_flash = FALSE;
		if( data->old_life_point < data->current_life_point ) {
			_DEBUG_MSG("tip\n");
			return;
		}
		new_n = data->current_life_point/data->full_life_point;
		old_n = data->old_life_point/data->full_life_point;
		/* 如果扣血时减少了血条数 */
		if( old_n > new_n ) {
			/* 如果扣除的血量超过1条，并且之前刚好是满槽血 */
			if( old_n - new_n > 1
			 && data->old_life_point % data->full_life_point == 0 ) {
				scale = 100;
			} else {
				 /* 计算闪光条的长度比例 */
				scale = data->old_life_point % data->full_life_point;
				scale = 100 * scale / data->full_life_point;
			}
			sprintf( scale_str, "%.2lf%%", scale );
			Widget_SetSize( data->flashbar[0], scale_str, "100%" );
			Widget_SetAlpha( data->flashbar[0], 255 );
			/* 在血条前面显示一个闪光，表示在之前那个血条里扣除的血量 */
			Widget_Show( data->flashbar[0] );

			sprintf( scale_str, "100%%", scale );
			Widget_SetSize( data->flashbar[1], scale_str, "100%" );
			Widget_SetAlpha( data->flashbar[1], 255 );
			/* 在本血条后面显示一个闪光，表示在当前血条里扣除的血量 */
			Widget_Show( data->flashbar[1] );
		} else {
			/* 否则，只在本血条后面显示一个闪光 */
			scale = data->old_life_point % data->full_life_point;
			scale = 100 * scale / data->full_life_point;
			sprintf( scale_str, "%.2lf%%", scale );
			Widget_SetSize( data->flashbar[1], scale_str, "100%" );
			Widget_SetAlpha( data->flashbar[1], 255 );
			Widget_Show( data->flashbar[1] );
		}
		return;
	}
	if( data->flashbar[0]->visible ) {
		alpha = Widget_GetAlpha( data->flashbar[0] );
		if( alpha > 10 ) {
			alpha -= 10;
			Widget_SetAlpha( data->flashbar[0], alpha );
		} else {
			alpha = 0;
			Widget_Hide( data->flashbar[0] );
		}
	}
	if( data->flashbar[1]->visible ) {
		alpha = Widget_GetAlpha( data->flashbar[1] );
		if( alpha > 10 ) {
			alpha -= 10;
			Widget_SetAlpha( data->flashbar[1], alpha );
		} else {
			alpha = 0;
			Widget_Hide( data->flashbar[1] );
		}
	}
}

static void LifeBar_ExecInit( LCUI_Widget *widget )
{
	LifeBarData *data;

	data = (LifeBarData*)Widget_NewPrivData( widget, sizeof(LifeBarData) );
	data->current_life_point = 0;
	data->full_life_point = 100;
	data->max_life_point = 200;
	data->old_life_point = 0;
	data->need_show_flash = FALSE;
	data->lifebar = Widget_New(NULL);
	data->flashbar[0] = Widget_New(NULL);
	data->flashbar[1] = Widget_New(NULL);
	data->oldlifebar = Widget_New(NULL);
	/* 加入至容器 */
	Widget_Container_Add( widget, data->lifebar );
	Widget_Container_Add( widget, data->flashbar[0] );
	Widget_Container_Add( widget, data->flashbar[1] );
	Widget_Container_Add( widget, data->oldlifebar );
	/* 调整堆叠顺序 */
	Widget_SetZIndex( data->flashbar[0], 3 );
	Widget_SetZIndex( data->lifebar, 2 );
	Widget_SetZIndex( data->flashbar[1], 1 );
	Widget_SetZIndex( data->oldlifebar, 0 );

	Widget_SetBorder( widget, Border(2,BORDER_STYLE_SOLID,RGB(150,150,150)) );
	Widget_SetPadding( widget, Padding(2,2,2,2) );
	Widget_SetAlign( data->lifebar, ALIGN_MIDDLE_LEFT, Pos(0,0) );
	Widget_SetAlign( data->oldlifebar, ALIGN_MIDDLE_LEFT, Pos(0,0) );
	Widget_SetAlign( data->flashbar[0], ALIGN_MIDDLE_LEFT, Pos(0,0) );
	Widget_SetAlign( data->flashbar[1], ALIGN_MIDDLE_LEFT, Pos(0,0) );
	Widget_SetBackgroundTransparent( data->lifebar, FALSE );
	Widget_SetBackgroundTransparent( widget, FALSE );
	Widget_SetBackgroundTransparent( data->flashbar[0], FALSE );
	Widget_SetBackgroundTransparent( data->flashbar[1], FALSE );
	Widget_SetBackgroundColor( data->flashbar[0], RGB(255,255,255) );
	Widget_SetBackgroundColor( data->flashbar[1], RGB(255,255,255) );
	Widget_SetBackgroundColor( data->lifebar, color_set[0] );
	Widget_Show( data->lifebar );
	/* 创建一个定时器，实现闪光条的淡出效果 */
	data->flashbar_timer = LCUITimer_Set( 20, LifeBar_UpdateFlashBar, (void*)widget, TRUE );
}

static void LifeBar_ExecUpdate( LCUI_Widget *widget )
{
	double scale;
	char scale_str[15];
	LifeBarData *data;

	data = (LifeBarData *)Widget_GetPrivData(widget);
	scale = data->current_life_point % data->full_life_point;
	scale = scale * 100 / data->full_life_point;
	sprintf( scale_str, "%.2lf%%", scale );
	Widget_SetSize( data->lifebar, scale_str, "100%" );
}

/** 创建一个生命条（血条） */
LCUI_Widget *LifeBar_New(void)
{
	return Widget_New(TYPE_NAME);
}

/** 为生命条设置一条生命槽能够容纳的生命值 */
void LifeBar_SetFullHP( LCUI_Widget *widget, int hp )
{
	LifeBarData *data;
	data = (LifeBarData *)Widget_GetPrivData(widget);
	data->full_life_point = hp;
	Widget_Update( widget );
}

/** 为生命条设置当前生命值 */
void LifeBar_SetHP( LCUI_Widget *widget, int hp )
{
	LifeBarData *data;
	int n_lifebar, n;
	LCUI_RGB back_color, fore_color;
	LCUI_Border border;

	data = (LifeBarData *)Widget_GetPrivData(widget);
	/* 生命值不得低于0 */
	if( hp < 0 ) {
		hp = 0;
	}
	/* 根据当前生命值，计算血条数量 */
	n_lifebar = data->current_life_point / data->full_life_point;
	/* 如果当前血条数不等于新血条数 */
	if( n_lifebar != (hp / data->full_life_point) ) {
		n_lifebar = hp / data->full_life_point;
		if( hp < data->full_life_point ) {
			/* 生命值满不了一条生命槽，改用另一种背景色 */
			back_color.red = 100;
			back_color.green = 100;
			back_color.blue = 100;
			fore_color = color_set[0];
		} else {
			/* 根据血条数，得出需要使用的颜色 */
			n = n_lifebar%5 - 1;
			/* 颜色序号小于0，则使用第4个颜色 */
			if( n<0 ) {
				n = 4;
			}
			back_color = color_set[n];
			fore_color = color_set[n_lifebar%5];
		}
		/* 上、下、左边框宽度为0 */
		border.top_width = 0;
		border.bottom_width = 0;
		border.left_width = 0;
		/* 设置右边框的宽度 */
		border.right_width = 1;
		border.right_style = BORDER_STYLE_SOLID;
		/* 设置右边框颜色 */
		border.right_color.red = fore_color.red;
		border.right_color.green = fore_color.green;
		border.right_color.blue = fore_color.blue;
		border.right_color.red += (255-fore_color.red)/2;
		border.right_color.green += (255-fore_color.green)/2;
		border.right_color.blue += (255-fore_color.blue)/2;
		/* 不使用圆角 */
		border.bottom_left_radius = 0;
		border.bottom_right_radius = 0;
		border.top_left_radius = 0;
		border.top_right_radius = 0;
		/* 设置生命槽里的血条的边框 */
		Widget_SetBorder( data->lifebar, border );
		/* 设置生命槽里的血条的背景颜色 */
		Widget_SetBackgroundColor( data->lifebar, fore_color );
		/* 生命槽的背景颜色 */
		Widget_SetBackgroundColor( widget, back_color );
		/* 生命槽需要重绘 */
		Widget_Draw( widget );
	}
	data->old_life_point = data->current_life_point;
	data->current_life_point = hp;
	/* 如果是扣血，那就记录扣血前的血量，并标记需要显示闪光 */
	if( hp < data->old_life_point ) {
		data->need_show_flash = TRUE;
	}
	/* 血条的长度需要更新 */
	Widget_Update( widget );
}

/** 为生命条设置最大生命值 */
void LifeBar_SetMaxHP( LCUI_Widget *widget, int hp )
{
	LifeBarData *data;
	data = (LifeBarData *)Widget_GetPrivData(widget);
	if( hp <= 0 ) {
		return;
	}
	data->max_life_point = hp;
	/* 更新当前血条的数据 */
	LifeBar_SetHP( widget, data->current_life_point );
}

/** 获取生命条的数量 */
int LifeBar_GetBarNum( LCUI_Widget *widget )
{
	int n;
	LifeBarData *data;

	data = (LifeBarData *)Widget_GetPrivData(widget);
	if( data->current_life_point <= data->full_life_point ) {
		n = 0;
	} else {
		n = data->current_life_point / data->full_life_point;
	}
	return n+1;
}

/** 注册生命条部件 */
void LifeBar_Regiser(void)
{
	WidgetType_Add( TYPE_NAME );
	WidgetFunc_Add( TYPE_NAME, LifeBar_ExecInit, FUNC_TYPE_INIT );
	WidgetFunc_Add( TYPE_NAME, LifeBar_ExecUpdate, FUNC_TYPE_UPDATE );
}