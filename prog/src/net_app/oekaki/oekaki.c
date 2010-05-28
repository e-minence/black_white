//============================================================================================
/**
 * @file  oekaki.c
 * @brief ���G�����{�[�h��ʏ���
 * @author  Akito Mori(�ڐA�j
 * @date    09.01.20
 *
 * @todo
 *   �E�ڑ������̊֐����܂�UNION_APP�̂��̂ɂ��Ă��Ȃ�(VIRTUAL_CONNECT_LIMIT)
 *
 */
//============================================================================================
#define DEBUGPLAY_ONE ( 0 )
#define VIRTUAL_CONNECT_LIMIT

#include <gflib.h>
#include <calctool.h>

#include "system/main.h"
#include "print/wordset.h"
#include "font/font.naix"
#include "arc/arc_def.h"
#include "arc/message.naix"
#include "msg/msg_oekaki.h"
#include "system/wipe.h"
#include "system/bmp_winframe.h"
#include "system/gfl_use.h"
#include "gamesystem/msgspeed.h"
#include "print/printsys.h"
#include "sound/pm_sndsys.h"
#include "app/app_menu_common.h"
#include "savedata/etc_save.h"

#include "net_app/oekaki.h"
#include "oekaki_local.h"
#include "comm_command_oekaki.h"

//#include "msgdata/msg_ev_win.h"


#include "arc/oekaki.naix"      // �O���t�B�b�N�A�[�J�C�u��`

//============================================================================================
//  �萔��`
//============================================================================================

// ������`��p�̃p���b�g��`
#define NAME_COL_MINE   ( PRINTSYS_LSB_Make(  3,  4,  0) )  // �����̖��O
#define NAME_COL_NORMAL ( PRINTSYS_LSB_Make(0x1,0x2,0xf) )  // �Q�������o�[�̖��O
#define STRING_COL_MSG  ( PRINTSYS_LSB_Make(0x1,0x2,0xf) )  // �u��߂�v

#define MESFRAME_PAL      ( 10 )   // ���b�Z�[�W�E�C���h�E
#define MENUFRAME_PAL     ( 11 )   // ���j���[�E�C���h�E

// SE��`
#define OEKAKI_DECIDE_SE     ( SEQ_SE_SELECT1 )
#define OEKAKI_BS_SE         ( SEQ_SE_SELECT4 )
#define OEKAKI_NEWMEMBER_SE  ( SEQ_SE_DECIDE1 )
#define OEKAKI_PEN_CHANGE_SE ( SEQ_SE_DECIDE2 )

// �͂��E������
#define YESNO_RET_YES   ( 0 )
#define YESNO_RET_NO    ( 1 )
#define YESNO_RET_NONE  ( 2 )

//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================

/*** �֐��v���g�^�C�v ***/
static void VBlankFunc( GFL_TCB *tcb, void * work );
static void VramBankSet(void);
static void BgInit( void );
static void InitWork( OEKAKI_WORK *wk );
static void FreeWork( OEKAKI_WORK *wk );
static void BgExit(void);
static void BgGraphicSet( OEKAKI_WORK * wk, ARCHANDLE* p_handle );
static void InitCellActor(OEKAKI_WORK *wk, ARCHANDLE* p_handle);
static void SetCellActor(OEKAKI_WORK *wk);
static void BmpWinInit(OEKAKI_WORK *wk, GFL_PROC* proc);
static void BmpWinDelete( OEKAKI_WORK *wk );
static void SetCursor_Pos( GFL_CLWK *act, int x, int y );
static void NormalTouchFunc(OEKAKI_WORK *wk);
static int Oekaki_MainNormal( OEKAKI_WORK *wk, int seq );
static void EndSequenceCommonFunc( OEKAKI_WORK *wk );
static int Oekaki_EndSelectPutString( OEKAKI_WORK *wk, int seq );
static int Oekaki_EndSelectWait( OEKAKI_WORK *wk, int seq );
static int Oekaki_EndSelectAnswerWait( OEKAKI_WORK *wk, int seq );
static int Oekaki_EndSelectAnswerOK( OEKAKI_WORK *wk, int seq );
static int Oekaki_EndSelectAnswerNG( OEKAKI_WORK *wk, int seq );
static int  Oekaki_EndChild( OEKAKI_WORK *wk, int seq );
static int  Oekaki_EndChildWait( OEKAKI_WORK *wk, int seq );
static int  Oekaki_EndChildWait2( OEKAKI_WORK *wk, int seq );
static int Oekaki_EndSelectParent( OEKAKI_WORK *wk, int seq );
static int Oekaki_EndSelectParentWait( OEKAKI_WORK *wk, int seq );
static int Oekaki_ForceEnd( OEKAKI_WORK *wk, int seq );
static int Oekaki_ForceEndWait( OEKAKI_WORK *wk, int seq );
static int Oekaki_ForceEndSynchronize( OEKAKI_WORK *wk, int seq );
static int Oekaki_EndParentOnly( OEKAKI_WORK *wk, int seq );
static int Oekaki_EndParentOnlyWait( OEKAKI_WORK *wk, int seq );
static void DrawPoint_to_Line(  GFL_BMPWIN *win,   const u8 *brush,  int px, int py, int *sx, int *sy,   int count, int flag );
static void Stock_OldTouch( TOUCH_INFO *all, OLD_TOUCH_INFO *stock );
static void DrawBrushLine( GFL_BMPWIN *win, TOUCH_INFO *all, OLD_TOUCH_INFO *old, int draw );
static void MoveCommCursor( OEKAKI_WORK *wk );
static void DebugTouchDataTrans( OEKAKI_WORK *wk );
static void CursorColTrans(u16 *CursorCol);
static void NameCheckPrint( GFL_BMPWIN *win[], PRINTSYS_LSB color, OEKAKI_WORK *wk );
static int ConnectCheck( OEKAKI_WORK *wk );
static void LineDataSendRecv( OEKAKI_WORK *wk );
static int MyStatusCheck( OEKAKI_WORK *wk );
static void EndMessagePrint( OEKAKI_WORK *wk, int msgno, int wait );
static int EndMessageWait( PRINT_STREAM *stream );
static void EndMessageWindowOff( OEKAKI_WORK *wk );
static int OnlyParentCheck( OEKAKI_WORK *wk );
static int Oekaki_LogoutChildMes( OEKAKI_WORK *wk, int seq );
static int Oekaki_LogoutChildClose( OEKAKI_WORK *wk, int seq );
static int Oekaki_LogoutChildMesWait( OEKAKI_WORK *wk, int seq );
static int MyStatusGetNum( OEKAKI_WORK *wk );
static int Oekaki_NewMemberWait( OEKAKI_WORK *wk, int seq );
static int Oekaki_NewMember( OEKAKI_WORK *wk, int seq );
static int Oekaki_NewMemberEnd( OEKAKI_WORK *wk, int seq );
static void PalButtonAppearChange( GFL_CLWK *act[], int no);
static void EndButtonAppearChange( GFL_CLWK *act, BOOL flag );
static void _BmpWinPrint_Rap(
      GFL_BMPWIN * win, void * src,
      int src_x, int src_y, int src_dx, int src_dy,
      int win_x, int win_y, int win_dx, int win_dy );
static void ChangeConnectMax( OEKAKI_WORK *wk, int plus );
static int ConnectNumControl( OEKAKI_WORK *wk );
static int Oekaki_ForceEndWaitNop( OEKAKI_WORK *wk, int seq );
static void SetNextSequence( OEKAKI_WORK *wk, int nextSequence );
static void OekakiSequenceControl( OEKAKI_WORK *wk, int proc_seq );

static int FakeEndYesNoSelect( OEKAKI_WORK  *wk );
static void SetTouchpanelData( TOUCH_INFO *touchResult, TP_ONE_DATA *tpData, int brush_color, int brush );
static int _get_connect_bit( OEKAKI_WORK *wk );
static int _get_connect_num( OEKAKI_WORK *wk );
static void _disp_on( void );
static void OEKAKI_entry_callback(NetID net_id, const MYSTATUS *mystatus, void *userwork);
static void OEKAKI_leave_callback(NetID net_id, const MYSTATUS *mystatus, void *userwork);
static APP_TASKMENU_WORK  *YesNoMenuInit( OEKAKI_WORK *wk );
static u32 YesNoMenuMain( OEKAKI_WORK *wk );
static void OekakiResetYesNoWin(OEKAKI_WORK *wk);
static void _comm_friend_add( OEKAKI_WORK *wk );


typedef struct{
  int (*func)(OEKAKI_WORK *wk, int seq);
  int execOn_SeqLeave;
}OEKAKI_FUNC_TABLE;

static OEKAKI_FUNC_TABLE FuncTable[]={
  {NULL,                      1,},  // OEKAKI_MODE_INIT  = 0, 
  {Oekaki_NewMember,          0,},  // OEKAKI_MODE_NEWMEMBER,
  {Oekaki_NewMemberWait,      0,},  // OEKAKI_MODE_NEWMEMBER_WAIT,
  {Oekaki_NewMemberEnd,       0,},  // OEKAKI_MODE_NEWMEMBER_END,
  {Oekaki_MainNormal,         0,},  // OEKAKI_MODE,
  {Oekaki_EndSelectPutString, 1,},  // OEKAKI_MODE_END_SELECT,
  {Oekaki_EndSelectWait,      1,},  // OEKAKI_MODE_END_SELECT_WAIT,
  {Oekaki_EndSelectAnswerWait,1,},  // OEKAKI_MODE_END_SELECT_ANSWER_WAIT
  {Oekaki_EndSelectAnswerOK,  1,},  // OEKAKI_MODE_END_SELECT_ANSWER_OK
  {Oekaki_EndSelectAnswerNG,  1,},  // OEKAKI_MODE_END_SELECT_ANSWER_NG
  {Oekaki_EndChild,           1,},  // OEKAKI_MODE_END_CHILD
  {Oekaki_EndChildWait,       1,},  // OEKAKI_MODE_END_CHILD_WAIT
  {Oekaki_EndChildWait2,      1,},  // OEKAKI_MODE_END_CHILD_WAIT2
  {Oekaki_EndSelectParent,    1,},  // OEKAKI_MODE_END_SELECT_PARENT
  {Oekaki_EndSelectParentWait,1,},  // OEKAKI_MODE_END_SELECT_PARENT_WAIT
  {Oekaki_ForceEnd,           1,},  // OEKAKI_MODE_FORCE_END
  {Oekaki_ForceEndWait,       1,},  // OEKAKI_MODE_FORCE_END_WAIT
  {Oekaki_ForceEndSynchronize,1,},  // OEKAKI_MODE_FORCE_END_SYNCHRONIZE
  {Oekaki_ForceEndWaitNop,    1,},  // OEKAKI_MODE_FORCE_END_WAIT_NOP
  {Oekaki_EndParentOnly,      1,},  // OEKAKI_MODE_END_PARENT_ONLY
  {Oekaki_EndParentOnlyWait,  1,},  // OEKAKI_MODE_END_PARENT_ONLY_WAIT
  {Oekaki_LogoutChildMes,     1,},  // OEKAKI_MODE_LOGOUT_CHILD
  {Oekaki_LogoutChildMesWait, 1,},  // OEKAKI_MODE_LOGOUT_CHILD_WAIT
  {Oekaki_LogoutChildClose,   1,},  // OEKAKI_MODE_LOGOUT_CHILD_CLOSE
};


//--------------------------------------------------------------------------------------------
/**
 * ���G����PROCDATA
 */
//--------------------------------------------------------------------------------------------
const GFL_PROC_DATA OekakiProcData = {
  OekakiProc_Init,
  OekakiProc_Main,
  OekakiProc_End,
};


#ifdef VIRTUAL_CONNECT_LIMIT
static void CommStateSetLimitNum( OEKAKI_WORK *wk, int num)
{
  return;
}
#endif

//============================================================================================
//  �v���Z�X�֐�
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F������
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT OekakiProc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  OEKAKI_WORK * wk;
  ARCHANDLE* p_handle;

  switch(*seq){
  case 0:
    GFL_DISP_GX_InitVisibleControl();
    GFL_DISP_GXS_InitVisibleControl();
  
    OS_Printf("OEKAKI_HEAP Cretae, before remain = %08x\n", GFL_HEAP_GetHeapFreeSize(GFL_HEAPID_APP));
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_OEKAKI, 0x40000 );

    wk = GFL_PROC_AllocWork( proc, sizeof(OEKAKI_WORK), HEAPID_OEKAKI );
    GFL_STD_MemFill( wk, 0, sizeof(OEKAKI_WORK) );
    GFL_BG_Init( HEAPID_OEKAKI );
    
    // ������}�l�[�W���[����
    wk->WordSet    = WORDSET_Create( HEAPID_OEKAKI );
    wk->MsgManager = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                                     NARC_message_oekaki_dat, HEAPID_OEKAKI );
    wk->font       = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr ,
                                      GFL_FONT_LOADTYPE_FILE , FALSE , HEAPID_OEKAKI );

    // VRAM �o���N�ݒ�
    VramBankSet();
    
    // BG�V�X�e��������
    BgInit();          

    WIPE_SetBrightness( WIPE_DISP_MAIN,WIPE_FADE_BLACK );
    WIPE_SetBrightness( WIPE_DISP_SUB,WIPE_FADE_BLACK );

    // �P�x�ύX�Z�b�g
    WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_HOLEIN, WIPE_TYPE_HOLEIN, WIPE_FADE_BLACK, 16, 1, HEAPID_OEKAKI );
    
    // �p�����[�^�擾
    {
      PICTURE_PARENT_WORK *param = (PICTURE_PARENT_WORK*)pwk;
      wk->param = param;
    }

    // �n���h���I�[�v��
    p_handle = GFL_ARC_OpenDataHandle( ARCID_OEKAKI, HEAPID_OEKAKI );

    //BG�O���t�B�b�N�Z�b�g
    BgGraphicSet( wk, p_handle );

//    InitTPNoBuff(2);
  
    // VBlank�֐��Z�b�g
    wk->vblankTcb = GFUser_VIntr_CreateTCB( VBlankFunc, wk, 0 );  
  
    // ���[�N������
    InitWork( wk );

    // CellActor�V�X�e��������
    InitCellActor(wk, p_handle);
    
    // CellActro�\���o�^
    SetCellActor(wk);

    // BMPWIN�o�^�E�`��
    BmpWinInit(wk,proc);

    // ���ON�i�o�͂̏㉺����ւ����j
    _disp_on();

    // �ʐM�R�}���h�����������{�[�h�p�ɕύX
    OEKAKIBOARD_CommandInitialize( wk );

        // ���G�������ɂ͐ڑ��ؒf�ŃG���[�������Ȃ�
//        CommStateSetErrorCheck(FALSE,TRUE);
    
    // �ڑ����Ă��锽�Α���ID�̃����o�[��ʐM�F�B�o�^
    _comm_friend_add(wk);

    // 3��܂Őڑ��\�ɏ�������(�J�n�������͂Q�l�ł����ɂ���̂ł��ƈ�l���������悤�ɂ��Ă����j
    if(GFL_NET_SystemGetCurrentID()==0){
          CommStateSetLimitNum(wk, 3);
    }

    //�����A�C�R���\��
    GFL_NET_WirelessIconEasy_HoldLCD( TRUE, HEAPID_OEKAKI );

    // �n���h���N���[�Y
    GFL_ARC_CloseDataHandle( p_handle );
    (*seq)++;
    break;
  case 1:
    (*seq) = SEQ_IN;
    return GFL_PROC_RES_FINISH;
    break;
  }
  return GFL_PROC_RES_CONTINUE;
}




//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F���C��
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------

GFL_PROC_RESULT OekakiProc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  OEKAKI_WORK * wk  = (OEKAKI_WORK *)mywk;

  if(GFL_NET_SystemGetCurrentID() == 0 && wk->ridatu_bit != 0){
    wk->ridatu_bit &= _get_connect_bit(wk);
  }
  
  // ���[�N�Ɍ��݂�proc�V�[�P���X��ۑ�
  wk->proc_seq = *seq;

  // �T�u�V�[�P���X�J�ڐ�����s��
  OekakiSequenceControl( wk, *seq );
  
  switch( *seq ){
  case SEQ_IN:
    // ���C�v�����҂�
    if( WIPE_SYS_EndCheck() ){
      // ����OK��Ԃɂ���
      Union_App_Parent_ResetEntryBlock( wk->param->uniapp );

      // �����E�ޏo�R�[���o�b�N�o�^
      Union_App_SetCallback( wk->param->uniapp, OEKAKI_entry_callback, OEKAKI_leave_callback, wk);
#if 0
      // �������q�@�Őڑ��䐔���Q��ȏゾ�����ꍇ�͂����G���`����Ă���
      if(GFL_NET_SystemGetCurrentID()!=0){
        if( (MyStatusGetNum(wk)>2) ){
          // �q�@�������N�G�X�g
          GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(),CO_OEKAKI_CHILD_JOIN, NULL, 0);
          OS_Printf("�������܂�\n");
        }
        *seq = SEQ_MAIN;
        break;
      }else{
        //�e�̓��C����
        *seq = SEQ_MAIN;
      }
#endif
      //�e�̓��C����
      *seq = SEQ_MAIN;

    }
    break;

  case SEQ_MAIN:
    // �J�[�\���ړ�

    // �V�[�P���X���̓���
    if(FuncTable[wk->seq].func!=NULL){
      *seq = FuncTable[wk->seq].func( wk, *seq );
    }
    // �ڑ��l�����m�F���ď��ʂɖ��O��`��
    NameCheckPrint( wk->TrainerNameWin, NAME_COL_NORMAL ,wk);

    if(GFL_NET_SystemGetCurrentID()==0){        // �e��
      int temp = ConnectNumControl(wk);
      if(*seq == SEQ_MAIN){
        // ���ɏI���V�[�P���X�Ȃǂɓ����Ă���ꍇ�͏��������Ȃ�
        *seq = temp;
      }
    }
    CursorColTrans(&wk->CursorPal);

    break;
  case SEQ_LEAVE:
    if(FuncTable[wk->seq].func!=NULL && FuncTable[wk->seq].execOn_SeqLeave){
      *seq = FuncTable[wk->seq].func( wk, *seq );
    }
    break;

  case SEQ_OUT:
    if( WIPE_SYS_EndCheck() ){
      OS_Printf("���C�v�I���m�F\n");
      // �ʐM�R�}���h�e�[�u�����
      GFL_NET_DelCommandTable( GFL_NET_CMD_PICTURE );
      // �ʐM�ؒf
      Union_App_Shutdown( wk->param->uniapp );
      *seq = SEQ_DISCONNECT_WAIT;
    }
    break;
  case SEQ_DISCONNECT_WAIT:
    // ���j�I���ʐM�ؒf�҂�
    if(Union_App_WaitShutdown(wk->param->uniapp)){
      return GFL_PROC_RES_FINISH;
    }
    break;
  }
  GFL_CLACT_SYS_Main();             // �Z���A�N�^�[�풓�֐�
  GFL_TCBL_Main( wk->pMsgTcblSys );
  PRINTSYS_QUE_Main( wk->printQue );
  {
    int i;
    for(i=0;i<OEKAKI_PRINT_UTIL_NAME_WIN4+1;i++){
      PRINT_UTIL_Trans( &wk->printUtil[i], wk->printQue );
    }
    PRINT_UTIL_Trans( &wk->printUtil[OEKAKI_PRINT_UTIL_MSG], wk->printQue );

  }
  
  return GFL_PROC_RES_CONTINUE;
}

#define DEFAULT_NAME_MAX    18

// �_�C���E�p�[���ŕς��񂾂낤
#define MALE_NAME_START     0
#define FEMALE_NAME_START   18

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F�I��
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT OekakiProc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  OEKAKI_WORK  *wk    = (OEKAKI_WORK  *)mywk;
  PICTURE_PARENT_WORK *param = (PICTURE_PARENT_WORK*)pwk;
  int i;

  switch(*seq){
  case 0:
    OS_Printf("�����������[�N��������˓�\n");

    // Vblank���Ԓ���BG�]���I��
    GFL_TCB_DeleteTask( wk->vblankTcb );

    // �Z���A�N�^�[���\�[�X���
    GFL_CLGRP_CGR_Release(  wk->resObjTbl[CLACT_RES_S_CHR] );     // �T�u��
    GFL_CLGRP_PLTT_Release( wk->resObjTbl[CLACT_RES_S_PLTT] );
    GFL_CLGRP_CELLANIM_Release( wk->resObjTbl[CLACT_RES_S_CELL] );
    
    GFL_CLGRP_CGR_Release(  wk->resObjTbl[CLACT_RES_SYS_CHR] );     // �V�X�e���A�C�R��
    GFL_CLGRP_PLTT_Release( wk->resObjTbl[CLACT_RES_SYS_PLTT] );
    GFL_CLGRP_CELLANIM_Release( wk->resObjTbl[CLACT_RES_SYS_CELL] );

    GFL_CLGRP_CGR_Release(  wk->resObjTbl[CLACT_RES_M_CHR] );     // ���C����
    GFL_CLGRP_PLTT_Release( wk->resObjTbl[CLACT_RES_M_PLTT] );
    GFL_CLGRP_CELLANIM_Release( wk->resObjTbl[CLACT_RES_M_CELL] );
    
    
    // �Z���A�N�^�[���j�b�g�j��
    GFL_CLACT_UNIT_Delete( wk->clUnit );

    //OAM�����_���[�j��
    GFL_CLACT_SYS_Delete();
    
    // BMP�E�B���h�E�J��
    BmpWinDelete( wk );

    // BGL�폜
    BgExit();

    // ���b�Z�[�W�}�l�[�W���[�E���[�h�Z�b�g�}�l�[�W���[���
    GFL_MSG_Delete( wk->MsgManager );
    WORDSET_Delete( wk->WordSet );
    GFL_FONT_Delete( wk->font );

    (*seq)++;
    break;
  case 1:
    // �ʐM�I��
  //  CommStateExitUnion();
//    CommStateSetLimitNum(1);
//    CommStateUnionBconCollectionRestart();
//    UnionRoomView_ObjInit( param->view );

    // ����ւ���Ă����㉺��ʏo�͂����ɖ߂�
    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);


    (*seq)++;
    break;
  case 2:
    if (wk->ireagalJoin){
      OS_Printf("�e�@�����ɂȂ�܂őҋ@\n");  
      if (_get_connect_bit(wk) == 1){
        (*seq)++;
      }
    }else{
      (*seq)++;
    }
    break;
  case 3:
    CommStateSetLimitNum(wk,2);
    // ���[�N���
    FreeWork( wk );

    GFL_PROC_FreeWork( proc );        // GFL_PROC���[�N�J��

    GFL_HEAP_DeleteHeap( HEAPID_OEKAKI );

    CommStateSetLimitNum(wk,2);

    return GFL_PROC_RES_FINISH;
  }
  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * VBlank�֐�
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void VBlankFunc( GFL_TCB *tcb, void * work )
{
  // �Z���A�N�^�[
  GFL_CLACT_SYS_VBlankFunc();

  // BG�]��  
  GFL_BG_VBlankFunc();
  
}


//----------------------------------------------------------------------------------
/**
 * @brief �y���j�I�������R�[���o�b�N�z��������
 *         �����R�[���o�b�N�͂R��ڂ���̎q�@�����������Ȃ�
 *
 * @param   net_id      �ڑ��h�c
 * @param   mystatus    MYSTATUS
 * @param   userwork    OEKAKI_WORK*
 */
//----------------------------------------------------------------------------------
static void OEKAKI_entry_callback(NetID net_id, const MYSTATUS *mystatus, void *userwork)
{
  OEKAKI_WORK  *wk    = (OEKAKI_WORK  *)userwork;
  if(net_id==0){
    OS_Printf("�e�͗��������ɂ��Ȃ� net_id=%d\n", net_id);
  }
  
  // �ʂȎq�@�̗����ɑΏ�
  if(GFL_NET_SystemGetCurrentID()==0){
      int ret;
      u8 id  = net_id;
      // 2��ڈȍ~�̎q�@�̗���
      // �S��Ɂu���ꂩ��G�𑗂�̂Ŏ~�܂��Ă��������v�Ƒ��M����
      ret=GFL_NET_SendData( GFL_NET_GetNetHandle( GFL_NET_NETID_SERVER), CO_OEKAKI_STOP, 1, &id);
      
      if(ret==FALSE){
        GF_ASSERT("�����R�[���o�b�N���M���s\n");
      }
  }

  // �ڑ����������o�[��ʐM�F�B�o�^
  _comm_friend_add(wk);


}

//----------------------------------------------------------------------------------
/**
 * @brief �y���j�I���ޏo�R�[���o�b�N�z�ޏo������
 *
 * @param   net_id      �ڑ��h�c
 * @param   mystatus    MYSTATUS
 * @param   userwork    OEKAKI_WORK*
 */
//----------------------------------------------------------------------------------
static void OEKAKI_leave_callback(NetID net_id, const MYSTATUS *mystatus, void *userwork)
{
  OS_Printf("���E�R�[���o�b�N net_id=%d\n", net_id);
}

//--------------------------------------------------------------------------------------------
// VRAM�ݒ�data
//--------------------------------------------------------------------------------------------
static const GFL_DISP_VRAM OekakiDispVramDat = {
    GX_VRAM_BG_128_A,           // ���C��2D�G���W����BG
    GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g

    GX_VRAM_SUB_BG_128_C,       // �T�u2D�G���W����BG
    GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g

    GX_VRAM_OBJ_128_B,          // ���C��2D�G���W����OBJ
    GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g

    GX_VRAM_SUB_OBJ_16_I,       // �T�u2D�G���W����OBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g

    GX_VRAM_TEX_NONE,           // �e�N�X�`���C���[�W�X���b�g
    GX_VRAM_TEXPLTT_NONE,       // �e�N�X�`���p���b�g�X���b�g

    GX_OBJVRAMMODE_CHAR_1D_32K, // ���C����OBJVRAM�T�C�Y
    GX_OBJVRAMMODE_CHAR_1D_32K, // �T�u��OBJVRAM�T�C�Y
    
  };


//--------------------------------------------------------------------------------------------
/**
 * VRAM�ݒ�
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void VramBankSet(void)
{
  GFL_DISP_SetBank( &OekakiDispVramDat );
}

//--------------------------------------------------------------------------------------------
/**
 * BG�ݒ�
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BgInit( void )
{
  // BG SYSTEM
  { 
    GFL_BG_SYS_HEADER BGsys_data = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &BGsys_data );
  }

  // ���C����ʕ�����0
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  GFL_BG_FRAME0_M );


  }

  // ���C����ʃ��N�K�L��
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  GFL_BG_FRAME1_M );
  }

  // ���C����ʔw�i
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
    };
    GFL_BG_SetBGControl(  GFL_BG_FRAME2_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
  }


  // �T�u��ʃe�L�X�g��
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
    };
    GFL_BG_SetBGControl(  GFL_BG_FRAME0_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  GFL_BG_FRAME0_S );
  }

  // �T�u��ʔw�i��
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
    };
    GFL_BG_SetBGControl(  GFL_BG_FRAME1_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
  }

  GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 32, 0, HEAPID_OEKAKI );
  GFL_BG_SetClearCharacter( GFL_BG_FRAME0_S, 32, 0, HEAPID_OEKAKI );
  GFL_BG_SetClearCharacter( GFL_BG_FRAME1_M, 32, 0, HEAPID_OEKAKI ); ///< �����ʈȊO���L�����N���A�����B080422
  GFL_BG_SetClearCharacter( GFL_BG_FRAME1_S, 32, 0, HEAPID_OEKAKI );

}


#define TALK_MESSAGE_BUF_NUM  ( 40*2 )

//------------------------------------------------------------------
/**
 * ���G�������[�N������
 *
 * @param   wk    OEKAKI_WORK*
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void InitWork( OEKAKI_WORK *wk )
{
  int i;

  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    wk->TrainerName[i] = GFL_STR_CreateBuffer( PERSON_NAME_SIZE+EOM_SIZE, HEAPID_OEKAKI );
    wk->AllTouchResult[i].size = 0;
    wk->OldTouch[i].size = 0;
    wk->TrainerStatus[i][0] = NULL;
    wk->TrainerStatus[i][1] = NULL;

  }
  // �u��߂�v������o�b�t�@�쐬
  wk->EndString  = GFL_STR_CreateBuffer( 10, HEAPID_OEKAKI );
  wk->TalkString = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_OEKAKI );

  // �u���V������
  wk->brush_color  = 0;
  wk->brush        = MIDDLE_BRUSH;
  wk->banFlag      = OEKAKI_BAN_OFF;
  wk->yesno_flag   = 0;
  wk->shareNum     = 2;
  wk->shareBit     = _get_connect_bit(wk);
  wk->firstChild   = 0;
  wk->ireagalJoin  = 0;
  wk->seq          = 0;
  wk->bookJoin     = 0;
  wk->joinBit      = 0;
  wk->app_menuwork = NULL;

  // �e�͒ʐM�ŊG���󂯎��K�v������
  if( GFL_NET_SystemGetCurrentID()==0 ){
    SetNextSequence( wk, OEKAKI_MODE );
  }else{
    // �G���󂯎��Ȃ��Ƃ����Ȃ�
    SetNextSequence( wk, OEKAKI_MODE );
  }
  
  // �u��߂�v�擾
  GFL_MSG_GetString( wk->MsgManager, msg_oekaki_08, wk->EndString );

  // ���G�����摜���k�f�[�^�̈�
  wk->lz_buf = GFL_HEAP_AllocMemory( HEAPID_OEKAKI, 30*16*32 );

  // �ڑ��l���Ď��p���[�N������
  wk->connectBackup = 0;
}

//------------------------------------------------------------------
/**
 * @brief   ���[�N���
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void FreeWork( OEKAKI_WORK *wk )
{
  int i;

  GFL_HEAP_FreeMemory(  wk->lz_buf );


  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    GFL_STR_DeleteBuffer( wk->TrainerName[i] );
  }
  GFL_STR_DeleteBuffer( wk->TalkString ); 
  GFL_STR_DeleteBuffer( wk->EndString  ); 

}

//--------------------------------------------------------------------------------------------
/**
 * BG���
 *
 * @param ini   BGL�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BgExit( void )
{

  GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
  GFL_BG_FreeBGControl( GFL_BG_FRAME0_S );
  GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
  GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
  GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );

  GFL_BG_Exit();
}


#define TALKFONT_PAL_ROW      ( 13 )
#define TALKFONT_PAL_OFFSET   ( TALKFONT_PAL_ROW*0x20 )
//--------------------------------------------------------------------------------------------
/**
 * �O���t�B�b�N�f�[�^�Z�b�g
 *
 * @param wk    �|�P�������X�g��ʂ̃��[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BgGraphicSet( OEKAKI_WORK * wk, ARCHANDLE* p_handle )
{

  // �㉺��ʂa�f�p���b�g�]��
  GFL_ARCHDL_UTIL_TransVramPalette(    p_handle, NARC_oekaki_oekaki_m_NCLR, PALTYPE_MAIN_BG, 0, 16*2*12,  HEAPID_OEKAKI);
  GFL_ARCHDL_UTIL_TransVramPalette(    p_handle, NARC_oekaki_oekaki_s_NCLR, PALTYPE_SUB_BG,  0, 16*2*12,  HEAPID_OEKAKI);
  
  // ��b�t�H���g�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
                                 TALKFONT_PAL_OFFSET, 32, HEAPID_OEKAKI );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, 
                                 TALKFONT_PAL_OFFSET, 32, HEAPID_OEKAKI );

  // ���C�����BG2�L�����]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_oekaki_oekaki_m_NCGR, GFL_BG_FRAME2_M, 0, 32*8*0x20, 0, HEAPID_OEKAKI);

  // ���C�����BG2�X�N���[���]��
  GFL_ARCHDL_UTIL_TransVramScreen(   p_handle, NARC_oekaki_oekaki_m_NSCR, GFL_BG_FRAME2_M, 0, 32*24*2, 0, HEAPID_OEKAKI);



  // �T�u���BG1�L�����]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_oekaki_oekaki_s_NCGR, GFL_BG_FRAME1_S, 0, 32*8*0x20, 0, HEAPID_OEKAKI);

  // �T�u���BG1�X�N���[���]��
  GFL_ARCHDL_UTIL_TransVramScreen(   p_handle, NARC_oekaki_oekaki_s_NSCR, GFL_BG_FRAME1_S, 0, 32*24*2, 0, HEAPID_OEKAKI);

  // �T�u��ʉ�b�E�C���h�E�O���t�B�b�N�]��
  BmpWinFrame_GraphicSet(
        GFL_BG_FRAME0_M, 1, MESFRAME_PAL,  0, HEAPID_OEKAKI );

  BmpWinFrame_GraphicSet(
        GFL_BG_FRAME0_M, 1+TALK_WIN_CGX_SIZ, MENUFRAME_PAL, 0, HEAPID_OEKAKI );

}


//------------------------------------------------------------------
/**
 * ���[�_�[��ʗp�Z���A�N�^�[������
 *
 * @param   wk    ���[�_�[�\���̂̃|�C���^
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void InitCellActor(OEKAKI_WORK *wk, ARCHANDLE* p_handle)
{
  int i;
  
  // ���ʑf�ރt�@�C���n���h���I�[�v��
  ARCHANDLE *c_handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_OEKAKI );
  // �Z���A�N�^�[������
  GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN, &OekakiDispVramDat, HEAPID_OEKAKI );
  
  
  // �Z���A�N�^�[������
  wk->clUnit = GFL_CLACT_UNIT_Create( 50+3, 1,  HEAPID_OEKAKI );
  
  
  
  //---------���C����ʗp-------------------
  // ���G�����f��
  //chara�ǂݍ���
  wk->resObjTbl[CLACT_RES_M_CHR] = GFL_CLGRP_CGR_Register( p_handle, NARC_oekaki_oekaki_m_obj_NCGR, 0, 
                                                           CLSYS_DRAW_MAIN, HEAPID_OEKAKI );

  //pal�ǂݍ���
  wk->resObjTbl[CLACT_RES_M_PLTT] = GFL_CLGRP_PLTT_Register( p_handle, NARC_oekaki_oekaki_m_NCLR, 
                                                             CLSYS_DRAW_MAIN, 0, HEAPID_OEKAKI );

  //cell�ǂݍ���
  wk->resObjTbl[CLACT_RES_M_CELL] = GFL_CLGRP_CELLANIM_Register( p_handle, 
                                                                 NARC_oekaki_oekaki_m_obj_NCER, 
                                                                 NARC_oekaki_oekaki_m_obj_NANR, 
                                                                 HEAPID_OEKAKI );

  //---------���C����ʗp-------------------
  // ���ʃ��j���[�f��
  //chara�ǂݍ���
  wk->resObjTbl[CLACT_RES_SYS_CHR] = GFL_CLGRP_CGR_Register( c_handle, APP_COMMON_GetBarIconCharArcIdx(), 0, 
                                                           CLSYS_DRAW_MAIN, HEAPID_OEKAKI );

  //pal�ǂݍ���
  wk->resObjTbl[CLACT_RES_SYS_PLTT] = GFL_CLGRP_PLTT_RegisterEx( c_handle, APP_COMMON_GetBarIconPltArcIdx(), 
                                                                 CLSYS_DRAW_MAIN, 11*32, 0, 3, HEAPID_OEKAKI );

  //cell�ǂݍ���
  wk->resObjTbl[CLACT_RES_SYS_CELL] = GFL_CLGRP_CELLANIM_Register( c_handle, 
                                                                 APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_32K), 
                                                                 APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_32K), 
                                                                 HEAPID_OEKAKI );

  //---------�T�u��ʗp-------------------

  //chara�ǂݍ���
  wk->resObjTbl[CLACT_RES_S_CHR] = GFL_CLGRP_CGR_Register( p_handle, NARC_oekaki_oekaki_m_obj_NCGR, 0, 
                                                           CLSYS_DRAW_SUB, HEAPID_OEKAKI );

  //pal�ǂݍ���
  wk->resObjTbl[CLACT_RES_S_PLTT] = GFL_CLGRP_PLTT_Register( p_handle, NARC_oekaki_oekaki_m_NCLR, 
                                                             CLSYS_DRAW_SUB, 0, HEAPID_OEKAKI );

  //cell�ǂݍ���
  wk->resObjTbl[CLACT_RES_S_CELL] = GFL_CLGRP_CELLANIM_Register( p_handle, 
                                                                 NARC_oekaki_oekaki_m_obj_NCER, 
                                                                 NARC_oekaki_oekaki_m_obj_NANR, 
                                                                 HEAPID_OEKAKI );
  // ���ʑf�ރt�@�C���n���h���N���[�Y
  GFL_ARC_CloseDataHandle( c_handle );

  

}

#define TRAINER_NAME_POS_X    ( 24 )
#define TRAINER_NAME_POS_Y    ( 32 )
#define TRAINER_NAME_POS_SPAN ( 32 )

#define END_ICON_X  ( 212 )
#define END_ICON_Y  ( 160 )


static const u16 pal_button_oam_table[][3]={
  {12     , 172, 5},    // ��
  {12+24*1, 172, 7},    // ��
  {12+24*2, 172, 9},    // ��
  {12+24*3, 172,11},    // ��
  {12+24*4, 172,13},    // ��
  {12+24*5, 172,15},    // ���F
  {12+24*6, 172,17},    // ��
  {12+24*7, 172,19},    // ���F
  {12+24*8+20, 171,21}, // �u��߂�v

  // �y����A�C�R��
  {20,    9,   29, },
  {36,      9,   32, }, // �����l�Ō����ԂɂȂ��Ă���
  {52,      9,   33, },
};

//------------------------------------------------------------------
/**
 * �Z���A�N�^�[�o�^
 *
 * @param   wk      OEKAKI_WORK*
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void SetCellActor(OEKAKI_WORK *wk)
{
  int i;
  // �Z���A�N�^�[�w�b�_�쐬

  {
    //�o�^���i�[
    GFL_CLWK_DATA add;

    add.pos_x = 0;
    add.pos_y = 0;
    add.anmseq    = 0;
    add.softpri   = 1;
    add.bgpri     = 1;

    //�Z���A�N�^�[�\���J�n

    // ���C����ʗp(���̓o�^�j
    for(i=0;i<5;i++){
      add.pos_x = TRAINER_NAME_POS_X + i * 40;
      add.pos_y = TRAINER_NAME_POS_Y + TRAINER_NAME_POS_SPAN;
      wk->MainActWork[i] = GFL_CLACT_WK_Create( wk->clUnit,
        wk->resObjTbl[CLACT_RES_M_CHR],
        wk->resObjTbl[CLACT_RES_M_PLTT],
        wk->resObjTbl[CLACT_RES_M_CELL],
        &add, CLSYS_DEFREND_MAIN, HEAPID_OEKAKI );

      GFL_CLACT_WK_SetAutoAnmFlag( wk->MainActWork[i], 1 );
      GFL_CLACT_WK_SetAnmSeq(      wk->MainActWork[i], i );
      GFL_CLACT_WK_SetDrawEnable(  wk->MainActWork[i], 0 );

    }

    // ���C����ʃp���b�g�E�u��߂�v�{�^���̓o�^
    for(i=0;i<9+3;i++){
      add.pos_x = pal_button_oam_table[i][0];
      add.pos_y = pal_button_oam_table[i][1];
      wk->ButtonActWork[i] = GFL_CLACT_WK_Create( wk->clUnit,
        wk->resObjTbl[CLACT_RES_M_CHR ],
        wk->resObjTbl[CLACT_RES_M_PLTT],
        wk->resObjTbl[CLACT_RES_M_CELL],
        &add, CLSYS_DEFREND_MAIN, HEAPID_OEKAKI );

      GFL_CLACT_WK_SetAutoAnmFlag( wk->ButtonActWork[i], 1 );
      GFL_CLACT_WK_SetAnmSeq( wk->ButtonActWork[i], pal_button_oam_table[i][2] );
      if(i >= 8){
        GFL_CLACT_WK_SetBgPri( wk->ButtonActWork[i], 2 );
      }
    }

    // ���F�p���b�g�͂ւ��܂��Ă���
    GFL_CLACT_WK_SetAnmSeq( wk->ButtonActWork[0], 6 );


    // �T�u��ʗp(���̓o�^�j
    for(i=0;i<5;i++){
      add.pos_x = TRAINER_NAME_POS_X;
      add.pos_y = (TRAINER_NAME_POS_Y+TRAINER_NAME_POS_SPAN*i)+NAMEIN_SUB_ACTOR_DISTANCE;
      wk->SubActWork[i] = GFL_CLACT_WK_Create( wk->clUnit,
        wk->resObjTbl[CLACT_RES_S_CHR],
        wk->resObjTbl[CLACT_RES_S_PLTT],
        wk->resObjTbl[CLACT_RES_S_CELL],
        &add, CLSYS_DEFREND_SUB, HEAPID_OEKAKI );


      GFL_CLACT_WK_SetAutoAnmFlag( wk->SubActWork[i], 1 );
      GFL_CLACT_WK_SetAnmSeq( wk->SubActWork[i],      i );
      GFL_CLACT_WK_SetSoftPri( wk->SubActWork[i],     1 );  // ���ꂼ��̃A�N�^�[�̃v���C�I���e�B�ݒ�
      GFL_CLACT_WK_SetDrawEnable( wk->SubActWork[i],  0 );
      
    }

    // �A�v�����ʑf�ޖ߂�{�^���̕\��
    add.pos_x = END_ICON_X;
    add.pos_y = END_ICON_Y;
    wk->EndIconActWork = GFL_CLACT_WK_Create( wk->clUnit,
                                              wk->resObjTbl[CLACT_RES_SYS_CHR],
                                              wk->resObjTbl[CLACT_RES_SYS_PLTT],
                                              wk->resObjTbl[CLACT_RES_SYS_CELL],
                                              &add, CLSYS_DEFREND_MAIN, HEAPID_OEKAKI );
    GFL_CLACT_WK_SetAutoAnmFlag( wk->EndIconActWork, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->EndIconActWork,      1 );
    GFL_CLACT_WK_SetDrawEnable( wk->EndIconActWork,  TRUE );
    
  } 

  //���C���E�T�u���OBJ�ʂn�m
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON ); 
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON ); 
 
  
}

// ���������{�[�hBMP�i����ʁj
#define OEKAKI_BOARD_POSX  ( 1 )
#define OEKAKI_BOARD_POSY  ( 2 )
#define OEKAKI_BOARD_W   ( 30 )
#define OEKAKI_BOARD_H   ( 15 )


// ���O�\��BMP�i���ʁj
#define OEKAKI_NAME_BMP_W  ( 10 )
#define OEKAKI_NAME_BMP_H  (  2 )
#define OEKAKI_NAME_BMP_SIZE (OEKAKI_NAME_BMP_W * OEKAKI_NAME_BMP_H)


// �u��߂�v������BMP�i����ʁj
#define OEKAKI_END_BMP_X  ( 26 )
#define OEKAKI_END_BMP_Y  ( 21 )
#define OEKAKI_END_BMP_W  ( 6  )
#define OEKAKI_END_BMP_H  ( 2  )


// ��b�E�C���h�E�\���ʒu��`
#define OEKAKI_TALK_X   (  2 )
#define OEKAKI_TALK_Y   (  1 )

#define MSG_WIN_W      ( 27 )
#define MSG_WIN_H      (  4 )
//------------------------------------------------------------------
/**
 * BMPWIN�����i�����p�l���Ƀt�H���g�`��j
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void BmpWinInit( OEKAKI_WORK *wk, GFL_PROC* proc )
{
  // ---------- ���C����� ------------------

  // BMPWIN�V�X�e���J�n
  GFL_BMPWIN_Init( HEAPID_OEKAKI );

  // BG0��BMP�i��b�E�C���h�E�j�E�C���h�E�m��
  wk->MsgWin = GFL_BMPWIN_Create( GFL_BG_FRAME0_M,
                                  OEKAKI_TALK_X, OEKAKI_TALK_Y, 
                                  MSG_WIN_W, MSG_WIN_H, 13, GFL_BMP_CHRAREA_GET_B );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin), 0x0f0f );
  
  PRINT_UTIL_Setup( &wk->printUtil[OEKAKI_PRINT_UTIL_MSG], wk->MsgWin );

  // BG1�ʗpBMP�i���G�����摜�j�E�C���h�E�m��
  wk->OekakiBoard = GFL_BMPWIN_Create( GFL_BG_FRAME1_M,
                                   OEKAKI_BOARD_POSX, OEKAKI_BOARD_POSY, 
                                   OEKAKI_BOARD_W, OEKAKI_BOARD_H, 0, GFL_BMP_CHRAREA_GET_B);
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->OekakiBoard), 0x0202 );

  // ���b�Z�[�W�\���V�X�e���p������ 
  wk->pMsgTcblSys = GFL_TCBL_Init( HEAPID_OEKAKI, HEAPID_OEKAKI, 32 , 32 );
  wk->printQue    = PRINTSYS_QUE_Create( HEAPID_OEKAKI );

  
  // ----------- �T�u��ʖ��O�\��BMP�m�� ------------------
  {
    int i;
    for(i=0;i<OEKAKI_MEMBER_MAX;i++){
      wk->TrainerNameWin[i] = GFL_BMPWIN_Create( GFL_BG_FRAME0_S,  
                                             TRAINER_NAME_POS_X/8+2, TRAINER_NAME_POS_Y/8+i*4-1, 
                                             10, 2, 13,  GFL_BMP_CHRAREA_GET_B);
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->TrainerNameWin[i] ), 0 );
      PRINT_UTIL_Setup( &wk->printUtil[OEKAKI_PRINT_UTIL_NAME_WIN0+i], wk->TrainerNameWin[i] );
      GFL_BMPWIN_MakeTransWindow( wk->TrainerNameWin[i] );
    }

    //�ŏ��Ɍ����Ă���ʂȂ̂ŕ����p�l���`��Ɠ]�����s��
    NameCheckPrint( wk->TrainerNameWin, NAME_COL_NORMAL, wk );
  }
  
} 

// �͂��E�������p��`�i����ʁj
#define YESNO_CHARA_OFFSET  (1 + TALK_WIN_CGX_SIZ + MENU_WIN_CGX_SIZ + MSG_WIN_W*MSG_WIN_H )
#define YESNO_CHARA_W   ( 8 )
#define YESNO_CHARA_H   ( 4 )





//------------------------------------------------------------------
/**
 * @brief   �m�ۂ���BMPWIN�����
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void BmpWinDelete( OEKAKI_WORK *wk )
{
  int i;

  // ���b�Z�[�W�\���p�V�X�e�����
  PRINTSYS_QUE_Delete( wk->printQue );
  GFL_TCBL_Exit( wk->pMsgTcblSys );

  
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    GFL_BMPWIN_Delete( wk->TrainerNameWin[i] );
  }
  GFL_BMPWIN_Delete( wk->OekakiBoard );
  GFL_BMPWIN_Delete( wk->MsgWin );

  // BMPWIN�V�X�e���I��
  GFL_BMPWIN_Exit();
}








//------------------------------------------------------------------
/**
 * @brief   �J�[�\���ʒu��ύX����
 *
 * @param   act   �A�N�^�[�̃|�C���^
 * @param   x   
 * @param   y   
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void SetCursor_Pos( GFL_CLWK *act, int x, int y )
{
  GFL_CLACTPOS pos;

  pos.x = x;
  pos.y = y-8;
  GFL_CLACT_WK_SetWldPos( act, &pos );

}


// ---------------------------------------------------------
// �ʏ펞�^�b�`���o�e�[�u��
// ---------------------------------------------------------

#define PAL_BUTTON_X  (  0  )
#define PAL_BUTTON_Y  ( 150 )
#define PAL_BUTTON_W  ( 3*8 )
#define PAL_BUTTON_H  ( 5*8 )
#define PAL_BUTTON_RECT ( 3*8 -1 )
#define PAL_BUTTON_RECT ( 3*8 -1 )
#define END_BUTTON_RECT ( 8*8 -1 )

#define PAL_BUTTON0_X ( PAL_BUTTON_X+PAL_BUTTON_W*0 )
#define PAL_BUTTON1_X ( PAL_BUTTON_X+PAL_BUTTON_W*1 )
#define PAL_BUTTON2_X ( PAL_BUTTON_X+PAL_BUTTON_W*2 )
#define PAL_BUTTON3_X ( PAL_BUTTON_X+PAL_BUTTON_W*3 )
#define PAL_BUTTON4_X ( PAL_BUTTON_X+PAL_BUTTON_W*4 )
#define PAL_BUTTON5_X ( PAL_BUTTON_X+PAL_BUTTON_W*5 )
#define PAL_BUTTON6_X ( PAL_BUTTON_X+PAL_BUTTON_W*6 )
#define PAL_BUTTON7_X ( PAL_BUTTON_X+PAL_BUTTON_W*7 )
#define END_BUTTON_X  ( PAL_BUTTON_X+PAL_BUTTON_W*8 )

#define BRUSH_BUTTON0_X ( 14 ) // 16,5 23,12
#define BRUSH_BUTTON1_X ( 30 ) // 16,5 23,12
#define BRUSH_BUTTON2_X ( 46 ) // 16,5 23,12
#define BRUSH_BUTTON_Y  (  1 ) // 16,5 23,12
#define BRUSH_BUTTON_W  ( 12 ) // 16,5 23,12
#define BRUSH_BUTTON_H  ( 12 ) // 16,5 23,12


static const GFL_UI_TP_HITTBL sub_button_hittbl[]={
// �p���b�g
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON0_X,PAL_BUTTON0_X+PAL_BUTTON_RECT},
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON1_X,PAL_BUTTON1_X+PAL_BUTTON_RECT},
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON2_X,PAL_BUTTON2_X+PAL_BUTTON_RECT},
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON3_X,PAL_BUTTON3_X+PAL_BUTTON_RECT},
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON4_X,PAL_BUTTON4_X+PAL_BUTTON_RECT},
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON5_X,PAL_BUTTON5_X+PAL_BUTTON_RECT},
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON6_X,PAL_BUTTON6_X+PAL_BUTTON_RECT},
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON7_X,PAL_BUTTON7_X+PAL_BUTTON_RECT},

// �I���{�^��
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,END_BUTTON_X,END_BUTTON_X+END_BUTTON_RECT},

// �y����{�^��
{BRUSH_BUTTON_Y,BRUSH_BUTTON_Y+BRUSH_BUTTON_H, BRUSH_BUTTON0_X,BRUSH_BUTTON0_X+BRUSH_BUTTON_W},
{BRUSH_BUTTON_Y,BRUSH_BUTTON_Y+BRUSH_BUTTON_H, BRUSH_BUTTON1_X,BRUSH_BUTTON1_X+BRUSH_BUTTON_W},
{BRUSH_BUTTON_Y,BRUSH_BUTTON_Y+BRUSH_BUTTON_H, BRUSH_BUTTON2_X,BRUSH_BUTTON2_X+BRUSH_BUTTON_W},

{GFL_UI_TP_HIT_END,0,0,0},   // �I���f�[�^
};


// ---------------------------------------------------------
// ���E�I���֎~���̍ۂ�YESNO�{�^�����o�e�[�u��
// ---------------------------------------------------------
#define OEKAKI_YESNO_BUTTON_X ( 25 )
#define OEKAKI_YESNO_BUTTON_Y ( 6  )

#define FAKE_YESNO_X  ( OEKAKI_YESNO_BUTTON_X*8 )
#define FAKE_YESNO_Y  ( OEKAKI_YESNO_BUTTON_Y*8 )
#define FAKE_YESNO_W  ( 6*8 )
#define FAKE_YESNO_H  ( 8*8 )

static const GFL_UI_TP_HITTBL fake_yesno_hittbl[]={
  {FAKE_YESNO_Y, FAKE_YESNO_Y+FAKE_YESNO_H, FAKE_YESNO_X, FAKE_YESNO_X+FAKE_YESNO_W},
  {GFL_UI_TP_HIT_END,0,0,0},
};

// ---------------------------------------------------------
// �`��̈挟�o�p�e�[�u��
// ---------------------------------------------------------
#define DRAW_AREA_X (  8  )
#define DRAW_AREA_Y ( 16  )
#define DRAW_AREA_W ( OEKAKI_BOARD_W*8 )
#define DRAW_AREA_H ( OEKAKI_BOARD_H*8)

static const GFL_UI_TP_HITTBL sub_canvas_touchtbl[]={
  {DRAW_AREA_Y,DRAW_AREA_Y+DRAW_AREA_H,DRAW_AREA_X,DRAW_AREA_X+DRAW_AREA_W},
  {GFL_UI_TP_HIT_END,0,0,0},   // �I���f�[�^
};




//------------------------------------------------------------------
/**
 * �^�b�`�p�l���ŉ������{�^�����ւ���
 *
 * @param   wk    OEKAKI_WORK�̃|�C���^
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void NormalTouchFunc(OEKAKI_WORK *wk)
{
  int button=-1,touch;
  int decide = FALSE;
  
  // ���͏���

  // �J���[�؂�ւ�
  button=GFL_UI_TP_HitTrg( sub_button_hittbl );
  if( button != GFL_UI_TP_HIT_NONE ){
    switch(button){
    case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
      // �F�ύX
      if(wk->brush_color!=button){
        wk->brush_color = button;
        PalButtonAppearChange( wk->ButtonActWork, button);
        PMSND_PlaySE( OEKAKI_DECIDE_SE );
      }
      break;
    case 8:
      // �u��߂�v����������E�C���h�E�`��J�n
      if(wk->seq==OEKAKI_MODE){
        // �e�@�̏ꍇ�͐ڑ�����
        if(GFL_NET_SystemGetCurrentID()==0){
          //�R�l�N�g���ƕ\���l�����قȂ�ꍇ�́A��߂�{�^���𖳔����ɂ���
          if (wk->shareBit != _get_connect_bit(wk)){
            PMSND_PlaySE(OEKAKI_BS_SE);
            break;
          }
          CommStateSetLimitNum(wk,_get_connect_num(wk));
          wk->banFlag = OEKAKI_BAN_ON;
          // �u������������߂܂����H�v
          EndMessagePrint( wk, msg_oekaki_02, 1 );
          SetNextSequence( wk, OEKAKI_MODE_END_SELECT );
          EndButtonAppearChange( wk->EndIconActWork, TRUE );
          decide = TRUE;
          PMSND_PlaySE(OEKAKI_DECIDE_SE);
        }else{
          if(wk->AllTouchResult[0].banFlag == OEKAKI_BAN_ON ){
            // �e�@�ɋ֎~����Ă���Ƃ��͂r�d�̂�
            PMSND_PlaySE(OEKAKI_BS_SE);
          }else{
            // �u������������߂܂����H�v
            EndMessagePrint( wk, msg_oekaki_02, 1 );
            SetNextSequence( wk, OEKAKI_MODE_END_SELECT );
            EndButtonAppearChange( wk->EndIconActWork, TRUE );
            decide = TRUE;
            PMSND_PlaySE(OEKAKI_DECIDE_SE);
          }
        }

      }
      break;
    case 9: case 10: case 11:
      // �y����ύX
      {
        int i;
        for(i=0;i<3;i++){
          if((button-9)==i){
            GFL_CLACT_WK_SetAnmSeq( wk->ButtonActWork[CELL_BRUSH_NO+i], 29+2*i+1 );   // ON
          }else{
            GFL_CLACT_WK_SetAnmSeq( wk->ButtonActWork[CELL_BRUSH_NO+i], 29+2*i ); // OFF
          }
        }
        if(wk->brush!=(SMALL_BRUSH + button-9)){
          wk->brush = SMALL_BRUSH + button-9;
          PMSND_PlaySE(OEKAKI_PEN_CHANGE_SE);
        }
      }
      break;
    }
  }

  // �J�[�\���ʒu�ύX
  touch = GFL_UI_TP_HitCont( sub_canvas_touchtbl );

  GFL_CLACT_WK_SetDrawEnable( wk->MainActWork[GFL_NET_SystemGetCurrentID()], 0 );
  if(touch!=GFL_UI_TP_HIT_NONE){
    // �����̃J�[�\���̓^�b�`�p�l���̍��W�𔽉f������
    u32 x,y;
    GFL_UI_TP_GetPointCont( &x, &y );
    SetCursor_Pos( wk->MainActWork[GFL_NET_SystemGetCurrentID()], x, y );
    GFL_CLACT_WK_SetDrawEnable( wk->MainActWork[GFL_NET_SystemGetCurrentID()], 1 );
  }

  // �T���v�����O�����擾���Ċi�[
  {
    TP_ONE_DATA tpData;
    int i;
//  if(GFL_UI_TP_Main( &tpData, TP_BUFFERING_JUST, 64 )==TP_OK){
    if(GFL_UI_TP_GetCont()==TRUE){
      u32 x,y;
      GFL_UI_TP_GetPointCont( &x, &y );
      tpData.Size = 1;
      tpData.TPDataTbl[0].x = x;
      tpData.TPDataTbl[0].y = y;
      SetTouchpanelData( &wk->MyTouchResult, &tpData, wk->brush_color, wk->brush );

      if(decide == TRUE){
        wk->MyTouchResult.size = 0;
      }
    }else{
      tpData.Size = 0;
      tpData.TPDataTbl[0].x = 0;
      tpData.TPDataTbl[0].y = 0;
      SetTouchpanelData( &wk->MyTouchResult, &tpData, wk->brush_color, wk->brush );
    }
  }


}


//------------------------------------------------------------------
/**
 * @brief   �p���b�g�Ƃ�߂�{�^���̃Z���A�N�^�[����
 *
 * @param   act   �A�N�^�[�̃|�C���^
 * @param   button    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void PalButtonAppearChange( GFL_CLWK *act[], int no )
{
  int i;
  
  for(i=0;i<8;i++){
    // �����ꂽ�{�^���͂ւ��ށB�ق��̃{�^���͖߂�
    if(i==no){
      GFL_CLACT_WK_SetAnmSeq( act[i], pal_button_oam_table[i][2]+1 );
    }else{
      GFL_CLACT_WK_SetAnmSeq( act[i], pal_button_oam_table[i][2] );
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief   ��߂�{�^���̃I���E�I�t
 *
 * @param   act   �A�N�^�[�̃|�C���^
 * @param   flag    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void EndButtonAppearChange( GFL_CLWK *act, BOOL flag )
{
  if(flag==TRUE){
    GFL_CLACT_WK_SetAnmSeq( act, 9 );
  }else{
    GFL_CLACT_WK_SetAnmSeq( act, 1 );
  }
}

//------------------------------------------------------------------
/**
 * @brief   ���G�����{�[�h�ʏ폈��
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_MainNormal( OEKAKI_WORK *wk, int seq )
{
  NormalTouchFunc(wk);      //  �^�b�`�p�l������


  
  if(GFL_NET_SystemGetCurrentID()==0){      // �e�@�̎�
    if(OnlyParentCheck(wk)!=1){     // ��l����Ȃ����H
      LineDataSendRecv( wk );
    }
  }else{
      LineDataSendRecv( wk );
  }
  MoveCommCursor( wk );
  DrawBrushLine( wk->OekakiBoard, wk->AllTouchResult, wk->OldTouch, 1 );

  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �I���I�������̋��ʏ����֐�(���ŕ`�擙�j
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void EndSequenceCommonFunc( OEKAKI_WORK *wk )
{
  MoveCommCursor( wk );
  DrawBrushLine( wk->OekakiBoard, wk->AllTouchResult, wk->OldTouch, 0 );
  
}


//------------------------------------------------------------------
/**
 * @brief   �V�����q�@�������̂Őe�@���摜�𑗐M���͂��߂�
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static int Oekaki_NewMember( OEKAKI_WORK *wk, int seq )
{
  // ���������񂪂͂����Ă��܂���
  OS_Printf("newMember��%d\n",wk->newMemberId);
  if(GFL_NET_SystemGetCurrentID()==wk->newMemberId){
    EndMessagePrint(wk, msg_oekaki_14, 0);
  }else{
    EndMessagePrint(wk, msg_oekaki_01, 0);
  }
  SetNextSequence( wk, OEKAKI_MODE_NEWMEMBER_WAIT );

  PMSND_PlaySE(OEKAKI_NEWMEMBER_SE);

  // �摜�]����ԂɂȂ�����P�x�_�E��
  G2_SetBlendBrightness(  GX_BLEND_PLANEMASK_BG1|
              GX_BLEND_PLANEMASK_BG2|
              GX_BLEND_PLANEMASK_BG3,  -6);

  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  
  return seq;

}

//------------------------------------------------------------------
/**
 * @brief   �V�����q�@�p�̉摜��M�҂�
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static int Oekaki_NewMemberWait( OEKAKI_WORK *wk, int seq )
{
  int i;

  // ���W�f�[�^�������Ă��鎞�̓J�[�\�����W�𔽉f������
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    GFL_CLACT_WK_SetDrawEnable( wk->MainActWork[i], 0 );
  }
  return seq;
  
}

//------------------------------------------------------------------
/**
 * @brief   �q�@������M�I��
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_NewMemberEnd( OEKAKI_WORK *wk, int seq )
{
  // �P�x�_�E������
  G2_BlendNone();

  ChangeConnectMax(wk, 1);

  SetNextSequence( wk, OEKAKI_MODE );
  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���

  //�����ς�
  wk->bookJoin = 0;
  OS_Printf("��������\n");
  return seq;
  
}

//------------------------------------------------------------------
/**
 * @brief   ���G�����{�[�h�u��߂�v��I��������
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndSelectPutString( OEKAKI_WORK *wk, int seq )
{
  if( EndMessageWait( wk->printStream ) ){

    // �͂��E�������J�n
    wk->app_menuwork = YesNoMenuInit( wk );

    SetNextSequence( wk, OEKAKI_MODE_END_SELECT_WAIT );
  }
  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �u��߂܂����H�v�͂��E�������I��҂�
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndSelectWait( OEKAKI_WORK *wk, int seq )
{
  u32 result;

  // �둗�M��h��
  wk->MyTouchResult.size = 0;

  if(wk->AllTouchResult[0].banFlag==OEKAKI_BAN_ON && GFL_NET_SystemGetCurrentID()!=0){
    EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���

    if(FakeEndYesNoSelect(wk)){
      // �e�@�ɋ֎~����Ă���Ƃ��͂r�d�̂�
      PMSND_PlaySE(OEKAKI_BS_SE);
    }

    return seq;
  }

  if(GFL_NET_SystemGetCurrentID() == 0 && wk->ridatu_bit != 0){
    EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
    return seq;
  }
  
  if(MyStatusGetNum(wk) != _get_connect_num(wk)){
    //��v���Ă��Ȃ��Ȃ�u��߂�v�����Ȃ�(�q���ʂ邱���͐e�����X�V����Ȃ�shareNum�͌��Ȃ�)
    EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
    return seq;
  }

  result = YesNoMenuMain( wk );
  switch(result){       //��߂܂����H
//  case TOUCH_SW_RET_YES:            //�͂�
  case YESNO_RET_YES:
      if(GFL_NET_SystemGetCurrentID()==0){    
        SetNextSequence( wk, OEKAKI_MODE_END_SELECT_PARENT );
        EndMessagePrint( wk, msg_oekaki_05, 1 );    // ���[�_�[����߂�Ɓc
      }else{
        COMM_OEKAKI_END_CHILD_WORK coec;
        
        MI_CpuClear8(&coec, sizeof(COMM_OEKAKI_END_CHILD_WORK));
        coec.request = COEC_REQ_RIDATU_CHECK;
        coec.ridatu_id = GFL_NET_SystemGetCurrentID();
        
        wk->status_end = TRUE;
        wk->ridatu_wait = 0;
  
        SetNextSequence( wk, OEKAKI_MODE_END_SELECT_ANSWER_WAIT );
        GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(),CO_OEKAKI_END_CHILD, 
                          sizeof(COMM_OEKAKI_END_CHILD_WORK), &coec );
        BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_OFF );
      }
  
      //���������ĕ`��
      GFL_BMPWIN_MakeTransWindow( wk->OekakiBoard );
  
    break;
//  case TOUCH_SW_RET_NO:           //������
  case YESNO_RET_NO:
    SetNextSequence( wk, OEKAKI_MODE );
    EndButtonAppearChange( wk->EndIconActWork, FALSE );
    BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_OFF );
  
    //���������ĕ`��
    GFL_BMPWIN_MakeTransWindow( wk->OekakiBoard );
      
    // �e�@�͐ڑ����ۂ�����
    if(GFL_NET_SystemGetCurrentID()==0){
      CommStateSetLimitNum(wk, _get_connect_num(wk)+1);
      wk->banFlag = OEKAKI_BAN_OFF;
    }

    break;
  }

  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���

  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �u��߂܂����H�v���u�͂��v�A�Őe���痣�E���҂�
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndSelectAnswerWait( OEKAKI_WORK *wk, int seq )
{
  // �둗�M��h��
  wk->MyTouchResult.size = 0;
  
  wk->ridatu_wait = 0;
  
  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �u��߂܂����H�v���u�͂��v�A�Őe���痣�E���҂���OK!
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndSelectAnswerOK( OEKAKI_WORK *wk, int seq )
{
  // �둗�M��h��
  wk->MyTouchResult.size = 0;

  if((wk->oya_share_num != _get_connect_num(wk))
      || (wk->oya_share_num != MyStatusGetNum(wk))){
    OS_TPrintf("share_nuM = %d, Comm = %d, My = %d, Bit = %d\n", wk->oya_share_num, _get_connect_num(wk), MyStatusGetNum(wk), _get_connect_bit(wk));
    wk->ridatu_wait = 0;
    SetNextSequence( wk, OEKAKI_MODE_END_SELECT_ANSWER_NG );
    EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
    return seq;
  }
  
  wk->ridatu_wait++;
  OS_TPrintf("ridatu_wait = %d\n", wk->ridatu_wait);
  if(wk->ridatu_wait > 30){
    COMM_OEKAKI_END_CHILD_WORK coec;
    
    MI_CpuClear8(&coec, sizeof(COMM_OEKAKI_END_CHILD_WORK));
    coec.request = COEC_REQ_RIDATU_EXE;
    coec.ridatu_id = GFL_NET_SystemGetCurrentID();

    GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), CO_OEKAKI_END_CHILD, 
                      sizeof(COMM_OEKAKI_END_CHILD_WORK), &coec );

    wk->ridatu_wait = 0;
    SetNextSequence( wk, OEKAKI_MODE_END_CHILD );
  }


  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �u��߂܂����H�v���u�͂��v�A�Őe���痣�E���҂���NG!
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndSelectAnswerNG( OEKAKI_WORK *wk, int seq )
{
  // �둗�M��h��
  wk->MyTouchResult.size = 0;

  wk->status_end = FALSE;
  SetNextSequence( wk, OEKAKI_MODE );
  EndButtonAppearChange( wk->EndIconActWork, FALSE );
  
  OS_TPrintf("==========���E�����L�����Z���I===========\n");
  
  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;
}

// OEKAKI_MODE_END_CHILD
//------------------------------------------------------------------
/**
 * @brief   �q�@�I�����b�Z�[�W�J�n
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int       
 */
//------------------------------------------------------------------
static int  Oekaki_EndChild( OEKAKI_WORK *wk, int seq )
{
  // ������������߂܂���
  EndMessagePrint( wk, msg_oekaki_07, 1 );  

  SetNextSequence( wk, OEKAKI_MODE_END_CHILD_WAIT );

  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;
}
        

// OEKAKI_MODE_END_CHILD_WAIT
//------------------------------------------------------------------
/**
 * @brief   �q�@�I�����b�Z�[�W�\���I���҂�
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int       
 */
//------------------------------------------------------------------
static int  Oekaki_EndChildWait( OEKAKI_WORK *wk, int seq )
{     

  if( EndMessageWait( wk->printStream ) ){
    wk->wait = 0;
    SetNextSequence( wk, OEKAKI_MODE_END_CHILD_WAIT2 );
  }

  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;
  
}

//------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�\���I���㏭���҂�
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int       
 */
//------------------------------------------------------------------
static int  Oekaki_EndChildWait2( OEKAKI_WORK *wk, int seq )
{     

  if( ++wk->wait > OEKAKI_MESSAGE_END_WAIT ){
    WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_HOLEOUT, WIPE_TYPE_HOLEOUT, WIPE_FADE_BLACK, 16, 1, HEAPID_OEKAKI );
    seq = SEQ_OUT;            //�I���V�[�P���X��
  }

  EndSequenceCommonFunc( wk );      //�I���I�����̋��ʏ���
  return seq;
  
}

//------------------------------------------------------------------
/**
 * @brief   �e�@���I������Ƃ��͂�����x�u���[�_�[����߂�Ɓc�v�Ǝ��₷��
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndSelectParent( OEKAKI_WORK *wk, int seq )
{
  if( EndMessageWait( wk->printStream ) ){

    // �͂��E�������J�n
    wk->app_menuwork = YesNoMenuInit( wk );

    SetNextSequence( wk, OEKAKI_MODE_END_SELECT_PARENT_WAIT );

  }

  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;
  
}

//------------------------------------------------------------------
/**
 * @brief   �e�@�̂Q��ڂ́u�͂��E�������v
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndSelectParentWait( OEKAKI_WORK *wk, int seq )
{
  u32 result = YesNoMenuMain( wk );

  if(wk->shareNum != MyStatusGetNum(wk) //��v���Ă��Ȃ��Ȃ�u��߂�v�����Ȃ�
      || wk->ridatu_bit != 0){  //���E���悤�Ƃ��Ă���q������Ȃ狖���Ȃ�
    EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
    return seq;
  }

  switch(result){       //��߂܂����H
  case YESNO_RET_YES:            //�͂�
    SetNextSequence( wk, OEKAKI_MODE_FORCE_END );
    GFL_NET_SendData( GFL_NET_GetNetHandle( GFL_NET_NETID_SERVER), 
                      CO_OEKAKI_END, NULL, 0 );  //�I���ʒm
    // �e�@�i�����j�̖��O��WORDSET
    WORDSET_RegisterPlayerName( wk->WordSet, 0, 
                                Union_App_GetMystatus(wk->param->uniapp, 0)); 
    seq = SEQ_LEAVE;
    OS_Printf("OEKAKI_MODE_FORCE_END�ɂ�������\n");
    break;
  case YESNO_RET_NO:           //������
    SetNextSequence( wk, OEKAKI_MODE );
    EndButtonAppearChange( wk->EndIconActWork, FALSE );
    BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_OFF );

    // �e�@�͐ڑ����ۂ�����
    if(GFL_NET_SystemGetCurrentID()==0){
//      CommStateSetEntryChildEnable(TRUE);
      CommStateSetLimitNum( wk, _get_connect_num(wk)+1);
      wk->banFlag = OEKAKI_BAN_OFF;
    }
    break;
  }


  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;
  
}

//------------------------------------------------------------------
/**
 * @brief   �e�@���I���ƌ������̂ŏI���
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_ForceEnd( OEKAKI_WORK *wk, int seq )
{
  // �e�@�i�����j�̖��O��WORDSET
  WORDSET_RegisterPlayerName( wk->WordSet, 0, Union_App_GetMystatus(wk->param->uniapp, 0) ); 
  
  EndMessagePrint( wk, msg_oekaki_04, 1 );        // ���[�_�[���������̂ŉ��U���܂��B
  SetNextSequence( wk, OEKAKI_MODE_FORCE_END_WAIT );

  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;

}


//------------------------------------------------------------------
/**
 * @brief   �e�@���I���ƌ������̂ŏI���
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_ForceEndWait( OEKAKI_WORK *wk, int seq )
{
  if( EndMessageWait( wk->printStream ) ){
    SetNextSequence( wk, OEKAKI_MODE_FORCE_END_SYNCHRONIZE );
    GFL_NET_HANDLE_TimeSyncStart( GFL_NET_HANDLE_GetCurrentHandle(),
                                  OEKAKI_SYNCHRONIZE_END, WB_NET_PICTURE);
    OS_Printf("�����J�n\n");
  }

  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;

}

//------------------------------------------------------------------
/**
 * @brief   �S���̒ʐM������҂�
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_ForceEndSynchronize( OEKAKI_WORK *wk, int seq )
{
  GFL_NETHANDLE *pNet = GFL_NET_HANDLE_GetCurrentHandle();
  if(GFL_NET_HANDLE_IsTimeSync( pNet,OEKAKI_SYNCHRONIZE_END, WB_NET_PICTURE) || _get_connect_num(wk) == 1){
    OS_Printf("�I������������  seq = %d\n", seq);
    OS_Printf("�R�l�N�g�l��%d\n",_get_connect_num(wk));
//    wk->seq = OEKAKI_MODE_FORCE_END_WAIT_NOP;
    WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_HOLEOUT, WIPE_TYPE_HOLEOUT, WIPE_FADE_BLACK, 16, 1, HEAPID_OEKAKI );
    seq = SEQ_OUT;            //�I���V�[�P���X��
  }
  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;
  
}

//------------------------------------------------------------------
/**
 * @brief   �������Ȃ�
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_ForceEndWaitNop( OEKAKI_WORK *wk, int seq )
{
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �e��l�ɂȂ����̂ŏI��
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndParentOnly( OEKAKI_WORK *wk, int seq )
{
  EndMessagePrint( wk, msg_oekaki_06, 1 );        // ���[�_�[���������̂ŉ��U���܂��B
  SetNextSequence( wk, OEKAKI_MODE_END_PARENT_ONLY_WAIT );

  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���

  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   ��l�ɂȂ������͂�\�����ďI���V�[�P���X��
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndParentOnlyWait( OEKAKI_WORK *wk, int seq )
{
  if( EndMessageWait( wk->printStream ) ){
    SetNextSequence( wk, OEKAKI_MODE_END_CHILD_WAIT2 );
  }

  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;

}

//------------------------------------------------------------------
/**
 * @brief   ���������񂪂�����܂���
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_LogoutChildMes( OEKAKI_WORK *wk, int seq )
{
  // ���������񂪂�����܂���
//  if( EndMessageWait( wk->printStream ) ){
//    //�\�����̃��b�Z�[�W������ꍇ�͋�����~
//    //GF_STR_PrintForceStop(wk->MsgIndex);
//    PRINTSYS_PrintStreamDelete( wk->printStream );
//  }

  EndMessagePrint( wk, msg_oekaki_03, 1 );  
  SetNextSequence( wk, OEKAKI_MODE_LOGOUT_CHILD_WAIT );
  PMSND_PlaySE(OEKAKI_NEWMEMBER_SE);

  // �ڑ��\�l������U���݂̐ڑ��l���ɗ��Ƃ�
  if(GFL_NET_SystemGetCurrentID()==0){
    ChangeConnectMax( wk, 0 );
  }
  wk->err_num = _get_connect_num(wk);

  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���

  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �q�@�����������͏I���҂�
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_LogoutChildMesWait( OEKAKI_WORK *wk, int seq )
{
  // �ڑ��l�����P���邩�`�F�b�N
  if(wk->err_num != 0 && _get_connect_num(wk) != wk->err_num){
    wk->err_num = 0;
  }

  if( EndMessageWait( wk->printStream ) ){
    SetNextSequence( wk, OEKAKI_MODE_LOGOUT_CHILD_CLOSE );
    wk->wait = 0;
  }

  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;

}

//------------------------------------------------------------------
/**
 * @brief   �\���I���E�F�C�g
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int       
 */
//------------------------------------------------------------------
static int  Oekaki_LogoutChildClose( OEKAKI_WORK *wk, int seq )
{     
  // �ڑ��l�����P����܂ł͑҂�
  if(wk->err_num != 0 && _get_connect_num(wk) != wk->err_num){
    wk->err_num = 0;
  }

  if( ++wk->wait > OEKAKI_MESSAGE_END_WAIT && wk->err_num == 0 ){
    EndMessageWindowOff( wk );
    SetNextSequence( wk, OEKAKI_MODE );
    if(GFL_NET_SystemGetCurrentID() == 0){
      wk->banFlag = OEKAKI_BAN_OFF;
      ChangeConnectMax(wk, 1);
    }
  }

  EndSequenceCommonFunc( wk );      //�I���I�����̋��ʏ���
  return seq;
  
}




//==============================================================================
/**
 * @brief   �ǂ�ȏ�Ԃł����Ă������I�Ƀ��C���V�[�P���X�`�F���W
 *
 * @param   wk    
 * @param   seq   
 * @param   id    �ʐMID
 *
 * @retval  none    
 */
//==============================================================================
void OekakiBoard_MainSeqForceChange( OEKAKI_WORK *wk, int seq, u8 id  )
{
  switch(seq){
  case OEKAKI_MODE_NEWMEMBER: 
    if(wk->seq==OEKAKI_MODE_END_SELECT_WAIT || wk->seq==OEKAKI_MODE_END_SELECT_PARENT_WAIT){
      OekakiResetYesNoWin(wk);
    }
    EndButtonAppearChange( wk->EndIconActWork, FALSE );
    // �w��̎q�@�̖��O��WORDSET�ɓo�^�i���E�E������)
    WORDSET_RegisterPlayerName( wk->WordSet, 0, Union_App_GetMystatus(wk->param->uniapp, id) );  
    wk->newMemberId = id;
    wk->ridatu_bit = 0;
    OS_Printf("�V�����l��ID %d\n",id);
    break;
  case OEKAKI_MODE_NEWMEMBER_END:
    EndMessageWindowOff( wk );
    break;
  case OEKAKI_MODE_LOGOUT_CHILD:
    if(wk->status_end == TRUE){
      return; //�������g�����E������
    }
    WORDSET_RegisterPlayerName( wk->WordSet, 0, Union_App_GetMystatus(wk->param->uniapp, id) );  
    if(id==GFL_NET_SystemGetCurrentID()){
      // ���������E����q�@�������ꍇ�́u�q�@�����Ȃ��Ȃ���v�Ƃ͌���Ȃ�
      return;
    }

    if(GFL_NET_SystemGetCurrentID() == 0){
      wk->ridatu_bit &= 0xffff ^ id;
    }
    if(wk->seq==OEKAKI_MODE_END_SELECT_WAIT || wk->seq==OEKAKI_MODE_END_SELECT_PARENT_WAIT){
      OekakiResetYesNoWin(wk);

      //���������ĕ`��
      GFL_BMPWIN_MakeTransWindow( wk->OekakiBoard );
    }
    EndButtonAppearChange( wk->EndIconActWork, FALSE );
    break;
  case OEKAKI_MODE_FORCE_END:
    break;
  case OEKAKI_MODE_END_SELECT_ANSWER_OK:
    break;
  case OEKAKI_MODE_END_SELECT_ANSWER_NG:
    break;
  default:
    GF_ASSERT( "�w��ȊO�̃V�[�P���X�`�F���W������" );
    return;
  }
  SetNextSequence( wk, seq );

}

//==============================================================================
/**
 * @brief   �������������Ă��烁�C�����[�v���V�[�P���X�`�F���W(�ʏ��Ԃł��邱�Ɓj
 *
 * @param   wk    
 * @param   seq   ���̃��C���V�[�P���X�Ƀ`�F���W������
 *
 * @retval  none    
 */
//==============================================================================
void OekakiBoard_MainSeqCheckChange( OEKAKI_WORK *wk, int seq, u8 id  )
{
  // �ʏ��ԂȂ�
  if(wk->seq == OEKAKI_MODE){
    OS_Printf("�q�@%d�̖��O��o�^\n",id);
    switch(seq){
    case OEKAKI_MODE_LOGOUT_CHILD:
      WORDSET_RegisterPlayerName( wk->WordSet, 0, Union_App_GetMystatus(wk->param->uniapp, id) );  
      if(id==GFL_NET_SystemGetCurrentID()){
        // ���������E����q�@�������ꍇ�́u�q�@�����Ȃ��Ȃ���v�Ƃ͌���Ȃ�
        return;
      }
      if(GFL_NET_SystemGetCurrentID() == 0){
        wk->ridatu_bit &= 0xffff ^ id;
      }
      SetNextSequence( wk, seq );
      break;
    
    default:
      GF_ASSERT( 0&&"�w��ȊO�̃V�[�P���X�`�F���W������" );
      return;
    }
  }

}



//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//11520
//---------------------------------------------------------
// �u���V�p�^�[��
//---------------------------------------------------------

// �ǂ����Ă��p���b�g�f�[�^��3bit�ɏk�߂Ă��܂������̂ŁA�����F��8�ɂ���
// �F�w�莩�̂�0-7�ɓ��ĂĂ���B0�͓����F����BD�ʂ����ɂ��č���������悤�ɂ��Ă���

// BMP�f�[�^�͍Œቡ8dot���K�v�Ȃ̂ŁA4x4�̃h�b�g�f�[�^����肽������
// �Q�o�C�g���ƂɎQ�Ƃ���Ȃ��f�[�^�������Q�o�C�g�K�v
static const u8 oekaki_brush[3][8][24]={

{ /*  0  */              /*  1  */               /*  2  */              /*  3  */
  {0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,  0x00,0x11,  0x00,0x00,  0x00,0x11, 0x00,0x00,  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,  0x00,0x22,  0x00,0x00,  0x00,0x22, 0x00,0x00,  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,  0x00,0x33,  0x00,0x00,  0x00,0x33, 0x00,0x00,  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,  0x00,0x44,  0x00,0x00,  0x00,0x44, 0x00,0x00,  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,  0x00,0x55,  0x00,0x00,  0x00,0x55, 0x00,0x00,  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,  0x00,0x66,  0x00,0x00,  0x00,0x66, 0x00,0x00,  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,  0x00,0x77,  0x00,0x00,  0x00,0x77, 0x00,0x00,  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,  0x00,0x88,  0x00,0x00,  0x00,0x88, 0x00,0x00,  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
},
{ /*  0  */              /*  1  */               /*  2  */              /*  3  */
  {0x00,0x00, 0x00,0x00, 0x00,0x11, 0x00,0x00,  0x10,0x11,  0x01,0x00,  0x10,0x11, 0x01,0x00,  0x00,0x11, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x22, 0x00,0x00,  0x20,0x22,  0x02,0x00,  0x20,0x22, 0x02,0x00,  0x00,0x22, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x33, 0x00,0x00,  0x30,0x33,  0x03,0x00,  0x30,0x33, 0x03,0x00,  0x00,0x33, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x44, 0x00,0x00,  0x40,0x44,  0x04,0x00,  0x40,0x44, 0x04,0x00,  0x00,0x44, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x55, 0x00,0x00,  0x50,0x55,  0x05,0x00,  0x50,0x55, 0x05,0x00,  0x00,0x55, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x66, 0x00,0x00,  0x60,0x66,  0x06,0x00,  0x60,0x66, 0x06,0x00,  0x00,0x66, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x77, 0x00,0x00,  0x70,0x77,  0x07,0x00,  0x70,0x77, 0x07,0x00,  0x00,0x77, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x88, 0x00,0x00,  0x80,0x88,  0x08,0x00,  0x80,0x88, 0x08,0x00,  0x00,0x88, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
},                                                                     
{   /*  0  */              /*  1  */               /*  2  */              /*  3  */
  {0x10,0x11, 0x01,0x00, 0x11,0x11, 0x11,0x00,  0x11,0x11,  0x11,0x00,  0x11,0x11, 0x11,0x00,  0x11,0x11, 0x11,0x00, 0x10,0x11, 0x01,0x00,},
  {0x20,0x22, 0x02,0x00, 0x22,0x22, 0x22,0x00,  0x22,0x22,  0x22,0x00,  0x22,0x22, 0x22,0x00,  0x22,0x22, 0x22,0x00, 0x20,0x22, 0x02,0x00,},
  {0x30,0x33, 0x03,0x00, 0x33,0x33, 0x33,0x00,  0x33,0x33,  0x33,0x00,  0x33,0x33, 0x33,0x00,  0x33,0x33, 0x33,0x00, 0x30,0x33, 0x03,0x00,},
  {0x40,0x44, 0x04,0x00, 0x44,0x44, 0x44,0x00,  0x44,0x44,  0x44,0x00,  0x44,0x44, 0x44,0x00,  0x44,0x44, 0x44,0x00, 0x40,0x44, 0x04,0x00,},
  {0x50,0x55, 0x05,0x00, 0x55,0x55, 0x55,0x00,  0x55,0x55,  0x55,0x00,  0x55,0x55, 0x55,0x00,  0x55,0x55, 0x55,0x00, 0x50,0x55, 0x05,0x00,},
  {0x60,0x66, 0x06,0x00, 0x66,0x66, 0x66,0x00,  0x66,0x66,  0x66,0x00,  0x66,0x66, 0x66,0x00,  0x66,0x66, 0x66,0x00, 0x60,0x66, 0x06,0x00,},
  {0x70,0x77, 0x07,0x00, 0x77,0x77, 0x77,0x00,  0x77,0x77,  0x77,0x00,  0x77,0x77, 0x77,0x00,  0x77,0x77, 0x77,0x00, 0x70,0x77, 0x07,0x00,},
  {0x80,0x88, 0x08,0x00, 0x88,0x88, 0x88,0x00,  0x88,0x88,  0x88,0x00,  0x88,0x88, 0x88,0x00,  0x88,0x88, 0x88,0x00, 0x80,0x88, 0x08,0x00,},
},
};

/*


















*/



//==============================================================================
/**
 * @brief   �`��J�n�ʒu���}�C�i�X�����ɂ����Ă��`��ł���BmpWinPrint���b�p�[
 * @retval  none    
 */
//==============================================================================
static void _BmpWinPrint_Rap(
      GFL_BMPWIN * win, void * src,
      int src_x, int src_y, int src_dx, int src_dy,
      int win_x, int win_y, int win_dx, int win_dy )
{
  GFL_BMP_DATA *src_win;
  // X�`��J�n�|�C���g���}�C�i�X��
  if(win_x < 0){
    int diff;
    diff = win_x*-1;
    if(diff>win_dx){  // ���̃}�C�i�X���͓]�����𒴂��ĂȂ���
      diff = win_dx;  // �����Ă���]�����Ɠ����ɂ���
    }

    // �]���Q�ƊJ�n�|�C���g�ƁA�]����������
    win_x   = 0;
    src_x  += diff;
    src_dx -= diff;
    win_dx -= diff;
  }

  // Y�`��J�n�|�C���g���}�C�i�X��
  if(win_y < 0){
    int diff;
    diff = win_y*-1;
    if(diff>win_dy){  // ���̃}�C�i�X���͓]�����𒴂��ĂȂ���
      diff = win_dy;  // �����Ă���]�����Ɠ����ɂ���
    }

    // �]���Q�ƊJ�n�|�C���g�ƁA�]����������
    win_y   = 0;
    src_y  += diff;
    src_dy -= diff;
    win_dy -= diff;
  }

//  GFL_BMPWINPrint( win, src, src_x, src_y, src_dx, src_dy, win_x, win_y, win_dx, win_dy );
  src_win =  GFL_BMP_CreateWithData( src, 8, 8, GFL_BMP_16_COLOR, HEAPID_OEKAKI );
  GFL_BMP_Print( src_win, GFL_BMPWIN_GetBmp(win), src_x, src_y, win_x, win_y, src_dx, src_dy, 0 );
  GFL_BMP_Delete( src_win );
}


#define POINT_W ( 6 )
#define POINT_H ( 6 )
//------------------------------------------------------------------
/**
 * @brief   ���C���`��
 *
 * @param   win   
 * @param   brush   
 * @param   px    
 * @param   py    
 * @param   sx    
 * @param   sy    
 * @param   count   
 * @param   flag    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void DrawPoint_to_Line( 
  GFL_BMPWIN *win, 
  const u8 *brush, 
  int px, int py, int *sx, int *sy, 
  int count, int flag )
{
  int dx, dy, s, step;
  int x1 = *sx;
  int y1 = *sy;
  int x2 = px;
  int y2 = py;

  // ����͌��_�ۑ��̂�
  if(count==0 && flag == 0){
    *sx = px;   *sy = py;
    return;
  }
  

  dx = MATH_IAbs(x2 - x1);  dy = MATH_IAbs(y2 - y1);
  if (dx > dy) {
    if (x1 > x2) {
      step = (y1 > y2) ? 1 : -1;
      s = x1;  x1 = x2;  x2 = s;  y1 = y2;
    } else step = (y1 < y2) ? 1: -1;
    _BmpWinPrint_Rap( win, (void*)brush,  0, 0, POINT_W, POINT_H, x1, y1, POINT_W, POINT_H );
    s = dx >> 1;
    while (++x1 <= x2) {
      if ((s -= dy) < 0) {
        s += dx;  y1 += step;
      };
      _BmpWinPrint_Rap( win, (void*)brush,  0, 0, POINT_W, POINT_H, x1, y1, POINT_W, POINT_H );
    }
  } else {
    if (y1 > y2) {
      step = (x1 > x2) ? 1 : -1;
      s = y1;  y1 = y2;  y2 = s;  x1 = x2;
    } else step = (x1 < x2) ? 1 : -1;
    _BmpWinPrint_Rap( win, (void*)brush,  0, 0, POINT_W, POINT_H, x1, y1, POINT_W, POINT_H );
    s = dy >> 1;
    while (++y1 <= y2) {
      if ((s -= dx) < 0) {
        s += dy;  x1 += step;
      }
      _BmpWinPrint_Rap( win, (void*)brush,  0, 0, POINT_W, POINT_H, x1, y1, POINT_W, POINT_H );
    }
  }
  
  
  *sx = px;     *sy = py;

}

//----------------------------------------------------------------------------------
/**
 * @brief �`��I����̃^�b�`���W��ۑ��p���[�N�Ɋi�[����
 *
 * @param   all   
 * @param   stock 
 */
//----------------------------------------------------------------------------------
static void Stock_OldTouch( TOUCH_INFO *all, OLD_TOUCH_INFO *stock )
{
  int i;
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    stock[i].size = all[i].size;
    if(all[i].size!=0){
      stock[i].x = all[i].x[all[i].size-1];
      stock[i].y = all[i].y[all[i].size-1];
    }
  }
}


static int debug_count;
//------------------------------------------------------------------
/**
 * @brief  �ʐM�Ŏ�M�����^�b�`�p�l���̌��ʃf�[�^�����ɕ`�悷��
 *
 * @param   win   
 * @param   all   
 * @param   draw  ��������ōs����CGX�ύX��]�����邩�H(0:���Ȃ�  1:����j
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void DrawBrushLine( GFL_BMPWIN *win, TOUCH_INFO *all, OLD_TOUCH_INFO *old, int draw )
{
  int px,py,i,r,flag=0, sx, sy;

//  OS_Printf("id0=%d,id1=%d,id2=%d,id3=%d,id4=%d\n",all[0].size,all[1].size,all[2].size,all[3].size,all[4].size);

  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    if(all[i].size!=0){
      if(old[i].size){
        sx = old[i].x-9;
        sy = old[i].y-17;
      }
      for(r=0;r<all[i].size;r++){
        px = all[i].x[r] - 9;
        py = all[i].y[r] - 17;
        // BG1�ʗpBMP�i���G�����摜�j�E�C���h�E�m��
        DrawPoint_to_Line(win, oekaki_brush[all[i].brush][all[i].color], px, py, &sx, &sy, r, old[i].size);
        flag = 1;
      }
      
    }
  }
  if(flag && draw){
    
//    OS_Printf("write board %d times\n",debug_count++);
    GFL_BMPWIN_MakeTransWindow( win );
  }
  
  // ����̍ŏI���W�̃o�b�N�A�b�v�����   
  Stock_OldTouch(all, old);
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    all[i].size = 0;    // ��x�`�悵������W���͎̂Ă�
  }
  
}

//------------------------------------------------------------------
/**
 * @brief   �ʐM�f�[�^����J�[�\���ʒu���ړ�������
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void MoveCommCursor( OEKAKI_WORK *wk )
{
  int i;
  TOUCH_INFO *all = wk->AllTouchResult;
  
  // ���W�f�[�^�������Ă��鎞�̓J�[�\�����W�𔽉f������
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    if(i!=GFL_NET_SystemGetCurrentID()){    // �����̃J�[�\���̓^�b�`�p�l�����璼�ڂƂ�
      if(all[i].size!=0 && Union_App_GetMystatus(wk->param->uniapp,i)!=NULL){
        GFL_CLACT_WK_SetDrawEnable( wk->MainActWork[i], 1 );
        SetCursor_Pos( wk->MainActWork[i], all[i].x[all[i].size-1],  all[i].y[all[i].size-1]);
      }else{
        GFL_CLACT_WK_SetDrawEnable( wk->MainActWork[i], 0 );
      }
    }
  }
  
}
//------------------------------------------------------------------
/**
 * @brief   �f�o�b�O�p�Ɏ����Ŏ擾����������M�o�b�t�@�ɃR�s�[����
 *
 * @param   wk    
 *
 * @retval  static    
 */
//------------------------------------------------------------------
static void DebugTouchDataTrans( OEKAKI_WORK *wk )
{
  wk->AllTouchResult[0] = wk->MyTouchResult;
}


//------------------------------------------------------------------
/**
 * �J�[�\���̃p���b�g�ύX�i�_�Łj
 *
 * @param   CursorCol sin�ɓn���p�����[�^�i360�܂Łj
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void CursorColTrans(u16 *CursorCol)
{
  fx32  sin;
  GXRgb tmp;
  int   r,g,b;

  *CursorCol+=20;
  if(*CursorCol>360){
    *CursorCol = 0;
  }

  sin = GFL_CALC_Sin360R(*CursorCol);
  g   = 15 +( sin * 10 ) / FX32_ONE;
  tmp = GX_RGB(29,g,0);


  GX_LoadOBJPltt((u16*)&tmp, ( 12 )*2, 2);
}

//------------------------------------------------------------------
/**
 * @brief   ���݉��l�ڑ����Ă��邩�H
 *
 * @param   none    
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int OnlyParentCheck( OEKAKI_WORK *wk )
{
  int i,result;
  const MYSTATUS *status;

  result = 0;
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    status = Union_App_GetMystatus(wk->param->uniapp,i);
    if(status!=NULL){
      result++;
    }
  }

  return result;
}


//------------------------------------------------------------------
/**
 * @brief   �I�����C���󋵂��m�F���ď��ʂɖ��O��\������
 *
 * @param   win   
 * @param   frame   
 * @param   color   
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void NameCheckPrint( GFL_BMPWIN *win[], PRINTSYS_LSB color, OEKAKI_WORK *wk )
{
  int i,id = GFL_NET_SystemGetCurrentID();
  int num;

  // ���O�擾�̏󋵂ɕω��������ꍇ�͏��������Ȃ�
  if(!MyStatusCheck(wk)){
    return;
  }

  // ���O�C���󋵂ɉ����ď��ʂ̖���ON�EOFF����
  for(i=0;i<5;i++){
    if(wk->TrainerStatus[i][0]==NULL){
      GFL_CLACT_WK_SetDrawEnable( wk->SubActWork[i], 0 );
    }else{
      GFL_CLACT_WK_SetDrawEnable( wk->SubActWork[i], 1 );
    }
  }

  // ���ꂼ��̕����p�l���̔w�i�F�ŃN���A
  for(i=0;i<5;i++){
    GFL_BMP_Fill( GFL_BMPWIN_GetBmp(win[i]), 0,0,OEKAKI_NAME_BMP_W*8, OEKAKI_NAME_BMP_H*8, 0 );
  }

  // �`��
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    if(wk->TrainerStatus[i][0]!=NULL){
      MyStatus_CopyNameString( wk->TrainerStatus[i][0], wk->TrainerName[i] );
      OS_Printf("name print id=%d\n", i);
      if(id==i){
          PRINT_UTIL_PrintColor( &wk->printUtil[OEKAKI_PRINT_UTIL_NAME_WIN0+i], wk->printQue, 
                                  0, 0, wk->TrainerName[i], wk->font, NAME_COL_MINE );

      }else{
          PRINT_UTIL_PrintColor( &wk->printUtil[OEKAKI_PRINT_UTIL_NAME_WIN0+i], wk->printQue, 
                                  0, 0, wk->TrainerName[i], wk->font, color );

      }
    }
    GFL_BMPWIN_MakeTransWindow( win[i] );
  }
  OS_Printf("���O�����������܂���\n");

}









#define PLATE_CHARA_OFFSET1 ( 12 )
#define PLATE_CHARA_OFFSET2 ( 16 )

static const u8 plate_num[4]={2,2,3,2};
static const u8 plate_table[4][3]={
  {8*8,17*8,0,},
  {8*8,17*8,0,},
  {5*8,12*8,18*8,},
  {8*8,17*8,0,},

};
static const u8 plate_chara_no[][5]={
  {PLATE_CHARA_OFFSET1,PLATE_CHARA_OFFSET1,},
  {PLATE_CHARA_OFFSET1,PLATE_CHARA_OFFSET1,},
  {PLATE_CHARA_OFFSET1,PLATE_CHARA_OFFSET1,PLATE_CHARA_OFFSET1,},
  {PLATE_CHARA_OFFSET1,PLATE_CHARA_OFFSET1,},
};

//------------------------------------------------------------------
/**
 * @brief   �ڑ����m�F���ꂽ�ꍇ�͖��O���擾����
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static int ConnectCheck( OEKAKI_WORK *wk )
{
  int i,result=0;
  const MYSTATUS *status;
  STRCODE  *namecode;

  // �ڑ��`�F�b�N
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    wk->ConnectCheck[i][0] = wk->ConnectCheck[i][1];

    if(Union_App_GetMemberNetBit(wk->param->uniapp)&(1<<i)){
      wk->ConnectCheck[i][0] = 1;
    }
//    wk->ConnectCheck[i][0] = CommIsConnect(i);

  }

  // �ڑ��������疼�O�𔽉f������
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    if(wk->ConnectCheck[i][0]){       // �ڑ����Ă��邩�H

      status = Union_App_GetMystatus(wk->param->uniapp,i);
      if(status!=NULL){         // MYSTATUS�͎擾�ł��Ă��邩�H
        namecode = (STRCODE*)MyStatus_GetMyName(status);
        GFL_STR_SetStringCode( wk->TrainerName[i], namecode );
      }

    }
  }
  return 0;
}


//------------------------------------------------------------------
/**
 * @brief   �^�b�`�p�l�����̑���M���s��
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void LineDataSendRecv( OEKAKI_WORK *wk )
{
  if( GFL_NET_SystemGetCurrentID()==0 ){
    GFL_NETHANDLE *pNet = GFL_NET_HANDLE_GetCurrentHandle();

    // �e�@�͎����̃^�b�`�p�l������ǉ����đ��M����
    if(GFL_NET_IsEmptySendData(pNet)){  // �p�P�b�g���󂢂Ă�Ȃ�
      wk->MyTouchResult.banFlag    = wk->banFlag;
      wk->ParentTouchResult[0] = wk->MyTouchResult;
      GFL_NET_SendData( pNet, CO_OEKAKI_LINEPOS_SERVER, 
                        COMM_SEND_5TH_PACKET_MAX*OEKAKI_MEMBER_MAX, wk->ParentTouchResult );
    }
  }else{
    GFL_NETHANDLE *pNet = GFL_NET_HANDLE_GetCurrentHandle();
    if(GFL_NET_IsEmptySendData(pNet)){
      // �q�@�͎����̃^�b�`�p�l������e�@�ɑ��M����
      GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), CO_OEKAKI_LINEPOS, 
                        COMM_SEND_5TH_PACKET_MAX, &wk->MyTouchResult );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief   ���݂̃I�����C�������擾
 *
 * @param   none    
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int MyStatusGetNum( OEKAKI_WORK *wk )
{
  return Union_App_GetMemberNum( wk->param->uniapp );
}


//------------------------------------------------------------------
/**
 * @brief   MYSTATUS�̎擾�󋵂ɕω������������H
 *
 * @param   wk    
 *
 * @retval  int   ��������1,  �����ꍇ��0
 */
//------------------------------------------------------------------
static int MyStatusCheck( OEKAKI_WORK *wk )
{
  int i,result=0;
  // �ڑ��������疼�O�𔽉f������
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    wk->TrainerStatus[i][1] = wk->TrainerStatus[i][0];
    wk->TrainerStatus[i][0] = Union_App_GetMystatus(wk->param->uniapp,i);
  }

  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    if(wk->TrainerStatus[i][1] != wk->TrainerStatus[i][0]){
      result = 1;
    }
  }

  return result;
}


//------------------------------------------------------------------
/**
 * @brief   ��b�E�C���h�E�\��
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void EndMessagePrint( OEKAKI_WORK *wk, int msgno, int wait )
{
  // ������擾
  STRBUF *tempbuf;
  
  tempbuf = GFL_STR_CreateBuffer(TALK_MESSAGE_BUF_NUM,HEAPID_OEKAKI);
  GFL_MSG_GetString(  wk->MsgManager, msgno, tempbuf );
  WORDSET_ExpandStr( wk->WordSet, wk->TalkString, tempbuf );
  GFL_STR_DeleteBuffer(tempbuf);

  // ��b�E�C���h�E�g�`��
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin),  0x0f0f );
  BmpWinFrame_Write( wk->MsgWin, WINDOW_TRANS_ON, 1, MESFRAME_PAL );


  // ���b�Z�[�W�X�s�[�h���w��
  if(wait==0){
    PRINT_UTIL_PrintColor( &wk->printUtil[OEKAKI_PRINT_UTIL_MSG], wk->printQue, 
                           0, 0, wk->TalkString, wk->font, STRING_COL_MSG );
  }else{
    wk->printStream = PRINTSYS_PrintStream( wk->MsgWin, 0, 0, wk->TalkString, wk->font,
                                            MSGSPEED_GetWait(), wk->pMsgTcblSys, 
                                            1, HEAPID_OEKAKI, 0x0f0f );
  }

  // ������`��J�n
//  wk->MsgIndex = GF_STR_PrintSimple( &wk->MsgWin, FONT_TALK, wk->TalkString, 0, 0, wait, NULL);
  GFL_BMPWIN_MakeTransWindow( wk->MsgWin );
}

//------------------------------------------------------------------
/**
 * @brief   ��b�\���E�C���h�E�I���҂�
 *
 * @param   msg_index   
 *
 * @retval  int   0:�\����  1:�I��
 */
//------------------------------------------------------------------
static int EndMessageWait( PRINT_STREAM *stream )
{
  PRINTSTREAM_STATE state;
  state = PRINTSYS_PrintStreamGetState( stream );
  if(state==PRINTSTREAM_STATE_PAUSE){
    if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_DECIDE){
      PRINTSYS_PrintStreamReleasePause( stream );
    }
    return 0;
  }else if(state==PRINTSTREAM_STATE_DONE){
    PRINTSYS_PrintStreamDelete( stream );
    return 1;
  }
  return 0;
}


//------------------------------------------------------------------
/**
 * @brief   
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void EndMessageWindowOff( OEKAKI_WORK *wk )
{
  BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );
}


//------------------------------------------------------------------
/**
 * @brief   �ڑ��l�������̎��ڑ��l���{�P�ɂȂ�悤�ɍĐݒ肷��
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void ChangeConnectMax( OEKAKI_WORK *wk, int plus )
{
  if(GFL_NET_SystemGetCurrentID()==0){
    int num = _get_connect_num(wk)+plus;
    if(num>5){
      num = 5;
    }
    CommStateSetLimitNum(wk, num);
    OS_Printf("�ڑ��l���� %d�l�ɕύX\n",num);
  }

}

//------------------------------------------------------------------
/**
 * @brief  �ڑ��l�����Ď����Đ����ς��� 
 *
 * @param   wk    
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int ConnectNumControl( OEKAKI_WORK *wk )
{
  int num;
  // �r�[�R��������������
  num = MyStatusGetNum(wk);

  switch(num){
  case 1:
    if(wk->seq<=OEKAKI_MODE_END_SELECT_PARENT_WAIT){
      if (_get_connect_bit(wk) != 1){
        OS_Printf("��l�ł͂Ȃ��Ȃ�܂����B\n");
        OS_Printf("bit:%d\n",_get_connect_bit(wk));
        wk->ireagalJoin = 1;
      }
      OS_Printf("OEKAKI_MODE_END_PARENT_ONLY�ɂ�������\n");
//      wk->seq = OEKAKI_MODE_END_PARENT_ONLY;
      wk->next_seq = OEKAKI_MODE_END_PARENT_ONLY; //�\��
      OS_Printf("�ڑ��������P�ɂ���");
          CommStateSetLimitNum(wk,1);
      // ��߂�I�𒆂������狭�����Z�b�g
      if(wk->yesno_flag){
        OekakiResetYesNoWin(wk);
        GFL_CLACT_WK_SetAnmSeq( wk->ButtonActWork[8], pal_button_oam_table[8][2] );
      }
      return SEQ_LEAVE;
    }
    break;
  case 2:case 3:case 4:
    // �܂�������
    // �ڑ��l�����������ꍇ�͐ڑ��ő�l�������炷
    if(num<wk->connectBackup){
      if(wk->banFlag==OEKAKI_BAN_ON){
        ChangeConnectMax( wk, 0 );
      }else{
        ChangeConnectMax( wk, 1 );
      }
    }
    break;
  case 5:
    // �����ł��B
    break;
  }

  // �摜���L�l�����������Ƃ��͍X�V
  if(num<wk->connectBackup){
    wk->shareNum = _get_connect_num(wk);
    wk->shareBit = _get_connect_bit(wk);
    OS_TPrintf("�ڑ��l�����������̂�shareNum��%d�l�ɕύX\n", _get_connect_num(wk));
    /*�������t���O�������Ă����ԂŁA�l����������*/
    if(wk->bookJoin){/*�����\��r�b�g�Ɣ�r���A���ꂪ�����Ă����ꍇ�́A�����҂��d����؂����Ƃ݂Ȃ�*/
      if (!(wk->shareBit&wk->joinBit)){
        //���~�b�g�����Đݒ�
        ChangeConnectMax( wk, 1 );
        //���E����
        wk->banFlag = OEKAKI_BAN_OFF;
        //�����\��L�����Z��
        wk->bookJoin = 0;
        wk->joinBit = 0;
        OS_Printf("�����L�����Z������܂���\n");
      }
    }
  }

  // �ڑ��l����ۑ�
  wk->connectBackup = MyStatusGetNum(wk);

  // �ʐM�ڑ��l�����摜���L�l�����������Ȃ����ꍇ�͗��E�֎~�t���O�𗧂Ă�
  if(wk->shareNum < _get_connect_num(wk)){
    OS_Printf("ban_flag_on:%d,%d\n",wk->shareNum, _get_connect_num(wk));
    OS_Printf("�����\��\n");
    wk->banFlag = OEKAKI_BAN_ON;
    //���������҂���
    wk->bookJoin = 1;
    //�������悤�Ƃ��Ă���l�̃r�b�g���擾
    wk->joinBit = wk->shareBit^_get_connect_bit(wk);
//    GF_ASSERT( (wk->joinBit == 2)||
//          (wk->joinBit == 4)||
//          (wk->joinBit == 8)||
//          (wk->joinBit == 16) );
    
  }

  return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   ���Ɉڍs���������V�[�P���X�ԍ�������
 *
 * @param   wk    
 * @param   nextSequence    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void SetNextSequence( OEKAKI_WORK *wk, int nextSequence )
{
  wk->next_seq = nextSequence;
}


//------------------------------------------------------------------
/**
 * @brief   wk->seq�̃R���g���[�����s��
 *
 * @param   wk    
 *
 * @retval  void      
 */
//------------------------------------------------------------------
static void OekakiSequenceControl( OEKAKI_WORK *wk, int proc_seq )
{
  // �V�[�P���X���Ⴄ�Ȃ�
  if(wk->seq!=wk->next_seq){
    // �I���V�[�P���X����Ȃ����Ƃ��m�F����
    if(proc_seq!=SEQ_LEAVE){ 
      wk->seq = wk->next_seq;
    }else{
      // �ꕔ���s�����̃V�[�P���X����Ȃ����Ƃ��m�F����
      if(FuncTable[wk->next_seq].execOn_SeqLeave){
        OS_Printf("now_next:%d,%d\n",wk->seq,wk->next_seq);
        // �V�[�P���X��������
        wk->seq = wk->next_seq;
      }
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief   �^�b�`�p�l���̂͂��E�������E�B���h�E�̏����֐�
 *
 * @param   wk    
 *
 * @retval  void      
 */
//------------------------------------------------------------------
static void OekakiResetYesNoWin(OEKAKI_WORK *wk)
{
  if(wk->yesno_flag){
    if(wk->app_menuwork!=NULL){
      APP_TASKMENU_CloseMenu( wk->app_menuwork );
    }
    wk->yesno_flag = 0;
  }
}


//------------------------------------------------------------------
/**
 * @brief   YESNO�{�^�����֎~���Ă���ԁA�R�̌��o���s��
 *
 * @param   wk    
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int FakeEndYesNoSelect( OEKAKI_WORK  *wk )
{

  int button=GFL_UI_TP_HitTrg( fake_yesno_hittbl );
  if( button != GFL_UI_TP_HIT_NONE ){
    return 1;
  }
  return 0;
}


//------------------------------------------------------------------
/**
 * @brief   �^�b�`�p�l���f�[�^�̊i�[����
 *
 * @param   touchResult   
 * @param   tpData    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void SetTouchpanelData( TOUCH_INFO *touchResult, TP_ONE_DATA *tpData, int brush_color, int brush )
{
  int i,n;

  // �ŏ��ɃT���v�����O���ꂽ�_���i�[
  if(tpData->Size!=0){
    touchResult->x[0] = tpData->TPDataTbl[0].x;
    touchResult->y[0] = tpData->TPDataTbl[0].y;

    // �Ō�ɃT���v�����O���ꂽ�_���i�[
    n = tpData->Size-1;
    touchResult->x[1] = tpData->TPDataTbl[n].x;
    touchResult->y[1] = tpData->TPDataTbl[n].y;

#if PLFIX_T1624
    // �^�b�`���W��0,0�Ȃ�ݒ肵�Ȃ�
    for( i=0; i<2; i++ ){
      if( (touchResult->x[i] + touchResult->y[i]) == 0 ){
        tpData->Size = 0; // �Ȃ��������Ƃɂ���
      }
    }
#endif
  }

  // �ő�S��T���v�����O�����͂������A����ł����M�f�[�^�͂Q�Ɗi�[����
  if(tpData->Size>=2){
    touchResult->size  = 2;
  }else{
    touchResult->size  = tpData->Size;
  }
  
  // �F�E�傫���ݒ�
  touchResult->color = brush_color;
  touchResult->brush = brush;

}


//----------------------------------------------------------------------------------
/**
 * @brief �ڑ��l���擾
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int _get_connect_num( OEKAKI_WORK *wk )
{
  return Union_App_GetMemberNum( wk->param->uniapp );
}

//----------------------------------------------------------------------------------
/**
 * @brief �ڑ���Ԃ�BITMAP�ŕ\��
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int _get_connect_bit( OEKAKI_WORK *wk )
{
  return Union_App_GetMemberNetBit( wk->param->uniapp );
}


//----------------------------------------------------------------------------------
/**
 * @brief ��ʕ\��ON�A�㉺����ւ��ݒ�
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void _disp_on( void )
{
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );   // ���C�����OBJ�ʂn�m
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );   // �T�u���OBJ��OFF

  GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
  
  GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
}


static const int yn_item[][2]={
  { msg_oekaki_yes,  APP_TASKMENU_WIN_TYPE_NORMAL },
  { msg_oekaki_no,   APP_TASKMENU_WIN_TYPE_NORMAL },
};


//----------------------------------------------------------------------------------
/**
 * @brief �͂��E�������J�n
 *
 * @param   wk    
 *
 * @retval  APP_TASKMENU_WORK  *    
 */
//----------------------------------------------------------------------------------
static APP_TASKMENU_WORK  *YesNoMenuInit( OEKAKI_WORK *wk )
{
  APP_TASKMENU_INITWORK init;
  APP_TASKMENU_WORK     *menuwork;
  int i;

  // APPMENU���\�[�X�ǂݍ���
  wk->app_res = APP_TASKMENU_RES_Create( GFL_BG_FRAME0_M, 2, wk->font, wk->printQue, HEAPID_OEKAKI );

  for(i=0;i<2;i++){
    wk->yn_menuitem[i].str      = GFL_MSG_CreateString( wk->MsgManager, yn_item[i][0] ); //���j���[�ɕ\�����镶����
    wk->yn_menuitem[i].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;   //�����F�B�f�t�H���g�ł悢�Ȃ��APP_TASKMENU_ITEM_MSGCOLOR
    wk->yn_menuitem[i].type     = yn_item[i][1];
  }


  init.heapId   = HEAPID_OEKAKI;
  init.itemNum  = 2;
  init.itemWork = wk->yn_menuitem;
  init.posType  = ATPT_LEFT_UP;
  init.charPosX = 21; //�E�B���h�E�J�n�ʒu(�L�����P��
  init.charPosY =  7;
  init.w = 10;  //�L�����P��
  init.h =  3;  //�L�����P��

  // �͂��E���������j���[�J�n
  menuwork = APP_TASKMENU_OpenMenu( &init, wk->app_res );
  
  return menuwork;
}


//----------------------------------------------------------------------------------
/**
 * @brief �͂��E�������I��҂�
 *
 * @param   wk    
 *
 * @retval  u32   
 */
//----------------------------------------------------------------------------------
static u32 YesNoMenuMain( OEKAKI_WORK *wk )
{
  u32 ret=YESNO_RET_NONE;

  // ���j���[���C��
  APP_TASKMENU_UpdateMenu( wk->app_menuwork );

  if(APP_TASKMENU_IsFinish( wk->app_menuwork )){
    if(APP_TASKMENU_GetCursorPos(wk->app_menuwork)==0){
      ret = YESNO_RET_YES;
    }else{
      ret = YESNO_RET_NO;
    }
    // �I������
    APP_TASKMENU_CloseMenu( wk->app_menuwork );
    GFL_STR_DeleteBuffer( wk->yn_menuitem[1].str );
    GFL_STR_DeleteBuffer( wk->yn_menuitem[0].str );
    APP_TASKMENU_RES_Delete( wk->app_res );
    wk->app_menuwork = NULL;
  }

  return ret;
  
}


//----------------------------------------------------------------------------------
/**
 * @brief ���邮��������s���������o�[��ʐM�F�B�Ƃ��ēo�^
 *
 * @param   g2m   
 */
//----------------------------------------------------------------------------------
static void _comm_friend_add( OEKAKI_WORK *wk )
{
  ETC_SAVE_WORK  *etc_save  = SaveData_GetEtc( 
                                GAMEDATA_GetSaveControlWork( wk->param->gamedata));
  int i;
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    const MYSTATUS *mystatus = Union_App_GetMystatus(wk->param->uniapp,i);
    if(mystatus!=NULL){
      if(GFL_NET_SystemGetCurrentID()!=i){
        EtcSave_SetAcquaintance( etc_save, MyStatus_GetID(mystatus) );
        OS_Printf("id=%d��F�B�o�^\n", i);
      }
    }
  }

}

