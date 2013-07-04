#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_ACTIVEBOX_H

#include "game_object.h"

enum PlayerState {
	STATE_STANCE,
	STATE_MOVE
};

typedef struct GamePlayer_ {
	int state;		/**< 玩家当前状态 */
	int hp, mp;		/**< 生命值和法力值 */
	LCUI_Widget *object;	/**< 游戏对象 */
} GamePlayer;

GamePlayer player;

/** 载入角色的移动动作动画资源 */
static ActionData* ActionRes_LoadMove(void)
{
	int i;
	ActionData *action;
	LCUI_Graph img_move[8];
	char path[512];

	action = Action_Create();
	for(i=0; i<8; ++i) {
		Graph_Init( &img_move[i] );
		sprintf( path, "drawable/move-%02d.png", i+1 );
		Graph_LoadImage( path, &img_move[i] );
		Action_AddFrame( action, 0,0, &img_move[i], 100 );
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
		Action_AddFrame( action, 3,0, &img_stance[i], 100 );
	}
	return action;
}

/** 载入角色的动作动画资源 */
ActionData* LoadGamePlayerRes( int action_type )
{
	switch( action_type ) {
	case STATE_STANCE: return ActionRes_LoadStance();
	case STATE_MOVE: return ActionRes_LoadMove();
	}
	return NULL;
}

int Game_Init(void)
{
	ActionData* action;
	/* 注册GameObject部件 */
	GameObject_Register();

	player.state = STATE_STANCE;
	player.hp = 100;
	player.mp = 0;
	/* 创建GameObject部件 */
	player.object = GameObject_New();
	/* 载入游戏角色资源 */
	action = LoadGamePlayerRes( player.state );
	/* 将动作集添加至游戏对象 */
	GameObject_AddAction( player.object, action );
	/* 移动游戏角色的位置 */
	GameObject_Move( player.object, 300, 300 );
	Widget_Show( player.object );
	return 0;
}

int Game_Start(void)
{
	GameObject_PlayAction( player.object );
	return 0;
}

int Game_Pause(void)
{
	return 0;
}
