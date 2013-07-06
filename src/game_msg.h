
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

typedef union GameMsgBuff_ {
	int msg_id;		/**< 消息ID */
	GameMotionMsg motion;	/**< 角色移动消息 */
	GameStateMsg state;	/**< 角色状态消息 */
	GameHPMsg hp;		/**< 角色生命值消息 */
} GameMsgBuff;


typedef struct GameMsg {
	int player_id;		/**< 针对的角色的ID */
	GameMsgBuff msg;	/**< 消息内容 */
} GameMsg;

extern void GameKeyboardProc( LCUI_KeyboardEvent *event, void *arg );

/** 向消息队列中投递消息 */
extern int Game_PostMsg( GameMsg *msg );

extern int GameMsgLoopStart( void );