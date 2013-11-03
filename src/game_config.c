#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_INPUT_H

#include "game_control.h"
#include "game_config.h"

#define CONFIG_FILE	"config.bin"

struct GameConfigData {
	LCUI_BOOL windowed;
	ControlKey control_key;
} game_config;

/** 初始化游戏键位设置 */
void GameConfig_InitKeyControl(void)
{
	game_config.control_key.a_attack = LCUIKEY_J;
	game_config.control_key.b_attack = LCUIKEY_K;
	game_config.control_key.defense = LCUIKEY_L;
	game_config.control_key.jump = LCUIKEY_SPACE;
	game_config.control_key.left = LCUIKEY_A;
	game_config.control_key.right = LCUIKEY_D;
	game_config.control_key.up = LCUIKEY_W;
	game_config.control_key.down = LCUIKEY_S;
}

/** 获取键位配置 */
void GameConfig_GetKeyControl( ControlKey *buff )
{
	*buff = game_config.control_key;
}

/** 设置键位配置 */
void GameConfig_SetKeyControl( ControlKey *buff )
{
	game_config.control_key = *buff;
}

/** 初始化游戏配置 */
void GameConfig_Init(void)
{
	game_config.windowed = TRUE;
	GameConfig_InitKeyControl();
}

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
