//============================================================================================
/**
 * @file	field_buildmodel.h
 * @brief		配置モデルの制御
 * @author	tamada GAMEFREAK inc.
 * @date	2009.04.24
 */
//============================================================================================
#pragma once

#include <gflib.h>

#include "field/areadata.h"
#include "field_g3d_mapper.h"
#include "fieldmap_resist.h"

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief 配置モデルIDの型定義
 */
//------------------------------------------------------------------
typedef u16 BMODEL_ID;
//------------------------------------------------------------------
//  配置モデルマネジャー
//------------------------------------------------------------------
typedef struct _FIELD_BMODEL_MAN FIELD_BMODEL_MAN;

//------------------------------------------------------------------
//------------------------------------------------------------------
extern FIELD_BMODEL_MAN * FIELD_BMODEL_MAN_Create(HEAPID heapID);

extern void FIELD_BMODEL_MAN_Delete(FIELD_BMODEL_MAN * man);

extern void FIELD_BMODEL_MAN_Main(FIELD_BMODEL_MAN * man);

//------------------------------------------------------------------
//  マップデータの読み込み処理
//------------------------------------------------------------------
extern void FIELD_BMODEL_MAN_Load(FIELD_BMODEL_MAN * man, u16 zoneid, const AREADATA * areadata);

extern const FLDMAPPER_RESISTDATA_OBJTBL * FIELD_BMODEL_MAN_GetOBJTBL(const FIELD_BMODEL_MAN * man);

//------------------------------------------------------------------
//  配置モデルIDを登録済み配置モデルのインデックスに変換する
//------------------------------------------------------------------
extern u16 FIELD_BMODEL_MAN_GetEntryIndex(const FIELD_BMODEL_MAN* man, BMODEL_ID id);


//============================================================================================
//    電光掲示板関連
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief 電光掲示板指定のための定義
 */
//------------------------------------------------------------------
typedef enum{ 
  FIELD_BMODEL_ELBOARD_ID1 = 0,
  FIELD_BMODEL_ELBOARD_ID2,

  FIELD_BMODEL_ELBOARD_ID_MAX,
}FIELD_BMODEL_ELBOARD_ID;

//------------------------------------------------------------------
/**
 * @brief 電光掲示板用文字列登録処理
 */
//------------------------------------------------------------------
extern void FIELD_BMODEL_MAN_EntryELString(const FIELD_BMODEL_MAN* man,
    FIELD_BMODEL_ELBOARD_ID id,
    const STRBUF* str);

extern void FIELD_BMODEL_MAN_EntryELStringID(const FIELD_BMODEL_MAN * man,
    FIELD_BMODEL_ELBOARD_ID id,
    ARCID msg_arc_id, u16 str_id);


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//  ↓このヘッダで宣言しているが実態はfield_g3d_mapper.cにある
//------------------------------------------------------------------
extern FIELD_BMODEL_MAN * FLDMAPPER_GetBuildModelManager( FLDMAPPER* g3Dmapper);

