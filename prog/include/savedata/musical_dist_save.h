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
extern void MUSICAL_DIST_SAVE_UnloadData( SAVE_CONTROL_WORK *sv );

//----------------------------------------------------------
//	�f�[�^����̂��߂̊֐�
//----------------------------------------------------------
extern void* MUSICAL_DIST_SAVE_GetProgramData( MUSICAL_DIST_SAVE *distSave , const HEAPID heapId , u32 *dataSize );
extern void* MUSICAL_DIST_SAVE_GetMessageData( MUSICAL_DIST_SAVE *distSave , const HEAPID heapId , u32 *dataSize );
extern void* MUSICAL_DIST_SAVE_GetScriptData( MUSICAL_DIST_SAVE *distSave , const HEAPID heapId , u32 *dataSize );
extern void* MUSICAL_DIST_SAVE_GetStreamingData( MUSICAL_DIST_SAVE *distSave , const HEAPID heapId , u32 *dataSize );
