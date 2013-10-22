//game_demo.c
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H

#include "game.h"
#include "skills/game_skill.h"

static LCUI_BOOL list_is_inited = FALSE;
static LCUI_Queue battle_list;

static void GamePlayerList_Delete( void* arg )
{

}

static void GameBattleList_Delete( void* arg )
{

}

/** 获取指定ID的对战数据 */
BattleData *GameBattle_GetBattle( int battle_id )
{
	int i, n;
	BattleData *p_data;

	if( !list_is_inited ) {
		return NULL;
	}
	n = Queue_GetTotal( &battle_list );
	for(i=0; i<n; ++i) {
		p_data = (BattleData*)Queue_Get( &battle_list, i );
		if( !p_data ) {
			continue;
		}
		if( p_data->id == battle_id ) {
			return p_data;
		}
	}
	return NULL;
}

/** 新建一个对战，并返回该对战的ID */
int GameBattle_New(void)
{
	BattleData data;
	static int battle_id = 0;

	if( !list_is_inited ) {
		Queue_Init( &battle_list, sizeof(BattleData), GameBattleList_Delete );
		list_is_inited = TRUE;
	}
	data.id = ++battle_id;
	data.need_sync_camera = FALSE;
	data.camera_x_padding = 0;
	data.scene_land_pos.x = 0;
	data.scene_land_pos.y = 0;
	data.scene_land_size.w = 0;
	data.scene_land_size.h = 0;
	data.camera_target = NULL;
	data.space = GameSpace_New();
	data.scene = Widget_New(NULL);
	Queue_Init( &data.player_list, sizeof(GamePlayer), GamePlayerList_Delete );
	GameObjectLibrary_Create( &data.gameobject_library );
	Queue_Add( &battle_list, &data );
	return data.id;
}

/** 开始对战 */
int GameBattle_Start( int battle_id )
{
	int i, n;
	BattleData *p_battle;
	GamePlayer *p_player;
	
	p_battle = GameBattle_GetBattle( battle_id );
	if( !p_battle ) {
		return -1;
	}
	n = Queue_GetTotal( &p_battle->player_list );
	/* 播放动作动画，并显示游戏角色 */
	for(i=0; i<n; ++i) {
		p_player = (GamePlayer*)Queue_Get( &p_battle->player_list, i );
		if( !p_player->enable ) {
			continue;
		}
		/* 播放动作动画 */
		GameObject_PlayAction( p_player->object );
		/* 显示游戏角色 */
		Widget_Show( p_player->object );
	}
	return 0;
}

/** 设置指定游戏角色的控制键 */
int GameBattle_SetPlayerControlKey(	int battle_id, 
					int player_id, 
					ControlKey *ctrlkey )
{
	GamePlayer *p_player;
	p_player = GameBattle_GetPlayer( battle_id, player_id );
	if( !p_player ) {
		return -1;
	}
	p_player->ctrlkey = *ctrlkey;
	return 0;
}

/** 通过控制键获取该键控制的角色 */
GamePlayer *GameBattle_GetPlayerByControlKey( int key_code )
{
	int j, i, m, n;
	BattleData *p_battle;
	GamePlayer *p_player;

	m = Queue_GetTotal( &battle_list );
	for(j=0; j<m; ++j) {
		p_battle = (BattleData*)Queue_Get( &battle_list, j );
		if( !p_battle ) {
			continue;
		}
		n = Queue_GetTotal( &p_battle->player_list );
		for(i=0; i<n; ++i) {
			p_player = (GamePlayer*)Queue_Get( &p_battle->player_list, i );
			if( !p_player ) {
				continue;
			}
			if( key_code == p_player->ctrlkey.a_attack
			 || key_code == p_player->ctrlkey.b_attack
			 || key_code == p_player->ctrlkey.defense
			 || key_code == p_player->ctrlkey.left
			 || key_code == p_player->ctrlkey.right
			 || key_code == p_player->ctrlkey.up
			 || key_code == p_player->ctrlkey.down
			 || key_code == p_player->ctrlkey.jump ) {
				 return p_player;
			}
		}
	}
	return NULL;
}

/** 通过GameObject部件获取游戏玩家数据 */
GamePlayer *GameBattle_GetPlayerByWidget( LCUI_Widget *gameobject )
{
	int j, i, m, n;
	BattleData *p_battle;
	GamePlayer *p_player;

	m = Queue_GetTotal( &battle_list );
	for(j=0; j<m; ++j) {
		p_battle = (BattleData*)Queue_Get( &battle_list, j );
		if( !p_battle ) {
			continue;
		}
		n = Queue_GetTotal( &p_battle->player_list );
		for(i=0; i<n; ++i) {
			p_player = (GamePlayer*)Queue_Get( &p_battle->player_list, i );
			if( !p_player ) {
				continue;
			}
			if( p_player->object == gameobject ) {
				 return p_player;
			}
		}
	}
	return NULL;
}

/** 获取指定ID的玩家数据 */
GamePlayer *GameBattle_GetPlayer( int battle_id, int player_id )
{
	int i, n;
	BattleData* p_data;
	GamePlayer* player;

	p_data = GameBattle_GetBattle( battle_id );
	if( !p_data ) {
		return NULL;
	}
	n = Queue_GetTotal( &p_data->player_list );
	for(i=0; i<n; ++i) {
		player = (GamePlayer*)Queue_Get( &p_data->player_list, i );
		if( !player ) {
			continue;
		}
		if( player->id == player_id ) {
			return player;
		}
	}
	return NULL;
}

static int GamePlayer_InitAction( GamePlayer *player, int id )
{
	int i;
	ActionData* action;

	player->state = STATE_STANCE;
	for(i=0; i<TOTAL_ACTION_NUM; ++i) {
		/* 载入游戏角色资源 */
		action = ActionRes_Load( id, i );
		/* 将动作集添加至游戏对象 */
		GameObject_AddAction( player->object, action, i );
	}
	return 0;
}

/** 响应游戏角色受到的攻击 */
static void GamePlayer_ResponseAttack( LCUI_Widget *widget )
{
	GamePlayer *player, *atk_player;
	AttackerInfo *p_info;
	LCUI_Queue *attacker_info;

	player = GameBattle_GetPlayerByWidget( widget );
	if( player == NULL ){
		return;
	}

	attacker_info = GameObject_GetAttackerInfo( widget );
	while(1) {
		p_info = (AttackerInfo*)Queue_Get( attacker_info, 0 );
		if( p_info == NULL ) {
			break;
		}
		atk_player = GameBattle_GetPlayerByWidget( p_info->attacker );
		/* 记录本次攻击的信息 */
		Game_RecordAttack(	atk_player, atk_player->attack_type_name,
					player, player->state );
		/* 删除攻击者记录 */
		Queue_Delete( attacker_info, 0 );
	}
}

/** 设置一个玩家 */
int GameBattle_SetPlayer(	int battle_id, 
				int player_id,
				int role_id, 
				LCUI_BOOL human_control )
{
	int i;
	RoleInfo *p_info;
	BattleData *p_battle;
	GamePlayer *player, player_data;
	LCUI_Graph img_shadow;

	p_battle = GameBattle_GetBattle( battle_id );
	if( !p_battle ) {
		return -1;
	}
	player = GameBattle_GetPlayer( battle_id, player_id );
	if( !player ) {
		Graph_Init( &img_shadow );
		GamePlayer_Init( &player_data );
		GameGraphRes_GetGraph( MAIN_RES, "shadow", &img_shadow );
		
		player_data.id = player_id;
		player_data.battle_id = battle_id;

		player_data.object = GameObject_New( p_battle->space, &p_battle->gameobject_library );
		GameObject_SetShadow( player_data.object, img_shadow );
		/* 设置响应游戏角色的受攻击信号 */
		GameObject_AtUnderAttack( player_data.object, GamePlayer_ResponseAttack );
		/* 将游戏对象添加进对战场景 */
		GameObject_AddToContainer( player_data.object, p_battle->scene );
		/* 记录玩家数据 */
		Queue_Add( &p_battle->player_list, &player_data );
		player = GameBattle_GetPlayer( battle_id, player_id );
		if( !player ) {
			return -1;
		}
	}
	
	p_info = Game_GetRoleInfo( role_id );
	if( !p_info ) {
		return -2;
	}

	player->type = p_info->type;
	player->role_id = role_id;
	player->property = p_info->property;
	player->human_control = human_control;

	for(i=0; i<p_info->total_skill && i<MAX_SKILL_NUM; ++i) {
		GamePlayer_EnableSkill( player, p_info->skills[i] );
		if( strcmp(p_info->skills[i], SKILLNAME_MACH_A_ATTACK) == 0 ) {
			GamePlayer_EnableSkill( player, SKILLNAME_JUMP_MACH_A_ATTACK );
		}
		else if( strcmp(p_info->skills[i], SKILLNAME_MACH_B_ATTACK) == 0 ) {
			GamePlayer_EnableSkill( player, SKILLNAME_JUMP_MACH_B_ATTACK );
		}
	}
	/* 初始化角色动作动画 */
	GamePlayer_InitAction( player, role_id );
	/* 根据职业来选择需要启用的特殊技能 */
	switch( player->type ) {
	case PLAYER_TYPE_FIGHTER:
		GamePlayer_EnableSkill( player, SKILLNAME_HUG_FRONT_PUT );
		GamePlayer_EnableSkill( player, SKILLNAME_HUG_BACK_PUT );
		break;
	case PLAYER_TYPE_MARTIAL_ARTIST:
		GamePlayer_EnableSkill( player, SKILLNAME_KNEEHIT );
		GamePlayer_EnableSkill( player, SKILLNAME_ELBOW );
		break;
	case PLAYER_TYPE_KUNG_FU:
		GamePlayer_EnableSkill( player, SKILLNAME_LIFT_JUMP );
		GamePlayer_EnableSkill( player, SKILLNAME_HUG_JUMP );
		break;
	case PLAYER_TYPE_JUDO_MASTER:
		GamePlayer_EnableSkill( player, SKILLNAME_JUDO );
		GamePlayer_EnableSkill( player, SKILLNAME_BACK_JUDO );
		break;
	case PLAYER_TYPE_TIGER:
		GamePlayer_EnableSkill( player, SKILLNAME_SPIN_DRILL );
	default:
		break;
	}
	return 0;
}

/** 设置是否启用一个玩家 */
int GameBattle_EnablePlayer(	int battle_id, 
				int player_id, 
				LCUI_BOOL flag )
{
	GamePlayer *player;
	player = GameBattle_GetPlayer( battle_id, player_id );
	if( !player ) {
		return -1;
	}
	player->enable = flag;
	return 0;
}

/** 设置指定对战中的玩家是否无敌 */
int GameBattlePlayer_SetInvincible(	int battle_id, 
					int player_id, 
					LCUI_BOOL flag )
{
	GamePlayer *player;
	player = GameBattle_GetPlayer( battle_id, player_id );
	if( !player ) {
		return -1;
	}
	player->is_invincible = flag;
	return 0;
}

/** 设置场景地面参数 */
int GameBattle_SetSceneLand(	int battle_id,
				LCUI_Pos land_pos, 
				LCUI_Size land_size )
{
	BattleData *p_battle;
	p_battle = GameBattle_GetBattle( battle_id );
	if( !p_battle ) {
		return -1;
	}
	p_battle->scene_land_pos = land_pos;
	p_battle->scene_land_size = land_size;
	/* 设置战斗场景的空间边界 */
	GameSpace_SetBound(	p_battle->space, 
				land_pos.x, land_size.w, 
				land_pos.y, land_size.h );
	return 0;
}

/** 获取场景地面参数 */
int GameBattle_GetSceneLand(	int battle_id,
				LCUI_Pos *land_pos, 
				LCUI_Size *land_size )
{
	BattleData *p_battle;
	p_battle = GameBattle_GetBattle( battle_id );
	if( !p_battle ) {
		return -1;
	}
	land_pos->x = p_battle->scene_land_pos.x;
	land_pos->y = p_battle->scene_land_pos.y;
	land_size->w = p_battle->scene_land_size.w;
	land_size->h = p_battle->scene_land_size.h;
	return 0;
}

/** 设置场景大小 */
int GameBattle_SetSceneSize( int battle_id, LCUI_Size size )
{
	BattleData *p_battle;
	p_battle = GameBattle_GetBattle( battle_id );
	if( !p_battle ) {
		return -1;
	}
	Widget_Resize( p_battle->scene, size );
	return 0;
}

/** 设置场景背景图 */
int GameBattle_SetSceneBackground(	int battle_id, 
					LCUI_Graph *img )
{
	return 0;
}

/** 获取场景 */
LCUI_Widget* GameBattle_GetScene( int battle_id )
{
	BattleData *p_battle;
	p_battle = GameBattle_GetBattle( battle_id );
	if( !p_battle ) {
		return NULL;
	}
	return p_battle->scene;
}

static int one_frame_remain_time;
static int64_t prev_frame_start_time;

/** 初始化帧数控制 */
static void FrameControl_Init( int ms_per_frame )
{
	one_frame_remain_time = ms_per_frame;
	prev_frame_start_time = LCUI_GetTickCount();
}

/** 让当前帧停留一段时间 */
static void FrameControl_RemainFrame(void)
{
	int n_ms;
	int64_t current_time;

	current_time = LCUI_GetTickCount();
	n_ms = (int)(current_time - prev_frame_start_time);
	if( n_ms < one_frame_remain_time ) {
		n_ms = one_frame_remain_time - n_ms;
		if( n_ms > 0 ) {
			LCUI_MSleep( n_ms );
		}
	}
	prev_frame_start_time += one_frame_remain_time;
}

static void GameBattle_ProcGameObject( void *arg )
{
	BattleData* p_battle;
	p_battle = (BattleData*)arg;
	/* 初始化帧数控制 */
	FrameControl_Init( 1000/FRAMES_PER_SEC );
	while(1) {
		GameSpace_Step( p_battle->space );
		GameObjectLibrary_UpdateAction( &p_battle->gameobject_library );
		GameObjectLibrary_DispatchEvent( &p_battle->gameobject_library );
		FrameControl_RemainFrame();
	}
}

/** 进入循环 */
int GameBattle_Loop( int battle_id )
{
	LCUI_Thread th;
	int i, n, n_found;
	BattleData* p_battle;
	GamePlayer* p_player;

	p_battle = GameBattle_GetBattle( battle_id );
	if( !p_battle ) {
		return -1;
	}
	/* 初始化游戏AI */
	GameAI_Init();
	/* 创建一个线程，用于处理游戏对象的数据更新 */
	LCUIThread_Create( &th, GameBattle_ProcGameObject, p_battle );
	/* 循环更新游戏数据 */
	while(1) {
		n = Queue_GetTotal( &p_battle->player_list );
		for(n_found=0,i=0; i<n; ++i) {
			p_player = (GamePlayer*)Queue_Get( &p_battle->player_list, i );
			if( !p_player ) {
				continue;
			}
			if( !p_player->enable
			 || !p_player->local_control ) {
				continue;
			}
			++n_found;
			/* 如果该游戏玩家不是由人类控制的 */
			if( !p_player->human_control ) {
				GameAI_Control( p_battle->id, p_player->id );
			} else {
				GamePlayer_SyncKeyControl( p_player );
			}
			GamePlayer_SyncData( p_player );
			Widget_Update( p_player->object );
			/* 将第一个有效的游戏角色设置为镜头焦点 */
			if( n_found == 1 ) {
				GameScene_SetCameraTarget( p_player->object );
			}
		}
		/* 如果需要更新镜头 */
		if( p_battle->need_sync_camera ) {
			GameScene_UpdateCamera();
		}
		/* 处理攻击 */
		Game_ProcAttack();
		LCUI_MSleep(10);
	}
	return 0;
}
