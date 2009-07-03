//=============================================================================================
/**
 * @file  hand_field.h
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g�t�@�N�^�[ [�t�B�[���h�G�t�F�N�g]
 * @author  taya
 *
 * @date  2009.07.03  �쐬
 */
//=============================================================================================
#pragma once

#include "..\btl_server_flow.h"
#include "..\btl_field.h"

extern BTL_EVENT_FACTOR*  BTL_HANDLER_FLD_Add( BtlFieldEffect effect, u8 sub_param );
extern void BTL_HANDLER_FLD_Remove( BTL_EVENT_FACTOR* factor );

