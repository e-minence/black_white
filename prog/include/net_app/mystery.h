//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mystery.h
 *	@brief  �ӂ����Ȃ�������̃v���Z�X
 *	@author	Toru=Nagihashi
 *	@date		2009.12.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <gflib.h>
#include "gamesystem/game_data.h"
#include "savedata/mystery_data.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
FS_EXTERN_OVERLAY(mystery);

//=============================================================================
/**
 *					����
*/
//=============================================================================
typedef struct
{ 
  int dummy;
}MYSTERY_PARAM;

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
extern const GFL_PROC_DATA MysteryGiftProcData;

//--------------------------------------------------------------
/**
 * @brief  �s�v�c�f�[�^����̃|�P�����쐬
 * @param  GIFT_PACK_DATA   �Z�[�u�f�[�^�Ɋi�[����Ă�s�v�c�f�[�^
 * @param  HEAPID                 POKEMON_PARAM������HEAPID
 * @param  GAMEDATA 
 * @param  POKEMON_PARAM  �s���Ȃ�NULL  �I������J�����Ă�������
 */
//--------------------------------------------------------------
extern POKEMON_PARAM* MYSTERY_CreatePokemon(const GIFT_PACK_DATA* pGift, HEAPID heapID, GAMEDATA* pGameData);

//--------------------------------------------------------------
/**
 * @brief  �s�v�c�f�[�^����̃A�C�e���쐬
 * @param  GIFT_PACK_DATA   �Z�[�u�f�[�^�Ɋi�[����Ă�s�v�c�f�[�^
 * @retval itemsym.h�̃A�C�e���ԍ� �s���̏ꍇ ITEM_DUMMY_DATA
 */
//--------------------------------------------------------------
extern int MYSTERY_CreateItem(const GIFT_PACK_DATA* pGift);

//--------------------------------------------------------------
/**
 * @brief  �s�v�c�f�[�^�����GPower�쐬
 * @param  GIFT_PACK_DATA   �Z�[�u�f�[�^�Ɋi�[����Ă�s�v�c�f�[�^
 * @retval @todo ����
 */
//--------------------------------------------------------------
extern int MYSTERY_CreateGPower(const GIFT_PACK_DATA* pGift);

