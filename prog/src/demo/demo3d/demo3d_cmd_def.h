//============================================================================
/**
 *
 *	@file		demo3d_cmd_def.h
 *	@brief  3Dデモコマンド コマンド定義
 *	@author		hosaka genya
 *	@data		2009.12.09
 *
 */
//============================================================================
#pragma once

//--------------------------------------------------------------
///	3Dデモコマンド定義
//==============================================================
typedef enum
{ 
  DEMO3D_CMD_TYPE_NULL = 0, ///< コマンドなし
  //======================================================
  //・SE再生
  //  PARAM: 0=SE_Label, 1=volume, 2=pan
  //======================================================
  DEMO3D_CMD_TYPE_SE_PLAY,
  //======================================================
  //・指定ラベルのSE再生終了
  //  PARAM: 0=SE_Label
  //======================================================
  DEMO3D_CMD_TYPE_SE_STOP,
  //======================================================
  //・輝度操作
  //  PARAM: 0=SYNC 1=終了時の輝度, 2=開始時の輝度SYNC
  //======================================================
  DEMO3D_CMD_TYPE_BRIGHTNESS_REQ,
  //======================================================
  //・モーションブラー 開始
  //  PARAM: 0=新しくブレンドする絵のα係数, 1=既にバッファされている絵のα係数
  //======================================================
  DEMO3D_CMD_TYPE_MOTIONBL_START,
  //======================================================
  //・モーションブラー 停止
  //  PARAM: none
  //======================================================
  DEMO3D_CMD_TYPE_MOTIONBL_END,
  DEMO3D_CMD_TYPE_END, ///< コマンド終了
  DEMO3D_CMD_TYPE_MAX,
} DEMO3D_CMD_TYPE;

#define DEMO3D_CMD_PARAM_MAX (8)  ///< 初期化パラメータの最大値
#define DEMO3D_CMD_PARAM_NULL (0) ///< 無効パラメータの初期化値
#define DEMO3D_CMD_SYNC_INIT (-1)     ///< frameにこの値が入っていたら初期化コマンドとみなす

//SE_PLAY
#define DEMO3D_SE_PARAM_DEFAULT (0xFFFFFFFF)

//--------------------------------------------------------------
///	3Dデモコマンドデータ
//==============================================================
typedef struct {
  DEMO3D_CMD_TYPE   type;     ///< コマンドタイプ
  int               frame;    ///< 再生フレーム
  int               param[ DEMO3D_CMD_PARAM_MAX ]; ///< 初期化パラメータ
} DEMO3D_CMD_DATA;

