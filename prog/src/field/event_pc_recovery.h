////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �|�P�Z���񕜃A�j���[�V�����C�x���g
 * @file   event_pc_recovery.h
 * @author obata_toshihiro
 * @date   2009.09.18
 */
//////////////////////////////////////////////////////////////////////////////// 
#pragma once 

//------------------------------------------------------------------------------
/**
 * @brief �|�P�Z���񕜃A�j���[�V�����C�x���g���쐬����
 *
 * @param gsys        �Q�[���V�X�e��
 * @param parent      �e�C�x���g
 * @param machine_pos �񕜃}�V���̍��W
 * @param pokemon_num �莝���|�P�����̐�
 *
 * @return �쐬�����񕜃A�j���C�x���g
 */
//------------------------------------------------------------------------------
GMEVENT* EVENT_PcRecoveryAnime( 
    GAMESYS_WORK* gsys, GMEVENT* parent, VecFx32* mechine_pos, u8 pokemon_num );
