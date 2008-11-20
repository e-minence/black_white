//=============================================================================================
/**
 * @file	btlv_scd.h
 * @brief	�|�P����WB �o�g�� �`�� ����ʐ��䃂�W���[��
 * @author	taya
 *
 * @date	2008.11.18	�쐬
 */
//=============================================================================================
#ifndef __BTLV_SCD_H__
#define __BTLV_SCD_H__

//--------------------------------------------------------------
/**
 *	���W���[���n���h���^
 */
//--------------------------------------------------------------
typedef struct _BTLV_SCD	BTLV_SCD;

#include <tcbl.h>
#include "print/gf_font.h"

#include "btl_pokeparam.h"
#include "btl_action.h"
#include "btlv_core.h"


extern BTLV_SCD*  BTLV_SCD_Create( const BTLV_CORE* vcore, GFL_TCBLSYS* tcbl, GFL_FONT* font, HEAPID heapID );
extern void BTLV_SCD_Delete( BTLV_SCD* wk );

extern void BTLV_SCD_Setup( BTLV_SCD* wk );

extern void BTLV_SCD_StartActionSelect( BTLV_SCD* wk, const BTL_POKEPARAM* pp );
extern BOOL BTLV_SCD_WaitActionSelect( BTLV_SCD* wk );

extern void BTLV_SCD_StartPokemonSelect( BTLV_SCD* wk );
extern BOOL BTLV_SCD_WaitPokemonSelect( BTLV_SCD* wk );


extern void BTLV_SCD_GetSelectAction( BTLV_SCD* wk, BTL_ACTION_PARAM* action );

#endif
