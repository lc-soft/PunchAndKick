#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H

enum GameMsgID {
	GAMEMSG_MOTION,
	GAMEMSG_STATE,
	GAMEMSG_HP
};

typedef struct GameMotionMsg_ {
	int msg_id;
	int x, y;	/**< 角色坐标 */
} GameMotionMsg;

typedef struct GameStateMsg_ {
	int msg_id;
	int state;	/**< 角色状态 */
} GameStateMsg;

typedef struct GameHPMsg_ {
	int msg_id;
	int hp;		/**< 角色生命值 */
} GameHPMsg;

typedef union GameMsg_ {
	int msg_id;		/**< 消息ID */
	GameMotionMsg motion;	/**< 角色移动消息 */
	GameStateMsg state;	/**< 角色状态消息 */
	GameHPMsg hp;		/**< 角色生命值消息 */
} GameMsg;

typedef struct GamePlayerComputer_ {
	char name[256];
	//....
} GamePlayerComputer;

static LCUI_Queue game_msg_queue;		/**< 游戏的消息队列 */
static LCUI_BOOL enable_lan_battle = FALSE;	/**< 是否启用局域网对战 */
static GamePlayerComputer second_computer;	/**< 对方玩家计算机信息 */

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
	return 0;
}

/* 从消息队列中取出一个消息 */
int Game_GetMsg( GameMsg *msg_buff )
{
	return 0;
}

/* 处理消息 */
int Game_ProcMsg( GameMsg *msg_ptr )
{
	return 0;
}

void GamePlayerControl( void *arg )
{
	int ret;
	GameMsg msg;

	while(1) {
		Game_RecvMsg();
		ret = Game_GetMsg( &msg );
		if( ret == 0 ) {
			Game_ProcMsg( &msg );
		}
	}
}
