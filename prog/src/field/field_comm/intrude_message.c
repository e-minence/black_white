//==============================================================================
/**
 * @file    intrude_message.c
 * @brief   �N���C�x���g�F���b�Z�[�W���ʏ���
 * @author  matsuda
 * @date    2009.10.27(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "field/fieldmap.h"
#include "field/scrcmd.h"
#include "intrude_message.h"
#include "message.naix"
#include "msg/msg_mission_msg.h"
#include "field/field_comm/intrude_types.h"
#include "intrude_mission.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�N���C�x���g�Ŏg�p����q�[�vID
#define INTRUDE_EVENT_HEAPID        (HEAPID_PROC)
///�N���C�x���g�Ŏg�p���郁�b�Z�[�W�o�b�t�@�T�C�Y
#define INTRUDE_EVENT_MSGBUF_SIZE   (256)


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void IntrudeEventPrint_ClearStream(INTRUDE_EVENT_MSGWORK *iem);
static void IntrudeEventPrint_SetupMenu(INTRUDE_EVENT_MSGWORK *iem, GAMESYS_WORK *gsys,
  const FLDMENUFUNC_LIST *menulist, int list_max, const FLDMENUFUNC_HEADER *head);
static void IntrudeEventPrint_SetupPrint(INTRUDE_EVENT_MSGWORK *iem);
static void IntrudeEventPrint_ExitPrint(INTRUDE_EVENT_MSGWORK *iem);
static void _PrintMsgWin( INTRUDE_EVENT_MSGWORK *iem, GFL_MSGDATA *msgdata, 
  FLDMSGWIN *fldmsg_win, u16 msg_id, int x, int y );
static void IntrudeEventPrint_ExitStream(INTRUDE_EVENT_MSGWORK *iem);


//==============================================================================
//
//  �N���C�x���g���ʃ��b�Z�[�W����
//
//==============================================================================
//--------------------------------------------------------------
/**
 * �v�����g����Z�b�g�A�b�v
 *
 * @param   iem		
 * @param   gsys		
 */
//--------------------------------------------------------------
void IntrudeEventPrint_SetupFieldMsg(INTRUDE_EVENT_MSGWORK *iem, GAMESYS_WORK *gsys)
{
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(fieldWork);
  
  iem->wordset = WORDSET_Create(INTRUDE_EVENT_HEAPID);
  iem->msgbuf = GFL_STR_CreateBuffer( INTRUDE_EVENT_MSGBUF_SIZE, INTRUDE_EVENT_HEAPID );
  iem->tmpbuf = GFL_STR_CreateBuffer( INTRUDE_EVENT_MSGBUF_SIZE, INTRUDE_EVENT_HEAPID );
  iem->msgdata = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_mission_msg_dat );
  iem->msgdata_mission = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_mission_dat );
  iem->msgdata_mission_mono = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_mission_monolith_dat );
  iem->msgBG = msgBG;
  iem->print_flag = FALSE;
}

//--------------------------------------------------------------
/**
 * �v�����g����폜
 *
 * @param   iem		
 */
//--------------------------------------------------------------
void IntrudeEventPrint_ExitFieldMsg(INTRUDE_EVENT_MSGWORK *iem)
{
  IntrudeEventPrint_ExitStream(iem);
  IntrudeEventPrint_ExitExtraMsgWin(iem);
  IntrudeEventPrint_ExitPrint(iem);
  
  if(iem->wordset != NULL){
    if(iem->msgwin_stream != NULL){
      FLDMSGWIN_STREAM_Delete(iem->msgwin_stream);
      iem->msgwin_stream = NULL;
    }
    FLDMSGBG_DeleteMSGDATA(iem->msgdata);
    FLDMSGBG_DeleteMSGDATA(iem->msgdata_mission);
    FLDMSGBG_DeleteMSGDATA(iem->msgdata_mission_mono);
    GFL_STR_DeleteBuffer(iem->tmpbuf);
    GFL_STR_DeleteBuffer(iem->msgbuf);
    WORDSET_Delete(iem->wordset);
    
    iem->wordset = NULL;  //����������̈�
  }
}

//--------------------------------------------------------------
/**
 * �v�����g�X�g���[���o�́F�����ݒ�
 *
 * @param   iem		
 */
//--------------------------------------------------------------
static void IntrudeEventPrint_SetupStream(INTRUDE_EVENT_MSGWORK *iem)
{
  if(iem->msgwin_stream != NULL){
    if(iem->print_flag == TRUE){
      IntrudeEventPrint_ClearStream(iem);
    }
    return;
  }
  iem->msgwin_stream = FLDMSGWIN_STREAM_AddTalkWin( iem->msgBG, iem->msgdata );
}

//--------------------------------------------------------------
/**
 * �v�����g�X�g���[���o�́F�폜
 *
 * @param   iem		
 */
//--------------------------------------------------------------
static void IntrudeEventPrint_ExitStream(INTRUDE_EVENT_MSGWORK *iem)
{
  if(iem->msgwin_stream == NULL){
    return;
  }
  FLDMSGWIN_STREAM_Delete(iem->msgwin_stream);
  iem->msgwin_stream = NULL;
}

//--------------------------------------------------------------
/**
 * �v�����g�X�g���[���o�́F���ʏ���
 *
 * @param   iem		
 * @param   strbuf		�o�̓��b�Z�[�W
 */
//--------------------------------------------------------------
static void _StartStream(INTRUDE_EVENT_MSGWORK *iem, STRBUF *strbuf)
{
  IntrudeEventPrint_SetupStream(iem);
  
  FLDMSGWIN_STREAM_PrintStrBufStart( iem->msgwin_stream, 0, 0, strbuf );
  iem->print_flag = TRUE;
}

//--------------------------------------------------------------
/**
 * �v�����g�X�g���[���o�́Fmission_msg.gmm�p
 *
 * @param   iem		
 * @param   msg_id		
 */
//--------------------------------------------------------------
void IntrudeEventPrint_StartStream(INTRUDE_EVENT_MSGWORK *iem, u16 msg_id)
{
  GFL_MSG_GetString( iem->msgdata, msg_id, iem->tmpbuf );
  WORDSET_ExpandStr( iem->wordset, iem->msgbuf, iem->tmpbuf );
  
  _StartStream(iem, iem->msgbuf);
}

//--------------------------------------------------------------
/**
 * �v�����g�X�g���[���o�́Fmission.gmm�p
 *      ���ݎ��{���̃~�b�V������\��
 *
 * @param   iem		
 * @param   
 */
//--------------------------------------------------------------
BOOL IntrudeEventPrint_StartStreamMission(INTRUDE_EVENT_MSGWORK *iem, INTRUDE_COMM_SYS_PTR intcomm)
{
  const MISSION_DATA *mdata;

  if(MISSION_RecvCheck(&intcomm->mission) == FALSE){
    return FALSE; //���{���Ă���~�b�V�����͖���
  }
  
  mdata = MISSION_GetRecvData(&intcomm->mission);

  GFL_MSG_GetString(iem->msgdata_mission, mdata->cdata.msg_id_contents, iem->tmpbuf);
  MISSIONDATA_Wordset(&mdata->cdata, &mdata->target_info, 
    iem->wordset, INTRUDE_EVENT_HEAPID);
  WORDSET_ExpandStr( iem->wordset, iem->msgbuf, iem->tmpbuf );
  
  _StartStream(iem, iem->msgbuf);
  return TRUE;
}

//==================================================================
/**
 * ���b�Z�[�W�̈���N���A
 *
 * @param   iem		
 */
//==================================================================
static void IntrudeEventPrint_ClearStream(INTRUDE_EVENT_MSGWORK *iem)
{
  GF_ASSERT(iem->msgwin_stream != NULL);
  FLDMSGWIN_STREAM_ClearMessage(iem->msgwin_stream);
  iem->print_flag = FALSE;
}

//--------------------------------------------------------------
/**
 * �v�����g�X�g���[���o�͑҂�
 *
 * @param   iem		
 *
 * @retval  BOOL TRUE=�\���I��,FALSE=�\����
 */
//--------------------------------------------------------------
BOOL IntrudeEventPrint_WaitStream(INTRUDE_EVENT_MSGWORK *iem)
{
  if(iem->msgwin_stream == NULL){
    return TRUE;
  }
  return FLDMSGWIN_STREAM_Print(iem->msgwin_stream);
}

//==================================================================
/**
 * ���X�g�L�[�҂�
 *
 * @retval  BOOL		TRUE�F���X�g�L�[�������ꂽ
 */
//==================================================================
BOOL IntrudeEventPrint_LastKeyWait(void)
{
  if( GFL_UI_KEY_GetTrg() & EVENT_WAIT_LAST_KEY ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�F�����ݒ�
 *
 * @param   iem		
 */
//--------------------------------------------------------------
static void IntrudeEventPrint_SetupPrint(INTRUDE_EVENT_MSGWORK *iem)
{
  if(iem->fldmsg_win == NULL){
    iem->fldmsg_win = FLDMSGWIN_Add(iem->msgBG, iem->msgdata, 1, 19, 30, 4);
  }
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�F�폜
 *
 * @param   iem		
 */
//--------------------------------------------------------------
static void IntrudeEventPrint_ExitPrint(INTRUDE_EVENT_MSGWORK *iem)
{
  if(iem->fldmsg_win != NULL){
    FLDMSGWIN_Delete(iem->fldmsg_win);
    iem->fldmsg_win = NULL;
  }
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�F�����`��
 *
 * @param   iem		
 * @param   msgdata		
 * @param   msg_id    ���b�Z�[�WID
 * @param   x		      �`��ʒuX
 * @param   y		      �`��ʒuY
 */
//--------------------------------------------------------------
static void _PrintMsgWin( INTRUDE_EVENT_MSGWORK *iem, GFL_MSGDATA *msgdata, FLDMSGWIN *fldmsg_win, u16 msg_id, int x, int y )
{
  GFL_MSG_GetString( msgdata, msg_id, iem->tmpbuf );
  WORDSET_ExpandStr( iem->wordset, iem->msgbuf, iem->tmpbuf );
  FLDMSGWIN_PrintStrBuf( fldmsg_win, x, y, iem->msgbuf );
}

//==================================================================
/**
 * �����ꊇ�`��
 *
 * @param   iem		
 * @param   msg_id		���b�Z�[�WID
 * @param   x		      �`��ʒuX
 * @param   y		      �`��ʒuY
 */
//==================================================================
void IntrudeEventPrint_Print(INTRUDE_EVENT_MSGWORK *iem, u16 msg_id, int x, int y)
{
  IntrudeEventPrint_SetupPrint(iem);
  _PrintMsgWin( iem, iem->msgdata, iem->fldmsg_win, msg_id, x, y );
}



//==============================================================================
//
//  ���j���[
//
//==============================================================================
///��b���C�����j���[���X�g
static const FLDMENUFUNC_LIST TalkMainMenuList[] =
{
  {msg_talk_menu_001, (void*)INTRUDE_TALK_STATUS_BATTLE},   //�ΐ�
  {msg_talk_menu_002, (void*)INTRUDE_TALK_STATUS_ITEM},     //�A�C�e���n��
  {msg_talk_menu_003, (void*)INTRUDE_TALK_STATUS_CANCEL},   //�L�����Z��
};

///���j���[�w�b�_�[(���C�����j���[�p)
static const FLDMENUFUNC_HEADER MenuHeader_MainMenu =
{
	3,		//���X�g���ڐ�
	3,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	12,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	0,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_NON,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	16,		//�����T�C�YY(�h�b�g
	32-15,		//�\�����WX �L�����P��
	1,		//�\�����WY �L�����P��
	14,		//�\���T�C�YX �L�����P��
	6,		//�\���T�C�YY �L�����P��
};

//--------------------------------------------------------------
/**
 * ���j���[���X�g�F�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 * @param   menulist		
 * @param   list_max		
 */
//--------------------------------------------------------------
static void IntrudeEventPrint_SetupMenu(INTRUDE_EVENT_MSGWORK *iem, GAMESYS_WORK *gsys,
  const FLDMENUFUNC_LIST *menulist, int list_max, const FLDMENUFUNC_HEADER *head)
{
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLDMENUFUNC_LISTDATA *fldmenu_listdata;
  
  GF_ASSERT(iem->fldmenu_func == NULL && iem->msgdata != NULL);
  
  fldmenu_listdata = FLDMENUFUNC_CreateMakeListData(
      menulist, list_max, iem->msgdata, INTRUDE_EVENT_HEAPID);
  iem->fldmenu_func = FLDMENUFUNC_AddMenu(iem->msgBG, head, fldmenu_listdata);
}

//--------------------------------------------------------------
/**
 * ���j���[���X�g�F�폜
 *
 * @param   unisys		
 */
//--------------------------------------------------------------
void IntrudeEventPrint_ExitMenu(INTRUDE_EVENT_MSGWORK *iem)
{
  if(iem->fldmenu_func != NULL){
    FLDMENUFUNC_DeleteMenu(iem->fldmenu_func);
    iem->fldmenu_func = NULL;
  }
}

//--------------------------------------------------------------
/**
 * ���j���[���X�g�F�I��҂�
 *
 * @param   unisys		
 *
 * @retval  u32		����
 */
//--------------------------------------------------------------
u32 IntrudeEventPrint_SelectMenu(INTRUDE_EVENT_MSGWORK *iem)
{
  GF_ASSERT(iem->fldmenu_func != NULL);
  return FLDMENUFUNC_ProcMenu(iem->fldmenu_func);
}

//--------------------------------------------------------------
/**
 * ���C�����j���[���X�g�F�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 * @param   menulist		
 * @param   list_max		
 */
//--------------------------------------------------------------
void IntrudeEventPrint_SetupMainMenu(INTRUDE_EVENT_MSGWORK *iem, GAMESYS_WORK *gsys)
{
  IntrudeEventPrint_SetupMenu(iem, gsys, 
    TalkMainMenuList, NELEMS(TalkMainMenuList), &MenuHeader_MainMenu);
}

//==============================================================================
//  �͂�/������ �I�����j���[
//==============================================================================
//==================================================================
/**
 * �͂�/������ �I�����j���[�F�Z�b�g�A�b�v
 *
 * @param   iem		
 * @param   gsys		
 */
//==================================================================
void IntrudeEventPrint_SetupYesNo(INTRUDE_EVENT_MSGWORK *iem, GAMESYS_WORK *gsys)
{
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);

  GF_ASSERT(iem->fldmenu_func == NULL && iem->msgdata != NULL);
  iem->fldmenu_func = FLDMENUFUNC_AddYesNoMenu(iem->msgBG, FLDMENUFUNC_YESNO_YES);
}

//--------------------------------------------------------------
/**
 * �͂�/������ �I�����j���[�F�폜
 *
 * @param   iem		
 */
//--------------------------------------------------------------
void IntrudeEventPrint_ExitYesNo(INTRUDE_EVENT_MSGWORK *iem)
{
  IntrudeEventPrint_ExitMenu(iem);
}

//--------------------------------------------------------------
/**
 * �͂�/������ �I�����j���[�F�I��҂�
 *
 * @param   iem		
 *
 * @retval  FLDMENUFUNC_YESNO		����
 */
//--------------------------------------------------------------
FLDMENUFUNC_YESNO IntrudeEventPrint_SelectYesNo(INTRUDE_EVENT_MSGWORK *iem)
{
  GF_ASSERT(iem->fldmenu_func != NULL);
  return FLDMENUFUNC_ProcYesNoMenu(iem->fldmenu_func);
}


//==============================================================================
//
//  MSGWIN�Z�b�g�A�b�v
//
//==============================================================================
//==================================================================
/**
 * ���b�Z�[�W�E�B���h�E(��b�ȊO)�F�Z�b�g�A�b�v
 *
 * @param   iem		
 * @param   gsys		
 * @param   bmp_x		  BMP X�ʒu(�L�����N�^�P��)
 * @param   bmp_y		  BMP Y�ʒu(�L�����N�^�P��)
 * @param   size_x		�T�C�YX(�L�����N�^�P��)
 * @param   size_y		�T�C�YY(�L�����N�^�P��)
 */
//==================================================================
void IntrudeEventPrint_SetupExtraMsgWin(INTRUDE_EVENT_MSGWORK *iem, GAMESYS_WORK *gsys, int bmp_x, int bmp_y, int size_x, int size_y)
{
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);

  GF_ASSERT(iem->fldmsg_win_extra == NULL);
  iem->fldmsg_win_extra = FLDMSGWIN_Add(iem->msgBG, iem->msgdata, bmp_x, bmp_y, size_x, size_y);
}

//==================================================================
/**
 * ���b�Z�[�W�E�B���h�E(��b�ȊO)�F�����`�� mission_monolith.gmm
 *
 * @param   iem		
 * @param   msg_id    ���b�Z�[�WID
 * @param   x		      �`��ʒuX
 * @param   y		      �`��ʒuY
 */
//==================================================================
void IntrudeEventPrint_PrintExtraMsgWin_MissionMono( INTRUDE_EVENT_MSGWORK *iem, u16 msg_id, int x, int y )
{
  _PrintMsgWin( iem, iem->msgdata_mission_mono, iem->fldmsg_win_extra, msg_id, x, y );
}

//==================================================================
/**
 * ���b�Z�[�W�E�B���h�E(��b�ȊO)�F�폜
 *
 * @param   iem		
 */
//==================================================================
void IntrudeEventPrint_ExitExtraMsgWin(INTRUDE_EVENT_MSGWORK *iem)
{
  if(iem->fldmsg_win_extra == NULL){
    return;
  }
  FLDMSGWIN_Delete(iem->fldmsg_win_extra);
  iem->fldmsg_win_extra = NULL;
}
