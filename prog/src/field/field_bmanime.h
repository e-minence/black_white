//============================================================================================
/**
 * @file  field_bmanime.h
 * @brief フィールド配置モデルアニメ制御
 * @date  2009.05.11
 * @author  tamada GAME FREAK inc.
 */
//============================================================================================

#pragma once

//============================================================================================
//============================================================================================

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _FIELD_BMANIME_DATA FIELD_BMANIME_DATA;

//------------------------------------------------------------------
/**
 * @brief 配置モデルアニメのタイプ指定
 */
//------------------------------------------------------------------
typedef enum {  
  BMANIME_TYPE_NONE = 0,    ///<アニメなし
  BMANIME_TYPE_ETERNAL,     ///<常に適用
  BMANIME_TYPE_EVENT,       ///<イベントで適用

  BMANIME_TYPE_MAX
}BMANIME_TYPE;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef enum {  
  BMANIME_PROG_TYPE_NONE = 0,

  BMANIME_PROG_TYPE_MAX,
}BMANIME_PROG_TYPE;


//------------------------------------------------------------------
//------------------------------------------------------------------
extern const FIELD_BMANIME_DATA * FIELD_BMODEL_MAN_GetAnimeData(FIELD_BMODEL_MAN * man, u16 bm_id);

extern const u16 * FIELD_BMANIME_DATA_getAnimeFileID(const FIELD_BMANIME_DATA * data);
extern u32 FIELD_BMANIME_DATA_getAnimeCount(const FIELD_BMANIME_DATA * data);
extern BMANIME_TYPE FIELD_BMANIME_DATA_getAnimeType(const FIELD_BMANIME_DATA * data);
extern BMANIME_PROG_TYPE FIELD_BMANIME_DATA_getProgType(const FIELD_BMANIME_DATA * data);

