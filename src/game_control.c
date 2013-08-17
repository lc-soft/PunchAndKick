#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_INPUT_H

#include "game.h"

static LCUI_Graph img_shadow;
static LCUI_Widget *game_scene;
static LCUI_Widget *player_status_area;

static GamePlayer player_data[4];
static int global_action_list[]={
	ACTION_READY,
	ACTION_STANCE,
	ACTION_WALK,
	ACTION_RUN,
	ACTION_A_ATTACK,
	ACTION_B_ATTACK,
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
	ACTION_KICK,
	ACTION_SPINHIT,
	ACTION_BOMBKICK,
	ACTION_MACH_STOMP,
	ACTION_CATCH,
	ACTION_BE_CATCH,
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

#define LIFT_HEIGHT	56

#define SHORT_REST_TIMEOUT	2000
#define LONG_REST_TIMEOUT	3000
#define BE_THROW_REST_TIMEOUT	500
#define BE_LIFT_REST_TIMEOUT	4500

#define XSPEED_RUN	60
#define XSPEED_WALK	15
#define YSPEED_WALK	8
#define XACC_STOPRUN	20
#define XACC_DASH	10
#define ZSPEED_JUMP	60
#define ZACC_JUMP	22

#define XSPEED_S_HIT_FLY	55
#define ZACC_S_HIT_FLY		7
#define ZSPEED_S_HIT_FLY	15

#define XACC_ROLL	15

#define XSPEED_WEAK_WALK 20

#define XSPEED_X_HIT_FLY	70
#define ZACC_XB_HIT_FLY		15
#define ZSPEED_XB_HIT_FLY	25
#define ZACC_XF_HIT_FLY		50
#define ZSPEED_XF_HIT_FLY	100

#define XSPEED_X_HIT_FLY2	20
#define ZACC_XF_HIT_FLY2	10
#define ZSPEED_XF_HIT_FLY2	15

#define XSPEED_HIT_FLY	20
#define ZSPEED_HIT_FLY	100
#define ZACC_HIT_FLY	50

#define ROLL_TIMEOUT	200

#define XSPEED_THROWUP_FLY	60
#define XSPEED_THROWDOWN_FLY	100
#define ZSPEED_THROWUP_FLY	60
#define ZSPEED_THROWDOWN_FLY	40
#define ZACC_THROWUP_FLY	30
#define ZACC_THROWDOWN_FLY	40

static GamePlayer *GamePlayer_GetPlayerByWidget( LCUI_Widget *widget )
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
	return &player_data[0];
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
	int current_action;

	current_action = GameObject_GetCurrentActionID( player->object );
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
	case STATE_A_ATTACK:
		action_type = ACTION_A_ATTACK;
		break;
	case STATE_B_ATTACK:
		action_type = ACTION_B_ATTACK;
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
	case STATE_BE_LIFT_SQUAT:
	case STATE_JSQUAT:
	case STATE_SSQUAT:
	case STATE_SQUAT:
		action_type = ACTION_SQUAT;
		break;
	case STATE_JUMP:
	case STATE_SJUMP:
		action_type = ACTION_JUMP;
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

/** 按百分比变更移动速度，n 取值范围为 0 ~ 100 */
static void GamePlayer_ReduceSpeed( GamePlayer *player, int n )
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

void GamePlayer_SetReady( GamePlayer *player )
{
	/* 如果处于站立举起状态，则不能切换为READY状态 */
	if( player->state == STATE_LIFT_STANCE ) {
		return;
	}
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
		LCUITimer_Free( player->t_death_timer );
	}
}

/** 让玩家去死 */
static void GamePlayer_SetDeath( GamePlayer *player )
{
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
	LCUITimer_Set( 50, GamePlayer_GraduallyDisappear, (void*)player, TRUE );
}

static int GamePlayer_SetLying( GamePlayer *player )
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

static int GamePlayer_SetTummy( GamePlayer *player )
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

/** 爆裂腿 */
static void GamePlayer_SetBombKick( GamePlayer *player );

/** 自旋击（翻转击） */
static void GamePlayer_SetSpinHit( GamePlayer *player );

/** 在跳跃结束时 */
static void GamePlayer_AtJumpDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
	if( LCUIKey_IsHit(player->ctrlkey.a_attack) ) {
		GamePlayer_SetSpinHit( player );
	}
	else if( LCUIKey_IsHit(player->ctrlkey.b_attack) ) {
		GamePlayer_SetBombKick( player );
	}
	else {
		GamePlayer_SetReady( player );
	}
}

/** 在着陆完成时 */
static void GamePlayer_AtLandingDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_SQUAT );
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

/** 在被举起的状态下，更新自身的位置 */
static void GamePlayer_UpdateLiftPosition( LCUI_Widget *widget )
{
	double x, y, z;
	GamePlayer *player, *other_player;
	LCUI_Widget *other;

	player = GamePlayer_GetPlayerByWidget( widget );
	/* 如果没有举起者，或自己不是被举起者 */
	if( !player->other ) {
		GameObject_AtMove( widget, NULL );
		return;
	}
	else if( !player->other->other ) {
		GameObject_AtMove( widget, NULL );
		return;
	}
	other_player = player->other;
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
static void GamePlayer_AtSquatDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_ChangeState( player, STATE_JUMP );
	/* 在跳跃过程中检测是否有碰撞 */
	GameObject_AtTouch( widget, GamePlayer_PorcJumpTouch );
}

static void GamePlayer_SetSquat( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	/* 跳跃后，重置X轴上的加速度为0 */
	GameObject_SetXAcc( player->object, 0 );
	GamePlayer_ChangeState( player, STATE_JSQUAT );
	GamePlayer_LockMotion( player );
	GameObject_AtActionDone( player->object, ACTION_SQUAT, GamePlayer_AtSquatDone );
	GameObject_AtLanding( player->object, ZSPEED_JUMP, -ZACC_JUMP, GamePlayer_AtLandingDone );
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
		GamePlayer_UnlockMotion( player );
		GamePlayer_ChangeState( player, STATE_LIFT_STANCE );
	}
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

/** 举着，起跳 */
static void GamePlayer_SetLiftJump( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_LockMotion( player );
	GamePlayer_ChangeState( player, STATE_LIFT_JUMP );
	GameObject_AtZeroZSpeed( player->object, GamePlayer_SetLiftFall );
	GameObject_AtLanding( player->object, ZSPEED_JUMP, -ZACC_JUMP, GamePlayer_AtLiftLanding );
}

/** 在歇息状态结束后 */
static void GamePlayer_AtRestTimeOut( GamePlayer *player )
{
	player->n_attack = 0;
	GamePlayer_UnlockAction( player );
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
		if( player->n_attack >= 3 ) {
			GamePlayer_SetRest( player );
		} else {
			GamePlayer_UnlockAction( player );
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


int GamePlayer_TryHit( GamePlayer *player )
{
	switch( player->state ) {
	case STATE_LYING:
	case STATE_LYING_HIT:
		player->n_attack = 0;
		GamePlayer_UnlockAction( player );
		GamePlayer_ChangeState( player, STATE_LYING_HIT );
		GamePlayer_LockAction( player );
		GameObject_AtActionDone( player->object, ACTION_LYING_HIT, GamePlayer_AtHitDone );
		break;
	case STATE_TUMMY:
	case STATE_TUMMY_HIT:
		player->n_attack = 0;
		GamePlayer_UnlockAction( player );
		GamePlayer_ChangeState( player, STATE_TUMMY_HIT );
		GamePlayer_LockAction( player );
		GameObject_AtActionDone( player->object, ACTION_TUMMY_HIT, GamePlayer_AtHitDone );
		break;
	case STATE_B_ROLL:
	case STATE_F_ROLL:
	case STATE_HIT_FLY:
	case STATE_HIT_FLY_FALL:
		player->n_attack = 0;
		break;
	default:
		return -1;
	}
	return 0;
}

void GamePlayer_SetHit( GamePlayer *player )
{
	if( GamePlayer_TryHit(player) == 0 ) {
		return;
	}
	GamePlayer_UnlockAction( player );
	GamePlayer_StopYMotion( player );
	GamePlayer_StopXMotion( player );
	GamePlayer_ChangeState( player, STATE_HIT );
	GamePlayer_LockAction( player );
	GamePlayer_SetRestTimeOut( player, 2000, GamePlayer_ResetCountAttack );
	GameObject_AtActionDone( player->object, ACTION_HIT, GamePlayer_AtHitDone );
}

static void  GamePlayer_AtStandDone( LCUI_Widget *widget )
{
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_SetReady( player );
}

/** 开始站起 */
static void GamePlayer_StartStand( GamePlayer *player )
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
	GamePlayer_UnlockMotion( player );
	GameObject_AtActionDone( player->object, ACTION_SQUAT, GamePlayer_AtStandDone );
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
	GameObject_AtLanding(
		player->object,
		ZSPEED_XF_HIT_FLY2,
		-ZACC_XF_HIT_FLY2,
		GamePlayer_AtHitFlyDone
	);
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

/** 让玩家从正面被击飞 */
void GamePlayer_SetFrontalXHitFly( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_HIT_FLY );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	if( GamePlayer_IsLeftOriented(player) ) {
		GameObject_SetXSpeed( player->object, XSPEED_X_HIT_FLY );
	} else {
		GameObject_SetXSpeed( player->object, -XSPEED_X_HIT_FLY );
	}
	GameObject_AtZSpeed( player->object, 20, GamePlayer_AtHitFlyMaxHeight );
	GameObject_AtLanding(
		player->object,
		ZSPEED_XF_HIT_FLY, -ZACC_XF_HIT_FLY,
		GamePlayer_AtFrontalHitFlyDone 
	);
}

/** 让玩家从背面被击飞 */
void GamePlayer_SetBackXHitFly( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_HIT_FLY );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	if( GamePlayer_IsLeftOriented(player) ) {
		GameObject_SetXSpeed( player->object, -XSPEED_X_HIT_FLY );
	} else {
		GameObject_SetXSpeed( player->object, XSPEED_X_HIT_FLY );
	}
	GameObject_AtLanding(
		player->object,
		ZSPEED_XB_HIT_FLY, -ZACC_XB_HIT_FLY,
		GamePlayer_AtHitFlyDone
	);
}

/** 向前翻滚超时后 */
static void GamePlayer_AtForwardRollTimeOut( GamePlayer *player )
{
	GameObject_SetXSpeed( player->object, 0 );
	GameObject_SetXAcc( player->object, 0 );
	if( GamePlayer_SetTummy( player ) == 0 ) {
		GamePlayer_SetRestTimeOut( 
			player, LONG_REST_TIMEOUT, 
			GamePlayer_StartStand 
		);
	}
}

/** 向后翻滚超时后 */
static void GamePlayer_AtBackwardRollTimeOut( GamePlayer *player )
{
	GameObject_SetXSpeed( player->object, 0 );
	GameObject_SetXAcc( player->object, 0 );
	if( GamePlayer_SetLying( player ) == 0 ) {
		GamePlayer_SetRestTimeOut( 
			player, LONG_REST_TIMEOUT, 
			GamePlayer_StartStand 
		);
	}
}

/** 开始朝左边进行前翻滚 */
static void GamePlayer_StartLeftForwardRoll( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_SetLeftOriented( player );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_F_ROLL );
	GamePlayer_LockAction( player );
	/* 一边滚动，一边减速 */
	GameObject_SetXAcc( player->object, XACC_ROLL );
	GamePlayer_SetActionTimeOut( player, ROLL_TIMEOUT, GamePlayer_AtForwardRollTimeOut );
}

/** 开始朝右边进行前翻滚 */
static void GamePlayer_StartRightForwardRoll( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_SetRightOriented( player );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_F_ROLL );
	GamePlayer_LockAction( player );
	GameObject_SetXAcc( player->object, -XACC_ROLL );
	GamePlayer_SetActionTimeOut( player, ROLL_TIMEOUT, GamePlayer_AtForwardRollTimeOut );
}

/** 开始朝左边进行后翻滚 */
static void GamePlayer_StartLeftBackwardRoll( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_B_ROLL );
	GamePlayer_LockAction( player );
	GameObject_SetXAcc( player->object, XACC_ROLL );
	GamePlayer_SetActionTimeOut( player, ROLL_TIMEOUT, GamePlayer_AtBackwardRollTimeOut );
}

/** 开始朝右边进行后翻滚 */
static void GamePlayer_StartRightBackwardRoll( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_B_ROLL );
	GamePlayer_LockAction( player );
	GameObject_SetXAcc( player->object, -XACC_ROLL );
	GamePlayer_SetActionTimeOut( player, ROLL_TIMEOUT, GamePlayer_AtBackwardRollTimeOut );
}

/** 让玩家从正面被撞飞 */
void GamePlayer_SetFrontalSHitFly( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_HIT_FLY );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	if( GamePlayer_IsLeftOriented(player) ) {
		GameObject_SetXSpeed( player->object, XSPEED_S_HIT_FLY );
		GameObject_AtLanding(
			player->object,
			ZSPEED_S_HIT_FLY, -ZACC_S_HIT_FLY,
			GamePlayer_StartRightBackwardRoll
		);
	} else {
		GameObject_SetXSpeed( player->object, -XSPEED_S_HIT_FLY );
		GameObject_AtLanding(
			player->object,
			ZSPEED_S_HIT_FLY, -ZACC_S_HIT_FLY,
			GamePlayer_StartLeftBackwardRoll
		);
	}
}

/** 让玩家从背面被撞飞 */
void GamePlayer_SetBackSHitFly( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_HIT_FLY );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	if( GamePlayer_IsLeftOriented(player) ) {
		GameObject_SetXSpeed( player->object, -XSPEED_S_HIT_FLY );
		GamePlayer_SetRightOriented( player );
		/* 落地时开始面朝左向前翻滚 */
		GameObject_AtLanding(
			player->object,
			ZSPEED_S_HIT_FLY, -ZACC_S_HIT_FLY,
			GamePlayer_StartLeftForwardRoll
		);
	} else {
		GameObject_SetXSpeed( player->object, XSPEED_S_HIT_FLY );
		GamePlayer_SetLeftOriented( player );
		/* 落地时开始面朝右向前翻滚 */
		GameObject_AtLanding( 
			player->object, 
			ZSPEED_S_HIT_FLY, -ZACC_S_HIT_FLY, 
			GamePlayer_StartRightForwardRoll
		);
	}
}

void GamePlayer_SetLeftHitFly( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_HIT_FLY );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_SetXAcc( player->object, 0 );
	GameObject_SetXSpeed( player->object, -XSPEED_HIT_FLY );
	GameObject_AtLanding( 
		player->object, 
		ZSPEED_HIT_FLY, -ZACC_HIT_FLY,
		GamePlayer_AtHitFlyDone );
}

void GamePlayer_SetRightHitFly( GamePlayer *player )
{
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

static void GamePlayer_StopLiftRun( GamePlayer *player )
{
	double speed, acc;

	if( GamePlayer_IsLeftOriented(player) ) {
		acc = XACC_STOPRUN;
	} else {
		acc = -XACC_STOPRUN;
	}
	GamePlayer_ChangeState( player, STATE_LIFT_STANCE );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_AtXSpeedToZero( player->object, acc, GamePlayer_AtRunEnd );
	speed = GameObject_GetYSpeed( player->object );
	acc = YSPEED_WALK * XACC_STOPRUN / XSPEED_RUN;
	if( speed < 0.0 ) {
		GameObject_SetYAcc( player->object, acc );
	}
	else if( speed > 0.0 ) {
		GameObject_SetYAcc( player->object, -acc );
	}
}

/** 停止奔跑 */
void GamePlayer_StopRun( GamePlayer *player )
{
	double speed, acc;
	if( player->state == STATE_LEFTRUN ) {
		acc = XACC_STOPRUN;
	}
	else if( player->state == STATE_RIGHTRUN ) {
		acc = -XACC_STOPRUN;
	} else {
		acc = 0.0;
	}
	player->control.run = FALSE;
	GamePlayer_SetReady( player );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_AtXSpeedToZero( player->object, acc, GamePlayer_AtRunEnd );
	speed = GameObject_GetYSpeed( player->object );
	acc = YSPEED_WALK * XACC_STOPRUN / XSPEED_RUN;
	if( speed < 0.0 ) {
		GameObject_SetYAcc( player->object, acc );
	}
	else if( speed > 0.0 ) {
		GameObject_SetYAcc( player->object, -acc );
	}
}

static void GamePlayer_AtCatchDone( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockAction( player->other );
	GamePlayer_SetReady( player );
	GamePlayer_SetRest( player->other );
}

static void GamePlayer_SetLeftCatch( GamePlayer *player )
{
	double x, y;
	if( !player->other ) {
		return;
	}
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockAction( player->other );
	GamePlayer_StopXMotion( player );
	GamePlayer_StopYMotion( player );
	x = GameObject_GetX( player->object );
	y = GameObject_GetY( player->object );
	GameObject_SetX( player->other->object, x-30 );
	GameObject_SetY( player->other->object, y );
	if( GamePlayer_IsLeftOriented(player->other) ) {
		GamePlayer_ChangeState( player->other, STATE_BACK_BE_CATCH );
	} else {
		GamePlayer_ChangeState( player->other, STATE_BE_CATCH );
	}
	GamePlayer_ChangeState( player, STATE_CATCH );
	GamePlayer_LockAction( player );
	GamePlayer_LockAction( player->other );
	GamePlayer_LockMotion( player );
	GamePlayer_LockMotion( player->other );
	GamePlayer_SetActionTimeOut( player, 2000, GamePlayer_AtCatchDone );
}

static void GamePlayer_SetRightCatch( GamePlayer *player )
{
	double x, y;
	if( !player->other ) {
		return;
	}
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockAction( player->other );
	GamePlayer_StopXMotion( player );
	GamePlayer_StopYMotion( player );
	x = GameObject_GetX( player->object );
	y = GameObject_GetY( player->object );
	GameObject_SetX( player->other->object, x+30 );
	GameObject_SetY( player->other->object, y );
	if( GamePlayer_IsLeftOriented(player->other) ) {
		GamePlayer_ChangeState( player->other, STATE_BE_CATCH );
	} else {
		GamePlayer_ChangeState( player->other, STATE_BACK_BE_CATCH );
	}
	GamePlayer_ChangeState( player, STATE_CATCH );
	GamePlayer_LockAction( player );
	GamePlayer_LockAction( player->other );
	GamePlayer_LockMotion( player );
	GamePlayer_LockMotion( player->other );
	GamePlayer_SetActionTimeOut( player, 2000, GamePlayer_AtCatchDone );
}

/** 抓住正处于喘气（歇息）状态下的玩家 */
static GamePlayer* GamePlayer_CatchGaspingPlayer( GamePlayer *player )
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
	GamePlayer *other_player;
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
	case STATE_WALK:
		GamePlayer_SetLeftOriented( player );
		other_player = GamePlayer_CatchGaspingPlayer( player );
		if( other_player ) {
			player->other = other_player;
			GamePlayer_SetLeftCatch( player );
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
	double x, y, speed;
	GamePlayer *other_player;

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
	case STATE_WALK:
		GamePlayer_SetRightOriented( player );
		other_player = GamePlayer_CatchGaspingPlayer( player );
		if( other_player ) {
			player->other = other_player;
			GamePlayer_SetRightCatch( player );
			break;
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
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
	GamePlayer_SetReady( player );
}

/** 在冲刺后的起跳动作结束时 */
static void GamePlayer_AtSprintSquatDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_ChangeState( player, STATE_SJUMP );
	GameObject_AtTouch( widget, GamePlayer_PorcJumpTouch );
}


/** 冲刺+下蹲 */
static void GamePlayer_SetSprintSquat( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GameObject_SetXAcc( player->object, 0 );
	GamePlayer_ChangeState( player, STATE_SSQUAT );
	GamePlayer_LockMotion( player );
	GameObject_AtActionDone( player->object, ACTION_SQUAT, GamePlayer_AtSprintSquatDone );
	GameObject_AtLanding( player->object, ZSPEED_JUMP, -ZACC_JUMP, GamePlayer_AtLandingDone );
}

/** 在被举起的状态下，主动起跳 */
static void GamePlayer_BeLiftActiveStartJump( GamePlayer *player )
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
	if( LCUIKey_IsHit(player->ctrlkey.left) ) {
		GameObject_SetXSpeed( player->object, -XSPEED_WALK );
	}
	else if( LCUIKey_IsHit(player->ctrlkey.right) ){
		GameObject_SetXSpeed( player->object, XSPEED_WALK );
	}
	GamePlayer_SetSquat( player );
}

/** 跳跃 */
void GamePlayer_StartJump( GamePlayer *player )
{
	switch(player->state) {
	case STATE_RIDE:
	case STATE_RIDE_ATTACK:
	case STATE_RIDE_JUMP:
	case STATE_SJUMP:
	case STATE_JUMP:
	case STATE_LIFT_JUMP:
	case STATE_LIFT_FALL:
	case STATE_SQUAT:
	case STATE_JSQUAT:
		break;
	case STATE_BE_LIFT_SQUAT:
	case STATE_BE_LIFT_STANCE:
		GamePlayer_BeLiftActiveStartJump( player );
		break;
	case STATE_LIFT_STANCE:
	case STATE_LIFT_WALK:
	case STATE_LIFT_RUN:
		player->control.run = FALSE;
		GamePlayer_SetLiftJump( player );
		break;
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
	case STATE_AS_ATTACK:
	case STATE_BS_ATTACK:
		player->control.run = FALSE;
		GamePlayer_SetSprintSquat( player );
		break;
	default:
		if( player->lock_action ) {
			return;
		}
		GamePlayer_SetSquat( player );
		break;
	}
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

static void GamePlayer_JumpElbowStep2( LCUI_Widget *widget )
{
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_JUMP_ELBOW );
	GamePlayer_LockAction( player );
	player->attack_type = ATTACK_TYPE_JUMP_ELBOW;
	GameObject_AtZeroZSpeed( widget, NULL );
	GameObject_ClearAttack( player->object );
}

static void GamePlayer_JumpElbowStep1( LCUI_Widget *widget )
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
	GameObject_AtZeroZSpeed( widget, GamePlayer_JumpElbowStep2 );
}

/** 进行跳跃+肘击 */
void GamePlayer_SetJumpElbow( GamePlayer *player )
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
	GameObject_AtActionDone( player->object, ACTION_SQUAT, GamePlayer_JumpElbowStep1 );
}

static void GamePlayer_JumpTreadStep2( LCUI_Widget *widget )
{
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_JUMP_STOMP );
	GamePlayer_LockAction( player );
	player->attack_type = ATTACK_TYPE_JUMP_TREAD;
	GameObject_AtZeroZSpeed( widget, NULL );
	GameObject_ClearAttack( player->object );
}
static void GamePlayer_JumpTreadStep1( LCUI_Widget *widget )
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
	GameObject_AtZeroZSpeed( widget, GamePlayer_JumpTreadStep2 );
}

/** 进行跳跃+踩 */
void GamePlayer_SetJumpTread( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GameObject_SetXAcc( player->object, 0 );
	GamePlayer_ChangeState( player, STATE_SQUAT );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_SetXSpeed( player->object, 0 );
	GameObject_SetYSpeed( player->object, 0 );
	GameObject_SetZSpeed( player->object, 0 );
	GameObject_AtActionDone( player->object, ACTION_SQUAT, GamePlayer_JumpTreadStep1 );
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

/** 检测当前角色是否能够举起另一个角色 */
static LCUI_BOOL GamePlayer_CanLiftPlayer( GamePlayer *player, GamePlayer *other_player )
{
	double x1, x2;
	x1 = GameObject_GetX( player->object );
	x2 = GameObject_GetX( other_player->object );
	/* 如果在中间位置附近 */
	if( x1 > x2-10 && x1 < x2+10 ) {
		return TRUE;
	}
	return FALSE;
}

/** 检测玩家是否能够攻击躺在地上的玩家 */
static LCUI_BOOL GamePlayer_CanAttackGroundPlayer( GamePlayer *player, GamePlayer *other_player )
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

/** 爆裂腿 */
static void GamePlayer_SetBombKick( GamePlayer *player )
{
	if( player->lock_motion ) {
		return;
	}
	if( player->lock_action ) {
		return;
	}
	if( !player->skill.bomb_kick ) {
		return;
	}
	GamePlayer_ChangeState( player, STATE_BOMBKICK );
	player->attack_type = ATTACK_TYPE_BOMB_KICK;
	GameObject_ClearAttack( player->object );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	if( GamePlayer_IsLeftOriented(player) ) {
		GameObject_SetXSpeed( player->object, -100 );
	} else {
		GameObject_SetXSpeed( player->object, 100 );
	}
	GameObject_AtLanding( player->object, 20, -10, GamePlayer_AtLandingDone );
}

/** 二段自旋击 */
static void GamePlayer_SetSecondSpinHit( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_SPINHIT );
	player->attack_type = ATTACK_TYPE_SPIN_HIT2;
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	if( LCUIKey_IsHit(player->ctrlkey.left) ) {
		GameObject_SetXSpeed( player->object, -40 );
	} 
	else if( LCUIKey_IsHit(player->ctrlkey.right) ) {
		GameObject_SetXSpeed( player->object, 40 );
	} else {
		GameObject_SetXSpeed( player->object, 0 );
	}
	GameObject_AtLanding( player->object, 80, -20, GamePlayer_AtLandingDone );
}

/** 自旋击（翻转击） */
static void GamePlayer_SetSpinHit( GamePlayer *player )
{
	if( player->lock_motion ) {
		return;
	}
	if( player->lock_action ) {
		return;
	}
	if( !player->skill.bomb_kick ) {
		return;
	}
	GamePlayer_ChangeState( player, STATE_SPINHIT );
	player->attack_type = ATTACK_TYPE_SPIN_HIT;
	GameObject_ClearAttack( player->object );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	if( GamePlayer_IsLeftOriented(player) ) {
		GameObject_SetXSpeed( player->object, -100 );
	} else {
		GameObject_SetXSpeed( player->object, 100 );
	}
	GameObject_AtLanding( player->object, 30, -10, GamePlayer_AtLandingDone );
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
	GameObject_AtLanding( widget, 20, -10, GamePlayer_AtAttackDone );
	GameObject_AtZeroZSpeed( widget, GamePlayer_AtBigElbowStep2 );
}

/** 肘压 */
static void GamePlayer_SetBigElbow( GamePlayer *player )
{
	double z_speed;
	GamePlayer_ChangeState( player, STATE_JUMP_ELBOW );
	player->attack_type = ATTACK_TYPE_BIG_ELBOW;
	GameObject_ClearAttack( player->object );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	z_speed = GameObject_GetZSpeed( player->object );
	GameObject_AtLanding( player->object, z_speed, -ZACC_JUMP, GamePlayer_AtBigElbowStep1 );
}

static void GamePlayer_StopMachStomp( GamePlayer *player )
{
	GamePlayer_AtAttackDone( player->object );
}

/** 高速踩踏 */
static void GamePlayer_SetMachStomp( GamePlayer *player )
{
	GamePlayer_ChangeState( player, STATE_MACH_STOMP );
	player->attack_type = ATTACK_TYPE_MACH_STOMP;
	GameObject_ClearAttack( player->object );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_SetXSpeed( player->object, 0 );
	GameObject_SetYSpeed( player->object, 0 );
	GameObject_AtActionDone( player->object, ACTION_MACH_STOMP, GameObject_ClearAttack );
	GamePlayer_SetActionTimeOut( player, 1250, GamePlayer_StopMachStomp );
}

/** 在肘压技能结束时 */
static void GamePlayer_AtElbowDone( LCUI_Widget *widget )
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
		Game_RecordAttack(	player, ATTACK_TYPE_ELBOW1, 
					player->other, player->other->state );
		break;
	case 2:
		GamePlayer_ChangeState( player->other, STATE_HIT_FLY );
		GameObject_AtActionDone( player->other->object, ACTION_HIT_FLY, NULL );
		break;
	case 4:
		/* 记录第二段攻击伤害 */
		Game_RecordAttack(	player, ATTACK_TYPE_ELBOW1, 
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

static void GamePlayer_SetFrontCatchSkillA( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockAction( player->other );
	/* 根据攻击者的类型，让受攻击者做出相应动作 */
	switch(player->type) {
	case PLAYER_TYPE_FIGHTER:break;
	case PLAYER_TYPE_MARTIAL_ARTISTS:
		GamePlayer_ChangeState( player->other, STATE_HIT );
		GameObject_AtActionDone( player->other->object, ACTION_HIT, NULL );
		if( GamePlayer_IsLeftOriented(player) ) {
			GamePlayer_SetLeftOriented( player->other );
		} else {
			GamePlayer_SetRightOriented( player->other );
		}
		break;
	case PLAYER_TYPE_KUNG_FU:break;
	case PLAYER_TYPE_JUDO_MASTERS:break;
	default:return;
	}
	GamePlayer_ChangeState( player, STATE_CATCH_SKILL_FA );
	GameObject_AtActionUpdate(
		player->object,
		ACTION_CATCH_SKILL_FA, 
		GamePlayer_AtElbowUpdate 
	);
	GameObject_AtActionDone(
		player->object, ACTION_CATCH_SKILL_FA, 
		GamePlayer_AtElbowDone 
	);
	/* 重置被攻击的次数 */
	player->other->n_attack = 0;
	GamePlayer_LockAction( player );
	GamePlayer_LockAction( player->other );
}

/** 在被膝击后落地时 */
static void GamePlayer_AtLandingByAfterKneeHit( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	/* 记录第二段攻击伤害 */
	Game_RecordAttack(	player->other, ATTACK_TYPE_KNEE_HIT2,
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

static void GamePlayer_BackCatchSkillAUpdate( LCUI_Widget *widget )
{
	GamePlayer *player;
	
	player = GamePlayer_GetPlayerByWidget( widget );
	switch( GameObject_GetCurrentActionFrameNumber( player->object) ) {
	case 0:
		break;
	case 1:
		/* 记录第一段攻击伤害 */
		Game_RecordAttack(	player, ATTACK_TYPE_KNEE_HIT1, 
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
		GameObject_AtLanding(	player->other->object, -20, 0,
					GamePlayer_AtLandingByAfterKneeHit );
	default:
		break;
	}
}

static void GamePlayer_SetBackCatchSkillA( GamePlayer *player )
{
	double x, y;
	int z_index;

	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockAction( player->other );
	/* 根据攻击者的类型，让受攻击者做出相应动作 */
	switch(player->type) {
	case PLAYER_TYPE_FIGHTER:break;
	case PLAYER_TYPE_MARTIAL_ARTISTS:
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
		GameObject_AtLanding( player->other->object, -20, 0, NULL );
		GameObject_AtActionUpdate(
			player->object, ACTION_CATCH_SKILL_BA, 
			GamePlayer_BackCatchSkillAUpdate 
		);
		break;
	case PLAYER_TYPE_KUNG_FU:break;
	case PLAYER_TYPE_JUDO_MASTERS:break;
	default:return;
	}
	GamePlayer_ChangeState( player, STATE_CATCH_SKILL_BA );
	GameObject_AtActionDone(
		player->object, ACTION_CATCH_SKILL_BA, 
		GamePlayer_AtElbowDone
	);
	/* 重置被攻击的次数 */
	player->other->n_attack = 0;
	GamePlayer_LockAction( player );
	GamePlayer_LockAction( player->other );
}

static void GamePlayer_AtLiftDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_LIFT_STANCE );
	GameObject_SetZ( player->other->object, LIFT_HEIGHT );
	/* 在举起者的位置变化时更新被举起者的位置 */
	GameObject_AtMove( widget, GamePlayer_UpdateLiftPosition );
}

/** 设置举起另一个角色 */
static void GamePlayer_SetLiftPlayer( GamePlayer *player )
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
	GamePlayer_UnlockAction( player );
	/* 改为下蹲状态 */
	GamePlayer_ChangeState( player, STATE_SQUAT );
	GamePlayer_LockAction( player );
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

static void GamePlayer_AtThrowLanding( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_StopXMotion( player );
	GamePlayer_StopYMotion( player );
	GamePlayer_StartStand( player );
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
	GamePlayer_ReduceSpeed( player, 75 );
	Game_RecordAttack( player->other, ATTACK_TYPE_THROW, player, player->state );
	player->other = NULL;
	GameObject_AtLanding(
		widget, ZSPEED_XF_HIT_FLY2, -ZACC_XF_HIT_FLY2,
		GamePlayer_AtThrowUpFlyDone
	);
}

/** 处理游戏角色在被抛出时与其他角色的碰撞 */
static void GamePlayer_ProcThrowUpFlyAttack( LCUI_Widget *self, LCUI_Widget *other )
{
	double x1, x2;
	GamePlayer *player, *other_player;

	player = GamePlayer_GetPlayerByWidget( self );
	other_player = GamePlayer_GetPlayerByWidget( other );
	if( !player || !other_player ) {
		return;
	}

	/* 判断自己的状态是否符合要求 */
	if( player->state != STATE_HIT_FLY
	 && player->state != STATE_HIT_FLY_FALL 
	 && player->state != STATE_LYING_HIT ) {
		return;
	}

	if( GamePlayer_TryHit( other_player ) == 0 ) {
		return;
	}
	x1 = GameObject_GetX( self );
	x2 = GameObject_GetX( other );
	/* 记录攻击 */
	Game_RecordAttack(	player->other, ATTACK_TYPE_BUMPED,
				other_player, other_player->state );
	/* 根据两者坐标，判断击飞的方向 */
	if( x1 < x2 ) {
		GamePlayer_SetRightHitFly( other_player );
	} else {
		GamePlayer_SetLeftHitFly( other_player );
	}
	other_player->n_attack = 0;
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

/** 将举起的角色向前抛出 */
static void GamePlayer_SetThrowUp( GamePlayer *player )
{
	double x_speed;
	/* 如果在跑步，那就减点速度 */
	if( player->state == STATE_LIFT_RUN ) {
		if( GamePlayer_IsLeftOriented(player) ) {
			GameObject_SetXAcc( player->object, XACC_STOPRUN );
		} else {
			GameObject_SetXAcc( player->object, -XACC_STOPRUN );
		}
	}
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_THROW );
	GameObject_AtActionDone( player->object, ACTION_THROW, GamePlayer_AtThrowDone );
	if( player->other ) {
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
			GameObject_AtTouch(
				player->other->object,
				GamePlayer_ProcThrowUpFlyAttack 
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
	}
	GamePlayer_LockAction( player );
	/* 解除举起者与被举起者的关系 */
	//player->other->other = NULL;
	player->other = NULL;
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
	} else {
		GamePlayer_ChangeState( player, STATE_LYING_HIT );
	}
	GamePlayer_LockAction( player );
	Game_RecordAttack( player->other, ATTACK_TYPE_THROW, player, player->state );
	player->other = NULL;
	/* 落地后缩减该角色75%的移动速度 */
	GamePlayer_ReduceSpeed( player, 75 );
	GameObject_AtLanding(
		player->object, ZSPEED_XF_HIT_FLY2,
		-ZACC_XF_HIT_FLY2, GamePlayer_AtBeThrowDownDone
	);
}

/** 将举起的角色向下砸 */
static void GamePlayer_SetThrowDown( GamePlayer *player )
{
	double x_speed;
	if( player->state == STATE_LIFT_RUN ) {
		if( GamePlayer_IsLeftOriented(player) ) {
			GameObject_SetXAcc( player->object, XACC_STOPRUN );
		} else {
			GameObject_SetXAcc( player->object, -XACC_STOPRUN );
		}
	}
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_THROW );
	GameObject_AtActionDone( player->object, ACTION_THROW, GamePlayer_AtThrowDone );
	if( player->other ) {
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
				x_speed = -XSPEED_THROWDOWN_FLY;
				GamePlayer_SetRightOriented( player->other );
			} else {
				x_speed = XSPEED_THROWDOWN_FLY;
				GamePlayer_SetLeftOriented( player->other );
			}
			x_speed += GameObject_GetXSpeed( player->object )/2;
			GamePlayer_LockMotion( player->other );
			GameObject_SetXSpeed( player->other->object, x_speed );
			GameObject_AtLanding(
				player->other->object, -ZSPEED_THROWDOWN_FLY,
				-ZACC_THROWDOWN_FLY, GamePlayer_AtBeThrowDownLanding
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
			x_speed += GameObject_GetXSpeed( player->object )/4;
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
	}
	GamePlayer_LockAction( player );
	/* 解除举起者与被举起者的关系 */
	//player->other->other = NULL;
	player->other = NULL;
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

/** 骑着攻击 */
static void GamePlayer_SetRideAttack( GamePlayer *player )
{
	if( !player->other ) {
		return;
	}
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_RIDE_ATTACK );
	GamePlayer_LockAction( player );
	GamePlayer_TryHit( player->other );
	Game_RecordAttack( player, ATTACK_TYPE_RIDE_ATTACK, player->other, player->other->state );
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
		Game_RecordAttack(	player, ATTACK_TYPE_RIDE_JUMP_ATTACK, 
					player->other, player->other->state );
		GamePlayer_SetRideAttack( player );
		break;
	default:
		player->other = NULL;
		GamePlayer_StartStand( player );
		break;
	}
}

static void GamePlayer_SetRideJump( GamePlayer *player )
{
	if( !player->other ) {
		return;
	}
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_RIDE_JUMP );
	GamePlayer_LockAction( player );
	GameObject_AtLanding( player->object, ZSPEED_JUMP, -ZACC_JUMP, GamePlayer_AtRideJumpDone );
}

/** 进行A攻击 */
void GamePlayer_StartAAttack( GamePlayer *player )
{
	double acc, speed;
	LCUI_Widget *widget;
	GamePlayer *other_player;
	void (*func)(LCUI_Widget*);

	if( player->state == STATE_CATCH && player->other ) {
		/* 根据方向，判断该使用何种技能 */
		if( GamePlayer_IsLeftOriented(player) ) {
			if( GamePlayer_IsLeftOriented(player->other) ) {
				GamePlayer_SetBackCatchSkillA( player );
			} else {
				GamePlayer_SetFrontCatchSkillA( player );
			}
		} else {
			if( GamePlayer_IsLeftOriented(player->other) ) {
				GamePlayer_SetFrontCatchSkillA( player );
			} else {
				GamePlayer_SetBackCatchSkillA( player );
			}
		}
		return;
	}
	if( player->lock_action ) {
		return;
	}
	
	switch(player->state) {
	case STATE_RIDE:
		GamePlayer_SetRideAttack( player );
	case STATE_RIDE_ATTACK:
	case STATE_A_ATTACK:
	case STATE_B_ATTACK:
		return;
	case STATE_LIFT_JUMP:
	case STATE_LIFT_FALL:
	case STATE_LIFT_RUN:
	case STATE_LIFT_STANCE:
	case STATE_LIFT_WALK:
		player->control.run = FALSE;
		/* 将举起的角色向下砸 */
		GamePlayer_SetThrowDown( player );
		return;
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
		player->control.run = FALSE;
		if( player->state == STATE_RIGHTRUN ) {
			GameObject_AtXSpeedToZero(
				player->object, -XACC_DASH, 
				GamePlayer_AtAttackDone 
			);
		} else {
			GameObject_AtXSpeedToZero(
				player->object, XACC_DASH, 
				GamePlayer_AtAttackDone 
			);
		}
		GamePlayer_ChangeState( player, STATE_AS_ATTACK );
		GamePlayer_LockAction( player );
		GamePlayer_LockMotion( player );
		speed = GameObject_GetYSpeed( player->object );
		acc = YSPEED_WALK * XACC_DASH / XSPEED_RUN;
		if( speed < 0.0 ) {
			GameObject_SetYAcc( player->object, acc );
		}
		else if( speed > 0.0 ) {
			GameObject_SetYAcc( player->object, -acc );
		}
		player->attack_type = ATTACK_TYPE_S_PUNCH;
		/* 清除攻击记录 */
		GameObject_ClearAttack( player->object );
		return;
	case STATE_JUMP:
	case STATE_JSQUAT:
		speed = GameObject_GetZSpeed( player->object );
		/* 如果满足使用 跳跃肘压 技能的条件 */
		if( speed < 0 && player->skill.big_elbow ) {
			GamePlayer_SetBigElbow( player );
			return;
		}
		GamePlayer_ChangeState( player, STATE_AJ_ATTACK );
		GamePlayer_LockAction( player );
		player->attack_type = ATTACK_TYPE_JUMP_PUNCH;
		GameObject_ClearAttack( player->object );
		return;
	case STATE_SJUMP:
	case STATE_SSQUAT:
		GamePlayer_ChangeState( player, STATE_ASJ_ATTACK );
		GamePlayer_LockAction( player );
		player->attack_type = ATTACK_TYPE_SJUMP_PUNCH;
		GameObject_ClearAttack( player->object );
		return;
	default: 
		break;
	}
	
	other_player = GamePlayer_GetGroundPlayer( player );
	if( other_player ) {
		if( GamePlayer_CanLiftPlayer( player, other_player ) ) {
			player->other = other_player;
			GamePlayer_SetLiftPlayer( player );
			return;
		}
		if( GamePlayer_CanAttackGroundPlayer(player, other_player) ) {
			if( player->skill.mach_stomp ) {
				GamePlayer_SetMachStomp( player );
			} else {
				GamePlayer_SetJumpElbow( player );
			}
			return;
		}
	}

	widget = GameObject_GetObjectInAttackRange( 
				player->object,
				ACTION_FINAL_BLOW,
				TRUE, ACTION_REST 
	);
	/* 若当前是在被举起的状态下，站立着，则改用相应回调函数 */
	if( player->state == STATE_BE_LIFT_STANCE
	|| player->state == STATE_BE_LIFT_SQUAT ) {
		func = GamePlayer_AtBeLiftAttackDone;
	} else {
		func = GamePlayer_AtAttackDone;
	}

	if( widget ) {
		GamePlayer_ChangeState( player, STATE_FINAL_BLOW );
		GameObject_AtActionDone( player->object, ACTION_FINAL_BLOW, func );
		player->attack_type = ATTACK_TYPE_FINAL_BLOW;
	} else {
		GamePlayer_ChangeState( player, STATE_A_ATTACK );
		GameObject_AtActionDone( player->object, ACTION_A_ATTACK, func );
		player->attack_type = ATTACK_TYPE_PUNCH;
	}
	GameObject_ClearAttack( player->object );
	GamePlayer_StopXWalk( player );
	GamePlayer_StopYMotion( player );
	GamePlayer_LockMotion( player );
	GamePlayer_LockAction( player );
	player->control.run = FALSE;
}

static void GamePlayer_AtWeakWalkDone( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_F_ROLL );
	GamePlayer_LockAction( player );
	GamePlayer_SetActionTimeOut( player, ROLL_TIMEOUT, GamePlayer_AtForwardRollTimeOut );
}

static void GamePlayer_SetPush( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockAction( player->other );
	GamePlayer_ChangeState( player, STATE_CATCH_SKILL_BB );
	GameObject_AtActionDone( player->object, ACTION_CATCH_SKILL_BB, GamePlayer_AtAttackDone );
	GamePlayer_ChangeState( player->other, STATE_WEAK_RUN );
	/* 在 STATE_WEAK_RUN 状态持续一段时间后结束 */
	GamePlayer_SetActionTimeOut( player->other, 2000, GamePlayer_AtWeakWalkDone );
	GamePlayer_LockAction( player );
	GamePlayer_LockAction( player->other );
	if( GamePlayer_IsLeftOriented(player->other) ) {
		GameObject_SetXSpeed( player->other->object, -XSPEED_WEAK_WALK );
	} else {
		GameObject_SetXSpeed( player->other->object, XSPEED_WEAK_WALK );
	}
	GamePlayer_LockMotion( player->other );
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
	/* 若推自己的角色的动作还未完成，则忽略他 */
	if( other_player->state == STATE_CATCH_SKILL_FB
	&& other_player->other == player ) {
		return;
	}
	x1 = GameObject_GetX( self );
	x2 = GameObject_GetX( other );
	/* 根据两者坐标，判断击飞的方向 */
	if( x1 < x2 ) {
		GamePlayer_SetLeftHitFly( player );
		GamePlayer_SetRightHitFly( other_player );
	} else {
		GamePlayer_SetRightHitFly( player );
		GamePlayer_SetLeftHitFly( other_player );
	}
	/* 两个都受到攻击伤害 */
	Game_RecordAttack( other_player, ATTACK_TYPE_BUMPED, player, player->state );
	Game_RecordAttack( player, ATTACK_TYPE_BUMPED, other_player, other_player->state );
	GameObject_AtTouch( player->object, NULL );
	player->n_attack = 0;
	other_player->n_attack = 0;
}

static void GamePlayer_SetPull( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockAction( player->other );
	if( GamePlayer_IsLeftOriented(player) ) {
		GamePlayer_SetRightOriented( player );
	} else {
		GamePlayer_SetLeftOriented( player );
	}
	GamePlayer_ChangeState( player, STATE_CATCH_SKILL_FB );
	GameObject_AtActionDone( player->object, ACTION_CATCH_SKILL_FB, GamePlayer_AtAttackDone );
	GamePlayer_ChangeState( player->other, STATE_WEAK_RUN_ATTACK );
	/* 在与其他对象触碰时进行响应 */
	GameObject_AtTouch( player->other->object, GamePlayer_ProcWeakWalkAttack );
	GamePlayer_SetActionTimeOut( player->other, 2000, GamePlayer_AtWeakWalkDone );
	GamePlayer_LockAction( player );
	GamePlayer_LockAction( player->other );
	if( GamePlayer_IsLeftOriented(player->other) ) {
		GameObject_SetXSpeed( player->other->object, -XSPEED_WEAK_WALK );
	} else {
		GameObject_SetXSpeed( player->other->object, XSPEED_WEAK_WALK );
	}
	GamePlayer_LockMotion( player->other );
}

/** 进行B攻击 */
void GamePlayer_StartBAttack( GamePlayer *player )
{
	double acc, speed;
	LCUI_Widget *widget;
	GamePlayer *other_player;
	void (*func)(LCUI_Widget*);

	if( player->state == STATE_CATCH && player->other ) {
		/* 根据方向，判断该使用何种技能 */
		if( GamePlayer_IsLeftOriented(player) ) {
			if( GamePlayer_IsLeftOriented(player->other) ) {
				GamePlayer_SetPush( player );
			} else {
				GamePlayer_SetPull( player );
			}
		} else {
			if( GamePlayer_IsLeftOriented(player->other) ) {
				GamePlayer_SetPull( player );
			} else {
				GamePlayer_SetPush( player );
			}
		}
		return;
	}
	if( player->lock_action ) {
		return;
	}

	switch(player->state) {
	case STATE_RIDE:
		GamePlayer_SetRideJump( player );
	case STATE_RIDE_ATTACK:
	case STATE_A_ATTACK:
	case STATE_B_ATTACK:
		return;
	case STATE_LIFT_JUMP:
	case STATE_LIFT_FALL:
	case STATE_LIFT_RUN:
	case STATE_LIFT_STANCE:
	case STATE_LIFT_WALK:
		player->control.run = FALSE;
		/* 将举起的角色向前抛 */
		GamePlayer_SetThrowUp( player );
		return;
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
		player->control.run = FALSE;
		if( player->state == STATE_RIGHTRUN ) {
			GameObject_AtXSpeedToZero(
				player->object, -XACC_DASH, 
				GamePlayer_AtAttackDone 
			);
		} else {
			GameObject_AtXSpeedToZero(
				player->object, XACC_DASH, 
				GamePlayer_AtAttackDone 
			);
		}
		GamePlayer_ChangeState( player, STATE_BS_ATTACK );
		GamePlayer_LockAction( player );
		GamePlayer_LockMotion( player );
		speed = GameObject_GetYSpeed( player->object );
		acc = YSPEED_WALK * XACC_DASH / XSPEED_RUN;
		if( speed < 0.0 ) {
			GameObject_SetYAcc( player->object, acc );
		}
		else if( speed > 0.0 ) {
			GameObject_SetYAcc( player->object, -acc );
		}
		player->attack_type = ATTACK_TYPE_S_KICK;
		GameObject_ClearAttack( player->object );
		return;
	case STATE_JUMP:
	case STATE_JSQUAT:
		GamePlayer_ChangeState( player, STATE_BJ_ATTACK );
		GamePlayer_LockAction( player );
		player->attack_type = ATTACK_TYPE_JUMP_KICK;
		GameObject_ClearAttack( player->object );
		return;
	case STATE_SJUMP:
	case STATE_SSQUAT:
		GamePlayer_ChangeState( player, STATE_BSJ_ATTACK );
		GamePlayer_LockAction( player );
		player->attack_type = ATTACK_TYPE_SJUMP_KICK;
		GameObject_ClearAttack( player->object );
		return;
	default:
		break;
	}
	/* 检测附近是否有躺在地上的其它角色 */
	other_player = GamePlayer_GetGroundPlayer( player );
	if( other_player ) {
		/* 如果符合举起该角色的要求 */
		if( GamePlayer_CanLiftPlayer( player, other_player ) ) {
			player->other = other_player;
			GamePlayer_SetLiftPlayer( player );
			return;
		}
		if( GamePlayer_CanAttackGroundPlayer(player, other_player) ) {
			GamePlayer_SetJumpTread( player );
			return;
		}
	}
	/* 清除攻击记录 */
	GameObject_ClearAttack( player->object );
	widget = GameObject_GetObjectInAttackRange( 
				player->object,
				ACTION_FINAL_BLOW,
				TRUE, ACTION_REST 
	);
	if( player->state == STATE_BE_LIFT_STANCE
	 || player->state == STATE_BE_LIFT_SQUAT ) {
		func = GamePlayer_AtBeLiftAttackDone;
	} else {
		func = GamePlayer_AtAttackDone;
	}
	if( widget ) {
		GamePlayer_ChangeState( player, STATE_FINAL_BLOW );
		player->attack_type = ATTACK_TYPE_FINAL_BLOW;
		GameObject_AtActionDone( player->object, ACTION_FINAL_BLOW, func );
	} else {
		GamePlayer_ChangeState( player, STATE_B_ATTACK );
		player->attack_type = ATTACK_TYPE_KICK;
		GameObject_AtActionDone( player->object, ACTION_B_ATTACK, func );
	}
	GamePlayer_StopXWalk( player );
	GamePlayer_StopYMotion( player );
	GamePlayer_LockMotion( player );
	GamePlayer_LockAction( player );
	player->control.run = FALSE;
}

/** 骑在躺地者身上 */
static void GamePlayer_SetRidePlayer( GamePlayer *self, GamePlayer *other )
{
	int z_index;
	double x, y;
	self->other = other;
	other->other = self;
	GamePlayer_StopXMotion( self );
	GamePlayer_StopYMotion( self );
	x = GameObject_GetX( other->object );
	y = GameObject_GetY( other->object );
	GameObject_SetX( self->object, x );
	GameObject_SetY( self->object, y );
	GamePlayer_ChangeState( self, STATE_RIDE );
	z_index = Widget_GetZIndex( other->object );
	Widget_SetZIndex( self->object, z_index+1 );
}

void GamePlayer_SetUpMotion( GamePlayer *player )
{
	double speed;
	GamePlayer *other_player;

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
		GamePlayer_ChangeState( player, STATE_WALK );
		break;
	case STATE_WALK:
		other_player = GamePlayer_GetGroundPlayer( player );
		if( !other_player ) {
			break;
		}
		if( !GamePlayer_CanLiftPlayer( player, other_player ) ) {
			break;
		}
		GamePlayer_SetRidePlayer( player, other_player );
		return;
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
	GamePlayer *other_player;

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
		GamePlayer_ChangeState( player, STATE_WALK );
	case STATE_WALK:
		other_player = GamePlayer_GetGroundPlayer( player );
		if( !other_player ) {
			break;
		}
		if( !GamePlayer_CanLiftPlayer( player, other_player ) ) {
			break;
		}
		GamePlayer_SetRidePlayer( player, other_player );
		return;
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
	case STATE_LIFT_RUN:
		break;
	default:return;
	}
	speed = YSPEED_WALK * player->property.speed / 100;
	GameObject_SetYSpeed( player->object, speed );
}

static void GamePlayer_JumpSpinKickStart( LCUI_Widget *widget )
{
	GamePlayer *player;

	player = GamePlayer_GetPlayerByWidget( widget );
	/* 增加下落的速度 */
	GameObject_SetZAcc( player->object, -(ZACC_JUMP+100) );
	/* 根据角色目前面向的方向，设定水平移动方向及速度 */
	if( GamePlayer_IsLeftOriented( player ) ) {
		GameObject_SetXSpeed( player->object, -(XSPEED_RUN+100) );
	} else {
		GameObject_SetXSpeed( player->object, XSPEED_RUN+100 );
	}
	GameObject_AtZeroZSpeed( player->object, NULL );
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_KICK );
	GamePlayer_LockAction( player );
}

static void GamePlayer_SetJumpSpinKick( GamePlayer *player )
{
	double z_speed;
	if( player->lock_action ) {
		return;
	}
	/* 如果该游戏角色没有 高跳旋转落踢 这一技能  */
	if( !player->skill.jump_spin_kick ) {
		return;
	}
	/* 如果该游戏角色并没处于奔跑后的 跳跃 的状态下 */
	if( player->state != STATE_SJUMP
	&& player->state != STATE_SSQUAT ) {
		return;
	}
	/* 增加点在Z轴的移动速度，以增加高度 */
	z_speed = GameObject_GetZSpeed( player->object );
	z_speed += 100;
	GameObject_SetZSpeed( player->object, z_speed );
	GameObject_SetZAcc( player->object, -(ZACC_JUMP+50) );
	player->attack_type = ATTACK_TYPE_JUMP_SPIN_KICK;
	GameObject_ClearAttack( player->object );
	/* 开始翻滚 */
	GamePlayer_ChangeState( player, STATE_SPINHIT );
	/* 锁定动作和移动 */
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	/* 滚了一会后再开始出脚 */
	GameObject_AtZeroZSpeed( player->object, GamePlayer_JumpSpinKickStart );
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
	player->property.speed = 100;
	/* 初始化角色动作动画 */
	GamePlayer_InitAction( player, role_id );
	GameObject_SetShadow( player->object, img_shadow );
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
		switch(atk_player->attack_type) {
		case ATTACK_TYPE_NONE:break;
		case ATTACK_TYPE_S_PUNCH:
		case ATTACK_TYPE_S_KICK:
			player->n_attack = 0;
			/* 减少攻击者百分之50的移动速度 */
			GamePlayer_ReduceSpeed( atk_player, 50 );
			if( GamePlayer_TryHit(player) == 0 ) {
				break;
			}
			atk_player = GamePlayer_GetPlayerByWidget( p_info->attacker );
			if( GamePlayer_IsLeftOriented( atk_player ) ) {
				if( GamePlayer_IsLeftOriented(player) ) {
					GamePlayer_SetBackSHitFly( player );
				} else {
					GamePlayer_SetFrontalSHitFly( player );
				}
			} else {
				if( GamePlayer_IsLeftOriented(player) ) {
					GamePlayer_SetFrontalSHitFly( player );
				} else {
					GamePlayer_SetBackSHitFly( player );
				}
			}
			break;
		case ATTACK_TYPE_SJUMP_KICK:
			/* 如果有 龙卷攻击 技能，且左/右键处于按住状态 */
			if( atk_player->skill.tornado_attack && 
			 ( LCUIKey_IsHit( atk_player->ctrlkey.left)
			  || LCUIKey_IsHit( atk_player->ctrlkey.right)) ) {
				GamePlayer_SetSecondSpinHit( atk_player );
			}
		case ATTACK_TYPE_SJUMP_PUNCH:
			player->n_attack = 0;
			if( GamePlayer_TryHit(player) == 0 ) {
				break;
			}
			if( GamePlayer_IsLeftOriented( atk_player ) ) {
				GamePlayer_SetLeftHitFly( player );
			} else {
				GamePlayer_SetRightHitFly( player );
			}
			break;
		case ATTACK_TYPE_SPIN_HIT:
			if( LCUIKey_IsHit( atk_player->ctrlkey.left)
			 || LCUIKey_IsHit( atk_player->ctrlkey.right) ) {
				GamePlayer_SetSecondSpinHit( atk_player );
			} else {
				/* 减少攻击者百分之50的移动速度 */
				GamePlayer_ReduceSpeed( atk_player, 50 );
			}
		case ATTACK_TYPE_SPIN_HIT2:
		case ATTACK_TYPE_BOMB_KICK:
			if( atk_player->attack_type == ATTACK_TYPE_BOMB_KICK ) {
				GamePlayer_ReduceSpeed( atk_player, 50 );
			}
		case ATTACK_TYPE_BIG_ELBOW:
		case ATTACK_TYPE_GUILLOTINE:
		case ATTACK_TYPE_FINAL_BLOW:
		case ATTACK_TYPE_JUMP_SPIN_KICK:
			if( GamePlayer_TryHit(player) == 0 ) {
				break;
			}
			player->n_attack = 0;
			/* 根据攻击者和受攻击者所面向的方向，得出受攻击者被击飞的方向 */
			if( GamePlayer_IsLeftOriented( atk_player ) ) {
				if( GamePlayer_IsLeftOriented(player) ) {
					GamePlayer_SetBackXHitFly( player );
				} else {
					GamePlayer_SetFrontalXHitFly( player );
				}
			} else {
				if( GamePlayer_IsLeftOriented(player) ) {
					GamePlayer_SetFrontalXHitFly( player );
				} else {
					GamePlayer_SetBackXHitFly( player );
				}
			}
			break;
		case ATTACK_TYPE_MACH_STOMP:
		case ATTACK_TYPE_JUMP_KICK:
		case ATTACK_TYPE_JUMP_PUNCH:
			if( GamePlayer_TryHit(player) == 0 ) {
				break;
			}
			/* 若当前玩家处于歇息状态，则将其击飞 */
			if( player->state == STATE_REST ) {
				player->n_attack = 0;
				if( GamePlayer_IsLeftOriented( atk_player ) ) {
					GamePlayer_SetLeftHitFly( player );
				} else {
					GamePlayer_SetRightHitFly( player );
				}
				break;
			}
		case ATTACK_TYPE_PUNCH:
		case ATTACK_TYPE_KICK:
		default:
			/* 累计该角色受到的攻击的次数 */
			if( player->state == STATE_HIT_FLY
			 || player->state == STATE_B_ROLL
			 || player->state == STATE_F_ROLL ) {
				player->n_attack = 0;
			} else {
				++player->n_attack;
				GamePlayer_SetHit( player );
			}
			break;
		}
		/* 记录本次攻击的信息 */
		Game_RecordAttack(	atk_player, atk_player->attack_type,
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
	player->skill.big_elbow = FALSE;
	player->skill.bomb_kick = FALSE;
	player->skill.guillotine = FALSE;
	player->skill.jump_spin_kick = FALSE;
	player->skill.mach_kick = FALSE;
	player->skill.mach_punch = FALSE;
	player->skill.mach_stomp = FALSE;
	player->skill.rock_defense = FALSE;
	player->skill.spin_hit = FALSE;
	player->skill.tornado_attack = FALSE;
	player->attack_type = 0;
	player->n_attack = 0;
	player->t_rest_timeout = -1;
	player->t_action_timeout = -1;
	player->t_death_timer = -1;
	player->object = NULL;
	player->statusbar = NULL;
	player->other = NULL;
	player->idea.type = idea_type_none;
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

	GamePlayer_Init( &player_data[0] );
	GamePlayer_Init( &player_data[1] );
	/* 记录玩家ID */
	player_data[0].id = 1;
	player_data[1].id = 2;
	player_data[0].enable = TRUE;
	player_data[1].enable = TRUE;

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
	/* 设置1号玩家的控制键 */
	GamePlayer_SetControlKey( 1, &ctrlkey );
	/* 设置1号玩家的角色 */
	GamePlayer_SetRole( 1, ROLE_RIKI );
	/* 设置1号玩家由人来控制 */
	GamePlayer_ControlByHuman( 1, TRUE );

	player_data[0].type = PLAYER_TYPE_MARTIAL_ARTISTS;
	player_data[0].skill.bomb_kick = TRUE;
	player_data[0].skill.jump_spin_kick = TRUE;
	player_data[0].skill.big_elbow = TRUE;
	player_data[0].skill.mach_stomp = TRUE;
	player_data[0].skill.tornado_attack = TRUE;

	player_data[0].property.max_hp = 400;
	player_data[0].property.cur_hp = 400;
	player_data[0].property.defense = 50;
	player_data[0].property.kick = 500;
	player_data[0].property.punch = 500;
	player_data[0].property.throw = 500;
	
	player_data[1].property.max_hp = 8000;
	player_data[1].property.cur_hp = 8000;
	player_data[1].property.defense = 100;
	player_data[1].property.kick = 50;
	player_data[1].property.punch = 50;

	/* 记录2号角色的控制键 */
	ctrlkey.up = LCUIKEY_UP;
	ctrlkey.down = LCUIKEY_DOWN;
	ctrlkey.left = LCUIKEY_LEFT;
	ctrlkey.right = LCUIKEY_RIGHT;
	ctrlkey.jump = 0;
	ctrlkey.a_attack = 0;
	ctrlkey.b_attack = 0;
	/* 设置2号玩家的控制键 */
	GamePlayer_SetControlKey( 2, &ctrlkey );
	/* 设置2号玩家的角色 */
	GamePlayer_SetRole( 2, ROLE_RIKI );
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
	
	Widget_Show( game_scene );
	/* 初始化角色状态信息区域 */
	ret |= Game_InitPlayerStatusArea();
	/* 初始化攻击记录 */
	Game_InitAttackRecord();
	/* 初始化游戏AI */
	GameAI_Init();
	return ret;
}

/** 响应按键的按下 */
static void GameKeyboardProcKeyDown( int key_code )
{
	GamePlayer *target;

	target = GamePlayer_GetPlayerByControlKey( key_code );
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

/** 同步游戏玩家的按键控制 */
static void GamePlayer_SyncKeyControl( GamePlayer *player )
{
	LCUI_BOOL stop_xmotion=FALSE, stop_ymotion=FALSE;

	player->control.left_motion = LCUIKey_IsHit(player->ctrlkey.left);
	player->control.right_motion = LCUIKey_IsHit(player->ctrlkey.right);
	player->control.up_motion = LCUIKey_IsHit(player->ctrlkey.up);
	player->control.down_motion = LCUIKey_IsHit(player->ctrlkey.down);
}

/** 同步游戏玩家的数据 */
static void GamePlayer_SyncData( GamePlayer *player )
{
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
		if( player->state == STATE_SJUMP
		 || player->state == STATE_SSQUAT ) {
			GamePlayer_SetJumpSpinKick( player );
		} else {
			GamePlayer_SetDownMotion( player );
		}
	}
	else {
		switch( player->state ) {
		case STATE_LEFTRUN:
		case STATE_RIGHTRUN:
		case STATE_WALK:
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

	if( player->control.a_attack ) {
		player->control.a_attack = FALSE;
		GamePlayer_StartAAttack( player );
	}
	else if( player->control.b_attack ) {
		player->control.b_attack = FALSE;
		GamePlayer_StartBAttack( player );
	}
	if( player->control.jump ) {
		player->control.jump = FALSE;
		GamePlayer_StartJump( player );
	}
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
	StatusBar_SetPlayerTypeNameW( player_data[1].statusbar, L"2P" );
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
