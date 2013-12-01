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

// 拳击家特有技能

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include "game.h"
#include "game_skill.h"

#define ATK_ELBOW_1		"elbow attack step 1"
#define ATK_ELBOW_2		"elbow attack step 2"
#define ATK_KNEEHIT_1		"knee hit attack step 1"
#define ATK_KNEEHIT_2		"knee hit attack step 2"

#define ZSPEED_KNEEHIT	100
#define ZACC_KNEEHIT	100

static int AttackDamage_Elbow1( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	double damage;
	damage = 20 + attacker->property.punch/5.0;
	return (int)damage;
}

static int AttackDamage_Elbow2( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	double damage;
	damage = attacker->property.max_hp;
	damage -= attacker->property.cur_hp;
	damage /= attacker->property.max_hp;
	damage = attacker->property.punch * damage;
	damage += attacker->property.punch/3.0;
	return (int)damage;
}

static int AttackDamage_KneeHit1( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	double damage;
	damage = attacker->property.max_hp;
	damage -= attacker->property.cur_hp;
	damage /= attacker->property.max_hp;
	damage = attacker->property.kick * damage;
	damage += (attacker->property.kick * 0.2);
	return (int)damage;
}

static int AttackDamage_KneeHit2( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	double damage;
	damage = attacker->property.throw * 0.5;
	return (int)damage;
}

static LCUI_BOOL CanUseElbow( GamePlayer *player )
{
	GamePlayer *attacker;
	/* 该技能不能给非拳击家的角色使用 */
	if( player->type != PLAYER_TYPE_MARTIAL_ARTIST ) {
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
	if( player->other->state != STATE_BE_CATCH ) {
		return FALSE;
	}
	/* 需要是面对面 */
	if( GamePlayer_IsLeftOriented(player)
	 == GamePlayer_IsLeftOriented(player->other) ) {
		return FALSE;
	}
	return TRUE;
}

static LCUI_BOOL CanUseKneeHit( GamePlayer *player )
{
	if( player->type != PLAYER_TYPE_MARTIAL_ARTIST ) {
		return FALSE;
	}
	if( !player->control.a_attack ) {
		return FALSE;
	}
	if( player->state != STATE_CATCH || !player->other ) {
		return FALSE;
	}
	/* 对方需要是背面被擒住状态 */
	if( player->other->state != STATE_BACK_BE_CATCH ) {
		return FALSE;
	}
	/* 需要是面朝同一方向 */
	if( GamePlayer_IsLeftOriented(player)
	 != GamePlayer_IsLeftOriented(player->other) ) {
			return FALSE;
	}
	return TRUE;
}

/** 在肘压技能结束时 */
static void GamePlayer_AtSkillDone( LCUI_Widget *widget )
{
	GamePlayer* player;
	player = GameBattle_GetPlayerByWidget( widget );
	GamePlayer_StartStand( player );
}

static void GamePlayer_AtElbowUpdate( LCUI_Widget *widget )
{
	GamePlayer *player;
	
	player = GameBattle_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player->other );
	switch( GameObject_GetCurrentActionFrameNumber( player->object) ) {
	case 0:
		GamePlayer_ChangeState( player->other, STATE_HIT );
		GameObject_AtActionDone( player->other->object, ACTION_HIT, NULL );
		break;
	case 1:
		/* 记录第一段攻击伤害 */
		Game_RecordAttack(	player, ATK_ELBOW_1, 
					player->other, player->other->state );
		break;
	case 2:
		GamePlayer_ChangeState( player->other, STATE_HIT_FLY );
		GameObject_AtActionDone( player->other->object, ACTION_HIT_FLY, NULL );
		break;
	case 4:
		/* 记录第二段攻击伤害 */
		Game_RecordAttack(	player, ATK_ELBOW_2,
					player->other, player->other->state );
	case 3:
		GamePlayer_ChangeState( player->other, STATE_LYING_HIT );
		GameObject_AtActionDone( player->other->object, ACTION_LYING_HIT, NULL );
		break;
	case 5:
		if( GamePlayer_SetLying( player->other ) == 0 ) {
			GamePlayer_SetRestTimeOut(
				player->other, SHORT_REST_TIMEOUT, 
				GamePlayer_StartStand 
			);
		}
		break;
	default:
		break;
	}
	GamePlayer_LockAction( player->other );
}

static void StartElbow( GamePlayer *player )
{
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
	GamePlayer_ChangeState( player, STATE_CATCH_SKILL_FA );
	GamePlayer_ChangeState( player->other, STATE_HIT );
	GamePlayer_LockAction( player );
	GamePlayer_LockAction( player->other );
	GamePlayer_LockMotion( player->other );

	GameObject_AtActionDone( player->other->object, ACTION_HIT, NULL );
	if( GamePlayer_IsLeftOriented(player) ) {
		GamePlayer_SetLeftOriented( player->other );
	} else {
		GamePlayer_SetRightOriented( player->other );
	}
	GameObject_AtActionUpdate(	player->object,
					ACTION_CATCH_SKILL_FA, 
					GamePlayer_AtElbowUpdate );
	GameObject_AtActionDone( player->object, ACTION_CATCH_SKILL_FA, 
				 GamePlayer_AtSkillDone  );
	/* 重置被攻击的次数 */
	player->other->n_attack = 0;
}

/** 在被膝击后落地时 */
static void GamePlayer_AtLandingByAfterKneeHit( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GameBattle_GetPlayerByWidget( widget );
	/* 记录第二段攻击伤害 */
	Game_RecordAttack(	player->other, ATK_KNEEHIT_2,
				player, STATE_LYING
	);
	GamePlayer_SetRestTimeOut( 
		player, SHORT_REST_TIMEOUT,
		GamePlayer_StartStand 
	);
	if( player->other ) {
		player->other->other = NULL;
	}
	player->other = NULL;
}

static void AtKneeHitUpdate( LCUI_Widget *widget )
{
	GamePlayer *player;
	
	player = GameBattle_GetPlayerByWidget( widget );
	switch( GameObject_GetCurrentActionFrameNumber( player->object) ) {
	case 0:
		break;
	case 1:
		/* 记录第一段攻击伤害 */
		Game_RecordAttack(	player, ATK_KNEEHIT_1, 
					player->other, STATE_LYING_HIT );
		GamePlayer_UnlockAction( player->other );
		GamePlayer_ChangeState( player->other, STATE_LYING_HIT );
		GamePlayer_LockAction( player->other );
	case 2:
		GameObject_SetZ( player->other->object, 24 );
		GameObject_SetZSpeed( player->other->object, 0 );
		break;
	case 3:
		GamePlayer_UnlockAction( player->other );
		GamePlayer_ChangeState( player->other, STATE_LYING );
		GamePlayer_LockAction( player->other );
		player->other->other = player;
		GameObject_AtLanding(	player->other->object,
					-ZSPEED_KNEEHIT, -ZACC_KNEEHIT,
					GamePlayer_AtLandingByAfterKneeHit );
	default:
		break;
	}
}

static void StartKneeHit( GamePlayer *player )
{
	double x, y;
	int z_index;
	
	/* 发动技能前，停止移动 */
	GamePlayer_StopXMotion( player );
	GamePlayer_StopYMotion( player );

	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockAction( player->other );
	if( GamePlayer_IsLeftOriented(player) ) {
		GamePlayer_SetLeftOriented( player->other );
	} else {
		GamePlayer_SetRightOriented( player->other );
	}
	GamePlayer_UnlockAction( player->other );
	GamePlayer_ChangeState( player->other, STATE_LYING );
	GamePlayer_LockAction( player->other );
		
	z_index = Widget_GetZIndex( player->object );
	/* 被攻击者需要显示在攻击者前面 */
	Widget_SetZIndex( player->other->object, z_index+1 );

	x = GameObject_GetX( player->object );
	y = GameObject_GetY( player->object );
	GameObject_SetX( player->other->object, x );
	GameObject_SetY( player->other->object, y );
	GameObject_SetZ( player->other->object, 56 );
	GameObject_AtLanding( player->other->object, -ZSPEED_KNEEHIT, -ZACC_KNEEHIT, NULL );
	GameObject_AtActionUpdate(
		player->object, ACTION_CATCH_SKILL_BA, 
		AtKneeHitUpdate 
	);
	GamePlayer_ChangeState( player, STATE_CATCH_SKILL_BA );
	GameObject_AtActionDone(
		player->object, ACTION_CATCH_SKILL_BA, 
		GamePlayer_AtSkillDone
	);
	/* 重置被攻击的次数 */
	player->other->n_attack = 0;
	GamePlayer_LockAction( player );
	GamePlayer_LockAction( player->other );
}

/** 注册拳击家特有的技能 */
void MartialArtistSkill_Register(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_ELBOW,
				SKILLPRIORITY_ELBOW,
				CanUseElbow,
				StartElbow
	);
	SkillLibrary_AddSkill(	SKILLNAME_KNEEHIT,
				SKILLPRIORITY_KNEEHIT,
				CanUseKneeHit,
				StartKneeHit
	);
	AttackLibrary_AddAttack( ATK_ELBOW_1, AttackDamage_Elbow1, NULL );
	AttackLibrary_AddAttack( ATK_ELBOW_2, AttackDamage_Elbow2, NULL );
	AttackLibrary_AddAttack( ATK_KNEEHIT_1, AttackDamage_KneeHit1, NULL );
	AttackLibrary_AddAttack( ATK_KNEEHIT_2, AttackDamage_KneeHit2, NULL );
}
