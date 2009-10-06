//======================================================================
/**
 * @file	app_menu_common.h
 * @brief	共通素材関係ソース
 * @author	ariizumi
 * @data	09/07/14
 *
 * モジュール名：APP_COMMON
 */
//======================================================================

#pragma once

#include "poke_tool/poke_tool.h"
#include "poke_tool/poketype.h"

//==============================================================================
//	定数定義
//==============================================================================
//バーのボタンのアニメインデックス
typedef enum
{
  APP_COMMON_BARICON_EXIT,
  APP_COMMON_BARICON_RETURN,
  APP_COMMON_BARICON_CURSOR_DOWN,
  APP_COMMON_BARICON_CURSOR_UP,
  APP_COMMON_BARICON_CURSOR_LEFT,
  APP_COMMON_BARICON_CURSOR_RIGHT,
  APP_COMMON_BARICON_CHECK_OFF,
  APP_COMMON_BARICON_CHECK_ON,

  APP_COMMON_BARICON_EXIT_ON,
  APP_COMMON_BARICON_RETURN_ON,
  APP_COMMON_BARICON_CURSOR_DOWN_ON,
  APP_COMMON_BARICON_CURSOR_UP_ON,
  APP_COMMON_BARICON_CURSOR_LEFT_ON,
  APP_COMMON_BARICON_CURSOR_RIGHT_ON,
  
  APP_COMMON_BARICON_EXIT_OFF,
  APP_COMMON_BARICON_RETURN_OFF,
  APP_COMMON_BARICON_CURSOR_DOWN_OFF,
  APP_COMMON_BARICON_CURSOR_UP_OFF,
  APP_COMMON_BARICON_CURSOR_LEFT_OFF,
  APP_COMMON_BARICON_CURSOR_RIGHT_OFF,

  APP_COMMON_BARICON_MAX,
}APP_COMMON_BARICON;

//バーのボタンのパレット本数
#define APP_COMMON_BARICON_PLT_NUM	(3)

//バーのパレット本数
#define APP_COMMON_BAR_PLT_NUM	(1)

///アイコンタイプ(技タイプ以外のタイプ)
enum{
  ICONTYPE_STYLE = POKETYPE_MAX,  ///<格好よさ
  ICONTYPE_BEAUTIFUL,             ///<美しさ
  ICONTYPE_CUTE,                  ///<可愛さ
  ICONTYPE_INTELLI,               ///<賢さ
  ICONTYPE_STRONG,                ///<逞しさ
};

//ポケモンタイプのパレットの本数
#define APP_COMMON_POKETYPE_PLT_NUM (3)

//セルの読みわけに使うマッピングモード定義
typedef enum
{
  APP_COMMON_MAPPING_32K,
  APP_COMMON_MAPPING_64K,
  APP_COMMON_MAPPING_128K,
}APP_COMMON_MAPPING;

// 状態異常アイコンのアニメ番号
enum {
	APP_COMMON_ST_ICON_POKERUS = 0,	// ポケルス
	APP_COMMON_ST_ICON_MAHI,				// 麻痺
	APP_COMMON_ST_ICON_KOORI,				// 氷
	APP_COMMON_ST_ICON_NEMURI,			// 眠り
	APP_COMMON_ST_ICON_DOKU,				// 毒
	APP_COMMON_ST_ICON_YAKEDO,			// 火傷
	APP_COMMON_ST_ICON_HINSI,				// 瀕死
	APP_COMMON_ST_ICON_NONE					// なし（アニメ番号的にもなし）
};


//ARCIDの取得
extern const u32 APP_COMMON_GetArcId(void);

//タイプ系素材取得
extern const u32 APP_COMMON_GetPokeTypePltArcIdx( void );
extern const u8  APP_COMMON_GetPokeTypePltOffset( const PokeType type );
extern const u32 APP_COMMON_GetPokeTypeCharArcIdx( const PokeType type );
extern const u32 APP_COMMON_GetPokeTypeCellArcIdx( const APP_COMMON_MAPPING mapping );
extern const u32 APP_COMMON_GetPokeTypeAnimeArcIdx( const APP_COMMON_MAPPING mapping );

//技分類系素材取得(パレット・セル・アニメはタイプと共通
extern const u8  APP_COMMON_GetWazaKindPltOffset( const PokeType type );
extern const u32 APP_COMMON_GetWazaKindCharArcIdx( const PokeType type );

//下部バーボタン系素材取得
extern const u32 APP_COMMON_GetBarIconPltArcIdx( void );
extern const u8  APP_COMMON_GetBarIconPltOffset( const APP_COMMON_BARICON type );
extern const u32 APP_COMMON_GetBarIconCharArcIdx();
extern const u32 APP_COMMON_GetBarIconCellArcIdx( const APP_COMMON_MAPPING mapping );
extern const u32 APP_COMMON_GetBarIconAnimeArcIdx( const APP_COMMON_MAPPING mapping );

//下部バー素材取得
extern const u32 APP_COMMON_GetBarPltArcIdx( void );
extern const u32 APP_COMMON_GetBarCharArcIdx( void );
extern const u32 APP_COMMON_GetBarScrnArcIdx( void );

// アイテムアイコン取得（ポケアイコン用）
extern const u32 APP_COMMON_GetPokeItemIconPltArcIdx( void );
extern const u32 APP_COMMON_GetPokeItemIconCharArcIdx( void );
extern const u32 APP_COMMON_GetPokeItemIconCellArcIdx( void );
extern const u32 APP_COMMON_GetPokeItemIconAnimeArcIdx( void ); 

//4 x 4 空データ取得（テクスチャ転送用）
extern const u32 APP_COMMON_GetNull4x4PltArcIdx( void );
extern const u32 APP_COMMON_GetNull4x4CharArcIdx( void );
extern const u32 APP_COMMON_GetNull4x4CellArcIdx( void );
extern const u32 APP_COMMON_GetNull4x4AnimeArcIdx( void );

// 状態異常アイコン
extern u32 APP_COMMON_GetStatusIconAnime( POKEMON_PARAM * pp );

//ポケモン絵OBJのためのダミーセル取得
extern u32 APP_COMMON_GetDummyCellArcIdx( const APP_COMMON_MAPPING mapping );
extern u32 APP_COMMON_GetDummyAnimeArcIdx( const APP_COMMON_MAPPING mapping );
