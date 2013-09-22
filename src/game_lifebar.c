// LifeBar：生命条，血条

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H

#define SHADOW_N_WAIT	30

#define SHADOW_STATE_CLEAN		1
#define SHADOW_STATE_END_CLEAN		0
#define SHADOW_STATE_NEED_CLEAN		2

typedef struct LifeBarData_ {
	int current_life_point;		/**< 当前生命点 */
	int old_life_point;		/**< 扣血前的生命点 */
	int full_life_point;		/**< 满血条的生命点数 */
	int max_life_point;		/**< 能够拥有的最大生命点 */
	int shadow_life_point;		/**< 血条残影表示的血量 */
	int timer_flashbar;		/**< 闪光条的定时器 */
	int timer_lifebar_shadow;	/**< 生命条残影的定时器 */
	int n_erase_wait;		/**< 抹去生命条残影前需要等待的次数 */
	int n_point_per_erase;		/**< 每次抹去时扣除的残影上的生命点数 */
	LCUI_BOOL need_show_flash;	/**< 指示是否需要显示闪光 */
	int lifebar_shadow_state;	/**< 生命条残影的状态 */
	LCUI_Widget *lifebar;		/**< 当前生命条 */
	LCUI_Widget *flashbar[2];	/**< 扣血时显示的闪光条 */
	LCUI_Widget *lifebar_shadow[3];	/**< 生命条残影 */
} LifeBarData;

static LCUI_RGB color_set[5]={
	{220,0,0},
	{255,130,0},
	{140,205,0},
	{10,110,210},
	{140,135,250}
};

#define TYPE_NAME	"GamePlayerLifeBar"

/** 计算生命条的数量 */
static int LifeBar_CountBarNum( int cur_hp, int full_hp )
{
	int n;
	if( cur_hp <= full_hp ) {
		return 1;
	}
	n = cur_hp / full_hp;
	if( cur_hp % full_hp == 0 ) {
		return n;
	}
	return n+1;
}

static void LifeBar_GetShadowForeColor( int n_lifebar, LCUI_RGB *color )
{
	if( n_lifebar > 0 ) {
		n_lifebar -= 1;
	}
	color->red = color_set[n_lifebar%5].red*4/5;
	color->green = color_set[n_lifebar%5].green*4/5;
	color->blue = color_set[n_lifebar%5].blue*4/5;
}

static void LifeBar_GetShadowBackColor( int n_lifebar, LCUI_RGB *color )
{
	int n;
	if( n_lifebar > 0 ) {
		n_lifebar -= 1;
	}
	n = n_lifebar%5 - 1;
	if( n<0 ) {
		n = 4;
	}
	color->red = color_set[n].red*4/5;
	color->green = color_set[n].green*4/5;
	color->blue = color_set[n].blue*4/5;
}

static void LifeBar_UpdateShadow( void *arg )
{
	LifeBarData *data;
	double scale;
	int cur_n, old_n;
	char scale_str[10];
	LCUI_RGB fore_color, back_color;

	data = (LifeBarData*)Widget_GetPrivData((LCUI_Widget*)arg);
	if( data->lifebar_shadow_state == SHADOW_STATE_END_CLEAN ) {
		return;
	}

	if( data->lifebar_shadow_state == SHADOW_STATE_CLEAN ) {
		/* 如果还需要等几次 */
		if( data->n_erase_wait > 0 ) {
			--data->n_erase_wait;
			return;
		} else {
			data->n_erase_wait = 0;
		}
	}

	if( data->lifebar_shadow_state == SHADOW_STATE_NEED_CLEAN ) {
		data->lifebar_shadow_state = SHADOW_STATE_CLEAN;
	}
	cur_n = LifeBar_CountBarNum( data->current_life_point, data->full_life_point );
	old_n = LifeBar_CountBarNum( data->shadow_life_point, data->full_life_point );
	if( old_n < cur_n ) {
		return;
	}
	else if( old_n == cur_n ) {
		LifeBar_GetShadowForeColor( cur_n, &fore_color );
		LifeBar_GetShadowBackColor( cur_n, &back_color );
		scale = data->shadow_life_point % data->full_life_point;
		if( scale == 0.0 ) {
			scale = 100.0;
		} else {
			scale = 100 * scale / data->full_life_point;
		}
		Widget_SetBackgroundColor( data->lifebar_shadow[2], fore_color );
		Widget_SetBackgroundTransparent( data->lifebar_shadow[2], FALSE );
		sprintf( scale_str, "%.2lf%%", scale );
		Widget_SetSize( data->lifebar_shadow[2], scale_str, "100%" );
		Widget_Show( data->lifebar_shadow[2] );
		Widget_Hide( data->lifebar_shadow[1] );
		Widget_Hide( data->lifebar_shadow[0] );
	}
	else if( old_n - cur_n == 1 ) {
		LifeBar_GetShadowForeColor( old_n, &fore_color );
		LifeBar_GetShadowForeColor( cur_n, &back_color );
		scale = data->shadow_life_point % data->full_life_point;
		if( scale == 0.0 ) {
			scale = 100.0;
		} else {
			scale = 100 * scale / data->full_life_point;
		}
		Widget_SetBackgroundColor( data->lifebar_shadow[1], fore_color );
		Widget_SetBackgroundColor( data->lifebar_shadow[2], back_color );
		Widget_SetBackgroundTransparent( data->lifebar_shadow[2], FALSE );
		Widget_SetBackgroundTransparent( data->lifebar_shadow[1], FALSE );
		sprintf( scale_str, "%.2lf%%", scale );
		Widget_SetSize( data->lifebar_shadow[1], scale_str, "100%" );
		Widget_SetSize( data->lifebar_shadow[2], "100%", "100%" );
		Widget_Show( data->lifebar_shadow[2] );
		Widget_Show( data->lifebar_shadow[1] );
		Widget_Hide( data->lifebar_shadow[0] );
	} else {
		LifeBar_GetShadowForeColor( old_n, &fore_color );
		LifeBar_GetShadowBackColor( old_n, &back_color );
		scale = data->shadow_life_point % data->full_life_point;
		if( scale == 0.0 ) {
			scale = 100.0;
		} else {
			scale = 100 * scale / data->full_life_point;
		}
		Widget_SetBackgroundColor( data->lifebar_shadow[0], fore_color );
		Widget_SetBackgroundColor( data->lifebar_shadow[1], back_color );
		Widget_SetBackgroundTransparent( data->lifebar_shadow[2], FALSE );
		Widget_SetBackgroundTransparent( data->lifebar_shadow[1], FALSE );
		Widget_SetBackgroundTransparent( data->lifebar_shadow[0], FALSE );
		sprintf( scale_str, "%.2lf%%", scale );
		Widget_SetSize( data->lifebar_shadow[0], scale_str, "100%" );
		Widget_SetSize( data->lifebar_shadow[1], "100%", "100%" );
		Widget_SetSize( data->lifebar_shadow[2], "100%", "100%" );
		Widget_Show( data->lifebar_shadow[2] );
		Widget_Show( data->lifebar_shadow[1] );
		Widget_Show( data->lifebar_shadow[0] );
	}
	/* 增加每次在残影血条上扣除的生命点，直到最大扣除量为止 */
	if( data->n_point_per_erase < data->full_life_point/20+1 ) {
		++data->n_point_per_erase;
	}
	data->shadow_life_point -= data->n_point_per_erase;
	if( data->shadow_life_point <= data->current_life_point ) {
		data->lifebar_shadow_state = SHADOW_STATE_END_CLEAN;
		Widget_Hide( data->lifebar_shadow[2] );
		Widget_Hide( data->lifebar_shadow[1] );
		Widget_Hide( data->lifebar_shadow[0] );
	}

}

/** 定时更新闪光条 */
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
			return;
		}
		new_n = LifeBar_CountBarNum( data->current_life_point, data->full_life_point );
		old_n = LifeBar_CountBarNum( data->old_life_point, data->full_life_point );
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

			Widget_SetSize( data->flashbar[1], "100%", "100%" );
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
	data->lifebar_shadow_state = SHADOW_STATE_END_CLEAN;
	data->n_erase_wait = SHADOW_N_WAIT;
	data->lifebar = Widget_New(NULL);
	data->flashbar[0] = Widget_New(NULL);
	data->flashbar[1] = Widget_New(NULL);
	data->lifebar_shadow[0] = Widget_New(NULL);
	data->lifebar_shadow[1] = Widget_New(NULL);
	data->lifebar_shadow[2] = Widget_New(NULL);
	/* 加入至容器 */
	Widget_Container_Add( widget, data->lifebar );
	Widget_Container_Add( widget, data->flashbar[0] );
	Widget_Container_Add( widget, data->flashbar[1] );
	Widget_Container_Add( widget, data->lifebar_shadow[0] );
	Widget_Container_Add( widget, data->lifebar_shadow[1] );
	Widget_Container_Add( widget, data->lifebar_shadow[2] );
	/* 调整堆叠顺序 */
	Widget_SetZIndex( data->flashbar[0], 5 );
	Widget_SetZIndex( data->lifebar_shadow[0], 4 );
	Widget_SetZIndex( data->lifebar_shadow[1], 3 );
	Widget_SetZIndex( data->lifebar, 2 );
	Widget_SetZIndex( data->flashbar[1], 1 );
	Widget_SetZIndex( data->lifebar_shadow[2], 0 );

	Widget_SetBorder( widget, Border(2,BORDER_STYLE_SOLID,RGB(150,150,150)) );
	Widget_SetPadding( widget, Padding(2,2,2,2) );
	Widget_SetAlign( data->lifebar, ALIGN_MIDDLE_LEFT, Pos(0,0) );
	Widget_SetAlign( data->lifebar_shadow[0], ALIGN_MIDDLE_LEFT, Pos(0,0) );
	Widget_SetAlign( data->lifebar_shadow[1], ALIGN_MIDDLE_LEFT, Pos(0,0) );
	Widget_SetAlign( data->lifebar_shadow[2], ALIGN_MIDDLE_LEFT, Pos(0,0) );
	Widget_SetAlign( data->flashbar[0], ALIGN_MIDDLE_LEFT, Pos(0,0) );
	Widget_SetAlign( data->flashbar[1], ALIGN_MIDDLE_LEFT, Pos(0,0) );
	Widget_SetBackgroundTransparent( data->lifebar, FALSE );
	Widget_SetBackgroundTransparent( widget, FALSE );
	Widget_SetBackgroundTransparent( data->flashbar[0], FALSE );
	Widget_SetBackgroundTransparent( data->flashbar[1], FALSE );
	Widget_SetBackgroundColor( data->flashbar[0], RGB(255,255,255) );
	Widget_SetBackgroundColor( data->flashbar[1], RGB(255,255,255) );
	Widget_SetBackgroundColor( data->lifebar, color_set[0] );
	Widget_SetBackgroundColor( widget, RGB(100,100,100) );
	Widget_Show( data->lifebar );
	/* 创建一个定时器，实现闪光条的淡出效果 */
	data->timer_flashbar = LCUITimer_Set( 20, LifeBar_UpdateFlashBar, (void*)widget, TRUE );
	data->timer_lifebar_shadow = LCUITimer_Set( 20, LifeBar_UpdateShadow, (void*)widget, TRUE );
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
		/* 如果不需要抹去血条残影，则说明可以显示血条残影 */
		if( data->lifebar_shadow_state == SHADOW_STATE_END_CLEAN ) {
			/* 记录本次扣血前的血量，并标记需要抹去血条残影 */
			data->shadow_life_point = data->old_life_point;
			data->n_point_per_erase = 1;
			data->n_erase_wait = SHADOW_N_WAIT;
			data->lifebar_shadow_state = SHADOW_STATE_NEED_CLEAN;
		}
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
	LifeBarData *data;

	data = (LifeBarData *)Widget_GetPrivData(widget);
	return LifeBar_CountBarNum( data->current_life_point, data->full_life_point );
}

/** 注册生命条部件 */
void LifeBar_Regiser(void)
{
	WidgetType_Add( TYPE_NAME );
	WidgetFunc_Add( TYPE_NAME, LifeBar_ExecInit, FUNC_TYPE_INIT );
	WidgetFunc_Add( TYPE_NAME, LifeBar_ExecUpdate, FUNC_TYPE_UPDATE );
}