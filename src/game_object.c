#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H

#include "game_object.h"
#include "physics_system.h"

#include <time.h>

#define PAUSE	0
#define PLAY	1

/** 每次数据刷新时的时间间隔(毫秒) */
#define REFRESH_INTERVAL_TIME 20

/** 动作记录 */
typedef struct ActionRec_ {
	int id;			/**< 动画的标识号 */
	int obj_id;		/**< 对象的标识号 */
	ActionData *action;	/**< 对应的动作集 */
} ActionRec;

/** 动作的状态 */
typedef struct ActionStatus_ {
	int obj_id;		/**< 对象标识号 */
	int state;		/**< 状态，指示播放还是暂停 */
	int current;		/**< 记录当前帧动作的序号，帧序号从1开始 */
	ActionData *action;	/**< 对应的动作集 */
	LCUI_Func func;		/**< 被关联的回调函数 */
} ActionStatus;

typedef struct GameObject_ {
	int state;			/**< 当前状态 */
	int x, y;			/**< 底线中点坐标 */
	int w, h;			/**< 动作容器的尺寸 */
	int global_bottom_line_y;	/**< 底线的Y轴坐标 */
	int global_center_x;		/**< 中心点的X轴坐标 */
	LCUI_BOOL data_valid;		/**< 当前使用的数据是否有效 */
	ActionRec *current;		/**< 当前动作动画 */
	LCUI_Queue action_list;		/**< 动作列表 */
	PhysicsObject *phys_obj;	/**< 对应的物理对象 */
} GameObject;

static LCUI_Queue action_database;
static LCUI_Queue action_stream;

static int database_init = FALSE;
static int frame_proc_timer = -1;

static void ActionStatus_Destroy( void *arg )
{

}

static void ActionStatus_Init( ActionStatus *p_status )
{
	p_status->obj_id = 0;
	p_status->action = NULL;
	p_status->current = 0;
	p_status->state = PAUSE;
	p_status->func.func = NULL;
}

static void Action_CallFunc( ActionStatus *p_status )
{
	AppTasks_CustomAdd( ADD_MODE_REPLACE | AND_ARG_S, &p_status->func );
}

static void ActionData_Destroy( void *arg )
{
	ActionData *action;
	action = (ActionData*)arg;
	Queue_Destroy( &action->frame );
}

static ActionStatus* Action_GetStatus( ActionData *action, int obj_id )
{
	int i, n;
	ActionStatus *p_status;

	if( action == NULL ) {
		return NULL;
	}
	Queue_Lock( &action_stream );
	n = Queue_GetTotal( &action_stream );
	for( i=0; i<n; ++i ) {
		p_status = (ActionStatus*)Queue_Get( &action_stream, i );
		if( !p_status || p_status->action != action ) {
			continue;
		}
		if( obj_id == p_status->obj_id ) {
			Queue_Unlock( &action_stream );
			return p_status;
		}
	}
	Queue_Unlock( &action_stream );
	return NULL;
}

/**
 * 创建一个动作集
 * 创建的动作集将记录至动作库中
 * @returns
 *	正常则返回指向动作库中的该动作集的指针，失败则返回NULL
 */
LCUI_API ActionData* Action_Create( void )
{
	int pos;
	ActionData *p, action;

	Queue_Init( &action.frame, sizeof(ActionFrameData), NULL );
	
	if( !database_init ) {
		Queue_Init(	&action_database,
				sizeof(ActionData),
				ActionData_Destroy );
		database_init = TRUE;
	}
	Queue_Lock( &action_database );
	/* 记录该动画至库中 */
	pos = Queue_Add( &action_database, &action );
	p = (ActionData*)Queue_Get( &action_database, pos );
	Queue_Unlock( &action_database );
	DEBUG_MSG("create action: %p\n", p);
	return p;
}

static int ActionStream_Delete( ActionData *action )
{
	int n;
	ActionData* tmp;

	Queue_Lock( &action_stream );
	n = Queue_GetTotal( &action_stream );
	for(n; n>=0; --n) {
		tmp = (ActionData*)Queue_Get( &action_stream, n );
		if( tmp == action ) {
			Queue_Delete( &action_stream, n );
			break;
		}
	}
	Queue_Unlock( &action_stream );
	if( n < 0 ) {
		return -1;
	}
	return 0;
}

/**
 * 删除一个动画
 * 从动画库中删除指定的动画
 * @param action
 *	需删除的动画
 * @returns
 *	正常则返回0，失败则返回-1
 */
LCUI_API int Action_Delete( ActionData* action )
{
	int n;
	ActionData* tmp;

	ActionStream_Delete( action );

	Queue_Lock( &action_database );
	n = Queue_GetTotal( &action_database );
	for(n; n>=0; --n) {
		tmp = (ActionData*)Queue_Get( &action_database, n );
		if( tmp == action ) {
			Queue_Delete( &action_database, n );
			break;
		}
	}
	Queue_Unlock( &action_database );
	if( n < 0 ) {
		return -1;
	}
	return 0;
}

/**
 * 为动作关联回调函数
 * 关联回调函数后，动画每更新一帧都会调用该函数
 * @param action
 *	目标动画
 * @param obj_id
 *	使用该动作的对象的ID
 * @param func
 *	指向回调函数的函数指针
 * @param arg
 *	需传递给回调函数的第二个参数
 * @returns
 *	正常则返回0，失败则返回-1
 * @warning
 *	必须在动画被第一次播放后调用此函数，否则将因找不到该动画的播放实例而导致关联失败
 * */
LCUI_API int Action_Connect(	ActionData *action,
				int obj_id,
				void (*func)(void*),
				void *arg )
{
	ActionStatus *p_status;

	if( !action ) {
		return -1;
	}
	p_status = Action_GetStatus( action, obj_id );
	if( p_status == NULL ) {
		return -1;
	}
	p_status->func.func = (CallBackFunc)func;
	p_status->func.id = LCUIApp_GetSelfID();
	p_status->func.arg[0] = arg;
	p_status->func.arg[1] = NULL;
	p_status->func.destroy_arg[0] = FALSE;
	p_status->func.destroy_arg[1] = FALSE;
	return 0;
}

static void ActionStream_Sort(void)
{
	int i, j, pos, total;
	ActionStatus *p_status;
	ActionFrameData *p, *q;

	Queue_Lock( &action_stream );
	total = Queue_GetTotal( &action_stream );
	for(i=0; i<total; ++i) {
		p_status = (ActionStatus*)Queue_Get( &action_stream, i );
		if( !p_status ) {
			continue;
		}
		if(p_status->current > 0) {
			pos = p_status->current-1;
		} else {
			pos = 0;
		}
		p = (ActionFrameData*)Queue_Get( &p_status->action->frame, pos );
		if( !p ) {
			continue;
		}

		for(j=i+1; j<total; ++j) {
			p_status = (ActionStatus*)Queue_Get( &action_stream, j );
			if( !p_status ) {
				continue;
			}
			if(p_status->current > 0) {
				pos = p_status->current-1;
			} else {
				pos = 0;
			}
			q = (ActionFrameData*)Queue_Get( &p_status->action->frame, pos );
			if( !q ) {
				continue;
			}
			if( q->current_time < p->current_time ) {
				Queue_Swap( &action_stream, j, i );
			}
		}
	}
	Queue_Unlock( &action_stream );
}

static void ActionStream_TimeSub( int time )
{
	ActionFrameData *frame;
	ActionStatus *p_status;
	int i, total, pos;

	Queue_Lock( &action_stream );
	total = Queue_GetTotal(&action_stream);
	DEBUG_MSG("start\n");
	for(i=0; i<total; ++i) {
		p_status = (ActionStatus*)Queue_Get( &action_stream, i );
		if( !p_status || p_status->state == PAUSE ) {
			continue;
		}
		if( p_status->current > 0 ) {
			pos = p_status->current-1;
		} else {
			pos = 0;
		}
		frame = (ActionFrameData*)
			Queue_Get( &p_status->action->frame, pos );
		if( !frame ) {
			continue;
		}
		frame->current_time -= time;
		DEBUG_MSG("action: %p, current: %d, time:%ld, sub:%d\n",
			p_status->action, pos, frame->current_time, time);
	}
	DEBUG_MSG("end\n");
	Queue_Unlock( &action_stream );
}

/** 更新流中的动画当前帧的停留时间 */
static ActionStatus* ActionStream_UpdateTime( int sleep_time )
{
	int i, total;
	LCUI_BOOL found = FALSE;
	ActionStatus *action_status, *temp = NULL;
	ActionFrameData *frame = NULL;
	
	ActionStream_TimeSub( sleep_time );
	DEBUG_MSG("start\n");
	total = Queue_GetTotal(&action_stream);
	for(i=0; i<total; ++i){
		action_status = (ActionStatus*)
				Queue_Get( &action_stream, i );
		if(action_status->state == PLAY) {
			break;
		}
	}
	if(i >= total || !action_status ) {
		return NULL;
	}
	/*
	 * 由于有些动画还未更新第一帧图像，动画槽里的图像也未载入第一帧的图像，因此，
	 * 需要优先处理帧序号为0的动画。
	 * */
	for(i=0; i<total; ++i){
		temp = (ActionStatus*)Queue_Get( &action_stream, i );
		if( action_status->state == PLAY && temp->current == 0 ) {
			action_status = temp;
			break;
		}
	}
	DEBUG_MSG("action_status->current: %d\n", action_status->current);
	if( action_status->current > 0 ) {
		frame = (ActionFrameData*)
			Queue_Get(	&action_status->action->frame,
					action_status->current-1 );
		if( !frame ) {
			return NULL;
		}
		DEBUG_MSG("current time: %ld\n", frame->current_time);
		/* 若当前帧的停留时间小于或等于0 */
		if(frame->current_time <= 0) {
			frame->current_time = frame->sleep_time;
			++action_status->current;
			found = TRUE;
		}

		total = Queue_GetTotal( &action_status->action->frame );
		if( action_status->current > total ) {
			action_status->current = 1;
		}
	} else {
		action_status->current = 1;
		found = TRUE;
	}
	/* 重新对流中的动作动画进行排序 */
	ActionStream_Sort();
	if( found ) {
		return action_status;
	}
	return NULL;
}

static void ActionStream_Proc( void )
{
	int lost_time;
	static clock_t current_time = 0;
	ActionStatus *action_status;

	while(!LCUI_Active()) {
		LCUI_MSleep(10);
	}
	lost_time = clock() - current_time;
	current_time = clock();
	action_status = ActionStream_UpdateTime( lost_time );
	if( action_status ) {
		Action_CallFunc( action_status );
	}
	PhysicsSystem_Step();
}

/**
/* 播放指定实例对象中的动作
 * @param action
 *	要播放的动画
 * @param obj_id
 *	对象实例对应的标识号，若不大于0，则会为该动作创建一个新的对象实例
 * @returns
 *	正常则返回该动画的播放标识号，失败则返回-1
 */
LCUI_API int Action_Play( ActionData *action, int obj_id )
{
	static int new_obj_id = 1;
	ActionStatus new_status, *p_status;

	if( !action ) {
		return -1;
	}
	if( frame_proc_timer == -1 ) {
		Queue_Init(
			&action_stream,
			sizeof(ActionStatus),
			ActionStatus_Destroy
		);
		frame_proc_timer = LCUITimer_Set( 
			REFRESH_INTERVAL_TIME,
			ActionStream_Proc,
			TRUE
		);
	}
	if( obj_id <= 0 ) {
		ActionStatus_Init( &new_status );
		new_status.obj_id = new_obj_id++;
		new_status.action = action;
		new_status.state = PLAY;
		Queue_Add( &action_stream, &new_status );
		return new_status.obj_id;
	}
	p_status = Action_GetStatus( action, obj_id );
	if( p_status == NULL ) {
		return -1;
	}
	p_status->state = PLAY;
	return obj_id;
}

/**
 * 暂停指定实例兑现中的动作
 * @param action
 *	要暂停的动画
 * @param obj_id
 *	与该动画的播放实例对应的标识号
 * @returns
 *	正常则返回该动画的播放标识号，失败则返回-1
 */
LCUI_API int Action_Pause( ActionData *action, int obj_id )
{
	static int new_play_id = 1;
	ActionStatus *p_status;

	if( !action ) {
		return -1;
	}
	if( frame_proc_timer == -1 ) {
		return -1;
	}
	if( obj_id <= 0 ) {
		return -1;
	}
	p_status = Action_GetStatus( action, obj_id );
	if( p_status == NULL ) {
		return -1;
	}
	p_status->state = PAUSE;
	return 0;
}

/** 重置动作动画 */
LCUI_API int Action_Reset( ActionData *action, int obj_id )
{
	static int new_play_id = 1;
	ActionStatus *p_status;

	if( !action ) {
		return -1;
	}
	if( frame_proc_timer == -1 ) {
		return -1;
	}
	if( obj_id <= 0 ) {
		return -1;
	}
	p_status = Action_GetStatus( action, obj_id );
	if( p_status == NULL ) {
		return -1;
	}
	p_status->current = 1;
	return 0;
}

static void GameObject_RefreshFrame( void *arg )
{
	LCUI_Widget *widget = (LCUI_Widget*)arg;
	DEBUG_MSG("refresh\n");
	Widget_Draw( widget );
}


static int __GameObject_PlayAction(	LCUI_Widget *widget,
					ActionRec *action_rec )
{
	int ret;
	ret = Action_Play(	action_rec->action,
				action_rec->obj_id );
	if( ret <= 0 ) {
		return -1;
	}
	/* 保存播放标识号 */
	action_rec->obj_id = ret;
	/* 关联回调函数 */
	Action_Connect(
		action_rec->action,
		action_rec->obj_id,
		GameObject_RefreshFrame,
		(void*)widget
	);
	return 0;
}

/**
 * 切换对象的动作
 * @param widget
 *	目标ActiveBox部件
 * @param action_id
 *	切换至的动画的标识号
 * @return
 *	切换成功则返回0，未找到指定ID的动画记录，则返回-1
 */
LCUI_API int GameObject_SwitchAction(	LCUI_Widget *widget,
					int action_id )
{
	int i, n;
	ActionRec *p_rec;
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData( widget );
	if( obj->current && obj->current->id == action_id ) {
		return 0;
	}
	n = Queue_GetTotal( &obj->action_list );
	for(i=0; i<n; ++i) {
		p_rec = (ActionRec*)
			Queue_Get( &obj->action_list, i );

		if( !p_rec || p_rec->id != action_id ) {
			continue;
		}
		if( obj->current ) {
			Action_Pause(	obj->current->action,
					obj->current->obj_id );
			Action_Reset(	obj->current->action,
					obj->current->obj_id );
		}
		if( obj->state == PLAY ) {
			__GameObject_PlayAction( widget, p_rec );
		} else {
			Action_Pause( p_rec->action, p_rec->obj_id );
			Action_Reset( p_rec->action, p_rec->obj_id );
		}
		obj->current = p_rec;
		/* 标记数据为无效，以根据当前动作动画来更新数据 */
		obj->data_valid = FALSE;
		Widget_Draw(widget);
		return 0;
	}
	return -1;
}

LCUI_API int GameObject_ResetAction( LCUI_Widget *widget )
{
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData(widget);
	if( obj->current == NULL ) {
		obj->current = (ActionRec*)Queue_Get(
					&obj->action_list, 0 );
		if( obj->current == NULL ) {
			return -1;
		}
	}
	return Action_Reset( obj->current->action, obj->current->obj_id );
}

/** 播放对象的动作 */
LCUI_API int GameObject_PlayAction( LCUI_Widget *widget )
{
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData(widget);
	if( obj->current == NULL ) {
		obj->current = (ActionRec*)Queue_Get(
					&obj->action_list, 0 );
		if( obj->current == NULL ) {
			return -1;
		}
	}
	obj->state = PLAY;
	return __GameObject_PlayAction( widget, obj->current );
}

/** 暂停对象的动作 */
LCUI_API int GameObject_PauseAction( LCUI_Widget *widget )
{
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData(widget);
	obj->state = PAUSE;
	if( obj->current == NULL ) {
		obj->current = (ActionRec*)Queue_Get(
					&obj->action_list, 0 );
		if( obj->current == NULL ) {
			return -1;
		}
	}
	return Action_Pause(	obj->current->action,
				obj->current->obj_id );
}

LCUI_API int Action_AddFrame(	ActionData* action,
				int offset_x,
				int offset_y,
				LCUI_Graph *graph,
				long int sleep_time )
{
	ActionFrameData frame;
	
	frame.sleep_time = sleep_time*REFRESH_INTERVAL_TIME;
	frame.current_time = frame.sleep_time;
	frame.offset.x = offset_x;
	frame.offset.y = offset_y;
	frame.graph = *graph;

	if( 0 <= Queue_Add( &action->frame, &frame ) ) {
		return 0;
	}
	return -1;
}

static void GameObject_ExecInit( LCUI_Widget *widget )
{
	GameObject *obj;

	obj = (GameObject*)Widget_NewPrivData( widget, sizeof(GameObject) );
	Queue_Init( &obj->action_list, sizeof(ActionRec), NULL );
	obj->current = NULL;
	obj->global_bottom_line_y = 0;
	obj->global_center_x = 0;
	obj->data_valid = FALSE;
	obj->state = 0;
	obj->x = 0;
	obj->y = 0;
	obj->w = 0;
	obj->h = 0;
	obj->phys_obj = PhysicsObject_New(0,0,0,0,0,0 );
}

/** 获取当前动作信息 */
static int GameObject_UpdateData( LCUI_Widget *widget )
{
	int i, n;
	GameObject *obj;
	LCUI_Queue *frame_list;
	ActionFrameData *frame;
	int frame_bottom_y, frame_center_x;
	int box_w=0, box_h=0, point_x=0, point_y=0;

	obj = (GameObject*)Widget_GetPrivData( widget );
	if( obj->current == NULL ) {
		return -1;
	}
	frame_list = &obj->current->action->frame;
	n = Queue_GetTotal( &obj->current->action->frame );
	for( i=0; i<n; ++i ) {
		frame = (ActionFrameData*)Queue_Get( frame_list, i );
		if( frame == NULL ) {
			continue;
		}
		/*
		 * frame->graph是当前帧动作的容器
		 * frame->graph.w 和 frame->graph.h 则是该容器的尺寸
		 * box_w 和 box_h 是该动作集的容器尺寸
		 * frame_bottom_y 是对象底线在当前帧的容器中的Y轴坐标
		 * frame_center_x 是对象中点在当前帧的容器中的X轴坐标
		 * point_y 和 point_x 与上面两个类似，但坐标是相对动作集的容器
		 */
		frame_bottom_y = frame->graph.h + frame->offset.y;
		frame_center_x = frame->graph.w/2 + frame->offset.x;
		/* 对比容器顶端到底线的距离 */
		if( frame_bottom_y > point_y ) {
			box_h = (box_h - point_y) + frame_bottom_y;
			point_y = frame_bottom_y;
		}
		/* 对比底线到容器底端的距离 */
		if( frame->graph.h - frame_bottom_y > box_h - point_y  ) {
			box_h = point_y + (frame->graph.h - frame_bottom_y);
		}
		/* 对比容器左端到中心点的距离 */
		if( frame_center_x > point_x ) {
			box_w = (box_w - point_x) + frame_center_x;
			point_x = frame_center_x;
		}
		/* 对比中心点到容器右端的距离 */
		if( frame->graph.w - frame_center_x > box_w - point_x ) {
			box_w = point_x + (frame->graph.w - frame_center_x);
		}
	}
	obj->global_center_x = point_x;
	obj->global_bottom_line_y = point_y;
	obj->w = box_w;
	obj->h = box_h;
	return 0;
}

/** 为对象添加一个动作 */
LCUI_API int GameObject_AddAction(	LCUI_Widget *widget,
					ActionData *action,
					int id )
{
	int ret;
	ActionRec rec;
	GameObject *obj;

	rec.action = action;
	rec.obj_id = 0;
	rec.id = id;
	obj = (GameObject*)Widget_GetPrivData( widget );
	/* 添加至动作列表中 */
	ret = Queue_Add( &obj->action_list, &rec );
	if( ret < 0 ) {
		return -1;
	}
	return 0;
}

static void GameObject_ExecUpdate( LCUI_Widget *widget )
{
	GameObject *obj;
	LCUI_Pos pos;

	obj = (GameObject*)Widget_GetPrivData( widget );
	if( obj->current == NULL ) {
		obj->current = (ActionRec*)Queue_Get( &obj->action_list, 0 );
		/* 既然没有动作动画，那就不进行更新 */
		if( obj->current == NULL ) {
			return;
		}
	}
	obj->x = obj->phys_obj->x;
	obj->y = obj->phys_obj->y+obj->phys_obj->z;
	/* 计算部件的坐标 */
	pos.x = obj->x - obj->global_center_x;
	pos.y = obj->y - obj->global_bottom_line_y;
	/* 移动部件的位置 */
	Widget_Move( widget, pos );
	/* 如果数据还有效 */
	if( obj->data_valid ) {
		return;
	}
	/* 如果数据更新成功 */
	if( GameObject_UpdateData( widget ) == 0 ) {
		pos.x = obj->x - obj->global_center_x;
		pos.y = obj->y - obj->global_bottom_line_y;
		Widget_Move( widget, pos );
		/* 并调整部件的尺寸，以正常显示对象的动画 */
		Widget_Resize( widget, Size(obj->w, obj->h) );
		obj->data_valid = TRUE;
	}
}

static void GameObject_ExecDraw( LCUI_Widget *widget )
{
	GameObject *obj;
	LCUI_Pos pos;
	ActionStatus *p_status;
	ActionFrameData *frame;
	LCUI_Graph *graph;
	
	obj = (GameObject*)Widget_GetPrivData( widget );
	if( obj->current == NULL ) {
		obj->current = (ActionRec*)Queue_Get( &obj->action_list, 0 );
		/* 既然没有动作动画，那就不进行更新 */
		if( obj->current == NULL ) {
			return;
		}
	}
	p_status = Action_GetStatus(
			obj->current->action,
			obj->current->obj_id
	);
	/* 获取当前帧动作图像 */
	frame = (ActionFrameData*)Queue_Get(
			&p_status->action->frame,
			p_status->current>0?p_status->current-1:0
	);
	if( frame == NULL ) {
		return;
	}
	/* 计算当前帧相对于部件的坐标 */
	pos.y = frame->graph.h + frame->offset.y;
	pos.y = obj->global_bottom_line_y - pos.y;
	pos.x = frame->graph.w/2 + frame->offset.x;
	pos.x = obj->global_center_x - pos.x;
	/* 获取部件自身图层的图像 */
	graph = Widget_GetSelfGraph( widget );
	/* 然后贴上去 */
	Graph_Replace( graph, &frame->graph, pos );
}

/** 设置游戏对象在X轴的移动速度 */
LCUI_API void GameObject_SetXSpeed( LCUI_Widget *widget, int x_speed )
{
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData( widget );
	obj->phys_obj->x_speed = x_speed;
	Widget_Update( widget );
}

/** 设置游戏对象在Y轴的移动速度 */
LCUI_API void GameObject_SetYSpeed( LCUI_Widget *widget, int y_speed )
{
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData( widget );
	obj->phys_obj->y_speed = y_speed;
	Widget_Update( widget );
}

/** 设置游戏对象在Z轴的移动速度 */
LCUI_API void GameObject_SetZSpeed( LCUI_Widget *widget, int z_speed )
{
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData( widget );
	obj->phys_obj->z_speed = z_speed;
	Widget_Update( widget );
}

/** 移动游戏对象的位置 */
LCUI_API void GameObject_SetPos( LCUI_Widget *widget, int x, int y )
{
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData( widget );
	obj->phys_obj->x = x;
	obj->phys_obj->y = x;
	Widget_Update( widget );
}

/** 获取游戏对象的位置 */
LCUI_API void GameObject_GetPos( LCUI_Widget *widget, int *x, int *y )
{
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData( widget );
	*x = obj->x;
	*y = obj->y;
}

LCUI_API LCUI_Widget* GameObject_New(void)
{
	return Widget_New("GameObject");
}


LCUI_API void GameObject_Register(void)
{
	WidgetType_Add("GameObject");
	WidgetFunc_Add("GameObject", GameObject_ExecInit, FUNC_TYPE_INIT );
	WidgetFunc_Add("GameObject", GameObject_ExecDraw, FUNC_TYPE_DRAW );
	WidgetFunc_Add("GameObject", GameObject_ExecUpdate, FUNC_TYPE_UPDATE );
}
