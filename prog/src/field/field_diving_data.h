//============================================================================================
/**
 * @file  field_diving_data.h
 * @brief �_�C�r���O�ڑ���`�F�b�N�p�f�[�^�Ƃ��̃A�N�Z�X�֐�
 * @date  2010.02.21
 * @author  tamada GAME FREAK inc.
 */
//============================================================================================
#pragma once

#include <gflib.h>

#include "field/fieldmap_proc.h"
//-----------------------------------------------------------------------------
/**
 * @brief �_�C�r���O���菈��
 * @param fieldmap  FIELDMAP_WORK�ւ̃|�C���^
 * @param zone_id   �J�ڐ�}�b�vID���󂯎�邽�߂̃|�C���^
 * @return  BOOL  TRUE�̂Ƃ��A�_�C�r���O�\
 */
//-----------------------------------------------------------------------------
extern BOOL DIVINGSPOT_Check( FIELDMAP_WORK * fieldmap, u16 * zone_id );

