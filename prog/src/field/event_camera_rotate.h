
#pragma once
#include "gflib.h"
#include "gamesystem/gamesystem.h"
#include "field/fieldmap_proc.h"


//----------------------------------------------------------------------
/**
 * @brief �J������]�C�x���g���Ăяo��( �������� )
 *
 * @param p_parent   �e�C�x���g
 * @param p_gsys     �Q�[���V�X�e��
 * @param p_fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//----------------------------------------------------------------------
extern void EVENT_CAMERA_ROTATE_CallDoorInEvent( 
    GMEVENT* p_parent, GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );

//---------------------------------------------------------------------
/**
 * @brief ���E�ɂ���h�A����o�Ă����ۂ�, �J�����̏����ݒ���s��
 *
 * @param p_fieldmap �t�B�[���h�}�b�v
 */
//---------------------------------------------------------------------
extern void EVENT_CAMERA_ROTATE_PrepareDoorOut( FIELDMAP_WORK* p_fieldmap );

//--------------------------------------------------------------------
/**
 * @brief �J������]�C�x���g���Ăяo��( �o���� )
 *
 * @param p_parent   �e�C�x���g
 * @param p_gsys     �Q�[���V�X�e��
 * @param p_fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//--------------------------------------------------------------------
extern void EVENT_CAMERA_ROTATE_CallDoorOutEvent( 
    GMEVENT* p_parent, GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap ); 
