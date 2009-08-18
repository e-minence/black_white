///////////////////////////////////////////////////////////////////////////////////////////////
/**
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
 * @param p_parent   �e�C�x���g
 * @param p_gsys     �Q�[���V�X�e��
 * @param p_fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//----------------------------------------------------------------------
extern void EVENT_CAMERA_ACT_CallDoorInEvent( 
    GMEVENT* p_parent, GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );

//---------------------------------------------------------------------
/**
 * @brief �h�A����o�Ă����ۂ�, �J�����̏����ݒ���s��
 *
 * @param p_fieldmap �t�B�[���h�}�b�v
 */
//---------------------------------------------------------------------
extern void EVENT_CAMERA_ACT_PrepareForDoorOut( FIELDMAP_WORK* p_fieldmap );

//--------------------------------------------------------------------
/**
 * @brief �J��������C�x���g���Ăяo��( �o���� )
 *
 * @param p_parent   �e�C�x���g
 * @param p_gsys     �Q�[���V�X�e��
 * @param p_fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//--------------------------------------------------------------------
extern void EVENT_CAMERA_ACT_CallDoorOutEvent( 
    GMEVENT* p_parent, GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap ); 

//--------------------------------------------------------------------
/**
 * @breif �J������Near�v���[����Far�v���[�����f�t�H���g�ݒ�ɖ߂�
 *
 * @param p_fieldmap �t�B�[���h�}�b�v
 */
//--------------------------------------------------------------------
extern void EVENT_CAMERA_ACT_ResetCameraParameter( FIELDMAP_WORK* p_fieldmap );
