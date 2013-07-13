#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

/*
  当前帧的中心点的Y轴平行线
         |
  _______|______
  |   O        |         矩形框代表当前帧的范围
  |  /|\       |
  |__/_\_______|________ 底线的X轴平行线
      |
      |
  对象的中点的Y轴平行线
*/

/** 一帧动作的信息 */
typedef struct ActionFrameData_ {
	LCUI_Pos offset;	/**< 当前对象底线中点相对于该帧图像底线中点的偏移量 */
	LCUI_Graph graph;	/**< 当前帧的图像 */
	long int sleep_time;	/**< 该帧显示的时长（单位：毫秒） */
} ActionFrameData;

/** 动作集的信息 */
typedef struct ActionData_ {
	LCUI_Queue frame;	/**< 用于记录该动作集的所有帧动作 */
} ActionData;


/**
 * 创建一个动作集
 * 创建的动作集将记录至动作库中
 * @returns
 *	正常则返回指向动作库中的该动作集的指针，失败则返回NULL
 */
LCUI_API ActionData* Action_Create( void );

/**
 * 删除一个动画
 * 从动画库中删除指定的动画
 * @param action
 *	需删除的动画
 * @returns
 *	正常则返回0，失败则返回-1
 */
LCUI_API int Action_Delete( ActionData* action );

LCUI_API void GameObject_AtActionDone(	LCUI_Widget *widget,
					void (*func)(LCUI_Widget*) );

LCUI_API void GameObject_AtXSpeedToZero(	LCUI_Widget *widget,
						double acc,
						void (*func)(LCUI_Widget*) );

/** 设置在对象着地时进行响应 */
LCUI_API void GameObject_AtLanding(	LCUI_Widget *widget,
					double z_speed,
					double z_acc,
					void (*func)(LCUI_Widget*) );

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
					int action_id );

/** 获取当前动作动画的ID */
LCUI_API int GameObject_GetCurrentActionID( LCUI_Widget *widget );

LCUI_API int GameObject_ResetAction( LCUI_Widget *widget );

/** 播放对象的动作 */
LCUI_API int GameObject_PlayAction( LCUI_Widget *widget );

/** 暂停对象的动作 */
LCUI_API int GameObject_PauseAction( LCUI_Widget *widget );

LCUI_API int Action_AddFrame(	ActionData* action,
				int offset_x,
				int offset_y,
				LCUI_Graph *graph,
				long int sleep_time );

/** 为对象添加一个动作 */
LCUI_API int GameObject_AddAction(	LCUI_Widget *widget,
					ActionData *action,
					int id );

/** 设置对象是否进行水平翻转 */
LCUI_API void GameObject_SetHorizFlip( LCUI_Widget *widget, LCUI_BOOL flag );

/** 设置相对于X轴的加速度 */
LCUI_API void GameObject_SetXAcc( LCUI_Widget *widget, double acc );

/** 获取相对于X轴的加速度 */
LCUI_API double GameObject_GetXAcc( LCUI_Widget *widget );

/** 设置相对于Y轴的加速度 */
LCUI_API void GameObject_SetYAcc( LCUI_Widget *widget, double acc );

/** 获取相对于Y轴的加速度 */
LCUI_API double GameObject_GetYAcc( LCUI_Widget *widget );

/** 设置相对于Y轴的加速度 */
LCUI_API void GameObject_SetZAcc( LCUI_Widget *widget, double acc );

/** 获取相对于Z轴的加速度 */
LCUI_API double GameObject_GetZAcc( LCUI_Widget *widget );

/** 设置游戏对象在X轴的移动速度 */
LCUI_API void GameObject_SetXSpeed( LCUI_Widget *widget, double x_speed );

/** 获取游戏对象在X轴的移动速度 */
LCUI_API double GameObject_GetXSpeed( LCUI_Widget *widget );

/** 设置游戏对象在Y轴的移动速度 */
LCUI_API void GameObject_SetYSpeed( LCUI_Widget *widget, double y_speed );

/** 获取游戏对象在Y轴的移动速度 */
LCUI_API double GameObject_GetYSpeed( LCUI_Widget *widget );

/** 设置游戏对象在Z轴的移动速度 */
LCUI_API void GameObject_SetZSpeed( LCUI_Widget *widget, double z_speed );

/** 获取游戏对象在Z轴的移动速度 */
LCUI_API double GameObject_GetZSpeed( LCUI_Widget *widget );

/** 移动游戏对象的位置 */
LCUI_API void GameObject_SetPos( LCUI_Widget *widget, double x, double y );

/** 获取游戏对象的位置 */
LCUI_API void GameObject_GetPos( LCUI_Widget *widget, double *x, double *y );

/** 设置游戏对象的阴影图像 */
LCUI_API void GameObject_SetShadow( LCUI_Widget *widget, LCUI_Graph img_shadow );

LCUI_API LCUI_Widget* GameObject_New(void);


LCUI_API void GameObject_Register(void);

#endif