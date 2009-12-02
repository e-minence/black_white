//=============================================================================================
/**
 * @file  btl_sick.h
 * @brief �|�P����WB �o�g���V�X�e�� ��Ԉُ폈���֘A
 * @author  taya
 *
 * @date  2009.07.09  �쐬
 */
//=============================================================================================
#pragma once

#include "btl_server_flow.h"
#include "btl_pokeparam.h"


extern void BTL_SICK_AddProc( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, WazaSick sick );

extern void BTL_SICK_TurnCheckCallback( BTL_POKEPARAM* bpp, WazaSick sick, BPP_SICK_CONT oldCont, BOOL fCure, void* work );
extern BOOL BTL_SICK_MakeSickDamageMsg( BTL_HANDEX_STR_PARAMS* strParam, const BTL_POKEPARAM* bpp, WazaSick sickID );
extern int BTL_SICK_GetDefaultSickCureStrID( WazaSick sickID, BOOL fUseItem );

extern void BTL_SICK_MakeDefaultMsg( WazaSick sickID, BPP_SICK_CONT cont, const BTL_POKEPARAM* bpp, BTL_HANDEX_STR_PARAMS* str );
BOOL BTL_SICK_MakeDefaultCureMsg( WazaSick sickID, BPP_SICK_CONT cont, const BTL_POKEPARAM* bpp,
  u16 itemID, BTL_HANDEX_STR_PARAMS* str );

extern void BTL_SICKEVENT_CheckNotEffectByType( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* defender );
extern void BTL_SICKEVENT_CheckDamageAffinity(  BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* defender );
extern void BTL_SICKEVENT_CheckFlying(  BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* defender );
extern void BTL_SICKEVENT_CheckPushOutFail( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp );


extern BOOL BTL_SICK_CheckBatonTouch( WazaSick sick );

