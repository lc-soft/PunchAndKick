#ifndef __GAME_STATUSBAR_H__
#define __GAME_STATUSBAR_H__

LCUI_Widget *StatusBar_New(void);

/** 设置玩家名称(最多8个字) */
void StatusBar_SetPlayerNameW( LCUI_Widget *widget, const wchar_t *name );

void StatusBar_SetPlayerName( LCUI_Widget *widget, const char *name );

/** 设置玩家类型名称，最多4个字，如：1P、2P或CPU */
void StatusBar_SetPlayerTypeNameW( LCUI_Widget *widget, const wchar_t *type_name );

void StatusBar_SetPlayerTypeName( LCUI_Widget *widget, const char *type_name );

/** 设置最大生命值 */
void StatusBar_SetMaxHealth( LCUI_Widget *widget, int value );

/** 设置当前生命值 */
void StatusBar_SetHealth( LCUI_Widget *widget, int value );

/** 设置头像 */
void StatusBar_SetAvatar( LCUI_Widget *widget, LCUI_Graph *img );

/** 注册状态栏部件 */
void StatusBar_Register(void);

#endif
