#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_LABEL_H
#include LC_PROGBAR_H
#include LC_CHARSET_H
#include <stdio.h>

#include "game_lifebar.h"
#include "game_resource.h"

#define TYPE_NAME	"GamePlayerStatusBar"

typedef struct StatusBarData_ {
	int hp;				/**< 当前生命值 */
	int max_hp;			/**< 最大生命值 */
	wchar_t player_name[9];		/**< 玩家名 */
	wchar_t type_name[5];		/**< 类型名 */
	LCUI_Graph *avatar;		/**< 头像 */
	LCUI_Widget *img_box;		/**< 头像框部件 */
	LCUI_Widget *lifebar;		/**< 血量条部件 */
	LCUI_Widget *lifebar_num;	/**< 显示血条数量的部件 */
	LCUI_Widget *label_type;	/**< 显示类型名的部件 */
	LCUI_Widget *label_type_area;	/**< 一个容器 */
	LCUI_Widget *label_name;	/**< 显示玩家名的部件 */
} StatusBarData;

LCUI_Widget *StatusBar_New(void)
{
	LCUI_Widget *widget;
	widget = Widget_New(TYPE_NAME);
	Widget_Resize( widget, Size(195,80) );
	return widget;
}

static void StatusBar_ExecInit( LCUI_Widget *widget )
{
	StatusBarData *data;
	LCUI_TextStyle style;

	data = (StatusBarData*)Widget_NewPrivData( widget, sizeof(StatusBarData));
	data->avatar = NULL;
	data->hp = 0;
	data->max_hp = 2000;
	wcsncpy( data->player_name, L"匿名", 8 );
	data->player_name[8] = 0;
	wcsncpy( data->type_name, L"NONE", 5 );
	data->player_name[5] = 0;
	/* 创建部件 */
	data->img_box = Widget_New(NULL);
	data->label_type = Widget_New("label");
	data->label_name = Widget_New("label");
	data->lifebar_num = Widget_New(NULL);
	data->label_type_area = Widget_New(NULL);
	data->lifebar = LifeBar_New();
	/* 加入至本容器 */
	Widget_Container_Add( widget, data->img_box );
	Widget_Container_Add( widget, data->label_type_area );
	Widget_Container_Add( widget, data->label_name );
	Widget_Container_Add( widget, data->lifebar );
	Widget_Container_Add( data->lifebar, data->lifebar_num );
	Widget_Container_Add( data->label_type_area, data->label_type );
	/* 设置默认的文本样式 */
	TextStyle_Init( &style );
	TextStyle_FontSize( &style, 14 );
	Label_TextStyle( data->label_type, style );
	Label_TextStyle( data->label_name, style );
	/* 调整尺寸 */
	Widget_Resize( data->label_type_area, Size(45,15));
	Widget_Resize( data->img_box, Size(45,45) );
	Widget_Resize( data->lifebar, Size(135,25) );
	/* 加上边框 */
	Widget_SetBorder( data->img_box, Border(1,BORDER_STYLE_SOLID,RGB(150,150,150)) );
	Widget_SetBorder( widget, Border(1,BORDER_STYLE_SOLID,RGB(150,150,150)) );
	/* 设置布局 */
	Widget_SetAlign( data->img_box, ALIGN_TOP_LEFT, Pos(5,25) );
	Widget_SetAlign( data->label_type, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	Widget_SetAlign( data->label_type_area, ALIGN_TOP_LEFT, Pos(5,5) );
	Widget_SetAlign( data->label_name, ALIGN_TOP_LEFT, Pos(55+2,5) );
	Widget_SetAlign( data->lifebar, ALIGN_TOP_LEFT, Pos(55,25) );
	Widget_SetAlign( data->lifebar_num, ALIGN_MIDDLE_RIGHT, Pos(-2,0));

	Widget_SetBackgroundColor( data->img_box, RGB(240,240,240) );
	Widget_SetBackgroundTransparent( data->img_box, FALSE );
	Widget_SetBackgroundColor( widget, RGB(255,255,255) );
	Widget_SetBackgroundTransparent( widget, FALSE );

	/* 设置生命条的参数 */
	LifeBar_SetFullHP( data->lifebar, 400 );
	LifeBar_SetMaxHP( data->lifebar, data->max_hp );
	LifeBar_SetHP( data->lifebar, data->hp );
	/* 设置显示的文本 */
	Label_TextW( data->label_type, data->type_name );
	Label_TextW( data->label_name, data->player_name );
	/* 全部显示 */
	Widget_Show( data->label_type );
	Widget_Show( data->label_type_area );
	Widget_Show( data->label_name );
	Widget_Show( data->img_box );
	Widget_Show( data->lifebar );
}

/** 更新血条数量 */
static void StatusBar_UpdateLifeBarNum( LCUI_Widget *widget )
{
	int i, n, w, x;
	char str[10], ch[2];
	StatusBarData *data;
	LCUI_Graph bg, graph[5];

	data = (StatusBarData*)Widget_GetPrivData( widget );
	n = LifeBar_GetBarNum( data->lifebar );
	if( n <= 1 ) {
		Widget_Hide( data->lifebar_num );
		return;
	}
	sprintf( str, "%d", n );
	n = strlen(str);
	GameGraphRes_GetGraph( FONT_RES, "x", &graph[0] );
	if( n > 4 ) {
		n = 4;
	}
	w = graph[0].w;
	/* 预先保存结束符 */
	ch[1] = 0;
	for(i=n-1; i>=0; --i) {
		/* 保存当前字符 */
		ch[0] = str[i];	
		/* 获取与该字符对应的字体图像 */
		if( GameGraphRes_GetGraph( FONT_RES, ch, &graph[i+1] ) != 0 ) {
			continue;
		}
		/* 累计图像宽度 */
		w += graph[i+1].w;
	}
	x = 0;
	Graph_Init( &bg );
	bg.color_type = COLOR_TYPE_RGBA;
	/* 创建合适尺寸的图像 */
	Graph_Create( &bg, w, 18 );
	for(i=0; i<=n; ++i) {
		/* 将获得到的字体图像覆盖至图像上 */
		Graph_Replace( &bg, &graph[i], Pos(x,0));
		x += graph[i].w;
	}
	/* 如果有背景图，那就释放它，因为这个部件的背景图只有本函数在设置 */
	if( Graph_IsValid( &data->lifebar_num->background.image ) ) {
		Graph_Free( &data->lifebar_num->background.image );
	}
	/* 设置背景 */
	Widget_SetBackgroundImage( data->lifebar_num, &bg );
	Widget_SetBackgroundTransparent( data->lifebar_num, TRUE );
	Widget_SetBackgroundLayout( data->lifebar_num, LAYOUT_NONE );
	/* 调整尺寸 */
	Widget_Resize( data->lifebar_num, Size(w,18) );
	/* 显示在血条前面 */
	Widget_SetZIndex( data->lifebar_num, 1000 );
	/* 显示之 */
	Widget_Show( data->lifebar_num );
}

/** 设置玩家名称(最多8个字) */
void StatusBar_SetPlayerNameW( LCUI_Widget *widget, const wchar_t *name )
{
	StatusBarData *data;
	data = (StatusBarData*)Widget_GetPrivData( widget );
	wcsncpy( data->player_name, name, 8 );
	Label_TextW( data->label_name, data->player_name );
}

void StatusBar_SetPlayerName( LCUI_Widget *widget, const char *name )
{

}

/** 设置玩家类型名称，最多4个字，如：1P、2P或CPU */
void StatusBar_SetPlayerTypeNameW( LCUI_Widget *widget, const wchar_t *type_name )
{
	StatusBarData *data;
	data = (StatusBarData*)Widget_GetPrivData( widget );
	wcsncpy( data->type_name, type_name, 4 );
	Label_TextW( data->label_type, data->type_name );
}

void StatusBar_SetPlayerTypeName( LCUI_Widget *widget, const char *type_name )
{

}

/** 设置最大生命值 */
void StatusBar_SetMaxHealth( LCUI_Widget *widget, int value )
{
	StatusBarData *data;
	data = (StatusBarData*)Widget_GetPrivData( widget );
	LifeBar_SetMaxHP( data->lifebar, value );
	StatusBar_UpdateLifeBarNum( widget );
}

/** 设置当前生命值 */
void StatusBar_SetHealth( LCUI_Widget *widget, int value )
{
	StatusBarData *data;
	data = (StatusBarData*)Widget_GetPrivData( widget );
	LifeBar_SetHP( data->lifebar, value );
	StatusBar_UpdateLifeBarNum( widget );
}

/** 设置头像 */
void StatusBar_SetAvatar( LCUI_Widget *widget, LCUI_Graph *img )
{

}

/** 注册状态栏部件 */
void StatusBar_Register(void)
{
	WidgetType_Add(TYPE_NAME);
	WidgetFunc_Add(TYPE_NAME, StatusBar_ExecInit, FUNC_TYPE_INIT );
}
