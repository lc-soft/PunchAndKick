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

// 老虎特有技能

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include "game.h"
#include "game_skill.h"

#define ZSPEED_SPIN_DRILL	1400
#define ZACC_SPIN_DRILL		2000

#define ZSPEED_BOUNCE	550
#define ZACC_BOUNCE	1500
#define XSPEED_BOUNCE	150

static int AttackDamage_SpinDrill( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	double damage;
	damage = victim->property.cur_hp * 0.5;
	if( damage > 800 ) {
		damage = 800;
	} else if( damage < 100 ) {
		damage = 100;
	}
	return DamageReduce( victim, victim_state, (int)damage );
}

static LCUI_BOOL CanUseSpinDrill( GamePlayer *player )
{
	GamePlayer *attacker;
	if( player->type != PLAYER_TYPE_TIGER ) {
		return FALSE;
	}
	if( !player->control.a_attack ) {
		return FALSE;
	}
	attacker = GetTargetInCatchRange( player );
	if( attacker ) {
		player->other = attacker;
		return TRUE;
	}
	if( player->state != STATE_CATCH || !player->other ) {
		return FALSE;
	}
	/* 对方需要是被擒住状态 */
	if( player->other->state != STATE_BE_CATCH
	 && player->other->state != STATE_BACK_BE_CATCH ) {
		return FALSE;
	}
	return TRUE;
}

static void AtLandingDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GameBattle_GetPlayerByWidget( widget );
	GamePlayer_UnlockMotion( player );
	GameObject_SetXSpeed( widget, 0 );
	GamePlayer_StartStand( player );
}

static void StartLeftBounce( LCUI_Widget *widget )
{
	GamePlayer *player;

	player = GameBattle_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_FALL );
	GamePlayer_LockAction( player );
	Game_RecordAttack( player, ATK_SPIN_DRILL, player->other, STATE_HALF_LYING );
	player->other = NULL;
	GamePlayer_SetRightOriented( player );
	GameObject_SetXSpeed( player->object, -XSPEED_BOUNCE );
	GameObject_AtLanding(
		player->object, ZSPEED_BOUNCE,
		-ZACC_BOUNCE, AtLandingDone
	);
}

static void StartRightBounce( LCUI_Widget *widget )
{
	GamePlayer *player;

	player = GameBattle_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_FALL );
	GamePlayer_LockAction( player );
	Game_RecordAttack( player, ATK_SPIN_DRILL, player->other, STATE_HALF_LYING );
	player->other = NULL;
	GamePlayer_SetLeftOriented( player );
	GameObject_SetXSpeed( player->object, XSPEED_BOUNCE );
	GameObject_AtLanding(
		player->object, ZSPEED_BOUNCE,
		-ZACC_BOUNCE, AtLandingDone
	);
}

static void AtTargetLandingDone( LCUI_Widget *widget )
{
	int ret = 0;
	GamePlayer *player;

	player = GameBattle_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	ret = GamePlayer_SetLying( player );
	GamePlayer_LockAction( player );
	GamePlayer_UnlockMotion( player );
	GamePlayer_StopXMotion( player );
	if( ret == 0 ) {
		GamePlayer_SetRestTimeOut( player, BE_THROW_REST_TIMEOUT, GamePlayer_StartStand );
	}
}

static void StartTargetRightBounce( LCUI_Widget *widget )
{
	GamePlayer *player;

	player = GameBattle_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_LYING_HIT );
	GameObject_AtActionDone( player->object, ACTION_LYING_HIT, NULL );
	GamePlayer_SetLeftOriented( player );
	GamePlayer_LockAction( player );
	player->other = NULL;
	GameObject_AtLanding(
		player->object, ZSPEED_BOUNCE,
		-ZACC_BOUNCE, AtTargetLandingDone
	);
	GameObject_SetXSpeed( player->object, XSPEED_BOUNCE );
}

static void StartTargetLeftBounce( LCUI_Widget *widget )
{
	GamePlayer *player;

	player = GameBattle_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_LYING_HIT );
	GameObject_AtActionDone( player->object, ACTION_LYING_HIT, NULL );
	GamePlayer_SetRightOriented( player );
	GamePlayer_LockAction( player );
	player->other = NULL;
	GameObject_AtLanding(
		player->object, ZSPEED_BOUNCE,
		-ZACC_BOUNCE, AtTargetLandingDone
	);
	GameObject_SetXSpeed( player->object, -XSPEED_BOUNCE );
}

static void StartSpinDrill( GamePlayer *player )
{
	int z_index;
	if( !player->other ) {
		return;
	}
	/* 发动技能前，停止移动 */
	GamePlayer_StopXMotion( player );
	GamePlayer_StopYMotion( player );
	/* 对目标进行调整，使技能能够正常实现 */
	CommonSkill_AdjustTargetAtBeCatch( player );
	
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockAction( player->other );
	GamePlayer_ChangeState( player->other, STATE_HALF_LYING );
	GamePlayer_ChangeState( player, STATE_CATCH_SKILL_FA );
	GamePlayer_LockAction( player );
	GamePlayer_LockAction( player->other );
	GamePlayer_LockMotion( player );
	GamePlayer_LockMotion( player->other );

	GameObject_SetX( player->other->object, GameObject_GetX(player->object) );
	GameObject_SetY( player->other->object, GameObject_GetY(player->object) );
	z_index = Widget_GetZIndex( player->other->object );
	Widget_SetZIndex( player->object, z_index+1 );
	if( GamePlayer_IsLeftOriented(player) ) {
		GameObject_AtLanding( player->other->object, ZSPEED_SPIN_DRILL, -ZACC_SPIN_DRILL, StartTargetLeftBounce );
		GameObject_AtLanding( player->object, ZSPEED_SPIN_DRILL, -ZACC_SPIN_DRILL, StartRightBounce );
	} else {
		GameObject_AtLanding( player->other->object, ZSPEED_SPIN_DRILL, -ZACC_SPIN_DRILL, StartTargetRightBounce );
		GameObject_AtLanding( player->object, ZSPEED_SPIN_DRILL, -ZACC_SPIN_DRILL, StartLeftBounce );
	}
	/* 重置被攻击的次数 */
	player->other->n_attack = 0;
}

/** 注册老虎特有的技能 */
void TigerSkill_Register(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_SPIN_DRILL,
				SKILLPRIORITY_SPIN_DRILL,
				CanUseSpinDrill,
				StartSpinDrill
	);
	AttackLibrary_AddAttack( ATK_SPIN_DRILL, AttackDamage_SpinDrill, NULL );
}
