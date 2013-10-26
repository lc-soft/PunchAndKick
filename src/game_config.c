#include <LCUI_Build.h>
#include LC_LCUI_H

#include "game_config.h"

#define CONFIG_FILE	"config.bin"

struct GameConfigData {
	LCUI_BOOL windowed;
} game_config = { TRUE };

/** 载入游戏配置 */
void GameConfig_Load(void)
{
	FILE *fp;
	fp = fopen( CONFIG_FILE, "rb" );
	if( !fp ) {
		return;
	}
	fread( &game_config, sizeof(game_config), 1, fp );
	fclose( fp );
}

/** 保存游戏配置 */
void GameConfig_Save(void)
{
	FILE *fp;
	fp = fopen( CONFIG_FILE, "wb+" );
	if( !fp ) {
		return;
	}
	fwrite( &game_config, sizeof(game_config), 1, fp );
	fclose( fp );
}

/** 配置游戏是否以窗口模式运行 */
void GameConfig_SetWindowed( LCUI_BOOL flag )
{
	game_config.windowed = flag;
}

/** 判断游戏是否要以窗口模式运行 */
LCUI_BOOL GameConfig_IsWindowed(void)
{
	return game_config.windowed;
}
