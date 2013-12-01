/* ****************************************************************************
 * Punch And Kick -- a simple 2D Fighting Game.
 *
 * Copyright (C) 2013 by
 * Liu Chao
 *
 * This file is part of the Punch And Kick project, and may only be used,
 * modified, and distributed under the terms of the GPLv2.
 *
 * (GPLv2 is abbreviation of GNU General Public License Version 2)
 *
 * By continuing to use, modify, or distribute this file you indicate that you
 * have read the license and understand and accept it fully.
 *
 * The Punch And Kick project is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GPL v2 for
 * more details.
 *
 * You should have received a copy of the GPLv2 along with this file. It is
 * usually in the LICENSE.TXT file, If not, see <http://www.gnu.org/licenses/>.
 * ***************************************************************************/

/* ****************************************************************************
 * Punch And Kick -- 一个普通的2D格斗游戏
 *
 * 版权所有 (C) 2013 归属于
 * 刘超
 *
 * 这个文件是 Punch And Kick 项目的一部分，并且只可以根据GPLv2许可协议来使用、
 * 更改和发布。
 *
 * (GPLv2 是 GNU通用公共许可证第二版 的英文缩写)
 *
 * 继续使用、修改或发布本文件，表明您已经阅读并完全理解和接受这个许可协议。
 *
 * Punch And Kick 项目是基于技术交流目的而加以散布的，但不负任何担保责任，例
 * 如：稳定性、可玩性、趣味性等的担保，甚至没有适销性或特定用途的隐含担保，详
 * 情请参照GPLv2许可协议。
 *
 * 您应已收到附随于本文件的GPLv2许可协议的副本，它通常在LICENSE.TXT文件中，如果
 * 没有，请查看：<http://www.gnu.org/licenses/>.
 * ***************************************************************************/

// 游戏角色的技能管理模块

//#define DEBUG
#include <LCUI_Build.h>
#include LC_LCUI_H
#include "game_control.h"
#include "game_skill.h"

typedef struct SkillData_ {
	int skill_id;				/**< 技能ID */
	char skill_name[64];			/**< 技能名称 */
	int priority;				/**< 优先级 */
	LCUI_BOOL (*can_use)(GamePlayer*);	/**< 回调函数，用于检测是否满足技能发动条件 */
	void (*run_skill)(GamePlayer*);		/**< 回调函数，用于实现技能 */
} SkillData;

typedef struct SkillRecord_ {
	int skill_id;
	void *data;
} SkillRecord;

static LCUI_BOOL skill_library_init = FALSE;
static LCUI_Queue skill_library;

/** 初始化技能库 */
void SkillLibrary_Init(void)
{
	Queue_Init( &skill_library, sizeof(SkillData), NULL );
	skill_library_init = TRUE;
	CommonSkill_Register();
	FighterSkill_Register();
	MartialArtistSkill_Register();
	KungFuSkill_Register();
	JudoMasterSkill_Register();
	TigerSkill_Register();
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
	skill.skill_id = (int)BKDRHash( skill_name );
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
		DEBUG_MSG("skill id: %d, pos: %d\n", exist_skill_id, n);
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
	skill_id = (int)BKDRHash( skill_name );
	return _GamePlayer_HaveSkill( player, skill_id );
}

static void DstroySkillRecord( void *arg )
{
	SkillRecord *p_data;
	p_data = (SkillRecord*)arg;
	if( p_data->data ) {
		free( p_data->data );
		p_data->data = NULL;
	}
}

/** 初始化游戏角色拥有的技能的记录 */
void GamePlayer_InitSkillRecord( GamePlayer *player )
{
	Queue_Init( &player->skills, sizeof(SkillRecord), DstroySkillRecord );
	GamePlayer_EnableSkill( player, SKILLNAME_A_FINALBLOW );
	GamePlayer_EnableSkill( player, SKILLNAME_B_FINALBLOW );
	GamePlayer_EnableSkill( player, SKILLNAME_DEFENSE );
	GamePlayer_EnableSkill( player, SKILLNAME_A_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_B_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_JUMP );
	GamePlayer_EnableSkill( player, SKILLNAME_JUMP_A_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_JUMP_B_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_SPRINT_A_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_SPRINT_B_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_SPRINT_JUMP_A_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_SPRINT_JUMP_B_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_A_LIFT );
	GamePlayer_EnableSkill( player, SKILLNAME_B_LIFT );
	GamePlayer_EnableSkill( player, SKILLNAME_A_THROW );
	GamePlayer_EnableSkill( player, SKILLNAME_B_THROW );
	GamePlayer_EnableSkill( player, SKILLNAME_JUMP_ELBOW );
	GamePlayer_EnableSkill( player, SKILLNAME_JUMP_TREAD );
	GamePlayer_EnableSkill( player, SKILLNAME_RIDE );
	GamePlayer_EnableSkill( player, SKILLNAME_RIDE_A_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_RIDE_B_ATTACK );
	GamePlayer_EnableSkill( player, SKILLNAME_CATCH );
	GamePlayer_EnableSkill( player, SKILLNAME_PUSH );
}

/** 为游戏角色启用一个技能 */
int GamePlayer_EnableSkill( GamePlayer *player, const char *skill_name )
{
	SkillRecord rec;
	rec.skill_id = (int)BKDRHash( skill_name );
	if( _GamePlayer_HaveSkill( player, rec.skill_id ) ) {
		return -1;
	}
	rec.data = NULL;
	Queue_Add( &player->skills, &rec );
	return 0;
}

/** 获取已启用的技能 */
static SkillRecord *GamePlayer_GetEnabledSkill(	GamePlayer *player,
						const char *skill_name )
{
	int n, skill_id;
	SkillRecord *p_rec;

	skill_id = (int)BKDRHash( skill_name );
	n = Queue_GetTotal( &player->skills );
	while(n--) {
		p_rec = (SkillRecord*)Queue_Get( &player->skills, n );
		if( p_rec && p_rec->skill_id == skill_id ) {
			return p_rec;
		}
	}
	return NULL;
}

/** 设置技能的数据，一些技能需要记录相应数据以在特殊情况下满足发动条件 */
int GamePlayer_SetSkillData(	GamePlayer *player,
				const char *skill_name,
				void *p_data,
				int data_size )
{
	SkillRecord *p_rec;
	p_rec = GamePlayer_GetEnabledSkill( player, skill_name );
	if( !p_rec ) {
		return -1;
	}
	if( !p_rec->data ) {
		p_rec->data = malloc( data_size );
		if( !p_rec->data ) {
			return -2;
		}
	}
	memcpy( p_rec->data, p_data, data_size );
	return 0;
}

/** 获取技能的数据 */
void *GamePlayer_GetSkillData( GamePlayer *player, const char *skill_name )
{
	SkillRecord *p_rec;
	p_rec = GamePlayer_GetEnabledSkill( player, skill_name );
	if( !p_rec ) {
		return NULL;
	}
	return p_rec->data;
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
			DEBUG_MSG("player id: %d, not enable\n", player->id);
			continue;
		}
		if( !p_skill->can_use ) {
			DEBUG_MSG("no check\n");
			continue;
		}
		DEBUG_MSG("function addr: %p\n", p_skill->can_use);
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
