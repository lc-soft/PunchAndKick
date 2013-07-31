#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

/** 范围框 */
typedef struct RangeBox_ {
	int x, y, z;
	int x_width, y_width, z_width;
} RangeBox;

typedef struct AttackerInfo_ {
	LCUI_Widget *attacker;	/** 攻击者 */
	int attacker_action;	/** 攻击者在攻击时的动作ID */
} AttackerInfo;

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
	RangeBox hitbox;	/**< 受攻击框 */
	RangeBox atkbox;	/**< 攻击框 */
	LCUI_BOOL new_attack;	/**< 是否为新攻击 */
} ActionFrameData;

/** 动作集的信息 */
typedef struct ActionData_ {
	LCUI_BOOL replay;	/**< 标志，指示该动作是否重复播放 */
	LCUI_Queue frame;	/**< 用于记录该动作集的所有帧动作 */
} ActionData;


/**
 * 创建一个动作集
 * 创建的动作集将记录至动作库中
 * @returns
 *	正常则返回指向动作库中的该动作集的指针，失败则返回NULL
 */
LCUI_API ActionData* Action_Create( void );

/** 设置动作是否重复播放 */
LCUI_API void Action_SetReplay( ActionData *action, LCUI_BOOL replay );

/**
 * 删除一个动画
 * 从动画库中删除指定的动画
 * @param action
 *	需删除的动画
 * @returns
 *	正常则返回0，失败则返回-1
 */
LCUI_API int Action_Delete( ActionData* action );

/**
 * 为动作添加一帧动作图
 * @param action
 *	目标动作
 * @param offset_x
 *	人物底线中点相对于动作图低边中点的X轴的偏移量
 * @param offset_y
 *	人物底线中点相对于动作图低边中点的Y轴的偏移量
 * @param graph
 *	动作图
 * @param sleep_time
 *	动作图的停留时间，单位时间为20毫秒，即当它的值为50时，该帧动作图停留1秒
 * @return
 *	正常则返回该帧动作图的序号，失败返回-1
 */
LCUI_API int Action_AddFrame(	ActionData* action,
				int offset_x,
				int offset_y,
				LCUI_Graph *graph,
				int sleep_time );

LCUI_API int Action_SetNewAttack( ActionData* action,
					int n_frame,
					LCUI_BOOL flag );

LCUI_API int Action_SetAttackRange(	ActionData* action,
					int n_frame,
					RangeBox attack_range );

LCUI_API int Action_SetHitRange(	ActionData* action,
					int n_frame,
					RangeBox hit_range );

LCUI_API int GameObject_AtActionDone(	LCUI_Widget *widget,
					int action_id,
					void (*func)(LCUI_Widget*) );

LCUI_API int GameObject_AtActionUpdate(	LCUI_Widget *widget,
					int action_id,
					void (*func)(LCUI_Widget*) );

/** 获取当前动作帧的序号 */
LCUI_API int GameObject_GetCurrentActionFrameNumber( LCUI_Widget *widget );

/** 获取当前帧的顶点相对于底线的距离 */
LCUI_API int GameObject_GetCurrentFrameTop( LCUI_Widget *widget );

LCUI_API void GameObject_AtXSpeedToZero(	LCUI_Widget *widget,
						double acc,
						void (*func)(LCUI_Widget*) );

/** 在Z轴移动速度为0时进行响应  */
LCUI_API void GameObject_AtZeroZSpeed(	LCUI_Widget *widget,
					void (*func)(LCUI_Widget*) );

/** 设置在对象着地时进行响应 */
LCUI_API void GameObject_AtLanding(	LCUI_Widget *widget,
					double z_speed,
					double z_acc,
					void (*func)(LCUI_Widget*) );

/** 设定当速度达到一定值时进行响应（在有加速度的情况下） */
LCUI_API void GameObject_AtZSpeed(	LCUI_Widget *widget,
					double target_z_speed,
					void (*func)(LCUI_Widget*) );

/** 设置在被攻击时进行响应 */
LCUI_API void GameObject_AtUnderAttack(	LCUI_Widget *widget,
					void (*func)(LCUI_Widget*) );

/** 在游戏对象移动时进行响应 */
LCUI_API void GameObject_AtMove(	LCUI_Widget *widget,
					void(*func)(LCUI_Widget*) );

/** 设置在被其它对象触碰到时进行响应 */
LCUI_API void GameObject_AtTouch(	LCUI_Widget *widget,
					void (*func)(LCUI_Widget*,LCUI_Widget*) );

/** 获取攻击者信息 */
LCUI_API LCUI_Queue* GameObject_GetAttackerInfo( LCUI_Widget* widget );

/** 获取对象的受攻击范围 */
LCUI_API int GameObject_GetHitRange( LCUI_Widget *widget, RangeBox *range );

/** 获取对象的攻击范围 */
LCUI_API int GameObject_GetAttackRange( LCUI_Widget *widget, RangeBox *range );

/** 判断两个范围是否相交 */
LCUI_API LCUI_BOOL RangeBox_IsIntersect( RangeBox *range1, RangeBox *range2 );

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

/** 设置在Y轴的坐标 */
LCUI_API void GameObject_SetY( LCUI_Widget *widget, double y );

/** 获取在Y轴的坐标 */
LCUI_API double GameObject_GetY( LCUI_Widget *widget );

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

/** 获取游戏对象在Z轴的坐标 */
LCUI_API double GameObject_GetZ( LCUI_Widget *widget );

/** 设置游戏对象在Z轴的移动速度 */
LCUI_API void GameObject_SetZSpeed( LCUI_Widget *widget, double z_speed );

/** 获取游戏对象在Z轴的移动速度 */
LCUI_API double GameObject_GetZSpeed( LCUI_Widget *widget );

/** 设置游戏对象在Z轴的坐标 */
LCUI_API void GameObject_SetZ( LCUI_Widget *widget, double z );

/** 移动游戏对象的位置 */
LCUI_API void GameObject_SetPos( LCUI_Widget *widget, double x, double y );

/** 获取游戏对象的位置 */
LCUI_API void GameObject_GetPos( LCUI_Widget *widget, double *x, double *y );

/** 设置游戏对象的X轴坐标 */
LCUI_API void GameObject_SetX( LCUI_Widget *widget, double x );

/** 获取游戏对象的X轴坐标 */
LCUI_API double GameObject_GetX( LCUI_Widget *widget );

/** 设置游戏对象的阴影图像 */
LCUI_API void GameObject_SetShadow( LCUI_Widget *widget, LCUI_Graph img_shadow );

/** 清空攻击记录 */
LCUI_API void GameObject_ClearAttack( LCUI_Widget *widget );

/** 获取指定范围内的游戏对象 */
LCUI_API LCUI_Widget* GameObject_GetObjectInRange(
					LCUI_Widget *widget,
					RangeBox range,
					LCUI_BOOL specific_action,
					int action_id );

/** 获取攻击范围内的游戏对象 */
LCUI_API LCUI_Widget* GameObject_GetObjectInAttackRange(
					LCUI_Widget *widget,
					int attack_action_id,
					LCUI_BOOL specific_action,
					int action_id );

LCUI_API LCUI_Widget* GameObject_New(void);

LCUI_API void GameObject_Register(void);

#endif