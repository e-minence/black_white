///////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  矢印
 * @file   arrow.h
 * @author obata
 * @date   2010.02.22
 */
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>


//=====================================================================================
// ■矢印の不完全型
//=====================================================================================
typedef struct _ARROW ARROW;


//=====================================================================================
// ■表示パラメータ
//=====================================================================================
typedef struct {
  u32 cgrIndex;      // OBJキャラデータの登録インデックス
  u32 plttIndex;     // OBJパレットデータの登録インデックス
  u32 cellAnimIndex; // OBJセルアニメデータの登録インデックス
  u16 setSerface;    // サーフェイス指定 ( CLSYS_DEFREND_MAIN/ CLSYS_DEFREND_SUB )
  u16 anmseqH;       // アニメーションシーケンス ( 横に伸びる )
  u16 anmseqV;       // アニメーションシーケンス ( 縦に伸びる )
  u16 anmseqCorner;  // アニメーションシーケンス ( コーナー )
  u16 anmseqEnd;     // アニメーションシーケンス ( 終点 )
} ARROW_DISP_PARAM;



//=====================================================================================
// ■生成・破棄
//=====================================================================================
extern ARROW* ARROW_Create( HEAPID heapID, const ARROW_DISP_PARAM* dispParam );
extern void ARROW_Delete( ARROW* arrow ); 

//=====================================================================================
// ■制御
//=====================================================================================
extern void ARROW_Main( ARROW* arrow );
extern void ARROW_Setup( ARROW* arrow, int startX, int startY, int endX, int endY );
extern void ARROW_StartAnime( ARROW* arrow );
extern void ARROW_Vanish( ARROW* arrow );
