// 拳击家特有技能
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include "../game.h"
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
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_StartStand( player );
}

static void GamePlayer_AtElbowUpdate( LCUI_Widget *widget )
{
	GamePlayer *player;
	
	player = GamePlayer_GetPlayerByWidget( widget );
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
	if( player->state != STATE_CATCH ) {
		GamePlayer_StopXMotion( player->other );
		GamePlayer_StopYMotion( player->other );
		CommonSkill_SetPositionAtCatch( player, player->other );
	}
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockAction( player->other );
	GamePlayer_ChangeState( player->other, STATE_HIT );
	GameObject_AtActionDone( player->other->object, ACTION_HIT, NULL );
	if( GamePlayer_IsLeftOriented(player) ) {
		GamePlayer_SetLeftOriented( player->other );
	} else {
		GamePlayer_SetRightOriented( player->other );
	}
	GamePlayer_ChangeState( player, STATE_CATCH_SKILL_FA );
	GameObject_AtActionUpdate(
		player->object,
		ACTION_CATCH_SKILL_FA, 
		GamePlayer_AtElbowUpdate 
	);
	GameObject_AtActionDone(
		player->object, ACTION_CATCH_SKILL_FA, 
		GamePlayer_AtSkillDone 
	);
	/* 重置被攻击的次数 */
	player->other->n_attack = 0;
	GamePlayer_LockAction( player );
	GamePlayer_LockAction( player->other );
	GamePlayer_LockMotion( player->other );
}

/** 在被膝击后落地时 */
static void GamePlayer_AtLandingByAfterKneeHit( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
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
	
	player = GamePlayer_GetPlayerByWidget( widget );
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
