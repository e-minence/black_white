//======================================================================
/**
 * @file	sta_local_def.h
 * @brief	ステージ用　定義
 * @author	ariizumi
 * @data	09/03/06
 */
//======================================================================
#ifndef STA_LOCAL_DEF_H__
#define STA_LOCAL_DEF_H__

#include "musical/musical_define.h"
#include "musical/comm/mus_comm_func.h"

//BG定義
#define ACT_FRAME_MAIN_3D   GFL_BG_FRAME0_M
#define ACT_FRAME_MAIN_FONT   GFL_BG_FRAME1_M
#define ACT_FRAME_MAIN_CURTAIN  GFL_BG_FRAME2_M
#define ACT_FRAME_MAIN_BG   GFL_BG_FRAME3_M

#define ACT_FRAME_SUB_AUDI_FACE    GFL_BG_FRAME0_S
#define ACT_FRAME_SUB_BG    GFL_BG_FRAME1_S
#define ACT_FRAME_SUB_AUDI_NECK    GFL_BG_FRAME2_S
#define ACT_FRAME_SUB_INFO    GFL_BG_FRAME3_S


//BBD用座標変換(カメラの幅から計算
#define ACT_POS_X(val)	FX32_CONST((val)/16.0f)
#define ACT_POS_Y(val)	FX32_CONST((192.0f-(val))/16.0f)
#define ACT_POS_X_FX(val)	((val)/16)
#define ACT_POS_Y_FX(val)	(FX32_CONST(192.0f)-(val))/16

//BBD用サイズ変換
//BBDが32を単位としてサイズを測る。しかも背景表示のために全体を４ばいしているので
#define ACT_BBD_SIZE(val) FX16_CONST( val/32.0f/4.0f )

//オブジェクト関係
#define ACT_OBJECT_MAX (5)

//エフェクト関係
#define ACT_EFFECT_MAX (3)

//スポットライト関係
#define ACT_LIGHT_MAX	(4)

//3D関係(ポリゴンID
#define ACT_POLYID_LIGHT (4)
#define ACT_POLYID_SHADOW (3)
//エフェクトは5〜63

//幕関係
#define ACT_CURTAIN_SCROLL_SPEED (2)
#define ACT_CURTAIN_SCROLL_MIN (0)
#define ACT_CURTAIN_SCROLL_MAX (224)

struct _STAGE_INIT_WORK
{
  MUS_COMM_WORK *commWork;
	MUSICAL_POKE_PARAM musPoke[MUSICAL_POKE_MAX];
	u16		repertorie;	//演目
};

#endif STA_LOCAL_DEF_H__
