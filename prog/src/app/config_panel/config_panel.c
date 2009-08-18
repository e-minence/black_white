//============================================================================================
/**
 *  @file config_panel.c
 *  @brief  �R���t�B�O���
 *  @date 06.01.30
 *  @author Miyuki Iwasawa
 *
 *  2008.12.11  tamada  DP�����ڐA
 */
//============================================================================================
#include <gflib.h>


#include "system/gfl_use.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menu.h"
#include "font/font.naix"


#include "app/config_panel.h"
#include "savedata/config.h"


#include "arc_def.h"
#include "system/main.h"
#define HEAPID_BASE_APP GFL_HEAPID_APP

#include "arc/config_gra.naix"

#include "message.naix"
#include "msg/msg_config.h"

#include "./config_dummy.h"

//============================================================================================
//============================================================================================
#define CONFIG_DECIDE_END (1)
#define CONFIG_CANCEL_END (2)

#define FONT_TALK_PAL (12)
#define FONT_SYS_PAL  (13)

#define WINCLR_COL(col) (((col)<<4)|(col))

#define CONFIG_BMPCHAR_BASE (10)
//20
#define BMPL_TITLE_PX (1)
#define BMPL_TITLE_PY (0)
#define BMPL_TITLE_SX (12)
#define BMPL_TITLE_SY (2)
#define BMPL_TITLE_PAL  (FONT_SYS_PAL)
#define BMPL_TITLE_CGX  (CONFIG_BMPCHAR_BASE)
#define BMPL_TITLE_FRM  (GFL_BG_FRAME1_M)

//600
#define BMPL_MENU_PX  (1)
#define BMPL_MENU_PY  (3)
#define BMPL_MENU_SX  (30)
#define BMPL_MENU_SY  (14)
#define BMPL_MENU_PAL (FONT_SYS_PAL)
#define BMPL_MENU_CGX (BMPL_TITLE_CGX+BMPL_TITLE_SX*BMPL_TITLE_SY)
#define BMPL_MENU_FRM (GFL_BG_FRAME1_M)

//112
#define BMPL_SUM_PX (2)
#define BMPL_SUM_PY (19)
#define BMPL_SUM_SX (27)
#define BMPL_SUM_SY (4)
#define BMPL_SUM_PAL  (FONT_TALK_PAL)
#define BMPL_SUM_CGX  (BMPL_MENU_CGX+BMPL_MENU_SX*BMPL_MENU_SY)
#define BMPL_SUM_FRM  (GFL_BG_FRAME1_M)

#define BMPL_MENU_WIN_CGX (BMPL_SUM_CGX+BMPL_SUM_SX*BMPL_SUM_SY)
#define BMPL_TALK_WIN_CGX (BMPL_MENU_WIN_CGX+MENU_WIN_CGX_SIZ)
#define BMPL_MENU_WIN_PAL (14)
#define BMPL_TALK_WIN_PAL (15)

// YesNoWin
#define BMPL_YESNO_CX (25)
#define BMPL_YESNO_CY (13)
#define BMPL_YESNO_CSX  (6)
#define BMPL_YESNO_CSY  (4)
#define BMPL_YESNO_PAL  (FONT_SYS_PAL)
#define BMPL_YESNO_CGX  (BMPL_TALK_WIN_CGX+TALK_WIN_CGX_SIZ)
#define BMPL_YESNO_FRM  (GFL_BG_FRAME3_M)

enum{
  WIN_TITLE,
  WIN_MENU,
  WIN_SUM,
  WIN_MAX
};

#define MENU_SEL_MAX  (20)
#define MENU_LINE_MAX (7)

#define MENU_STRSIZ (10*2)
#define SUM_STRSIZ  (256)

#define MENU_SEL_OFSX (12*8+4)
#define MENU_SEL_OFSY (0)
#define MENU_SEL_DSIZ (48)
#define MENU_SEL_ASIZX  (48*8)
#define MENU_SEL_ASIZY  (16)
#define MENU_WIN_OFSY (24)

//�E�B���h�E�̂Ƃ���������ɂ��邩�H
#define USE_KANJI_MODE (1)

enum{
 MENU_MSG_SPD,
 MENU_SOUND,
 MENU_BTL_EFF,
 MENU_BTL_RULE,
 MENU_BTN_MODE,
#if USE_KANJI_MODE
 MENU_MOJIMODE,
#else
 MENU_WIN,
#endif
 MENU_MAX,
};

typedef enum{
  CONFIG_SEQ_INITFADEIN,      // ��ʂ̏������ƃt�F�[�h�C��
  CONFIG_SEQ_INITFADEINWAIT,    // �t�F�[�h�C���܂�
  CONFIG_SEQ_MAIN,        // ���C��
  CONFIG_SEQ_SETCHECKINIT,    // �ύX�𔽉f�����邩�`�F�b�N�@���b�Z�[�W�\��
  CONFIG_SEQ_SETCHECK,      // �ύX�𔽉f�����邩�`�F�b�N�@YESNO�E�B���h�E�\��
  CONFIG_SEQ_SETWAIT,       // �ύX�𔽉f�����邩�`�F�b�N�@YESNO�Z���N�g
  CONFIG_SEQ_FADEOUT,       // �t�F�[�h�A�E�g
  CONFIG_SEQ_FADEOUTWAIT,     // �t�F�[�h�A�E�g�܂�
  CONFIG_SEQ_RELEASE,       // ��ʂ̊J��
}CONFIG_SEQUENCE;

//-----------------------------------------------------------------------------
//�O�����J���[�N
//-----------------------------------------------------------------------------
typedef struct _CONFIG_PARAM
{
  u16 msg_spd:4;
  u16 sound:2;
  u16 btl_eff:1;
  u16 btl_rule:1;
  u16 input_mode:2;
  u16 win_type:5;
  u16 moji_mode:1;
}CONFIG_PARAM;

typedef struct _CONFIG_MENU{
  u16 num;
  u16 sel;
  STRBUF  *dat[MENU_SEL_MAX];
}CONFIG_MENU;

typedef struct _CONFIG_MAIN_DAT
{
  int heapID;
  CONFIG_SEQUENCE seq;
  int sub_seq;
  int wipe_f;
  u32 end_f:2;
  u32 line:3;
  u32 side:16;
  u32 wincgx_f:1;
  u32 pad:10;

  CONFIG_PARAM  param;
  CONFIG      *save;

  ///���b�Z�[�W���\�[�X
  GFL_MSGDATA*  pMsg;
  
  void* pScrBuf;
  NNSG2dScreenData* pScr01;

  GFL_BMPWIN *  win[WIN_MAX]; ///<BMP�E�B���h�E�f�[�^
  CONFIG_MENU   menu[MENU_LINE_MAX];

  BMPMENU_WORK* pYesNoWork; ///<BMPYESNO�E�B���h�E�f�[�^
  u32 msg_no;

  CATS_SYS_PTR  pCActSys; ///<�Z���A�N�^�[�V�X�e��
  CATS_RES_PTR  pCActRes; ///<�Z���A�N�^�[���\�[�X
  CATS_ACT_PTR  pCAct;    ///<�A�N�g

  GFL_TCB * vintr_tcb;
  GFL_FONT * hSysFont;
  GFL_FONT * hMsgFont;
}CONFIG_MAIN_DAT;


//================================================================
///�v���g�^�C�v�G���A
//================================================================
///VBlank�֘A
static void ConfigVBlank(GFL_TCB * tcb, void * work);
///VRAM�o���N��`
static void ConfigVramBankSet(void);

static int ConfigInitCommon(CONFIG_MAIN_DAT* wk);
static int ConfigReleaseCommon(CONFIG_MAIN_DAT* wk);

static void ConfigBGLInit(CONFIG_MAIN_DAT* wk);
static void ConfigBGLRelease(CONFIG_MAIN_DAT* wk);
static void Config2DGraInit(CONFIG_MAIN_DAT* wk);
static void Config2DGraRelease(CONFIG_MAIN_DAT* wk);
static void ConfigBmpWinInit(CONFIG_MAIN_DAT* wk);
static void ConfigBmpWinRelease(CONFIG_MAIN_DAT* wk);
static void ConfigBmpWinWriteFirst(CONFIG_MAIN_DAT* wk);
static void ConfigBmpWinWriteMenu(CONFIG_MAIN_DAT* wk,u16 idx);
static void ConfigBmpWinWriteSum( CONFIG_MAIN_DAT* wk,u16 line, BOOL allput );
static void ConfigBmpWinWriteMessege( CONFIG_MAIN_DAT* wk, u16 msg_idx, BOOL allput );
static BOOL ConfigBmpWinWriteMessegeEndCheck( const CONFIG_MAIN_DAT* cp_wk );
static void ConfigMenuStrGet(CONFIG_MAIN_DAT* wk);
static void ConfigKeyIn(CONFIG_MAIN_DAT* wk);
static BOOL ConfigDiffParam( CONFIG_MAIN_DAT* p_wk );
static void ConfigYesNoWinInit( CONFIG_MAIN_DAT* p_wk );
static u32 ConfigYesNoWinMain( CONFIG_MAIN_DAT* p_wk );

///�I�[�o�[���C�v���Z�X
static GFL_PROC_RESULT ConfigProc_Init( GFL_PROC *proc,int *seq, void *pwk, void *work);
static GFL_PROC_RESULT ConfigProc_Main( GFL_PROC *proc,int *seq, void *pwk, void *work);
static GFL_PROC_RESULT ConfigProc_End( GFL_PROC *proc,int *seq, void *pwk, void *work);

//================================================================
///�f�[�^��`�G���A
//================================================================
const GFL_PROC_DATA ConfigPanelProcData = {
  ConfigProc_Init,
  ConfigProc_Main,
  ConfigProc_End
};


//============================================================================================
//
//
//        �v���O�����G���A
//
//
//============================================================================================
//----------------------------------------------------------------
/**
 *  @brief  �R���t�B�O �v���Z�X������
 *  @param  proc  �v���Z�X�f�[�^
 *  @param  seq   �V�[�P���X
 *
 *  @return ������
 */
//----------------------------------------------------------------
GFL_PROC_RESULT ConfigProc_Init( GFL_PROC *proc,int *seq, void *pwk, void *work)
{
  CONFIG_MAIN_DAT *wk = NULL;
  CONFIG * sp;
  
  sp = (CONFIG*)pwk;

  //���[�N�G���A�擾
  HeapStatePush();

  //�q�[�v�쐬
  GFL_HEAP_CreateHeap(HEAPID_BASE_APP,HEAPID_CONFIG,0x10000 + 0x8000);
  
  wk = GFL_PROC_AllocWork( proc,sizeof(CONFIG_MAIN_DAT),HEAPID_CONFIG);
  GFL_STD_MemFill(wk,0,sizeof(CONFIG_MAIN_DAT));
  
  
  wk->heapID = HEAPID_CONFIG;
  if (sp == NULL) {
    //�Ƃ肠����
    sp = CONFIG_AllocWork(wk->heapID);
    CONFIG_Init(sp);
  }

  wk->save = sp;
  //�p�����[�^���p��
  wk->param.msg_spd   = CONFIG_GetMsgSpeed(sp);
  wk->param.btl_eff   = CONFIG_GetWazaEffectMode(sp);
  wk->param.btl_rule    = CONFIG_GetBattleRule(sp);
  wk->param.sound     = CONFIG_GetSoundMode(sp);
  wk->param.input_mode  = CONFIG_GetInputMode(sp);
  wk->param.win_type    = CONFIG_GetWindowType(sp);
  wk->param.moji_mode    = CONFIG_GetMojiMode(sp);


  // ���b�Z�[�W�\���֌W��ݒ�
  MsgPrintSkipFlagSet(MSG_SKIP_OFF);  // �X�L�b�v�ł��Ȃ�

  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------
/**
 *  @brief  �R���t�B�O �v���Z�X�I��
 *  @param  proc  �v���Z�X�f�[�^
 *  @param  seq   �V�[�P���X
 *
 *  @return ������
 */
//----------------------------------------------------------------
GFL_PROC_RESULT ConfigProc_End( GFL_PROC *proc,int *seq, void *pwk, void *work)
{
  CONFIG_MAIN_DAT* wk = work;

  //�Z�[�u�p�����[�^�����o��
  if(wk->end_f == CONFIG_DECIDE_END){
    wk->param.msg_spd = wk->menu[MENU_MSG_SPD].sel;
    wk->param.btl_eff = wk->menu[MENU_BTL_EFF].sel;
    wk->param.btl_rule = wk->menu[MENU_BTL_RULE].sel;
    wk->param.sound  = wk->menu[MENU_SOUND].sel;
    wk->param.input_mode   = wk->menu[MENU_BTN_MODE].sel;
#if USE_KANJI_MODE
    wk->param.moji_mode   = wk->menu[MENU_MOJIMODE].sel;
#else
    wk->param.win_type   = wk->menu[MENU_WIN].sel;
#endif
  }

  // ���݂̐ݒ�𔽉f
  // �R���t�B�O���Őݒ肳�ꂽ�l�����ɖ߂�
  CONFIG_SetMsgSpeed(wk->save,wk->param.msg_spd);
  CONFIG_SetWazaEffectMode(wk->save,wk->param.btl_eff);
  CONFIG_SetBattleRule(wk->save,wk->param.btl_rule);
  CONFIG_SetSoundMode(wk->save,wk->param.sound);
  CONFIG_SetInputMode(wk->save,wk->param.input_mode);
  CONFIG_SetWindowType(wk->save,wk->param.win_type);
  CONFIG_SetMojiMode(wk->save,wk->param.moji_mode);
  Snd_SetMonoFlag(wk->param.sound);
  CONFIG_SetKeyConfig(wk->param.input_mode);

  GFL_MSGSYS_SetLangID( wk->param.moji_mode );

  // ���b�Z�[�W�\���֌W��ݒ�
  MsgPrintSkipFlagSet(MSG_SKIP_ON); // �X�L�b�v�ł���

  if (pwk == NULL) {
    GFL_HEAP_FreeMemory(wk->save);
  }
  //���[�N�G���A���
  GFL_PROC_FreeWork(proc);

  HeapStatePop();
  HeapStateCheck(wk->heapID);
  GFL_HEAP_DeleteHeap(wk->heapID);

  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------
/**
 *  @brief  �R���t�B�O �v���Z�X���C��
 *  @param  proc  �v���Z�X�f�[�^
 *  @param  seq   �V�[�P���X
 *
 *  @return ������
 */
//----------------------------------------------------------------
GFL_PROC_RESULT ConfigProc_Main( GFL_PROC *proc,int *seq, void *pwk, void *work)
{
  CONFIG_MAIN_DAT* wk = work;
  BOOL result;
  u32 yesno_res;

  switch((CONFIG_SEQUENCE)wk->seq){
  case CONFIG_SEQ_INITFADEIN:
    if(!ConfigInitCommon(wk)){
      return GFL_PROC_RES_CONTINUE;
    }
    WIPE_SYS_Start(WIPE_PATTERN_M,
        WIPE_TYPE_FADEIN,
        WIPE_TYPE_FADEIN,0x0000,COMM_BRIGHTNESS_SYNC,1,wk->heapID
        );
    break;
  case CONFIG_SEQ_INITFADEINWAIT:
    if(!WIPE_SYS_EndCheck()){
      return GFL_PROC_RES_CONTINUE;
    }
    break;
  case CONFIG_SEQ_MAIN:
#if 0
    //���C��
    if( ((GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE) && (wk->line == (MENU_LINE_MAX-1))) ||
      (GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL) ){

      result = ConfigDiffParam( wk );
      
      // �ύX�����������`�F�b�N
      if( result == TRUE ){
        wk->seq = CONFIG_SEQ_SETCHECKINIT;
      }else{
        Snd_PlaySE(SEQ_SE_DP_SELECT);
        wk->end_f = CONFIG_CANCEL_END;
        wk->seq = CONFIG_SEQ_FADEOUT;
      }
      return GFL_PROC_RES_CONTINUE;
    }
    ConfigKeyIn(wk);
#endif
    //GS�Ō����̂͂��E���������o�Ȃ������̂ł������ɂ��킹��
    if((GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE) && (wk->line == (MENU_LINE_MAX-1)))
    {
      result = ConfigDiffParam( wk );
      
      // �ύX�����������`�F�b�N
      if( result == TRUE )
      {
        Snd_PlaySE(SEQ_SE_DP_SAVE);
        wk->end_f = CONFIG_DECIDE_END;
      }
      else
      {
        Snd_PlaySE(SEQ_SE_DP_SELECT);
        wk->end_f = CONFIG_CANCEL_END;
      }
      wk->seq = CONFIG_SEQ_FADEOUT;
    }
    else
    if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL)
    {
      Snd_PlaySE(SEQ_SE_DP_SELECT);
      wk->end_f = CONFIG_CANCEL_END;
      wk->seq = CONFIG_SEQ_FADEOUT;
    }
    ConfigKeyIn(wk);

    return GFL_PROC_RES_CONTINUE;
    
  case CONFIG_SEQ_SETCHECKINIT:
    // ���f�����邩����
    MsgPrintSkipFlagSet(MSG_SKIP_ON); // �X�L�b�v�ł���
    ConfigBmpWinWriteMessege( wk, mes_config_comment07, FALSE );
    break;

  case CONFIG_SEQ_SETCHECK:
    // YESNO���o��
    if( ConfigBmpWinWriteMessegeEndCheck( wk ) ){
      MsgPrintSkipFlagSet(MSG_SKIP_OFF);  // �X�L�b�v�ł���
      ConfigYesNoWinInit( wk );
      wk->seq = CONFIG_SEQ_SETWAIT;
    }
    return GFL_PROC_RES_CONTINUE;
    
  case CONFIG_SEQ_SETWAIT:
    // �I�����Ă��炤
    yesno_res = ConfigYesNoWinMain( wk );
    if( yesno_res != BMPMENU_NULL )
    {
      if( yesno_res == 0 ) // �Z�[�u����Ȃ�
      {
        Snd_SeStopBySeqNo(SEQ_SE_DP_SELECT,0);    //�����ɖ�̂��������
        Snd_PlaySE(SEQ_SE_DP_SAVE);
        wk->end_f = CONFIG_DECIDE_END;
      }
      else
      {
        wk->end_f = CONFIG_CANCEL_END;
      }
      wk->seq = CONFIG_SEQ_FADEOUT;
    }
    return GFL_PROC_RES_CONTINUE;
  
  case CONFIG_SEQ_FADEOUT:
#if 0
    //add pl MSG�t���[�Y�΍� ���b�Z�[�W�\���^�X�N������~ 080701 kaga
    if( GF_MSG_PrintEndCheck(wk->msg_no) ){
      GF_STR_PrintForceStop( wk->msg_no );
    }
#endif
    
    WIPE_SYS_Start(WIPE_PATTERN_M,
        WIPE_TYPE_FADEOUT,
        WIPE_TYPE_FADEOUT,0x0000,COMM_BRIGHTNESS_SYNC,1,wk->heapID
        );
    break;
  case CONFIG_SEQ_FADEOUTWAIT:
    if(!WIPE_SYS_EndCheck()){
      return GFL_PROC_RES_CONTINUE;
    }
    break;
  case CONFIG_SEQ_RELEASE:
    //�I������
    if(!ConfigReleaseCommon(wk)){
      return GFL_PROC_RES_CONTINUE;
    }
    return GFL_PROC_RES_FINISH;
  }
  ++wk->seq;
  return GFL_PROC_RES_CONTINUE;
}

//----------------------------------------------------------------
/**
 *  @brief  �R���t�B�O���VramBank�Z�b�g
 */
//----------------------------------------------------------------
static void ConfigVramBankSet(void)
{
  GFL_DISP_VRAM vramSetTable = {
    GX_VRAM_BG_128_A,       // ���C��2D�G���W����BG
    GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
    GX_VRAM_SUB_BG_128_C,     // �T�u2D�G���W����BG
    GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
    GX_VRAM_OBJ_16_G,       // ���C��2D�G���W����OBJ
    GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g
    GX_VRAM_SUB_OBJ_16_I,     // �T�u2D�G���W����OBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
    GX_VRAM_TEX_NONE,       // �e�N�X�`���C���[�W�X���b�g
    GX_VRAM_TEXPLTT_NONE,     // �e�N�X�`���p���b�g�X���b�g
    GX_OBJVRAMMODE_CHAR_1D_32K,   // ���C��OBJ�}�b�s���O���[�h
    GX_OBJVRAMMODE_CHAR_1D_32K,   // �T�uOBJ�}�b�s���O���[�h
  };

  GFL_DISP_SetBank( &vramSetTable );
}

//----------------------------------------------------------------
/**
 *  @brief  �R���t�B�O���VBlank
 */
//----------------------------------------------------------------
static void ConfigVBlank(GFL_TCB * tcb, void * work)
{
  CONFIG_MAIN_DAT* wk = work;

#if 0
  //��b�E�B���h�E�L�����N�^�̍ē]��
  if(wk->wincgx_f){
    TalkWinGraphicSet(wk->bgl,BMPL_SUM_FRM,
      BMPL_TALK_WIN_CGX, BMPL_TALK_WIN_PAL, wk->menu[MENU_WIN].sel, wk->heapID);

    wk->wincgx_f = 0;
  }
  CATS_RenderOamTrans();
  NNS_GfdDoVramTransfer();  //VRam�]���}�l�[�W�����s
  GF_BGL_VBlankFunc( wk->bgl );
  OS_SetIrqCheckFlag( OS_IE_V_BLANK);
#endif
}

//----------------------------------------------------------------
/**
 *  @brief  �e�평��������
 */
//----------------------------------------------------------------
static int ConfigInitCommon(CONFIG_MAIN_DAT *wk)
{
  switch(wk->sub_seq){
  case 0:
    //sys_VBlankFuncChange(NULL,NULL);  
    //sys_HBlankIntrStop();
  
    GFL_DISP_GX_InitVisibleControl();
    GFL_DISP_GXS_InitVisibleControl();
    GX_SetVisiblePlane(0);
    GXS_SetVisiblePlane(0);

    //Bank�Z�b�g
    ConfigVramBankSet();

    WIPE_ResetWndMask(WIPE_DISP_MAIN);
    WIPE_ResetWndMask(WIPE_DISP_SUB);

    //BGL�Z�b�g
    ConfigBGLInit(wk);
    break;
  case 1:
    //2D���\�[�X�擾
    Config2DGraInit(wk);

    wk->hSysFont = GFL_FONT_Create(ARCID_FONT, NARC_font_small_nftr,
        GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID);
    wk->hMsgFont = GFL_FONT_Create(ARCID_FONT, NARC_font_large_nftr,
        GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID);
    //���b�Z�[�W���\�[�X�擾
    wk->pMsg = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL,ARCID_MESSAGE,
        NARC_message_config_dat,wk->heapID);
    ConfigMenuStrGet(wk);
    break;
  case 2:
    ConfigBmpWinInit(wk);
    ConfigBmpWinWriteFirst(wk);
    //�ʐM�A�C�R���p��OBJ��ON
    //initVramTransferManagerHeap(32,wk->heapID);
    GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_OBJ,VISIBLE_ON);

    GFL_BG_LoadScreenReq(GFL_BG_FRAME1_M);
    // ��M���x�A�C�R����ʐM�ڑ����Ȃ�\�����邵�A
    // ���j�I���̂悤�ɒʐM��H�͓��삵�Ă��邪�ڑ��͂��Ă��Ȃ���ԂȂ�o���Ȃ�
    // Change by Mori 2006/07/19
    //WirelessIconEasyUnion();

    wk->vintr_tcb = GFUser_VIntr_CreateTCB(ConfigVBlank, wk, 0 );
    //sys_VBlankFuncChange(ConfigVBlank,wk);  
    wk->sub_seq = 0;
    return 1;
    
  }
  ++wk->sub_seq;
  return 0;
}

//----------------------------------------------------------------
/**
 *  @brief  �e��I������
 */
//----------------------------------------------------------------
static int ConfigReleaseCommon(CONFIG_MAIN_DAT *wk)
{
  int i = 0,j = 0;
  
  switch(wk->sub_seq){
  case 0:
    //DellVramTransferManager();
    //Bmp�E�B���h�E�j��
    ConfigBmpWinRelease(wk);
    //���b�Z�[�W���\�[�X���
    for(i = 0;i < MENU_LINE_MAX;i++){
      for(j = 0;j < wk->menu[i].num;j++){
        GFL_STR_DeleteBuffer(wk->menu[i].dat[j]);
      }
    }
    GFL_FONT_Delete(wk->hSysFont);
    GFL_FONT_Delete(wk->hMsgFont);
    GFL_MSG_Delete(wk->pMsg);
    //2D���\�[�X���
    Config2DGraRelease(wk);
    ConfigBGLRelease(wk);
    break;
  case 1:
    GFL_TCB_DeleteTask(wk->vintr_tcb);
    //sys_VBlankFuncChange(NULL,NULL);  
    //sys_HBlankIntrStop();
  
    GFL_DISP_GX_InitVisibleControl();
    GFL_DISP_GXS_InitVisibleControl();
    GX_SetVisiblePlane(0);
    GXS_SetVisiblePlane(0);
    wk->sub_seq = 0;
    return 1;
  }
  ++wk->sub_seq;
  return 0;
}

//----------------------------------------------------------------
/**
 *  @brief  �R���t�B�O���BGL�V�X�e��������
 */
//----------------------------------------------------------------
static void ConfigBGLInit(CONFIG_MAIN_DAT* wk)
{
  int i;

  GFL_BG_Init(wk->heapID);

  { //BG SYSTEM
    GFL_BG_SYS_HEADER BGsys_data = {
      GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_2D
    };
    GFL_BG_SetBGMode(&BGsys_data);
  }
  
  {
  GFL_BG_BGCNT_HEADER TextBgCntDat[] = {
    { //MAIN BG0  �I��g��
      0,0,0x800,0,GFL_BG_SCRSIZ_256x256,GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800,GX_BG_CHARBASE_0x00000,0x4000,GX_BG_EXTPLTT_01,
      1,0,0,FALSE},
    { //MAIN BG1  �e�L�X�g�\���v���[��
      0,0,0x800,0,GFL_BG_SCRSIZ_256x256,GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000,GX_BG_CHARBASE_0x04000,0x8000,GX_BG_EXTPLTT_01,
      2,0,0,FALSE},
    { //MAIN BG2
      0,0,0x800,0,GFL_BG_SCRSIZ_256x256,GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800,GX_BG_CHARBASE_0x00000,0x4000,GX_BG_EXTPLTT_01,
      3,0,0,FALSE},
    { //MAIN BG3  �u�͂��E�������v�\���p�v���[��
      0,0,0x800,0,GFL_BG_SCRSIZ_256x256,GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000,GX_BG_CHARBASE_0x00000,0x4000,GX_BG_EXTPLTT_01,
      0,0,0,FALSE},
    { //SUB BG0
      0,0,0x800,0,GFL_BG_SCRSIZ_256x256,GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800,GX_BG_CHARBASE_0x00000,0x4000,GX_BG_EXTPLTT_01,
      0,0,0,FALSE},
  };
  for(i = 0;i < 5;i++){
    static const frame[5] = {
      GFL_BG_FRAME0_M,GFL_BG_FRAME1_M,GFL_BG_FRAME2_M,GFL_BG_FRAME3_M,GFL_BG_FRAME0_S
    };
    GFL_BG_SetBGControl(frame[i],&(TextBgCntDat[i]),GFL_BG_MODE_TEXT);
    GFL_BG_ClearScreen(frame[i]);
    GFL_BG_SetVisible(frame[i], VISIBLE_ON);
  }
  }
  GFL_BG_SetClearCharacter(GFL_BG_FRAME0_M,0x20,0x0000,wk->heapID);
  GFL_BG_SetClearCharacter(GFL_BG_FRAME0_S,0x20,0x0000,wk->heapID);
  GFL_BG_SetClearCharacter(GFL_BG_FRAME0_M,0x20,0x4000,wk->heapID);
}

//----------------------------------------------------------------
/**
 *  @brief  �R���t�B�O���BGL�V�X�e���j��
 */
//----------------------------------------------------------------
static void ConfigBGLRelease(CONFIG_MAIN_DAT* wk)
{
  GFL_BG_FreeBGControl(GFL_BG_FRAME0_S);
  GFL_BG_FreeBGControl(GFL_BG_FRAME3_M);
  GFL_BG_FreeBGControl(GFL_BG_FRAME2_M);
  GFL_BG_FreeBGControl(GFL_BG_FRAME1_M);
  GFL_BG_FreeBGControl(GFL_BG_FRAME0_M);

  GFL_BG_Exit();
}

//----------------------------------------------------------------
/**
 *  @brief  �R���t�B�O��� 2D���\�[�X�擾
 */
//----------------------------------------------------------------
static void Config2DGraInit(CONFIG_MAIN_DAT* wk)
{
  void* tmp;
  u32 size;
  ARCHANDLE* handle;
  void* pSrc;
  NNSG2dCharacterData* pChar;
  NNSG2dPaletteData*  pPal;
  
  handle = GFL_ARC_OpenDataHandle(ARCID_CONFIG_GRA,wk->heapID);

  //�L�����N�^�]��
  size = GFL_ARC_GetDataSizeByHandle(handle,NARC_config_gra_config01_ncgr);
  pSrc = GFL_HEAP_AllocMemoryLo(wk->heapID,size);
  GFL_ARC_LoadDataByHandle(handle,NARC_config_gra_config01_ncgr,(void*)pSrc);
  
  NNS_G2dGetUnpackedCharacterData(pSrc,&pChar); 
  GFL_BG_LoadCharacter(GFL_BG_FRAME0_M,pChar->pRawData,pChar->szByte,0);
  GFL_BG_LoadCharacter(GFL_BG_FRAME0_S,pChar->pRawData,pChar->szByte,0);
  GFL_HEAP_FreeMemory(pSrc);

  //�p���b�g�]��
  size = GFL_ARC_GetDataSizeByHandle(handle,NARC_config_gra_config01_nclr);
  pSrc = GFL_HEAP_AllocMemoryLo(wk->heapID,size);
  GFL_ARC_LoadDataByHandle(handle,NARC_config_gra_config01_nclr,(void*)pSrc);

  NNS_G2dGetUnpackedPaletteData(pSrc,&pPal);
  GFL_BG_LoadPalette(GFL_BG_FRAME0_M,pPal->pRawData,0x20*1,0);
  GFL_BG_LoadPalette(GFL_BG_FRAME0_S,pPal->pRawData,0x20*1,0);
  GFL_HEAP_FreeMemory(pSrc);

  //�X�N���[���擾
  size = GFL_ARC_GetDataSizeByHandle(handle,NARC_config_gra_config01_nscr);
  wk->pScrBuf = GFL_HEAP_AllocMemory(wk->heapID,size);
  GFL_ARC_LoadDataByHandle(handle,NARC_config_gra_config01_nscr,(void*)wk->pScrBuf);
  NNS_G2dGetUnpackedScreenData(wk->pScrBuf,&(wk->pScr01)); 

  GFL_ARC_CloseDataHandle( handle );
  
  GFL_BG_FillScreen(GFL_BG_FRAME2_M,0x0001,0,0,32,32,GFL_BG_SCRWRT_PALIN );
  GFL_BG_FillScreen(GFL_BG_FRAME0_S,0x0001,0,0,32,32,GFL_BG_SCRWRT_PALIN );
  
  //BG�`��
  GFL_BG_WriteScreenFree(GFL_BG_FRAME0_M,
      0,0,32,2,
      wk->pScr01->rawData,
      0,0,
      wk->pScr01->screenWidth/8,wk->pScr01->screenHeight/8);
  GFL_BG_SetScroll(GFL_BG_FRAME0_M,GFL_BG_SCROLL_Y_SET,-MENU_WIN_OFSY);
  
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_M);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME0_M);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME0_S);
}

//----------------------------------------------------------------
/**
 *  @brief  �R���t�B�O��� 2D���\�[�X���
 */
//----------------------------------------------------------------
static void Config2DGraRelease(CONFIG_MAIN_DAT* wk)
{
  GFL_HEAP_FreeMemory(wk->pScrBuf);
}

//----------------------------------------------------------------
/**
 *  @brief  �R���t�B�O��� BMP�E�B���h�E������
 */
//----------------------------------------------------------------
static void ConfigBmpWinInit(CONFIG_MAIN_DAT* wk)
{
  GFL_BMPWIN_Init(wk->heapID);

  wk->win[WIN_TITLE] = GFL_BMPWIN_Create(BMPL_TITLE_FRM,
    BMPL_TITLE_PX, BMPL_TITLE_PY,
    BMPL_TITLE_SX, BMPL_TITLE_SY, BMPL_TITLE_PAL, GFL_BMP_CHRAREA_GET_F );

  wk->win[WIN_MENU] = GFL_BMPWIN_Create(BMPL_MENU_FRM,
    BMPL_MENU_PX, BMPL_MENU_PY,
    BMPL_MENU_SX, BMPL_MENU_SY, BMPL_MENU_PAL, GFL_BMP_CHRAREA_GET_F );

  wk->win[WIN_SUM] = GFL_BMPWIN_Create(BMPL_SUM_FRM,
    BMPL_SUM_PX, BMPL_SUM_PY,
    BMPL_SUM_SX, BMPL_SUM_SY, BMPL_SUM_PAL, GFL_BMP_CHRAREA_GET_F );

  //�E�B���h�E�O���t�B�b�N�Z�b�g
  BmpWinFrame_CgxSet(BMPL_MENU_FRM,
      BMPL_MENU_WIN_CGX, MENU_TYPE_SYSTEM, wk->heapID);
  BmpWinFrame_CgxSet(BMPL_MENU_FRM,
      BMPL_TALK_WIN_CGX, MENU_TYPE_SYSTEM, wk->heapID);
  
  //�t�H���g�p�p���b�g�Z�b�g
  //�t�H���g�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
    0x20*FONT_SYS_PAL, 0x20, HEAPID_CONFIG);
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, 
    0x20*FONT_SYS_PAL, 0x20, HEAPID_CONFIG);
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
    0x20*FONT_TALK_PAL, 0x20, HEAPID_CONFIG);
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, 
    0x20*FONT_TALK_PAL, 0x20, HEAPID_CONFIG);


  //BMP�E�B���h�E�N���A
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->win[WIN_TITLE]), WINCLR_COL(FBMP_COL_NULL));
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->win[WIN_MENU]), WINCLR_COL(FBMP_COL_WHITE));
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->win[WIN_SUM]), WINCLR_COL(FBMP_COL_WHITE));
  GFL_BMPWIN_ClearScreen(wk->win[WIN_SUM]);
  GFL_BMPWIN_ClearScreen(wk->win[WIN_MENU]);
  GFL_BMPWIN_ClearScreen(wk->win[WIN_TITLE]);
  GFL_BMPWIN_MakeScreen(wk->win[WIN_SUM]);
  GFL_BMPWIN_MakeScreen(wk->win[WIN_MENU]);
  GFL_BMPWIN_MakeScreen(wk->win[WIN_TITLE]);
  
  //�E�B���h�E�`��
  BmpWinFrame_Write( wk->win[WIN_MENU], WINDOW_TRANS_ON, BMPL_MENU_WIN_CGX, BMPL_MENU_WIN_PAL );
  BmpWinFrame_Write( wk->win[WIN_SUM], WINDOW_TRANS_ON, BMPL_TALK_WIN_CGX, BMPL_TALK_WIN_PAL);
}

//----------------------------------------------------------------
/**
 *  @brief  �R���t�B�O��� BMP�E�B���h�E���
 */
//----------------------------------------------------------------
static void ConfigBmpWinRelease(CONFIG_MAIN_DAT* wk)
{
  u16 i;
  
  //�E�B���h�E�N���A
  BmpWinFrame_Clear(wk->win[WIN_MENU], WINDOW_TRANS_ON);
  BmpWinFrame_Clear(wk->win[WIN_SUM], WINDOW_TRANS_ON);

  for(i = 0;i < WIN_MAX;i++){
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->win[i]), 0);
    GFL_BMPWIN_ClearScreen(wk->win[i]);
    GFL_BMPWIN_Delete(wk->win[i]);
  }
  GFL_BMPWIN_Exit();
}

/**
 *  @brief  �R���t�B�O��� BMP�E�B���h�E�����`��
 */
static void ConfigBmpWinWriteFirst(CONFIG_MAIN_DAT* wk)
{
  u32 siz,ofs;
  u16 i;
  STRBUF  *buf;
  STRBUF  *sum;
  static const u8 menu_str[ MENU_LINE_MAX ] = {
    mes_config_menu01,
    mes_config_menu04,
    mes_config_menu02,
    mes_config_menu03,
    mes_config_menu05,
#if USE_KANJI_MODE
    mes_config_menu09,
#else
    mes_config_menu06,
#endif
    mes_config_menu08,
  };
  GFL_BMP_DATA * title_bmp;

  title_bmp = GFL_BMPWIN_GetBmp(wk->win[WIN_TITLE]);
  
  buf = GFL_STR_CreateBuffer(SUM_STRSIZ,wk->heapID);

  //�����Ă���������
  GFL_MSG_GetString(wk->pMsg,mes_config_title,buf);

  ofs = 2;
  GFL_FONTSYS_SetColor(FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL);
  PRINTSYS_Print( title_bmp, ofs, 2, buf, wk->hSysFont);
  
  //���j���[
  ofs = 4;
  for(i = 0;i < MENU_LINE_MAX;i++){
    GFL_STR_ClearBuffer(buf);
    GFL_MSG_GetString(wk->pMsg,menu_str[i],buf);
  
    GFL_FONTSYS_SetDefaultColor();
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->win[WIN_MENU]), ofs, 16*i, buf, wk->hSysFont);
  }
  for(i = 0;i < MENU_LINE_MAX;i++){
    ConfigBmpWinWriteMenu(wk,i);
  }

  //�T���l�C��
  ConfigBmpWinWriteSum( wk, 0, TRUE );
  
  GFL_BMPWIN_TransVramCharacter(wk->win[WIN_TITLE]);
  GFL_BMPWIN_TransVramCharacter(wk->win[WIN_MENU]);

  GFL_STR_DeleteBuffer(buf);
}

//----------------------------------------------------------------
/**
 *  @brief  �R���t�B�O��� �Z���N�g���j���[������擾
 */
//----------------------------------------------------------------
static void ConfigMenuStrGet(CONFIG_MAIN_DAT* wk)
{
  u16 i,j;
  static const selSiz[MENU_LINE_MAX] = {
    MSGSPEED_MAX,
    SOUNDMODE_MAX,
    WAZAEFF_MODE_MAX,
    BATTLERULE_MAX,
    INPUTMODE_MAX,
#if USE_KANJI_MODE
    MOJIMODE_MAX,
#else
    WINTYPE_MAX,
#endif
    0
  };
  static const u8 sc_selMsg_start[MENU_LINE_MAX] = {
    mes_config_sm01_00,
    mes_config_sm04_00,
    mes_config_sm02_00,
    mes_config_sm03_00,
    mes_config_sm05_00,
#if USE_KANJI_MODE
    mes_config_sm07_01,
#else 
    mes_config_sm06_00,
#endif
    0,
  };

  for(i = 0;i < MENU_LINE_MAX;i++){
    wk->menu[i].num = selSiz[i];
    for(j = 0;j < selSiz[i];j++){
      wk->menu[i].dat[j] = GFL_MSG_CreateString(wk->pMsg,sc_selMsg_start[i]+j);
    }
  }
  //���݂̑I���i���o�[�擾
  wk->menu[MENU_MSG_SPD].sel = wk->param.msg_spd;
  wk->menu[MENU_BTL_EFF].sel = wk->param.btl_eff;
  wk->menu[MENU_BTL_RULE].sel = wk->param.btl_rule;
  wk->menu[MENU_SOUND].sel = wk->param.sound;
  wk->menu[MENU_BTN_MODE].sel = wk->param.input_mode;
#if USE_KANJI_MODE
  wk->menu[MENU_MOJIMODE].sel = wk->param.moji_mode;
#else
  wk->menu[MENU_WIN].sel = wk->param.win_type;
#endif
}

//----------------------------------------------------------------
/**
 *  @brief  �R���t�B�O��� �Z���N�g���j���[�`��
 */
//----------------------------------------------------------------
static void ConfigBmpWinWriteMenu(CONFIG_MAIN_DAT* wk,u16 idx)
{
  u16 i;
  s8  x_ofs = 0;
  static const s8 ofs[] = { 0,0,0,0,0,0,0};

  //��U�N���A
  GFL_BMP_Fill(GFL_BMPWIN_GetBmp(wk->win[WIN_MENU]),
    MENU_SEL_OFSX+ofs[idx], MENU_SEL_OFSY+idx*MENU_SEL_ASIZY,MENU_SEL_ASIZX,MENU_SEL_ASIZY,
    WINCLR_COL(FBMP_COL_WHITE));

  //�����Ă���������
#if USE_KANJI_MODE
#else
  if(idx == MENU_WIN){
    GFL_FONTSYS_SetColor(FBMP_COL_RED,FBMP_COL_RED_SDW,FBMP_COL_WHITE);
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->win[WIN_MENU]),
        1*MENU_SEL_DSIZ+MENU_SEL_OFSX, 
        MENU_SEL_ASIZY*idx+MENU_SEL_OFSY,
        wk->menu[idx].dat[wk->menu[idx].sel], wk->hSysFont);
    GFL_BMPWIN_TransVramCharacter(wk->win[WIN_MENU]);

    //�E�B���h�E�L�����N�^�đ��t���OOn
    wk->wincgx_f = 1;
    return;
  }
#endif

  //�T�E���h���[�h�̏ꍇ�A�X�e���I�ƃ��m������؂�ւ���
  if(idx == MENU_SOUND){
    Snd_SetMonoFlag(wk->menu[idx].sel);
  }
  // �{�^�����[�h���f
  else if(idx == MENU_BTN_MODE){
    CONFIG_SetKeyConfig(wk->menu[idx].sel);
  }
  // ���b�Z�[�W�X�s�[�h���f
  else if(idx == MENU_MSG_SPD ){
    CONFIG_SetMsgSpeed( wk->save, wk->menu[idx].sel );

    //�T���l�C�����ĕ`��
    ConfigBmpWinWriteSum( wk, idx, FALSE );
  }
  x_ofs = 0;
  for(i = 0;i < wk->menu[idx].num;i++){
    if(i == wk->menu[idx].sel){
      GFL_FONTSYS_SetColor(FBMP_COL_RED,FBMP_COL_RED_SDW,FBMP_COL_WHITE);
    }else{
      GFL_FONTSYS_SetColor(FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE);
    }

    if(idx == MENU_BTN_MODE){
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->win[WIN_MENU]),
              MENU_SEL_OFSX-0+x_ofs,
              MENU_SEL_ASIZY*idx+MENU_SEL_OFSY,
              wk->menu[idx].dat[i],
              wk->hSysFont);
      
      x_ofs += PRINTSYS_GetStrWidth(wk->menu[idx].dat[i], wk->hSysFont, 0) + 12;
    }else{
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->win[WIN_MENU]),
              i*MENU_SEL_DSIZ+MENU_SEL_OFSX+ofs[idx],
              MENU_SEL_ASIZY*idx+MENU_SEL_OFSY,
              wk->menu[idx].dat[i],
              wk->hSysFont);
    }
  }
  //GF_BGL_BmpWinOn(wk->win[WIN_MENU]);
  GFL_BG_LoadScreenReq(GFL_BG_FRAME1_M);
  GFL_BMPWIN_TransVramCharacter(wk->win[WIN_MENU]);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�̕\��
 *
 *  @param  wk    ���[�N
 *  @param  msg_idx ���b�Z�[�WID
 *  @param  allput  ��C�ɕ\�����邩
 */
//-----------------------------------------------------------------------------
static void ConfigBmpWinWriteMessege( CONFIG_MAIN_DAT* wk, u16 msg_idx, BOOL allput )
{
  STRBUF  *buf;
  u8 msg_speed;

  // �b�r���̂Ƃ��͍폜����
  if( ConfigBmpWinWriteMessegeEndCheck( wk ) == FALSE ){
    // �����I��
    //GF_STR_PrintForceStop( wk->msg_no );
  }

  // ���݂̃��b�Z�[�W�X�s�[�h�ݒ�
  msg_speed = CONFIG_GetMsgPrintSpeed( wk->save );

  // BMP�̃N���[��
  //GF_BGL_BmpWinDataFill( &(wk->win[WIN_SUM]), FBMP_COL_WHITE);
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->win[WIN_SUM]), FBMP_COL_WHITE);

  // �J���[�쐬
  GFL_FONTSYS_SetColor(FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE);

  // STRBUF�쐬
  buf = GFL_STR_CreateBuffer(SUM_STRSIZ,wk->heapID);

  // �ύX�������b�Z�[�W�X�s�[�h�ŏ�������
  GFL_MSG_GetString(wk->pMsg,msg_idx,buf);

  if( allput == FALSE ){
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->win[WIN_SUM]),
        4,0, buf, wk->hMsgFont);
#if 0
    wk->msg_no = GF_STR_PrintColor( wk->win[WIN_SUM],FONT_TALK,buf,
              4,0,
              msg_speed,fcol,NULL );
#endif
  }else{
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->win[WIN_SUM]),
        4,0, buf, wk->hMsgFont);
#if 0
    GF_STR_PrintColor(  wk->win[WIN_SUM],FONT_TALK,buf,
              4,0,
              MSG_NO_PUT,fcol,NULL );
#endif
    //GF_BGL_BmpWinOnVReq( wk->win[WIN_SUM] );
    GFL_BMPWIN_TransVramCharacter( wk->win[WIN_SUM] );
  }

  // STRBUF�j��
  GFL_STR_DeleteBuffer( buf );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�I���҂�
 *
 *  @param  cp_wk ���[�N
 *
 *  @retval TRUE  ���b�Z�[�W�I��
 *  @retval FALSE ���b�Z�[�W�r��
 */
//-----------------------------------------------------------------------------
static BOOL ConfigBmpWinWriteMessegeEndCheck( const CONFIG_MAIN_DAT* cp_wk )
{
  return TRUE;
#if 0
  if( GF_MSG_PrintEndCheck( cp_wk->msg_no ) == FALSE ){
    return TRUE;
  }
  return FALSE;
#endif
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �ݒ��ʃL�[�擾
 */
//-----------------------------------------------------------------------------
static void ConfigKeyIn(CONFIG_MAIN_DAT* wk)
{
  CONFIG_MENU *mp;
  
  mp = &(wk->menu[wk->line]);
  if(wk->line != MENU_MAX){ //���胉�C���̎��͍��E�L�[���͖���
    if(GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT){
      mp->sel = (mp->sel+1)%mp->num;  
      ConfigBmpWinWriteMenu(wk,wk->line);
      Snd_PlaySE(SEQ_SE_DP_SELECT);
    }else if(GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT){
      mp->sel = (mp->sel+mp->num-1)%mp->num;  
      ConfigBmpWinWriteMenu(wk,wk->line);
      Snd_PlaySE(SEQ_SE_DP_SELECT);
    }
  }
  if(GFL_UI_KEY_GetTrg() & PAD_KEY_UP){
    wk->line = (wk->line+MENU_LINE_MAX-1)%MENU_LINE_MAX;
    GFL_BG_SetScrollReq(GFL_BG_FRAME0_M,GFL_BG_SCROLL_Y_SET,
        -(wk->line*MENU_SEL_ASIZY+MENU_WIN_OFSY));

    //�T���l�C��
    ConfigBmpWinWriteSum( wk, wk->line, TRUE );
    Snd_PlaySE(SEQ_SE_DP_SELECT);
  }else if(GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN){
    wk->line = (wk->line+1)%MENU_LINE_MAX;
    GFL_BG_SetScrollReq(GFL_BG_FRAME0_M,GFL_BG_SCROLL_Y_SET,
        -(wk->line*MENU_SEL_ASIZY+MENU_WIN_OFSY));

    //�T���l�C��
    ConfigBmpWinWriteSum( wk, wk->line, TRUE );
    Snd_PlaySE(SEQ_SE_DP_SELECT);
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ݒ�p�����[�^�ɕύX�����邩�`�F�b�N
 *
 *  @param  p_wk  ���[�N
 *
 *  @retval TRUE  �ݒ�p�����[�^�ɕύX����
 *  @retval FALSE �ݒ�p�����[�^�ɕύX�Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL ConfigDiffParam( CONFIG_MAIN_DAT* p_wk )
{
  if( (p_wk->param.msg_spd  !=  p_wk->menu[MENU_MSG_SPD].sel) ||
    (p_wk->param.btl_eff  !=  p_wk->menu[MENU_BTL_EFF].sel) ||
    (p_wk->param.btl_rule !=  p_wk->menu[MENU_BTL_RULE].sel) ||
    (p_wk->param.sound    !=  p_wk->menu[MENU_SOUND].sel) ||
    (p_wk->param.input_mode !=  p_wk->menu[MENU_BTN_MODE].sel) ||
#if USE_KANJI_MODE
    (p_wk->param.moji_mode !=  p_wk->menu[MENU_MOJIMODE].sel) )
#else
    (p_wk->param.win_type !=  p_wk->menu[MENU_WIN].sel) )
#endif
  {
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  YESNO�E�B���h�E�̏�����
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void ConfigYesNoWinInit( CONFIG_MAIN_DAT* p_wk )
{
#if 0
  static const BMPWIN_DAT c_data = {
    BMPL_YESNO_FRM,
    BMPL_YESNO_CX, BMPL_YESNO_CY,
    BMPL_YESNO_CSX, BMPL_YESNO_CSY,
    BMPL_YESNO_PAL, BMPL_YESNO_CGX
  };
  p_wk->pYesNoWork = BmpYesNoSelectInit( p_wk->bgl, &c_data, 
      BMPL_MENU_WIN_CGX, BMPL_MENU_WIN_PAL, p_wk->heapID );
#endif
  static const BMPWIN_YESNO_DAT c_data =
  {
    BMPL_YESNO_FRM , 
    BMPL_YESNO_CX , BMPL_YESNO_CY ,
    BMPL_YESNO_PAL, BMPL_YESNO_CGX ,
  };
  p_wk->pYesNoWork = BmpMenu_YesNoSelectInit( &c_data,
       BMPL_MENU_WIN_CGX, BMPL_MENU_WIN_PAL,0, p_wk->heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  YESNO�E�B���h�E���C��
 *
 *  @param  p_wk  ���[�N
 *
 * @retval  "BMPMENU_NULL �I������Ă��Ȃ�"
 * @retval  "0        �͂���I��"
 * @retval  "BMPMENU_CANCEL ������or�L�����Z��"
 */
//-----------------------------------------------------------------------------
static u32 ConfigYesNoWinMain( CONFIG_MAIN_DAT* p_wk )
{
  return BmpMenu_YesNoSelectMain( p_wk->pYesNoWork );
//  return BmpYesNoSelectMain( p_wk->pYesNoWork, p_wk->heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �T���l�C���`��
 *
 *  @param  wk    ���[�N
 *  @param  line  ���C��
 *  @param  allput  ��C�ɂЂ傤�����邩
 */
//-----------------------------------------------------------------------------
static void ConfigBmpWinWriteSum( CONFIG_MAIN_DAT* wk,u16 line, BOOL allput )
{
  static const u8 line_msg[ MENU_LINE_MAX ] = {
    mes_config_comment01,
    mes_config_comment04,
    mes_config_comment02,
    mes_config_comment03,
    mes_config_comment05,
#if USE_KANJI_MODE
    mes_config_comment11,
#else
    mes_config_comment06,
#endif
    mes_config_comment10,
  };
  
  //�T���l�C��
  ConfigBmpWinWriteMessege( wk, line_msg[line], allput );
}

