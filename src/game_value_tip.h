#ifndef __GAME_VALUE_TIP_H__
#define __GAME_VALUE_TIP_H__

typedef struct ValueTipData_ {
	LCUI_BOOL is_inited;
	int timer_id;
	LCUI_Widget *widget_container;
	LCUI_Queue tip_data_list;
} ValueTipData;

/** 初始化数值提示功能 */
void GameValueTip_Init( ValueTipData *p_data, LCUI_Widget *container );

/** 退出数值提示功能 */
void GameValueTip_Quit( ValueTipData *p_data );

/** 添加一条数值提示 */
void GameValueTip_AddTip( ValueTipData *p_data, int id, LCUI_Pos pos, int value );

#endif
