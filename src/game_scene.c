#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_DISPLAY_H

#include "game.h"

#define CAMERA_X_PANDDING 200

static GameSpaceData *game_space;
static LCUI_Widget *game_scene;
static LCUI_Size scene_land_size = {0,0};
static LCUI_Pos scene_land_pos = {0,0};
static LCUI_Widget *camera_target = NULL;

/** 获取游戏场景 */
LCUI_Widget *GetGameScene(void)
{
	return game_scene;
}
#ifdef aaaaa
/** 初始化演示场景 */
int GameDemoScene_Init(void)
{
	int ret;
	LCUI_Size scene_size;

	scene_size.w = 800;
	scene_size.h = 600;
	game_demo_scene = Widget_New(NULL);
	if( !game_demo_scene ) {
		return -1;
	}
	Widget_Resize( game_demo_scene, scene_size );
	Widget_SetAlign( game_demo_scene, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	demo_scene_land_pos.x = 50;
	demo_scene_land_pos.y = 400;
	demo_scene_land_size.w = scene_size.w - 2*demo_scene_land_pos.x;
	demo_scene_land_size.h = 10;
	GameSpace_SetBound(	demo_scene_land_pos.x, 
				demo_scene_land_size.w, 
				demo_scene_land_pos.y,
				demo_scene_land_size.h );
	return 0;
}
#endif

/** 初始化战斗场景 */
int GameScene_Init( void )
{
	int ret;
	LCUI_Size scene_size;
	LCUI_Graph scene_graph;
	Graph_Init( &scene_graph );
	ret = GameGraphRes_LoadFromFile("scenes.data");
	if( ret != 0 ) {
		LCUI_MessageBoxW(
			MB_ICON_ERROR,
			L"场景资源载入出错，请检查程序的完整性！",
			L"错误", MB_BTN_OK );
		return ret;
	}
	/* 载入战斗场景的图形资源 */
	GameGraphRes_GetGraph( SCENES_RES, "default", &scene_graph );
	scene_size = Graph_GetSize( &scene_graph );
	game_scene = Widget_New(NULL);
	game_space = GameSpace_New();
	/* 设置战斗场景 */
	Widget_SetBackgroundImage( game_scene, &scene_graph );
	Widget_SetBackgroundLayout( game_scene, LAYOUT_CENTER );
	Widget_SetBackgroundColor( game_scene, RGB(0,0,0) );
	Widget_SetBackgroundTransparent( game_scene, FALSE );
	/* 调整战斗场景的尺寸 */
	Widget_Resize( game_scene, scene_size );
	/* 设置战斗场景显示的位置 */
	Widget_SetAlign( game_scene, ALIGN_BOTTOM_CENTER, Pos(0,-STATUS_BAR_HEIGHT) );
	scene_land_pos.x = 20;
	scene_land_pos.y = 268;
	/* 记录当前战斗场景的地面尺寸 */
	scene_land_size.w = scene_size.w - 2*scene_land_pos.x;
	scene_land_size.h = scene_size.h - scene_land_pos.y;
	/* 设置战斗场景的空间边界 */
	GameSpace_SetBound(	game_space, 
				scene_land_pos.x, 
				scene_land_size.w, 
				scene_land_pos.y, 
				scene_land_size.h );
	return 0;
}

/** 获取战斗场景的地面尺寸 */
void GameScene_GetLandSize( LCUI_Size *size )
{
	*size = scene_land_size;
}

/** 获取空间地面在场景中的起点X轴坐标 */
void GameScene_GetLandStartX( int *x )
{
	*x = scene_land_pos.x;
}

/** 获取空间地面在场景中的起点Y轴坐标 */
void GameScene_GetLandStartY( int *y )
{
	*y = scene_land_pos.y;
}

/** 设置场景镜头焦点 */
int GameScene_SetCameraTarget( LCUI_Widget *target )
{
	if( target->parent != game_scene ) {
		return -1;
	}
	camera_target = target;
	return 0;
}

/** 更新场景上的镜头位置，使目标游戏对象处于镜头区域内 */
int GameScene_UpdateCamera( void )
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
	if( wdg_scene_x + target_x < CAMERA_X_PANDDING ) {
		/* 计算出战斗场景的位置 */
		offset_x = CAMERA_X_PANDDING - target_x;
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
	if( wdg_scene_x + target_x > screen_width - CAMERA_X_PANDDING ) {
		offset_x = screen_width - CAMERA_X_PANDDING - target_x;
		if( offset_x < screen_width - scene_size.w ) {
			offset_x = screen_width - scene_size.w;
		}
		offset_x = offset_x - start_x;
		Widget_SetAlign( game_scene, ALIGN_BOTTOM_CENTER, Pos(offset_x,-STATUS_BAR_HEIGHT) );
	}
	return 0;
}

/** 退出战斗场景，释放占用的内存资源 */
void GameScene_Quit( void )
{
	Widget_SetBackgroundImage( game_scene, NULL );
	GameGraphRes_Free( SCENES_RES );
	Widget_Destroy( game_scene );
}
