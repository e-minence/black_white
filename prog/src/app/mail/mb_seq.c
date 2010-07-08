//============================================================================================
/**
 * @file  mb_seq.c
 * @brief ���[���{�b�N�X��� ���C���V�[�P���X
 * @author  Hiroyuki Nakamura
 * @date  09.01.31
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "system/wipe.h"
#include "gamesystem/game_data.h"
#include "sound/pm_sndsys.h"
#include "poke_tool/pokeparty.h"
#include "item/item.h"
#include "savedata/myitem_savedata.h"

#include "app/pokelist.h"
#include "app/mailtool.h"

#include "mb_main.h"
#include "mb_seq.h"
#include "mb_bmp.h"
#include "mb_obj.h"
#include "mb_ui.h"
#include "mb_snd_def.h"


// �L���ɂ���ƉE���ƍ������P�t���[�����^�b�`����
#ifdef PM_DEBUG
//#define DEBUG_RAPID_PAGE
#endif

//============================================================================================
//  �萔��`
//============================================================================================

// �T�u�v���Z�X�^�C�v
enum {
  SUBPROC_TYPE_PL_PUT = 0,    // ��������
  SUBPROC_TYPE_PL_DEL,      // ����
  SUBPROC_TYPE_MAILWRITE,     // ���[���쐬
  SUBPROC_TYPE_PL_MW_END,     // ���[���쐬��
};

// �T�u�v���Z�X�f�[�^
typedef struct {
  pMailBoxFunc  call; // �Ăяo���֐�
  pMailBoxFunc  exit; // �I���֐�
  u32 rcvSeq;       // ���A�V�[�P���X
}SUBPROC_DATA;

// �͂��E������ID
enum {
  YESNO_ID_DELETE = 0,  // ���[��������
  YESNO_ID_POKEADD,   // �|�P�����Ɏ�������
};

// �͂��E����������
typedef struct {
  pMailBoxFunc  yes;
  pMailBoxFunc  no;
}YESNO_DATA;

// �g���K�[�E�F�C�g
enum {
  MSG_TRGWAIT_OFF = 0,
  MSG_TRGWAIT_ON,
};


//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================
static int MainSeq_Init( MAILBOX_SYS_WORK * syswk );
static int MainSeq_Release( MAILBOX_SYS_WORK * syswk );
static int MainSeq_Wipe( MAILBOX_SYS_WORK * syswk );
static int MainSeq_MsgWait( MAILBOX_SYS_WORK * syswk );
static int MainSeq_YesNo( MAILBOX_SYS_WORK * syswk );
static int MainSeq_ButtonAnm( MAILBOX_SYS_WORK * syswk );
static int MainSeq_SubProcCall( MAILBOX_SYS_WORK * syswk );
static int MainSeq_SubProcMain( MAILBOX_SYS_WORK * syswk );
static int MainSeq_Start( MAILBOX_SYS_WORK * syswk );
static int MainSeq_MailSelectMain( MAILBOX_SYS_WORK * syswk );
static int MainSeq_MailBoxEndSet( MAILBOX_SYS_WORK * syswk );
static int MainSeq_MailSelectPageChg( MAILBOX_SYS_WORK * syswk );
static int MainSeq_MailMenuMsgSet( MAILBOX_SYS_WORK * syswk );
static int MainSeq_MailMenuSet( MAILBOX_SYS_WORK * syswk );
static int MainSeq_MailMenuMain( MAILBOX_SYS_WORK * syswk );
static int MainSeq_MailReadMain( MAILBOX_SYS_WORK * syswk );
static int MainSeq_MailReadEnd( MAILBOX_SYS_WORK * syswk );
static int MainSeq_MailDelYesNoSet( MAILBOX_SYS_WORK * syswk );
static int MainSeq_MailPokeSetMsg( MAILBOX_SYS_WORK * syswk );
static int MainSeq_MailPokeSetYesNoSet( MAILBOX_SYS_WORK * syswk );
static int MainSeq_MailDelEnd( MAILBOX_SYS_WORK * syswk );
static int MainSeq_MailWriteExit( MAILBOX_SYS_WORK * syswk );
static int MainSeq_MailboxRetAnimeWait( MAILBOX_SYS_WORK *syswk );
static int MainSeq_MailboxEndAnimeWait( MAILBOX_SYS_WORK *syswk );

static int MainSeq_EraseMailPokeListExit( MAILBOX_SYS_WORK * syswk );
static int MainSeq_EraseMailPokeSetCancel( MAILBOX_SYS_WORK * syswk );
static int MainSeq_EraseMailPokeSetCancelEnd( MAILBOX_SYS_WORK * syswk );

static int FadeInSet( MAILBOX_SYS_WORK * syswk, int next );
static int FadeOutSet( MAILBOX_SYS_WORK * syswk, int next );
static int MsgWaitSet( MAILBOX_SYS_WORK * syswk, u32 wait, int next );
static int YesNoSet( MAILBOX_SYS_WORK * syswk, u32 id );
static int BgWinFrmButtonAnmSet( MAILBOX_SYS_WORK * syswk, u32 frmID, int next );
static int ObjButtonAnmSet( MAILBOX_SYS_WORK * syswk, u32 objID, int next );
static void AlphaSet( MAILBOX_APP_WORK * appwk, BOOL flg );
static void SubDispMailWrite( MAILBOX_SYS_WORK * syswk );
static void SubDispMailClear( MAILBOX_SYS_WORK * syswk );
static int MailPokeListCall( MAILBOX_SYS_WORK * syswk );
static int MailPutPokeListExit( MAILBOX_SYS_WORK * syswk );
static int EraseMailPokeListExit( MAILBOX_SYS_WORK * syswk );

static void PokeListWorkCreate( MAILBOX_SYS_WORK * syswk, u32 mode, u16 item, u16 pos );

static int MailDelYes( MAILBOX_SYS_WORK * syswk );
static int MailPokeAddYes( MAILBOX_SYS_WORK * syswk );
static int MailPokeAddNo( MAILBOX_SYS_WORK * syswk );

static void MailBagSet( MAILBOX_SYS_WORK * syswk );
static void SelMailDelete( MAILBOX_SYS_WORK * syswk );

static int MailWriteCall( MAILBOX_SYS_WORK * syswk );
static int MailWriteExit( MAILBOX_SYS_WORK * syswk );


static int MailWritePokeListEnd( MAILBOX_SYS_WORK * syswk );


//============================================================================================
//  �O���[�o���ϐ�
//============================================================================================

// ���C���V�[�P���X�e�[�u��
static const pMailBoxFunc MainSeqFunc[] = {
  MainSeq_Init,                       // MBSEQ_MAINSEQ_INIT = 0,       // ������
  MainSeq_Release,                    // MBSEQ_MAINSEQ_RELEASE,        // ���
  MainSeq_Wipe,                       // MBSEQ_MAINSEQ_WIPE,         // ���C�v
  MainSeq_MsgWait,                    // MBSEQ_MAINSEQ_MSG_WAIT,       // ���b�Z�[�W
  MainSeq_YesNo,                      // MBSEQ_MAINSEQ_YESNO,        // �͂��E������
  MainSeq_ButtonAnm,                  // MBSEQ_MAINSEQ_BUTTON_ANM,     // �{�^���A�j
  MainSeq_SubProcCall,                // MBSEQ_MAINSEQ_SUBPROC_CALL,     // �T�u�v��
  MainSeq_SubProcMain,                // MBSEQ_MAINSEQ_SUBPROC_MAIN,     // �T�u�v��
  MainSeq_Start,                      // MBSEQ_MAINSEQ_START,        // ���[���{�b�N
  MainSeq_MailSelectMain,             // MBSEQ_MAINSEQ_MAIL_SELECT_MAIN,   // ���[��
  MainSeq_MailBoxEndSet,              // MBSEQ_MAINSEQ_MAILBOX_END_SET,    // ���[��
  MainSeq_MailSelectPageChg,          // MBSEQ_MAINSEQ_MAIL_SELECT_PAGE_CHG, // ���[
  MainSeq_MailMenuMsgSet,             // MBSEQ_MAINSEQ_MAIL_MENU_MSG_SET,  // ���j��
  MainSeq_MailMenuSet,                // MBSEQ_MAINSEQ_MAIL_MENU_SET,    // ���j���[
  MainSeq_MailMenuMain,               // MBSEQ_MAINSEQ_MAIL_MENU_MAIN,   // ���j���[
  MainSeq_MailReadMain,               // MBSEQ_MAINSEQ_MAIL_READ_MAIN,   //�u���[��
  MainSeq_MailReadEnd,                // MBSEQ_MAINSEQ_MAIL_READ_END,    //�u���[��
  MainSeq_MailDelYesNoSet,            // MBSEQ_MAINSEQ_MAILDEL_YESNO_SET,    //�u��
  MainSeq_MailPokeSetMsg,             // MBSEQ_MAINSEQ_MAILPOKESET_MSG,      // ���[
  MainSeq_MailPokeSetYesNoSet,        // MBSEQ_MAINSEQ_MAILPOKESET_YESNO_SET,  //��
  MainSeq_EraseMailPokeListExit,      // MBSEQ_MAINSEQ_ERASEMAIL_POKELIST_EXIT,    
  MainSeq_EraseMailPokeSetCancel,     // MBSEQ_MAINSEQ_ERASEMAIL_POKESET_CANCEL,   
  MainSeq_EraseMailPokeSetCancelEnd,  // MBSEQ_MAINSEQ_ERASEMAIL_POKESET_CANCEL_END
  MainSeq_MailWriteExit,              // MBSEQ_MAINSEQ_MAILWRITE_END,        // ���[
  MainSeq_MailboxRetAnimeWait,         // MBSEQ_MAINSEQ_MAILBOX_RET_ANIME_WAIT
  MainSeq_MailboxEndAnimeWait,         // MBSEQ_MAINSEQ_MAILBOX_END_ANIME_WAIT
  MainSeq_MailDelEnd,                 // MBSEQ_MAINSEQ_MAILDEL_END,        //�u���[
};                                    // MBSEQ_MAINSEQ_END         // �I��
                                      
// �T�u�v���Z�X�f�[�^
static const SUBPROC_DATA SubProcFunc[] =
{
  { MailPokeListCall, MailPutPokeListExit, MBSEQ_MAINSEQ_INIT },              // ��������
  { MailPokeListCall, EraseMailPokeListExit, MBSEQ_MAINSEQ_ERASEMAIL_POKELIST_EXIT },   // ����
  { MailWriteCall, MailWriteExit, MBSEQ_MAINSEQ_MAILWRITE_END },              // ���[���쐬
  { MailPokeListCall, MailWritePokeListEnd, MBSEQ_MAINSEQ_INIT },             // ���[���쐬��
};

static const YESNO_DATA YesNoFunc[] =
{
  { MailDelYes, MainSeq_MailDelEnd },   // ���[��������
  { MailPokeAddYes, MailPokeAddNo },    // �|�P�����Ɏ�������
};


//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X����
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param seq   ���C���V�[�P���X
 *
 * @retval  "TRUE = ������"
 * @retval  "FALSE = �I��"
 */
//--------------------------------------------------------------------------------------------
BOOL MBSEQ_Main( MAILBOX_SYS_WORK * syswk, int * seq )
{
  *seq = MainSeqFunc[*seq]( syswk );

  if( *seq == MBSEQ_MAINSEQ_END ){
    return FALSE;
  }

  if( syswk->app != NULL ){
    int i;
    
    BGWINFRM_MoveMain( syswk->app->wfrm );
    MBOBJ_Main( syswk->app );

    if(syswk->app->printQue!=NULL){
      PRINTSYS_QUE_Main( syswk->app->printQue );
      for(i=0;i<MBMAIN_BMPWIN_ID_MAILMSG1;i++){
        PRINT_UTIL_Trans( &syswk->app->printUtil[i], syswk->app->printQue );
      }
    }
    if(syswk->app->pMsgTcblSys!=NULL){
      GFL_TCBL_Main( syswk->app->pMsgTcblSys );
    }
    if(syswk->app->pmsPrintque){
      int i;
      PMS_DRAW_Main( syswk->app->pms_draw_work );
      PRINTSYS_QUE_Main( syswk->app->pmsPrintque );
      for(i=MBMAIN_BMPWIN_ID_MAILMSG1;i<MBMAIN_BMPWIN_ID_MAX;i++){
        PRINT_UTIL_Trans( &syswk->app->printUtil[i], syswk->app->printQue );
      }
    }
  }

  return TRUE;
}



//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Init( MAILBOX_SYS_WORK * syswk )
{
  // ���荞�ݒ�~
//    sys_VBlankFuncChange( NULL, NULL );
  if(syswk->vintr_tcb!=NULL){
      GFL_TCB_DeleteTask( syswk->vintr_tcb );
      syswk->vintr_tcb = NULL;
  }
  // �\��������
  GFL_DISP_GX_SetVisibleControlDirect( 0 );
  GFL_DISP_GXS_SetVisibleControlDirect( 0 );
  // �u�����h������
  G2_BlendNone();
  G2S_BlendNone();

  // �T�u��ʂ����C����
  GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );

  // �A�v���q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MAILBOX_APP, 0x60000 );
  syswk->app = GFL_HEAP_AllocMemory( HEAPID_MAILBOX_APP, sizeof(MAILBOX_APP_WORK) );
  MI_CpuClearFast( syswk->app, sizeof(MAILBOX_APP_WORK) );

  MBMAIN_MailDataInit( syswk );

  MBMAIN_VramInit();

  MBMAIN_BgInit( syswk );
  MBMAIN_BgGraphicLoad( syswk );
  MBMAIN_WindowLoad( syswk );
  MBBMP_Init( syswk );
  MBMAIN_BgWinFrmInit( syswk );

  MBOBJ_Init( syswk );

  MBMAIN_PmsDrawInit( syswk );

//  MBMAIN_YesNoWinInit( syswk );

  MBUI_CursorMoveInit( syswk );

  MBMAIN_MsgInit( syswk->app );

  MBBMP_DefaultStrSet( syswk );
//  MBBMP_PagePut( syswk );   // �y�[�W�\��
  MBMAIN_MailButtonInit( syswk );
  MBMAIN_SelBoxInit( syswk );

//  sys_VBlankFuncChange( MBMAIN_VBlank, syswk );
  syswk->vintr_tcb = GFUser_VIntr_CreateTCB( MBMAIN_VBlank, syswk, 1 );

  // �ʐM�A�C�R���\��
  GFL_NET_ReloadIconTopOrBottom( FALSE, HEAPID_MAILBOX_APP );


  return syswk->next_seq;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Release( MAILBOX_SYS_WORK * syswk )
{
//  sys_VBlankFuncChange( NULL, NULL );
  if(syswk->vintr_tcb!=NULL){
      GFL_TCB_DeleteTask( syswk->vintr_tcb );
      syswk->vintr_tcb = NULL;
  }

  MBMAIN_SelBoxExit( syswk );

  MBMAIN_MsgExit( syswk->app );

  MBUI_CursorMoveExit( syswk );

//  MBMAIN_YesNoWinExit( syswk );

  MBMAIN_PmsDrawExit( syswk );

  MBOBJ_Exit( syswk );

  MBMAIN_BgWinFrmExit( syswk );
  MBBMP_Exit( syswk );
  MBMAIN_BgExit( syswk );

  MBMAIN_MailDataExit( syswk );


  G2_BlendNone();
  G2S_BlendNone();

  GFL_DISP_GX_SetVisibleControlDirect( 0 );
  GFL_DISP_GXS_SetVisibleControlDirect( 0 );

  GFL_HEAP_FreeMemory( syswk->app );
  syswk->app = NULL;

  GFL_HEAP_DeleteHeap( HEAPID_MAILBOX_APP );

  return syswk->next_seq;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F���C�v
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Wipe( MAILBOX_SYS_WORK * syswk )
{
  if( WIPE_SYS_EndCheck() == TRUE ){
    return syswk->app->wipe_seq;
  }
  return MBSEQ_MAINSEQ_WIPE;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F���b�Z�[�W�I���҂�
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MsgWait( MAILBOX_SYS_WORK * syswk )
{
  if(PRINTSYS_PrintStreamGetState( syswk->app->printStream )==PRINTSTREAM_STATE_DONE)
  {
    // �E�F�C�g�Ȃ�
    if( syswk->app->msgTrg == MSG_TRGWAIT_OFF ){
      PRINTSYS_PrintStreamDelete( syswk->app->printStream );
      return syswk->next_seq;
    }

    // �^�b�`�҂� ���@�{�^���҂�
    if( GFL_UI_TP_GetTrg() == TRUE 
    || GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE))
    {
      PRINTSYS_PrintStreamDelete( syswk->app->printStream );
      PMSND_PlaySE( SND_MB_SELECT );
      return syswk->next_seq;
    }
    
  }

  return MBSEQ_MAINSEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�͂��E������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_YesNo( MAILBOX_SYS_WORK * syswk )
{
  APP_TASKMENU_UpdateMenu(syswk->app->menuwork);
  if( APP_TASKMENU_IsFinish( syswk->app->menuwork )==FALSE){
    return MBSEQ_MAINSEQ_YESNO;
  }
  switch( APP_TASKMENU_GetCursorPos(syswk->app->menuwork) ){
  case 0: // �͂�
    APP_TASKMENU_CloseMenu( syswk->app->menuwork );
    return YesNoFunc[syswk->app->ynID].yes( syswk );
    break;
  case 1: // ������
    APP_TASKMENU_CloseMenu( syswk->app->menuwork );
    return YesNoFunc[syswk->app->ynID].no( syswk );
    break;
  }


  return MBSEQ_MAINSEQ_YESNO;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�^���A�j��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ButtonAnm( MAILBOX_SYS_WORK * syswk )
{
  if( MBMAIN_ButtonAnmMain( syswk ) == FALSE ){
    return syswk->next_seq;
  }
  return MBSEQ_MAINSEQ_BUTTON_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�T�u�v���Z�X�Ăяo��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_SubProcCall( MAILBOX_SYS_WORK * syswk )
{
  SubProcFunc[syswk->subProcType].call( syswk );
  return MBSEQ_MAINSEQ_SUBPROC_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�T�u�v���Z�X���C��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_SubProcMain( MAILBOX_SYS_WORK * syswk )
{
  //���[�J��PROC�̍X�V����
  GFL_PROC_MAIN_STATUS proc_status = GFL_PROC_LOCAL_Main( syswk->mbProcSys );

  // �I���҂�
  if(proc_status == GFL_PROC_MAIN_VALID){
    return MBSEQ_MAINSEQ_SUBPROC_MAIN;
  }

  syswk->next_seq = SubProcFunc[syswk->subProcType].exit( syswk );
  return SubProcFunc[syswk->subProcType].rcvSeq;

  
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F���[���{�b�N�X��ʊJ�n
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Start( MAILBOX_SYS_WORK * syswk )
{
  return FadeInSet( syswk, MBSEQ_MAINSEQ_MAIL_SELECT_MAIN );
}


#ifdef DEBUG_RAPID_PAGE
static int debug_flag=0;
#endif
//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F���[���I�����C��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MailSelectMain( MAILBOX_SYS_WORK * syswk )
{
  u32 ret;
  u32 touch;
  // printQue���󂭂܂ł͑���֎~
  if(PRINTSYS_QUE_IsFinished( syswk->app->printQue )==FALSE){
    return MBSEQ_MAINSEQ_MAIL_SELECT_MAIN;
  }
  
  touch = MBUI_MailboxTouchButtonCheck( syswk );

#ifdef DEBUG_RAPID_PAGE
  if(GFL_UI_KEY_GetCont()&PAD_BUTTON_R){
    debug_flag ^=1;
    if(debug_flag==0){
      touch=0;
    }else{
      touch=1;
    }
  }else{
    debug_flag=0;
  }
#endif
  
  // �^�b�`�̂݃{�^���`�F�b�N
  switch( touch ){
  case 0:   // �����
    if( syswk->sel_page != 0 ){
      syswk->sel_page--;
      PMSND_PlaySE( SND_MB_SELECT );
      return ObjButtonAnmSet( syswk, MBMAIN_OBJ_ARROW_L, MBSEQ_MAINSEQ_MAIL_SELECT_PAGE_CHG );
    }
    return MBSEQ_MAINSEQ_MAIL_SELECT_MAIN;
    break;
  case 1:   // �E���
    if( syswk->sel_page != syswk->app->page_max ){
      syswk->sel_page++;
      PMSND_PlaySE( SND_MB_SELECT );
      return ObjButtonAnmSet( syswk, MBMAIN_OBJ_ARROW_R, MBSEQ_MAINSEQ_MAIL_SELECT_PAGE_CHG );
    }
    return MBSEQ_MAINSEQ_MAIL_SELECT_MAIN;
    break;
  case 2:   // ��߂�
    PMSND_PlaySE( SND_MB_CANCEL );
    MBOBJ_AnmSet( syswk->app, MBMAIN_OBJ_RET_BTN, 9 );
    syswk->next_seq = MBSEQ_MAINSEQ_MAILBOX_END_SET;
    return MBSEQ_MAINSEQ_MAILBOX_RET_ANIME_WAIT;
    break;
  case 3:   // �u�~�v
    PMSND_PlaySE( SND_MB_END );
    MBOBJ_AnmSet( syswk->app, MBMAIN_OBJ_END_BTN, 8 );
    syswk->next_seq = MBSEQ_MAINSEQ_MAILBOX_END_SET;
    return MBSEQ_MAINSEQ_MAILBOX_END_ANIME_WAIT;
    break;
  }

  ret = CURSORMOVE_MainCont( syswk->app->cmwk );

  switch( ret ){
  case MBUI_MAILSEL_MAIL1:  // 00: ���[���P
  case MBUI_MAILSEL_MAIL2:  // 01: ���[���Q
  case MBUI_MAILSEL_MAIL3:  // 02: ���[���R
  case MBUI_MAILSEL_MAIL4:  // 03: ���[���S
  case MBUI_MAILSEL_MAIL5:  // 04: ���[���T
  case MBUI_MAILSEL_MAIL6:  // 05: ���[���U
  case MBUI_MAILSEL_MAIL7:  // 06: ���[���V
  case MBUI_MAILSEL_MAIL8:  // 07: ���[���W
  case MBUI_MAILSEL_MAIL9:  // 08: ���[���X
  case MBUI_MAILSEL_MAIL10: // 09: ���[���P�O
    if( syswk->app->mail_list[syswk->sel_page*MBMAIN_MAILLIST_MAX+ret] != MBMAIN_MAILLIST_NULL ){
      syswk->sel_pos = ret;
      syswk->lst_pos = syswk->app->mail_list[ syswk->sel_page * MBMAIN_MAILLIST_MAX + ret ];
      PMSND_PlaySE( SND_MB_DECIDE );
      return BgWinFrmButtonAnmSet( syswk, ret,  MBSEQ_MAINSEQ_MAIL_MENU_MSG_SET );
    }
    break;

  case CURSORMOVE_CANCEL:   // �L�����Z��
    PMSND_PlaySE( SND_MB_CANCEL );
    MBOBJ_AnmSet( syswk->app, MBMAIN_OBJ_RET_BTN, 9 );
    syswk->next_seq = MBSEQ_MAINSEQ_MAILBOX_END_SET;
    return MBSEQ_MAINSEQ_MAILBOX_RET_ANIME_WAIT;

  case CURSORMOVE_NO_MOVE_RIGHT:     // �J�[�\���͓������E�������ꂽ
    {
      u8  pos = CURSORMOVE_PosGet( syswk->app->cmwk );
      if( syswk->sel_page != syswk->app->page_max ){
        syswk->sel_page++;
        PMSND_PlaySE( SND_MB_SELECT );
        return ObjButtonAnmSet( syswk, MBMAIN_OBJ_ARROW_R, MBSEQ_MAINSEQ_MAIL_SELECT_PAGE_CHG );
      }
    }
    break;
  case CURSORMOVE_NO_MOVE_LEFT:     // �J�[�\���͓��������������ꂽ
    {
      u8  pos = CURSORMOVE_PosGet( syswk->app->cmwk );
      if( syswk->sel_page != 0 ){
        syswk->sel_page--;
        PMSND_PlaySE( SND_MB_SELECT );
        return ObjButtonAnmSet( syswk, MBMAIN_OBJ_ARROW_L, MBSEQ_MAINSEQ_MAIL_SELECT_PAGE_CHG );
      }
    }
    break;

  case CURSORMOVE_CURSOR_ON:    // �J�[�\���\��
    break;

  case CURSORMOVE_CURSOR_MOVE:  // �ړ�
    PMSND_PlaySE( SND_MB_SELECT );
    break;
  default:
    // X�{�^���`�F�b�N
    if(MBUI_EndButtonCheck()){
      PMSND_PlaySE( SND_MB_END );
      MBOBJ_AnmSet( syswk->app, MBMAIN_OBJ_END_BTN, 8 );
      syswk->next_seq = MBSEQ_MAINSEQ_MAILBOX_END_SET;
      return MBSEQ_MAINSEQ_MAILBOX_END_ANIME_WAIT;
    }
  }

  

  return MBSEQ_MAINSEQ_MAIL_SELECT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F���[���{�b�N�X�I���ݒ�
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MailBoxEndSet( MAILBOX_SYS_WORK * syswk )
{
  syswk->next_seq = MBSEQ_MAINSEQ_END;
  return FadeOutSet( syswk, MBSEQ_MAINSEQ_RELEASE );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F���[���I���y�[�W�؂�ւ�
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MailSelectPageChg( MAILBOX_SYS_WORK * syswk )
{
  if( syswk->sel_page == 0 ){
    MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_ARROW_L, FALSE );
    MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_ARROW_R, TRUE );
  }else{
    MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_ARROW_L, TRUE );
    MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_ARROW_R, FALSE );
  }

//  MBBMP_PagePut( syswk );   // �y�[�W�\��
  MBMAIN_MailButtonInit( syswk );
  return MBSEQ_MAINSEQ_MAIL_SELECT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F���j���[���b�Z�[�W�Z�b�g
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MailMenuMsgSet( MAILBOX_SYS_WORK * syswk )
{
  MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_CURSOR, FALSE );
  AlphaSet( syswk->app, TRUE );
  MBMAIN_SelMailTabPut( syswk );
  MBBMP_TalkWinPut( syswk );
  MBBMP_SelectMsgPut( syswk );
  return MsgWaitSet( syswk, MSG_TRGWAIT_OFF, MBSEQ_MAINSEQ_MAIL_MENU_SET );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F���j���[�Z�b�g
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MailMenuSet( MAILBOX_SYS_WORK * syswk )
{
  MBMAIN_SelBoxPut( syswk );
  return MBSEQ_MAINSEQ_MAIL_MENU_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F���j���[���C��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MailMenuMain( MAILBOX_SYS_WORK * syswk )
{
  APP_TASKMENU_UpdateMenu(syswk->app->menuwork);
  if( APP_TASKMENU_IsFinish( syswk->app->menuwork )==FALSE){
    return MBSEQ_MAINSEQ_MAIL_MENU_MAIN;
  }
//  switch( SelectBoxMain(syswk->app->sbwk) ){
  switch( APP_TASKMENU_GetCursorPos(syswk->app->menuwork) ){
  case MBMAIN_MENU_READ:    // ���[����ǂ�
    MBMAIN_SelBoxDel( syswk );
    MBBMP_TalkWinDel( syswk );
    MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_CURSOR, FALSE );
    MBBMP_ClosePut( syswk->app );
    MBOBJ_MailReadCurMove( syswk->app );
    SubDispMailWrite( syswk );
    MBOBJ_AnmSet( syswk->app, MBMAIN_OBJ_END_BTN, 14 ); //END�{�^�����Â�
    return MBSEQ_MAINSEQ_MAIL_READ_MAIN;

  case MBMAIN_MENU_DELETE:  // ���[��������
    MBMAIN_SelBoxDel( syswk );
    MBBMP_DeleteMsgPut( syswk, MBBMP_DELMSG_ID_CHECK );
    return MsgWaitSet( syswk, MSG_TRGWAIT_OFF, MBSEQ_MAINSEQ_MAILDEL_YESNO_SET );

  case MBMAIN_MENU_POKESET: // ��������
    MBMAIN_SelBoxDel( syswk );
    {
      MAIL_DATA * md = syswk->app->mail[ syswk->lst_pos ];
//      OS_Printf("listpos = %d\n", syswk->lst_pos);
      PokeListWorkCreate( syswk, PL_MODE_MAILBOX, ITEM_MailNumGet(MailData_GetDesignNo(md)), 
                          syswk->lst_pos);
    }
    syswk->subProcType = SUBPROC_TYPE_PL_PUT;
    syswk->next_seq = MBSEQ_MAINSEQ_SUBPROC_CALL;
    return FadeOutSet( syswk, MBSEQ_MAINSEQ_RELEASE );

  case MBMAIN_MENU_CANCEL:  // ��߂�
  case BMPMENU_CANCEL:
    MBMAIN_SelBoxDel( syswk );
    MBBMP_TalkWinDel( syswk );
    BGWINFRM_FrameOff( syswk->app->wfrm, MBMAIN_BGWF_SELMAIL );
    AlphaSet( syswk->app, FALSE );
    MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_CURSOR, TRUE );
    return MBSEQ_MAINSEQ_MAIL_SELECT_MAIN;

  case BMPMENU_NULL:
    break;
  }

  return MBSEQ_MAINSEQ_MAIL_MENU_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u���[������ށv���C��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MailReadMain( MAILBOX_SYS_WORK * syswk )
{
  if( MBUI_MailReadCheck() == TRUE ){
    PMSND_PlaySE( SND_MB_CANCEL );
    MBOBJ_AnmSet( syswk->app, MBMAIN_OBJ_RET_BTN, 9 );
    syswk->next_seq = MBSEQ_MAINSEQ_MAIL_READ_END;
    return MBSEQ_MAINSEQ_MAILBOX_RET_ANIME_WAIT;

  }
  return MBSEQ_MAINSEQ_MAIL_READ_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u���[������ށv�I��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MailReadEnd( MAILBOX_SYS_WORK * syswk )
{
  SubDispMailClear( syswk );
  MBUI_MailSelCurMove( syswk, CURSORMOVE_PosGet(syswk->app->cmwk) );
  MBBMP_ReturnPut( syswk->app );
  BGWINFRM_FrameOff( syswk->app->wfrm, MBMAIN_BGWF_SELMAIL );
  MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_CURSOR, TRUE );
  AlphaSet( syswk->app, FALSE );
  return MBSEQ_MAINSEQ_MAIL_SELECT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u���[���������v�͂��E�������Z�b�g
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MailDelYesNoSet( MAILBOX_SYS_WORK * syswk )
{
  return YesNoSet( syswk, YESNO_ID_DELETE );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P�����ɂ�������v���b�Z�[�W�\��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MailPokeSetMsg( MAILBOX_SYS_WORK * syswk )
{
  MBBMP_DeleteMsgPut( syswk, MBBMP_DELMSG_ID_POKE );
  return MsgWaitSet( syswk, MSG_TRGWAIT_OFF, MBSEQ_MAINSEQ_MAILPOKESET_YESNO_SET );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P�����ɂ�������v�͂��E�������Z�b�g
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MailPokeSetYesNoSet( MAILBOX_SYS_WORK * syswk )
{
  return YesNoSet( syswk, YESNO_ID_POKEADD );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�������[������������|�P������I����
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_EraseMailPokeListExit( MAILBOX_SYS_WORK * syswk )
{
  if( syswk->ret_mode != PL_RET_MAILSET ){
//  if( syswk->ret_poke > PL_SEL_POS_POKE6 ){
    syswk->next_seq = MBSEQ_MAINSEQ_ERASEMAIL_POKESET_CANCEL;
    return MBSEQ_MAINSEQ_INIT;
  }

  syswk->subProcType = SUBPROC_TYPE_MAILWRITE;
  return MBSEQ_MAINSEQ_SUBPROC_CALL;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�������[������������|�P�����I�����L�����Z��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_EraseMailPokeSetCancel( MAILBOX_SYS_WORK * syswk )
{
  MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_CURSOR, FALSE );
  AlphaSet( syswk->app, TRUE );
  return FadeInSet( syswk, MBSEQ_MAINSEQ_ERASEMAIL_POKESET_CANCEL_END );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�������[������������|�P�����I�����L�����Z����̃��b�Z�[�W�\��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_EraseMailPokeSetCancelEnd( MAILBOX_SYS_WORK * syswk )
{
  MBBMP_TalkWinPut( syswk );
  return MailPokeAddNo( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�������[���ɕ�������������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MailWriteExit( MAILBOX_SYS_WORK * syswk )
{
  if( syswk->mw_ret == FALSE ){
    syswk->next_seq = MBSEQ_MAINSEQ_ERASEMAIL_POKESET_CANCEL;
    return MBSEQ_MAINSEQ_INIT;
  }

  PokeListWorkCreate( syswk, PL_MODE_MAILSET, syswk->ret_item, syswk->ret_poke );
  syswk->subProcType = SUBPROC_TYPE_PL_MW_END;
  return MBSEQ_MAINSEQ_SUBPROC_CALL;
}

//----------------------------------------------------------------------------------
/**
 * @brief �߂�A�C�R���̃A�j���҂�
 *
 * @param   syswk   
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int MainSeq_MailboxRetAnimeWait( MAILBOX_SYS_WORK *syswk )
{
  if(GFL_CLACT_WK_CheckAnmActive(syswk->app->clwk[MBMAIN_OBJ_RET_BTN])==FALSE ){
    syswk->dat->retMode = FALSE;
    MBOBJ_AnmSet( syswk->app, MBMAIN_OBJ_END_BTN, 0 ); //END�{�^���𖾂邭
    return syswk->next_seq;
  }
  return MBSEQ_MAINSEQ_MAILBOX_RET_ANIME_WAIT;
}

//----------------------------------------------------------------------------------
/**
 * @brief �I���A�C�R���̃A�j���҂�
 *
 * @param   syswk   
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int MainSeq_MailboxEndAnimeWait( MAILBOX_SYS_WORK *syswk )
{
  if(GFL_CLACT_WK_CheckAnmActive(syswk->app->clwk[MBMAIN_OBJ_END_BTN])==FALSE ){
    // �p�\�R�����j���[�ɂ͖߂炸���ڃt�B�[���h�ɖ߂�
    syswk->dat->retMode = TRUE;
    return syswk->next_seq;
  }
  return MBSEQ_MAINSEQ_MAILBOX_END_ANIME_WAIT;

}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u���[���������v�I��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MailDelEnd( MAILBOX_SYS_WORK * syswk )
{
  MBBMP_TalkWinDel( syswk );
  BGWINFRM_FrameOff( syswk->app->wfrm, MBMAIN_BGWF_SELMAIL );
  MBUI_MailSelCurMove( syswk, CURSORMOVE_PosGet(syswk->app->cmwk) );
  MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_CURSOR, TRUE );
  AlphaSet( syswk->app, FALSE );
  return MBSEQ_MAINSEQ_MAIL_SELECT_MAIN;
}


//============================================================================================
//  �T�u���[�`��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h�C���Z�b�g�i���C�v�j
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param next  �t�F�[�h��̃V�[�P���X
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int FadeInSet( MAILBOX_SYS_WORK * syswk, int next )
{
  WIPE_SYS_Start(
    WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
    WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_MAILBOX_APP );

  syswk->app->wipe_seq = next;

  return MBSEQ_MAINSEQ_WIPE;
}

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h�C���Z�b�g�i���C�v�j
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param next  �t�F�[�h��̃V�[�P���X
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int FadeOutSet( MAILBOX_SYS_WORK * syswk, int next )
{
  WIPE_SYS_Start(
    WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
    WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_MAILBOX_APP );

  syswk->app->wipe_seq = next;

  return MBSEQ_MAINSEQ_WIPE;
}

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�\���Z�b�g
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param wait  �g���K�[�E�F�C�g
 * @param next  �\����̃V�[�P���X
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MsgWaitSet( MAILBOX_SYS_WORK * syswk, u32 wait, int next )
{
  syswk->app->msgTrg = wait;
  syswk->next_seq = next;
  return MBSEQ_MAINSEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������Z�b�g
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param id    �͂��E�������h�c
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int YesNoSet( MAILBOX_SYS_WORK * syswk, u32 id )
{
  syswk->app->ynID = id;
  MBMAIN_YesNoWinSet( syswk );
  return MBSEQ_MAINSEQ_YESNO;
}

//--------------------------------------------------------------------------------------------
/**
 * BGWINFRM�̃{�^���A�j���Z�b�g
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param frmID BGWINFRM�̂h�c
 * @param next  �A�j����̃V�[�P���X
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BgWinFrmButtonAnmSet( MAILBOX_SYS_WORK * syswk, u32 frmID, int next )
{
  BUTTON_ANM_WORK * bawk;
  u16 sx, sy;
  s8  px, py;
  
  BGWINFRM_PosGet( syswk->app->wfrm, frmID, &px, &py );
  BGWINFRM_SizeGet( syswk->app->wfrm, frmID, &sx, &sy );
  
  bawk = &syswk->app->bawk;

  bawk->btn_mode = MBMAIN_BTN_ANM_MODE_BG;
  bawk->btn_id   = MBMAIN_BGF_BTN_M;
  bawk->btn_pal1 = 2;
  bawk->btn_pal2 = 1;
  bawk->btn_seq  = 0;
  bawk->btn_cnt  = 0;
  bawk->btn_px   = px;
  bawk->btn_py   = py;
  bawk->btn_sx   = sx;
  bawk->btn_sy   = sy;

  syswk->next_seq = next;

  return MBSEQ_MAINSEQ_BUTTON_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * OBJ�̃{�^���A�j���Z�b�g
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param objID OBJ ID
 * @param next  �A�j����̃V�[�P���X
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int ObjButtonAnmSet( MAILBOX_SYS_WORK * syswk, u32 objID, int next )
{
  BUTTON_ANM_WORK * bawk = &syswk->app->bawk;

  bawk->btn_mode = MBMAIN_BTN_ANM_MODE_OBJ;
  bawk->btn_id   = objID;
  bawk->btn_pal1 = 1;
  bawk->btn_pal2 = 0;
  bawk->btn_seq  = 0;
  bawk->btn_cnt  = 0;

  syswk->next_seq = next;

  return MBSEQ_MAINSEQ_BUTTON_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * �������ݒ�
 *
 * @param flg   TRUE = ������, FALSE = �ʏ�
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void AlphaSet( MAILBOX_APP_WORK * appwk, BOOL flg )
{
  if( flg == TRUE ){
    MBOBJ_BlendModeSet( appwk, MBMAIN_OBJ_ARROW_L, TRUE );
    MBOBJ_BlendModeSet( appwk, MBMAIN_OBJ_ARROW_R, TRUE );
    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_OBJ, 
                      GX_BLEND_PLANEMASK_BG3, 4, 12 );
  }else{
    MBOBJ_BlendModeSet( appwk, MBMAIN_OBJ_ARROW_L, FALSE );
    MBOBJ_BlendModeSet( appwk, MBMAIN_OBJ_ARROW_R, FALSE );
    G2_BlendNone();
  }
}

//--------------------------------------------------------------------------------------------
/**
 * ���[���\��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void SubDispMailWrite( MAILBOX_SYS_WORK * syswk )
{
  MBMAIN_MailGraphcLoad( syswk );
  MBBMP_MailMesPut( syswk );
//  MBOBJ_PokeIconPut( syswk );
}

#define MAIL_PMS_NUM  ( 4 )
//--------------------------------------------------------------------------------------------
/**
 * ���[����\��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void SubDispMailClear( MAILBOX_SYS_WORK * syswk )
{
  int i;
  PMS_DATA * pms;

  // �ȈՉ�b�`����s���Ă�����N���A
  for(i=0;i<MAIL_PMS_NUM-1;i++){
    pms = MailData_GetMsgByIndex( syswk->app->mail[syswk->lst_pos], i );
    if(PMSDAT_IsEnabled(pms)  ){
      PMS_DRAW_Clear( syswk->app->pms_draw_work, i, FALSE );
    }
  }
  // �ȈՒP��`����s���Ă�����N���A
  if(syswk->app->tmpPms.word[0]!=0){
    PMS_DRAW_Clear( syswk->app->pms_draw_work, i, FALSE );
  }
  
  
  GFL_BG_ClearScreen( MBMAIN_BGF_MAILMES_S );
  GFL_BG_ClearScreen( MBMAIN_BGF_MAIL_S );
}

//--------------------------------------------------------------------------------------------
/**
 * �I�����[�����o�b�O�֓����
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void MailBagSet( MAILBOX_SYS_WORK * syswk )
{
  MYITEM    *myitem;
  MAIL_DATA *mail;
  
  myitem = GAMEDATA_GetMyItem( syswk->dat->gamedata );
  mail   = syswk->app->mail[ syswk->lst_pos ];

  syswk->ret_item = ITEM_MailNumGet( MailData_GetDesignNo(mail) );

  if( MYITEM_AddItem( myitem, syswk->ret_item, 1, HEAPID_MAILBOX_APP ) == TRUE ){
    syswk->check_item = 0;
  }else{
    syswk->check_item = 1;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �I�����[��������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void SelMailDelete( MAILBOX_SYS_WORK * syswk )
{
  MAIL_DelMailData( syswk->sv_mail, MAILBLOCK_PASOCOM, syswk->lst_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������X�g�̃��[�N�쐬
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param mode  �Ăяo�����[�h
 * @param item  ��������A�C�e��
 * @param pos   �����ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeListWorkCreate( MAILBOX_SYS_WORK * syswk, u32 mode, u16 item, u16 pos )
{
  PLIST_DATA * pld = GFL_HEAP_AllocClearMemory( HEAPID_MAILBOX_SYS, sizeof(PLIST_DATA) );

  pld->pp        = GAMEDATA_GetMyPokemon( syswk->dat->gamedata );
  pld->myitem    = GAMEDATA_GetMyItem( syswk->dat->gamedata );
  pld->mailblock = syswk->sv_mail;
  pld->type      = PL_TYPE_SINGLE;
  pld->mode      = mode;
  pld->item      = item;
  pld->mail_pos  = pos;
  pld->zkn       = GAMEDATA_GetZukanSave(syswk->dat->gamedata);   // �͂����񂾂܃M���e�B�i�p

  syswk->subProcWork = pld;
}
FS_EXTERN_OVERLAY(pokelist);

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������X�g�Ăяo��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  0
 */
//--------------------------------------------------------------------------------------------
static int MailPokeListCall( MAILBOX_SYS_WORK * syswk )
{
  PLIST_DATA * pld = (PLIST_DATA*)syswk->subProcWork;

//  OS_Printf("�������������|�P������%d\n", syswk->ret_poke);

  // �쐬�������[����n���|�P�����̎w��
  pld->ret_sel = syswk->ret_poke;
  
  GFL_PROC_LOCAL_CallProc( syswk->mbProcSys, FS_OVERLAY_ID(pokelist), &PokeList_ProcData, syswk->subProcWork );
  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * �u���[������������v�̃|�P�������X�g�I����̏���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̎��̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MailPutPokeListExit( MAILBOX_SYS_WORK * syswk )
{
  PLIST_DATA * pld;
  POKEMON_PARAM * pp;

  // �Ăяo�����[�N�폜
  GFL_HEAP_FreeMemory( syswk->subProcWork );

  return MBSEQ_MAINSEQ_START;
}

//--------------------------------------------------------------------------------------------
/**
 * �u���[���������v�̃|�P�������X�g�I����̏���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  0
 */
//--------------------------------------------------------------------------------------------
static int EraseMailPokeListExit( MAILBOX_SYS_WORK * syswk )
{
  PLIST_DATA * pld;

  pld = syswk->subProcWork;

  syswk->ret_poke = pld->ret_sel;
  syswk->ret_mode = pld->ret_mode;

//  OS_Printf("�I�������|�P������%d\n", syswk->ret_poke);

  GFL_HEAP_FreeMemory( syswk->subProcWork );

  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * �u���[���������v�Ń��[������������̃|�P�������X�g�I����̏���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̎��̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MailWritePokeListEnd( MAILBOX_SYS_WORK * syswk )
{
  GFL_HEAP_FreeMemory( syswk->subProcWork );
  return MBSEQ_MAINSEQ_START;
}

//--------------------------------------------------------------------------------------------
/**
 * ���[���쐬��ʌĂяo��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  0
 */
//--------------------------------------------------------------------------------------------
static int MailWriteCall( MAILBOX_SYS_WORK * syswk )
{
  MAIL_PARAM * prm;

  prm = MAILSYS_GetWorkCreate(
      syswk->dat->gamedata,
      MAILBLOCK_TEMOTI,
      syswk->ret_poke,
      ITEM_GetMailDesign(syswk->ret_item),
      HEAPID_MAILBOX_SYS );

//  prm->pKeytouch = syswk->dat->kt_status;

  syswk->subProcWork = prm;

  {
    // �I�[�o�[���C�v���Z�X��`�f�[�^
    static const GFL_PROC_DATA MailSysProcData = {
      MailSysProc_Init,
      MailSysProc_Main,
      MailSysProc_End,
    };

    GFL_PROC_LOCAL_CallProc( syswk->mbProcSys, NO_OVERLAY_ID, &MailSysProcData, syswk->subProcWork );
  }

  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ���[���쐬��ʏI����
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  0
 */
//--------------------------------------------------------------------------------------------
static int MailWriteExit( MAILBOX_SYS_WORK * syswk )
{
  syswk->mw_ret = MailSys_IsDataCreate( syswk->subProcWork );

  if( syswk->mw_ret == TRUE ){
    POKEMON_PARAM * pp;
    MYITEM * myitem;
    
    pp = PokeParty_GetMemberPointer(
        GAMEDATA_GetMyPokemon(syswk->dat->gamedata),
        syswk->ret_poke );
    MailSys_PushDataToSavePoke( syswk->subProcWork, pp );

    myitem = GAMEDATA_GetMyItem( syswk->dat->gamedata );
    MYITEM_SubItem( myitem, syswk->ret_item, 1, HEAPID_MAILBOX_SYS );
  }

  MailSys_ReleaseCallWork( syswk->subProcWork );

  return 0;

}

//--------------------------------------------------------------------------------------------
/**
 * �u���[���������v�Łu�͂��v��I�������Ƃ�
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MailDelYes( MAILBOX_SYS_WORK * syswk )
{
  MailBagSet( syswk );    // �o�b�O��
  SelMailDelete( syswk );   // ���[���폜

  // ��ʍX�V
  MBMAIN_MailDataExit( syswk );
  MBMAIN_MailDataInit( syswk );
  if( syswk->app->mail_max <= MBMAIN_MAILLIST_MAX ){
    MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_ARROW_L, FALSE );
    MBOBJ_Vanish( syswk->app, MBMAIN_OBJ_ARROW_R, FALSE );
  }
  if( syswk->sel_page > syswk->app->page_max ){
    syswk->sel_page = syswk->app->page_max;
    syswk->sel_pos  = 0;
  }
//  MBBMP_PagePut( syswk );   // �y�[�W�\��
  MBMAIN_MailButtonInit( syswk );
  BGWINFRM_FrameOff( syswk->app->wfrm, MBMAIN_BGWF_SELMAIL );

  MBBMP_DeleteMsgPut( syswk, MBBMP_DELMSG_ID_ERASE );
  return MsgWaitSet( syswk, MSG_TRGWAIT_ON, MBSEQ_MAINSEQ_MAILPOKESET_MSG );
}

//--------------------------------------------------------------------------------------------
/**
 * �u���[�����������܂����v�Łu�͂��v��I�������Ƃ�
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MailPokeAddYes( MAILBOX_SYS_WORK * syswk )
{
  PokeListWorkCreate( syswk, PL_MODE_ITEMSET, syswk->ret_item, 0 );

  syswk->subProcType = SUBPROC_TYPE_PL_DEL;
  syswk->next_seq = MBSEQ_MAINSEQ_SUBPROC_CALL;
  return FadeOutSet( syswk, MBSEQ_MAINSEQ_RELEASE );
}

//--------------------------------------------------------------------------------------------
/**
 * �u���[�����������܂����v�Łu�������v��I�������Ƃ�
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MailPokeAddNo( MAILBOX_SYS_WORK * syswk )
{
  if( syswk->check_item == 0 ){
    MBBMP_DeleteMsgPut( syswk, MBBMP_DELMSG_ID_BAG );
  }else{
    MBBMP_DeleteMsgPut( syswk, MBBMP_DELMSG_ID_DELETE );
  }
  return MsgWaitSet( syswk, MSG_TRGWAIT_ON, MBSEQ_MAINSEQ_MAILDEL_END );
}
