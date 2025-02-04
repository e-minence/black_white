//======================================================================
/**
 * @file    scrcmd_proc_fld.h
 * @brief   スクリプトコマンド用関数　各種アプリ呼び出し系(フィールド常駐)
 * @author  Miyuki Iwasawa 
 * @date    09.10.23
 *
 * フェードやフィールドプロセスコントロールを自前で行うプロセス呼び出し法を取り
 * プロセス呼び出しコマンド自体が常駐している必要があるときは
 * scrcmd_proc.cにコマンドを置いてください
 */
//======================================================================

#pragma once

//--------------------------------------------------------------
/**
 * @brief   ボックスプロセスを呼び出します
 * @param   core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdCallBoxProc( VMHANDLE *core, void *wk ); 

//--------------------------------------------------------------
/**
 * @brief   すれ違い『挨拶メッセージ』入力画面の呼び出し
 * @param   core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdCrossCommHelloMessageInputCall( VMHANDLE *core, void *wk );

//--------------------------------------------------------------
/**
 * @brief   すれ違い『お礼メッセージ』入力画面の呼び出し
 * @param   core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdCrossCommThanksMessageInputCall( VMHANDLE *core, void *wk );

//--------------------------------------------------------------
//  ゲームクリア処理呼び出し
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdGameClearDemo( VMHANDLE *core, void *wk );

//--------------------------------------------------------------
/// Cギア入手デモ呼び出し
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdCallCGearGetDemo( VMHANDLE *core, void *wk );

///ジオネット呼び出し
extern VMCMD_RESULT EvCmdCallGeonet( VMHANDLE *core, void *wk ); 

// 殿堂入り画面呼び出し
extern VMCMD_RESULT EvCmdCallDendouProc( VMHANDLE *core, void *wk );

// ゲーム内マニュアルの呼び出し
extern VMCMD_RESULT EvCmdCallGameManual( VMHANDLE *core, void *wk );
