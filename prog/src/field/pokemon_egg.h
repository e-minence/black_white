//////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �|�P�����^�}�S
 * @file   pokemon_egg.h
 * @author obata
 * @date   2010.04.21
 */
////////////////////////////////////////////////////////////////////////////////////////// 
#pragma once

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S���쐬����
 *
 * @param heap_id  �g�p����q�[�vID
 * @param mystatus �^�}�S�̏��L��
 * @param memo     �g���[�i�[�����̋L��
 * @param poke1    �e�|�P����1
 * @param poke2    �e�|�P����2
 * @param egg      �쐬�����^�}�S�̊i�[��
 */
//---------------------------------------------------------------------------------------- 
extern void POKEMON_EGG_Create( 
    HEAPID heap_id, const MYSTATUS* mystatus, int memo, 
    const POKEMON_PARAM* poke1, const POKEMON_PARAM* poke2, POKEMON_PARAM* egg );

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S��z��������
 *
 * @param egg     �z��������^�}�S
 * @param owner   �^�}�S�̏�����
 * @param heap_id �g�p����q�[�vID
 */
//---------------------------------------------------------------------------------------- 
extern void POKEMON_EGG_Birth( POKEMON_PARAM* egg, const MYSTATUS* owner, HEAPID heap_id );


#ifdef PM_DEBUG
//---------------------------------------------------------------------------------------- 
/**
 * @brief �f�o�b�O�p�^�}�S���쐬����
 *
 * @param gameData
 * @param heapID
 *
 * @return �쐬�����^�}�S
 */
//---------------------------------------------------------------------------------------- 
extern POKEMON_PARAM* DEBUG_POKEMON_EGG_CreatePlainEgg( GAMEDATA* gameData, HEAPID heap_id ); // �Y�܂ꂽ��
extern POKEMON_PARAM* DEBUG_POKEMON_EGG_CreateQuickEgg( GAMEDATA* gameData, HEAPID heap_id ); // ���������z��
extern POKEMON_PARAM* DEBUG_POKEMON_EGG_CreateIllegalEgg( GAMEDATA* gameData, HEAPID heap_id ); // ���߃^�}�S
#endif // PM_DEBUG
