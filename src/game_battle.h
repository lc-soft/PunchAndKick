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

#ifndef __GAME_BATTLE_H__
#define __GAME_BATTLE_H__

#include "game_value_tip.h"

/** 新建一个对战，并返回该对战的ID */
int GameBattle_New(void);

/** 退出对战 */
int GameBattle_Quit( int battle_id );

/** 设置是否启用数值提示功能 */
int GameBattle_SetEnableValueTip( int battle_id, LCUI_BOOL need_enable );

/** 开始对战 */
int GameBattle_Start( int battle_id );

/** 设置指定游戏角色的控制键 */
int GameBattle_SetPlayerControlKey(	int battle_id, 
					int player_id, 
					ControlKey *ctrlkey );

/** 通过控制键获取该键控制的角色 */
GamePlayer *GameBattle_GetPlayerByControlKey( int key_code );

/** 通过GameObject部件获取游戏玩家数据 */
GamePlayer *GameBattle_GetPlayerByWidget( LCUI_Widget *gameobject );

/** 获取指定ID的玩家数据 */
GamePlayer *GameBattle_GetPlayer( int battle_id, int player_id );

/** 设置一个玩家 */
int GameBattle_SetPlayer(	int battle_id, 
				int player_id,
				int role_id, 
				LCUI_BOOL human_control );

/** 设置是否启用一个玩家 */
int GameBattle_EnablePlayer(	int battle_id, 
				int player_id, 
				LCUI_BOOL flag );

/** 设置指定对战中的玩家是否无敌 */
int GameBattlePlayer_SetInvincible(	int battle_id, 
					int player_id, 
					LCUI_BOOL flag );
/** 设置场景地面参数 */
int GameBattle_SetSceneLand(	int battle_id,
				LCUI_Pos land_pos, 
				LCUI_Size land_size );

/** 获取场景地面参数 */
int GameBattle_GetSceneLand(	int battle_id,
				LCUI_Pos *land_pos, 
				LCUI_Size *land_size );

/** 设置是否自动同步镜头 */
int GameBattle_SetAutoSyncCamera( int battle_id, LCUI_BOOL is_true );

/** 设置场景大小 */
int GameBattle_SetSceneSize( int battle_id, LCUI_Size size );

/** 获取场景大小 */
int GameBattle_GetSceneSize( int battle_id, LCUI_Size *size );

/** 设置场景背景图 */
int GameBattle_SetSceneBackground(	int battle_id, 
					LCUI_Graph *img );

/** 获取场景 */
LCUI_Widget* GameBattle_GetScene( int battle_id );

/** 获取攻击记录 */
LCUI_Queue* GameBattle_GetAttackRecord( int battle_id );

/** 设置是否暂停对战 */
int GameBattle_Pause( int battle_id, LCUI_BOOL need_pause );

/** 进入循环 */
int GameBattle_Loop( int battle_id );

#endif
