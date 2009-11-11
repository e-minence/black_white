//==============================================================================
/**
 * @file  trade.c
 * @brief �|�P�����������
 * @author  matsuda
 * @date  2008.12.05(��)
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>
#include "system\main.h"
#include "print\printsys.h"
#include "print\gf_font.h"
#include "arc_def.h"
#include "message.naix"
#include "msg\msg_d_matsu.h"
#include "test/performance.h"
#include "font/font.naix"
#include "pm_define.h"
#include "gamesystem/game_data.h"
#include "net\network_define.h"
#include "trade.h"
#include "backup_system.h"
#include "system\gfl_use.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�A�N�^�[�ő吔
#define ACT_MAX     (64)

///�t�H���g�̃p���b�g�ԍ�
#define D_FONT_PALNO  (14)

///�^�C�g�����SBG��BG�v���C�I���e�B
enum{
  BGPRI_MSG = 0,
  BGPRI_MIST = 0,
  BGPRI_TITLE_LOGO = 2,
  BGPRI_3D = 1,
};

///�t���[���ԍ�
enum{
  //���C�����
  FRAME_WALL_M = GFL_BG_FRAME3_M,   ///<�ǎ�
  FRAME_MSG_M = GFL_BG_FRAME1_M,    ///<���b�Z�[�W�t���[��

  //�T�u���
  FRAME_WALL_S = GFL_BG_FRAME3_S,   ///<�ǎ�
};


//==============================================================================
//  �\���̒�`
//==============================================================================
typedef struct{
  GFL_BMPWIN    *win;
  GFL_BMP_DATA  *bmp;
  PRINT_UTIL    printUtil[1];
  BOOL      message_req;    ///<TRUE:���b�Z�[�W���N�G�X�g��������
}DM_MSG_DRAW_WIN;

typedef struct {
  u16   seq;
  HEAPID  heapID;
  BOOL connect_ok;

  GFL_FONT    *fontHandle;
  PRINT_QUE   *printQue;
  PRINT_STREAM  *printStream;
  GFL_MSGDATA   *mm;
  STRBUF      *strbuf;
  GFL_TCBLSYS   *tcbl;
  DM_MSG_DRAW_WIN drawwin;

  BOOL poke_receive;  ///<TRUE:����̃|�P��������M����
  TRADE_DISP_PARENT *disp_parent;

  //�A�N�^�[
  GFL_CLUNIT *clunit;
  GFL_TCB *vintr_tcb;
}TRADE_WORK;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
GFL_PROC_RESULT TradeProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
GFL_PROC_RESULT TradeProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
GFL_PROC_RESULT TradeProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static void Local_VramSetting(TRADE_WORK *tw);
static void Local_BGFrameSetting(TRADE_WORK *tw);
static void Local_MessageSetting(TRADE_WORK *tw);
static void Local_ActorSetting(TRADE_WORK *tw);
static void Local_MessagePrintMain(TRADE_WORK *tw);
static void Local_MessagePut(TRADE_WORK *tw, int win_index, STRBUF *strbuf, int x, int y);
static void Local_MsgLoadPokeNameAll(TRADE_WORK *tw, POKEPARTY *party);
static void _connectCallBack(void* pWork, int netID);
static void _endCallBack(void* pWork);
static void _RecvPokemon(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void VintrTCB_VblankFunc(GFL_TCB *tcb, void *work);


//==============================================================================
//  �f�[�^
//==============================================================================
///�^�C�g����ʌĂяo���悤��PROC�f�[�^
const GFL_PROC_DATA TradeProcData = {
  TradeProcInit,
  TradeProcMain,
  TradeProcEnd,
};

//--------------------------------------------------------------
//  �ʐM
//--------------------------------------------------------------
static const NetRecvFuncTable _CommPacketTbl[] = {
    {_RecvPokemon,         NULL},    ///NET_CMD_POKEMON
};

enum{
  NET_CMD_POKEMON = GFL_NET_CMD_TRADE,
};

#define _MAXNUM       2//(4)         // �ő�ڑ��l��
#define _MAXSIZE    (32)        // �ő呗�M�o�C�g��
#define _BCON_GET_NUM (16)    // �ő�r�[�R�����W��

static const GFLNetInitializeStruct aGFLNetInit = {
    _CommPacketTbl,  // ��M�֐��e�[�u��
    NELEMS(_CommPacketTbl), // ��M�e�[�u���v�f��
    NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
    _connectCallBack,    ///< �l�S�V�G�[�V�����������ɃR�[��
    NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
    NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
    NULL,  // �r�[�R���f�[�^�擾�֐�
    NULL,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    NULL,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
    FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    _endCallBack,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    NULL,   ///< DWC�`���̗F�B���X�g
    NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    0,   ///< DWC�ւ�HEAP�T�C�Y
    TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
    0x531,//0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    HEAPID_IRC,   //��check�@�ԊO���ʐM�p��create�����HEAPID
    GFL_WICON_POSX, GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    _MAXNUM,     // �ő�ڑ��l��
    _MAXSIZE,  //�ő呗�M�o�C�g��
    _BCON_GET_NUM,    // �ő�r�[�R�����W��
    TRUE,     // CRC�v�Z
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    GFL_NET_TYPE_WIRELESS,  //GFL_NET_TYPE_IRC_WIRELESS,//GFL_NET_TYPE_WIRELESS,//GFL_NET_TYPE_IRC,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
    WB_NET_TRADE_SERVICEID,  //GameServiceID
#if GFL_NET_IRC
  IRC_TIMEOUT_STANDARD, // �ԊO���^�C���A�E�g����
#endif
  0,//MP�e�ő�T�C�Y 512�܂�
  0,//dummy
};

//==============================================================================
//
//
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief
 *
 * @param   proc
 * @param   seq
 * @param   pwk
 * @param   mywk
 *
 * @retval
 */
//--------------------------------------------------------------
GFL_PROC_RESULT TradeProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  TRADE_WORK *tw;
  TRADE_DISP_PARENT *disp_parent = pwk;

  // �e����ʃ��W�X�^������
  GX_SetMasterBrightness(-16);
  GXS_SetMasterBrightness(-16);
  GFL_DISP_GX_SetVisibleControlDirect(0);   //�SBG&OBJ�̕\��OFF
  GFL_DISP_GXS_SetVisibleControlDirect(0);
  G2_BlendNone();
  G2S_BlendNone();
  GX_SetVisibleWnd(GX_WNDMASK_NONE);
  GXS_SetVisibleWnd(GX_WNDMASK_NONE);

  //�q�[�v�쐬
//  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_TRADE, 0x70000 );
  tw = GFL_PROC_AllocWork( proc, sizeof(TRADE_WORK), HEAPID_TRADE );
  GFL_STD_MemClear(tw, sizeof(TRADE_WORK));
  tw->disp_parent = pwk;

  // �㉺��ʐݒ�
  GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
  GFL_DISP_SetDispOn();

  //TCB�쐬
  tw->tcbl = GFL_TCBL_Init( HEAPID_TRADE, HEAPID_TRADE, 4, 32 );

  //VRAM�ݒ� & BG�t���[���ݒ�
  Local_VramSetting(tw);
  Local_BGFrameSetting(tw);

  //���b�Z�[�W�֘A�쐬
  GFL_BMPWIN_Init( HEAPID_TRADE );
  GFL_FONTSYS_Init();
  Local_MessageSetting(tw);

  //�A�N�^�[�ݒ�
  Local_ActorSetting(tw);

  GFL_BG_SetVisible(FRAME_WALL_M, VISIBLE_ON);
  GFL_BG_SetVisible(FRAME_MSG_M, VISIBLE_ON);
//  GFL_BG_SetVisible(FRAME_WALL_S, VISIBLE_ON);

  GFL_FADE_SetMasterBrightReq(
    GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 0);

  tw->vintr_tcb = GFUser_VIntr_CreateTCB(VintrTCB_VblankFunc, tw, 5);

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
GFL_PROC_RESULT TradeProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  TRADE_WORK* tw = mywk;
  TRADE_DISP_PARENT *disp_parent = pwk;
  enum{
    SEQ_INIT,
    SEQ_WAIT,
    SEQ_CONNECT_WAIT,
    SEQ_SELECT_INIT,
    SEQ_SELECT_WAIT,

    SEQ_CHANGE_START,
    SEQ_CHANGE_TIMING,
    SEQ_CHANGE_TIMING_WAIT,
    SEQ_CHANGE_SEND_START,
    SEQ_CHANGE_SEND_WAIT,
    SEQ_CHANGE_SAVE_BEFORE_TIMING_WAIT,
    SEQ_CHANGE_SAVE_WAIT,
    SEQ_CHANGE_SAVE_LAST_TIMING_WAIT,
    SEQ_CHANGE_SAVE_LAST,
    SEQ_CHANGE_SAVE_AFTER_TIMING_WAIT,
    SEQ_CHANGE_END,

    SEQ_SIO_SHUTDOWN_BEFORE,
    SEQ_SIO_SHUTDOWN_BEFORE_WAIT,
    SEQ_SIO_SHUTDOWN,
    SEQ_SIO_SHUTDOWN_WAIT,
    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
  };

  GFL_TCBL_Main( tw->tcbl );
  Local_MessagePrintMain(tw);

  switch(tw->seq){
  case SEQ_INIT:
    if(GFL_FADE_CheckFade() == TRUE){
      break;
    }

    if(disp_parent->first_seq == TRADE_STARTUP_SEQ_CHANGE){
      disp_parent->first_seq = TRADE_STARTUP_SEQ_SELECT;
      tw->seq = SEQ_CHANGE_START;
      break;
    }
    if(disp_parent->first_seq == TRADE_STARTUP_SEQ_SELECT){
      tw->seq = SEQ_SELECT_INIT;
      break;
    }

    GFL_MSG_GetString(tw->mm, DM_MSG_TRADE005, tw->strbuf); //�ʐM�����T���Ă��܂�
    Local_MessagePut(tw, 0, tw->strbuf, 0, 0);

    //�ʐM�����ݒ�
    GFL_NET_Init(&aGFLNetInit, NULL, tw); //�ʐM������

    tw->seq++;
    break;
  case SEQ_WAIT:
    if(GFL_NET_IsInit() == TRUE){
      GFL_NET_ChangeoverConnect(NULL); // �����ڑ�
      tw->seq++;
    }
    break;
  case SEQ_CONNECT_WAIT:
    //�����ڑ��҂�
    if(tw->connect_ok == TRUE){
      OS_TPrintf("�ڑ�����\n");
      tw->seq++;
    }
    break;
  case SEQ_SELECT_INIT:
    GFL_MSG_GetString(tw->mm, DM_MSG_TRADE000, tw->strbuf); //�I�����Ă�������
    Local_MessagePut(tw, 0, tw->strbuf, 0, 0);
    tw->seq++;
    break;
  case SEQ_SELECT_WAIT:
    if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_A){
      disp_parent->select_menu = TRADE_MENU_LIST;
      tw->seq = SEQ_FADEOUT_START;
    }
    else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_B){
      disp_parent->select_menu = TRADE_MENU_CANCEL;
      tw->seq = SEQ_SIO_SHUTDOWN_BEFORE;
    }
    break;

  //�|�P���������J�n
  case SEQ_CHANGE_START:
    GFL_MSG_GetString(tw->mm, DM_MSG_TRADE001, tw->strbuf); //xxx�������ɏo���܂�
    Local_MessagePut(tw, 0, tw->strbuf, 0, 0);
    GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 0);
    tw->seq++;
    break;
  case SEQ_CHANGE_TIMING:
    if(GFL_FADE_CheckFade() == FALSE){
      GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,16);
      tw->seq++;
    }
    break;
  case SEQ_CHANGE_TIMING_WAIT:
    if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),16) == TRUE){
      GFL_MSG_GetString(tw->mm, DM_MSG_TRADE002, tw->strbuf); //�ʐM���Ă��܂�
      Local_MessagePut(tw, 0, tw->strbuf, 0, 2*8);
      tw->seq++;
    }
    break;
  case SEQ_CHANGE_SEND_START:
    if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),
        NET_CMD_POKEMON, POKETOOL_GetWorkSize(),
        PokeParty_GetMemberPointer(disp_parent->party, disp_parent->sel_mons))){
      tw->seq++;
    }
    break;
  case SEQ_CHANGE_SEND_WAIT:
    if(tw->poke_receive == TRUE){
      GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,17);
      tw->seq++;
    }
    break;
  case SEQ_CHANGE_SAVE_BEFORE_TIMING_WAIT:
    if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),17) == TRUE){
      GFL_MSG_GetString(tw->mm, DM_MSG_TRADE003, tw->strbuf); //�Z�[�u���Ă��܂�
      Local_MessagePut(tw, 0, tw->strbuf, 0, 2*8);
      SaveControl_SaveAsyncInit(disp_parent->sv);
      tw->seq++;
    }
    break;
  case SEQ_CHANGE_SAVE_WAIT:
    if(SaveControl_SaveAsyncMain(disp_parent->sv) == SAVE_RESULT_LAST){
      GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,18);
      OS_TPrintf("���X�g1�o�C�g�@�����J�n\n");
      tw->seq++;
    }
    break;
  case SEQ_CHANGE_SAVE_LAST_TIMING_WAIT:
    if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),18) == TRUE){
      OS_TPrintf("���X�g1�o�C�g�@��������\n");
      tw->seq++;
    }
    break;
  case SEQ_CHANGE_SAVE_LAST:
    if(SaveControl_SaveAsyncMain(disp_parent->sv) == SAVE_RESULT_OK){
      GFL_MSG_GetString(tw->mm, DM_MSG_TRADE004, tw->strbuf); //�Z�[�u���Ă��܂�
      GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,19);
      tw->seq++;
    }
    break;
  case SEQ_CHANGE_SAVE_AFTER_TIMING_WAIT:
    if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),19) == TRUE){
      Local_MessagePut(tw, 0, tw->strbuf, 0, 4*8);
      tw->seq++;
    }
    break;
  case SEQ_CHANGE_END:
    if(GFL_UI_KEY_GetTrg()){
      tw->seq = SEQ_SELECT_INIT;
    }
    break;

  case SEQ_SIO_SHUTDOWN_BEFORE:
    GFL_MSG_GetString(tw->mm, DM_MSG_TRADE007, tw->strbuf); //����̑I����҂��Ă��܂�
    Local_MessagePut(tw, 0, tw->strbuf, 0, 6*8);
    GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,100);
    tw->seq++;
    break;
  case SEQ_SIO_SHUTDOWN_BEFORE_WAIT:
    if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),100) == TRUE){
      tw->seq++;
    }
    break;
  case SEQ_SIO_SHUTDOWN:
    if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_CMD_EXIT_REQ, 0, NULL)){
      GFL_MSG_GetString(tw->mm, DM_MSG_TRADE006, tw->strbuf); //�ؒf���Ă��܂�
      Local_MessagePut(tw, 0, tw->strbuf, 0, 6*8);
      tw->seq++;
    }
    break;
  case SEQ_SIO_SHUTDOWN_WAIT:
    if(tw->connect_ok == FALSE){
      tw->seq = SEQ_FADEOUT_START;
    }
    break;
  case SEQ_FADEOUT_START:
    GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 0, 16, 0);
    tw->seq++;
  case SEQ_FADEOUT_WAIT:
    if(GFL_FADE_CheckFade() == FALSE){
      return GFL_PROC_RES_FINISH;
    }
    break;
  }

  GFL_CLACT_SYS_Main();
  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
GFL_PROC_RESULT TradeProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  TRADE_WORK* tw = mywk;

  GFL_TCB_DeleteTask(tw->vintr_tcb);

  GFL_STR_DeleteBuffer(tw->strbuf);
  GFL_BMPWIN_Delete(tw->drawwin.win);

  PRINTSYS_QUE_Delete(tw->printQue);
  GFL_MSG_Delete(tw->mm);

  GFL_CLACT_UNIT_Delete(tw->clunit);
  GFL_CLACT_SYS_Delete();

  GFL_FONT_Delete(tw->fontHandle);
  GFL_TCBL_Exit(tw->tcbl);

  GFL_BG_Exit();
  GFL_BMPWIN_Exit();

  GFL_PROC_FreeWork(proc);
//  GFL_HEAP_DeleteHeap(HEAPID_TRADE);

  return GFL_PROC_RES_FINISH;
}

static void VintrTCB_VblankFunc(GFL_TCB *tcb, void *work)
{
  GFL_CLACT_SYS_VBlankFunc();
}

//==============================================================================
//
//==============================================================================
static const GFL_DISP_VRAM vramBank = {
  GX_VRAM_BG_128_B,       // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
  GX_VRAM_SUB_BG_128_C,     // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
  GX_VRAM_OBJ_64_E,       // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g
  GX_VRAM_SUB_OBJ_16_I,     // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
  GX_VRAM_TEX_0_A,        // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_01_FG,      // �e�N�X�`���p���b�g�X���b�g
  GX_OBJVRAMMODE_CHAR_1D_128K,  // ���C��OBJ�}�b�s���O���[�h
  GX_OBJVRAMMODE_CHAR_1D_32K,   // �T�uOBJ�}�b�s���O���[�h
};
//--------------------------------------------------------------
/**
 * @brief   VRAM�o���N�����[�h�ݒ�
 * @param   tw    �^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_VramSetting(TRADE_WORK *tw)
{

  static const GFL_BG_SYS_HEADER sysHeader = {
    GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
  };

  //VRAM�N���A
  GFL_STD_MemFill16((void*)HW_BG_VRAM, 0x0000, HW_BG_VRAM_SIZE);
  GFL_STD_MemFill16((void*)HW_BG_PLTT, 0x7fff, HW_BG_PLTT_SIZE);  //�p���b�gVRAM�N���A

  GFL_DISP_SetBank( &vramBank );
  GFL_BG_Init( HEAPID_TRADE );
  GFL_BG_SetBGMode( &sysHeader );
}

//--------------------------------------------------------------
/**
 * @brief   BG�t���[���ݒ�
 * @param   tw    �^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_BGFrameSetting(TRADE_WORK *tw)
{
  static const GFL_BG_BGCNT_HEADER bgcnt_frame[] = {  //���C�����BG�t���[��
    {//FRAME_WALL_M
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x0c000, 0x4000,
      GX_BG_EXTPLTT_01, BGPRI_TITLE_LOGO, 0, 0, FALSE
    },
    {//FRAME_MSG_M
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x04000, 0x8000,
      GX_BG_EXTPLTT_01, BGPRI_MIST, 0, 0, FALSE
    },
  };

  static const GFL_BG_BGCNT_HEADER sub_bgcnt_frame[] = {  //�T�u���BG�t���[��
    {//FRAME_WALL_S
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, 0x8000,
      GX_BG_EXTPLTT_01, BGPRI_TITLE_LOGO, 0, 0, FALSE
    },
  };

  GFL_BG_SetBGControl( FRAME_WALL_M,   &bgcnt_frame[0],   GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( FRAME_MSG_M,   &bgcnt_frame[1],   GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( FRAME_WALL_S,   &sub_bgcnt_frame[0],   GFL_BG_MODE_TEXT );

  GFL_BG_FillCharacter( FRAME_WALL_M, 0x00, 1, 0 );
  GFL_BG_FillCharacter( FRAME_MSG_M, 0x00, 1, 0 );
  GFL_BG_FillCharacter( FRAME_WALL_S, 0x00, 1, 0 );

  GFL_BG_FillScreen( FRAME_WALL_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
  GFL_BG_FillScreen( FRAME_MSG_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
  GFL_BG_FillScreen( FRAME_WALL_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

  GFL_BG_LoadScreenReq( FRAME_WALL_M );
  GFL_BG_LoadScreenReq( FRAME_MSG_M );
  GFL_BG_LoadScreenReq( FRAME_WALL_S );
}

//--------------------------------------------------------------
/**
 * @brief   �A�N�^�[�ݒ�
 * @param   tw    �^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_ActorSetting(TRADE_WORK *tw)
{
  GFL_CLSYS_INIT clsys_init = GFL_CLSYSINIT_DEF_DIVSCREEN;

  clsys_init.oamst_main = GFL_CLSYS_OAMMAN_INTERVAL;  //�ʐM�A�C�R���̕�
  clsys_init.oamnum_main = 128-GFL_CLSYS_OAMMAN_INTERVAL;
  clsys_init.tr_cell = ACT_MAX;
  GFL_CLACT_SYS_Create(&clsys_init, &vramBank, HEAPID_TRADE);

  tw->clunit = GFL_CLACT_UNIT_Create(ACT_MAX, 0, HEAPID_TRADE);
  GFL_CLACT_UNIT_SetDefaultRend(tw->clunit);

  //OBJ�\��ON
  GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
}

//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�֘A�ݒ�
 * @param   tw    �^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_MessageSetting(TRADE_WORK *tw)
{
  int i;

  //BMPWIN�쐬
  tw->drawwin.win
    = GFL_BMPWIN_Create(FRAME_MSG_M, 0, 0, 30, 20, D_FONT_PALNO, GFL_BMP_CHRAREA_GET_F);
  tw->drawwin.bmp = GFL_BMPWIN_GetBmp(tw->drawwin.win);
  GFL_BMP_Clear( tw->drawwin.bmp, 0x00 );
  GFL_BMPWIN_MakeScreen( tw->drawwin.win );
  GFL_BMPWIN_TransVramCharacter( tw->drawwin.win );
  PRINT_UTIL_Setup( tw->drawwin.printUtil, tw->drawwin.win );
  GFL_BG_LoadScreenReq( FRAME_MSG_M );

  //�t�H���g�쐬
  tw->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
    GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_TRADE );

  tw->printQue = PRINTSYS_QUE_Create( HEAPID_TRADE );

  tw->mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_matsu_dat, HEAPID_TRADE );

  //STRBUF�쐬
  tw->strbuf = GFL_STR_CreateBuffer( 256, tw->heapID );

  //�t�H���g�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
    0x20*D_FONT_PALNO, 0x20, HEAPID_TRADE);
}

//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�`�惁�C��
 * @param   tw    �^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_MessagePrintMain(TRADE_WORK *tw)
{
  int i, main_ret;

  main_ret = PRINTSYS_QUE_Main( tw->printQue );

  if( PRINT_UTIL_Trans( tw->drawwin.printUtil, tw->printQue ) == FALSE){
    //return GFL_PROC_RES_CONTINUE;
  }
  else{
    if(main_ret == TRUE && tw->drawwin.message_req == TRUE){
//      GFL_BMP_Clear( tw->bmp, 0xff );
      GFL_BMPWIN_TransVramCharacter( tw->drawwin.win );
      tw->drawwin.message_req = FALSE;
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W��\������
 *
 * @param   tw
 * @param   strbuf    �\�����郁�b�Z�[�W�f�[�^
 * @param   x     X���W(dot)
 * @param   y     Y���W(dot)
 */
//--------------------------------------------------------------
static void Local_MessagePut(TRADE_WORK *tw, int win_index, STRBUF *strbuf, int x, int y)
{
  GFL_BMP_Clear( tw->drawwin.bmp, 0x00 );
  PRINTSYS_PrintQue(tw->printQue, tw->drawwin.bmp, x, y, strbuf, tw->fontHandle);
  tw->drawwin.message_req = TRUE;
}


//==============================================================================
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �ڑ������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------
static void _connectCallBack(void* pWork, int netID)
{
  TRADE_WORK *tw = pWork;

    OS_TPrintf("�l�S�V�G�[�V��������\n");
    tw->connect_ok = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �ؒf�����R�[���o�b�N
 * @param   pWork
 */
//--------------------------------------------------------------
static void _endCallBack(void* pWork)
{
  TRADE_WORK *tw = pWork;

    NET_PRINT("endCallBack�I��\n");
    tw->connect_ok = FALSE;
}

//==============================================================================
//  ��M�֐�
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �|�P�����f�[�^��M
 *
 * @param   netID
 * @param   size
 * @param   pData
 * @param   pWork
 * @param   pNetHandle
 */
//--------------------------------------------------------------
static void _RecvPokemon(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  TRADE_WORK *tw = pWork;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return; //�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if(netID == GFL_NET_HANDLE_GetNetHandleID(pNetHandle)){
    return; //�����̃f�[�^�͖���
  }

  OS_TPrintf("�|�P������M netID = %d, size = %d\n", netID, size);

  //�����̑I�������|�P�������폜���A��M�����|�P������ǉ�
  PokeParty_Delete(tw->disp_parent->party, tw->disp_parent->sel_mons);
  PokeParty_Add(tw->disp_parent->party, pData);

  tw->poke_receive = TRUE;
}
