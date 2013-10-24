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

static LCUI_BOOL module_is_inited = FALSE;
LCUI_Queue menu_stack;

/** 获取菜单的显示位置 */
static int GameMenuProc_GetMenuPos( LCUI_Widget *menu, LCUI_Pos *menu_pos )
{
	LCUI_Pos pos = {0,0};
	GameMenuData *p_data;
	GameChildMenuData *p_childmenu_data;
	LCUI_Widget *p_btn, *p_exist_btn, *parent_menu;
	int i, n, btn_y, menu_height;
	
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
		pos.x += Widget_GetPos( menu ).x;
		pos.y += Widget_GetPos( menu ).y;
	}
	pos.y += menu_height/2;
	menu_pos->x = pos.x;
	menu_pos->y = pos.y;
	return 0;
}

/** 显示菜单 */
static void GameMenuProc_ShowMenu( LCUI_Widget *menu )
{
	int i, n;
	LCUI_Widget *exist_menu;

	n = Queue_GetTotal( &menu_stack );
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
	}
	/* 重置菜单焦点 */
	Widget_ResetFocus( menu );
}

static void GameMenuProc_Dispatch( LCUI_MouseButtonEvent *event, void *unused )
{
	int i, n;
	LCUI_Widget *widget, *btn, *menu;
	LCUI_Widget *exist_menu;

	if( event->state == LCUIKEYSTATE_PRESSED ) {
		return;
	}
	/* 获取点击的部件 */
	widget = Widget_At( NULL, Pos(event->x,event->y) );
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

	if( !module_is_inited ) {
		/* 关联鼠标点击事件，以处理子菜单的调度显示 */
		LCUI_MouseButtonEvent_Connect( GameMenuProc_Dispatch, NULL );
		Queue_Init( &menu_stack, 0, NULL );
		Queue_UsingPointer( &menu_stack );
		module_is_inited = TRUE;
	}
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

void GameMenu_Register(void)
{
	WidgetType_Add( WIDGET_TYPE_GAME_MENU );
	WidgetFunc_Add( WIDGET_TYPE_GAME_MENU, GameMenu_ExecInit, FUNC_TYPE_INIT );
	WidgetFunc_Add( WIDGET_TYPE_GAME_MENU, GameMenu_ExecUpdate, FUNC_TYPE_UPDATE );
	WidgetFunc_Add( WIDGET_TYPE_GAME_MENU, GameMenu_ExecDestroy, FUNC_TYPE_DESTROY );
	WidgetFunc_Add( WIDGET_TYPE_GAME_MENU, GameMenuProc_ShowMenu, FUNC_TYPE_SHOW );
}
