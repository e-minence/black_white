///////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   event_camera_act.h
 * @brief  �h�A�ɏo���肷��ۂ̃J��������C�x���g
 * @author obata
 * @date   2009.08.17
 */
///////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "gflib.h"
#include "gamesystem/gamesystem.h"
#include "field/fieldmap_proc.h"


//----------------------------------------------------------------------
/**
 * @brief �J��������C�x���g���Ăяo��( �������� )
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//----------------------------------------------------------------------
extern void EVENT_CAMERA_ACT_CallDoorInEvent( 
    GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap );

//---------------------------------------------------------------------
/**
 * @brief �h�A����o�Ă����ۂ�, �J�����̏����ݒ���s��
 *
 * @param fieldmap �t�B�[���h�}�b�v
 */
//---------------------------------------------------------------------
extern void EVENT_CAMERA_ACT_PrepareForDoorOut( FIELDMAP_WORK* fieldmap );

//--------------------------------------------------------------------
/**
 * @brief �J��������C�x���g���Ăяo��( �o���� )
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//--------------------------------------------------------------------
extern void EVENT_CAMERA_ACT_CallDoorOutEvent( 
    GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap ); 

//--------------------------------------------------------------------
/**
 * @breif �J������Near�v���[����Far�v���[�����f�t�H���g�ݒ�ɖ߂�
 *
 * @param fieldmap �t�B�[���h�}�b�v
 */
//--------------------------------------------------------------------
extern void EVENT_CAMERA_ACT_ResetCameraParameter( FIELDMAP_WORK* fieldmap );
