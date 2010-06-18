//======================================================================
/**
 * @file	scrcmd_work.h
 * @brief	�X�N���v�g�R�}���h�p���[�N
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"
#include "gamesystem/gamesystem.h"

#include "msgdata.h"
#include "print/wordset.h"

#include "field/fldmmdl.h"
#include "field/script.h"
#include "field/field_msgbg.h"
#include "field/fieldmap_proc.h"

#include "script_local.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
/// SCR_MENU_JUSTIFY ���j���[���E�l�ߎ���
//--------------------------------------------------------------
typedef enum
{
  SCR_MENU_JUST_L = 0, ///<���l
  SCR_MENU_JUST_R, ///<�E�l
  SCR_MENU_JUST_MAX, ///<�ő�
}SCR_MENU_JUSTIFY;

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	SCRCMD_WORK
//--------------------------------------------------------------
typedef struct _TAG_SCRCMD_WORK SCRCMD_WORK;

typedef struct _SCRCMD_GLOBAL_WORK SCRCMD_GLOBAL;

//--------------------------------------------------------------
///	SCRCMD_WORK_HEADER �X�N���v�g�R�}���h�ŎQ�Ƃ��郏�[�N���W�߂��w�b�_�[
//--------------------------------------------------------------
typedef struct
{
  u16 zone_id;
  u16 script_id;
  SCRIPT_TYPE scr_type; ///<�X�N���v�g�̎��
  BOOL sp_flag;   ///<����X�N���v�g���ǂ����̔��ʃw�b�_
  
	SCRIPT_WORK *script_work;
	
}SCRCMD_WORK_HEADER;

//--------------------------------------------------------------
/// SCRCMD_WORK_BALLOONWIN�@�o���[���E�B���h�E�֘A���[�N
//--------------------------------------------------------------
typedef struct
{
  u16 obj_id;
  u16 win_idx;
  VecFx32 tail_pos;
  VecFx32 tail_pos_org;
  VecFx32 tail_offs;
}SCRCMD_BALLOONWIN_WORK;

typedef struct
{
  GAMESYS_WORK *gsys;
  SCRCMD_WORK   *ScrCmdWk;
  SCRIPT_WORK *ScrWk;
}SCREND_CHECK;

//======================================================================
//	proto
//======================================================================
//�X�N���v�g���[�N�i���ʕ����j������
extern SCRCMD_GLOBAL * SCRCMD_GLOBAL_Create( SCRIPT_WORK * sc, HEAPID heapID );
//�X�N���v�g���[�N�i���ʕ����j�폜
extern void SCRCMD_GLOBAL_Delete( SCRCMD_GLOBAL * gwork );

//�X�N���v�g���[�N�iVM�ʕ����j������
extern SCRCMD_WORK * SCRCMD_WORK_Create( const SCRCMD_WORK_HEADER *head, HEAPID heapID );
//�X�N���v�g���[�N�iVM�ʕ����j�폜
extern void SCRCMD_WORK_Delete( SCRCMD_WORK *work );

//�Q�ƁA�擾
extern HEAPID SCRCMD_WORK_GetHeapID( SCRCMD_WORK *work );

extern u16 SCRCMD_WORK_GetZoneID( SCRCMD_WORK *work );
extern u16 SCRCMD_WORK_GetScriptID( SCRCMD_WORK *work );
extern SCRIPT_TYPE SCRCMD_WORK_GetScriptType( const SCRCMD_WORK *work );
extern BOOL SCRCMD_WORK_GetSpScriptFlag( const SCRCMD_WORK *work );
extern GAMESYS_WORK * SCRCMD_WORK_GetGameSysWork( SCRCMD_WORK *work );
extern GAMEDATA *SCRCMD_WORK_GetGameData( SCRCMD_WORK *work );
extern MMDLSYS * SCRCMD_WORK_GetMMdlSys( SCRCMD_WORK *work );
extern SCRIPT_WORK * SCRCMD_WORK_GetScriptWork( SCRCMD_WORK *work );
extern FLDMSGBG * SCRCMD_WORK_GetFldMsgBG( SCRCMD_WORK *work );

extern void SCRCMD_WORK_SetMsgData(SCRCMD_WORK *work, GFL_MSGDATA *msgData);
extern GFL_MSGDATA * SCRCMD_WORK_GetMsgData( SCRCMD_WORK *work );
extern u16 SCRCMD_WORK_GetMsgDataID( const SCRCMD_WORK * work );

extern void SCRCMD_WORK_SetMsgWinPtr( SCRCMD_WORK *work, void *msgWin );
extern void * SCRCMD_WORK_GetMsgWinPtr( SCRCMD_WORK *work );
extern void SCRCMD_WORK_SetTimeIconPtr( SCRCMD_WORK *work, void *timeIcon );
extern void * SCRCMD_WORK_GetTimeIconPtr( SCRCMD_WORK *work );

extern SCRCMD_BALLOONWIN_WORK * SCRCMD_WORK_GetBalloonWinWork(
    SCRCMD_WORK *work );

extern void SCRCMD_WORK_SetCallProcTCB( SCRCMD_WORK *work, GFL_TCB *tcb );
extern GFL_TCB * SCRCMD_WORK_GetCallProcTCB( SCRCMD_WORK *work );
extern u8 SCRCMD_WORK_GetBeforeWindowPosType( const SCRCMD_WORK *work );
extern void SCRCMD_WORK_SetBeforeWindowPosType( SCRCMD_WORK *work, u8 type );
extern u8 SCRCMD_WORK_GetWindowPosType( const SCRCMD_WORK *work );
extern void SCRCMD_WORK_SetWindowPosType( SCRCMD_WORK *work, u8 type );
//--------------------------------------------------------------
// �E�F�C�g�l�̑���
//--------------------------------------------------------------
extern u16 SCRCMD_WORK_GetWaitCount( const SCRCMD_WORK *work );
extern void SCRCMD_WORK_SetWaitCount( SCRCMD_WORK *work, u16 wait );
extern BOOL SCRCMD_WORK_WaitCountDown( SCRCMD_WORK * work );

//--------------------------------------------------------------
// �|�P��������
//--------------------------------------------------------------
extern u32 SCRCMD_WORK_GetPMVoiceIndex( const SCRCMD_WORK * work );
extern void SCRCMD_WORK_SetPMVoiceIndex( SCRCMD_WORK * work, u32 voice_index );

//--------------------------------------------------------------
//  �G���x�[�^�[�f�[�^�̃A�h���X���擾
//--------------------------------------------------------------
extern void SCRCMD_WORK_SetElevatorDataAddress( SCRCMD_WORK * work, void * adrs );
extern void * SCRCMD_WORK_GetElevatorDataAddress( const SCRCMD_WORK * work );

//�A�j���[�V�����R�}���h
extern void SCRCMD_WORK_SetMMdlAnmTCB( SCRCMD_WORK *work, GFL_TCB *tcb );
extern BOOL SCRCMD_WORK_CheckMMdlAnmTCB( SCRCMD_WORK *work );

//���b�Z�[�W�f�[�^
extern void SCRCMD_WORK_CreateMsgData( SCRCMD_WORK *work, u32 arcID, u32 datID );

//���j���[
extern void SCRCMD_WORK_InitMenuWork( SCRCMD_WORK *work,
  u16 x, u16 y, u16 cursor, u16 cancel, SCR_MENU_JUSTIFY lr_just, u16 *ret,
  WORDSET *wordset, GFL_MSGDATA *msgData );
extern void SCRCMD_WORK_AddMenuList(
    SCRCMD_WORK *work, u32 msg_id, u32 ex_msg_id, u32 param,
    STRBUF *msgbuf, STRBUF *tmpbuf );
extern void SCRCMD_WORK_StartMenu( SCRCMD_WORK *work );
extern BOOL SCRCMD_WORK_ProcMenu( SCRCMD_WORK *work );
extern BOOL SCRCMD_WORK_ProcMenu_Breakable( SCRCMD_WORK *work );

//�ޔ�p���[�N����
extern void SCRCMD_WORK_BackupUserWork( SCRCMD_WORK *work );
extern void SCRCMD_WORK_RestoreUserWork( SCRCMD_WORK *work );

//�Ď�VMID�̑���
extern void SCRCMD_WORK_SetWaitVMID( SCRCMD_WORK *work, VMHANDLE_ID vm_id );
extern VMHANDLE_ID SCRCMD_WORK_GetWaitVMID( const SCRCMD_WORK *work );

