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

#ifndef __GAME_RESOURCE_H__
#define __GAME_RESOURCE_H__

#define ACTION_RES_CLASS_RIKI	"graphics::action::riki"
#define ACTION_RES_CLASS_KUNI	"graphics::action::kuni"
#define ACTION_RES_CLASS_MIKE	"graphics::action::mike"
#define ACTION_RES_CLASS_BEN	"graphics::action::ben"
#define ACTION_RES_CLASS_TORAJI	"graphics::action::toraji"
#define MAIN_RES		"graphics::main"
#define SCENES_RES		"graphics::senes"
#define FONT_RES		"graphics::font"

/** 初始化 */
LCUI_API int GameGraphRes_Init( void );

/** 释放全部资源 */
LCUI_API void GameGraphRes_FreeAll( void );

/** 释放指定类名的资源 */
LCUI_API int GameGraphRes_Free( const char *class_name );

/** 添加一个新资源类，返回该资源类的标识号 */
LCUI_API int GameGraphRes_AddClass( const char *class_name );

LCUI_API int GameGraphRes_AddGraph( int class_id, const char *name, LCUI_Graph *graph );

/** 从文件内载入资源 */
LCUI_API int GameGraphRes_LoadFromFile( const char *filepath );

/** 将指定类资源写入至文件 */
LCUI_API int GameGraphRes_WriteToFile( const char *filepath, const char *class_name );

/** 在指定资源类里面获取指定名称的资源 */
LCUI_API int GameGraphRes_GetGraph(	const char *class_name,
					const char *name,
					LCUI_Graph *buff );

#endif