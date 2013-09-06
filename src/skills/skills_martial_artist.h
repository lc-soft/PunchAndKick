
#ifndef __MARTIAL_ARTIST_SKILL_H__
#define __MARTIAL_ARTIST_SKILL_H__

#define SKILLNAME_ELBOW		"elbow"
#define SKILLNAME_KNEEHIT	"knee hit"
/** 定义这两个技能的优先级 */
#define SKILLPRIORITY_ELBOW	SKILLPRIORITY_MACH_A_ATTACK
#define SKILLPRIORITY_KNEEHIT	SKILLPRIORITY_ELBOW

/** 注册拳击家特有的技能 */
void MartialArtistSkill_Register(void);

#endif
