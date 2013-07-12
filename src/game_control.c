#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_INPUT_H

#include "game.h"

static GamePlayer player_data[4];

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
	/* 若动作不一致 */
	if( current_action != action_id ) {
		msg.player_id = player->id;
		msg.msg.msg_id = GAMEMSG_ACTION;
		msg.msg.action.action_id = action_id;
		Game_PostMsg( &msg );
	}
}

void GamePlayer_ChangeState( GamePlayer *player, int state )
{
	int action_type; 

	if( player->lock_action ) {
		return;
	}
	switch(state) {
	case STATE_READY:
		action_type = ACTION_READY;
		break;
	case STATE_STANCE: 
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
	default:return;
	}
	player->state = state;
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

void GamePlayer_SetXSpeedToZero(	GamePlayer *player,
					int acc,
					void(*func)(GamePlayer*) )
{
	player->func[AT_XSPEED_TO_ZERO] = func;
	GameObject_SetXAcc( player->object, acc );
}

int GamePlayer_InitAction( GamePlayer *player, int id )
{
	ActionData* action;

	player->state = STATE_STANCE;
	/* 创建GameObject部件 */
	player->object = GameObject_New();
	/* 载入游戏角色资源 */
	action = ActionRes_Load( id, ACTION_STANCE );
	/* 将动作集添加至游戏对象 */
	GameObject_AddAction( player->object, action, ACTION_STANCE );
	
	action = ActionRes_Load( id, ACTION_WALK );
	GameObject_AddAction( player->object, action, ACTION_WALK );
	
	action = ActionRes_Load( id, ACTION_RUN );
	GameObject_AddAction( player->object, action, ACTION_RUN );
	
	action = ActionRes_Load( id, ACTION_READY );
	GameObject_AddAction( player->object, action, ACTION_READY );

	action = ActionRes_Load( id, ACTION_A_ATTACK );
	GameObject_AddAction( player->object, action, ACTION_A_ATTACK );

	action = ActionRes_Load( id, ACTION_B_ATTACK );
	GameObject_AddAction( player->object, action, ACTION_B_ATTACK );
	//Widget_SetBorder( player->game_object, Border(1,BORDER_STYLE_SOLID, RGB(0,0,0)) );
	Widget_Show( player->object );
	return 0;
}

static int GamePlayer_Init( GamePlayer *player )
{
	int ret;
	/* 设置基本属性 */
	player->walk_speed = 100;
	player->run_speed = 100;
	player->human_control = TRUE;
	player->local_control = TRUE;
	player->right_direction = TRUE;
	player->lock_action = FALSE;
	player->lock_motion = FALSE;
	ControlKey_Init( &player->ctrlkey );
	player->func[0] = NULL;
	player->func[1] = NULL;
	/* 初始化角色动作动画 */
	ret = GamePlayer_InitAction( player, ROLE_RIKI );
	if( ret != 0 ) {
		return -1;
	}
	return 0;
}

static void GamePlayer_SetLeftWalk( GamePlayer *player )
{
	int speed;
	if( player->lock_motion ) {
		return;
	}
	speed = -3 * player->walk_speed / 100;
	GameObject_SetXSpeed( player->object, speed );
	GamePlayer_ChangeState( player, STATE_WALK );
}

static void GamePlayer_SetRightWalk( GamePlayer *player )
{
	int speed;
	if( player->lock_motion ) {
		return;
	}
	speed = 3 * player->walk_speed / 100;
	GameObject_SetXSpeed( player->object, speed );
	GamePlayer_ChangeState( player, STATE_WALK );
}

void GamePlayer_SetLeftRun( GamePlayer *player )
{
	int speed;
	if( player->lock_motion ) {
		return;
	}
	speed = -10 * player->walk_speed / 100;
	GameObject_SetXSpeed( player->object, speed );
	GamePlayer_ChangeState( player, STATE_LEFTRUN );
}

void GamePlayer_SetRightRun( GamePlayer *player )
{
	int speed;
	if( player->lock_motion ) {
		return;
	}
	speed = 10 * player->walk_speed / 100;
	GameObject_SetXSpeed( player->object, speed );
	GamePlayer_ChangeState( player, STATE_RIGHTRUN );
}

static void GamePlayer_AtRunEnd( GamePlayer *player )
{
	GamePlayer_UnlockAction( player );
	GamePlayer_UnlockMotion( player );
}

static GamePlayer *GamePlayer_GetPlayerByWidget( LCUI_Widget *widget )
{
	int i;
	for(i=0; i<4; ++i) {
		if( widget == player_data[i].object ) {
			return 	&player_data[i];
		}
	}
	return NULL;
}

static void GamePlayer_AtReadyDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	/* 撤销响应动作结束信号 */
	GameObject_AtActionDone( player->object, NULL );
	GamePlayer_ChangeState( player, STATE_STANCE );
}

void GamePlayer_SetReady( GamePlayer *player )
{
	GamePlayer_ChangeState( player, STATE_READY );
	/* 设置响应动作结束信号 */
	GameObject_AtActionDone( player->object, GamePlayer_AtReadyDone );
}

/** 停止奔跑 */
void GamePlayer_StopRun( GamePlayer *player )
{
	int acc;
	if( player->state == STATE_LEFTRUN ) {
		acc = 1 * player->walk_speed / 100;
	} else {
		acc = -1 * player->walk_speed / 100;
	}
	GamePlayer_SetReady( player );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GamePlayer_SetXSpeedToZero( player, acc, GamePlayer_AtRunEnd );
}

static void GamePlayer_ProcLeftKey( GamePlayer *player )
{
	if( player->lock_motion ) {
		return;
	}
	switch(player->state) {
	case STATE_READY:
		GameObject_AtActionDone( player->object, NULL );
	case STATE_STANCE:
	case STATE_WALK:
		if( LCUIKey_IsDoubleHit(player->ctrlkey.left,250) ) {
			 GamePlayer_SetLeftRun( player );
		} else {
			 GamePlayer_SetLeftWalk( player );
		}
	case STATE_LEFTRUN:
		break;
	case STATE_RIGHTRUN:
		GamePlayer_StopRun( player );
		break;
	default:break;
	}
}

static void GamePlayer_ProcRightKey( GamePlayer *player )
{
	if( player->lock_motion ) {
		return;
	}
	switch(player->state) {
	case STATE_READY:
		GameObject_AtActionDone( player->object, NULL );
	case STATE_STANCE:
	case STATE_WALK:
		if( LCUIKey_IsDoubleHit(player->ctrlkey.right,250) ) {
			 GamePlayer_SetRightRun( player );
		} else {
			 GamePlayer_SetRightWalk( player );
		}
	case STATE_RIGHTRUN:
		break;
	case STATE_LEFTRUN:
		GamePlayer_StopRun( player );
		break;
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
		return;
	default:
		GameObject_SetXSpeed( player->object, 0 );
		break;
	}
}

void GamePlayer_StopYMotion( GamePlayer *player )
{
	if( player->lock_motion ) {
		return;
	}
	GameObject_SetYSpeed( player->object, 0 );
}

static void GamePlayer_AtAttackDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	player = GamePlayer_GetPlayerByWidget( widget );
	GamePlayer_UnlockMotion( player );
	GamePlayer_UnlockAction( player );
	GamePlayer_SetReady( player );
}

void GamePlayer_StartAAttack( GamePlayer *player )
{
	if( player->lock_action ) {
		return;
	}
	GamePlayer_ChangeState( player, STATE_A_ATTACK );
	GamePlayer_StopXWalk( player );
	GamePlayer_StopYMotion( player );
	GamePlayer_LockMotion( player );
	GamePlayer_LockAction( player );
	GameObject_AtActionDone( player->object, GamePlayer_AtAttackDone );
}

void GamePlayer_StartBAttack( GamePlayer *player )
{
	if( player->lock_action ) {
		return;
	}
	GamePlayer_ChangeState( player, STATE_B_ATTACK );
	GamePlayer_StopXWalk( player );
	GamePlayer_StopYMotion( player );
	GamePlayer_LockMotion( player );
	GamePlayer_LockAction( player );
	GameObject_AtActionDone( player->object, GamePlayer_AtAttackDone );
}

void GamePlayer_SetUpMotion( GamePlayer *player )
{
	int speed;
	if( player->lock_motion ) {
		return;
	}
	switch(player->state) {
	case STATE_READY:
		GameObject_AtActionDone( player->object, NULL );
	case STATE_STANCE:
		GamePlayer_ChangeState( player, STATE_WALK );
	case STATE_WALK:
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
		speed = -3 * player->walk_speed / 100;
		GameObject_SetYSpeed( player->object, speed );
		break;
	default:break;
	}
}

void GamePlayer_SetDownMotion( GamePlayer *player )
{
	int speed;
	if( player->lock_motion ) {
		return;
	}
	switch(player->state) {
	case STATE_READY:
		GameObject_AtActionDone( player->object, NULL );
	case STATE_STANCE:
		GamePlayer_ChangeState( player, STATE_WALK );
	case STATE_WALK:
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
		speed = 3 * player->walk_speed / 100;
		GameObject_SetYSpeed( player->object, speed );
		break;
	default:break;
	}
}

static void GameKeyboardProcKeyDown( int key_code )
{
	GamePlayer *target;

	target = GamePlayer_GetPlayerByControlKey( key_code );
	
	if( key_code == target->ctrlkey.a_attack ) {
		GamePlayer_StartAAttack( target );
	}
	else if( key_code == target->ctrlkey.b_attack ) {
		GamePlayer_StartBAttack( target );
	}
	
	Widget_Update( target->object );
}

static void GameKeyboardProcKeyUp( int key_code )
{
	GamePlayer *target;
	
	target = GamePlayer_GetPlayerByControlKey( key_code );
	
	if( key_code == target->ctrlkey.left
	 || key_code == target->ctrlkey.right ) {
		GamePlayer_StopXWalk( target );
	}
	else if( key_code == target->ctrlkey.up
	 || key_code == target->ctrlkey.down ) {
		GamePlayer_StopYMotion( target );
	}
	
	Widget_Update( target->object );
}

static void GameKeyboardProc( LCUI_KeyboardEvent *event, void *arg )
{
	if( event->type == LCUI_KEYDOWN ) {
		GameKeyboardProcKeyDown( event->key_code );
	} else {
		//GameKeyboardProcKeyUp( event->key_code );
	}
}


int Game_Init(void)
{
	int ret;
	/* 注册GameObject部件 */
	GameObject_Register();
	/* 记录玩家ID */
	player_data[0].id = 1;
	player_data[1].id = 2;
	player_data[2].id = 3;
	player_data[3].id = 4;
	ret = GamePlayer_Init( &player_data[0] );
	/* 设置该角色的控制键 */
	player_data[0].ctrlkey.up = LCUIKEY_W;
	player_data[0].ctrlkey.down = LCUIKEY_S;
	player_data[0].ctrlkey.left = LCUIKEY_A;
	player_data[0].ctrlkey.right = LCUIKEY_D;
	player_data[0].ctrlkey.jump = LCUIKEY_SPACE;
	player_data[0].ctrlkey.a_attack = LCUIKEY_J;
	player_data[0].ctrlkey.b_attack = LCUIKEY_K;
	/* 响应按键输入 */
	ret |= LCUI_KeyboardEvent_Connect( GameKeyboardProc, NULL );
	ret |= GameMsgLoopStart();
	return ret;
}

static void GamePlayer_SyncData( GamePlayer *player )
{
	LCUI_BOOL stop_xmotion=FALSE, stop_ymotion=FALSE;
	int x_acc, x_speed;

	x_acc = GameObject_GetXAcc( player->object );
	x_speed = GameObject_GetXSpeed( player->object );
	/* 若速度接近0 */
	if( (x_acc > 0 && x_speed >= 0 )
	|| (x_acc < 0 && x_speed <= 0 ) ) {
		GameObject_SetXAcc( player->object, 0 );
		GameObject_SetXSpeed( player->object, 0 );
		if( player->func[AT_XSPEED_TO_ZERO] ) {
			player->func[AT_XSPEED_TO_ZERO]( player );
		}
	}
	if( LCUIKey_IsHit(player->ctrlkey.left) ) {
		player->right_direction = FALSE;
	}
	if( LCUIKey_IsHit(player->ctrlkey.right) ) {
		player->right_direction = TRUE;
	}
	if( LCUIKey_IsHit(player->ctrlkey.left) ) {
		GamePlayer_ProcLeftKey( player );
	}
	else if( LCUIKey_IsHit(player->ctrlkey.right) ) {
		GamePlayer_ProcRightKey( player );
	}
	else {
		GamePlayer_StopXWalk( player );
		stop_xmotion = TRUE;
	}
	if( LCUIKey_IsHit(player->ctrlkey.up) ) {
		GamePlayer_SetUpMotion( player );
	}
	else if( LCUIKey_IsHit(player->ctrlkey.down) ) {
		GamePlayer_SetDownMotion( player );
	}
	else {
		GamePlayer_StopYMotion( player );
		stop_ymotion = TRUE;
	}
	if( stop_xmotion && stop_ymotion ) {
		if( player->state == STATE_WALK ) {
			GamePlayer_ChangeState( player, STATE_STANCE );
		}
	}
}

static void GamePlayer_Control( void *arg )
{
	int i;
	while(1) {
		/* 总玩家数为4个，由于还处于开发阶段，先设为1 */
		for(i=0; i<1; ++i) {
			if( !player_data[i].local_control ) {
				continue;
			}
			GamePlayer_SyncData( &player_data[i] );
			Widget_Update( player_data[i].object );
		}
		LCUI_MSleep( 10 );
	}
}

int Game_Start(void)
{
	LCUI_Thread t;
	LCUIThread_Create( &t, GamePlayer_Control, NULL );
	GameObject_PlayAction( player_data[0].object );
	/* 移动游戏角色的位置 */
	GameObject_SetPos( player_data[0].object, 300, 300 );
	return 0;
}

int Game_Pause(void)
{
	return 0;
}
