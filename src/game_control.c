#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_INPUT_H

#include "game.h"

static LCUI_Graph img_shadow;
static GamePlayer player_data[4];

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
	case STATE_AS_ATTACK:
		action_type = ACTION_AS_ATTACK;
		break;
	case STATE_BS_ATTACK:
		action_type = ACTION_BS_ATTACK;
		break;
	case STATE_ASJ_ATTACK:
		action_type = ACTION_AS_ATTACK;
		break;
	case STATE_BSJ_ATTACK:
		action_type = ACTION_BS_ATTACK;
		break;
	case STATE_AJ_ATTACK:
		action_type = ACTION_AJ_ATTACK;
		break;
	case STATE_BJ_ATTACK:
		action_type = ACTION_BJ_ATTACK;
		break;
	case STATE_SQUAT:
		action_type = ACTION_SQUAT;
		break;
	case STATE_JUMP:
		action_type = ACTION_JUMP;
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

static int GamePlayer_InitAction( GamePlayer *player, int id )
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

	action = ActionRes_Load( id, ACTION_AS_ATTACK );
	GameObject_AddAction( player->object, action, ACTION_AS_ATTACK );

	action = ActionRes_Load( id, ACTION_BS_ATTACK );
	GameObject_AddAction( player->object, action, ACTION_BS_ATTACK );
	
	action = ActionRes_Load( id, ACTION_SQUAT );
	GameObject_AddAction( player->object, action, ACTION_SQUAT );
	
	action = ActionRes_Load( id, ACTION_JUMP );
	GameObject_AddAction( player->object, action, ACTION_JUMP );
	
	action = ActionRes_Load( id, ACTION_AJ_ATTACK );
	GameObject_AddAction( player->object, action, ACTION_AJ_ATTACK );
	
	action = ActionRes_Load( id, ACTION_BJ_ATTACK );
	GameObject_AddAction( player->object, action, ACTION_BJ_ATTACK );
	
	action = ActionRes_Load( id, ACTION_ASJ_ATTACK );
	GameObject_AddAction( player->object, action, ACTION_ASJ_ATTACK );
	
	action = ActionRes_Load( id, ACTION_BSJ_ATTACK );
	GameObject_AddAction( player->object, action, ACTION_BSJ_ATTACK );
	//Widget_SetBorder( player->object, Border(1,BORDER_STYLE_SOLID, RGB(0,0,0)) );
	return 0;
}

static void GamePlayer_SetLeftWalk( GamePlayer *player )
{
	double speed;
	if( player->lock_motion ) {
		return;
	}
	speed = -4.0 * player->walk_speed / 100;
	GameObject_SetXSpeed( player->object, speed );
	GamePlayer_ChangeState( player, STATE_WALK );
}

static void GamePlayer_SetRightWalk( GamePlayer *player )
{
	double speed;
	if( player->lock_motion ) {
		return;
	}
	speed = 4.0 * player->walk_speed / 100;
	GameObject_SetXSpeed( player->object, speed );
	GamePlayer_ChangeState( player, STATE_WALK );
}

void GamePlayer_SetLeftRun( GamePlayer *player )
{
	double speed;
	if( player->lock_motion ) {
		return;
	}
	speed = -12.0 * player->walk_speed / 100;
	GameObject_SetXSpeed( player->object, speed );
	GamePlayer_ChangeState( player, STATE_LEFTRUN );
}

void GamePlayer_SetRightRun( GamePlayer *player )
{
	double speed;
	if( player->lock_motion ) {
		return;
	}
	speed = 12.0 * player->walk_speed / 100;
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
	double acc;
	if( player->state == STATE_LEFTRUN ) {
		acc = 1.0 * player->walk_speed / 100;
	}
	else if( player->state == STATE_RIGHTRUN ) {
		acc = -1.0 * player->walk_speed / 100;
	}
	GamePlayer_SetReady( player );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_AtXSpeedToZero( player->object, acc, GamePlayer_AtRunEnd );
}

static void GamePlayer_ProcLeftKey( GamePlayer *player )
{
	if( player->lock_motion ) {
		if( player->state == STATE_JUMP
		 || player->state == STATE_SQUAT ) {
			GameObject_SetHorizFlip( player->object, TRUE );
		}
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
		GameObject_SetHorizFlip( player->object, TRUE );
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
		if( player->state == STATE_JUMP
		 || player->state == STATE_SQUAT ) {
			GameObject_SetHorizFlip( player->object, FALSE );
		}
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
		GameObject_SetHorizFlip( player->object, FALSE );
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
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_SQUAT );
	GamePlayer_LockAction( player );
	GameObject_SetXSpeed( player->object, 0 );
	GameObject_SetYSpeed( player->object, 0 );
	GameObject_AtActionDone( player->object, GamePlayer_AtJumpDone );
}

/** 在起跳时 */
static void GamePlayer_AtSquatDone( LCUI_Widget *widget )
{
	GamePlayer *player;
	double z_speed, z_acc;
	player = GamePlayer_GetPlayerByWidget( widget );
	z_acc = -2.5;
	z_speed = 24.0;
	GamePlayer_UnlockAction( player );
	GamePlayer_ChangeState( player, STATE_JUMP );
	GameObject_AtLanding( widget, z_speed, z_acc, GamePlayer_AtLandingDone );
	/* 撤销响应，以避免本函数被重复调用 */
	GameObject_AtActionDone( widget, NULL );
}

void GamePlayer_StartJump( GamePlayer *player )
{
	if( player->state == STATE_JUMP ) {
		return;
	}
	switch(player->state) {
	case STATE_JUMP:return;
	case STATE_AS_ATTACK:
	case STATE_BS_ATTACK:break;
	default:
		if( player->lock_action ) {
			return;
		}
		break;
	}
	GamePlayer_UnlockAction( player );
	/* 跳跃后，重置X轴上的加速度为0 */
	GameObject_SetXAcc( player->object, 0 );
	GamePlayer_ChangeState( player, STATE_SQUAT );
	GamePlayer_LockAction( player );
	GamePlayer_LockMotion( player );
	GameObject_AtActionDone( player->object, GamePlayer_AtSquatDone );
}

void GamePlayer_StartAAttack( GamePlayer *player )
{
	double acc;
	if( player->lock_action ) {
		return;
	}
	if( player->state == STATE_LEFTRUN ) {
		acc = 0.5 * player->walk_speed / 100;
	}
	else if( player->state == STATE_RIGHTRUN ) {
		acc = -0.5 * player->walk_speed / 100;
	} else {
		acc = 0.0;
	}

	switch(player->state) {
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
		GamePlayer_ChangeState( player, STATE_AS_ATTACK );
		GamePlayer_LockAction( player );
		GamePlayer_LockMotion( player );
		GameObject_AtXSpeedToZero( player->object, acc, GamePlayer_AtAttackDone );
		break;
	case STATE_JUMP:
		GamePlayer_ChangeState( player, STATE_AJ_ATTACK );
		GamePlayer_LockAction( player );
		break;
	default:
		GamePlayer_ChangeState( player, STATE_A_ATTACK );
		GamePlayer_StopXWalk( player );
		GamePlayer_StopYMotion( player );
		GamePlayer_LockMotion( player );
		GamePlayer_LockAction( player );
		GameObject_AtActionDone( player->object, GamePlayer_AtAttackDone );
		break;
	}
}

void GamePlayer_StartBAttack( GamePlayer *player )
{
	double acc;
	if( player->lock_action ) {
		return;
	}
	if( player->state == STATE_LEFTRUN ) {
		acc = 0.5 * player->walk_speed / 100;
	}
	else if( player->state == STATE_RIGHTRUN ) {
		acc = -0.5 * player->walk_speed / 100;
	} else {
		acc = 0.0;
	}

	switch(player->state) {
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
		GamePlayer_ChangeState( player, STATE_BS_ATTACK );
		GamePlayer_LockAction( player );
		GamePlayer_LockMotion( player );
		GameObject_AtXSpeedToZero( player->object, acc, GamePlayer_AtAttackDone );
		break;
	case STATE_JUMP:
		GamePlayer_ChangeState( player, STATE_BJ_ATTACK );
		GamePlayer_LockAction( player );
		break;
	default:
		GamePlayer_ChangeState( player, STATE_B_ATTACK );
		GamePlayer_StopXWalk( player );
		GamePlayer_StopYMotion( player );
		GamePlayer_LockMotion( player );
		GamePlayer_LockAction( player );
		GameObject_AtActionDone( player->object, GamePlayer_AtAttackDone );
		break;
	}
}

void GamePlayer_SetUpMotion( GamePlayer *player )
{
	double speed;
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
		speed = -2.0 * player->walk_speed / 100;
		GameObject_SetYSpeed( player->object, speed );
		break;
	default:break;
	}
}

void GamePlayer_SetDownMotion( GamePlayer *player )
{
	double speed;
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
		speed = 2.0 * player->walk_speed / 100;
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
	else if( key_code == target->ctrlkey.jump ) {
		GamePlayer_StartJump( target );
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
	player->run_speed = 100;
	player->walk_speed = 100;
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

int Game_Init(void)
{
	int ret;
	ControlKey ctrlkey;

	/* 注册GameObject部件 */
	GameObject_Register();
	/* 记录玩家ID */
	player_data[0].id = 1;
	player_data[1].id = 2;
	player_data[2].id = 3;
	player_data[3].id = 4;
	player_data[0].enable = TRUE;
	player_data[1].enable = TRUE;
	player_data[2].enable = FALSE;
	player_data[3].enable = FALSE;
	player_data[0].local_control = TRUE;
	player_data[1].local_control = TRUE;
	player_data[2].local_control = FALSE;
	player_data[3].local_control = FALSE;

	Graph_Init( &img_shadow );
	ret = Graph_LoadImage("drawable/shadow.png", &img_shadow );

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

	/* 响应按键输入 */
	ret |= LCUI_KeyboardEvent_Connect( GameKeyboardProc, NULL );
	ret |= GameMsgLoopStart();
	return ret;
}

static void GamePlayer_SyncData( GamePlayer *player )
{
	LCUI_BOOL stop_xmotion=FALSE, stop_ymotion=FALSE;

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
		for(i=0; i<4; ++i) {
			if( !player_data[i].enable
			 || !player_data[i].local_control ) {
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
	int i;
	LCUI_Thread t;
	LCUIThread_Create( &t, GamePlayer_Control, NULL );
	/* 移动游戏角色的位置 */
	GameObject_SetPos( player_data[0].object, 200, 300 );
	GameObject_SetPos( player_data[1].object, 400, 300 );
	GameObject_SetHorizFlip( player_data[0].object, FALSE );
	GameObject_SetHorizFlip( player_data[1].object, TRUE );
	for(i=0; i<4; ++i) {
		if( !player_data[i].enable ) {
			continue;
		}
		GameObject_PlayAction( player_data[i].object );
		Widget_Show( player_data[i].object );
	}
	return 0;
}

int Game_Pause(void)
{
	return 0;
}
