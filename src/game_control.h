#ifndef __GAME_CONTROL_H__
#define __GAME_CONTROL_H__

typedef struct ControlKey_ {
	int up, down, left, right, a_attack, b_attack, jump;
} ControlKey;

enum GamePlayerType {
	PLAYER_TYPE_FIGHTER,
	PLAYER_TYPE_MARTIAL_ARTISTS,
	PLAYER_TYPE_KUNG_FU,
	PLAYER_TYPE_JUDO_MASTERS
};

enum GamePlayerState {
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
	STATE_AS_ATTACK,
	STATE_BS_ATTACK,
	STATE_AJ_ATTACK,
	STATE_BJ_ATTACK,
	STATE_ASJ_ATTACK,
	STATE_BSJ_ATTACK,
	STATE_FINAL_BLOW,
	STATE_SQUAT,
	STATE_JSQUAT,
	STATE_SSQUAT,
	STATE_JUMP,
	STATE_SJUMP,
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
	STATE_ELBOW,
	STATE_JUMP_ELBOW,
	STATE_JUMP_STOMP,
	STATE_KICK,
	STATE_SPINHIT,
	STATE_BOMBKICK,
	STATE_MACH_STOMP,
	STATE_CATCH,
	STATE_BE_CATCH,
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
	STATE_BE_LIFT_STANCE,		/**< 被举起，站立 */
	STATE_BE_LIFT_SQUAT,		/**< 被举起，蹲着 */
	STATE_BE_LIFT_LYING,		/**< 被举起，躺着 */
	STATE_BE_LIFT_TUMMY,		/**< 被举起，趴着 */
	STATE_BE_LIFT_LYING_HIT,	/**< 被举起，躺着挨打 */
	STATE_BE_LIFT_TUMMY_HIT,	/**< 被举起，趴着挨打 */
	STATE_THROW,
	STATE_RIDE,
	STATE_RIDE_ATTACK,
	STATE_RIDE_JUMP
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
	ACTION_HIT_FLY,
	ACTION_HIT_FLY_FALL,
	ACTION_LYING,
	ACTION_LYING_HIT,
	ACTION_TUMMY,
	ACTION_TUMMY_HIT,
	ACTION_REST,
	ACTION_SQUAT,
	ACTION_JUMP,
	ACTION_F_ROLL,
	ACTION_B_ROLL,
	ACTION_ELBOW,
	ACTION_JUMP_ELBOW,
	ACTION_JUMP_STOMP,
	ACTION_KICK,
	ACTION_SPINHIT,
	ACTION_BOMBKICK,
	ACTION_MACH_STOMP,
	ACTION_CATCH,
	ACTION_BE_CATCH,
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
};

/** 攻击类型 */
enum AttackType {
	ATTACK_TYPE_NONE,
	ATTACK_TYPE_PUNCH,
	ATTACK_TYPE_KICK,
	ATTACK_TYPE_RIDE_ATTACK,
	ATTACK_TYPE_RIDE_JUMP_ATTACK,
	ATTACK_TYPE_THROW,
	ATTACK_TYPE_BUMPED,
	ATTACK_TYPE_MACH_STOMP,
	ATTACK_TYPE_ELBOW1,
	ATTACK_TYPE_ELBOW2,
	ATTACK_TYPE_KNEE_HIT1,
	ATTACK_TYPE_KNEE_HIT2,
	ATTACK_TYPE_JUMP_ELBOW,
	ATTACK_TYPE_JUMP_TREAD,
	ATTACK_TYPE_JUMP_KICK,
	ATTACK_TYPE_JUMP_PUNCH,
	ATTACK_TYPE_S_PUNCH,
	ATTACK_TYPE_S_KICK,
	ATTACK_TYPE_SJUMP_KICK,
	ATTACK_TYPE_SJUMP_PUNCH,
	ATTACK_TYPE_BIG_ELBOW,
	ATTACK_TYPE_GUILLOTINE,
	ATTACK_TYPE_SPIN_HIT,
	ATTACK_TYPE_SPIN_HIT2,
	ATTACK_TYPE_BOMB_KICK,
	ATTACK_TYPE_JUMP_SPIN_KICK,
	ATTACK_TYPE_FINAL_BLOW
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

typedef struct SpecialSkill_ {
	LCUI_BOOL rock_defense;		/**< 磐石防御 */
	LCUI_BOOL mach_punch;		/**< 高速拳 */
	LCUI_BOOL mach_kick;		/**< 高速踢 */
	LCUI_BOOL mach_stomp;		/**< 高速跺脚 */
	LCUI_BOOL tornado_attack;	/**< 龙卷攻击 */
	LCUI_BOOL big_elbow;		/**< 跳跃肘压 */
	LCUI_BOOL guillotine;		/**< 断头台 */
	LCUI_BOOL spin_hit;		/**< 翻滚击 */
	LCUI_BOOL bomb_kick;		/**< 爆裂踢 */
	LCUI_BOOL jump_spin_kick;	/**< 高跳旋转落踢 */
} SpecialSkill;

typedef struct GamePlayer_ GamePlayer;
struct GamePlayer_{
	int id;				/**< 玩家ID */
	int role_id;			/**< 角色ID */
	int type;			/**< 角色类型 */
	int state;			/**< 状态 */
	LCUI_BOOL enable;		/**< 是否启用该角色 */
	LCUI_BOOL right_direction;	/**< 角色是否朝着右方 */
	LCUI_BOOL human_control;	/**< 是否由人类控制 */
	LCUI_BOOL local_control;	/**< 是否由此处玩家控制 */
	LCUI_BOOL lock_action;		/**< 是否锁定动作 */
	LCUI_BOOL lock_motion;		/**< 是否锁定移动 */
	GamePlayerProperty property;	/**< 角色的各项属性 */
	SpecialSkill skill;		/**< 角色拥有的特殊技能 */
	int attack_type;		/**< 当前的攻击类型 */
	int n_attack;			/**< 被攻击的次数 */
	int t_rest_timeout;		/**< 定时器，用于限定休息时间 */
	int t_action_timeout;		/**< 定时器，用于处理动作超时 */
	int t_death_timer;		/**< 死亡定时器 */
	LCUI_Widget *object;		/**< 游戏对象 */
	LCUI_Widget *statusbar;		/**< 状态栏 */
	ControlKey ctrlkey;		/**< 该角色的控制键 */
	GamePlayer *other;
	AIIdeaData idea;		/**< 记录AI的想法 */
};

extern int Game_Init(void);
extern int Game_Start(void);
extern int Game_Loop(void);
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

void GamePlayer_StopXMotion( GamePlayer *player );

void GamePlayer_StartAAttack( GamePlayer *player );

void GamePlayer_SetUpMotion( GamePlayer *player );

void GamePlayer_SetDownMotion( GamePlayer *player );

#endif
