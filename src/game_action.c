#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_GRAPH_H
#include "game.h"

/** 载入角色的动作动画资源 */
static ActionData* __ActionRes_Load( const ActionInfo *action_set, const char *class_name, int n_max, int action_type )
{
	int i, j;
	ActionData *action;
	LCUI_Graph *p_graph, graph_buff, tmp_graph;

	action = Action_Create();
	/* 在动作数据集里查找指定动作的数据 */
	for(i=0; i<n_max; ++i) {
		if( action_set[i].action_type == action_type ) {
			break;
		}
	}
	/* 未找到则退出 */
	if( i >= n_max ) {
		return NULL;
	}
	/* 设置该动作是否重复播放 */
	Action_SetReplay( action, action_set[i].replay );
	for(j=0; j<MAX_FRAME_NUM; ++j) {
		if( !action_set[i].frame[j].enable ) {
			break;
		}
		Graph_Init( &graph_buff );
		/* 获取动作图资源 */
		GameGraphRes_GetGraph(
			class_name, 
			action_set[i].frame[j].graph_name, 
			&graph_buff
		);
		/* 若需要水平翻转 */
		if( action_set[i].frame[j].horiz_flip ) {
			Graph_Init( &tmp_graph );
			tmp_graph.color_type = graph_buff.color_type;
			Graph_HorizFlip( &graph_buff, &tmp_graph );
			p_graph = &tmp_graph;
		} else {
			p_graph = &graph_buff;
		}
		/* 添加一帧动作 */
		Action_AddFrame(
			action, 
			action_set[i].frame[j].offset_x, 
			action_set[i].frame[j].offset_y,
			p_graph,
			action_set[i].frame[j].remain_time
		);
		/* 设置受攻击范围 */
		Action_SetHitRange(
			action, j, action_set[i].frame[j].hit 
		);
		/* 设置攻击范围 */
		Action_SetAttackRange(
			action, j, action_set[i].frame[j].atk
		);
		/* 设置该帧动作是否产生新攻击 */
		Action_SetNewAttack(
			action, j, action_set[i].frame[j].new_attack
		);
	}
	return action;
}

/** 载入指定角色的动作动画 */
ActionData* ActionRes_Load( int role_id, int action_type )
{
	int total_action;
	const ActionInfo *action_set;
	char *class_name;

	switch(role_id) {
	case ROLE_KUNI:
		total_action = ActionRes_Kuni_GetActionTotalNum();
		action_set = ActionRes_Kuni_GetActionSet();
		class_name = ACTION_RES_CLASS_KUNI;
		break;
	case ROLE_RIKI: 
		total_action = ActionRes_Riki_GetActionTotalNum();
		action_set = ActionRes_Riki_GetActionSet();
		class_name = ACTION_RES_CLASS_RIKI;
		break;
	case ROLE_MIKE:
		total_action = ActionRes_Mike_GetActionTotalNum();
		action_set = ActionRes_Mike_GetActionSet();
		class_name = ACTION_RES_CLASS_MIKE;
		break;
	case ROLE_TORAJI:
		total_action = ActionRes_Toraji_GetActionTotalNum();
		action_set = ActionRes_Toraji_GetActionSet();
		class_name = ACTION_RES_CLASS_TORAJI;
		break;
	default:
		return NULL;
	}
	return __ActionRes_Load( action_set, class_name, total_action, action_type );
}
