//=============================================================================================
/**
 * @file  hand_side.h
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g�t�@�N�^�[ [�T�C�h�G�t�F�N�g]
 * @author  taya
 *
 * @date  2009.06.18  �쐬
 */
//=============================================================================================
#pragma once

#include "..\btl_pokeparam.h"
#include "..\btl_server_flow.h"


extern void BTL_HANDLER_SIDE_InitSystem( void );
extern BTL_EVENT_FACTOR*  BTL_HANDLER_SIDE_Add( BtlSide side, BtlSideEffect sideEffect, BPP_SICK_CONT contParam );
extern BOOL BTL_HANDLER_SIDE_Remove( BtlSide side, BtlSideEffect sideEffect );
extern BOOL BTL_HANDER_SIDE_IsExist( BtlSide side, BtlSideEffect effect );

/**
 *  �^�[���`�F�b�N���Ɍ��ʂ��؂ꂽ�G�t�F�N�g�̎�ށ��T�C�h��ʒm���邽�߂̃R�[���o�b�N�֐��^
 */
typedef void (*pSideEffEndCallBack)( BtlSide side, BtlSideEffect sideEffect, void* arg );

extern void BTL_HANDLER_SIDE_TurnCheck( pSideEffEndCallBack callBack, void* callbackArg );

