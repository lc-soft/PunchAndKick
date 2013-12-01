/* ****************************************************************************
 * Punch And Kick -- a simple 2D Fighting Game.
 *
 * Copyright (C) 2013 by
 * Liu Chao
 *
 * This file is part of the Punch And Kick project, and may only be used,
 * modified, and distributed under the terms of the GPLv2.
 *
 * (GPLv2 is abbreviation of GNU General Public License Version 2)
 *
 * By continuing to use, modify, or distribute this file you indicate that you
 * have read the license and understand and accept it fully.
 *
 * The Punch And Kick project is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GPL v2 for
 * more details.
 *
 * You should have received a copy of the GPLv2 along with this file. It is
 * usually in the LICENSE.TXT file, If not, see <http://www.gnu.org/licenses/>.
 * ***************************************************************************/

/* ****************************************************************************
 * Punch And Kick -- 一个普通的2D格斗游戏
 *
 * 版权所有 (C) 2013 归属于
 * 刘超
 *
 * 这个文件是 Punch And Kick 项目的一部分，并且只可以根据GPLv2许可协议来使用、
 * 更改和发布。
 *
 * (GPLv2 是 GNU通用公共许可证第二版 的英文缩写)
 *
 * 继续使用、修改或发布本文件，表明您已经阅读并完全理解和接受这个许可协议。
 *
 * Punch And Kick 项目是基于技术交流目的而加以散布的，但不负任何担保责任，例
 * 如：稳定性、可玩性、趣味性等的担保，甚至没有适销性或特定用途的隐含担保，详
 * 情请参照GPLv2许可协议。
 *
 * 您应已收到附随于本文件的GPLv2许可协议的副本，它通常在LICENSE.TXT文件中，如果
 * 没有，请查看：<http://www.gnu.org/licenses/>.
 * ***************************************************************************/

// 处理游戏角色的攻击伤害

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include "game.h"

#define NAME_MAX_LEN	64

typedef struct AttackRecord_ {
	GamePlayer *attacker;			/**< 攻击者 */
	GamePlayer *victim;			/**< 受害者 */
	char attack_type_name[NAME_MAX_LEN];	/**< 攻击类型 */
	int victim_state;			/**< 受害者的状态 */
} AttackRecord;

typedef struct AttackInfo_ {
	int attack_type_id;				/**< 攻击标识符 */
	char attack_type_name[NAME_MAX_LEN];		/**< 攻击名称 */
	int (*get_damage)(GamePlayer*,GamePlayer*,int);	/**< 获取实际伤害值 */
	void (*effect)(GamePlayer*, GamePlayer*);	/**< 实现攻击带来的效果 */
} AttackInfo;

static LCUI_Queue attack_library;

static AttackInfo *AttackLibrary_GetInfo( const char *attack_type_name )
{
	AttackInfo *p_info;
	int n, attack_type_id;
	attack_type_id = (int)BKDRHash( attack_type_name );
	n = Queue_GetTotal( &attack_library );
	while(n--) {
		p_info = (AttackInfo*)Queue_Get( &attack_library, n );
		if( !p_info ) {
			continue;
		}
		if( p_info->attack_type_id == attack_type_id ) {
			return p_info;
		}
	}
	return FALSE;
}

/**< 添加攻击数据 */
int AttackLibrary_AddAttack(	const char *attack_name, 
				int (*get_damage)(GamePlayer*,GamePlayer*,int),
				void (*effect)(GamePlayer*,GamePlayer*) )
{
	AttackInfo data;
	strncpy( data.attack_type_name, attack_name, NAME_MAX_LEN );
	data.attack_type_name[NAME_MAX_LEN-1] = 0;
	data.attack_type_id = (int)BKDRHash( data.attack_type_name );
	data.get_damage = get_damage;
	data.effect = effect;
	Queue_Add( &attack_library, &data );
	return data.attack_type_id;
}

/** 根据受害者的防御力及其它情况，获取减免后的实际伤害 */
int DamageReduce( GamePlayer *victim, int victim_state, int damage )
{
	double reduce;
	/* 根据受害者的防御力，计算伤害的减免比例 */
	reduce = victim->property.defense + 100.0;
	reduce = 1.0 - 100.0 / reduce;
	/* 根据受害者的状态，增加伤害减免比例 */
	switch( victim_state ) {
	case STATE_SOLID_DEFENSE:
		reduce += 0.40;
		break;
	case STATE_DEFENSE:
	case STATE_TUMMY:
	case STATE_TUMMY_HIT:
	case STATE_LYING:
	case STATE_LYING_HIT:
	case STATE_BE_LIFT_TUMMY:
	case STATE_BE_LIFT_TUMMY_HIT:
	case STATE_BE_LIFT_LYING:
	case STATE_BE_LIFT_LYING_HIT:
		/* 躺着或者趴着，减免20%的伤害 */
		reduce += 0.20;
		break;
	case STATE_F_ROLL:
	case STATE_B_ROLL:
		reduce += 0.10;
		break;
	default:
		break;
	}
	/* 限制最大伤害减免比例为95% */
	if( reduce > 0.95 ) {
		reduce = 0.95;
	}
	return damage - (int)(damage*reduce);
}

/** 记录攻击者对受害者的攻击 */
void Game_RecordAttack( GamePlayer *attacker,
			const char *attack_type_name,
			GamePlayer *victim,
			int victim_state )
{
	AttackRecord data;
	LCUI_Queue *p_atk_record;
	/* 若受害者都没有，则直接退出 */
	if( !victim ) {
		return;
	}
	data.attacker = attacker;
	strncpy( data.attack_type_name, attack_type_name, NAME_MAX_LEN );
	data.attack_type_name[NAME_MAX_LEN-1] = 0;
	data.victim = victim;
	data.victim_state = victim_state;
	/* 获取受害者所在对战中的攻击记录 */
	p_atk_record = GameBattle_GetAttackRecord( victim->battle_id );
	Queue_Lock( p_atk_record );
	Queue_Add( p_atk_record, &data );
	Queue_Unlock( p_atk_record );
}

/** 处理已经记录的攻击 */
void Game_ProcAttack( LCUI_Queue *p_attakc_record, ValueTipData *value_tip )
{
	int n, true_damage;
	AttackRecord *p_data;
	AttackInfo *p_info;
	LCUI_Pos start_pos;

	Queue_Lock( p_attakc_record );
	n = Queue_GetTotal( p_attakc_record );
	for( ; n>=0; --n) {
		p_data = (AttackRecord*)Queue_Get( p_attakc_record, 0 );
		if( !p_data || !p_data->victim ) {
			continue;
		}

		/* 获取该类型攻击的信息 */
		p_info = AttackLibrary_GetInfo( p_data->attack_type_name );
		if( !p_info ) {
			Queue_Delete( p_attakc_record, 0 );
			continue;
		}
		if( p_info->effect ) {
			/* 实现攻击产生的效果 */
			p_info->effect( p_data->attacker, p_data->victim );
		}
		if( p_info->get_damage ) {
			/* 计算真实伤害 */
			true_damage = p_info->get_damage( p_data->attacker, p_data->victim, p_data->victim_state );
			DEBUG_MSG("attack type: %s, damage: %d\n", p_data->attack_type_name, true_damage);
		} else {
			true_damage = 0;
		}
		/* 如果该受害者是无敌的，则不扣血 */
		if( p_data->victim->is_invincible ) {
			Queue_Delete( p_attakc_record, 0 );
			continue;
		}
		/** 如果已经初始化数值提示功能 */
		if( value_tip->is_inited ) {
			/* 获取受害者脚底的坐标（相对于战斗场景的二维坐标） */
			GameObject_GetFootPos( p_data->victim->object, &start_pos );
			/* 减去当前帧的高度 */
			start_pos.y -= GameObject_GetCurrentFrameTop( p_data->victim->object );
			/* 再减去一点，确保该数值提示显示在受害者头上 */
			start_pos.y -= 20;
			/* 添加数值提示 */
			GameValueTip_AddTip( value_tip, p_data->victim->id, start_pos, true_damage );
		}
		/* 计算现在的血量 */
		p_data->victim->property.cur_hp -= true_damage;
		if( p_data->victim->property.cur_hp < 0 ) {
			p_data->victim->property.cur_hp = 0;
		}
		/* 设置状态栏上显示的血量 */
		StatusBar_SetHealth(	p_data->victim->statusbar,
					p_data->victim->property.cur_hp );
			/* 删除该攻击记录 */
		Queue_Delete( p_attakc_record, 0 );
	}
	Queue_Unlock( p_attakc_record );
}

/** 初始化攻击信息库 */
void Game_InitAttackLibrary(void)
{
	Queue_Init( &attack_library, sizeof(AttackInfo), NULL );
}

/** 初始化攻击记录 */
void Game_InitAttackRecord( LCUI_Queue *p_attakc_record )
{
	Queue_Init( p_attakc_record, sizeof(AttackRecord), NULL );
}

void Game_DestroyAttackRecord(void)
{

}