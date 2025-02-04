//=============================================================================================
/**
 * @file  btlv_scd.h
 * @brief ポケモンWB バトル 描画 下画面制御モジュール
 * @author  taya
 *
 * @date  2008.11.18  作成
 */
//=============================================================================================
#ifndef __BTLV_SCD_H__
#define __BTLV_SCD_H__

//--------------------------------------------------------------
/**
 *  モジュールハンドラ型
 */
//--------------------------------------------------------------
typedef struct _BTLV_SCD  BTLV_SCD;

#include <tcbl.h>
#include "print/gf_font.h"

#include "battle/btl_main.h"
#include "battle/btl_pokeparam.h"
#include "battle/btl_action.h"
#include "battle/btl_pokeselect.h"
#include "btlv_input.h"
#include "btlv_core.h"

//--------------------------------------------------------------
/**
 *  アクション選択中の経過
 */
//--------------------------------------------------------------
typedef enum {
  BTLV_SCD_SelAction_Still = 0, ///< まだ選択中
  BTLV_SCD_SelAction_Done,      ///< 選択おわった

  BTLV_SCD_SelAction_Warn_Kodawari, ///< こだわり中、出せないワザが選ばれた
  BTLV_SCD_SelAction_Warn_WazaLock, ///< 出せないワザが選ばれた

  BTLV_SCD_SelAction_Fight,
  BTLV_SCD_SelAction_Item,
  BTLV_SCD_SelAction_Pokemon,
  BTLV_SCD_SelAction_Escape,

}BtlvScd_SelAction_Result;







extern BTLV_SCD*  BTLV_SCD_Create( const BTLV_CORE* vcore,
    const BTL_MAIN_MODULE* mainModule, const BTL_POKE_CONTAINER* pokeCon, GFL_TCBLSYS* tcbl, GFL_FONT* font,
    const BTL_CLIENT* client, BtlBagMode bagMode, HEAPID heapID );

extern void BTLV_SCD_Delete( BTLV_SCD* wk );

extern void BTLV_SCD_Init( BTLV_SCD* wk );

extern void BTLV_SCD_Cleanup( BTLV_SCD* wk );
extern void BTLV_SCD_Setup( BTLV_SCD* wk );

extern void BTLV_SCD_RestartUI( BTLV_SCD* wk );
extern BOOL BTLV_SCD_WaitRestartUI( BTLV_SCD* wk );
extern void BTLV_SCD_RestartUI_IfNotStandBy( BTLV_SCD* wk );



extern void BTLV_SCD_StartActionSelect( BTLV_SCD* wk, const BTL_POKEPARAM* bpp, BOOL fPrevButton, u8 shooterEnergy, BTL_ACTION_PARAM* dest );
extern void BTLV_SCD_StartActionSelectDemoCapture( BTLV_SCD* wk, const BTL_POKEPARAM* bpp, BOOL fPrevButton, BTL_ACTION_PARAM* dest );
extern BtlAction BTLV_SCD_WaitActionSelect( BTLV_SCD* wk );

extern void BTLV_SCD_StartWazaSelect( BTLV_SCD* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* dest );
extern void BTLV_SCD_StartRotationWazaSelect( BTLV_SCD* wk, BTLV_ROTATION_WAZASEL_PARAM* selParam, BTL_ACTION_PARAM* actionParam );
extern void BTLV_SCD_StartWazaSelectDemoCapture( BTLV_SCD* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* dest );
extern BOOL BTLV_SCD_WaitWazaSelect( BTLV_SCD* wk );
extern void BTLV_SCD_RestartWazaSelect( BTLV_SCD* wk );

extern void BTLV_SCD_StartTargetSelect( BTLV_SCD* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* dest );
extern BOOL BTLV_SCD_WaitTargetSelect( BTLV_SCD* wk );

extern void BTLV_SCD_ForceQuitSelect( BTLV_SCD* wk );


extern BOOL BTLV_SCD_IsSelTargetDone( BTLV_SCD* wk );

extern void BTLV_SCD_FadeOut( BTLV_SCD* wk, BOOL start );
extern void BTLV_SCD_FadeIn( BTLV_SCD* wk );
extern BOOL BTLV_SCD_FadeFwd( BTLV_SCD* wk );
extern u8*  BTLV_SCD_GetCursorFlagPtr( BTLV_SCD* wk );

extern void BTLV_SCD_SelectRotate_Start( BTLV_SCD* wk, BTLV_INPUT_ROTATE_PARAM* param );
extern BOOL BTLV_SCD_SelectRotate_Wait( BTLV_SCD* wk, BtlRotateDir* result );
extern void BTLV_SCD_SelectRotate_ForceQuit( BTLV_SCD* wk );


extern void BTLV_SCD_SelectYesNo_Start( BTLV_SCD* wk, BTLV_INPUT_YESNO_PARAM* param );
extern BOOL BTLV_SCD_SelectYesNo_Wait( BTLV_SCD* wk, BtlYesNo* result );

//=============================================================================================
//  時間制限によるアクション選択の強制終了対応
//=============================================================================================
extern void   BTLV_SCD_ForceQuitInput_Notify( BTLV_SCD* wk );
extern BOOL BTLV_SCD_ForceQuitInput_Wait( BTLV_SCD* wk );


//=============================================================================================
//  録画再生モード
//=============================================================================================
extern void BTLV_SCD_SetupRecPlayerMode( BTLV_SCD* wk, const BTLV_INPUT_BATTLE_RECORDER_PARAM* param );
extern int BTLV_SCD_CheckRecPlayerInput( BTLV_SCD* wk );


#endif
