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
  DEMO3D_CMD_TYPE_SE,
  DEMO3D_CMD_TYPE_END, ///< コマンド終了
} DEMO3D_CMD_TYPE;

#define DEMO3D_CMD_PARAM_MAX (6) ///< 初期化パラメータの最大値

//--------------------------------------------------------------
///	3Dデモコマンドデータ
//==============================================================
typedef struct {
  DEMO3D_CMD_TYPE   type;     ///< コマンドタイプ
  int               frame;    ///< 再生フレーム
  int               param[ DEMO3D_CMD_PARAM_MAX ]; ///< 初期化パラメータ
} DEMO3D_CMD_DATA;


