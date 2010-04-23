//======================================================================
/**
 * @file	eventwork.h
 * @brief	�Z�[�u�t���O�A���[�N����
 * @author	tamada  GAMEFREAK inc.
 * @date	05.10.22
 */
//======================================================================
#pragma once

#include <gflib.h>


//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
/// SYS_FLAG_MODE
//--------------------------------------------------------------
typedef enum
{
  SYS_FLAG_MODE_SET = 0, ///<�t���O�Z�b�g
  SYS_FLAG_MODE_RESET, ///<�t���O���Z�b�g
  SYS_FLAG_MODE_CHECK, ///<�t���O�`�F�b�N
}SYS_FLAG_MODE;

//======================================================================
//  struct
//======================================================================
//----------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�^��`
 */
//----------------------------------------------------------
typedef struct _EVENTWORK EVENTWORK;

//======================================================================
//  extern
//======================================================================
//----------------------------------------------------------
//	�Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//----------------------------------------------------------
extern int EVENTWORK_GetWorkSize(void);
extern void EVENTWORK_InitWork( u8 *work );
#if 0
extern void EVENTWORK_LoadEventWork(
    EVENTWORK *save, const EVENTWORK *load );
extern EVENTWORK * EVENTWORK_AllocWork( HEAPID heapID );
extern void EVENTWORK_FreeWork( EVENTWORK *evwk );
extern void EVENTWORK_Init(EVENTWORK * evwk);
#endif

//----------------------------------------------------------
//	EVENTWORK����̂��߂̊֐�
//----------------------------------------------------------
extern BOOL EVENTWORK_CheckEventFlag( EVENTWORK * ev, u16 flag_no);
extern void EVENTWORK_SetEventFlag( EVENTWORK * ev, u16 flag_no);
extern void EVENTWORK_ResetEventFlag( EVENTWORK * ev, u16 flag_no);
extern void EVENTWORK_ClearEventFlags( EVENTWORK * ev, u16 start_no, u16 end_no );

extern u16 * EVENTWORK_GetEventWorkAdrs( EVENTWORK * ev, u16 work_no );
extern void EVENTWORK_ClearEventWorks( EVENTWORK * ev, u16 start_no, u16 end_no );

//----------------------------------------------------------
//    ���ԃt���O�̃N���A
//----------------------------------------------------------
extern void EVENTWORK_ClearTimeFlags( EVENTWORK * ev );

//----------------------------------------------------------
//    ���ԍX�V�������N�G�X�g�̎��o��
//    ������A�B���A�C�e�������p
//----------------------------------------------------------
extern BOOL EVENTWORK_PopTimeRequest( EVENTWORK * ev );

//----------------------------------------------------------
//	�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
//extern EVENTWORK * SaveData_GetEventWork(SAVEDATA * sv);


