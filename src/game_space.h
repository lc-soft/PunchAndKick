#ifndef __PHYSICS_SYSTEM_H__
#define __PHYSICS_SYSTEM_H__

#define FRAMES_PER_SEC	50

/** 游戏空间 */
typedef struct GameSpaceData_ {
	LCUI_Queue space_object_list;
	int space_x, space_x_width;
	int space_y, space_y_width;
} GameSpaceData;

/**

坐标系如下：

       Z |
         |
         |
         |
         |_____________ X
        /
       /
      /  Y

ACC: acceleration 加速度
**/

typedef struct SpaceObject_ {
	GameSpaceData *space;		/**< 所属空间 */
	double x,y,z;			/**< 对象在空间中的坐标 */
	unsigned int x_width;		/**< 在X轴上的宽度 */
	unsigned int y_width;		/**< 在Y轴上的宽度 */
	unsigned int z_width;		/**< 在Z轴上的宽度 */
	double x_speed;			/**< 在X轴上的移动速度 */
	double y_speed;			/**< 在Y轴上的移动速度 */
	double z_speed;			/**< 在Z轴上的移动速度 */
	double x_acc;			/**< 在X轴上的加速度 */
	double y_acc;			/**< 在Y轴上的加速度 */
	double z_acc;			/**< 在Z轴上的加速度 */
} SpaceObject;

/** 设置空间边界  */
void GameSpace_SetBound( GameSpaceData *space, int x, int x_width, int y, int y_width );

/** 获取Y轴空间范围 */
void GameSpace_GetXBound( GameSpaceData *space, int* x, int* x_width );

/** 获取X轴空间范围 */
void GameSpace_GetYBound( GameSpaceData *space, int* y, int* y_width );

/** 处理空间内的对象的运动 */
void GameSpace_Step( GameSpaceData *space );

/** 创建一个游戏空间 */
GameSpaceData *GameSpace_New(void);

/**
创建一个新的对象
@param x
	对象所在的X轴的坐标
@param y
	对象所在的Y轴的坐标
@param z
	对象所在的Z轴的坐标
@param x_width
	对象在X轴上的宽度
@param y_width
	对象在Y轴上的宽度
@param z_width
	对象在Z轴上的宽度
@return
	创建成功则返回该对象，失败则返回NULL
*/
SpaceObject* SpaceObject_New( GameSpaceData *space, int x, int y, int z, int x_width, int y_width, int z_width );

void SpaceObject_Destroy( SpaceObject* object );

#endif
