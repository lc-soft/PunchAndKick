#include <LCUI_Build.h>
#include LC_LCUI_H
#include "../game_control.h"

typedef struct SkillData_ {
	int skill_id;				/**< 技能ID */
	char skill_name[64];			/**< 技能名称 */
	LCUI_BOOL (*can_use)(GamePlayer*);	/**< 回调函数，用于检测是否满足技能发动条件 */
	void (*run_skill)(GamePlayer*);		/**< 回调函数，用于实现技能 */
} SkillData;

static LCUI_BOOL skill_library_init = FALSE;
static LCUI_Queue skill_library;

/** 初始化技能库 */
static void SkillLibrary_Init(void)
{
	skill_library_init = TRUE;
	Queue_Init( &skill_library, sizeof(SkillData), NULL );
}

/** 向技能库中添加新技能信息 */
int SkillLibrary_AddSkil(	const char *skill_name, 
				LCUI_BOOL (*can_use)(GamePlayer*),
				void (*run_skill)(GamePlayer*) )
{
	int n;
	SkillData skill, *p_skill;

	if( !skill_library_init ) {
		SkillLibrary_Init();
	}
	skill.skill_id = BKDRHash( skill_name );
	n = Queue_GetTotal( &skill_library );
	while(n--) {
		p_skill = (SkillData*)Queue_Get( &skill_library, n );
		if( p_skill && p_skill->skill_id == skill.skill_id ) {
			break;
		}
	}
	if( n >= 0 ) {
		p_skill->can_use = can_use;
		p_skill->run_skill = run_skill;
	} else {
		skill.run_skill = run_skill;
		skill.can_use = can_use;
		strncpy( skill.skill_name, skill_name, 64 );
		skill.skill_name[63] = 0;
		Queue_Add( &skill_library, &skill );
	}
	return skill.skill_id;
}

/** 检测玩家是否拥有指定技能 */
static LCUI_BOOL GamePlayer_HaveSkill( GamePlayer *player, int skill_id )
{
	int n, *exist_skill_id;

	n = Queue_GetTotal( &player->skills );
	while(n--) {
		exist_skill_id = (int*)Queue_Get( &player->skills, n );
		if( exist_skill_id && *exist_skill_id == skill_id ) {
			return TRUE;
		}
	}
	return FALSE;
}

/** 初始化游戏角色拥有的技能的记录 */
void GamePlayer_InitSkillRecord( GamePlayer *player )
{
	Queue_Init( &player->skills, sizeof(int), NULL );
}

/** 为游戏角色启用一个技能 */
int GamePlayer_EnableSkill( GamePlayer *player, const char *skill_name )
{
	int skill_id;

	skill_id = BKDRHash( skill_name );
	if( GamePlayer_HaveSkill( player, skill_id ) ) {
		return -1;
	}
	Queue_Add( &player->skills, &skill_id );
	return 0;
}

/** 为游戏角色禁用一个技能 */
int GamePlayer_DisableSkill( GamePlayer *player, const char *skill_name )
{
	int n, *exist_skill_id, skill_id;

	skill_id = BKDRHash( skill_name );
	n = Queue_GetTotal( &player->skills );
	while(n--) {
		exist_skill_id = (int*)Queue_Get( &player->skills, n );
		/* 如果该技能已经被启用，则移除该技能的记录 */
		if( exist_skill_id && *exist_skill_id == skill_id ) {
			Queue_Delete( &player->skills, n );
			return 0;
		}
	}
	return -1;
}

/** 从技能库中获取一个满足发动条件的技能 */
int SkillLibrary_GetSkill( GamePlayer* player )
{
	int n;
	SkillData *p_skill;

	n = Queue_GetTotal( &skill_library );
	while(n--) {
		p_skill = (SkillData*)Queue_Get( &skill_library, n );
		if( !p_skill ) {
			continue;	
		}
		if( !GamePlayer_HaveSkill( player, p_skill->skill_id ) ) {
			continue;
		}
		if( !p_skill->can_use ) {
			continue;
		}
		if( !p_skill->can_use( player ) ) {
			return p_skill->skill_id;
		}
	}
	return -2;
}

/** 让游戏角色发动技能 */
int GamePlayer_RunSkill( GamePlayer *player, int skill_id )
{
	int n;
	SkillData *p_skill;

	n = Queue_GetTotal( &skill_library );
	while(n--) {
		p_skill = (SkillData*)Queue_Get( &skill_library, n );
		if( !p_skill ) {
			continue;	
		}
		if( p_skill->skill_id == skill_id ) {
			if( p_skill->run_skill ) {
				p_skill->run_skill( player );
			}
			return 0;
		}
	}
	return -1;
}
