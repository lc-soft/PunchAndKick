// 处理游戏角色的攻击伤害，以及受攻击时的动作
#ifndef __GAME_ATTACK_H__
#define __GAME_ATTACK_H__

#define ATK_NONE			"none"
#define ATK_ELBOW_1			"elbow attack step 1"
#define ATK_ELBOW_2			"elbow attack step 2"
#define ATK_KNEE_HIT_1			"knee hit attack step 1"
#define ATK_KNEE_HIT_2			"knee hit attack step 2"
#define ATK_THROW			"throw attack"
#define ATK_RIDE_A_ATTACK		"ride A attack"
#define ATK_RIDE_B_ATTACK		"ride B attack"
#define ATK_BUMPED			"bumped attack"
#define ATK_FINALBLOW			"final blow attack"
#define ATK_SPRINT_JUMP_A_ATTACK	"sprint jump A attack"
#define ATK_SPRINT_JUMP_B_ATTACK	"sprint jump B attack"
#define ATK_A_ATTACK			"A attack"
#define ATK_B_ATTACK			"B attack"
#define ATK_JUMP_TREAD			"jump tread attack"
#define ATK_JUMP_ELBOW			"jump elbow attack"
#define ATK_MACH_STOMP			"mach stomp attack"
#define ATK_JUMP_A_ATTACK		"jump A attack"
#define ATK_JUMP_B_ATTACK		"jump A attack"
#define ATK_BOMBKICK			"bomb kick attack"
#define ATK_SPINHIT			"spin hit attack"
#define ATK_BIG_ELBOW			"big elbow attack"
#define ATK_JUMP_SPINKICK		"jump spin kick attack"


/** 记录攻击者对受害者的攻击 */
void Game_RecordAttack( GamePlayer *attacker,
			const char *attack_type_name,
			GamePlayer *victim,
			int victim_state );

/** 处理已经记录的攻击 */
void Game_ProcAttack(void);

/** 初始化攻击记录 */
void Game_InitAttackRecord(void);

#endif