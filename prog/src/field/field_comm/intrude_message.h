//==============================================================================
/**
 * @file    intrude_message.h
 * @brief   �N���C�x���g�F���b�Z�[�W���ʏ���
 * @author  matsuda
 * @date    2009.10.27(��)
 */
//==============================================================================
#pragma once



//==============================================================================
//  �\���̒�`
//==============================================================================
///�v�����g����\����
typedef struct{
  WORDSET *wordset;
  STRBUF *msgbuf;
  STRBUF *tmpbuf;
  GFL_MSGDATA *msgdata;
  GFL_MSGDATA *msgdata_mission;
  GFL_MSGDATA *msgdata_mission_mono;
  FLDMSGWIN_STREAM *msgwin_stream;
  FLDMSGWIN *fldmsg_win;
  FLDMSGWIN *fldmsg_win_extra;
  FLDMSGBG *msgBG;
  u8 print_flag;        ///<�V�X�e�������Ŏg�p  TRUE:�o�͂����f�[�^������
  u8 padding[3];
  
  FLDMENUFUNC *fldmenu_func;
}INTRUDE_EVENT_MSGWORK;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void IntrudeEventPrint_SetupFieldMsg(INTRUDE_EVENT_MSGWORK *iem, GAMESYS_WORK *gsys);
extern void IntrudeEventPrint_ExitFieldMsg(INTRUDE_EVENT_MSGWORK *iem);
extern void IntrudeEventPrint_StartStream(INTRUDE_EVENT_MSGWORK *iem, u16 msg_id);
extern BOOL IntrudeEventPrint_WaitStream(INTRUDE_EVENT_MSGWORK *iem);
extern BOOL IntrudeEventPrint_LastKeyWait(void);
extern void IntrudeEventPrint_Print(INTRUDE_EVENT_MSGWORK *iem, u16 msg_id, int x, int y);
extern void IntrudeEventPrint_ExitMenu(INTRUDE_EVENT_MSGWORK *iem);
extern u32 IntrudeEventPrint_SelectMenu(INTRUDE_EVENT_MSGWORK *iem);
extern void IntrudeEventPrint_SetupMainMenu(INTRUDE_EVENT_MSGWORK *iem, GAMESYS_WORK *gsys);
extern void IntrudeEventPrint_SetupYesNo(INTRUDE_EVENT_MSGWORK *iem, GAMESYS_WORK *gsys);
extern void IntrudeEventPrint_ExitYesNo(INTRUDE_EVENT_MSGWORK *iem);
extern FLDMENUFUNC_YESNO IntrudeEventPrint_SelectYesNo(INTRUDE_EVENT_MSGWORK *iem);
extern void IntrudeEventPrint_SetupExtraMsgWin(INTRUDE_EVENT_MSGWORK *iem, GAMESYS_WORK *gsys, int bmp_x, int bmp_y, int size_x, int size_y);
extern void IntrudeEventPrint_PrintExtraMsgWin_MissionMono( INTRUDE_EVENT_MSGWORK *iem, u16 msg_id, int x, int y );
extern void IntrudeEventPrint_ExitExtraMsgWin(INTRUDE_EVENT_MSGWORK *iem);

