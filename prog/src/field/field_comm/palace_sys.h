//==============================================================================
/**
 * @file    palace_sys.h
 * @brief   フィールド通信：パレスのヘッダ
 * @author  matsuda
 * @date    2009.05.01(金)
 */
//==============================================================================
#pragma once

//==============================================================================
//  型定義
//==============================================================================
///パレスシステムワーク構造体への不定形ポインタ
typedef struct _PALACE_SYS_WORK * PALACE_SYS_PTR;


//==============================================================================
//  外部関数宣言
//==============================================================================
extern PALACE_SYS_PTR PALACE_SYS_Alloc(HEAPID heap_id);
extern void PALACE_SYS_Free(PALACE_SYS_PTR palace);
extern void PALACE_SYS_Update(PALACE_SYS_PTR palace, PLAYER_WORK *plwork, FIELD_PLAYER *fldply);
extern int PALACE_SYS_GetArea(PALACE_SYS_PTR palace);
