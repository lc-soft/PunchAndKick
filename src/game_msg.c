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

// 游戏消息循环

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_INPUT_H
#include "game.h"

typedef struct GamePlayerComputer_ {
	char name[256];
	//....
} GamePlayerComputer;

static LCUI_Thread msg_thread_id = 0;
static LCUI_Queue game_msg_queue;		/**< 游戏的消息队列 */
static LCUI_BOOL enable_lan_battle = FALSE;	/**< 是否启用局域网对战 */
static GamePlayerComputer second_computer;	/**< 对方玩家计算机信息 */
static LCUI_Sleeper msg_sleeper;

/* 向对方主机发送消息 */
static int Game_SendMsg( GameMsg *msg )
{
	/* 若启用局域网对战 */
	if( enable_lan_battle ) {
		//...
	}
	return 0;
}

/* 接收对方主机发送过来的消息 */
static int Game_RecvMsg( void )
{
	if( enable_lan_battle ) {
		//...
	}
	return 0;
}

/* 向消息队列中投递消息 */
int Game_PostMsg( GameMsg *msg )
{
	Queue_Lock( &game_msg_queue );
	if( !Queue_Add( &game_msg_queue, msg ) ) {
		Queue_Unlock( &game_msg_queue );
		return -1;
	}
	Queue_Unlock( &game_msg_queue );
	return LCUISleeper_BreakSleep( &msg_sleeper );
}

/* 从消息队列中取出一个消息 */
int Game_GetMsg( GameMsg *msg_buff )
{
	GameMsg *p_msg;

	Game_RecvMsg();
	while( Queue_GetTotal(&game_msg_queue)<=0 ) {
		LCUISleeper_StartSleep( &msg_sleeper, 1000 );
	}
	Queue_Lock( &game_msg_queue );
	p_msg = (GameMsg*)Queue_Get( &game_msg_queue, 0 );
	if( p_msg == NULL ) {
		Queue_Unlock( &game_msg_queue );
		return -1;
	}
	*msg_buff = *p_msg;
	Queue_Delete( &game_msg_queue, 0 );
	Queue_Unlock( &game_msg_queue );
	return 0;
}

/* 处理消息 */
int Game_ProcMsg( GameMsg *p_msg )
{
	GamePlayer *target;

	target = GameBattle_GetPlayer( p_msg->battle_id, p_msg->player_id );
	if( target == NULL ) {
		return -1;
	}
	switch( p_msg->msg.msg_id ) {
	case GAMEMSG_ACTION:
		if( GameObject_GetCurrentActionID( target->object )
			== p_msg->msg.action.action_id ) {
			break;
		}
		GameObject_SwitchAction( target->object, p_msg->msg.action.action_id );
	default:
		break;
	}
	return 0;
}

static void GameMsgProcThread( void *arg )
{
	GameMsg msg;

	while(1) {
		if( Game_GetMsg( &msg ) == 0 ) {
			Game_ProcMsg( &msg );
		} 
	}
}

int GameMsgLoopStart( void )
{
	if( msg_thread_id != 0 ) {
		return -1;
	}
	LCUISleeper_Create( &msg_sleeper );
	Queue_Init( &game_msg_queue, sizeof(GameMsg), NULL );
	return LCUIThread_Create( &msg_thread_id, GameMsgProcThread, NULL );
}

void GameMsgLoopStop( void )
{

}
