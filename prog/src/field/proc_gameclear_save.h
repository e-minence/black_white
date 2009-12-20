//============================================================================================
/**
 * @file  proc_gameclear_save.c
 * @date  2009.12.20
 * @author  tamada GAMEFREAK inc.
 * @brief ゲームクリア時のセーブなど
 *
 * @todo
 * 適当に作成した仮画面なので、実際にはUI班がきちんとした仕様をもとに作り直す。
 */
//============================================================================================
#pragma once

#include <gflib.h>

//--------------------------------------------------------------
/// ゲームクリア後デモ用ProcData
//--------------------------------------------------------------
extern const GFL_PROC_DATA GameClearMsgProcData;

//--------------------------------------------------------------
/// ゲームクリア後デモ用ProcDataが存在するオーバーレイID
//--------------------------------------------------------------
FS_EXTERN_OVERLAY(gameclear_demo);

