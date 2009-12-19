///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �|�P�Z��PC�֘A�C�x���g
 * @file   event_pokecen_pc.c
 * @author obata
 * @date   2009.12.07
 */
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "gamesystem/gamesystem.h"  // for GMEVENT, GAMESYS_WORK
#include "field/fieldmap_proc.h"  // for FIELDMAP_WORK


//-----------------------------------------------------------------------------------------
/**
 * @brief �|�P�Z����PC�N���C�x���g���쐬����
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------
extern GMEVENT* EVENT_PokecenPcOn( GMEVENT* parent, 
                                   GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap );

//-----------------------------------------------------------------------------------------
/**
 * @brief �|�P�Z����PC�N�����C�x���g���쐬����
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------
extern GMEVENT* EVENT_PokecenPcRun( GMEVENT* parent, 
                                    GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap );

//-----------------------------------------------------------------------------------------
/**
 * @brief �|�P�Z����PC��~�C�x���g���쐬����
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------
extern GMEVENT* EVENT_PokecenPcOff( GMEVENT* parent, 
                                    GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap );
