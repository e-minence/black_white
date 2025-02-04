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
#include "btlv_scu_def.h"



#include <tcbl.h>
#include "print/gf_font.h"
#include  "battle/btl_main.h"
#include  "btlv_core.h"
#include  "btlv_effect.h"


extern BTLV_SCU*  BTLV_SCU_Create( const BTLV_CORE* vcore, const BTL_MAIN_MODULE* mainModule,
                const BTL_POKE_CONTAINER* pokeCon, GFL_TCBLSYS* tcbl,
                GFL_FONT* defaultFont, GFL_FONT* smallFont, u8 playerClientID, HEAPID heapID );
extern void BTLV_SCU_Delete( BTLV_SCU* wk );
extern void BTLV_SCU_Setup( BTLV_SCU* wk );
extern void BTLV_SCU_RestoreDefaultScreen( const BTLV_SCU* wk );

extern void BTLV_SCU_StartBtlIn( BTLV_SCU* wk, BOOL fChapterSkipMode );
extern BOOL BTLV_SCU_WaitBtlIn( BTLV_SCU* wk );
extern void BTLV_SCU_StartWazaDamageAct( BTLV_SCU* wk, BtlPokePos defPos, WazaID wazaID, BOOL fChapterSkipMode );
extern BOOL BTLV_SCU_WaitWazaDamageAct( BTLV_SCU* wk );
extern void BTLV_SCU_StartDeadAct( BTLV_SCU* wk, BtlPokePos pos, BOOL skipFlag );
extern void BTLV_SCU_StartMigawariDamageAct( BTLV_SCU* wk, BtlPokePos pos, WazaID wazaID );
extern BOOL BTLV_SCU_WaitMigawariDamageAct( BTLV_SCU* wk );
extern BOOL BTLV_SCU_WaitDeadAct( BTLV_SCU* wk );
extern void BTLV_SCU_StartReliveAct( BTLV_SCU* wk, BtlPokePos pos );
extern BOOL BTLV_SCU_WaitReliveAct( BTLV_SCU* wk );
extern void BTLV_SCU_StartPokeIn( BTLV_SCU* wk, BtlPokePos pokePos, u8 clientID, u8 memberIdx, BOOL fEffectSkip );
extern BOOL BTLV_SCU_WaitPokeIn( BTLV_SCU* wk );
extern void BTLV_SCU_StartMemberOutAct( BTLV_SCU* wk, BtlvMcssPos vpos, u16 effectNo, BOOL fSkipMode );
extern BOOL BTLV_SCU_WaitMemberOutAct( BTLV_SCU* wk );
extern void BTLV_SCU_StartHPGauge( BTLV_SCU* wk, BtlPokePos pos, BOOL fSkipEffect );
extern BOOL BTLV_SCU_WaitHPGauge( BTLV_SCU* wk );

extern void BTLV_SCU_StartWazaEffect( BTLV_SCU* wk, BtlvMcssPos atPos, BtlvMcssPos defPos,
  WazaID waza, WazaTarget wazaRange, BtlvWazaEffect_TurnType turnType, u8 continueCount );
extern BOOL BTLV_SCU_WaitWazaEffect( BTLV_SCU* wk );

extern void BTLV_SCU_TokWin_DispStart( BTLV_SCU* wk, BtlPokePos pos, BOOL fFlash );
extern BOOL BTLV_SCU_TokWin_DispWait( BTLV_SCU* wk, BtlPokePos pos );
extern void BTLV_SCU_TokWin_HideStart( BTLV_SCU* wk, BtlPokePos pos );
extern BOOL BTLV_SCU_TokWin_HideWait( BTLV_SCU* wk, BtlPokePos pos );

extern void BTLV_SCU_TokWin_Renew_Start( BTLV_SCU* wk, BtlPokePos pos );
extern BOOL BTLV_SCU_TokWin_Renew_Wait( BTLV_SCU* wk, BtlPokePos pos );

extern void BTLV_SCU_TokWin_Swap_Start( BTLV_SCU* wk, BtlPokePos pos1, BtlPokePos pos2 );
extern BOOL BTLV_SCU_TokWin_Swap_Wait( BTLV_SCU* wk );


extern void BTLV_SCU_MoveGauge_Start( BTLV_SCU* wk, BtlPokePos pos1, BtlPokePos pos2 );
extern BOOL BTLV_SCU_MoveGauge_Wait( BTLV_SCU* wk, BtlPokePos pos1, BtlPokePos pos2 );

extern void BTLV_SCU_KinomiAct_Start( BTLV_SCU* wk, BtlvMcssPos pos );
extern BOOL BTLV_SCU_KinomiAct_Wait( BTLV_SCU* wk, BtlvMcssPos pos );

extern void BTLV_SCU_UpdateGauge_Start( BTLV_SCU* wk, BtlPokePos pos );
extern BOOL BTLV_SCU_UpdateGauge_Wait( BTLV_SCU* wk, BtlPokePos pos );
extern void BTLV_SCU_FakeDisable_Start( BTLV_SCU* wk, BtlPokePos pos, BOOL fSkipMode );
extern BOOL BTLV_SCU_FakeDisable_Wait( BTLV_SCU* wk );
extern void BTLV_SCU_Hensin_Start( BTLV_SCU* wk, BtlvMcssPos vpos, BtlvMcssPos vpos_target, BOOL fSkipMode );
extern BOOL BTLV_SCU_Hensin_Wait( BTLV_SCU* wk );
extern void BTLV_SCU_ChangeForm_Start( BTLV_SCU* wk, BtlvMcssPos vpos, BOOL fSkipMode );
extern BOOL BTLV_SCU_ChangeForm_Wait( BTLV_SCU* wk );


//=============================================================================================
/**
 *
 *
 * @param   wk
 * @param   str
 *
 * @retval  extern void
 */
//=============================================================================================
extern void BTLV_SCU_PrintMsgAtOnce( BTLV_SCU* wk, const STRBUF* str );


//=============================================================================================
/**
 * メッセージ表示開始
 *
 * @param   wk
 * @param   str     表示文字列
 * @param   wait    表示後の待ち方( 0:何もしない / 1〜:通常時はキー待ち，通信時は指定フレーム待ち）
 * @param   callbackFunc    コールバック関数ポインタ（不要ならNULL）
 *
 */
//=============================================================================================
extern void BTLV_SCU_StartMsg( BTLV_SCU* wk, const STRBUF* str, u16 wait, pPrintCallBack callbackFunc );


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

extern BOOL BTLV_SCU_IsJustDoneMsg( BTLV_SCU* wk );

extern void BTLV_SCU_StartCommWaitInfo( BTLV_SCU* wk );
extern BOOL BTLV_SCU_WaitCommWaitInfo( BTLV_SCU* wk );
extern void BTLV_SCU_ClearCommWaitInfo( BTLV_SCU* wk );


extern void BTLV_SCU_MsgWinHide_Start( BTLV_SCU* wk );
extern BOOL BTLV_SCU_MsgWinHide_Wait( BTLV_SCU* wk );

//=============================================================================================
//  録画再生時フェードアウト処理
//=============================================================================================
extern void BTLV_SCU_RecPlayFadeOut_Start( BTLV_SCU* wk );
extern BOOL BTLV_SCU_RecPlayFadeOut_Wait( BTLV_SCU* wk );
extern void BTLV_SCU_RecPlayFadeIn_Start( BTLV_SCU* wk );
extern BOOL BTLV_SCU_RecPlayFadeIn_Wait( BTLV_SCU* wk );

//=============================================================================================
//  レベルアップ情報表示処理
//=============================================================================================
extern void BTLV_SCU_LvupWin_StartDisp( BTLV_SCU* wk, const BTL_POKEPARAM* bpp, const BTL_LEVELUP_INFO* lvupInfo );
extern BOOL BTLV_SCU_LvupWin_WaitDisp( BTLV_SCU* wk );
extern void BTLV_SCU_LvupWin_StartHide( BTLV_SCU* wk );
extern BOOL BTLV_SCU_LvupWin_WaitHide( BTLV_SCU* wk );
extern void BTLV_SCU_LvupWin_StepFwd( BTLV_SCU* wk );
extern BOOL BTLV_SCU_LvupWin_WaitFwd( BTLV_SCU* wk );



#endif
