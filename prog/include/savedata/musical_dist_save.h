//============================================================================================
/**
 * @file	musical_dist_save.h
 * @brief	�~���[�W�J���z�M�p�Z�[�u�f�[�^
 * @author	Nobuhiko Ariizumi
 * @date	2009/04/08
 */
//============================================================================================

#pragma once

#include "gamesystem/game_data.h"
//======================================================================
//	typedef struct
//======================================================================
//�m�ۂ���T�C�Y(256�Z�[�u�S�̃t�b�^�E4�u���b�N�t�b�^
#define MUSICAL_DIST_SAVE_SIZE ((128*1024)-(256+4))
//Alloc����T�C�Y
#define MUSICAL_DIST_SAVE_WORK_SIZE (128*1024)

typedef struct _MUSICAL_DIST_SAVE MUSICAL_DIST_SAVE;


//======================================================================
//	proto
//======================================================================
//�~���[�W�J���z�M�f�[�^��naix�������Ă��Ȃ����A�R���o�[�^�ŏ��ԌŒ�ŏo�͂��Ă���̂�
//�R�R��enum��`
enum
{
  MUSICAL_ARCDATAID_PROGDATA = 0,
  MUSICAL_ARCDATAID_GMMDATA = 1,
  MUSICAL_ARCDATAID_SCRIPTDATA = 2,
  MUSICAL_ARCDATAID_SBNKDATA = 3,
  MUSICAL_ARCDATAID_SSEQDATA = 4,
  MUSICAL_ARCDATAID_SWAVDATA = 5,
};

//----------------------------------------------------------
//	�Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//----------------------------------------------------------
extern int MUSICAL_DIST_SAVE_GetWorkSize(void);
extern void MUSICAL_DIST_SAVE_InitWork(MUSICAL_DIST_SAVE * musSave);

//----------------------------------------------------------
//	�f�[�^�̃Z�[�u�E���[�h
//----------------------------------------------------------
extern MUSICAL_DIST_SAVE* MUSICAL_DIST_SAVE_LoadData( GAMEDATA *gamedata , HEAPID heapId );
extern void MUSICAL_DIST_SAVE_UnloadData( MUSICAL_DIST_SAVE *distSave );

//----------------------------------------------------------
//	�f�[�^����̂��߂̊֐�
//----------------------------------------------------------
//�f�[�^�̐ݒ�(�Z�[�u�ALoad�EUnLoad����)
extern MUSICAL_DIST_SAVE* MUSICAL_DIST_SAVE_SaveMusicalArchive_Init( GAMEDATA *gamedata , void *arcData , const u32 size , const HEAPID heapId );
extern const BOOL MUSICAL_DIST_SAVE_SaveMusicalArchive_Main( MUSICAL_DIST_SAVE *distSave );
//�f�[�^�̎擾
extern void* MUSICAL_DIST_SAVE_GetMusicalArc( MUSICAL_DIST_SAVE *distSave );
extern const u32 MUSICAL_DIST_SAVE_GetMusicalArcSize( MUSICAL_DIST_SAVE *distSave );
