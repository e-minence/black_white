
//============================================================================================
/**
 * @file  tr_ai.h
 * @brief �g���[�i�[AI�v���O����
 * @author  HisashiSogabe
 * @date  06.04.26
 */
//============================================================================================
#pragma once

#include  "battle/btl_common.h"
#include  "battle/btl_main.h"
#include  "../btl_server_flow_def.h"

extern  VMHANDLE* TR_AI_Init( const BTL_MAIN_MODULE* wk, BTL_SVFLOW_WORK* svfWork, const BTL_POKE_CONTAINER* pokeCon, u32 ai_bit, HEAPID heapID );
extern  BOOL      TR_AI_Main( VMHANDLE* vmh );
extern  void      TR_AI_Exit( VMHANDLE* vmh );
extern  void      TR_AI_Start( VMHANDLE* vmh, u8* unselect, BtlPokePos pos );
extern  u8        TR_AI_GetSelectWazaPos( VMHANDLE* vmh );
extern  u8        TR_AI_GetSelectWazaDir( VMHANDLE* vmh );
