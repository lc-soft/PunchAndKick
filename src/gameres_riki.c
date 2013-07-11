#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_GRAPH_H

#include "game.h"

/** 载入角色的移动动作动画资源 */
static ActionData* ActionRes_LoadWalk(void)
{
	ActionData *action;
	LCUI_Graph img_move[4];

	action = Action_Create();
	Graph_Init( &img_move[0] );
	Graph_LoadImage( "drawable/walk-01.png", &img_move[0] );
	Action_AddFrame( action, 0,2, &img_move[0], 5 );
	Graph_Init( &img_move[1] );
	Graph_LoadImage( "drawable/walk-02.png", &img_move[1] );
	Action_AddFrame( action, 0,0, &img_move[1], 5 );
	Graph_Init( &img_move[2] );
	Graph_LoadImage( "drawable/walk-03.png", &img_move[2] );
	Action_AddFrame( action, 0,2, &img_move[2], 5 );
	Graph_Init( &img_move[3] );
	Graph_LoadImage( "drawable/walk-04.png", &img_move[3] );
	Action_AddFrame( action, 0,0, &img_move[3], 5 );
	return action;
}

/** 载入角色的站立动作动画资源 */
static ActionData* ActionRes_LoadStance(void)
{
	int i;
	ActionData *action;
	LCUI_Graph img_stance[1];
	char path[512];
	
	action = Action_Create();
	for(i=0; i<1; ++i) {
		Graph_Init( &img_stance[i] );
		sprintf( path, "drawable/stance-%02d.png", i+1 );
		Graph_LoadImage( path, &img_stance[i] );
		Action_AddFrame( action, 0,0, &img_stance[i], 100 );
	}
	return action;
}

/** 载入角色的奔跑动作动画资源 */
static ActionData* ActionRes_LoadRun(void)
{
	int i;
	ActionData *action;
	LCUI_Graph img_run[6];
	char path[512];
	
	action = Action_Create();
	for(i=0; i<6; ++i) {
		Graph_Init( &img_run[i] );
		sprintf( path, "drawable/run-%02d.png", i+1 );
		Graph_LoadImage( path, &img_run[i] );
		Action_AddFrame( action, 0,0, &img_run[i], 3 );
	}
	return action;
}

/** 载入角色的A攻击动作的动画资源 */
static ActionData* ActionRes_LoadAAttack(void)
{
	int i;
	ActionData *action;
	LCUI_Graph img_attack[3];
	char path[512];
	
	action = Action_Create();
	for(i=0; i<3; ++i) {
		Graph_Init( &img_attack[i] );
		sprintf( path, "drawable/A-attack-%02d.png", i+1 );
		Graph_LoadImage( path, &img_attack[i] );
		Action_AddFrame( action, -5,0, &img_attack[i], 5 );
	}
	return action;
}

/** 载入角色的B攻击动作的动画资源 */
static ActionData* ActionRes_LoadBAttack(void)
{
	int i;
	ActionData *action;
	LCUI_Graph img_attack[2];
	char path[512];
	
	action = Action_Create();
	for(i=0; i<2; ++i) {
		Graph_Init( &img_attack[i] );
		sprintf( path, "drawable/B-attack-%02d.png", i+1 );
		Graph_LoadImage( path, &img_attack[i] );
		Action_AddFrame( action, 0,0, &img_attack[i], 5 );
	}
	return action;
}
/** 载入角色的动作动画资源 */
ActionData* ActionRes_LoadRiki( int action_type )
{
	switch( action_type ) {
	case ACTION_STANCE: return ActionRes_LoadStance();
	case ACTION_WALK: return ActionRes_LoadWalk();
	case ACTION_RUN: return ActionRes_LoadRun();
	case ACTION_A_ATTACK: return ActionRes_LoadAAttack();
	case ACTION_B_ATTACK: return ActionRes_LoadBAttack();
	default:break;
	}
	return NULL;
}
