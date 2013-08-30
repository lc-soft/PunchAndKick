#ifndef __GAME_COMMON_SKILL_H__
#define __GAME_COMMON_SKILL_H__

#define SKILLNAME_A_ATTACK		"A attack"
#define SKILLNAME_B_ATTACK		"B attack"
#define SKILLNAME_MACH_A_ATTACK		"mach A attack"
#define SKILLNAME_MACH_B_ATTACK		"mach B attack"
#define SKILLNAME_A_FINALBLOW		"A final blow"
#define SKILLNAME_B_FINALBLOW		"B final blow"

enum SkillPriority_AAttack {
	SKILLPRIORITY_A_FINALBLOW,
	SKILLPRIORITY_MACH_A_ATTACK,
	SKILLPRIORITY_A_ATTACK
};

enum SkillPriority_BAttack {
	SKILLPRIORITY_B_FINALBLOW,
	SKILLPRIORITY_MACH_B_ATTACK,
	SKILLPRIORITY_B_ATTACK
};

/** 注册通用技能 */
void CommonSkill_Register(void);

#endif
