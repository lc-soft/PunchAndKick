#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_ACTIVEBOX_H
#include LC_INPUT_H

#include "game.h"

static GamePlayer player_data[4];

/** 载入角色的移动动作动画资源 */
static ActionData* ActionRes_LoadWalk(void)
{
	int i;
	ActionData *action;
	LCUI_Graph img_move[8];
	char path[512];

	action = Action_Create();
	for(i=0; i<8; ++i) {
		Graph_Init( &img_move[i] );
		sprintf( path, "drawable/walk-%02d.png", i+1 );
		Graph_LoadImage( path, &img_move[i] );
		Action_AddFrame( action, -6,0, &img_move[i], 100 );
	}
	return action;
}

/** 载入角色的站立动作动画资源 */
static ActionData* ActionRes_LoadStance(void)
{
	int i;
	ActionData *action;
	LCUI_Graph img_stance[4];
	char path[512];
	
	action = Action_Create();
	for(i=0; i<4; ++i) {
		Graph_Init( &img_stance[i] );
		sprintf( path, "drawable/stance-%02d.png", i+1 );
		Graph_LoadImage( path, &img_stance[i] );
		Action_AddFrame( action, -5,0, &img_stance[i], 100 );
	}
	return action;
}

/** 载入角色的动作动画资源 */
ActionData* LoadGamePlayerRes( int action_type )
{
	switch( action_type ) {
	case STATE_STANCE: return ActionRes_LoadStance();
	case STATE_WALK: return ActionRes_LoadWalk();
	}
	return NULL;
}

static int GamePlayer_InitAction( GamePlayer *player )
{
	ActionData* action;

	player->state = STATE_STANCE;
	/* 创建GameObject部件 */
	player->game_object = GameObject_New();
	/* 载入游戏角色资源 */
	action = LoadGamePlayerRes( STATE_STANCE );
	/* 将动作集添加至游戏对象 */
	GameObject_AddAction( player->game_object, action, STATE_STANCE );
	
	action = LoadGamePlayerRes( STATE_WALK );
	GameObject_AddAction( player->game_object, action, STATE_WALK );

	//Widget_SetBorder( player->game_object, Border(1,BORDER_STYLE_SOLID, RGB(0,0,0)) );
	Widget_Show( player->game_object );
	return 0;
}

static int GamePlayer_Init( GamePlayer *player )
{
	int ret;
	PhysicsObject *obj;
	/* 设置基本属性 */
	player->walk_speed = 100;
	player->run_speed = 100;
	player->human_control = TRUE;
	player->local_control = TRUE;
	player->right_direction = TRUE;
	/* 初始化角色动作动画 */
	ret = GamePlayer_InitAction( player );
	if( ret != 0 ) {
		return -1;
	}
	/* 创建一个对应的物理对象 */
	obj = PhysicsObject_New( 0,0,0,0,0,0 );
	if( obj == NULL ) {
		return -2;
	}
	player->phys_object = obj;
	return 0;
}


int Game_Init(void)
{
	int ret;
	/* 注册GameObject部件 */
	GameObject_Register();
	/* 启动物理系统 */
	PhysicsSystem_Start();
	/* 记录玩家ID */
	player_data[0].id = 1;
	player_data[1].id = 2;
	player_data[2].id = 3;
	player_data[3].id = 4;
	ret = GamePlayer_Init( &player_data[0] );
	/* 响应按键输入 */
	ret |= LCUI_KeyboardEvent_Connect( GameKeyboardProc, NULL );
	ret |= GameMsgLoopStart();
	return ret;
}

static void GamePlayer_SetPos( GamePlayer *player, int x, int y )
{
	player->phys_object->x = x;
	player->phys_object->y = y;
	GameObject_Move( player->game_object, x, y );
}

static void GamePlayer_SyncData( GamePlayer *player )
{
	if( LCUIKey_IsHit(LCUIKEY_A) ) {
		player->phys_object->x_speed = -2*player->walk_speed/100;
	} else if( LCUIKey_IsHit(LCUIKEY_D) ) {
		player->phys_object->x_speed = 2*player->walk_speed/100;
	} else {
		player->phys_object->x_speed = 0;
	}
	if( LCUIKey_IsHit(LCUIKEY_W) ) {
		player->phys_object->y_speed = -2*player->walk_speed/100;
	} else if( LCUIKey_IsHit(LCUIKEY_S) ) {
		player->phys_object->y_speed = 2*player->walk_speed/100;
	} else {
		player->phys_object->y_speed = 0;
	}
	GameObject_Move(
		player->game_object,
		player->phys_object->x,
		player->phys_object->y
	);
}

static void GamePlayer_ProcState( GamePlayer *player, int state )
{
	if( LCUIKey_IsHit(LCUIKEY_A) ) {
		player->right_direction = FALSE;
	}
	if( LCUIKey_IsHit(LCUIKEY_D) ) {
		player->right_direction = TRUE;
	}
	if( LCUIKey_IsHit(LCUIKEY_A)
	 || LCUIKey_IsHit(LCUIKEY_D)
	 || LCUIKey_IsHit(LCUIKEY_W)
	 || LCUIKey_IsHit(LCUIKEY_S) ) {
		 GamePlayer_ChangeState( player, STATE_WALK );
	} else {
		 GamePlayer_ChangeState( player, STATE_STANCE );
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
			GamePlayer_ProcState( &player_data[i], player_data[i].state );
			GamePlayer_SyncData( &player_data[i] );
		}
		LCUI_MSleep( 10 );
	}
}

int Game_Start(void)
{
	LCUI_Thread t;
	LCUIThread_Create( &t, GamePlayer_Control, NULL );
	GameObject_PlayAction( player_data[0].game_object );
	/* 移动游戏角色的位置 */
	GamePlayer_SetPos( &player_data[0], 300, 300 );
	//GameObject_PlayAction( player_data[1].object );
	//GameObject_PlayAction( player_data[2].object );
	//GameObject_PlayAction( player_data[3].object );
	return 0;
}

int Game_Pause(void)
{
	return 0;
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

/** 改变角色的状态  */
void GamePlayer_ChangeState( GamePlayer *player, int state )
{
	GameMsg msg;

	switch(state) {
	case STATE_RUN:
	case STATE_STANCE:
	default:
		break;
	}
	/* 若状态不一致 */
	if( player->state != state ) {
		msg.player_id = player->id;
		msg.msg.msg_id = GAMEMSG_STATE;
		msg.msg.state.state = state;
		Game_PostMsg( &msg );
		player->state = state;
	}
}
