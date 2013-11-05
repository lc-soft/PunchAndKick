// 数值提示，例如：攻击产生的伤害值
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H

#include "game_value_tip.h"
#include "game_resource.h"

#define LAST_STEP_NUM	60
#define Y_DISTANCE	80

#define MAX_REMAIN_COUNT	30

#define FONT_COLOR_RED		0
#define FONT_COLOR_GREEN	1

typedef struct ValueTipTaskData_ {
	int id;
	int value;
	int step_num;
	LCUI_Pos start_pos;
	LCUI_Widget *wdg_tip;
} ValueTipTaskData;

static void ValueTipTask_Destroy( void *arg )
{
	ValueTipTaskData *p_task_data;
	p_task_data = (ValueTipTaskData*)arg;
	Widget_Destroy( p_task_data->wdg_tip );
}

/** 动态更新显示的数值 */
static void GameValueTip_Proc( void *arg )
{
	int i, n;
	double tmp_val;
	ValueTipData *p_data;
	ValueTipTaskData *p_task_data;
	LCUI_Pos pos;
	LCUI_Size size, new_size;
	uchar_t alpha;

	p_data = (ValueTipData*)arg;
	Queue_Lock( &p_data->tip_data_list );
	n = Queue_GetTotal( &p_data->tip_data_list );
	for(i=0; i<n; ++i) {
		p_task_data = (ValueTipTaskData*)
		Queue_Get( &p_data->tip_data_list, i );
		if( !p_data ) {
			continue;
		}
		if( p_task_data->step_num >= LAST_STEP_NUM ) {
			Widget_SetAlpha( p_task_data->wdg_tip, 0 );
			Widget_Hide( p_task_data->wdg_tip );
			p_task_data->value = 0;
			continue;
		}
		++p_task_data->step_num;
		/* 记录数值提示的显示位置 */
		pos.x = p_task_data->start_pos.x;
		pos.y = p_task_data->start_pos.y;
		/* 记录背景图的尺寸，作为部件的原始尺寸 */
		size.w = p_task_data->wdg_tip->background.image.w;
		size.h = p_task_data->wdg_tip->background.image.h;
		/* 前5次更新用于实现膨胀效果 */
		if( p_task_data->step_num < 5 ) {
			/* 尺寸随p_task_data->step_num的增长而增长 */
			new_size.w = size.w + p_task_data->step_num*(size.w/2)/5;
			new_size.h = size.h + p_task_data->step_num*(size.h/2)/5;
			/* 根据尺寸，调整位置，以使部件中心在p_task_data->start_pos处 */
			pos.x -= new_size.w/2;
			pos.y -= new_size.h/2;
			Widget_Resize( p_task_data->wdg_tip, new_size );
		}
		/* 后5次更新用于实现缩小效果 */
		else if( p_task_data->step_num < 10 ) {
			/* 尺寸随p_task_data->step_num的增长而缩小 */
			new_size.w = size.w + (10-p_task_data->step_num)*(size.w/2)/5;
			new_size.h = size.h + (10-p_task_data->step_num)*(size.h/2)/5;
			pos.x -= new_size.w/2;
			pos.y -= new_size.h/2;
			Widget_Resize( p_task_data->wdg_tip, new_size );
		}
		/* 当达到第10次更新时，还原部件应有的尺寸 */
		else if( p_task_data->step_num == 10 ) {
			pos.x -= size.w/2;
			pos.y -= size.h/2;
			Widget_Resize( p_task_data->wdg_tip, size );
		} else {
			pos.x -= size.w/2;
			pos.y -= size.h/2;
		}
		/* 当更新次数大于或等于MAX_REMAIN_COUNT时 */
		if( p_task_data->step_num >= MAX_REMAIN_COUNT ) {
			tmp_val = 1.0*p_task_data->step_num - MAX_REMAIN_COUNT;
			tmp_val = tmp_val/(LAST_STEP_NUM - MAX_REMAIN_COUNT);
			/* 按比例计算部件透明度，以及的Y轴坐标 */
			alpha = (uchar_t)(255 - tmp_val*255);
			pos.y -= (int)(Y_DISTANCE*tmp_val);
		} else {
			alpha = 255;
		}

		Widget_Move( p_task_data->wdg_tip, pos );
		Widget_SetAlpha( p_task_data->wdg_tip, alpha );
	}
	Queue_Unlock( &p_data->tip_data_list );
}

/** 初始化数值提示功能 */
void GameValueTip_Init( ValueTipData *p_data, LCUI_Widget *container )
{
	p_data->widget_container = container;
	Queue_Init( &p_data->tip_data_list, sizeof(ValueTipTaskData), ValueTipTask_Destroy );
	p_data->timer_id = LCUITimer_Set( 10, GameValueTip_Proc, p_data, TRUE );
}

/** 退出数值提示功能 */
void GameValueTip_Quit( ValueTipData *p_data )
{
	/* 先关掉定时器 */
	LCUITimer_Free( p_data->timer_id );
	/* 再释放占用的资源 */
	Queue_Destroy( &p_data->tip_data_list );
}

/** 设置显示的值 */
static void GameValueTipWidget_SetValue( LCUI_Widget *widget, int color_type, int value )
{
	int i, n, w = 0, x;
	char str[10], res_name[64];
	LCUI_Graph bg, graph[10];
	
	/* 转换成字符串 */
	sprintf( str, "%d", value );
	/* 获取位数 */
	n = strlen(str);
	for(i=n-1; i>=0; --i) {
		/* 生成资源名称 */
		if( color_type == FONT_COLOR_RED ) {
			sprintf( res_name, "red-%c", str[i] );
		} else {
			sprintf( res_name, "green-%c", str[i] );
		}
		Graph_Init( &graph[i] );
		/* 获取与该字符对应的字体图像 */
		if( GameGraphRes_GetGraph( FONT_RES, res_name, &graph[i] ) != 0 ) {
			continue;
		}
		/* 累计图像宽度 */
		w += graph[i].w;
	}
	x = 0;
	Graph_Init( &bg );
	bg.color_type = COLOR_TYPE_RGBA;
	/* 创建合适尺寸的图像 */
	Graph_Create( &bg, w, 24 );
	for(i=0; i<n; ++i) {
		/* 将获得到的字体图像覆盖至图像上 */
		Graph_Replace( &bg, &graph[i], Pos(x,0));
		x += graph[i].w;
	}
	/* 如果有背景图，那就释放它，因为这个部件的背景图只有本函数在设置 */
	if( Graph_IsValid( &widget->background.image ) ) {
		Graph_Free( &widget->background.image );
	}
	/* 设置背景 */
	Widget_SetBackgroundImage( widget, &bg );
	Widget_SetBackgroundTransparent( widget, TRUE );
	/* 设置背景布局类型为拉伸，背景图随部件的尺寸变化而变化 */
	Widget_SetBackgroundLayout( widget, LAYOUT_STRETCH );
	/* 调整尺寸 */
	Widget_Resize( widget, Graph_GetSize(&bg) );
}

/** 获取可用的数据 */
static ValueTipTaskData* GameValueTip_GetValidData( ValueTipData *p_data, int id )
{
	int i, n;
	ValueTipTaskData *p_task_data;

	Queue_Lock( &p_data->tip_data_list );
	n = Queue_GetTotal( &p_data->tip_data_list );
	for(i=0; i<n; ++i) {
		p_task_data = (ValueTipTaskData*)
		Queue_Get( &p_data->tip_data_list, i );
		if( !p_task_data ) {
			continue;
		}
		if( (p_task_data->id == id
		 && p_task_data->step_num < MAX_REMAIN_COUNT)
		 || p_task_data->step_num >= LAST_STEP_NUM ) {
			Queue_Unlock( &p_data->tip_data_list );
			return p_task_data;
		}
	}
	Queue_Unlock( &p_data->tip_data_list );
	return NULL;
}

/** 添加一条数值提示 */
void GameValueTip_AddTip( ValueTipData *p_data, int id, LCUI_Pos pos, int value )
{
	ValueTipTaskData new_task_data, *p_task_data;

	p_task_data = GameValueTip_GetValidData( p_data, id );
	if( p_task_data ) {
		p_task_data->id = id;
		p_task_data->start_pos = pos;
		p_task_data->step_num = 0;
		/* 与上次的值进行累加 */
		p_task_data->value += value;
		/* 更新数据 */
		GameValueTipWidget_SetValue( p_task_data->wdg_tip, FONT_COLOR_RED, p_task_data->value );
		Widget_Show( p_task_data->wdg_tip );
		return;
	}

	new_task_data.id = id;
	new_task_data.start_pos = pos;
	new_task_data.step_num = 0;
	new_task_data.value = value;
	new_task_data.wdg_tip = Widget_New(NULL);
	
	Widget_Container_Add( p_data->widget_container, new_task_data.wdg_tip );
	Widget_SetAlpha( new_task_data.wdg_tip, 0 );
	Widget_SetZIndex( new_task_data.wdg_tip, 1000 );
	Widget_Show( new_task_data.wdg_tip );

	GameValueTipWidget_SetValue( new_task_data.wdg_tip, FONT_COLOR_RED, value );

	Queue_Lock( &p_data->tip_data_list );
	Queue_Add( &p_data->tip_data_list, &new_task_data );
	Queue_Unlock( &p_data->tip_data_list );
}
