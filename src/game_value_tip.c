// 数值提示，例如：攻击产生的伤害值
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H

#include "game_value_tip.h"
#include "game_resource.h"

#define LAST_STEP_NUM	60
#define Y_DISTANCE	80

#define FONT_COLOR_RED		0
#define FONT_COLOR_GREEN	1

typedef struct ValueTipTaskData_ {
	int step_num;
	LCUI_Pos start_pos;
	LCUI_Widget *wdg_tip;
} ValueTipTaskData;

/** 动态更新显示的数值 */
static void GameValueTip_Proc( void *arg )
{
	int i, n;
	ValueTipData *p_data;
	ValueTipTaskData *p_task_data;
	LCUI_Pos pos;
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
			continue;
		}
		++p_task_data->step_num;
		pos.x = p_task_data->start_pos.x;
		pos.x -= _Widget_GetWidth(p_task_data->wdg_tip)/2;
		pos.y = p_task_data->start_pos.y;
		if( p_task_data->step_num < LAST_STEP_NUM-20 ) {
			alpha = 255*p_task_data->step_num/(LAST_STEP_NUM-20);
			pos.y -= Y_DISTANCE*p_task_data->step_num/(LAST_STEP_NUM-20);
		} else {
			alpha = 255;
			pos.y -= Y_DISTANCE;
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
	Queue_Init( &p_data->tip_data_list, sizeof(ValueTipTaskData), NULL );
	p_data->timer_id = LCUITimer_Set( 10, GameValueTip_Proc, p_data, TRUE );
}

/** 退出数值提示功能 */
void GameValueTip_Quit( ValueTipData *p_data )
{

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
	Widget_SetBackgroundLayout( widget, LAYOUT_NONE );
	/* 调整尺寸 */
	Widget_Resize( widget, Graph_GetSize(&bg) );
}

/** 添加一条提示 */
void GameValueTip_AddTip( ValueTipData *p_data, LCUI_Pos pos, int value )
{
	int i, n;
	ValueTipTaskData new_task_data, *p_task_data = NULL;

	Queue_Lock( &p_data->tip_data_list );
	n = Queue_GetTotal( &p_data->tip_data_list );
	for(i=0; i<n; ++i) {
		p_task_data = (ValueTipTaskData*)
		Queue_Get( &p_data->tip_data_list, i );
		if( !p_task_data ) {
			continue;
		}
		if( p_task_data->step_num >= LAST_STEP_NUM ) {
			break;
		}
	}
	Queue_Unlock( &p_data->tip_data_list );
	if( i < n && p_data ) {
		p_task_data->start_pos = pos;
		p_task_data->step_num = 0;
		GameValueTipWidget_SetValue( p_task_data->wdg_tip, FONT_COLOR_RED, value );
		Widget_Show( p_task_data->wdg_tip );
		return;
	}

	new_task_data.start_pos = pos;
	new_task_data.step_num = 0;
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
