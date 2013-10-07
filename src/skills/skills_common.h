#ifndef __GAME_COMMON_SKILL_H__
#define __GAME_COMMON_SKILL_H__

#define LIFT_HEIGHT	56

#define SHORT_REST_TIMEOUT	2000
#define LONG_REST_TIMEOUT	3000
#define BE_THROW_REST_TIMEOUT	500
#define BE_LIFT_REST_TIMEOUT	4500

#define XSPEED_RUN	300
#define XSPEED_WALK	80
#define YSPEED_WALK	40
#define XACC_DASH	1500
#define ZSPEED_JUMP	500
#define ZACC_JUMP	1500

#define XSPEED_S_HIT_FLY	500
#define ZACC_S_HIT_FLY		1000
#define ZSPEED_S_HIT_FLY	200

#define XSPEED_WEAK_WALK	200
#define WEAK_WALK_TIMEOUT	2500

#define XSPEED_X_HIT_FLY	500
#define ZACC_XB_HIT_FLY		1500
#define ZSPEED_XB_HIT_FLY	300
#define ZACC_XF_HIT_FLY		3000
#define ZSPEED_XF_HIT_FLY	800

#define XSPEED_X_HIT_FLY2	100
#define ZACC_XF_HIT_FLY2	1000
#define ZSPEED_XF_HIT_FLY2	200

#define XSPEED_HIT_FLY	200
#define ZSPEED_HIT_FLY	600
#define ZACC_HIT_FLY	2000

#define XSPEED_THROWUP_FLY	400
#define XSPEED_THROWDOWN_FLY	1000
#define ZSPEED_THROWUP_FLY	500
#define ZSPEED_THROWDOWN_FLY	1000
#define ZACC_THROWUP_FLY	2000
#define ZACC_THROWDOWN_FLY	2000

#define SKILLNAME_A_ATTACK		"A attack"
#define SKILLNAME_B_ATTACK		"B attack"
#define SKILLNAME_DEFENSE		"defense"
#define SKILLNAME_SOLID_DEFENSE		"solid defense"
#define SKILLNAME_JUMP			"jump"
#define SKILLNAME_JUMP_A_ATTACK		"jump A attack"
#define SKILLNAME_JUMP_B_ATTACK		"jump B attack"
#define SKILLNAME_JUMP_MACH_A_ATTACK	"jump mach A attack"
#define SKILLNAME_JUMP_MACH_B_ATTACK	"jump mach B attack"
#define SKILLNAME_SPRINT_A_ATTACK	"sprint A attack"
#define SKILLNAME_SPRINT_B_ATTACK	"sprint B attack"
#define SKILLNAME_SPRINT_JUMP_A_ATTACK	"sprint jump A attack"
#define SKILLNAME_SPRINT_JUMP_B_ATTACK	"sprint jump B attack"
#define SKILLNAME_MACH_A_ATTACK		"mach A attack"
#define SKILLNAME_MACH_B_ATTACK		"mach B attack"
#define SKILLNAME_A_FINALBLOW		"A final blow"
#define SKILLNAME_B_FINALBLOW		"B final blow"
#define SKILLNAME_JUMP_ELBOW		"jump elbow"
#define SKILLNAME_JUMP_TREAD		"jump tread"
#define SKILLNAME_A_LIFT		"A lift"
#define SKILLNAME_B_LIFT		"B lift"
#define SKILLNAME_A_THROW		"A throw"
#define SKILLNAME_B_THROW		"B throw"
#define SKILLNAME_MACH_STOMP		"mach stomp"
#define SKILLNAME_BOMBKICK		"bomb kick"
#define SKILLNAME_SPINHIT		"spin hit"
#define SKILLNAME_RIDE			"ride"
#define SKILLNAME_RIDE_A_ATTACK		"ride A attack"
#define SKILLNAME_RIDE_B_ATTACK		"ride B attack"
#define SKILLNAME_CATCH			"catch"
#define SKILLNAME_BIG_ELBOW		"big elbow"
#define SKILLNAME_GUILLOTINE		"guillotine"
#define SKILLNAME_PUSH			"push"
#define SKILLNAME_JUMP_SPINKICK		"jump spin kick"
#define SKILLNAME_TORNADO_ATTACK	"tornado_attack"

enum SkillPriority_AAttack {
	SKILLPRIORITY_A_LIFT,
	SKILLPRIORITY_A_THROW,
	SKILLPRIORITY_MACH_STOMP,
	SKILLPRIORITY_JUMP_ELBOW,
	SKILLPRIORITY_A_FINALBLOW,
	SKILLPRIORITY_SPINHIT,
	SKILLPRIORITY_SPRINT_A_ATTACK,
	SKILLPRIORITY_SPRINT_JUMP_A_ATTACK,
	SKILLPRIORITY_BIG_ELBOW,
	SKILLPRIORITY_JUMP_MACH_A_ATTACK,
	SKILLPRIORITY_JUMP_A_ATTACK,
	SKILLPRIORITY_MACH_A_ATTACK,
	SKILLPRIORITY_A_ATTACK,
	SKILLPRIORITY_RIDE_A_ATTACK
};

enum SkillPriority_BAttack {
	SKILLPRIORITY_B_LIFT,
	SKILLPRIORITY_B_THROW,
	SKILLPRIORITY_JUMP_TREAD,
	SKILLPRIORITY_B_FINALBLOW,
	SKILLPRIORITY_BOMBKICK,
	SKILLPRIORITY_SPRINT_B_ATTACK,
	SKILLPRIORITY_SPRINT_JUMP_B_ATTACK,
	SKILLPRIORITY_GUILLOTINE,
	SKILLPRIORITY_JUMP_MACH_B_ATTACK,
	SKILLPRIORITY_JUMP_B_ATTACK,
	SKILLPRIORITY_PUSH,
	SKILLPRIORITY_MACH_B_ATTACK,
	SKILLPRIORITY_B_ATTACK,
	SKILLPRIORITY_RIDE_B_ATTACK
};

enum SkillPriority_HorizMotion {
	SKILLPRIORITY_RIDE,
	SKILLPRIORITY_JUMP_SPINKICK
};

enum SkillPriority_VertiMotion {
	SKILLPRIORITY_CATCH
};

enum SkillPriority_Jump {
	SKILLPRIORITY_JUMP
};

enum SkillPriority_Defense {
	SKILLPRIORITY_DEFENSE
};

/** 设置自己在擒获住对方时对方的位置 */
void CommonSkill_SetPositionAtCatch( GamePlayer *self, GamePlayer *other );

/** 注册通用技能 */
void CommonSkill_Register(void);

/** 在擒获范围内，获取处于冲撞攻击状态的玩家 */
GamePlayer *GetSpirntAttackerInCatchRange( GamePlayer *self );

/** 设置游戏角色被攻击命中 */
void GamePlayer_SetHit( GamePlayer *player );

/** 设置游戏角色为下落状态 */
void GamePlayer_SetFall( GamePlayer *player );
#endif
