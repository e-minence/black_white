//============================================================================
/**
 *
 *	@file		demo3d_mb.h
 *	@brief  モーションブラー
 *	@author	hosaka genya(HGSSからnakamura hiroyukiさんのソースを移植)
 *	@data		2010.01.19
 *
 */
//============================================================================
#pragma once

typedef struct _DEMO3D_MBL_WORK DEMO3D_MBL_WORK;

//=============================================================================
/**
 *								定数定義
 */
//=============================================================================

#define ENCOUNT_LEGEND_PACKAGE_MOTIONBL_EVA	( 5 )	// モーションブラー係数　新しくブレンドされる絵
#define ENCOUNT_LEGEND_PACKAGE_MOTIONBL_EAB	( 13 )	// モーションブラー係数　すでにバッファされている絵

//=============================================================================
/**
 *								構造体定義
 */
//=============================================================================

//=============================================================================
/**
 *								EXTERN宣言
 */
//=============================================================================

extern DEMO3D_MBL_WORK * DEMO3D_MotionBlInit( GFL_TCBSYS* tcbsys, int eva, int evb );
extern void DEMO3D_MotionBlExit( DEMO3D_MBL_WORK * mb );
