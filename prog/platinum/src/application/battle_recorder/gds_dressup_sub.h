//==============================================================================
/**
 *
 * GDSドレスアップのサブ構造体や定数定義
 *
 */
//==============================================================================
#ifndef __GT_DRESSUP_SUB_H__
#define __GT_DRESSUP_SUB_H__

#include "gds_define.h"

//==============================================================================
//	定数定義
//==============================================================================
#define GT_IMC_SAVEDATA_NICKNAME_STR_NUM	(11)	///<ニックネームの文字列数
#define GT_IMC_SAVEDATA_OYANAME_STR_NUM 	(8)		///<親ネームの文字列数
#define GT_IMC_SAVEDATA_STRBUF_NUM	  		(12)	///<STRBUFFの必要要素数
#define	GT_IMC_RBOX_OBJ_MAX					(11)	// 右ボックスのオブジェクト登録最大数
													// アクセサリ10 + ポケモン1
#define GT_IMC_SAVEDATA_TELEVISION_ACCE_NUM	( GT_IMC_RBOX_OBJ_MAX - 1 )		///<テレビ局アクセサリ登録数


//==============================================================================
//	構造体定義
//==============================================================================
//-------------------------------------
//	ドレスアップ用ポケモンデータ
//=====================================
typedef struct {
	u32 personal_rnd;				///<ポケモンの個性乱数
	u32 idno;						///<IDNo	レアポケモン用
	u16	monsno;						///<モンスターナンバー
	u16 oyaname[ GT_IMC_SAVEDATA_OYANAME_STR_NUM ];	///<ニックネーム
	s8 poke_pri;					///<ポケモン優先順位
	u8 poke_x;						///<ポケモンｘ座標
	u8 poke_y;						///<ポケモンｙ座標
	u8 form_id;					///<形状アトリビュート
}GT_IMC_SAVEDATA_POKEMON;

//-------------------------------------
//	ドレスアップ用アクセサリデータ構造体
//=====================================
typedef struct {
	u8 accessory_no;	///<アクセサリナンバー
	u8 accessory_x;		///<アクセサリｘ座標
	u8 accessory_y;		///<アクセサリｙ座標
	s8 accessory_pri;	///<アクセサリ表示優先
}GT_IMC_SAVEDATA_ACCESSORIE;



#endif	//__GT_DRESSUP_SUB_H__
