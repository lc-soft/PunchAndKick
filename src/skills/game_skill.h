#ifndef __GAME_SKILL_H__
#define __GAME_SKILL_H__

#include "skills_common.h"
#include "skills_martial_artist.h"
#include "skills_kung_fu.h"
#include "skills_tiger.h"

/** 初始化技能库 */
void SkillLibrary_Init(void);

/** 向技能库中添加新技能信息 */
int SkillLibrary_AddSkill(	const char *skill_name, 
				int priority,
				LCUI_BOOL (*can_use)(GamePlayer*),
				void (*run_skill)(GamePlayer*) );

/** 检测玩家是否拥有指定技能 */
LCUI_BOOL GamePlayer_HaveSkill( GamePlayer *player, const char *skill_name );

/** 初始化游戏角色拥有的技能的记录 */
void GamePlayer_InitSkillRecord( GamePlayer *player );

/** 为游戏角色启用一个技能 */
int GamePlayer_EnableSkill( GamePlayer *player, const char *skill_name );

/** 为游戏角色禁用一个技能 */
int GamePlayer_DisableSkill( GamePlayer *player, const char *skill_name );

/** 从技能库中获取一个满足发动条件的技能 */
int SkillLibrary_GetSkill( GamePlayer* player );

/** 让游戏角色发动技能 */
int GamePlayer_RunSkill( GamePlayer *player, int skill_id );

#endif
