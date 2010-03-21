//==============================================================================
/**
 * @file  symbol_map.c
 * @brief シンボルエンカウント用マップ関連
 * @author  tamada GAME FREAK inc.
 * @date  2010.03.19
 */
//==============================================================================

#include <gflib.h>


#include "savedata/save_control.h"  //SAVE_CONTROL_WORK
#include "gamesystem/gamesystem.h"
#include "savedata/symbol_save.h"
#include "savedata/intrude_save.h"

#include "arc/fieldmap/zone_id.h"
#include "field/field_dir.h"

#include "field/intrude_symbol.h"

#include "symbol_map.h"
//==============================================================================
//==============================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
enum {
  SYMMAP_MAX_HEIGHT = 10,
  SYMMAP_MAX_WIDTH = 3,

  SYMMAP_SIZE = SYMMAP_MAX_HEIGHT * SYMMAP_MAX_WIDTH,

  SYMMAP_TOP_ID = 1,    ///<一番奥のID
  SYMMAP_ENT_ID = 6,    ///<入り口のID

  SYMMAP_IDX_TOP = 1,

  SYMMAP_SMALL_LEVEL_LIMIT = SYMBOL_MAP_LEVEL_SMALL_MAX + 1,
};

//==============================================================================
//==============================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static u8 map_level1_0[SYMMAP_SIZE] = {
  0,  1,  0,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static u8 map_level2_0[SYMMAP_SIZE] = {
  0,  1,  0,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static u8 map_level3_0[SYMMAP_SIZE] = {
  0,  1,  0,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static u8 map_level4_0[SYMMAP_SIZE] = {
  0,  1,  0,
  17, 18, 19,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static u8 map_level5_0[SYMMAP_SIZE] = {
  0,  1,  0,
  20, 21, 22,
  17, 18, 19,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static u8 map_level6_0[SYMMAP_SIZE] = {
  0,  1,  0,
  23, 24, 25,
  20, 21, 22,
  17, 18, 19,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
};
static u8 map_level7_0[SYMMAP_SIZE] = {
  0,  1,  0,
  26, 27, 28,
  23, 24, 25,
  20, 21, 22,
  17, 18, 19,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
};

static u8 map_level1_1[SYMMAP_SIZE] = {
  0,  1,  0,
  2,  3,  4,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static u8 map_level2_1[SYMMAP_SIZE] = {
  0,  1,  0,
  2,  3,  4,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static u8 map_level3_1[SYMMAP_SIZE] = {
  0,  1,  0,
  2,  3,  4,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static u8 map_level4_1[SYMMAP_SIZE] = {
  0,  1,  0,
  2,  3,  4,
  17, 18, 19,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static u8 map_level5_1[SYMMAP_SIZE] = {
  0,  1,  0,
  2,  3,  4,
  20, 21, 22,
  17, 18, 19,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
};
static u8 map_level6_1[SYMMAP_SIZE] = {
  0,  1,  0,
  2,  3,  4,
  23, 24, 25,
  20, 21, 22,
  17, 18, 19,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
};
static u8 map_level7_1[SYMMAP_SIZE] = {
  0,  1,  0,
  2,  3,  4,
  26, 27, 28,
  23, 24, 25,
  20, 21, 22,
  17, 18, 19,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
};

//==============================================================================
//==============================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static inline SYMBOL_MAP_ID LSIDtoSYMMAPID( u8 lsid )
{
  return lsid - 1;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static inline u8 SYMMAPIDtoLSID( SYMBOL_MAP_ID symmap_id )
{
  return symmap_id + 1;
}
//--------------------------------------------------------------
/// 左側にマップがあるか？
//--------------------------------------------------------------
static inline BOOL hasLeft( const u8 * map, int idx )
{
  return ( (idx % SYMMAP_MAX_WIDTH != 0) && idx != SYMMAP_IDX_TOP );
}
//--------------------------------------------------------------
/// 右側にマップがあるか？
//--------------------------------------------------------------
static inline BOOL hasRight( const u8 * map, int idx )
{
  return ( (idx % SYMMAP_MAX_WIDTH != SYMMAP_MAX_WIDTH - 1) && idx != SYMMAP_IDX_TOP );
}
//--------------------------------------------------------------
/// 上側にマップがあるか？
//--------------------------------------------------------------
static inline BOOL hasUp( const u8 * map, int idx )
{
  return (idx - SYMMAP_MAX_WIDTH >= 0 && map[idx - SYMMAP_MAX_WIDTH] != 0 );
}
//--------------------------------------------------------------
/// 下側にマップがあるか？
//--------------------------------------------------------------
static inline BOOL hasDown( const u8 * map, int idx )
{
  return ( idx + SYMMAP_MAX_WIDTH < SYMMAP_SIZE && map[idx + SYMMAP_MAX_WIDTH] != 0 );
}
//--------------------------------------------------------------
//  SIDからマップテーブルへのインデックスを取得する
//--------------------------------------------------------------
static u8 getIndex( const u8 * map, u8 now_lsid )
{
  int i;
#ifdef  PM_DEBUG
  for ( i = 0; i < SYMMAP_SIZE; i+=3 )
  {
    OS_TPrintf( "%2d %2d %2d\n", map[i+0],map[i+1],map[i+2] );
  }
  OS_TPrintf( "search sid == %d\n", now_lsid );
#endif
  for ( i = 0; i < SYMMAP_SIZE; i ++ )
  {
    if ( map[i] == now_lsid ) return i;
  }
  GF_ASSERT(0);
  return SYMMAP_IDX_TOP;
}

//--------------------------------------------------------------
/// SIDから対応するゾーンIDを取得する
//--------------------------------------------------------------
u16 getSymbolMapZoneID( const u8 * map, u8 now_lsid )
{
  int idx = getIndex( map, now_lsid );
  BOOL left = hasLeft( map, idx );
  BOOL right = hasRight( map, idx );
  BOOL up = hasUp( map, idx );
  BOOL down = hasDown( map, idx );

  if ( now_lsid == SYMMAP_TOP_ID ) return ZONE_ID_PALACE03;
  if ( now_lsid == SYMMAP_ENT_ID ) return ZONE_ID_PALACE07;

  if ( left && right )
  { //左右ある時は上下もある！
    GF_ASSERT( up && down );
    return ZONE_ID_PALACE07;
  }

  if ( left && !right )
  { //右端の場合
    GF_ASSERT( up || down );  //上下どちらかはある！
    if ( up && down ) return ZONE_ID_PALACE08;
    if ( up )         return ZONE_ID_PALACE10;
    if ( down )       return ZONE_ID_PALACE05;
  }

  if ( !left && right )
  { //左端の場合
    GF_ASSERT( up || down );  //上下どちらかはある！
    if ( up && down ) return ZONE_ID_PALACE06;
    if ( up )         return ZONE_ID_PALACE09;
    if ( down )       return ZONE_ID_PALACE04;
  }
  GF_ASSERT( 0 );
  return ZONE_ID_PALACE03;  //万が一のエラー処理：一番制限されたものを返す
}

//--------------------------------------------------------------
/// 現在のSIDと方向から次のマップのSIDを取得する
//--------------------------------------------------------------
static u16 getNextMapLSID( const u8 * map, u8 now_lsid, u8 dir_id )
{
  int index = getIndex( map, now_lsid );
  switch ( dir_id )
  {
  case DIR_UP:
    GF_ASSERT( hasUp( map, index ) );
    return map[ index - SYMMAP_MAX_WIDTH ];
  case DIR_DOWN:
    GF_ASSERT( hasDown( map, index ) );
    return map[ index + SYMMAP_MAX_WIDTH ];
  case DIR_LEFT:
    GF_ASSERT( hasLeft( map, index ) );
    return map[ index - 1 ];
  case DIR_RIGHT:
    GF_ASSERT( hasRight( map, index ) );
    return map[ index + 1 ];
  default:
    GF_ASSERT( 0 );
  }
  return now_lsid;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static const u8 * getMapTable( SYMBOL_MAP_LEVEL_LARGE large_lvl, SYMBOL_MAP_LEVEL_SMALL small_lvl )
{
  static const u8 * table[] = {
    map_level1_0,
    map_level2_0,
    map_level3_0,
    map_level4_0,
    map_level5_0,
    map_level6_0,
    map_level7_0,

    map_level1_1,
    map_level2_1,
    map_level3_1,
    map_level4_1,
    map_level5_1,
    map_level6_1,
    map_level7_1,
  };
  int index = small_lvl + SYMMAP_SMALL_LEVEL_LIMIT * large_lvl;
  GF_ASSERT( index < NELEMS(table) );
  return table[index];
}

//==============================================================================
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief 対応するゾーンのIDを取得
 * @param symmap_id   シンボルマップID
 * @return  u16
 */
//--------------------------------------------------------------
u16 SYMBOLMAP_GetZoneID( SYMBOL_MAP_ID symmap_id )
{
  int large_lvl = SYMBOL_MAP_LEVEL_LARGE_NONE;
  int small_lvl = SYMBOL_MAP_LEVEL_SMALL_1;
  const u8 * tbl = getMapTable( large_lvl, small_lvl );

  return getSymbolMapZoneID( tbl, SYMMAPIDtoLSID( symmap_id ) );
}

//--------------------------------------------------------------
/**
 * @brief 移動先のシンボルマップID取得
 * @param now_symmap_id   現在のシンボルマップID
 * @param dir_id          移動先マップのある方向
 * @return  SYMBOL_MAP_ID 移動先のシンボルマップID
 */
//--------------------------------------------------------------
SYMBOL_MAP_ID SYMBOLMAP_GetNextSymbolMapID( SYMBOL_MAP_ID now_symmap_id, u16 dir_id )
{
  int large_lvl = SYMBOL_MAP_LEVEL_LARGE_NONE;
  int small_lvl = SYMBOL_MAP_LEVEL_SMALL_1;
  const u8 * tbl = getMapTable( large_lvl, small_lvl );

  u8 next_lsid;
  next_lsid = getNextMapLSID( tbl, SYMMAPIDtoLSID(now_symmap_id), dir_id );
  return LSIDtoSYMMAPID( next_lsid );
}

//--------------------------------------------------------------
/**
 * @brief キープゾーンか？のチェック
 * @param symmap_id   シンボルマップID
 * @return BOOL   TRUEのとき、キープゾーンのマップ
 */
//--------------------------------------------------------------
BOOL SYMBOLMAP_IsKeepzoneID( SYMBOL_MAP_ID symmap_id )
{
  return SYMMAPIDtoLSID( symmap_id ) == SYMMAP_TOP_ID;
}

//--------------------------------------------------------------
/**
 * @brief   パレスの森の入口マップか？のチェック
 * @param symmap_id   シンボルマップID
 * @return BOOL   TRUEの時、パレスの森の入り口マップ
 */
//--------------------------------------------------------------
BOOL SYMBOLMAP_IsEntranceID( SYMBOL_MAP_ID symmap_id )
{
  return SYMMAPIDtoLSID( symmap_id ) == SYMMAP_ENT_ID;
}



