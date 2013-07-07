#include <LCUI_Build.h>
#include LC_LCUI_H

#include "physics_system.h"

static LCUI_BOOL system_thread_active = FALSE;
static LCUI_BOOL system_thread_pause = FALSE;
static LCUI_Thread system_thread_id = 0;
static LCUI_Queue physics_object_list;

/** 处理物理对象的数据 */
static void PhysicsSystem_Porc( void *arg )
{
	int i, n;
	PhysicsObject *obj;

	while(system_thread_active) {
		if( system_thread_pause ) {
			LCUI_MSleep(SYNC_INTERVAL_TIME);
			continue;
		}
		Queue_Lock( &physics_object_list );
		n = Queue_GetTotal( &physics_object_list );
		for(i=0; i<n; ++i) {
			obj = (PhysicsObject*)Queue_Get(
					&physics_object_list, i );
			if( !obj ) {
				continue;
			}
			/* 更新对象的空间坐标 */
			obj->x += ( obj->x_speed + obj->x_acc );
			obj->y += ( obj->y_speed + obj->y_acc );
			obj->z += ( obj->z_speed + obj->z_acc );
		}
		Queue_Unlock( &physics_object_list );
		LCUI_MSleep(SYNC_INTERVAL_TIME);
	}
	LCUIThread_Exit(NULL);
}

/** 让物理系统开始运作 */
int PhysicsSystem_Start(void)
{
	if( system_thread_active || system_thread_id != 0 ) {
		return -1;
	}
	system_thread_active = TRUE;
	Queue_Init( &physics_object_list, sizeof(PhysicsObject), NULL );
	LCUIThread_Create( &system_thread_id, PhysicsSystem_Porc, NULL );
	return 0;
}

/** 让物理系统暂停运作 */
void PhysicsSystem_Pause(void)
{
	system_thread_pause = TRUE;
}

/** 让物理系统继续运作 */
void PhysicsSystem_Continue(void)
{
	system_thread_pause = FALSE;
}

/** 让物理系统停止运作 */
int PhysicsSystem_Stop(void)
{
	if( !system_thread_active || system_thread_id == 0 ) {
		return -1;
	}
	system_thread_active = FALSE;
	LCUIThread_Join( system_thread_id, NULL );
	Queue_Destroy( &physics_object_list );
	return 0;
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
	Queue_Lock( &physics_object_list );
	pos = Queue_Add( &physics_object_list, &obj );
	p = (PhysicsObject*)Queue_Get( &physics_object_list, pos );
	Queue_Unlock( &physics_object_list );
	return p;
}
