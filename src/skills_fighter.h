#ifndef __FIGHTER_SKILL_H__
#define __FIGHTER_SKILL_H__
#define SKILLNAME_HUG_FRONT_PUT		"hug front put"
#define SKILLNAME_HUG_BACK_PUT		"hug back put"
#define SKILLPRIORITY_HUG_FRONT_PUT	SKILLPRIORITY_MACH_A_ATTACK
#define SKILLPRIORITY_HUG_BACK_PUT	SKILLPRIORITY_MACH_A_ATTACK

/** 注册格斗家特有的技能 */
void FighterSkill_Register(void);
#endif
