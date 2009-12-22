//============================================================================
/**
 *
 *	@file		intro_cmd_type.h
 *	@brief  コマンド定義
 *	@author	hosaka genya
 *	@data		2009.12.11
 *
 */
#pragma once

//=============================================================================
/**
 *								定数定義
 */
//=============================================================================

// intro_cmd.c に実体あり
typedef enum
{ 
  INTRO_CMD_TYPE_NULL = 0,  ///< 0:コマンドなし
  
  //======================================================
  // 特殊コマンド
  //======================================================

  //======================================================
  //次のシーンをセット
  //  PARAM[0]  INTRO_SCENE_ID
  //======================================================
  INTRO_CMD_TYPE_SET_SCENE,
  //======================================================
  //外部から指定されたシーンにジャンプ
  //======================================================
  INTRO_CMD_TYPE_START_SCENE,
  //======================================================
  //2択選択肢 
  //  TRUEなら一個下のコマンドをストア、FALSEなら2個下のコマンドをストア
  //======================================================
  INTRO_CMD_TYPE_YESNO,
  //======================================================
  //比較
  //  TRUEなら一個下のコマンドをストア、FALSEなら2個下のコマンドをストア
  //  PARAM[0] INTRO_CMD_COMP
  //======================================================
  INTRO_CMD_TYPE_COMP,

  //======================================================
  // 一般コマンド
  //======================================================

  //======================================================
  //グラフィックロード(BG)
  //======================================================
  INTRO_CMD_TYPE_LOAD_BG,
  //======================================================
  //外部アプリに対する戻り値を設定
  //======================================================
  INTRO_CMD_TYPE_SET_RETCODE,
  //======================================================
  //ウェイト
  //  PARAM[0]  SYNC
  //======================================================
  INTRO_CMD_TYPE_WAIT,
  //======================================================
  //フェードリクエスト
  //  PARAM[0]  GFL_FADE_MASTER_BRIGHT_XXX
  //  PARAM[1]  スタート輝度(0〜16)
  //  PARAM[2]  エンド輝度(0〜16)
  //  PARAM[3]  フェードスピード
  //======================================================
  INTRO_CMD_TYPE_FADE_REQ,
  //======================================================
  //輝度セット
  //  PARAM[0]:  輝度
  //======================================================
  INTRO_CMD_TYPE_BRIGHTNESS_SET,
  //======================================================
  //輝度リクエスト
  //  PARAM[0]: Sync
  //  PARAM[1]: 1=End輝度
  //  PARAM[2]: 2=Start輝度
  //======================================================
  INTRO_CMD_TYPE_BRIGHTNESS_REQ,
  //======================================================
  //輝度ウェイト
  //======================================================
  INTRO_CMD_TYPE_BRIGHTNESS_WAIT,
  //======================================================
  //BGM再生
  //  PARAM[0]: BGM_Label
  //======================================================
  INTRO_CMD_TYPE_BGM,
  //======================================================
  //BGMフェードアウト
  //  PARAM[0]: BGMフェードアウト
  //======================================================
  INTRO_CMD_TYPE_BGM_FADEOUT,
  //======================================================
  //SE再生
  //  PARAM[0]: SE_Label
  //  PARAM[1]: volume
  //  PARAM[2]: pan
  //======================================================
  INTRO_CMD_TYPE_SE,
  //======================================================
  //指定ラベルのSE再生終了
  //  PARAM[0]: 0=SE_Label
  //======================================================
  INTRO_CMD_TYPE_SE_STOP,
  //======================================================
  //キーウェイト
  //======================================================
  INTRO_CMD_TYPE_KEY_WAIT,
  //======================================================
  //GMMをリロード
  //  PARAM[0]: 0=GflMsgLoadType, 1=msg_dat_id
  //======================================================
  INTRO_CMD_TYPE_LOAD_GMM,
  //======================================================
  //ワードセット
  //  PARAM[0]: INTRO_WORDSET 種類
  //  PARAM[1]: bufID
  //======================================================
  INTRO_CMD_TYPE_WORDSET,
  //======================================================
  //メッセージ表示
  //　PARAM[0]: str_id
  //======================================================
  INTRO_CMD_TYPE_PRINT_MSG,
  //======================================================
  //MCSSをロード
  //	PARAM:0=博士, monsoで指定ポケモン表示
  //======================================================
  INTRO_CMD_TYPE_MCSS_LOAD,
  //======================================================
  //MCSS表示切替
  //  PARAM[0]  MCSS_ID
  //  PARAM[1]  0=非表示,1=表示
  //======================================================
  INTRO_CMD_TYPE_MCSS_SET_VISIBLE,
  //======================================================
  //MCSSアニメーション設定
  //  PARAM[0]  MCSS_ID
  //  PARAM[1]  アニメーションID
  //======================================================
  INTRO_CMD_TYPE_MCSS_SET_ANIME,
  //======================================================
  //MCSSフェード(アルファ)
  //  PARAM[0]  MCSS_ID
  //  PARAM[1]  TRUE:IN, FALSE:OUT
  //======================================================
  INTRO_CMD_TYPE_MCSS_FADE_REQ,

  //======================================================
  // イントロデモ用コマンド
  //======================================================
  
  //======================================================
  //ひらがな／漢字モードを決定
  //  PARAM:0=ひらがな,1=漢字
  //======================================================
  INTRO_CMD_TYPE_SELECT_MOJI,
  //======================================================
  //性別を決定
  //  PARAM[0]  0=おとこのこ, 1=おんなのこ
  //======================================================
  INTRO_CMD_TYPE_SELECT_SEX,
  //======================================================
  //ポケモン出現演出
  //======================================================
  INTRO_CMD_TYPE_POKEMON_APPER,
  //======================================================
  //パーティクルによるモンスターボール
  //  PARAM[0]  PX
  //  PARAM[1]  PY
  //======================================================
  INTRO_CMD_TYPE_PARTICLE_MONSTERBALL,
  //======================================================
  //3D男女選択肢：表示切替
  //======================================================
  INTRO_CMD_TYPE_G3D_SELECT_SEX_SET_VISIBLE,
  //======================================================
  //3D男女選択肢：フレーム直指定
  //  PARAM[0] フレーム
  //======================================================
  INTRO_CMD_TYPE_G3D_SELECT_SEX_SET_FRAME,
  //======================================================
  //3D男女選択肢
  //======================================================
  INTRO_CMD_TYPE_G3D_SELECT_SEX_INIT,
  //======================================================
  //3D男女選択肢
  //======================================================
  INTRO_CMD_TYPE_G3D_SELECT_SEX_MAIN,
  //======================================================
  //======================================================
  INTRO_CMD_TYPE_G3D_SELECT_SEX_RETURN,
  //======================================================
  //======================================================
  INTRO_CMD_TYPE_SAVE_INIT,
  INTRO_CMD_TYPE_SAVE_SASUPEND,
  INTRO_CMD_TYPE_SAVE_RESUME,
  INTRO_CMD_TYPE_SAVE_MYSTATUS,
  INTRO_CMD_TYPE_SAVE_CHECK_ALL_END,

  INTRO_CMD_TYPE_END, ///< コマンド終了
  INTRO_CMD_TYPE_MAX,
} INTRO_CMD_TYPE;

//--------------------------------------------------------------
///	比較関数ラベル
//==============================================================
typedef enum {
  //======================================================
  //男女判定
  //  TRUE=男
  //======================================================
  INTRO_CMD_COMP_SEX,
  INTRO_CMD_COMP_MAX,
} INTRO_CMD_COMP;

//--------------------------------------------------------------
///	ワードセット指定コード
//==============================================================
typedef enum
{ 
  INTRO_WORDSET_TRAINER,
  INTRO_WORDSET_MAX,
} INTRO_WORDSET;



