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

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_INPUT_H

#include "game_control.h"
#include "game_config.h"

#define CONFIG_FILE	"config.bin"

struct GameConfigData {
	LCUI_BOOL windowed;
	ControlKey control_key;
} game_config;

/** 初始化游戏键位设置 */
void GameConfig_InitKeyControl(void)
{
	game_config.control_key.a_attack = LCUIKEY_J;
	game_config.control_key.b_attack = LCUIKEY_K;
	game_config.control_key.defense = LCUIKEY_L;
	game_config.control_key.jump = LCUIKEY_SPACE;
	game_config.control_key.left = LCUIKEY_A;
	game_config.control_key.right = LCUIKEY_D;
	game_config.control_key.up = LCUIKEY_W;
	game_config.control_key.down = LCUIKEY_S;
}

/** 获取键位配置 */
void GameConfig_GetKeyControl( ControlKey *buff )
{
	*buff = game_config.control_key;
}

/** 设置键位配置 */
void GameConfig_SetKeyControl( ControlKey *buff )
{
	game_config.control_key = *buff;
}

/** 初始化游戏配置 */
void GameConfig_Init(void)
{
	game_config.windowed = TRUE;
	GameConfig_InitKeyControl();
}

/** 载入游戏配置 */
void GameConfig_Load(void)
{
	FILE *fp;
	fp = fopen( CONFIG_FILE, "rb" );
	if( !fp ) {
		return;
	}
	fread( &game_config, sizeof(game_config), 1, fp );
	fclose( fp );
}

/** 保存游戏配置 */
void GameConfig_Save(void)
{
	FILE *fp;
	fp = fopen( CONFIG_FILE, "wb+" );
	if( !fp ) {
		return;
	}
	fwrite( &game_config, sizeof(game_config), 1, fp );
	fclose( fp );
}

/** 配置游戏是否以窗口模式运行 */
void GameConfig_SetWindowed( LCUI_BOOL flag )
{
	game_config.windowed = flag;
}

/** 判断游戏是否要以窗口模式运行 */
LCUI_BOOL GameConfig_IsWindowed(void)
{
	return game_config.windowed;
}
