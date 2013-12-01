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

// 处理游戏角色的攻击伤害，以及受攻击时的动作

#ifndef __GAME_ATTACK_H__
#define __GAME_ATTACK_H__

#include "game_value_tip.h"

#define ATK_NONE			"none"
#define ATK_THROW			"throw attack"
#define ATK_RIDE_A_ATTACK		"ride A attack"
#define ATK_RIDE_B_ATTACK		"ride B attack"
#define ATK_BUMPED			"bumped attack"
#define ATK_BUMPED2			"bumped attack, no effcet"
#define ATK_FINALBLOW			"final blow attack"
#define ATK_SPRINT_JUMP_A_ATTACK	"sprint jump A attack"
#define ATK_SPRINT_JUMP_B_ATTACK	"sprint jump B attack"
#define ATK_A_ATTACK			"A attack"
#define ATK_B_ATTACK			"B attack"
#define ATK_SPRINT_A_ATTACK		"sprint A attack"
#define ATK_SPRINT_B_ATTACK		"sprint B attack"
#define ATK_MACH_A_ATTACK		"mach A attack"
#define ATK_MACH_B_ATTACK		"mach B attack"
#define ATK_JUMP_TREAD			"jump tread attack"
#define ATK_JUMP_ELBOW			"jump elbow attack"
#define ATK_MACH_STOMP			"mach stomp attack"
#define ATK_JUMP_A_ATTACK		"jump A attack"
#define ATK_JUMP_B_ATTACK		"jump A attack"
#define ATK_JUMP_MACH_A_ATTACK		"jump mach A attack"
#define ATK_JUMP_MACH_B_ATTACK		"jump mach A attack"
#define ATK_BOMBKICK			"bomb kick attack"
#define ATK_SPINHIT			"spin hit attack"
#define ATK_SPINHIT_2			"second spin hit attack"
#define ATK_BIG_ELBOW			"big elbow attack"
#define ATK_GUILLOTINE			"guillotine attack"
#define ATK_JUMP_SPINKICK		"jump spin kick attack"


/** 根据受害者的防御力及其它情况，获取减免后的实际伤害 */
int DamageReduce( GamePlayer *victim, int victim_state, int demage );

/**< 添加攻击数据 */
int AttackLibrary_AddAttack(	const char *attack_name, 
				int (*get_damage)(GamePlayer*,GamePlayer*,int),
				void (*effect)(GamePlayer*,GamePlayer*) );

/** 记录攻击者对受害者的攻击 */
void Game_RecordAttack( GamePlayer *attacker,
			const char *attack_type_name,
			GamePlayer *victim,
			int victim_state );

/** 处理已经记录的攻击 */
void Game_ProcAttack( LCUI_Queue *p_attakc_record, ValueTipData *value_tip );

/** 初始化攻击信息库 */
void Game_InitAttackLibrary(void);

/** 初始化攻击记录 */
void Game_InitAttackRecord( LCUI_Queue *p_attakc_record );

#endif