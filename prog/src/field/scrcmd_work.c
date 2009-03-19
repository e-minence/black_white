//======================================================================
/**
 * @file	scrcmd_work.c
 * @brief	�X�N���v�g�R�}���h�p���[�N
 * @author	kagaya
 * @data	05.07.13
 */
//======================================================================
#include "scrcmd_work.h"
#include "arc_def.h"

//======================================================================
//	define
//======================================================================
#define SCRCMD_ACMD_MAX (8)

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	SCRCMD_WORK
//--------------------------------------------------------------
struct _TAG_SCRCMD_WORK
{
	HEAPID heapID;
	SCRCMD_WORK_HEADER head;
	
	GFL_MSGDATA *msgData;
	FLDMSGWIN *msgWin;
	
	GFL_TCB *tcb_anm_tbl[SCRCMD_ACMD_MAX];
};

//======================================================================
//	proto
//======================================================================

//======================================================================
//	SCRCMD_WORK �������A�폜
//======================================================================
//--------------------------------------------------------------
//
//--------------------------------------------------------------
void SCRCMD_WORK_SetFldMMdlAnmTCB( SCRCMD_WORK *work, GFL_TCB *tcb )
{
	int i;
	for( i = 0; i < SCRCMD_ACMD_MAX; i++ ){
		if( work->tcb_anm_tbl[i] == NULL ){
			work->tcb_anm_tbl[i] = tcb;
			return;
		}
	}
	GF_ASSERT( 0 );
}

BOOL SCRCMD_WORK_CheckFldMMdlAnmTCB( SCRCMD_WORK *work )
{
	BOOL flag = FALSE;
	int i;
	for( i = 0; i < SCRCMD_ACMD_MAX; i++ ){
		if( work->tcb_anm_tbl[i] != NULL ){
			if( FLDMMDL_CheckEndAcmdList(work->tcb_anm_tbl[i]) == TRUE ){
				FLDMMDL_EndAcmdList( work->tcb_anm_tbl[i] );
				work->tcb_anm_tbl[i] = NULL;
			}else{
				flag = TRUE;
			}
		}
	}
	return( flag );
}

//--------------------------------------------------------------
/**
 * SCRCMD_WORK �쐬
 * @param	head	SCRCMD_WORK_HEADER
 * @param	heapID	HEAPID
 * @retval	SCRCMD_WORK
 */
//--------------------------------------------------------------
SCRCMD_WORK * SCRCMD_WORK_Create(
	const SCRCMD_WORK_HEADER *head, HEAPID heapID )
{
	SCRCMD_WORK *work;
	work = GFL_HEAP_AllocClearMemoryLo( heapID, sizeof(SCRCMD_WORK) );
	work->heapID = heapID;
	work->head = *head;
	return( work );
}

//--------------------------------------------------------------
/**
 * SCRCMD_WORK �폜
 * @param	work	SCRCMD_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void SCRCMD_WORK_Delete( SCRCMD_WORK *work )
{
	if( work->msgData != NULL ){
		GFL_MSG_Delete( work->msgData );
	}
	
	GFL_HEAP_FreeMemory( work );
}

//======================================================================
//	SCRCMD_WORK �Q�ƁA�擾
//======================================================================
//--------------------------------------------------------------
/**
 * SCRCMD_WORK HEAPID�擾
 * @param	work SCRCMD_WORK
 * @retval	GAMESYS_WORK
 */
//--------------------------------------------------------------
HEAPID SCRCMD_WORK_GetHeapID( SCRCMD_WORK *work )
{
	return( work->heapID );
}

//--------------------------------------------------------------
/**
 * SCRCMD_WORK GAMESYS_WORK�擾
 * @param	work	SCRCMD_WORK
 * @retval	GAMESYS_WORK
 */
//--------------------------------------------------------------
GAMESYS_WORK * SCRCMD_WORK_GetGameSysWork( SCRCMD_WORK *work )
{
	return( work->head.gsys );
}

//--------------------------------------------------------------
/**
 * SCRCMD_WORK GAMEDATA�擾
 * @param	work	SCRCMD_WORK
 * @retval	GAMESYSDATA
 */
//--------------------------------------------------------------
GAMEDATA * SCRCMD_WORK_GetGameData( SCRCMD_WORK *work )
{
	return( work->head.gdata );
}

//--------------------------------------------------------------
/**
 * SCRCMD_WORK FLDMMDLSYS�擾
 * @param	work	SCRCMD_WORK
 * @retval	FLDMMDLSYS
 */
//--------------------------------------------------------------
FLDMMDLSYS * SCRCMD_WORK_GetFldMMdlSys( SCRCMD_WORK *work )
{
	return( work->head.fldmmdlsys );
}

//--------------------------------------------------------------
/**
 * SCRCMD_WORK SCRIPT�擾
 * @param	work	SCRCMD_WORK
 * @retval	SCRIPT
 */
//--------------------------------------------------------------
SCRIPT_WORK * SCRCMD_WORK_GetScriptWork( SCRCMD_WORK *work )
{
	return( work->head.script );
}

//--------------------------------------------------------------
/**
 * SCRCMD_WORK FLDMSGBG�擾
 * @param	work	SCRCMD_WORK
 * @retval	FLDMSGBG
 */
//--------------------------------------------------------------
FLDMSGBG * SCRCMD_WORK_GetFldMsgBG( SCRCMD_WORK *work )
{
	return( work->head.fldMsgBG );
}

//--------------------------------------------------------------
/**
 * SCRCMD_WORK GFL_MSGDATA�Z�b�g
 * @param	work SCRCMD_WORK
 * @retval	GFL_MSGDATA*
 */
//--------------------------------------------------------------
void SCRCMD_WORK_SetMsgData( SCRCMD_WORK *work, GFL_MSGDATA *msgData )
{
	work->msgData = msgData;
}

//--------------------------------------------------------------
/**
 * SCRCMD_WORK GFL_MSGDATA�擾
 * @param	work SCRCMD_WORK
 * @retval	GFL_MSGDATA*
 */
//--------------------------------------------------------------
GFL_MSGDATA * SCRCMD_WORK_GetMsgData( SCRCMD_WORK *work )
{
	return( work->msgData );
}

//--------------------------------------------------------------
/**
 * SCRCMD_WORK FLDMSGWIN�Z�b�g
 * @param	work SCRCMD_WORK
 * @param	msgWin FLDMSGWIN*
 * @retval	nothing
 */
//--------------------------------------------------------------
void SCRCMD_WORK_SetFldMsgWin( SCRCMD_WORK *work, FLDMSGWIN *msgWin )
{
	work->msgWin = msgWin;
}

//--------------------------------------------------------------
/**
 * SCRCMD_WORK GFL_MSGDATA�擾
 * @param	work SCRCMD_WORK
 * @retval	FLDMSGWIN*
 */
//--------------------------------------------------------------
FLDMSGWIN * SCRCMD_WORK_GetFldMsgWin( SCRCMD_WORK *work )
{
	return( work->msgWin );
}

//======================================================================
//	SCRCMD_WORK ���̑�
//======================================================================
//--------------------------------------------------------------
/**
 * SCRCMD_WORK MSGDATA������
 * @param	work	SCRCMD_WORK
 * @param	datID	�A�[�J�C�u�f�[�^�C���f�b�N�X
 * @retval	nothing
 */
//--------------------------------------------------------------
void SCRCMD_WORK_CreateMsgData( SCRCMD_WORK *work, u32 datID )
{
	GFL_MSGDATA *msgData = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, datID, work->heapID );
	SCRCMD_WORK_SetMsgData( work, msgData );
}
