
#ifndef __GAME_AI_H__
#define __GAME_AI_H__

enum AIIdeaType {
	idea_type_none,
	idea_type_close,	/**< 靠近目标 */
	idea_type_away,		/**< 远离目标 */
	idea_type_observe,	/**< 观察目标 */
	idea_type_stop_run,	/**< 停止奔跑 */
	idea_type_a_attack,	/**< A攻击 */
	idea_type_b_attack,	/**< B攻击 */
	idea_type_jump		/**< 跳跃 */
};

typedef struct AIIdeaData_Move_ {
	int type;
	int target_id;
	LCUI_BOOL need_run;
} AIIdeaData_Close, AIIdeaData_Away;

typedef union AIIdeaData_ {
	int type;
	AIIdeaData_Close close;
	AIIdeaData_Away away;
} AIIdeaData;


/** 初始化游戏AI */
void GameAI_Init(void);

/** 将游戏玩家交给AI控制 */
void GameAI_Control( int player_id );

#endif
