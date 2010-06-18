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
#include "script_local.h"
#include "script_def.h"

#include "script_message.naix"

//======================================================================
//	define
//======================================================================
#define SCRCMD_ACMD_MAX (8)

#define SCRCMD_MENU_LIST_MAX (32)
#define EV_WIN_B_CANCEL (0xfffe)
#define EV_WIN_X_BREAK  (0xfffd)

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
  u8 cancel;
  u8 lr_just;
  u16 *ret;
  
  WORDSET *wordset;

  BOOL free_msg;
  GFL_MSGDATA *msgData;
  
  FLDMENUFUNC_LISTDATA *listData;
	FLDMENUFUNC *menuFunc;

  STRBUF *exMsgBuf[SCRCMD_MENU_LIST_MAX];
}SCRCMD_MENU_WORK;

//--------------------------------------------------------------
/**
 * @brief �X�N���v�g�R�}���h�p���[�N�i�e���z�}�V�����ʁj
 */
//--------------------------------------------------------------
struct _SCRCMD_GLOBAL_WORK {

	SCRIPT_WORK *script_work;
	GAMESYS_WORK *gsys;
	GAMEDATA *gamedata;
	MMDLSYS *mmdlsys;
	
  GFL_TCB *tcb_callproc;
	GFL_TCB *tcb_anm_tbl[SCRCMD_ACMD_MAX];
  
  SCRCMD_MENU_WORK menuWork;
  SCRCMD_BALLOONWIN_WORK balloonWinWork;
  
  void *msgWin;           ///<���j���[�n���p���b�Z�[�W�E�B���h�E
  void *timeIcon;         ///<���j���[�n���p���b�Z�[�W�E�B���h�E�^�C���A�C�R��

  u8 before_win_pos_type;
  u8 win_pos_type;
  u16 wait_count;          ///<�E�F�C�g�p���[�N�A����I���J�n�܂ł̃E�F�C�g�݂̂Ɏg�p
  
  u32 voice_index;          ///<�|�P���������p�C���f�b�N�X
  void * elevator_adrs;  ///<�G���x�[�^�[�f�[�^�̃A�h���X
};

//--------------------------------------------------------------
/**
 * @brief �X�N���v�g�R�}���h�p���[�N�i�e���z�}�V���ʁj
 */
//--------------------------------------------------------------
struct _TAG_SCRCMD_WORK
{
	HEAPID heapID;
	SCRCMD_WORK_HEADER head;
	
	GFL_MSGDATA *msgData;   ///<MSGDATA�ւ̃|�C���^
  u16 msg_naix_id;        ///<MSGDATA�̃f�[�^ID
  
  u8 wait_vm_id;          ///<��~���ɊĎ����鉼�z�}�V����ID
  u8 dummy;               ///<padding

  void * backup_work;     ///<VM�؂�ւ����̃X�N���v�g���[�N���o�b�N�A�b�v���邽�߂̃|�C���^
  
  SCRCMD_GLOBAL * gwork;
};

//======================================================================
//	proto
//======================================================================
static void mmdlAnimeTCB_Delete( SCRCMD_WORK *work );

static void	BmpMenu_CallbackFunc(BMPMENULIST_WORK * wk,u32 param,u8 mode);

//======================================================================
//	SCRCMD_WORK �������A�폜
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
SCRCMD_GLOBAL * SCRCMD_GLOBAL_Create( SCRIPT_WORK * sc, HEAPID heapID )
{
  SCRCMD_GLOBAL * gwork;
  gwork = GFL_HEAP_AllocClearMemory( heapID, sizeof(SCRCMD_GLOBAL) );
  gwork->script_work = sc;
  gwork->gsys = SCRIPT_GetGameSysWork( gwork->script_work );
  gwork->gamedata = GAMESYSTEM_GetGameData( gwork->gsys );
  gwork->mmdlsys = GAMEDATA_GetMMdlSys( gwork->gamedata );

  gwork->before_win_pos_type = SCRCMD_MSGWIN_MAX;
  return gwork;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void SCRCMD_GLOBAL_Delete( SCRCMD_GLOBAL * gwork )
{
  GFL_HEAP_FreeMemory( gwork );
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
  work->gwork = SCRIPT_GetScrCmdGlobal( head->script_work );
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
  if( work->backup_work != NULL )
  {
    GF_ASSERT( 0 );
    GFL_HEAP_FreeMemory( work->backup_work );
  }

	if( work->msgData != NULL ){
		GFL_MSG_Delete( work->msgData );
	}
	
  mmdlAnimeTCB_Delete( work );
  
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
//--------------------------------------------------------------
BOOL SCRCMD_WORK_GetSpScriptFlag( const SCRCMD_WORK *work )
{
  return ( work->head.sp_flag );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
SCRIPT_TYPE SCRCMD_WORK_GetScriptType( const SCRCMD_WORK *work )
{
  return ( work->head.scr_type );
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
  return work->gwork->gsys;
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
  return work->gwork->gamedata;
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
  return work->gwork->mmdlsys;
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
  return work->gwork->script_work;
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
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM * fld_param = SCRIPT_GetFieldParam( sc );
  return fld_param->msgBG;
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
 * @brief MSGDATA��DATAID���擾����
 * @return  u16 datID( script_message.naix���̃C���f�b�N�X�l�j
 */
//--------------------------------------------------------------
u16 SCRCMD_WORK_GetMsgDataID( const SCRCMD_WORK * work )
{
  return work->msg_naix_id;
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
	work->gwork->msgWin = msgWin;
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
	return work->gwork->msgWin;
}

//----------------------------------------------------------------------------
/**
 *	@brief  SCRCMD_WORK �^�C���A�C�R�����[�N��ݒ�
 *
 *	@param	work      ���[�N
 *	@param	timeIcon  �^�C���A�C�R��
 */
//-----------------------------------------------------------------------------
void SCRCMD_WORK_SetTimeIconPtr( SCRCMD_WORK *work, void *timeIcon )
{
  work->gwork->timeIcon = timeIcon;
}
//----------------------------------------------------------------------------
/**
 *	@brief  SCRCMD_WORK �^�C���A�C�R�����[�N���擾
 *
 *	@param	work  ���[�N
 *
 *	@return �^�C���A�C�R�����[�N
 */
//-----------------------------------------------------------------------------
void * SCRCMD_WORK_GetTimeIconPtr( SCRCMD_WORK *work )
{
  return work->gwork->timeIcon;
}


//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
SCRCMD_BALLOONWIN_WORK * SCRCMD_WORK_GetBalloonWinWork( SCRCMD_WORK *work )
{
  return( &work->gwork->balloonWinWork );
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
  work->gwork->tcb_callproc = tcb;
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
  return( work->gwork->tcb_callproc );
}

//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 *
 */
//--------------------------------------------------------------
u8 SCRCMD_WORK_GetBeforeWindowPosType( const SCRCMD_WORK *work )
{
  return( work->gwork->before_win_pos_type );
}

//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 */
//--------------------------------------------------------------
void SCRCMD_WORK_SetBeforeWindowPosType( SCRCMD_WORK *work, u8 type )
{
  work->gwork->before_win_pos_type = type;
}

//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 *
 */
//--------------------------------------------------------------
u8 SCRCMD_WORK_GetWindowPosType( const SCRCMD_WORK *work )
{
  return( work->gwork->win_pos_type );
}

//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 */
//--------------------------------------------------------------
void SCRCMD_WORK_SetWindowPosType( SCRCMD_WORK *work, u8 type )
{
  work->gwork->win_pos_type = type;
}

//--------------------------------------------------------------
/**
 * @brief �E�F�C�g�l�̎擾
 */
//--------------------------------------------------------------
u16 SCRCMD_WORK_GetWaitCount( const SCRCMD_WORK *work )
{
  return work->gwork->wait_count;
}

//--------------------------------------------------------------
/**
 * @brief �E�F�C�g�l�̐ݒ�
 */
//--------------------------------------------------------------
void SCRCMD_WORK_SetWaitCount( SCRCMD_WORK *work, u16 wait )
{
  work->gwork->wait_count = wait;
}

//--------------------------------------------------------------
/**
 * @brief �E�F�C�g�l�̃J�E���g�_�E��
 */
//--------------------------------------------------------------
BOOL SCRCMD_WORK_WaitCountDown( SCRCMD_WORK * work )
{
  if ( work->gwork->wait_count == 0 )
  {
    return TRUE;
  }
  -- ( work->gwork->wait_count );
  return FALSE;
}
//--------------------------------------------------------------
/**
 * @brief �|�P���������R���g���[���[�C���f�b�N�X�̎擾
 */
//--------------------------------------------------------------
u32 SCRCMD_WORK_GetPMVoiceIndex( const SCRCMD_WORK * work )
{
  return work->gwork->voice_index;
}

//--------------------------------------------------------------
/**
 * @brief �|�P���������R���g���[���[�C���f�b�N�X�̃Z�b�g
 */
//--------------------------------------------------------------
void SCRCMD_WORK_SetPMVoiceIndex( SCRCMD_WORK * work, u32 voice_index )
{
  work->gwork->voice_index = voice_index;
}
//--------------------------------------------------------------
/**
 * @brief  �G���x�[�^�[�f�[�^�̃A�h���X���擾
 */
//--------------------------------------------------------------
void * SCRCMD_WORK_GetElevatorDataAddress( const SCRCMD_WORK * work )
{
  return work->gwork->elevator_adrs;
}
//--------------------------------------------------------------
/**
 * @brief �G���x�[�^�[�f�[�^�̃A�h���X���Z�b�g
 */
//--------------------------------------------------------------
void SCRCMD_WORK_SetElevatorDataAddress( SCRCMD_WORK * work, void * adrs )
{
  work->gwork->elevator_adrs = adrs;
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
		if( work->gwork->tcb_anm_tbl[i] == NULL ){
			work->gwork->tcb_anm_tbl[i] = tcb;
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
		if( work->gwork->tcb_anm_tbl[i] != NULL ){
			if( MMDL_CheckEndAcmdList(work->gwork->tcb_anm_tbl[i]) == TRUE ){
				MMDL_EndAcmdList( work->gwork->tcb_anm_tbl[i] );
				work->gwork->tcb_anm_tbl[i] = NULL;
			}else{
				flag = TRUE;
			}
		}
	}
	return( flag );
}

//--------------------------------------------------------------
/**
 * ���샂�f���A�j���pTCB����`�F�b�N
 * @param work SCRCMD_WORK
 * @retval BOOL TRUE=���쒆�BFALSE=����I��
 */
//--------------------------------------------------------------
static void mmdlAnimeTCB_Delete( SCRCMD_WORK *work )
{
	int i;
  
	for( i = 0; i < SCRCMD_ACMD_MAX; i++ ){
		if( work->gwork->tcb_anm_tbl[i] != NULL ){
      GF_ASSERT( 0 && "SCRCMD FORGET OBJ ANIME WAIT" );
      MMDL_EndAcmdList( work->gwork->tcb_anm_tbl[i] );
      work->gwork->tcb_anm_tbl[i] = NULL;
		}
	}
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
void SCRCMD_WORK_CreateMsgData( SCRCMD_WORK *work, u32 arcID, u32 datID )
{
	GFL_MSGDATA *msgData = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, arcID, datID, work->heapID );

  work->msgData = msgData;
  work->msg_naix_id = datID;
}

//======================================================================
//  SCRCMD_WORK ���j���[�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * SCRCMD_WORK ���j���[�@�֘A�p�̃��[�N������
 * @param work  SCRCMD_WORK*
 * @param x X�\�����W�@�L�����P��
 * @param y Y�\�����W�@�L�����P��
 * @param cursor �J�[�\�������ʒu
 * @param cancel B�L�����Z���L���A���� TRUE=�L��
 * @param lr_just ���E�l�ߎ���
 * @param ret ���ʊi�[��
 * @param wordset ������\���Ɏg�p����WORDSET*
 * @param msgData ������\���Ɏg�p����GFL_MSGDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void SCRCMD_WORK_InitMenuWork( SCRCMD_WORK *work,
  u16 x, u16 y, u16 cursor, u16 cancel, SCR_MENU_JUSTIFY lr_just, u16 *ret,
  WORDSET *wordset, GFL_MSGDATA *msgData )
{
  SCRCMD_MENU_WORK *menuWork = &work->gwork->menuWork;
	MI_CpuClear8( menuWork, sizeof(SCRCMD_MENU_WORK) );
  
  menuWork->x = x;
  menuWork->y = y;
  menuWork->cursor = cursor;
  menuWork->cancel = cancel;
  menuWork->lr_just = lr_just;
  menuWork->ret = ret;
  menuWork->wordset = wordset;
  menuWork->msgData = msgData;
 
  MI_CpuClear32( menuWork->exMsgBuf, 4*SCRCMD_MENU_LIST_MAX );

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
    SCRCMD_WORK *work, u32 msg_id, u32 ex_msg_id, u32 param,
    STRBUF *msgbuf, STRBUF *tmpbuf )
{
  SCRCMD_MENU_WORK *menuWork = &work->gwork->menuWork;
  
  if(ex_msg_id != SCR_BMPMENU_EXMSG_NULL){
    u32  num = BmpMenuWork_GetListMax( (BMP_MENULIST_DATA*)menuWork->listData );
    if( num >= SCRCMD_MENU_LIST_MAX ){
      GF_ASSERT(0);
      return;
    }
    GFL_MSG_GetString( menuWork->msgData, ex_msg_id, tmpbuf );
    WORDSET_ExpandStr( menuWork->wordset, msgbuf, tmpbuf );
    menuWork->exMsgBuf[num] = GFL_STR_CreateCopyBuffer( msgbuf, work->heapID );
  }
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
 * SCRCMD_WORK ���j���[�@�J�n�@
 * @param work SCRCMD_WORK*
 * @param r_just TRUE=�E�l�ō��W�ݒ�
 * @retval nothing
 */
//--------------------------------------------------------------
void SCRCMD_WORK_StartMenu( SCRCMD_WORK *work )
{
  u32 sx,sy,count;
  SCRCMD_MENU_WORK *menuWork = &work->gwork->menuWork;
  FLDMENUFUNC_HEADER menuH = data_MenuHeader;
  FLDMSGBG *fmb = SCRCMD_WORK_GetFldMsgBG( work );
  
  sx = FLDMENUFUNC_GetListMenuWidth( fmb,
      menuWork->listData, menuH.font_size_x, menuH.msg_spc );
  
  if( menuWork->lr_just == SCR_MENU_JUST_R ){ //�E�l
    menuWork->x -= sx; //���W�␳
  }
  
#if 0 //old
  sy = FLDMENUFUNC_GetListMenuHeight(
      menuWork->listData, menuH.font_size_y, menuH.line_spc );
#else
  //���ڐ����\���ő吔�𒴂��Ă��邩�`�F�b�N���A�E�B���h�E�̏c�������肷��
  {
    u32 len;
    u32 max = FLDMENUFUNC_GetListMax( menuWork->listData );
    
    if( max > data_MenuHeader.line ){
      len = data_MenuHeader.line;
    }else{
      len = max;
    }
    
    sy = FLDMENUFUNC_GetListMenuLen(
        len, menuH.font_size_y, menuH.line_spc );
    
    if( max > data_MenuHeader.line ){ //1�L�������₵�����𔼕���������
      sy++;
    }
  }
#endif
  
  count = FLDMENUFUNC_GetListMax( menuWork->listData );
  FLDMENUFUNC_InputHeaderListSize(
      &menuH, count, menuWork->x, menuWork->y, sx, sy );
  
  {
	  menuWork->menuFunc = FLDMENUFUNC_AddEventMenuList(
      fmb, &menuH, menuWork->listData,
      BmpMenu_CallbackFunc, work,
      0, menuWork->cursor, menuWork->cancel );
  }
}

//--------------------------------------------------------------
/**
 * @brief ���j���[�@����
 * @param
 */
//--------------------------------------------------------------
static void CleanUpMenuWork( SCRCMD_MENU_WORK* menuWork )
{
  FLDMENUFUNC_DeleteMenu( menuWork->menuFunc );
  
  if( menuWork->free_msg == TRUE ){
    GFL_MSG_Delete( menuWork->msgData );
  }
  {
    int i;

    for(i = 0;i < SCRCMD_MENU_LIST_MAX;i++){
      if( menuWork->exMsgBuf[i] != NULL ){
        GFL_STR_DeleteBuffer( menuWork->exMsgBuf[i] );
        menuWork->exMsgBuf[i] = NULL;
      }
    }
  }
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
  SCRCMD_MENU_WORK *menuWork = &work->gwork->menuWork;
  
  ret = FLDMENUFUNC_ProcMenu( menuWork->menuFunc );

	if( ret == FLDMENUFUNC_NULL ){	//���얳��
    return( FALSE );
  }

  CleanUpMenuWork( menuWork );
  
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

//--------------------------------------------------------------
/**
 * SCRCMD_WORK ���j���[�@���j���[����(x�{�^���ɂ�钆�f����)
 * @param
 * @retval
 */
//--------------------------------------------------------------
BOOL SCRCMD_WORK_ProcMenu_Breakable( SCRCMD_WORK *work )
{
  u32 ret;
  SCRCMD_MENU_WORK *menuWork = &work->gwork->menuWork;

  // �ʏ�̃��j���[����
  ret = SCRCMD_WORK_ProcMenu( work );

  // �I������Ȃ������ꍇ ==> x�{�^������
  if( ret == FALSE )
  {
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
    {
      CleanUpMenuWork( menuWork );
      *(menuWork->ret) = EV_WIN_X_BREAK;
      ret = TRUE;
    }
  }
  return ret;
}

//--------------------------------------------------------------
/*
 *  @brief  ���j���[�R�[���o�b�N
 */
//--------------------------------------------------------------
static void	BmpMenu_CallbackFunc(BMPMENULIST_WORK * wk,u32 param,u8 mode)
{
  u16 cursor = 0;
  SCRCMD_WORK* work = (SCRCMD_WORK*)BmpMenuList_GetWorkPtr(wk);
  SCRCMD_MENU_WORK* menu = &work->gwork->menuWork;

  BmpMenuList_DirectPosGet( wk, &cursor );

  if( menu->exMsgBuf[ cursor ] == NULL ){
    return;
  }
  if( SCREND_CHK_CheckBit(SCREND_CHK_WIN_OPEN) ) //�V�X�e���E�B���h�E
  {
    FLDSYSWIN_STREAM *sysWin;
    sysWin = SCRCMD_WORK_GetMsgWinPtr( work );
    FLDSYSWIN_STREAM_ClearMessage( sysWin );
    FLDSYSWIN_STREAM_AllPrintStrBuf( sysWin, 0, 0, menu->exMsgBuf[ cursor ] );
  }
  else if( SCREND_CHK_CheckBit(SCREND_CHK_BALLON_WIN_OPEN) ) //�o���[���E�B���h�E
  {
  }
  else if( SCREND_CHK_CheckBit(SCREND_CHK_PLAINWIN_OPEN) ) //�v���[���E�B���h�E
  {
  
  }else{
    GF_ASSERT(0);
  }
}


//======================================================================
//
//  �X�N���v�g���[�N�֘A����
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �ޔ�p���[�N:�ޔ�����
 *
 * �X�N���v�g�t�@�C�����[�J���ȕϐ����o�b�N�A�b�v����
 */
//--------------------------------------------------------------
void SCRCMD_WORK_BackupUserWork( SCRCMD_WORK *work )
{
  GF_ASSERT( work->backup_work == NULL );
  work->backup_work = SCRIPT_BackupUserWork( SCRCMD_WORK_GetScriptWork( work ) );
}

//--------------------------------------------------------------
/**
 * @brief �ޔ�p���[�N�F���A����
 *
 * �X�N���v�g�t�@�C�����[�J���ȕϐ��𕜋A����
 */
//--------------------------------------------------------------
void SCRCMD_WORK_RestoreUserWork( SCRCMD_WORK *work )
{
  void * backup_work = work->backup_work;
  GF_ASSERT( backup_work != NULL );
  SCRIPT_RestoreUserWork( SCRCMD_WORK_GetScriptWork( work ), work->backup_work );
  work->backup_work = NULL;
}

//--------------------------------------------------------------
/**
 * @brief �Ď��Ώ�VM��ID���Z�b�g
 */
//--------------------------------------------------------------
void SCRCMD_WORK_SetWaitVMID( SCRCMD_WORK *work, VMHANDLE_ID vm_id )
{
  work->wait_vm_id = vm_id;
}

//--------------------------------------------------------------
/**
 * @brief �Ď��Ώ�VM��ID���擾
 */
//--------------------------------------------------------------
VMHANDLE_ID SCRCMD_WORK_GetWaitVMID( const SCRCMD_WORK *work )
{
  return work->wait_vm_id;
}

