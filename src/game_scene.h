
/** 初始化战斗场景 */
int GameScene_Init( LCUI_Widget *scene );

/** 获取战斗场景的地面尺寸 */
void GameScene_GetLandSize( LCUI_Widget *scene, LCUI_Size *size );

/** 获取空间地面在场景中的起点Y轴坐标 */
void GameScene_GetLandStartY( LCUI_Widget *scene, int *y );

/** 获取空间地面在场景中的起点X轴坐标 */
void GameScene_GetLandStartX( LCUI_Widget *scene, int *x );

/** 更新场景上的镜头位置，使目标游戏对象处于镜头区域内 */
int GameScene_UpdateCamera( LCUI_Widget *scene, LCUI_Widget *target );

/** 退出战斗场景，释放占用的内存资源 */
void GameScene_Quit( LCUI_Widget *scene );
