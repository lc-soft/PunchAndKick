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

// 柔道家的技能

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include "game.h"
#include "game_skill.h"

#define ATK_JUDO	"judo attack"
#define ATK_BACK_JUDO	"back judo attack"

#define XSPEED_JUDO_THROW	1000
#define XACC_JUDO_THROW		200
#define ZSPEED_JUDO_THROW	200
#define ZACC_JUDO_THROW		1500

int AttackDamage_Judo( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	double damage;
	damage = attacker->property.punch;
	damage += attacker->property.throw;
	return DamageReduce( victim, victim_state, (int)damage );
}

int AttackDamage_BackJudo( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	double damage;
	damage = attacker->property.punch;
	damage += attacker->property.kick;
	damage /= 2.0;
	damage += attacker->property.throw;
	return DamageReduce( victim, victim_state, (int)damage );
}

LCUI_BOOL CanUseJudo( GamePlayer *player )
{
	GamePlayer *attacker;
	/* 该技能不能给非格斗家的角色使用 */
	if( player->type != PLAYER_TYPE_JUDO_MASTER ) {
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

LCUI_BOOL CanUseBackJudo( GamePlayer *player )
{
	if( player->type != PLAYER_TYPE_JUDO_MASTER ) {
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

static void SelfAtSkillDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GameBattle_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
	GamePlayer_SetReady( player );
}

static void Judo_SetTargetLying( GamePlayer *player )
{
	int ret;
	GamePlayer_UnlockAction( player );
	ret = GamePlayer_SetLying( player );
	GamePlayer_LockAction( player );
	GamePlayer_StopXMotion( player );
	if( ret == 0 ) {
		GamePlayer_SetRestTimeOut( player, 1000, GamePlayer_StartStand );
	}
}

static void AtTargetLanding( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GameBattle_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_LYING_HIT );
	GameObject_AtActionDone( widget, ACTION_LYING_HIT, NULL );
	GamePlayer_LockAction( player );
	if( player->other ) {
		Game_RecordAttack(	player->other, ATK_JUDO, 
					player, STATE_HIT_FLY_FALL );
		player->other->other = NULL;
	}
	player->other = NULL;
	GamePlayer_SetActionTimeOut( player, 200, Judo_SetTargetLying );
}

/* 处理目标在被摔倒时与其它玩家的碰撞 */
static void Judo_ProcTargetTouchAttack(	LCUI_Widget *self, LCUI_Widget *other )
{
	GamePlayer *player, *other_player;

	player = GameBattle_GetPlayerByWidget( self );
	other_player = GameBattle_GetPlayerByWidget( other );
	if( !player || !other_player ) {
		return;
	}
	if( other_player == player->other
	 && other_player->state == STATE_CATCH_SKILL_FA ) {
		return;
	}
	if( player->state != STATE_HIT_FLY_FALL ) {
		GameObject_AtTouch( self, NULL );
		return;
	}
	/* 如果已经是受攻击者了，则退出，不重复记录攻击 */
	if( GameObject_HaveVictim(self, other) ) {
		return;
	}
	/* 设置自己的攻击类型 */
	GamePlayer_SetAttackTypeName( player, ATK_JUMP_TREAD );
	/* 添加受害者记录 */
	GameObject_DirectAddVictim( self, other );
	/* 重置对方的受攻击次数 */
	other_player->n_attack = 0;
}

static void Judo_AtActionUpdate( LCUI_Widget *widget )
{
	int z_index;
	double x, z, y;
	GamePlayer *player, *other_player;
	
	player = GameBattle_GetPlayerByWidget( widget );
	other_player = player->other;
	if( !other_player ) {
		return;
	}
	z_index = Widget_GetZIndex( player->object );
	Widget_SetZIndex( other_player->object, z_index-1 );

	switch( GameObject_GetCurrentActionFrameNumber(widget) ) {
	case 1:
		x = GameObject_GetX( widget );
		y = GameObject_GetY( widget );
		z = GameObject_GetZ( widget );
		GameObject_SetX( other_player->object, x );
		GameObject_SetY( other_player->object, y );
		GameObject_SetZ( other_player->object, z+40 );
		if( GamePlayer_IsLeftOriented(player) ) {
			GamePlayer_SetRightOriented( other_player );
		} else {
			GamePlayer_SetLeftOriented( other_player );
		}
		GamePlayer_UnlockAction( other_player );
		GamePlayer_ChangeState( other_player, STATE_TUMMY_HIT_FLY );
		GamePlayer_LockAction( other_player );
		break;
	case 2:
		x = GameObject_GetX( widget );
		y = GameObject_GetY( widget );
		z = GameObject_GetZ( widget );
		GamePlayer_UnlockAction( other_player );
		GamePlayer_ChangeState( other_player, STATE_HIT_FLY_FALL );
		if( GamePlayer_IsLeftOriented(player) ) {
			GamePlayer_SetLeftOriented( other_player );
			x += 30;
		} else {
			GamePlayer_SetRightOriented( other_player );
			x -= 30;
		}
		GamePlayer_LockAction( other_player );
		GameObject_SetX( other_player->object, x );
		GameObject_SetY( other_player->object, y );
		GameObject_SetZ( other_player->object, z );
		/* 清除对方攻击记录，以让对方能够对其它角色造成新攻击 */
		GameObject_ClearAttack( other_player->object );
		GameObject_AtTouch( other_player->object, Judo_ProcTargetTouchAttack );
		GameObject_AtLanding( other_player->object, 0, -ZACC_JUMP, AtTargetLanding );
		break;
	case 4:
		break;
	default:break;
	}
}

void StartJudo( GamePlayer *player )
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
	GamePlayer_ChangeState( player, STATE_CATCH_SKILL_FA );
	GamePlayer_ChangeState( player->other, STATE_HIT );
	GamePlayer_LockAction( player->other );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );

	GameObject_AtActionUpdate( player->object, ACTION_CATCH_SKILL_FA, 
					Judo_AtActionUpdate );
	GameObject_AtActionDone( player->object, ACTION_CATCH_SKILL_FA, 
					SelfAtSkillDone );
	z_index = Widget_GetZIndex( player->object );
	Widget_SetZIndex( player->other->object, z_index-1 );
	player->other->n_attack = 0;
}

static void TargetStartLying( LCUI_Widget *widget )
{
	int ret;
	GamePlayer *player;

	player = GameBattle_GetPlayerByWidget( widget );
	if( player->other ) {
		Game_RecordAttack(	player->other, ATK_BACK_JUDO, 
					player, STATE_HIT_FLY_FALL );
		player->other->other = NULL;
		GameObject_AtTouch( widget, NULL );
	}
	player->other = NULL;
	GamePlayer_UnlockAction( player );
	ret = GamePlayer_SetLying( player );
	GamePlayer_LockAction( player );
	GamePlayer_StopXMotion( player );
	if( ret == 0 ) {
		GamePlayer_SetRestTimeOut( player, 1000, GamePlayer_StartStand );
	}
}

static void ChangeTargetAction( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GameBattle_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_HIT_FLY_FALL );
	GamePlayer_LockAction( player );
	GameObject_AtZSpeed( widget, -ZSPEED_JUDO_THROW/2, NULL );
}

/* 处理目标在被扔飞后与其它玩家的碰撞 */
static void BackJudo_ProcTargetTouchAttack(	LCUI_Widget *self, 
						LCUI_Widget *other )
{
	GamePlayer *player, *other_player;

	player = GameBattle_GetPlayerByWidget( self );
	other_player = GameBattle_GetPlayerByWidget( other );
	if( !player || !other_player ) {
		return;
	}
	if( other_player == player->other
	 && other_player->state == STATE_CATCH_SKILL_BA ) {
		return;
	}
	switch( player->state ) {
	case STATE_HIT_FLY_FALL:
	case STATE_LYING_HIT:
		break;
	default:
		GameObject_AtTouch( self, NULL );
		return;
	}
	/* 如果已经是受攻击者了，则退出，不重复记录攻击 */
	if( GameObject_HaveVictim(self, other) ) {
		return;
	}
	/* 设置自己的攻击类型 */
	GamePlayer_SetAttackTypeName( player, ATK_BUMPED );
	/* 添加受害者记录 */
	GameObject_DirectAddVictim( self, other );
	/* 重置对方的受攻击次数 */
	other_player->n_attack = 0;
}

static void BackJudo_AtActionUpdate( LCUI_Widget *widget )
{
	int z_index;
	double x, z, x_speed, x_acc;
	GamePlayer *player, *other_player;
	
	player = GameBattle_GetPlayerByWidget( widget );
	other_player = player->other;
	if( !other_player ) {
		return;
	}
	z_index = Widget_GetZIndex( player->object );
	Widget_SetZIndex( other_player->object, z_index-1 );

	switch( GameObject_GetCurrentActionFrameNumber(widget) ) {
	case 1:
		GamePlayer_UnlockAction( other_player );
		GamePlayer_ChangeState( other_player, STATE_HIT_FLY);
		GamePlayer_LockAction( other_player );
		break;
	case 3:
		z = GameObject_GetZ( player->object );
		x = GameObject_GetX( player->object );
		GameObject_SetZ( other_player->object, z+40 );
		GameObject_SetX( other_player->object, x );
		GamePlayer_UnlockAction( other_player );
		GamePlayer_ChangeState( other_player, STATE_LYING_HIT);
		GameObject_AtActionDone( other_player->object, ACTION_LYING_HIT, NULL );
		GamePlayer_LockAction( other_player );
		x_speed = XSPEED_JUDO_THROW;
		x_acc = -XACC_JUDO_THROW;
		if( !GamePlayer_IsLeftOriented(player) ) {
			x_speed = 0 - x_speed;
			x_acc = 0 - x_acc;
		}
		GameObject_SetXSpeed( other_player->object, x_speed );
		GameObject_SetXAcc( other_player->object, x_acc );
		GameObject_AtZSpeed(	other_player->object, 
					-ZSPEED_JUDO_THROW/2, 
					ChangeTargetAction );
		GameObject_AtLanding(	other_player->object, 
					ZSPEED_JUDO_THROW, -ZACC_JUDO_THROW, 
					TargetStartLying );
		GameObject_ClearAttack( other_player->object );
		GameObject_AtTouch( other_player->object, BackJudo_ProcTargetTouchAttack );
		break;
	default:break;
	}
}

void StartBackJudo( GamePlayer *player )
{
	int z_index;
	if( !player->other ) {
		return;
	}
	/* 发动技能前，停止移动 */
	GamePlayer_StopXMotion( player );
	GamePlayer_StopYMotion( player );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_CATCH_SKILL_BA );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );

	GameObject_AtActionDone( player->object, ACTION_CATCH_SKILL_BA,
				 SelfAtSkillDone );
	GameObject_AtActionUpdate( player->object, ACTION_CATCH_SKILL_BA,
					BackJudo_AtActionUpdate );
	
	z_index = Widget_GetZIndex( player->object );
	Widget_SetZIndex( player->other->object, z_index-1 );
	player->other->n_attack = 0;
}

/** 注册柔道家特有的技能 */
void JudoMasterSkill_Register(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_JUDO,
				SKILLPRIORITY_JUDO,
				CanUseJudo,
				StartJudo
	);
	SkillLibrary_AddSkill(	SKILLNAME_BACK_JUDO,
				SKILLPRIORITY_BACK_JUDO,
				CanUseBackJudo,
				StartBackJudo
	);
	AttackLibrary_AddAttack( ATK_JUDO, AttackDamage_Judo, NULL );
	AttackLibrary_AddAttack( ATK_BACK_JUDO, AttackDamage_BackJudo, NULL );
}
