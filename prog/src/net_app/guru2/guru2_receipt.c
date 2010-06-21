//******************************************************************************
/**
 * 
 * @file    guru2_receipt.c
 * @brief   ���邮������@��t
 * @author  Akito Mori(�ڐA�j / kagaya 
 * @data    2010.01.20
 *
 */
//******************************************************************************
//  ARCID_WIFILEADING     "wifileadingchar.narc"
//  ARCID_WIFIUNIONCHAR   "wifi_unionobj.narc"

// sharenum�������Ɋm�F����
#define CONFIRM_SHARENUM

#include <gflib.h>
#include "system/main.h"
#include "arc/arc_def.h"
#include "font/font.naix"
#include "gamesystem/msgspeed.h"
#include "arc/worldtrade.naix"
#include "print/wordset.h"
#include "arc/message.naix"
#include "msg/msg_guru2_receipt.h"
#include "system/wipe.h"
#include "system/bmp_winframe.h"
#include "system/net_err.h"
#include "print/printsys.h"
#include "poke_tool/monsno_def.h"
#include "system/bmp_menu.h"
#include "sound/pm_sndsys.h"

#include "net_app/union/union_beacon_tool.h"

#include "guru2_local.h"
#include "guru2_receipt.h"
#include "arc/wifileadingchar.naix"
#include "arc/wifi_unionobj.naix"
#include "arc/wifi_unionobj_plt.cdat"
#include "app/app_nogear_subscreen.h"
//#include "comm_command_record.h"


#define FIELDOBJ_PAL_START  ( 7 )

#define RECORD_1SEC_WAIT  ( 45 )

#define MESFRAME_PAL_INDEX  ( 10 )  // �E�C���h�E�t���[���̃p���b�g�̍s
#define MENUFRAME_PAL_INDEX ( 11 )

#include "arc/guru2_2d.naix" //��p2D���\�[�X�A�[�J�C�u�ǉ�
#include "c_gear.naix"

//============================================================================================
//  �萔��`
//============================================================================================
enum {
  SEQ_IN = 0,
  SEQ_MAIN,
  SEQ_LEAVE,
  SEQ_OUT,
  SEQ_DISCONNECT_WAIT,
};

enum{
  LIMIT_MODE_NONE,  ///<�����Ȃ�
  LIMIT_MODE_TRUE,  ///<����������
  LIMIT_MODE_ONLY,  ///<����������(��l�̂�)
};

//==============================================================================
//  �v���g�^�C�v
//==============================================================================
static void VBlankFunc( GFL_TCB *tcb, void * work );
static void VramBankSet(void);
static void BgInit( void );
static void InitWork( GURU2RC_WORK *wk, ARCHANDLE* p_handle );
static void FreeWork( GURU2RC_WORK *wk );
static void BgExit( void );
static void BgGraphicSet( GURU2RC_WORK * wk, ARCHANDLE* p_handle );
static void InitCellActor(GURU2RC_WORK *wk, ARCHANDLE* p_handle);
static void SetCellActor(GURU2RC_WORK *wk);
static void ReleaseCellActor( GURU2RC_WORK *wk );
static void BmpWinInit(GURU2RC_WORK *wk );
static void BmpWinDelete( GURU2RC_WORK *wk );
static void SetCursor_Pos( GFL_CLWK *act, int x, int y );
static void EndSequenceCommonFunc( GURU2RC_WORK *wk );
static void CursorColTrans(u16 *CursorCol);
static BOOL NameCheckPrint( GFL_BMPWIN *win, int frame, PRINTSYS_LSB color, GURU2RC_WORK *wk );
static int  ConnectCheck( GURU2RC_WORK *wk );
static BOOL  MyStatusCheck( GURU2RC_WORK *wk );
static void RecordMessagePrint( GURU2RC_WORK *wk, int msgno, int all_put );
static int  EndMessageWait( GURU2RC_WORK *wk );
static void EndMessageWindowOff( GURU2RC_WORK *wk );
static int  OnlyParentCheck( GURU2RC_WORK *wk );
static int  MyStatusGetNum( GURU2RC_WORK *wk );
static u32  MyStatusGetNumBit( GURU2RC_WORK *wk );
static void RecordDataSendRecv( GURU2RC_WORK *wk );
static void CenteringPrint(GFL_BMPWIN *win, STRBUF *strbuf, GFL_FONT *font);
static void SequenceChange_MesWait( GURU2RC_WORK *wk, int next );
static void TrainerObjFunc( GURU2RC_WORK *wk );
static void LoadFieldObjData( GURU2RC_WORK *wk, ARCHANDLE* p_handle );
static void FreeFieldObjData( GURU2RC_WORK *wk );
static void TransFieldObjData( GURU2RC_WORK *wk, NNSG2dPaletteData *PalData, int id, int view, int sex );
static void TransPal( GFL_TCB* tcb, void *work );
static int  GetTalkSpeed( GURU2RC_WORK *wk );
static int  RecordCorner_BeaconControl( GURU2RC_WORK *wk, int plus );
static void PadControl( GURU2RC_WORK *wk );
static int  _get_key_trg( void );
static void _print_func( GURU2RC_WORK *wk );

// FuncTable����V�[�P���X�J�ڂŌĂ΂��֐�
static int Record_MainInit( GURU2RC_WORK *wk, int seq );
static int Record_MainNormal( GURU2RC_WORK *wk, int seq );
static int Record_StartRecordCommand( GURU2RC_WORK *wk, int seq );
static int Record_RecordSendData( GURU2RC_WORK *wk, int seq );
static int Record_StartSelectWait( GURU2RC_WORK *wk, int seq );
static int Record_StartSelect( GURU2RC_WORK *wk, int seq );
static int Record_EndSelectPutString( GURU2RC_WORK *wk, int seq );
static int Record_EndSelectWait( GURU2RC_WORK *wk, int seq );
static int Record_EndSelectAnswerWait( GURU2RC_WORK *wk, int seq );
static int Record_EndSelectAnswerOK( GURU2RC_WORK *wk, int seq );
static int Record_EndSelectAnswerNG( GURU2RC_WORK *wk, int seq );
static int Record_EndChild( GURU2RC_WORK *wk, int seq );
static int Record_EndChildWait( GURU2RC_WORK *wk, int seq );
static int Record_EndSelectParent( GURU2RC_WORK *wk, int seq );
static int Record_EndSelectParentWait( GURU2RC_WORK *wk, int seq );
static int Record_ForceEnd( GURU2RC_WORK *wk, int seq );
static int Record_ForceEndWait( GURU2RC_WORK *wk, int seq );
static int Record_ForceEndSynchronize( GURU2RC_WORK *wk, int seq );
static int Record_EndParentOnly( GURU2RC_WORK *wk, int seq );
static int Record_EndParentOnlyWait( GURU2RC_WORK *wk, int seq );
static int Record_MessageWaitSeq( GURU2RC_WORK *wk, int seq );
static int Record_LogoutChildMes( GURU2RC_WORK *wk, int seq );
static int Record_LogoutChildClose( GURU2RC_WORK *wk, int seq );
static int Record_LogoutChildMesWait( GURU2RC_WORK *wk, int seq );
static int Record_NewMember( GURU2RC_WORK *wk, int seq );
static int Record_NewMemberEnd( GURU2RC_WORK *wk, int seq );
static int Record_FroceEndMesWait( GURU2RC_WORK *wk, int seq );

static int  Record_Guru2PokeSelStart( GURU2RC_WORK *wk, int seq );



// ���邮�������t���C���V�[�P���X�p�֐��z���`
static int (* const FuncTable[])(GURU2RC_WORK *wk, int seq)={
  Record_MainInit,            // RECORD_MODE_INIT  = 0, 
  Record_NewMember,           // RECORD_MODE_NEWMEMBER,
  Record_NewMemberEnd,        // RECORD_MODE_NEWMEMBER_END,
  Record_MainNormal,          // RECORD_MODE,
  Record_EndSelectPutString,  // RECORD_MODE_END_SELECT,
  Record_EndSelectWait,       // RECORD_MODE_END_SELECT_WAIT,
  Record_EndSelectAnswerWait, // RECORD_MODE_END_SELECT_ANSWER_WAIT
  Record_EndSelectAnswerOK,   // RECORD_MODE_END_SELECT_ANSWER_OK
  Record_EndSelectAnswerNG,   // RECORD_MODE_END_SELECT_ANSWER_NG
  Record_EndChild,            // RECORD_MODE_END_CHILD,
  Record_EndChildWait,        // RECORD_MODE_END_CHILD_WAIT,
  Record_EndSelectParent,     // RECORD_MODE_END_SELECT_PARENT,
  Record_EndSelectParentWait, // RECORD_MODE_END_SELECT_PARENT_WAIT,
  Record_ForceEnd,            // RECORD_MODE_FORCE_END,
  Record_FroceEndMesWait,     // RECORD_MODE_FORCE_END_MES_WAIT,
  Record_ForceEndWait,        // RECORD_MODE_FORCE_END_WAIT,
  Record_ForceEndSynchronize, // RECORD_MODE_FORCE_END_SYNCHRONIZE,
  Record_EndParentOnly,       // RECORD_MODE_END_PARENT_ONLY,
  Record_EndParentOnlyWait,   // RECORD_MODE_END_PARENT_ONLY_WAIT,
  Record_LogoutChildMes,      // RECORD_MODE_LOGOUT_CHILD,
  Record_LogoutChildMesWait,  // RECORD_MODE_LOGOUT_CHILD_WAIT,
  Record_LogoutChildClose,    // RECORD_MODE_LOGOUT_CHILD_CLOSE,
  Record_StartSelect,         // RECORD_MODE_START_SELECT,
  Record_StartSelectWait,     // RECORD_MODE_START_SELECT_WAIT
  Record_StartRecordCommand,  // RECORD_MODE_START_RECORD_COMMAND
  Record_RecordSendData,      // RECORD_MODE_RECORD_SEND_DATA ���������R�[�h�f�[�^��M�`�F�b�N

  Record_MessageWaitSeq,      // RECORD_MODE_MESSAGE_WAIT,
  
  //���邮��
  Record_Guru2PokeSelStart,   // RECORD_MODE_GURU2_POKESEL_START,
};

//==============================================================================
//  ���邮���t
//==============================================================================
//--------------------------------------------------------------
/**
 * ���邮���t�@������
 * @param proc  GFL_PROC *
 * @param seq   seq
 * @retval  GFL_PROC_RESULT GFL_PROC_RES_CONTINUE,GFL_PROC_RES_FINISH
 */
//--------------------------------------------------------------
GFL_PROC_RESULT Guru2ReceiptProc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  GURU2RC_WORK *wk;
  ARCHANDLE *p_handle;
  GURU2PROC_WORK *g2p = (GURU2PROC_WORK *)pwk;
  
  switch( *seq ){
  case 0:
    GFL_DISP_GX_InitVisibleControl();
    GFL_DISP_GXS_InitVisibleControl();
    GX_SetVisiblePlane( 0 );
    GXS_SetVisiblePlane( 0 );
  
    //�q�[�v�쐬
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_GURU2, GURU2_HEAPSIZE );
  
    p_handle = GFL_ARC_OpenDataHandle( ARCID_GURU2_2D, HEAPID_GURU2 );
    GF_ASSERT( p_handle );
    
    //��t�p���[�N�m��
    wk = GFL_PROC_AllocWork( proc, sizeof(GURU2RC_WORK), HEAPID_GURU2 );
    GFL_STD_MemFill( wk, 0, sizeof(GURU2RC_WORK) );
    
    g2p->g2r = wk;
    wk->g2p = g2p;
    wk->g2c = g2p->g2c;
    
    GFL_BG_Init( HEAPID_GURU2 );
  
    // ������}�l�[�W���[����
    wk->WordSet    = WORDSET_Create( HEAPID_GURU2 );
    wk->MsgManager = GFL_MSG_Create(  GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                                     NARC_message_guru2_receipt_dat, HEAPID_GURU2 );
    wk->font       = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr ,
                              GFL_FONT_LOADTYPE_FILE , FALSE , HEAPID_GURU2 );
   
    // VRAM �o���N�ݒ�
    VramBankSet();
      
    // BGL���W�X�^�ݒ�
    BgInit();          
    
  /* WIPE���Z�b�g�ɂ���ʂ������Ă��܂��o�O*/
  //  WIPE_ResetBrightness( WIPE_DISP_MAIN );
  //  WIPE_ResetBrightness( WIPE_DISP_SUB );
  
    // ���C�v�t�F�[�h�J�n
    WIPE_SYS_Start(
      WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN,
      WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, 16, 1, HEAPID_GURU2 );
  
    //BG�O���t�B�b�N�Z�b�g
    BgGraphicSet( wk, p_handle );
  
    // VBlank�֐��Z�b�g
    wk->vintr_tcb = GFUser_VIntr_CreateTCB( VBlankFunc, wk, 1 );
    // ���b�Z�[�W�X�g���[�������p�^�X�N�V�X�e������
    wk->pMsgTcblSys = GFL_TCBL_Init( HEAPID_GURU2, HEAPID_GURU2, 32 , 32 );
    wk->printQue    = PRINTSYS_QUE_Create( HEAPID_GURU2 );
  
    // ���[�N������
    InitWork( wk, p_handle );
  
  
    // CellActor�V�X�e��������
    InitCellActor(wk, p_handle);
      
    // CellActro�\���o�^
    SetCellActor(wk);
  
    // BMPWIN�o�^�E�`��
    BmpWinInit(wk);
    
    // �ʐM�R�}���h���������X�g�p�ɕύX
    Guru2Comm_CommandInit( g2p->g2c );
    
    
    // �ʐM�A�C�R���\��
    GFL_NET_ReloadIconTopOrBottom( TRUE , HEAPID_GURU2 );
  
    GFL_ARC_CloseDataHandle( p_handle );

    (*seq)++;
    break;
  case 1:
    (*seq) = 0;
    return( GFL_PROC_RES_FINISH );
  }
  
  return( GFL_PROC_RES_CONTINUE );
}

//----------------------------------------------------------------------------------
/**
 * @brief ���j�I�����[���ʐM���[�N�|�C���^�擾
 *
 * @param   wk    
 *
 * @retval  UNION_APP_PTR   
 */
//----------------------------------------------------------------------------------
static UNION_APP_PTR _get_unionwork(GURU2RC_WORK *wk)
{
//  OS_Printf("union app adr=%08x\n",(u32)wk->g2p->param.uniapp);
  return wk->g2p->param->uniapp;
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
static void GURU2RC_entry_callback(NetID net_id, const MYSTATUS *mystatus, void *userwork)
{
  GURU2RC_WORK *wk = (GURU2RC_WORK *)userwork;
  if(net_id==0){
    OS_Printf("�e�͗��������ɂ��Ȃ� net_id=%d\n", net_id);
  }
  
  OS_Printf("�����R�[���o�b�N�ʒB id=%d\n", net_id);
  // �ʂȎq�@�̗����ɑΏ�
  if(GFL_NET_SystemGetCurrentID()==0){
      int ret;
      u8 id  = net_id;
      // 2��ڈȍ~�̎q�@�̗���
      // �S��Ɂu���ꂩ��G�𑗂�̂Ŏ~�܂��Ă��������v�Ƒ��M����
      ret=Guru2Comm_SendData( wk->g2c, G2COMM_RC_STOP, &id, 1);
      
      if(ret==FALSE){
        GF_ASSERT("�����R�[���o�b�N���M���s\n");
      }
//    }
  }
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
static void GURU2RC_leave_callback(NetID net_id, const MYSTATUS *mystatus, void *userwork)
{
  OS_Printf("���E�R�[���o�b�N net_id=%d\n", net_id);
}


//--------------------------------------------------------------
/**
 * ���邮�������t�@���C��
 * @param proc  GFL_PROC *
 * @param seq   seq
 * @retval  GFL_PROC_RESULT GFL_PROC_RES_CONTINUE,GFL_PROC_RES_FINISH
 */
//--------------------------------------------------------------
GFL_PROC_RESULT Guru2ReceiptProc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  GURU2RC_WORK *wk = (GURU2RC_WORK *)mywk;

  if(GFL_NET_SystemGetCurrentID() == 0 && wk->g2c->ridatu_bit != 0){
    wk->g2c->ridatu_bit &= Union_App_GetMemberNetBit(_get_unionwork(wk));
  }
  
  switch( wk->proc_seq ){
  case SEQ_IN:  //�@���C�v�����҂�
    if( WIPE_SYS_EndCheck() ){
      wk->proc_seq = SEQ_MAIN;
      // ����OK��Ԃɂ���
      Union_App_Parent_ResetEntryBlock( _get_unionwork(wk) );
      // �����E�ޏo�R�[���o�b�N�o�^
      Union_App_SetCallback( _get_unionwork(wk), GURU2RC_entry_callback, GURU2RC_leave_callback, wk);
      
#if 0
      // �������q�@�Őڑ��䐔���Q��ȏゾ�����ꍇ�͂��邮������ɗ����������
      if(GFL_NET_SystemGetCurrentID()!=0){
        if(MyStatusGetNum(wk)>2){
          //Guru2Comm_SendData(wk->g2c,G2COMM_RC_CHILD_JOIN, NULL, 0);
        }
      }
#endif
    }

    break;
  case SEQ_MAIN:    // �J�[�\���ړ�
    // �V�[�P���X���̓���
    if(FuncTable[wk->seq]!=NULL){
      wk->proc_seq = (*FuncTable[wk->seq])( wk, wk->proc_seq );
    }
    
    if(wk->g2c->record_execute == FALSE){ //���R�[�h�������̎��͍X�V���Ȃ�
      NameCheckPrint(
        wk->TrainerNameWin, 0, PRINTSYS_LSB_Make(1, 4, 0), wk );
    }
    
    TrainerObjFunc(wk);

    // �ω������l���ɉ����ăr�[�R����ς���
    if(GFL_NET_SystemGetCurrentID()==0){
      int temp = RecordCorner_BeaconControl( wk, 1 );
      if(wk->proc_seq==SEQ_MAIN){
        // �I���V�[�P���X�Ȃǂɗ���Ă��Ȃ���ΕύX����
        wk->proc_seq = temp;
      }
    }

    break;
  case SEQ_LEAVE:
    if(FuncTable[wk->seq]!=NULL){
      wk->proc_seq = (*FuncTable[wk->seq])( wk, wk->proc_seq );
    }
    break;
  case SEQ_OUT:
    if( WIPE_SYS_EndCheck() ){
      // ���邮��J�n�ł���ΏI��������
      if(wk->end_next_flag ){
        return GFL_PROC_RES_FINISH;

      // �����łȂ���ΒʐM�ؒf��
      }else{
        GFL_NET_DelCommandTable( GFL_NET_CMD_GURUGURU );
        Union_App_Shutdown( _get_unionwork(wk) );  // �ʐM�ؒf�J�n
        wk->proc_seq = SEQ_DISCONNECT_WAIT;
      }
    }
    break;
  case SEQ_DISCONNECT_WAIT:
    if(Union_App_WaitShutdown(_get_unionwork(wk))){
      return GFL_PROC_RES_FINISH;
    }
    break;
  }
  
  // �t�F�[�h������Ȃ����ɒʐM�G���[�`�F�b�N
  if( WIPE_SYS_EndCheck() ){
    if(NetErr_App_CheckError()!=NET_ERR_STATUS_NULL){
      OS_Printf("------------------------�ʐM�G���[--------------------\n");
      return ( GFL_PROC_RES_FINISH );
    }
  }

//  OS_Printf("*seq=%d, subseq=%d\n", wk->proc_seq, wk->seq);

  GFL_CLACT_SYS_Main();               // �Z���A�N�^�[�풓�֐�
  _print_func(wk);
  

  return GFL_PROC_RES_CONTINUE;
}



//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F�I��
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT Guru2ReceiptProc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  int i;
  GURU2RC_WORK *wk = (GURU2RC_WORK *)mywk;
  
  // ���b�Z�[�W�\���p�V�X�e�����
  GFL_TCBL_DeleteAll( wk->pMsgTcblSys );
  GFL_TCBL_Exit( wk->pMsgTcblSys );
  PRINTSYS_QUE_Clear( wk->printQue );
  PRINTSYS_QUE_Delete( wk->printQue );

  // �Z���A�N�^�[�I��
  ReleaseCellActor( wk );

  // BMP�E�B���h�E�J��
  BmpWinDelete( wk );

  // BGL�폜
  BgExit( );

  // ���b�Z�[�W�}�l�[�W���[�E���[�h�Z�b�g�}�l�[�W���[���
  GFL_MSG_Delete( wk->MsgManager );
  WORDSET_Delete( wk->WordSet );
  GFL_FONT_Delete( wk->font );


  // ����ւ���Ă����㉺��ʏo�͂����ɖ߂�
  GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);

  GFL_TCB_DeleteTask( wk->vintr_tcb );  // VBlank�֐�����
  
  //���[�N���f
  wk->g2p->receipt_ret = wk->end_next_flag;
  wk->g2p->receipt_num = MyStatusGetNum(wk);
  wk->g2p->receipt_bit = MyStatusGetNumBit(wk);
  
  //���[�N���
  FreeWork( wk );
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_GURU2 );
  return GFL_PROC_RES_FINISH;
}

static const u8 palanimetable[][2]={
  { 8,2,},
  { 8,3,},
  { 8,4,},
  {24,0,},
  { 0,0xff,},
};

//------------------------------------------------------------------
/**
 * @brief   �p���b�g�]���^�X�N
 *
 * @param   tcb   
 * @param   work    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void TransPal( GFL_TCB* tcb, void *work )
{
  GURU2RC_WORK     *wk  = (GURU2RC_WORK *)work;
  VTRANS_PAL_WORK *pal = &wk->palwork;
  
  // �p���b�g�A�j��
  if(pal->sw){
    if(pal->wait>palanimetable[pal->seq][0]){
      u16 *src;
      pal->wait=0;
      pal->seq++;
      if(palanimetable[pal->seq][1]==0xff){
        pal->seq = 0;
      }
      src = (u16*)pal->palbuf->pRawData;
      GX_LoadOBJPltt(&src[16*palanimetable[pal->seq][1]], 0, 32);
//      OS_Printf("�p���b�g�]�� seq = %d \n",pal->seq);
    }else{
      wk->palwork.wait++;
    }
    
  }


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
  
  // �p���b�g�]��
  TransPal( tcb, work );
  
  GFL_CLACT_SYS_VBlankFunc();
  
  GFL_BG_VBlankFunc();
}


//----------------------------------------------------------------------------------
/**
 * @brief �����`��֘A�풓�֐�
 *
 * @param wk    
 */
//----------------------------------------------------------------------------------
static void _print_func( GURU2RC_WORK *wk )
{
  if(wk->printQue!=NULL){
    PRINTSYS_QUE_Main( wk->printQue );
  }
  GFL_TCBL_Main( wk->pMsgTcblSys );

}


static const GFL_DISP_VRAM Guru2DispVramDat = {

    GX_VRAM_BG_128_A,           // ���C��2D�G���W����BG
    GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g

    GX_VRAM_SUB_BG_128_C,       // �T�u2D�G���W����BG
    GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g

    GX_VRAM_OBJ_128_B,          // ���C��2D�G���W����OBJ
    GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g

    GX_VRAM_SUB_OBJ_16_I,       // �T�u2D�G���W����OBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g

    GX_VRAM_TEX_NONE,           // �e�N�X�`���C���[�W�X���b�g
    GX_VRAM_TEXPLTT_NONE,        // �e�N�X�`���p���b�g�X���b�g
  
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

  GFL_DISP_SetBank( &Guru2DispVramDat );
}

//--------------------------------------------------------------------------------------------
/**
 * BG�ݒ�
 *
 * @param ini   BGL�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BgInit()
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
    GFL_BG_SetBGControl( GFL_BG_FRAME0_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen( GFL_BG_FRAME0_S );


  }

  // �T�u��ʃe�L�X�g��
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen( GFL_BG_FRAME1_S );
  }

  // ���C����ʔw�i
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
    };
    GFL_BG_SetBGControl(  GFL_BG_FRAME2_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
  }
  GFL_BG_SetClearCharacter( GFL_BG_FRAME0_S, 32, 0, HEAPID_GURU2 );

  // ���C�����1
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  GFL_BG_FRAME0_M );
  }

  // �T�u��ʔw�i��
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
  }
  GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 32, 0, HEAPID_GURU2 );

  // �T�uBG�ʂɂ�NO-CGEAR�X�N���[����\������
//  APP_NOGEAR_SUBSCREEN_Init();

}


#define TALK_MESSAGE_BUF_NUM  ( 90*2 )
#define TITLE_MESSAGE_BUF_NUM ( 20*2 )

//------------------------------------------------------------------
/**
 * ���G�������[�N������
 *
 * @param   wk    GURU2RC_WORK*
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void InitWork( GURU2RC_WORK *wk, ARCHANDLE* p_handle )
{
  int i;

  for(i=0;i<RECORD_CORNER_MEMBER_MAX;i++){
    wk->TrainerName[i] = GFL_STR_CreateBuffer( PERSON_NAME_SIZE+EOM_SIZE, HEAPID_GURU2 );
    wk->TrainerStatus[i][0] = NULL;
    wk->TrainerStatus[i][1] = NULL;
    wk->TrainerReq[i]       = RECORD_EXIST_NO;

  }
  // ������o�b�t�@�쐬
  wk->TalkString  = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_GURU2 );
  wk->TitleString = GFL_STR_CreateBuffer( TITLE_MESSAGE_BUF_NUM, HEAPID_GURU2 );

  wk->seq = RECORD_MODE_INIT;
  
  // ���R�[�h��������ڂ��イ���I������擾
  GFL_MSG_GetString(  wk->MsgManager, msg_guru2_receipt_title_01, wk->TitleString );

  // �t�B�[���hOBJ�摜�ǂݍ���
  LoadFieldObjData( wk, p_handle );

//  wk->ObjPaletteTable = UnionView_PalleteTableAlloc( HEAPID_GURU2 );


  // �p���b�g�A�j���p���[�N������
  wk->palwork.sw       = 0;
  wk->palwork.wait     = 0;
  wk->palwork.seq      = 0;
  wk->palwork.paldata  = GFL_ARCHDL_UTIL_LoadPalette( p_handle, NARC_guru2_2d_record_s_obj_NCLR, &wk->palwork.palbuf, HEAPID_GURU2 );
  wk->connectBackup    = 0;
  wk->YesNoMenuWork    = NULL;
  wk->g2c->shareNum    = 2;
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
static void FreeWork( GURU2RC_WORK *wk )
{
  int i;

  // �t�B�[���hOBJ�摜���
  FreeFieldObjData( wk );

  // �@�B�̃p���b�g�A�j���p�f�[�^���
  GFL_HEAP_FreeMemory( wk->palwork.paldata );

  // ���j�I��OBJ�̃p���b�g�f�[�^���
//  GFL_HEAP_FreeMemory( wk->ObjPaletteTable );

  for(i=0;i<RECORD_CORNER_MEMBER_MAX;i++){
    GFL_STR_DeleteBuffer( wk->TrainerName[i] );
  }
  GFL_STR_DeleteBuffer( wk->TitleString ); 
  GFL_STR_DeleteBuffer( wk->TalkString ); 

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
//  APP_NOGEAR_SUBSCREEN_Exit();

  GFL_BG_FreeBGControl( GFL_BG_FRAME2_S );
  GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
  GFL_BG_FreeBGControl( GFL_BG_FRAME0_S );
  GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
  GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );

  GFL_BG_Exit();
}


//--------------------------------------------------------------------------------------------
/**
 * �O���t�B�b�N�f�[�^�Z�b�g
 *
 * @param wk    �|�P�������X�g��ʂ̃��[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BgGraphicSet( GURU2RC_WORK * wk, ARCHANDLE* p_handle )
{
  // ------���ʗpBG���\�[�X�]��---------
  // �㉺��ʂa�f�p���b�g�]��
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle,  NARC_guru2_2d_record_s_NCLR, PALTYPE_MAIN_BG, 0, 16*16*2, HEAPID_GURU2 );
  // ���C�����BG1�L�����]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_guru2_2d_record_s_NCGR,  GFL_BG_FRAME1_M, 0, 32*8*0x20, 0, HEAPID_GURU2);
  // ���C�����BG1�X�N���[���]��
  GFL_ARCHDL_UTIL_TransVramScreen(   p_handle, NARC_guru2_2d_record_s_NSCR,  GFL_BG_FRAME1_M, 0, 32*24*2, 0, HEAPID_GURU2);

  // ��b�t�H���g�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
                                 13*0x20, 32, HEAPID_GURU2 );

  // ------����ʗpBG���\�[�X�]��---------
  // �T�u��ʃp���b�g�]��

  GFL_ARC_UTIL_TransVramPalette( ARCID_C_GEAR, NARC_c_gear_c_gear_m_NCLR,  PALTYPE_SUB_BG,  0, 32*11,   HEAPID_GURU2 );
  // �T�u��ʃL�����]��
  GFL_ARC_UTIL_TransVramBgCharacter( ARCID_C_GEAR, NARC_c_gear_c_gear_m1_NCGR,   GFL_BG_FRAME2_S, 0, 0, 0, HEAPID_GURU2);
  // �T�u��ʃX�N���[���L�����]��
  GFL_ARC_UTIL_TransVramScreen(      ARCID_C_GEAR, NARC_c_gear_c_gear01_n_NSCR, GFL_BG_FRAME2_S, 0, 0, 0, HEAPID_GURU2);

#if 0
  {
    MYSTATUS *my = GAMEDATA_GetMyStatus( wk->g2p->param->gamedata);
    int gender   = MyStatus_GetMySex( my );
    APP_NOGEAR_SUBSCREEN_Trans( HEAPID_GURU2, gender );
  }
#endif
  
  // ���C����ʉ�b�E�C���h�E�O���t�B�b�N�]��
  BmpWinFrame_GraphicSet(
     GFL_BG_FRAME0_M, 1, MESFRAME_PAL_INDEX,
     0, HEAPID_GURU2 );

  BmpWinFrame_GraphicSet(
         GFL_BG_FRAME0_M, 1+TALK_WIN_CGX_SIZ, MENUFRAME_PAL_INDEX, 0, HEAPID_GURU2 );

  GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_ON );

}


// ��l��OAM���\�[�X�̒j���ʒ�`
static const int res_tbl[][2]={
  { NARC_wifileadingchar_hero_simple_NCGR, NARC_wifileadingchar_heroine_simple_NCGR  },
  { NARC_wifileadingchar_hero_NCLR,        NARC_wifileadingchar_heroine_NCLR         },
  { NARC_wifileadingchar_hero_simple_NCER, NARC_wifileadingchar_heroine_simple_NCER, },
  { NARC_wifileadingchar_hero_simple_NANR, NARC_wifileadingchar_heroine_simple_NANR, },

};
//------------------------------------------------------------------
/**
 * ���[�_�[��ʗp�Z���A�N�^�[������
 *
 * @param   wk    ���[�_�[�\���̂̃|�C���^
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void InitCellActor(GURU2RC_WORK *wk, ARCHANDLE* p_handle)
{
  int i;
  MYSTATUS *my = GAMEDATA_GetMyStatus( wk->g2p->param->gamedata);
  int gender   = MyStatus_GetMySex( my );
  // ��l���Z���f�[�^�I�[�v��
  ARCHANDLE *handle = GFL_ARC_OpenDataHandle( ARCID_WIFILEADING, HEAPID_GURU2 );
  
  // �Z���A�N�^�[������
  GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN, &Guru2DispVramDat, HEAPID_GURU2 );
  
  wk->clUnit = GFL_CLACT_UNIT_Create( 30, 1, HEAPID_GURU2 );
  GF_ASSERT( wk->clUnit );
  
  //---------���ʐl��OBJ�ǂݍ���-------------------
  for(i=0;i<PLAYER_OAM_NUM;i++){
    //chara�ǂݍ���
    wk->resObjTbl[GURU2_CLACT_OBJ0_RES_CHR+i] = GFL_CLGRP_CGR_Register( handle,  res_tbl[0][gender], 
                                                                 0, CLSYS_DRAW_MAIN, HEAPID_GURU2 );
    //pal�ǂݍ���
    wk->resObjTbl[GURU2_CLACT_OBJ0_RES_PLTT+i] = GFL_CLGRP_PLTT_RegisterEx( handle, res_tbl[1][gender],
                                                                 CLSYS_DRAW_MAIN, i*32, 0, 1, 
                                                                 HEAPID_GURU2 );
  }
  //cell�ǂݍ���
  wk->resObjTbl[GURU2_CLACT_RES_CELL] = GFL_CLGRP_CELLANIM_Register( handle, 
                                            res_tbl[2][gender], res_tbl[3][gender],
                                            HEAPID_GURU2 );
  GFL_ARC_CloseDataHandle( handle );

  // ���j�I��OBJ���\�[�X�̃n���h���I�[�v��
  wk->union_handle = GFL_ARC_OpenDataHandle( ARCID_WIFIUNIONCHAR, HEAPID_GURU2 );
}


//----------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[����������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void ReleaseCellActor( GURU2RC_WORK *wk )
{
  int i;
  
  // ���j�I��OBJ���\�[�X�n���h���N���[�Y
  GFL_ARC_CloseDataHandle( wk->union_handle );

  // �Z���A�N�^�[���\�[�X���
  for(i=0;i<PLAYER_OAM_NUM;i++){
    // �L�����j��
    GFL_CLGRP_CGR_Release( wk->resObjTbl[GURU2_CLACT_OBJ0_RES_PLTT+i] );
    // �p���b�g�j��
    GFL_CLGRP_PLTT_Release( wk->resObjTbl[GURU2_CLACT_OBJ0_RES_CHR+i] );
  }
  // �Z���E�A�j���j��
  GFL_CLGRP_CELLANIM_Release( wk->resObjTbl[GURU2_CLACT_RES_CELL] );
  
  // �Z���A�N�^�[�Z�b�g�j��
  GFL_CLACT_UNIT_Delete( wk->clUnit );

  //OAM�����_���[�j��
  GFL_CLACT_SYS_Delete();

}

#define TRAINER_NAME_POS_X    ( 24 )
#define TRAINER_NAME_POS_Y    ( 32 )
#define TRAINER_NAME_POS_SPAN ( 32 )

#define TRAINER_NAME_WIN_X    (  2 )
#define TRAINER1_NAME_WIN_Y   (  6 )
#define TRAINER2_NAME_WIN_Y   (  7 )

static const u16 obj_pos_tbl[][2]={
  { 204,  84   },
  { 204,  61-8 },
  { 233,  80-8 },
  { 223, 111-8 },
  { 184, 111-8 },
  { 174,  80-8 },
};

//------------------------------------------------------------------
/**
 * �Z���A�N�^�[�o�^
 *
 * @param   wk      GURU2RC_WORK*
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void SetCellActor(GURU2RC_WORK *wk)
{
  int i;

  {
    //�o�^���i�[
    GFL_CLWK_DATA add;
  
    add.bgpri   = 1;
    add.softpri = 0;
    add.anmseq  = 0;
    //�Z���A�N�^�[�\���J�n

    // ���C����ʗp(�l���̓o�^�j
    for(i=0;i<PLAYER_OAM_NUM;i++){
      add.pos_x = obj_pos_tbl[i+1][0];
      add.pos_y = obj_pos_tbl[i+1][1];
      wk->MainActWork[i+1] = GFL_CLACT_WK_Create( wk->clUnit, 
                                               wk->resObjTbl[GURU2_CLACT_OBJ0_RES_CHR+i],
                                               wk->resObjTbl[GURU2_CLACT_OBJ0_RES_PLTT+i],
                                               wk->resObjTbl[GURU2_CLACT_RES_CELL],
                                               &add, CLSYS_DEFREND_MAIN, HEAPID_GURU2);
      GFL_CLACT_WK_SetAutoAnmFlag( wk->MainActWork[i+1],1 );
      GFL_CLACT_WK_SetAnmSeq( wk->MainActWork[i+1], 0 );
      GFL_CLACT_WK_SetDrawEnable( wk->MainActWork[i+1], 0 );
    }

  } 
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );   // ���C�����OBJ�ʂn�m
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );  // �T�u���OBJ��OFF
  
}

// ���������{�[�hBMP�i����ʁj
#define OEKAKI_BOARD_POSX  ( 1 )
#define OEKAKI_BOARD_POSY  ( 2 )
#define OEKAKI_BOARD_W   ( 30 )
#define OEKAKI_BOARD_H   ( 15 )


// ���O�\��BMP�i���ʁj
#define RECORD_NAME_BMP_W  ( 16 )
#define RECORD_NAME_BMP_H  ( 11 )
#define RECORD_NAME_BMP_SIZE (RECORD_NAME_BMP_W * RECORD_NAME_BMP_H)


// �u��߂�v������BMP�i����ʁj
#define OEKAKI_END_BMP_X  ( 26 )
#define OEKAKI_END_BMP_Y  ( 20 )
#define OEKAKI_END_BMP_W  ( 6  )
#define OEKAKI_END_BMP_H  ( 2  )


// ��b�E�C���h�E�\���ʒu��`
#define RECORD_TALK_X   (  2 )
#define RECORD_TALK_Y   (  19 )

#define RECORD_TITLE_X    (   3  )
#define RECORD_TITLE_Y    (   1  )
#define RECORD_TITLE_W    (  26  )
#define RECORD_TITLE_H    (   2  )

#define MESSAGE_WINDOW_W  ( 27 )
#define MESSAGE_WINDOW_H  (  4 )

#define RECORD_MSG_WIN_OFFSET     ( 1 + TALK_WIN_CGX_SIZ   + MENU_WIN_CGX_SIZ)
#define RECORD_YESNO_WIN_OFFSET   ( RECORD_MSG_WIN_OFFSET  + RECORD_NAME_BMP_SIZE )

//------------------------------------------------------------------
/**
 * BMPWIN�����i�����p�l���Ƀt�H���g�`��j
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void BmpWinInit(GURU2RC_WORK *wk )
{
  // ---------- ���C����� ------------------
  GFL_BMPWIN_Init( HEAPID_GURU2 );

  // BG1��BMP�i��߂�j�E�C���h�E�m�ہE�`��
  wk->EndWin = GFL_BMPWIN_Create( GFL_BG_FRAME1_S,
                                  OEKAKI_END_BMP_X, OEKAKI_END_BMP_Y, 
                                  OEKAKI_END_BMP_W, OEKAKI_END_BMP_H, 13,  
                                  GFL_BMP_CHRAREA_GET_B );

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->EndWin), 0 );

  // ----------- �T�u��ʖ��O�\��BMP�m�� ------------------
  // BG0��BMP�i��b�E�C���h�E�j�m��
  wk->MsgWin = GFL_BMPWIN_Create( GFL_BG_FRAME0_M,
    RECORD_TALK_X, RECORD_TALK_Y, MESSAGE_WINDOW_W, MESSAGE_WINDOW_H, 13,  GFL_BMP_CHRAREA_GET_B );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin), 0x0f0f );

  // BG0��BMP�i�^�C�g���E�C���h�E�j�m��
  wk->TitleWin = GFL_BMPWIN_Create( GFL_BG_FRAME0_M,
                                    RECORD_TITLE_X, RECORD_TITLE_Y, 
                                    RECORD_TITLE_W, RECORD_TITLE_H, 15,  
                                    GFL_BMP_CHRAREA_GET_B );
  CenteringPrint(wk->TitleWin, wk->TitleString, wk->font );

  // ���O�\���E�C���h�E
  {
    int i;
    wk->TrainerNameWin = GFL_BMPWIN_Create( GFL_BG_FRAME0_M,  
                                               TRAINER_NAME_WIN_X, TRAINER1_NAME_WIN_Y, 
                                               RECORD_NAME_BMP_W, RECORD_NAME_BMP_H, 15,  
                                               GFL_BMP_CHRAREA_GET_B );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->TrainerNameWin), 0 );

    //�ŏ��Ɍ����Ă���ʂȂ̂ŕ����p�l���`��Ɠ]�����s��
    NameCheckPrint( wk->TrainerNameWin, 0, PRINTSYS_LSB_Make(1, 4, 0), wk );


  }

  // BMP��PRINT_UTIL�̊֘A�t��
  PRINT_UTIL_Setup( &wk->printUtil, wk->TitleWin );

  OS_Printf("YESNO win offset = %d\n",RECORD_YESNO_WIN_OFFSET);

/*
3713 2D4A 5694 089D 5EBF 7DC0 76EF 0000
0000 0000 0000 7FFF 62FF 6F7B 7F52 7FFF

*/



} 

//------------------------------------------------------------------
/**
 * @brief   �Z���^�����O���ăv�����g
 *
 * @param   win   GFL_BMPWIN
 * @param   strbuf  
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void CenteringPrint(GFL_BMPWIN *win, STRBUF *strbuf, GFL_FONT *font)
{
  int length = PRINTSYS_GetStrWidth( strbuf, font, 0 );
  int x      = (RECORD_TITLE_W*8-length)/2;
  
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win), 0x0000 );
  PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(win), x, 1, strbuf, font, PRINTSYS_LSB_Make(1, 4, 0) );
  GFL_BMPWIN_MakeTransWindow( win );
}


// �͂��E������BMP�i����ʁj
#define YESNO_WIN_FRAME_CHAR  ( 1 + TALK_WIN_CGX_SIZ )
#define YESNO_CHARA_OFFSET    ( 1 + TALK_WIN_CGX_SIZ + MENU_WIN_CGX_SIZ + MESSAGE_WINDOW_W*MESSAGE_WINDOW_H )
#define YESNO_WINDOW_X      ( 22 )
#define YESNO_WINDOW_Y1     (  7 )
#define YESNO_WINDOW_Y2     ( 13 )
#define YESNO_CHARA_W     (  8 )
#define YESNO_CHARA_H     (  4 )


//------------------------------------------------------------------
/**
 * @brief   �m�ۂ���BMPWIN�����
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void BmpWinDelete( GURU2RC_WORK *wk )
{
  // �͂��E���������j���[���J�����܂܂���������
  if(wk->YesNoMenuWork!=NULL){
    BmpMenu_YesNoMenuExit( wk->YesNoMenuWork );
  }

  GFL_BMPWIN_Delete( wk->TrainerNameWin );
  GFL_BMPWIN_Delete( wk->TitleWin );
  GFL_BMPWIN_Delete( wk->EndWin );
  GFL_BMPWIN_Delete( wk->MsgWin );

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
  pos.y = y;
  
  GFL_CLACT_WK_SetPos( act, &pos, CLSYS_DEFREND_MAIN );

}

//==============================================================================
//  ��t�@���C���V�[�P���X
//==============================================================================
//------------------------------------------------------------------
/**
 * @brief   �J�n���̃��b�Z�[�W
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static int Record_MainInit( GURU2RC_WORK *wk, int seq )
{
  // �e�̎���A�{�^���ŊJ�n���b�Z�[�W�B�q�@�͊J�n�҂����b�Z�[�W
  if(GFL_NET_SystemGetCurrentID()==0){
      GFL_NET_SetAutoErrorCheck( FALSE ); 
    // �ڑ��l����2�l��葽���ꍇ�́uA�{�^���ł������v��\��
    if(Union_App_GetMemberNum(_get_unionwork(wk))>=2){
      RecordMessagePrint(wk, msg_guru2_receipt_01_01, 0 );
    }
  }else{
    // �q�@�ɂ͒ʐM�G���[����������
    GFL_NET_SetAutoErrorCheck( TRUE ); 
    RecordMessagePrint(wk, msg_guru2_receipt_01_07, 0 );
  }
  
  SequenceChange_MesWait(wk, RECORD_MODE);
  
//  wk->seq = RECORD_MODE;

  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   ���邮�������t�ʏ폈��
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Record_MainNormal( GURU2RC_WORK *wk, int seq )
{
  // �S�̑���֎~�łȂ���΃L�[����
  PadControl( wk );

  //TrainerObjFunc(wk);
  
  if(GFL_NET_SystemGetCurrentID()==0){        // �e�@�̎�
    if(OnlyParentCheck(wk)!=1){     // ��l����Ȃ����H
      RecordDataSendRecv( wk );
    }
  }else{
      RecordDataSendRecv( wk );
  }

  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   �L�[���͊Ǘ�
 *
 * @param   wk    
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static void PadControl( GURU2RC_WORK *wk )
{
  // ���R�[�h�������J�n���܂����H�i�e�̂݁j
  if(_get_key_trg() & PAD_BUTTON_DECIDE){
    if(GFL_NET_SystemGetCurrentID()==0){
      OS_Printf("currentID =%d\n", GFL_NET_SystemGetCurrentID());
#ifdef CONFIRM_SHARENUM
      if(MyStatusGetNum(wk)==wk->g2c->shareNum && wk->g2c->ridatu_bit == 0){
#else
      if(1){
#endif
        PMSND_PlaySE(SEQ_SE_DECIDE1);

        // ���E�֎~�ʒB(FALSE�̏ꍇ�͐i�s���Ȃ��j
        Union_App_Parent_EntryBlock( _get_unionwork(wk) );
//        if(Union_App_Parent_EntryBlock( _get_unionwork(wk) )){

//          u8 flag = GURU2COMM_BAN_ON;
          RecordMessagePrint( wk, msg_guru2_receipt_01_02, 0 );
          SequenceChange_MesWait(wk,RECORD_MODE_START_SELECT);
//        }
        
        // �ڑ��l������ON
        //ChangeConnectMax( wk, 0 );
      }
      else{
        PMSND_PlaySE(SEQ_SE_SELECT1);
      }
    }

  // �L�����Z��
  }else if(_get_key_trg()&PAD_BUTTON_CANCEL){
    // ��߂܂����H
    if(GFL_NET_SystemGetCurrentID()){
      // �q�@�͐e�@����֎~�����Ă���ꍇ�͏I���ł��Ȃ�
      if(wk->g2c->banFlag==0){
        RecordMessagePrint( wk, msg_guru2_receipt_01_03, 0 );
        SequenceChange_MesWait(wk,RECORD_MODE_END_SELECT);
      }
      else{
        PMSND_PlaySE(SEQ_SE_SELECT1);
      }
    }else{
//      if( MyStatusGetNum(wk)==wk->g2c->shareNum ){
      if( Union_App_GetMemberNum(_get_unionwork(wk))==wk->g2c->shareNum && wk->g2c->ridatu_bit == 0){
        // �e�@�͏I�����j���[��
        if(Union_App_Parent_EntryBlock( _get_unionwork(wk) )){
          u8 flag = GURU2COMM_BAN_ON;
          RecordMessagePrint( wk, msg_guru2_receipt_01_03, 0 );
          SequenceChange_MesWait(wk,RECORD_MODE_END_SELECT);
          PMSND_PlaySE(SEQ_SE_CANCEL1);
          // ���E�֎~�ʒB
        //  Guru2Comm_SendData(wk->g2c, G2COMM_RC_BAN, &flag, 1 );

        // �ڑ��l������ON
        //ChangeConnectMax( wk, 0 );
        }
      }
      else{
        PMSND_PlaySE(SEQ_SE_SELECT1);
      }
    }
  }
  // �ʏ펞
  else{
    if(wk->beacon_flag == GURU2COMM_BAN_NONE){
      if(GFL_NET_SystemGetCurrentID() == 0 && Union_App_GetMemberNum(_get_unionwork(wk))==wk->g2c->shareNum){
        // ���E�֎~�����ʒB
        Union_App_Parent_ResetEntryBlock(_get_unionwork(wk));
      }
    }
  }

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
static void EndSequenceCommonFunc( GURU2RC_WORK *wk )
{
  
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
static int Record_NewMember( GURU2RC_WORK *wk, int seq )
{
  // ���������񂪂͂����Ă��܂���
//  RecordMessagePrint(wk, msg_oekaki_01);
//  wk->seq = RECORD_MODE_NEWMEMBER_WAIT;
//  SequenceChange_MesWait(wk, RECORD_MODE_NEWMEMBER_END );
  wk->seq = RECORD_MODE_NEWMEMBER_END;

  // �摜�]����ԂɂȂ�����P�x�_�E��
//  G2_SetBlendBrightness(  GX_BLEND_PLANEMASK_BG1|
//              GX_BLEND_PLANEMASK_BG2|
//              GX_BLEND_PLANEMASK_BG3|
//              GX_BLEND_PLANEMASK_OBJ
//                ,  -6);

  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;

}

//------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�I����҂��Ď��̃V�[�P���X�ɍs���֐�
 *
 * @param   wk    
 * @param   next    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void SequenceChange_MesWait( GURU2RC_WORK *wk, int next )
{
  wk->seq     = RECORD_MODE_MESSAGE_WAIT;
  wk->nextseq = next;
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
static int Record_MessageWaitSeq( GURU2RC_WORK *wk, int seq )
{
  if( EndMessageWait( wk ) ){
    wk->seq = wk->nextseq;
  }
  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  
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
static int Record_NewMemberEnd( GURU2RC_WORK *wk, int seq )
{
  // �P�x�_�E������
//  G2_BlendNone();

  if(GFL_NET_SystemGetCurrentID()==0){
    int flag = GURU2COMM_BAN_NONE;
    // ���E�֎~�����ʒB
    Union_App_Parent_ResetEntryBlock(_get_unionwork(wk));
//    Guru2Comm_SendData(wk->g2c, G2COMM_RC_BAN, &flag, 1 );
  }

  wk->seq = RECORD_MODE;
  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;
  
}


// �͂��E������
#define BMP_YESNO_PX  ( 23 )
#define BMP_YESNO_PY  ( 13 )
#define BMP_YESNO_SX  ( 7 )
#define BMP_YESNO_SY  ( 4 )
#define BMP_YESNO_PAL ( 13 )
#define BMP_YESNO_CGX ( RECORD_YESNO_WIN_OFFSET )


// �͂��E������
static const BMPWIN_YESNO_DAT YesNoBmpWin = {
  GFL_BG_FRAME0_M, 
  BMP_YESNO_PX, BMP_YESNO_PY,
  BMP_YESNO_PAL, BMP_YESNO_CGX
};

//------------------------------------------------------------------
/**
 * @brief   ���邮�������t�u��߂�v��I��������
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Record_EndSelectPutString( GURU2RC_WORK *wk, int seq )
{
  // �͂��E�������\��
  wk->YesNoMenuWork = BmpMenu_YesNoSelectInit( &YesNoBmpWin, YESNO_WIN_FRAME_CHAR,
                                               MENUFRAME_PAL_INDEX, 0, HEAPID_GURU2 );


  wk->seq = RECORD_MODE_END_SELECT_WAIT;
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
static int Record_EndSelectWait( GURU2RC_WORK *wk, int seq )
{
  int result;
  u32 ret;
  
  // �q�@�͐e�@���瑀��֎~�����Ă���ꍇ�͑���ł��Ȃ�
  if(GFL_NET_SystemGetCurrentID()){
    if(wk->g2c->banFlag){
      if(_get_key_trg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL | PAD_KEY_UP | PAD_KEY_DOWN)){
        PMSND_PlaySE(SEQ_SE_SELECT1);
      }
      EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
      return seq;
    }
  }
  else{ //�e�̏ꍇ
    if(wk->g2c->ridatu_bit != 0){     //���E���悤�Ƃ��Ă���q������Ȃ狖���Ȃ�
      if(_get_key_trg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL | PAD_KEY_UP | PAD_KEY_DOWN)){
        PMSND_PlaySE(SEQ_SE_SELECT1);
      }
      EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
      return seq;
    }
  }

//  if(wk->g2c->shareNum != MyStatusGetNum(wk)){  //��v���Ă��Ȃ��Ȃ�u��߂�v�����Ȃ�
  if(MyStatusGetNum(wk) != Union_App_GetMemberNum(_get_unionwork(wk))){
    //��v���Ă��Ȃ��Ȃ�u��߂�v�����Ȃ�(�q���ʂ邱���͐e�����X�V����Ȃ�shareNum�͌��Ȃ�)
    EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
    return seq;
  }

  // ���j���[����
  ret = BmpMenu_YesNoSelectMain( wk->YesNoMenuWork );

  if(ret!=BMPMENU_NULL){
    if(ret==BMPMENU_CANCEL){
      if(GFL_NET_SystemGetCurrentID()==0){
        int flag = GURU2COMM_BAN_NONE;
        // ���E�֎~�����ʒB
        Union_App_Parent_ResetEntryBlock(_get_unionwork(wk));
//        Guru2Comm_SendData(wk->g2c, G2COMM_RC_BAN, &flag, 1 );

        // �ڑ��l������OFF
        //ChangeConnectMax( wk, 1 );
      }
      wk->seq = RECORD_MODE_INIT;
//      SequenceChange_MesWait( wk, RECORD_MODE_INIT );
    }else{
      if(GFL_NET_SystemGetCurrentID()==0){    
        SequenceChange_MesWait( wk, RECORD_MODE_END_SELECT_PARENT );
        RecordMessagePrint( wk, msg_guru2_receipt_01_13, 0 );   // ���[�_�[����߂�Ɓc
      }else{
        GURU2COMM_END_CHILD_WORK crec;
        
        MI_CpuClear8(&crec, sizeof(GURU2COMM_END_CHILD_WORK));
        crec.request   = CREC_REQ_RIDATU_CHECK;
        crec.ridatu_id = GFL_NET_SystemGetCurrentID();
        
        if(Guru2Comm_SendData(wk->g2c, G2COMM_RC_END_CHILD, &crec, sizeof(GURU2COMM_END_CHILD_WORK) )){
          wk->status_end = TRUE;
          wk->ridatu_wait = 0;
          wk->seq = RECORD_MODE_END_SELECT_ANSWER_WAIT;
        }else{
          // ���M���s�����ꍇ�͗��R������͂��Ȃ̂ŁA�ŏ��ɖ߂�
          wk->seq = RECORD_MODE_INIT;
        }
      }
    }
    wk->YesNoMenuWork = NULL;
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
static int Record_EndSelectAnswerWait( GURU2RC_WORK *wk, int seq )
{
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
static int Record_EndSelectAnswerOK( GURU2RC_WORK *wk, int seq )
{
  if((wk->g2c->oya_share_num != Union_App_GetMemberNum(_get_unionwork(wk)))
      || (wk->g2c->oya_share_num != MyStatusGetNum(wk))){
    OS_TPrintf("share_nuM = %d, Comm = %d, My = %d, Bit = %d\n", wk->g2c->oya_share_num, Union_App_GetMemberNum(_get_unionwork(wk)), MyStatusGetNum(wk), Union_App_GetMemberNetBit(_get_unionwork(wk)));
    wk->ridatu_wait = 0;
    wk->seq = RECORD_MODE_END_SELECT_ANSWER_NG;
    EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
    return seq;
  }
  
  wk->ridatu_wait++;
  OS_TPrintf("ridatu_wait = %d\n", wk->ridatu_wait);
  if(wk->ridatu_wait > 30){
    GURU2COMM_END_CHILD_WORK crec;
    
    MI_CpuClear8(&crec, sizeof(GURU2COMM_END_CHILD_WORK));
    crec.request = CREC_REQ_RIDATU_EXE;
    crec.ridatu_id = GFL_NET_SystemGetCurrentID();

    Guru2Comm_SendData(wk->g2c, G2COMM_RC_END_CHILD, &crec, sizeof(GURU2COMM_END_CHILD_WORK) );

    wk->ridatu_wait = 0;
    wk->seq = RECORD_MODE_END_CHILD;
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
static int Record_EndSelectAnswerNG( GURU2RC_WORK *wk, int seq )
{
  wk->status_end = FALSE;
  SequenceChange_MesWait( wk, RECORD_MODE_INIT );
  
  OS_TPrintf("==========���E�����L�����Z���I===========\n");
  
  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   ���̃����o�[�Ń��R�[�h���J�n���܂����H
 *
 * @param   wk    
 * @param   seq   
 * @retval  int   
 */
//------------------------------------------------------------------
static int Record_StartSelect( GURU2RC_WORK *wk, int seq )
{
  // �͂��E�������\��
//  wk->YesNoMenuWork = BmpYesNoSelectInit( &YesNoBmpWin, YESNO_WIN_FRAME_CHAR, MENUFRAME_PAL_INDEX, HEAPID_GURU2 );
  wk->YesNoMenuWork = BmpMenu_YesNoSelectInit( &YesNoBmpWin, YESNO_WIN_FRAME_CHAR,
                                               MENUFRAME_PAL_INDEX, 0, HEAPID_GURU2 );


  wk->seq = RECORD_MODE_START_SELECT_WAIT;
  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �u�J�n���܂����H�v�͂��E�������I��҂�
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Record_StartSelectWait( GURU2RC_WORK *wk, int seq )
{
  int result;
  u32 ret;

#ifdef CONFIRM_SHARENUM
  if(MyStatusGetNum(wk) != wk->g2c->shareNum    //��v���Ă��Ȃ��Ȃ狖���Ȃ�
      || wk->g2c->ridatu_bit != 0){     //���E���悤�Ƃ��Ă���q������Ȃ狖���Ȃ�
    if(_get_key_trg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL | PAD_KEY_UP | PAD_KEY_DOWN)){
      PMSND_PlaySE(SEQ_SE_SELECT1);
    }
    EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
    return seq;
  }
#endif
    
  ret = BmpMenu_YesNoSelectMain( wk->YesNoMenuWork );

  if(ret!=BMPMENU_NULL){
    if(ret==BMPMENU_CANCEL){
      int flag = GURU2COMM_BAN_NONE;
      // ���E�֎~�����ʒB
      Union_App_Parent_ResetEntryBlock(_get_unionwork(wk));

      // �r�[�R����ԕύX
      wk->seq = RECORD_MODE_INIT;
    }else{
      // ��΂����ɂ���̂͐e������
      if(GFL_NET_SystemGetCurrentID()==0){
                // �ڑ��֎~�ɏ�������
        wk->seq = RECORD_MODE_START_RECORD_COMMAND;
        wk->start_num = MyStatusGetNum(wk);
      }else{
        GF_ASSERT(0);
      }
    }
    wk->YesNoMenuWork = NULL;
  }


  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���

  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   �e�@��
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Record_StartRecordCommand( GURU2RC_WORK *wk, int seq )
{
  if(wk->record_send == 0){
    if(Guru2Comm_SendData(wk->g2c, G2COMM_RC_START, NULL, 0 ) == TRUE){
      OS_Printf("���邮��J�n�𑗐M\n");
      wk->seq = RECORD_MODE_GURU2_POKESEL_START;
      // �q�@�����Ȃ��ꍇ�̓G���[�ɂȂ�B(�����Ń����o�[�m��j
      GFL_NET_SetNoChildErrorCheck(TRUE);
    }
  }
  
//  SequenceChange_MesWait( wk, RECORD_MODE_RECORD_SEND_DATA );
//  RecordMessagePrint( wk, msg_guru2_receipt_01_11, 1 );   // �܂��Ă��܂�

  return seq;

}


//------------------------------------------------------------------
/**
 * @brief   �f�[�^���M�E��M�҂�
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Record_RecordSendData( GURU2RC_WORK *wk, int seq )
{
  int i,result;

  result = 0;
//  for(i=0;i<RECORD_CORNER_MEMBER_MAX;i++){
//    result += CommIsSpritDataRecv(i);
//  }

  if( GFL_NET_SystemGetCurrentID() == 0 && Union_App_GetMemberNum(_get_unionwork(wk)) != wk->start_num){
    //�J�n�������̐l���ƌ��݂̐l�����ς���Ă���Ȃ�ʐM�G���[�ɂ���
//    OS_TPrintf("�l�����ς���Ă���̂ŋ����I�ɒʐM�G���[�ɂ��܂��@�J�n����%d, ���݁�%d\n", wk->start_num, Union_App_GetMemberNum(_get_unionwork(wk)));
//    CommStateSetError();
  }
  
  // ��M���I�������l���Ɛڑ��l���������ɂȂ�����I��
  if( Union_App_GetMemberNum(_get_unionwork(wk))==wk->g2c->recv_count ){

    // �f�[�^�����鏈����
    wk->seq = RECORD_MODE_GURU2_POKESEL_START;
    
#if 0
    for(i=0;i<RECORD_CORNER_MEMBER_MAX;i++){
      OS_TPrintf("��M�f�[�^ id=%d, seed=%d, xor=%08x\n",i,wk->recv_data[i].seed, wk->recv_data[i]._xor);
    }
#endif
  }

  return seq;
}




// RECORD_MODE_END_CHILD
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
static int  Record_EndChild( GURU2RC_WORK *wk, int seq )
{
  u8 temp;
  
  // ���邮�������t���ʂ��܂���
  RecordMessagePrint( wk, msg_guru2_receipt_01_04, 0 ); 
  OS_Printf("------------------�u����ł́`�v�\��\n");

  // �I���ʒB
  temp = 0;
//  Guru2Comm_SendData(wk->g2c, G2COMM_RC_END_CHILD, &temp, 1 );

  SequenceChange_MesWait( wk, RECORD_MODE_END_CHILD_WAIT );

  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;
}
        

// RECORD_MODE_END_CHILD_WAIT
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
static int  Record_EndChildWait( GURU2RC_WORK *wk, int seq )
{     
  if( ++wk->wait > RECORD_CORNER_MESSAGE_END_WAIT ){
    // ���C�v�t�F�[�h�J�n
    WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
                    WIPE_FADE_BLACK, 16, 1, HEAPID_GURU2 );
    seq = SEQ_OUT;            //�I���V�[�P���X��
  }

  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
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
static int Record_EndSelectParent( GURU2RC_WORK *wk, int seq )
{

  // �͂��E�������\��
//  wk->YesNoMenuWork = BmpYesNoSelectInit( &YesNoBmpWin, YESNO_WIN_FRAME_CHAR, MENUFRAME_PAL_INDEX, HEAPID_GURU2 );
  wk->YesNoMenuWork = BmpMenu_YesNoSelectInit( &YesNoBmpWin, YESNO_WIN_FRAME_CHAR,
                                               MENUFRAME_PAL_INDEX, 0, HEAPID_GURU2 );

  wk->seq = RECORD_MODE_END_SELECT_PARENT_WAIT;
  

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
static int Record_EndSelectParentWait( GURU2RC_WORK *wk, int seq )
{
  int ret;

  if(wk->g2c->shareNum != MyStatusGetNum(wk)      //��v���Ă��Ȃ��Ȃ�u��߂�v�����Ȃ�
      || wk->g2c->ridatu_bit != 0){     //���E���悤�Ƃ��Ă���q������Ȃ狖���Ȃ�
    if(_get_key_trg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL | PAD_KEY_UP | PAD_KEY_DOWN)){
      PMSND_PlaySE( SEQ_SE_DECIDE1 );
    }
    EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
    return seq;
  }

  ret = BmpMenu_YesNoSelectMain( wk->YesNoMenuWork );

  if(ret!=BMPMENU_NULL){
    if(ret==BMPMENU_CANCEL){
      int flag = GURU2COMM_BAN_NONE;
      
      wk->seq = RECORD_MODE_INIT;

      // ���E�֎~�����ʒB
      Union_App_Parent_ResetEntryBlock(_get_unionwork(wk));
//      Guru2Comm_SendData(wk->g2c, G2COMM_RC_BAN, &flag, 1 );

      // �ڑ��l������OFF
      //ChangeConnectMax( wk, 1 );

    }else{
      wk->seq = RECORD_MODE_FORCE_END;
      Guru2Comm_SendData(wk->g2c, G2COMM_RC_END, NULL, 0 ); //�I���ʒm
      WORDSET_RegisterPlayerName(   // �e�@�i�����j�̖��O��WORDSET
        wk->WordSet, 0, Union_App_GetMystatus(_get_unionwork(wk),0) );
    }
    wk->YesNoMenuWork = NULL;
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
static int Record_ForceEnd( GURU2RC_WORK *wk, int seq )
{
  if(GFL_NET_SystemGetCurrentID()==0){
    // ����ł́@���邮������𒆎~�@���܂�
    RecordMessagePrint( wk, msg_guru2_receipt_01_04, 0 );
    OS_Printf("------------------�u����ł́`�v�\��\n");
  }else{
    // ��������邭�Ȃ��������o�[������̂ł������񂵂܂�
    WORDSET_RegisterPlayerName( wk->WordSet, 0, Union_App_GetMystatus(_get_unionwork(wk),0) ); // �e�@�i�����j�̖��O��WORDSET
    RecordMessagePrint( wk, msg_guru2_receipt_01_15, 0 );
  }
  SequenceChange_MesWait(wk,RECORD_MODE_FORCE_END_MES_WAIT);
  wk->wait = 0;

  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;

}


//------------------------------------------------------------------
/**
 * @brief   �����I�����b�Z�[�W�P�b�҂�
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Record_FroceEndMesWait( GURU2RC_WORK *wk, int seq )
{
  wk->wait++;
  if(wk->wait > RECORD_1SEC_WAIT){
    wk->seq = RECORD_MODE_FORCE_END_WAIT;
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
static int Record_ForceEndWait( GURU2RC_WORK *wk, int seq )
{
  // �ʐM�����҂�
//  CommTimingSyncStart(COMM_GURU2_TIMINGSYNC_NO);
  GFL_NETHANDLE *pNet = GFL_NET_HANDLE_GetCurrentHandle();
  GFL_NET_HANDLE_TimeSyncStart(pNet, COMM_GURU2_TIMINGSYNC_NO,WB_NET_GURUGURU);

  wk->seq = RECORD_MODE_FORCE_END_SYNCHRONIZE;

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
static int Record_ForceEndSynchronize( GURU2RC_WORK *wk, int seq )
{
  GFL_NETHANDLE *pNet = GFL_NET_HANDLE_GetCurrentHandle();

  if(GFL_NET_HANDLE_IsTimeSync(pNet, COMM_GURU2_TIMINGSYNC_NO,WB_NET_GURUGURU)){
    GFL_NET_SetAutoErrorCheck(FALSE);
    OS_Printf("�I������������\n");
    // ���C�v�t�F�[�h�J�n
    WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
                    WIPE_FADE_BLACK, 16, 1, HEAPID_GURU2);

    seq = SEQ_OUT;            //�I���V�[�P���X��
  }
  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
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
static int Record_EndParentOnly( GURU2RC_WORK *wk, int seq )
{
/* ���邮�������t��3�l�ȏ�̎��A�O�D�T�b���炢�̃^�C�~���O�Ŏq�@�������Đe����l�ɂ����
 �u���������Ȃ������o�[���c�v�Ƃ������b�Z�[�W���Q�d�ɕ\������ăE�C���h�E����  
  �\�������Ă��܂��o�O��Ώ� */
  // ���b�Z�[�W�\�����͌Ăяo���Ȃ��悤�ɂ���
  if( EndMessageWait( wk ) ){
    RecordMessagePrint( wk, msg_guru2_receipt_01_08, 0 ); // ���[�_�[���������̂ŉ��U���܂��B
    wk->seq = RECORD_MODE_END_PARENT_ONLY_WAIT;
  }

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
static int Record_EndParentOnlyWait( GURU2RC_WORK *wk, int seq )
{
  if( EndMessageWait( wk ) ){
    wk->seq = RECORD_MODE_END_CHILD_WAIT;
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
static int Record_LogoutChildMes( GURU2RC_WORK *wk, int seq )
{
  // ���������񂪂�����܂���
  RecordMessagePrint(wk, msg_guru2_receipt_01_14, 1 );
  wk->seq = RECORD_MODE_LOGOUT_CHILD_WAIT;

  // �ڑ��\�l������U���݂̐ڑ��l���ɗ��Ƃ�
  if(GFL_NET_SystemGetCurrentID()==0){
    //ChangeConnectMax( wk, 0 );
  }


  OS_TPrintf("�l���`�F�b�N��logout child %d\n", Union_App_GetMemberNum(_get_unionwork(wk)));
  wk->err_num = Union_App_GetMemberNum(_get_unionwork(wk));
    wk->err_num_timeout = ERRNUM_TIMEOUT;
  
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
static int Record_LogoutChildMesWait( GURU2RC_WORK *wk, int seq )
{
  // �ڑ��l�����P���邩�`�F�b�N
  if(wk->err_num != 0 && Union_App_GetMemberNum(_get_unionwork(wk)) != wk->err_num){
    wk->err_num = 0;
  }

  wk->seq  = RECORD_MODE_LOGOUT_CHILD_CLOSE;
  wk->wait = 0;

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
static int  Record_LogoutChildClose( GURU2RC_WORK *wk, int seq )
{     

  // �ڑ��l�����P����܂ł͑҂�
  if(wk->err_num != 0 && Union_App_GetMemberNum(_get_unionwork(wk)) != wk->err_num){
    wk->err_num = 0;
  }
    wk->err_num_timeout--;
    if(wk->err_num_timeout<0){
        wk->err_num = 0;
    }
  
  if( ++wk->wait > RECORD_CORNER_MESSAGE_END_WAIT && wk->err_num == 0){
    wk->seq = RECORD_MODE_INIT;//RECORD_MODE;       //�I���V�[�P���X��

    // ���E�҂����Ȃ��Ȃ����̂ŁA�{�P��Ԃň�l�͓����悤�ɂ���
    if(GFL_NET_SystemGetCurrentID()==0){
      //ChangeConnectMax( wk, 1 );
    }
  }

  EndSequenceCommonFunc( wk );      //�I���I�����̋��ʏ���
  return seq;
  
}

#if 0
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
static int Record_LogoutChildMes( GURU2RC_WORK *wk, int seq )
{
  // ���������񂪂�����܂���
  wk->seq = RECORD_MODE_LOGOUT_CHILD_WAIT;

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
static int Record_LogoutChildMesWait( GURU2RC_WORK *wk, int seq )
{
    wk->seq  = RECORD_MODE_LOGOUT_CHILD_CLOSE;
    wk->wait = 0;

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
static int  Record_LogoutChildClose( GURU2RC_WORK *wk, int seq )
{     

  if( ++wk->wait > RECORD_CORNER_MESSAGE_END_WAIT ){
    wk->seq = RECORD_MODE;        //�I���V�[�P���X��
  }

  EndSequenceCommonFunc( wk );      //�I���I�����̋��ʏ���
  return seq;
  
}
#endif

//==============================================================================
//  ���邮������p
//==============================================================================
//------------------------------------------------------------------
/**
 * @brief   �|�P�����Z���N�g��
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int       
 */
//------------------------------------------------------------------
static int  Record_Guru2PokeSelStart( GURU2RC_WORK *wk, int seq )
{
  OS_Printf( "���邮��@�|�P�����Z���N�g��\n" );
  
//  Union_BeaconChange( UNION_PARENT_MODE_GURU2 );
  
  // ���C�v�t�F�[�h�J�n
  WIPE_SYS_Start(
    WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
    WIPE_FADE_BLACK, 16, 1, HEAPID_GURU2);
  
  wk->end_next_flag = TRUE;
  seq = SEQ_OUT;
  EndSequenceCommonFunc( wk );    //�I���I�����̋��ʏ���
  return seq;
}

//==============================================================================
/**
 * @brief   �ǂ�ȏ�Ԃł����Ă������I�Ƀ��C���V�[�P���X�`�F���W
 *
 * @param   wk    
 * @param   seq   
 * @param   id    
 *
 * @retval  none    
 */
//==============================================================================
void Guru2Rc_MainSeqForceChange( GURU2RC_WORK *wk, int seq, u8 id  )
{
  switch(seq){
  case RECORD_MODE_NEWMEMBER_END:
    EndMessageWindowOff( wk );
    break;
  case RECORD_MODE_FORCE_END:
    if(wk->YesNoMenuWork!=NULL){
      BmpMenu_YesNoMenuExit( wk->YesNoMenuWork );
      wk->YesNoMenuWork = NULL;
    }
    break;
  case RECORD_MODE_RECORD_SEND_DATA:
    RecordMessagePrint( wk, msg_guru2_receipt_01_11, 0 );   // �u�܂��Ă��܂��v�\��
    if(wk->YesNoMenuWork!=NULL){
      BmpMenu_YesNoMenuExit( wk->YesNoMenuWork );
      wk->YesNoMenuWork = NULL;
    }
    break;
  case RECORD_MODE_LOGOUT_CHILD:
    if(wk->status_end == TRUE){
      return; //�������g�����E������
    }
    WORDSET_RegisterPlayerName( wk->WordSet, 0, Union_App_GetMystatus(_get_unionwork(wk),id) );  
    if(id==GFL_NET_SystemGetCurrentID()){
      // ���������E����q�@�������ꍇ�́u�q�@�����Ȃ��Ȃ���v�Ƃ͌���Ȃ�
      return;
    }
    if(wk->YesNoMenuWork!=NULL){
      BmpMenu_YesNoMenuExit( wk->YesNoMenuWork );
      wk->YesNoMenuWork = NULL;
    }
    if(GFL_NET_SystemGetCurrentID() == 0){
      wk->g2c->ridatu_bit &= 0xffff ^ id;
    }
    break;
  case RECORD_MODE_END_SELECT_ANSWER_OK:
    break;
  case RECORD_MODE_END_SELECT_ANSWER_NG:
    break;
  case RECORD_MODE_GURU2_POKESEL_START:
    if(EndMessageWait(wk) == 0){
      //�\�����̃��b�Z�[�W������ꍇ�͋�����~
      PRINTSYS_PrintStreamStop( wk->printStream );
  }
    
    if(wk->YesNoMenuWork!=NULL){
      BmpMenu_YesNoMenuExit( wk->YesNoMenuWork );
      wk->YesNoMenuWork = NULL;
    }
    break;
  default:
    GF_ASSERT( "�w��ȊO�̃V�[�P���X�`�F���W������" );
    return;
  }
  wk->seq = seq;

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
void Guru2Rc_MainSeqCheckChange( GURU2RC_WORK *wk, int seq, u8 id  )
{
  const MYSTATUS *mystatus=NULL;
  // �ʏ��ԂȂ�
  if(wk->seq == RECORD_MODE){
    OS_Printf("�q�@%d�̖��O��o�^\n",id);
    switch(seq){
    case RECORD_MODE_NEWMEMBER: 
      // �w��̎q�@�̖��O��WORDSET�ɓo�^�i���E�E������)
      mystatus = Union_App_GetMystatus(_get_unionwork(wk),id);
      if(mystatus!=NULL){
        WORDSET_RegisterPlayerName( wk->WordSet, 0, mystatus );  
      }
      //ChangeConnectMax( wk, 1 );
      wk->seq      = seq;
      wk->g2c->shareNum = Union_App_GetMemberNum(_get_unionwork(wk));
      wk->g2c->ridatu_bit = 0;
//      if(GFL_NET_SystemGetCurrentID()==0){
//        int flag = GURU2COMM_BAN_ON;//NONE;
//        // ���E�֎~�����ʒB
//        Guru2Comm_SendData(wk->g2c, G2COMM_RC_BAN, &flag, 1 );
//        
//      }
      break;
      // ������ 
    case RECORD_MODE_LOGOUT_CHILD:
      WORDSET_RegisterPlayerName( wk->WordSet, 0, Union_App_GetMystatus(_get_unionwork(wk),id) );  
      if(id==GFL_NET_SystemGetCurrentID()){
        // ���������E����q�@�������ꍇ�́u�q�@�����Ȃ��Ȃ���v�Ƃ͌���Ȃ�
        return;
      }
      if(GFL_NET_SystemGetCurrentID() == 0){
        wk->g2c->ridatu_bit &= 0xffff ^ id;
      }
      wk->seq = seq;
      break;
    default:
      GF_ASSERT( 0&&"�w��ȊO�̃V�[�P���X�`�F���W������" );
      return;
    }


  }
  else if(seq == RECORD_MODE_NEWMEMBER){
    wk->g2c->ridatu_bit = 0;
    wk->g2c->shareNum = Union_App_GetMemberNum(_get_unionwork(wk));
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



//------------------------------------------------------------------
/**
 * @brief   ���݉��l�ڑ����Ă��邩�H
 *
 * @param   none    
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int OnlyParentCheck( GURU2RC_WORK *wk )
{
  int i,result;
  const MYSTATUS *status;

  result = 0;
  for(i=0;i<RECORD_CORNER_MEMBER_MAX;i++){
    status = Union_App_GetMystatus(_get_unionwork(wk),i);
    if(status!=NULL){
      result++;
    }
  }

  return result;
}

#define NAME_PRINT_HABA ( 18 )
#define ID_STRING_NUM (2+1+5+1)
//------------------------------------------------------------------
/**
 * @brief   �I�����C���󋵂��m�F���ď��ʂɖ��O��ID��\������
 *
 * @param   win   
 * @param   frame   
 * @param   color   
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static BOOL NameCheckPrint( GFL_BMPWIN *win, int frame, PRINTSYS_LSB color, GURU2RC_WORK *wk )
{
  int i,id = GFL_NET_SystemGetCurrentID();
  STRBUF *id_str  = NULL;
  STRBUF *tmp_str = NULL;
  PRINTSYS_LSB tmpcol = color;

  // ���O�擾�̏󋵂ɕω��������ꍇ�͏��������Ȃ�
  if(!MyStatusCheck(wk)){
    return FALSE;
  }


  // ���ꂼ��̕����p�l���̔w�i�F�ŃN���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win),0x0000);
  OS_Printf("NamePrint-------------------------------------------------\n");

  // �`��
  for(i=0;i<RECORD_CORNER_MEMBER_MAX;i++){
    if(wk->TrainerStatus[i][0]!=NULL){
      // �g���[�i�[ID�̎擾
      u16 tid = MyStatus_GetID_Low( wk->TrainerStatus[i][0] );
      id_str  = GFL_STR_CreateBuffer( BUFLEN_PERSON_NAME, HEAPID_GURU2 );

      // �����̖��O�擾
      MyStatus_CopyNameString( wk->TrainerStatus[i][0], wk->TrainerName[i] );

      // �g���[�i�[ID�𖄂ߍ��񂾕�����uID 12345�v���擾
      WORDSET_RegisterNumber( wk->WordSet, 0, tid, 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_ZENKAKU );
      tmp_str = GFL_MSG_CreateString( wk->MsgManager, msg_guru2_receipt_name_02 );
      WORDSET_ExpandStr( wk->WordSet, id_str, tmp_str );
      
      if(id==i){  // �����̖��O�̎��͕`��F��ԐF��
        color = PRINTSYS_LSB_Make(2,3,0);
      }else{
        color = tmpcol;
      }
      OS_Printf("id=%d, i=%d\n", id, i);
      // ���O��ID�`��
      PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(win), 5, 1+i*NAME_PRINT_HABA, wk->TrainerName[i], 
                           wk->font, color );

      PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(win), 5+13*5, 1+i*NAME_PRINT_HABA, id_str, 
                            wk->font, color );
      // ID����������
      GFL_STR_DeleteBuffer( id_str );
      GFL_STR_DeleteBuffer( tmp_str );
    }
  }
  GFL_BMPWIN_MakeTransWindow( win );

  return TRUE;
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
static int ConnectCheck( GURU2RC_WORK *wk )
{
  int i,result=0;
  const MYSTATUS *status;
  STRCODE  *namecode;
  u32 bit = Union_App_GetMemberNetBit(_get_unionwork(wk));

  // �ڑ��`�F�b�N
  for(i=0;i<RECORD_CORNER_MEMBER_MAX;i++){
    wk->ConnectCheck[i][0] = wk->ConnectCheck[i][1];
    if(bit&(1<<i)){
      wk->ConnectCheck[i][0] = 1;
    }else{
      wk->ConnectCheck[i][0] = 0;
    }

  }

  // �ڑ��������疼�O�𔽉f������
  for(i=0;i<RECORD_CORNER_MEMBER_MAX;i++){
    if(wk->ConnectCheck[i][0]){       // �ڑ����Ă��邩�H
      // �ڑ����Ă���

      status = Union_App_GetMystatus(_get_unionwork(wk),i);
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
static void RecordDataSendRecv( GURU2RC_WORK *wk )
{
  
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
static int MyStatusGetNum( GURU2RC_WORK *wk )
{
  int i,result;
  for(result=0,i=0;i<RECORD_CORNER_MEMBER_MAX;i++){
    if(Union_App_GetMystatus(_get_unionwork(wk),i)!=NULL){
      result++;
    }
  }
  return result;
}

//------------------------------------------------------------------
/**
 * @brief   ���݂̃I�����C�������r�b�g�Ŏ擾
 *
 * @param   none    
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static u32 MyStatusGetNumBit( GURU2RC_WORK *wk )
{
  int i;
  u32 result;
  for(result=0,i=0;i<RECORD_CORNER_MEMBER_MAX;i++){
    if(Union_App_GetMystatus(_get_unionwork(wk),i)!=NULL){
      result |= 1 << i;
    }
  }
  return result;
}


//  RECORD_EXIST_NO=0,
//  RECORD_EXIST_APPEAR_REQ,
//  RECORD_EXIST_APPEAR,
//  RECORD_EXIST_BYE_REQ,


//------------------------------------------------------------------
/**
 * @brief   MYSTATUS�̎擾�󋵂ɕω������������H
 *
 * @param   wk    
 *
 * @retval  int   ��������1,  �����ꍇ��0
 */
//------------------------------------------------------------------
static BOOL MyStatusCheck( GURU2RC_WORK *wk )
{
  int i,result=FALSE;
  
  // �ڑ��������疼�O�𔽉f������
  for(i=0;i<RECORD_CORNER_MEMBER_MAX;i++){
    wk->TrainerStatus[i][1] = wk->TrainerStatus[i][0];
    wk->TrainerStatus[i][0] = Union_App_GetMystatus(_get_unionwork(wk),i);

    // 080626 tomoya �u�ԓI�ɐl������ւ�����Ƃ��̂��Ƃ��l���A�g���[�iid��ۑ�
    wk->trainer_id[i][1]  = wk->trainer_id[i][0];
    if( wk->TrainerStatus[i][0] != NULL ){
      wk->trainer_id[i][0].data.tr_id = MyStatus_GetID( wk->TrainerStatus[i][0] );
      wk->trainer_id[i][0].data.in  = TRUE;

    }else{
      // �SOFF
      wk->trainer_id[i][0].check  = 0;
    }

  }

  for(i=0;i<RECORD_CORNER_MEMBER_MAX;i++){

    if(wk->TrainerStatus[i][1] != wk->TrainerStatus[i][0]){

      // �ω�����
      result = TRUE;

      // �o��E����Ȃ烊�N�G�X�g�̔��s
      if(wk->TrainerStatus[i][0]==NULL){
        wk->TrainerReq[i] = RECORD_EXIST_BYE_REQ;
      }else{
        wk->TrainerReq[i] = RECORD_EXIST_APPEAR_REQ;
      }
    }else{

      // 080626 tomoya �u�ԓI�ɐl������ւ�����Ƃ��̂��Ƃ��l��
      // �g���[�iID���ς���ĂȂ����`�F�b�N
      if( wk->trainer_id[i][0].check != wk->trainer_id[i][1].check ){

        // �ω�����
        result = TRUE;

        // �u�ԓI��i�Ԗڂ̐l������ւ�����A
        // �u�ԓI�ɓ��ꂳ����
        if( wk->trainer_id[i][0].data.in == FALSE ){  // �����炭FALSE�ł����ɗ��邱�Ƃ͂Ȃ�
          wk->TrainerReq[i] = RECORD_EXIST_BYE_REQ;
        }else{
          wk->TrainerReq[i] = RECORD_EXIST_APPEAR_REQ;
        }
      }
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
static void RecordMessagePrint( GURU2RC_WORK *wk, int msgno, int all_put )
{
  // ������擾
  STRBUF *tempbuf;
  
  tempbuf = GFL_STR_CreateBuffer(TALK_MESSAGE_BUF_NUM,HEAPID_GURU2);
  GFL_MSG_GetString(  wk->MsgManager, msgno, tempbuf );
  WORDSET_ExpandStr( wk->WordSet, wk->TalkString, tempbuf );
  GFL_STR_DeleteBuffer(tempbuf);

  // ��b�E�C���h�E�g�`��
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin),  0x0f0f );
  BmpWinFrame_Write( wk->MsgWin, WINDOW_TRANS_ON, 1, MESFRAME_PAL_INDEX );

  // ���ɕ�����`�悪�����Ă��Ă������I�ɏI�������ď㏑������
  if(wk->printStream!=NULL){
    PRINTSYS_PrintStreamDelete( wk->printStream );
    OS_Printf("PrintStream���Փ˂���\n");
  }
  wk->printStream = PRINTSYS_PrintStream( wk->MsgWin, 0,0, wk->TalkString,
                                          wk->font, MSGSPEED_GetWait(), wk->pMsgTcblSys,
                                          0,HEAPID_GURU2, 15 );
  OS_Printf("msgno:%d �\���J�n\n", msgno);

  GFL_BMPWIN_MakeTransWindow( wk->MsgWin );
}

//------------------------------------------------------------------
/**
 * @brief   ��b�\���E�C���h�E�I���҂�
 *
 * @param   msg_index   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int EndMessageWait( GURU2RC_WORK *wk )
{
  PRINTSTREAM_STATE state;
  if(wk->printStream==NULL){
    return 1;
  }

  state = PRINTSYS_PrintStreamGetState( wk->printStream );
  if(state == PRINTSTREAM_STATE_RUNNING){
    if(GFL_UI_KEY_GetCont()&PAD_BUTTON_DECIDE){
      PRINTSYS_PrintStreamShortWait( wk->printStream, 0 );
    }
  }else if(state==PRINTSTREAM_STATE_PAUSE){
    if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_DECIDE){
      PRINTSYS_PrintStreamReleasePause( wk->printStream );
    }
  }else if(state==PRINTSTREAM_STATE_DONE){
    PRINTSYS_PrintStreamDelete( wk->printStream );
    wk->printStream = NULL;
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
static void EndMessageWindowOff( GURU2RC_WORK *wk )
{
  BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );
}


//------------------------------------------------------------------
/**
 * @brief   �g���[�i�[OBJ�̕\������
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void TrainerObjFunc( GURU2RC_WORK *wk )
{
  int i,seflag=0;
  
  for(i=0;i<RECORD_CORNER_MEMBER_MAX;i++){
    switch(wk->TrainerReq[i]){
    case RECORD_EXIST_NO:
      break;
    case RECORD_EXIST_APPEAR_REQ:   // �������ꂽ�̂łn�a�i�o��
      {
        int view,sex;
        GF_ASSERT(wk->TrainerStatus[i][0]!=NULL);
        
        sex = MyStatus_GetMySex( wk->TrainerStatus[i][0] );
        view  = MyStatus_GetTrainerView( wk->TrainerStatus[i][0] );
        if(GFL_NET_SystemGetCurrentID()==i){
          // ��l��OBJ�p�̃A�j��
          GFL_CLACT_WK_SetAnmSeq( wk->MainActWork[i+1], 4 );
        }else{
          // �t�B�[���hOBJ�p�̃A�j��
          TransFieldObjData( wk, wk->UnionObjPalData,  i, view, sex );
          GFL_CLACT_WK_SetAnmSeq( wk->MainActWork[i+1], 4 );
        }

      }
      GFL_CLACT_WK_SetDrawEnable( wk->MainActWork[i+1], 1 );
      wk->TrainerReq[i] = RECORD_EXIST_APPEAR;
      seflag = 1;
      break;
    case RECORD_EXIST_APPEAR:
      break;
    case RECORD_EXIST_BYE_REQ:     // �ؒf���ꂽ�̂ŃT���i���n�a�i
      if(GFL_NET_SystemGetCurrentID()==i){
        int sex = MyStatus_GetMySex( wk->TrainerStatus[i][0] );
        GFL_CLACT_WK_SetAnmSeq( wk->MainActWork[i+1], 5 );
      }else{
        GFL_CLACT_WK_SetAnmSeq( wk->MainActWork[i+1], 5 );
      }
      wk->TrainerReq[i] = RECORD_EXIST_NO;
      break;
    }
  }
  
  // �o��SE���K�v�ȏꍇ�͖炷
  if(seflag){
    PMSND_PlaySE( SEQ_SE_FLD_05 );  
  }
}


//------------------------------------------------------------------
/**
 * @brief   �t�B�[���hOBJ�摜�ǂݍ��݁i���j�I���Ǝ��@�j
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void LoadFieldObjData( GURU2RC_WORK *wk, ARCHANDLE* p_handle )
{
  // �p���b�g�ǂݍ���
  wk->UnionObjPalBuf = GFL_ARC_UTIL_LoadPalette( ARCID_WIFIUNIONCHAR, 
                                                 NARC_wifi_unionobj_wifi_union_obj_NCLR, 
                                                 &wk->UnionObjPalData, HEAPID_GURU2 );
//  wk->FieldObjPalBuf[1] = GFL_ARCHDL_UTIL_LoadPalette( p_handle, NARC_guru2_2d_union_chara_NCLR, &(wk->FieldObjPalData[1]), HEAPID_GURU2 );

  // �摜�ǂݍ���
//  wk->FieldObjCharaBuf[0] = GFL_ARC_UTIL_LoadOBJCharacter( ARCID_WORLDTRADE_GRA, NARC_worldtrade_hero_lz_ncgr, 1, &(wk->FieldObjCharaData[0]), HEAPID_GURU2 );
//  wk->FieldObjCharaBuf[1] = GFL_ARCHDL_UTIL_LoadOBJCharacter( p_handle, NARC_guru2_2d_union_chara_NCGR,  0, &(wk->FieldObjCharaData[1]), HEAPID_GURU2 );

}

//------------------------------------------------------------------
/**
 * @brief   �t�B�[���hOBJ�摜���
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void FreeFieldObjData( GURU2RC_WORK *wk )
{

  GFL_HEAP_FreeMemory( wk->UnionObjPalBuf  );
//  GFL_HEAP_FreeMemory( wk->FieldObjPalBuf[1]  );
                      
//  GFL_HEAP_FreeMemory( wk->FieldObjCharaBuf[0] );
//  GFL_HEAP_FreeMemory( wk->FieldObjCharaBuf[1] );
}


#define OBJ_TRANS_SIZE  ( 3 * 4*4 )
static const u16 obj_offset[]={
  ( 13                    )*0x20,
  ( 13 + OBJ_TRANS_SIZE*1 )*0x20,
  ( 13 + OBJ_TRANS_SIZE*2 )*0x20,
  ( 13 + OBJ_TRANS_SIZE*3 )*0x20,
  ( 13 + OBJ_TRANS_SIZE*4 )*0x20,
};


static int _obj_no = 0;
static int _pal_no = 0;

//------------------------------------------------------------------
/**
 * @brief   �n���ꂽ���j�I�������ڃR�[�h����t�B�[���hOBJ�̃L������]������
 *
 * @param   id    
 * @param   view    
 * @param   sex   
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void TransFieldObjData( GURU2RC_WORK *wk, NNSG2dPaletteData *PalData, int id, int view, int sex )
{
/*
  u8 *chara, *pal;
  pos   = view;

  chara = (u8*)CharaData[1]->pRawData;
  pal   = (u8*)PalData[1]->pRawData;

  GX_LoadOBJ( &chara[OBJ_TRANS_SIZE*pos*0x20], obj_offset[id], OBJ_TRANS_SIZE*0x20 );
  GX_LoadOBJPltt( &pal[pos*32], (id+FIELDOBJ_PAL_START)*32, 32 );
*/
  void *chrbuf;
  NNSG2dCharacterData *data;
  int offset = sex*8+view;    // �j���W�l�v�P�U�l���̃t�@�C���A�N�Z�X
  u16 *pal = (u16*)PalData->pRawData;
  
  // �p���b�g�]��
  // �p���b�g�f�[�^�𒼐ړ]��
  GX_LoadOBJPltt( &pal[sc_wifi_unionobj_plt[view]*16], id*32, 32 );

  OS_Printf("id=%d, sex=%d, view=%d, offset=%d\n", id, sex, view, offset);

  // �L�����]��
  chrbuf = GFL_ARC_UTIL_LoadOBJCharacter( ARCID_WIFIUNIONCHAR, NARC_wifi_unionobj_normal00_NCGR+view,
                                       0, &data, HEAPID_GURU2 );  
  GFL_CLGRP_CGR_Replace( wk->resObjTbl[GURU2_CLACT_OBJ0_RES_CHR+id], data );

  GFL_HEAP_FreeMemory( chrbuf );
  OS_Printf("ID=%d �̃��j�I�������ڂ� %d\n", id, view);

  
  
}


//------------------------------------------------------------------
/**
 * @brief   ��b�X�s�[�h���擾
 *
 * @param   wk    
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int GetTalkSpeed( GURU2RC_WORK *wk )
{
  return 1;
//  return CONFIG_GetMsgPrintSpeed(wk->g2p->param.config);
}


//------------------------------------------------------------------
/**
 * @brief   ��W�l���ƃr�[�R������ݒ肷��
 *
 * @param   wk    
 * @param   plus  1�Ȃ��W���������A0�Ȃ���ߐ؂肽���Ƃ�
 *
 * @retval  none
 */
//------------------------------------------------------------------
static int RecordCorner_BeaconControl( GURU2RC_WORK *wk, int plus )
{
  int num;
  // �r�[�R��������������
  num = MyStatusGetNum(wk);

  if(num>wk->g2c->shareNum){
    if(Union_App_Parent_EntryBlock(_get_unionwork(wk))){
      u8 flag = GURU2COMM_BAN_ON;
      // ���E�֎~�ʒB
//      Guru2Comm_SendData(wk->g2c, G2COMM_RC_BAN, &flag, 1 );
      wk->beacon_flag = GURU2COMM_BAN_ON;
    }
  }
  else{
    wk->beacon_flag = GURU2COMM_BAN_NONE;
  }

  // �ڑ��l���ƃ��R�[�h�����\�l������v����܂ł͑���֎~
  if(num==wk->connectBackup){
    return SEQ_MAIN;
  }

  OS_TPrintf("�l�����ς����\n");

  // �������Ȃ��A�����ߐ؂�ł��Ȃ��Ȃ��W���s
  switch(num){
  case 1:
    // �ЂƂ�ɂȂ����̂ŏI��
    OS_TPrintf("ONLY!! Comm = %d, My = %d, Bit = %d\n", Union_App_GetMemberNum(_get_unionwork(wk)), MyStatusGetNum(wk), Union_App_GetMemberNetBit(_get_unionwork(wk)));
    if(  Union_App_GetMemberNum(_get_unionwork(wk)) > 1 
      || Union_App_GetMemberNetBit(_get_unionwork(wk)) > 1){
      return SEQ_MAIN;  //���S�Ɉ�l�ɂ͂Ȃ��Ă��Ȃ��̂ł܂��������Ȃ�
    }
    wk->seq = RECORD_MODE_END_PARENT_ONLY;
    //ChangeConnectMax( wk, -1 );
    if(wk->YesNoMenuWork!=NULL){
      BmpMenu_YesNoMenuExit( wk->YesNoMenuWork );
      wk->YesNoMenuWork = NULL;
    }
    return SEQ_LEAVE;
    break;
  case 2: case 3:case 4:
    // �܂�������
//    Union_BeaconChange( UNION_PARENT_MODE_GURU2_FREE );

    // �ڑ��l�����������ꍇ�͐ڑ��ő�l�������炷
    if(num<wk->connectBackup){
      switch(wk->limit_mode){
      case LIMIT_MODE_NONE:
        //ChangeConnectMax( wk, plus );
        break;
      case LIMIT_MODE_TRUE:
        //ChangeConnectMax(wk, 0);
        break;
      }
      wk->g2c->shareNum = Union_App_GetMemberNum(_get_unionwork(wk));
    }
    break;
  case 5:
    // �����ς��ł�
//    Union_BeaconChange( UNION_PARENT_MODE_GURU2 );
    //ChangeConnectMax( wk, plus );
    break;
  }

  
  // �ڑ��l����ۑ�
  wk->connectBackup = MyStatusGetNum(wk);

  return SEQ_MAIN;
}



//----------------------------------------------------------------------------------
/**
 * @brief �^�b�`��A�{�^���Ƃ݂Ȃ��L�[�擾���[�`��
 *
 * @param   none    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int _get_key_trg( void )
{
  // �L�[���擾
  int key = GFL_UI_KEY_GetTrg();

  // �^�b�`TRG�ł�A�{�^������ǉ�
  //if( GFL_UI_TP_GetTrg() ){
  //  key |= PAD_BUTTON_DECIDE;
  //}

  return key;
}
