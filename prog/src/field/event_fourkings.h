//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_fourkings.h
 *	@brief  �l�V�������@���o
 *	@author	tomoya takahashi
 *	@date		2009.11.21
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "gflib.h"
#include "gamesystem/gamesystem.h"
#include "field/fieldmap_proc.h"
#include "field/fieldmap_func.h"

FS_EXTERN_OVERLAY(field_event_fourkings);

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
#define FIELD_EVENT_FOURKINGS_MAX (4)

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


// �����Ă����鉉�o�C�x���g
extern GMEVENT* EVENT_FourKings_CircleWalk( 
     GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap, u32 fourkins_no );


//  ����J����ID�̃e�[�u��
extern u32 EVENT_FourKings_GetCameraID( u32 fourkins_no );

// �l�V�������Ǘ��^�X�N����
extern const FLDMAPFUNC_DATA c_FLDMAPFUNC_GHOST_SPARK_SOUND;
extern const FLDMAPFUNC_DATA c_FLDMAPFUNC_ESPERT_SOUND;
extern const FLDMAPFUNC_DATA c_FLDMAPFUNC_BAD_SOUND;
extern const FLDMAPFUNC_DATA c_FLDMAPFUNC_FIGHT_SOUND;

#ifdef _cplusplus
}	// extern "C"{
#endif



