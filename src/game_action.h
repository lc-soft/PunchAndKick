#ifndef __GAME_ACTION_H__
#define __GAME_ACTION_H__

#include "game_object.h"

#define GLOBAL_Y_WIDTH	12
#define MAX_FRAME_NUM	10
#define END_ACTION_FRAME {FALSE,}

enum ActionType {
	ACTION_START = 0,
	ACTION_READY,
	ACTION_STANCE,
	ACTION_WALK,
	ACTION_RUN,
	ACTION_A_ATTACK,
	ACTION_B_ATTACK,
	ACTION_DEFENSE,
	ACTION_SOLID_DEFENSE,
	ACTION_MACH_A_ATTACK,
	ACTION_MACH_B_ATTACK,
	ACTION_JUMP_MACH_A_ATTACK,
	ACTION_JUMP_MACH_B_ATTACK,
	ACTION_AS_ATTACK,
	ACTION_BS_ATTACK,
	ACTION_AJ_ATTACK,
	ACTION_BJ_ATTACK,
	ACTION_ASJ_ATTACK,
	ACTION_BSJ_ATTACK,
	ACTION_FINAL_BLOW,
	ACTION_FALL,
	ACTION_HIT,
	ACTION_HIT_FLY,
	ACTION_HIT_FLY_FALL,
	ACTION_LYING,
	ACTION_LYING_HIT,
	ACTION_TUMMY_HIT_FLY,
	ACTION_TUMMY,
	ACTION_TUMMY_HIT,
	ACTION_REST,
	ACTION_SQUAT,
	ACTION_JUMP,
	ACTION_F_ROLL,
	ACTION_ROLL_DOWN,	/**< 面朝上卷曲 */
	ACTION_ROLL_UP,		/**< 面朝下卷曲 */
	ACTION_B_ROLL,
	ACTION_JUMP_ELBOW,
	ACTION_JUMP_STOMP,
	ACTION_KICK,
	ACTION_GUILLOTINE,
	ACTION_SPINHIT,
	ACTION_BOMBKICK,
	ACTION_MACH_STOMP,
	ACTION_CATCH,
	ACTION_BE_CATCH,
	ACTION_BE_PUSH,
	ACTION_HALF_LYING,
	ACTION_HALF_STANCE,
	ACTION_BACK_BE_CATCH,
	ACTION_CATCH_SKILL_FA,	/**< 正面抓握压制技能 A */
	ACTION_CATCH_SKILL_FB,	/**< 正面抓握压制技能 B */
	ACTION_CATCH_SKILL_BA,	/**< 背面抓握压制技能 A */
	ACTION_CATCH_SKILL_BB,	/**< 背面抓握压制技能 B */
	ACTION_WEAK_RUN,
	ACTION_LIFT_STANCE,
	ACTION_LIFT_WALK,
	ACTION_LIFT_RUN,
	ACTION_LIFT_JUMP,
	ACTION_LIFT_FALL,
	ACTION_THROW,
	ACTION_RIDE,
	ACTION_RIDE_ATTACK,
	ACTION_LYING_DYING,	/**< 躺着，快死了 */
	ACTION_TUMMY_DYING,	/**< 趴着，快死了 */
	TOTAL_ACTION_NUM	/** 动作的总数量 */
};

enum RoleID {
	ROLE_KUNIO,
	ROLE_RIKI,
	ROLE_MIKE,
	ROLE_BEN,
	ROLE_TORAJI,
	TOTAL_ROLE_NUM
};

typedef struct ActionFrameInfo_ {
	LCUI_BOOL enable;
	LCUI_BOOL new_attack;
	LCUI_BOOL horiz_flip;
	char *graph_name;
	int remain_time;
	int offset_x, offset_y;
	RangeBox hit, atk;
} ActionFrameInfo;

typedef struct ActionInfo_ {
	int action_type;
	LCUI_BOOL replay;
	ActionFrameInfo frame[MAX_FRAME_NUM];
} ActionInfo;

int ActionRes_Kunio_GetActionTotalNum(void);
const ActionInfo *ActionRes_Kunio_GetActionSet(void);

int ActionRes_Riki_GetActionTotalNum(void);
const ActionInfo *ActionRes_Riki_GetActionSet(void);

int ActionRes_Mike_GetActionTotalNum(void);
const ActionInfo *ActionRes_Mike_GetActionSet(void);

int ActionRes_Toraji_GetActionTotalNum(void);
const ActionInfo *ActionRes_Toraji_GetActionSet(void);

int ActionRes_Ben_GetActionTotalNum(void);
const ActionInfo *ActionRes_Ben_GetActionSet(void);

/** 载入指定角色的动作动画 */
ActionData* ActionRes_Load( int id, int action_type );
#endif
