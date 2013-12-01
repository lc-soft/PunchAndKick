/* ****************************************************************************
 * Punch And Kick -- a simple 2D Fighting Game.
 *
 * Copyright (C) 2013 by
 * Liu Chao
 *
 * This file is part of the Punch And Kick project, and may only be used,
 * modified, and distributed under the terms of the GPLv2.
 *
 * (GPLv2 is abbreviation of GNU General Public License Version 2)
 *
 * By continuing to use, modify, or distribute this file you indicate that you
 * have read the license and understand and accept it fully.
 *
 * The Punch And Kick project is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GPL v2 for
 * more details.
 *
 * You should have received a copy of the GPLv2 along with this file. It is
 * usually in the LICENSE.TXT file, If not, see <http://www.gnu.org/licenses/>.
 * ***************************************************************************/

/* ****************************************************************************
 * Punch And Kick -- 一个普通的2D格斗游戏
 *
 * 版权所有 (C) 2013 归属于
 * 刘超
 *
 * 这个文件是 Punch And Kick 项目的一部分，并且只可以根据GPLv2许可协议来使用、
 * 更改和发布。
 *
 * (GPLv2 是 GNU通用公共许可证第二版 的英文缩写)
 *
 * 继续使用、修改或发布本文件，表明您已经阅读并完全理解和接受这个许可协议。
 *
 * Punch And Kick 项目是基于技术交流目的而加以散布的，但不负任何担保责任，例
 * 如：稳定性、可玩性、趣味性等的担保，甚至没有适销性或特定用途的隐含担保，详
 * 情请参照GPLv2许可协议。
 *
 * 您应已收到附随于本文件的GPLv2许可协议的副本，它通常在LICENSE.TXT文件中，如果
 * 没有，请查看：<http://www.gnu.org/licenses/>.
 * ***************************************************************************/

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

/** 销毁一个游戏空间 */
void GameSapce_Destroy( GameSpaceData *space );

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
