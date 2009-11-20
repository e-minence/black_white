//======================================================================
/**
 * @file	scrcmd_work.c
 * @brief	�X�N���v�g�R�}���h�p���[�N
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "fieldmap.h"
#include "scrcmd_work.h"
#include "arc_def.h"

#include "script_message.naix"

//======================================================================
//	define
//======================================================================
#define SCRCMD_ACMD_MAX (8)

#define SCRCMD_MENU_LIST_MAX (16)
#define EV_WIN_B_CANCEL (0xfffe)

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
/// SCRCMD_MENU_WORK ���j���[�֘A�p���[�N
//--------------------------------------------------------------
typedef struct
{
  u16 x;
  u16 y;
  u16 cursor;
  u16 cancel;
  u16 *ret;
  
  WORDSET *wordset;

  BOOL free_msg;
  GFL_MSGDATA *msgData;
  
  FLDMENUFUNC_LISTDATA *listData;
	FLDMENUFUNC *menuFunc;
}SCRCMD_MENU_WORK;

//--------------------------------------------------------------
///	SCRCMD_WORK
//--------------------------------------------------------------
struct _TAG_SCRCMD_WORK
{
	HEAPID heapID;
  HEAPID temp_heapID;
	SCRCMD_WORK_HEADER head;
	
	GFL_MSGDATA *msgData;
	VecFx32 talkMsgWinTailPos;
  
  void *msgWin; //���j���[�n���p
  
  GFL_TCB *tcb_callproc;
	GFL_TCB *tcb_anm_tbl[SCRCMD_ACMD_MAX];
  
  SCRCMD_MENU_WORK menuWork;
};

//======================================================================
//	proto
//======================================================================

//======================================================================
//	SCRCMD_WORK �������A�폜
//======================================================================
//--------------------------------------------------------------
/**
 * SCRCMD_WORK �쐬
 * @param	head	SCRCMD_WORK_HEADER
 * @param	heapID	HEAPID
 * @retval	SCRCMD_WORK
 */
//--------------------------------------------------------------
SCRCMD_WORK * SCRCMD_WORK_Create(
	const SCRCMD_WORK_HEADER *head, HEAPID heapID, HEAPID temp_heapID )
{
	SCRCMD_WORK *work;
	work = GFL_HEAP_AllocClearMemoryLo( heapID, sizeof(SCRCMD_WORK) );
	work->heapID = heapID;
  work->temp_heapID = temp_heapID;
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

#if 0
//--------------------------------------------------------------
/**
 * SCRCMD_WORK �e���|�����pHEAPID�擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
HEAPID SCRCMD_WORK_GetTempHeapID( SCRCMD_WORK *work )
{
  return( work->temp_heapID );
}
#endif

//--------------------------------------------------------------
//--------------------------------------------------------------
u16 SCRCMD_WORK_GetZoneID( SCRCMD_WORK *work )
{
  return( work->head.zone_id );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
u16 SCRCMD_WORK_GetScriptID( SCRCMD_WORK *work )
{
  return( work->head.script_id );
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
 * SCRCMD_WORK MMDLSYS�擾
 * @param	work	SCRCMD_WORK
 * @retval	MMDLSYS
 */
//--------------------------------------------------------------
MMDLSYS * SCRCMD_WORK_GetMMdlSys( SCRCMD_WORK *work )
{
	return( work->head.mmdlsys );
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
 * SCRCMD_WORK ���b�Z�[�W�E�B���h�E�|�C���^�Z�b�g
 * @param	work SCRCMD_WORK
 * @param	msgWin ���b�Z�[�W�E�B���h�E�|�C���^
 * @retval	nothing
 */
//--------------------------------------------------------------
void SCRCMD_WORK_SetMsgWinPtr( SCRCMD_WORK *work, void *msgWin )
{
	work->msgWin = msgWin;
}

//--------------------------------------------------------------
/**
 * SCRCMD_WORK FLDMSGWIN_STREAM�擾
 * @param	work SCRCMD_WORK
 * @retval void* ���b�Z�[�W�E�B���h�E�|�C���^
 */
//--------------------------------------------------------------
void * SCRCMD_WORK_GetMsgWinPtr( SCRCMD_WORK *work )
{
	return( work->msgWin );
}

//--------------------------------------------------------------
/**
 * SCRCMD_WORK �����o���E�B���h�E�p���W�Z�b�g
 * @param work SCRCMD_WORK
 * @param pos �Z�b�g������W
 * @retval nothing
 */
//--------------------------------------------------------------
void SCRCMD_WORK_SetTalkMsgWinTailPos(
    SCRCMD_WORK *work, const VecFx32 *pos )
{
  work->talkMsgWinTailPos = *pos;
}

//--------------------------------------------------------------
/**
 * SCRCMD_WORK �����o���E�B���h�E�p���W�|�C���^�擾
 * @param work SCRCMD_WORK
 * @retval VecFx32*
 */
//--------------------------------------------------------------
const VecFx32 * SCRCMD_WORK_GetTalkMsgWinTailPos( SCRCMD_WORK *work )
{
  return( &work->talkMsgWinTailPos );
}

//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 *
 */
//--------------------------------------------------------------
void SCRCMD_WORK_SetCallProcTCB( SCRCMD_WORK *work, GFL_TCB *tcb )
{
  work->tcb_callproc = tcb;
}

//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 *
 */
//--------------------------------------------------------------
GFL_TCB * SCRCMD_WORK_GetCallProcTCB( SCRCMD_WORK *work )
{
  return( work->tcb_callproc );
}

//======================================================================
//  ���샂�f���A�j���[�V����  
//======================================================================
//--------------------------------------------------------------
/**
 * ���샂�f���A�j���pTCB���Z�b�g
 * @param work SCRCMD_WORK
 * @param tcb �A�j���[�V�����R�}���h�pTCB
 * @retval nothing
 */
//--------------------------------------------------------------
void SCRCMD_WORK_SetMMdlAnmTCB( SCRCMD_WORK *work, GFL_TCB *tcb )
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

//--------------------------------------------------------------
/**
 * ���샂�f���A�j���pTCB����`�F�b�N
 * @param work SCRCMD_WORK
 * @retval BOOL TRUE=���쒆�BFALSE=����I��
 */
//--------------------------------------------------------------
BOOL SCRCMD_WORK_CheckMMdlAnmTCB( SCRCMD_WORK *work )
{
	BOOL flag = FALSE;
	int i;
	for( i = 0; i < SCRCMD_ACMD_MAX; i++ ){
		if( work->tcb_anm_tbl[i] != NULL ){
			if( MMDL_CheckEndAcmdList(work->tcb_anm_tbl[i]) == TRUE ){
				MMDL_EndAcmdList( work->tcb_anm_tbl[i] );
				work->tcb_anm_tbl[i] = NULL;
			}else{
				flag = TRUE;
			}
		}
	}
	return( flag );
}

//======================================================================
//	SCRCMD_WORK ���b�Z�[�W�f�[�^
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
#if 1
	GFL_MSGDATA *msgData = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE, datID, work->heapID );
#else //�e���|�����pHEAPID�ɕύX�B
	GFL_MSGDATA *msgData = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE, datID, work->temp_heapID );
#endif
	SCRCMD_WORK_SetMsgData( work, msgData );
}

//======================================================================
//  SCRCMD_WORK ���j���[�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * SCRCMD_WORK ���j���[�@�֘A�p�̃��[�N������
 * @param 
 * @retval
 */
//--------------------------------------------------------------
void SCRCMD_WORK_InitMenuWork( SCRCMD_WORK *work,
  u16 x, u16 y, u16 cursor, u16 cancel, u16 *ret,
  WORDSET *wordset, GFL_MSGDATA *msgData )
{
  SCRCMD_MENU_WORK *menuWork = &work->menuWork;
	MI_CpuClear8( menuWork, sizeof(SCRCMD_MENU_WORK) );
  
  menuWork->x = x;
  menuWork->y = y;
  menuWork->cursor = cursor;
  menuWork->cancel = cancel;
  menuWork->ret = ret;
  menuWork->wordset = wordset;
  menuWork->msgData = msgData;
  
  if( menuWork->msgData == NULL ){ //���j���[�p���ʃ��b�Z�[�W
    menuWork->free_msg = TRUE;
    menuWork->msgData = GFL_MSG_Create(
        GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE,
        NARC_script_message_ev_win_dat, work->heapID );
  }
  
  menuWork->listData = FLDMENUFUNC_CreateListData(
      SCRCMD_MENU_LIST_MAX, work->heapID );
}

//--------------------------------------------------------------
/**
 * SCRCMD_WORK ���j���[�@���X�g�ǉ�
 * @param
 * @retval
 */
//--------------------------------------------------------------
void SCRCMD_WORK_AddMenuList(
    SCRCMD_WORK *work, u32 msg_id, u32 param,
    STRBUF *msgbuf, STRBUF *tmpbuf )
{
  SCRCMD_MENU_WORK *menuWork = &work->menuWork;
  GFL_MSG_GetString( menuWork->msgData, msg_id, tmpbuf );
  WORDSET_ExpandStr( menuWork->wordset, msgbuf, tmpbuf );
  FLDMENUFUNC_AddStringListData(
      menuWork->listData, msgbuf, param, work->heapID );
}

//--------------------------------------------------------------
//  ���j���[�w�b�_�[
//--------------------------------------------------------------
static const FLDMENUFUNC_HEADER data_MenuHeader =
{
	1,		//���X�g���ڐ�
	6,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	1,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_LRKEY,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	12,		//�����T�C�YY(�h�b�g
	0,		//�\�����WX �L�����P��
	0,		//�\�����WY �L�����P��
	0,		//�\���T�C�YX �L�����P��
	0,		//�\���T�C�YY �L�����P��
};

//--------------------------------------------------------------
/**
 * SCRCMD_WORK ���j���[�@�J�n
 * @param
 * @retval
 */
//--------------------------------------------------------------
void SCRCMD_WORK_StartMenu( SCRCMD_WORK *work )
{
  u32 sx,sy,count;
  SCRCMD_MENU_WORK *menuWork = &work->menuWork;
  FLDMENUFUNC_HEADER menuH = data_MenuHeader;

  sx = FLDMENUFUNC_GetListMenuWidth(
      menuWork->listData, menuH.font_size_x, menuH.msg_spc );
  sy = FLDMENUFUNC_GetListMenuHeight(
      menuWork->listData, menuH.font_size_y, menuH.line_spc );
  count = FLDMENUFUNC_GetListMax( menuWork->listData );
  FLDMENUFUNC_InputHeaderListSize(
      &menuH, count, menuWork->x, menuWork->y, sx, sy );

	menuWork->menuFunc = FLDMENUFUNC_AddEventMenuList(
      work->head.fldMsgBG, &menuH, menuWork->listData, 0, 0,
      menuWork->cancel );
}

//--------------------------------------------------------------
/**
 * SCRCMD_WORK ���j���[�@���j���[����
 * @param
 * @retval
 */
//--------------------------------------------------------------
BOOL SCRCMD_WORK_ProcMenu( SCRCMD_WORK *work )
{
  u32 ret;
  SCRCMD_MENU_WORK *menuWork = &work->menuWork;
  
  ret = FLDMENUFUNC_ProcMenu( menuWork->menuFunc );

	if( ret == FLDMENUFUNC_NULL ){	//���얳��
    return( FALSE );
  }
  
  FLDMENUFUNC_DeleteMenu( menuWork->menuFunc );
  
  if( menuWork->free_msg == TRUE ){
    GFL_MSG_Delete( menuWork->msgData );
  }

  if( ret != FLDMENUFUNC_CANCEL ){	//����
    *(menuWork->ret) = ret;
  }else{
#if 0
    if( menuWork->cancel == FALSE ){ //B�L�����Z���L���ł͂Ȃ�
      return( FALSE );
    }
#endif
    *(menuWork->ret) = EV_WIN_B_CANCEL;
  }
  
  return( TRUE );
}

