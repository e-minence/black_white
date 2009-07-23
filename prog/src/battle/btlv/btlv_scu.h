//=============================================================================================
/**
 * @file  btlv_scu.h
 * @brief ポケモンWB バトル 描画 上画面制御モジュール
 * @author  taya
 *
 * @date  2008.11.18  作成
 */
//=============================================================================================
#ifndef __BTLV_SCU_H__
#define __BTLV_SCU_H__

//--------------------------------------------------------------
/**
 *  モジュールハンドラ型
 */
//--------------------------------------------------------------
typedef struct _BTLV_SCU  BTLV_SCU;

//--------------------------------------------------------------
/**
 *  メッセージウェイト指定
 */
//--------------------------------------------------------------
enum {

  BTLV_MSGWAIT_NONE = 0,
  BTLV_MSGWAIT_STD = 120,

};


#include <tcbl.h>
#include "print/gf_font.h"
#include  "battle/btl_main.h"
#include  "btlv_core.h"
#include  "btlv_effect.h"


extern BTLV_SCU*  BTLV_SCU_Create( const BTLV_CORE* vcore, const BTL_MAIN_MODULE* mainModule,
                const BTL_POKE_CONTAINER* pokeCon, GFL_TCBLSYS* tcbl, GFL_FONT* defaultFont, u8 playerClientID, HEAPID heapID );
extern void BTLV_SCU_Delete( BTLV_SCU* wk );
extern void BTLV_SCU_Setup( BTLV_SCU* wk );
extern void BTLV_SCU_StartBtlIn( BTLV_SCU* wk );
extern BOOL BTLV_SCU_WaitBtlIn( BTLV_SCU* wk );
extern void BTLV_SCU_StartWazaDamageAct( BTLV_SCU* wk, BtlPokePos defPos );
extern BOOL BTLV_SCU_WaitWazaDamageAct( BTLV_SCU* wk );
extern void BTLV_SCU_StartDeadAct( BTLV_SCU* wk, BtlPokePos pos );
extern BOOL BTLV_SCU_WaitDeadAct( BTLV_SCU* wk );
extern void BTLV_SCU_StartPokeIn( BTLV_SCU* wk, BtlPokePos pokePos, u8 clientID, u8 memberIdx );
extern BOOL BTLV_SCU_WaitPokeIn( BTLV_SCU* wk );
extern void BTLV_SCU_StartMemberOutAct( BTLV_SCU* wk, BtlvMcssPos vpos );
extern BOOL BTLV_SCU_WaitMemberOutAct( BTLV_SCU* wk );
extern void BTLV_SCU_StartHPGauge( BTLV_SCU* wk, BtlPokePos pos );
extern BOOL BTLV_SCU_WaitHPGauge( BTLV_SCU* wk );

extern void BTLV_SCU_StartWazaEffect( BTLV_SCU* wk, BtlvMcssPos atPos, BtlvMcssPos defPos,
  WazaID waza, BtlvWazaEffect_TurnType turnType, u8 continueCount );
extern BOOL BTLV_SCU_WaitWazaEffect( BTLV_SCU* wk );

extern void BTLV_SCU_DispTokWin( BTLV_SCU* wk, BtlPokePos pos );
extern void BTLV_SCU_HideTokWin( BTLV_SCU* wk, BtlPokePos pos );
extern void BTLV_SCU_TokWin_Renew_Start( BTLV_SCU* wk, BtlPokePos pos );
extern BOOL BTLV_SCU_TokWin_Renew_Wait( BTLV_SCU* wk, BtlPokePos pos );

extern void BTLV_SCU_KinomiAct_Start( BTLV_SCU* wk, BtlvMcssPos pos );
extern BOOL BTLV_SCU_KinomiAct_Wait( BTLV_SCU* wk, BtlvMcssPos pos );


//=============================================================================================
/**
 * メッセージ表示開始
 *
 * @param   wk
 * @param   str     表示文字列
 * @param   wait    表示後の待ち方( 0:何もしない / 1〜:通常時はキー待ち，通信時は指定フレーム待ち）
 *
 */
//=============================================================================================
extern void BTLV_SCU_StartMsg( BTLV_SCU* wk, const STRBUF* str, u16 waitFrames );

//=============================================================================================
/**
 * メッセージ表示終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL    TRUEで終了
 */
//=============================================================================================
extern BOOL BTLV_SCU_WaitMsg( BTLV_SCU* wk );


#endif
