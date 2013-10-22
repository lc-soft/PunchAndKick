// 功夫家特有技能
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include "../game.h"
#include "game_skill.h"

#define ATK_HUG_JUMP			"hug jump attack"
#define ATK_LIFT_JUMP			"lift jump attack"

#define ZSPEED_HUG_JUMP	400
#define ZACC_HUG_JUMP	1000
#define XSPEED_BOUNCE	100
#define ZSPEED_BOUNCE	100
#define ZACC_BOUNCE	500

static int AttackDamage_HugJump( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	double damage;
	damage = victim->property.defense;
	damage += (victim->property.max_hp*0.05);
	return DamageReduce( victim, victim_state, (int)damage );
}

static int AttackDamage_LiftJump( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	double damage;
	damage = victim->property.defense;
	damage += (victim->property.max_hp*0.05);
	damage += attacker->property.throw;
	return DamageReduce( victim, victim_state, (int)damage );
}

static LCUI_BOOL CanUseHugJump( GamePlayer *player )
{
	GamePlayer *attacker;
	/* 该技能不能给非功夫家的角色使用 */
	if( player->type != PLAYER_TYPE_KUNG_FU ) {
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

/** 在技能结束时 */
static void SelfAtSkillDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GameBattle_GetPlayerByWidget( widget );
	if( player->other ) {
		player->other->other = NULL;
	}
	player->other = NULL;
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
	GamePlayer_ResetAttackControl( player );
	GamePlayer_SetReady( player );
}

static void TargetAtHugJumpDone( LCUI_Widget *widget )
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
	GameObject_AtLanding( widget, ZSPEED_BOUNCE, -ZACC_BOUNCE, TargetAtHugJumpDone );
}

static void SelfStartBounce( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GameBattle_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_SQUAT );
	GameObject_AtActionDone( widget, ACTION_SQUAT, NULL );
	GamePlayer_LockAction( player );
	GameObject_AtLanding( widget, ZSPEED_BOUNCE, -ZACC_BOUNCE, SelfAtSkillDone );
	Game_RecordAttack( player, ATK_HUG_JUMP, player->other, STATE_HALF_LYING );
}

static void ChangeTargetAction( GamePlayer *player )
{
	if( !player->other ) {
		return;
	}
	GamePlayer_UnlockAction( player->other );
	GamePlayer_ChangeState( player->other, STATE_HALF_LYING );
	GamePlayer_UnlockAction( player->other );
}

static void GamePlayer_AtHugJumpUpdate( LCUI_Widget *widget )
{
	GamePlayer *player;
	
	player = GameBattle_GetPlayerByWidget( widget );
	if( GameObject_GetCurrentActionFrameNumber( player->object) != 3 ) {
		return;
	}
	GameObject_AtLanding( widget, ZSPEED_HUG_JUMP, -ZACC_HUG_JUMP, SelfStartBounce );
	GameObject_AtLanding( player->other->object, ZSPEED_HUG_JUMP, -ZACC_HUG_JUMP, TargetStartBounce );
	GameObject_AtActionUpdate( player->object, ACTION_CATCH_SKILL_FA, NULL );
}

static void StartHugJump( GamePlayer *player )
{
	double x;
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
	GamePlayer_ChangeState( player->other, STATE_ROLL_DOWN );
	GamePlayer_ChangeState( player, STATE_CATCH_SKILL_FA );
	GamePlayer_LockAction( player );
	GamePlayer_LockAction( player->other );
	GamePlayer_LockMotion( player->other );

	GameObject_AtActionUpdate(	player->object,
					ACTION_CATCH_SKILL_FA, 
					GamePlayer_AtHugJumpUpdate );
	GamePlayer_SetActionTimeOut( player, 200, ChangeTargetAction );
	/* 调整对方的位置 */
	x = GameObject_GetX( player->object );
	if( GamePlayer_IsLeftOriented(player) ) {
		x -= 15;
	} else {
		x += 15;
	}
	GameObject_SetX( player->other->object, x );
	z_index = Widget_GetZIndex( player->object );
	/* 被攻击者需要显示在攻击者后面 */
	Widget_SetZIndex( player->other->object, z_index-1 );
	/* 重置被攻击的次数 */
	player->other->n_attack = 0;
}

static LCUI_BOOL CanUseLiftJump( GamePlayer *player )
{
	if( player->type != PLAYER_TYPE_KUNG_FU ) {
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

static void ChangeSelfActionToLiftFall( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_LIFT_FALL );
	GamePlayer_LockAction( player );
}

/** 在技能结束时 */
static void SelfAtLiftJumpDone( LCUI_Widget *widget )
{
	GamePlayer *player;

	player = GameBattle_GetPlayerByWidget( widget );
	GamePlayer_SetActionTimeOut( player, 100, GamePlayer_StartStand );
	if( player->other ) {
		Game_RecordAttack( player, ATK_LIFT_JUMP, player->other, STATE_LYING );
		GamePlayer_SetHit( player->other );
	}
}

/** 在技能发动者达到最高处时降落 */
static void SelfStartFall( LCUI_Widget *widget )
{
	double z;
	int z_index, h1, h2;
	GamePlayer *player;

	player = GameBattle_GetPlayerByWidget( widget );
	if( !player->other ) {
		GamePlayer_SetFall( player );
	}
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_HALF_LYING );
	GamePlayer_LockAction( player );
	h1 = GameObject_GetFrameTop( player->object, ACTION_LIFT_JUMP, 0 );
	h2 = GameObject_GetFrameTop( player->other->object, ACTION_LYING, 0 );
	z = GameObject_GetZ( player->object );
	GameObject_SetZ( player->object, z+h1/2 );
	GameObject_SetZ( player->other->object, z+h1/2-h2 );
	z_index = Widget_GetZIndex( player->object );
	/* 被攻击者需要显示在攻击者后面 */
	Widget_SetZIndex( player->other->object, z_index-1 );
	GameObject_AtZeroZSpeed( player->object, NULL );
	GameObject_AtLanding( player->object, -ZSPEED_JUMP, -5*ZACC_JUMP, SelfAtLiftJumpDone );
	GameObject_AtLanding( player->other->object, -ZSPEED_JUMP, -5*ZACC_JUMP, NULL );
}

/* 开始发动技能 */
static void StartLiftJump( GamePlayer *player )
{
	double x, y, z;
	/* 发动技能前，停止移动 */
	GamePlayer_StopXMotion( player );
	GamePlayer_StopYMotion( player );

	GamePlayer_UnlockAction( player->other );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player->other, STATE_LYING );
	GamePlayer_ChangeState( player, STATE_LIFT_JUMP );
	GamePlayer_LockAction( player->other );
	GamePlayer_LockAction( player );

	GamePlayer_SetRestTimeOut( player->other, SHORT_REST_TIMEOUT, GamePlayer_StartStand );
	x = GameObject_GetX( player->object );
	y = GameObject_GetY( player->object );
	z = GameObject_GetZ( player->object );
	z += GameObject_GetFrameTop( player->object, ACTION_LIFT_JUMP, 0 );
	GameObject_SetX( player->other->object, x );
	GameObject_SetY( player->other->object, y );
	GameObject_SetZ( player->other->object, z );
	GameObject_AtLanding( player->object, ZSPEED_JUMP, -ZACC_JUMP, NULL );
	GameObject_AtLanding( player->other->object, ZSPEED_JUMP, -ZACC_JUMP, NULL );
	GamePlayer_SetActionTimeOut( player, 100, ChangeSelfActionToLiftFall );
	GameObject_AtZeroZSpeed( player->object, SelfStartFall );
}

/** 注册功夫家特有的技能 */
void KungFuSkill_Register(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_HUG_JUMP,
				SKILLPRIORITY_HUG_JUMP,
				CanUseHugJump,
				StartHugJump
	);
	SkillLibrary_AddSkill(	SKILLNAME_LIFT_JUMP,
				SKILLPRIORITY_LIFT_JUMP,
				CanUseLiftJump,
				StartLiftJump
	);
	AttackLibrary_AddAttack( ATK_HUG_JUMP, AttackDamage_HugJump, NULL );
	AttackLibrary_AddAttack( ATK_LIFT_JUMP, AttackDamage_LiftJump, NULL );
}
