#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_CURSOR_H
#include LC_WIDGET_H
#include LC_LABEL_H
#include LC_INPUT_H

#include "game.h"
#define ITEM_BOX_HEIGHT	50

static LCUI_Widget *main_menu_box;
static LCUI_Widget *main_menu_item_box;
static LCUI_Widget *front_wave[2], *back_wave[2];
static LCUI_Graph img_res[3];

static char img_path[3][256]={
	"bg.png",
	"wave1.png",
	"wave2.png"
};

static void UIEffect_MoveWave1( void *arg )
{
	LCUI_Pos offset_pos;
	static LCUI_Pos wave_offset_pos[2] = {
		{0,-ITEM_BOX_HEIGHT},
		{0,-ITEM_BOX_HEIGHT}
	};

	if( wave_offset_pos[0].x == 0 ) {
		wave_offset_pos[1].x = Graph_GetSize(&img_res[1]).w;
	}
	--wave_offset_pos[0].x;
	--wave_offset_pos[1].x;
	if( wave_offset_pos[0].x <= -Graph_GetSize(&img_res[1]).w ) {
		wave_offset_pos[0].x = Graph_GetSize(&img_res[1]).w;
	}
	if( wave_offset_pos[1].x <= -Graph_GetSize(&img_res[1]).w ) {
		wave_offset_pos[1].x = Graph_GetSize(&img_res[1]).w;
	}
	offset_pos.x = wave_offset_pos[0].x;
	offset_pos.y = wave_offset_pos[0].y;
	Widget_SetAlign( front_wave[0], ALIGN_BOTTOM_LEFT, offset_pos );
	offset_pos.x = wave_offset_pos[1].x;
	offset_pos.y = wave_offset_pos[1].y;
	Widget_SetAlign( front_wave[1], ALIGN_BOTTOM_LEFT, offset_pos );
}

static void UIEffect_MoveWave2( void *arg )
{
	LCUI_Pos offset_pos;
	static LCUI_Pos wave_offset_pos[2] = {
		{0,-ITEM_BOX_HEIGHT},
		{0,-ITEM_BOX_HEIGHT}
	};

	if( wave_offset_pos[0].x == 0 ) {
		wave_offset_pos[1].x = Graph_GetSize(&img_res[2]).w;
	}
	--wave_offset_pos[0].x;
	--wave_offset_pos[1].x;
	if( wave_offset_pos[0].x <= -Graph_GetSize(&img_res[2]).w ) {
		wave_offset_pos[0].x = Graph_GetSize(&img_res[2]).w;
	}
	if( wave_offset_pos[1].x <= -Graph_GetSize(&img_res[2]).w ) {
		wave_offset_pos[1].x = Graph_GetSize(&img_res[2]).w;
	}
	offset_pos.x = wave_offset_pos[0].x;
	offset_pos.y = wave_offset_pos[0].y;
	Widget_SetAlign( back_wave[0], ALIGN_BOTTOM_LEFT, offset_pos );
	offset_pos.x = wave_offset_pos[1].x;
	offset_pos.y = wave_offset_pos[1].y;
	Widget_SetAlign( back_wave[1], ALIGN_BOTTOM_LEFT, offset_pos );
}

int Game_LoadMainMenuRes(void)
{
	int i, ret;
	ret = 0;
	for(i=0; i<3; ++i) {
		ret |= Graph_LoadImage( img_path[i], &img_res[i] );
	}
	return ret;
}

void Game_InitMainMenu(void)
{
	Game_LoadMainMenuRes();
	
	main_menu_box = Widget_New(NULL);
	main_menu_item_box = Widget_New(NULL);
	front_wave[0] = Widget_New(NULL);
	front_wave[1] = Widget_New(NULL);
	back_wave[0] = Widget_New(NULL);
	back_wave[1] = Widget_New(NULL);

	Widget_SetSize( main_menu_box, "100%", "100%" );
	Widget_SetAlign( main_menu_box, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	Widget_SetAlpha( main_menu_box, 0 );
	Widget_SetBackgroundImage( main_menu_box, &img_res[0] );
	Widget_SetBackgroundLayout( main_menu_box, LAYOUT_TILE );
	Widget_SetBackgroundTransparent ( main_menu_box, FALSE );
	
	Widget_Container_Add( main_menu_box, main_menu_item_box );
	Widget_SetHeight( main_menu_item_box, ITEM_BOX_HEIGHT );
	Widget_SetSize( main_menu_item_box, "100%", NULL );
	Widget_SetAlign( main_menu_item_box, ALIGN_BOTTOM_CENTER, Pos(0,0) );
	Widget_SetBackgroundColor( main_menu_item_box, RGB(255,255,255) );
	Widget_SetBackgroundTransparent ( main_menu_item_box, FALSE );

	Widget_Container_Add( main_menu_box, front_wave[0] );
	Widget_Container_Add( main_menu_box, front_wave[1] );
	Widget_Resize( front_wave[0], Graph_GetSize(&img_res[1]) );
	Widget_Resize( front_wave[1], Graph_GetSize(&img_res[1]) );
	Widget_SetBackgroundImage( front_wave[0], &img_res[1] );
	Widget_SetBackgroundImage( front_wave[1], &img_res[1] );
	Widget_SetBackgroundLayout( front_wave[0], LAYOUT_CENTER );
	Widget_SetBackgroundLayout( front_wave[1], LAYOUT_CENTER );
	Widget_SetAlign( front_wave[0], ALIGN_BOTTOM_LEFT, Pos(0,-ITEM_BOX_HEIGHT) );
	Widget_SetAlign( front_wave[1], ALIGN_BOTTOM_LEFT, Pos(Graph_GetSize(&img_res[1]).w,-ITEM_BOX_HEIGHT) );
	
	Widget_Container_Add( main_menu_box, back_wave[0] );
	Widget_Container_Add( main_menu_box, back_wave[1] );
	Widget_Resize( back_wave[0], Graph_GetSize(&img_res[2]) );
	Widget_Resize( back_wave[1], Graph_GetSize(&img_res[2]) );
	Widget_SetBackgroundImage( back_wave[0], &img_res[2] );
	Widget_SetBackgroundImage( back_wave[1], &img_res[2] );
	Widget_SetBackgroundLayout( back_wave[0], LAYOUT_CENTER );
	Widget_SetBackgroundLayout( back_wave[1], LAYOUT_CENTER );
	Widget_SetAlign( back_wave[0], ALIGN_BOTTOM_LEFT, Pos(0,-ITEM_BOX_HEIGHT) );
	Widget_SetAlign( back_wave[1], ALIGN_BOTTOM_LEFT, Pos(Graph_GetSize(&img_res[2]).w,-ITEM_BOX_HEIGHT) );
	
	Widget_Show( front_wave[0] );
	Widget_Show( front_wave[1] );
	Widget_Show( back_wave[0] );
	Widget_Show( back_wave[1] );
	Widget_Show( main_menu_item_box );
	Widget_Show( main_menu_box );

	LCUITimer_Set( 20, UIEffect_MoveWave1, NULL, TRUE );
	LCUITimer_Set( 60, UIEffect_MoveWave2, NULL, TRUE );
}

void Game_ShowMainMenu(void)
{
	uchar_t alpha;

	Widget_Show( main_menu_box );
	/* 以淡入的效果显示菜单 */
	for( alpha=0; alpha<240; alpha+=20 ) {
		Widget_SetAlpha( main_menu_box, alpha );
		LCUI_MSleep(25);
	}
	Widget_SetAlpha( main_menu_box, 255 );
}

void Game_HideMainMenu(void)
{
	uchar_t alpha;
	/* 以淡出的效果显示菜单 */
	for( alpha=240; alpha>0; alpha-=20 ) {
		Widget_SetAlpha( main_menu_box, alpha );
		LCUI_MSleep(25);
	}
	Widget_SetAlpha( main_menu_box, 0 );
	Widget_Hide( main_menu_box );
}
