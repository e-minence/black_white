//============================================================================================
/**
 * @file	eventflag.h
 * @brief	�Z�[�u�t���O�A���[�N����
 * @author	Tamada
 * @author	Satoshi Nohara
 * @date	05.10.22
 */
//============================================================================================
#ifndef	__EVENTFLAG_H__
#define	__EVENTFLAG_H__

#include <gflib.h>
//#include "system/savedata_def.h"


//=============================================================================
//=============================================================================
//----------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�^��`
 */
//----------------------------------------------------------
typedef struct _EVENTWORK EVENTWORK;

//=============================================================================
//=============================================================================
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
extern u8 * EVENTWORK_GetEventFlagAdrs( EVENTWORK * ev, u16 flag_no );
extern u16 * EVENTWORK_GetEventWorkAdrs( EVENTWORK * ev, u16 work_no );

//----------------------------------------------------------
//	�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
//extern EVENTWORK * SaveData_GetEventWork(SAVEDATA * sv);


#endif	//__EVENTFLAG_H__
