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

#ifndef __GAME_STATUSBAR_H__
#define __GAME_STATUSBAR_H__

LCUI_Widget *StatusBar_New(void);

/** 设置玩家名称(最多8个字) */
void StatusBar_SetPlayerNameW( LCUI_Widget *widget, const wchar_t *name );

void StatusBar_SetPlayerName( LCUI_Widget *widget, const char *name );

/** 设置玩家类型名称，最多4个字，如：1P、2P或CPU */
void StatusBar_SetPlayerTypeNameW( LCUI_Widget *widget, const wchar_t *type_name );

void StatusBar_SetPlayerTypeName( LCUI_Widget *widget, const char *type_name );

/** 设置最大生命值 */
void StatusBar_SetMaxHealth( LCUI_Widget *widget, int value );

/** 设置当前生命值 */
void StatusBar_SetHealth( LCUI_Widget *widget, int value );

/** 设置头像 */
void StatusBar_SetAvatar( LCUI_Widget *widget, LCUI_Graph *img );

/** 注册状态栏部件 */
void StatusBar_Register(void);

#endif
