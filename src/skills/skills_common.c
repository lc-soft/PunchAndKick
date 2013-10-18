// 通用技能
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include "../game.h"
#include "game_skill.h"

#define XSPEED_SPINHIT	600
#define ZSPEED_SPINHIT	200
#define ZACC_SPINHIT	550

#define XSPEED_BOMBKICK	800
#define ZSPEED_BOMBKICK	200
#define ZACC_BOMBKICK	800

#define ZSPEED_BOUNCE	200
#define ZACC_BOUNCE	800

#define ROLL_TIMEOUT	200

#define ZSPEED_SPINHIT_2	550
#define ZACC_SPINHIT_2		1000
#define XSPEED_SPINHIT_2	300

/** 可擒获范围的水平宽度 */
#define CATCH_RANGE_X_WIDTH	50

#define XSPEED_BE_PUSH	100
#define YSPEED_BE_PUSH	100
#define YACC_BE_PUSH	1000

/** 计算A攻击的伤害值 */
static int AttackDamage_AAttack( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = attacker->property.punch/10;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

/** 计算B攻击的伤害值 */
static int AttackDamage_BAttack( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = attacker->property.kick/10;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

/** 计算A攻击的伤害值 */
static int AttackDamage_MachAAttack( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = attacker->property.punch/20;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

/** 计算B攻击的伤害值 */
static int AttackDamage_MachBAttack( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = attacker->property.kick/20;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static int AttackDamage_SprintAAttack( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = attacker->property.punch/3;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static int AttackDamage_SprintBAttack( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = attacker->property.kick/3;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static int AttackDamage_FinalBlow( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = attacker->property.punch/4;
	damage += attacker->property.kick/4;
	damage *= 2;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static int AttackDamage_SprintJumpAAttack( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = attacker->property.punch/3;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static int AttackDamage_SprintJumpBAttack( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = attacker->property.kick/2;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static int AttackDamage_Bumped( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = 50;
	if( attacker ) {
		damage += attacker->property.defense/2;
	}
	return DamageReduce( victim, victim_state, damage );;
}

static int AttackDamage_MachStomp( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = attacker->property.kick/6;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static int AttackDamage_JumpTread( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = attacker->property.kick/6;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static int AttackDamage_JumpElbow( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = attacker->property.punch/6;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static int AttackDamage_BombKick( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = 50 + attacker->property.kick/4;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static int AttackDamage_SpinHit( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = 30 + attacker->property.kick/4;
	damage += attacker->property.punch/4;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static int AttackDamage_Throw( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = 20 + attacker->property.throw/2;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static int AttackDamage_RideAAttack( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = 10 + attacker->property.punch/8;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static int AttackDamage_RideBAttack( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = 20 + (attacker->property.punch + attacker->property.kick)/8;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static int AttackDamage_BigElbow( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = 20 + attacker->property.punch/3;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static int AttackDamage_Guillotine( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = 20 + attacker->property.kick/3;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static int AttackDamage_JumpSpinKick( GamePlayer *attacker, GamePlayer *victim, int victim_state )
{
	int damage;
	damage = 40 + attacker->property.kick/4;
	damage = DamageReduce( victim, victim_state, damage );
	return damage;
}

static void GamePlayer_AtLandingDone( LCUI_Widget *widget );
static void CommonSkill_StartNormalJump( GamePlayer *player );
static void AttackEffect_ShortHitFly( GamePlayer *attacker, GamePlayer *victim );

static void GamePlayer_AtHitDone( LCUI_Widget *widget )
{
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	switch( player->state ) {
	case STATE_LYING_HIT:
		GamePlayer_SetLying( player );
		break;
	case STATE_TUMMY_HIT:
		GamePlayer_SetTummy( player );
		break;
	default:
		if( player->n_attack >= 4 ) {
			GamePlayer_SetRest( player );
		} else {
			GamePlayer_UnlockAction( player );
			GamePlayer_UnlockMotion( player );
			GamePlayer_SetReady( player );
		}
		break;
	}
}

/** 重置角色的受攻击次数 */
static void GamePlayer_ResetCountAttack( GamePlayer *player )
{
	player->n_attack = 0;
}

/** 检测目标游戏角色是否能够应用攻击效果 */
static LCUI_BOOL GamePlayer_CanUseAttackEffect( GamePlayer *player )
{
	switch( player->state ) {
	case STATE_LYING:
	case STATE_LYING_HIT:
	case STATE_TUMMY:
	case STATE_TUMMY_HIT:
	case STATE_B_ROLL:
	case STATE_F_ROLL:
	case STATE_HIT_FLY:
	case STATE_HIT_FLY_FALL:
	case STATE_DEFENSE:
	case STATE_SOLID_DEFENSE:
	case STATE_BE_PUSH:
		break;
	default:
		return TRUE;
	}
	return FALSE;
}

/** 打断技能的效果 */
static void GamePlayer_BreakSkillEffect( GamePlayer *player )
{
	GameObject_AtZeroZSpeed( player->object, NULL );
	GameObject_AtZSpeed( player->object, 0, NULL );
}

/** 若指定游戏角色擒住了另一角色，则撤销擒住状态 */
static void GamePlayer_CancelStateAtCatch( GamePlayer *player )
{
	GamePlayer *other_player;
	
	other_player = player->other;
	if( !other_player ) {
		return;
	}
	switch(player->state) {
	case STATE_CATCH:
		other_player->other = NULL;
		player->other = NULL;
		GamePlayer_SetRest( other_player );
		break;
	case STATE_CATCH_SKILL_BA:
	case STATE_CATCH_SKILL_FA:
		other_player->other = NULL;
		player->other = NULL;
		GamePlayer_UnlockAction( other_player );
		GamePlayer_ChangeState( other_player, STATE_REST );
		GamePlayer_LockAction( other_player );
		AttackEffect_ShortHitFly( player, other_player );
		break;
	default:return;
	}
}

/** 若指定游戏角色被擒住，则撤销被擒住状态 */
static void GamePlayer_CancelStateAtBeCatch( GamePlayer *player )
{
	GamePlayer *other_player;

	switch(player->state) {
	case STATE_BE_CATCH:
	case STATE_BACK_BE_CATCH:
		break;
	default:return;
	}
	other_player = player->other;
	if( !other_player ) {
		return;
	}
	switch(other_player->state) {
	case STATE_CATCH:
		other_player->other = NULL;
		player->other = NULL;
		GamePlayer_UnlockAction( other_player );
		GamePlayer_UnlockMotion( other_player );
		GamePlayer_SetReady( other_player );
		break;
	case STATE_CATCH_SKILL_BA:
	case STATE_CATCH_SKILL_FA:
		other_player->other = NULL;
		player->other = NULL;
		GamePlayer_UnlockAction( other_player );
		GamePlayer_ChangeState( other_player, STATE_REST );
		GamePlayer_LockAction( other_player );
		AttackEffect_ShortHitFly( other_player, player );
	default:break;
	}
}

/** 在空中受到攻击后 */
static void GamePlayer_AtAirHitDone( LCUI_Widget *widget )
{
	GamePlayer_SetFall( GamePlayer_GetPlayerByWidget( widget ) );
}

/** 设置游戏角色被攻击命中 */
void GamePlayer_SetHit( GamePlayer *player )
{
	switch( player->state ) {
	case STATE_LYING:
	case STATE_LYING_HIT:
		player->n_attack = 0;
		GamePlayer_UnlockAction( player );
		GamePlayer_ChangeState( player, STATE_LYING_HIT );
		GamePlayer_LockAction( player );
		GameObject_AtActionDone( player->object, ACTION_LYING_HIT, GamePlayer_AtHitDone );
		return;
	case STATE_TUMMY:
	case STATE_TUMMY_HIT:
		player->n_attack = 0;
		GamePlayer_UnlockAction( player );
		GamePlayer_ChangeState( player, STATE_TUMMY_HIT );
		GamePlayer_LockAction( player );
		GameObject_AtActionDone( player->object, ACTION_TUMMY_HIT, GamePlayer_AtHitDone );
		return;
	case STATE_BE_LIFT_LYING:
	case STATE_BE_LIFT_LYING_HIT:
		player->n_attack = 0;
		GamePlayer_UnlockAction( player );
		GamePlayer_ChangeState( player, STATE_BE_LIFT_LYING_HIT );
		GamePlayer_LockAction( player );
		GameObject_AtActionDone( player->object, ACTION_LYING_HIT, GamePlayer_AtHitDone );
		return;
	case STATE_BE_LIFT_TUMMY:
	case STATE_BE_LIFT_TUMMY_HIT:
		player->n_attack = 0;
		GamePlayer_UnlockAction( player );
		GamePlayer_ChangeState( player, STATE_BE_LIFT_TUMMY_HIT );
		GamePlayer_LockAction( player );
		GameObject_AtActionDone( player->object, ACTION_TUMMY_HIT, GamePlayer_AtHitDone );
		return;
	case STATE_B_ROLL:
	case STATE_F_ROLL:
	case STATE_HIT_FLY:
	case STATE_HIT_FLY_FALL:
	case STATE_DEFENSE:
	case STATE_SOLID_DEFENSE:
	case STATE_BE_PUSH:
		player->n_attack = 0;
		return;
	default:break;
	}
	GamePlayer_StopYMotion( player );
	GamePlayer_StopXMotion( player );
	GamePlayer_CancelStateAtCatch( player );
	GamePlayer_CancelStateAtBeCatch( player );
	GamePlayer_BreakSkillEffect( player );
	GamePlayer_UnlockAction( player );
	/* 如果Z轴坐标大于0，说明在空中 */
	if( GameObject_GetZ(player->object) > 0 ) {
		double z_speed;
		z_speed = GameObject_GetZSpeed( player->object );
		GameObject_AtActionDone( player->object, ACTION_HIT, GamePlayer_AtAirHitDone );
		GameObject_AtLanding( player->object, z_speed, -ZACC_JUMP, GamePlayer_AtLandingDone );
	} else {
		GameObject_AtActionDone( player->object, ACTION_HIT, GamePlayer_AtHitDone );
	}
	GamePlayer_ChangeState( player, STATE_HIT );
	GamePlayer_LockAction( player );
	GamePlayer_SetRestTimeOut( player, 2000, GamePlayer_ResetCountAttack );
}

static void AttackEffect_BePushed( GamePlayer *player );

/** 普通攻击产生的效果 */
static void AttackEffect_Normal( GamePlayer *attacker, GamePlayer *victim )
{
	if( (victim->state == STATE_A_ATTACK 
	 || victim->state == STATE_B_ATTACK
	 || victim->state ==  STATE_MACH_A_ATTACK
	 || victim->state ==  STATE_MACH_B_ATTACK)
	 && GamePlayer_IsLeftOriented(attacker)
	 == GamePlayer_IsLeftOriented(victim) ) {
		victim->n_attack = 0;
		AttackEffect_BePushed( victim );
		return;
	}
	++victim->n_attack;
	GamePlayer_SetHit( victim );
}

/** 若指定游戏角色被举起，则撤销被举起状态 */
static void GamePlayer_CancelStateAtBeLift( GamePlayer *player )
{
	double z_speed, z_acc;
	GamePlayer *other_player;

	switch(player->state) {
	case STATE_BE_LIFT_LYING:
	case STATE_BE_LIFT_LYING_HIT:
	case STATE_BE_LIFT_TUMMY:
	case STATE_BE_LIFT_TUMMY_HIT:
	case STATE_BE_LIFT_SQUAT:
	case STATE_BE_LIFT_STANCE:
		break;
	default:return;
	}

	other_player = player->other;
	/* 如果没有举起者的记录 */
	if( !other_player ) {
		GamePlayer_SetFall( player );
		return;
	}
	z_speed = GameObject_GetZSpeed( other_player->object );
	z_acc = GameObject_GetZAcc( other_player->object );
	player->other = NULL;
	other_player->other = NULL;
	/* 根据举起者当前的状态，切换至相应的普通状态 */
	switch(other_player->state) {
	case STATE_LIFT_FALL:
		GamePlayer_UnlockAction( other_player );
		GamePlayer_ChangeState( other_player, STATE_FALL );
		GamePlayer_LockAction( other_player );
		GameObject_AtLanding( other_player->object, z_speed, z_acc, GamePlayer_AtLandingDone );
		break;
	case STATE_LIFT_JUMP:
		GamePlayer_UnlockAction( other_player );
		GamePlayer_ChangeState( other_player, STATE_JUMP );
		GamePlayer_LockAction( other_player );
		GameObject_AtLanding( other_player->object, z_speed, z_acc, GamePlayer_AtLandingDone );
		break;
	case STATE_LIFT_RUN:
		if( GamePlayer_IsLeftOriented(other_player) ) {
			GamePlayer_ChangeState( other_player, STATE_LEFTRUN );
		} else {
			GamePlayer_ChangeState( other_player, STATE_RIGHTRUN );
		}
		break;
	case STATE_LIFT_SQUAT:
		GamePlayer_StartStand( player );
		break;
	case STATE_LIFT_STANCE:
		GamePlayer_UnlockAction( other_player );
		GamePlayer_SetReady( other_player );
		break;
	case STATE_LIFT_WALK:
		GamePlayer_UnlockAction( other_player );
		GamePlayer_ChangeState( other_player, STATE_WALK );
	default:break;
	}
}

static void GamePlayer_AtHitFlyDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	/* 停止移动 */
	GameObject_SetYSpeed( widget, 0 );
	GameObject_SetXSpeed( widget, 0 );
	GameObject_SetZSpeed( widget, 0 );
	
	if( GamePlayer_SetLying( player ) == 0 ) {
		GamePlayer_SetRestTimeOut( 
			player, SHORT_REST_TIMEOUT,
			GamePlayer_StartStand 
		);
	}
}

void GamePlayer_AtFrontalHitFlyDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	if( GamePlayer_IsLeftOriented(player) ) {
		GameObject_SetXSpeed( player->object, XSPEED_X_HIT_FLY2 );
	} else {
		GameObject_SetXSpeed( player->object, -XSPEED_X_HIT_FLY2 );
	}
	GameObject_AtLanding(	player->object, ZSPEED_XF_HIT_FLY2,
				-ZACC_XF_HIT_FLY2, GamePlayer_AtHitFlyDone );
}

static void GamePlayer_AtHitFlyFall( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_HIT_FLY_FALL );
	GamePlayer_LockAction( player );
	GameObject_AtZSpeed( widget, 0, NULL );
}

/** 在接近最大高度时，调整身体为平躺式 */
static void GamePlayer_AtHitFlyMaxHeight( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_LYING_HIT );
	GamePlayer_LockAction( player );
	GameObject_AtZSpeed( widget, -20, GamePlayer_AtHitFlyFall );
}

/** 在缓冲完攻击者的碰撞攻击时，解除僵直状态 */
static void GameObject_AtBumpBufferDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
}

/** 向左远距离击飞 */
static void AttackEffect_LeftLongHitFly( GamePlayer *player )
{
	if( !GamePlayer_CanUseAttackEffect(player) ) {
		double speed;
		GamePlayer_SetHit( player );
		GamePlayer_LockAction( player );
		GamePlayer_LockMotion( player );
		speed = -XSPEED_X_HIT_FLY * 0.5;
		speed += GameObject_GetXSpeed( player->object );
		GameObject_SetXSpeed( player->object, speed );
		GameObject_AtXSpeedToZero( player->object,-speed*5, GameObject_AtBumpBufferDone );
		return;
	}
	GamePlayer_CancelStateAtBeLift( player );
	GamePlayer_CancelStateAtCatch( player );
	GamePlayer_CancelStateAtBeCatch( player );
	GamePlayer_BreakSkillEffect( player );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_HIT_FLY );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_SetXAcc( player->object, 0 );
	GameObject_SetXSpeed( player->object, -XSPEED_X_HIT_FLY );
	/* 如果游戏角色面向左方 */
	if( GamePlayer_IsLeftOriented( player ) ) {
		GameObject_AtLanding(
			player->object,
			ZSPEED_XB_HIT_FLY, -ZACC_XB_HIT_FLY,
			GamePlayer_AtHitFlyDone
		);
		return;
	}
	GameObject_AtZeroZSpeed( player->object, NULL );
	GameObject_AtZSpeed( player->object, 20, GamePlayer_AtHitFlyMaxHeight );
	GameObject_AtLanding(
		player->object,
		ZSPEED_XF_HIT_FLY, -ZACC_XF_HIT_FLY,
		GamePlayer_AtFrontalHitFlyDone 
	);
}

/** 向右远距离击飞 */
static void AttackEffect_RightLongHitFly( GamePlayer *player )
{
	if( !GamePlayer_CanUseAttackEffect(player) ) {
		double speed;
		GamePlayer_SetHit( player );
		GamePlayer_LockAction( player );
		GamePlayer_LockMotion( player );
		speed = XSPEED_X_HIT_FLY * 0.5;
		speed += GameObject_GetXSpeed( player->object );
		GameObject_SetXSpeed( player->object, speed );
		GameObject_AtXSpeedToZero( player->object,-speed*5, GameObject_AtBumpBufferDone );
		return;
	}
	GamePlayer_CancelStateAtBeLift( player );
	GamePlayer_CancelStateAtCatch( player );
	GamePlayer_CancelStateAtBeCatch( player );
	GamePlayer_BreakSkillEffect( player );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_HIT_FLY );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_SetXAcc( player->object, 0 );
	GameObject_SetXSpeed( player->object, XSPEED_X_HIT_FLY );
	/* 如果游戏角色面向右方 */
	if( !GamePlayer_IsLeftOriented( player ) ) {
		GameObject_AtLanding(
			player->object,
			ZSPEED_XB_HIT_FLY, -ZACC_XB_HIT_FLY,
			GamePlayer_AtHitFlyDone
		);
		return;
	}
	GameObject_AtZeroZSpeed( player->object, NULL );
	GameObject_AtZSpeed( player->object, 20, GamePlayer_AtHitFlyMaxHeight );
	GameObject_AtLanding(
		player->object,
		ZSPEED_XF_HIT_FLY, -ZACC_XF_HIT_FLY,
		GamePlayer_AtFrontalHitFlyDone 
	);
}

static int CommonSkill_StartSecondSpinHit( GamePlayer *player );

static void AttackEffect_LongHitFly( GamePlayer *attacker, GamePlayer *victim )
{
	double attacker_speed;

	attacker_speed = GameObject_GetXSpeed( attacker->object );
	/* 如果当前的攻击类型是否为 自旋击 */
	if( strcmp( attacker->attack_type_name, ATK_SPINHIT ) == 0 ) {
		/* 如果能够使用二段自旋击 */
		if( CommonSkill_StartSecondSpinHit( attacker ) == 0 ) {
			goto skip_speed_reduce;
		}
	}
	/* 根据攻击者面向的方向，以及移动方向，判定是否需要对攻击者进行减速
	 * 主要是为了在多个角色互相攻击都被击飞时，不对击飞速度进行减速
	 */
	if( (attacker_speed > 0 && !GamePlayer_IsLeftOriented(attacker))
	 || (attacker_speed < 0 && GamePlayer_IsLeftOriented(attacker))) {
		 GamePlayer_ReduceSpeed( attacker, 50 );
	}
skip_speed_reduce:
	/* 根据攻击者面向的方向，判定受攻击者的击飞方向 */
	if( GamePlayer_IsLeftOriented(attacker) ) {
		AttackEffect_LeftLongHitFly( victim );
		return;
	}
	AttackEffect_RightLongHitFly( victim );
}

/** 向左短距离击飞 */
static void AttackEffect_LeftShortHitFly( GamePlayer *player )
{
	if( !GamePlayer_CanUseAttackEffect( player ) ) {
		double speed;
		GamePlayer_SetHit( player );
		GamePlayer_LockAction( player );
		GamePlayer_LockMotion( player );
		speed = -XSPEED_HIT_FLY * 0.5;
		speed += GameObject_GetXSpeed( player->object );
		GameObject_SetXSpeed( player->object, speed );
		GameObject_AtXSpeedToZero( player->object,-speed*5, GameObject_AtBumpBufferDone );
		return;
	}
	GamePlayer_CancelStateAtBeLift( player );
	GamePlayer_CancelStateAtCatch( player );
	GamePlayer_CancelStateAtBeCatch( player );
	GamePlayer_BreakSkillEffect( player );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_HIT_FLY );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_SetXAcc( player->object, 0 );
	GameObject_SetXSpeed( player->object, -XSPEED_HIT_FLY );
	GameObject_AtLanding(
		player->object,
		ZSPEED_HIT_FLY, -ZACC_HIT_FLY, 
		GamePlayer_AtHitFlyDone
	);
}

/** 向右短距离击飞 */
static void AttackEffect_RightShortHitFly( GamePlayer *player )
{
	if( !GamePlayer_CanUseAttackEffect( player ) ) {
		double speed;
		GamePlayer_SetHit( player );
		GamePlayer_LockAction( player );
		GamePlayer_LockMotion( player );
		speed = XSPEED_HIT_FLY * 0.5;
		speed += GameObject_GetXSpeed( player->object );
		GameObject_SetXSpeed( player->object, speed );
		GameObject_AtXSpeedToZero( player->object,-speed*5, GameObject_AtBumpBufferDone );
		return;
	}
	GamePlayer_CancelStateAtBeLift( player );
	GamePlayer_CancelStateAtCatch( player );
	GamePlayer_CancelStateAtBeCatch( player );
	GamePlayer_BreakSkillEffect( player );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_HIT_FLY );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_SetXAcc( player->object, 0 );
	GameObject_SetXSpeed( player->object, XSPEED_HIT_FLY );
	GameObject_AtLanding(
		player->object,
		ZSPEED_HIT_FLY, -ZACC_HIT_FLY, 
		GamePlayer_AtHitFlyDone
	);
}

static void AttackEffect_TouchHitFly( GamePlayer *attacker, GamePlayer *victim )
{
	double attacker_x, victim_x;

	victim_x = GameObject_GetX( victim->object );
	if( attacker ) {
		attacker_x = GameObject_GetX( attacker->object );
		GamePlayer_ReduceSpeed( attacker, 50 );
	} else {
		attacker_x = victim_x;
	}
	/* 如果攻击者在受攻击者的右方 */
	if( attacker_x > victim_x+5 ) {
		/* 将受攻击者向左方短距离击飞 */
		AttackEffect_LeftShortHitFly( victim );
		return;
	}
	else if( attacker_x < victim_x-5 ) {
		AttackEffect_RightShortHitFly( victim );
		return;
	}
	/* 距离在10以内，则由受攻击者面向的方向决定击飞方向 */
	if( GamePlayer_IsLeftOriented(victim) ) {
		AttackEffect_RightShortHitFly( victim );
		return;
	}
	AttackEffect_LeftShortHitFly( victim );
}

void AttackEffect_ShortHitFly( GamePlayer *attacker, GamePlayer *victim )
{
	double attacker_speed;

	attacker_speed = GameObject_GetXSpeed( attacker->object );
	/* 检测该游戏角色是否有TORNADO_ATTACK技能 */
	if( GamePlayer_HaveSkill( attacker, SKILLNAME_TORNADO_ATTACK ) ) {
		/* 检测攻击类型是否为ATK_SPRINT_JUMP_B_ATTACK */
		if( strcmp( attacker->attack_type_name, ATK_SPRINT_JUMP_B_ATTACK ) == 0 ) {
			CommonSkill_StartSecondSpinHit( attacker );
			goto skip_speed_reduce;
		}
	}
	/* 根据攻击者面向的方向，以及移动方向，判定是否需要对攻击者进行减速
	 * 主要是为了在多个角色互相攻击都被击飞时，不对击飞速度进行减速
	 */
	if( (attacker_speed > 0 && !GamePlayer_IsLeftOriented(attacker))
	 || (attacker_speed < 0 && GamePlayer_IsLeftOriented(attacker))) {
		 GamePlayer_ReduceSpeed( attacker, 50 );
	}
skip_speed_reduce:
	/* 根据攻击者面向的方向，判定受攻击者的击飞方向 */
	if( GamePlayer_IsLeftOriented(attacker) ) {
		AttackEffect_LeftShortHitFly( victim );
		return;
	}
	AttackEffect_RightShortHitFly( victim );
}

/** 普通攻击产生的效果（若对方处于喘气状态，则击飞对方） */
static void AttackEffect_Normal2( GamePlayer *attacker, GamePlayer *victim )
{
	GamePlayer_SetHit( victim );
	if( !GamePlayer_CanUseAttackEffect(victim) ) {
		return;
	}
	/* 若当前玩家处于歇息状态，则将其击飞 */
	if( victim->n_attack >= 4
	 || victim->state == STATE_REST ) {
		victim->n_attack = 0;
		AttackEffect_ShortHitFly( attacker, victim );
	} else {
		++victim->n_attack;
	}
}

/** 向后翻滚超时后 */
static void GamePlayer_AtBackwardRollTimeOut( GamePlayer *player )
{
	GameObject_SetXSpeed( player->object, 0 );
	GameObject_SetYSpeed( player->object, 0 );
	GameObject_SetYAcc( player->object, 0 );
	GameObject_SetXAcc( player->object, 0 );
	if( GamePlayer_SetLying( player ) == 0 ) {
		GamePlayer_SetRestTimeOut( 
			player, LONG_REST_TIMEOUT, 
			GamePlayer_StartStand 
		);
	}
}

/** 向前翻滚超时后 */
static void GamePlayer_AtForwardRollTimeOut( GamePlayer *player )
{
	GameObject_SetXSpeed( player->object, 0 );
	GameObject_SetYSpeed( player->object, 0 );
	GameObject_SetYAcc( player->object, 0 );
	GameObject_SetXAcc( player->object, 0 );
	if( GamePlayer_SetTummy( player ) == 0 ) {
		GamePlayer_SetRestTimeOut( 
			player, LONG_REST_TIMEOUT, 
			GamePlayer_StartStand 
		);
	}
}

/** 开始朝左边进行前翻滚 */
static void GamePlayer_StartLeftForwardRoll( LCUI_Widget *widget )
{
	double speed;
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_SetLeftOriented( player );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_F_ROLL );
	GamePlayer_LockAction( player );
	/* 一边滚动，一边减速 */
	speed = GameObject_GetXSpeed( player->object );
	GameObject_SetXAcc( player->object, -speed*4 );
	GamePlayer_SetActionTimeOut( player, ROLL_TIMEOUT, GamePlayer_AtForwardRollTimeOut );
}

/** 开始朝右边进行前翻滚 */
static void GamePlayer_StartRightForwardRoll( LCUI_Widget *widget )
{
	double speed;
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_SetRightOriented( player );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_F_ROLL );
	GamePlayer_LockAction( player );
	speed = GameObject_GetXSpeed( player->object );
	GameObject_SetXAcc( player->object, -speed*4 );
	GamePlayer_SetActionTimeOut( player, ROLL_TIMEOUT, GamePlayer_AtForwardRollTimeOut );
}

/** 开始朝左边进行后翻滚 */
static void GamePlayer_StartLeftBackwardRoll( LCUI_Widget *widget )
{
	double speed;
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_B_ROLL );
	GamePlayer_LockAction( player );
	speed = GameObject_GetXSpeed( player->object );
	GameObject_SetXAcc( player->object, -speed*4 );
	GamePlayer_SetActionTimeOut( player, ROLL_TIMEOUT, GamePlayer_AtBackwardRollTimeOut );
}

/** 开始朝右边进行后翻滚 */
static void GamePlayer_StartRightBackwardRoll( LCUI_Widget *widget )
{
	double speed;
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_B_ROLL );
	GamePlayer_LockAction( player );
	speed = GameObject_GetXSpeed( player->object );
	GameObject_SetXAcc( player->object, -speed*4 );
	GamePlayer_SetActionTimeOut( player, ROLL_TIMEOUT, GamePlayer_AtBackwardRollTimeOut );
}

/** 撞飞 */
static void AttackEffect_BumpToFly( GamePlayer *attacker, GamePlayer *victim )
{
	double victim_speed, attacker_speed;
	/* 如果受攻击者对攻击者使用了擒制技能，那么此攻击效果无效 */
	if( victim->state == STATE_CATCH_SKILL_FA
	 && victim->other == attacker ) {
		 return;
	}
	attacker_speed = GameObject_GetXSpeed( attacker->object );
	if( (attacker_speed > 0 && !GamePlayer_IsLeftOriented(attacker))
	 || (attacker_speed < 0 && GamePlayer_IsLeftOriented(attacker))) {
		 GamePlayer_ReduceSpeed( attacker, 50 );
	}

	if( !GamePlayer_CanUseAttackEffect(victim) ) {
		GamePlayer_SetHit(victim);
		GamePlayer_LockAction( victim );
		GamePlayer_LockMotion( victim );
		victim_speed = GameObject_GetXSpeed( victim->object );
		victim_speed += attacker_speed;
		GameObject_SetXSpeed( victim->object, victim_speed );
		GameObject_AtXSpeedToZero( victim->object,-victim_speed*5, GameObject_AtBumpBufferDone );
		return;
	}
	GamePlayer_CancelStateAtBeLift( victim );
	GamePlayer_CancelStateAtCatch( victim );
	GamePlayer_CancelStateAtBeCatch( victim );
	GamePlayer_BreakSkillEffect( victim );
	GamePlayer_UnlockAction( victim );
	GamePlayer_ChangeState( victim, STATE_HIT_FLY );
	GamePlayer_LockAction( victim );
	GamePlayer_LockMotion( victim );
	GameObject_SetXAcc( victim->object, 0 );
	if( GamePlayer_IsLeftOriented(attacker) ) {
		GameObject_SetXSpeed( victim->object, -XSPEED_S_HIT_FLY );
	} else {
		GameObject_SetXSpeed( victim->object, XSPEED_S_HIT_FLY );
	}
	/* 如果两者朝向相同 */
	if( GamePlayer_IsLeftOriented( attacker )
	 == GamePlayer_IsLeftOriented( victim ) ) {
		 /* 如果攻击者朝向左方 */
		if( GamePlayer_IsLeftOriented(attacker) ) {
			/* 落地时开始向左边进行 前翻滚 */
			GameObject_AtLanding(
				victim->object,
				ZSPEED_S_HIT_FLY, -ZACC_S_HIT_FLY,
				GamePlayer_StartLeftForwardRoll
			);
		} else {
			/* 落地时开始向右边进行 前翻滚 */
			GameObject_AtLanding(
				victim->object,
				ZSPEED_S_HIT_FLY, -ZACC_S_HIT_FLY,
				GamePlayer_StartRightForwardRoll
			);
		}
		return;
	}
	/* 如果攻击者朝向左方 */
	if( GamePlayer_IsLeftOriented(attacker) ) {
		/* 落地时开始向左边进行 前翻滚 */
		GameObject_AtLanding(
			victim->object,
			ZSPEED_S_HIT_FLY, -ZACC_S_HIT_FLY,
			GamePlayer_StartLeftBackwardRoll
		);
	} else {
		/* 落地时开始向右边进行 后翻滚 */
		GameObject_AtLanding(
			victim->object,
			ZSPEED_S_HIT_FLY, -ZACC_S_HIT_FLY,
			GamePlayer_StartRightBackwardRoll
		);
	}
}

static void AttackEffect_BePushedToLeft2( LCUI_Widget *widget )
{
	GameObject_AtLanding( widget, YSPEED_BE_PUSH, -YACC_BE_PUSH, GamePlayer_StartLeftForwardRoll );
}

static void AttackEffect_BePushedToLeft1( LCUI_Widget *widget )
{
	GameObject_AtLanding( widget, YSPEED_BE_PUSH, -YACC_BE_PUSH, AttackEffect_BePushedToLeft2 );
}

static void AttackEffect_BePushedToLeft( GamePlayer *player )
{
	GameObject_SetXSpeed( player->object, -XSPEED_BE_PUSH );
	GameObject_AtLanding( player->object, YSPEED_BE_PUSH, -YACC_BE_PUSH, AttackEffect_BePushedToLeft1 );
}

static void AttackEffect_BePushedToRight2( LCUI_Widget *widget )
{
	GameObject_AtLanding( widget, YSPEED_BE_PUSH, -YACC_BE_PUSH, GamePlayer_StartRightForwardRoll );
}

static void AttackEffect_BePushedToRight1( LCUI_Widget *widget )
{
	GameObject_AtLanding( widget, YSPEED_BE_PUSH, -YACC_BE_PUSH, AttackEffect_BePushedToRight2 );
}

static void AttackEffect_BePushedToRight( GamePlayer *player )
{
	GameObject_SetXSpeed( player->object, XSPEED_BE_PUSH );
	GameObject_AtLanding( player->object, YSPEED_BE_PUSH, -YACC_BE_PUSH, AttackEffect_BePushedToRight1 );
}

static void AttackEffect_BePushed( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_CancelStateAtBeLift( player );
	GamePlayer_CancelStateAtCatch( player );
	GamePlayer_CancelStateAtBeCatch( player );
	GamePlayer_BreakSkillEffect( player );
	GamePlayer_ChangeState( player, STATE_BE_PUSH );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	if( GamePlayer_IsLeftOriented(player) ) {
		AttackEffect_BePushedToLeft( player );
	} else {
		AttackEffect_BePushedToRight( player );
	}
}

static LCUI_BOOL GamePlayerStateCanNormalAttack( GamePlayer *player )
{
	switch(player->state) {
	case STATE_READY:
	case STATE_WALK:
	case STATE_STANCE:
	case STATE_DEFENSE:
	case STATE_BE_LIFT_STANCE:
		return TRUE;
	default: 
		break;
	}
	return FALSE;
}

static LCUI_BOOL GamePlayerStateCanJumpAttack( GamePlayer *player )
{
	switch(player->state) {
	case STATE_JUMP:
	case STATE_JSQUAT:
		return TRUE;
	default: 
		break;
	}
	return FALSE;
}

static LCUI_BOOL GamePlayerStateCanSprintJumpAttack( GamePlayer *player )
{
	switch(player->state) {
	case STATE_SJUMP:
	case STATE_SSQUAT:
		return TRUE;
	default: 
		break;
	}
	return FALSE;
}

static LCUI_BOOL GamePlayerStateCanThrow( GamePlayer *player )
{
	switch(player->state) {
	case STATE_LIFT_JUMP:
	case STATE_LIFT_FALL:
	case STATE_LIFT_RUN:
	case STATE_LIFT_STANCE:
	case STATE_LIFT_WALK:
		return TRUE;
	default:break;
	}
	return FALSE;
}

/** 处于被举起的状态下，在攻击结束时  */
static void GamePlayer_AtBeLiftAttackDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_SetAttackTypeName( player, ATK_NONE );
	GamePlayer_ChangeState( player, STATE_BE_LIFT_STANCE );
}

/** 在攻击结束时  */
static void GamePlayer_AtAttackDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GameObject_AtXSpeedToZero( widget, 0, NULL );
	GamePlayer_SetAttackTypeName( player, ATK_NONE );
	GamePlayer_ResetAttackControl( player );
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
	GamePlayer_SetReady( player );
}

/** 检测是否能够使用终结一击 */
static LCUI_BOOL CommonSkill_CanUseFinalBlow( GamePlayer *player )
{
	LCUI_Widget *widget;

	if( player->lock_action ) {
		return FALSE;
	}
	/* 如果并没有要使用A/B攻击，则不符合发动条件 */
	if( !player->control.a_attack && !player->control.b_attack ) {
		return FALSE;
	}
	if( !GamePlayerStateCanNormalAttack( player ) ) {
		return FALSE;
	}
	/** 检测终结一击攻击范围内是否有处于喘气状态下的游戏角色 */
	widget = GameObject_GetObjectInAttackRange( 
				player->object,
				ACTION_FINAL_BLOW,
				TRUE, ACTION_REST 
	);
	/* 没有就不发动 */
	if( !widget ) {
		return FALSE;
	}
	return TRUE;
}

/** 开始发动终结一击 */
static void CommonSkill_StartFinalBlow( GamePlayer *player )
{
	void (*func)(LCUI_Widget*);
	/* 若当前是在被举起的状态下，站立着，则改用相应回调函数 */
	if( player->state == STATE_BE_LIFT_STANCE
	|| player->state == STATE_BE_LIFT_SQUAT ) {
		func = GamePlayer_AtBeLiftAttackDone;
	} else {
		func = GamePlayer_AtAttackDone;
	}

	GamePlayer_ChangeState( player, STATE_FINAL_BLOW );
	GameObject_AtActionDone( player->object, ACTION_FINAL_BLOW, func );
	GameObject_ClearAttack( player->object );
	GamePlayer_SetAttackTypeName( player, ATK_FINALBLOW );
	GamePlayer_StopXWalk( player );
	GamePlayer_StopYMotion( player );
	GamePlayer_LockMotion( player );
	GamePlayer_LockAction( player );
}

static void _StartNormalAttack( GamePlayer *player, int state, int action, const char *attack_type_name )
{
	void (*func)(LCUI_Widget*);

	if( player->state == STATE_BE_LIFT_STANCE
	|| player->state == STATE_BE_LIFT_SQUAT ) {
		func = GamePlayer_AtBeLiftAttackDone;
	} else {
		func = GamePlayer_AtAttackDone;
	}
	GamePlayer_ChangeState( player, state );
	GameObject_AtActionDone( player->object, action, func );
	GamePlayer_SetAttackTypeName( player, attack_type_name );
	GamePlayer_StopXWalk( player );
	GamePlayer_StopYMotion( player );
	GamePlayer_LockMotion( player );
	GamePlayer_LockAction( player );
}

/** 检测是否能够使用A攻击 */
static LCUI_BOOL CommonSkill_CanUseAAttack( GamePlayer *player )
{
	if( player->lock_action ) {
		return FALSE;
	}
	if( !player->control.a_attack ) {
		return FALSE;
	}
	return GamePlayerStateCanNormalAttack( player );
}

/** 检测是否能够使用B攻击 */
static LCUI_BOOL CommonSkill_CanUseBAttack( GamePlayer *player )
{
	if( player->lock_action ) {
		return FALSE;
	}
	if( !player->control.b_attack ) {
		return FALSE;
	}
	return GamePlayerStateCanNormalAttack( player );
}

/** 检测是否能够使用跳跃A攻击 */
static LCUI_BOOL CommonSkill_CanUseJumpAAttack( GamePlayer *player )
{
	if( player->lock_action ) {
		return FALSE;
	}
	if( !player->control.a_attack ) {
		return FALSE;
	}
	return GamePlayerStateCanJumpAttack( player );
}

/** 检测是否能够使用跳跃B攻击 */
static LCUI_BOOL CommonSkill_CanUseJumpBAttack( GamePlayer *player )
{
	if( player->lock_action ) {
		return FALSE;
	}
	if( !player->control.b_attack ) {
		return FALSE;
	}
	return GamePlayerStateCanJumpAttack( player );
}

/** 开始发动A普通攻击 */
static void CommonSkill_StartAAttack( GamePlayer *player )
{
	_StartNormalAttack(	player, STATE_A_ATTACK, 
				ACTION_A_ATTACK, ATK_A_ATTACK );
}

/** 开始发动B普通攻击 */
static void CommonSkill_StartBAttack( GamePlayer *player )
{
	_StartNormalAttack(	player, STATE_B_ATTACK, 
				ACTION_B_ATTACK, ATK_B_ATTACK );
}

/** 开始发动高速A攻击 */
static void CommonSkill_StartMachAAttack( GamePlayer *player )
{
	_StartNormalAttack(	player, STATE_MACH_A_ATTACK,
			ACTION_MACH_A_ATTACK, ATK_MACH_A_ATTACK );
}

/** 开始发动高速B攻击 */
static void CommonSkill_StartMachBAttack( GamePlayer *player )
{
	_StartNormalAttack(	player, STATE_MACH_B_ATTACK, 
			ACTION_MACH_B_ATTACK, ATK_MACH_B_ATTACK );
}

/** 检测游戏角色在当前状态下是否能够进行冲撞攻击 */
static LCUI_BOOL GamePlayerStateCanSprintAttack( GamePlayer *player )
{
	if( player->state == STATE_LEFTRUN
	 || player->state == STATE_RIGHTRUN ) {
		return TRUE;
	}
	return FALSE;
}

/** 开始发动冲撞攻击 */
static void _StartSprintAttack( GamePlayer *player, int state, const char *attack_type_name )
{
	double speed;
	if( player->state != STATE_RIGHTRUN
	 && player->state != STATE_LEFTRUN ){
		return;
	}
	player->control.run = FALSE;
	speed = GameObject_GetXSpeed( player->object );
	GameObject_AtXSpeedToZero( player->object, -speed*1.5,  GamePlayer_AtAttackDone );
	GamePlayer_ChangeState( player, state );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	speed = GameObject_GetYSpeed( player->object );
	GameObject_SetYAcc( player->object, -speed*1.5 );
	GamePlayer_SetAttackTypeName( player, attack_type_name );
}

/** 检测是否能够使用冲撞A攻击 */
static LCUI_BOOL CommonSkill_CanUseASprintAttack( GamePlayer *player )
{
	if( player->lock_action ) {
		return FALSE;
	}
	if( !player->control.a_attack ) {
		return FALSE;
	}
	return GamePlayerStateCanSprintAttack( player );
}

/** 开始发动冲撞A攻击 */
static void CommonSkill_StartSprintAAttack( GamePlayer *player )
{
	_StartSprintAttack( player, STATE_AS_ATTACK, ATK_SPRINT_A_ATTACK );
}

/** 检测是否能够使用冲撞B攻击 */
static LCUI_BOOL CommonSkill_CanUseBSprintAttack( GamePlayer *player )
{
	if( player->lock_action ) {
		return FALSE;
	}
	if( !player->control.b_attack ) {
		return FALSE;
	}
	return GamePlayerStateCanSprintAttack( player );
}

/** 开始发动冲撞B攻击 */
static void CommonSkill_StartSprintBAttack( GamePlayer *player )
{
	_StartSprintAttack( player, STATE_BS_ATTACK, ATK_SPRINT_B_ATTACK );
}

static LCUI_BOOL CommonSkill_CanUseSprintJumpAAttack( GamePlayer *player )
{
	if( player->lock_action || !player->control.a_attack ) {
		return FALSE;
	}
	return GamePlayerStateCanSprintJumpAttack( player );
}

static LCUI_BOOL CommonSkill_CanUseSprintJumpBAttack( GamePlayer *player )
{
	if( player->lock_action || !player->control.b_attack ) {
		return FALSE;
	}
	return GamePlayerStateCanSprintJumpAttack( player );
}

static void CommonSkill_StartSprintJumpAAttack( GamePlayer *player )
{
	GamePlayer_ChangeState( player, STATE_ASJ_ATTACK );
	GamePlayer_LockAction( player );
	GamePlayer_SetAttackTypeName( player, ATK_SPRINT_JUMP_A_ATTACK );
}

static void CommonSkill_StartSprintJumpBAttack( GamePlayer *player )
{
	GamePlayer_ChangeState( player, STATE_BSJ_ATTACK );
	GamePlayer_LockAction( player );
	GamePlayer_SetAttackTypeName( player, ATK_SPRINT_JUMP_B_ATTACK );
}

/** 获取当前角色附近躺地的角色 */
static GamePlayer* GamePlayer_GetGroundPlayer( GamePlayer *player )
{
	RangeBox range;
	LCUI_Widget *widget;

	range.x = -5;
	range.x_width = 10;
	range.y = -GLOBAL_Y_WIDTH/2;
	range.y_width = GLOBAL_Y_WIDTH;
	range.z = 0;
	range.z_width = 20;

	/* 检测当前角色是否站在躺地角色的头和脚的位置上 */
	widget = GameObject_GetObjectInRange(	player->object, range,
						TRUE, ACTION_LYING );
	if( widget ) {
		return GamePlayer_GetPlayerByWidget( widget );;
	}
	widget = GameObject_GetObjectInRange(	player->object, range,
						TRUE, ACTION_TUMMY );
	if( widget ) {
		return GamePlayer_GetPlayerByWidget( widget );
	}
	widget = GameObject_GetObjectInRange(	player->object, range,
						TRUE, ACTION_LYING_HIT );
	if( widget ) {
		return GamePlayer_GetPlayerByWidget( widget );
	}
	widget = GameObject_GetObjectInRange(	player->object, range,
						TRUE, ACTION_TUMMY_HIT );
	if( widget ) {
		return GamePlayer_GetPlayerByWidget( widget );
	}
	return NULL;
}

/** 检测是否可以攻击到躺地玩家 */
static LCUI_BOOL CommonSkill_CanAttackGround( GamePlayer *player )
{
	double x1, x2;
	GamePlayer *other_player;

	other_player = GamePlayer_GetGroundPlayer( player );
	if( !other_player ) {
		return FALSE;
	}
	x1 = GameObject_GetX( player->object );
	x2 = GameObject_GetX( other_player->object );
	/* 游戏角色必须面向躺地的角色的中心位置 */
	if( GamePlayer_IsLeftOriented( player ) ) {
		if( x1 <= x2 ) {
			return FALSE;
		}
	} else {
		if( x1 > x2 ) {
			return FALSE;
		}
	}
	return TRUE;
}

static LCUI_BOOL CommonSkill_CanAAttackGround( GamePlayer *player )
{
	if( !GamePlayerStateCanNormalAttack( player ) ) {
		return FALSE;
	}
	if( !player->control.a_attack ) {
		return FALSE;
	}
	return CommonSkill_CanAttackGround( player );
}

static LCUI_BOOL CommonSkill_CanBAttackGround( GamePlayer *player )
{
	if( !GamePlayerStateCanNormalAttack( player ) ) {
		return FALSE;
	}
	if( !player->control.b_attack ) {
		return FALSE;
	}
	return CommonSkill_CanAttackGround( player );
}

/** 检测当前角色是否能够举起另一个角色 */
LCUI_BOOL GamePlayer_CanLiftPlayer( GamePlayer *player, GamePlayer *other_player )
{
	double x1, x2, z1, z2;
	x1 = GameObject_GetX( player->object );
	x2 = GameObject_GetX( other_player->object );
	/* 如果不在中间位置附近 */
	if( x1 <= x2-15 || x1 >= x2+15 ) {
		return FALSE;
	}
	z1 = GameObject_GetZ( player->object );
	z2 = GameObject_GetZ( other_player->object );
	/* 两者高度差距不能太大 */
	if( z1 <= z2-15 || z1 >= z2+15 ) {
		return FALSE;
	}
	/* 如果有其它游戏角色 */
	if( other_player->other ) {
		/* 如果其它游戏角色已经骑在该游戏角色身上，
		 * 或者举起了该游戏角色，则不能再次被其它游
		 * 戏角色举起或骑乘 */
		switch(other_player->other->state) {
		case STATE_RIDE:
		case STATE_RIDE_ATTACK:
		case STATE_LIFT_SQUAT:
		case STATE_RIDE_JUMP:
		case STATE_LIFT_FALL:
		case STATE_LIFT_WALK:
		case STATE_LIFT_STANCE:
		case STATE_LIFT_RUN:
			return FALSE;
		default: break;
		}
	}
	return TRUE;
}

/** 检测是否能够举起躺地玩家 */
static LCUI_BOOL CommonSkill_CanLift( GamePlayer *player )
{
	GamePlayer *other_player;
	if( player->lock_action ) {
		return FALSE;
	}
	if( !GamePlayerStateCanNormalAttack( player ) ) {
		return FALSE;
	}
	other_player = GamePlayer_GetGroundPlayer( player );
	if( !other_player ) { 
		return FALSE;
	}
	if( !GamePlayer_CanLiftPlayer( player, other_player ) ) {
		return FALSE;
	}
	/* 记录对方，以便接着举起 */
	player->other = other_player;
	return TRUE;
}

static LCUI_BOOL CommonSkill_CanALift( GamePlayer *player )
{
	if( !player->control.a_attack ) {
		return FALSE;
	}
	return CommonSkill_CanLift( player );
}

static LCUI_BOOL CommonSkill_CanBLift( GamePlayer *player )
{
	if( !player->control.b_attack ) {
		return FALSE;
	}
	return CommonSkill_CanLift( player );
}

/** 被举着，站立 */
static void GamePlayer_SetBeLiftStance( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	if( player->state != STATE_BE_LIFT_SQUAT ) {
		return;
	}
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_BE_LIFT_STANCE );
}

/** 被举着，准备站起 */
static void GamePlayer_BeLiftStartStand( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_BE_LIFT_SQUAT );
	GameObject_AtActionDone( player->object, ACTION_SQUAT, GamePlayer_SetBeLiftStance );
	GamePlayer_LockAction( player );
}

/** 在跳跃结束时 */
static void GamePlayer_AtJumpDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
	GamePlayer_SetReady( player );
}

static void GamePlayer_AtLandingSuqatDone( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_JUMP_DONE );
	GameObject_AtActionDone( player->object, ACTION_SQUAT, GamePlayer_AtJumpDone );
	GamePlayer_LockAction( player );
}

/** 在着陆完成时 */
static void GamePlayer_AtLandingDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_ResetAttackControl( player );
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
	if( player->control.left_motion ) {
		GamePlayer_SetLeftOriented( player );
	}
	else if( player->control.right_motion ) {
		GamePlayer_SetRightOriented( player );
	}
	/* 撤销在动作完成时的响应 */
	GameObject_AtActionDone( player->object, ACTION_SQUAT, NULL );
	GamePlayer_ChangeState( player, STATE_SQUAT );
	GamePlayer_LockAction( player );
	GameObject_SetXSpeed( player->object, 0 );
	GameObject_SetYSpeed( player->object, 0 );
	GamePlayer_SetActionTimeOut( player, 100, GamePlayer_AtLandingSuqatDone );
}

/** 设置游戏角色为下落状态 */
void GamePlayer_SetFall( GamePlayer *player )
{
	double z_speed;
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
	GamePlayer_ChangeState( player, STATE_FALL );
	GamePlayer_BreakRest( player );
	z_speed = GameObject_GetZ( player->object );
	GameObject_AtLanding( player->object, z_speed, -ZACC_JUMP, GamePlayer_AtLandingDone );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
}

/** 在被举起的状态下，更新自身的位置 */
static void GamePlayer_UpdateLiftPosition( LCUI_Widget *widget )
{
	double x, y, z;
	GamePlayer *player, *other_player;
	LCUI_Widget *other;

	player = GamePlayer_GetPlayerByWidget( widget );
	other_player = player->other;
	/* 如果没有举起者，或自己不是被举起者 */
	if( !player->other ) {
		GameObject_AtMove( widget, NULL );
		return;
	}
	else if( !player->other->other ) {
		/* 如果举起者并不处于举起状态 */
		if( !GamePlayer_IsInLiftState( player ) ) {
			/* 如果举起者不是要要投掷自己 */
			if( player->state != STATE_THROW ) {
				GamePlayer_SetFall( other_player );
			}
		}
		GameObject_AtMove( widget, NULL );
		return;
	}
	if( !GamePlayer_IsInLiftState( player ) ) {
		if( player->state != STATE_THROW ) {
			GamePlayer_SetFall( other_player );
		}
		GameObject_AtMove( widget, NULL );
		return;
	}
	other = player->other->object;
	x = GameObject_GetX( widget );
	y = GameObject_GetY( widget );
	z = GameObject_GetZ( widget );
	/* 获取当前帧的顶点相对于底线的距离 */
	z += GameObject_GetCurrentFrameTop( widget );
	GameObject_SetZ( other, z );
	GameObject_SetX( other, x );
	GameObject_SetY( other, y );
}

static void GamePlayer_AtLiftDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
	if( !player->other ) {
		GamePlayer_SetReady( player );
		return;
	}
	GameObject_SetZ( player->other->object, LIFT_HEIGHT );
	GamePlayer_ChangeState( player, STATE_LIFT_STANCE );
	/* 在举起者的位置变化时更新被举起者的位置 */
	GameObject_AtMove( widget, GamePlayer_UpdateLiftPosition );
}

/** 开始举起另一个角色 */
static void CommonSkill_StartLift( GamePlayer *player )
{
	double x, y;
	int z_index;
	GamePlayer *other_player;

	if( !player->other ) {
		return;
	}
	other_player = player->other;
	/* 对方不处于躺地状态则退出 */
	if( other_player->state != STATE_LYING
	 && other_player->state != STATE_LYING_HIT
	 && other_player->state != STATE_TUMMY
	 && other_player->state != STATE_TUMMY_HIT ) {
		player->other = NULL;
		return;
	}
	
	/* 被举起的角色，需要记录举起他的角色 */
	other_player->other = player;
	GamePlayer_BreakRest( other_player );
	z_index = Widget_GetZIndex( player->object );
	Widget_SetZIndex( other_player->object, z_index+1);
	
	GamePlayer_UnlockAction( other_player );
	if( other_player->state == STATE_LYING
	 || other_player->state == STATE_LYING_HIT ) {
		GamePlayer_ChangeState( other_player, STATE_BE_LIFT_LYING );
	} else {
		GamePlayer_ChangeState( other_player, STATE_BE_LIFT_TUMMY );
	}
	GamePlayer_LockAction( other_player );
	/* 举起前，先停止移动 */
	GamePlayer_StopXMotion( player );
	GamePlayer_StopYMotion( player );
	player->control.left_motion = FALSE;
	player->control.right_motion = FALSE;
	GamePlayer_ResetAttackControl( player );
	GamePlayer_UnlockAction( player );
	/* 改为下蹲状态 */
	GamePlayer_ChangeState( player, STATE_LIFT_SQUAT );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_AtActionDone( player->object, ACTION_SQUAT, GamePlayer_AtLiftDone );
	GamePlayer_SetRestTimeOut(
		other_player, 
		BE_LIFT_REST_TIMEOUT, 
		GamePlayer_BeLiftStartStand 
	);
	/* 改变躺地角色的坐标 */
	x = GameObject_GetX( player->object );
	y = GameObject_GetY( player->object );
	GameObject_SetX( other_player->object, x );
	GameObject_SetY( other_player->object, y );
	GameObject_SetZ( other_player->object, 20 );
}

static void GamePlayer_AtGroundAttackDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_SQUAT );
	GamePlayer_LockAction( player );
	GameObject_SetXSpeed( player->object, 0 );
	GameObject_SetYSpeed( player->object, 0 );
	GameObject_AtActionDone( player->object, ACTION_SQUAT, GamePlayer_AtAttackDone );
}

static void CommonSkill_JumpTreadStep2( LCUI_Widget *widget )
{
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_JUMP_STOMP );
	GamePlayer_LockAction( player );
	GamePlayer_SetAttackTypeName( player, ATK_JUMP_TREAD );
	GameObject_AtZeroZSpeed( widget, NULL );
	GameObject_ClearAttack( player->object );
}

static void CommonSkill_JumpTreadStep1( LCUI_Widget *widget )
{
	double z_speed, z_acc;
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	z_acc = -ZACC_JUMP;
	z_speed = ZSPEED_JUMP;
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_JUMP );
	GamePlayer_LockAction( player );
	GameObject_AtLanding( widget, z_speed, z_acc, GamePlayer_AtGroundAttackDone );
	GameObject_AtZeroZSpeed( widget, CommonSkill_JumpTreadStep2 );
}

/** 进行跳跃+踩 */
static void CommonSkill_StartJumpTread( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GameObject_SetXAcc( player->object, 0 );
	GamePlayer_ChangeState( player, STATE_SQUAT );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_SetXSpeed( player->object, 0 );
	GameObject_SetYSpeed( player->object, 0 );
	GameObject_SetZSpeed( player->object, 0 );
	GameObject_AtActionDone( player->object, ACTION_SQUAT, CommonSkill_JumpTreadStep1 );
}

static void CommonSkill_JumpElbowStep2( LCUI_Widget *widget )
{
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_JUMP_ELBOW );
	GamePlayer_LockAction( player );
	GamePlayer_SetAttackTypeName( player, ATK_JUMP_ELBOW );
	GameObject_AtZeroZSpeed( widget, NULL );
	GameObject_ClearAttack( player->object );
}

static void CommonSkill_JumpElbowStep1( LCUI_Widget *widget )
{
	double z_speed, z_acc;
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	z_acc = -ZACC_JUMP;
	z_speed = ZSPEED_JUMP;
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_JUMP );
	GamePlayer_LockAction( player );
	GameObject_AtLanding( widget, z_speed, z_acc, GamePlayer_AtGroundAttackDone );
	GameObject_AtZeroZSpeed( widget, CommonSkill_JumpElbowStep2 );
}

/** 进行跳跃+肘击 */
void CommonSkill_StartJumpElbow( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GameObject_SetXAcc( player->object, 0 );
	GamePlayer_ChangeState( player, STATE_SQUAT );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_SetXSpeed( player->object, 0 );
	GameObject_SetYSpeed( player->object, 0 );
	GameObject_SetZSpeed( player->object, 0 );
	GameObject_ClearAttack( player->object );
	GameObject_AtActionDone( player->object, ACTION_SQUAT, CommonSkill_JumpElbowStep1 );
}

static void GamePlayer_StopMachStomp( GamePlayer *player )
{
	GamePlayer_AtAttackDone( player->object );
}

/** 高速踩踏 */
static void CommonSkill_StartMachStomp( GamePlayer *player )
{
	GamePlayer_ChangeState( player, STATE_MACH_STOMP );
	GamePlayer_SetAttackTypeName( player, ATK_MACH_STOMP );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_SetXSpeed( player->object, 0 );
	GameObject_SetYSpeed( player->object, 0 );
	GamePlayer_SetActionTimeOut( player, 1250, GamePlayer_StopMachStomp );
}

/** 开始发动跳跃A攻击 */
static void CommonSkill_StartJumpAAttack( GamePlayer *player )
{
	GamePlayer_ChangeState( player, STATE_AJ_ATTACK );
	GamePlayer_LockAction( player );
	GamePlayer_SetAttackTypeName( player, ATK_JUMP_A_ATTACK );
}

/** 开始发动跳跃高速A攻击 */
static void CommonSkill_StartJumpMachAAttack( GamePlayer *player )
{
	GamePlayer_ChangeState( player, STATE_MAJ_ATTACK );
	GamePlayer_LockAction( player );
	GamePlayer_SetAttackTypeName( player, ATK_JUMP_A_ATTACK );
}

/** 开始发动跳跃B攻击 */
static void CommonSkill_StartJumpBAttack( GamePlayer *player )
{
	GamePlayer_ChangeState( player, STATE_BJ_ATTACK );
	GamePlayer_LockAction( player );
	GamePlayer_SetAttackTypeName( player, ATK_JUMP_B_ATTACK );
}

/** 开始发动跳跃高速B攻击 */
static void CommonSkill_StartJumpMachBAttack( GamePlayer *player )
{
	GamePlayer_ChangeState( player, STATE_MBJ_ATTACK );
	GamePlayer_LockAction( player );
	GamePlayer_SetAttackTypeName( player, ATK_JUMP_B_ATTACK );
}

static LCUI_BOOL CommonSkill_CanUseBombKick( GamePlayer *player )
{
	if( player->state != STATE_JUMP_DONE ) {
		return FALSE;
	}
	if( !player->control.b_attack ) {
		return FALSE;
	}
	return TRUE;
}

/** 开始发动 爆裂腿技能 */
static void CommonSkill_StartBombKick( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_BOMBKICK );
	GamePlayer_SetAttackTypeName( player, ATK_BOMBKICK );
	GameObject_ClearAttack( player->object );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	if( player->control.left_motion ) {
		GameObject_SetXSpeed( player->object, -XSPEED_BOMBKICK );
		GamePlayer_SetLeftOriented( player );
	}
	else if( player->control.right_motion ) {
		GameObject_SetXSpeed( player->object, XSPEED_BOMBKICK );
		GamePlayer_SetRightOriented( player );
	}
	else if( GamePlayer_IsLeftOriented(player) ) {
		GameObject_SetXSpeed( player->object, -XSPEED_BOMBKICK );
	} else {
		GameObject_SetXSpeed( player->object, XSPEED_BOMBKICK );
	}
	GameObject_AtLanding(	player->object,
				ZSPEED_BOMBKICK, -ZACC_BOMBKICK,
				GamePlayer_AtLandingDone 
	);
}

static LCUI_BOOL CommonSkill_CanUseSpinHit( GamePlayer *player )
{
	if( player->state != STATE_JUMP_DONE ) {
		return FALSE;
	}
	if( !player->control.a_attack ) {
		return FALSE;
	}
	return TRUE;
}

static int CommonSkill_StartSecondSpinHit( GamePlayer *player )
{
	double x_speed;
	if( player->control.left_motion ) {
		x_speed = -XSPEED_SPINHIT_2;
	} else if( player->control.right_motion ) {
		x_speed = XSPEED_SPINHIT_2;
	} else {
		return -1;
	}
	GameObject_SetXSpeed( player->object, x_speed );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_SPINHIT );
	GamePlayer_SetAttackTypeName( player, ATK_SPINHIT_2 );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_AtLanding( player->object, ZSPEED_SPINHIT_2, -ZACC_SPINHIT_2, GamePlayer_AtLandingDone );
	return 0;
}


/** 开始发动 自旋击（翻转击） 技能 */
static void CommonSkill_StartSpinHit( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_SPINHIT );
	GamePlayer_SetAttackTypeName( player, ATK_SPINHIT );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	if( player->control.left_motion ) {
		GameObject_SetXSpeed( player->object, -XSPEED_SPINHIT );
		GamePlayer_SetLeftOriented( player );
	}
	else if( player->control.right_motion ) {
		GameObject_SetXSpeed( player->object, XSPEED_SPINHIT );
		GamePlayer_SetRightOriented( player );
	}
	else if( GamePlayer_IsLeftOriented(player) ) {
		GameObject_SetXSpeed( player->object, -XSPEED_SPINHIT );
	} else {
		GameObject_SetXSpeed( player->object, XSPEED_SPINHIT );
	}
	GameObject_AtLanding( player->object, ZSPEED_SPINHIT, -ZACC_SPINHIT, GamePlayer_AtLandingDone );
}

static LCUI_BOOL CommonSkill_CanUseAThrow( GamePlayer *player )
{
	if( player->lock_action ) {
		return FALSE;
	}
	if( !player->control.a_attack ) {
		return FALSE;
	}
	return GamePlayerStateCanThrow( player );
}

static LCUI_BOOL CommonSkill_CanUseBThrow( GamePlayer *player )
{
	if( player->lock_action ) {
		return FALSE;
	}
	if( !player->control.b_attack ) {
		return FALSE;
	}
	return GamePlayerStateCanThrow( player );
}

/** 在被举起的状态下，被动起跳 */
static void GamePlayer_BeLiftPassiveStartJump( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	player->other = NULL;
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_SJUMP );
}

static void GamePlayer_AtThrowLanding( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_StopXMotion( player );
	GamePlayer_StopYMotion( player );
	GamePlayer_StartStand( player );
}

/** 在抛飞结束时 */
static void GamePlayer_AtThrowUpFlyDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_StopXMotion( player );
	GameObject_AtTouch( widget, NULL );
	if( GamePlayer_SetLying( player ) == 0 ) {
		GamePlayer_SetRestTimeOut(
			player, BE_THROW_REST_TIMEOUT, 
			GamePlayer_StartStand 
		);
	}
}

/** 被抛后落地时，弹起 */
static void GamePlayer_LandingBounce( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_ReduceSpeed( player, 50 );
	Game_RecordAttack( player->other, ATK_THROW, player, player->state );
	player->other = NULL;
	GameObject_AtLanding(
		widget, ZSPEED_XF_HIT_FLY2, -ZACC_XF_HIT_FLY2,
		GamePlayer_AtThrowUpFlyDone
	);
}

/** 处理游戏角色在被抛出时与其他角色的碰撞 */
static void GamePlayer_ProcThrowFlyTouch( LCUI_Widget *self, LCUI_Widget *other )
{
	GamePlayer *player, *other_player;

	player = GamePlayer_GetPlayerByWidget( self );
	other_player = GamePlayer_GetPlayerByWidget( other );
	if( !player || !other_player ) {
		return;
	}
	
	/* 判断自己的状态是否符合要求 */
	if( player->state != STATE_HIT_FLY
	 && player->state != STATE_HIT_FLY_FALL
	 && player->state != STATE_LYING
	 && player->state != STATE_TUMMY
	 && player->state != STATE_LYING_HIT
	 && player->state != STATE_TUMMY_HIT ) {
		return;
	}
	/* 如果有抛掷者记录 */
	if( player->other ) {
		/* 如果触碰者是抛掷者，且抛掷者还处于抛掷动作 */
		if( other_player == player->other
		 && player->other->state == STATE_THROW ) {
				return;
		}
	}
	/* 如果已经是受攻击者了，则退出，不重复记录攻击 */
	if( GameObject_HaveVictim(self, other) ) {
		return;
	}
	/* 设置自己的攻击类型 */
	GamePlayer_SetAttackTypeName( player, ATK_BUMPED );
	/* 添加受害者记录 */
	GameObject_DirectAddVictim( self, other );
	other_player->n_attack = 0;
}

/** 在抛投动作结束时 */
static void GamePlayer_AtThrowDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	double z, z_speed, z_acc;

	player = GamePlayer_GetPlayerByWidget( widget );
	z = GameObject_GetZ( widget );
	z_speed = GameObject_GetZSpeed( widget );
	z_acc = GameObject_GetZAcc( widget );
	/* 若还未落地，则维持该动作，直至落地时切换 */
	if( z > 0 && z_acc < 0 ) {
		GameObject_AtLanding( widget, z_speed, z_acc, GamePlayer_AtThrowLanding );
	} else {
		GamePlayer_StopXMotion( player );
		GamePlayer_StopYMotion( player );
		GamePlayer_UnlockAction( player );
		GamePlayer_SetReady( player );
	}
}

static void GamePlayer_AtBeThrowDownDone( LCUI_Widget *widget )
{
	int ret = 0;
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	switch( player->state ) {
	case STATE_TUMMY:
	case STATE_TUMMY_HIT:
		ret = GamePlayer_SetTummy( player );
		break;
	case STATE_LYING:
	case STATE_LYING_HIT:
		ret = GamePlayer_SetLying( player );
		break;
	default:
		break;
	}
	GamePlayer_LockAction( player );
	GamePlayer_StopXMotion( player );
	if( ret == 0 ) {
		GamePlayer_SetRestTimeOut( player, BE_THROW_REST_TIMEOUT, GamePlayer_StartStand );
	}
}

static void GamePlayer_AtBeThrowDownLanding( LCUI_Widget *widget )
{
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	if( player->state == STATE_BE_LIFT_TUMMY ) {
		GamePlayer_ChangeState( player, STATE_TUMMY_HIT );
		GameObject_AtActionDone( player->object, ACTION_TUMMY_HIT, NULL );
	} else {
		GamePlayer_ChangeState( player, STATE_LYING_HIT );
		GameObject_AtActionDone( player->object, ACTION_LYING_HIT, NULL );
	}
	GamePlayer_LockAction( player );
	Game_RecordAttack( player->other, ATK_THROW, player, player->state );
	player->other = NULL;
	GamePlayer_ReduceSpeed( player, 80 );
	GameObject_AtLanding(
		player->object, ZSPEED_XF_HIT_FLY2,
		-ZACC_XF_HIT_FLY2, GamePlayer_AtBeThrowDownDone
	);
	/* 撤销响应游戏对象之间的触碰 */
	GameObject_AtTouch( player->object, NULL );
}

/** 将举起的角色向下摔 */
static void CommonSkill_StartAThrow( GamePlayer *player )
{
	int state;
	double x_speed;

	if( !GamePlayer_IsInLiftState(player)
	|| player->state == STATE_LIFT_SQUAT ) {
		return;
	}
	if( player->state == STATE_LIFT_RUN ) {
		x_speed = GameObject_GetXSpeed( player->object );
		GameObject_SetXAcc( player->object, -x_speed*2.0 );
	}
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_THROW );
	GameObject_AtActionDone( player->object, ACTION_THROW, GamePlayer_AtThrowDone );
	if( !player->other ) {
		GamePlayer_LockAction( player );
		/* 解除举起者与被举起者的关系 */
		//player->other->other = NULL;
		player->other = NULL;
	}
	GamePlayer_BreakRest( player->other );
	GamePlayer_UnlockAction( player->other );
	switch( player->other->state ) {
	case STATE_BE_LIFT_LYING:
	case STATE_BE_LIFT_LYING_HIT:
		state = STATE_LYING;
		break;
	case STATE_BE_LIFT_TUMMY:
	case STATE_BE_LIFT_TUMMY_HIT:
		state = STATE_TUMMY;
		break;
	case STATE_SQUAT:
	case STATE_BE_LIFT_SQUAT:
	case STATE_BE_LIFT_STANCE:
		if( GamePlayer_IsLeftOriented(player) ) {
			x_speed = -XSPEED_RUN/2;
		} else {
			x_speed = XSPEED_RUN/2;
		}
		x_speed += GameObject_GetXSpeed( player->object )/2;
		GamePlayer_LockMotion( player->other );
		GameObject_SetXSpeed( player->other->object, x_speed );
		GamePlayer_ChangeState( player->other, STATE_BE_LIFT_SQUAT );
		GameObject_AtLanding( 
			player->other->object, 
			ZSPEED_JUMP, -ZACC_JUMP, 
			GamePlayer_AtLandingDone
		);
		GameObject_AtActionDone(
			player->other->object, ACTION_SQUAT,
			GamePlayer_BeLiftPassiveStartJump 
		);
	default:
		GamePlayer_LockAction( player->other );
		GamePlayer_LockAction( player );
		player->other = NULL;
		return;
	}
	/* 如果被举起的角色还处于 躺/趴着 的状态，
	 * 那就根据举起者的方向，将被举起者向相应方向扔。
	 */
	if( GamePlayer_IsLeftOriented(player) ) {
		x_speed = -XSPEED_THROWDOWN_FLY;
		GamePlayer_SetRightOriented( player->other );
	} else {
		x_speed = XSPEED_THROWDOWN_FLY;
		GamePlayer_SetLeftOriented( player->other );
	}
	/* 改变状态 */
	GamePlayer_ChangeState( player->other, state );
	/* 加上投掷者的速度 */
	x_speed += GameObject_GetXSpeed( player->object )/2;
	/* 锁定被投掷者的移动 */
	GamePlayer_LockMotion( player->other );
	/* 设置被投掷者的移动速度 */
	GameObject_SetXSpeed( player->other->object, x_speed );
	/* 在落地时进行响应 */
	GameObject_AtLanding(
		player->other->object, -ZSPEED_THROWDOWN_FLY,
		-ZACC_THROWDOWN_FLY, GamePlayer_AtBeThrowDownLanding
	);
	/* 清除攻击记录 */
	GameObject_ClearAttack( player->other->object );
	/* 在被抛出者触碰到其它游戏角色时进行响应 */
	GameObject_AtTouch(
		player->other->object,
		GamePlayer_ProcThrowFlyTouch
	);
	/* 锁定两者的动作 */
	GamePlayer_LockAction( player->other );
	GamePlayer_LockAction( player );
	player->other = NULL;
}

/** 将举起的角色向前抛出 */
static void CommonSkill_StartBThrow( GamePlayer *player )
{
	double x_speed;
	if( !GamePlayer_IsInLiftState(player)
	|| player->state == STATE_LIFT_SQUAT ) {
		return;
	}
	/* 如果在跑步，那就减点速度 */
	if( player->state == STATE_LIFT_RUN ) {
		x_speed = GameObject_GetXSpeed( player->object );
		GameObject_SetXAcc( player->object, -x_speed*2.0 );
	}
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_THROW );
	GameObject_AtActionDone( player->object, ACTION_THROW, GamePlayer_AtThrowDone );
	if( !player->other ) {
		GamePlayer_LockAction( player );
		player->other = NULL;
	}
	/* 打断休息时限倒计时，避免角色被抛出时，在空中站起 */
	GamePlayer_BreakRest( player->other );
	GamePlayer_UnlockAction( player->other );
	switch( player->other->state ) {
	case STATE_BE_LIFT_LYING:
	case STATE_BE_LIFT_LYING_HIT:
	case STATE_BE_LIFT_TUMMY:
	case STATE_BE_LIFT_TUMMY_HIT:
		/* 如果被举起的角色还处于 躺/趴着 的状态，
			* 那就根据举起者的方向，将被举起者向相应方向扔。
			*/
		if( GamePlayer_IsLeftOriented(player) ) {
			x_speed = -XSPEED_THROWUP_FLY;
			GamePlayer_SetRightOriented( player->other );
		} else {
			x_speed = XSPEED_THROWUP_FLY;
			GamePlayer_SetLeftOriented( player->other );
		}
		x_speed += GameObject_GetXSpeed( player->object )/4;
		GamePlayer_LockMotion( player->other );
		GameObject_SetXSpeed( player->other->object, x_speed );
		GamePlayer_ChangeState( player->other, STATE_HIT_FLY_FALL );
		GameObject_AtLanding(
			player->other->object, ZSPEED_THROWUP_FLY, 
			-ZACC_THROWUP_FLY, GamePlayer_LandingBounce
		);
		/* 清除攻击记录 */
		GameObject_ClearAttack( player->other->object );
		/* 在被抛出者触碰到其它游戏角色时进行响应 */
		GameObject_AtTouch(
			player->other->object,
			GamePlayer_ProcThrowFlyTouch
		);
		break;
	case STATE_SQUAT:
	case STATE_BE_LIFT_SQUAT:
	case STATE_BE_LIFT_STANCE:
		if( GamePlayer_IsLeftOriented(player) ) {
			x_speed = -XSPEED_RUN/2;
		} else {
			x_speed = XSPEED_RUN/2;
		}
		x_speed += GameObject_GetXSpeed( player->object )/2;
		GamePlayer_LockMotion( player->other );
		GameObject_SetXSpeed( player->other->object, x_speed );
		GamePlayer_ChangeState( player->other, STATE_BE_LIFT_SQUAT );
		GameObject_AtLanding( 
			player->other->object, 
			ZSPEED_JUMP, -ZACC_JUMP, 
			GamePlayer_AtLandingDone
		);
		GameObject_AtActionDone(
			player->other->object, ACTION_SQUAT,
			GamePlayer_BeLiftPassiveStartJump 
		);
	default:
		break;
	}
	GamePlayer_LockAction( player->other );
	GamePlayer_LockAction( player );
	player->other = NULL;
}

static LCUI_BOOL CommonSkill_CanUseRide( GamePlayer *player )
{
	GamePlayer *other_player;
	if( player->lock_motion ) {
		return FALSE;
	}
	if( !player->control.up_motion
	 && !player->control.down_motion ) {
		 return FALSE;
	}
	if( !GamePlayerStateCanNormalAttack( player ) ) {
		return FALSE;
	}
	other_player = GamePlayer_GetGroundPlayer( player );
	if( !other_player ) {
		return FALSE;
	}
	if( !GamePlayer_CanLiftPlayer( player, other_player ) ) {
		return FALSE;
	}
	player->other = other_player;
	return TRUE;
}

static void CommonSkill_StartRide( GamePlayer *player )
{
	int z_index;
	double x, y;
	if( player->other ) {
		player->other->other = player;
	} else {
		return;
	}
	GamePlayer_StopXMotion( player );
	GamePlayer_StopYMotion( player );
	x = GameObject_GetX( player->other->object );
	y = GameObject_GetY( player->other->object );
	GameObject_SetX( player->object, x );
	GameObject_SetY( player->object, y );
	GamePlayer_ChangeState( player, STATE_RIDE );
	z_index = Widget_GetZIndex( player->other->object );
	Widget_SetZIndex( player->object, z_index+1 );
}

static LCUI_BOOL CommonSkill_CanUseCatch( GamePlayer *player )
{
	double  z1, z2;
	GamePlayer *other_player;

	if( player->lock_motion ) {
		return FALSE;
	}
	if( !player->control.left_motion
	 && !player->control.right_motion ) {
		 return FALSE;
	}
	switch( player->state ) {
	case STATE_READY:
	case STATE_STANCE:
	case STATE_WALK:
		other_player = GamePlayer_CatchGaspingPlayer( player );
		if( other_player ) {
			z1 = GameObject_GetZ( player->object );
			z2 = GameObject_GetZ( other_player->object );
			/* 两者高度差距不能太大 */
			if( z1 <= z2-15 || z1 >= z2+15 ) {
				return FALSE;
			}
			/* 如对方被其它玩家抓住了 */
			if( other_player->other && other_player->other != player 
			 && other_player->other->other == other_player ) {
				return FALSE;
			}
			/* 记录擒获者和被擒者 */
			player->other = other_player;
			other_player->other = player;
			return TRUE;
		}
	default:
		break;
	}
	return FALSE;
}

static void GamePlayer_AtCatchDone( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
	GamePlayer_SetReady( player );
	if( player->other ) {
		GamePlayer_UnlockAction( player->other );
		GamePlayer_SetRest( player->other );
	}
}

/** 在擒住状态下，对目标的状态进行调整 */
void CommonSkill_AdjustTargetAtBeCatch( GamePlayer *self )
{
	GamePlayer *target_player;

	target_player = self->other;
	if( !target_player ) {
		return;
	}
	GamePlayer_UnlockAction( target_player );
	GamePlayer_StopXMotion( target_player );
	GamePlayer_StopYMotion( target_player );
	GamePlayer_ChangeState( target_player, STATE_BE_CATCH );
	GamePlayer_LockAction( target_player );
	CommonSkill_SetPositionAtCatch( self, target_player );
	target_player->other = self;
}

/** 设置自己在擒获住对方时对方的位置 */
void CommonSkill_SetPositionAtCatch( GamePlayer *self, GamePlayer *other )
{
	double x, y;
	
	if( !self->other ) {
		return;
	}

	x = GameObject_GetX( self->object );
	y = GameObject_GetY( self->object );
	GameObject_SetY( self->other->object, y );

	if( self->control.left_motion && self->control.right_motion ) {
		if( GamePlayer_IsLeftOriented(self->other) ) {
			if( x < GameObject_GetX( self->other->object ) ) {
				return;
			}
			if( GamePlayer_IsLeftOriented(self->other) ) {
				GamePlayer_ChangeState( self->other, STATE_BACK_BE_CATCH );
				GameObject_SetX( self->other->object, x-25 );
			} else {
				GamePlayer_ChangeState( self->other, STATE_BE_CATCH );
				GameObject_SetX( self->other->object, x-30 );
			}
			return;
		}
		if( x > GameObject_GetX( self->other->object ) ) {
			return;
		}
		if( GamePlayer_IsLeftOriented(self->other) ) {
			GamePlayer_ChangeState( self->other, STATE_BE_CATCH );
			GameObject_SetX( self->other->object, x+30 );
		} else {
			GamePlayer_ChangeState( self->other, STATE_BACK_BE_CATCH );
			GameObject_SetX( self->other->object, x+25 );
		}
		return;
	}

	if( self->control.left_motion || GamePlayer_IsLeftOriented(self) ) {
		if( GamePlayer_IsLeftOriented(self->other) ) {
			GamePlayer_ChangeState( self->other, STATE_BACK_BE_CATCH );
			GameObject_SetX( self->other->object, x-25 );
		} else {
			GamePlayer_ChangeState( self->other, STATE_BE_CATCH );
			GameObject_SetX( self->other->object, x-30 );
		}
		return;
	}

	if( self->control.right_motion || !GamePlayer_IsLeftOriented(self) ) {
		if( GamePlayer_IsLeftOriented(self->other) ) {
			GamePlayer_ChangeState( self->other, STATE_BE_CATCH );
			GameObject_SetX( self->other->object, x+30 );
		} else {
			GamePlayer_ChangeState( self->other, STATE_BACK_BE_CATCH );
			GameObject_SetX( self->other->object, x+25 );
		}
	}
}

static void CommonSkill_StartCatch( GamePlayer *player )
{
	if( !player->other ) {
		return;
	}
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockAction( player->other );
	GamePlayer_StopXMotion( player );
	GamePlayer_StopYMotion( player );
	CommonSkill_SetPositionAtCatch( player, player->other );
	GamePlayer_ChangeState( player, STATE_CATCH );
	GamePlayer_LockAction( player );
	GamePlayer_LockAction( player->other );
	GamePlayer_LockMotion( player );
	GamePlayer_LockMotion( player->other );
	GamePlayer_SetActionTimeOut( player, 2000, GamePlayer_AtCatchDone );
}

static LCUI_BOOL CommonSkill_CanUseRideAAttack( GamePlayer *player )
{
	if( player->lock_action ) {
		return FALSE;
	}
	if( !player->control.a_attack ) {
		return FALSE;
	}
	if( player->state != STATE_RIDE ) {
		return FALSE;
	}
	if( !player->other ) {
		return FALSE;
	}
	if( player->other->other != player ) {
		return FALSE;
	}
	if( player->other->state != STATE_LYING
	&& player->other->state != STATE_LYING_HIT
	&& player->other->state != STATE_TUMMY
	&& player->other->state != STATE_TUMMY_HIT ) {
		return FALSE;
	}
	return TRUE;
}

static LCUI_BOOL CommonSkill_CanUseRideBAttack( GamePlayer *player )
{
	if( player->lock_action ) {
		return FALSE;
	}
	if( !player->control.b_attack ) {
		return FALSE;
	}
	if( player->state != STATE_RIDE ) {
		return FALSE;
	}
	if( !player->other ) {
		return FALSE;
	}
	if( player->other->other != player ) {
		return FALSE;
	}
	if( player->other->state != STATE_LYING
	&& player->other->state != STATE_LYING_HIT
	&& player->other->state != STATE_TUMMY
	&& player->other->state != STATE_TUMMY_HIT ) {
		return FALSE;
	}
	return TRUE;
}

static void GamePlayer_AtRideAttackDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	/* 如果还骑在对方身上 */
	if( player->other ) {
		GamePlayer_ChangeState( player, STATE_RIDE );
	}
}

static void CommonSkill_StartRideAAttack( GamePlayer *player )
{
	if( !player->other ) {
		return;
	}
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_RIDE_ATTACK );
	GamePlayer_LockAction( player );
	GamePlayer_SetHit( player->other );
	Game_RecordAttack( player, ATK_RIDE_A_ATTACK, player->other, player->other->state );
	GameObject_AtActionDone( player->object, ACTION_RIDE_ATTACK, GamePlayer_AtRideAttackDone );
}

static void GamePlayer_AtRideJumpDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	if( !player->other ) {
		GamePlayer_StartStand( player );
		return;
	}
	switch( player->other->state ) {
	case STATE_TUMMY:
	case STATE_TUMMY_HIT:
	case STATE_LYING:
	case STATE_LYING_HIT:
		Game_RecordAttack(	player, ATK_RIDE_B_ATTACK, 
					player->other, player->other->state );
		CommonSkill_StartRideAAttack( player );
		break;
	default:
		player->other = NULL;
		GamePlayer_StartStand( player );
		break;
	}
}

static void CommonSkill_StartRideBAttack( GamePlayer *player )
{
	if( !player->other ) {
		return;
	}
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_RIDE_JUMP );
	GamePlayer_LockAction( player );
	GameObject_AtLanding( player->object, ZSPEED_JUMP, -ZACC_JUMP, GamePlayer_AtRideJumpDone );
}

static LCUI_BOOL CommonSkill_CanUseBigElbow( GamePlayer *player )
{
	double speed;
	if( !player->control.a_attack ) {
		return FALSE;
	}
	switch( player->state ) {
	case STATE_JUMP:
	case STATE_JSQUAT:
		speed = GameObject_GetZSpeed( player->object );
		if( speed < 0 ) {
			return TRUE;
		}
	default:
		break;
	}
	return FALSE;
}

static LCUI_BOOL CommonSkill_CanUseGuillotine( GamePlayer *player )
{
	double speed;
	if( !player->control.b_attack ) {
		return FALSE;
	}
	switch( player->state ) {
	case STATE_JUMP:
	case STATE_JSQUAT:
		speed = GameObject_GetZSpeed( player->object );
		if( speed < 0 ) {
			return TRUE;
		}
	default:
		break;
	}
	return FALSE;
}

static void GamePlayer_AtBigElbowStep2( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GameObject_AtZeroZSpeed( widget, NULL );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_SQUAT );
	/* 撤销再 下蹲 动作结束时的响应 */
	GameObject_AtActionDone( widget, ACTION_SQUAT, NULL );
	GamePlayer_LockAction( player );
}

static void GamePlayer_AtBigElbowStep1( LCUI_Widget *widget )
{
	GameObject_SetXSpeed( widget, 0 );
	GameObject_AtLanding( widget, ZSPEED_BOUNCE, -ZACC_BOUNCE, GamePlayer_AtAttackDone );
	GameObject_AtZeroZSpeed( widget, GamePlayer_AtBigElbowStep2 );
}

static void CommonSkill_StartBigElbow( GamePlayer *player )
{
	double z_speed;
	GamePlayer_ChangeState( player, STATE_BIG_ELBOW );
	GamePlayer_SetAttackTypeName( player, ATK_BIG_ELBOW );
	GameObject_ClearAttack( player->object );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	z_speed = GameObject_GetZSpeed( player->object );
	GameObject_AtLanding( player->object, z_speed, -ZACC_JUMP, GamePlayer_AtBigElbowStep1 );
}

static void CommonSkill_StartGuillotine( GamePlayer *player )
{
	double z_speed;
	GamePlayer_ChangeState( player, STATE_GUILLOTINE );
	GamePlayer_SetAttackTypeName( player, ATK_GUILLOTINE );
	GameObject_ClearAttack( player->object );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	z_speed = GameObject_GetZSpeed( player->object );
	GameObject_AtLanding( player->object, z_speed, -ZACC_JUMP, GamePlayer_AtBigElbowStep1 );
}

static LCUI_BOOL CommonSkill_CanUseJump( GamePlayer *player )
{
	if( !player->control.jump ) {
		return FALSE;
	}
	player->control.jump = FALSE;
	switch(player->state) {
	case STATE_WALK:
	case STATE_STANCE:
	case STATE_READY:
		if( player->lock_action ) {
			return FALSE;
		}
	case STATE_BE_LIFT_SQUAT:
	case STATE_BE_LIFT_STANCE:
	case STATE_LIFT_STANCE:
	case STATE_LIFT_WALK:
	case STATE_LIFT_RUN:
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
	case STATE_AS_ATTACK:
	case STATE_BS_ATTACK:
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

static void GamePlayer_PorcJumpTouch( LCUI_Widget *self, LCUI_Widget *other )
{
	int state;
	double x, y;
	GamePlayer *player, *other_player;
	RangeBox head_range, foot_range;
	
	player = GamePlayer_GetPlayerByWidget( self );
	if( player->state != STATE_JUMP
	 && player->state != STATE_SJUMP ) {
		GameObject_AtTouch( self, NULL );
		return;
	}
	/* 若自己不处于下落状态 */
	if( GameObject_GetZSpeed( self ) > -10 ) {
		return;
	}
	GameObject_GetHitRange( other, &head_range );
	GameObject_GetHitRange( self, &foot_range );
	/* 计算对方的头部范围 */
	head_range.z = head_range.z_width - 5;
	head_range.z_width = 5;
	head_range.x += 5;
	head_range.x_width -= 10;
	/* 计算自己的脚部范围 */
	foot_range.z_width = 5;
	foot_range.x += 5;
	foot_range.x_width -= 10;
	/* 若不相交 */
	if( !RangeBox_IsIntersect( &foot_range, &head_range ) ) {
		return;
	}
	other_player = GamePlayer_GetPlayerByWidget( other );
	switch( other_player->state ) {
	case STATE_READY:
	case STATE_STANCE:
		state = STATE_LIFT_STANCE;
		break;
	case STATE_WALK:
		state = STATE_LIFT_WALK;
		break;
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
		state = STATE_LIFT_RUN;
		break;
	default:
		/* 对方的状态不符合要求则退出 */
		 return;
	}
	player->other = other_player;
	other_player->other = player;
	x = GameObject_GetX( other );
	y = GameObject_GetY( other );
	GameObject_SetX( self, x );
	GameObject_SetY( self, y );
	GamePlayer_ChangeState( other_player, state );
	GamePlayer_StopXMotion( player );
	GameObject_SetZSpeed( self, 0 );
	GameObject_SetZAcc( self, 0 );
	GameObject_SetZ( self, LIFT_HEIGHT );
	GamePlayer_ChangeState( player, STATE_BE_LIFT_SQUAT );
	GameObject_AtActionDone( self, ACTION_SQUAT, GamePlayer_SetBeLiftStance );
	/* 在举起者的位置变化时更新被举起者的位置 */
	GameObject_AtMove( other, GamePlayer_UpdateLiftPosition );
}

/** 在起跳动作结束时 */
static void GamePlayer_AtSquatActionDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_ChangeState( player, STATE_JUMP );
	/* 在跳跃过程中检测是否有碰撞 */
	GameObject_AtTouch( widget, GamePlayer_PorcJumpTouch );
}

static void CommonSkill_StartNormalJump( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	/* 跳跃后，重置X轴上的加速度为0 */
	GameObject_SetXAcc( player->object, 0 );
	GamePlayer_ChangeState( player, STATE_JSQUAT );
	GamePlayer_LockMotion( player );
	GameObject_AtActionDone( player->object, ACTION_SQUAT, GamePlayer_AtSquatActionDone );
	GameObject_AtLanding( player->object, ZSPEED_JUMP, -ZACC_JUMP, GamePlayer_AtLandingDone );
}

/** 在被举起的状态下，主动起跳 */
static void CommonSkill_StartActiveJumpOnBeLift( GamePlayer *player )
{
	if( player->other ) {
		switch( player->other->state ) {
		case STATE_LIFT_FALL:
		case STATE_LIFT_JUMP:
			GamePlayer_ChangeState( player->other, STATE_JUMP );
			break;
		case STATE_LIFT_STANCE:
			GamePlayer_ChangeState( player->other, STATE_STANCE );
			break;
		case STATE_LIFT_WALK:
			GamePlayer_ChangeState( player->other, STATE_WALK );
			break;
		case STATE_LIFT_RUN:
			if( GamePlayer_IsLeftOriented(player->other) ) {
				GamePlayer_ChangeState( player->other, STATE_LEFTRUN );
			} else {
				GamePlayer_ChangeState( player->other, STATE_RIGHTRUN );
			}
			break;
		default: break;
		}
		player->other->other = NULL;
		player->other = NULL;
	}
	GamePlayer_LockMotion( player );
	if( player->control.left_motion ) {
		GameObject_SetXSpeed( player->object, -XSPEED_WALK );
	}
	else if( player->control.right_motion ){
		GameObject_SetXSpeed( player->object, XSPEED_WALK );
	}
	CommonSkill_StartNormalJump( player );
}

/** 举着，下落 */
static void GamePlayer_SetLiftFall( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	/* 撤销响应 */
	GameObject_AtZeroZSpeed( player->object, NULL );
	if( player->other == NULL ) {
		GamePlayer_ChangeState( player, STATE_JUMP );
		return;
	}
	GamePlayer_ChangeState( player, STATE_LIFT_FALL );
}

/** 举着，着陆 */
static void GamePlayer_AtLiftLanding( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	if( player->state == STATE_THROW ) {
		GamePlayer_ReduceSpeed( player, 75 );
	}
	else if( player->state == STATE_LIFT_JUMP
	 || player->state == STATE_LIFT_FALL ) {
		GamePlayer_ChangeState( player, STATE_LIFT_STANCE );
	}
	GamePlayer_UnlockMotion( player );
}

/** 举着，起跳 */
static void CommonSkill_StartJumpOnLift( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_LockMotion( player );
	GamePlayer_ChangeState( player, STATE_LIFT_JUMP );
	GameObject_AtZeroZSpeed( player->object, GamePlayer_SetLiftFall );
	GameObject_AtLanding( player->object, ZSPEED_JUMP, -ZACC_JUMP, GamePlayer_AtLiftLanding );
}

/** 在冲刺后的起跳动作结束时 */
static void GamePlayer_AtSprintSquatDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_ChangeState( player, STATE_SJUMP );
	GameObject_AtTouch( widget, GamePlayer_PorcJumpTouch );
}

/** 冲刺+起跳 */
static void CommonSkill_StartJumpOnRun( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GameObject_SetXAcc( player->object, 0 );
	GamePlayer_ChangeState( player, STATE_SSQUAT );
	GamePlayer_LockMotion( player );
	GameObject_AtActionDone( player->object, ACTION_SQUAT, GamePlayer_AtSprintSquatDone );
	GameObject_AtLanding( player->object, ZSPEED_JUMP, -ZACC_JUMP, GamePlayer_AtLandingDone );
}

static void CommonSkill_StartJump( GamePlayer *player )
{
	switch(player->state) {
	case STATE_WALK:
	case STATE_STANCE:
	case STATE_READY:
		CommonSkill_StartNormalJump( player );
		break;
	case STATE_BE_LIFT_SQUAT:
	case STATE_BE_LIFT_STANCE:
		CommonSkill_StartActiveJumpOnBeLift( player );
		break;
	case STATE_LIFT_STANCE:
	case STATE_LIFT_WALK:
	case STATE_LIFT_RUN:
		player->control.run = FALSE;
		CommonSkill_StartJumpOnLift( player );
		break;
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
	case STATE_AS_ATTACK:
	case STATE_BS_ATTACK:
		player->control.run = FALSE;
		CommonSkill_StartJumpOnRun( player );
		break;
	default:
		break;
	}
}

static LCUI_BOOL CommonSkill_CanUsePush( GamePlayer *player )
{
	if( player->state != STATE_CATCH || !player->other ) {
		return FALSE;
	}
	if( !player->control.b_attack ) {
		return FALSE;
	}
	return TRUE;
}

static void GamePlayer_AtWeakWalkDone( GamePlayer *player )
{
	double speed;
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_F_ROLL );
	GamePlayer_LockAction( player );
	speed = GameObject_GetXSpeed( player->object );
	GameObject_SetXAcc( player->object, -speed*4 );
	GamePlayer_SetActionTimeOut( player, ROLL_TIMEOUT, GamePlayer_AtForwardRollTimeOut );
}

static void CommonSkill_StartBackPush( GamePlayer *player )
{
	GamePlayer *other_player;

	other_player = player->other;
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockAction( player->other );
	GamePlayer_ChangeState( player, STATE_CATCH_SKILL_BB );
	GameObject_AtActionDone( player->object, ACTION_CATCH_SKILL_BB, GamePlayer_AtAttackDone );
	GamePlayer_ChangeState( player->other, STATE_WEAK_RUN );
	/* 在 STATE_WEAK_RUN 状态持续一段时间后结束 */
	GamePlayer_SetActionTimeOut( player->other, WEAK_WALK_TIMEOUT, GamePlayer_AtWeakWalkDone );
	GamePlayer_LockAction( player );
	GamePlayer_LockAction( player->other );
	if( GamePlayer_IsLeftOriented(player->other) ) {
		GameObject_SetXSpeed( player->other->object, -XSPEED_WEAK_WALK );
	} else {
		GameObject_SetXSpeed( player->other->object, XSPEED_WEAK_WALK );
	}
	GamePlayer_LockMotion( player->other );
	player->other = NULL;
	other_player->other = NULL;
}

/** 处理游戏角色在进行虚弱奔跑时与其他角色的碰撞 */
static void GamePlayer_ProcWeakWalkAttack( LCUI_Widget *self, LCUI_Widget *other )
{
	double x1, x2;
	GamePlayer *player, *other_player;

	player = GamePlayer_GetPlayerByWidget( self );
	other_player = GamePlayer_GetPlayerByWidget( other );
	if( !player || !other_player ) {
		return;
	}
	/* 如果自己并不是处于 虚弱奔跑（带攻击） 的状态 */
	if( player->state != STATE_WEAK_RUN_ATTACK ) {
		return;
	}
	x1 = GameObject_GetX( self );
	x2 = GameObject_GetX( other );
	/* 如果自己面朝左边 */
	if( GamePlayer_IsLeftOriented( player ) ) {
		/* 若对方位置并不在自己左边，则不产生碰撞 */
		if( x2 > x1-10 ) {
			return;
		}
	}
	/* 反之，也一样 */
	else if( x2 < x1+10 ) {
		return;
	}
	if( GamePlayer_IsLeftOriented( player ) ) {
		AttackEffect_RightShortHitFly( player );
	} else {
		AttackEffect_LeftShortHitFly( player );
	}
	/* 如果对方是背对着自己，且可以应用攻击效果，则对方会被推走 */
	if( GamePlayer_IsLeftOriented(player)
	 == GamePlayer_IsLeftOriented(other_player)
	 && GamePlayer_CanUseAttackEffect(other_player) ) {
		AttackEffect_BePushed( other_player );
	} else {
		/* 根据自己面向的方向，判定短距离击飞的方向 */
		if( GamePlayer_IsLeftOriented(player) ) {
			AttackEffect_LeftShortHitFly( other_player );
		} else {
			AttackEffect_RightShortHitFly( other_player );
		}
	}
	/* 两个都受到攻击伤害 */
	Game_RecordAttack( other_player, ATK_BUMPED2, player, player->state );
	Game_RecordAttack( player, ATK_BUMPED2, other_player, other_player->state );
	GameObject_AtTouch( player->object, NULL );
	player->n_attack = 0;
	other_player->n_attack = 0;
}

static void CommonSkill_StartFrontPush( GamePlayer *player )
{
	GamePlayer *other_player;

	other_player = player->other;
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockAction( other_player );
	if( GamePlayer_IsLeftOriented(player) ) {
		GamePlayer_SetRightOriented( player );
	} else {
		GamePlayer_SetLeftOriented( player );
	}
	GamePlayer_ChangeState( player, STATE_CATCH_SKILL_FB );
	GameObject_AtActionDone( player->object, ACTION_CATCH_SKILL_FB, GamePlayer_AtAttackDone );
	GamePlayer_ChangeState( player->other, STATE_WEAK_RUN_ATTACK );
	/* 在与其他对象触碰时进行响应 */
	GameObject_AtTouch( other_player->object, GamePlayer_ProcWeakWalkAttack );
	GamePlayer_SetActionTimeOut( player->other, WEAK_WALK_TIMEOUT, GamePlayer_AtWeakWalkDone );
	GamePlayer_LockAction( player );
	GamePlayer_LockAction( other_player );
	if( GamePlayer_IsLeftOriented(player->other) ) {
		GameObject_SetXSpeed( other_player->object, -XSPEED_WEAK_WALK );
	} else {
		GameObject_SetXSpeed( other_player->object, XSPEED_WEAK_WALK );
	}
	GamePlayer_LockMotion( other_player );
	player->other = NULL;
	other_player->other = NULL;
}

static void CommonSkill_StartPush( GamePlayer *player )
{
	/* 根据方向，判断该使用何种技能 */
	if( GamePlayer_IsLeftOriented(player) ) {
		if( GamePlayer_IsLeftOriented(player->other) ) {
			CommonSkill_StartBackPush( player );
		} else {
			CommonSkill_StartFrontPush( player );
		}
	} else {
		if( GamePlayer_IsLeftOriented(player->other) ) {
			CommonSkill_StartFrontPush( player );
		} else {
			CommonSkill_StartBackPush( player );
		}
	}
}

static LCUI_BOOL CommonSkill_CanUseDefense( GamePlayer *player )
{
	if( player->lock_action ) {
		return FALSE;
	}
	if( !player->control.defense ) {
		return FALSE;
	}
	if( !GamePlayerStateCanNormalAttack(player) ) {
		return FALSE;
	}
	return TRUE;
}

static void CommonSkill_StartDefense( GamePlayer *player )
{
	GamePlayer_StopXMotion( player );
	GamePlayer_StopYMotion( player );
	GamePlayer_LockMotion( player );
	if( GamePlayer_HaveSkill( player, SKILLNAME_SOLID_DEFENSE ) ) {
		GamePlayer_ChangeState( player, STATE_SOLID_DEFENSE );
	} else {
		GamePlayer_ChangeState( player, STATE_DEFENSE );
	}
}

static LCUI_BOOL CommonSkill_CanUseJumpSpinKick( GamePlayer *player )
{
	if( player->lock_action ) {
		return FALSE;
	}
	if( player->state != STATE_SJUMP
	 && player->state != STATE_SSQUAT ) {
		 return FALSE;
	}
	if( !player->control.down_motion ) {
		return FALSE;
	}
	return TRUE;
}

static void CommonSkill_StartSpinKickDown( LCUI_Widget *widget )
{
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	/* 增加下落的速度 */
	GameObject_SetZSpeed( player->object, -ZSPEED_JUMP*2.5 );
	GameObject_SetZAcc( player->object, 0 );
	/* 根据角色目前面向的方向，设定水平移动方向及速度 */
	if( GamePlayer_IsLeftOriented( player ) ) {
		GameObject_SetXSpeed( player->object, -(XSPEED_RUN*5) );
	} else {
		GameObject_SetXSpeed( player->object, XSPEED_RUN*5 );
	}
	GameObject_AtZeroZSpeed( player->object, NULL );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_KICK );
	GamePlayer_LockAction( player );
}

static void CommonSkill_StartJumpSpinKick( GamePlayer *player )
{
	double z_speed;
	/* 如果该游戏角色并没处于奔跑后的 跳跃 的状态下 */
	if( player->state != STATE_SJUMP
	&& player->state != STATE_SSQUAT ) {
		return;
	}
	/* 增加点在Z轴的移动速度，以增加高度 */
	z_speed = GameObject_GetZSpeed( player->object );
	z_speed += ZSPEED_JUMP;
	GameObject_SetZSpeed( player->object, z_speed );
	GameObject_SetZAcc( player->object, -z_speed*3 );
	GamePlayer_SetAttackTypeName( player, ATK_JUMP_SPINKICK );
	GameObject_ClearAttack( player->object );
	/* 开始翻滚 */
	GamePlayer_ChangeState( player, STATE_SPINHIT );
	/* 锁定动作和移动 */
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	/* 滚了一会后再开始出脚 */
	GameObject_AtZeroZSpeed( player->object, CommonSkill_StartSpinKickDown );
}

/** 注册A攻击 */
static void CommonSkill_RegisterAAttack(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_A_ATTACK, 
				SKILLPRIORITY_A_ATTACK,
				CommonSkill_CanUseAAttack,
				CommonSkill_StartAAttack
	);
	SkillLibrary_AddSkill(	SKILLNAME_MACH_A_ATTACK, 
				SKILLPRIORITY_MACH_A_ATTACK,
				CommonSkill_CanUseAAttack,
				CommonSkill_StartMachAAttack
	);
	AttackLibrary_AddAttack( ATK_A_ATTACK, AttackDamage_AAttack, AttackEffect_Normal );
	AttackLibrary_AddAttack( ATK_MACH_A_ATTACK, AttackDamage_MachAAttack, AttackEffect_Normal );
}

/** 注册B攻击 */
static void CommonSkill_RegisterBAttack(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_B_ATTACK, 
				SKILLPRIORITY_B_ATTACK,
				CommonSkill_CanUseBAttack,
				CommonSkill_StartBAttack
	);
	SkillLibrary_AddSkill(	SKILLNAME_MACH_B_ATTACK, 
				SKILLPRIORITY_MACH_B_ATTACK,
				CommonSkill_CanUseBAttack,
				CommonSkill_StartMachBAttack
	);
	AttackLibrary_AddAttack( ATK_B_ATTACK, AttackDamage_BAttack, AttackEffect_Normal );
	AttackLibrary_AddAttack( ATK_MACH_B_ATTACK, AttackDamage_MachBAttack, AttackEffect_Normal );
}

/** 注册跳跃A攻击 */
static void CommonSkill_RegisterJumpAAttack(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_JUMP_A_ATTACK, 
				SKILLPRIORITY_JUMP_A_ATTACK,
				CommonSkill_CanUseJumpAAttack,
				CommonSkill_StartJumpAAttack
	);
	SkillLibrary_AddSkill(	SKILLNAME_JUMP_MACH_A_ATTACK, 
				SKILLPRIORITY_JUMP_MACH_A_ATTACK,
				CommonSkill_CanUseJumpAAttack,
				CommonSkill_StartJumpMachAAttack
	);
	AttackLibrary_AddAttack( ATK_JUMP_A_ATTACK, AttackDamage_MachAAttack, AttackEffect_Normal2 );
	AttackLibrary_AddAttack( ATK_JUMP_MACH_A_ATTACK, AttackDamage_MachAAttack, AttackEffect_Normal2 );
}

/** 注册跳跃B攻击 */
static void CommonSkill_RegisterJumpBAttack(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_JUMP_B_ATTACK, 
				SKILLPRIORITY_JUMP_B_ATTACK,
				CommonSkill_CanUseJumpBAttack,
				CommonSkill_StartJumpBAttack
	);
	SkillLibrary_AddSkill(	SKILLNAME_JUMP_MACH_B_ATTACK, 
				SKILLPRIORITY_JUMP_MACH_B_ATTACK,
				CommonSkill_CanUseJumpBAttack,
				CommonSkill_StartJumpMachBAttack
	);
	AttackLibrary_AddAttack( ATK_JUMP_B_ATTACK, AttackDamage_BAttack, AttackEffect_Normal2 );
	AttackLibrary_AddAttack( ATK_JUMP_MACH_B_ATTACK, AttackDamage_BAttack, AttackEffect_Normal2 );
}

/** 注册冲刺跳跃A攻击 */
static void CommonSkill_RegisterSprintJumpAAttack(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_SPRINT_JUMP_A_ATTACK, 
				SKILLPRIORITY_SPRINT_JUMP_A_ATTACK,
				CommonSkill_CanUseSprintJumpAAttack,
				CommonSkill_StartSprintJumpAAttack
	);
	AttackLibrary_AddAttack( ATK_SPRINT_JUMP_A_ATTACK, AttackDamage_SprintJumpAAttack, AttackEffect_ShortHitFly );
}

/** 注册冲刺跳跃B攻击 */
static void CommonSkill_RegisterSprintJumpBAttack(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_SPRINT_JUMP_B_ATTACK, 
				SKILLPRIORITY_SPRINT_JUMP_B_ATTACK,
				CommonSkill_CanUseSprintJumpBAttack,
				CommonSkill_StartSprintJumpBAttack
	);
	AttackLibrary_AddAttack( ATK_SPRINT_JUMP_B_ATTACK, AttackDamage_SprintJumpBAttack, AttackEffect_ShortHitFly );
}

/** 注册 终结一击 技能 */
static void CommonSkill_RegisterFinalBlow(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_A_FINALBLOW, 
				SKILLPRIORITY_A_FINALBLOW,
				CommonSkill_CanUseFinalBlow,
				CommonSkill_StartFinalBlow
	);
	SkillLibrary_AddSkill(	SKILLNAME_B_FINALBLOW, 
				SKILLPRIORITY_B_FINALBLOW,
				CommonSkill_CanUseFinalBlow,
				CommonSkill_StartFinalBlow
	);
	AttackLibrary_AddAttack( ATK_FINALBLOW, AttackDamage_FinalBlow, AttackEffect_LongHitFly );
}

static void CommonSkill_RegisterSprintAAttack(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_SPRINT_A_ATTACK,
				SKILLPRIORITY_SPRINT_A_ATTACK,
				CommonSkill_CanUseASprintAttack,
				CommonSkill_StartSprintAAttack
	);
	AttackLibrary_AddAttack( ATK_SPRINT_A_ATTACK, AttackDamage_SprintAAttack, AttackEffect_BumpToFly );
}

static void CommonSkill_RegisterSprintBAttack(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_SPRINT_B_ATTACK,
				SKILLPRIORITY_SPRINT_B_ATTACK,
				CommonSkill_CanUseBSprintAttack,
				CommonSkill_StartSprintBAttack
	);
	AttackLibrary_AddAttack( ATK_SPRINT_B_ATTACK, AttackDamage_SprintBAttack, AttackEffect_BumpToFly );
}

static void CommonSkill_RegisterPush(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_PUSH,
				SKILLPRIORITY_PUSH,
				CommonSkill_CanUsePush,
				CommonSkill_StartPush
	);
	AttackLibrary_AddAttack( ATK_BUMPED2, AttackDamage_Bumped, NULL );
}

static void CommonSkill_RegisterMachStomp(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_MACH_STOMP,
				SKILLPRIORITY_MACH_STOMP,
				CommonSkill_CanAAttackGround,
				CommonSkill_StartMachStomp
	);
	AttackLibrary_AddAttack( ATK_MACH_STOMP, AttackDamage_MachStomp, AttackEffect_Normal2 );
}

static void CommonSkill_RegisterJumpElbow(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_JUMP_ELBOW,
				SKILLPRIORITY_JUMP_ELBOW,
				CommonSkill_CanAAttackGround,
				CommonSkill_StartJumpElbow
	);
	AttackLibrary_AddAttack( ATK_JUMP_ELBOW, AttackDamage_JumpElbow, AttackEffect_Normal2 );
}

static void CommonSkill_RegisterJumpTread(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_JUMP_TREAD,
				SKILLPRIORITY_JUMP_TREAD,
				CommonSkill_CanBAttackGround,
				CommonSkill_StartJumpTread
	);
	AttackLibrary_AddAttack( ATK_JUMP_TREAD, AttackDamage_JumpTread, AttackEffect_Normal2 );
}

static void CommonSkill_RegisterLift(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_A_LIFT,
				SKILLPRIORITY_A_LIFT,
				CommonSkill_CanALift,
				CommonSkill_StartLift
	);
	SkillLibrary_AddSkill(	SKILLNAME_B_LIFT,
				SKILLPRIORITY_B_LIFT,
				CommonSkill_CanBLift,
				CommonSkill_StartLift
	);
}

/** 注册 爆裂腿技能 */
static void CommonSkill_RegisterBombKick(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_BOMBKICK,
				SKILLPRIORITY_BOMBKICK,
				CommonSkill_CanUseBombKick,
				CommonSkill_StartBombKick
	);
	AttackLibrary_AddAttack( ATK_BOMBKICK, AttackDamage_BombKick, AttackEffect_LongHitFly );
}

/** 注册 自旋击技能 */
static void CommonSkill_RegisterSpinHit(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_SPINHIT,
				SKILLPRIORITY_SPINHIT,
				CommonSkill_CanUseSpinHit,
				CommonSkill_StartSpinHit
	);
	AttackLibrary_AddAttack( ATK_SPINHIT, AttackDamage_SpinHit, AttackEffect_LongHitFly );
}

/** 注册 二段自旋击技能 */
static void CommonSkill_RegisterSpinHit2(void)
{
	AttackLibrary_AddAttack( ATK_SPINHIT_2, AttackDamage_SpinHit, AttackEffect_LongHitFly );
}

static void CommonSkill_RegisterThrow(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_A_THROW,
				SKILLPRIORITY_A_THROW,
				CommonSkill_CanUseAThrow,
				CommonSkill_StartAThrow
	);
	SkillLibrary_AddSkill(	SKILLNAME_B_THROW,
				SKILLPRIORITY_B_THROW,
				CommonSkill_CanUseBThrow,
				CommonSkill_StartBThrow
	);
	AttackLibrary_AddAttack( ATK_THROW, AttackDamage_Throw, NULL );
	AttackLibrary_AddAttack( ATK_BUMPED, AttackDamage_Bumped, AttackEffect_TouchHitFly );
}

static void CommonSkill_RegisterRide(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_RIDE,
				SKILLPRIORITY_RIDE,
				CommonSkill_CanUseRide,
				CommonSkill_StartRide
	);
}

static void CommonSkill_RegisterRideAttack(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_RIDE_A_ATTACK,
				SKILLPRIORITY_RIDE_A_ATTACK,
				CommonSkill_CanUseRideAAttack,
				CommonSkill_StartRideAAttack
	);
	SkillLibrary_AddSkill(	SKILLNAME_RIDE_B_ATTACK,
				SKILLPRIORITY_RIDE_B_ATTACK,
				CommonSkill_CanUseRideBAttack,
				CommonSkill_StartRideBAttack
	);
	AttackLibrary_AddAttack( ATK_RIDE_A_ATTACK, AttackDamage_RideAAttack, AttackEffect_Normal );
	AttackLibrary_AddAttack( ATK_RIDE_B_ATTACK, AttackDamage_RideBAttack, AttackEffect_Normal );
}

static void CommonSkill_RegisterCatch(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_CATCH,
				SKILLPRIORITY_CATCH,
				CommonSkill_CanUseCatch,
				CommonSkill_StartCatch
	);
}

static void CommonSkill_RegisterBigElbow(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_BIG_ELBOW,
				SKILLPRIORITY_BIG_ELBOW,
				CommonSkill_CanUseBigElbow,
				CommonSkill_StartBigElbow
	);
	AttackLibrary_AddAttack( ATK_BIG_ELBOW, AttackDamage_BigElbow, AttackEffect_LongHitFly );
}

static void CommonSkill_RegisterGuillotine(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_GUILLOTINE,
				SKILLPRIORITY_GUILLOTINE,
				CommonSkill_CanUseGuillotine,
				CommonSkill_StartGuillotine
	);
	AttackLibrary_AddAttack( ATK_GUILLOTINE, AttackDamage_Guillotine, AttackEffect_LongHitFly );
}

static void CommonSkill_RegisterJump(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_JUMP,
				SKILLPRIORITY_JUMP,
				CommonSkill_CanUseJump,
				CommonSkill_StartJump
	);
}

static void CommonSkill_RegisterJumpSpinKick(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_JUMP_SPINKICK,
				SKILLPRIORITY_JUMP_SPINKICK,
				CommonSkill_CanUseJumpSpinKick,
				CommonSkill_StartJumpSpinKick
	);
	AttackLibrary_AddAttack( ATK_JUMP_SPINKICK, AttackDamage_JumpSpinKick, AttackEffect_LongHitFly );
}

static void CommonSkill_RegisterDefense(void)
{
	SkillLibrary_AddSkill(	SKILLNAME_DEFENSE,
				SKILLPRIORITY_DEFENSE,
				CommonSkill_CanUseDefense,
				CommonSkill_StartDefense
	);
}

/** 注册通用技能 */
void CommonSkill_Register(void)
{
	CommonSkill_RegisterAAttack();
	CommonSkill_RegisterBAttack();
	CommonSkill_RegisterJump();
	CommonSkill_RegisterDefense();
	CommonSkill_RegisterJumpAAttack();
	CommonSkill_RegisterJumpBAttack();
	CommonSkill_RegisterSprintAAttack();
	CommonSkill_RegisterSprintBAttack();
	CommonSkill_RegisterSprintJumpAAttack();
	CommonSkill_RegisterSprintJumpBAttack();
	CommonSkill_RegisterFinalBlow();
	CommonSkill_RegisterJumpElbow();
	CommonSkill_RegisterJumpTread();
	CommonSkill_RegisterLift();
	CommonSkill_RegisterThrow();
	CommonSkill_RegisterMachStomp();
	CommonSkill_RegisterBombKick();
	CommonSkill_RegisterSpinHit();
	CommonSkill_RegisterSpinHit2();
	CommonSkill_RegisterBigElbow();
	CommonSkill_RegisterGuillotine();
	CommonSkill_RegisterRide();
	CommonSkill_RegisterRideAttack();
	CommonSkill_RegisterCatch();
	CommonSkill_RegisterPush();
	CommonSkill_RegisterJumpSpinKick();
}

/** 在擒获范围内，获取处于冲撞攻击状态的玩家 */
GamePlayer *GetSpirntAttackerInCatchRange( GamePlayer *self )
{
	RangeBox catch_range;
	LCUI_Widget *a_attacker, *b_attacker;
	/* 自己必须处于READY或STANCE状态 */
	if( self->state != STATE_READY
	 && self->state != STATE_STANCE
	 && self->state != STATE_WALK ) {
		 return NULL;
	}
	catch_range.x = 0;
	catch_range.y = 0;
	catch_range.z = 0;
	catch_range.x_width = CATCH_RANGE_X_WIDTH;
	catch_range.y_width = GLOBAL_Y_WIDTH;
	catch_range.z_width = 40;
	/* 获取正使用冲撞A/B攻击的游戏对象 */
	a_attacker = GameObject_GetObjectInRange( 
				self->object, catch_range,
				TRUE, ACTION_AS_ATTACK );
	b_attacker = GameObject_GetObjectInRange( 
				self->object, catch_range, 
				TRUE, ACTION_BS_ATTACK );
	/* 若有两个 */
	 if( a_attacker && b_attacker ) {
		 /* 根据自己的朝向，确保a_attacker记录的是离自己最近的攻击者 */
		if( GamePlayer_IsLeftOriented(self) ) {
			if(GameObject_GetX(a_attacker)
			 < GameObject_GetX(b_attacker) ) {
				 a_attacker = b_attacker;
			 }
		} else {
			if(GameObject_GetX(b_attacker)
			 < GameObject_GetX(a_attacker) ) {
				 a_attacker = b_attacker;
			 }
		}
	}
	else if( b_attacker ) {
		a_attacker = b_attacker;
	}
	if( !a_attacker ) {
		return NULL;
	}
	return GamePlayer_GetPlayerByWidget( a_attacker );
}
