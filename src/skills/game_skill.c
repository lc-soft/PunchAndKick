//#define DEBUG
#include <LCUI_Build.h>
#include LC_LCUI_H
#include "../game_control.h"
#include "game_skill.h"

typedef struct SkillData_ {
	int skill_id;				/**< 技能ID */
	char skill_name[64];			/**< 技能名称 */
	int priority;				/**< 优先级 */
	LCUI_BOOL (*can_use)(GamePlayer*);	/**< 回调函数，用于检测是否满足技能发动条件 */
	void (*run_skill)(GamePlayer*);		/**< 回调函数，用于实现技能 */
} SkillData;

static LCUI_BOOL skill_library_init = FALSE;
static LCUI_Queue skill_library;

/** 初始化技能库 */
void SkillLibrary_Init(void)
{
	Queue_Init( &skill_library, sizeof(SkillData), NULL );
	skill_library_init = TRUE;
	CommonSkill_Register();
}

/** 向技能库中添加新技能信息 */
int SkillLibrary_AddSkill(	const char *skill_name, 
				int priority,
				LCUI_BOOL (*can_use)(GamePlayer*),
				void (*run_skill)(GamePlayer*) )
{
	int i, n;
	SkillData skill, *p_skill;

	if( !skill_library_init ) {
		return -1;
	}
	skill.skill_id = BKDRHash( skill_name );
	n = Queue_GetTotal( &skill_library );
	DEBUG_MSG("add skill: %s, id: %d, priority: %d\n", 
	skill_name, skill.skill_id, priority);
#ifdef DEBUG
	_DEBUG_MSG("skill list: \n");
	for(i=0; i<n; ++i) {
		p_skill = (SkillData*)Queue_Get( &skill_library, i );
		if( !p_skill ) {
			continue;	
		}
		_DEBUG_MSG("skill: %s, id: %d, priority: %d, pos: %d\n", 
		p_skill->skill_name, p_skill->skill_id, p_skill->priority, i);
	}
	_DEBUG_MSG("end skill list\n\n");
#endif
	for(i=0; i<n; ++i) {
		p_skill = (SkillData*)Queue_Get( &skill_library, i );
		if( !p_skill ) {
			continue;	
		}
		DEBUG_MSG("skill: %s, id: %d, priority: %d, pos: %d\n", 
		p_skill->skill_name, p_skill->skill_id, p_skill->priority, i);
		if( p_skill->skill_id == skill.skill_id ) {
			p_skill->can_use = can_use;
			p_skill->run_skill = run_skill;
			return skill.skill_id;
		}
		if( p_skill->priority >= priority ) {
			break;
		}
	}
	skill.priority = priority;
	skill.run_skill = run_skill;
	skill.can_use = can_use;
	strncpy( skill.skill_name, skill_name, 64 );
	skill.skill_name[63] = 0;
	DEBUG_MSG("i=%d\n", i);
	if( i >= n ) {
		Queue_Add( &skill_library, &skill );
	} else {
		Queue_Insert( &skill_library, i, &skill );
	}
	return skill.skill_id;
}

static LCUI_BOOL _GamePlayer_HaveSkill( GamePlayer *player, int skill_id )
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

/** 检测玩家是否拥有指定技能 */
LCUI_BOOL GamePlayer_HaveSkill( GamePlayer *player, const char *skill_name )
{
	int skill_id;
	skill_id = BKDRHash( skill_name );
	return _GamePlayer_HaveSkill( player, skill_id );
}

/** 初始化游戏角色拥有的技能的记录 */
void GamePlayer_InitSkillRecord( GamePlayer *player )
{
	Queue_Init( &player->skills, sizeof(int), NULL );
	GamePlayer_EnableSkill( player, SKILLNAME_A_FINALBLOW );
	GamePlayer_EnableSkill( player, SKILLNAME_B_FINALBLOW );
	GamePlayer_EnableSkill( player, SKILLNAME_A_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_B_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_JUMP_A_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_JUMP_B_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_JUMP_MACH_A_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_JUMP_MACH_B_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_SPRINT_A_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_SPRINT_B_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_SPRINT_JUMP_A_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_SPRINT_JUMP_B_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_MACH_A_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_MACH_B_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_A_LIFT );
	GamePlayer_EnableSkill( player, SKILLNAME_B_LIFT );
	GamePlayer_EnableSkill( player, SKILLNAME_A_THROW );
	GamePlayer_EnableSkill( player, SKILLNAME_B_THROW );
	GamePlayer_EnableSkill( player, SKILLNAME_JUMP_ELBOW );
	GamePlayer_EnableSkill( player, SKILLNAME_JUMP_TREAD );
	GamePlayer_EnableSkill( player, SKILLNAME_MACH_STOMP );
	GamePlayer_EnableSkill( player, SKILLNAME_BOMBKICK );
	GamePlayer_EnableSkill( player, SKILLNAME_SPINHIT );
	GamePlayer_EnableSkill( player, SKILLNAME_RIDE_A_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_RIDE_B_ATTACK );
}

/** 为游戏角色启用一个技能 */
int GamePlayer_EnableSkill( GamePlayer *player, const char *skill_name )
{
	int skill_id;

	skill_id = BKDRHash( skill_name );
	if( _GamePlayer_HaveSkill( player, skill_id ) ) {
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
	int i, n;
	SkillData *p_skill;

	n = Queue_GetTotal( &skill_library );
	for(i=0; i<n; ++i) {
		p_skill = (SkillData*)Queue_Get( &skill_library, i );
		if( !p_skill ) {
			continue;	
		}
		DEBUG_MSG("skill: %s, id: %d, pos: %d\n", 
		p_skill->skill_name, p_skill->skill_id, i);
		if( !_GamePlayer_HaveSkill( player, p_skill->skill_id ) ) {
			continue;
		}
		if( !p_skill->can_use ) {
			continue;
		}
		if( p_skill->can_use( player ) ) {
			DEBUG_MSG("use this skill\n");
			return p_skill->skill_id;
		}
	}
	DEBUG_MSG("no skill\n");
	return -1;
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
