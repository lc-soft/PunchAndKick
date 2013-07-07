
enum GamePlayerState {
	STATE_STANCE,	/**< 站立 */
	STATE_WALK,	/**< 步行 */
	STATE_RUN,	/**< 奔跑 */
	STATE_HIT,	/**< 被命中 */
	STATE_REST	/**< 歇息 */
};

typedef struct GamePlayer_ {
	int id;				/**< 玩家ID */
	int state;			/**< 状态 */
	LCUI_BOOL right_direction;	/**< 角色是否朝着右方 */
	LCUI_BOOL human_control;	/**< 是否由人类控制 */
	LCUI_BOOL local_control;	/**< 是否由此处玩家控制 */
	int walk_speed;			/**< 步行的速度 */
	int run_speed;			/**< 奔跑的速度 */
	LCUI_Widget *game_object;	/**< 游戏对象 */
	PhysicsObject *phys_object;	/**< 物理对象 */
} GamePlayer;

extern int Game_Init(void);
extern int Game_Start(void);
extern int Game_Pause(void);

extern void GamePlayer_ChangeState( GamePlayer *player, int state );

/** 通过控制键获取该键控制的角色 */
extern GamePlayer *GamePlayer_GetPlayerByControlKey( int key_code );

/** 通过角色ID来获取角色 */
extern GamePlayer *GamePlayer_GetByID( int player_id );