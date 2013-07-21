#include <LCUI_Build.h>
#include LC_LCUI_H

#include "physics_system.h"

static LCUI_BOOL init = FALSE;
static LCUI_Queue physics_object_list;

/** 处理物理对象的数据 */
void PhysicsSystem_Step( void )
{
	int i, n;
	PhysicsObject *obj;

	Queue_Lock( &physics_object_list );
	n = Queue_GetTotal( &physics_object_list );
	for(i=0; i<n; ++i) {
		obj = (PhysicsObject*)Queue_Get(
				&physics_object_list, i );
		if( !obj ) {
			continue;
		}
		/* 更新对象的空间坐标 */
		obj->x_speed += (obj->x_acc/MSEC_PER_FRAME);
		obj->y_speed += (obj->y_acc/MSEC_PER_FRAME);
		obj->z_speed += (obj->z_acc/MSEC_PER_FRAME);
		obj->x += (obj->x_speed/MSEC_PER_FRAME);
		obj->y += (obj->y_speed/MSEC_PER_FRAME);
		obj->z += (obj->z_speed/MSEC_PER_FRAME);
	}
	Queue_Unlock( &physics_object_list );
}


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
PhysicsObject* PhysicsObject_New( int x, int y, int z, int x_width, int y_width, int z_width )
{
	int pos;
	PhysicsObject *p, obj;

	obj.x = x;
	obj.y = y;
	obj.z = z;
	obj.x_width = x_width;
	obj.y_width = y_width;
	obj.z_width = z_width;
	obj.x_speed = 0;
	obj.y_speed = 0;
	obj.z_speed = 0;
	obj.x_acc = 0;
	obj.y_acc = 0;
	obj.z_acc = 0;
	if( !init ) {
		Queue_Init( &physics_object_list, sizeof(PhysicsObject), NULL ); 
		init = TRUE;
	}
	Queue_Lock( &physics_object_list );
	pos = Queue_Add( &physics_object_list, &obj );
	p = (PhysicsObject*)Queue_Get( &physics_object_list, pos );
	Queue_Unlock( &physics_object_list );
	return p;
}
