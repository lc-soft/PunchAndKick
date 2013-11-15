// 游戏窗口-键位设置
#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_BUTTON_H
#include LC_LABEL_H
#include LC_WINDOW_H
#include LC_INPUT_H

#include "game_control.h"
#include "game_config.h"
#include "gamewnd_setkeyboard.h"

#define WINDOW_SIZE		Size(345,245)
#define BTN_SIZE		Size(80,30)
#define KEYBTN_SIZE		Size(50,50)
#define TIPBOX_SIZE		Size(240,120)

#define TEXT_OK			L"确定"
#define TEXT_RESET		L"恢复默认"
#define TEXT_SET_KEYBOARD	L" <size=15px>键位设置</size> "
#define TEXT_LEFT		L"<size=15px>左</size>"
#define TEXT_RIGHT		L"<size=15px>右</size>"
#define TEXT_UP			L"<size=15px>上</size>"
#define TEXT_DOWN		L"<size=15px>下</size>"
#define TEXT_JUMP		L"<size=15px>跳跃</size>"
#define TEXT_DEFENSE		L"<size=15px>防御</size>"
#define TEXT_A_ATK		L"<size=15px>拳击</size>"
#define TEXT_B_ATK		L"<size=15px>脚踢</size>"
#define TEXT_PLEASE_PRESS_KEY	L"<size=18px>请按下要使用的键</size>"
#define TEXT_KEY_IS_NOT_VALID	L"<size=18px>该键不可用，请按下另外的键</size>"

LCUI_Widget *window, *box, *label, *btn_ok, *btn_reset;
LCUI_Widget *tip_box, *tip_label;

LCUI_Widget	*btn_left, *btn_right, *btn_up, *btn_down, 
		*btn_a_atk, *btn_b_atk, *btn_defense, *btn_jump;
LCUI_Widget	*label_left, *label_right, *label_up, *label_down, 
		*label_a_atk, *label_b_atk, *label_defense, *label_jump;

static int keyboard_connect_id = -1;

static LCUI_BOOL KeyIsValid( int key_code )
{
	switch(key_code) {
	case LCUIKEY_UP:
	case LCUIKEY_DOWN:
	case LCUIKEY_LEFT:
	case LCUIKEY_RIGHT:
	case LCUIKEY_SPACE:
	case LCUIKEY_0:
	case LCUIKEY_1:
	case LCUIKEY_2:
	case LCUIKEY_3:
	case LCUIKEY_4:
	case LCUIKEY_5:
	case LCUIKEY_6:
	case LCUIKEY_7:
	case LCUIKEY_8:
	case LCUIKEY_9:
	case LCUIKEY_A:
	case LCUIKEY_B:
	case LCUIKEY_C:
	case LCUIKEY_D:
	case LCUIKEY_E:
	case LCUIKEY_F:
	case LCUIKEY_G:
	case LCUIKEY_H:
	case LCUIKEY_I:
	case LCUIKEY_J:
	case LCUIKEY_K:
	case LCUIKEY_L:
	case LCUIKEY_M:
	case LCUIKEY_N:
	case LCUIKEY_O:
	case LCUIKEY_P:
	case LCUIKEY_Q:
	case LCUIKEY_R:
	case LCUIKEY_S:
	case LCUIKEY_T:
	case LCUIKEY_U:
	case LCUIKEY_V:
	case LCUIKEY_W:
	case LCUIKEY_X:
	case LCUIKEY_Y:
	case LCUIKEY_Z:
	break;
	default:return FALSE;
	}
	return TRUE;
}


static LCUI_BOOL GetKeyText( int key_code, wchar_t *text )
{
	switch(key_code) {
	case LCUIKEY_UP: text[0] = L'↑'; text[1] = 0; break;
	case LCUIKEY_DOWN: text[0] = L'↓'; text[1] = 0; break;
	case LCUIKEY_LEFT: text[0] = L'←'; text[1] = 0; break;
	case LCUIKEY_RIGHT: text[0] = L'→'; text[1] = 0; break;
	case LCUIKEY_SPACE: text[0] = L'空'; text[1] = L'格'; text[2] = 0; break;
	case LCUIKEY_0: text[0] = L'0'; text[1] = 0; break;
	case LCUIKEY_1: text[0] = L'1'; text[1] = 0; break;
	case LCUIKEY_2: text[0] = L'2'; text[1] = 0; break;
	case LCUIKEY_3: text[0] = L'3'; text[1] = 0; break;
	case LCUIKEY_4: text[0] = L'4'; text[1] = 0; break;
	case LCUIKEY_5: text[0] = L'5'; text[1] = 0; break;
	case LCUIKEY_6: text[0] = L'6'; text[1] = 0; break;
	case LCUIKEY_7: text[0] = L'7'; text[1] = 0; break;
	case LCUIKEY_8: text[0] = L'8'; text[1] = 0; break;
	case LCUIKEY_9: text[0] = L'9'; text[1] = 0; break;
	case LCUIKEY_A: text[0] = L'A'; text[1] = 0; break;
	case LCUIKEY_B: text[0] = L'B'; text[1] = 0; break;
	case LCUIKEY_C: text[0] = L'C'; text[1] = 0; break;
	case LCUIKEY_D: text[0] = L'D'; text[1] = 0; break;
	case LCUIKEY_E: text[0] = L'E'; text[1] = 0; break;
	case LCUIKEY_F: text[0] = L'F'; text[1] = 0; break;
	case LCUIKEY_G: text[0] = L'G'; text[1] = 0; break;
	case LCUIKEY_H: text[0] = L'H'; text[1] = 0; break;
	case LCUIKEY_I: text[0] = L'I'; text[1] = 0; break;
	case LCUIKEY_J: text[0] = L'J'; text[1] = 0; break;
	case LCUIKEY_K: text[0] = L'K'; text[1] = 0; break;
	case LCUIKEY_L: text[0] = L'L'; text[1] = 0; break;
	case LCUIKEY_M: text[0] = L'M'; text[1] = 0; break;
	case LCUIKEY_N: text[0] = L'N'; text[1] = 0; break;
	case LCUIKEY_O: text[0] = L'O'; text[1] = 0; break;
	case LCUIKEY_P: text[0] = L'P'; text[1] = 0; break;
	case LCUIKEY_Q: text[0] = L'Q'; text[1] = 0; break;
	case LCUIKEY_R: text[0] = L'R'; text[1] = 0; break;
	case LCUIKEY_S: text[0] = L'S'; text[1] = 0; break;
	case LCUIKEY_T: text[0] = L'T'; text[1] = 0; break;
	case LCUIKEY_U: text[0] = L'U'; text[1] = 0; break;
	case LCUIKEY_V: text[0] = L'V'; text[1] = 0; break;
	case LCUIKEY_W: text[0] = L'W'; text[1] = 0; break;
	case LCUIKEY_X: text[0] = L'X'; text[1] = 0; break;
	case LCUIKEY_Y: text[0] = L'Y'; text[1] = 0; break;
	case LCUIKEY_Z: text[0] = L'Z'; text[1] = 0; break;
	break;
	default:return FALSE;
	}
	return TRUE;
}

/** 更新按钮上显示的按键文字 */
static void UpdateKeyBtn(void)
{
	ControlKey ctrlkey;
	wchar_t btn_text[24], buff_text[12];

	GameConfig_GetKeyControl( &ctrlkey );
	
	GetKeyText( ctrlkey.left, buff_text );
	swprintf( btn_text, sizeof(btn_text), L"<size=18px>%s</size>", buff_text );
	Button_TextW( btn_left, btn_text );
	
	GetKeyText( ctrlkey.right, buff_text );
	swprintf( btn_text, sizeof(btn_text), L"<size=18px>%s</size>", buff_text );
	Button_TextW( btn_right, btn_text );
	
	GetKeyText( ctrlkey.up, buff_text );
	swprintf( btn_text, sizeof(btn_text), L"<size=18px>%s</size>", buff_text );
	Button_TextW( btn_up, btn_text );
	
	GetKeyText( ctrlkey.down, buff_text );
	swprintf( btn_text, sizeof(btn_text), L"<size=18px>%s</size>", buff_text );
	Button_TextW( btn_down, btn_text );

	GetKeyText( ctrlkey.a_attack, buff_text );
	swprintf( btn_text, sizeof(btn_text), L"<size=18px>%s</size>", buff_text );
	Button_TextW( btn_a_atk, btn_text );

	GetKeyText( ctrlkey.b_attack, buff_text );
	swprintf( btn_text, sizeof(btn_text), L"<size=18px>%s</size>", buff_text );
	Button_TextW( btn_b_atk, btn_text );

	GetKeyText( ctrlkey.defense, buff_text );
	swprintf( btn_text, sizeof(btn_text), L"<size=18px>%s</size>", buff_text );
	Button_TextW( btn_defense, btn_text );

	GetKeyText( ctrlkey.jump, buff_text );
	swprintf( btn_text, sizeof(btn_text), L"<size=18px>%s</size>", buff_text );
	Button_TextW( btn_jump, btn_text );
}

static void ProcKeyDown( LCUI_Event *event, void *arg )
{
	ControlKey ctrlkey;
	LCUI_Widget *target_btn;
	/** 如果按键不可用 */
	if( !KeyIsValid(event->key.key_code) ) {
		Label_TextW( tip_label, TEXT_KEY_IS_NOT_VALID );
		return;
	}
	GameConfig_GetKeyControl( &ctrlkey );
	target_btn = (LCUI_Widget*)arg;
	if( target_btn == btn_left ) {
		ctrlkey.left = event->key.key_code;
	}
	else if( target_btn == btn_right ) {
		ctrlkey.right = event->key.key_code;
	}
	else if( target_btn == btn_up ) {
		ctrlkey.up = event->key.key_code;
	}
	else if( target_btn == btn_down ) {
		ctrlkey.down = event->key.key_code;
	}
	else if( target_btn == btn_a_atk ) {
		ctrlkey.a_attack = event->key.key_code;
	}
	else if( target_btn == btn_b_atk ) {
		ctrlkey.b_attack = event->key.key_code;
	}
	else if( target_btn == btn_jump ) {
		ctrlkey.jump = event->key.key_code;
	}
	else if( target_btn == btn_defense ) {
		ctrlkey.defense = event->key.key_code;
	}
	GameConfig_SetKeyControl( &ctrlkey );
	GameConfig_Save();
	UpdateKeyBtn();
	Widget_Destroy( tip_box );
	LCUISysEvent_Disconnect( LCUI_KEYDOWN, keyboard_connect_id );
	tip_box = NULL;
	keyboard_connect_id = -1;
}

static void StartCatchKey( LCUI_Widget *btn )
{
	if( tip_box ) {
		return;
	}
	tip_box = Widget_New(NULL);
	tip_label = Widget_New("label");

	Widget_Container_Add( tip_box, tip_label );
	Label_TextW( tip_label, TEXT_PLEASE_PRESS_KEY );
	Widget_SetAlign( tip_label, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	Widget_SetAlign( tip_box, ALIGN_MIDDLE_CENTER, Pos(0,0) );
	Widget_Resize( tip_box, TIPBOX_SIZE );
	Widget_SetBackgroundTransparent( tip_box, FALSE );
	Widget_SetBorder( tip_box, Border(1,BORDER_STYLE_SOLID,RGB(200,200,200)) );
	
	keyboard_connect_id = LCUISysEvent_Connect( LCUI_KEYDOWN, ProcKeyDown, btn );

	Widget_Show( tip_label );
	Widget_SetModal( tip_box, TRUE );
	Widget_Show( tip_box );
}

static void btn_ok_on_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *event )
{
	GameWindow_HideSetKeyboardWindow();
	GameWindow_DestroySetKeyboardWindow();
}

static void btn_reset_on_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *event )
{
	GameConfig_InitKeyControl();
	GameConfig_Save();
	UpdateKeyBtn();
}

static void btn_key_on_clicked( LCUI_Widget *widget, LCUI_WidgetEvent *event )
{
	StartCatchKey( widget );
}

void GameWindow_InitSetKeyboardWindow(void)
{
	window = Widget_New("window");
	box = Widget_New(NULL);
	label = Widget_New("label");
	label_left = Widget_New("label");
	label_right = Widget_New("label");
	label_up = Widget_New("label");
	label_down = Widget_New("label");
	label_jump = Widget_New("label");
	label_defense = Widget_New("label");
	label_a_atk = Widget_New("label");
	label_b_atk = Widget_New("label");
	btn_left = Widget_New("button");
	btn_right = Widget_New("button");
	btn_up = Widget_New("button");
	btn_down = Widget_New("button");
	btn_a_atk = Widget_New("button");
	btn_b_atk = Widget_New("button");
	btn_jump = Widget_New("button");
	btn_defense = Widget_New("button");
	btn_ok = Widget_New("button");
	btn_reset = Widget_New("button");
	
	Widget_Container_Add( box, btn_left );
	Widget_Container_Add( box, btn_right );
	Widget_Container_Add( box, btn_up );
	Widget_Container_Add( box, btn_down );
	Widget_Container_Add( box, btn_jump );
	Widget_Container_Add( box, btn_defense );
	Widget_Container_Add( box, btn_a_atk );
	Widget_Container_Add( box, btn_b_atk );
	Widget_Container_Add( box, label_left );
	Widget_Container_Add( box, label_right );
	Widget_Container_Add( box, label_up );
	Widget_Container_Add( box, label_down );
	Widget_Container_Add( box, label_jump );
	Widget_Container_Add( box, label_defense );
	Widget_Container_Add( box, label_a_atk );
	Widget_Container_Add( box, label_b_atk );
	Window_ClientArea_Add( window, box );
	Window_ClientArea_Add( window, label );
	Window_ClientArea_Add( window, btn_ok );
	Window_ClientArea_Add( window, btn_reset );
	
	Widget_SetAutoSize( btn_left, FALSE, 0 );
	Widget_SetAutoSize( btn_right, FALSE, 0 );
	Widget_SetAutoSize( btn_up, FALSE, 0 );
	Widget_SetAutoSize( btn_down, FALSE, 0 );
	Widget_SetAutoSize( btn_jump, FALSE, 0 );
	Widget_SetAutoSize( btn_a_atk, FALSE, 0 );
	Widget_SetAutoSize( btn_b_atk, FALSE, 0 );
	Widget_SetAutoSize( btn_defense, FALSE, 0 );
	Widget_SetAutoSize( btn_ok, FALSE, 0 );

	Widget_Resize( btn_left, KEYBTN_SIZE );
	Widget_Resize( btn_right, KEYBTN_SIZE );
	Widget_Resize( btn_up, KEYBTN_SIZE );
	Widget_Resize( btn_down, KEYBTN_SIZE );
	Widget_Resize( btn_jump, KEYBTN_SIZE );
	Widget_Resize( btn_a_atk, KEYBTN_SIZE );
	Widget_Resize( btn_b_atk, KEYBTN_SIZE );
	Widget_Resize( btn_defense, KEYBTN_SIZE );
	Widget_Resize( btn_ok, BTN_SIZE );
	Widget_Resize( btn_reset, BTN_SIZE );
	
	Widget_Move( btn_left, Pos(0,50+22) );
	Widget_Move( btn_right, Pos(100,50+22) );
	Widget_Move( btn_up, Pos(50,0+22) );
	Widget_Move( btn_down, Pos(50,50+22) );
	Widget_Move( btn_jump, Pos(200,50+22) );
	Widget_Move( btn_a_atk, Pos(200,0+22) );
	Widget_Move( btn_b_atk, Pos(250,0+22) );
	Widget_Move( btn_defense, Pos(250,50+22) );
	
	Widget_Move( label_left, Pos(10,125) );
	Widget_Move( label_right, Pos(110,125) );
	Widget_Move( label_up, Pos(60,0) );
	Widget_Move( label_down, Pos(60,125) );
	Widget_Move( label_a_atk, Pos(210,0) );
	Widget_Move( label_b_atk, Pos(260,0) );
	Widget_Move( label_jump, Pos(210,125) );
	Widget_Move( label_defense, Pos(260,125) );

	UpdateKeyBtn();

	Label_TextW( label_left, TEXT_LEFT );
	Label_TextW( label_right, TEXT_RIGHT );
	Label_TextW( label_up, TEXT_UP );
	Label_TextW( label_down, TEXT_DOWN );
	Label_TextW( label_jump, TEXT_JUMP );
	Label_TextW( label_defense, TEXT_DEFENSE );
	Label_TextW( label_a_atk, TEXT_A_ATK );
	Label_TextW( label_b_atk, TEXT_B_ATK );

	Widget_SetStyleID( window, WINDOW_STYLE_LINE );
	Widget_Resize( window, WINDOW_SIZE );
	Widget_SetPadding( Window_GetClientArea(window), Padding(10,10,10,10) );

	Label_TextW( label, TEXT_SET_KEYBOARD );
	Widget_SetBackgroundColor( label, RGB(255,255,255) );
	Widget_SetBackgroundTransparent( label, FALSE );

	Button_TextW( btn_ok, TEXT_OK );
	Button_TextW( btn_reset, TEXT_RESET );

	Widget_SetSize( box, "100%", "165px" );
	Widget_SetBorder( box, Border(1,BORDER_STYLE_SOLID,RGB(200,200,200)) );
	Widget_SetPadding( box, Padding(10,10,10,10) );

	Widget_SetAlign( box, ALIGN_TOP_CENTER, Pos(0,12) );
	Widget_SetAlign( label, ALIGN_TOP_CENTER, Pos(0,0) );
	Widget_SetAlign( btn_ok, ALIGN_BOTTOM_CENTER, Pos(-50,-5) );
	Widget_SetAlign( btn_reset, ALIGN_BOTTOM_CENTER, Pos(50,-5) );
	
	Widget_ConnectEvent( btn_ok, EVENT_CLICKED, btn_ok_on_clicked );
	Widget_ConnectEvent( btn_reset, EVENT_CLICKED, btn_reset_on_clicked );
	Widget_ConnectEvent( btn_left, EVENT_CLICKED, btn_key_on_clicked );
	Widget_ConnectEvent( btn_right, EVENT_CLICKED, btn_key_on_clicked );
	Widget_ConnectEvent( btn_up, EVENT_CLICKED, btn_key_on_clicked );
	Widget_ConnectEvent( btn_down, EVENT_CLICKED, btn_key_on_clicked );
	Widget_ConnectEvent( btn_b_atk, EVENT_CLICKED, btn_key_on_clicked );
	Widget_ConnectEvent( btn_a_atk, EVENT_CLICKED, btn_key_on_clicked );
	Widget_ConnectEvent( btn_jump, EVENT_CLICKED, btn_key_on_clicked );
	Widget_ConnectEvent( btn_defense, EVENT_CLICKED, btn_key_on_clicked );
	
	Widget_Show( btn_left );
	Widget_Show( btn_right );
	Widget_Show( btn_up );
	Widget_Show( btn_down );
	Widget_Show( btn_jump );
	Widget_Show( btn_a_atk );
	Widget_Show( btn_b_atk );
	Widget_Show( btn_defense );
	Widget_Show( label_left );
	Widget_Show( label_right );
	Widget_Show( label_up );
	Widget_Show( label_down );
	Widget_Show( label_jump );
	Widget_Show( label_a_atk );
	Widget_Show( label_b_atk );
	Widget_Show( label_defense );
	Widget_Show( btn_ok );
	Widget_Show( btn_reset );
	Widget_Show( box );
	Widget_Show(label );
}

void GameWindow_ShowSetKeyboardWindow(void)
{
	Widget_SetModal( window, TRUE );
	Widget_Show( window );
}

void GameWindow_HideSetKeyboardWindow(void)
{
	Widget_Hide( window );
}

void GameWindow_DestroySetKeyboardWindow(void)
{
	Widget_Destroy( window );
}
