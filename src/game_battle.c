//game_demo.c
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_DISPLAY_H

#include "game.h"
#include "skills/game_skill.h"
#include "game_value_tip.h"

typedef struct BattleFrameStatus_ {
	LCUI_BOOL is_run;
	LCUI_Sleeper wait_pause;
	LCUI_Sleeper wait_continue;
	int one_frame_remain_time;
	int64_t prev_frame_start_time;
} BattleFrameStatus;

typedef struct BattleData_ {
	int id;
	LCUI_Queue player_list;
	LCUI_Widget *scene;
	LCUI_Size scene_size;
	LCUI_Pos scene_land_pos;
	LCUI_Size scene_land_size;
	LCUI_BOOL need_sync_camera;
	LCUI_Widget *camera_target;
	int camera_x_padding;
	GameSpaceData *space;
	LCUI_Queue gameobject_library;
	LCUI_Queue attack_record;
	BattleFrameStatus animation_frame;
	BattleFrameStatus data_proc_frame;
	ValueTipData value_tip_proc;
} BattleData;

typedef struct FrameControlData_ {
	int one_frame_remain_time;
	int64_t prev_frame_start_time;
} FrameControlData;

static LCUI_BOOL list_is_inited = FALSE;
static LCUI_Queue battle_list;

/** 初始化游戏帧数控制 */
static void GameBattleFrame_Init(	BattleFrameStatus *p_status,
					int ms_per_frame )
{
	p_status->is_run = TRUE;
	p_status->one_frame_remain_time = ms_per_frame;
	p_status->prev_frame_start_time = LCUI_GetTickCount();
	LCUISleeper_Create( &p_status->wait_continue );
	LCUISleeper_Create( &p_status->wait_pause );
}

/** 暂停游戏数据帧的更新 */
static void GameBattleFrame_Pause(	BattleFrameStatus *p_status, 
					LCUI_BOOL need_pause )
{
	if( p_status->is_run && need_pause ) {
		LCUISleeper_BreakSleep( &p_status->wait_pause );
	}
	else if( !p_status->is_run && !need_pause ){
		LCUISleeper_BreakSleep( &p_status->wait_continue );
	}
	p_status->is_run = !need_pause;
}

/** 让当前帧停留一定时间 */
static void GameBattleFrame_Remain( BattleFrameStatus *p_status )
{
	int n_ms, lost_ms;
	int64_t current_time;

	current_time = LCUI_GetTickCount();
	n_ms = (int)(current_time - p_status->prev_frame_start_time);
	if( n_ms > p_status->one_frame_remain_time ) {
		goto normal_exit;
	}
	n_ms = p_status->one_frame_remain_time - n_ms;
	if( n_ms < 1 ) {
		goto normal_exit;
	}
	LCUISleeper_StartSleep( &p_status->wait_pause, n_ms );
	if( p_status->is_run ) {
		goto normal_exit;
	}

	lost_ms = LCUISleeper_StartSleep( &p_status->wait_continue, MAXINT32 );
	p_status->prev_frame_start_time += lost_ms;
	return;

normal_exit:
	p_status->prev_frame_start_time += p_status->one_frame_remain_time;
}

static void GamePlayerList_Delete( void* arg )
{

}

static void GameBattleList_Delete( void* arg )
{

}

/** 获取指定ID的对战数据 */
static BattleData *GameBattle_GetBattle( int battle_id )
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
	data.camera_x_padding = 200;
	data.scene_land_pos.x = 0;
	data.scene_land_pos.y = 0;
	data.scene_land_size.w = 0;
	data.scene_land_size.h = 0;
	data.camera_target = NULL;
	data.space = GameSpace_New();
	data.scene = Widget_New(NULL);
	data.value_tip_proc.is_inited = FALSE;
	Queue_Init( &data.player_list, sizeof(GamePlayer), GamePlayerList_Delete );
	Game_InitAttackRecord( &data.attack_record );
	GameObjectLibrary_Create( &data.gameobject_library );
	Queue_Add( &battle_list, &data );
	return data.id;
}

/** 退出对战 */
int GameBattle_Quit( int battle_id )
{
	return 0;
}

/** 设置是否启用数值提示功能 */
int GameBattle_SetEnableValueTip( int battle_id, LCUI_BOOL need_enable )
{
	BattleData *p_battle;
	
	p_battle = GameBattle_GetBattle( battle_id );
	if( !p_battle ) {
		return -1;
	}

	if( need_enable == p_battle->value_tip_proc.is_inited ) {
		return 1;
	}
	if( !need_enable && p_battle->value_tip_proc.is_inited ) {
		p_battle->value_tip_proc.is_inited = FALSE;
		GameValueTip_Quit( &p_battle->value_tip_proc );
	}
	p_battle->value_tip_proc.is_inited = TRUE;
	GameValueTip_Init( &p_battle->value_tip_proc, p_battle->scene );
	return 0;
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

/** 设置是否自动同步镜头 */
int GameBattle_SetAutoSyncCamera( int battle_id, LCUI_BOOL is_true )
{
	BattleData *p_battle;
	p_battle = GameBattle_GetBattle( battle_id );
	if( !p_battle ) {
		return -1;
	}
	p_battle->need_sync_camera = is_true;
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
	p_battle->scene_size = size;
	Widget_Resize( p_battle->scene, size );
	return 0;
}

/** 获取场景大小 */
int GameBattle_GetSceneSize( int battle_id, LCUI_Size *size )
{
	BattleData *p_battle;
	p_battle = GameBattle_GetBattle( battle_id );
	if( !p_battle ) {
		return -1;
	}
	*size = p_battle->scene_size;
	return 0;
}

/** 设置场景背景图 */
int GameBattle_SetSceneBackground(	int battle_id, 
					LCUI_Graph *stage_img )
{
	BattleData *p_battle;
	p_battle = GameBattle_GetBattle( battle_id );
	if( !p_battle ) {
		return -1;
	}
	Widget_SetBackgroundImage( p_battle->scene, stage_img );
	Widget_SetBackgroundLayout( p_battle->scene, LAYOUT_CENTER );
	Widget_SetBackgroundColor( p_battle->scene, RGB(0,0,0) );
	Widget_SetBackgroundTransparent( p_battle->scene, FALSE );
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

/** 获取攻击记录 */
LCUI_Queue* GameBattle_GetAttackRecord( int battle_id )
{
	BattleData *p_battle;
	p_battle = GameBattle_GetBattle( battle_id );
	if( !p_battle ) {
		return NULL;
	}
	return &p_battle->attack_record;
}

/** 更新场景上的镜头位置，使目标游戏对象处于镜头区域内 */
static int GameScene_UpdateCamera(	LCUI_Widget *game_scene, 
					LCUI_Widget *camera_target, 
					int x_padding )
{
	int target_x, offset_x, wdg_scene_x, screen_width, start_x;
	LCUI_Size scene_size;
	static int speed = 1;

	scene_size = Widget_GetSize( game_scene );
	screen_width = LCUIScreen_GetWidth();
	start_x = (screen_width - scene_size.w)/2;
	target_x = (int)GameObject_GetX( camera_target );
	wdg_scene_x = Widget_GetPos( game_scene ).x;
	/* 既然大于0，则说明战斗场景小于屏幕区域，无需移动镜头 */
	if( wdg_scene_x > 0 ) {
		return 0;
	}
	/* 如果目标位置在镜头的左内边框的左边 */
	if( wdg_scene_x + target_x < x_padding ) {
		/* 计算出战斗场景的位置 */
		offset_x = x_padding - target_x;
		/* 判断镜头是否会超出战斗场景的范围 */
		if( offset_x > 0 ) {
			offset_x = 0;
		}
		/* 得出坐标偏移量 */
		offset_x = offset_x - start_x;
		/* 将战斗场景向右移动 */
		Widget_SetAlign( game_scene, ALIGN_BOTTOM_CENTER, Pos(offset_x,-STATUS_BAR_HEIGHT) );
		return 0;
	}
	/* 如果目标位置在镜头的右内边框的右边 */
	if( wdg_scene_x + target_x > screen_width - x_padding ) {
		offset_x = screen_width - x_padding - target_x;
		if( offset_x < screen_width - scene_size.w ) {
			offset_x = screen_width - scene_size.w;
		}
		offset_x = offset_x - start_x;
		Widget_SetAlign( game_scene, ALIGN_BOTTOM_CENTER, Pos(offset_x,-STATUS_BAR_HEIGHT) );
	}
	return 0;
}

static void GameBattle_ProcGameObject( void *arg )
{
	BattleData* p_battle;
	p_battle = (BattleData*)arg;
	/* 初始化游戏动画帧处理 */
	GameBattleFrame_Init( &p_battle->animation_frame, 20 );
	while(1) {
		GameSpace_Step( p_battle->space );
		GameObjectLibrary_UpdateAction( &p_battle->gameobject_library );
		GameObjectLibrary_DispatchEvent( &p_battle->gameobject_library );
		GameBattleFrame_Remain( &p_battle->animation_frame );
	}
}

/** 设置是否暂停对战 */
int GameBattle_Pause( int battle_id, LCUI_BOOL need_pause )
{
	BattleData* p_battle;

	p_battle = GameBattle_GetBattle( battle_id );
	if( !p_battle ) {
		return -1;
	}
	GameBattleFrame_Pause( &p_battle->data_proc_frame, need_pause );
	GameBattleFrame_Pause( &p_battle->animation_frame, need_pause );
	return 0;
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
	/* 初始化游戏数据帧处理 */
	GameBattleFrame_Init( &p_battle->data_proc_frame, 20 );
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
				p_battle->camera_target = p_player->object;
			}
		}
		/* 如果需要更新镜头 */
		if( p_battle->need_sync_camera && p_battle->camera_target ) {
			GameScene_UpdateCamera( p_battle->scene, 
						p_battle->camera_target, 
						p_battle->camera_x_padding );
		}
		/* 处理攻击 */
		Game_ProcAttack( &p_battle->attack_record, &p_battle->value_tip_proc );
		/* 本帧数据处理完后，停留一段时间 */
		GameBattleFrame_Remain( &p_battle->data_proc_frame );
	}
	return 0;
}
