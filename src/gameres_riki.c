#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_GRAPH_H

#include "game.h"

#define GLOBAL_Y_WIDTH	12

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
	RangeBox range;
	ActionData *action;
	LCUI_Graph img_stance;
	
	action = Action_Create();
	range.x = -16;
	range.x_width = 32;
	range.y = -GLOBAL_Y_WIDTH/2;
	range.y_width = GLOBAL_Y_WIDTH;
	range.z = 0;
	range.z_width = 64;

	Graph_Init( &img_stance );
	Graph_LoadImage( "drawable/stance-01.png", &img_stance );
	Action_AddFrame( action, 0,0, &img_stance, 100 );
	Action_SetHitRange( action, 0, range );
	return action;
}

/** 载入角色的站立动作动画资源 */
static ActionData* ActionRes_LoadReady(void)
{
	ActionData *action;
	LCUI_Graph img_ready;
	RangeBox hit_range;
	
	action = Action_Create();
	hit_range.x = -16;
	hit_range.x_width = 44;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 62;

	Graph_Init( &img_ready );
	Graph_LoadImage( "drawable/ready-01.png", &img_ready );
	Action_AddFrame( action, -6,0, &img_ready, 50 );
	Action_SetHitRange( action, 0, hit_range );

	return action;
}
/** 载入角色的奔跑动作动画资源 */
static ActionData* ActionRes_LoadRun(void)
{
	int i;
	ActionData *action;
	LCUI_Graph img_run[6];
	RangeBox hit_range;
	char path[512];
	
	action = Action_Create();

	hit_range.x = -24;
	hit_range.x_width = 48;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 60;

	for(i=0; i<6; ++i) {
		Graph_Init( &img_run[i] );
		sprintf( path, "drawable/run-%02d.png", i+1 );
		Graph_LoadImage( path, &img_run[i] );
		Action_AddFrame( action, 0,0, &img_run[i], 3 );
		Action_SetHitRange( action, i, hit_range );
	}
	return action;
}

/** 载入角色的A攻击动作的动画资源 */
static ActionData* ActionRes_LoadAAttack(void)
{
	ActionData *action;
	RangeBox hit_range, attack_range;
	LCUI_Graph img_attack[3];
	
	action = Action_Create();
	
	attack_range.x = 20;
	attack_range.x_width = 26;
	attack_range.y = -GLOBAL_Y_WIDTH/2;
	attack_range.y_width = GLOBAL_Y_WIDTH;
	attack_range.z = 31;
	attack_range.z_width = 18;

	hit_range.x = -18;
	hit_range.x_width = 38;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 62;

	Graph_Init( &img_attack[0] );
	Graph_Init( &img_attack[2] ); 
	Graph_Init( &img_attack[1] );

	Graph_LoadImage( "drawable/A-attack-01.png", &img_attack[0] );
	Action_AddFrame( action, -14,0, &img_attack[0], 3 );
	Action_SetHitRange( action, 0, hit_range );

	Graph_LoadImage( "drawable/A-attack-02.png", &img_attack[1] );
	Action_AddFrame( action, -14,0, &img_attack[1], 3 );
	Action_SetHitRange( action, 1, hit_range );

	Graph_LoadImage( "drawable/A-attack-03.png", &img_attack[2] );
	Action_AddFrame( action, -14,0, &img_attack[2], 3 );
	Action_SetAttackRange( action, 2, attack_range );
	Action_SetHitRange( action, 2, hit_range );

	Action_AddFrame( action, -14,0, &img_attack[1], 3 );
	Action_SetHitRange( action, 3, hit_range );

	return action;
}

/** 载入角色的B攻击动作的动画资源 */
static ActionData* ActionRes_LoadBAttack(void)
{
	ActionData *action;
	RangeBox hit_range, attack_range;
	LCUI_Graph img_attack[3];
	
	action = Action_Create();
	
	attack_range.x = 10;
	attack_range.x_width = 25;
	attack_range.y = -GLOBAL_Y_WIDTH/2;
	attack_range.y_width = GLOBAL_Y_WIDTH;
	attack_range.z = 0;
	attack_range.z_width = 44;

	hit_range.x = -13;
	hit_range.x_width = 34;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 60;

	Graph_Init( &img_attack[0] );
	Graph_Init( &img_attack[1] );

	Graph_LoadImage( "drawable/B-attack-01.png", &img_attack[0] );
	Action_AddFrame( action, -11,0, &img_attack[0], 5 );
	Action_SetHitRange( action, 0, hit_range );

	Graph_LoadImage( "drawable/B-attack-02.png", &img_attack[1] );
	Action_AddFrame( action, -11,0, &img_attack[1], 5 );
	Action_SetHitRange( action, 1, hit_range );
	Action_SetAttackRange( action, 1, attack_range );
	
	Action_AddFrame( action, -11,0, &img_attack[0], 5 );
	Action_SetHitRange( action, 2, hit_range );
	return action;
}

static ActionData* ActionRes_LoadASprintAttack(void)
{
	ActionData *action;
	RangeBox hit_range, attack_range;
	LCUI_Graph img_attack[2];
	
	action = Action_Create();
	
	attack_range.x = -13;
	attack_range.x_width = 36;
	attack_range.y = -GLOBAL_Y_WIDTH/2;
	attack_range.y_width = GLOBAL_Y_WIDTH;
	attack_range.z = 31;
	attack_range.z_width = 25;

	hit_range.x = -23;
	hit_range.x_width = 38;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 58;

	Graph_Init( &img_attack[0] );
	Graph_Init( &img_attack[1] );

	Graph_LoadImage( "drawable/A-sprint-attack-01.png", &img_attack[0] );
	Action_AddFrame( action, 0,0, &img_attack[0], 5 );
	Action_SetHitRange( action, 0, hit_range );

	Graph_LoadImage( "drawable/A-sprint-attack-02.png", &img_attack[1] );
	Action_AddFrame( action, 0,0, &img_attack[1], 100 );
	Action_SetHitRange( action, 1, hit_range );
	Action_SetAttackRange( action, 1, attack_range );

	return action;
}

static ActionData* ActionRes_LoadBSprintAttack(void)
{
	ActionData *action;
	RangeBox hit_range, attack_range;
	LCUI_Graph img_attack[2];
	
	action = Action_Create();
	
	attack_range.x = 10;
	attack_range.x_width = 25;
	attack_range.y = -GLOBAL_Y_WIDTH/2;
	attack_range.y_width = GLOBAL_Y_WIDTH;
	attack_range.z = 0;
	attack_range.z_width = 44;

	hit_range.x = -13;
	hit_range.x_width = 34;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 60;

	Graph_Init( &img_attack[0] );
	Graph_Init( &img_attack[1] );

	Graph_LoadImage( "drawable/B-sprint-attack-01.png", &img_attack[0] );
	Action_AddFrame( action, 0,0, &img_attack[0], 5 );
	Action_SetHitRange( action, 0, hit_range );

	Graph_LoadImage( "drawable/B-sprint-attack-02.png", &img_attack[1] );
	Action_AddFrame( action, 0,0, &img_attack[1], 100 );
	Action_SetAttackRange( action, 1, attack_range );
	Action_SetHitRange( action, 1, hit_range );
	return action;
}

/** 载入角色的冲刺+跳跃+A攻击动作的动画资源 */
static ActionData* ActionRes_LoadASprintJumpAttack(void)
{
	ActionData *action;
	LCUI_Graph img_attack[3];
	RangeBox hit_range, attack_range;

	action = Action_Create();
	
	attack_range.x = 20;
	attack_range.x_width = 26;
	attack_range.y = -GLOBAL_Y_WIDTH/2;
	attack_range.y_width = GLOBAL_Y_WIDTH;
	attack_range.z = 31;
	attack_range.z_width = 18;

	hit_range.x = -18;
	hit_range.x_width = 38;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 62;

	Graph_Init( &img_attack[0] );
	Graph_Init( &img_attack[1] );
	Graph_Init( &img_attack[2] ); 

	Graph_LoadImage( "drawable/A-attack-01.png", &img_attack[0] );
	Action_AddFrame( action, -14,0, &img_attack[0], 3 );
	Action_SetHitRange( action, 0, hit_range );

	Graph_LoadImage( "drawable/A-attack-02.png", &img_attack[1] );
	Action_AddFrame( action, -14,0, &img_attack[1], 3 );
	Action_SetHitRange( action, 1, hit_range );

	Graph_LoadImage( "drawable/A-attack-03.png", &img_attack[2] );
	Action_AddFrame( action, -14,0, &img_attack[2], 1000 );
	Action_SetAttackRange( action, 2, attack_range );
	Action_SetHitRange( action, 2, hit_range );

	return action;
}

/** 载入角色的冲刺+跳跃+B攻击动作的动画资源 */
static ActionData* ActionRes_LoadBSprintJumpAttack(void)
{
	ActionData *action;
	RangeBox hit_range, attack_range;
	LCUI_Graph img_attack[2];
	
	action = Action_Create();
	
	attack_range.x = 10;
	attack_range.x_width = 25;
	attack_range.y = -GLOBAL_Y_WIDTH/2;
	attack_range.y_width = GLOBAL_Y_WIDTH;
	attack_range.z = 0;
	attack_range.z_width = 44;

	hit_range.x = -13;
	hit_range.x_width = 34;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 60;

	Graph_Init( &img_attack[0] );
	Graph_LoadImage( "drawable/B-attack-01.png", &img_attack[0] );
	Action_AddFrame( action, -11,0, &img_attack[0], 5 );
	Action_SetHitRange( action, 0, hit_range );

	Graph_Init( &img_attack[1] );
	Graph_LoadImage( "drawable/B-attack-02.png", &img_attack[1] );
	Action_AddFrame( action, -11,0, &img_attack[1], 1000 );
	Action_SetHitRange( action, 1, hit_range );
	Action_SetAttackRange( action, 1, attack_range );

	return action;
}

/** 载入角色的跳跃+A攻击动作的动画资源 */
static ActionData* ActionRes_LoadAJumpAttack(void)
{
	ActionData *action;
	RangeBox hit_range, attack_range;
	LCUI_Graph img_attack[3];
	
	action = Action_Create();
	
	attack_range.x = 20;
	attack_range.x_width = 26;
	attack_range.y = -GLOBAL_Y_WIDTH/2;
	attack_range.y_width = GLOBAL_Y_WIDTH;
	attack_range.z = 31;
	attack_range.z_width = 18;

	hit_range.x = -18;
	hit_range.x_width = 38;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 62;

	Graph_Init( &img_attack[0] );
	Graph_Init( &img_attack[1] );
	Graph_Init( &img_attack[2] ); 

	Graph_LoadImage( "drawable/A-attack-01.png", &img_attack[0] );
	Action_AddFrame( action, -14,0, &img_attack[0], 3 );
	Action_SetHitRange( action, 0, hit_range );

	Graph_LoadImage( "drawable/A-attack-02.png", &img_attack[1] );
	Action_AddFrame( action, -14,0, &img_attack[1], 3 );
	Action_SetHitRange( action, 1, hit_range );

	Graph_LoadImage( "drawable/A-attack-03.png", &img_attack[2] );
	Action_AddFrame( action, -14,0, &img_attack[2], 1000 );
	Action_SetHitRange( action, 2, hit_range );
	Action_SetAttackRange( action, 2, attack_range );

	return action;
}

/** 载入角色的跳跃+B攻击动作的动画资源 */
static ActionData* ActionRes_LoadBJumpAttack(void)
{
	ActionData *action;
	RangeBox hit_range, attack_range;
	LCUI_Graph img_attack[3];
	
	action = Action_Create();
	
	attack_range.x = 10;
	attack_range.x_width = 25;
	attack_range.y = -GLOBAL_Y_WIDTH/2;
	attack_range.y_width = GLOBAL_Y_WIDTH;
	attack_range.z = 0;
	attack_range.z_width = 44;

	hit_range.x = -13;
	hit_range.x_width = 34;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 60;

	Graph_Init( &img_attack[0] );
	Graph_Init( &img_attack[1] );

	Graph_LoadImage( "drawable/B-attack-01.png", &img_attack[0] );
	Action_AddFrame( action, -11,0, &img_attack[0], 5 );
	Action_SetHitRange( action, 0, hit_range );

	Graph_LoadImage( "drawable/B-attack-02.png", &img_attack[1] );
	Action_AddFrame( action, -11,0, &img_attack[1], 1000 );
	Action_SetHitRange( action, 1, hit_range );
	Action_SetAttackRange( action, 1, attack_range );
	
	return action;
}

static ActionData* ActionRes_LoadJump(void)
{
	ActionData *action;
	LCUI_Graph img_jump;
	RangeBox hit_range;
	
	action = Action_Create();
	
	hit_range.x = -16;
	hit_range.x_width = 32;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 54;

	Graph_Init( &img_jump );
	Graph_LoadImage( "drawable/jump.png", &img_jump );
	/* 让该帧停留20秒，应该够了，现阶段不会遇到从起跳到着陆需要20秒的情况 */
	Action_AddFrame( action, 0,0, &img_jump, 1000 );
	Action_SetHitRange( action, 0, hit_range );

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

static ActionData* ActionRes_LoadHit(void)
{
	ActionData *action;
	LCUI_Graph img_hit;
	RangeBox hit_range;

	action = Action_Create();
	hit_range.x = -23;
	hit_range.x_width = 46;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 62;
	Graph_Init( &img_hit );
	Graph_LoadImage( "drawable/hit.png", &img_hit );
	Action_AddFrame( action, 0,0, &img_hit, 10 );
	Action_SetHitRange( action, 0, hit_range );

	return action;
}

static ActionData* ActionRes_LoadRest(void)
{
	ActionData *action;
	LCUI_Graph img_rest[2];
	RangeBox hit_range;

	action = Action_Create();
	hit_range.x = -16;
	hit_range.x_width = 40;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 56;
	Graph_Init( &img_rest[0] );
	Graph_Init( &img_rest[1] );
	Graph_LoadImage( "drawable/rest-01.png", &img_rest[0] );
	Graph_LoadImage( "drawable/rest-02.png", &img_rest[1] );
	Action_AddFrame( action, -6,0, &img_rest[0], 25 );
	Action_AddFrame( action, -6,0, &img_rest[1], 25 );
	Action_SetHitRange( action, 0, hit_range );
	Action_SetHitRange( action, 1, hit_range );

	return action;
}

/** 终结一击 */
static ActionData *ActionRes_LoadFinalBlow( void )
{
	ActionData *action;
	LCUI_Graph img_attack[3];
	RangeBox hit_range, attack_range;

	action = Action_Create();
	hit_range.x = -4;
	hit_range.x_width = 36;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 60;
	attack_range.x = -12;
	attack_range.x_width = 50;
	attack_range.y = -GLOBAL_Y_WIDTH/2;
	attack_range.y_width = GLOBAL_Y_WIDTH;
	attack_range.z = 0;
	attack_range.z_width = 64;
	Graph_Init( &img_attack[0] );
	Graph_Init( &img_attack[1] );
	Graph_Init( &img_attack[2] );
	Graph_LoadImage( "drawable/final-blow-01.png", &img_attack[0] );
	Graph_LoadImage( "drawable/final-blow-02.png", &img_attack[1] );
	Graph_LoadImage( "drawable/final-blow-03.png", &img_attack[2] );
	Action_AddFrame( action, -2, 0, &img_attack[0], 3 );
	Action_AddFrame( action, -2, 0, &img_attack[1], 3 );
	Action_AddFrame( action, -2, 0, &img_attack[2], 10 );
	Action_SetHitRange( action, 0, hit_range );
	Action_SetHitRange( action, 1, hit_range );
	Action_SetHitRange( action, 2, hit_range );
	Action_SetAttackRange( action, 1, attack_range );
	Action_SetAttackRange( action, 2, attack_range );
	return action;
}

/** 躺着 */
static ActionData *ActionRes_LoadLying(void)
{
	ActionData *action;
	LCUI_Graph img_lying;
	RangeBox hit_range;
	
	action = Action_Create();

	hit_range.x = -44;
	hit_range.x_width = 64;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 26;

	Graph_Init( &img_lying );
	Graph_LoadImage( "drawable/lying.png", &img_lying );
	Action_AddFrame( action, 12, 0, &img_lying, 50 );
	Action_SetHitRange( action, 0, hit_range );

	return action;
}

/** 躺着也被打 */
static ActionData *ActionRes_LoadLyingHit(void)
{
	ActionData *action;
	LCUI_Graph img_lying;
	RangeBox hit_range;
	
	action = Action_Create();

	hit_range.x = -44;
	hit_range.x_width = 64;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 26;

	Graph_Init( &img_lying );
	Graph_LoadImage( "drawable/lying-hit.png", &img_lying );
	Action_AddFrame( action, 12, 0, &img_lying, 10 );
	Action_SetHitRange( action, 0, hit_range );

	return action;
}

/** 趴着 */
static ActionData *ActionRes_LoadTummy(void)
{
	ActionData *action;
	LCUI_Graph img_tummy;
	RangeBox hit_range;
	
	action = Action_Create();

	hit_range.x = -20;
	hit_range.x_width = 64;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 26;

	Graph_Init( &img_tummy );
	Graph_LoadImage( "drawable/tummy.png", &img_tummy );
	Action_AddFrame( action, -12, 0, &img_tummy, 10 );
	Action_SetHitRange( action, 0, hit_range );

	return action;
}

/** 趴着也被打 */
static ActionData *ActionRes_LoadTummyHit(void)
{
	ActionData *action;
	LCUI_Graph img_tummy;
	RangeBox hit_range;
	
	action = Action_Create();

	hit_range.x = -20;
	hit_range.x_width = 64;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 26;

	Graph_Init( &img_tummy );
	Graph_LoadImage( "drawable/tummy-hit.png", &img_tummy );
	Action_AddFrame( action, -12, 0, &img_tummy, 5 );
	Action_SetHitRange( action, 0, hit_range );

	return action;
}

/** 被终结一击从正面击飞 */
static ActionData *ActionRes_LoadFrontalHitFly(void)
{
	ActionData *action;
	LCUI_Graph img[3];
	RangeBox hit_range;
	
	action = Action_Create();

	Graph_Init( &img[0] );
	Graph_Init( &img[1] );
	Graph_Init( &img[2] );
	Graph_LoadImage( "drawable/hit-fly.png", &img[0] );
	Graph_LoadImage( "drawable/lying-hit.png", &img[1] );
	Graph_LoadImage( "drawable/fall.png", &img[2] );
	Action_AddFrame( action, -6, 0, &img[0], 8 );
	Action_AddFrame( action, 0, 0, &img[1], 8 );
	Action_AddFrame( action, 0, 0, &img[2], 1000 );
	
	hit_range.x = -18;
	hit_range.x_width = 38;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 64;
	Action_SetHitRange( action, 0, hit_range );
	
	hit_range.x = -44;
	hit_range.x_width = 64;
	hit_range.z = 0;
	hit_range.z_width = 26;
	Action_SetHitRange( action, 1, hit_range );

	
	hit_range.x = -18;
	hit_range.x_width = 38;
	hit_range.z = 0;
	hit_range.z_width = 64;
	Action_SetHitRange( action, 2, hit_range );

	return action;
}

/** 被终结一击从背面击飞 */
static ActionData *ActionRes_LoadBackHitFly(void)
{
	ActionData *action;
	LCUI_Graph img;
	RangeBox hit_range;
	
	action = Action_Create();

	hit_range.x = -18;
	hit_range.x_width = 38;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 64;

	Graph_Init( &img );
	Graph_LoadImage( "drawable/hit-fly.png", &img );
	Action_AddFrame( action, -6, 0, &img, 1000 );
	Action_SetHitRange( action, 0, hit_range );

	return action;
}

/** 普通的击飞 */
static ActionData *ActionRes_LoadHitFly(void)
{
	ActionData *action;
	LCUI_Graph img;
	RangeBox hit_range;
	
	action = Action_Create();

	hit_range.x = -18;
	hit_range.x_width = 38;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 64;

	Graph_Init( &img );
	Graph_LoadImage( "drawable/hit-fly.png", &img );
	Action_AddFrame( action, -6, 0, &img, 1000 );
	Action_SetHitRange( action, 0, hit_range );

	return action;
}

/** 向前方滚动 */
static ActionData *ActionRes_LoadForwardRoll(void)
{
	ActionData *action;
	LCUI_Graph img[4];
	RangeBox hit_range;
	
	action = Action_Create();
	
	hit_range.x = -22;
	hit_range.x_width = 44;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 44;

	Graph_Init( &img[0] );
	Graph_Init( &img[1] );
	Graph_Init( &img[2] );
	Graph_Init( &img[3] );
	Graph_LoadImage( "drawable/roll-01.png", &img[0] );
	Graph_LoadImage( "drawable/roll-02.png", &img[1] );
	Graph_LoadImage( "drawable/roll-03.png", &img[2] );
	Graph_LoadImage( "drawable/roll-04.png", &img[3] );
	Action_AddFrame( action, 0, 0, &img[0], 5 );
	Action_AddFrame( action, 0, 0, &img[1], 5 );
	Action_AddFrame( action, 0, 0, &img[2], 5 );
	Action_AddFrame( action, 0, 0, &img[3], 5 );
	Action_SetHitRange( action, 0, hit_range );
	Action_SetHitRange( action, 1, hit_range );
	Action_SetHitRange( action, 2, hit_range );
	Action_SetHitRange( action, 3, hit_range );

	return action;
}

/** 向后方滚动 */
static ActionData *ActionRes_LoadBackwardRoll(void)
{
	ActionData *action;
	LCUI_Graph img[4];
	RangeBox hit_range;
	
	action = Action_Create();
	
	hit_range.x = -22;
	hit_range.x_width = 44;
	hit_range.y = -GLOBAL_Y_WIDTH/2;
	hit_range.y_width = GLOBAL_Y_WIDTH;
	hit_range.z = 0;
	hit_range.z_width = 44;

	Graph_Init( &img[0] );
	Graph_Init( &img[1] );
	Graph_Init( &img[2] );
	Graph_Init( &img[3] );
	Graph_LoadImage( "drawable/roll-04.png", &img[0] );
	Graph_LoadImage( "drawable/roll-03.png", &img[1] );
	Graph_LoadImage( "drawable/roll-02.png", &img[2] );
	Graph_LoadImage( "drawable/roll-01.png", &img[3] );
	Action_AddFrame( action, 0, 0, &img[0], 3 );
	Action_AddFrame( action, 0, 0, &img[1], 3 );
	Action_AddFrame( action, 0, 0, &img[2], 3 );
	Action_AddFrame( action, 0, 0, &img[3], 3 );
	Action_SetHitRange( action, 0, hit_range );
	Action_SetHitRange( action, 1, hit_range );
	Action_SetHitRange( action, 2, hit_range );
	Action_SetHitRange( action, 3, hit_range );

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
	case ACTION_AJ_ATTACK: return ActionRes_LoadAJumpAttack();
	case ACTION_BJ_ATTACK: return ActionRes_LoadBJumpAttack();
	case ACTION_ASJ_ATTACK: return ActionRes_LoadASprintJumpAttack();
	case ACTION_BSJ_ATTACK: return ActionRes_LoadBSprintJumpAttack();
	case ACTION_JUMP: return ActionRes_LoadJump();
	case ACTION_SQUAT: return ActionRes_LoadSquat();
	case ACTION_HIT: return ActionRes_LoadHit();
	case ACTION_REST: return ActionRes_LoadRest();
	case ACTION_FINAL_BLOW:return ActionRes_LoadFinalBlow();
	case ACTION_F_HIT_FLY: return ActionRes_LoadFrontalHitFly();
	case ACTION_B_HIT_FLY: return ActionRes_LoadBackHitFly();
	case ACTION_HIT_FLY: return ActionRes_LoadHitFly();
	case ACTION_LYING: return ActionRes_LoadLying();
	case ACTION_LYING_HIT: return ActionRes_LoadLyingHit();
	case ACTION_TUMMY: return ActionRes_LoadTummy();
	case ACTION_TUMMY_HIT: return ActionRes_LoadTummyHit();
	case ACTION_F_ROLL: return ActionRes_LoadForwardRoll();
	case ACTION_B_ROLL: return ActionRes_LoadBackwardRoll();
	default:break;
	}
	return NULL;
}
