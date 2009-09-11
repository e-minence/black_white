//============================================================================================
/**
 * @file	b_plist_bmp.c
 * @brief	戦闘用ポケモンリスト画面BMP関連
 * @author	Hiroyuki Nakamura
 * @date	05.02.01
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "message.naix"
#include "item/item.h"
#include "gamesystem/msgspeed.h"
#include "waza_tool/waza_tool.h"
#include "system/bmp_winframe.h"

/*↑[GS_CONVERT_TAG]*/
//#include "system/procsys.h"
/*↑[GS_CONVERT_TAG]*/
/*
#include "system/palanm.h"
#include "system/window.h"
#include "system/fontproc.h"
#include "system/msgdata.h"
#include "system/numfont.h"
#include "system/wordset.h"
#include "system/pmfprint.h"
#include "system/buflen.h"
*/
//#include "system/clact_tool.h"
/*↑[GS_CONVERT_TAG]*/
/*
#include "msgdata/msg.naix"
#include "msgdata/msg_b_plist.h"
#include "battle/battle_common.h"
#include "battle/fight_tool.h"
#include "battle/wazano_def.h"
#include "poketool/pokeparty.h"
#include "poketool/waza_tool.h"
#include "contest/contest.h"
#include "contest/con_tool.h"
#include "itemtool/item.h"
#include "application/app_tool.h"
#include "application/p_status.h"
#include "b_app_tool.h"
*/

#include "msg/msg_b_plist.h"

#include "b_plist.h"
#include "b_plist_bmp_def.h"
#include "b_plist_main.h"
#include "b_plist_bmp.h"


//============================================================================================
//	定数定義
//============================================================================================
#define	PCOL_N_BLACK	( PRINTSYS_LSB_Make( 1, 2, 0 ) )		// フォントカラー：黒
#define	PCOL_N_WHITE	( PRINTSYS_LSB_Make( 15, 14, 0 ) )		// フォントカラー：白
#define	PCOL_N_BLUE		( PRINTSYS_LSB_Make( 7, 8, 0 ) )		// フォントカラー：青
#define	PCOL_N_RED		( PRINTSYS_LSB_Make( 3, 4, 0 ) )		// フォントカラー：赤

#define	PCOL_BTN		( PRINTSYS_LSB_Make( 7, 8, 9 ) )		// フォントカラー：ボタン
#define	PCOL_B_WHITE	( PRINTSYS_LSB_Make( 15, 14, 0 ) )		// フォントカラー：ボタン用白
#define	PCOL_B_BLUE		( PRINTSYS_LSB_Make( 10, 11, 0 ) )		// フォントカラー：ボタン用青
#define	PCOL_B_RED		( PRINTSYS_LSB_Make( 12, 13, 0 ) )		// フォントカラー：ボタン用赤

#define	HP_GAGE_COL_G1	( 1 )	// HPゲージカラー緑１
#define	HP_GAGE_COL_G2	( 2 )	// HPゲージカラー緑２
#define	HP_GAGE_COL_Y1	( 3 )	// HPゲージカラー黄１
#define	HP_GAGE_COL_Y2	( 4 )	// HPゲージカラー黄２
#define	HP_GAGE_COL_R1	( 5 )	// HPゲージカラー赤１
#define	HP_GAGE_COL_R2	( 6 )	// HPゲージカラー赤２


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static void BPL_Page1BmpWrite( BPLIST_WORK * wk );
static void BPL_ChgPageBmpWrite( BPLIST_WORK * wk );
static void BPL_StMainPageBmpWrite( BPLIST_WORK * wk );
static void BPL_StWazaSelPageBmpWrite( BPLIST_WORK * wk );
static void BPL_StWazaInfoPageBmpWrite( BPLIST_WORK * wk );
static void BPL_PPRcvPageBmpWrite( BPLIST_WORK * wk );
static void BPL_WazaDelSelPageBmpWrite( BPLIST_WORK * wk );
static void BPL_Page6BmpWrite( BPLIST_WORK * wk );
static void BPL_Page8BmpWrite( BPLIST_WORK * wk );


//============================================================================================
//	グローバル変数
//============================================================================================
// 共通デフォルトウィンドウデータ
static const u8 CommBmpData[][6] =
{
	{	// コメント表示
		GFL_BG_FRAME0_S, WIN_P1_COMMENT_PX, WIN_P1_COMMENT_PY,
		WIN_P1_COMMENT_SX, WIN_P1_COMMENT_SY, WIN_P1_COMMENT_PAL
	},
	{	// メッセージ表示
		GFL_BG_FRAME0_S, WIN_TALK_PX, WIN_TALK_PY,
		WIN_TALK_SX, WIN_TALK_SY, WIN_TALK_PAL
	}
};

// ページ１のBMPウィンドウデータ
static const u8 Page1_BmpData[][6] =
{
	{	// ポケモン１
		GFL_BG_FRAME1_S, WIN_P1_POKE1_PX, WIN_P1_POKE1_PY,
		WIN_P1_POKE1_SX, WIN_P1_POKE1_SY, WIN_P1_POKE1_PAL
	},
	{	// ポケモン２
		GFL_BG_FRAME1_S, WIN_P1_POKE2_PX, WIN_P1_POKE2_PY,
		WIN_P1_POKE2_SX, WIN_P1_POKE2_SY, WIN_P1_POKE2_PAL
	},
	{	// ポケモン３
		GFL_BG_FRAME1_S, WIN_P1_POKE3_PX, WIN_P1_POKE3_PY,
		WIN_P1_POKE3_SX, WIN_P1_POKE3_SY, WIN_P1_POKE3_PAL
	},
	{	// ポケモン４
		GFL_BG_FRAME1_S, WIN_P1_POKE4_PX, WIN_P1_POKE4_PY,
		WIN_P1_POKE4_SX, WIN_P1_POKE4_SY, WIN_P1_POKE4_PAL
	},
	{	// ポケモン５
		GFL_BG_FRAME1_S, WIN_P1_POKE5_PX, WIN_P1_POKE5_PY,
		WIN_P1_POKE5_SX, WIN_P1_POKE5_SY, WIN_P1_POKE5_PAL
	},
	{	// ポケモン６
		GFL_BG_FRAME1_S, WIN_P1_POKE6_PX, WIN_P1_POKE6_PY,
		WIN_P1_POKE6_SX, WIN_P1_POKE6_SY, WIN_P1_POKE6_PAL
	},
};

// 入れ替えページのBMPウィンドウデータ
static const u8 ChgPage_BmpData[][6] =
{
	{	// 名前
		GFL_BG_FRAME1_S, WIN_CHG_NAME_PX, WIN_CHG_NAME_PY,
		WIN_CHG_NAME_SX, WIN_CHG_NAME_SY, WIN_CHG_NAME_PAL
	},
	{	// 「いれかえる」
		GFL_BG_FRAME1_S, WIN_CHG_IREKAE_PX, WIN_CHG_IREKAE_PY,
		WIN_CHG_IREKAE_SX, WIN_CHG_IREKAE_SY, WIN_CHG_IREKAE_PAL
	},
	{	// 「つよさをみる」
		GFL_BG_FRAME1_S, WIN_CHG_STATUS_PX, WIN_CHG_STATUS_PY,
		WIN_CHG_STATUS_SX, WIN_CHG_STATUS_SY, WIN_CHG_STATUS_PAL
	},
	{	// 「わざをみる」
		GFL_BG_FRAME1_S, WIN_CHG_WAZACHECK_PX, WIN_CHG_WAZACHECK_PY,
		WIN_CHG_WAZACHECK_SX, WIN_CHG_WAZACHECK_SY, WIN_CHG_WAZACHECK_PAL
	}
};

// ステータス技選択ページのBMPウィンドウデータ
static const u8 StWazaSelPage_BmpData[][6] =
{
	{	// 名前
		GFL_BG_FRAME1_S, WIN_ST_NAME_PX, WIN_ST_NAME_PY,
		WIN_ST_NAME_SX, WIN_ST_NAME_SY, WIN_ST_NAME_PAL
	},
	{	// 技１
		GFL_BG_FRAME1_S, WIN_STW_SKILL1_PX, WIN_STW_SKILL1_PY,
		WIN_STW_SKILL1_SX, WIN_STW_SKILL1_SY, WIN_STW_SKILL1_PAL
	},
	{	// 技２
		GFL_BG_FRAME1_S, WIN_STW_SKILL2_PX, WIN_STW_SKILL2_PY,
		WIN_STW_SKILL2_SX, WIN_STW_SKILL2_SY, WIN_STW_SKILL2_PAL
	},
	{	// 技３
		GFL_BG_FRAME1_S, WIN_STW_SKILL3_PX, WIN_STW_SKILL3_PY,
		WIN_STW_SKILL3_SX, WIN_STW_SKILL3_SY, WIN_STW_SKILL3_PAL
	},
	{	// 技４
		GFL_BG_FRAME1_S, WIN_STW_SKILL4_PX, WIN_STW_SKILL4_PY,
		WIN_STW_SKILL4_SX, WIN_STW_SKILL4_SY, WIN_STW_SKILL4_PAL
	},

	{	// 「つよさをみる」
		GFL_BG_FRAME1_S, WIN_STW_STATUS_PX, WIN_STW_STATUS_PY,
		WIN_STW_STATUS_SX, WIN_STW_STATUS_SY, WIN_STW_STATUS_PAL
	},

	{	// 名前（スワップ）
		GFL_BG_FRAME1_S, WIN_ST_NAME_PX, WIN_ST_NAME_PY,
		WIN_ST_NAME_SX, WIN_ST_NAME_SY, WIN_ST_NAME_PAL
	},
	{	// 技１（スワップ）
		GFL_BG_FRAME1_S, WIN_STW_SKILL1_PX, WIN_STW_SKILL1_PY,
		WIN_STW_SKILL1_SX, WIN_STW_SKILL1_SY, WIN_STW_SKILL1_PAL
	},
	{	// 技２（スワップ）
		GFL_BG_FRAME1_S, WIN_STW_SKILL2_PX, WIN_STW_SKILL2_PY,
		WIN_STW_SKILL2_SX, WIN_STW_SKILL2_SY, WIN_STW_SKILL2_PAL
	},
	{	// 技３（スワップ）
		GFL_BG_FRAME1_S, WIN_STW_SKILL3_PX, WIN_STW_SKILL3_PY,
		WIN_STW_SKILL3_SX, WIN_STW_SKILL3_SY, WIN_STW_SKILL3_PAL
	},
	{	// 技４（スワップ）
		GFL_BG_FRAME1_S, WIN_STW_SKILL4_PX, WIN_STW_SKILL4_PY,
		WIN_STW_SKILL4_SX, WIN_STW_SKILL4_SY, WIN_STW_SKILL4_PAL
	},
};

// ステータスメインページのBMPウィンドウデータ
static const u8 StMainPage_BmpData[][6] =
{
	{	// 名前
		GFL_BG_FRAME1_S, WIN_ST_NAME_PX, WIN_ST_NAME_PY,
		WIN_ST_NAME_SX, WIN_ST_NAME_SY, WIN_ST_NAME_PAL
	},
	{	// 特性名
		GFL_BG_FRAME1_S, WIN_STM_SPANAME_PX, WIN_STM_SPANAME_PY,
		WIN_STM_SPANAME_SX, WIN_STM_SPANAME_SY, WIN_STM_SPANAME_PAL
	},
	{	// 特性説明
		GFL_BG_FRAME1_S, WIN_STM_SPAINFO_PX, WIN_STM_SPAINFO_PY,
		WIN_STM_SPAINFO_SX, WIN_STM_SPAINFO_SY, WIN_STM_SPAINFO_PAL
	},
	{	// 道具名
		GFL_BG_FRAME1_S, WIN_STM_ITEMNAME_PX, WIN_STM_ITEMNAME_PY,
		WIN_STM_ITEMNAME_SX, WIN_STM_ITEMNAME_SY, WIN_STM_ITEMNAME_PAL
	},
	{	// HP値
		GFL_BG_FRAME1_S, WIN_STM_HPNUM_PX, WIN_STM_HPNUM_PY,
		WIN_STM_HPNUM_SX, WIN_STM_HPNUM_SY, WIN_STM_HPNUM_PAL
	},
	{	// 攻撃値
		GFL_BG_FRAME1_S, WIN_STM_POWNUM_PX, WIN_STM_POWNUM_PY,
		WIN_STM_POWNUM_SX, WIN_STM_POWNUM_SY, WIN_STM_POWNUM_PAL
	},
	{	// 防御値
		GFL_BG_FRAME1_S, WIN_STM_DEFNUM_PX, WIN_STM_DEFNUM_PY,
		WIN_STM_DEFNUM_SX, WIN_STM_DEFNUM_SY, WIN_STM_DEFNUM_PAL
	},
	{	// 素早さ値
		GFL_BG_FRAME1_S, WIN_STM_AGINUM_PX, WIN_STM_AGINUM_PY,
		WIN_STM_AGINUM_SX, WIN_STM_AGINUM_SY, WIN_STM_AGINUM_PAL
	},
	{	// 特攻値
		GFL_BG_FRAME1_S, WIN_STM_SPPNUM_PX, WIN_STM_SPPNUM_PY,
		WIN_STM_SPPNUM_SX, WIN_STM_SPPNUM_SY, WIN_STM_SPPNUM_PAL
	},
	{	// 特防値
		GFL_BG_FRAME1_S, WIN_STM_SPDNUM_PX, WIN_STM_SPDNUM_PY,
		WIN_STM_SPDNUM_SX, WIN_STM_SPDNUM_SY, WIN_STM_SPDNUM_PAL
	},
	{	// HPゲージ
		GFL_BG_FRAME1_S, WIN_STM_HPGAGE_PX, WIN_STM_HPGAGE_PY,
		WIN_STM_HPGAGE_SX, WIN_STM_HPGAGE_SY, WIN_STM_HPGAGE_PAL
	},
	{	// レベル値
		GFL_BG_FRAME1_S, WIN_STM_LVNUM_PX, WIN_STM_LVNUM_PY,
		WIN_STM_LVNUM_SX, WIN_STM_LVNUM_SY, WIN_STM_LVNUM_PAL
	},
	{	// 次のレベル値
		GFL_BG_FRAME1_S, WIN_STM_NEXTNUM_PX, WIN_STM_NEXTNUM_PY,
		WIN_STM_NEXTNUM_SX, WIN_STM_NEXTNUM_SY, WIN_STM_NEXTNUM_PAL
	},

	{	// 「HP」
		GFL_BG_FRAME1_S, WIN_STM_HP_PX, WIN_STM_HP_PY,
		WIN_STM_HP_SX, WIN_STM_HP_SY, WIN_STM_HP_PAL
	},
	{	// 「こうげき」
		GFL_BG_FRAME1_S, WIN_STM_POW_PX, WIN_STM_POW_PY,
		WIN_STM_POW_SX, WIN_STM_POW_SY, WIN_STM_POW_PAL
	},
	{	// 「ぼうぎょ」
		GFL_BG_FRAME1_S, WIN_STM_DEF_PX, WIN_STM_DEF_PY,
		WIN_STM_DEF_SX, WIN_STM_DEF_SY, WIN_STM_DEF_PAL
	},
	{	// 「すばやさ」
		GFL_BG_FRAME1_S, WIN_STM_AGI_PX, WIN_STM_AGI_PY,
		WIN_STM_AGI_SX, WIN_STM_AGI_SY, WIN_STM_AGI_PAL
	},
	{	// 「とくこう」
		GFL_BG_FRAME1_S, WIN_STM_SPP_PX, WIN_STM_SPP_PY,
		WIN_STM_SPP_SX, WIN_STM_SPP_SY, WIN_STM_SPP_PAL
	},
	{	// 「とくぼう」
		GFL_BG_FRAME1_S, WIN_STM_SPD_PX, WIN_STM_SPD_PY,
		WIN_STM_SPD_SX, WIN_STM_SPD_SY, WIN_STM_SPD_PAL
	},
	{	// 「Lv.」
		GFL_BG_FRAME1_S, WIN_STM_LV_PX, WIN_STM_LV_PY,
		WIN_STM_LV_SX, WIN_STM_LV_SY, WIN_STM_LV_PAL
	},
	{	// 「つぎのレベルまで」
		GFL_BG_FRAME1_S, WIN_STM_NEXT_PX, WIN_STM_NEXT_PY,
		WIN_STM_NEXT_SX, WIN_STM_NEXT_SY, WIN_STM_NEXT_PAL
	},
	{	// 「わざをみる」
		GFL_BG_FRAME1_S, WIN_STM_WAZACHECK_PX, WIN_STM_WAZACHECK_PY,
		WIN_STM_WAZACHECK_SX, WIN_STM_WAZACHECK_SY, WIN_STM_WAZACHECK_PAL
	},

	{	// 名前（スワップ）
		GFL_BG_FRAME1_S, WIN_ST_NAME_PX, WIN_ST_NAME_PY,
		WIN_ST_NAME_SX, WIN_ST_NAME_SY, WIN_ST_NAME_PAL
	},
	{	// 特性名（スワップ）
		GFL_BG_FRAME1_S, WIN_STM_SPANAME_PX, WIN_STM_SPANAME_PY,
		WIN_STM_SPANAME_SX, WIN_STM_SPANAME_SY, WIN_STM_SPANAME_PAL
	},
	{	// 特性説明（スワップ）
		GFL_BG_FRAME1_S, WIN_STM_SPAINFO_PX, WIN_STM_SPAINFO_PY,
		WIN_STM_SPAINFO_SX, WIN_STM_SPAINFO_SY, WIN_STM_SPAINFO_PAL
	},
	{	// 道具名（スワップ）
		GFL_BG_FRAME1_S, WIN_STM_ITEMNAME_PX, WIN_STM_ITEMNAME_PY,
		WIN_STM_ITEMNAME_SX, WIN_STM_ITEMNAME_SY, WIN_STM_ITEMNAME_PAL
	},
	{	// HP値（スワップ）
		GFL_BG_FRAME1_S, WIN_STM_HPNUM_PX, WIN_STM_HPNUM_PY,
		WIN_STM_HPNUM_SX, WIN_STM_HPNUM_SY, WIN_STM_HPNUM_PAL
	},
	{	// 攻撃値（スワップ）
		GFL_BG_FRAME1_S, WIN_STM_POWNUM_PX, WIN_STM_POWNUM_PY,
		WIN_STM_POWNUM_SX, WIN_STM_POWNUM_SY, WIN_STM_POWNUM_PAL
	},
	{	// 防御値（スワップ）
		GFL_BG_FRAME1_S, WIN_STM_DEFNUM_PX, WIN_STM_DEFNUM_PY,
		WIN_STM_DEFNUM_SX, WIN_STM_DEFNUM_SY, WIN_STM_DEFNUM_PAL
	},
	{	// 素早さ値（スワップ）
		GFL_BG_FRAME1_S, WIN_STM_AGINUM_PX, WIN_STM_AGINUM_PY,
		WIN_STM_AGINUM_SX, WIN_STM_AGINUM_SY, WIN_STM_AGINUM_PAL
	},
	{	// 特攻値（スワップ）
		GFL_BG_FRAME1_S, WIN_STM_SPPNUM_PX, WIN_STM_SPPNUM_PY,
		WIN_STM_SPPNUM_SX, WIN_STM_SPPNUM_SY, WIN_STM_SPPNUM_PAL
	},
	{	// 特防値（スワップ）
		GFL_BG_FRAME1_S, WIN_STM_SPDNUM_PX, WIN_STM_SPDNUM_PY,
		WIN_STM_SPDNUM_SX, WIN_STM_SPDNUM_SY, WIN_STM_SPDNUM_PAL
	},
	{	// HPゲージ（スワップ）
		GFL_BG_FRAME1_S, WIN_STM_HPGAGE_PX, WIN_STM_HPGAGE_PY,
		WIN_STM_HPGAGE_SX, WIN_STM_HPGAGE_SY, WIN_STM_HPGAGE_PAL
	},
	{	// レベル値（スワップ）
		GFL_BG_FRAME1_S, WIN_STM_LVNUM_PX, WIN_STM_LVNUM_PY,
		WIN_STM_LVNUM_SX, WIN_STM_LVNUM_SY, WIN_STM_LVNUM_PAL
	},
	{	// 次のレベル値（スワップ）
		GFL_BG_FRAME1_S, WIN_STM_NEXTNUM_PX, WIN_STM_NEXTNUM_PY,
		WIN_STM_NEXTNUM_SX, WIN_STM_NEXTNUM_SY, WIN_STM_NEXTNUM_PAL
	}
};

// ステータス技詳細ページのBMPウィンドウデータ
static const u8 StWazaInfoPage_BmpData[][6] =
{
	{	// 技名
		GFL_BG_FRAME0_S, WIN_P4_SKILL_PX, WIN_P4_SKILL_PY,
		WIN_P4_SKILL_SX, WIN_P4_SKILL_SY, WIN_P4_SKILL_PAL
	},
	{	// PP/PP
		GFL_BG_FRAME0_S, WIN_P4_PPNUM_PX, WIN_P4_PPNUM_PY,
		WIN_P4_PPNUM_SX, WIN_P4_PPNUM_SY, WIN_P4_PPNUM_PAL
	},
	{	// 命中値
		GFL_BG_FRAME0_S, WIN_P4_HITNUM_PX, WIN_P4_HITNUM_PY,
		WIN_P4_HITNUM_SX, WIN_P4_HITNUM_SY, WIN_P4_HITNUM_PAL
	},
	{	// 威力値
		GFL_BG_FRAME0_S, WIN_P4_POWNUM_PX, WIN_P4_POWNUM_PY,
		WIN_P4_POWNUM_SX, WIN_P4_POWNUM_SY, WIN_P4_POWNUM_PAL
	},
	{	// 技説明
		GFL_BG_FRAME0_S, WIN_P4_INFO_PX, WIN_P4_INFO_PY,
		WIN_P4_INFO_SX, WIN_P4_INFO_SY, WIN_P4_INFO_PAL
	},
	{	// 分類名
		GFL_BG_FRAME0_S, WIN_P4_BRNAME_PX, WIN_P4_BRNAME_PY,
		WIN_P4_BRNAME_SX, WIN_P4_BRNAME_SY, WIN_P4_BRNAME_PAL
	},

	{	// 名前
		GFL_BG_FRAME0_S, WIN_P4_NAME_PX, WIN_P4_NAME_PY,
		WIN_P4_NAME_SX, WIN_P4_NAME_SY, WIN_P4_NAME_PAL
	},
	{	// PP
		GFL_BG_FRAME0_S, WIN_P4_PP_PX, WIN_P4_PP_PY,
		WIN_P4_PP_SX, WIN_P4_PP_SY, WIN_P4_PP_PAL
	},
	{	// 「めいちゅう」
		GFL_BG_FRAME0_S, WIN_P4_HIT_PX, WIN_P4_HIT_PY,
		WIN_P4_HIT_SX, WIN_P4_HIT_SY, WIN_P4_HIT_PAL
	},
	{	// 「いりょく」
		GFL_BG_FRAME0_S, WIN_P4_POW_PX, WIN_P4_POW_PY,
		WIN_P4_POW_SX, WIN_P4_POW_SY, WIN_P4_POW_PAL
	},
	{	// 「ぶんるい」
		GFL_BG_FRAME0_S, WIN_P4_BUNRUI_PX, WIN_P4_BUNRUI_PY,
		WIN_P4_BUNRUI_SX, WIN_P4_BUNRUI_SY, WIN_P4_BUNRUI_PAL
	},

	{	// 技名（スワップ）
		GFL_BG_FRAME0_S, WIN_P4_SKILL_PX, WIN_P4_SKILL_PY,
		WIN_P4_SKILL_SX, WIN_P4_SKILL_SY, WIN_P4_SKILL_PAL
	},
	{	// PP/PP（スワップ）
		GFL_BG_FRAME0_S, WIN_P4_PPNUM_PX, WIN_P4_PPNUM_PY,
		WIN_P4_PPNUM_SX, WIN_P4_PPNUM_SY, WIN_P4_PPNUM_PAL
	},
	{	// 命中値（スワップ）
		GFL_BG_FRAME0_S, WIN_P4_HITNUM_PX, WIN_P4_HITNUM_PY,
		WIN_P4_HITNUM_SX, WIN_P4_HITNUM_SY, WIN_P4_HITNUM_PAL
	},
	{	// 威力値（スワップ）
		GFL_BG_FRAME0_S, WIN_P4_POWNUM_PX, WIN_P4_POWNUM_PY,
		WIN_P4_POWNUM_SX, WIN_P4_POWNUM_SY, WIN_P4_POWNUM_PAL
	},
	{	// 技説明（スワップ）
		GFL_BG_FRAME0_S, WIN_P4_INFO_PX, WIN_P4_INFO_PY,
		WIN_P4_INFO_SX, WIN_P4_INFO_SY, WIN_P4_INFO_PAL
	},
	{	// 分類名（スワップ）
		GFL_BG_FRAME0_S, WIN_P4_BRNAME_PX, WIN_P4_BRNAME_PY,
		WIN_P4_BRNAME_SX, WIN_P4_BRNAME_SY, WIN_P4_BRNAME_PAL
	},
};

// ページ５のBMPウィンドウデータ
static const u8 Page5_BmpData[][6] =
{
	{	// 名前
		GFL_BG_FRAME0_S, WIN_P5_NAME_PX, WIN_P5_NAME_PY,
		WIN_P5_NAME_SX, WIN_P5_NAME_SY, WIN_P5_NAME_PAL
	},
	{	// 技１
		GFL_BG_FRAME0_S, WIN_P5_SKILL1_PX, WIN_P5_SKILL1_PY,
		WIN_P5_SKILL1_SX, WIN_P5_SKILL1_SY, WIN_P5_SKILL1_PAL
	},
	{	// 技２
		GFL_BG_FRAME0_S, WIN_P5_SKILL2_PX, WIN_P5_SKILL2_PY,
		WIN_P5_SKILL2_SX, WIN_P5_SKILL2_SY, WIN_P5_SKILL2_PAL
	},
	{	// 技３
		GFL_BG_FRAME0_S, WIN_P5_SKILL3_PX, WIN_P5_SKILL3_PY,
		WIN_P5_SKILL3_SX, WIN_P5_SKILL3_SY, WIN_P5_SKILL3_PAL
	},
	{	// 技４
		GFL_BG_FRAME0_S, WIN_P5_SKILL4_PX, WIN_P5_SKILL4_PY,
		WIN_P5_SKILL4_SX, WIN_P5_SKILL4_SY, WIN_P5_SKILL4_PAL
	},
	{	// 技５
		GFL_BG_FRAME0_S, WIN_P5_SKILL5_PX, WIN_P5_SKILL5_PY,
		WIN_P5_SKILL5_SX, WIN_P5_SKILL5_SY, WIN_P5_SKILL5_PAL
	},
};

// ページ６のBMPウィンドウデータ
static const u8 Page6_BmpData[][6] =
{
	{	// 名前
		GFL_BG_FRAME1_S, WIN_P6_NAME_PX, WIN_P6_NAME_PY,
		WIN_P6_NAME_SX, WIN_P6_NAME_SY, WIN_P6_NAME_PAL
	},
	{	// 技名
		GFL_BG_FRAME1_S, WIN_P6_SKILL_PX, WIN_P6_SKILL_PY,
		WIN_P6_SKILL_SX, WIN_P6_SKILL_SY, WIN_P6_SKILL_PAL
	},
	{	// PP
		GFL_BG_FRAME1_S, WIN_P6_PP_PX, WIN_P6_PP_PY,
		WIN_P6_PP_SX, WIN_P6_PP_SY, WIN_P6_PP_PAL
	},
	{	// PP/PP
		GFL_BG_FRAME1_S, WIN_P6_PPNUM_PX, WIN_P6_PPNUM_PY,
		WIN_P6_PPNUM_SX, WIN_P6_PPNUM_SY, WIN_P6_PPNUM_PAL
	},
	{	// 「めいちゅう」
		GFL_BG_FRAME1_S, WIN_P6_HIT_PX, WIN_P6_HIT_PY,
		WIN_P6_HIT_SX, WIN_P6_HIT_SY, WIN_P6_HIT_PAL
	},
	{	// 「いりょく」
		GFL_BG_FRAME1_S, WIN_P6_POW_PX, WIN_P6_POW_PY,
		WIN_P6_POW_SX, WIN_P6_POW_SY, WIN_P6_POW_PAL
	},
	{	// 命中値
		GFL_BG_FRAME1_S, WIN_P6_HITNUM_PX, WIN_P6_HITNUM_PY,
		WIN_P6_HITNUM_SX, WIN_P6_HITNUM_SY, WIN_P6_HITNUM_PAL
	},
	{	// 威力値
		GFL_BG_FRAME1_S, WIN_P6_POWNUM_PX, WIN_P6_POWNUM_PY,
		WIN_P6_POWNUM_SX, WIN_P6_POWNUM_SY, WIN_P6_POWNUM_PAL
	},
	{	// 技説明
		GFL_BG_FRAME1_S, WIN_P6_INFO_PX, WIN_P6_INFO_PY,
		WIN_P6_INFO_SX, WIN_P6_INFO_SY, WIN_P6_INFO_PAL
	},
	{	// 「ぶんるい」
		GFL_BG_FRAME1_S, WIN_P6_BUNRUI_PX, WIN_P6_BUNRUI_PY,
		WIN_P6_BUNRUI_SX, WIN_P6_BUNRUI_SY, WIN_P6_BUNRUI_PAL
	},
	{	// 分類名
		GFL_BG_FRAME1_S, WIN_P6_BRNAME_PX, WIN_P6_BRNAME_PY,
		WIN_P6_BRNAME_SX, WIN_P6_BRNAME_SY, WIN_P6_BRNAME_PAL
	},
	{	// 「わすれる」
		GFL_BG_FRAME1_S, WIN_P6_WASURERU_PX, WIN_P6_WASURERU_PY,
		WIN_P6_WASURERU_SX, WIN_P6_WASURERU_SY, WIN_P6_WASURERU_PAL
	}
};

// 技回復ページのBMPウィンドウデータ
static const u8 PPRcvPage_BmpData[][6] =
{
	{	// 名前
		GFL_BG_FRAME0_S, WIN_P7_NAME_PX, WIN_P7_NAME_PY,
		WIN_P7_NAME_SX, WIN_P7_NAME_SY, WIN_P7_NAME_PAL
	},
	{	// 技１
		GFL_BG_FRAME0_S, WIN_P7_SKILL1_PX, WIN_P7_SKILL1_PY,
		WIN_P7_SKILL1_SX, WIN_P7_SKILL1_SY, WIN_P7_SKILL1_PAL
	},
	{	// 技２
		GFL_BG_FRAME0_S, WIN_P7_SKILL2_PX, WIN_P7_SKILL2_PY,
		WIN_P7_SKILL2_SX, WIN_P7_SKILL2_SY, WIN_P7_SKILL2_PAL
	},
	{	// 技３
		GFL_BG_FRAME0_S, WIN_P7_SKILL3_PX, WIN_P7_SKILL3_PY,
		WIN_P7_SKILL3_SX, WIN_P7_SKILL3_SY, WIN_P7_SKILL3_PAL
	},
	{	// 技４
		GFL_BG_FRAME0_S, WIN_P7_SKILL4_PX, WIN_P7_SKILL4_PY,
		WIN_P7_SKILL4_SX, WIN_P7_SKILL4_SY, WIN_P7_SKILL4_PAL
	},
};

// ページ８のBMPウィンドウデータ
static const u8 Page8_BmpData[][6] =
{
	{	// 名前
		GFL_BG_FRAME1_S, WIN_P8_NAME_PX, WIN_P8_NAME_PY,
		WIN_P8_NAME_SX, WIN_P8_NAME_SY, WIN_P8_NAME_PAL
	},
	{	// 技名
		GFL_BG_FRAME1_S, WIN_P8_SKILL_PX, WIN_P8_SKILL_PY,
		WIN_P8_SKILL_SX, WIN_P8_SKILL_SY, WIN_P8_SKILL_PAL
	},
	{	// PP
		GFL_BG_FRAME1_S, WIN_P8_PP_PX, WIN_P8_PP_PY,
		WIN_P8_PP_SX, WIN_P8_PP_SY, WIN_P8_PP_PAL
	},
	{	// PP/PP
		GFL_BG_FRAME1_S, WIN_P8_PPNUM_PX, WIN_P8_PPNUM_PY,
		WIN_P8_PPNUM_SX, WIN_P8_PPNUM_SY, WIN_P8_PPNUM_PAL
	},
	{	// 「アピールポイント」
		GFL_BG_FRAME1_S, WIN_P8_APP_PX, WIN_P8_APP_PY,
		WIN_P8_APP_SX, WIN_P8_APP_SY, WIN_P8_APP_PAL
	},
	{	// 技説明
		GFL_BG_FRAME1_S, WIN_P8_INFO_PX, WIN_P8_INFO_PY,
		WIN_P8_INFO_SX, WIN_P8_INFO_SY, WIN_P8_INFO_PAL
	},
	{	// 「わすれる」
		GFL_BG_FRAME1_S, WIN_P8_WASURERU_PX, WIN_P8_WASURERU_PY,
		WIN_P8_WASURERU_SX, WIN_P8_WASURERU_SY, WIN_P8_WASURERU_PAL
	},
};


// ポケモン名のメッセージID
static const u32 NameMsgID_Tbl[] = {
	mes_b_plist_01_001,
	mes_b_plist_01_101,
	mes_b_plist_01_201,
	mes_b_plist_01_301,
	mes_b_plist_01_401,
	mes_b_plist_01_501
};

// 技名のメッセージID
static const u32 WazaMsgID_Tbl[] = {
	mes_b_plist_05_100,
	mes_b_plist_05_200,
	mes_b_plist_05_300,
	mes_b_plist_05_400,
	mes_b_plist_05_500
};



//--------------------------------------------------------------------------------------------
/**
 * BMPウィンドウ初期化
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_BmpInit( BPLIST_WORK * wk )
{
	const u8 * dat;
	u32	i;

	dat = CommBmpData[0];
	for( i=0; i<WIN_MAX; i++ ){
//		GF_BGL_BmpWinAddEx( wk->bgl, &wk->win[i], &CommBmpData[i] );
		wk->win[i].win = GFL_BMPWIN_Create(
											dat[0],
											dat[1], dat[2],
											dat[3], dat[4],
											dat[5],
											GFL_BMP_CHRAREA_GET_B );
		dat += 6;
	}
	BattlePokeList_BmpAdd( wk, wk->page );
}

//--------------------------------------------------------------------------------------------
/**
 * ページごとのBMPウィンドウ追加
 *
 * @param	wk		ワーク
 * @param	page	ページ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_BmpAdd( BPLIST_WORK * wk, u32 page )
{
	const u8 * dat;
	u32	i;

	switch( page ){
	case BPLIST_PAGE_SELECT:		// ポケモン選択ページ
		dat = Page1_BmpData[0];
		wk->bmp_add_max = WIN_P1_MAX;
		break;

	case BPLIST_PAGE_POKE_CHG:		// ポケモン入れ替えページ
		dat = ChgPage_BmpData[0];
		wk->bmp_add_max = WIN_CHG_MAX;
		break;

	case BPLIST_PAGE_MAIN:			// ステータスメインページ
		dat = StMainPage_BmpData[0];
		wk->bmp_add_max = WIN_P3_MAX;
		break;

	case BPLIST_PAGE_WAZA_SEL:		// ステータス技選択ページ
		dat = StWazaSelPage_BmpData[0];
		wk->bmp_add_max = WIN_STW_MAX;
		break;

	case BPLIST_PAGE_SKILL:			// ステータス技ページ
		dat = StWazaInfoPage_BmpData[0];
		wk->bmp_add_max = WIN_P4_MAX;
		break;

	case BPLIST_PAGE_PP_RCV:		// PP回復技選択ページ
		dat = PPRcvPage_BmpData[0];
		wk->bmp_add_max = WIN_P7_MAX;
		break;

	case BPLIST_PAGE_WAZASET_BS:	// ステータス技忘れ１ページ（戦闘技選択）
	case BPLIST_PAGE_WAZASET_CS:	// ステータス技忘れ４ページ（コンテスト技選択）
		dat = Page5_BmpData[0];
		wk->bmp_add_max = WIN_P5_MAX;
		break;

	case BPLIST_PAGE_WAZASET_BI:	// ステータス技忘れ２ページ（戦闘技詳細）
		dat = Page6_BmpData[0];
		wk->bmp_add_max = WIN_P6_MAX;
		break;

	case BPLIST_PAGE_WAZASET_CI:	// ステータス技忘れ３ページ（コンテスト技詳細）
		dat = Page8_BmpData[0];
		wk->bmp_add_max = WIN_P8_MAX;
		break;
	}

//	wk->add_win = GFL_BMPWIN_Init( wk->dat->heap, wk->bmp_add_max );

	for( i=0; i<wk->bmp_add_max; i++ ){
//		GF_BGL_BmpWinAddEx( wk->bgl, &wk->add_win[i], &dat[i] );
		wk->add_win[i].win = GFL_BMPWIN_Create(
													dat[0],
													dat[1], dat[2],
													dat[3], dat[4],
													dat[5],
													GFL_BMP_CHRAREA_GET_B );
		dat += 6;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * 追加BMPウィンドウ削除
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_BmpFree( BPLIST_WORK * wk )
{
	u32	i;

	for( i=0; i<wk->bmp_add_max; i++ ){
		GFL_BMPWIN_Delete( wk->add_win[i].win );
	}

//	GF_BGL_BmpWinFree( wk->add_win, wk->bmp_add_max );
}

//--------------------------------------------------------------------------------------------
/**
 * BMPウィンドウ全削除
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_BmpFreeAll( BPLIST_WORK * wk )
{
	u32	i;

//	GF_BGL_BmpWinFree( wk->add_win, wk->bmp_add_max );
	BattlePokeList_BmpFree( wk );
	for( i=0; i<WIN_MAX; i++ ){
		GFL_BMPWIN_Delete( wk->win[i].win );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * BMP書き込み
 *
 * @param	wk		ワーク
 * @param	page	ページ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_BmpWrite( BPLIST_WORK * wk, u32 page )
{
	switch( page ){
	case BPLIST_PAGE_SELECT:		// ポケモン選択ページ
		BPL_Page1BmpWrite( wk );
		break;
	case BPLIST_PAGE_POKE_CHG:		// ポケモン入れ替えページ
		BPL_ChgPageBmpWrite( wk );
		break;
	case BPLIST_PAGE_MAIN:			// ステータスメインページ
		BPL_StMainPageBmpWrite( wk );
		break;
	case BPLIST_PAGE_WAZA_SEL:		// ステータス技選択ページ
		BPL_StWazaSelPageBmpWrite( wk );
		break;
	case BPLIST_PAGE_SKILL:			// ステータス技詳細ページ
		BPL_StWazaInfoPageBmpWrite( wk );
		break;

	case BPLIST_PAGE_PP_RCV:		// PP回復技選択ページ
		BPL_PPRcvPageBmpWrite( wk );
		break;

	case BPLIST_PAGE_WAZASET_BS:	// ステータス技忘れ１ページ（戦闘技選択）
	case BPLIST_PAGE_WAZASET_CS:	// ステータス技忘れ４ページ（コンテスト技選択）
		BPL_WazaDelSelPageBmpWrite( wk );
		break;

	case BPLIST_PAGE_WAZASET_BI:	// ステータス技忘れ２ページ（戦闘技詳細）
		BPL_Page6BmpWrite( wk );
		break;

	case BPLIST_PAGE_WAZASET_CI:	// ステータス技忘れ３ページ（コンテスト技詳細）
		BPL_Page8BmpWrite( wk );
		break;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモン名表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 * @param	pos		ポケモン位置
 * @param	px		表示X座標
 * @param	py		表示Y座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_NamePut( BPLIST_WORK * wk, u32 idx, u16 pos, u8 px, u8 py )
{
	GFL_BMPWIN * win;
	BPL_POKEDATA * pd;
	STRBUF * exp;
	STRBUF * str;
	u32	sex_px;

	win = wk->add_win[idx].win;
	pd  = &wk->poke[pos];
	exp = GFL_STR_CreateBuffer( BUFLEN_POKEMON_NAME, wk->dat->heap );
	str = GFL_MSG_CreateString( wk->mman, NameMsgID_Tbl[pos] );

	WORDSET_RegisterPokeNickName( wk->wset, 0, pd->pp );
	WORDSET_ExpandStr( wk->wset, exp, str );

/*
	if( fidx == FONT_SYSTEM ){
		GF_STR_PrintColor( win, fidx, exp, px, py, MSG_NO_PUT, PCOL_N_WHITE, NULL );
	}else{
		GF_STR_PrintColor( win, fidx, exp, px, py, MSG_NO_PUT, PCOL_BTN, NULL );
	}
*/
	PRINTSYS_PrintQueColor( wk->que, GFL_BMPWIN_GetBmp(win), px, py, exp, wk->dat->font, PCOL_N_WHITE );

	GFL_STR_DeleteBuffer( str );
	GFL_STR_DeleteBuffer( exp );

	// 性別
	if( pd->sex_put == 0 && pd->egg == 0 ){
		if( pd->sex == PTL_SEX_MALE ){
			str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_502 );
			sex_px = GFL_BMPWIN_GetSizeX(win)*8-PRINTSYS_GetStrWidth(str,wk->dat->font,0);
/*
			if( fidx == FONT_SYSTEM ){
				GF_STR_PrintColor(
					win, FONT_SYSTEM, str, sex_px, py, MSG_NO_PUT, PCOL_N_BLUE, NULL );
			}else{
				GF_STR_PrintColor(
					win, FONT_SYSTEM, str, sex_px, py, MSG_NO_PUT, PCOL_B_BLUE, NULL );
			}
*/
			PRINTSYS_PrintQueColor( wk->que, GFL_BMPWIN_GetBmp(win), sex_px, py, str, wk->dat->font, PCOL_N_BLUE );
			GFL_STR_DeleteBuffer( str );
		}else if( pd->sex == PTL_SEX_FEMALE ){
			str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_503 );
			sex_px = GFL_BMPWIN_GetSizeX(win)*8-PRINTSYS_GetStrWidth(str,wk->dat->font,0);
/*
			if( fidx == FONT_SYSTEM ){
				GF_STR_PrintColor(
					win, FONT_SYSTEM, str, sex_px, py, MSG_NO_PUT, PCOL_N_RED, NULL );
			}else{
				GF_STR_PrintColor(
					win, FONT_SYSTEM, str, sex_px, py, MSG_NO_PUT, PCOL_B_RED, NULL );
			}
*/
			PRINTSYS_PrintQueColor( wk->que, GFL_BMPWIN_GetBmp(win), sex_px, py, str, wk->dat->font, PCOL_N_BLUE );
			GFL_STR_DeleteBuffer( str );
		}
	}

	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * レベル表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 * @param	pos		ポケモン位置
 * @param	px		表示X座標
 * @param	py		表示Y座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_LvPut( BPLIST_WORK * wk, u32 idx, u16 pos, u8 px, u8 py )
{
	BPL_POKEDATA * pd;
	STRBUF * str;

	pd  = &wk->poke[pos];
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_01_701 );

	WORDSET_RegisterNumber(
		wk->wset, 0, pd->lv, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[idx].win), px+8, py, wk->msg_buf, wk->nfnt, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );

	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * HP表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 * @param	pos		ポケモン位置
 * @param	px		表示X座標
 * @param	py		表示Y座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_HPPut( BPLIST_WORK * wk, u32 idx, u16 pos, u8 px, u8 py )
{
	BPL_POKEDATA * pd;
	STRBUF * str;
	u8	sx;

	pd  = &wk->poke[pos];

	//「／」
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_01_702 );
	sx  = PRINTSYS_GetStrWidth( str, wk->nfnt, 0 );
	px  = px - ( sx / 2 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[idx].win), px, py, str, wk->nfnt, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	// HP
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_01_703 );
	WORDSET_RegisterNumber(
		wk->wset, 0, pd->hp, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[idx].win),
		px-PRINTSYS_GetStrWidth(wk->msg_buf,wk->nfnt,0), py, wk->msg_buf, wk->nfnt, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	// MHP
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_01_703 );
	WORDSET_RegisterNumber(
		wk->wset, 0, pd->mhp, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[idx].win),
		px+sx, py, wk->msg_buf, wk->nfnt, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );

	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * HPゲージ表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 * @param	pos		ポケモン位置
 * @param	px		表示X座標
 * @param	py		表示Y座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_HPGagePut( BPLIST_WORK * wk, u32 idx, u16 pos, u8 px, u8 py )
{
	BPL_POKEDATA * pd;
	u8	col=1;
	u8	dot;

	pd  = &wk->poke[pos];
	col = 1;
//	dot = GetNumDotto( pd->hp, pd->mhp, BPL_HP_DOTTO_MAX );
	dot = BPL_HP_DOTTO_MAX;

/*
	switch( GetHPGaugeDottoColor( pd->hp, pd->mhp, BPL_HP_DOTTO_MAX ) ){
	case HP_DOTTO_NULL:
		GFL_BMPWIN_MakeTransWindow_VBlank( wk->add_win[idx] );
		return;
	case HP_DOTTO_MAX:		// 緑
	case HP_DOTTO_GREEN:
		col = HP_GAGE_COL_G1;
		break;
	case HP_DOTTO_YELLOW:	// 黄
		col = HP_GAGE_COL_Y1;
		break;
	case HP_DOTTO_RED:		// 赤
		col = HP_GAGE_COL_R1;
		break;
	}
*/
	col = HP_GAGE_COL_G1;

	GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->add_win[idx].win), px, py+1, dot, 1, col+1 );
	GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->add_win[idx].win), px, py+2, dot, 2, col );
	GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->add_win[idx].win), px, py+4, dot, 1, col+1 );

	GFL_BMPWIN_MakeTransWindow_VBlank( wk->add_win[idx].win );
}

//--------------------------------------------------------------------------------------------
/**
 * 特性表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 * @param	pos		ポケモン位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_TokuseiPut( BPLIST_WORK * wk, u32 idx, u32 pos )
{
	BPL_POKEDATA * pd;
	STRBUF * exp;
	STRBUF * str;

	pd  = &wk->poke[pos];
	exp = GFL_STR_CreateBuffer( BUFLEN_POKEMON_ABILITY_NAME, wk->dat->heap );
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_000 );

	WORDSET_RegisterTokuseiName( wk->wset, 0, pd->spa );
	WORDSET_ExpandStr( wk->wset, exp, str );

	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[idx].win), 0, 0, exp, wk->dat->font, PCOL_N_WHITE );

	GFL_STR_DeleteBuffer( str );
	GFL_STR_DeleteBuffer( exp );

	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * アイテム表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 * @param	pos		ポケモン位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ItemPut( BPLIST_WORK * wk, u32 idx, u32 pos )
{
	BPL_POKEDATA * pd;
	STRBUF * exp;
	STRBUF * str;

	pd  = &wk->poke[pos];
	if( pd->item == 0 ){
		exp = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_600 );
	}else{
		exp = GFL_STR_CreateBuffer( BUFLEN_ITEM_NAME, wk->dat->heap );
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_001 );
		WORDSET_RegisterItemName( wk->wset, 0, pd->item );
		WORDSET_ExpandStr( wk->wset, exp, str );
		GFL_STR_DeleteBuffer( str );
	}
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[idx].win), 0, 0, exp, wk->dat->font, PCOL_N_WHITE );

	GFL_STR_DeleteBuffer( exp );

	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * 技名表示
 *
 * @param	wk		ワーク
 * @param	waza	技番号
 * @param	widx	ウィンドウインデックス
 * @param	midx	メッセージインデックス
 * @param	fidx	フォントインデックス
 * @param	py		Y座標
 * @param	col		カラー
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaNamePut(
				BPLIST_WORK * wk, u32 waza, u32 widx, u32 midx, u16 py, u32 col )
{
	GFL_BMPWIN * win;
	STRBUF * exp;
	STRBUF * str;
	u32	px;

	win = wk->add_win[widx].win;

	exp = GFL_STR_CreateBuffer( BUFLEN_WAZA_NAME, wk->dat->heap );
	str = GFL_MSG_CreateString( wk->mman, midx );

	WORDSET_RegisterWazaName( wk->wset, 0, waza );
	WORDSET_ExpandStr( wk->wset, exp, str );
/*
	if( fidx == FONT_TOUCH ){
		px = (GFL_BMPWIN_GetSizeX(win)*8-PRINTSYS_GetStrWidth(exp,wk->dat->font,0))/2;
	}else{
		px = 0;
	}
*/
	px = 0;

	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win), px, py, exp, wk->dat->font, col );

	GFL_STR_DeleteBuffer( str );
	GFL_STR_DeleteBuffer( exp );

	BAPPTOOL_PrintQueOn( &wk->add_win[widx] );
}

//--------------------------------------------------------------------------------------------
/**
 * 「PP」表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 * @param	px		表示X座標
 * @param	py		表示Y座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_PPPut( BPLIST_WORK * wk, u16 idx, u8 px, u8 py )
{
	STRBUF * str;

	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_500 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[idx].win), px, py, str, wk->dat->font, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * PP値表示
 *
 * @param	wk		ワーク
 * @param	waza	技データ
 * @param	idx		ウィンドウインデックス
 * @param	px		表示X座標
 * @param	py		表示Y座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_PPNumPut( BPLIST_WORK * wk, BPL_POKEWAZA * waza, u16 idx, u8 px, u8 py )
{
	NUMFONT_WriteNumber(
		wk->nfnt, waza->pp, 2, NUMFONT_MODE_SPACE, &wk->add_win[idx], px, py );
	NUMFONT_WriteMark(
		wk->nfnt, NUMFONT_MARK_SLASH, &wk->add_win[idx], px+NUMFONT_NUM_WIDTH*2, py );
	NUMFONT_WriteNumber(
		wk->nfnt, waza->mpp, 2, NUMFONT_MODE_LEFT,
		&wk->add_win[idx], px+NUMFONT_NUM_WIDTH*2+8, py );

//	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * ポケモン選択ページコメント表示
 *
 * @param	wk		ワーク
 * @param	midx	メッセージインデックス
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_PokeSelStrPut( BPLIST_WORK * wk, u32 midx )
{
	STRBUF * str;

/*
	BmpWinFrame_Write(
		&wk->win[WIN_COMMENT], WINDOW_TRANS_OFF, TALK_WIN_CGX_POS, BPL_PAL_TALK_WIN );
*/
	BmpWinFrame_Write(
		wk->win[WIN_COMMENT].win, WINDOW_TRANS_OFF, TALK_WIN_CGX_POS, BPL_PAL_TALK_WIN );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[WIN_COMMENT].win), 15 );

	str = GFL_MSG_CreateString( wk->mman, midx );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->win[WIN_COMMENT].win), 0, 0, str, wk->dat->font, PCOL_N_BLACK );
	GFL_STR_DeleteBuffer( str );

	BAPPTOOL_PrintQueOn( &wk->win[WIN_COMMENT] );
}

//--------------------------------------------------------------------------------------------
/**
 * 「いれかえ」表示
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
#define	IREKAE_PY	( 8-1 )

static void BPL_StrIrekaePut( BPLIST_WORK * wk )
{
	STRBUF * str;
	u32	siz;

	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_501 );
	siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_CHG_IREKAE].win),
		(WIN_CHG_IREKAE_SX*8-siz)/2, IREKAE_PY, str, wk->dat->font, PCOL_BTN );
	GFL_STR_DeleteBuffer( str );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_CHG_IREKAE] );
}

//--------------------------------------------------------------------------------------------
/**
 * コマンド表示
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
#define	COMMAND_STR_PY	( 6-1 )

static void BPL_StrCommandPut( BPLIST_WORK * wk, u32 wid, u32 mid )
{
	GFL_BMPWIN * win;
	STRBUF * str;
	u32	siz;

	win = wk->add_win[wid].win;
	str = GFL_MSG_CreateString( wk->mman, mid );
	siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win),
		(GFL_BMPWIN_GetSizeX(win)*8-siz)/2, COMMAND_STR_PY, str, wk->dat->font, PCOL_BTN );
	GFL_STR_DeleteBuffer( str );
	BAPPTOOL_PrintQueOn( &wk->add_win[wid] );
}

//--------------------------------------------------------------------------------------------
/**
 * ページ３のレベル表示
 *
 * @param	wk		ワーク
 * @param	pos		ポケモン位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_P3_LvPut( BPLIST_WORK * wk, u32 pos )
{
	BPL_POKEDATA * pd;
	STRBUF * str;
	STRBUF * exp;
	u16	px;
	u16	swap;

	pd   = &wk->poke[pos];
	swap = WIN_P3_NAME_S * wk->bmp_swap;

	// 「Lv.」
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_000 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_LV].win), 0, 0, str, wk->dat->font, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	// レベル値
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_001 );
	exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap );	// (3桁+EOM_)x2(ローカライズ用に一応)
	WORDSET_RegisterNumber(
		wk->wset, 0, pd->lv, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
	WORDSET_ExpandStr( wk->wset, exp, str );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_LVNUM+swap].win), 0, 0, exp, wk->dat->font, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	GFL_STR_DeleteBuffer( exp );
	// 「つぎのレベルまで」
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_100 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_NEXT].win), 0, 0, str, wk->dat->font, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	// 次のレベル値
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_101 );
	exp = GFL_STR_CreateBuffer( (6+1)*2, wk->dat->heap );	// (6桁+EOM_)x2(ローカライズ用に一応)
	if( pd->lv < 100 ){
		WORDSET_RegisterNumber(
			wk->wset, 0, pd->next_lv_exp - pd->now_exp,
			6, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
	}else{
		WORDSET_RegisterNumber(
			wk->wset, 0, 0, 6, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
	}
	WORDSET_ExpandStr( wk->wset, exp, str );
	px = GFL_BMPWIN_GetSizeX( wk->add_win[WIN_P3_NEXTNUM+swap].win ) * 8
			- PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_NEXTNUM+swap].win), px, 0, exp, wk->dat->font, PCOL_N_BLACK );
	GFL_STR_DeleteBuffer( str );
	GFL_STR_DeleteBuffer( exp );

	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_LV] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_LVNUM+swap] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_NEXT] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_NEXTNUM+swap] );
}

//--------------------------------------------------------------------------------------------
/**
 * ページ３の攻撃表示
 *
 * @param	wk		ワーク
 * @param	pos		ポケモン位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_P3_PowPut( BPLIST_WORK * wk, u32 pos )
{
	BPL_POKEDATA * pd;
	STRBUF * str;
	STRBUF * exp;
	u16	swap;
	u8	siz;
	u8	px;

	pd   = &wk->poke[pos];
	swap = WIN_P3_NAME_S * wk->bmp_swap;

	// 「こうげき」
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_400 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_POW].win), 0, 0, str, wk->dat->font, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	// 攻撃値
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_401 );
	exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap );	// (3桁+EOM_)x2(ローカライズ用に一応)
	WORDSET_RegisterNumber(
		wk->wset, 0, pd->pow, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
	WORDSET_ExpandStr( wk->wset, exp, str );
	siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
	px  = GFL_BMPWIN_GetSizeX( wk->add_win[WIN_P3_POWNUM+swap].win ) * 8 - siz;
/*↑[GS_CONVERT_TAG]*/
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_POWNUM+swap].win), px, 0, exp, wk->dat->font, PCOL_N_BLACK );
	GFL_STR_DeleteBuffer( str );
	GFL_STR_DeleteBuffer( exp );

	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_POW] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_POWNUM+swap] );
}

//--------------------------------------------------------------------------------------------
/**
 * ページ３の防御表示
 *
 * @param	wk		ワーク
 * @param	pos		ポケモン位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_P3_DefPut( BPLIST_WORK * wk, u32 pos )
{
	BPL_POKEDATA * pd;
	STRBUF * str;
	STRBUF * exp;
	u16	swap;
	u8	siz;
	u8	px;

	pd   = &wk->poke[pos];
	swap = WIN_P3_NAME_S * wk->bmp_swap;

	// 「ぼうぎょ」
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_500 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_DEF].win), 0, 0, str, wk->dat->font, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	// 防御値
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_501 );
	exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap );	// (3桁+EOM_)x2(ローカライズ用に一応)
	WORDSET_RegisterNumber(
		wk->wset, 0, pd->def, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
	WORDSET_ExpandStr( wk->wset, exp, str );
	siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
	px  = GFL_BMPWIN_GetSizeX( wk->add_win[WIN_P3_DEFNUM+swap].win ) * 8 - siz;
/*↑[GS_CONVERT_TAG]*/
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_DEFNUM+swap].win), px, 0, exp, wk->dat->font, PCOL_N_BLACK );
	GFL_STR_DeleteBuffer( str );
	GFL_STR_DeleteBuffer( exp );

	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_DEF] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_DEFNUM+swap] );
}

//--------------------------------------------------------------------------------------------
/**
 * ページ３の素早さ表示
 *
 * @param	wk		ワーク
 * @param	pos		ポケモン位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_P3_AgiPut( BPLIST_WORK * wk, u32 pos )
{
	BPL_POKEDATA * pd;
	STRBUF * str;
	STRBUF * exp;
	u16	swap;
	u8	siz;
	u8	px;

	pd   = &wk->poke[pos];
	swap = WIN_P3_NAME_S * wk->bmp_swap;

	// 「すばやさ」
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_800 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_AGI].win), 0, 0, str, wk->dat->font, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	// 素早さ値
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_801 );
	exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap );	// (3桁+EOM_)x2(ローカライズ用に一応)
	WORDSET_RegisterNumber(
		wk->wset, 0, pd->agi, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
	WORDSET_ExpandStr( wk->wset, exp, str );
	siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
	px  = GFL_BMPWIN_GetSizeX( wk->add_win[WIN_P3_AGINUM+swap].win ) * 8 - siz;
/*↑[GS_CONVERT_TAG]*/
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_AGINUM+swap].win), px, 0, exp, wk->dat->font, PCOL_N_BLACK );
	GFL_STR_DeleteBuffer( str );
	GFL_STR_DeleteBuffer( exp );

	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_AGI] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_AGINUM+swap] );
}

//--------------------------------------------------------------------------------------------
/**
 * ページ３の特攻表示
 *
 * @param	wk		ワーク
 * @param	pos		ポケモン位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_P3_SppPut( BPLIST_WORK * wk, u32 pos )
{
	BPL_POKEDATA * pd;
	STRBUF * str;
	STRBUF * exp;
	u16	swap;
	u8	siz;
	u8	px;

	pd   = &wk->poke[pos];
	swap = WIN_P3_NAME_S * wk->bmp_swap;

	// 「とくこう」
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_600 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPP].win), 0, 0, str, wk->dat->font, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	// 特攻値
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_601 );
	exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap );	// (3桁+EOM_)x2(ローカライズ用に一応)
	WORDSET_RegisterNumber(
		wk->wset, 0, pd->spp, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
	WORDSET_ExpandStr( wk->wset, exp, str );
	siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
	px  = GFL_BMPWIN_GetSizeX( wk->add_win[WIN_P3_SPPNUM+swap].win ) * 8 - siz;
/*↑[GS_CONVERT_TAG]*/
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPPNUM+swap].win), px, 0, exp, wk->dat->font, PCOL_N_BLACK );
	GFL_STR_DeleteBuffer( str );
	GFL_STR_DeleteBuffer( exp );

	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_SPP] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_SPPNUM+swap] );
}

//--------------------------------------------------------------------------------------------
/**
 * ページ３の特防表示
 *
 * @param	wk		ワーク
 * @param	pos		ポケモン位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_P3_SpdPut( BPLIST_WORK * wk, u32 pos )
{
	BPL_POKEDATA * pd;
	STRBUF * str;
	STRBUF * exp;
	u16	swap;
	u8	siz;
	u8	px;

	pd   = &wk->poke[pos];
	swap = WIN_P3_NAME_S * wk->bmp_swap;

	// 「とくぼう」
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_700 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPD].win), 0, 0, str, wk->dat->font, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	// 特防値
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_701 );
	exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap );	// (3桁+EOM_)x2(ローカライズ用に一応)
	WORDSET_RegisterNumber(
		wk->wset, 0, pd->spd, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
	WORDSET_ExpandStr( wk->wset, exp, str );
	siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
	px  = GFL_BMPWIN_GetSizeX( wk->add_win[WIN_P3_SPDNUM+swap].win ) * 8 - siz;
/*↑[GS_CONVERT_TAG]*/
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPDNUM+swap].win), px, 0, exp, wk->dat->font, PCOL_N_BLACK );
	GFL_STR_DeleteBuffer( str );
	GFL_STR_DeleteBuffer( exp );

	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_SPD] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_SPDNUM+swap] );
}

//--------------------------------------------------------------------------------------------
/**
 * ページ３のHP表示
 *
 * @param	wk		ワーク
 * @param	pos		ポケモン位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_P3_HPPut( BPLIST_WORK * wk, u32 pos )
{
	BPL_POKEDATA * pd;
	STRBUF * str;
	STRBUF * exp;
	u32	sra_siz, tmp_siz;
	u16	px;
	u16	swap;

	pd   = &wk->poke[pos];
	swap = WIN_P3_NAME_S * wk->bmp_swap;

	// 「HP」
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_300 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_HP].win), 0, 0, str, wk->dat->font, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	// 「/」
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_303 );
	sra_siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
	px = (GFL_BMPWIN_GetSizeX( wk->add_win[WIN_P3_HPNUM].win )*8-sra_siz)/2;
/*↑[GS_CONVERT_TAG]*/
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_HPNUM+swap].win), px, 0, str, wk->dat->font, PCOL_N_BLACK );
	GFL_STR_DeleteBuffer( str );
	// HP値
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_301 );
	exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap );	// (3桁+EOM_)x2(ローカライズ用に一応)
	WORDSET_RegisterNumber(
		wk->wset, 0, pd->hp, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
	WORDSET_ExpandStr( wk->wset, exp, str );
	tmp_siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_HPNUM+swap].win),
		px-tmp_siz, 0, exp, wk->dat->font, PCOL_N_BLACK );
	GFL_STR_DeleteBuffer( str );
	GFL_STR_DeleteBuffer( exp );
	// 最大HP値
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_302 );
	exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap );	// (3桁+EOM_)x2(ローカライズ用に一応)
	WORDSET_RegisterNumber(
		wk->wset, 0, pd->mhp, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
/*↑[GS_CONVERT_TAG]*/
/*↑[GS_CONVERT_TAG]*/
	WORDSET_ExpandStr( wk->wset, exp, str );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_HPNUM+swap].win),
		px+sra_siz, 0, exp, wk->dat->font, PCOL_N_BLACK );
	GFL_STR_DeleteBuffer( str );
	GFL_STR_DeleteBuffer( exp );

	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_HP] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_HPNUM+swap] );
}

//--------------------------------------------------------------------------------------------
/**
 * ページ３の特性表示
 *
 * @param	wk		ワーク
 * @param	pos		ポケモン位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_P3_TokuseiInfoPut( BPLIST_WORK * wk, u32 pos )
{
/*	特性のメッセージがわからないのでコメントアウト
	BPL_POKEDATA * pd;
	GFL_MSGDATA * man;
	STRBUF * str;
	u32	swap;

	pd   = &wk->poke[pos];
	swap = WIN_P3_NAME_S * wk->bmp_swap;

	man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_msg_tokuseiinfo_dat, wk->dat->heap );

	str = GFL_MSG_CreateString( man, pd->spa );
	GF_STR_PrintColor(
		&wk->add_win[WIN_P3_SPAINFO+swap],
		FONT_SYSTEM, str, 0, 0, MSG_NO_PUT, PCOL_N_BLACK, NULL );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPAINFO+swap]), 0, 0, str, wk->dat->font, PCOL_N_BLACK );
	GFL_STR_DeleteBuffer( str );
	GFL_MSG_Delete( man );
	GFL_BMPWIN_MakeTransWindow_VBlank( wk->add_win[WIN_P3_SPAINFO+swap] );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * 「めいちゅう」表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 *
 * @return	none
 *
 *	ページ４・６で使用
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaHitStrPut( BPLIST_WORK * wk, u32 idx )
{
	GFL_BMPWIN * win;
	STRBUF * str;

	win = wk->add_win[idx].win;
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_200 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win), 0, 0, str, wk->dat->font, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * 技の命中値表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 * @param	hit		命中値
 *
 * @return	none
 *
 *	ページ４・６で使用
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaHitNumPut( BPLIST_WORK * wk, u32 idx, u32 hit )
{
	GFL_BMPWIN * win;
	STRBUF * str;
	STRBUF * exp;
	u16	siz;
	u16	px;

	win = wk->add_win[idx].win;

	if( hit == 0 ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_102 );
		siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
		px  = GFL_BMPWIN_GetSizeX(win) * 8 - siz;
/*↑[GS_CONVERT_TAG]*/
		PRINTSYS_PrintQueColor(
			wk->que, GFL_BMPWIN_GetBmp(win), px, 0, str, wk->dat->font, PCOL_N_BLACK );
		GFL_STR_DeleteBuffer( str );
	}else{
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_201 );
		exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap );	// (3桁+EOM_)x2(ローカライズ用に一応)
		WORDSET_RegisterNumber(
			wk->wset, 0, hit, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
		WORDSET_ExpandStr( wk->wset, exp, str );
		siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
		px  = GFL_BMPWIN_GetSizeX(win) * 8 - siz;
/*↑[GS_CONVERT_TAG]*/
		PRINTSYS_PrintQueColor(
			wk->que, GFL_BMPWIN_GetBmp(win), px, 0, exp, wk->dat->font, PCOL_N_BLACK );
		GFL_STR_DeleteBuffer( str );
		GFL_STR_DeleteBuffer( exp );
	}

	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * 「いりょく」表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 *
 * @return	none
 *
 *	ページ４・６で使用
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaPowStrPut( BPLIST_WORK * wk, u32 idx )
{
	GFL_BMPWIN * win;
	STRBUF * str;

	win = wk->add_win[idx].win;
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_100 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win), 0, 0, str, wk->dat->font, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * 技の威力値表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 * @param	pow		威力値
 *
 * @return	none
 *
 *	ページ４・６で使用
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaPowNumPut( BPLIST_WORK * wk, u32 idx, u32 pow )
{
	GFL_BMPWIN * win;
	STRBUF * str;
	STRBUF * exp;
	u16	siz;
	u16	px;

	win = wk->add_win[idx].win;

	if( pow <= 1 ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_102 );
		siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
		px  = GFL_BMPWIN_GetSizeX(win) * 8 - siz;
/*↑[GS_CONVERT_TAG]*/
		PRINTSYS_PrintQueColor(
			wk->que, GFL_BMPWIN_GetBmp(win), px, 0, str, wk->dat->font, PCOL_N_BLACK );
		GFL_STR_DeleteBuffer( str );
	}else{
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_101 );
		exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap );	// (3桁+EOM_)x2(ローカライズ用に一応)
		WORDSET_RegisterNumber(
			wk->wset, 0, pow, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
		WORDSET_ExpandStr( wk->wset, exp, str );
		siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
		px  = GFL_BMPWIN_GetSizeX(win) * 8 - siz;
/*↑[GS_CONVERT_TAG]*/
		PRINTSYS_PrintQueColor(
			wk->que, GFL_BMPWIN_GetBmp(win), px, 0, exp, wk->dat->font, PCOL_N_BLACK );
		GFL_STR_DeleteBuffer( str );
		GFL_STR_DeleteBuffer( exp );
	}

	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * 技説明表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 * @param	waza	技ID
 *
 * @return	none
 *
 *	ページ４・６で使用
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaInfoPut( BPLIST_WORK * wk, u32 idx, u32 waza )
{
/*	技情報のメッセージがわからないのでコメントアウト
	GFL_MSGDATA * man;
	GFL_BMPWIN * win;
	STRBUF * str;

	win = wk->add_win[idx];
	man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_msg_wazainfo_dat, wk->dat->heap );
	str = GFL_MSG_CreateString( man, waza );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win), 0, 0, str, wk->dat->font, PCOL_N_BLACK );
	GFL_STR_DeleteBuffer( str );
	GFL_MSG_Delete( man );
	GFL_BMPWIN_MakeTransWindow_VBlank( win );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * コンテスト技説明表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 * @param	waza	技ID
 *
 * @return	none
 *
 *	ページ８で使用
 */
//--------------------------------------------------------------------------------------------
/*	コンテスト技の説明を表示しないようにした
static void BPL_ContestWazaInfoPut( BPLIST_WORK * wk, u32 idx, u32 waza )
{
	GFL_MSGDATA * man;
	GFL_BMPWIN * win;
	STRBUF * str;
	u32	ap_no;
	u32	msg_id;

	win = wk->add_win[idx];

	ap_no  = WT_WazaDataParaGet( waza, ID_WTD_ap_no );
//	msg_id = ConTool_GetListAPSetumeiMsgID( ap_no );

	man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_msg_cmsg_wazaexplain_dat, wk->dat->heap );

	str = GFL_MSG_CreateString( man, msg_id );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win), 0, 0, str, wk->dat->font, PCOL_N_BLACK );
	GFL_STR_DeleteBuffer( str );

	GFL_MSG_Delete( man );
	GFL_BMPWIN_MakeTransWindow_VBlank( win );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * 「ぶんるい」表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 *
 * @return	none
 *
 *	ページ４・６で使用
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaBunruiStrPut( BPLIST_WORK * wk, u32 idx )
{
	GFL_BMPWIN * win;
	STRBUF * str;
	u16	siz;
	u16	px;

	win = wk->add_win[idx].win;
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_300 );
	siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
	px  = ( GFL_BMPWIN_GetSizeX(win) * 8 - siz ) / 2;
/*↑[GS_CONVERT_TAG]*/
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win), px, 0, str, wk->dat->font, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * 分類種類表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 * @param	kind	分類種類
 *
 * @return	none
 *
 *	ページ４・６で使用
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaKindPut( BPLIST_WORK * wk, u32 idx, u32 kind )
{
	GFL_BMPWIN * win;
	STRBUF * str;

	win = wk->add_win[idx].win;

	switch( kind ){
	case 0:		// 物理
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_301 );
		break;
	case 1:		// 変化
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_303 );
		break;
	case 2:		// 特殊
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_302 );
	}
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win), 0, 0, str, wk->dat->font, PCOL_N_BLACK );
	GFL_STR_DeleteBuffer( str );

	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * 技PP表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 * @param	npp		現在のPP
 * @param	mpp		最大PP
 *
 * @return	none
 *
 *	ページ４・６で使用
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaPPPut( BPLIST_WORK * wk, u32 idx, u32 npp, u32 mpp )
{
	GFL_BMPWIN * win;
	STRBUF * str;
	STRBUF * exp;
	u32	sra_siz, tmp_siz;
	u32	px;

	win = wk->add_win[idx].win;

	// 「/」
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_004 );
	sra_siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
	px = ( GFL_BMPWIN_GetSizeX(win) * 8 - sra_siz ) / 2;
/*↑[GS_CONVERT_TAG]*/
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win), px, 0, str, wk->dat->font, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	// PP値
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_002 );
	exp = GFL_STR_CreateBuffer( (2+1)*2, wk->dat->heap );	// (2桁+EOM_)x2(ローカライズ用に一応)
	WORDSET_RegisterNumber(
		wk->wset, 0, npp, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
	WORDSET_ExpandStr( wk->wset, exp, str );
	tmp_siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win), px-tmp_siz, 0, exp, wk->dat->font, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	GFL_STR_DeleteBuffer( exp );
	// 最大PP値
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_003 );
	exp = GFL_STR_CreateBuffer( (2+1)*2, wk->dat->heap );	// (2桁+EOM_)x2(ローカライズ用に一応)
	WORDSET_RegisterNumber(
		wk->wset, 0, mpp, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
	WORDSET_ExpandStr( wk->wset, exp, str );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win), px+sra_siz, 0, exp, wk->dat->font, PCOL_N_WHITE );
	GFL_STR_DeleteBuffer( str );
	GFL_STR_DeleteBuffer( exp );

	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

#define	P5_PPNUM5_PX	( 56 )		// PP値表示X座標
#define	P5_PPNUM5_PY	( 32 )		// PP値表示Y座標

//--------------------------------------------------------------------------------------------
/**
 * ページ５のPP値表示
 *
 * @param	wk		ワーク
 * @param	waza	技ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_P5_PPNumPut( BPLIST_WORK * wk, u32 waza )
{
	GFL_BMPWIN * win;
	u32	pp;

	win = wk->add_win[WIN_P5_SKILL5].win;
	pp  = WT_WazaDataParaGet( waza, ID_WTD_pp );

	NUMFONT_WriteNumber( wk->nfnt, pp, 2, NUMFONT_MODE_SPACE, win, P5_PPNUM5_PX, P5_PPNUM5_PY );
	NUMFONT_WriteNumber(
		wk->nfnt, pp, 2, NUMFONT_MODE_LEFT,
		win, P5_PPNUM5_PX+NUMFONT_NUM_WIDTH*2+8, P5_PPNUM5_PY );

	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P5_SKILL5] );
}
*/

#define	P6_WASURERU_PY	( 6-1 )	// 「わすれる」表示Y座標

//--------------------------------------------------------------------------------------------
/**
 * 「わすれる」表示
 *
 * @param	wk		ワーク
 * @param	idx		ウィンドウインデックス
 *
 * @return	none
 *
 *	ページ６で使用
 */
//--------------------------------------------------------------------------------------------
static void BPL_WasureruStrPut( BPLIST_WORK * wk, u32 idx )
{
	GFL_BMPWIN * win;
	STRBUF * str;
	u32	siz;

	win = wk->add_win[idx].win;

	if( wk->dat->sel_wp == 4 ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_05_001 );
	}else{
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_05_000 );
	}
	siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win),
		(WIN_P6_WASURERU_SX*8-siz)/2, P6_WASURERU_PY, str, wk->dat->font, PCOL_BTN );
	GFL_STR_DeleteBuffer( str );
	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * 秘伝技エラーメッセージ表示
 *
 * @param	wk		ワーク
 *
 * @return	none
 *
 *	ページ６で使用
 */
//--------------------------------------------------------------------------------------------
void BPL_HidenMsgPut( BPLIST_WORK * wk )
{
	BAPP_BMPWIN_QUE * dat;
	STRBUF * str;

	if( wk->page == BPLIST_PAGE_WAZASET_BI ){
		dat = &wk->add_win[WIN_P6_INFO];
	}else{
		dat = &wk->add_win[WIN_P8_INFO];
	}
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(dat->win), 0 );
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_05_002 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(dat->win), 0, 0, str, wk->dat->font, PCOL_N_BLACK );
	GFL_STR_DeleteBuffer( str );
	BAPPTOOL_PrintQueOn( dat );
}



#define	BTN_PP_PX		( 40 )		// 「PP」表示X座標
#define	BTN_PP_PY		( 24 )		// 「PP」表示Y座標
#define	BTN_PPSRA_PX	( 80 )		// 「/」表示X座標
#define	BTN_PPSRA_PY	( 24 )		// 「/」表示Y座標

//--------------------------------------------------------------------------------------------
/**
 * ボタン上のPPを表示
 *
 * @param	wk		ワーク
 * @param	waza	技データ
 * @param	idx		ウィンドウインデックス
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaButtonPPPut( BPLIST_WORK * wk, BPL_POKEWAZA * waza, u32 idx )
{
	GFL_BMPWIN * win;
	STRBUF * str;
	STRBUF * exp;
	u32	siz;

	win = wk->add_win[idx].win;
	exp = GFL_STR_CreateBuffer( (2+1)*2, wk->dat->heap );	// (2桁+EOM_)x2(ローカライズ用に一応)

	// 「PP」
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_001 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win), BTN_PP_PX, BTN_PP_PY, str, wk->dat->font, PCOL_B_WHITE );
	GFL_STR_DeleteBuffer( str );
	
	// 「/」
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_004 );
	siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win),
		BTN_PPSRA_PX, BTN_PPSRA_PY, str, wk->dat->font, PCOL_B_WHITE );
	GFL_STR_DeleteBuffer( str );

	// 最大PP値
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_003 );
	WORDSET_RegisterNumber(
		wk->wset, 0, waza->mpp, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
	WORDSET_ExpandStr( wk->wset, exp, str );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win),
		BTN_PPSRA_PX+siz, BTN_PPSRA_PY, exp, wk->dat->font, PCOL_B_WHITE );
	GFL_STR_DeleteBuffer( str );

	// PP値
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_002 );
	WORDSET_RegisterNumber(
		wk->wset, 0, waza->pp, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
	WORDSET_ExpandStr( wk->wset, exp, str );
	siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win),
		BTN_PPSRA_PX-siz, BTN_PPSRA_PY, exp, wk->dat->font, PCOL_B_WHITE );
	GFL_STR_DeleteBuffer( str );

	GFL_STR_DeleteBuffer( exp );

	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * ボタン上のPP値を表示
 *
 * @param	wk		ワーク
 * @param	waza	技データ
 * @param	idx		ウィンドウインデックス
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaButtonPPRcv( BPLIST_WORK * wk, BPL_POKEWAZA * waza, u32 idx )
{
	GFL_BMPWIN * win;
	STRBUF * str;
	STRBUF * exp;
	u32	siz;

	win = wk->add_win[idx].win;
	exp = GFL_STR_CreateBuffer( (2+1)*2, wk->dat->heap );	// (2桁+EOM_)x2(ローカライズ用に一応)

	// クリア
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_001 );
	siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
	GFL_STR_DeleteBuffer( str );
	GFL_BMP_Fill( GFL_BMPWIN_GetBmp(win), BTN_PP_PX+siz, BTN_PPSRA_PY, BTN_PPSRA_PX-(BTN_PP_PX+siz), 16, 0 );

	// PP値
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_002 );
	WORDSET_RegisterNumber(
		wk->wset, 0, waza->pp, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
	WORDSET_ExpandStr( wk->wset, exp, str );
	siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win),
		BTN_PPSRA_PX-siz, BTN_PPSRA_PY, exp, wk->dat->font, PCOL_B_WHITE );
	GFL_STR_DeleteBuffer( str );

	GFL_STR_DeleteBuffer( exp );

	BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

// パラメータ表示座標
#define	P1_NAME_PX		( 32 )
#define	P1_NAME_PY		( 8-1 )
#define	P1_HP_PX		( 92 )//( 56 )
#define	P1_HP_PY		( 32 )
#define	P1_HP_SX		( 24 )
#define	P1_HP_SY		( 8 )
#define	P1_HPGAGE_PX	( 48+16 )
#define	P1_HPGAGE_PY	( 24 )
#define	P1_HPGAGE_SX	( 64 )
#define	P1_HPGAGE_SY	( 8 )
#define	P1_LV_PX		( 0 )
#define	P1_LV_PY		( 32 )

//--------------------------------------------------------------------------------------------
/**
 * ページ１のBMP表示
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_Page1BmpWrite( BPLIST_WORK * wk )
{
	s32	i;

	for( i=0; i<PokeParty_GetPokeCount(wk->dat->pp); i++ ){
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P1_POKE1+i].win), 0 );

		if( wk->poke[i].mons == 0 ){ continue; }

		BPL_NamePut( wk, WIN_P1_POKE1+i, i, P1_NAME_PX, P1_NAME_PY );

		if( wk->poke[i].egg == 0 ){
			BattlePokeList_P1_HPPut( wk, i );
		}

//		if( BadStatusIconAnmGet( wk->poke[i].pp ) != ST_ICON_NONE ){
		if( 0 ){
			continue;
		}

		BattlePokeList_P1_LvPut( wk, i );
	}

	if( wk->dat->mode == BPL_MODE_ITEMUSE ){
		BPL_PokeSelStrPut( wk, mes_b_plist_01_601 );
	}else{
		BPL_PokeSelStrPut( wk, mes_b_plist_01_600 );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ページ１のHP表示
 *
 * @param	wk		ワーク
 * @param	pos		並び位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_P1_HPPut( BPLIST_WORK * wk, u8 pos )
{
	GFL_BMP_Fill(
		GFL_BMPWIN_GetBmp(wk->add_win[WIN_P1_POKE1+pos].win), P1_HP_PX, P1_HP_PY, P1_HP_SX, P1_HP_SY, 0 );
	GFL_BMP_Fill(
		GFL_BMPWIN_GetBmp(wk->add_win[WIN_P1_POKE1+pos].win),
		P1_HPGAGE_PX, P1_HPGAGE_PY, P1_HPGAGE_SX, P1_HPGAGE_SY, 0 );
	BPL_HPPut( wk, WIN_P1_POKE1+pos, pos, P1_HP_PX, P1_HP_PY );
	BPL_HPGagePut( wk, WIN_P1_POKE1+pos, pos, P1_HPGAGE_PX, P1_HPGAGE_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * ページ１のLv表示
 *
 * @param	wk		ワーク
 * @param	pos		並び位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_P1_LvPut( BPLIST_WORK * wk, u8 pos )
{
	if( wk->poke[pos].egg == 0 ){
		BPL_LvPut( wk, WIN_P1_POKE1+pos, pos, P1_LV_PX, P1_LV_PY );
	}
}

#define	P_CHG_NAME_PX	( 0 )
#define	P_CHG_NAME_PY	( 8 )
static void BPL_IrekaeNamePut( BPLIST_WORK * wk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * 入れ替えページのBMP表示
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//-------------------------------------------------------------------------------------------
static void BPL_ChgPageBmpWrite( BPLIST_WORK * wk )
{
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_CHG_NAME].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_CHG_IREKAE].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_CHG_STATUS].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_CHG_WAZACHECK].win), 0 );

//	BPL_NamePut(
//		wk, WIN_CHG_NAME, FONT_TOUCH, wk->dat->sel_poke, P_CHG_NAME_PX, P_CHG_NAME_PY );

	BPL_IrekaeNamePut( wk, wk->dat->sel_poke );


	BPL_StrCommandPut( wk, WIN_CHG_IREKAE, mes_b_plist_02_501 );

	if( wk->poke[wk->dat->sel_poke].egg == 0 ){
		BPL_StrCommandPut( wk, WIN_CHG_STATUS, mes_b_plist_02_504 );
		BPL_StrCommandPut( wk, WIN_CHG_WAZACHECK, mes_b_plist_02_505 );
	}else{
//		BAPPTOOL_PrintQueOn( &wk->add_win[WIN_CHG_STATUS] );
//		BAPPTOOL_PrintQueOn( &wk->add_win[WIN_CHG_WAZACHECK] );
//		GFL_BMPWIN_MakeTransWindow_VBlank( wk->add_win[ WIN_CHG_STATUS ].win );
//		GFL_BMPWIN_MakeTransWindow_VBlank( wk->add_win[ WIN_CHG_WAZACHECK ].win );
	}
}

#define	NAME_CENTER_SPC_SX		( 8 )

//--------------------------------------------------------------------------------------------
/**
 * 入れ替えページの名前表示
 *
 * @param	wk		ワーク
 * @param	pos		並び位置
 *
 * @return	none
 */
//-------------------------------------------------------------------------------------------
static void BPL_IrekaeNamePut( BPLIST_WORK * wk, u32 pos )
{
	GFL_BMPWIN * win;
	BPL_POKEDATA * pd;
	STRBUF * exp;
	STRBUF * str;
	u8	name_siz;
	u8	sex_siz;
	u8	spc_siz;
	u8	px;

	win = wk->add_win[WIN_CHG_NAME].win;
	pd  = &wk->poke[pos];
	exp = GFL_STR_CreateBuffer( BUFLEN_POKEMON_NAME, wk->dat->heap );
	str = GFL_MSG_CreateString( wk->mman, NameMsgID_Tbl[pos] );

	WORDSET_RegisterPokeNickName( wk->wset, 0, pd->pp );
	WORDSET_ExpandStr( wk->wset, exp, str );
	GFL_STR_DeleteBuffer( str );

	str = NULL;
	if( pd->sex_put == 0 && pd->egg == 0 ){
		if( pd->sex == PTL_SEX_MALE ){
			str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_502 );
		}else if( pd->sex == PTL_SEX_FEMALE ){
			str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_503 );
		}
	}

	name_siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
	if( str == NULL ){
		sex_siz = 0;
		spc_siz = 0;
	}else{
		sex_siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
		spc_siz = NAME_CENTER_SPC_SX;
	}
	px = (GFL_BMPWIN_GetSizeX(win)*8-name_siz-sex_siz-spc_siz)/2;
/*↑[GS_CONVERT_TAG]*/

	PRINTSYS_PrintQueColor(
		wk->que, GFL_BMPWIN_GetBmp(win), px, P_CHG_NAME_PY-1, exp, wk->dat->font, PCOL_BTN );
	GFL_STR_DeleteBuffer( exp );

	if( str != NULL ){
		if( pd->sex == PTL_SEX_MALE ){
			PRINTSYS_PrintQueColor(
				wk->que, GFL_BMPWIN_GetBmp(win),
				px+name_siz+spc_siz, P_CHG_NAME_PY, str, wk->dat->font, PCOL_B_BLUE );
		}else{
			PRINTSYS_PrintQueColor(
				wk->que, GFL_BMPWIN_GetBmp(win),
				px+name_siz+spc_siz, P_CHG_NAME_PY, str, wk->dat->font, PCOL_B_RED );
		}
		GFL_STR_DeleteBuffer( str );
	}

	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_CHG_NAME] );
}




#define	STW_NAME_PX		( 0 )
#define	STW_NAME_PY		( 0 )
#define	STW_WAZANAME_PY	( 8-1 )
#define	P2_PPNUM_PX		( 0 )
#define	P2_PPNUM_PY		( 0 )
#define	P2_PP_PX		( 0 )
#define	P2_PP_PY		( 0 )

//--------------------------------------------------------------------------------------------
/**
 * ステータス技選択ページのBMP表示
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_StWazaSelPageBmpWrite( BPLIST_WORK * wk )
{
	BPL_POKEWAZA * waza;
	u16	i, swap;

	swap = WIN_STW_NAME_S * wk->bmp_swap;

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_STW_NAME+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_STW_SKILL1+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_STW_SKILL2+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_STW_SKILL3+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_STW_SKILL4+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_STW_STATUS].win), 0 );

	BPL_NamePut( wk, WIN_STW_NAME+swap, wk->dat->sel_poke, STW_NAME_PX, STW_NAME_PY );

	for( i=0; i<4; i++ ){
		waza = &wk->poke[wk->dat->sel_poke].waza[i];
		if( waza->id == 0 ){ continue; }

		BPL_WazaNamePut(
			wk, waza->id, WIN_STW_SKILL1+swap+i,
			WazaMsgID_Tbl[i], STW_WAZANAME_PY, PCOL_BTN );

		BPL_WazaButtonPPPut( wk, waza, WIN_STW_SKILL1+swap+i );
	}

	BPL_StrCommandPut( wk, WIN_STW_STATUS, mes_b_plist_02_504 );

	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_STW_SKILL1+swap] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_STW_SKILL2+swap] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_STW_SKILL3+swap] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_STW_SKILL4+swap] );

	wk->bmp_swap ^= 1;
}

#define	P3_NAME_PX		( 0 )
#define	P3_NAME_PY		( 0 )
#define	P3_HPGAGE_PX	( 0 )
#define	P3_HPGAGE_PY	( 0 )

//--------------------------------------------------------------------------------------------
/**
 * ステータスメインページのBMP表示
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_StMainPageBmpWrite( BPLIST_WORK * wk )
{
	u32	swap;

	swap = WIN_P3_NAME_S * wk->bmp_swap;

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_LV].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_NEXT].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_POW].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_DEF].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_AGI].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPP].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPD].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_HP].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_WAZACHECK].win), 0 );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_NAME+swap ].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_HPGAGE+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_LVNUM+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_NEXTNUM+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_POWNUM+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_DEFNUM+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_AGINUM+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPPNUM+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPDNUM+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_HPNUM+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPANAME+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPAINFO+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_ITEMNAME+swap].win), 0 );

	BPL_NamePut( wk, WIN_P3_NAME+swap, wk->dat->sel_poke, P3_NAME_PX, P3_NAME_PY );
	BPL_P3_HPPut( wk, wk->dat->sel_poke );
	BPL_HPGagePut( wk, WIN_P3_HPGAGE+swap, wk->dat->sel_poke, P3_HPGAGE_PX, P3_HPGAGE_PY );
	BPL_P3_LvPut( wk, wk->dat->sel_poke );
	BPL_P3_PowPut( wk, wk->dat->sel_poke );
	BPL_P3_DefPut( wk, wk->dat->sel_poke );
	BPL_P3_AgiPut( wk, wk->dat->sel_poke );
	BPL_P3_SppPut( wk, wk->dat->sel_poke );
	BPL_P3_SpdPut( wk, wk->dat->sel_poke );
	BPL_TokuseiPut( wk, WIN_P3_SPANAME+swap, wk->dat->sel_poke );
	BPL_ItemPut( wk, WIN_P3_ITEMNAME+swap, wk->dat->sel_poke );
	BPL_P3_TokuseiInfoPut( wk, wk->dat->sel_poke );
	BPL_StrCommandPut( wk, WIN_P3_WAZACHECK, mes_b_plist_02_505 );

	wk->bmp_swap ^= 1;
}

#define	P4_NAME_PX		( 0 )
#define	P4_NAME_PY		( 0 )
#define	P4_WAZANAME_PY	( 0 )
#define	P4_PP_PX		( 0 )
#define	P4_PP_PY		( 0 )

//--------------------------------------------------------------------------------------------
/**
 * ステータス技詳細ページのBMP表示
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_StWazaInfoPageBmpWrite( BPLIST_WORK * wk )
{
	BPL_POKEWAZA * waza;
	u32	swap;

	swap = WIN_P4_SKILL_S * wk->bmp_swap;

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_NAME].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_PP].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_HIT].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_POW].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_BUNRUI].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_PPNUM+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_SKILL+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_HITNUM+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_POWNUM+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_BRNAME+swap].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_INFO+swap].win), 0 );

	waza = &wk->poke[wk->dat->sel_poke].waza[wk->dat->sel_wp];

	BPL_NamePut( wk, WIN_P4_NAME, wk->dat->sel_poke, P4_NAME_PX, P4_NAME_PY );
	BPL_PPPut( wk, WIN_P4_PP, P4_PP_PX, P4_PP_PY );
	BPL_WazaNamePut(
		wk, waza->id, WIN_P4_SKILL+swap, 
		WazaMsgID_Tbl[wk->dat->sel_wp], P4_WAZANAME_PY, PCOL_N_WHITE );
	BPL_WazaHitStrPut( wk, WIN_P4_HIT );
	BPL_WazaHitNumPut( wk, WIN_P4_HITNUM+swap, waza->hit );
	BPL_WazaPowStrPut( wk, WIN_P4_POW );
	BPL_WazaPowNumPut( wk, WIN_P4_POWNUM+swap, waza->pow );
	BPL_WazaInfoPut( wk, WIN_P4_INFO+swap, waza->id );
	BPL_WazaBunruiStrPut( wk, WIN_P4_BUNRUI );
	BPL_WazaKindPut( wk, WIN_P4_BRNAME+swap, waza->kind );
	BPL_WazaPPPut( wk, WIN_P4_PPNUM+swap, waza->pp, waza->mpp );

	wk->bmp_swap ^= 1;
}

#define	P5_NAME_PX		( 0 )
#define	P5_NAME_PY		( 0 )
#define	P5_WAZANAME_PY	( 8-1 )

//--------------------------------------------------------------------------------------------
/**
 * 技忘れ選択ページのBMP表示
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaDelSelPageBmpWrite( BPLIST_WORK * wk )
{
	BPL_POKEWAZA * waza;
	u32	i;

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P5_NAME].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P5_SKILL1].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P5_SKILL2].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P5_SKILL3].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P5_SKILL4].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P5_SKILL5].win), 0 );

	BPL_NamePut( wk, WIN_P5_NAME, wk->dat->sel_poke, P5_NAME_PX, P5_NAME_PY );

	for( i=0; i<4; i++ ){
		waza = &wk->poke[wk->dat->sel_poke].waza[i];
		if( waza->id == 0 ){ continue; }

		BPL_WazaNamePut(
			wk, waza->id, WIN_P5_SKILL1+i, 
			WazaMsgID_Tbl[i], P5_WAZANAME_PY, PCOL_BTN );

		BPL_WazaButtonPPPut( wk, waza, WIN_P5_SKILL1+i );
	}

	BPL_WazaNamePut(
		wk, wk->dat->chg_waza, WIN_P5_SKILL5, 
		WazaMsgID_Tbl[4], P5_WAZANAME_PY, PCOL_BTN );
	{
		BPL_POKEWAZA	tmp;

		tmp.pp  = WT_WazaDataParaGet( wk->dat->chg_waza, ID_WTD_pp );
		tmp.mpp = tmp.pp;
		BPL_WazaButtonPPPut( wk, &tmp, WIN_P5_SKILL5 );
	}

	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P5_SKILL1] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P5_SKILL2] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P5_SKILL3] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P5_SKILL4] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P5_SKILL5] );
}

#define	P6_NAME_PX		( 0 )
#define	P6_NAME_PY		( 0 )
#define	P6_WAZANAME_PY	( 0 )
#define	P6_PP_PX		( 0 )
#define	P6_PP_PY		( 0 )

//--------------------------------------------------------------------------------------------
/**
 * ページ６のBMP表示
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_Page6BmpWrite( BPLIST_WORK * wk )
{
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_NAME].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_PP].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_PPNUM].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_SKILL].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_HIT].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_HITNUM].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_POW].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_POWNUM].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_BUNRUI].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_BRNAME].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_INFO].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_WASURERU].win), 0 );

	BPL_NamePut( wk, WIN_P6_NAME, wk->dat->sel_poke, P6_NAME_PX, P6_NAME_PY );
	BPL_PPPut( wk, WIN_P6_PP, P6_PP_PX, P6_PP_PY );
	BPL_WazaHitStrPut( wk, WIN_P6_HIT );
	BPL_WazaPowStrPut( wk, WIN_P6_POW );
	BPL_WazaBunruiStrPut( wk, WIN_P6_BUNRUI );

	if( wk->dat->sel_wp < 4 ){
		BPL_POKEWAZA * waza = &wk->poke[wk->dat->sel_poke].waza[wk->dat->sel_wp];

		BPL_WazaNamePut(
			wk, waza->id, WIN_P6_SKILL, 
			WazaMsgID_Tbl[wk->dat->sel_wp], P6_WAZANAME_PY, PCOL_N_WHITE );
		BPL_WazaHitNumPut( wk, WIN_P6_HITNUM, waza->hit );
		BPL_WazaPowNumPut( wk, WIN_P6_POWNUM, waza->pow );
		BPL_WazaInfoPut( wk, WIN_P6_INFO, waza->id );
		BPL_WazaKindPut( wk, WIN_P6_BRNAME, waza->kind );
		BPL_WazaPPPut( wk, WIN_P6_PPNUM, waza->pp, waza->mpp );
	}else{
		u32	pp = WT_WazaDataParaGet( wk->dat->chg_waza, ID_WTD_pp );

		BPL_WazaNamePut(
			wk, wk->dat->chg_waza, WIN_P6_SKILL, 
			WazaMsgID_Tbl[4], P6_WAZANAME_PY, PCOL_N_WHITE );
		BPL_WazaInfoPut( wk, WIN_P6_INFO, wk->dat->chg_waza );
		BPL_WazaHitNumPut(
			wk, WIN_P6_HITNUM, WT_WazaDataParaGet(wk->dat->chg_waza,ID_WTD_hitprobability) );
		BPL_WazaPowNumPut(
			wk, WIN_P6_POWNUM, WT_WazaDataParaGet(wk->dat->chg_waza,ID_WTD_damage) );
		BPL_WazaKindPut(
			wk, WIN_P6_BRNAME, WT_WazaDataParaGet(wk->dat->chg_waza,ID_WTD_kind) );
		BPL_WazaPPPut( wk, WIN_P6_PPNUM, pp, pp );
	}

	BPL_WasureruStrPut( wk, WIN_P6_WASURERU );
}

#define	P7_NAME_PX		( 0 )
#define	P7_NAME_PY		( 0 )
#define	P7_WAZANAME_PY	( 8-1 )

//--------------------------------------------------------------------------------------------
/**
 * 技回復選択ページのBMP表示
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_PPRcvPageBmpWrite( BPLIST_WORK * wk )
{
	BPL_POKEWAZA * waza;
	u32	i;

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P7_NAME].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P7_SKILL1].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P7_SKILL2].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P7_SKILL3].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P7_SKILL4].win), 0 );

	BPL_NamePut( wk, WIN_P7_NAME, wk->dat->sel_poke, P7_NAME_PX, P7_NAME_PY );

	for( i=0; i<4; i++ ){
		waza = &wk->poke[wk->dat->sel_poke].waza[i];
		if( waza->id == 0 ){ continue; }

		BPL_WazaNamePut(
			wk, waza->id, WIN_P7_SKILL1+i, 
			WazaMsgID_Tbl[i], P7_WAZANAME_PY, PCOL_BTN );
		BPL_WazaButtonPPPut( wk, waza, WIN_P7_SKILL1+i );
	}

	if( ITEM_GetParam( wk->dat->item, ITEM_PRM_ALL_PP_RCV, wk->dat->heap ) == 0 ){
		BPL_PokeSelStrPut( wk, mes_b_plist_m19 );
	}

	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P7_SKILL1] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P7_SKILL2] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P7_SKILL3] );
	BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P7_SKILL4] );
}

//--------------------------------------------------------------------------------------------
/**
 * 技回復選択ページのBMP表示
 *
 * @param	wk		ワーク
 * @param	widx	ウィンドウインデックス
 * @param	pos		技位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_PPRcv( BPLIST_WORK * wk, u16 widx, u16 pos )
{
/*
	BPL_POKEWAZA * waza;

	GFL_BMP_Clear( &wk->add_win[ widx ], 0 );
	waza = &wk->poke[wk->dat->sel_poke].waza[pos];
	BPL_WazaNamePut(
		wk, waza->id, widx, 
		WazaMsgID_Tbl[pos], FONT_TOUCH, P7_WAZANAME_PY, PCOL_BTN );
	BPL_WazaButtonPPPut( wk, waza, widx );
	GFL_BMPWIN_MakeTransWindow_VBlank( wk->add_win[ widx ] );
*/
	BPL_POKEWAZA * waza = &wk->poke[wk->dat->sel_poke].waza[pos];

	BPL_WazaButtonPPRcv( wk, waza, widx );
}

#define	P8_NAME_PX		( 0 )
#define	P8_NAME_PY		( 0 )
#define	P8_WAZANAME_PY	( 0 )
#define	P8_PP_PX		( 0 )
#define	P8_PP_PY		( 0 )

//--------------------------------------------------------------------------------------------
/**
 * ページ８のBMP表示
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_Page8BmpWrite( BPLIST_WORK * wk )
{
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P8_NAME].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P8_PP].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P8_PPNUM].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P8_SKILL].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P8_APP].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P8_INFO].win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P8_WASURERU].win), 0 );

	BPL_NamePut( wk, WIN_P8_NAME, wk->dat->sel_poke, P8_NAME_PX, P8_NAME_PY );
	BPL_PPPut( wk, WIN_P8_PP, P8_PP_PX, P8_PP_PY );

	{
		STRBUF * str;

		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_400 );
		PRINTSYS_PrintQueColor(
			wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P8_APP].win), 0, 0, str, wk->dat->font, PCOL_N_WHITE );
		GFL_STR_DeleteBuffer( str );
		BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P8_APP] );
	}

	if( wk->dat->sel_wp < 4 ){
		BPL_POKEWAZA * waza = &wk->poke[wk->dat->sel_poke].waza[wk->dat->sel_wp];

		BPL_WazaNamePut(
			wk, waza->id, WIN_P8_SKILL, 
			WazaMsgID_Tbl[wk->dat->sel_wp], P8_WAZANAME_PY, PCOL_N_WHITE );
//		BPL_ContestWazaInfoPut( wk, WIN_P8_INFO, waza->id );

/*
		BPL_WazaHitNumPut( wk, WIN_P6_HITNUM, waza->hit );
		BPL_WazaPowNumPut( wk, WIN_P6_POWNUM, waza->pow );
		BPL_WazaKindPut( wk, WIN_P6_BRNAME, waza->kind );
*/
		BPL_WazaPPPut( wk, WIN_P8_PPNUM, waza->pp, waza->mpp );

	}else{
		u32	pp = WT_WazaDataParaGet( wk->dat->chg_waza, ID_WTD_pp );

		BPL_WazaNamePut(
			wk, wk->dat->chg_waza, WIN_P8_SKILL, 
			WazaMsgID_Tbl[4], P8_WAZANAME_PY, PCOL_N_WHITE );
//		BPL_ContestWazaInfoPut( wk, WIN_P8_INFO, wk->dat->chg_waza );

/*
		BPL_WazaHitNumPut(
			wk, WIN_P6_HITNUM, WT_WazaDataParaGet(wk->dat->chg_waza,ID_WTD_hitprobability) );
		BPL_WazaPowNumPut(
			wk, WIN_P6_POWNUM, WT_WazaDataParaGet(wk->dat->chg_waza,ID_WTD_damage) );
		BPL_WazaKindPut(
			wk, WIN_P6_BRNAME, WT_WazaDataParaGet(wk->dat->chg_waza,ID_WTD_kind) );
*/
		BPL_WazaPPPut( wk, WIN_P8_PPNUM, pp, pp );
	}

	BPL_WasureruStrPut( wk, WIN_P8_WASURERU );
}


//--------------------------------------------------------------------------------------------
/**
 * メッセージ表示
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_TalkMsgSet( BPLIST_WORK * wk )
{
//	BmpWinFrame_Write( &wk->win[WIN_TALK], WINDOW_TRANS_OFF, TALK_WIN_CGX_POS, BPL_PAL_TALK_WIN );
	BmpWinFrame_Write( wk->win[WIN_TALK].win, WINDOW_TRANS_OFF, TALK_WIN_CGX_POS, BPL_PAL_TALK_WIN );
//	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[WIN_TALK]), FBMP_COL_WHITE );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[WIN_TALK].win), 15 );
	BattlePokeList_TalkMsgStart( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * メッセージ表示開始
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_TalkMsgStart( BPLIST_WORK * wk )
{
	wk->stream = PRINTSYS_PrintStream(
									wk->win[WIN_TALK].win,
									0, 0, wk->msg_buf, wk->dat->font,
									MSGSPEED_GetWait(),
									wk->tcbl,
									10,		// tcbl pri
									wk->dat->heap,
									15 );	// clear color

	GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_TALK].win );

/*
	MsgPrintSkipFlagSet( MSG_SKIP_ON );
	wk->midx = GF_STR_PrintSimple(
				&wk->win[WIN_TALK], FONT_TALK, wk->msg_buf,
				0, 0, BattleWorkConfigMsgSpeedGet(wk->dat->bw), NULL );
*/
}





#define	BPL_RCV_SLEEP		( 0x01 )	// 眠り
#define	BPL_RCV_POISON		( 0x02 )	// 毒
#define	BPL_RCV_BURN		( 0x04 )	// 火傷
#define	BPL_RCV_ICE			( 0x08 )	// 氷
#define	BPL_RCV_PARALYZE	( 0x10 )	// 麻痺
#define	BPL_RCV_PANIC		( 0x20 )	// 混乱
#define	BPL_RCV_MEROMERO	( 0x40 )	// メロメロ
#define	BPL_RCV_ALL			( 0x7f )	// 全快


//--------------------------------------------------------------------------------------------
/**
 * アイテム使用メッセージセット
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_ItemUseMsgSet( BPLIST_WORK * wk )
{
/*
	POKEMON_PARAM * pp;
	BPLIST_DATA * dat;
	void * item;
	STRBUF * str;
	u16	hp;
	u8	prm;
	u8	i;

	dat  = wk->dat;
	item = ITEM_GetItemArcData( dat->item, ITEM_GET_DATA, dat->heap );
	pp   = BattleWorkPokemonParamGet( dat->bw, dat->client_no, dat->list_row[dat->sel_poke] );
	hp   = PokeParaGet( pp, ID_PARA_hp, NULL );
	prm  = 0;
	if( ITEM_GetBufParam( item, ITEM_PRM_SLEEP_RCV ) != 0 ){
		prm |= BPL_RCV_SLEEP;
	}
	if( ITEM_GetBufParam( item, ITEM_PRM_POISON_RCV ) != 0 ){
		prm |= BPL_RCV_POISON;
	}
	if( ITEM_GetBufParam( item, ITEM_PRM_BURN_RCV ) != 0 ){
		prm |= BPL_RCV_BURN;
	}
	if( ITEM_GetBufParam( item, ITEM_PRM_ICE_RCV ) != 0 ){
		prm |= BPL_RCV_ICE;
	}
	if( ITEM_GetBufParam( item, ITEM_PRM_PARALYZE_RCV ) != 0 ){
		prm |= BPL_RCV_PARALYZE;
	}
	if( ITEM_GetBufParam( item, ITEM_PRM_PANIC_RCV ) != 0 ){
		prm |= BPL_RCV_PANIC;
	}
	if( ITEM_GetBufParam( item, ITEM_PRM_MEROMERO_RCV ) != 0 ){
		prm |= BPL_RCV_MEROMERO;
	}

	// 瀕死回復
	if( wk->poke[dat->sel_poke].hp == 0 && hp != 0 ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m13 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	// HP回復
	}else if( wk->poke[dat->sel_poke].hp != hp ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m07 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
		WORDSET_RegisterNumber(
			wk->wset, 1, hp-wk->poke[dat->sel_poke].hp, 3,
			STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	// PP回復
	}else if( ITEM_GetBufParam( item, ITEM_PRM_PP_RCV ) != 0 ||
		ITEM_GetBufParam( item, ITEM_PRM_ALL_PP_RCV ) != 0 ){

		GFL_MSG_GetString( wk->mman, mes_b_plist_m12, wk->msg_buf );
	// 眠り
	}else if( prm == BPL_RCV_SLEEP ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m17 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	// 毒
	}else if( prm == BPL_RCV_POISON ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m08 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	// 火傷
	}else if( prm == BPL_RCV_BURN ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m10 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	// 氷
	}else if( prm == BPL_RCV_ICE ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m11 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	// 麻痺
	}else if( prm == BPL_RCV_PARALYZE ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m09 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	// 混乱
	}else if( prm == BPL_RCV_PANIC ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m15 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	// メロメロ
	}else if( prm == BPL_RCV_MEROMERO ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m16 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	// ステータス全快
//	}else if( prm == BPL_RCV_ALL ){
	}else{
		str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m14 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	}

	GFL_HEAP_FreeMemory( item );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * 技によるアイテム使用エラーメッセージセット
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_ItemUseSkillErrMsgSet( BPLIST_WORK * wk )
{
/*
	POKEMON_PARAM * pp;
	BPLIST_DATA * dat;
	STRBUF * str;

	dat = wk->dat;
	pp  = BattleWorkPokemonParamGet( dat->bw, dat->client_no, dat->list_row[dat->sel_poke] );
	str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m20 );
	WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
	WORDSET_RegisterWazaName( wk->wset, 1, WAZANO_SASIOSAE );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	GFL_STR_DeleteBuffer( str );
*/
}



void BPLISTBMP_PrintMain( BPLIST_WORK * wk )
{
	if( PRINTSYS_QUE_Main( wk->que ) == TRUE ){
		BAPPTOOL_PrintQueTrans( wk->win, WIN_MAX );
		BAPPTOOL_PrintQueTrans( wk->add_win, wk->bmp_add_max );
	}
}
