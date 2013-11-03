#ifndef __GAME_CONFIG_H__
#define __GAME_CONFIG_H__

#define CONFIG_FILE	"config.bin"

/** 初始化游戏键位设置 */
void GameConfig_InitKeyControl(void);

/** 获取键位设置 */
void GameConfig_GetKeyControl( ControlKey *buff );

/** 初始化游戏配置 */
void GameConfig_Init(void);

/** 载入游戏配置 */
void GameConfig_Load(void);

/** 保存游戏配置 */
void GameConfig_Save(void);

/** 配置游戏是否以窗口模式运行 */
void GameConfig_SetWindowed( LCUI_BOOL flag );

/** 判断游戏是否要以窗口模式运行 */
LCUI_BOOL GameConfig_IsWindowed(void);

#endif
