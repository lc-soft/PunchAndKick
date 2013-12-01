/* ****************************************************************************
 * Punch And Kick -- a simple 2D Fighting Game.
 *
 * Copyright (C) 2013 by
 * Liu Chao
 *
 * This file is part of the Punch And Kick project, and may only be used,
 * modified, and distributed under the terms of the GPLv2.
 *
 * (GPLv2 is abbreviation of GNU General Public License Version 2)
 *
 * By continuing to use, modify, or distribute this file you indicate that you
 * have read the license and understand and accept it fully.
 *
 * The Punch And Kick project is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GPL v2 for
 * more details.
 *
 * You should have received a copy of the GPLv2 along with this file. It is
 * usually in the LICENSE.TXT file, If not, see <http://www.gnu.org/licenses/>.
 * ***************************************************************************/

/* ****************************************************************************
 * Punch And Kick -- 一个普通的2D格斗游戏
 *
 * 版权所有 (C) 2013 归属于
 * 刘超
 *
 * 这个文件是 Punch And Kick 项目的一部分，并且只可以根据GPLv2许可协议来使用、
 * 更改和发布。
 *
 * (GPLv2 是 GNU通用公共许可证第二版 的英文缩写)
 *
 * 继续使用、修改或发布本文件，表明您已经阅读并完全理解和接受这个许可协议。
 *
 * Punch And Kick 项目是基于技术交流目的而加以散布的，但不负任何担保责任，例
 * 如：稳定性、可玩性、趣味性等的担保，甚至没有适销性或特定用途的隐含担保，详
 * 情请参照GPLv2许可协议。
 *
 * 您应已收到附随于本文件的GPLv2许可协议的副本，它通常在LICENSE.TXT文件中，如果
 * 没有，请查看：<http://www.gnu.org/licenses/>.
 * ***************************************************************************/

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_GRAPH_H
#include "game.h"

/** 载入角色的动作动画资源 */
static ActionData* __ActionRes_Load(	const ActionInfo *action_set, 
					const char *class_name, 
					int n_max, 
					int action_type )
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
	case ROLE_KUNIO:
		total_action = ActionRes_Kunio_GetActionTotalNum();
		action_set = ActionRes_Kunio_GetActionSet();
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
	case ROLE_BEN:
		total_action = ActionRes_Ben_GetActionTotalNum();
		action_set = ActionRes_Ben_GetActionSet();
		class_name = ACTION_RES_CLASS_BEN;
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
