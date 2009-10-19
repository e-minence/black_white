/////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief 3Dサウンドユニット
 * @file iss_3ds_unit.c
 * @author obata
 * @date 2009.09.08
 *
 */
/////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "iss_3ds_unit.h"


//===============================================================================
/**
 * @brief 1つの音源を扱う構造体
 */
//===============================================================================
struct _ISS_3DS_UNIT
{ 
  VecFx32   pos;  // 音源の位置
  fx32    range;  // 音が届く距離
  int maxVolume;  // 最大音量(0〜127)
  u16 trackBit;   // 操作トラックマスク
};


//===============================================================================
// ■作成・破棄
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief ユニットを作成する
 *
 * @param heap_id 使用するヒープID
 *
 * @return 作成したユニット
 */
//-------------------------------------------------------------------------------
ISS_3DS_UNIT* ISS_3DS_UNIT_Create( HEAPID heap_id )
{
  ISS_3DS_UNIT* unit;
  unit = GFL_HEAP_AllocMemory( heap_id, sizeof( ISS_3DS_UNIT ) );
  return unit;
}

//-------------------------------------------------------------------------------
/**
 * @brief ユニットを破棄する
 *
 * @param unit 破棄するユニット
 */
//-------------------------------------------------------------------------------
void ISS_3DS_UNIT_Delete( ISS_3DS_UNIT* unit )
{
  GFL_HEAP_FreeMemory( unit );
}


//===============================================================================
// ■情報取得
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief 位置を取得する
 *
 * @param unit 取得対象ユニット
 * @param dest 位置ベクトルの格納先
 */
//-------------------------------------------------------------------------------
void ISS_3DS_UNIT_GetPos( const ISS_3DS_UNIT* unit, VecFx32* dest )
{
  dest->x = unit->pos.x;
  dest->y = unit->pos.y;
  dest->z = unit->pos.z;
}

//-------------------------------------------------------------------------------
/**
 * @brief 音の可聴距離を取得する
 *
 * @param unit 取得対象ユニット
 *
 * @return 音が届く距離
 */
//-------------------------------------------------------------------------------
fx32 ISS_3DS_UNIT_GetRange( const ISS_3DS_UNIT* unit )
{
  return unit->range; 
}

//-------------------------------------------------------------------------------
/**
 * @brief 最大音量を取得する
 *
 * @param unit 取得対象ユニット
 *
 * @return 最大音量(0〜127)
 */
//-------------------------------------------------------------------------------
int ISS_3DS_UNIT_GetMaxVolume( const ISS_3DS_UNIT* unit )
{
  return unit->maxVolume;
}

//-------------------------------------------------------------------------------
/**
 * @brief 操作対象トラックのビットマスクを取得する
 *
 * @param unit 取得対象ユニット
 *
 * @return トラックマスク
 */
//-------------------------------------------------------------------------------
u16 ISS_3DS_UNIT_GetTrackBit( const ISS_3DS_UNIT* unit )
{
  return unit->trackBit;
}


//===============================================================================
// ■情報設定
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief 設定データを読み込む
 * 
 * @param unit    設定対象のユニット
 * @param arc_id  読み込むデータのアーカイブＩＤ
 * @param dat_id  アーカイブ内データＩＤ
 * @param heap_id 使用するヒープＩＤ
 */
//-------------------------------------------------------------------------------
void ISS_3DS_UNIT_Load(  
    ISS_3DS_UNIT* unit, ARCID arc_id, ARCDATID dat_id, HEAPID heap_id )
{
  void* data;
  int ofs = 0;
  int track, volume;
  float range;

  // アーカイブファイルデータを読み込む
  data   = GFL_ARC_LoadDataAlloc( arc_id, dat_id, heap_id );
  track  = *( (int*)((int)data + ofs) );   ofs += sizeof(int);
  range  = *( (float*)((int)data + ofs) ); ofs += sizeof(float);
  volume = *( (int*)((int)data + ofs) );   ofs += sizeof(int);
  GFL_HEAP_FreeMemory( data );

  // 読み込んだデータをセット
  unit->trackBit  = ( 1 << (track-1) );
  unit->range     = FX_F32_TO_FX32( range );
  unit->maxVolume = volume;

  // DEBUG:
  OBATA_Printf( "track = %d\n", track );
  OBATA_Printf( "range = %d\n", (int)range );
  OBATA_Printf( "volume = %d\n", volume );
}

//-------------------------------------------------------------------------------
/**
 * @brief 位置を設定する
 *
 * @param unit 設定対象ユニット
 * @param pos  設定値
 */
//-------------------------------------------------------------------------------
void ISS_3DS_UNIT_SetPos( ISS_3DS_UNIT* unit, const VecFx32* pos )
{
  unit->pos.x = pos->x;
  unit->pos.y = pos->y;
  unit->pos.z = pos->z;
}

//-------------------------------------------------------------------------------
/**
 * @brief 音の可聴距離を設定する
 *
 * @param unit  設定対象ユニット
 * @param range 設定値
 */
//-------------------------------------------------------------------------------
void ISS_3DS_UNIT_SetRange( ISS_3DS_UNIT* unit, fx32 range )
{
  unit->range = range;
}

//-------------------------------------------------------------------------------
/**
 * @brief 最大音量を設定する
 *
 * @param unit   設定対象ユニット
 * @param volume 設定値(0〜127)
 */
//-------------------------------------------------------------------------------
void ISS_3DS_UNIT_SetMaxVolume( ISS_3DS_UNIT* unit, int volume )
{ 
  unit->maxVolume = volume;
}

//-------------------------------------------------------------------------------
/**
 * @brief 操作対象トラックを設定する
 *
 * @param unit   設定対象ユニット
 * @param track 操作対象トラックのマスク
 */
//-------------------------------------------------------------------------------
void ISS_3DS_UNIT_SetTrackBit( ISS_3DS_UNIT* unit, u16 track )
{
  unit->trackBit = track;
}
