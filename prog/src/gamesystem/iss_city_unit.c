///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_city_unit.c
 * @brief  街ISSユニット
 * @author obata_toshihiro
 * @date   2009.11.02
 */
///////////////////////////////////////////////////////////////////////////////////////////
#include "iss_city_unit.h"
#include "field/field_const.h" // for FX32_TO_GRID
#include "arc_def.h"           // for ARCID_ISS_CITY
#include "../../../resource/fldmapdata/zonetable/zone_id.h" 


//=========================================================================================
// ■定数
//=========================================================================================
//#define DEBUG_PRINT_ON        // デバッグ出力スイッチ
#define PRINT_DEST       (1)  // デバッグ情報の出力先
#define VOLUME_SPACE_NUM (6)  // 1つの街ISSユニットが持つ音量空間の数

#define ABS(n) ((n)<0 ? -(n) : (n))  // 絶対値マクロ


//=========================================================================================
// ■街ISSユニット データ
//=========================================================================================
typedef struct
{
	// 配置場所のゾーンID
	u16 zoneID;
  u16 padding;

	// 座標[grid]
	int gx;
	int gy;
	int gz;

	// 音量空間
  u8 volume[ VOLUME_SPACE_NUM ];  // [0, 127]
  u8 xRange[ VOLUME_SPACE_NUM ];  // [grid]
  u8 yRange[ VOLUME_SPACE_NUM ];  // [grid]
  u8 zRange[ VOLUME_SPACE_NUM ];  // [grid]
  
} ISS_C_UNIT_DATA;


//=========================================================================================
// ■街ISSユニット
//=========================================================================================
struct _ISS_C_UNIT
{
	// 配置場所のゾーンID
	u16 zoneID;

	// 座標[grid]]
	int xGrid;
	int yGrid;
	int zGrid;

	// 音量空間
  u8 volume[ VOLUME_SPACE_NUM ]; // [0,127]
  u8 xRange[ VOLUME_SPACE_NUM ]; // [grid]
  u8 yRange[ VOLUME_SPACE_NUM ]; // [grid]
  u8 zRange[ VOLUME_SPACE_NUM ]; // [grid]
};


//=========================================================================================
// ■非公開関数
//=========================================================================================
// 生成
static void SetupUnit( const ISS_C_UNIT_DATA* unitData, ISS_C_UNIT* unit );
// ボリューム算出
static u8 GetVolume          ( const ISS_C_UNIT* unit, const VecFx32* playerPos );
static u8 GetVolumeX         ( const ISS_C_UNIT* unit, const VecFx32* playerPos );
static u8 GetVolumeY         ( const ISS_C_UNIT* unit, const VecFx32* playerPos );
static u8 GetVolumeZ         ( const ISS_C_UNIT* unit, const VecFx32* playerPos );
static u8 SearchBasePriorityX( const ISS_C_UNIT* unit, const VecFx32* playerPos );
static u8 SearchBasePriorityY( const ISS_C_UNIT* unit, const VecFx32* playerPos );
static u8 SearchBasePriorityZ( const ISS_C_UNIT* unit, const VecFx32* playerPos ); 
static int GetWorlUnitPosX( const ISS_C_UNIT* unit ); // ユニットのワールドx座標を取得する
static int GetWorldUnitPosY( const ISS_C_UNIT* unit ); // ユニットのワールドy座標を取得する
static int GetWorldUnitPosZ( const ISS_C_UNIT* unit ); // ユニットのワールドz座標を取得する
static int CalcLerp( int startVal, int endVal, int startPos, int endPos, int calcPos );
// デバッグ
static void DebugPrint_unitParam( const ISS_C_UNIT* unit );


//=========================================================================================
// ■作成/破棄
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @breif ユニットを作成する
 *
 * @param heapID 使用するヒープID
 * @param datID  読み込むユニットデータのアーカイブ内インデックス
 */
//-----------------------------------------------------------------------------------------
ISS_C_UNIT* ISS_C_UNIT_Create( HEAPID heapID, ARCDATID datID )
{
  ISS_C_UNIT_DATA* unitData;
  ISS_C_UNIT* unit;

  // 生成
  unit     = GFL_HEAP_AllocMemory( heapID, sizeof(ISS_C_UNIT) );
  unitData = GFL_ARC_LoadDataAllocOfs( ARCID_ISS_CITY, datID, heapID, 0, sizeof(ISS_C_UNIT_DATA) );
  SetupUnit( unitData, unit );
  GFL_HEAP_FreeMemory( unitData );

  // DEBUG:
#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C-UNIT: create\n" );
  DebugPrint_unitParam( unit );
#endif

  return unit; 
}

//-----------------------------------------------------------------------------------------
/**
 * @breif ユニットを破棄する
 *
 * @param unit 破棄するユニット
 */
//-----------------------------------------------------------------------------------------
void ISS_C_UNIT_Delete( ISS_C_UNIT* unit )
{
  GFL_HEAP_FreeMemory( unit );

  // DEBUG:
#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C-UNIT: delete\n" );
#endif
}


//=========================================================================================
// ■取得
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ゾーンIDを取得する
 *
 * @param unit
 *
 * @return 指定したユニットのゾーンID
 */
//-----------------------------------------------------------------------------------------
u16 ISS_C_UNIT_GetZoneID( const ISS_C_UNIT* unit )
{
  return unit->zoneID;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief 音量を取得する
 *
 * @param unit
 * @param pos  プレイヤー座標
 *
 * @return 指定したプレイヤー座標における音量[0, 127]
 */
//-----------------------------------------------------------------------------------------
int ISS_C_UNIT_GetVolume( const ISS_C_UNIT* unit, const VecFx32* pos )
{
  return GetVolume( unit, pos );
}


//=========================================================================================
// ■非公開関数
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ユニットオブジェクトを作成する
 *
 * @param unitData ユニットデータ
 * @param unit     変換したデータの格納先
 */
//-----------------------------------------------------------------------------------------
static void SetupUnit( const ISS_C_UNIT_DATA* unitData, ISS_C_UNIT* unit )
{
  int i;

  // ゾーンID
  unit->zoneID = unitData->zoneID;

  // 座標 [grid]
  unit->xGrid = unitData->gx;
  unit->yGrid = unitData->gy;
  unit->zGrid = unitData->gz;

  // 音量空間
  for( i=0; i<VOLUME_SPACE_NUM; i++ )
  {
    unit->volume[i] = unitData->volume[i];
    unit->xRange[i] = unitData->xRange[i];
    unit->yRange[i] = unitData->yRange[i];
    unit->zRange[i] = unitData->zRange[i];
  } 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ボリュームを取得する
 *
 * @param unit
 * @param playerPos プレイヤー座標
 *
 * @return 指定した座標におけるボリューム
 */
//-----------------------------------------------------------------------------------------
static u8 GetVolume( const ISS_C_UNIT* unit, const VecFx32* playerPos )
{
  u8 volume[3];
  u8 vol;

  volume[0] = GetVolumeX( unit, playerPos );
  volume[1] = GetVolumeY( unit, playerPos );
  volume[2] = GetVolumeZ( unit, playerPos );

  vol = (volume[0] * volume[1] * volume[2]) / (127 * 127);
  return vol;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief 指定した座標におけるボリュームを取得する
 *
 * @param unit
 * @param playerPos
 *
 * @return 指定したプレイヤー座標でのボリューム
 */
//-----------------------------------------------------------------------------------------
static u8 GetVolumeX( const ISS_C_UNIT* unit, const VecFx32* playerPos )
{
  int i;
  u8 basePri, destPri;
  u8 volume;

  basePri = SearchBasePriorityX( unit, playerPos );

  if( basePri == 0 ){ destPri = 0; }
  else{ destPri = basePri - 1; }

  if( basePri == destPri )
  {
    return unit->volume[ basePri ];
  }
  else
  {
    int dist, playerX, unitX;
    int startVal, endVal;
    int startPos, endPos;

    playerX = FX_Whole( playerPos->x );
    unitX   = GetWorlUnitPosX( unit );
    dist    = ABS( playerX - unitX );

    startVal = unit->volume[ basePri ];
    endVal   = unit->volume[ destPri ];
    startPos = unit->xRange[ basePri ] * FIELD_CONST_GRID_SIZE;
    endPos   = unit->xRange[ destPri ] * FIELD_CONST_GRID_SIZE;

    return CalcLerp( startPos, startVal, endPos, endVal, dist );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief 指定した座標におけるボリュームを取得する
 *
 * @param unit
 * @param playerPos
 *
 * @return 指定したプレイヤー座標でのボリューム
 */
//-----------------------------------------------------------------------------------------
static u8 GetVolumeY( const ISS_C_UNIT* unit, const VecFx32* playerPos )
{
  int i;
  u8 basePri, destPri;
  u8 volume;

  basePri = SearchBasePriorityY( unit, playerPos );

  if( basePri == 0 ){ destPri = 0; }
  else{ destPri = basePri - 1; }

  if( basePri == destPri )
  {
    return unit->volume[ basePri ];
  }
  else
  {
    int dist, playerY, unitY;
    int startVal, endVal;
    int startPos, endPos;

    playerY = FX_Whole( playerPos->y );
    unitY   = GetWorldUnitPosY( unit );
    dist    = ABS( playerY - unitY );

    startVal = unit->volume[ basePri ];
    endVal   = unit->volume[ destPri ];
    startPos = unit->yRange[ basePri ] * FIELD_CONST_GRID_SIZE;
    endPos   = unit->yRange[ destPri ] * FIELD_CONST_GRID_SIZE;

    return CalcLerp( startPos, startVal, endPos, endVal, dist );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief 指定した座標におけるボリュームを取得する
 *
 * @param unit
 * @param playerPos
 *
 * @return 指定したプレイヤー座標でのボリューム
 */
//-----------------------------------------------------------------------------------------
static u8 GetVolumeZ( const ISS_C_UNIT* unit, const VecFx32* playerPos )
{
  int i;
  u8 basePri, destPri;
  u8 volume;

  basePri = SearchBasePriorityZ( unit, playerPos );

  if( basePri == 0 ){ destPri = 0; }
  else{ destPri = basePri - 1; }

  if( basePri == destPri )
  {
    return unit->volume[ basePri ];
  }
  else
  {
    int dist, playerZ, unitZ;
    int startVal, endVal;
    int startPos, endPos;

    playerZ = FX_Whole( playerPos->z );
    unitZ   = GetWorldUnitPosZ( unit );
    dist    = ABS( playerZ - unitZ );

    startVal = unit->volume[ basePri ];
    endVal   = unit->volume[ destPri ];
    startPos = unit->zRange[ basePri ] * FIELD_CONST_GRID_SIZE;
    endPos   = unit->zRange[ destPri ] * FIELD_CONST_GRID_SIZE;

    return CalcLerp( startPos, startVal, endPos, endVal, dist );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @breif 補間の基準となるプライオリティを決定する
 *
 * @param unit
 * @param playerPos プレイヤー座標
 *
 * @return 指定した座標での補間の基準となるプライオリティ
 */
//-----------------------------------------------------------------------------------------
static u8 SearchBasePriorityX( const ISS_C_UNIT* unit, const VecFx32* playerPos )
{
  int spaceIdx;
  int dist, playerX, unitX;

  // 距離を求める
  playerX = FX_Whole( playerPos->x );
  unitX   = GetWorlUnitPosX( unit );
  dist    = ABS( playerX - unitX );

  // 検索
  for( spaceIdx=0; spaceIdx < VOLUME_SPACE_NUM; spaceIdx++ )
  {
    if( unit->xRange[ spaceIdx ]*FIELD_CONST_GRID_SIZE <= dist )
    {
      return spaceIdx;
    }
  }
  GF_ASSERT(0);
  return 0;
}
//-----------------------------------------------------------------------------------------
/**
 * @breif 補間の基準となるプライオリティを決定する
 *
 * @param unit
 * @param playerPos プレイヤー座標
 *
 * @return 指定した座標での補間の基準となるプライオリティ
 */
//-----------------------------------------------------------------------------------------
static u8 SearchBasePriorityY( const ISS_C_UNIT* unit, const VecFx32* playerPos )
{
  int spaceIdx;
  int dist, playerY, unitY;

  // 距離を求める
  playerY = FX_Whole( playerPos->y );
  unitY   = GetWorldUnitPosY( unit );
  dist    = ABS( playerY - unitY );

  // 検索
  for( spaceIdx=0; spaceIdx < VOLUME_SPACE_NUM; spaceIdx++ )
  {
    if( unit->yRange[ spaceIdx ] * FIELD_CONST_GRID_SIZE <= dist )
    {
      return spaceIdx;
    }
  }
  GF_ASSERT(0);
  return 0;
}
//-----------------------------------------------------------------------------------------
/**
 * @breif 補間の基準となるプライオリティを決定する
 *
 * @param unit
 * @param playerPos プレイヤー座標
 *
 * @return 指定した座標での補間の基準となるプライオリティ
 */
//-----------------------------------------------------------------------------------------
static u8 SearchBasePriorityZ( const ISS_C_UNIT* unit, const VecFx32* playerPos )
{
  int spaceIdx;
  int dist, playerZ, unitZ;

  // 距離を求める
  playerZ = FX_Whole( playerPos->z );
  unitZ   = GetWorldUnitPosZ( unit );
  dist    = ABS( playerZ - unitZ );

  // 検索
  for( spaceIdx=0; spaceIdx < VOLUME_SPACE_NUM; spaceIdx++ )
  {
    if( unit->zRange[ spaceIdx ] * FIELD_CONST_GRID_SIZE <= dist )
    {
      return spaceIdx;
    }
  } 
  GF_ASSERT(0);
  return 0;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ユニットのワールド座標を取得する
 *
 * @param unit
 *
 * @return ユニットのワールドx座標
 */
//-----------------------------------------------------------------------------------------
static int GetWorlUnitPosX( const ISS_C_UNIT* unit )
{
  return unit->xGrid * FIELD_CONST_GRID_SIZE + FIELD_CONST_GRID_SIZE * 0.5;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ユニットのワールド座標を取得する
 *
 * @param unit
 *
 * @return ユニットのワールドy座標
 */
//-----------------------------------------------------------------------------------------
static int GetWorldUnitPosY( const ISS_C_UNIT* unit )
{
  return unit->yGrid * FIELD_CONST_GRID_SIZE + FIELD_CONST_GRID_SIZE * 0.5;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ユニットのワールド座標を取得する
 *
 * @param unit
 *
 * @return ユニットのワールドz座標
 */
//-----------------------------------------------------------------------------------------
static int GetWorldUnitPosZ( const ISS_C_UNIT* unit )
{
  return unit->zGrid * FIELD_CONST_GRID_SIZE + FIELD_CONST_GRID_SIZE * 0.5;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief 線形補間
 *
 * @param startPos 補間点1
 * @param startVal 補間点1の値
 * @param endPos   補間点2
 * @param endVal   補間点2の値
 * @param calcPos  補間点3
 *
 * @return 補間点3の値
 */
//-----------------------------------------------------------------------------------------
static int CalcLerp( int startPos, int startVal, int endPos, int endVal, int calcPos )
{
  int x1 = startPos;
  int y1 = startVal;
  int x2 = endPos;
  int y2 = endVal;
  int x3 = calcPos;
  //float t  = (float)(x3 - x1) / (float)(x2 - x1);
  //int y3 = (y2 - y1) * t + y1;
  int y3 = (y2 - y1) * (x3 - x1) / (x2 - x1) + y1;

  return y3;
}


#ifdef DEBUG_PRINT_ON
//-----------------------------------------------------------------------------------------
/**
 * @brief ユニット情報を出力する
 *
 * @param unit 出力対象ユニット
 */
//-----------------------------------------------------------------------------------------
static void DebugPrint_unitParam( const ISS_C_UNIT* unit )
{
  int spaceIdx;

  OS_TFPrintf( PRINT_DEST, "ISS-C-UNIT: zoneID = %d\n", unit->zoneID );
  OS_TFPrintf( PRINT_DEST, "ISS-C-UNIT: pos = (%d, %d, %d)\n", unit->xGrid, unit->yGrid, unit->zGrid );

  for( spaceIdx=0; spaceIdx < VOLUME_SPACE_NUM; spaceIdx++ )
  {
    OS_TFPrintf( PRINT_DEST, 
                 "ISS-C-UNIT: volume, range = %d, (%d, %d, %d)\n",
                 unit->volume[ spaceIdx ], 
                 unit->xRange[ spaceIdx ], unit->yRange[ spaceIdx ], unit->zRange[ spaceIdx ] );
  }
}
#endif
