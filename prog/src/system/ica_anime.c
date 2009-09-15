/////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  icaアニメーション ストリーミング再生
 * @file   ica_anime.c
 * @author obata
 * @date   2009.09.04
 *
 */
/////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "ica_anime.h"

// デバッグ出力スイッチ
//#define ICA_ANIME_DEBUG


//===================================================================================
/**
 * @brief icaデータ管理ワーク
 */
//===================================================================================
struct _ICA_ANIME
{
  HEAPID     heapID;        // 使用するヒープID
  ARCHANDLE* arcHandle;     // アーカイブハンドル
  ARCDATID   datID;         // アーカイブ内データインデックス
  u32        frameSize;     // アニメーションの全フレーム数
  fx32       nowFrame;      // 現在のフレーム数
  int        bufStartFrame; // バッファ先頭データのフレーム数
  int        bufSize;       // バッファサイズ
  VecFx32*   transBuf;      // 平行移動バッファ
  VecFx32*   rotateBuf;     // 回転バッファ
};


//===================================================================================
/**
 * @brief プロトタイプ宣言
 */
//===================================================================================
void UpdateBuf( ICA_ANIME* anime, fx32 start_frame );


//===================================================================================
/**
 * @breif 公開関数の定義
 */
//===================================================================================

//-----------------------------------------------------------------------------------
/**
 * @brief アニメーションを作成する (データ一括読み込み)
 *
 * @param heap_id 使用するヒープID
 * @param arc_id  アニメーションデータのアーカイブファイル指定
 * @param dat_id  アーカイブファイル上のインデックスナンバー
 *
 * @return 作成したアニメーション管理オブジェクト
 */
//-----------------------------------------------------------------------------------
ICA_ANIME* ICA_ANIME_Create( HEAPID heap_id, ARCID arc_id, ARCDATID dat_id )
{
  ICA_ANIME* anime;

  // ワークを確保
  anime = (ICA_ANIME*)GFL_HEAP_AllocMemory( heap_id, sizeof(ICA_ANIME) );

  // 初期化
  anime->heapID        = heap_id;
  anime->arcHandle     = GFL_ARC_OpenDataHandle( arc_id, heap_id );
  anime->datID         = dat_id;
  GFL_ARC_LoadDataOfsByHandle( anime->arcHandle, anime->datID, 0, sizeof(u32), &anime->frameSize );
  anime->nowFrame      = 0;
  anime->bufStartFrame = 0;
  anime->bufSize       = anime->frameSize;    // MEMO:ストリーミングとの違いはココだけ
  anime->transBuf      = GFL_HEAP_AllocMemory( heap_id, sizeof(VecFx32) * anime->bufSize );
  anime->rotateBuf     = GFL_HEAP_AllocMemory( heap_id, sizeof(VecFx32) * anime->bufSize );
  UpdateBuf( anime, 0 );

  return anime;
}

//-----------------------------------------------------------------------------------
/**
 * @brief アニメーションを作成する( ストリーミング再生 )
 *
 * @param heap_id      使用するヒープID
 * @param arc_id       アニメーションデータのアーカイブファイル指定
 * @param dat_id       アーカイブファイル上のインデックスナンバー
 * @param buf_interval データ読み込みの間隔 (指定フレーム数に一度, 読み込みを行う)
 *
 * @return 作成したアニメーション管理オブジェクト
 */
//-----------------------------------------------------------------------------------
ICA_ANIME* ICA_ANIME_CreateStreaming( 
    HEAPID heap_id, ARCID arc_id, ARCDATID dat_id, int buf_interval )
{
  ICA_ANIME* anime;

  // ワークを確保
  anime = (ICA_ANIME*)GFL_HEAP_AllocMemory( heap_id, sizeof(ICA_ANIME) );

  // 初期化
  anime->heapID        = heap_id;
  anime->arcHandle     = GFL_ARC_OpenDataHandle( arc_id, heap_id );
  anime->datID         = dat_id;
  GFL_ARC_LoadDataOfsByHandle( anime->arcHandle, anime->datID, 0, sizeof(u32), &anime->frameSize );
  anime->nowFrame      = 0;
  anime->bufStartFrame = 0;
  anime->bufSize       = buf_interval;    // MEMO:一括読み込みとの違いはココだけ
  anime->transBuf      = GFL_HEAP_AllocMemory( heap_id, sizeof(VecFx32) * anime->bufSize );
  anime->rotateBuf     = GFL_HEAP_AllocMemory( heap_id, sizeof(VecFx32) * anime->bufSize );
  UpdateBuf( anime, 0 );

  return anime;
}

//-----------------------------------------------------------------------------------
/**
 * @brief アニメーションを破棄する
 *
 * @param anime 破棄するアニメーション
 */
//-----------------------------------------------------------------------------------
void ICA_ANIME_Delete( ICA_ANIME* anime )
{
  GFL_ARC_CloseDataHandle( anime->arcHandle );
  GFL_HEAP_FreeMemory( anime->transBuf );
  GFL_HEAP_FreeMemory( anime->rotateBuf );
  GFL_HEAP_FreeMemory( anime );
}

//-----------------------------------------------------------------------------------
/**
 * @brief アニメーションを進める
 *
 * @param anime 更新するアニメーション
 * @param frame 進めるフレーム数
 */
//-----------------------------------------------------------------------------------
void ICA_ANIME_IncAnimeFrame( ICA_ANIME* anime, fx32 frame )
{
  int now;

  // 現在フレームを更新
  anime->nowFrame += frame;
  now = anime->nowFrame >> FX32_SHIFT;

  // 最終フレームを超えたら, ゼロフレーム目に戻る
  if( anime->frameSize <= now )
  {
    anime->nowFrame = 0;
    now = 0;
  }

  // 現在フレームがバッファリング範囲をはずれたら, バッファを更新する
  if( ( now < anime->bufStartFrame ) ||
      ( (anime->bufStartFrame + anime->bufSize) <= now ) )
  {
    UpdateBuf( anime, anime->nowFrame );
  }
}

//-----------------------------------------------------------------------------------
/**
 * @brief 現在フレームにおける平行移動データを取得する
 *
 * @param anime   取得対象アニメーション
 * @param vec_dst 取得したデータの格納先
 */
//-----------------------------------------------------------------------------------
void ICA_ANIME_GetTranslate( ICA_ANIME* anime, VecFx32* vec_dst )
{
  // バッファ内インデックスを計算
  int buf_index = (anime->nowFrame>>FX32_SHIFT) - anime->bufStartFrame;

  // 現在フレームの値を取得
  *vec_dst = anime->transBuf[ buf_index ];
}

//-----------------------------------------------------------------------------------
/**
 * @brief 現在フレームにおける回転データを取得する
 *
 * @param anime   取得対象アニメーション
 * @param vec_dst 取得したデータの格納先
 */
//-----------------------------------------------------------------------------------
void ICA_ANIME_GetRotate( ICA_ANIME* anime, VecFx32* vec_dst )
{
  // バッファ内インデックスを計算
  int buf_index = (anime->nowFrame>>FX32_SHIFT) - anime->bufStartFrame;

  // 現在フレームの値を取得
  *vec_dst = anime->rotateBuf[ buf_index ];
}

//---------------------------------------------------------------------------
/**
 * @brief カメラ座標・ターゲット座標を現在フレームの状態に設定する
 *
 * @param anime   設定アニメーション
 * @param camera  設定対象のカメラ
 */
//---------------------------------------------------------------------------
void ICA_ANIME_SetCameraStatus( ICA_ANIME* anime, GFL_G3D_CAMERA* camera )
{
  float x, y, z;
  u16 rx, ry, rz;
  MtxFx33 matrix;
  VecFx32 rotate;
  VecFx32 def_forward = { 0, 0, -FX32_ONE };
  VecFx32 def_upward  = { 0, FX32_ONE, 0 }; 
  VecFx32 pos, target, forward, upward;

  // 回転行列を作成
  ICA_ANIME_GetRotate( anime, &rotate );
  x = FX_FX32_TO_F32( rotate.x );
  y = FX_FX32_TO_F32( rotate.y );
  z = FX_FX32_TO_F32( rotate.z );
  while( x < 0 ) x += 360.0f;
  while( y < 0 ) y += 360.0f;
  while( z < 0 ) z += 360.0f;
  rx = x / 360.0f * 0xffff;
  ry = y / 360.0f * 0xffff;
  rz = z / 360.0f * 0xffff; 
  GFL_CALC3D_MTX_CreateRot( rx, ry, rz, &matrix );

  // ベクトル回転でカメラの向きを算出
  MTX_MultVec33( &def_forward, &matrix, &forward );
  MTX_MultVec33( &def_upward, &matrix, &upward );

  // カメラ・注視点位置を求める
  ICA_ANIME_GetTranslate( anime, &pos );
  VEC_Add( &pos, &forward, &target );

  // カメラの設定
  GFL_G3D_CAMERA_SetPos( camera, &pos );
  GFL_G3D_CAMERA_SetTarget( camera, &target );
  GFL_G3D_CAMERA_SetCamUp( camera, &upward );
}


//===================================================================================
/**
 * @breif 公開関数の定義
 */
//===================================================================================

//---------------------------------------------------------------------------
/**
 * @brief バッファを更新する
 * 
 * @param anime       更新対象のアニメーション管理オブジェクト
 * @param start_frame 新しいデータの開始フレーム
 */
//---------------------------------------------------------------------------
void UpdateBuf( ICA_ANIME* anime, fx32 start_frame )
{
  int    i, j;
  int    frame     = (start_frame >> FX32_SHIFT) % anime->frameSize;
  int    ofs       = 4 + sizeof(float) * 6 * frame;
  int    data_size = sizeof(float) * 6 * anime->bufSize;
  void* data       = GFL_HEAP_AllocMemoryLo( anime->heapID, data_size );

  // 指定フレーム位置から 一定フレーム数分のデータを取得し, バッファ更新
  GFL_ARC_LoadDataOfsByHandle( anime->arcHandle, anime->datID, ofs, data_size, data );
  for( i=0, j=0; i<anime->bufSize; i++ )
  {
    anime->rotateBuf[i].x = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
    anime->rotateBuf[i].y = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
    anime->rotateBuf[i].z = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
    anime->transBuf[i].x = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
    anime->transBuf[i].y = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
    anime->transBuf[i].z = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
  }
  GFL_HEAP_FreeMemory( data );

  // バッファ開始フレーム位置を記憶
  anime->bufStartFrame = start_frame >> FX32_SHIFT;

#ifdef ICA_ANIME_DEBUG
  OBATA_Printf( "update ica anime buffer\n" );
#endif
}
