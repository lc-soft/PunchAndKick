// 处理游戏角色的攻击伤害，以及受攻击时的动作
#ifndef __GAME_ATTACK_H__
#define __GAME_ATTACK_H__

#define ATK_NONE			"none"
#define ATK_THROW			"throw attack"
#define ATK_RIDE_A_ATTACK		"ride A attack"
#define ATK_RIDE_B_ATTACK		"ride B attack"
#define ATK_BUMPED			"bumped attack"
#define ATK_BUMPED2			"bumped attack, no effcet"
#define ATK_FINALBLOW			"final blow attack"
#define ATK_SPRINT_JUMP_A_ATTACK	"sprint jump A attack"
#define ATK_SPRINT_JUMP_B_ATTACK	"sprint jump B attack"
#define ATK_A_ATTACK			"A attack"
#define ATK_B_ATTACK			"B attack"
#define ATK_SPRINT_A_ATTACK		"sprint A attack"
#define ATK_SPRINT_B_ATTACK		"sprint B attack"
#define ATK_MACH_A_ATTACK		"mach A attack"
#define ATK_MACH_B_ATTACK		"mach B attack"
#define ATK_JUMP_TREAD			"jump tread attack"
#define ATK_JUMP_ELBOW			"jump elbow attack"
#define ATK_MACH_STOMP			"mach stomp attack"
#define ATK_JUMP_A_ATTACK		"jump A attack"
#define ATK_JUMP_B_ATTACK		"jump A attack"
#define ATK_JUMP_MACH_A_ATTACK		"jump mach A attack"
#define ATK_JUMP_MACH_B_ATTACK		"jump mach A attack"
#define ATK_BOMBKICK			"bomb kick attack"
#define ATK_SPINHIT			"spin hit attack"
#define ATK_SPINHIT_2			"second spin hit attack"
#define ATK_BIG_ELBOW			"big elbow attack"
#define ATK_GUILLOTINE			"guillotine attack"
#define ATK_JUMP_SPINKICK		"jump spin kick attack"


/** 根据受害者的防御力及其它情况，获取减免后的实际伤害 */
int DamageReduce( GamePlayer *victim, int victim_state, int demage );

/**< 添加攻击数据 */
int AttackLibrary_AddAttack(	const char *attack_name, 
				int (*get_damage)(GamePlayer*,GamePlayer*,int),
				void (*effect)(GamePlayer*,GamePlayer*) );

/** 记录攻击者对受害者的攻击 */
void Game_RecordAttack( GamePlayer *attacker,
			const char *attack_type_name,
			GamePlayer *victim,
			int victim_state );

/** 处理已经记录的攻击 */
void Game_ProcAttack( LCUI_Queue *p_attakc_record );

/** 初始化攻击信息库 */
void Game_InitAttackLibrary(void);

/** 初始化攻击记录 */
void Game_InitAttackRecord( LCUI_Queue *p_attakc_record );

#endif