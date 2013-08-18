
#ifndef __GAME_AI_H__
#define __GAME_AI_H__

typedef struct GameAI_Data_ {
	int strategy_id;	/**< 策略的ID */
	int action_num;		/**< 当前正执行的动作序号 */
	int start_time;		/**< 动作开始时的时间 */
} GameAI_Data;

/** 初始化游戏AI */
void GameAI_Init(void);

/** 将游戏玩家交给AI控制 */
void GameAI_Control( int player_id );

#endif
