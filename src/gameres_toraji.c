#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_GRAPH_H

#include "game.h"
#include "game_resource.h"

#define MAX_ACTION_NUM	45
static const ActionInfo action_set[MAX_ACTION_NUM]={
	/* 步行动作 */
	{ ACTION_WALK, TRUE, {
		{ TRUE, FALSE, FALSE, "walk-01", 5, 0, 2,
			{-16, -GLOBAL_Y_WIDTH/2, 0, 32, GLOBAL_Y_WIDTH, 64},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "walk-02", 5, 0, 0,
			{-16, -GLOBAL_Y_WIDTH/2, 0, 32, GLOBAL_Y_WIDTH, 64},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "walk-03", 5, 0, 2,
			{-16, -GLOBAL_Y_WIDTH/2, 0, 32, GLOBAL_Y_WIDTH, 64},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "walk-04", 5, 0, 0,
			{-16, -GLOBAL_Y_WIDTH/2, 0, 32, GLOBAL_Y_WIDTH, 64},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	} },
	/* 站立动作 */
	{ ACTION_STANCE, TRUE, {
		{ TRUE, FALSE, FALSE, "stance", 100, 0, 0, 
			{-16, -GLOBAL_Y_WIDTH/2, 0, 32, GLOBAL_Y_WIDTH, 64},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 待攻击动作 */
	{ ACTION_READY, TRUE, {
		{ TRUE, FALSE, FALSE, "ready", 50, -6, 0,
			{-16, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 62},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 奔跑动作 */
	{ ACTION_RUN, TRUE, {
		{ TRUE, FALSE, FALSE, "run-01", 3, 0, 0,
			{-24, -GLOBAL_Y_WIDTH/2, 0, 48, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "run-02", 3, 0, 0,
			{-24, -GLOBAL_Y_WIDTH/2, 0, 48, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "run-03", 3, 0, 0,
			{-24, -GLOBAL_Y_WIDTH/2, 0, 48, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "run-04", 3, 0, 0,
			{-24, -GLOBAL_Y_WIDTH/2, 0, 48, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "run-05", 3, 0, 0,
			{-24, -GLOBAL_Y_WIDTH/2, 0, 48, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "run-06", 3, 0, 0,
			{-24, -GLOBAL_Y_WIDTH/2, 0, 48, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* A 攻击动作 */
	{ ACTION_A_ATTACK, FALSE, {
		{ TRUE, FALSE, FALSE, "A-attack-01", 5, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{0,0,0,0,0,0} 
		},
		{ TRUE, TRUE, FALSE, "A-attack-02", 5, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{20, -GLOBAL_Y_WIDTH/2, 31, 26, GLOBAL_Y_WIDTH, 18} 
		},
		END_ACTION_FRAME
	}},
	/* 高速A攻击 动作 */
	{ ACTION_MACH_A_ATTACK, FALSE, {
		{ TRUE, TRUE, FALSE, "A-attack-02", 2, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "A-attack-01", 3, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{20, -GLOBAL_Y_WIDTH/2, 31, 26, GLOBAL_Y_WIDTH, 18} 
		},
		{ TRUE, TRUE, FALSE, "A-attack-02", 2, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "A-attack-01", 3, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{20, -GLOBAL_Y_WIDTH/2, 31, 26, GLOBAL_Y_WIDTH, 18} 
		},
		{ TRUE, TRUE, FALSE, "A-attack-02", 2, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "A-attack-01", 3, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{20, -GLOBAL_Y_WIDTH/2, 31, 26, GLOBAL_Y_WIDTH, 18} 
		},
		{ TRUE, TRUE, FALSE, "A-attack-02", 2, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 高速A攻击 动作 */
	{ ACTION_JUMP_MACH_A_ATTACK, TRUE, {
		{ TRUE, TRUE, FALSE, "A-attack-02", 2, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "A-attack-01", 3, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{20, -GLOBAL_Y_WIDTH/2, 31, 26, GLOBAL_Y_WIDTH, 18} 
		},
		{ TRUE, TRUE, FALSE, "A-attack-02", 2, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "A-attack-01", 3, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{20, -GLOBAL_Y_WIDTH/2, 31, 26, GLOBAL_Y_WIDTH, 18} 
		},
		{ TRUE, TRUE, FALSE, "A-attack-02", 2, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "A-attack-01", 3, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{20, -GLOBAL_Y_WIDTH/2, 31, 26, GLOBAL_Y_WIDTH, 18} 
		},
		{ TRUE, TRUE, FALSE, "A-attack-02", 2, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* B攻击 动作 */
	{ ACTION_B_ATTACK, FALSE, {
		{ TRUE, TRUE, FALSE, "B-attack-01", 5, -11, 0, 
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "B-attack-02", 5, -11, 0, 
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{10, -GLOBAL_Y_WIDTH/2, 0, 25, GLOBAL_Y_WIDTH, 44}
		},
		END_ACTION_FRAME
	}},
	/* 高速B攻击 动作 */
	{ ACTION_MACH_B_ATTACK, FALSE, {
		{ TRUE, TRUE, FALSE, "B-attack-01", 2, -11, 0, 
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "B-attack-02", 3, -11, 0, 
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{10, -GLOBAL_Y_WIDTH/2, 0, 25, GLOBAL_Y_WIDTH, 44}
		},
		{ TRUE, TRUE, FALSE, "B-attack-01", 2, -11, 0, 
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "B-attack-02", 3, -11, 0, 
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{10, -GLOBAL_Y_WIDTH/2, 0, 25, GLOBAL_Y_WIDTH, 44}
		},
		{ TRUE, TRUE, FALSE, "B-attack-01", 2, -11, 0, 
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "B-attack-02", 3, -11, 0, 
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{10, -GLOBAL_Y_WIDTH/2, 0, 25, GLOBAL_Y_WIDTH, 44}
		},
		{ TRUE, TRUE, FALSE, "B-attack-01", 2, -11, 0, 
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 跳跃高速B攻击 动作 */
	{ ACTION_JUMP_MACH_B_ATTACK, TRUE, {
		{ TRUE, TRUE, FALSE, "B-attack-01", 2, -11, 0, 
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "B-attack-02", 3, -11, 0, 
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{10, -GLOBAL_Y_WIDTH/2, 0, 25, GLOBAL_Y_WIDTH, 44}
		},
		{ TRUE, TRUE, FALSE, "B-attack-01", 2, -11, 0, 
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "B-attack-02", 3, -11, 0, 
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{10, -GLOBAL_Y_WIDTH/2, 0, 25, GLOBAL_Y_WIDTH, 44}
		},
		{ TRUE, TRUE, FALSE, "B-attack-01", 2, -11, 0, 
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "B-attack-02", 3, -11, 0, 
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{10, -GLOBAL_Y_WIDTH/2, 0, 25, GLOBAL_Y_WIDTH, 44}
		},
		{ TRUE, TRUE, FALSE, "B-attack-01", 2, -11, 0, 
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* A 冲刺攻击动作 */
	{ ACTION_AS_ATTACK, TRUE, {
		{ TRUE, TRUE, FALSE, "A-sprint-attack-01", 2, 0, 0,
			{-23, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 58},
			{0,0,0,0,0,0} 
		},
		{ TRUE, TRUE, FALSE, "A-sprint-attack-02", 2, 0, 0,
			{-23, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 58},
			{0,0,0,0,0,0} 
		},
		{ TRUE, TRUE, FALSE, "A-sprint-attack-03", 2, 0, 0,
			{-23, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 58},
			{0,0,0,0,0,0} 
		},
		{ TRUE, TRUE, FALSE, "A-sprint-attack-04", 2, 0, 0,
			{-23, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 58},
			{0,0,0,0,0,0} 
		},
		{ TRUE, TRUE, FALSE, "A-sprint-attack-05", 2, 0, 0,
			{-23, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 58},
			{0,0,0,0,0,0} 
		},
		{ TRUE, TRUE, FALSE, "A-sprint-attack-06", 2, 0, 0,
			{-23, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 58},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* A 冲刺跳跃攻击动作 */
	{ ACTION_ASJ_ATTACK, TRUE, {
		{ TRUE, TRUE, FALSE, "A-sprint-attack-01", 2, 0, 0,
			{-23, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 58},
			{0,0,0,0,0,0} 
		},
		{ TRUE, TRUE, FALSE, "A-sprint-attack-02", 2, 0, 0,
			{-23, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 58},
			{0,0,0,0,0,0} 
		},
		{ TRUE, TRUE, FALSE, "A-sprint-attack-03", 2, 0, 0,
			{-23, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 58},
			{0,0,0,0,0,0} 
		},
		{ TRUE, TRUE, FALSE, "A-sprint-attack-04", 2, 0, 0,
			{-23, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 58},
			{0,0,0,0,0,0} 
		},
		{ TRUE, TRUE, FALSE, "A-sprint-attack-05", 2, 0, 0,
			{-23, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 58},
			{0,0,0,0,0,0} 
		},
		{ TRUE, TRUE, FALSE, "A-sprint-attack-06", 2, 0, 0,
			{-23, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 58},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* B 冲刺跳跃攻击动作 */
	{ ACTION_BSJ_ATTACK, TRUE, {
		{ TRUE, TRUE, FALSE, "kick", 10, 0, 0,
			{0,0,0,0,0,0},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* A 跳跃攻击动作 */
	{ ACTION_AJ_ATTACK, FALSE, {
		{ TRUE, TRUE, FALSE, "A-attack-02", 3, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "A-attack-01", 3, -14, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 62},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* B 跳跃攻击动作 */
	{ ACTION_BJ_ATTACK, FALSE, {
		{ TRUE, TRUE, FALSE, "B-attack-02", 5, -11, 0,
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "B-attack-01", 100, -11, 0,
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{10, -GLOBAL_Y_WIDTH/2, 0, 25, GLOBAL_Y_WIDTH, 44},
		},
		END_ACTION_FRAME
	}},
	/* 跳跃动作 */
	{ ACTION_JUMP, TRUE, {
		{ TRUE, FALSE, FALSE, "jump", 100, 0, 0,
			{-16, -GLOBAL_Y_WIDTH/2, 0, 32, GLOBAL_Y_WIDTH, 54},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 下蹲动作 */
	{ ACTION_SQUAT, FALSE, {
		{ TRUE, FALSE, FALSE, "squat", 10, 0, 0,
			{0,0,0,0,0,0},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/** 踢 动作 */
	{ ACTION_KICK, FALSE, {
		{ TRUE, FALSE, FALSE, "kick", 5, -11, 0,
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{10, -GLOBAL_Y_WIDTH/2, 0, 25, GLOBAL_Y_WIDTH, 44}
		},
		END_ACTION_FRAME
	}},
	/* 向前翻滚 动作 */
	{ ACTION_F_ROLL, TRUE, {
		{ TRUE, FALSE, FALSE, "roll-01", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "roll-02", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "roll-03", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "roll-04", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "roll-05", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "roll-06", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "roll-07", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "roll-08", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 向后翻滚 动作 */
	{ ACTION_B_ROLL, TRUE, {
		{ TRUE, FALSE, FALSE, "roll-08", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "roll-07", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "roll-06", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "roll-05", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "roll-04", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "roll-03", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "roll-02", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "roll-01", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 自旋击 动作 */
	{ ACTION_SPINHIT, TRUE, {
		{ TRUE, TRUE, FALSE, "roll-01", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44}
		},
		{ TRUE, FALSE, FALSE, "roll-02", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44}
		},
		{ TRUE, FALSE, FALSE, "roll-03", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44}
		},
		{ TRUE, FALSE, FALSE, "roll-04", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44}
		},
		{ TRUE, FALSE, FALSE, "roll-05", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44}
		},
		{ TRUE, FALSE, FALSE, "roll-06", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44}
		},
		{ TRUE, FALSE, FALSE, "roll-07", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44}
		},
		{ TRUE, FALSE, FALSE, "roll-08", 2, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44},
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 44}
		},
		END_ACTION_FRAME
	}},
	/* 爆裂踢 动作 */
	{ ACTION_BOMBKICK, FALSE, {
		{ TRUE, FALSE, FALSE, "run-04", 100, 0, 0,
			{0, -GLOBAL_Y_WIDTH/2, 0, 24, GLOBAL_Y_WIDTH, 60},
			{-21, -GLOBAL_Y_WIDTH/2, 0, 42, GLOBAL_Y_WIDTH, 60}
		},
		END_ACTION_FRAME
	}},
	/* 高速踩踏 动作 */
	{ ACTION_MACH_STOMP, TRUE, {
		{ TRUE, FALSE, FALSE, "defense", 3, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 51},
			{0,0,0,0}
		},
		{ TRUE, TRUE, FALSE, "stomp", 3, 0, 0,
			{-16, -GLOBAL_Y_WIDTH/2, 0, 32, GLOBAL_Y_WIDTH, 60},
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 51}
		},
		{ TRUE, FALSE, FALSE, "defense", 3, 0, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 51},
			{0,0,0,0}
		},
		{ TRUE, TRUE, TRUE, "stomp", 3, 0, 0,
			{-16, -GLOBAL_Y_WIDTH/2, 0, 32, GLOBAL_Y_WIDTH, 60},
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 51}
		},
		END_ACTION_FRAME
	}},
	/* 终结一击 动作 */
	{ ACTION_FINAL_BLOW, FALSE, {
		{ TRUE, FALSE, FALSE, "squat", 3, -2, 0,
			{-4, -GLOBAL_Y_WIDTH/2, 0, 36, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, TRUE, "squat", 3, -2, 0,
			{-4, -GLOBAL_Y_WIDTH/2, 0, 36, GLOBAL_Y_WIDTH, 60},
			{-4, -GLOBAL_Y_WIDTH/2, 0, 36, GLOBAL_Y_WIDTH, 60},
		},
		{ TRUE, FALSE, FALSE, "final-blow-01", 10, -2, 0,
			{-4, -GLOBAL_Y_WIDTH/2, 0, 36, GLOBAL_Y_WIDTH, 60},
			{-12, -GLOBAL_Y_WIDTH/2, 0, 55, GLOBAL_Y_WIDTH, 64},
		},
		{ TRUE, FALSE, FALSE, "final-blow-02", 10, -2, 0,
			{-4, -GLOBAL_Y_WIDTH/2, 0, 36, GLOBAL_Y_WIDTH, 60},
			{-12, -GLOBAL_Y_WIDTH/2, 0, 55, GLOBAL_Y_WIDTH, 64},
		},
		END_ACTION_FRAME
	}},
	/* B 冲刺攻击动作 */
	{ ACTION_BS_ATTACK, FALSE, {
		{ TRUE, TRUE, FALSE, "B-sprint-attack", 50, 0, 0,
			{-13, -GLOBAL_Y_WIDTH/2, 0, 34, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/** 擒住 动作 */
	{ ACTION_CATCH, FALSE, {
		{ TRUE, FALSE, FALSE, "catch", 5, 0, 0,
			{-19, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 正面压制技能B（拉推） 动作 */
	{ ACTION_CATCH_SKILL_FB, FALSE, {
		{ TRUE, FALSE, FALSE, "pull", 10, 12, 0,
			{13, -GLOBAL_Y_WIDTH/2, 0, 54, GLOBAL_Y_WIDTH, 58},
			{0,0,0,0}
		},
		{ TRUE, FALSE, FALSE, "push", 10, -12, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 56},
			{0,0,0,0}
		},
		END_ACTION_FRAME
	}},
	/* 背面压制技能B（推） 动作 */
	{ ACTION_CATCH_SKILL_BB, FALSE, {
		{ TRUE, FALSE, FALSE, "rest-01", 10, -6, 0,
			{-16, -GLOBAL_Y_WIDTH/2, 0, 40, GLOBAL_Y_WIDTH, 56},
			{0,0,0,0}
		},
		{ TRUE, FALSE, FALSE, "push", 10, -12, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 56},
			{0,0,0,0}
		},
		END_ACTION_FRAME
	}},
	/* 喘气/歇息 动作 */
	{ ACTION_REST, TRUE, {
		{ TRUE, FALSE, FALSE, "rest-01", 25, -6, 0,
			{-16, -GLOBAL_Y_WIDTH/2, 0, 40, GLOBAL_Y_WIDTH, 56},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "rest-02", 25, -6, 0,
			{-16, -GLOBAL_Y_WIDTH/2, 0, 40, GLOBAL_Y_WIDTH, 56},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/** 被擒住 动作 */
	{ ACTION_BE_CATCH, FALSE, {
		{ TRUE, FALSE, FALSE, "rest-01", 5, -6, 0,
			{-16, -GLOBAL_Y_WIDTH/2, 0, 40, GLOBAL_Y_WIDTH, 56},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/** 背面被擒住 动作 */
	{ ACTION_BACK_BE_CATCH, FALSE, {
		{ TRUE, FALSE, FALSE, "back-be-catch", 5, -6, 0,
			{-16, -GLOBAL_Y_WIDTH/2, 0, 40, GLOBAL_Y_WIDTH, 56},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/** 正面压制技能（A） 动作 */
	{ ACTION_CATCH_SKILL_FA, TRUE, {
		{ TRUE, FALSE, FALSE, "spin-up-01", 3, 0, 0,
			{-17, -GLOBAL_Y_WIDTH/2, 0, 50, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "spin-up-02", 3, 0, 0,
			{-17, -GLOBAL_Y_WIDTH/2, 0, 54, GLOBAL_Y_WIDTH, 60},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, TRUE, "spin-up-01", 3, 0, 0,
			{-15, -GLOBAL_Y_WIDTH/2, 8, 64, GLOBAL_Y_WIDTH, 50},
			{0,0,0,0,0,0} 
		},
		{ TRUE, FALSE, FALSE, "spin-up-03", 3, 0, 0,
			{-30, -GLOBAL_Y_WIDTH/2, 0, 64, GLOBAL_Y_WIDTH, 50},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 虚弱奔跑 动作 */
	{ ACTION_WEAK_RUN, TRUE, {
		{ TRUE, FALSE, FALSE, "weak-walk", 10, -6, 2,
			{-16, -GLOBAL_Y_WIDTH/2, 0, 40, GLOBAL_Y_WIDTH, 56},
			{0,0,0,0}
		},
		{ TRUE, FALSE, FALSE, "rest-01", 10, -6, 0,
			{-22, -GLOBAL_Y_WIDTH/2, 0, 44, GLOBAL_Y_WIDTH, 56},
			{0,0,0,0}
		},
		END_ACTION_FRAME
	}},
	/* 投掷/抛 动作 */
	{ ACTION_THROW, FALSE, {
		{ TRUE, FALSE, FALSE, "push", 10, -11, 0,
			{-12, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 56},
			{0,0,0,0}
		},
		END_ACTION_FRAME
	}},
	/* 被击中 动作 */
	{ ACTION_HIT, FALSE, {
		{ TRUE, FALSE, FALSE, "hit", 10, 0, 0,
			{-23, -GLOBAL_Y_WIDTH/2, 0, 46, GLOBAL_Y_WIDTH, 62},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 被击飞 动作 */
	{ ACTION_HIT_FLY, FALSE, {
		{ TRUE, FALSE, FALSE, "hit-fly", 100, -6, 0,
			{-18, -GLOBAL_Y_WIDTH/2, 0, 38, GLOBAL_Y_WIDTH, 64},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 被击飞（头朝地坠落） 动作 */
	{ ACTION_HIT_FLY_FALL, FALSE, {
		{ TRUE, FALSE, FALSE, "hit-fly-fall", 100, 0, 0,
			{-24, -GLOBAL_Y_WIDTH/2, 0, 32, GLOBAL_Y_WIDTH, 50},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 降落 动作 */
	{ ACTION_FALL, FALSE, {
		{ TRUE, FALSE, FALSE, "fall", 100, 0, 0,
			{-24, -GLOBAL_Y_WIDTH/2, 0, 32, GLOBAL_Y_WIDTH, 50},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 躺地动作 */
	{ ACTION_LYING, FALSE, {
		{ TRUE, FALSE, FALSE, "lying", 100, 0, 0,
			{-32, -GLOBAL_Y_WIDTH/2, 0, 64, GLOBAL_Y_WIDTH, 26},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 躺着被击中 动作 */
	{ ACTION_LYING_HIT, FALSE, {
		{ TRUE, FALSE, FALSE, "lying-hit", 10, 0, 0,
			{-32, -GLOBAL_Y_WIDTH/2, 0, 64, GLOBAL_Y_WIDTH, 26},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 趴地动作 */
	{ ACTION_TUMMY, FALSE, {
		{ TRUE, FALSE, FALSE, "tummy", 100, 0, 0,
			{-32, -GLOBAL_Y_WIDTH/2, 0, 64, GLOBAL_Y_WIDTH, 26},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 趴着被击中 动作 */
	{ ACTION_TUMMY_HIT, FALSE, {
		{ TRUE, FALSE, FALSE, "tummy-hit", 10, 0, 0,
			{-32, -GLOBAL_Y_WIDTH/2, 0, 64, GLOBAL_Y_WIDTH, 26},
			{0,0,0,0,0,0} 
		},
		END_ACTION_FRAME
	}},
	/* 躺着濒死 动作 */
	{ ACTION_LYING_DYING, TRUE, {
		{ TRUE, FALSE, FALSE, "lying", 5, 0, 0,
			{0,0,0,0},
			{0,0,0,0}
		},
		{ TRUE, FALSE, FALSE, "lying-hit", 5, 0, 0,
			{0,0,0,0},
			{0,0,0,0}
		},
		END_ACTION_FRAME
	}},
	/* 趴着濒死 动作 */
	{ ACTION_TUMMY_DYING, TRUE, {
		{ TRUE, FALSE, FALSE, "tummy", 5, 0, 0,
			{0,0,0,0},
			{0,0,0,0}
		},
		{ TRUE, FALSE, FALSE, "tummy-hit", 5, 0, 0,
			{0,0,0,0},
			{0,0,0,0}
		},
		END_ACTION_FRAME
	}},
	/* 半躺 动作 */
	{ ACTION_HALF_LYING, FALSE, {
		{ TRUE, FALSE, FALSE, "half-lying", 50, 0, 0,
			{0,0,0,0},
			{0,0,0,0}
		},
		END_ACTION_FRAME
	}}
};

int ActionRes_Toraji_GetActionTotalNum(void)
{
	return MAX_ACTION_NUM;
}

const ActionInfo *ActionRes_Toraji_GetActionSet(void)
{
	return action_set;
}
