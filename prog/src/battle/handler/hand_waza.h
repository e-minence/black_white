//=============================================================================================
/**
 * @file  hand_waza.h
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g�t�@�N�^�[�i���U�j�ǉ�����
 * @author  taya
 *
 * @date  2009.05.15  �쐬
 */
//=============================================================================================
#pragma once


#include "..\btl_pokeparam.h"
#include "..\btl_event_factor.h"
#include "..\btl_server_local.h"


extern BTL_EVENT_FACTOR*  BTL_HANDLER_Waza_Add( const BTL_POKEPARAM* pp, WazaID waza );
extern void BTL_HANDLER_Waza_Remove( const BTL_POKEPARAM* pp, WazaID waza );
extern void BTL_HANDLER_Waza_RemoveForce( const BTL_POKEPARAM* pp, WazaID waza );


