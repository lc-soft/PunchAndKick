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

	switch(action_id) {
	case ACTION_RUN:
	case ACTION_STANCE:
	default:
		break;
	}
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
	switch(state) {
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
	ControlKey_Init( &player->ctrlkey );
	/* 初始化角色动作动画 */
	ret = GamePlayer_InitAction( player, ROLE_RIKI );
	if( ret != 0 ) {
		return -1;
	}
	return 0;
}

/** 停止奔跑 */
void GamePlayer_StopRun( GamePlayer *player )
{
	GameObject_SetXSpeed( player->object, 0 );
	if( LCUIKey_IsHit(player->ctrlkey.up)
	 || LCUIKey_IsHit(player->ctrlkey.down) ) {
		 GamePlayer_ChangeState( player, STATE_WALK );
	} else {
		 GamePlayer_ChangeState( player, STATE_STANCE );
	}
}

static void GamePlayer_ProcLeftKeyDown( GamePlayer *player )
{
	int speed;
	switch(player->state) {
	case STATE_STANCE:
	case STATE_WALK:
		if( LCUIKey_IsDoubleHit(player->ctrlkey.left,250) ) {
			speed = -10 * player->walk_speed / 100;
			GameObject_SetXSpeed( player->object, speed );
			GamePlayer_ChangeState( player, STATE_LEFTRUN );
		} else {
			speed = -3 * player->walk_speed / 100;
			GameObject_SetXSpeed( player->object, speed );
			GamePlayer_ChangeState( player, STATE_WALK );
		}
	case STATE_LEFTRUN:
		break;
	case STATE_RIGHTRUN:
		_DEBUG_MSG("STATE_RIGHTRUN\n");
		GamePlayer_StopRun( player );
		break;
	default:break;
	}
}

static void GamePlayer_ProcRightKeyDown( GamePlayer *player )
{
	int speed;
	switch(player->state) {
	case STATE_STANCE:
	case STATE_WALK:
		if( LCUIKey_IsDoubleHit(player->ctrlkey.right,250) ) {
			speed = 10 * player->walk_speed / 100;
			GameObject_SetXSpeed( player->object, speed );
			GamePlayer_ChangeState( player, STATE_RIGHTRUN );
		} else {
			speed = 3 * player->walk_speed / 100;
			GameObject_SetXSpeed( player->object, speed );
			GamePlayer_ChangeState( player, STATE_WALK );
		}
	case STATE_RIGHTRUN:
		break;
	case STATE_LEFTRUN:
		GamePlayer_StopRun( player );
		break;
	default:break;
	}
}

static void GamePlayer_ProcUpKeyDown( GamePlayer *player )
{
	int speed;

	switch(player->state) {
	case STATE_STANCE:
		GamePlayer_ChangeAction( player, ACTION_WALK );
	case STATE_WALK:
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
		speed = -3 * player->walk_speed / 100;
		GameObject_SetYSpeed( player->object, speed );
		break;
	default:break;
	}
}
static void GamePlayer_StopWalk( GamePlayer *player )
{
	if( player->state != STATE_WALK ) {
		return;
	}
	if( LCUIKey_IsHit(player->ctrlkey.right)
	|| LCUIKey_IsHit(player->ctrlkey.left)
	|| LCUIKey_IsHit(player->ctrlkey.up)
	|| LCUIKey_IsHit(player->ctrlkey.down) ) {
		return;
	}
	GameObject_SetXSpeed( player->object, 0 );
	GamePlayer_ChangeAction( player, ACTION_STANCE );
}

static void GamePlayer_StopYMove( GamePlayer *player )
{
	GameObject_SetYSpeed( player->object, 0 );
	if( LCUIKey_IsHit(player->ctrlkey.left)
	 || LCUIKey_IsHit(player->ctrlkey.right) ) {
		if( player->state != STATE_LEFTRUN
		 && player->state != STATE_RIGHTRUN ) {
			GamePlayer_ChangeState( player, STATE_WALK );
		}
	} else {
		 GamePlayer_ChangeState( player, STATE_STANCE );
	}
}

static void GamePlayer_ProcDownKeyDown( GamePlayer *player )
{
	int speed;
	switch(player->state) {
	case STATE_STANCE:
		GamePlayer_ChangeAction( player, ACTION_WALK );
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
	
	if( key_code == target->ctrlkey.left ) {
		GamePlayer_ProcLeftKeyDown( target );
	}
	else if( key_code == target->ctrlkey.right ) {
		GamePlayer_ProcRightKeyDown( target );
	}
	else if( key_code == target->ctrlkey.up ) {
		GamePlayer_ProcUpKeyDown( target );
	}
	else if( key_code == target->ctrlkey.down ) {
		GamePlayer_ProcDownKeyDown( target );
	}
	
	Widget_Update( target->object );
}

static void GameKeyboardProcKeyUp( int key_code )
{
	GamePlayer *target;
	
	target = GamePlayer_GetPlayerByControlKey( key_code );
	
	if( key_code == target->ctrlkey.left
	 || key_code == target->ctrlkey.right ) {
		GamePlayer_StopWalk( target );
	}
	else if( key_code == target->ctrlkey.up
	 || key_code == target->ctrlkey.down ) {
		GamePlayer_StopYMove( target );
	}
	
	Widget_Update( target->object );
}

static void GameKeyboardProc( LCUI_KeyboardEvent *event, void *arg )
{
	if( event->type == LCUI_KEYDOWN ) {
		GameKeyboardProcKeyDown( event->key_code );
	} else {
		GameKeyboardProcKeyUp( event->key_code );
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
	LCUI_BOOL	can_x_move=FALSE, can_y_move=FALSE,
			can_attack=FALSE, can_jump=FALSE;
	switch(player->state) {
	case STATE_STANCE:
	case STATE_WALK:
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
		can_x_move = TRUE;
		can_y_move = TRUE;
		can_jump = TRUE;
		can_attack = TRUE;
	default:break;
	}
	if( can_y_move ) {
		if( LCUIKey_IsHit(player->ctrlkey.up) ) {
			GameObject_SetYSpeed(
				player->object,
				-3*player->walk_speed/100
			);
			if( player->state != STATE_LEFTRUN
			&& player->state != STATE_RIGHTRUN ) {
				player->state = STATE_WALK;
			}
		}
		else if( LCUIKey_IsHit(player->ctrlkey.down) ) {
			GameObject_SetYSpeed(
				player->object,
				3*player->walk_speed/100
			);
			if( player->state != STATE_LEFTRUN
			&& player->state != STATE_RIGHTRUN ) {
				player->state = STATE_WALK;
			}
		} else {
			GameObject_SetYSpeed( player->object, 0 );
			if( player->state != STATE_LEFTRUN
			&& player->state != STATE_RIGHTRUN ) {
				player->state = STATE_STANCE;
			}
		}
	}
	if( can_x_move ) {
		/* 若在250毫秒内按了两次左键 */
		if( LCUIKey_IsDoubleHit(player->ctrlkey.left, 250) ) {
			player->state = STATE_LEFTRUN;
			GameObject_SetXSpeed(
				player->object,
				-10*player->walk_speed/100
			);
		} else if( LCUIKey_IsDoubleHit(player->ctrlkey.right, 250) ) {
			player->state = STATE_RIGHTRUN;
			GameObject_SetXSpeed(
				player->object,
				10*player->walk_speed/100
			);
		}
		if( LCUIKey_IsHit(player->ctrlkey.left)
		 && player->state != STATE_LEFTRUN ) {
			GameObject_SetXSpeed(
				player->object,
				-3*player->walk_speed/100
			);
			player->state = STATE_WALK;
		} else if( LCUIKey_IsHit(player->ctrlkey.right)
			 && player->state != STATE_RIGHTRUN ) {
			GameObject_SetXSpeed(
				player->object,
				3*player->walk_speed/100 
			);
			player->state = STATE_WALK;
		} else {
			if( player->state != STATE_LEFTRUN
			&& player->state != STATE_RIGHTRUN ) {
				GameObject_SetXSpeed( player->object, 0 );
				if( player->state != STATE_WALK ) {
					player->state = STATE_STANCE;
				}
			}
		}
	}
	Widget_Update( player->object );
}

static void GamePlayer_ProcState( GamePlayer *player )
{
	if( LCUIKey_IsHit(player->ctrlkey.left) ) {
		player->right_direction = FALSE;
	}
	if( LCUIKey_IsHit(player->ctrlkey.right) ) {
		player->right_direction = TRUE;
	}
	switch( player->state ) {
	case STATE_STANCE:
		GamePlayer_ChangeAction( player, ACTION_STANCE );
		break;
	case STATE_WALK:
		GamePlayer_ChangeAction( player, ACTION_WALK );
		break;
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
		GamePlayer_ChangeAction( player, ACTION_RUN );
	default:
		break;
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
			//GamePlayer_SyncData( &player_data[i] );
			//GamePlayer_ProcState( &player_data[i] );
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
