#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H

#include "game_object.h"
#include "physics_system.h"

#include <time.h>

#define PAUSE	0
#define PLAY	1

enum FuncUse {
	FUNC_USE_AT_END,
	FUNC_USE_AT_UPDATE
};

/** 动作记录 */
typedef struct ActionRec_ {
	int id;			/**< 动画的标识号 */
	ActionData *action;	/**< 对应的动作集 */
} ActionRec;

/** 每次数据刷新时的时间间隔(毫秒) */
#define REFRESH_INTERVAL_TIME 20

typedef struct GameObject_ {
	int state;			/**< 当前状态 */
	int x, y;			/**< 底线中点坐标 */
	int w, h;			/**< 动作容器的尺寸 */
	int global_bottom_line_y;	/**< 底线的Y轴坐标 */
	int global_center_x;		/**< 中心点的X轴坐标 */
	LCUI_BOOL data_valid;		/**< 当前使用的数据是否有效 */
	ActionRec *current;		/**< 当前动作动画记录 */
	LCUI_Queue action_list;		/**< 动作列表 */
	int n_frame;			/**< 记录当前帧动作的序号，帧序号从0开始 */
	long int remain_time;		/**< 当前帧剩下的停留时间 */
	LCUI_Func func[2];		/**< 被关联的回调函数 */
	PhysicsObject *phys_obj;	/**< 对应的物理对象 */
} GameObject;

static LCUI_Queue action_database;
static LCUI_Queue gameobject_stream;

static int database_init = FALSE;
static int frame_proc_timer = -1;

static void GameObject_CallFunc( LCUI_Widget *widget, int func_use )
{
	GameObject *obj;
	obj = (GameObject*)Widget_GetPrivData( widget );
	AppTasks_CustomAdd( ADD_MODE_REPLACE | AND_ARG_S, &obj->func[func_use] );
}

static void ActionData_Destroy( void *arg )
{
	ActionData *action;
	action = (ActionData*)arg;
	Queue_Destroy( &action->frame );
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

static int GameObject_Connect(	LCUI_Widget *widget,
				int func_use,
				void (*func)(LCUI_Widget*) )
{
	GameObject *obj;
	obj = (GameObject*)Widget_GetPrivData( widget );
	obj->func[func_use].func = (CallBackFunc)func;
	obj->func[func_use].id = LCUIApp_GetSelfID();
	obj->func[func_use].arg[0] = widget;
	obj->func[func_use].arg[1] = NULL;
	obj->func[func_use].destroy_arg[0] = FALSE;
	obj->func[func_use].destroy_arg[1] = FALSE;
	return 0;
}


LCUI_API void GameObject_AtActionDone(	LCUI_Widget *widget,
					void (*func)(LCUI_Widget*) )
{
	GameObject_Connect( widget, FUNC_USE_AT_END, func );
}

static void GameObjectStream_Sort(void)
{
	int i, j, total;
	LCUI_Widget *widget;
	GameObject *tmp, *p1, *p2;

	Queue_Lock( &gameobject_stream );
	total = Queue_GetTotal( &gameobject_stream );
	for(i=0; i<total; ++i) {
		widget = (LCUI_Widget*)Queue_Get( &gameobject_stream, i );
		p1 = (GameObject*)Widget_GetPrivData( widget );
		if( !p1 ) {
			continue;
		}
		for(j=i+1; j<total; ++j) {
			widget = (LCUI_Widget*)Queue_Get( &gameobject_stream, j );
			p2 = (GameObject*)Widget_GetPrivData( widget );
			if( !p2 ) {
				continue;
			}
			if( p1->remain_time < p2->remain_time ) {
				tmp = p1;
				p1 = p2;
				p2 = tmp;
				Queue_Swap( &gameobject_stream, j, i );
			}
		}
	}
	Queue_Unlock( &gameobject_stream );
}

static void GameObjectStream_TimeSub( int time )
{
	int i, total;
	LCUI_Widget *widget;
	GameObject *obj;

	Queue_Lock( &gameobject_stream );
	total = Queue_GetTotal(&gameobject_stream);
	DEBUG_MSG("start\n");
	for(i=0; i<total; ++i) {
		widget = (LCUI_Widget*)Queue_Get( &gameobject_stream, i );
		obj = (GameObject*)Widget_GetPrivData( widget );
		if( !obj || obj->state == PAUSE ) {
			continue;
		}
		obj->remain_time -= time;
	}
	Queue_Unlock( &gameobject_stream );
}

static long int Action_GetFrameSleepTime( ActionData *action, int n_frame )
{
	ActionFrameData *frame;
	frame = (ActionFrameData*)Queue_Get( &action->frame, n_frame );
	if( frame ) {
		return frame->sleep_time;
	}
	return REFRESH_INTERVAL_TIME;
}

/** 更新流中的动画当前帧的停留时间 */
static void GameObjectStream_UpdateTime( int sleep_time )
{
	int i, total;
	LCUI_BOOL need_draw=FALSE;
	LCUI_Widget *widget;
	GameObject *obj;
	ActionFrameData *frame = NULL;
	
	GameObjectStream_TimeSub( sleep_time );
	total = Queue_GetTotal(&gameobject_stream);
	for(i=0; i<total; ++i){
		widget = (LCUI_Widget*)Queue_Get( &gameobject_stream, i );
		obj = (GameObject*)Widget_GetPrivData( widget );
		if( obj && obj->state == PLAY) {
			break;
		}
	}
	if(i >= total || !widget || !obj ) {
		return;
	}
	if( obj->current == NULL ) {
		GameObjectStream_Sort();
		return;
	}
	/* 若当前帧的停留时间小于或等于0 */
	if(obj->remain_time <= 0) {
		++obj->n_frame;
		obj->remain_time = Action_GetFrameSleepTime(
					obj->current->action,
					obj->n_frame
		);
		GameObject_CallFunc( widget, FUNC_USE_AT_UPDATE );
		need_draw = TRUE;
	}
	total = Queue_GetTotal( &obj->current->action->frame );
	if( obj->n_frame >= total ) {
		obj->n_frame = 0;
		obj->remain_time = Action_GetFrameSleepTime(
					obj->current->action,
					obj->n_frame
		);
		GameObject_CallFunc( widget, FUNC_USE_AT_END );
	}
	if( need_draw ) {
		Widget_Draw( widget );
	}
	GameObjectStream_Sort();
}

static void GameObjectStream_Proc( void* arg )
{
	int lost_time;
	static clock_t current_time = 0;

	while(!LCUI_Active()) {
		LCUI_MSleep(10);
	}
	lost_time = clock() - current_time;
	current_time = clock();
	GameObjectStream_UpdateTime( lost_time );
	PhysicsSystem_Step();
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
	n = Queue_GetTotal( &obj->action_list );
	for(i=0; i<n; ++i) {
		p_rec = (ActionRec*)Queue_Get( &obj->action_list, i );
		if( !p_rec || p_rec->id != action_id ) {
			continue;
		}
		obj->current = p_rec;
		obj->n_frame = 0;
		obj->remain_time = Action_GetFrameSleepTime( p_rec->action, 0 );
		/* 标记数据为无效，以根据当前动作动画来更新数据 */
		obj->data_valid = FALSE;
		Widget_Draw(widget);
		return 0;
	}
	return -1;
}

/** 获取当前动作动画的ID */
LCUI_API int GameObject_GetCurrentActionID( LCUI_Widget *widget )
{
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData( widget );
	if( obj->current == NULL ) {
		obj->current = (ActionRec*)Queue_Get( &obj->action_list, 0 );
		if( obj->current == NULL ) {
			return -1;
		}
	}
	return obj->current->id;
}

LCUI_API int GameObject_ResetAction( LCUI_Widget *widget )
{
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData(widget);
	if( obj->current == NULL ) {
		obj->current = (ActionRec*)Queue_Get( &obj->action_list, 0 );
		if( obj->current == NULL ) {
			return -1;
		}
	}
	obj->n_frame = 0;
	obj->remain_time = Action_GetFrameSleepTime( obj->current->action, 0 );
	Widget_Draw( widget );
	return 0;
}

/** 播放对象的动作 */
LCUI_API int GameObject_PlayAction( LCUI_Widget *widget )
{
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData(widget);
	if( obj->current == NULL ) {
		obj->current = (ActionRec*)Queue_Get( &obj->action_list, 0 );
		if( obj->current == NULL ) {
			return -1;
		}
	}
	obj->state = PLAY;
	obj->n_frame = 0;
	obj->remain_time = Action_GetFrameSleepTime( obj->current->action, 0 );
	Widget_Draw( widget );
	return 0;
}

/** 暂停对象的动作 */
LCUI_API int GameObject_PauseAction( LCUI_Widget *widget )
{
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData(widget);
	if( obj->current == NULL ) {
		return -1;
	}
	obj->state = PAUSE;
	return 0;
}

LCUI_API int Action_AddFrame(	ActionData* action,
				int offset_x,
				int offset_y,
				LCUI_Graph *graph,
				long int sleep_time )
{
	ActionFrameData frame;
	
	frame.sleep_time = sleep_time*REFRESH_INTERVAL_TIME;
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
	obj->global_bottom_line_y = 0;
	obj->global_center_x = 0;
	obj->data_valid = FALSE;
	obj->state = 0;
	obj->x = 0;
	obj->y = 0;
	obj->w = 0;
	obj->h = 0;
	obj->phys_obj = PhysicsObject_New(0,0,0,0,0,0 );
	obj->current = NULL;
	obj->n_frame = 0;
	obj->remain_time = 0;
	obj->state = PAUSE;
	Queue_Init( &obj->action_list, sizeof(ActionRec), NULL );
	if( frame_proc_timer == -1 ) {
		Queue_Init( &gameobject_stream, sizeof(LCUI_Widget*), NULL );
		Queue_UsingPointer( &gameobject_stream );
		frame_proc_timer = LCUITimer_Set( 
			REFRESH_INTERVAL_TIME,
			GameObjectStream_Proc,
			NULL, TRUE
		);
	}
	Queue_AddPointer( &gameobject_stream, widget );
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
	if( obj->current->action == NULL ) {
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
	ActionFrameData *frame;
	LCUI_Graph *graph;
	
	obj = (GameObject*)Widget_GetPrivData( widget );
	if( obj->current == NULL ) {
		obj->current = (ActionRec*)Queue_Get( &obj->action_list, 0 );
		if( obj->current == NULL ) {
			return;
		}
	}
	/* 获取当前帧动作图像 */
	frame = (ActionFrameData*)Queue_Get(
			&obj->current->action->frame,
			obj->n_frame
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

/** 设置加速度 */
LCUI_API void GameObject_SetXAcc( LCUI_Widget *widget, int acc )
{
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData( widget );
	obj->phys_obj->x_acc = acc;
}

/** 获取加速度 */
LCUI_API int GameObject_GetXAcc( LCUI_Widget *widget )
{
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData( widget );
	return obj->phys_obj->x_acc;
}

/** 设置游戏对象在X轴的移动速度 */
LCUI_API void GameObject_SetXSpeed( LCUI_Widget *widget, int x_speed )
{
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData( widget );
	obj->phys_obj->x_speed = x_speed;
	Widget_Update( widget );
}

/** 获取游戏对象在X轴的移动速度 */
LCUI_API int GameObject_GetXSpeed( LCUI_Widget *widget )
{
	GameObject *obj;

	obj = (GameObject*)Widget_GetPrivData( widget );
	return obj->phys_obj->x_speed;
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
