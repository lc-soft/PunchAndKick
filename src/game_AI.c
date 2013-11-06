#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H

#include <time.h>
#include "game.h"

#define RANGE_MAX		-1
#define MAX_STRATEGY_NUM	35
#define MAX_ACTION_NUM		5

enum AIActionType {
	ai_action_type_none,
	ai_action_type_wait,
	ai_action_type_run_close,	/**< 跑步靠近目标 */
	ai_action_type_walk_close,	/**< 步行靠近目标 */
	ai_action_type_run_away,	/**< 跑步远离目标 */
	ai_action_type_walk_away,	/**< 步行远离目标 */
	ai_action_type_observe,		/**< 观察目标 */
	ai_action_type_stop_run,	/**< 停止奔跑 */
	ai_action_type_a_attack,	/**< A攻击 */
	ai_action_type_b_attack,	/**< B攻击 */
	ai_action_type_jump,		/**< 跳跃 */
	ai_action_type_down
};

/** 距离范围 */
typedef struct DistanceRange_ {
	int min_x_width, max_x_width;
	int min_y_width, max_y_width;
} DistanceRange;

typedef struct AIActionData_ {
	LCUI_BOOL enable;	/**< 该动作是否启用 */
	unsigned int time;	/**< 切换至下个动作前需要等待的时间(ms) */
	int action_type;	/**< 动作类型 */
} AIActionData;

/** AI的对战策略 */
typedef struct AIStrategy_ {
	int priority;				/**< 优先级，-20至20，从高到低 */
	LCUI_BOOL (*self_state)(int);		/**< 回调函数，用于检测自身的状态是否符合要求 */
	LCUI_BOOL (*target_state)(int);		/**< 回调函数，用于检测对方的状态是否符合要求 */
	DistanceRange range;			/**< 与目标的距离范围 */
	AIActionData action[MAX_ACTION_NUM];	/**< 该策略中用到的连续动作 */
} AIStrategy;

/** 随机想法 */
typedef struct RandIdea_ {
	int id;			/**< 该想法的标识号 */
	int probability;	/**< 该想法出现的概率，取值范围为0(0.0%)至10000(100%) */
} RandIdea;

static LCUI_BOOL PlayerCanChangeOriented( int state )
{
	switch( state ) {
	case STATE_READY:
	case STATE_STANCE:
	case STATE_WALK:
	case STATE_JUMP:
	case STATE_FALL:
	case STATE_SQUAT:
	case STATE_SSQUAT:
	case STATE_JUMP_DONE:
	case STATE_BE_LIFT_STANCE:
		return TRUE;
	default:break;
	}
	return FALSE;
}

static LCUI_BOOL ICanAction( int state )
{
	switch( state ) {
	case STATE_READY:
	case STATE_STANCE:
	case STATE_WALK:
	case STATE_JUMP:
	case STATE_FALL:
	case STATE_BE_LIFT_STANCE:
		return TRUE;
	default:break;
	}
	return FALSE;
}

static LCUI_BOOL PlayerCanReady( int state )
{
	switch( state ) {
	case STATE_READY:
	case STATE_STANCE:
	case STATE_WALK:
		return TRUE;
	default:break;
	}
	return FALSE;
}

static LCUI_BOOL TargetCanAction( int state )
{
	switch( state ) {
	case STATE_LYING:
	case STATE_TUMMY:
	case STATE_LYING_HIT:
	case STATE_TUMMY_HIT:
	case STATE_REST:
	case STATE_JUMP:
	case STATE_SJUMP:
	case STATE_SQUAT:
	case STATE_SSQUAT:
	case STATE_JUMP_DONE:
		return FALSE;
	default:break;
	}
	return TRUE;
}

static LCUI_BOOL TargetIsRest( int state )
{
	if( state == STATE_REST ) {
		return TRUE;
	}
	return FALSE;
}

static LCUI_BOOL IAmJumpDone( int state )
{
	switch( state ) {
	case STATE_JUMP_DONE:
		return TRUE;
	default:break;
	}
	return FALSE;
}

static LCUI_BOOL IAmRunning( int state )
{
	switch( state ) {
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
		return TRUE;
	default:break;
	}
	return FALSE;
}

static LCUI_BOOL IAmSJump( int state )
{
	if( state == STATE_SJUMP ) {
		return TRUE;
	}
	return FALSE;
}

static LCUI_BOOL PlayerWillSprintJumpAttack( int state )
{
	if( state == STATE_SJUMP
	 || state == STATE_ASJ_ATTACK
	 || state == STATE_BSJ_ATTACK ) {
		return TRUE;
	}
	return FALSE;
}

static LCUI_BOOL ICanCloseTarget( int state )
{
	switch( state ) {
	case STATE_READY:
	case STATE_STANCE:
	case STATE_WALK:
	case STATE_LEFTRUN:
	case STATE_RIGHTRUN:
		return TRUE;
	default:break;
	}
	return FALSE;
}

static LCUI_BOOL ICatchTarget( int state )
{
	if( state == STATE_CATCH ) {
		return TRUE;
	}
	return FALSE;
}

static LCUI_BOOL ILifeTarget( int state )
{
	switch( state ) {
	case STATE_LIFT_STANCE:
	case STATE_LIFT_FALL:
	case STATE_LIFT_JUMP:
	case STATE_LIFT_RUN:
	case STATE_LIFT_WALK:
		return TRUE;
	default:break;
	}
	return FALSE;
}

static LCUI_BOOL TargetIsLying( int state )
{
	switch( state ) {
	case STATE_LYING:
	case STATE_TUMMY:
	case STATE_LYING_HIT:
	case STATE_TUMMY_HIT:
		return TRUE;
	}
	return FALSE;
}

static LCUI_BOOL IAmRide( int state )
{
	if( state == STATE_RIDE ) {
		return TRUE;
	}
	return FALSE;
}

static AIStrategy global_strategy_set[MAX_STRATEGY_NUM] = {
	{ 
		0,
		/* 距离在0-55的范围内，对目标进行A攻击 */
		ICanAction, TargetCanAction, {0,55,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 200, ai_action_type_a_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离在25-40的范围内，目标躺地，对目标进行A攻击 */
		ICanAction, TargetIsLying, {25,40,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 200, ai_action_type_a_attack},
			{FALSE, 0, ai_action_type_none}
		}
	},{ 
		0,
		/* 距离在0-55的范围内，对目标进行B攻击 */
		ICanAction, NULL, {0,55,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 200, ai_action_type_b_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离在0-55的范围内，目标处于喘气状态，跳跃+攻击目标 */
		ICanAction, TargetIsRest, {0,55,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 200, ai_action_type_jump},
			{TRUE, 0, ai_action_type_b_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离在0-55的范围内，目标处于喘气状态，攻击目标 */
		ICanAction, TargetIsRest, {0,55,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 200, ai_action_type_a_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离在0-55的范围内，目标处于喘气状态，站着观察 */
		ICanAction, TargetIsRest, {0,55,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 300, ai_action_type_observe},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离在0-55的范围内，目标处于喘气状态，靠近他 */
		ICanAction, TargetIsRest, {0,55,0,GLOBAL_Y_WIDTH}, 
		{
			{TRUE, 50, ai_action_type_walk_close},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		ICanAction, TargetIsLying, {10,55,0,RANGE_MAX},
		{
			{TRUE, 100, ai_action_type_walk_close},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		ICanAction, TargetIsLying, {0,10,0,GLOBAL_Y_WIDTH/2-2},
		{
			{TRUE, 200, ai_action_type_a_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		ICanAction, TargetIsLying, {0,10,0,GLOBAL_Y_WIDTH/2-2},
		{
			{TRUE, 50, ai_action_type_down},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		IAmRide, TargetIsLying, {0,10,0,GLOBAL_Y_WIDTH/2-2},
		{
			{TRUE, 50, ai_action_type_a_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		IAmRide, TargetIsLying, {0,10,0,GLOBAL_Y_WIDTH/2-2},
		{
			{TRUE, 50, ai_action_type_b_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离在0-20的范围内，步行远离目标 */
		ICanAction, TargetCanAction, {0,20,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 500, ai_action_type_walk_away},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离在0-40的范围内，跑步远离目标 */
		ICanAction, TargetCanAction, {0,40,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 300, ai_action_type_run_away},
			{TRUE, 50, ai_action_type_stop_run},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离在150以上的范围内，跑步靠近目标 */
		ICanCloseTarget, NULL, {150,RANGE_MAX,0,RANGE_MAX}, 
		{
			{TRUE, 50, ai_action_type_run_close},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 跳跃落地时，若目标在攻击范围内，则使用自旋击 */
		IAmJumpDone, NULL, {20,400,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 100, ai_action_type_a_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 跳跃落地时，若目标在攻击范围内，则使用爆裂腿 */
		IAmJumpDone, NULL, {20,400,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 100, ai_action_type_b_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 跳跃落地后，观察目标 */
		IAmJumpDone, NULL, {20,400,0,RANGE_MAX}, 
		{
			{TRUE, 200, ai_action_type_observe},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		ICanCloseTarget, TargetCanAction, {65,80,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 0, ai_action_type_run_close},
			{TRUE, 100, ai_action_type_a_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		ICanCloseTarget, NULL, {65,80,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 0, ai_action_type_run_close},
			{TRUE, 100, ai_action_type_b_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离在10至150的范围内，处于跑步状态，使用冲刺A攻击 */
		IAmRunning, TargetCanAction, {10,150,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 100, ai_action_type_a_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离在10至150的范围内，处于跑步状态，使用冲刺B攻击 */
		IAmRunning, NULL, {10,150,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 100, ai_action_type_b_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离500以上，观察目标 */
		ICanAction, TargetCanAction, {500,RANGE_MAX,0,RANGE_MAX}, 
		{
			{TRUE, 200, ai_action_type_observe},
			{FALSE, 0, ai_action_type_none}
		}
	}, {
		0,
		/* 距离在0-150的范围内，步行靠近目标 */
		ICanAction, NULL, {0,150,0,RANGE_MAX}, 
		{
			{TRUE, 50, ai_action_type_walk_close},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离在280-300的范围内，进行跳跃 */
		IAmRunning, NULL, {250,300,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 0, ai_action_type_jump},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离在250-300的范围内，进行跳跃+A攻击 */
		IAmRunning, TargetCanAction, {250,300,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 50, ai_action_type_jump},
			{TRUE, 50, ai_action_type_a_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离在250-300的范围内，进行跳跃+B攻击 */
		IAmRunning, NULL, {250,300,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 50, ai_action_type_jump},
			{TRUE, 50, ai_action_type_b_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离在20-150的范围内，自己处于奔跑后的跃起状态，进行A攻击 */
		IAmSJump, TargetCanAction, {20,150,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 50, ai_action_type_a_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离在20-150的范围内，自己处于奔跑后的跃起状态，进行B攻击 */
		IAmSJump, NULL, {20,150,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 50, ai_action_type_b_attack},
			{FALSE, 0, ai_action_type_none}
		}
	},{ 
		0,
		/* 距离在20-300的范围内，对方若要进行跳跃攻击，跑步远离 */
		ICanAction, PlayerWillSprintJumpAttack, {20,300,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 200, ai_action_type_run_away},
			{TRUE, 50, ai_action_type_stop_run},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 距离在350-420的范围内，进行跳跃，并使用高挑旋转落体 */
		IAmRunning, NULL, {350,420,0,GLOBAL_Y_WIDTH/2-2}, 
		{
			{TRUE, 50, ai_action_type_jump},
			{TRUE, 200, ai_action_type_down},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 如果自己擒住了一个目标，则使用A攻击 */
		ICatchTarget, NULL, {0,RANGE_MAX,0,GLOBAL_Y_WIDTH}, 
		{
			{TRUE, 500, ai_action_type_wait},
			{TRUE, 500, ai_action_type_a_attack},
			{TRUE, 500, ai_action_type_observe},
			{FALSE, 0, ai_action_type_none}
		}
	}, { 
		0,
		/* 如果自己擒住了一个目标，则使用B攻击 */
		ICatchTarget, NULL, {0,RANGE_MAX,0,GLOBAL_Y_WIDTH}, 
		{
			{TRUE, 500, ai_action_type_wait},
			{TRUE, 200, ai_action_type_b_attack},
			{TRUE, 500, ai_action_type_observe},
			{FALSE, 0, ai_action_type_none}
		}
	}, {
		0,
		/* 如果自己举起了一个目标，则抛掷出去 */
		ILifeTarget, NULL, {0,RANGE_MAX,0,GLOBAL_Y_WIDTH}, 
		{
			{TRUE, 500, ai_action_type_wait},
			{TRUE, 500, ai_action_type_a_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}, {
		0,
		/* 如果自己举起了一个目标，则往下摔 */
		ILifeTarget, NULL, {0,RANGE_MAX,0,GLOBAL_Y_WIDTH}, 
		{
			{TRUE, 500, ai_action_type_wait},
			{TRUE, 500, ai_action_type_b_attack},
			{FALSE, 0, ai_action_type_none}
		}
	}
};

/** 获取自己的攻击目标 */
static GamePlayer *GamePlayer_GetTarget( GamePlayer *self )
{
	int id, i;
	GamePlayer *player;
	if( LCUI_GetTickCount() < self->ai_data.target_update_time ) {
		player = GameBattle_GetPlayer( self->battle_id, self->ai_data.target_id );
		if( player && player->enable
		&& player->state != STATE_DIED
		&& player->state != STATE_LYING_DYING
		&& player->state != STATE_TUMMY_DYING ) {
			return player;
		}
	}
	self->ai_data.target_update_time = LCUI_GetTickCount() + 10000;
	/* 最多重选10次目标 */
	for(i=0; i<10; ++i) {
		id = rand()%4+1;
		if( id != self->id ) {
			self->ai_data.target_id = id;
			player = GameBattle_GetPlayer( self->battle_id, id );
			if( !player || !player->enable
			|| player->state == STATE_DIED
			|| player->state == STATE_LYING_DYING
			|| player->state == STATE_TUMMY_DYING ) {
				continue;
			}
			return player;
		}
	}
	return NULL;
}

/** 获取自己与对方的距离 */
static LCUI_BOOL GamePlayer_GetDistance( GamePlayer *self, GamePlayer *target, int *x_width, int *y_width )
{
	if( !self || !target || !self->object || !target->object ) {
		return FALSE;
	}
	*x_width = (int)GameObject_GetX( self->object );
	*x_width -= (int)GameObject_GetX( target->object );
	*y_width = (int)GameObject_GetY( self->object );
	*y_width -= (int)GameObject_GetY( target->object );
	return TRUE;
}

/*******************************************************************************
假设有A、B、C三个随机事件，概率分别为：0.95,、0.045、0.005，随机情况如下：

随机次数		20			200
-------------------------------------------------------------
随机事件
A，0.95，1.05：
		20/1.05 = 19.05		200/1.05 = 190.48
		19.05 - 19 = 0.05	190.48 - 190 = 0.48
B，0.045，22.22：
		20/22.22 =0.90		200/22.22 = 9.0009
		0.90 - 0 = 0.90		9.0009 - 9 = 0.0009
C，0.005，200：
		20/200 = 0.1		200/200 = 1
		0.1 - 0 = 0.1		1 - 1 = 0
随机结果：
		A事件可能会出现		C事件可能会出现
*******************************************************************************/

/** 从若干个随机想法中选择一个想法 */
static int RandIdea_Select( RandIdea* ideas, int n_idea )
{
	int  *idea_pos;
	double *idea_amount, avg;
	int i, j, num, total_amount, val;
	
	if( n_idea < 1 ) {
		return -1;
	}
	num = rand()%10000;
	total_amount = 10000;
	idea_amount = (double*)malloc( n_idea *sizeof(double) );
	if( !idea_amount ) {
		return -2;
	}
	idea_pos = (int*)malloc( n_idea *sizeof(int) );
	if( !idea_pos ) {
		return -3;
	}
	for(avg=0,i=0; i<n_idea; ++i) {
		if( ideas[i].probability > total_amount ) {
			ideas[i].probability = total_amount;
		}
		total_amount -= ideas[i].probability;
		idea_amount[i] = 1.0 * num * ideas[i].probability / 10000;
		val = (int)idea_amount[i];
		idea_amount[i] -= val;
		avg += idea_amount[i];
	}
	avg = avg / n_idea;
	for(j=0,i=0; i<n_idea; ++i) {
		/* 如果小于平均概率，则记录该想法的下标 */
		if( idea_amount[i] <= avg ) {
			idea_pos[j] = i;
			++j;
		}
	}
	/* 返回被随机选中的想法的ID */
	return ideas[idea_pos[rand()%j]].id;
}

/** 初始化游戏AI */
void GameAI_Init(void)
{
	srand((unsigned int)time(NULL));
}

LCUI_BOOL StrategyActionIsTimeOut( GameAI_Data *p_data )
{
	int64_t cur_time;
	unsigned int diff_val;
	AIStrategy *p_strategy;

	/* 若动作序号超出有效范围 */
	if( p_data->action_num >= MAX_ACTION_NUM ) {
		return TRUE;
	}
	p_strategy = &global_strategy_set[p_data->strategy_id];
	/* 若该动作并没有启用 */
	if( !p_strategy->action[p_data->action_num].enable ) {
		return TRUE;
	}
	cur_time = LCUI_GetTickCount();
	diff_val = (unsigned int)(cur_time - p_data->start_time);
	/* 若该动作的停留时间超出时限 */
	if( diff_val >= p_strategy->action[p_data->action_num].time ) {
		return TRUE;
	}
	return FALSE;
}

/** 检测当前策略是否有效 */
LCUI_BOOL StrategyIsValid( GameAI_Data *p_data, int target_state, int x_width, int y_width )
{
	int n_last_action;
	AIStrategy *p_strategy;

	/* 若策略ID超出有效范围 */
	if( p_data->strategy_id < 0 && p_data->strategy_id >= MAX_STRATEGY_NUM ) {
		return FALSE;
	}
	p_strategy = &global_strategy_set[p_data->strategy_id];
	for(n_last_action=0; n_last_action<MAX_ACTION_NUM; ++n_last_action) {
		if( !p_strategy->action[n_last_action].enable ) {
			if( n_last_action > 0 ) {
				--n_last_action;
			}
			break;
		}
	};
	/* 如果这是最后一个动作，则检测该动作是否超时 */
	if( p_data->action_num == n_last_action ) {
		return !StrategyActionIsTimeOut( p_data );
	}
	else if( p_data->action_num > n_last_action ) {
		return FALSE;
	}
	return TRUE;
}

/** 按条件过滤策略，并获取符合条件的策略ID */
int FilterStrategy( int *strategy_buff, int max_num, int self_state, int target_state, int x_width, int y_width )
{
	int i, j;
	/* 自己死了或者目标死了，那就没策略了 */
	if( self_state == STATE_DIED
	|| target_state == STATE_DIED ) {
		return 0;
	}
	/* 取距离的绝对值 */
	if( x_width < 0 ) {
		x_width = 0 - x_width;
	}
	if( y_width < 0 ) {
		y_width = 0 - y_width;
	}
	DEBUG_MSG("[%d], y_width: %d, max: %d\n", i, y_width, global_strategy_set[i].range.max_y_width);
	/* 遍历查找符合条件的策略 */
	for(j=0,i=0; i<MAX_STRATEGY_NUM; ++i) {
		/* 判断状态是否符合要求 */
		if( global_strategy_set[i].self_state ) {
			if( !global_strategy_set[i].self_state( self_state ) ) {
				continue;;
			}
		}
		if( global_strategy_set[i].target_state ) {
			if( !global_strategy_set[i].target_state( target_state ) ) {
				continue;;
			}
		}
		/* 判断与目标的距离是否符合要求 */
		if( x_width < global_strategy_set[i].range.min_x_width ) {
			continue;
		}
		if( global_strategy_set[i].range.max_x_width != RANGE_MAX ) {
			if( x_width >= global_strategy_set[i].range.max_x_width ) {
				continue;
			}
		}
		if( y_width < global_strategy_set[i].range.min_y_width ) {
			continue;
		}
		if( global_strategy_set[i].range.max_y_width != RANGE_MAX ) {
			if( y_width > global_strategy_set[i].range.max_y_width ) {
				continue;
			}
		}
		/* 记录符合要求的策略的ID */
		strategy_buff[j] = i;
		++j;
	}
	return j;
}

/** 根据各个策略的优先级，随机选择一个策略 */
int RandSelectStrategy( int *strategy_list, int n )
{
	int i;
	DEBUG_MSG("strategy list:\n");
	for( i=0; i<n; ++i) {
		DEBUG_MSG("%d\n", strategy_list[i]);
	}
	i = rand()%n;
	DEBUG_MSG("select strategy: %d\n", i);
	return strategy_list[i];
}

void GamePlayer_Standby( GamePlayer *player )
{
	if( player->state == STATE_LEFTRUN
		|| player->state == STATE_RIGHTRUN ) {
		GamePlayer_StopRun( player );
	}
	player->control.a_attack = FALSE;
	player->control.b_attack = FALSE;
	player->control.left_motion = FALSE;
	player->control.right_motion = FALSE;
	player->control.up_motion = FALSE;
	player->control.down_motion = FALSE;
	player->control.run = FALSE;
}

/** 执行策略 */
void ExecuteStrategy( GamePlayer *player )
{
	int action_type;
	GamePlayer *target;
	AIStrategy *p_strategy;
	int x_width, y_width, n;

	target = GamePlayer_GetTarget( player );
	if( target == NULL ) {
		action_type = ai_action_type_none;
	} else {
		GamePlayer_GetDistance( player, target, &x_width, &y_width );
		p_strategy = &global_strategy_set[player->ai_data.strategy_id];
		action_type = p_strategy->action[player->ai_data.action_num].action_type;
	}
	switch( action_type ) {
	case ai_action_type_wait:
		break;
	case ai_action_type_none:
		GamePlayer_Standby( player );
		DEBUG_MSG("ai_action_type_none\n");
		break;
	case ai_action_type_run_close:
		player->control.run = TRUE;
		if( x_width > 0 ) {
			player->control.left_motion = TRUE;
			player->control.right_motion = FALSE;
		} else {
			player->control.left_motion = FALSE;
			player->control.right_motion = TRUE;
		}
		if( y_width > GLOBAL_Y_WIDTH/2-2 ) {
			player->control.up_motion = TRUE;
			player->control.down_motion = FALSE;
		}
		else if( y_width < -(GLOBAL_Y_WIDTH/2-2) ) {
			player->control.up_motion = FALSE;
			player->control.down_motion = TRUE;
		} else {
			player->control.up_motion = FALSE;
			player->control.down_motion = FALSE;
		}
		DEBUG_MSG("ai_action_type_run_close\n");
		break;
	case ai_action_type_walk_close:
		player->control.run = FALSE;
		if( x_width > 10 ) {
			player->control.left_motion = TRUE;
			player->control.right_motion = FALSE;
		}
		else if( x_width < -10 ) {
			player->control.left_motion = FALSE;
			player->control.right_motion = TRUE;
		} else {
			player->control.left_motion = FALSE;
			player->control.right_motion = FALSE;
		}
		if( y_width > GLOBAL_Y_WIDTH/2-2 ) {
			player->control.up_motion = TRUE;
			player->control.down_motion = FALSE;
		}
		else if( y_width < -(GLOBAL_Y_WIDTH/2-2) ) {
			player->control.up_motion = FALSE;
			player->control.down_motion = TRUE;
		} else {
			player->control.up_motion = FALSE;
			player->control.down_motion = FALSE;
		}
		DEBUG_MSG("ai_action_type_walk_close\n");
		break;
	case ai_action_type_run_away:
	case ai_action_type_walk_away:
		if( action_type == ai_action_type_walk_away ) {
			player->control.run = FALSE;
			DEBUG_MSG("ai_action_type_walk_away\n");
		} else {
			player->control.run = TRUE;
			DEBUG_MSG("ai_action_type_run_away\n");
		}
		if( x_width < -20 ) {
			player->control.left_motion = TRUE;
			player->control.right_motion = FALSE;
		} 
		else if( x_width > 20 ) {
			player->control.left_motion = FALSE;
			player->control.right_motion = TRUE;
		} else {
			/* 随机确定X轴移动方向 */
			n = rand()%2;
			if( n == 0 ) {
				player->control.left_motion = TRUE;
				player->control.right_motion = FALSE;
			} else {
				player->control.left_motion = FALSE;
				player->control.right_motion = TRUE;
			}
		}
		/* 随机确定Y轴移动方向 */
		n = rand()%3;
		if( n == 0 ) {
			player->control.up_motion = FALSE;
			player->control.down_motion = FALSE;
		}
		else if( n == 1 ) {
			player->control.up_motion = FALSE;
			player->control.down_motion = TRUE;
		} else {
			player->control.up_motion = TRUE;
			player->control.down_motion = FALSE;
		}
		break;
	case ai_action_type_observe:
		player->control.left_motion = FALSE;
		player->control.right_motion = FALSE;
		player->control.up_motion = FALSE;
		player->control.down_motion = FALSE;
		if( !player->lock_action
		 && PlayerCanChangeOriented(player->state) ) {
			if( x_width < 0 ) {
				GamePlayer_SetRightOriented( player );
			} 
			else if( x_width > 0 ) {
				GamePlayer_SetLeftOriented( player );
			}
		}
		/* 如果能够自主活动 */
		if( PlayerCanReady(player->state) ) {
			GamePlayer_SetReady( player );
		}
		DEBUG_MSG("ai_action_type_observe\n");
		break;
	case ai_action_type_stop_run:
		DEBUG_MSG("ai_action_type_stop_run\n");
		GamePlayer_StopRun( player );
		break;
	case ai_action_type_a_attack:
		if( !player->lock_action
		 && PlayerCanChangeOriented(player->state) ) {
			if( x_width < 0 ) {
				GamePlayer_SetRightOriented( player );
			} else if( x_width > 0 ) {
				GamePlayer_SetLeftOriented( player );
			}
		}
		player->control.a_attack = TRUE;
		DEBUG_MSG("ai_action_type_a_attack\n");
		break;
	case ai_action_type_b_attack:
		if( !player->lock_action
		 && PlayerCanChangeOriented(player->state) ) {
			if( x_width < 0 ) {
				GamePlayer_SetRightOriented( player );
			} else if( x_width > 0 ) {
				GamePlayer_SetLeftOriented( player );
			}
		}
		player->control.b_attack = TRUE;
		DEBUG_MSG("ai_action_type_b_attack\n");
		break;
	case ai_action_type_jump:
		player->control.jump = TRUE;
		DEBUG_MSG("ai_action_type_jump\n");
		break;
	case ai_action_type_down:
		player->control.down_motion = TRUE;
		DEBUG_MSG("ai_action_type_down\n");
	default:
		break;
	}
}

/** 检查是否还能继续靠近目标 */
LCUI_BOOL CanCloseTarget( GamePlayer *player ) 
{
	int action_type;
	GamePlayer *target;
	AIStrategy *p_strategy;
	int x_width, y_width;
	
	p_strategy = &global_strategy_set[player->ai_data.strategy_id];
	action_type = p_strategy->action[player->ai_data.action_num].action_type;
	if( action_type != ai_action_type_run_close ) {
		return TRUE;
	}
	target = GamePlayer_GetTarget( player );
	if( !target ) {
		return FALSE;
	}
	GamePlayer_GetDistance( player, target, &x_width, &y_width );
	p_strategy = &global_strategy_set[player->ai_data.strategy_id];
	if( player->state == STATE_LEFTRUN ) {
		if( x_width < 0 ) {
			DEBUG_MSG("left run, x_width: %d\n", x_width);
			GamePlayer_StopRun( player );
			return FALSE;
		}
	}
	else if( player->state == STATE_RIGHTRUN ) {
		if( x_width > 0 ) {
			GamePlayer_StopRun( player );
			DEBUG_MSG("right run, x_width: %d\n", x_width);
			return FALSE;
		}
	}
	return TRUE;
}

/** 将游戏玩家交给AI控制 */
void GameAI_Control( int battle_id, int player_id )
{
	GamePlayer *self, *target;
	int x_width, y_width;
	int n, strategy_buff[10];
	
	self = GameBattle_GetPlayer( battle_id, player_id );
	/* 获取自己的目标 */
	target = GamePlayer_GetTarget( self );
	if( !target ) {
		return;
	}
	/* 检测与目标的距离 */
	if( !GamePlayer_GetDistance( self, target, &x_width, &y_width ) ) {
		return;
	}
	DEBUG_MSG("x_width: %d\n", x_width);
	/*如果当前条件不满足当前策略 */
	if( StrategyIsValid(&self->ai_data, target->state, x_width, y_width)
	 && CanCloseTarget(self) ) {
		/* 检查当前动作是否超时 */
		DEBUG_MSG("action: %d\n", self->ai_data.action_num);
		if( StrategyActionIsTimeOut(&self->ai_data) ) {
			DEBUG_MSG("timeout\n");
			++self->ai_data.action_num;
			self->ai_data.start_time = (unsigned int)LCUI_GetTickCount();
			ExecuteStrategy( self );
		}
		return;
	}
	DEBUG_MSG("strategy not valid\n");
	/* 根据当前条件筛选出候选策略 */
	n = FilterStrategy( strategy_buff, 10, self->state, target->state, x_width, y_width );
	DEBUG_MSG("n = %d\n", n);
	if( n <= 0 ) {
		GamePlayer_Standby( self );
		return;
	}
	/* 在候选策略中随机抽选一个策略 */
	n = RandSelectStrategy( strategy_buff, n );
	self->ai_data.action_num = 0;
	self->ai_data.strategy_id = n;
	DEBUG_MSG("strategy_id = %d\n", n);
	self->ai_data.start_time = (unsigned int)LCUI_GetTickCount();
	/* 执行这个策略 */
	ExecuteStrategy( self );
}
