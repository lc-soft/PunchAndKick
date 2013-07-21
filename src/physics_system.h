#ifndef __PHYSICS_SYSTEM_H__
#define __PHYSICS_SYSTEM_H__

#define MSEC_PER_FRAME 10

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

typedef struct PhysicsObject_ {
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
} PhysicsObject;


/** 处理物理对象的数据 */
extern void PhysicsSystem_Step( void );

/**
创建一个新的物理对象
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
extern PhysicsObject* PhysicsObject_New( int x, int y, int z, int x_width, int y_width, int z_width );

#endif
