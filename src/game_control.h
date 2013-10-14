#ifndef __GAME_CONTROL_H__
#define __GAME_CONTROL_H__

#include "game_AI.h"

typedef struct ControlKey_ {
	int up, down, left, right, a_attack, b_attack, defense, jump;
} ControlKey;

enum GamePlayerType {
	PLAYER_TYPE_FIGHTER,
	PLAYER_TYPE_MARTIAL_ARTIST,
	PLAYER_TYPE_KUNG_FU,
	PLAYER_TYPE_JUDO_MASTER,
	PLAYER_TYPE_TIGER
};

enum GamePlayerState {
	STATE_START = 0,
	STATE_DIED,		/**< 已经死亡 */
	STATE_LYING_DYING,	/**< 躺着，快死了 */
	STATE_TUMMY_DYING,	/**< 趴着，快死了 */
	STATE_READY,
	STATE_STANCE,		/**< 站立 */
	STATE_WALK,		/**< 步行 */
	STATE_LEFTRUN,		/**< 向左奔跑 */
	STATE_RIGHTRUN,		/**< 向右奔跑 */
	STATE_A_ATTACK,
	STATE_B_ATTACK,
	STATE_DEFENSE,
	STATE_SOLID_DEFENSE,
	STATE_MACH_A_ATTACK,
	STATE_MACH_B_ATTACK,
	STATE_AS_ATTACK,
	STATE_BS_ATTACK,
	STATE_AJ_ATTACK,
	STATE_BJ_ATTACK,
	STATE_MAJ_ATTACK,
	STATE_MBJ_ATTACK,
	STATE_ASJ_ATTACK,
	STATE_BSJ_ATTACK,
	STATE_FINAL_BLOW,
	STATE_SQUAT,
	STATE_JSQUAT,
	STATE_SSQUAT,
	STATE_JUMP,
	STATE_SJUMP,
	STATE_JUMP_DONE,
	STATE_FALL,
	STATE_HIT,		/**< 被命中 */
	STATE_HIT_FLY,		/**< 被击飞 */
	STATE_HIT_FLY_FALL,	/**< 被击飞（头朝地） */
	STATE_LYING,
	STATE_LYING_HIT,
	STATE_TUMMY,
	STATE_TUMMY_HIT,
	STATE_REST,		/**< 歇息 */
	STATE_F_ROLL,		/**< 向前翻滚 */
	STATE_B_ROLL,		/**< 向后翻滚 */
	STATE_ROLL_UP,
	STATE_ROLL_DOWN,
	STATE_ELBOW,
	STATE_JUMP_ELBOW,
	STATE_JUMP_STOMP,
	STATE_KICK,
	STATE_GUILLOTINE,
	STATE_BIG_ELBOW,
	STATE_SPINHIT,
	STATE_BOMBKICK,
	STATE_MACH_STOMP,
	STATE_CATCH,
	STATE_BE_CATCH,
	STATE_BE_PUSH,
	STATE_HALF_LYING,
	STATE_HALF_STANCE,
	STATE_BACK_BE_CATCH,
	STATE_CATCH_SKILL_FA,
	STATE_CATCH_SKILL_FB,
	STATE_CATCH_SKILL_BA,
	STATE_CATCH_SKILL_BB,
	STATE_WEAK_RUN,
	STATE_WEAK_RUN_ATTACK,
	STATE_LIFT_STANCE,		/**< 举着，站立 */
	STATE_LIFT_WALK,		/**< 举着，行走 */
	STATE_LIFT_RUN,			/**< 举着，奔跑 */
	STATE_LIFT_JUMP,		/**< 举着，跳起 */
	STATE_LIFT_FALL,		/**< 举着，下落 */
	STATE_LIFT_SQUAT,
	STATE_BE_LIFT_STANCE,		/**< 被举起，站立 */
	STATE_BE_LIFT_SQUAT,		/**< 被举起，蹲着 */
	STATE_BE_LIFT_LYING,		/**< 被举起，躺着 */
	STATE_BE_LIFT_TUMMY,		/**< 被举起，趴着 */
	STATE_BE_LIFT_LYING_HIT,	/**< 被举起，躺着挨打 */
	STATE_BE_LIFT_TUMMY_HIT,	/**< 被举起，趴着挨打 */
	STATE_THROW,
	STATE_RIDE,
	STATE_RIDE_ATTACK,
	STATE_RIDE_JUMP,
	TOTAL_STATE_NUM			/**< 状态的总数量 */
};

typedef struct GamePlayerProperty_ {
	int max_hp;	/**< 最大生命值 */
	int cur_hp;	/**< 当前生命值 */
	int punch;	/**< 拳击的伤害 */
	int kick;	/**< 踢的伤害 */
	int speed;	/**< 移动速度 */
	int throw;	/**< 投掷伤害 */
	int defense;	/**< 防御力 */
} GamePlayerProperty;

typedef struct ControlState_ {
	LCUI_BOOL run;			/**< 是否奔跑 */
	LCUI_BOOL left_motion;		/**< 是否向左移动 */
	LCUI_BOOL right_motion;		/**< 是否向右移动 */
	LCUI_BOOL up_motion;		/**< 是否向上移动 */
	LCUI_BOOL down_motion;		/**< 是否向下移动 */
	LCUI_BOOL a_attack;		/**< 是否进行A攻击 */
	LCUI_BOOL b_attack;		/**< 是否进行B攻击 */
	LCUI_BOOL jump;			/**< 是否进行跳跃 */
	LCUI_BOOL defense;		/**< 是否进行防御 */
} ControlState;

#define MAX_SKILL_NUM 10

typedef struct RoleInfo_ {
	int role_id;				/**< 角色ID */
	wchar_t name[12];			/**< 角色名 */
	int type;				/**< 角色类型 */
	GamePlayerProperty property;		/**< 角色的各项属性 */
	char skills[MAX_SKILL_NUM][256];	/**< 拥有的技能 */
	int total_skill;			/**< 技能总数 */
} RoleInfo;

typedef struct GamePlayer_ GamePlayer;
struct GamePlayer_{
	int state;			/**< 状态 */
	int id;				/**< 玩家ID */
	int role_id;			/**< 角色ID */
	int type;			/**< 角色类型 */
	GamePlayerProperty property;	/**< 游戏角色属性 */
	LCUI_Queue skills;		/**< 已经启用的技能 */
	LCUI_BOOL enable;		/**< 是否启用该角色 */
	LCUI_BOOL right_direction;	/**< 角色是否朝着右方 */
	LCUI_BOOL human_control;	/**< 是否由人类控制 */
	LCUI_BOOL local_control;	/**< 是否由此处玩家控制 */
	LCUI_BOOL lock_action;		/**< 是否锁定动作 */
	LCUI_BOOL lock_motion;		/**< 是否锁定移动 */
	char attack_type_name[64];	/**< 当前的攻击类型 */
	int n_attack;			/**< 被攻击的次数 */
	int t_rest_timeout;		/**< 定时器，用于限定休息时间 */
	int t_action_timeout;		/**< 定时器，用于处理动作超时 */
	int t_death_timer;		/**< 死亡定时器 */
	LCUI_Widget *object;		/**< 游戏对象 */
	LCUI_Widget *statusbar;		/**< 状态栏 */
	ControlKey ctrlkey;		/**< 该角色的控制键 */
	GamePlayer *other;
	GameAI_Data ai_data;		/**< 记录AI的数据 */
	ControlState control;		/**< 角色的控制状态 */
};

extern int Game_Init(void);
extern int Game_Start(void);
extern int Game_Loop(void);
extern int Game_Pause(void);

/** 通过部件获取游戏玩家数据 */
extern GamePlayer *GamePlayer_GetPlayerByWidget( LCUI_Widget *widget );

/** 重置攻击控制 */
extern void GamePlayer_ResetAttackControl( GamePlayer *player );

extern void GamePlayer_SetReady( GamePlayer *player );

/** 设置角色面向右方 */
void GamePlayer_SetRightOriented( GamePlayer *player );

/** 设置角色面向左方 */
void GamePlayer_SetLeftOriented( GamePlayer *player );

/** 判断角色是否面向左方 */
LCUI_BOOL GamePlayer_IsLeftOriented( GamePlayer *player );

/** 改变角色的动作动画  */
extern void GamePlayer_ChangeAction( GamePlayer *player, int action_id );

/** 通过控制键获取该键控制的角色 */
GamePlayer *GamePlayer_GetPlayerByControlKey( int key_code );

/** 通过角色ID来获取角色 */
GamePlayer *GamePlayer_GetByID( int player_id );

/** 为游戏角色的动作设置时限，并在超时后进行响应 */
void GamePlayer_SetActionTimeOut(	GamePlayer *player,
					int n_ms,
					void (*func)(GamePlayer*) );

/** 打断休息 */
void GamePlayer_BreakRest( GamePlayer *player );

/** 检测玩家是否处于举起状态 */
LCUI_BOOL GamePlayer_IsInLiftState( GamePlayer *player );

/** 为游戏设置休息的时限，并在超时后进行响应 */
void GamePlayer_SetRestTimeOut(	GamePlayer *player,
				int n_ms,
				void (*func)(GamePlayer*) );

/** 设置游戏角色当前的攻击类型名称 */
void GamePlayer_SetAttackTypeName( GamePlayer *player, const char *attack_type_name );

/** 按百分比变更移动速度，n 取值范围为 0 ~ 100 */
void GamePlayer_ReduceSpeed( GamePlayer *player, int n );

int GamePlayer_SetLying( GamePlayer *player );

int GamePlayer_SetTummy( GamePlayer *player );

/** 开始站起 */
void GamePlayer_StartStand( GamePlayer *player );

void GamePlayer_SetRightHitFly( GamePlayer *player );

void GamePlayer_SetLeftHitFly( GamePlayer *player );

/** 抓住正处于喘气（歇息）状态下的玩家 */
GamePlayer* GamePlayer_CatchGaspingPlayer( GamePlayer *player );

/** 设置为歇息状态 */
void GamePlayer_SetRest( GamePlayer *player );

/** 改变角色的动作动画  */
void GamePlayer_ChangeAction( GamePlayer *player, int action_id );

void GamePlayer_ChangeState( GamePlayer *player, int state );

void GamePlayer_LockAction( GamePlayer *player );

void GamePlayer_UnlockAction( GamePlayer *player );

void GamePlayer_LockMotion( GamePlayer *player );

void GamePlayer_UnlockMotion( GamePlayer *player );

/** 停止奔跑 */
void GamePlayer_StopRun( GamePlayer *player );

void GamePlayer_StopXWalk( GamePlayer *player );

void GamePlayer_StopYMotion( GamePlayer *player );

void GamePlayer_StopXMotion( GamePlayer *player );

void GamePlayer_SetUpMotion( GamePlayer *player );

void GamePlayer_SetDownMotion( GamePlayer *player );

/** 设置游戏角色的控制键 */
int Game_SetGamePlayerControlKey( int player_id, ControlKey *key );

/** 获取角色信息 */
RoleInfo *Game_GetRoleInfo( int role_id );

/** 设置游戏角色 */
int Game_SetGamePlayer( int id, int role_id, LCUI_BOOL human_control );

/** 启用一个游戏角色 */
void Game_EnableGamePlayer( int id );

/** 初始化对战 */
int Game_InitBattle(void);

/** 开始对战 */
void Game_StartBattle( void );

#endif
