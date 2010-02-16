//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		debug_mystery_card.h
 *	@brief  �f�o�b�O�p�ӂ����Ȃ�������̍쐬  
 *	@author	Toru=Nagihashi
 *	@date		2010.02.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
//�Z�[�u�f�[�^
#include "savedata/mystery_data.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================

//�|�P�����f�[�^�쐬
extern void DEBUG_MYSTERY_SetGiftPokeData( GIFT_PACK_DATA *p_data, u32 event_id );

//�ǂ����f�[�^�쐬
extern void DEBUG_MYSTERY_SetGiftItemData( GIFT_PACK_DATA *p_data, u32 event_id );

//GPOWER�f�[�^�쐬
void DEBUG_MYSTERY_SetGiftGPowerData( GIFT_PACK_DATA *p_data, u32 event_id );

//���f�[�^�ȊO�̂��̂��Z�b�g
extern void DEBUG_MYSTERY_SetGiftCommonData( GIFT_PACK_DATA *p_data, u32 event_id, BOOL only_one_flag );
