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

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_INPUT_H

#include "game_menubtn.h"
#include "game_menu.h"

typedef struct GameChildMenuData_ {
	LCUI_Widget *target_button;
	LCUI_Widget *menu;
} GameChildMenuData;

typedef struct GameMenuData_ {
	int button_height;		/**< 每个菜单按钮的高度 */
	int font_size;			/**< 按钮中的缺省字体大小 */
	LCUI_Queue color_set;		/**< 配色集 */
	LCUI_Queue button_list;		/**< 菜单的按钮列表 */
	LCUI_Queue childmenu_data;	/**< 子菜单记录 */
	LCUI_Widget *parent_menu;	/**< 父级菜单 */
} GameMenuData;

#define AVG_WIDTH	30

static LCUI_BOOL module_is_inited = FALSE;
static LCUI_Queue menu_stack;
static LCUI_BOOL need_update_effect = FALSE;
static LCUI_Pos root_menu_pos={0,0};
static LCUI_Pos root_menu_offset_pos={0,0};
static int menu_effect_timer = -1;
static int connect_id[4];

/** 获取菜单的显示位置 */
static int GameMenuProc_GetMenuPos( LCUI_Widget *menu, LCUI_Pos *menu_pos )
{
	LCUI_Pos pos = {0,0};
	GameMenuData *p_data;
	GameChildMenuData *p_childmenu_data;
	LCUI_Widget *p_btn, *p_exist_btn, *parent_menu;
	int i, n, btn_y, menu_height, root_menu_height;
	
	p_data = (GameMenuData*)Widget_GetPrivData( menu );
	/* 获取该菜单的按钮数量 */
	n = Queue_GetTotal( &p_data->button_list );
	/* 根据菜单按钮数量，计算该菜单的高度 */
	menu_height = n*(p_data->button_height+2);
	
	parent_menu = p_data->parent_menu;
	while( parent_menu ) {
		p_data = (GameMenuData*)Widget_GetPrivData( parent_menu );
		n = Queue_GetTotal( &p_data->childmenu_data );
		/* 确定本级菜单与哪个按钮关联 */
		for(p_btn=NULL,i=0; i<n; ++i) {
			p_childmenu_data = (GameChildMenuData*)
			Queue_Get( &p_data->childmenu_data, i );
			
			if( !p_childmenu_data ) {
				continue;
			}
			if( p_childmenu_data->menu == menu ) {
				p_btn = p_childmenu_data->target_button;
				break;
			}
		}
		if( i>= n ) {
			break;
		}

		n = Queue_GetTotal( &p_data->button_list );
		/* 确定按钮在菜单中的位置 */
		for(btn_y=0,i=0; i<n; ++i) {
			p_exist_btn = (LCUI_Widget*)Queue_Get( &p_data->button_list, i );
			if( !p_exist_btn ) {
				continue;
			}
			if( p_exist_btn == p_btn ) {
				break;
			}
			/* 计算下个按钮的位置 */
			btn_y += p_data->button_height;
			btn_y += 2;
		}
		
		/* 减去菜单中心的Y轴坐标，得出相对于中心的坐标 */
		btn_y -= n*(p_data->button_height+2)/2;
		/* 累计Y轴坐标 */
		pos.y += btn_y;
		/* 累计X轴坐标 */
		pos.x += Widget_GetWidth( parent_menu );
		/* 记录本级菜单 */
		menu = parent_menu;
		/* 切换至父级菜单 */
		parent_menu = p_data->parent_menu;
	}
	if( menu ) {
		pos.x += _Widget_GetPos( menu ).x;
		pos.y += _Widget_GetPos( menu ).y;
		p_data = (GameMenuData*)Widget_GetPrivData( menu );
		n = Queue_GetTotal( &p_data->button_list );
		root_menu_height = n*(p_data->button_height+2);
		pos.y += p_data->button_height/2;
	}
	pos.y += root_menu_height/2;
	pos.y -= menu_height/2;
	menu_pos->x = pos.x;
	menu_pos->y = pos.y;
	return 0;
}

static void GameMenuProc_UpdateEffect( void *arg )
{
	LCUI_Pos pos;
	int target_offset_x, i, n;
	static int current_offset_x = 0;
	uchar_t alpha;
	LCUI_Widget *menu;

	if( !need_update_effect ) {
		return;
	}
	n = Queue_GetTotal( &menu_stack );
	/* 计算目标X坐标偏移量 */
	target_offset_x = (n-1)*AVG_WIDTH;
	/* 如果当前X坐标偏移量已经达到目标X坐标偏移量 */
	if( current_offset_x == target_offset_x ) {
		return;
	}
	/* 如果当前X坐标偏移量小于目标X坐标偏移量 */
	if( current_offset_x < target_offset_x ) {
		current_offset_x += 4;
		if( current_offset_x > target_offset_x ) {
			current_offset_x = target_offset_x;
		}
	} else {
		current_offset_x -= 4;
		if( current_offset_x < target_offset_x ) {
			current_offset_x = target_offset_x;
		}
	}
	menu = (LCUI_Widget*)Queue_Get( &menu_stack, 0 );
	/* 根据部件的布局类型，进行相应方式的位置设置 */
	if( menu->align == ALIGN_NONE ) {
		pos.x = root_menu_pos.x - current_offset_x;
		pos.y = Widget_GetPos(menu).y;
		Widget_Move( menu, pos );
	} else {
		pos.x = root_menu_offset_pos.x-current_offset_x;
		pos.y = root_menu_offset_pos.y;
		Widget_SetAlign( menu, menu->align, pos );
	}
	/* 初始透明度为255 */
	alpha = 255;
	for(i=n-1; i>=0; --i) {
		menu = (LCUI_Widget*)Queue_Get( &menu_stack, i );
		/* 更新各个菜单的坐标 */
		GameMenuProc_GetMenuPos( menu, &pos );
		Widget_Move( menu, pos );
		/* 设置本级菜单的透明度 */
		Widget_SetAlpha( menu, alpha );
		/* 每到上一级菜单，减少一定的透明度 */
		alpha = (uchar_t)(alpha * 0.65);
	}
}

/** 显示菜单 */
static void GameMenuProc_ShowMenu( LCUI_Widget *menu )
{
	int i, n;
	LCUI_Widget *exist_menu;

	n = Queue_GetTotal( &menu_stack );
	/* 将第一个菜单作为主菜单，并记录它的坐标 */
	if( n == 0 ) {
		root_menu_pos = Widget_GetPos( menu );
		root_menu_offset_pos = menu->offset;
	}
	for(i=n-1;i>=0;--i) {
		exist_menu = (LCUI_Widget*)Queue_Get( &menu_stack, n );
		if( exist_menu && exist_menu == menu ) {
			return;
		}
	}
	Queue_AddPointer( &menu_stack, menu );
}

/* 显示子菜单 */
static int GameMenuProc_ShowChildMenu( LCUI_Widget *menu, LCUI_Widget *btn )
{
	int i, n;
	LCUI_Pos menu_pos;
	GameMenuData *p_data;
	LCUI_Widget *p_exist_btn;
	GameChildMenuData *p_childmenu_data;

	p_data = (GameMenuData*)Widget_GetPrivData( menu );
	n = Queue_GetTotal( &p_data->button_list );
	/* 先检查按钮是不是属于菜单里的 */
	for(i=0; i<n; ++i) {
		p_exist_btn = (LCUI_Widget*)Queue_Get( &p_data->button_list, i );
		if( !p_exist_btn || p_exist_btn != btn ) {
			continue;
		}
		break;
	}
	/* 若没有该按钮的记录 */
	if( i>=n ) {
		return -1;
	}

	n = Queue_GetTotal( &p_data->childmenu_data );
	for(i=0; i<n; ++i) {
		p_childmenu_data = (GameChildMenuData*)Queue_Get( &p_data->childmenu_data, i );
		if( !p_childmenu_data ) {
			continue;
		}
		/* 如果找到与该按钮关联的菜单，则显示它 */
		if( p_childmenu_data->target_button == btn ) {
			/* 获取菜单的坐标 */
			GameMenuProc_GetMenuPos( p_childmenu_data->menu, &menu_pos );
			/* 设置菜单的坐标 */
			Widget_Move( p_childmenu_data->menu, menu_pos );
			Widget_SetModal( p_childmenu_data->menu, TRUE );
			/* 显示菜单 */
			Widget_Show( p_childmenu_data->menu ); 
			/* 标记需要更新菜单效果 */
			need_update_effect = TRUE;
		}
	}
	return 0;
}

/* 隐藏最上层菜单 */
static void GameMenuProc_HideTopMenu(void)
{
	int n;
	LCUI_Widget *menu;
	
	n = Queue_GetTotal( &menu_stack );
	if( n <= 0 ) {
		return;
	}
	menu = (LCUI_Widget*)Queue_Get( &menu_stack, n-1 );
	/* 只在显示一个以上的菜单时隐藏最上层菜单 */
	if( n > 1 ) {
		Queue_DeletePointer( &menu_stack, n-1 );
		Widget_SetModal( menu, FALSE );
		Widget_Hide( menu );
		/* 标记需要更新菜单效果 */
		need_update_effect = TRUE;
	}
	/* 重置菜单焦点 */
	Widget_ResetFocus( menu );
}

static void GameMenuProc_Dispatch( LCUI_Event *event, void *unused )
{
        int i, n;
        LCUI_Widget *widget, *btn, *menu;
        LCUI_Widget *exist_menu;

        if( event->button.state == LCUIKEYSTATE_PRESSED ) {
                return;
        }
        exist_menu = (LCUI_Widget*)Queue_Get( &menu_stack, 0 );
        /* 如果该菜单的父部件不被允许响应事件 */
        if( !exist_menu || !Widget_IsAllowResponseEvent(exist_menu->parent) ) {
                return;
        }
        /* 获取点击的部件 */
        widget = Widget_At( NULL, Pos(event->button.x,event->button.y) );
        if( !widget ) {
                goto final_work;
        }

        /* 如果该部件类型就是菜单按钮 */
        if( 0 == _LCUIString_Cmp(&widget->type_name, WIDGET_TYPE_GAME_MENU_BUTTON ) ) {
                btn = widget;
        } else {
                /* 否则，查找菜单按钮类型的父级部件 */
                btn = Widget_GetParent( widget, WIDGET_TYPE_GAME_MENU_BUTTON );
                if( !btn ) {
                        goto final_work;
                }
        }

        menu = Widget_GetParent( btn, WIDGET_TYPE_GAME_MENU );
        if( !menu ) {
                goto final_work;
        }
        n = Queue_GetTotal( &menu_stack );
        for(i=n-1;i>=0;--i) {
                exist_menu = (LCUI_Widget*)Queue_Get( &menu_stack, i );
                if( exist_menu && exist_menu == menu ) {
                        break;
                }
        }
        if( i < 0 ) {
                return;
        }
        else if( i == n-1 ) {
                GameMenuProc_ShowChildMenu( menu, btn );
                return;
        }
final_work:
        GameMenuProc_HideTopMenu();
}

static LCUI_Widget *GameMenu_GetFocusButton( LCUI_Widget *menu )
{
	LCUI_Widget *btn;
	GameMenuData *p_data;
	
	p_data = (GameMenuData*)Widget_GetPrivData( menu );
	btn = menu->focus_widget;
	/* 如果没有按钮获得焦点，则让第一个按钮获得焦点 */
	if( !btn ) {
		btn = (LCUI_Widget*)Queue_Get( &p_data->button_list, 0 );
		/* 如果连按钮都没有，那就退出 */
		if( !btn ) {
			return NULL;
		}
	}
	return btn;
}

static void GameMenuProc_KeyboardControl( LCUI_Event *event, void *unused )
{
	int i, n;
	LCUI_Widget *menu, *other_btn, *btn;
	GameMenuData *p_data;
	LCUI_WidgetEvent wdg_event;
	static LCUI_Widget *click_btn = NULL;

	n = Queue_GetTotal( &menu_stack );
	if( n <= 0 ) {
		return;	
	}
	menu = (LCUI_Widget*)Queue_Get( &menu_stack, n-1 );
	/* 如果该菜单不被允许响应事件 */
	if( !Widget_IsAllowResponseEvent(menu) ) {
		return;
	}
	if( !menu->parent->visible ) {
		return;
	}
	p_data = (GameMenuData*)Widget_GetPrivData( menu );
	switch( event->key.key_code ) {
	case LCUIKEY_A:
	case LCUIKEY_K:
	case LCUIKEY_LEFT:
	case LCUIKEY_ESC:
		if( event->type != LCUI_KEYDOWN ) {
			return;
		}
		GameMenuProc_HideTopMenu();
		return;
	case LCUIKEY_D:
	case LCUIKEY_J:
	case LCUIKEY_RIGHT:
	case LCUIKEY_ENTER:
		btn = menu->focus_widget;
		/* 如果没有按钮获得焦点 */
		if( !btn ) {
			break;
		}
		Widget_SetFocus( btn );
		GameMenuProc_ShowChildMenu( menu, btn );
		/* 
		 * 如果是“按键按下”事件，则记录当前按下的
		 * 按钮部件，并设置该按钮的状态为ACTIVE 
		 */
		if( event->type == LCUI_KEYDOWN ) {
			click_btn = btn;
			Widget_SetState( btn, WIDGET_STATE_ACTIVE );
			return;
		}
		/*
		 * 否则是“按键释放”事件，若按下按键时和释放
		 * 按键的按钮一样，则向该按钮部件发送CLICKED事件，
		 */
		if( click_btn == btn ) {
			wdg_event.type = EVENT_CLICKED;
			wdg_event.clicked.rel_pos.x = 0;
			wdg_event.clicked.rel_pos.y = 0;
			Widget_DispatchEvent( btn, &wdg_event );
		}
		/* 将之前按住的按钮部件的状态设置为NORMAL */
		Widget_SetState( click_btn, WIDGET_STATE_NORMAL );
		/* 重置记录 */
		click_btn = NULL;
		return;
	case LCUIKEY_W:
	case LCUIKEY_UP:
		if( event->type != LCUI_KEYDOWN ) {
			return;
		}
		btn = menu->focus_widget;
		if( !btn ) {
			break;
		}
		n = Queue_GetTotal( &p_data->button_list );
		for(i=0; i<n; ++i) {
			other_btn = (LCUI_Widget*)Queue_Get( &p_data->button_list, i );
			if( other_btn == btn ) {
				break;
			}
		}
		--i;
		if( i < 0 ) {
			i = 0;
		}
		/* 获取上个按钮 */
		btn = (LCUI_Widget*)Queue_Get( &p_data->button_list, i );
		/* 设置焦点 */
		Widget_SetFocus( btn );
		return;
	case LCUIKEY_S:
	case LCUIKEY_DOWN:
		if( event->type != LCUI_KEYDOWN ) {
			return;
		}
		btn = menu->focus_widget;
		if( !btn ) {
			break;
		}
		n = Queue_GetTotal( &p_data->button_list );
		for(i=0; i<n; ++i) {
			other_btn = (LCUI_Widget*)Queue_Get( &p_data->button_list, i );
			if( other_btn == btn ) {
				break;
			}
		}
		++i;
		if( i >= n ) {
			i = n-1;
		}
		/* 获取下个按钮 */
		btn = (LCUI_Widget*)Queue_Get( &p_data->button_list, i );
		/* 设置焦点 */
		Widget_SetFocus( btn );
	default:return;
	}
	/* 若之前还有按住的按钮部件，则重置它的状态为NORMAL，并重置记录 */
	if( click_btn ) {
		Widget_SetState( click_btn, WIDGET_STATE_NORMAL );
		click_btn = NULL;
	}
	/* 取第一个按钮 */
	btn = (LCUI_Widget*)Queue_Get( &p_data->button_list, 0 );
	/* 如果连按钮都没有，那就退出 */
	if( !btn ) {
		return;
	}
	/* 设置焦点 */
	Widget_SetFocus( btn );
}

static void GameMenuProc_Init(void)
{
	if( module_is_inited ) {
		return;
	}
	/* 关联鼠标点击事件，以处理子菜单的调度显示 */
	connect_id[0] = LCUISysEvent_Connect( LCUI_MOUSEBUTTONDOWN, GameMenuProc_Dispatch, NULL );
	connect_id[1] = LCUISysEvent_Connect( LCUI_MOUSEBUTTONUP, GameMenuProc_Dispatch, NULL );
	/* 关联键盘事件，以实现按键控制菜单 */
	connect_id[2] = LCUISysEvent_Connect( LCUI_KEYUP, GameMenuProc_KeyboardControl, NULL );
	connect_id[3] = LCUISysEvent_Connect( LCUI_KEYDOWN, GameMenuProc_KeyboardControl, NULL );
	Queue_Init( &menu_stack, 0, NULL );
	Queue_UsingPointer( &menu_stack );
	menu_effect_timer = LCUITimer_Set( 20, GameMenuProc_UpdateEffect, NULL, TRUE );
	module_is_inited = TRUE;
}

static void GameMenu_ExecInit( LCUI_Widget *widget )
{
	GameMenuData *p_data;

	p_data = (GameMenuData*)Widget_NewPrivData( widget, sizeof(GameMenuData) );
	p_data->button_height = 34;
	p_data->font_size = 16;
	p_data->parent_menu = NULL;

	Queue_Init( &p_data->color_set, sizeof(LCUI_RGB), NULL );
	Queue_Init( &p_data->button_list, 0, NULL );
	Queue_Init( &p_data->childmenu_data, sizeof(GameChildMenuData), NULL );
	Queue_UsingPointer( &p_data->button_list );
	GameMenuProc_Init();
}

static void GameMenu_ExecDestroy( LCUI_Widget *widget )
{

}

static void GameMenu_ExecUpdate( LCUI_Widget *widget )
{
	LCUI_RGB *p_color;
	GameMenuData *p_data;
	LCUI_Widget *wdg_btn;
	LCUI_Pos btn_pos = {0,0};
	int btn_i, color_i=0, total_btn, total_color;

	p_data = (GameMenuData*)Widget_GetPrivData( widget );
	total_btn = Queue_GetTotal( &p_data->button_list );
	total_color = Queue_GetTotal( &p_data->color_set );

	for(btn_i=0; btn_i<total_btn; ++btn_i) {
		wdg_btn = (LCUI_Widget*)Queue_Get( &p_data->button_list, btn_i );
		if( !wdg_btn ) {
			continue;
		}
		/* 移动按钮位置 */
		Widget_Move( wdg_btn, btn_pos );
		/* 计算下个按钮的位置 */
		btn_pos.y += p_data->button_height;
		btn_pos.y += 2;
		/* 获取本按钮的配色 */
		p_color = (LCUI_RGB*)Queue_Get( &p_data->color_set, color_i );
		if( p_color ) {
			/* 设置按钮前景部件的颜色 */
			GameMenuBtn_SetForeWidgetColor( wdg_btn, *p_color );
			++color_i;
			if( color_i >= total_color ) {
				color_i = 0;
			}
			continue;
		}
		GameMenuBtn_SetForeWidgetColor( wdg_btn, RGB(0,0,0) );
	}
}

/** 创建一个游戏菜单 */
LCUI_Widget *GameMenu_New(void)
{
	return Widget_New( WIDGET_TYPE_GAME_MENU );
}

/** 为菜单添加一个按钮 */
LCUI_Widget *GameMenu_NewButtonW( LCUI_Widget *menu, const wchar_t *unicode_text )
{
	char height_str[12];
	LCUI_Widget *btn;
	GameMenuData *p_data;
	
	btn = GameMenuBtn_New();
	if( !btn ) {
		return NULL;
	}

	p_data = (GameMenuData*)Widget_GetPrivData( menu );
	Queue_AddPointer( &p_data->button_list, btn );
	Widget_Container_Add( menu, btn );
	sprintf( height_str, "%dpx", p_data->button_height );
	Widget_SetSize( btn, "100%", height_str );
	GameMenuBtn_SetTextW( btn, unicode_text );
	GameMenuBtn_SetFontSize( btn, p_data->font_size );
	GameMenuBtn_SetForeWidgetColor( btn, RGB(0,0,0) );
	Widget_Show( btn );
	Widget_Update( menu );
	return btn;
}

/** 为菜单设置配色方案，以让菜单按钮拥有相应的颜色 */
void GameMenu_SetColorScheme(	LCUI_Widget *menu,
				LCUI_RGB *color_set, 
				int n_color )
{
	int i;
	GameMenuData *p_data;
	p_data = (GameMenuData*)Widget_GetPrivData( menu );
	for(i=0; i<n_color; ++i) {
		Queue_Add( &p_data->color_set, &color_set[i] );
	}
}

/** 为菜单中的按钮设置子菜单，当该按钮被点击后，会显示子菜单 */
int GameMenu_SetChildMenu(	LCUI_Widget *menu,
				LCUI_Widget *menu_button, 
				LCUI_Widget *child_menu )
{
	int i, n;
	GameMenuData *p_data;
	LCUI_Widget *p_exist_btn;
	GameChildMenuData childmenu_data;

	p_data = (GameMenuData*)Widget_GetPrivData( menu );
	n = Queue_GetTotal( &p_data->button_list );
	/* 先检查按钮是不是属于菜单里的 */
	for(i=0; i<n; ++i) {
		p_exist_btn = (LCUI_Widget*)Queue_Get( &p_data->button_list, i );
		if( !p_exist_btn || p_exist_btn != menu_button ) {
			continue;
		}
		break;
	}
	/* 若没有该按钮的记录 */
	if( i>=n ) {
		return -1;
	}
	/* 准备子菜单数据 */
	childmenu_data.target_button = menu_button;
	childmenu_data.menu = child_menu;
	/* 为菜单添加子菜单记录 */
	Queue_Add( &p_data->childmenu_data, &childmenu_data );
	p_data = (GameMenuData*)Widget_GetPrivData( child_menu );
	/* 记录该子菜单的父级菜单 */
	p_data->parent_menu = menu;
	return 0;
}

/** 仅显示主菜单 */
void GameMenu_OnlyShowMainMenu(void)
{
	int n;
	n = Queue_GetTotal( &menu_stack );
	for(; n>=1; --n) {
		GameMenuProc_HideTopMenu();
	}
}

void GameMenu_Register(void)
{
	WidgetType_Add( WIDGET_TYPE_GAME_MENU );
	WidgetFunc_Add( WIDGET_TYPE_GAME_MENU, GameMenu_ExecInit, FUNC_TYPE_INIT );
	WidgetFunc_Add( WIDGET_TYPE_GAME_MENU, GameMenu_ExecUpdate, FUNC_TYPE_UPDATE );
	WidgetFunc_Add( WIDGET_TYPE_GAME_MENU, GameMenu_ExecDestroy, FUNC_TYPE_DESTROY );
	WidgetFunc_Add( WIDGET_TYPE_GAME_MENU, GameMenuProc_ShowMenu, FUNC_TYPE_SHOW );
}
