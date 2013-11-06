// 格斗家特有技能
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include "../game.h"
#include "game_skill.h"

#define ATK_HUG_FRONT_PUT		"hug front put attack"
#define ATK_HUG_BACK_PUT		"hug back put attack"

#define XSPEED_FRONT_PUT	100
#define ZSPEED_FRONT_PUT	100
#define ZACC_FRONT_PUT		100

#define XSPEED_BOUNCE	100
#define ZSPEED_BOUNCE	100
#define ZACC_BOUNCE	500

static int AttackDamage_HugFrontPut( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	double damage;
	damage = attacker->property.throw;
	damage += (victim->property.max_hp*0.05);
	return DamageReduce( victim, victim_state, (int)damage );
}

static int AttackDamage_HugBackPut( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	double damage, percent;
	damage = attacker->property.throw;
	damage += (victim->property.max_hp*0.05);
	percent = attacker->property.max_hp - attacker->property.cur_hp;
	percent /= attacker->property.max_hp;
	damage += (damage*percent);
	return DamageReduce( victim, victim_state, (int)damage );
}

static LCUI_BOOL CanUseHugFrontPut( GamePlayer *player )
{
	GamePlayer *attacker;
	/* 该技能不能给非格斗家的角色使用 */
	if( player->type != PLAYER_TYPE_FIGHTER ) {
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

static LCUI_BOOL CanUseHugBackPut( GamePlayer *player )
{
	if( player->type != PLAYER_TYPE_FIGHTER ) {
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

static void TargetAtHugFrontPutDone( LCUI_Widget *widget )
{
	int ret = 0;
	GamePlayer *player;

	player = GameBattle_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	ret = GamePlayer_SetLying( player );
	GamePlayer_LockAction( player );
	GamePlayer_StopXMotion( player );
	if( ret == 0 ) {
		GamePlayer_SetRestTimeOut( player, SHORT_REST_TIMEOUT, GamePlayer_StartStand );
	}
}

static void TargetStartLying( GamePlayer *player )
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

static void TargetStartBounce( LCUI_Widget *widget )
{
	double x_speed;
	GamePlayer *player;
	player = GameBattle_GetPlayerByWidget( widget );
	if( GamePlayer_IsLeftOriented(player) ) {
		x_speed = XSPEED_BOUNCE;
	} else {
		x_speed = -XSPEED_BOUNCE;
	}
	GameObject_SetXSpeed( widget, x_speed );
	Game_RecordAttack( player->other, ATK_HUG_FRONT_PUT, player, STATE_HALF_LYING );
	if( player->other ) {
		player->other->other = NULL;
	}
	player->other = NULL;
	GameObject_AtLanding( widget, ZSPEED_BOUNCE, -ZACC_BOUNCE, TargetAtHugFrontPutDone );
}

static void SelfAtSkillDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GameBattle_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
	GamePlayer_SetReady( player );
}

static void HugFrontPut_AtActionUpdate( LCUI_Widget *widget )
{
	int z_index;
	GamePlayer *player;
	double x, y, z, x_speed;
	
	player = GameBattle_GetPlayerByWidget( widget );
	if( !player->other ) {
		return;
	}
	z_index = Widget_GetZIndex( player->object );
	Widget_SetZIndex( player->other->object, z_index-1 );

	switch( GameObject_GetCurrentActionFrameNumber(widget) ) {
	case 1:
		x = GameObject_GetX( player->object );
		y = GameObject_GetY( player->object );
		z = GameObject_GetZ( player->object );
		if( GamePlayer_IsLeftOriented(player) ) {
			GamePlayer_SetLeftOriented( player->other );
			x -= 30;
		} else {
			GamePlayer_SetRightOriented( player->other );
			x += 30;
		}
		GamePlayer_UnlockAction( player->other );
		GamePlayer_ChangeState( player->other, STATE_HIT_FLY_FALL );
		GamePlayer_LockAction( player->other );
		GameObject_SetX( player->other->object, x );
		GameObject_SetY( player->other->object, y );
		GameObject_SetZ( player->other->object, z+20 );
		break;
	case 2:
		x = GameObject_GetX( player->object );
		y = GameObject_GetY( player->object );
		z = GameObject_GetZ( player->object );
		if( GamePlayer_IsLeftOriented(player) ) {
			GamePlayer_SetRightOriented( player->other );
			x += 15;
		} else {
			GamePlayer_SetLeftOriented( player->other );
			x -= 15;
		}
		GamePlayer_UnlockAction( player->other );
		GamePlayer_ChangeState( player->other, STATE_ROLL_UP );
		GamePlayer_LockAction( player->other );
		GameObject_SetX( player->other->object, x );
		GameObject_SetY( player->other->object, y );
		GameObject_SetZ( player->other->object, z+25 );
		break;
	case 3:
		x = GameObject_GetX( player->object );
		y = GameObject_GetY( player->object );
		z = GameObject_GetZ( player->object );
		if( GamePlayer_IsLeftOriented(player) ) {
			GamePlayer_SetRightOriented( player->other );
			x += 25;
		} else {
			GamePlayer_SetLeftOriented( player->other );
			x -= 25;
		}
		GamePlayer_UnlockAction( player->other );
		GamePlayer_ChangeState( player->other, STATE_HALF_STANCE );
		GamePlayer_LockAction( player->other );
		GameObject_SetX( player->other->object, x );
		GameObject_SetY( player->other->object, y );
		GameObject_SetZ( player->other->object, z+20 );
		break;
	case 4:
		x = GameObject_GetX( player->object );
		y = GameObject_GetY( player->object );
		z = GameObject_GetZ( player->object );
		if( GamePlayer_IsLeftOriented(player) ) {
			GamePlayer_SetLeftOriented( player->other );
			x -= 25;
		} else {
			GamePlayer_SetRightOriented( player->other );
			x += 25;
		}
		GamePlayer_UnlockAction( player->other );
		GamePlayer_ChangeState( player->other, STATE_ROLL_UP );
		GamePlayer_LockAction( player->other );
		GameObject_SetX( player->other->object, x );
		GameObject_SetY( player->other->object, y );
		GameObject_SetZ( player->other->object, z+15 );
		break;
	case 5:
		x = GameObject_GetX( player->object );
		y = GameObject_GetY( player->object );
		z = GameObject_GetZ( player->object );
		if( GamePlayer_IsLeftOriented(player) ) {
			x_speed = -XSPEED_FRONT_PUT;
			GamePlayer_SetRightOriented( player->other );
			x -= 40;
		} else {
			GamePlayer_SetLeftOriented( player->other );
			x_speed = XSPEED_FRONT_PUT;
			x += 40;
		}
		GamePlayer_UnlockAction( player->other );
		GamePlayer_ChangeState( player->other, STATE_HALF_LYING );
		GamePlayer_LockAction( player->other );
		GameObject_SetX( player->other->object, x );
		GameObject_SetY( player->other->object, y );
		GameObject_SetZ( player->other->object, z+10 );

		GameObject_SetXSpeed( player->other->object, x_speed );
		GameObject_AtLanding(	player->other->object,
					-ZSPEED_FRONT_PUT, -ZACC_FRONT_PUT, 
					TargetStartBounce );
		break;
	}
}

static void HugBackPut_AtActionUpdate( LCUI_Widget *widget )
{
	int z_index;
	GamePlayer *player;
	double x, y, z;

	player = GameBattle_GetPlayerByWidget( widget );
	if( !player->other ) {
		return;
	}
	z_index = Widget_GetZIndex( player->object );
	Widget_SetZIndex( player->other->object, z_index-1 );

	switch( GameObject_GetCurrentActionFrameNumber(widget) ) {
	//case 1:break;
	case 2:
		x = GameObject_GetX( player->object );
		y = GameObject_GetY( player->object );
		z = GameObject_GetZ( player->object );
		if( GamePlayer_IsLeftOriented(player) ) {
			GamePlayer_SetLeftOriented( player->other );
			x += 30;
		} else {
			GamePlayer_SetRightOriented( player->other );
			x -= 30;
		}
		GamePlayer_UnlockAction( player->other );
		GameObject_AtActionDone( player->other->object, ACTION_LYING_HIT, NULL );
		GamePlayer_ChangeState( player->other, STATE_LYING_HIT );
		GamePlayer_LockAction( player->other );
		GameObject_SetX( player->other->object, x );
		GameObject_SetY( player->other->object, y );
		GameObject_SetZ( player->other->object, z+30 );
		break;
	case 4:
		x = GameObject_GetX( player->object );
		y = GameObject_GetY( player->object );
		z = GameObject_GetZ( player->object );
		if( GamePlayer_IsLeftOriented(player) ) {
			GamePlayer_SetLeftOriented( player->other );
			x += 35;
		} else {
			GamePlayer_SetRightOriented( player->other );
			x -= 35;
		}
		GamePlayer_UnlockAction( player->other );
		GamePlayer_ChangeState( player->other, STATE_HALF_LYING );
		GamePlayer_LockAction( player->other );
		GameObject_SetX( player->other->object, x );
		GameObject_SetY( player->other->object, y );
		GameObject_SetZ( player->other->object, z );
		Game_RecordAttack( player, ATK_HUG_BACK_PUT, player->other, STATE_HALF_LYING );
		GameObject_AtLanding( player->other->object, -100, -ZACC_JUMP, NULL );
		GamePlayer_SetActionTimeOut( player->other, 500, TargetStartLying );
		if( player->other ) {
			player->other->other = NULL;
		}
		player->other = NULL;
	default:
		break;
	}
}

static void StartHugFrontPut( GamePlayer *player )
{
	int z_index;
	double x, y, z;

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
	GamePlayer_ChangeState( player->other, STATE_ROLL_DOWN );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GamePlayer_LockAction( player->other );

	x = GameObject_GetX( player->object );
	y = GameObject_GetY( player->object );
	z = GameObject_GetZ( player->object );
	if( GamePlayer_IsLeftOriented(player) ) {
		GamePlayer_SetRightOriented( player->other );
		x -= 30;
	} else {
		GamePlayer_SetLeftOriented( player->other );
		x += 30;
	}
	GameObject_SetX( player->other->object, x );
	GameObject_SetY( player->other->object, y );
	GameObject_SetZ( player->other->object, z );
	z_index = Widget_GetZIndex( player->object );
	Widget_SetZIndex( player->other->object, z_index-1 );
	GameObject_AtActionDone( player->object, ACTION_CATCH_SKILL_FA, SelfAtSkillDone );
	GameObject_AtActionUpdate( player->object, ACTION_CATCH_SKILL_FA, HugFrontPut_AtActionUpdate );
	/* 重置被攻击的次数 */
	player->other->n_attack = 0;
}

static void StartHugBackPut( GamePlayer *player )
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

	GameObject_AtActionUpdate( player->object, ACTION_CATCH_SKILL_BA, HugBackPut_AtActionUpdate );
	GameObject_AtActionDone( player->object, ACTION_CATCH_SKILL_BA, SelfAtSkillDone );
	
	z_index = Widget_GetZIndex( player->object );
	Widget_SetZIndex( player->other->object, z_index-1 );
	player->other->n_attack = 0;
}

/** 注册格斗家特有的技能 */
void FighterSkill_Register(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_HUG_FRONT_PUT,
				SKILLPRIORITY_HUG_FRONT_PUT,
				CanUseHugFrontPut,
				StartHugFrontPut
	);
	SkillLibrary_AddSkill(	SKILLNAME_HUG_BACK_PUT,
				SKILLPRIORITY_HUG_BACK_PUT,
				CanUseHugBackPut,
				StartHugBackPut
	);
	AttackLibrary_AddAttack( ATK_HUG_FRONT_PUT, AttackDamage_HugFrontPut, NULL );
	AttackLibrary_AddAttack( ATK_HUG_BACK_PUT, AttackDamage_HugBackPut, NULL );
}
