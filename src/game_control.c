#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_INPUT_H
#include LC_TEXTSTYLE_H
#include LC_DISPLAY_H
#include LC_LABEL_H

#include "game.h"
#include "skills/game_skill.h"

static LCUI_Graph img_shadow;
static LCUI_Widget *game_scene;
static LCUI_Widget *player_status_area;

static GamePlayer player_data[4];
static int global_action_list[]={
	ACTION_READY,
	ACTION_STANCE,
	ACTION_WALK,
	ACTION_RUN,
	ACTION_DEFENSE,
	ACTION_A_ATTACK,
	ACTION_B_ATTACK,
	ACTION_MACH_A_ATTACK,
	ACTION_MACH_B_ATTACK,
	ACTION_JUMP_MACH_A_ATTACK,
	ACTION_JUMP_MACH_B_ATTACK,
	ACTION_AS_ATTACK,
	ACTION_BS_ATTACK,
	ACTION_AJ_ATTACK,
	ACTION_BJ_ATTACK,
	ACTION_ASJ_ATTACK,
	ACTION_BSJ_ATTACK,
	ACTION_FINAL_BLOW,
	ACTION_HIT,
	ACTION_HIT_FLY,
	ACTION_HIT_FLY_FALL,
	ACTION_LYING,
	ACTION_LYING_HIT,
	ACTION_TUMMY,
	ACTION_TUMMY_HIT,
	ACTION_REST,
	ACTION_SQUAT,
	ACTION_JUMP,
	ACTION_F_ROLL,
	ACTION_B_ROLL,
	ACTION_ELBOW,
	ACTION_JUMP_ELBOW,
	ACTION_JUMP_STOMP,
	ACTION_FALL,
	ACTION_KICK,
	ACTION_SPINHIT,
	ACTION_BOMBKICK,
	ACTION_MACH_STOMP,
	ACTION_CATCH,
	ACTION_BE_CATCH,
	ACTION_HALF_LYING,
	ACTION_BACK_BE_CATCH,
	ACTION_CATCH_SKILL_FA,
	ACTION_CATCH_SKILL_BA,
	ACTION_CATCH_SKILL_BB,
	ACTION_CATCH_SKILL_FB,
	ACTION_WEAK_RUN,
	ACTION_LIFT_STANCE,
	ACTION_LIFT_WALK,
	ACTION_LIFT_RUN,
	ACTION_LIFT_JUMP,
	ACTION_LIFT_FALL,
	ACTION_THROW,
	ACTION_RIDE,
	ACTION_RIDE_ATTACK,
	ACTION_LYING_DYING,
	ACTION_TUMMY_DYING,
};

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
	if( player_id > 4 ) {
		return NULL;
	}
	return &player_data[player_id-1];
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

void GamePlayer_ChangeState( GamePlayer *player, int state )
{
	int action_type;
	if( player->lock_action ) {
		return;
	}
	if( player->state == STATE_LIFT_WALK ) {
		if( state == STATE_STANCE ) {
			abort();
		}
	}
	switch(state) {
	case STATE_LYING_DYING:
		action_type = ACTION_LYING_DYING;
		break;
	case STATE_TUMMY_DYING:
		action_type = ACTION_TUMMY_DYING;
		break;
	case STATE_READY:
		action_type = ACTION_READY;
		break;
	case STATE_STANCE: 
	case STATE_BE_LIFT_STANCE:
		action_type = ACTION_STANCE; 
		break;
	case STATE_WALK:
		action_type = ACTION_WALK;
		break;
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
		action_type = ACTION_RUN;
		break;
	case STATE_DEFENSE:
		action_type = ACTION_DEFENSE;
		break;
	case STATE_A_ATTACK:
		action_type = ACTION_A_ATTACK;
		break;
	case STATE_B_ATTACK:
		action_type = ACTION_B_ATTACK;
		break;
	case STATE_MAJ_ATTACK:
		action_type = ACTION_JUMP_MACH_A_ATTACK;
		break;
	case STATE_MACH_A_ATTACK:
		action_type = ACTION_MACH_A_ATTACK;
		break;
	case STATE_MBJ_ATTACK:
		action_type = ACTION_JUMP_MACH_B_ATTACK;
		break;
	case STATE_MACH_B_ATTACK:
		action_type = ACTION_MACH_B_ATTACK;
		break;
	case STATE_AS_ATTACK:
		action_type = ACTION_AS_ATTACK;
		break;
	case STATE_BS_ATTACK:
		action_type = ACTION_BS_ATTACK;
		break;
	case STATE_ASJ_ATTACK:
		action_type = ACTION_ASJ_ATTACK;
		break;
	case STATE_BSJ_ATTACK:
		action_type = ACTION_BSJ_ATTACK;
		break;
	case STATE_AJ_ATTACK:
		action_type = ACTION_AJ_ATTACK;
		break;
	case STATE_BJ_ATTACK:
		action_type = ACTION_BJ_ATTACK;
		break;
	case STATE_FINAL_BLOW:
		action_type = ACTION_FINAL_BLOW;
		break;
	case STATE_JUMP_DONE:
	case STATE_BE_LIFT_SQUAT:
	case STATE_LIFT_SQUAT:
	case STATE_JSQUAT:
	case STATE_SSQUAT:
	case STATE_SQUAT:
		action_type = ACTION_SQUAT;
		break;
	case STATE_JUMP:
	case STATE_SJUMP:
		action_type = ACTION_JUMP;
		break;
	case STATE_FALL:
		action_type = ACTION_FALL;
		break;
	case STATE_HIT:
		action_type = ACTION_HIT;
		break;
	case STATE_HIT_FLY:
		action_type = ACTION_HIT_FLY;
		break;
	case STATE_HIT_FLY_FALL:
		action_type = ACTION_HIT_FLY_FALL;
		break;
	case STATE_LYING:
	case STATE_BE_LIFT_LYING:
		action_type = ACTION_LYING;
		break;
	case STATE_LYING_HIT:
	case STATE_BE_LIFT_LYING_HIT:
		action_type = ACTION_LYING_HIT;
		break;
	case STATE_TUMMY:
	case STATE_BE_LIFT_TUMMY:
		action_type = ACTION_TUMMY;
		break;
	case STATE_TUMMY_HIT:
	case STATE_BE_LIFT_TUMMY_HIT:
		action_type = ACTION_TUMMY_HIT;
		break;
	case STATE_REST:
		action_type = ACTION_REST;
		break;
	case STATE_F_ROLL:
		action_type = ACTION_F_ROLL;
		break;
	case STATE_B_ROLL:
		action_type = ACTION_B_ROLL;
		break;
	case STATE_ELBOW:
		action_type = ACTION_ELBOW;
		break;
	case STATE_JUMP_ELBOW:
		action_type = ACTION_JUMP_ELBOW;
		break;
	case STATE_RIDE_JUMP:
	case STATE_JUMP_STOMP:
		action_type = ACTION_JUMP_STOMP;
		break;
	case STATE_KICK:
		action_type = ACTION_KICK;
		break;
	case STATE_SPINHIT:
		action_type = ACTION_SPINHIT;
		break;
	case STATE_BOMBKICK:
		action_type = ACTION_BOMBKICK;
		break;
	case STATE_MACH_STOMP:
		action_type = ACTION_MACH_STOMP;
		break;
	case STATE_CATCH:
		action_type = ACTION_CATCH;
		break;
	case STATE_BE_CATCH:
		action_type = ACTION_BE_CATCH;
		break;
	case STATE_HALF_LYING:
		action_type = ACTION_HALF_LYING;
		break;
	case STATE_BACK_BE_CATCH:
		action_type = ACTION_BACK_BE_CATCH;
		break;
	case STATE_CATCH_SKILL_FA:
		action_type = ACTION_CATCH_SKILL_FA;
		break;
	case STATE_CATCH_SKILL_BA:
		action_type = ACTION_CATCH_SKILL_BA;
		break;
	case STATE_CATCH_SKILL_BB:
		action_type = ACTION_CATCH_SKILL_BB;
		break;
	case STATE_CATCH_SKILL_FB:
		action_type = ACTION_CATCH_SKILL_FB;
		break;
	case STATE_WEAK_RUN:
	case STATE_WEAK_RUN_ATTACK:
		action_type = ACTION_WEAK_RUN;
		break;
	case STATE_LIFT_STANCE:
		action_type = ACTION_LIFT_STANCE;
		break;
	case STATE_LIFT_WALK:
		action_type = ACTION_LIFT_WALK;
		break;
	case STATE_LIFT_RUN:
		action_type = ACTION_LIFT_RUN;
		break;
	case STATE_LIFT_JUMP:
		action_type = ACTION_LIFT_JUMP;
		break;
	case STATE_LIFT_FALL:
		action_type = ACTION_LIFT_FALL;
		break;
	case STATE_THROW:
		action_type = ACTION_THROW;
		break;
	case STATE_RIDE:
		action_type = ACTION_RIDE;
		break;
	case STATE_RIDE_ATTACK:
		action_type = ACTION_RIDE_ATTACK;
		break;
	default:return;
	}
	player->state = state;
	/* 在切换动作时，撤销动作超时的响应 */
	GamePlayer_BreakActionTiming( player );
	GamePlayer_ChangeAction( player, action_type );
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
	/* 创建GameObject部件 */
	player->object = GameObject_New();

	for(i=0; i<sizeof(global_action_list)/sizeof(int); ++i) {
		/* 载入游戏角色资源 */
		action = ActionRes_Load( id, global_action_list[i] );
		/* 将动作集添加至游戏对象 */
		GameObject_AddAction( player->object, action, global_action_list[i] );
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
	/* 如果处于举起状态，则不能切换为READY状态 */
	if( GamePlayer_IsInLiftState(player) ) {
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
	/* 如果自己正被对方举起，那么现在就不站起了 */
	if( player->other ) {
		other_player = player->other;
		switch( other_player->state ) {
		case STATE_SQUAT:
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

static void GamePlayer_SetFall( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
	GamePlayer_ChangeState( player, STATE_JUMP );
	GamePlayer_BreakRest( player );
	GameObject_AtLanding( player->object, 0, -ZACC_JUMP, GamePlayer_AtJumpDone );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
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
	double x, y, speed;

	if( player->lock_motion ) {
		if( player->state == STATE_JUMP
		 || player->state == STATE_SJUMP
		 || player->state == STATE_SQUAT
		 || player->state == STATE_LIFT_JUMP
		 || player->state == STATE_LIFT_FALL ) {
			GamePlayer_SetLeftOriented( player );
		}
		else if( player->state == STATE_CATCH && player->other
		 && player->other->state == STATE_BACK_BE_CATCH ) {
			GamePlayer_SetLeftOriented( player );
			GamePlayer_SetLeftOriented( player->other );
			x = GameObject_GetX( player->object );
			y = GameObject_GetY( player->object );
			GameObject_SetX( player->other->object, x-30 );
			GameObject_SetY( player->other->object, y );
		}
		else if( player->state == STATE_BE_LIFT_STANCE ) {
			GamePlayer_SetLeftOriented( player );
		}
		return;
	}
	switch(player->state) {
	case STATE_LIFT_WALK:
	case STATE_LIFT_STANCE:
		GamePlayer_SetLeftOriented( player );
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
		GamePlayer_SetLeftOriented( player );
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
	double x, y, speed;

	if( player->lock_motion ) {
		if( player->state == STATE_JUMP
		 || player->state == STATE_SJUMP
		 || player->state == STATE_SQUAT
		 || player->state == STATE_LIFT_JUMP
		 || player->state == STATE_LIFT_FALL ) {
			GamePlayer_SetRightOriented( player );
		}
		else if( player->state == STATE_CATCH && player->other
		 && player->other->state == STATE_BACK_BE_CATCH ) {
			GamePlayer_SetRightOriented( player );
			GamePlayer_SetRightOriented( player->other );
			x = GameObject_GetX( player->object );
			y = GameObject_GetY( player->object );
			GameObject_SetX( player->other->object, x+30 );
			GameObject_SetY( player->other->object, y );
		}
		else if( player->state == STATE_BE_LIFT_STANCE ) {
			GamePlayer_SetRightOriented( player );
		}
		return;
	}
	switch(player->state) {
	case STATE_LIFT_WALK:
	case STATE_LIFT_STANCE:
		GamePlayer_SetRightOriented( player );
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
		GamePlayer_SetRightOriented( player );
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

/** 获取当前角色附近躺地的角色 */
GamePlayer* GamePlayer_GetGroundPlayer( GamePlayer *player )
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

/** 检测当前角色是否能够举起另一个角色 */
LCUI_BOOL GamePlayer_CanLiftPlayer( GamePlayer *player, GamePlayer *other_player )
{
	double x1, x2;
	x1 = GameObject_GetX( player->object );
	x2 = GameObject_GetX( other_player->object );
	/* 如果在中间位置附近 */
	if( x1 > x2-15 && x1 < x2+15 ) {
		return TRUE;
	}
	return FALSE;
}

/** 检测玩家是否能够攻击躺在地上的玩家 */
LCUI_BOOL GamePlayer_CanAttackGroundPlayer( GamePlayer *player, GamePlayer *other_player )
{
	double x1, x2;
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
int GamePlayer_SetControlKey( int player_id, ControlKey *key )
{
	GamePlayer *player;
	player = GamePlayer_GetByID( player_id );
	if( player == NULL ){
		return -1;
	}
	player->ctrlkey = *key;
	return 0;
}

/** 设置游戏角色的角色ID */
int GamePlayer_SetRole( int player_id, int role_id )
{
	GamePlayer *player;
	player = GamePlayer_GetByID( player_id );
	if( player == NULL ){
		return -1;
	}
	player->role_id = role_id;
	/* 初始化角色动作动画 */
	GamePlayer_InitAction( player, role_id );
	GameObject_SetShadow( player->object, img_shadow );
	/* 根据职业来选择需要启用的特殊技能 */
	switch( player->type ) {
	case PLAYER_TYPE_MARTIAL_ARTIST:
		GamePlayer_EnableSkill( player, SKILLNAME_KNEEHIT );
		GamePlayer_EnableSkill( player, SKILLNAME_ELBOW );
		break;
	case PLAYER_TYPE_TIGER:
		GamePlayer_EnableSkill( player, SKILLNAME_SPIN_DRILL );
	default:
		break;
	}

	return 0;
}

/** 设置游戏角色是否由人类控制 */
int GamePlayer_ControlByHuman( int player_id, LCUI_BOOL flag )
{
	GamePlayer *player;
	player = GamePlayer_GetByID( player_id );
	if( player == NULL ){
		return -1;
	}
	player->human_control = flag;
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
	int ret;
	ret = GameGraphRes_LoadFromFile( "font.data" );
	if( ret != 0 ) {
		LCUI_MessageBoxW(
			MB_ICON_ERROR,
			L"字体资源载入出错，请检查程序的完整性！",
			L"错误", MB_BTN_OK );
		return -1;
	}
	player_status_area = Widget_New(NULL);
	Widget_SetBackgroundColor( player_status_area, RGB(240,240,240) );
	Widget_SetBackgroundTransparent( player_status_area, FALSE );
	Widget_SetBorder( player_status_area, Border(1,BORDER_STYLE_SOLID,RGB(150,150,150)));
	Widget_Resize( player_status_area, Size(800,STATUS_BAR_HEIGHT) );
	Widget_SetAlign( player_status_area, ALIGN_BOTTOM_CENTER, Pos(0,0) );
	/* 创建状态栏 */
	player_data[0].statusbar = StatusBar_New();
	player_data[1].statusbar = StatusBar_New();
	Widget_Container_Add( player_status_area, player_data[0].statusbar );
	Widget_Container_Add( player_status_area, player_data[1].statusbar );
	Widget_SetAlign( player_data[0].statusbar, ALIGN_TOP_LEFT, Pos(5,5) );
	Widget_SetAlign( player_data[1].statusbar, ALIGN_TOP_LEFT, Pos(5+200,5) );
	return 0;
}

void GamePlayer_Init( GamePlayer *player )
{
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
	ControlKey_Init( &player->ctrlkey );
}

static void UpdateViewFPS( void *arg )
{
	char str[10];
	LCUI_Widget *label = (LCUI_Widget*)arg;
	sprintf( str, "FPS: %d", LCUIScreen_GetFPS() );
	Label_Text( label, str );
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

int Game_Init(void)
{
	int ret;
	ControlKey ctrlkey;

	game_scene = Widget_New(NULL);
	ret = GameScene_Init( game_scene );
	if( ret != 0 ) {
		return ret;
	}
	ret = GameGraphRes_LoadFromFile("action-riki.data");
	ret |= GameGraphRes_LoadFromFile("action-toraji.data");
	if( ret != 0 ) {
		LCUI_MessageBoxW(
			MB_ICON_ERROR,
			L"角色资源载入出错，请检查程序的完整性！",
			L"错误", MB_BTN_OK );
		return ret;
	}
	/* 注册所需部件 */
	GameObject_Register();
	StatusBar_Register();
	LifeBar_Regiser();
	/* 初始化攻击记录 */
	Game_InitAttackRecord();
	/** 初始化技能库 */
	SkillLibrary_Init();
	/* 初始化角色信息 */
	GamePlayer_Init( &player_data[0] );
	GamePlayer_Init( &player_data[1] );
	/* 记录玩家ID */
	player_data[0].id = 1;
	player_data[1].id = 2;
	player_data[0].enable = TRUE;
	player_data[1].enable = TRUE;
	player_data[0].type = PLAYER_TYPE_TIGER;
	player_data[1].type = PLAYER_TYPE_TIGER;
	
	player_data[0].property.max_hp = 2000;
	player_data[0].property.cur_hp = 2000;
	player_data[0].property.defense = 100;
	player_data[0].property.kick = 100;
	player_data[0].property.punch = 100;
	player_data[0].property.throw = 100;
	player_data[0].property.speed = 100;
	
	player_data[1].property.max_hp = 2000;
	player_data[1].property.cur_hp = 2000;
	player_data[1].property.defense = 300;
	player_data[1].property.kick = 300;
	player_data[1].property.punch = 300;
	player_data[1].property.throw = 300;
	player_data[1].property.speed = 200;

	Graph_Init( &img_shadow );
	GameGraphRes_GetGraph( MAIN_RES, "shadow", &img_shadow );

	/* 记录1号角色的控制键 */
	ctrlkey.up = LCUIKEY_W;
	ctrlkey.down = LCUIKEY_S;
	ctrlkey.left = LCUIKEY_A;
	ctrlkey.right = LCUIKEY_D;
	ctrlkey.jump = LCUIKEY_SPACE;
	ctrlkey.a_attack = LCUIKEY_J;
	ctrlkey.b_attack = LCUIKEY_K;
	ctrlkey.defense = LCUIKEY_L;
	/* 设置1号玩家的控制键 */
	GamePlayer_SetControlKey( 1, &ctrlkey );
	/* 设置1号玩家的角色 */
	GamePlayer_SetRole( 1, ROLE_TORAJI );
	/* 设置1号玩家由人来控制 */
	GamePlayer_ControlByHuman( 1, TRUE );

	/* 记录2号角色的控制键 */
	ctrlkey.up = LCUIKEY_UP;
	ctrlkey.down = LCUIKEY_DOWN;
	ctrlkey.left = LCUIKEY_LEFT;
	ctrlkey.right = LCUIKEY_RIGHT;
	ctrlkey.defense = 0;
	ctrlkey.jump = 0;
	ctrlkey.a_attack = 0;
	ctrlkey.b_attack = 0;
	/* 设置2号玩家的控制键 */
	GamePlayer_SetControlKey( 2, &ctrlkey );
	/* 设置2号玩家的角色 */
	GamePlayer_SetRole( 2, ROLE_TORAJI );
	/* 设置2号玩家由人来控制 */
	GamePlayer_ControlByHuman( 2, TRUE );
	/* 设置响应游戏角色的受攻击信号 */
	GameObject_AtUnderAttack( player_data[0].object, GamePlayer_ResponseAttack );
	GameObject_AtUnderAttack( player_data[1].object, GamePlayer_ResponseAttack );
	/* 将游戏对象放入战斗场景内 */
	GameObject_AddToContainer( player_data[0].object, game_scene );
	GameObject_AddToContainer( player_data[1].object, game_scene );
	/* 响应按键输入 */
	ret |= LCUI_KeyboardEvent_Connect( GameKeyboardProc, NULL );
	ret |= GameMsgLoopStart();
	/* 初始化在场景上显示的文本 */
	InitSceneText( game_scene );
	/* 显示场景 */
	Widget_Show( game_scene );
	/* 初始化角色状态信息区域 */
	ret |= Game_InitPlayerStatusArea();
	return ret;
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
	 && player->state != STATE_DEFENSE ) {
		return;
	}
	skill_id = SkillLibrary_GetSkill( player );
	if( skill_id > 0 ) {
		GamePlayer_RunSkill( player, skill_id );
		player->control.a_attack = FALSE;
		player->control.b_attack = FALSE;
		return;
	}
	if( player->state != STATE_DEFENSE ) {
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

int Game_Start(void)
{
	int i;
	int x, y, start_x, start_y;
	LCUI_Size scene_size;

	GameScene_GetLandSize( game_scene, &scene_size );
	GameScene_GetLandStartX( game_scene, &start_x );
	GameScene_GetLandStartY( game_scene, &start_y );
	/* 计算并设置游戏角色的位置 */
	x = scene_size.w/2 - 150;
	GameObject_SetX( player_data[0].object, start_x+x );
	x = scene_size.w/2 + 150;
	GameObject_SetX( player_data[1].object, start_x+x );
	y = scene_size.h/2;
	GameObject_SetY( player_data[0].object, start_y+y );
	GameObject_SetY( player_data[1].object, start_y+y );

	/* 改变游戏角色的朝向 */
	GamePlayer_SetRightOriented( &player_data[0] );
	GamePlayer_SetLeftOriented( &player_data[1] );
	/* 设置游戏角色的初始状态 */
	GamePlayer_SetReady( &player_data[0] );
	GamePlayer_SetReady( &player_data[1] );
	/* 播放动作动画，并显示游戏角色 */
	for(i=0; i<4; ++i) {
		if( !player_data[i].enable ) {
			continue;
		}
		GameObject_PlayAction( player_data[i].object );
		Widget_Show( player_data[i].object );
	}
	/* 设置状态栏里的信息 */
	StatusBar_SetPlayerNameW( player_data[0].statusbar, GetPlayerName() );
	StatusBar_SetPlayerNameW( player_data[1].statusbar, GetPlayerName() );
	StatusBar_SetPlayerTypeNameW( player_data[0].statusbar, L"1P" );
	StatusBar_SetPlayerTypeNameW( player_data[1].statusbar, L"CPU" );
	StatusBar_SetHealth( player_data[0].statusbar, player_data[0].property.cur_hp );
	StatusBar_SetHealth( player_data[1].statusbar, player_data[1].property.cur_hp );
	StatusBar_SetMaxHealth( player_data[0].statusbar, player_data[0].property.max_hp );
	StatusBar_SetMaxHealth( player_data[1].statusbar, player_data[1].property.max_hp );
	/* 显示状态栏 */
	Widget_Show( player_data[0].statusbar );
	Widget_Show( player_data[1].statusbar );
	Widget_Show( player_status_area );
	return 0;
}

int Game_Loop(void)
{
	int i;
	/* 初始化游戏AI */
	GameAI_Init();
	/* 循环更新游戏数据 */
	while(1) {
		for(i=0; i<4; ++i) {
			if( !player_data[i].enable
			 || !player_data[i].local_control ) {
				continue;
			}
			/* 如果该游戏玩家不是由人类控制的 */
			if( !player_data[i].human_control ) {
				GameAI_Control( player_data[i].id );
			} else {
				GamePlayer_SyncKeyControl( &player_data[i] );
			}
			GamePlayer_SyncData( &player_data[i] );
			Widget_Update( player_data[i].object );
		}
		/* 更新镜头 */
		GameScene_UpdateCamera( game_scene, player_data[0].object );
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
