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

#ifndef __GAME_SKILL_H__
#define __GAME_SKILL_H__

#include "skills_common.h"
#include "skills_judo_master.h"
#include "skills_fighter.h"
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
