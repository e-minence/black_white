//=============================================================================================
/**
 * @file	hand_item.h
 * @brief	�|�P����WB �o�g���V�X�e��	�C�x���g�t�@�N�^�[�i�A�C�e���j�ǉ�����
 * @author	taya
 *
 * @date	2009.04.21	�쐬
 */
//=============================================================================================
#ifndef	__HAND_ITEM_H__
#define	__HAND_ITEM_H__


#include "..\btl_pokeparam.h"
#include "..\btl_event_factor.h"
#include "..\btl_server_local.h"

extern BTL_EVENT_FACTOR*  BTL_HANDLER_ITEM_Add( const BTL_POKEPARAM* pp );
extern void BTL_HANDLER_ITEM_Remove( const BTL_POKEPARAM* pp );



#endif	/* #ifndef __HAND_ITEM_H__ */
