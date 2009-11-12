//=============================================================================================
/**
 * @file  btlv_scd.h
 * @brief �|�P����WB �o�g�� �`�� ����ʐ��䃂�W���[��
 * @author  taya
 *
 * @date  2008.11.18  �쐬
 */
//=============================================================================================
#ifndef __BTLV_SCD_H__
#define __BTLV_SCD_H__

//--------------------------------------------------------------
/**
 *  ���W���[���n���h���^
 */
//--------------------------------------------------------------
typedef struct _BTLV_SCD  BTLV_SCD;

#include <tcbl.h>
#include "print/gf_font.h"

#include "battle/btl_main.h"
#include "battle/btl_pokeparam.h"
#include "battle/btl_action.h"
#include "battle/btl_pokeselect.h"
#include "btlv_core.h"

//--------------------------------------------------------------
/**
 *  �A�N�V�����I�𒆂̌o��
 */
//--------------------------------------------------------------
typedef enum {
  BTLV_SCD_SelAction_Still = 0, ///< �܂��I��
  BTLV_SCD_SelAction_Done,      ///< �I���������

  BTLV_SCD_SelAction_Warn_Kodawari, ///< ������蒆�A�o���Ȃ����U���I�΂ꂽ
  BTLV_SCD_SelAction_Warn_WazaLock, ///< �o���Ȃ����U���I�΂ꂽ

  BTLV_SCD_SelAction_Fight,
  BTLV_SCD_SelAction_Item,
  BTLV_SCD_SelAction_Pokemon,
  BTLV_SCD_SelAction_Escape,

}BtlvScd_SelAction_Result;







extern BTLV_SCD*  BTLV_SCD_Create( const BTLV_CORE* vcore,
    const BTL_MAIN_MODULE* mainModule, const BTL_POKE_CONTAINER* pokeCon, GFL_TCBLSYS* tcbl, GFL_FONT* font, u8 playerClientID, HEAPID heapID );
extern void BTLV_SCD_Delete( BTLV_SCD* wk );

extern void BTLV_SCD_Init( BTLV_SCD* wk );

extern void BTLV_SCD_Cleanup( BTLV_SCD* wk );
extern void BTLV_SCD_Setup( BTLV_SCD* wk );

extern void BTLV_SCD_RestartUI( BTLV_SCD* wk );

extern void BTLV_SCD_StartActionSelect( BTLV_SCD* wk, const BTL_POKEPARAM* bpp, BOOL fPrevButton, BTL_ACTION_PARAM* dest );
extern BtlAction BTLV_SCD_WaitActionSelect( BTLV_SCD* wk );
extern void BTLV_SCD_RestartActionSelect( BTLV_SCD* wk );

extern void BTLV_SCD_StartWazaSelect( BTLV_SCD* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* dest );
extern BOOL BTLV_SCD_WaitWazaSelect( BTLV_SCD* wk );

extern void BTLV_SCD_StartTargetSelect( BTLV_SCD* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* dest );
extern BOOL BTLV_SCD_WaitTargetSelect( BTLV_SCD* wk );

extern BOOL BTLV_SCD_IsSelTargetDone( BTLV_SCD* wk );

extern void BTLV_SCD_FadeOut( BTLV_SCD* wk );
extern void BTLV_SCD_FadeIn( BTLV_SCD* wk );
extern BOOL BTLV_SCD_FadeFwd( BTLV_SCD* wk );
extern u8*  BTLV_SCD_GetCursorFlagPtr( BTLV_SCD* wk );


#endif
