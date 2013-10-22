#ifndef __GAME_BATTLE_H__
#define __GAME_BATTLE_H__

typedef struct BattleDate_ {
	int id;
	LCUI_Queue player_list;
	LCUI_Widget *scene;
	LCUI_Pos scene_land_pos;
	LCUI_Size scene_land_size;
	LCUI_BOOL need_sync_camera;
	LCUI_Widget *camera_target;
	int camera_x_padding;
	GameSpaceData *space;
	LCUI_Queue gameobject_library;
} BattleData;

/** 获取指定ID的对战数据 */
BattleData *GameBattle_GetBattle( int battle_id );

/** 新建一个对战，并返回该对战的ID */
int GameBattle_New(void);

/** 开始对战 */
int GameBattle_Start( int battle_id );

/** 设置指定游戏角色的控制键 */
int GameBattle_SetPlayerControlKey(	int battle_id, 
					int player_id, 
					ControlKey *ctrlkey );

/** 通过控制键获取该键控制的角色 */
GamePlayer *GameBattle_GetPlayerByControlKey( int key_code );

/** 通过GameObject部件获取游戏玩家数据 */
GamePlayer *GameBattle_GetPlayerByWidget( LCUI_Widget *gameobject );

/** 获取指定ID的玩家数据 */
GamePlayer *GameBattle_GetPlayer( int battle_id, int player_id );

/** 设置一个玩家 */
int GameBattle_SetPlayer(	int battle_id, 
				int player_id,
				int role_id, 
				LCUI_BOOL human_control );

/** 设置是否启用一个玩家 */
int GameBattle_EnablePlayer(	int battle_id, 
				int player_id, 
				LCUI_BOOL flag );

/** 设置指定对战中的玩家是否无敌 */
int GameBattlePlayer_SetInvincible(	int battle_id, 
					int player_id, 
					LCUI_BOOL flag );
/** 设置场景地面参数 */
int GameBattle_SetSceneLand(	int battle_id,
				LCUI_Pos land_pos, 
				LCUI_Size land_size );

/** 获取场景地面参数 */
int GameBattle_GetSceneLand(	int battle_id,
				LCUI_Pos *land_pos, 
				LCUI_Size *land_size );

/** 设置场景大小 */
int GameBattle_SetSceneSize( int battle_id, LCUI_Size size );

/** 设置场景背景图 */
int GameBattle_SetSceneBackground(	int battle_id, 
					LCUI_Graph *img );

/** 获取场景 */
LCUI_Widget* GameBattle_GetScene( int battle_id );

/** 进入循环 */
int GameBattle_Loop( int battle_id );

#endif
