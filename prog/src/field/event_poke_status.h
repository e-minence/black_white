///////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief �|�P�����n�̉�ʌĂяo���C�x���g
 * @file  event_poke_status.c
 * @author ariizumi
 * @date   2009.10.22
 *
 */
///////////////////////////////////////////////////////////////////////////////////////

#pragma once

//-------------------------------------------------------------------------------------
/**
 * @brief �|�P�����I���C�x���g
 *
 * @param gsys      �Q�[���V�X�e��
 * @param retDecide �I�����ʂ̊i�[�惏�[�N
 * @param retValue  �I���ʒu�̊i�[�惏�[�N
 *
 * @return �C�x���g
 */
//-------------------------------------------------------------------------------------
extern GMEVENT * EVENT_CreatePokeSelect( 
    GAMESYS_WORK * gsys, u16* retDecide , u16* retPos );

//------------------------------------------------------------------
/**
 * @brief �|�P�����I���C�x���g�F�킴�o��
 *
 * @param gsys      �Q�[���V�X�e��
 * @param fieldmap  �t�B�[���h�}�b�v
 * @param retDecide �I�����ʂ̊i�[�惏�[�N
 * @param retValue  �I���ʒu�̊i�[�惏�[�N
 * @param learnBit  �o������|�P�����������r�b�g
 * @param wazano    �o����Ώۂ̂킴�i���o�[
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_CreatePokeSelectWazaOboe( 
    GAMESYS_WORK * gsys, u16* retDecide , u16* retPos, u8 learnBit, u16 wazano );

//-------------------------------------------------------------------------------------
/**
 * @brief �|�P�����I���C�x���g�F�~���[�W�J��
 *
 * @param gsys      �Q�[���V�X�e��
 * @param retDecide �I�����ʂ̊i�[�惏�[�N
 * @param retValue  �I���ʒu�̊i�[�惏�[�N
 *
 * @return �C�x���g
 */
//-------------------------------------------------------------------------------------
extern GMEVENT * EVENT_CreatePokeSelectMusical( 
    GAMESYS_WORK * gsys, u16* retDecide , u16* retPos );

//------------------------------------------------------------------
/**
 * @brief �|�P�����I���C�x���g
 *
 * @param gsys      �Q�[���V�X�e��
 * @param fieldmap  �t�B�[���h�}�b�v
 * @param pokePos   �|�P�����̈ʒu
 * @param retDecide �I�����ʂ̊i�[�惏�[�N
 * @param retPos    �I���ʒu�̊i�[�惏�[�N
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_CreateWazaSelect( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, u16 pokePos , u16* retDecide , u16* retPos );
