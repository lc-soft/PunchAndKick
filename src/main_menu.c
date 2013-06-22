#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_CURSOR_H
#include LC_WIDGET_H
#include LC_LABEL_H
#include LC_INPUT_H

#include "game.h"

#define ITEM_SIZE	Size(135,30)
#define ITEM_BOX_SIZE	Size(135,175)
#define MOVE_SPEED	(10)

static LCUI_Graph menu_imte_img[5];
static LCUI_Widget *menu_item[5];
static LCUI_Widget *select_box;
static int timer_id = -1;
static int select_item_pos = 0;
static int select_box_des_y = 0;

static char img_path[5][256]={
	"drawable/human-vs-computer.png",
	"drawable/human-vs-human.png",
	"drawable/keyboard-set.png",
	"drawable/about.png",
	"drawable/exit-game.png"
};

int Game_LoadMenuRes(void)
{
	int i, ret;
	for(i=0; i<5; ++i) {
		ret |= Graph_LoadImage( img_path[i], &menu_imte_img[i] );
	}
	return ret;
}

static void DrawSelectBox( LCUI_Graph *buff )
{
	LCUI_Graph bound_img;
	int x, y, w, h;

	w = 135;
	h = 30;
	Graph_Init( &bound_img );
	bound_img.color_type = COLOR_TYPE_RGBA;
	buff->color_type = COLOR_TYPE_RGBA;
	Graph_Create( buff, w, h );
	Graph_Create( &bound_img, 38, h );
	Graph_FillColor( &bound_img, RGB(210,10,10) );
	Graph_FillColor( buff, RGB(210,10,10) );
	Graph_FillAlpha( buff, 255 );

	/* 进行从左至右的渐变透明处理 */
	for(y=0; y<h; ++y) {
		for(x=0; x<38; ++x) {
			bound_img.rgba[3][y*38+x] = x*255/38;
		}
	}
	Graph_Replace( buff, &bound_img, Pos(0,0) );
	/* 进行从左至右的渐变透明处理 */
	for(y=0; y<h; ++y) {
		for(x=37; x>=0; --x) {
			bound_img.rgba[3][y*38+x] = (37-x)*255/38;
		}
	}
	Graph_Replace( buff, &bound_img, Pos(w-38,0) );
	Graph_DrawHorizLine( buff, RGB(0,0,0), 1,Pos(0,0), w );
	Graph_DrawHorizLine( buff, RGB(0,0,0), 1,Pos(0,h-1), w );
	Graph_DrawHorizLine( buff, RGB(200,165,0) ,1,Pos(0,1), w );
	Graph_DrawHorizLine( buff, RGB(200,165,0), 1,Pos(0,h-2), w );
}


static void BlinkSelectBox(void)
{
	static LCUI_BOOL show = TRUE;
	static uchar_t alpha = 255;
	if( show ) {
		if( alpha <= 100 ) {
			alpha = 100;
			show = FALSE;
		} else {
			alpha -= 5;
		}
	} else {
		if( alpha > 240 ) {
			alpha = 255;
			show = TRUE;
		} else {
			alpha += 5;
		}
	}
	Widget_SetAlpha( select_box, alpha );
}

/* 更新选择框的坐标 */
static void UpdateSelectBoxPos( void *arg )
{
	int new_y, n;
	LCUI_Widget *widget;
	
	widget = (LCUI_Widget*)arg;
	while(1) {
		new_y = widget->pos.y;
		n = select_box_des_y - widget->pos.y;
		if( n < 0 ) {
			/* 向上移动 */
			if( n < -MOVE_SPEED ) {
				new_y -= MOVE_SPEED;
				Widget_Move( widget, Pos(0,new_y) );
			} else {
				Widget_Move( widget, Pos(0,select_box_des_y) );
			}
		} else if( n > 0 ) {
			/* 向下移动 */
			if( n > MOVE_SPEED ) {
				new_y += MOVE_SPEED;
				Widget_Move( widget, Pos(0,new_y) );
			} else {
				Widget_Move( widget, Pos(0,select_box_des_y) );
			}
		}
		select_item_pos = select_box_des_y/35;
		LCUI_MSleep(10);
	}
}

static void PorcMouseMovtion( LCUI_Widget *widget, LCUI_WidgetEvent *event )
{
	LCUI_Widget *wdg_item;
	wdg_item = Widget_At( widget, event->mouse_motion.rel_pos );
	if( wdg_item == NULL ) {
		return;
	}
	select_box_des_y = wdg_item->pos.y;
}

static void Game_ShowAbout(void)
{

}

static void Game_Exit(void)
{
	LCUI_MainLoop_Quit(NULL);
}

static void Item0_ProcClicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{
	
}

static void Item1_ProcClicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{

}

static void Item2_ProcClicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{

}

static void Item3_ProcClicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{

}

static void Item4_ProcClicked( LCUI_Widget *widget, LCUI_WidgetEvent *unused )
{
	Game_Exit();
}

static void MainMenu_ProcInput( LCUI_KeyboardEvent *event, void *arg )
{
	LCUI_WidgetEvent wdg_event;

	switch( event->key_code ) {
	case LCUIKEY_W:
	case LCUIKEY_UP:
		if( select_item_pos > 0 ) {
			--select_item_pos;
		}
		break;
	case LCUIKEY_S:
	case LCUIKEY_DOWN:
		if( select_item_pos < 4 ) {
			++select_item_pos;
		}
		break;
	case LCUIKEY_J:
	case LCUIKEY_ENTER:
		wdg_event.type = EVENT_CLICKED;
		wdg_event.clicked.rel_pos.x = 0;
		wdg_event.clicked.rel_pos.y = 0;
		Widget_DispatchEvent( menu_item[select_item_pos], &wdg_event );
	default:
		break;
	}
	select_box_des_y = select_item_pos*35;
}

void Game_MainMenu( void )
{
	int i, y=0;
	uchar_t alpha;
	LCUI_Thread t;
	LCUI_Graph select_box_bg;
	LCUI_Widget *box;
	void (*func[5])(LCUI_Widget*,LCUI_WidgetEvent*)={
		Item0_ProcClicked, Item1_ProcClicked,
		Item2_ProcClicked, Item3_ProcClicked,
		Item4_ProcClicked
	};
	box = Widget_New(NULL);
	select_box = Widget_New(NULL);
	Widget_Container_Add( box, select_box );
	Widget_Resize( box, ITEM_BOX_SIZE );
	Widget_Resize( select_box, ITEM_SIZE );

	Graph_Init( &select_box_bg );
	DrawSelectBox( &select_box_bg );
	Widget_SetBackgroundImage( select_box, &select_box_bg );

	for(i=0; i<5; ++i) {
		menu_item[i] = Widget_New(NULL);
		Widget_Container_Add( box, menu_item[i] );
		Widget_Resize( menu_item[i], ITEM_SIZE );
		Widget_SetBackgroundImage( menu_item[i], &menu_imte_img[i] );
		Widget_SetBackgroundLayout( menu_item[i], LAYOUT_CENTER );
		Widget_Move( menu_item[i], Pos(0,y) );
		Widget_SetZIndex( menu_item[i], 10 );
		Widget_Event_Connect( menu_item[i], EVENT_CLICKED, func[i] );
		Widget_Show( menu_item[i] );
		y += 35;
	}
	Widget_SetAlign( box, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	Widget_SetAlpha( box, 0 );
	Widget_Show( box );
	Widget_Show( select_box );
	
	/* 以淡入的效果显示LOGO */
	for( alpha=0; alpha<240; alpha+=20 ) {
		Widget_SetAlpha( box, alpha );
		LCUI_MSleep(25);
	}
	Widget_SetAlpha( box, 255 );

	Widget_Event_Connect( box, EVENT_MOUSEMOTION, PorcMouseMovtion );
	LCUI_KeyboardEvent_Connect( MainMenu_ProcInput, NULL );
	LCUIThread_Create( &t, UpdateSelectBoxPos, (void*)select_box );
	timer_id = LCUITimer_Set( 10, BlinkSelectBox, TRUE );
}
