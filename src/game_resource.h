
#ifndef __GAME_RESOURCE_H__
#define __GAME_RESOURCE_H__

#define ACTION_RES_CLASS_RIKI	"graphics::action::riki"
#define ACTION_RES_CLASS_KUNI	"graphics::action::kuni"
#define ACTION_RES_CLASS_MIKE	"graphics::action::mike"
#define ACTION_RES_CLASS_BEN	"graphics::action::ben"
#define ACTION_RES_CLASS_TORAJI	"graphics::action::toraji"
#define MAIN_RES		"graphics::main"
#define SCENES_RES		"graphics::senes"
#define FONT_RES		"graphics::font"

/** 初始化 */
LCUI_API int GameGraphRes_Init( void );

/** 释放全部资源 */
LCUI_API void GameGraphRes_FreeAll( void );

/** 释放指定类名的资源 */
LCUI_API int GameGraphRes_Free( const char *class_name );

/** 添加一个新资源类，返回该资源类的标识号 */
LCUI_API int GameGraphRes_AddClass( const char *class_name );

LCUI_API int GameGraphRes_AddGraph( int class_id, const char *name, LCUI_Graph *graph );

/** 从文件内载入资源 */
LCUI_API int GameGraphRes_LoadFromFile( const char *filepath );

/** 将指定类资源写入至文件 */
LCUI_API int GameGraphRes_WriteToFile( const char *filepath, const char *class_name );

/** 在指定资源类里面获取指定名称的资源 */
LCUI_API int GameGraphRes_GetGraph(	const char *class_name,
					const char *name,
					LCUI_Graph *buff );

#endif