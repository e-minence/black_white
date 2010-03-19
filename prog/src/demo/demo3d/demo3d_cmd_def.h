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
//======================================================

//======================================================
  DEMO3D_CMD_TYPE_NULL = 0, ///< コマンドなし

//======================================================
/**
 *	@brief  SE再生コマンド
 *
 *	@param	param[0] SE_Label
 *	@param	param[1] volume 0〜127 : DEMO3D_SE_PARAM_DEFAULTなら無効
 *	@param	param[2] pan -128〜127 ：DEMO3D_SE_PARAM_DEFAULTなら無効
 *	@param	param[3] pitch -32768〜32767(+/-64で半音)：DEMO3D_SE_PARAM_DEFAULTなら無効
 *	@param	param[4] playerNo 0〜3 : DEMO3D_SE_PARAM_DEFAULTなら無効
 */
//======================================================
  DEMO3D_CMD_TYPE_SE_PLAY,

//======================================================
/**
 *	@brief  SE:再生停止
 *
 *	@param	param[0] SE_Label
 */
//======================================================
  DEMO3D_CMD_TYPE_SE_STOP,

//======================================================
/**
 *	@brief  SEのボリューム動的変化リクエスト
 *
 *  @param  param[0]  SE_Label
 *  @param  param[1]  スタートの値(0〜127) 
 *  @param  param[2]  エンドの値(0〜127)
 *  @param  param[3]  スタート〜エンドまでのフレーム数(1〜)
 *  @param  param[4]  プレイヤーNo(0〜3 デフォルトでいい時はDEMO3D_SE_PARAM_DEFAULT)
 */
//======================================================
  DEMO3D_CMD_TYPE_SE_VOLUME_EFFECT_REQ,

//======================================================
/**
 *	@brief  SEのパン動的変化リクエスト
 *
 *  @param  param[0]  SE_Label
 *  @param  param[1]  スタートの値(-128〜127) 
 *  @param  param[2]  エンドの値(-128〜127)
 *  @param  param[3]  スタート〜エンドまでのフレーム数(1〜)
 *  @param  param[4]  プレイヤーNo(0〜3 デフォルトでいい時はDEMO3D_SE_PARAM_DEFAULT)
 */
//======================================================
  DEMO3D_CMD_TYPE_SE_PAN_EFFECT_REQ,

//======================================================
/**
 *	@brief  SEのピッチ動的変化リクエスト
 *
 *  @param  param[0]  SE_Label
 *  @param  param[1]  スタートの値(-32768〜32767 +/-64で半音) 
 *  @param  param[2]  エンドの値(-32768〜32767 +/-64で半音) 
 *  @param  param[3]  スタート〜エンドまでのフレーム数(1〜)
 *  @param  param[4]  プレイヤーNo(0〜3 デフォルトでいい時はDEMO3D_SE_PARAM_DEFAULT)
 */
//======================================================
  DEMO3D_CMD_TYPE_SE_PITCH_EFFECT_REQ,

//======================================================
/**
 *	@brief  マスター輝度を用いたブライトネス 
 *
 *  @param  param[0]  適用する画面 < "main"(1),"sub"(2),"double"(3) >
 *  @param  param[1]  何frameでフェードするか？ (0は開始時輝度値を即時反映)
 *  @param  param[2]  フェード開始時の輝度 < -16〜16(黒〜白) >
 *  @param  param[3]  フェード終了時の輝度 < -16〜16(黒〜白) >
 */
//======================================================
  DEMO3D_CMD_TYPE_BRIGHTNESS_REQ,

//======================================================
/**
 *	@brief  マスター輝度を用いたフラッシュリクエスト
 *
 *  @param  param[0]  適用する画面 < "main"(1),"sub"(2),"double"(3) >
 *  @param  param[1]  startの輝度 < -16〜16(黒〜白) >
 *  @param  param[2]  max輝度     < -16〜16(黒〜白) >
 *  @param  param[3]  endの輝度   < -16〜16(黒〜白) >
 *  @param  param[4]  startからmaxまでのフレーム < 1〜 >
 *  @param  param[5]  maxからendまでのフレーム < 1〜 >
 */
//======================================================
  DEMO3D_CMD_TYPE_FLASH_REQ,

//======================================================
/**
 *	@brief  モーションブラー 開始
 *
 *	@param	wk
 *	@param	param[0] モーションブラー係数 新しくブレンドする絵のα係数
 *	@param	param[1] モーションブラー係数 既にバッファされている絵のα係数
 */
//======================================================
  DEMO3D_CMD_TYPE_MOTIONBL_START,

//======================================================
/**
 *	@brief  モーションブラー 停止
 */
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

typedef enum{
  DEMO3D_SE_EFF_VOLUME,
  DEMO3D_SE_EFF_PAN,
  DEMO3D_SE_EFF_PITCH,
}DEMO3D_SE_EFFECT;

//--------------------------------------------------------------
///	3Dデモコマンドデータ
//==============================================================
typedef struct {
  DEMO3D_CMD_TYPE   type;     ///< コマンドタイプ
  int               frame;    ///< 再生フレーム
  int               param[ DEMO3D_CMD_PARAM_MAX ]; ///< 初期化パラメータ
} DEMO3D_CMD_DATA;

