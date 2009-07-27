//============================================================================================
/**
 * @file	eventdata_type.h
 * @brief	イベント起動チェック用データ定義ヘッダ
 * @date	2005.10.14
 *
 * src/fielddata/eventdata/のイベントデータコンバータからも参照されている。
 * そのためこの中に記述できる内容にはかなり制限があることを注意すること。
 *
 * 2008.11.20	DPプロジェクトからコピー。なので上記注釈は今のところ無効
 * 2009.07.25 eventdata_sxy.hから分離
 */
//============================================================================================
#pragma once

//#include "field/eventdata_system.h"
//#include "field/location.h"
//#include "field/fldmmdl.h"

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
///特殊接続指定用ID
//------------------------------------------------------------------
#define EXIT_ID_NONE		(0xffff)

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef enum {
	EXIT_TYPE_NONE = 0,
	EXIT_TYPE_MAT,
	EXIT_TYPE_STAIRS,
	EXIT_TYPE_DOOR,
	EXIT_TYPE_WALL,
}EXIT_TYPE;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef enum {
	EXIT_DIR_NON = 0,
	EXIT_DIR_UP,
	EXIT_DIR_DOWN,
	EXIT_DIR_LEFT,
	EXIT_DIR_RIGHT,

	EXIT_DIR_MAX
}EXIT_DIR;


//------------------------------------------------------------------
//------------------------------------------------------------------
typedef enum{
  BG_TALK_TYPE_NORMAL,  //話し掛けイベント
  BG_TALK_TYPE_BOARD, //看板イベント
  BG_TALK_TYPE_HIDE,  //隠しアイテム
}BG_TALK_TYPE;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef enum{
  BG_TALK_DIR_DOWN = 0,
  BG_TALK_DIR_LEFT,
  BG_TALK_DIR_RIGHT,
  BG_TALK_DIR_UP,
  BG_TALK_DIR_ALL,
  BG_TALK_DIR_SIDE,
  BG_TALK_DIR_UPDOWN,
}BG_TALK_DIR;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _EVENTDATA_HEADER EVENTDATA_HEADER;

//------------------------------------------------------------------
///ドアリンク用データ構造体
//------------------------------------------------------------------
typedef struct _CONNECT_DATA CONNECT_DATA;

//------------------------------------------------------------------
///BG話かけデータ構造体
//------------------------------------------------------------------
typedef struct _BG_TALK_DATA BG_TALK_DATA;

//------------------------------------------------------------------
///POS発動イベントデータ構造体
//------------------------------------------------------------------
typedef struct _POS_EVENT_DATA POS_EVENT_DATA;

