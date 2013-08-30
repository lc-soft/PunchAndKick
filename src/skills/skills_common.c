// 通用技能
#include <LCUI_Build.h>
#include LC_LCUI_H
#include "../game.h"
#include "game_skill.h"

/** 处于被举起的状态下，在攻击结束时  */
static void GamePlayer_AtBeLiftAttackDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	player->attack_type = ATTACK_TYPE_NONE;
	GamePlayer_ChangeState( player, STATE_BE_LIFT_STANCE );
}

/** 在攻击结束时  */
static void GamePlayer_AtAttackDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GameObject_AtXSpeedToZero( widget, 0, NULL );
	player->attack_type = ATTACK_TYPE_NONE;
	GamePlayer_ResetAttackControl( player );
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
	GamePlayer_SetReady( player );
}

/** 检测是否能够使用终结一击 */
static LCUI_BOOL CommonSkill_CanUseFinalBlow( GamePlayer *player )
{
	LCUI_Widget *widget;
	/* 如果并没有要使用A/B攻击，则不符合发动条件 */
	if( !player->control.a_attack && !player->control.b_attack ) {
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
	player->attack_type = ATTACK_TYPE_FINAL_BLOW;
	GamePlayer_StopXWalk( player );
	GamePlayer_StopYMotion( player );
	GamePlayer_LockMotion( player );
	GamePlayer_LockAction( player );
}

static LCUI_BOOL GamePlayerStateCanNormalAttack( GamePlayer *player )
{
	switch(player->state) {
	case STATE_READY:
	case STATE_WALK:
	case STATE_STANCE:
	case STATE_BE_LIFT_STANCE:
		return TRUE;
	default: 
		break;
	}
	return FALSE;
}

/** 检测是否能够使用A攻击 */
static LCUI_BOOL CommonSkill_CanUseAAttack( GamePlayer *player )
{
	if( !player->control.a_attack ) {
		return FALSE;
	}
	return GamePlayerStateCanNormalAttack( player );
}

/** 检测是否能够使用B攻击 */
static LCUI_BOOL CommonSkill_CanUseBAttack( GamePlayer *player )
{
	if( !player->control.b_attack ) {
		return FALSE;
	}
	return GamePlayerStateCanNormalAttack( player );
}

/** 开始发动A普通攻击 */
static void CommonSkill_StartAAttack( GamePlayer *player )
{
	void (*func)(LCUI_Widget*);

	if( player->state == STATE_BE_LIFT_STANCE
	|| player->state == STATE_BE_LIFT_SQUAT ) {
		func = GamePlayer_AtBeLiftAttackDone;
	} else {
		func = GamePlayer_AtAttackDone;
	}
	GamePlayer_ChangeState( player, STATE_A_ATTACK );
	GameObject_AtActionDone( player->object, ACTION_A_ATTACK, func );
	player->attack_type = ATTACK_TYPE_PUNCH;
	GamePlayer_StopXWalk( player );
	GamePlayer_StopYMotion( player );
	GamePlayer_LockMotion( player );
	GamePlayer_LockAction( player );
}

static void _StartAttack( GamePlayer *player, int state, int action, int attack_type )
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
	player->attack_type = attack_type;
	GamePlayer_StopXWalk( player );
	GamePlayer_StopYMotion( player );
	GamePlayer_LockMotion( player );
	GamePlayer_LockAction( player );
}

/** 开始发动B普通攻击 */
static void CommonSkill_StartBAttack( GamePlayer *player )
{
	_StartAttack(	player, STATE_B_ATTACK, 
			ACTION_B_ATTACK, ATTACK_TYPE_KICK );
}

/** 开始发动高速A攻击 */
static void CommonSkill_StartMachAAttack( GamePlayer *player )
{
	_StartAttack(	player, STATE_MACH_A_ATTACK,
			ACTION_MACH_A_ATTACK, ATTACK_TYPE_PUNCH );
}

/** 开始发动高速B攻击 */
static void CommonSkill_StartMachBAttack( GamePlayer *player )
{
	_StartAttack(	player, STATE_MACH_B_ATTACK, 
			ACTION_MACH_B_ATTACK, ATTACK_TYPE_KICK );
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
}

/** 注册通用技能 */
void CommonSkill_Register(void)
{
	CommonSkill_RegisterAAttack();
	CommonSkill_RegisterBAttack();
	CommonSkill_RegisterFinalBlow();
}
