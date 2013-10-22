#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__
/** 获取游戏场景 */
LCUI_Widget *GetGameScene(void);

/** 初始化战斗场景 */
int GameScene_Init( void );

/** 获取战斗场景的地面尺寸 */
void GameScene_GetLandSize( LCUI_Size *size );

/** 获取空间地面在场景中的起点X轴坐标 */
void GameScene_GetLandStartX( int *x );

/** 获取空间地面在场景中的起点Y轴坐标 */
void GameScene_GetLandStartY( int *y );

/** 设置场景镜头焦点 */
int GameScene_SetCameraTarget( LCUI_Widget *target );

/** 更新场景上的镜头位置，使目标游戏对象处于镜头区域内 */
int GameScene_UpdateCamera( void );

/** 退出战斗场景，释放占用的内存资源 */
void GameScene_Quit( void );

void GameDemoScene_Quit( void );

#endif
