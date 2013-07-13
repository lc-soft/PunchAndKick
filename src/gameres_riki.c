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

/** 载入角色的站立动作动画资源 */
static ActionData* ActionRes_LoadReady(void)
{
	int i;
	ActionData *action;
	LCUI_Graph img_ready[1];
	char path[512];
	
	action = Action_Create();
	for(i=0; i<1; ++i) {
		Graph_Init( &img_ready[i] );
		sprintf( path, "drawable/ready-%02d.png", i+1 );
		Graph_LoadImage( path, &img_ready[i] );
		Action_AddFrame( action, -6,0, &img_ready[i], 50 );
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
	ActionData *action;
	LCUI_Graph img_attack[3];
	
	action = Action_Create();

	Graph_Init( &img_attack[0] );
	Graph_LoadImage( "drawable/A-attack-01.png", &img_attack[0] );
	Action_AddFrame( action, -14,0, &img_attack[0], 3 );
	
	Graph_Init( &img_attack[1] );
	Graph_LoadImage( "drawable/A-attack-02.png", &img_attack[1] );
	Action_AddFrame( action, -14,0, &img_attack[1], 3 );

	Graph_Init( &img_attack[2] ); 
	Graph_LoadImage( "drawable/A-attack-03.png", &img_attack[2] );
	Action_AddFrame( action, -14,0, &img_attack[2], 3 );

	Action_AddFrame( action, -14,0, &img_attack[1], 3 );

	return action;
}

/** 载入角色的B攻击动作的动画资源 */
static ActionData* ActionRes_LoadBAttack(void)
{
	ActionData *action;
	LCUI_Graph img_attack[3];
	
	action = Action_Create();

	Graph_Init( &img_attack[0] );
	Graph_LoadImage( "drawable/B-attack-01.png", &img_attack[0] );
	Action_AddFrame( action, -11,0, &img_attack[0], 5 );
	
	Graph_Init( &img_attack[1] );
	Graph_LoadImage( "drawable/B-attack-02.png", &img_attack[1] );
	Action_AddFrame( action, -11,0, &img_attack[1], 5 );
	Action_AddFrame( action, -11,0, &img_attack[0], 5 );
	return action;
}

static ActionData* ActionRes_LoadASprintAttack(void)
{
	ActionData *action;
	LCUI_Graph img_attack[2];
	
	action = Action_Create();

	Graph_Init( &img_attack[0] );
	Graph_LoadImage( "drawable/A-sprint-attack-01.png", &img_attack[0] );
	Action_AddFrame( action, 0,0, &img_attack[0], 5 );
	
	Graph_Init( &img_attack[1] );
	Graph_LoadImage( "drawable/A-sprint-attack-02.png", &img_attack[1] );
	Action_AddFrame( action, 0,0, &img_attack[1], 100 );
	return action;
}

static ActionData* ActionRes_LoadBSprintAttack(void)
{
	ActionData *action;
	LCUI_Graph img_attack[2];
	
	action = Action_Create();

	Graph_Init( &img_attack[0] );
	Graph_LoadImage( "drawable/B-sprint-attack-01.png", &img_attack[0] );
	Action_AddFrame( action, 0,0, &img_attack[0], 5 );
	
	Graph_Init( &img_attack[1] );
	Graph_LoadImage( "drawable/B-sprint-attack-02.png", &img_attack[1] );
	Action_AddFrame( action, 0,0, &img_attack[1], 100 );
	return action;
}

/** 载入角色的冲刺+跳跃+A攻击动作的动画资源 */
static ActionData* ActionRes_LoadASprintJumpAttack(void)
{
	ActionData *action;
	LCUI_Graph img_attack[3];
	
	action = Action_Create();

	Graph_Init( &img_attack[0] );
	Graph_LoadImage( "drawable/A-attack-01.png", &img_attack[0] );
	Action_AddFrame( action, -14,0, &img_attack[0], 3 );
	
	Graph_Init( &img_attack[1] );
	Graph_LoadImage( "drawable/A-attack-02.png", &img_attack[1] );
	Action_AddFrame( action, -14,0, &img_attack[1], 3 );

	Graph_Init( &img_attack[2] ); 
	Graph_LoadImage( "drawable/A-attack-03.png", &img_attack[2] );
	Action_AddFrame( action, -14,0, &img_attack[2], 1000 );

	return action;
}

/** 载入角色的冲刺+跳跃+B攻击动作的动画资源 */
static ActionData* ActionRes_LoadBSprintJumpAttack(void)
{
	ActionData *action;
	LCUI_Graph img_attack[3];
	
	action = Action_Create();

	Graph_Init( &img_attack[0] );
	Graph_LoadImage( "drawable/B-attack-01.png", &img_attack[0] );
	Action_AddFrame( action, -11,0, &img_attack[0], 5 );
	
	Graph_Init( &img_attack[1] );
	Graph_LoadImage( "drawable/B-attack-02.png", &img_attack[1] );
	Action_AddFrame( action, -11,0, &img_attack[1], 1000 );
	return action;
}

/** 载入角色的跳跃+A攻击动作的动画资源 */
static ActionData* ActionRes_LoadAJumpAttack(void)
{
	ActionData *action;
	LCUI_Graph img_attack[3];
	
	action = Action_Create();

	Graph_Init( &img_attack[0] );
	Graph_LoadImage( "drawable/A-attack-01.png", &img_attack[0] );
	Action_AddFrame( action, -14,0, &img_attack[0], 3 );
	
	Graph_Init( &img_attack[1] );
	Graph_LoadImage( "drawable/A-attack-02.png", &img_attack[1] );
	Action_AddFrame( action, -14,0, &img_attack[1], 3 );

	Graph_Init( &img_attack[2] ); 
	Graph_LoadImage( "drawable/A-attack-03.png", &img_attack[2] );
	Action_AddFrame( action, -14,0, &img_attack[2], 1000 );

	return action;
}

/** 载入角色的跳跃+B攻击动作的动画资源 */
static ActionData* ActionRes_LoadBJumpAttack(void)
{
	ActionData *action;
	LCUI_Graph img_attack[3];
	
	action = Action_Create();

	Graph_Init( &img_attack[0] );
	Graph_LoadImage( "drawable/B-attack-01.png", &img_attack[0] );
	Action_AddFrame( action, -11,0, &img_attack[0], 5 );
	
	Graph_Init( &img_attack[1] );
	Graph_LoadImage( "drawable/B-attack-02.png", &img_attack[1] );
	Action_AddFrame( action, -11,0, &img_attack[1], 1000 );
	return action;
}

static ActionData* ActionRes_LoadJump(void)
{
	ActionData *action;
	LCUI_Graph img_jump;
	
	action = Action_Create();
	Graph_Init( &img_jump );
	Graph_LoadImage( "drawable/jump.png", &img_jump );
	/* 让该帧停留20秒，应该够了，现阶段不会遇到从起跳到着陆需要20秒的情况 */
	Action_AddFrame( action, 0,0, &img_jump, 1000 );
	
	return action;
}

static ActionData* ActionRes_LoadSquat(void)
{
	ActionData *action;
	LCUI_Graph img_squat;
	
	action = Action_Create();
	Graph_Init( &img_squat );
	Graph_LoadImage( "drawable/squat.png", &img_squat );
	Action_AddFrame( action, 0,0, &img_squat, 5 );
	
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
	case ACTION_READY: return ActionRes_LoadReady();
	case ACTION_AS_ATTACK: return ActionRes_LoadASprintAttack();
	case ACTION_BS_ATTACK: return ActionRes_LoadBSprintAttack();
	case ACTION_ASJ_ATTACK: return ActionRes_LoadAJumpAttack();
	case ACTION_BSJ_ATTACK: return ActionRes_LoadBJumpAttack();
	case ACTION_AJ_ATTACK: return ActionRes_LoadASprintJumpAttack();
	case ACTION_BJ_ATTACK: return ActionRes_LoadBSprintJumpAttack();
	case ACTION_JUMP: return ActionRes_LoadJump();
	case ACTION_SQUAT: return ActionRes_LoadSquat();
	default:break;
	}
	return NULL;
}
