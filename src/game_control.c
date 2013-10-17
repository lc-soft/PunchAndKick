#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_INPUT_H
#include LC_TEXTSTYLE_H
#include LC_DISPLAY_H
#include LC_LABEL_H

#include "game.h"
#include "skills/game_skill.h"

static GamePlayer player_data[4];
static LCUI_Widget *player_status_area;
static int state_action_map[TOTAL_STATE_NUM];

/** 通过部件获取游戏玩家数据 */
GamePlayer *GamePlayer_GetPlayerByWidget( LCUI_Widget *widget )
{
	int i;
	for(i=0; i<4; ++i) {
		if( widget == player_data[i].object ) {
			return &player_data[i];
		}
	}
	return NULL;
}

static void ControlKey_Init( ControlKey *key )
{
	key->up = 0;
	key->down = 0;
	key->left = 0;
	key->right = 0;
	key->a_attack = 0;
	key->b_attack = 0;
	key->jump = 0;
	key->defense = 0;
}

/** 重置攻击控制 */
void GamePlayer_ResetAttackControl( GamePlayer *player )
{
	player->control.a_attack = FALSE;
	player->control.b_attack = FALSE;
}

/** 设置角色面向右方 */
void GamePlayer_SetRightOriented( GamePlayer *player )
{
	player->right_direction = TRUE;
	GameObject_SetHorizFlip( player->object, FALSE );
}

/** 设置角色面向左方 */
void GamePlayer_SetLeftOriented( GamePlayer *player )
{
	player->right_direction = FALSE;
	GameObject_SetHorizFlip( player->object, TRUE );
}

/** 判断角色是否面向左方 */
LCUI_BOOL GamePlayer_IsLeftOriented( GamePlayer *player )
{
	return !player->right_direction;
}

/** 通过控制键获取该键控制的角色 */
GamePlayer *GamePlayer_GetPlayerByControlKey( int key_code )
{
	int i;
	for(i=0; i<4; ++i) {
		if( key_code == player_data[i].ctrlkey.a_attack
		 || key_code == player_data[i].ctrlkey.b_attack
		 || key_code == player_data[i].ctrlkey.defense
		 || key_code == player_data[i].ctrlkey.left
		 || key_code == player_data[i].ctrlkey.right
		 || key_code == player_data[i].ctrlkey.up
		 || key_code == player_data[i].ctrlkey.down
		 || key_code == player_data[i].ctrlkey.jump ) {
			 return &player_data[i];
		}
	}
	return NULL;
}

/** 通过角色ID来获取角色 */
GamePlayer *GamePlayer_GetByID( int player_id )
{
	int i;
	for(i=0; i<4; ++i) {
		if( player_data[i].id == player_id ) {
			return &player_data[i];
		}
	}
	return NULL;
}

/** 改变角色的动作动画  */
void GamePlayer_ChangeAction( GamePlayer *player, int action_id )
{
	GameMsg msg;

	msg.player_id = player->id;
	msg.msg.msg_id = GAMEMSG_ACTION;
	msg.msg.action.action_id = action_id;
	Game_PostMsg( &msg );
}

static void GamePlayer_BreakActionTiming( GamePlayer *player )
{
	if( player->t_action_timeout != -1 ) {
		LCUITimer_Free( player->t_action_timeout );
		player->t_action_timeout = -1;
	}
}

/** 为游戏角色的动作设置时限，并在超时后进行响应 */
void GamePlayer_SetActionTimeOut(	GamePlayer *player,
					int n_ms,
					void (*func)(GamePlayer*) )
{
	GamePlayer_BreakActionTiming( player );
	player->t_action_timeout = LCUITimer_Set( n_ms, (void(*)(void*))func, player, FALSE );
}

/** 初始化状态与动作的映射表 */
static void Game_InitStateActionMap(void)
{
	state_action_map[STATE_START] = ACTION_START;
	state_action_map[STATE_LYING_DYING] = ACTION_LYING_DYING;
	state_action_map[STATE_TUMMY_DYING] = ACTION_TUMMY_DYING;
	state_action_map[STATE_READY] = ACTION_READY;
	state_action_map[STATE_STANCE] =
	state_action_map[STATE_BE_LIFT_STANCE] = ACTION_STANCE; 
	state_action_map[STATE_WALK] = ACTION_WALK;
	state_action_map[STATE_LEFTRUN] = 
	state_action_map[STATE_RIGHTRUN] = ACTION_RUN;
	state_action_map[STATE_DEFENSE] = ACTION_DEFENSE;
	state_action_map[STATE_SOLID_DEFENSE] = ACTION_SOLID_DEFENSE;
	state_action_map[STATE_A_ATTACK] = ACTION_A_ATTACK;
	state_action_map[STATE_B_ATTACK] = ACTION_B_ATTACK;
	state_action_map[STATE_MAJ_ATTACK] = ACTION_JUMP_MACH_A_ATTACK;
	state_action_map[STATE_MACH_A_ATTACK] = ACTION_MACH_A_ATTACK;
	state_action_map[STATE_MBJ_ATTACK] = ACTION_JUMP_MACH_B_ATTACK;
	state_action_map[STATE_MACH_B_ATTACK] = ACTION_MACH_B_ATTACK;
	state_action_map[STATE_AS_ATTACK] = ACTION_AS_ATTACK;
	state_action_map[STATE_BS_ATTACK] = ACTION_BS_ATTACK;
	state_action_map[STATE_ASJ_ATTACK] = ACTION_ASJ_ATTACK;
	state_action_map[STATE_BSJ_ATTACK] = ACTION_BSJ_ATTACK;
	state_action_map[STATE_AJ_ATTACK] = ACTION_AJ_ATTACK;
	state_action_map[STATE_BJ_ATTACK] = ACTION_BJ_ATTACK;
	state_action_map[STATE_FINAL_BLOW] = ACTION_FINAL_BLOW;
	state_action_map[STATE_JUMP_DONE] =
	state_action_map[STATE_BE_LIFT_SQUAT] =
	state_action_map[STATE_LIFT_SQUAT] =
	state_action_map[STATE_JSQUAT] =
	state_action_map[STATE_SSQUAT] =
	state_action_map[STATE_SQUAT] = ACTION_SQUAT;
	state_action_map[STATE_JUMP] =
	state_action_map[STATE_SJUMP] = ACTION_JUMP;
	state_action_map[STATE_FALL] = ACTION_FALL;
	state_action_map[STATE_HIT] = ACTION_HIT;
	state_action_map[STATE_HIT_FLY] = ACTION_HIT_FLY;
	state_action_map[STATE_HIT_FLY_FALL] = ACTION_HIT_FLY_FALL;
	state_action_map[STATE_LYING] =
	state_action_map[STATE_BE_LIFT_LYING] = ACTION_LYING;
	state_action_map[STATE_LYING_HIT] =
	state_action_map[STATE_BE_LIFT_LYING_HIT] = ACTION_LYING_HIT;
	state_action_map[STATE_TUMMY_HIT_FLY] = ACTION_TUMMY_HIT_FLY;
	state_action_map[STATE_TUMMY] = 
	state_action_map[STATE_BE_LIFT_TUMMY] = ACTION_TUMMY;
	state_action_map[STATE_TUMMY_HIT] = 
	state_action_map[STATE_BE_LIFT_TUMMY_HIT] = ACTION_TUMMY_HIT;
	state_action_map[STATE_REST] = ACTION_REST;
	state_action_map[STATE_ROLL_DOWN] = ACTION_ROLL_DOWN;
	state_action_map[STATE_ROLL_UP] = ACTION_ROLL_UP;
	state_action_map[STATE_F_ROLL] = ACTION_F_ROLL;
	state_action_map[STATE_B_ROLL] = ACTION_B_ROLL;
	state_action_map[STATE_GUILLOTINE] = ACTION_GUILLOTINE;
	state_action_map[STATE_BIG_ELBOW] =
	state_action_map[STATE_JUMP_ELBOW] = ACTION_JUMP_ELBOW;
	state_action_map[STATE_RIDE_JUMP] = ACTION_FALL;
	state_action_map[STATE_JUMP_STOMP] = ACTION_JUMP_STOMP;
	state_action_map[STATE_KICK] = ACTION_KICK;
	state_action_map[STATE_SPINHIT] = ACTION_SPINHIT;
	state_action_map[STATE_BOMBKICK] = ACTION_BOMBKICK;
	state_action_map[STATE_MACH_STOMP] = ACTION_MACH_STOMP;
	state_action_map[STATE_CATCH] = ACTION_CATCH;
	state_action_map[STATE_BE_CATCH] = ACTION_BE_CATCH;
	state_action_map[STATE_HALF_LYING] = ACTION_HALF_LYING;
	state_action_map[STATE_HALF_STANCE] = ACTION_HALF_STANCE;
	state_action_map[STATE_BACK_BE_CATCH] = ACTION_BACK_BE_CATCH;
	state_action_map[STATE_CATCH_SKILL_FA] = ACTION_CATCH_SKILL_FA;
	state_action_map[STATE_CATCH_SKILL_BA] = ACTION_CATCH_SKILL_BA;
	state_action_map[STATE_CATCH_SKILL_BB] = ACTION_CATCH_SKILL_BB;
	state_action_map[STATE_CATCH_SKILL_FB] = ACTION_CATCH_SKILL_FB;
	state_action_map[STATE_BE_PUSH] = ACTION_BE_PUSH;
	state_action_map[STATE_WEAK_RUN] =
	state_action_map[STATE_WEAK_RUN_ATTACK] = ACTION_WEAK_RUN;
	state_action_map[STATE_LIFT_STANCE] = ACTION_LIFT_STANCE;
	state_action_map[STATE_LIFT_WALK] = ACTION_LIFT_WALK;
	state_action_map[STATE_LIFT_RUN] = ACTION_LIFT_RUN;
	state_action_map[STATE_LIFT_JUMP] = ACTION_LIFT_JUMP;
	state_action_map[STATE_LIFT_FALL] = ACTION_LIFT_FALL;
	state_action_map[STATE_THROW] = ACTION_THROW;
	state_action_map[STATE_RIDE] = ACTION_RIDE;
	state_action_map[STATE_RIDE_ATTACK] = ACTION_RIDE_ATTACK;
}

/** 改变游戏角色的当前状态 */
void GamePlayer_ChangeState( GamePlayer *player, int state )
{
	if( player->lock_action ) {
		return;
	}
	if( state >= TOTAL_STATE_NUM ) {
		return;
	}
	player->state = state;;
	/* 在切换动作时，撤销动作超时的响应 */
	GamePlayer_BreakActionTiming( player );
	GamePlayer_ChangeAction( player, state_action_map[state] );
}

void GamePlayer_LockAction( GamePlayer *player )
{
	player->lock_action = TRUE;
}

void GamePlayer_UnlockAction( GamePlayer *player )
{
	player->lock_action = FALSE;
}

void GamePlayer_LockMotion( GamePlayer *player )
{
	player->lock_motion = TRUE;
}

void GamePlayer_UnlockMotion( GamePlayer *player )
{
	player->lock_motion = FALSE;
}

/** 打断休息 */
void GamePlayer_BreakRest( GamePlayer *player )
{
	if( player->t_rest_timeout != -1 ) {
		LCUITimer_Free( player->t_rest_timeout );
	}
}

/** 为游戏设置休息的时限，并在超时后进行响应 */
void GamePlayer_SetRestTimeOut(	GamePlayer *player,
				int n_ms,
				void (*func)(GamePlayer*) )
{
	GamePlayer_BreakRest( player );
	player->t_rest_timeout = LCUITimer_Set( n_ms, (void(*)(void*))func, player, FALSE );
}

static int GamePlayer_InitAction( GamePlayer *player, int id )
{
	int i;
	ActionData* action;

	player->state = STATE_STANCE;

	for(i=0; i<TOTAL_ACTION_NUM; ++i) {
		/* 载入游戏角色资源 */
		action = ActionRes_Load( id, i );
		/* 将动作集添加至游戏对象 */
		GameObject_AddAction( player->object, action, i );
	}
	
	//Widget_SetBorder( player->object, Border(1,BORDER_STYLE_SOLID, RGB(0,0,0)) );
	return 0;
}

/** 设置游戏角色当前的攻击类型名称 */
void GamePlayer_SetAttackTypeName( GamePlayer *player, const char *attack_type_name )
{
	strncpy( player->attack_type_name, attack_type_name, 64 );
	player->attack_type_name[63] = 0;
}

/** 按百分比扣除移动速度，n 取值范围为 0 ~ 100 */
void GamePlayer_ReduceSpeed( GamePlayer *player, int n )
{
	double speed;
	speed = GameObject_GetXSpeed( player->object );
	speed = speed - (speed * n / 100.0);
	GameObject_SetXSpeed( player->object, speed );
	speed = GameObject_GetYSpeed( player->object );
	speed = speed - (speed * n / 100.0);
	GameObject_SetYSpeed( player->object, speed );
}

static void GamePlayer_SetLeftLiftWalk( GamePlayer *player )
{
	double speed;
	if( player->lock_motion ) {
		return;
	}
	speed = -XSPEED_WALK * player->property.speed / 100;
	GameObject_SetXSpeed( player->object, speed );
	GamePlayer_ChangeState( player, STATE_LIFT_WALK );
}

static void GamePlayer_SetLeftWalk( GamePlayer *player )
{
	double speed;
	if( player->lock_motion ) {
		return;
	}
	speed = -XSPEED_WALK * player->property.speed / 100;
	GameObject_SetXSpeed( player->object, speed );
	GamePlayer_ChangeState( player, STATE_WALK );
}

static void GamePlayer_SetRightWalk( GamePlayer *player )
{
	double speed;
	if( player->lock_motion ) {
		return;
	}
	speed = XSPEED_WALK * player->property.speed / 100;
	GameObject_SetXSpeed( player->object, speed );
	GamePlayer_ChangeState( player, STATE_WALK );
}

static void GamePlayer_SetRightLiftWalk( GamePlayer *player )
{
	double speed;
	if( player->lock_motion ) {
		return;
	}
	speed = XSPEED_WALK * player->property.speed / 100;
	GameObject_SetXSpeed( player->object, speed );
	GamePlayer_ChangeState( player, STATE_LIFT_WALK );
}

static void GamePlayer_SetLeftRun( GamePlayer *player )
{
	double speed;
	if( player->lock_motion ) {
		return;
	}
	speed = -XSPEED_RUN * player->property.speed / 100;
	GameObject_SetXSpeed( player->object, speed );
	GamePlayer_ChangeState( player, STATE_LEFTRUN );
}

static void GamePlayer_SetRightRun( GamePlayer *player )
{
	double speed;
	if( player->lock_motion ) {
		return;
	}
	speed = XSPEED_RUN * player->property.speed / 100;
	GameObject_SetXSpeed( player->object, speed );
	GamePlayer_ChangeState( player, STATE_RIGHTRUN );
}

static void GamePlayer_AtRunEnd( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
}

static void GamePlayer_AtReadyTimeOut( GamePlayer *player )
{
	player->t_action_timeout = -1;
	if( player->state == STATE_READY ) {
		/* 改为站立状态 */
		GamePlayer_ChangeState( player, STATE_STANCE );
	}
}

/** 检测玩家是否处于举起状态 */
LCUI_BOOL GamePlayer_IsInLiftState( GamePlayer *player )
{
	switch( player->state ) {
	case STATE_LIFT_SQUAT:
	case STATE_LIFT_FALL:
	case STATE_LIFT_STANCE:
	case STATE_LIFT_JUMP:
	case STATE_LIFT_RUN:
	case STATE_LIFT_WALK:
		return TRUE;
	default:break;
	}
	return FALSE;
}

void GamePlayer_SetReady( GamePlayer *player )
{
	if( player->lock_action ) {
		return;
	}
	GamePlayer_ChangeState( player, STATE_READY );
	/* 设置响应动作超时信号 */
	GamePlayer_SetActionTimeOut( player, 1000, GamePlayer_AtReadyTimeOut );
}

/** 响应定时器，让玩家逐渐消失 */
static void GamePlayer_GraduallyDisappear( void *arg )
{
	unsigned char alpha;
	GamePlayer *player;

	player = (GamePlayer*)arg;
	alpha = Widget_GetAlpha( player->object );
	if( alpha >= 5 ) {
		alpha -= 5;
		Widget_SetAlpha( player->object, alpha );
	} else {
		Widget_SetAlpha( player->object, 0 );
		Widget_Hide( player->object );
		player->enable = FALSE;
		player->state = STATE_DIED;
		LCUITimer_Free( player->t_death_timer );
	}
}

static void  GamePlayer_AtStandDone( LCUI_Widget *widget )
{
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
	GamePlayer_ResetAttackControl( player );
	GamePlayer_SetReady( player );
}

/** 开始站起 */
void GamePlayer_StartStand( GamePlayer *player )
{
	GamePlayer *other_player;
	/* 如果已经死了，就不站起来了 */
	if( player->state == STATE_DIED
	 || player->state == STATE_LYING_DYING
	 || player->state == STATE_TUMMY_DYING ) {
		 return;
	}
	/* 如果有其他玩家记录 */
	if( player->other ) {
		other_player = player->other;
		/* 根据对方玩家状态， */
		switch( other_player->state ) {
		/* 如果对方正处于举起状态，那么现在就不站起了 */
		case STATE_LIFT_SQUAT: 
			return;
		case STATE_RIDE_JUMP:
			break;
		case STATE_RIDE:
		case STATE_RIDE_ATTACK:
			GamePlayer_UnlockAction( player->other );
			/* 解除对方的记录 */
			player->other->other = NULL;
			player->other = NULL;
			/* 让骑在自己身上的角色站起来 */
			GamePlayer_StartStand( other_player );
			break;
		default:break;
		}
	}
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_SQUAT );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_AtActionDone( player->object, ACTION_SQUAT, GamePlayer_AtStandDone );
}

/** 让玩家去死 */
static void GamePlayer_SetDeath( GamePlayer *player )
{
	GamePlayer *other_player;
	if( player->state == STATE_LYING ) {
		GamePlayer_UnlockAction( player );
		GamePlayer_ChangeState( player, STATE_LYING_DYING );
		GamePlayer_LockAction( player );
	}
	else if( player->state == STATE_TUMMY ) {
		GamePlayer_UnlockAction( player );
		GamePlayer_ChangeState( player, STATE_TUMMY_DYING );
		GamePlayer_LockAction( player );
	} else {
		return;
	}
	if( player->other ) {
		other_player = player->other;
		switch( other_player->state ) {
		case STATE_RIDE:
		case STATE_RIDE_ATTACK:
			GamePlayer_UnlockAction( player->other );
			/* 解除对方的记录 */
			player->other->other = NULL;
			player->other = NULL;
			/* 让骑在自己身上的角色站起来 */
			GamePlayer_StartStand( other_player );
			break;
		case STATE_RIDE_JUMP:
		default:break;
		}
	}
	LCUITimer_Set( 50, GamePlayer_GraduallyDisappear, (void*)player, TRUE );
}

int GamePlayer_SetLying( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_LYING );
	GamePlayer_LockAction( player );
	/* 如果没血了 */
	if( player->property.cur_hp <= 0 ) {
		GamePlayer_SetDeath( player );
		return -1;
	}
	return 0;
}

int GamePlayer_SetTummy( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_TUMMY );
	GamePlayer_LockAction( player );
	if( player->property.cur_hp <= 0 ) {
		GamePlayer_SetDeath( player );
		return -1;
	}
	return 0;
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

/** 在着陆完成时 */
static void GamePlayer_AtLandingDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_ResetAttackControl( player );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_JUMP_DONE );
	GamePlayer_LockAction( player );
	GameObject_SetXSpeed( player->object, 0 );
	GameObject_SetYSpeed( player->object, 0 );
	GameObject_AtActionDone( player->object, ACTION_SQUAT, GamePlayer_AtJumpDone );
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

/** 在歇息状态结束后 */
static void GamePlayer_AtRestTimeOut( GamePlayer *player )
{
	player->n_attack = 0;
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
	GamePlayer_SetReady( player );
}

/** 设置为歇息状态 */
void GamePlayer_SetRest( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_REST );
	GamePlayer_LockAction( player );
	/* 该状态最多维持两秒 */
	GamePlayer_SetActionTimeOut( player, 2000, GamePlayer_AtRestTimeOut );
}

static void GamePlayer_StopLiftRun( GamePlayer *player )
{
	double speed, acc;
	acc = 0 - GameObject_GetXSpeed( player->object )*2.0;
	player->control.run = FALSE;
	GamePlayer_ChangeState( player, STATE_LIFT_STANCE );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_AtXSpeedToZero( player->object, acc, GamePlayer_AtRunEnd );
	speed = GameObject_GetYSpeed( player->object );
	acc = 0 - GameObject_GetYSpeed(player->object)*2.0;
	GameObject_SetYAcc( player->object, acc );
}

/** 停止奔跑 */
void GamePlayer_StopRun( GamePlayer *player )
{
	double speed, acc;
	acc = 0 - GameObject_GetXSpeed( player->object )*3;
	player->control.run = FALSE;
	GamePlayer_SetReady( player );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_AtXSpeedToZero( player->object, acc, GamePlayer_AtRunEnd );
	speed = GameObject_GetYSpeed( player->object );
	acc = 0 - GameObject_GetYSpeed(player->object)*2.0;
	GameObject_SetYAcc( player->object, acc );
}

/** 抓住正处于喘气（歇息）状态下的玩家 */
GamePlayer* GamePlayer_CatchGaspingPlayer( GamePlayer *player )
{
	RangeBox range;
	LCUI_Widget *obj;
	
	/* 前面一块区域 */
	range.x = 11;
	range.x_width = 5;
	range.y = -GLOBAL_Y_WIDTH/2;
	range.y_width = GLOBAL_Y_WIDTH;
	range.z = 0;
	range.z_width = 64;

	obj =  GameObject_GetObjectInRange(	player->object, range,
						TRUE, ACTION_REST );
	if( obj == NULL ) {
		return NULL;
	}
	return GamePlayer_GetPlayerByWidget( obj );
}

static void GamePlayer_SetLeftMotion( GamePlayer *player )
{
	int skill_id;
	double speed;

	if( player->lock_motion ) {
		if( player->state == STATE_JUMP
		 || player->state == STATE_SJUMP
		 || player->state == STATE_SQUAT
		 || player->state == STATE_LIFT_JUMP
		 || player->state == STATE_LIFT_FALL ) {
			if( !player->lock_action ) {
				GamePlayer_SetLeftOriented( player );
			}
		}
		else if( player->state == STATE_CATCH && player->other
		 && player->other->state == STATE_BACK_BE_CATCH ) {
			GamePlayer_SetLeftOriented( player );
			GamePlayer_SetLeftOriented( player->other );
			CommonSkill_SetPositionAtCatch( player, player->other );
		}
		else if( player->state == STATE_BE_LIFT_STANCE ) {
			GamePlayer_SetLeftOriented( player );
		}
		return;
	}
	switch(player->state) {
	case STATE_LIFT_WALK:
	case STATE_LIFT_STANCE:
		if( !player->lock_action ) {
			GamePlayer_SetLeftOriented( player );
		}
		if( player->control.run ) {
			speed = -XSPEED_RUN * player->property.speed / 100;
			GameObject_SetXSpeed( player->object, speed );
			GamePlayer_ChangeState( player, STATE_LIFT_RUN );
		} else {
			 GamePlayer_SetLeftLiftWalk( player );
		}
		break;
	case STATE_READY:
	case STATE_STANCE:
	case STATE_DEFENSE:
	case STATE_WALK:
		if( !player->lock_action ) {
			GamePlayer_SetLeftOriented( player );
		}
		skill_id = SkillLibrary_GetSkill( player );
		if( skill_id > 0 ) {
			GamePlayer_RunSkill( player, skill_id );
		}
		if( player->control.run ) {
			 GamePlayer_SetLeftRun( player );
		} else {
			 GamePlayer_SetLeftWalk( player );
		}
	case STATE_LEFTRUN:
		break;
	case STATE_RIGHTRUN:
		GamePlayer_StopRun( player );
		break;
	case STATE_LIFT_RUN:
		if( GamePlayer_IsLeftOriented(player) ) {
			break;
		}
		GamePlayer_StopLiftRun( player );
	default:break;
	}
}

static void GamePlayer_SetRightMotion( GamePlayer *player )
{
	int skill_id;
	double speed;

	if( player->lock_motion ) {
		if( player->state == STATE_JUMP
		 || player->state == STATE_SJUMP
		 || player->state == STATE_SQUAT
		 || player->state == STATE_LIFT_JUMP
		 || player->state == STATE_LIFT_FALL ) {
			if( !player->lock_action ) {
				GamePlayer_SetRightOriented( player );
			}
		}
		else if( player->state == STATE_CATCH && player->other
		 && player->other->state == STATE_BACK_BE_CATCH ) {
			GamePlayer_SetRightOriented( player );
			GamePlayer_SetRightOriented( player->other );
			CommonSkill_SetPositionAtCatch( player, player->other );
		}
		else if( player->state == STATE_BE_LIFT_STANCE ) {
			GamePlayer_SetRightOriented( player );
		}
		return;
	}
	switch(player->state) {
	case STATE_LIFT_WALK:
	case STATE_LIFT_STANCE:
		if( !player->lock_action ) {
			GamePlayer_SetRightOriented( player );
		}
		if( player->control.run ) {
			speed = XSPEED_RUN * player->property.speed / 100;
			GameObject_SetXSpeed( player->object, speed );
			GamePlayer_ChangeState( player, STATE_LIFT_RUN );
		} else {
			GamePlayer_SetRightLiftWalk( player );
		}
		break;
	case STATE_READY:
	case STATE_STANCE:
	case STATE_DEFENSE:
	case STATE_WALK:
		if( !player->lock_action ) {
			GamePlayer_SetRightOriented( player );
		}
		skill_id = SkillLibrary_GetSkill( player );
		if( skill_id > 0 ) {
			GamePlayer_RunSkill( player, skill_id );
		}
		if( player->control.run ) {
			 GamePlayer_SetRightRun( player );
		} else {
			 GamePlayer_SetRightWalk( player );
		}
	case STATE_RIGHTRUN:
		break;
	case STATE_LEFTRUN:
		GamePlayer_StopRun( player );
		break;
	case STATE_LIFT_RUN:
		if( !GamePlayer_IsLeftOriented(player) ) {
			break;
		}
		GamePlayer_StopLiftRun( player );
	default:break;
	}
}

void GamePlayer_StopXWalk( GamePlayer *player )
{
	if( player->lock_motion ) {
		return;
	}
	switch(player->state) {
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
	case STATE_LIFT_RUN:
	case STATE_LIFT_JUMP:
	case STATE_LIFT_FALL:
	case STATE_THROW:
		return;
	default:
		GameObject_SetXSpeed( player->object, 0 );
		break;
	}
}

void GamePlayer_StopYMotion( GamePlayer *player )
{
	GameObject_SetYSpeed( player->object, 0 );
	GameObject_SetYAcc( player->object, 0 );
}

void GamePlayer_StopXMotion( GamePlayer *player )
{
	GameObject_SetXSpeed( player->object, 0 );
	GameObject_SetXAcc( player->object, 0 );
}

void GamePlayer_SetUpMotion( GamePlayer *player )
{
	double speed;
	int skill_id;

	if( player->lock_motion ) {
		return;
	}
	switch(player->state) {
	case STATE_LIFT_STANCE:
	case STATE_LIFT_WALK:
		GamePlayer_ChangeState( player, STATE_LIFT_WALK );
		break;
	case STATE_READY:
	case STATE_STANCE:
	case STATE_DEFENSE:
		GamePlayer_ChangeState( player, STATE_WALK );
		break;
	case STATE_WALK:
		skill_id = SkillLibrary_GetSkill( player );
		if( skill_id > 0 ) {
			GamePlayer_RunSkill( player, skill_id );
			return;
		}
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
	case STATE_LIFT_RUN:
		break;
	default:return;
	}
	speed = -YSPEED_WALK * player->property.speed / 100;
	GameObject_SetYSpeed( player->object, speed );
}

/** 设置向上移动 */
void GamePlayer_SetDownMotion( GamePlayer *player )
{
	double speed;
	int skill_id;

	if( player->lock_motion ) {
		return;
	}
	switch(player->state) {
	case STATE_LIFT_STANCE:
	case STATE_LIFT_WALK:
		GamePlayer_ChangeState( player, STATE_LIFT_WALK );
		break;
	case STATE_READY:
	case STATE_STANCE:
	case STATE_DEFENSE:
		GamePlayer_ChangeState( player, STATE_WALK );
	case STATE_WALK:
		skill_id = SkillLibrary_GetSkill( player );
		if( skill_id > 0 ) {
			GamePlayer_RunSkill( player, skill_id );
			return;
		}
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
	case STATE_LIFT_RUN:
		break;
	default:return;
	}
	speed = YSPEED_WALK * player->property.speed / 100;
	GameObject_SetYSpeed( player->object, speed );
}

/** 设置游戏角色的控制键 */
int Game_SetGamePlayerControlKey( int player_id, ControlKey *key )
{
	GamePlayer *player;
	player = GamePlayer_GetByID( player_id );
	if( player == NULL ){
		return -1;
	}
	player->ctrlkey = *key;
	return 0;
}

/** 响应游戏角色受到的攻击 */
static void GamePlayer_ResponseAttack( LCUI_Widget *widget )
{
	GamePlayer *player, *atk_player;
	AttackerInfo *p_info;
	LCUI_Queue *attacker_info;

	player = GamePlayer_GetPlayerByWidget( widget );
	if( player == NULL ){
		return;
	}

	attacker_info = GameObject_GetAttackerInfo( widget );
	while(1) {
		p_info = (AttackerInfo*)Queue_Get( attacker_info, 0 );
		if( p_info == NULL ) {
			break;
		}
		atk_player = GamePlayer_GetPlayerByWidget( p_info->attacker );
		/* 记录本次攻击的信息 */
		Game_RecordAttack(	atk_player, atk_player->attack_type_name,
					player, player->state );
		/* 删除攻击者记录 */
		Queue_Delete( attacker_info, 0 );
	}
}

static int Game_InitPlayerStatusArea(void)
{
	player_status_area = Widget_New(NULL);
	Widget_SetBackgroundColor( player_status_area, RGB(240,240,240) );
	Widget_SetBackgroundTransparent( player_status_area, FALSE );
	Widget_SetBorder( player_status_area, Border(1,BORDER_STYLE_SOLID,RGB(150,150,150)));
	Widget_Resize( player_status_area, Size(800,STATUS_BAR_HEIGHT) );
	Widget_SetAlign( player_status_area, ALIGN_BOTTOM_CENTER, Pos(0,0) );
	/* 创建状态栏 */
	player_data[0].statusbar = StatusBar_New();
	player_data[1].statusbar = StatusBar_New();
	player_data[2].statusbar = StatusBar_New();
	player_data[3].statusbar = StatusBar_New();
	Widget_Container_Add( player_status_area, player_data[0].statusbar );
	Widget_Container_Add( player_status_area, player_data[1].statusbar );
	Widget_Container_Add( player_status_area, player_data[2].statusbar );
	Widget_Container_Add( player_status_area, player_data[3].statusbar );
	Widget_SetAlign( player_data[0].statusbar, ALIGN_TOP_LEFT, Pos(5,5) );
	Widget_SetAlign( player_data[1].statusbar, ALIGN_TOP_LEFT, Pos(5+200,5) );
	Widget_SetAlign( player_data[2].statusbar, ALIGN_TOP_LEFT, Pos(5+400,5) );
	Widget_SetAlign( player_data[3].statusbar, ALIGN_TOP_LEFT, Pos(5+600,5) );
	return 0;
}

void GamePlayer_Init( GamePlayer *player )
{
	LCUI_Graph img_shadow;
	
	Graph_Init( &img_shadow );
	player->id = 0;
	player->role_id = 0;
	player->type = 0;
	player->state = 0;
	player->enable = FALSE;
	player->right_direction = TRUE;
	player->human_control = TRUE;
	player->local_control = TRUE;
	player->lock_action = FALSE;
	player->lock_motion = FALSE;
	player->property.cur_hp = 0;
	player->property.max_hp = 0;
	player->property.defense = 0;
	player->property.kick = 0;
	player->property.punch = 0;
	player->property.speed = 100;
	player->property.throw = 0;

	GamePlayer_InitSkillRecord( player );

	player->n_attack = 0;
	player->t_rest_timeout = -1;
	player->t_action_timeout = -1;
	player->t_death_timer = -1;
	player->object = NULL;
	player->statusbar = NULL;
	player->other = NULL;
	player->control.a_attack = FALSE;
	player->control.b_attack = FALSE;
	player->control.run = FALSE;
	player->control.left_motion = FALSE;
	player->control.right_motion = FALSE;
	player->control.up_motion = FALSE;
	player->control.down_motion = FALSE;
	player->control.jump = FALSE;
	player->ai_data.target_update_time = 0;

	ControlKey_Init( &player->ctrlkey );
	/* 创建GameObject部件 */
	player->object = GameObject_New();
	/* 设置响应游戏角色的受攻击信号 */
	GameObject_AtUnderAttack( player->object, GamePlayer_ResponseAttack );
	GameGraphRes_GetGraph( MAIN_RES, "shadow", &img_shadow );
	/* 设置阴影 */
	GameObject_SetShadow( player->object, img_shadow );
}

static void UpdateViewFPS( void *arg )
{
	char str[10];
	LCUI_Widget *label = (LCUI_Widget*)arg;
	sprintf( str, "FPS: %d", LCUIScreen_GetFPS() );
	Label_Text( label, str );
}

/** 响应按键的按下 */
static void GameKeyboardProcKeyDown( int key_code )
{
	GamePlayer *target;

	target = GamePlayer_GetPlayerByControlKey( key_code );
	if( target == NULL ) {
		return;
	}
	if( key_code == target->ctrlkey.left ) {
		if( LCUIKey_IsDoubleHit(target->ctrlkey.left,250) ) {
			target->control.run = TRUE;
		}
	}
	else if( key_code == target->ctrlkey.right ) {
		if( LCUIKey_IsDoubleHit(target->ctrlkey.right,250) ) {
			target->control.run = TRUE;
		}
	}
	if( key_code == target->ctrlkey.a_attack ) {
		target->control.a_attack = TRUE;
	}
	else if( key_code == target->ctrlkey.b_attack ) {
		target->control.b_attack = TRUE;
	}
	else if( key_code == target->ctrlkey.jump ) {
		target->control.jump = TRUE;
	}
}

static void GameKeyboardProc( LCUI_KeyboardEvent *event, void *arg )
{
	if( event->type == LCUI_KEYDOWN ) {
		GameKeyboardProcKeyDown( event->key_code );
	}
}

static void InitSceneText( LCUI_Widget *scene )
{
	LCUI_Widget *text, *fps_text;
	LCUI_TextStyle style;

	text = Widget_New("label");
	fps_text = Widget_New("label");
	Widget_Container_Add( scene, text );
	Widget_Container_Add( scene, fps_text );
	TextStyle_Init( &style );
	TextStyle_FontSize( &style, 18 );
	Label_TextStyle( fps_text, style );
	Widget_SetAlign( text, ALIGN_TOP_CENTER, Pos(0,40) );
	Widget_SetAlign( fps_text, ALIGN_TOP_CENTER, Pos(0,100) );
	Label_TextW( text, L"<size=42px>游戏测试</size>");
	Widget_SetZIndex( fps_text, -5000 );
	Widget_SetZIndex( text, -5000 );
	Widget_Show( fps_text );
	Widget_Show( text );
	LCUITimer_Set( 500, UpdateViewFPS, fps_text, TRUE );
}

/** 同步游戏玩家的按键控制 */
static void GamePlayer_SyncKeyControl( GamePlayer *player )
{
	player->control.left_motion = LCUIKey_IsHit(player->ctrlkey.left);
	player->control.right_motion = LCUIKey_IsHit(player->ctrlkey.right);
	player->control.up_motion = LCUIKey_IsHit(player->ctrlkey.up);
	player->control.down_motion = LCUIKey_IsHit(player->ctrlkey.down);
	player->control.defense = LCUIKey_IsHit(player->ctrlkey.defense);
}

/** 同步游戏玩家的数据 */
static void GamePlayer_SyncData( GamePlayer *player )
{
	int skill_id;
	LCUI_BOOL stop_xmotion=FALSE, stop_ymotion=FALSE;

	if( player->control.left_motion ) {
		GamePlayer_SetLeftMotion( player );
	}
	else if( player->control.right_motion ) {
		GamePlayer_SetRightMotion( player );
	}
	else {
		GamePlayer_StopXWalk( player );
		stop_xmotion = TRUE;
	}
	if( player->control.up_motion ) {
		GamePlayer_SetUpMotion( player );
	}
	else if( player->control.down_motion ) {
		GamePlayer_SetDownMotion( player );
	}
	else {
		switch( player->state ) {
		case STATE_LEFTRUN:
		case STATE_RIGHTRUN:
		case STATE_WALK:
		case STATE_READY:
		case STATE_STANCE:
		case STATE_LIFT_RUN:
		case STATE_LIFT_WALK:
			GamePlayer_StopYMotion( player );
			stop_ymotion = TRUE;
		default:
			break;
		}
	}
	if( stop_xmotion && stop_ymotion ) {
		if( player->state == STATE_WALK ) {
			GamePlayer_ChangeState( player, STATE_STANCE );
		}
		else if( player->state == STATE_LIFT_WALK ) {
			GamePlayer_ChangeState( player, STATE_LIFT_STANCE );
		}
	}
	if( !player->control.a_attack && !player->control.b_attack
	 && !player->control.jump && !player->control.left_motion 
	 && !player->control.right_motion && !player->control.up_motion
	 && !player->control.down_motion && !player->control.defense
	 && player->state != STATE_DEFENSE
	 && player->state != STATE_SOLID_DEFENSE ) {
		return;
	}
	skill_id = SkillLibrary_GetSkill( player );
	if( skill_id > 0 ) {
		GamePlayer_RunSkill( player, skill_id );
		player->control.a_attack = FALSE;
		player->control.b_attack = FALSE;
		return;
	}
	player->control.a_attack = FALSE;
	player->control.b_attack = FALSE;
	if( player->state != STATE_DEFENSE
	 && player->state != STATE_SOLID_DEFENSE ) {
		return;
	}
	if( player->lock_action || player->control.defense ) {
		return;
	}
	GamePlayer_UnlockMotion( player );
	/* 如果处于被举起的状态，则改变状态为STATE_BE_LIFT_STANCE */
	if( player->other && GamePlayer_IsInLiftState(player->other) )  {
		GamePlayer_ChangeState( player, STATE_BE_LIFT_STANCE );
		return;
	}
	GamePlayer_SetReady( player );
}

static void GamePlayer_SetToReady( LCUI_Widget* widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
	GamePlayer_SetReady( player );
}

static void GamePlayer_SetStart( GamePlayer *player )
{
	GamePlayer_ChangeState( player, STATE_START );
	GameObject_AtActionDone( player->object, ACTION_START, GamePlayer_SetToReady );
}

int Game_Loop(void)
{
	int i, n_found;
	/* 初始化游戏AI */
	GameAI_Init();

	/* 循环更新游戏数据 */
	while(1) {
		for(n_found=0,i=0; i<4; ++i) {
			if( !player_data[i].enable
			 || !player_data[i].local_control ) {
				continue;
			}
			++n_found;
			/* 如果该游戏玩家不是由人类控制的 */
			if( !player_data[i].human_control ) {
				GameAI_Control( player_data[i].id );
			} else {
				GamePlayer_SyncKeyControl( &player_data[i] );
			}
			GamePlayer_SyncData( &player_data[i] );
			Widget_Update( player_data[i].object );
			/* 将第一个有效的游戏角色设置为镜头焦点 */
			if( n_found == 1 ) {
				GameScene_SetCameraTarget( player_data[i].object );
			}
		}
		/* 更新镜头 */
		GameScene_UpdateCamera();
		/* 处理攻击 */
		Game_ProcAttack();
		LCUI_MSleep( 10 );
	}
	return 0;
}

int Game_Pause(void)
{
	return 0;
}

static RoleInfo role_library[TOTAL_ROLE_NUM] = {
	{ ROLE_KUNI, L"国夫", PLAYER_TYPE_FIGHTER, {
		1800, 1800, 60, 200, 150, 100, 100}, {
			SKILLNAME_MACH_B_ATTACK,
			SKILLNAME_SPINHIT,
			SKILLNAME_BOMBKICK,
		}, 3
	}, { ROLE_RIKI, L"阿力", PLAYER_TYPE_MARTIAL_ARTIST, {
		1900, 1900, 240, 60, 100, 100, 130}, {
			SKILLNAME_MACH_A_ATTACK,
			SKILLNAME_SPINHIT,
			SKILLNAME_TORNADO_ATTACK,
		}, 3
	}, { ROLE_MIKE, L"姬山", PLAYER_TYPE_KUNG_FU, {
		1800, 1800, 60, 240, 150, 110, 130}, {
			SKILLNAME_MACH_B_ATTACK,
			SKILLNAME_BOMBKICK,
		}, 2
	}, { ROLE_BEN, L"御堂", PLAYER_TYPE_JUDO_MASTER, {
		2200, 2200, 150, 150, 100, 150, 200}, {
			SKILLNAME_SOLID_DEFENSE,
			SKILLNAME_BOMBKICK,
			SKILLNAME_SPINHIT,
		}, 3
	}, { ROLE_TORAJI, L"寅", PLAYER_TYPE_TIGER, {
		2000, 2000, 180, 180, 150, 150, 150}, {
			SKILLNAME_MACH_A_ATTACK,
			SKILLNAME_MACH_B_ATTACK,
			SKILLNAME_MACH_STOMP,
			SKILLNAME_BOMBKICK,
			SKILLNAME_SPINHIT,
			SKILLNAME_SOLID_DEFENSE,
			SKILLNAME_JUMP_SPINKICK,
			SKILLNAME_TORNADO_ATTACK,
			SKILLNAME_BIG_ELBOW,
			SKILLNAME_GUILLOTINE
		}, 10
	}
};

/** 获取角色信息 */
RoleInfo *Game_GetRoleInfo( int role_id )
{
	int i;
	for(i=0; i<TOTAL_ROLE_NUM; ++i) {
		if( role_library[i].role_id == role_id ) {
			return &role_library[i];
		}
	}
	return NULL;
}

/** 设置游戏角色 */
int Game_SetGamePlayer( int id, int role_id, LCUI_BOOL human_control )
{
	int i;
	RoleInfo *p_info;
	GamePlayer *player;

	player = GamePlayer_GetByID( id );
	p_info = Game_GetRoleInfo( role_id );
	if( !player || !p_info ) {
		return -1;
	}

	player->type = p_info->type;
	player->role_id = role_id;
	player->property = p_info->property;
	player->human_control = human_control;

	for(i=0; i<p_info->total_skill && i<MAX_SKILL_NUM; ++i) {
		GamePlayer_EnableSkill( player, p_info->skills[i] );
		if( strcmp(p_info->skills[i], SKILLNAME_MACH_A_ATTACK) == 0 ) {
			GamePlayer_EnableSkill( player, SKILLNAME_JUMP_MACH_A_ATTACK );
		}
		else if( strcmp(p_info->skills[i], SKILLNAME_MACH_B_ATTACK) == 0 ) {
			GamePlayer_EnableSkill( player, SKILLNAME_JUMP_MACH_B_ATTACK );
		}
	}
	/* 初始化角色动作动画 */
	GamePlayer_InitAction( player, role_id );
	/* 根据职业来选择需要启用的特殊技能 */
	switch( player->type ) {
	case PLAYER_TYPE_FIGHTER:
		GamePlayer_EnableSkill( player, SKILLNAME_HUG_FRONT_PUT );
		GamePlayer_EnableSkill( player, SKILLNAME_HUG_BACK_PUT );
		break;
	case PLAYER_TYPE_MARTIAL_ARTIST:
		GamePlayer_EnableSkill( player, SKILLNAME_KNEEHIT );
		GamePlayer_EnableSkill( player, SKILLNAME_ELBOW );
		break;
	case PLAYER_TYPE_KUNG_FU:
		GamePlayer_EnableSkill( player, SKILLNAME_LIFT_JUMP );
		GamePlayer_EnableSkill( player, SKILLNAME_HUG_JUMP );
		break;
	case PLAYER_TYPE_JUDO_MASTER:
		GamePlayer_EnableSkill( player, SKILLNAME_JUDO );
		GamePlayer_EnableSkill( player, SKILLNAME_BACK_JUDO );
		break;
	case PLAYER_TYPE_TIGER:
		GamePlayer_EnableSkill( player, SKILLNAME_SPIN_DRILL );
	default:
		break;
	}
	return 0;
}

/** 启用一个游戏角色 */
void Game_EnableGamePlayer( int id )
{
	GamePlayer *player;
	player = GamePlayer_GetByID( id );
	if( player ) {
		player->enable = TRUE;
	}
}

/** 初始化对战 */
int Game_InitBattle(void)
{
	int ret;

	ret = GameScene_Init();
	if( ret != 0 ) {
		return ret;
	}
	/* 注册所需部件 */
	StatusBar_Register();
	LifeBar_Regiser();
	/* 初始化攻击记录 */
	Game_InitAttackRecord();
	/* 初始化状态与动作的映射表 */
	Game_InitStateActionMap();
	/** 初始化技能库 */
	SkillLibrary_Init();
	/* 初始化角色信息 */
	GamePlayer_Init( &player_data[0] );
	GamePlayer_Init( &player_data[1] );
	GamePlayer_Init( &player_data[2] );
	GamePlayer_Init( &player_data[3] );
	/* 记录玩家ID */
	player_data[0].id = 1;
	player_data[1].id = 2;
	player_data[2].id = 3;
	player_data[3].id = 4;
	/* 初始化角色状态信息区域 */
	ret |= Game_InitPlayerStatusArea();
	/* 将游戏对象放入战斗场景内 */
	GameObject_AddToContainer( player_data[0].object, GetGameScene() );
	GameObject_AddToContainer( player_data[1].object, GetGameScene() );
	GameObject_AddToContainer( player_data[2].object, GetGameScene() );
	GameObject_AddToContainer( player_data[3].object, GetGameScene() );
	/* 响应按键输入 */
	ret |= LCUI_KeyboardEvent_Connect( GameKeyboardProc, NULL );
	ret |= GameMsgLoopStart();
	/* 初始化在场景上显示的文本 */
	InitSceneText( GetGameScene() );
	return ret;
}

/** 开始对战 */
void Game_StartBattle( void )
{
	int i;
	RoleInfo *p_role_info;
	int x, y, start_x, start_y;
	LCUI_Size scene_size;
	wchar_t player_type_name[5];

	GameScene_GetLandSize( &scene_size );
	GameScene_GetLandStartX( &start_x );
	GameScene_GetLandStartY( &start_y );
	/* 计算并设置游戏角色的位置 */
	x = scene_size.w/2 - 150;
	GameObject_SetX( player_data[0].object, start_x+x );
	GameObject_SetX( player_data[1].object, start_x+x-50 );
	x = scene_size.w/2 + 150;
	GameObject_SetX( player_data[2].object, start_x+x );
	GameObject_SetX( player_data[3].object, start_x+x+50 );
	y = scene_size.h/2;
	GameObject_SetY( player_data[0].object, start_y+y-50 );
	GameObject_SetY( player_data[1].object, start_y+y+50 );
	GameObject_SetY( player_data[2].object, start_y+y-50 );
	GameObject_SetY( player_data[3].object, start_y+y+50 );
	/* 改变游戏角色的朝向 */
	GamePlayer_SetRightOriented( &player_data[0] );
	GamePlayer_SetRightOriented( &player_data[1] );
	GamePlayer_SetLeftOriented( &player_data[2] );
	GamePlayer_SetLeftOriented( &player_data[3] );
	/* 设置游戏角色的初始状态 */
	GamePlayer_SetStart( &player_data[0] );
	GamePlayer_SetStart( &player_data[1] );
	GamePlayer_SetStart( &player_data[2] );
	GamePlayer_SetStart( &player_data[3] );
	/* 播放动作动画，并显示游戏角色 */
	for(i=0; i<4; ++i) {
		if( !player_data[i].enable ) {
			continue;
		}
		/* 播放动作动画 */
		GameObject_PlayAction( player_data[i].object );
		if( player_data[i].human_control ) {
			player_type_name[0] = L'1'+i;
			player_type_name[1] = L'P';
			player_type_name[2] = 0;
		} else {
			player_type_name[0] = L'C';
			player_type_name[1] = L'P';
			player_type_name[2] = L'U';
			player_type_name[3] = 0;
		}
		p_role_info = Game_GetRoleInfo( player_data[i].role_id );
		StatusBar_SetPlayerNameW( player_data[i].statusbar, p_role_info->name );
		/* 设置角色类型名 */
		StatusBar_SetPlayerTypeNameW( player_data[i].statusbar, player_type_name );
		/* 设置血量 */
		StatusBar_SetHealth( player_data[i].statusbar, player_data[i].property.cur_hp );
		StatusBar_SetMaxHealth( player_data[i].statusbar, player_data[i].property.max_hp );
		/* 显示游戏角色 */
		Widget_Show( player_data[i].object );
		/* 显示状态栏 */
		Widget_Show( player_data[i].statusbar );
	}
	Widget_Show( player_status_area );
	/* 显示场景 */
	Widget_Show( GetGameScene() );
}
