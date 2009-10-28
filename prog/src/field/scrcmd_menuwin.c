//======================================================================
/**
 * @file	scrcmd_menuwin.c
 * @brief	�X�N���v�g�R�}���h�F���j���[�A��b�E�B���h�E�֘A
 * @date  2009.09.13
 * @author	tamada GAMEFREAK inc.
 *
 * 2009.09.13 scrcmd.c��蕪��������
 */
//======================================================================

#include <wchar.h>
#include <gflib.h>

#include "system/vm_cmd.h"

#include "scrcmd.h"
#include "script_local.h"
#include "scrcmd_work.h"
#include "scrcmd_menuwin.h"

#include "fieldmap.h"

#include "arc/script_message.naix"
#include "msg/script/msg_common_scr.h"


// �������\���E�B���h�E
#define GOLD_WIN_WIDTH (14) // ��(�L�����N�^�P��)
#define GOLD_WIN_HEIGHT (2) // ����(�L�����N�^�P��)
#define GOLD_WIN_KETA   (6) // ���������l�̌���

static void CloseWin(SCRCMD_WORK *work);

//--------------------------------------------------------------
/**
 *  �E�B���h�E�I��
 * @param   work    �X�N���v�g�R�}���h���[�N�|�C���^
 * @retval  none
 */
//--------------------------------------------------------------
static void CloseWin(SCRCMD_WORK *work)
{
  FLDMSGWIN_STREAM *msgWin;
  msgWin = SCRCMD_WORK_GetFldMsgWinStream( work );
  FLDMSGWIN_STREAM_Delete( msgWin );
  SCREND_CHK_SetBitOff(SCREND_CHK_WIN_OPEN);
}

//--------------------------------------------------------------
/**
 *�X�N���v�g�I�����E�B���h�E�I���`�F�b�N
 * @param   end_chk     �`�F�b�N�\����
 * @param   seq     �T�u�V�[�P���T
 * @retval  BOOL    TRUE�Ń`�F�b�N�I��
 */
//--------------------------------------------------------------
BOOL SCREND_CheckEndWin(SCREND_CHECK *end_check , int *seq)
{
  CloseWin(end_check->ScrCmdWk);
  return  TRUE;
}


//======================================================================
//  �͂��A�������@����
//======================================================================
//--------------------------------------------------------------
/**
 * �u�͂��E�������v���� �E�F�C�g����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL EvYesNoWinSelect( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  FLDMENUFUNC **mw = SCRIPT_GetMemberWork( sc, ID_EVSCR_MENUWORK );
  u16 *ret_wk = SCRIPT_GetEventWork( sc, gdata, core->vm_register[0] );
  FLDMENUFUNC_YESNO ret = FLDMENUFUNC_ProcYesNoMenu( *mw );
  
  if( ret == FLDMENUFUNC_YESNO_NULL ){
    return FALSE;
  }
  
  if( ret == FLDMENUFUNC_YESNO_YES ){
    *ret_wk = 0;
  }else{
    *ret_wk = 1;
  }
  
  FLDMENUFUNC_DeleteMenu( *mw );
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �u�͂��E�������v����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdYesNoWin( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  FLDMENUFUNC **mw  = SCRIPT_GetMemberWork( sc, ID_EVSCR_MENUWORK );
  u16 wk_id      = VMGetU16( core );
  
  *mw = FLDMENUFUNC_AddYesNoMenu( fparam->msgBG, 0 );
  core->vm_register[0] = wk_id;
   
  VMCMD_SetWait( core, EvYesNoWinSelect );
  return VMCMD_RESULT_SUSPEND;
}

//======================================================================
//  ���j���[
//======================================================================
//--------------------------------------------------------------
/**
 * BMP���j���[������
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBmpMenuInit( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  u8 x = VMGetU8(core);
  u8 y = VMGetU8(core);
  u8 cursor = VMGetU8(core);
  u8 cancel = VMGetU8(core);
  u16 wk_id = VMGetU16( core );
  u16 *ret = SCRIPT_GetEventWork( sc, gdata, wk_id );
  WORDSET **wordset = SCRIPT_GetMemberWork( sc, ID_EVSCR_WORDSET );

  SCRCMD_WORK_InitMenuWork( work,
      x, y, cursor, cancel, ret,
      *wordset, NULL );
  
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * BMP���j���[������(�ǂݍ���ł���gmm�t�@�C�����g�p����) 
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBmpMenuInitEx( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  u8 x = VMGetU8(core);
  u8 y = VMGetU8(core);
  u8 cursor = VMGetU8(core);
  u8 cancel = VMGetU8(core);
  u16 wk_id = VMGetU16( core );
  u16 *ret = SCRIPT_GetEventWork( sc, gdata, wk_id );
  WORDSET **wordset = SCRIPT_GetMemberWork( sc, ID_EVSCR_WORDSET );
  GFL_MSGDATA *msgData = SCRCMD_WORK_GetMsgData( work );

  SCRCMD_WORK_InitMenuWork( work,
      x, y, cursor, cancel, ret,
      *wordset, msgData );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * BMP���j���[	���X�g�쐬
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBmpMenuMakeList( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u16 msg_id	= VMGetU16( core );
	u16 param	= VMGetU16( core );
  STRBUF **msgbuf = SCRIPT_GetMemberWork( sc, ID_EVSCR_MSGBUF );
  STRBUF **tmpbuf = SCRIPT_GetMemberWork( sc, ID_EVSCR_TMPBUF );
  SCRCMD_WORK_AddMenuList( work, msg_id, param, *msgbuf, *tmpbuf );
	return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * BMP���j���[	�E�F�C�g����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL EvSelWinWait( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  if( SCRCMD_WORK_ProcMenu(work) == TRUE ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * BMP���j���[	�J�n
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBmpMenuStart( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRCMD_WORK_StartMenu( work );
	VMCMD_SetWait( core, EvSelWinWait );
	return VMCMD_RESULT_SUSPEND;
}


//======================================================================
//  �t�B�[���h�@��b�E�B���h�E
//======================================================================
//--------------------------------------------------------------
/**
 * ��b�E�B���h�E�\���ǉ�
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void talkMsgWin_AddWindow( SCRCMD_WORK *work )
{
  SCRIPT_WORK *sc;
  SCRIPT_FLDPARAM *fparam;
  GFL_MSGDATA *msgData;
  FLDMSGWIN_STREAM *msgWin;
///  u8 *win_open_flag;
  
  sc = SCRCMD_WORK_GetScriptWork( work );
  fparam = SCRIPT_GetFieldParam( sc );
  msgData = SCRCMD_WORK_GetMsgData( work );
  msgWin = FLDMSGWIN_STREAM_AddTalkWin( fparam->msgBG, msgData );
  SCRCMD_WORK_SetFldMsgWinStream( work, msgWin );
  
///  win_open_flag = SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_OPEN_FLAG );
///  *win_open_flag = TRUE;  //ON;
  SCREND_CHK_SetBitOn(SCREND_CHK_WIN_OPEN);
}

//--------------------------------------------------------------
/**
 * ��b�E�B���h�E���b�Z�[�W�\���@�E�F�C�g����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL TalkMsgWait( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  FLDMSGWIN_STREAM *msgWin;
  msgWin = SCRCMD_WORK_GetFldMsgWinStream( work );
  
  if( FLDMSGWIN_STREAM_Print(msgWin) == TRUE ){
    return( 1 );
  }
   
  return( 0 );
}

//--------------------------------------------------------------
/**
 * �o�^���ꂽ�P����g���ĕ�����W�J�@��b�E�B���h�E���b�Z�[�W�\��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTalkMsg( VMHANDLE *core, void *wk )
{
  FLDMSGWIN_STREAM *msgWin;
  SCRCMD_WORK *work = wk;
  u16 msg_id = SCRCMD_GetVMWorkValue( core, work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  STRBUF **msgbuf = SCRIPT_GetMemberWork( sc, ID_EVSCR_MSGBUF );
#if 0  
  {
    u8 *win_open_flag = SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_OPEN_FLAG );
    
    if( (*win_open_flag) == 0 ){
      talkMsgWin_AddWindow( work );
    }
  }
#else
  {
    if( !SCREND_CHK_CheckBit(SCREND_CHK_WIN_OPEN) ){
      talkMsgWin_AddWindow( work );
    }
  }

#endif
  {
    GFL_MSGDATA *msgData = SCRCMD_WORK_GetMsgData( work );
    WORDSET **wordset = SCRIPT_GetMemberWork( sc, ID_EVSCR_WORDSET );
    STRBUF **tmpbuf = SCRIPT_GetMemberWork( sc, ID_EVSCR_TMPBUF );
    GFL_MSG_GetString( msgData, msg_id, *tmpbuf );
    WORDSET_ExpandStr( *wordset, *msgbuf, *tmpbuf );
  }
  
  msgWin = SCRCMD_WORK_GetFldMsgWinStream( work );
  FLDMSGWIN_STREAM_ClearMessage( msgWin );
  FLDMSGWIN_STREAM_PrintStrBufStart( msgWin, 0, 0, *msgbuf );
  VMCMD_SetWait( core, TalkMsgWait );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �������E�B���h�E��\������
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//-------------------------------------------------------------- 
VMCMD_RESULT EvCmdGoldWinOpen( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work = wk;
  u16                   x = SCRCMD_GetVMWorkValue( core, work );  // �X�N���v�g��1����
  u16                   y = SCRCMD_GetVMWorkValue( core, work );  // �X�N���v�g��2����
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  PLAYER_WORK*     player = GAMEDATA_GetMyPlayerWork( gdata ); 
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FLDMSGBG*        msg_bg = FIELDMAP_GetFldMsgBG( fieldmap );
  FLDMSGWIN*      msg_win = FIELDMAP_GetGoldMsgWin( fieldmap );
  SCRIPT_WORK*         sc = SCRCMD_WORK_GetScriptWork( work );
  WORDSET**       wordset = SCRIPT_GetMemberWork( sc, ID_EVSCR_WORDSET );
  STRBUF**        tempbuf = SCRIPT_GetMemberWork( sc, ID_EVSCR_TMPBUF );
  HEAPID          heap_id = FIELDMAP_GetHeapID( fieldmap );
  GFL_MSGDATA*   msg_data = GFL_MSG_Create( 
      GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE, NARC_script_message_common_scr_dat, heap_id );
  STRBUF*          strbuf = GFL_STR_CreateBuffer( 128, heap_id );
  u32                gold = MyStatus_GetGold( &player->mystatus );

  // �\�����̃E�B���h�E���폜
  if( msg_win != NULL )
  {
    FLDMSGWIN_Delete( msg_win );
  }

  // �E�B���h�E��\��
  msg_win = FLDMSGWIN_Add( msg_bg, msg_data, x, y, GOLD_WIN_WIDTH, GOLD_WIN_HEIGHT );
  FIELDMAP_SetGoldMsgWin( fieldmap, msg_win );

  // ��������쐬
  WORDSET_RegisterNumber( 
      *wordset, 2, gold, GOLD_WIN_KETA, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
  GFL_MSG_GetString( msg_data, msg_yen_01, *tempbuf );
  WORDSET_ExpandStr( *wordset, strbuf, *tempbuf );
  FLDMSGWIN_PrintStrBuf( msg_win, 0, 0, strbuf );
  GFL_STR_DeleteBuffer( strbuf );
  GFL_MSG_Delete( msg_data );

  OBATA_Printf( "EvCmdGoldWinOpen\n" );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �������E�B���h�E���X�V����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//-------------------------------------------------------------- 
VMCMD_RESULT EvCmdGoldWinUpdate( VMHANDLE *core, void *wk )
{
  // �ĕ\������
  return EvCmdGoldWinOpen( core, wk );
}

//--------------------------------------------------------------
/**
 * �������E�B���h�E����������
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//-------------------------------------------------------------- 
VMCMD_RESULT EvCmdGoldWinClose( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work = wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FLDMSGWIN*      msg_win = FIELDMAP_GetGoldMsgWin( fieldmap );

  if( msg_win )
  {
    FLDMSGWIN_Delete( msg_win );
    FIELDMAP_SetGoldMsgWin( fieldmap, NULL );
  }
  OBATA_Printf( "EvCmdGoldWinClose\n" );
  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
//--------------------------------------------------------------
#if 0
static VMCMD_RESULT EvCmdTalkMsg( VMHANDLE *core, void *wk )
{
  u16 dir;
  VecFx32 pos;
  const VecFx32 *pos_p;
  WORDSET **wordset;
  STRBUF **msgbuf;
  STRBUF **tmpbuf;
  MMDL *fmmdl;
  FLDTALKMSGWIN *tmsg;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u8 msg_id = VMGetU8(core);
  GFL_MSGDATA *msgData = SCRCMD_WORK_GetMsgData( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  
  fmmdl = MMDLSYS_SearchOBJID( SCRCMD_WORK_GetMMdlSys(work), 0xff );
  MMDL_GetVectorPos( fmmdl, &pos );
  dir = MMDL_GetDirDisp( fmmdl );
  MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
  
  if( dir == DIR_UP ){        //������
    pos.x += FX32_ONE*8;
    pos.z -= FX32_ONE*8;
  }else if( dir == DIR_DOWN ){ //�ォ��
    pos.x += FX32_ONE*8;
    pos.z -= FX32_ONE*8;
  }else if( dir == DIR_LEFT ){ //�E����
    pos.x += -FX32_ONE*8;
    pos.z += FX32_ONE*16;
  }else{                       //������
    pos.x += FX32_ONE*8;
    pos.z += FX32_ONE*16;
  }
  
  SCRCMD_WORK_SetTalkMsgWinTailPos( work, &pos );
  pos_p = SCRCMD_WORK_GetTalkMsgWinTailPos( work );
  
  {
    wordset = SCRIPT_GetMemberWork( sc, ID_EVSCR_WORDSET );
    msgbuf = SCRIPT_GetMemberWork( sc, ID_EVSCR_MSGBUF );
    tmpbuf = SCRIPT_GetMemberWork( sc, ID_EVSCR_TMPBUF );
    KAGAYA_Printf( "EvCmdTalkMsg MSG ID = %d\n", msg_id );
    GFL_MSG_GetString( msgData, msg_id, *tmpbuf );
    WORDSET_ExpandStr( *wordset, *msgbuf, *tmpbuf );
  }
  
  {
    FLDTALKMSGWIN_IDX idx = FLDTALKMSGWIN_IDX_LOWER;
    
    if( dir == DIR_UP ){
      idx = FLDTALKMSGWIN_IDX_UPPER;
    }
    
    tmsg = FLDTALKMSGWIN_AddStrBuf( fparam->msgBG, idx, pos_p, *msgbuf );
  }
  
  SCRCMD_WORK_SetFldMsgWinStream( work, (FLDMSGWIN_STREAM*)tmsg );
  PMSND_PlaySystemSE( SEQ_SE_MESSAGE );

  VMCMD_SetWait( core, TalkMsgWait );
  return VMCMD_RESULT_SUSPEND;
}
#endif

//--------------------------------------------------------------
/**
 * �o�^���ꂽ�P����g���ĕ�����W�J�@��b�E�B���h�E���b�Z�[�W�S�\��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTalkMsgAllPut( VMHANDLE *core, void *wk )
{
  FLDMSGWIN_STREAM *msgWin;
  SCRCMD_WORK *work = wk;
  u16 msg_id = SCRCMD_GetVMWorkValue( core, work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  STRBUF **msgbuf = SCRIPT_GetMemberWork( sc, ID_EVSCR_MSGBUF );
#if 0  
  {
    u8 *win_open_flag = SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_OPEN_FLAG );
    
    if( (*win_open_flag) == 0 ){
      talkMsgWin_AddWindow( work );
    }
  }
#else
  {
    if ( !SCREND_CHK_CheckBit(SCREND_CHK_WIN_OPEN) ){
      talkMsgWin_AddWindow( work );
    }
  }
#endif
  {
    GFL_MSGDATA *msgData = SCRCMD_WORK_GetMsgData( work );
    WORDSET **wordset = SCRIPT_GetMemberWork( sc, ID_EVSCR_WORDSET );
    STRBUF **tmpbuf = SCRIPT_GetMemberWork( sc, ID_EVSCR_TMPBUF );
    GFL_MSG_GetString( msgData, msg_id, *tmpbuf );
    WORDSET_ExpandStr( *wordset, *msgbuf, *tmpbuf );
  }
  
  msgWin = SCRCMD_WORK_GetFldMsgWinStream( work );
  FLDMSGWIN_STREAM_ClearMessage( msgWin );
  FLDMSGWIN_STREAM_AllPrintStrBuf( msgWin, 0, 0, *msgbuf );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ��b�E�B���h�E�\��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTalkWinOpen( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  talkMsgWin_AddWindow( work );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ��b�E�B���h�E�����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTalkWinClose( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
#if 0  
  SCRIPT_WORK *sc;
  FLDMSGWIN_STREAM *msgWin;
  u8 *win_open_flag;
  
  sc = SCRCMD_WORK_GetScriptWork( work );
  win_open_flag = SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_OPEN_FLAG );
  if( (*win_open_flag) == TRUE ){
    msgWin = SCRCMD_WORK_GetFldMsgWinStream( work );
    FLDMSGWIN_STREAM_Delete( msgWin );
  }
  
  *win_open_flag = FALSE;  //OFF;
#else
  if ( SCREND_CHK_CheckBit(SCREND_CHK_WIN_OPEN) ){
    CloseWin(work);
  }
#endif
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//  �����o���E�B���h�E
//======================================================================
//--------------------------------------------------------------
/**
 * �����o���E�B���h�E�`��
 * @param work  SCRCMD_WORK
 * @param objID �����o�����o��OBJID
 * @param arcID �\�����郁�b�Z�[�W�̃A�[�J�C�u�w��ID
 * @param msgID �\�����郁�b�Z�[�WID
 * @retval BOOL TRUE=�\�� FALSE=�G���[
 *
 * arcID=0x400�̏ꍇ�A�f�t�H���g�̃��b�Z�[�W�A�[�J�C�u���g�p����
 */
//--------------------------------------------------------------
static BOOL balloonWin_Write( SCRCMD_WORK *work, u16 objID, u16 arcID, u16 msgID )
{
  VecFx32 pos,jiki_pos;
  const VecFx32 *p_pos;
  STRBUF **msgbuf;
  FLDTALKMSGWIN *tmsg;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GFL_MSGDATA *msgData = SCRCMD_WORK_GetMsgData( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  MMDL *jiki = MMDLSYS_SearchOBJID( mmdlsys, MMDL_ID_PLAYER );
  MMDL *npc = MMDLSYS_SearchOBJID( mmdlsys, objID );
  
  if( npc == NULL ){
    OS_Printf("�X�N���v�g�G���[ �����o���Ώۂ�OBJ�����݂��܂���\n");
    return( FALSE );
  }
  
  MMDL_GetVectorPos( npc, &pos );
  MMDL_GetVectorPos( jiki, &jiki_pos );

  SCRCMD_WORK_SetTalkMsgWinTailPos( work, &pos );
  p_pos = SCRCMD_WORK_GetTalkMsgWinTailPos( work );
  
  msgbuf = SCRIPT_GetMemberWork( sc, ID_EVSCR_MSGBUF );
  
  {
    WORDSET **wordset = SCRIPT_GetMemberWork( sc, ID_EVSCR_WORDSET );
    STRBUF **tmpbuf = SCRIPT_GetMemberWork( sc, ID_EVSCR_TMPBUF );
    if (arcID == 0x400)
    {
      GFL_MSG_GetString( msgData, msgID, *tmpbuf );
    } else {
      msgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE,
          arcID, SCRCMD_WORK_GetHeapID( work ) );
      GFL_MSG_GetString( msgData, msgID, *tmpbuf );
      GFL_MSG_Delete( msgData );
    }
    WORDSET_ExpandStr( *wordset, *msgbuf, *tmpbuf );
  }
  
  {
    FLDTALKMSGWIN_IDX idx = FLDTALKMSGWIN_IDX_LOWER;
    
    if( pos.z < jiki_pos.z ){
      idx = FLDTALKMSGWIN_IDX_UPPER;
    }
    
    tmsg = FLDTALKMSGWIN_AddStrBuf( fparam->msgBG, idx, p_pos, *msgbuf );
  }
  
  SCRCMD_WORK_SetFldMsgWinStream( work, (FLDMSGWIN_STREAM*)tmsg );
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �����o���E�B���h�E�@�\���҂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL BallonWinMsgWait( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  FLDTALKMSGWIN *tmsg;
  tmsg = (FLDTALKMSGWIN*)SCRCMD_WORK_GetFldMsgWinStream( work );
  
  if( FLDTALKMSGWIN_Print(tmsg) == TRUE ){
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �����o���E�B���h�E �`�� OBJID�w��L��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBalloonWinWrite( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  u16 arc_id = SCRCMD_GetVMWorkValue( core, work );
  u16 msg_id = SCRCMD_GetVMWorkValue( core, work );
  u8 obj_id = VMGetU8( core );
  
  KAGAYA_Printf( "�����o���E�B���h�E OBJID =%d\n", obj_id );

  if( balloonWin_Write(work,obj_id,arc_id,msg_id) == TRUE ){
    VMCMD_SetWait( core, BallonWinMsgWait );
    return VMCMD_RESULT_SUSPEND;
  }
  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �����o���E�B���h�E �`�� �b���|��OBJ�Ώ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBalloonWinTalkWrite( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  MMDL **mmdl = SCRIPT_GetMemberWork( sc, ID_EVSCR_TARGET_OBJ );
  u16 arc_id = SCRCMD_GetVMWorkValue( core, work );
  u16 msg_id = SCRCMD_GetVMWorkValue( core, work );
  
  if( *mmdl == NULL ){
    OS_Printf( "�X�N���v�g�G���[ �b���|���Ώۂ�OBJ�����܂���\n" );
    return VMCMD_RESULT_SUSPEND;
  }

  {
    u8 obj_id = MMDL_GetOBJID( *mmdl );
    
    if( balloonWin_Write(work,obj_id,arc_id,msg_id) == TRUE ){
      VMCMD_SetWait( core, BallonWinMsgWait );
      return VMCMD_RESULT_SUSPEND;
    }
  }
  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �����o���E�B���h�E�����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBalloonWinClose( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  FLDTALKMSGWIN *tmsg;
  
  tmsg = (FLDTALKMSGWIN*) SCRCMD_WORK_GetFldMsgWinStream( work );
  FLDTALKMSGWIN_Delete( tmsg );
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//  �g���[�i�[���b�Z�[�W
//======================================================================
//--------------------------------------------------------------
/**
 * �g���[�i�[��b�Ăяo��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerMessageSet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u16 *script_id = SCRIPT_GetMemberWork( sc, ID_EVSCR_SCRIPT_ID );
  
  u16 tr_id = SCRCMD_GetVMWorkValue( core, work );
  u16 kind_id = SCRCMD_GetVMWorkValue( core, work );
  
  WORDSET **wordset = SCRIPT_GetMemberWork( sc, ID_EVSCR_WORDSET );
  STRBUF **msgbuf = SCRIPT_GetMemberWork( sc, ID_EVSCR_MSGBUF );
  STRBUF **tmpbuf = SCRIPT_GetMemberWork( sc, ID_EVSCR_TMPBUF );
  GFL_MSGDATA *msgData = SCRCMD_WORK_GetMsgData( work );
///  u8 *win_open_flag = SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_OPEN_FLAG );
  u8 *msg_index      = SCRIPT_GetMemberWork( sc, ID_EVSCR_MSGINDEX );
  
  KAGAYA_Printf( "TR ID =%d, KIND ID =%d\n", tr_id, kind_id );

  TT_TrainerMessageGet(
      tr_id, kind_id, *msgbuf, SCRCMD_WORK_GetHeapID(work) );
  
  {
    u16 dir;
    VecFx32 pos;
    const VecFx32 *pos_p;
    MMDL *fmmdl;
    
    fmmdl = MMDLSYS_SearchOBJID( SCRCMD_WORK_GetMMdlSys(work), 0xff );
    MMDL_GetVectorPos( fmmdl, &pos );
    dir = MMDL_GetDirDisp( fmmdl );
    MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
    
    if( dir == DIR_UP ){        //������
      pos.x += FX32_ONE*8;
      pos.z -= FX32_ONE*8;
    }else if( dir == DIR_DOWN ){ //�ォ��
      pos.x += FX32_ONE*8;
      pos.z -= FX32_ONE*8;
    }else if( dir == DIR_LEFT ){ //�E����
      pos.x += -FX32_ONE*8;
      pos.z += FX32_ONE*16;
    }else{                       //������
      pos.x += FX32_ONE*8;
      pos.z += FX32_ONE*16;
    }
    
    SCRCMD_WORK_SetTalkMsgWinTailPos( work, &pos );
    pos_p = SCRCMD_WORK_GetTalkMsgWinTailPos( work );
    
    {
      FLDTALKMSGWIN *tmsg;
      FLDTALKMSGWIN_IDX idx = FLDTALKMSGWIN_IDX_LOWER;
      SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
      
      if( dir == DIR_UP ){
        idx = FLDTALKMSGWIN_IDX_UPPER;
      }
      
      tmsg = FLDTALKMSGWIN_AddStrBuf( fparam->msgBG, idx, pos_p, *msgbuf );
      
      SCRCMD_WORK_SetFldMsgWinStream( work, (FLDMSGWIN_STREAM*)tmsg );
      
      VMCMD_SetWait( core, BallonWinMsgWait );
      return VMCMD_RESULT_SUSPEND;
    }
  }
}

