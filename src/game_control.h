
typedef struct ControlKey_ {
	int up, down, left, right, a_attack, b_attack, jump;
} ControlKey;

enum GamePlayerState {
	STATE_READY,
	STATE_STANCE,	/**< 站立 */
	STATE_WALK,	/**< 步行 */
	STATE_LEFTRUN,	/**< 向左奔跑 */
	STATE_RIGHTRUN,	/**< 向右奔跑 */
	STATE_A_ATTACK,
	STATE_B_ATTACK,
	STATE_AS_ATTACK,
	STATE_BS_ATTACK,
	STATE_AJ_ATTACK,
	STATE_BJ_ATTACK,
	STATE_ASJ_ATTACK,
	STATE_BSJ_ATTACK,
	STATE_FINAL_BLOW,
	STATE_SQUAT,
	STATE_SSQUAT,
	STATE_JUMP,
	STATE_SJUMP,
	STATE_HIT,	/**< 被命中 */
	STATE_REST	/**< 歇息 */
};

enum ActionType {
	ACTION_READY,
	ACTION_STANCE,
	ACTION_WALK,
	ACTION_RUN,
	ACTION_A_ATTACK,
	ACTION_B_ATTACK,
	ACTION_AS_ATTACK,
	ACTION_BS_ATTACK,
	ACTION_AJ_ATTACK,
	ACTION_BJ_ATTACK,
	ACTION_ASJ_ATTACK,
	ACTION_BSJ_ATTACK,
	ACTION_FINAL_BLOW,
	ACTION_HIT,
	ACTION_REST,
	ACTION_SQUAT,
	ACTION_JUMP
};

typedef struct GamePlayer_ GamePlayer;
struct GamePlayer_{
	int id;				/**< 玩家ID */
	int role_id;			/**< 角色ID */
	int state;			/**< 状态 */
	LCUI_BOOL enable;		/**< 是否启用该角色 */
	LCUI_BOOL right_direction;	/**< 角色是否朝着右方 */
	LCUI_BOOL human_control;	/**< 是否由人类控制 */
	LCUI_BOOL local_control;	/**< 是否由此处玩家控制 */
	LCUI_BOOL lock_action;		/**< 是否锁定动作 */
	LCUI_BOOL lock_motion;		/**< 是否锁定移动 */
	int walk_speed;			/**< 步行的速度 */
	int run_speed;			/**< 奔跑的速度 */
	int n_attack;			/**< 被攻击的次数 */
	int timer;			/**< 用于重置被攻击次数的定时器 */
	LCUI_Widget *object;		/**< 游戏对象 */
	ControlKey ctrlkey;		/**< 该角色的控制键 */
};

extern int Game_Init(void);
extern int Game_Start(void);
extern int Game_Pause(void);

/** 改变角色的动作动画  */
extern void GamePlayer_ChangeAction( GamePlayer *player, int action_id );

/** 通过控制键获取该键控制的角色 */
GamePlayer *GamePlayer_GetPlayerByControlKey( int key_code );

/** 通过角色ID来获取角色 */
GamePlayer *GamePlayer_GetByID( int player_id );

/** 改变角色的动作动画  */
void GamePlayer_ChangeAction( GamePlayer *player, int action_id );

void GamePlayer_ChangeState( GamePlayer *player, int state );

void GamePlayer_LockAction( GamePlayer *player );

void GamePlayer_UnlockAction( GamePlayer *player );

void GamePlayer_LockMotion( GamePlayer *player );

void GamePlayer_UnlockMotion( GamePlayer *player );

void GamePlayer_SetLeftWalk( GamePlayer *player );

void GamePlayer_SetRightWalk( GamePlayer *player );

void GamePlayer_SetLeftRun( GamePlayer *player );

void GamePlayer_SetRightRun( GamePlayer *player );

/** 停止奔跑 */
void GamePlayer_StopRun( GamePlayer *player );

void GamePlayer_StopXWalk( GamePlayer *player );

void GamePlayer_StopYMotion( GamePlayer *player );

void GamePlayer_StartAAttack( GamePlayer *player );

void GamePlayer_SetUpMotion( GamePlayer *player );

void GamePlayer_SetDownMotion( GamePlayer *player );
