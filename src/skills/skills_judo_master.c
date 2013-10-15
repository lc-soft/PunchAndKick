// 柔道家的技能
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include "../game.h"
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
	attacker = GetSpirntAttackerInCatchRange( player );
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
	player = GamePlayer_GetPlayerByWidget( widget );
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
	player = GamePlayer_GetPlayerByWidget( widget );
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

static void Judo_AtActionUpdate( LCUI_Widget *widget )
{
	int z_index;
	double x, z, y;
	GamePlayer *player, *other_player;
	
	player = GamePlayer_GetPlayerByWidget( widget );
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
	player->other->n_attack = 0;
	z_index = Widget_GetZIndex( player->object );
	Widget_SetZIndex( player->other->object, z_index-1 );
	GameObject_AtActionUpdate(	player->object, 
					ACTION_CATCH_SKILL_FA,
					Judo_AtActionUpdate
	);
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockAction( player->other );
	GamePlayer_ChangeState( player, STATE_CATCH_SKILL_FA );
	GamePlayer_ChangeState( player->other, STATE_HIT );
	GamePlayer_LockAction( player->other );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_AtActionDone(	player->object, 
					ACTION_CATCH_SKILL_FA,
					SelfAtSkillDone 
	);
}

static void TargetStartLying( LCUI_Widget *widget )
{
	int ret;
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	if( player->other ) {
		Game_RecordAttack(	player->other, ATK_BACK_JUDO, 
					player, STATE_HIT_FLY_FALL );
		player->other->other = NULL;
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
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_HIT_FLY_FALL );
	GamePlayer_LockAction( player );
}

static void BackJudo_AtActionUpdate( LCUI_Widget *widget )
{
	int z_index;
	double x, z, x_speed, x_acc;
	GamePlayer *player, *other_player;
	
	player = GamePlayer_GetPlayerByWidget( widget );
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
	player->other->n_attack = 0;
	z_index = Widget_GetZIndex( player->object );
	Widget_SetZIndex( player->other->object, z_index-1 );
	GameObject_AtActionUpdate(	player->object, 
					ACTION_CATCH_SKILL_BA,
					BackJudo_AtActionUpdate
	);
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_CATCH_SKILL_BA );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_AtActionDone(	player->object, 
					ACTION_CATCH_SKILL_BA,
					SelfAtSkillDone 
	);
	
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
