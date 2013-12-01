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

#ifndef __GAME_COMMON_SKILL_H__
#define __GAME_COMMON_SKILL_H__

#define LIFT_HEIGHT	56

#define SHORT_REST_TIMEOUT	2000
#define LONG_REST_TIMEOUT	3000
#define BE_THROW_REST_TIMEOUT	500
#define BE_LIFT_REST_TIMEOUT	4500

#define XSPEED_RUN	300
#define XSPEED_WALK	80
#define YSPEED_WALK	40
#define XACC_DASH	1500
#define ZSPEED_JUMP	500
#define ZACC_JUMP	1500

#define XSPEED_S_HIT_FLY	500
#define ZACC_S_HIT_FLY		1000
#define ZSPEED_S_HIT_FLY	200

#define XSPEED_WEAK_WALK	200
#define WEAK_WALK_TIMEOUT	2500

#define XSPEED_X_HIT_FLY	500
#define ZACC_XB_HIT_FLY		1500
#define ZSPEED_XB_HIT_FLY	300
#define ZACC_XF_HIT_FLY		3000
#define ZSPEED_XF_HIT_FLY	800

#define XSPEED_X_HIT_FLY2	100
#define ZACC_XF_HIT_FLY2	1000
#define ZSPEED_XF_HIT_FLY2	200

#define XSPEED_HIT_FLY	200
#define ZSPEED_HIT_FLY	600
#define ZACC_HIT_FLY	2000

#define XSPEED_THROWUP_FLY	400
#define XSPEED_THROWDOWN_FLY	1000
#define ZSPEED_THROWUP_FLY	500
#define ZSPEED_THROWDOWN_FLY	1000
#define ZACC_THROWUP_FLY	2000
#define ZACC_THROWDOWN_FLY	2000

#define SKILLNAME_A_ATTACK		"A attack"
#define SKILLNAME_B_ATTACK		"B attack"
#define SKILLNAME_DEFENSE		"defense"
#define SKILLNAME_SOLID_DEFENSE		"solid defense"
#define SKILLNAME_JUMP			"jump"
#define SKILLNAME_JUMP_A_ATTACK		"jump A attack"
#define SKILLNAME_JUMP_B_ATTACK		"jump B attack"
#define SKILLNAME_JUMP_MACH_A_ATTACK	"jump mach A attack"
#define SKILLNAME_JUMP_MACH_B_ATTACK	"jump mach B attack"
#define SKILLNAME_SPRINT_A_ATTACK	"sprint A attack"
#define SKILLNAME_SPRINT_B_ATTACK	"sprint B attack"
#define SKILLNAME_SPRINT_JUMP_A_ATTACK	"sprint jump A attack"
#define SKILLNAME_SPRINT_JUMP_B_ATTACK	"sprint jump B attack"
#define SKILLNAME_MACH_A_ATTACK		"mach A attack"
#define SKILLNAME_MACH_B_ATTACK		"mach B attack"
#define SKILLNAME_A_FINALBLOW		"A final blow"
#define SKILLNAME_B_FINALBLOW		"B final blow"
#define SKILLNAME_JUMP_ELBOW		"jump elbow"
#define SKILLNAME_JUMP_TREAD		"jump tread"
#define SKILLNAME_A_LIFT		"A lift"
#define SKILLNAME_B_LIFT		"B lift"
#define SKILLNAME_A_THROW		"A throw"
#define SKILLNAME_B_THROW		"B throw"
#define SKILLNAME_MACH_STOMP		"mach stomp"
#define SKILLNAME_BOMBKICK		"bomb kick"
#define SKILLNAME_SPINHIT		"spin hit"
#define SKILLNAME_RIDE			"ride"
#define SKILLNAME_RIDE_A_ATTACK		"ride A attack"
#define SKILLNAME_RIDE_B_ATTACK		"ride B attack"
#define SKILLNAME_CATCH			"catch"
#define SKILLNAME_BIG_ELBOW		"big elbow"
#define SKILLNAME_GUILLOTINE		"guillotine"
#define SKILLNAME_PUSH			"push"
#define SKILLNAME_JUMP_SPINKICK		"jump spin kick"
#define SKILLNAME_TORNADO_ATTACK	"tornado_attack"

enum SkillPriority_AAttack {
	SKILLPRIORITY_A_LIFT,
	SKILLPRIORITY_A_THROW,
	SKILLPRIORITY_MACH_STOMP,
	SKILLPRIORITY_JUMP_ELBOW,
	SKILLPRIORITY_A_FINALBLOW,
	SKILLPRIORITY_SPINHIT,
	SKILLPRIORITY_SPRINT_A_ATTACK,
	SKILLPRIORITY_SPRINT_JUMP_A_ATTACK,
	SKILLPRIORITY_BIG_ELBOW,
	SKILLPRIORITY_JUMP_MACH_A_ATTACK,
	SKILLPRIORITY_JUMP_A_ATTACK,
	SKILLPRIORITY_MACH_A_ATTACK,
	SKILLPRIORITY_A_ATTACK,
	SKILLPRIORITY_RIDE_A_ATTACK
};

enum SkillPriority_BAttack {
	SKILLPRIORITY_B_LIFT,
	SKILLPRIORITY_B_THROW,
	SKILLPRIORITY_JUMP_TREAD,
	SKILLPRIORITY_B_FINALBLOW,
	SKILLPRIORITY_BOMBKICK,
	SKILLPRIORITY_SPRINT_B_ATTACK,
	SKILLPRIORITY_SPRINT_JUMP_B_ATTACK,
	SKILLPRIORITY_GUILLOTINE,
	SKILLPRIORITY_JUMP_MACH_B_ATTACK,
	SKILLPRIORITY_JUMP_B_ATTACK,
	SKILLPRIORITY_PUSH,
	SKILLPRIORITY_MACH_B_ATTACK,
	SKILLPRIORITY_B_ATTACK,
	SKILLPRIORITY_RIDE_B_ATTACK
};

enum SkillPriority_HorizMotion {
	SKILLPRIORITY_RIDE,
	SKILLPRIORITY_JUMP_SPINKICK
};

enum SkillPriority_VertiMotion {
	SKILLPRIORITY_CATCH
};

enum SkillPriority_Jump {
	SKILLPRIORITY_JUMP
};

enum SkillPriority_Defense {
	SKILLPRIORITY_DEFENSE
};

/** 在擒住状态下，对目标的状态进行调整 */
void CommonSkill_AdjustTargetAtBeCatch( GamePlayer *self );

/** 设置自己在擒获住对方时对方的位置 */
void CommonSkill_SetPositionAtCatch( GamePlayer *self, GamePlayer *other );

/** 注册通用技能 */
void CommonSkill_Register(void);

/** 在擒获范围内，获取可被直接擒获的目标 */
GamePlayer *GetTargetInCatchRange( GamePlayer *self );

/** 设置游戏角色被攻击命中 */
void GamePlayer_SetHit( GamePlayer *player );

/** 设置游戏角色为下落状态 */
void GamePlayer_SetFall( GamePlayer *player );
#endif
