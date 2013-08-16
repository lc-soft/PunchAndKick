#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H

#include <time.h>
#include "game.h"

/** 随机想法 */
typedef struct RandIdea_ {
	int id;			/**< 该想法的标识号 */
	int probability;	/**< 该想法出现的概率，取值范围为0(0.0%)至10000(100%) */
} RandIdea;


/** 获取自己的攻击目标 */
static GamePlayer *GamePlayer_GetTarget( GamePlayer *self )
{
	return NULL;
}

/** 获取自己与对方的距离 */
static void GamePlayer_GetDistance( GamePlayer *self, GamePlayer *target, int *x_width, int *y_width )
{

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
	
	num = rand()%10000;
	total_amount = 10000;
	idea_amount = (double*)malloc( n_idea *sizeof(double) );
	idea_pos = (int*)malloc( n_idea *sizeof(int) );
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

/** 实现AI的想法 */
static void GameAI_RealizeIdea( GamePlayer *player )
{

}

/** 初始化游戏AI */
void GameAI_Init(void)
{
	srand((unsigned int)time(NULL));
}

/** 将游戏玩家交给AI控制 */
void GameAI_Control( int player_id )
{
	GamePlayer *self, *target;
	int x_width, y_width;
	RandIdea idea[10];
	
	self = GamePlayer_GetByID( player_id );
	/* 获取自己的目标 */
	target = GamePlayer_GetTarget( self );
	/* 检测与目标的距离 */
	GamePlayer_GetDistance( self, target, &x_width, &y_width );
	/* 根据自己的当前状态，决定该干什么 */
	switch( self->state ) {
	case STATE_READY:
	case STATE_STANCE:
	case STATE_WALK:
		if( x_width > 150 ) {
			/* 有75%的概率想靠近目标 */
			idea[0].id = idea_type_close;
			idea[0].probability = 7500;
			/* 其余的概率是站着观察 */
			idea[1].id = idea_type_observe;	
			/* 决定想法 */
			self->idea.type = RandIdea_Select( idea, 2 );
			/* 有80%的概率是跑着靠近目标 */
			idea[0].id = 0;
			idea[0].probability = 8000;
			/* 剩余概率是步行靠近目标 */
			idea[1].id = 1;
			if( RandIdea_Select( idea, 2 ) == 0 ) {
				self->idea.close.need_run = TRUE;
			} else {
				self->idea.close.need_run = FALSE;
			}
			self->idea.close.target_id = target->id;
		}
		else if( x_width > 20 && x_width < 50 ) {
			/* 有65%的概率想对目标进行A攻击 */
			idea[0].id = idea_type_a_attack;
			idea[0].probability = 4500;
			/* 有35%的概率想对目标进行B攻击 */
			idea[1].id = idea_type_b_attack;
			idea[1].probability = 3500;
			/* 其余的概率是想远离目标 */
			idea[1].id = idea_type_away;
			self->idea.type = RandIdea_Select( idea, 3 );
			/* 如果是决定远离目标 */
			if( self->idea.type == idea_type_away ) {
				idea[0].id = 0;
				idea[0].probability = 4000;
				idea[1].id = 1;
				if( RandIdea_Select( idea, 2 ) == 0 ) {
					self->idea.away.need_run = TRUE;
				} else {
					self->idea.away.need_run = FALSE;
				}
			}
		}
		else if( x_width < 20 && x_width >= 0 ) {
			/* 有50%的概率想对目标进行攻击 */
			idea[0].id = idea_type_a_attack;
			idea[1].id = idea_type_b_attack;
			idea[0].probability = 2500;
			idea[1].probability = 2500;
			/* 有50%的概率是想远离目标 */
			idea[1].id = idea_type_away;
			self->idea.type = RandIdea_Select( idea, 3 );
			if( self->idea.type == idea_type_away ) {
				idea[0].id = 0;
				idea[0].probability = 4000;
				idea[1].id = 1;
				if( RandIdea_Select( idea, 2 ) == 0 ) {
					self->idea.away.need_run = TRUE;
				} else {
					self->idea.away.need_run = FALSE;
				}
			}
		}
		/* 如果与目标在Y轴的距离小 */
		if( y_width < GLOBAL_Y_WIDTH ) {
			break;
		}
		/* 如果想法是攻击目标 */
		if( self->idea.type == idea_type_a_attack
		|| self->idea.type == idea_type_a_attack ) {
			idea[0].id = idea_type_close;
			idea[1].id = idea_type_observe;	
			idea[2].id = idea_type_away;
			idea[0].probability = 6000;
			idea[1].probability = 2000;
			idea[2].probability = 2000;
			self->idea.type = RandIdea_Select( idea, 3 );
			idea[0].id = 0;
			idea[0].probability = 3000;
			idea[1].id = 1;
			switch(self->idea.type) {
			case idea_type_away:
				if( RandIdea_Select( idea, 2 ) == 0 ) {
					self->idea.away.need_run = TRUE;
				} else {
					self->idea.away.need_run = FALSE;
				}
				self->idea.away.target_id = target->id;
				break;
			case idea_type_close:
				if( RandIdea_Select( idea, 2 ) == 0 ) {
					self->idea.close.need_run = TRUE;
				} else {
					self->idea.close.need_run = FALSE;
				}
				self->idea.close.target_id = target->id;
			default:break;
			}
		}
	case STATE_LEFTRUN:
		if( x_width < 0 ) {
			self->idea.type = idea_type_stop_run;
		}
		else if( x_width > 420 ) {
			//继续跑吧
		}
		else if( (x_width >= 400 && x_width <= 420 )
		 || ( x_width >= 280 && x_width <= 320) ) {
			self->idea.type = idea_type_jump;
		}
		else if( x_width <= 150 && x_width > 50 ) {
			idea[0].id = idea_type_a_attack;
			idea[1].id = idea_type_b_attack;
			idea[0].probability = 5000;
			idea[1].probability = 5000;
			self->idea.type = RandIdea_Select( idea, 2 );
		}
		break;
	case STATE_RIGHTRUN:
		if( x_width > 0 ) {
			self->idea.type = idea_type_stop_run;
		}
		else if( x_width < -420 ) {
			//继续跑吧
		}
		else if( (x_width < -400 && x_width > -420 )
		 || ( x_width < -280 && x_width > -320) ) {
			self->idea.type = idea_type_jump;
		}
		else if( x_width > -150 && x_width < -50 ) {
			idea[0].id = idea_type_a_attack;
			idea[1].id = idea_type_b_attack;
			idea[0].probability = 5000;
			idea[1].probability = 5000;
			self->idea.type = RandIdea_Select( idea, 2 );
		}
		break;
	default:
		break;
	}
	GameAI_RealizeIdea( self );
}
