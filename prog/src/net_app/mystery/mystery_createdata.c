//============================================================================================
/**
 * @file    mystery_createdata.c
 * @brief   �s�v�c�ʐM�f�[�^����Q�[���p�f�[�^�����֐�

             ���̊֐���NOTWIFI�ɂ����Ă���܂����A
             �ʐM���ł��g����������mystery_inline.h�ֈڂ��܂����Bnagihashi

 * @author  k.ohno
 * @date    10.01.27
 */
//============================================================================================
#include "mystery_inline.h"
#include "net_app/mystery.h"
//--------------------------------------------------------------
/**
 * @brief  �s�v�c�f�[�^����̃|�P�����쐬
 * @param  GIFT_PACK_DATA   �Z�[�u�f�[�^�Ɋi�[����Ă�s�v�c�f�[�^
 * @param  HEAPID                 POKEMON_PARAM������HEAPID
 * @param  GAMEDATA 
 * @retval POKEMON_PARAM  ���s������NULL �I������J�����Ă�������
 */
//--------------------------------------------------------------
POKEMON_PARAM* MYSTERY_CreatePokemon(const GIFT_PACK_DATA* pPack, HEAPID heapID, GAMEDATA* pGameData)
{
  return Mystery_CreatePokemon( pPack, heapID, pGameData);
}

//--------------------------------------------------------------
/**
 * @brief  �s�v�c�f�[�^����̃A�C�e���쐬
 * @param  GIFT_PACK_DATA   �Z�[�u�f�[�^�Ɋi�[����Ă�s�v�c�f�[�^
 * @retval itemsym.h�̃A�C�e���ԍ� �s���̏ꍇ ITEM_DUMMY_DATA
 */
//--------------------------------------------------------------
int MYSTERY_CreateItem(const GIFT_PACK_DATA* pPack)
{
  return Mystery_CreateItem(pPack);
}

//--------------------------------------------------------------
/**
 * @brief  �s�v�c�f�[�^�����GPower�쐬
 * @param  GIFT_PACK_DATA   �Z�[�u�f�[�^�Ɋi�[����Ă�s�v�c�f�[�^
 * @retval GPowerID
 */
//--------------------------------------------------------------
int MYSTERY_CreateGPower(const GIFT_PACK_DATA* pPack)
{
  return Mystery_CreateGPower(pPack);
}
