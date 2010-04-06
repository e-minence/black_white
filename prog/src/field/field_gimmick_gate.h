/////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �M�~�b�N�o�^�֐�(�Q�[�g)
 * @file   field_gimmick_gate.h
 * @author obata
 * @date   2009.10.21
 *
 */
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "fieldmap.h"
#include "gimmick_obj_elboard.h"


//===========================================================================
// ���M�~�b�N�o�^�֐�
//===========================================================================
extern void GATE_GIMMICK_Setup( FIELDMAP_WORK* fieldmap );
extern void GATE_GIMMICK_End( FIELDMAP_WORK* fieldmap );
extern void GATE_GIMMICK_Move( FIELDMAP_WORK* fieldmap );


//---------------------------------------------------------------------------
/**
 * @brief �d���f���̌������擾����
 *
 * @param fieldmap
 *
 * @return �d���f���̌���( DIR_xxxx )
 */
//---------------------------------------------------------------------------
extern u8 GATE_GIMMICK_GetElboardDir( FIELDMAP_WORK* fieldmap );
