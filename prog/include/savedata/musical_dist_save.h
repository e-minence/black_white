//============================================================================================
/**
 * @file	musical_dist_save.h
 * @brief	�~���[�W�J���z�M�p�Z�[�u�f�[�^
 * @author	Nobuhiko Ariizumi
 * @date	2009/04/08
 */
//============================================================================================

#pragma once

#include "savedata/save_control.h"	//SAVE_CONTROL_WORK�Q�Ƃ̂���
//======================================================================
//	typedef struct
//======================================================================

typedef struct _MUSICAL_DIST_SAVE MUSICAL_DIST_SAVE;


//======================================================================
//	proto
//======================================================================

//----------------------------------------------------------
//	�Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//----------------------------------------------------------
extern int MUSICAL_DIST_SAVE_GetWorkSize(void);
extern void MUSICAL_DIST_SAVE_InitWork(MUSICAL_DIST_SAVE * musSave);

//----------------------------------------------------------
//	�f�[�^�̃Z�[�u�E���[�h
//----------------------------------------------------------
extern MUSICAL_DIST_SAVE* MUSICAL_DIST_SAVE_LoadData( SAVE_CONTROL_WORK *sv , HEAPID heapId );
extern void MUSICAL_DIST_SAVE_UnloadData( MUSICAL_DIST_SAVE *distSave );

//----------------------------------------------------------
//	�f�[�^����̂��߂̊֐�
//----------------------------------------------------------
//�f�[�^�̐ݒ�(�Z�[�u�ALoad�EUnLoad����)
extern MUSICAL_DIST_SAVE* MUSICAL_DIST_SAVE_SaveMusicalArchive_Init( SAVE_CONTROL_WORK *sv , void *arcData , const u32 size , const HEAPID heapId );
extern const BOOL MUSICAL_DIST_SAVE_SaveMusicalArchive_Main( MUSICAL_DIST_SAVE *distSave );
//�f�[�^�̎擾
extern void* MUSICAL_DIST_SAVE_GetMusicalArc( MUSICAL_DIST_SAVE *distSave );
extern const u32 MUSICAL_DIST_SAVE_GetMusicalArcSize( MUSICAL_DIST_SAVE *distSave );
