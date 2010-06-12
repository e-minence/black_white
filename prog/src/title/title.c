//==============================================================================
/**
 * @file  title.c
 * @brief �^�C�g�����
 * @author  matsuda
 * @date  2008.12.03(��)
 *
 * @NOTE �^�C�g����ʂ̋Ȃ̒����̕ύX��������Ή�����B(TOTAL_WAIT)
 *       4/2���_�ł͂V�X�b�B�^�C�g����ʂ̕\�����Ă��钷���͋Ȃ̒����Ɠ����ł悢�B
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/ica_anime.h"
#include "system/ica_camera.h"
#include "sound/pm_voice.h"
#include "print/printsys.h"
#include "print/gf_font.h"
#include "arc_def.h"
#include "message.naix"
#include "msg/msg_title.h"
#include "font/font.naix"
#include "title/startmenu.h"
#include "test/testmode.h"
#include "test/performance.h"
#include "backup_erase.h"
#include "app/mictest.h"
#include "sound/pm_sndsys.h"
#include "poke_tool/monsno_def.h"

#include "title_res.h"

#include "demo3d.naix"
#include "title/title.h"

#ifdef PM_DEBUG
#include "corporate.h"

//#define DELAY_CHECK
#endif



//==============================================================================
//  �萔��`
//==============================================================================
/// �^�C�g����ʂ̒���
#define TOTAL_WAIT      (60*78)

/// �㉺��ʂ����ւ���t���[��
#define TITLE_FLIP_WAIT   ( 678 )

/// �`���|�P���������t���[��
#define POKE_VOICE_FRAME    ( 5001 )

/// ���[�v�p�[�g�ɓ��������̃t���[��
#define POKE_LOOP_PART_FRAME  ( 5301 )

// ���E���ňႤ��`
#if PM_VERSION==VERSION_BLACK
/// ���F�|���S���f���̃��[�v�I���t���[���̎w��
#define POKE_LOOP_END_FRAME   ( 5619 )
/// ���F�^�C�g����ʂ̖������o���|�P�����̎w��
#define PLAY_MONSNO ( MONSNO_RESIRAMU )
#else
/// ���F�|���S���f���̃��[�v�I���t���[���̎w��
#define POKE_LOOP_END_FRAME   ( 5779 )
/// ���F�^�C�g����ʂ̖������o���|�P�����̎w��
#define PLAY_MONSNO ( MONSNO_ZEKUROMU )
#endif

/// ���̓g���K�}�X�N
#define NEXT_PROC_MASK    ( PAD_BUTTON_START | PAD_BUTTON_A )
#define BACKUP_ERASE_MASK ( PAD_KEY_UP | PAD_BUTTON_SELECT | PAD_BUTTON_B )
#define MIC_TEST_MASK     ( PAD_KEY_DOWN | PAD_BUTTON_X | PAD_BUTTON_Y )
#define LOOP_PART_MASK    ( PAD_BUTTON_START | PAD_BUTTON_A | PAD_BUTTON_B)

///�t�H���g���g�p����p���b�g�ԍ�
#define D_FONT_PALNO  (0xa)
///�A�C�e���A�C�R���̃p���b�g�W�J�ʒu
#define D_MATSU_ICON_PALNO    (0)
///[PUSH START BUTTON]�̓_�ŊԊu
#define PUSH_TIMER_WAIT     (45)

///���[�V�����u���[�W��
#define NORMAL_MOTION_BL_A (12)
#define NORMAL_MOTION_BL_B (4)
#define VOICE_MOTION_BL_A (8)
#define VOICE_MOTION_BL_B (8)

///�^�C�g�����SBG��BG�v���C�I���e�B
enum{
  BGPRI_MSG = 0,        ///<
  BGPRI_TITLE_LOGO = 2, ///<
  BGPRI_TITLE_BACK = 3, ///<
  BGPRI_3D = 1,         ///<
  BGPRI_BKGR = 0,       ///<
};

enum{
  FRAME_BKGR = GFL_BG_FRAME3_M,   ///< ���C����ʔw�i�uDevelped by GameFreak�v
  FRAME_MSG  = GFL_BG_FRAME1_S,   ///< ���b�Z�[�W�t���[��
  FRAME_LOGO = GFL_BG_FRAME3_S,   ///< �^�C�g�����S��BG�t���[��
  FRAME_BACK = GFL_BG_FRAME2_S,   ///< �`�F�b�N�̔w�iBG
};


/// �V�[�P���XID��`
enum{
  SEQ_INIT = 0,   ///<
  SEQ_SETUP,      ///<
  SEQ_FADEIN,     ///<
  SEQ_MAIN,       ///<
  SEQ_VOICE_PLAY, ///<
  SEQ_NEXT,       ///<
  SEQ_FADEOUT,    ///<
  SEQ_END,        ///<
  SEQ_SKIP,       ///<
};

/// �I�����[�h
enum {
  END_SELECT = 0,
  END_TIMEOUT,
  END_BACKUP_ERASE,
  END_MIC_TEST,
#ifdef  PM_DEBUG
  END_DEBUG_CALL,
#endif  // PM_DEBUG
};


enum{
  OBJ_RES_PAL,
  OBJ_RES_CHR,
  OBJ_RES_CEL,
  OBJ_RES_MAX,
};



#ifdef PM_DEBUG
//#define  DEBUG_MOTION_BL_CONTROL
#endif

#ifdef DEBUG_MOTION_BL_CONTROL
static BOOL DEBUG_is_motion = TRUE;
#endif


//==============================================================================
//  �\���̒�`
//==============================================================================
typedef struct{
  GFL_TCBLSYS*  tcbl;
  GFL_TCB       *vintr_tcb;
  s16 mb_param_a;
  s16 mb_param_b;
}SYS_CONTROL;

typedef struct{
  GFL_BMPWIN*     bmpwin;
  GFL_BMP_DATA*   bmp;
  PRINT_UTIL      printUtil;
  BOOL            message_req;    ///<TRUE:���b�Z�[�W���N�G�X�g��������
  GFL_FONT*       fontHandle;
  PRINT_QUE*      printQue;
  PRINT_STREAM*   printStream;
  GFL_MSGDATA*    mm;
  STRBUF*         strbufENG;
  int             scrol_work;
}G2D_CONTROL;

typedef struct{
  GFL_G3D_UTIL*     g3Dutil;
  u16               g3DutilUnitIdx;
  GFL_G3D_LIGHTSET* g3Dlightset;
  ICA_ANIME* icaAnime;
  GFL_G3D_CAMERA* camera;

}G3D_CONTROL;

typedef struct{
  GFL_CLUNIT*               clunit;
  GFL_CLWK*                 clwk;    //�A�C�e���A�C�R���A�N�^�[
  NNSG2dImagePaletteProxy   icon_pal_proxy;
  NNSG2dCellDataBank*       icon_cell_data;
  NNSG2dAnimBankData*       icon_anm_data;
  void*                     icon_cell_res;
  void*                     icon_anm_res;
  u32                       clres[OBJ_RES_MAX];
}OAM_CONTROL;

typedef struct {
  u16           seq;
  HEAPID        heapID;
  u32           mode;     ///<���̃��j���[��ʂֈ����n�����[�h
  
  SYS_CONTROL   CSys;
  G2D_CONTROL   CG2d;
  G3D_CONTROL   CG3d;
  OAM_CONTROL   COam;
  
  u32           totalWait;
  int           voiceIndex;
  int           scene_flag;
}TITLE_WORK;

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
GFL_PROC_RESULT TitleProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
GFL_PROC_RESULT TitleProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
GFL_PROC_RESULT TitleProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

//==============================================================================
//  �O���֐��錾
//==============================================================================
extern const  GFL_PROC_DATA TestMainProcData;
FS_EXTERN_OVERLAY(testmode);

extern const  GFL_PROC_DATA CorpProcData;
FS_EXTERN_OVERLAY(title);

//==============================================================================
//  �f�[�^
//==============================================================================
//--------------------------------------------------------------
//  PROC
//--------------------------------------------------------------
const GFL_PROC_DATA TitleProcData = {
  TitleProcInit,
  TitleProcMain,
  TitleProcEnd,
};

//==============================================================================
//
//  
//
//==============================================================================
static void initIO(void);
static void initVRAM(void);

static void setupSystem(SYS_CONTROL* CSys, HEAPID heapID);
static void mainSystem(SYS_CONTROL* CSys, HEAPID heapID);
static void releaseSystem(SYS_CONTROL* CSys);
static void setSystemMbParam(SYS_CONTROL* CSys, u16 para_a, u16 para_b);
static void addSystemMbParam(SYS_CONTROL* CSys, s16 add);

static void setupG2Dcontrol(G2D_CONTROL* CG2d, HEAPID heapID);
static void mainG2Dcontrol(G2D_CONTROL* CG2d, HEAPID heapID);
static void releaseG2Dcontrol(G2D_CONTROL* CG2d);

static void setupG3Dcontrol(G3D_CONTROL* CG3d, HEAPID heapID);
static void mainG3Dcontrol(G3D_CONTROL* CG3d, HEAPID heapID);
static void releaseG3Dcontrol(G3D_CONTROL* CG3d);
static void setLegendPokeScene( G3D_CONTROL *CG3d, int anime_frame );

static void setupOAMcontrol(OAM_CONTROL* COam, HEAPID heapID);
static void mainOAMcontrol(OAM_CONTROL* COam, HEAPID heapID);
static void releaseOAMcontrol(OAM_CONTROL* COam);

static void MainFunc(TITLE_WORK* tw);
static void VintrFunc(GFL_TCB *tcb, void *work);
//--------------------------------------------------------------
/**
 * @brief   �^�C�g����ʏ�����
 *
 * @param   proc  
 * @param   seq   
 * @param   pwk   
 * @param   mywk  �^�C�g����ʃ��[�N�i�����Ŋm�ہj
 *
 * @retval  
 */
//--------------------------------------------------------------
GFL_PROC_RESULT TitleProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  TITLE_WORK *tw;
  
  //�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_TITLE_DEMO, 0x120000 );
  tw = GFL_PROC_AllocWork( proc, sizeof(TITLE_WORK), HEAPID_TITLE_DEMO );
  GFL_STD_MemClear(tw, sizeof(TITLE_WORK));
  tw->heapID = HEAPID_TITLE_DEMO;

#ifdef PM_DEBUG // �f�o�b�O�p�X�L�b�v����
  if( pwk != NULL ){
    int * corp = pwk;
    if( *corp == CORPORATE_RET_DEBUG ){
      tw->mode = END_DEBUG_CALL;
      tw->seq = SEQ_SKIP;
    }
  }
  DEBUG_PerformanceSetActive(TRUE); //�f�o�b�O�F�p�t�H�[�}���X���[�^�[�L��
#endif

  return GFL_PROC_RES_FINISH;
}



//----------------------------------------------------------------------------------
/**
 * @brief �o�ߎ��Ԃŉ�ʐ؂�ւ���I���J�ڂ��Ǘ�����
 *
 * @param   tw    
 */
//----------------------------------------------------------------------------------
static void _timewait_func( TITLE_WORK *tw )
{
  // �w��t���[���ɗ���Ə㉺��ʂ��؂�ւ��
  if(tw->totalWait==TITLE_FLIP_WAIT){
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 16, 0, 3);
    GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );

    GFL_BG_SetVisible(FRAME_BACK, VISIBLE_OFF);
    GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
    GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_BG3, VISIBLE_OFF );
    tw->scene_flag = 1;
  }
  
  if(tw->scene_flag==2){
    if(ICA_ANIME_GetNowFrame( tw->CG3d.icaAnime )==POKE_LOOP_END_FRAME*FX32_ONE){
      setLegendPokeScene( &tw->CG3d, POKE_LOOP_PART_FRAME );
      OS_Printf("���[�v�J�n�t���[���֖߂���\n");
    }
  }

  // �\�����Ԕ��ʏ���
  tw->totalWait++;
  if(tw->totalWait > TOTAL_WAIT){
    tw->mode = END_TIMEOUT;
    tw->seq = SEQ_NEXT;
  }

}



//----------------------------------------------------------------------------------
/**
 * @brief 
 *
 * @param   tw    
 */
//----------------------------------------------------------------------------------
static void _key_check( TITLE_WORK *tw )
{
  if( GFL_UI_KEY_GetTrg() & LOOP_PART_MASK && tw->scene_flag==1){
    
    GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );
#ifdef DELAY_CHECK
    GFL_BG_SetVisible( FRAME_BACK, VISIBLE_OFF );
#else
    GFL_BG_SetVisible( FRAME_BACK, VISIBLE_ON );
#endif
    GFL_BG_SetVisible( FRAME_BKGR, VISIBLE_ON );
    GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
    setLegendPokeScene( &tw->CG3d, POKE_LOOP_PART_FRAME );
    tw->scene_flag=2;
    return;
  }
  // �Q�[���J�n
  if(  GFL_UI_KEY_GetTrg() & NEXT_PROC_MASK){
    tw->mode = END_SELECT;
    tw->seq  = SEQ_VOICE_PLAY;
    tw->voiceIndex = PMVOICE_Play(  PLAY_MONSNO,  0,  64, FALSE,  
                  0,  // �R�[���X�{�����[����
                  0,  // �Đ����x��
                  FALSE,    // �t�Đ��t���O
                  0   // ���[�U�[�p�����[�^�[ 
                  );
    setLegendPokeScene( &tw->CG3d, POKE_VOICE_FRAME );
    GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );
    GFL_BG_SetVisible(FRAME_BACK, VISIBLE_ON);
    GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 16, 0, 3);
    PMSND_FadeOutBGM( 4 );

    // ���[�V�����u���[�W����ύX
    setSystemMbParam( &tw->CSys, VOICE_MOTION_BL_A, VOICE_MOTION_BL_B );
    return;
  }
  // �Z�[�u�f�[�^������
  if( GFL_UI_KEY_GetCont() == BACKUP_ERASE_MASK ){
    tw->mode = END_BACKUP_ERASE;
    tw->seq = SEQ_NEXT;
    return;
  }
  // �}�C�N�e�X�g
  if( GFL_UI_KEY_GetCont() == MIC_TEST_MASK ){
    tw->mode = END_MIC_TEST;
    tw->seq = SEQ_NEXT;
    return;
  }
#ifdef  PM_DEBUG
  // �f�o�b�O���j���[��
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
    tw->mode = END_DEBUG_CALL;
    tw->seq = SEQ_NEXT;
    return;
  }
#endif  // PM_DEBUG

}

#ifdef PM_DEBUG

static u32 oldVCount;
static u32 delayVCount;
static u32 delayVCountTotal;

static const u16 pal_tbl[2]={
#if PM_VERSION==VERSION_BLACK
  0x0000, 0xffff
#else
  0x0000, 0xffff
#endif
};

static void _vcount_func(void)
{
  { //�������������o
    u32 nowVCount = OS_GetVBlankCount();
    u32 subVCount = nowVCount - oldVCount;
  
    delayVCount = 0;
    if( oldVCount > 0 && subVCount > 1 )
    {
      // ������������
      delayVCount = (subVCount-1);
      GXS_LoadBGPltt( &pal_tbl[0], 0, 2);
    }else{
      // ���ĂȂ�
      GXS_LoadBGPltt( &pal_tbl[1], 0, 2);
    }
    oldVCount = nowVCount;
    delayVCountTotal += delayVCount;
  }

}
#endif

//=============================================================================================
/**
 * @brief �^�C�g����ʃ��C��
 *
 * @param   proc    
 * @param   seq   
 * @param   pwk   
 * @param   mywk  �^�C�g����ʃ��[�N
 *
 * @retval  GFL_PROC_RESULT   
 */
//=============================================================================================
GFL_PROC_RESULT TitleProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  TITLE_WORK* tw = mywk;

  switch(tw->seq){
  case SEQ_INIT:
    // ������
    initIO();
    initVRAM();

    tw->seq = SEQ_SETUP;
    break;

  case SEQ_SETUP:
    // �Z�b�g�A�b�v
    setupSystem(&tw->CSys, tw->heapID);
    setupG2Dcontrol(&tw->CG2d, tw->heapID);
    setupG3Dcontrol(&tw->CG3d, tw->heapID);
    setupOAMcontrol(&tw->COam, tw->heapID);

    // FADEIN�ݒ�
    GFL_FADE_SetMasterBrightReq
      (GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 2);
    tw->totalWait = 0;
    tw->seq = SEQ_FADEIN;
    PMSND_PlayBGM_WideChannel( SEQ_BGM_POKEMON_THEME );

    break;

  case SEQ_FADEIN:
    if(GFL_FADE_CheckFade() == FALSE){ tw->seq = SEQ_MAIN; }
    MainFunc(tw);
    break;

  case SEQ_MAIN:
    MainFunc(tw);
    // �L�[�`�F�b�N
    _key_check(tw);
    // �o�ߎ��ԏ���
    _timewait_func( tw );
#ifdef PM_DEBUG
#ifdef DELAY_CHECK
    _vcount_func();
#endif
#endif
    break;
  case SEQ_VOICE_PLAY:
    if(PMVOICE_CheckPlay(tw->voiceIndex)==FALSE){
      tw->seq  = SEQ_NEXT;
    }
    MainFunc(tw);
    break;
  case SEQ_NEXT:
    // FADEOUT�ݒ�
    GFL_FADE_SetMasterBrightReq
      (GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 0, 16, 2);
    tw->seq = SEQ_FADEOUT;
    MainFunc(tw);
    PMSND_FadeOutBGM( 16 );
    break;

  case SEQ_FADEOUT:
    if( GFL_FADE_CheckFade() == FALSE &&
        PMSND_CheckFadeOnBGM() == FALSE ){
      PMSND_StopBGM();
      tw->seq = SEQ_END;
    }
    MainFunc(tw);
    break;

  case SEQ_END:
    // ���
    releaseOAMcontrol(&tw->COam);
    releaseG3Dcontrol(&tw->CG3d);
    releaseG2Dcontrol(&tw->CG2d);
    releaseSystem(&tw->CSys);

    return GFL_PROC_RES_FINISH;

  case SEQ_SKIP:
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}

#ifdef DEBUG_ONLY_FOR_hudson
#include "title/game_start.h"
#include "debug/debug_hudson.h"
static void HudsonInit( void )
{ 
  GameStart_DebugNet();
}
#endif // DEBUG_ONLY_FOR_hudson

//=============================================================================================
/**
 * @brief �^�C�g����ʏI��
 *
 * @param   proc    
 * @param   seq   
 * @param   pwk   
 * @param   mywk  �^�C�g����ʃ��[�N
 *
 * @retval  GFL_PROC_RESULT   
 */
//=============================================================================================
GFL_PROC_RESULT TitleProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  TITLE_WORK * tw = mywk;
  u32 mode;

  mode = tw->mode;

  GFL_PROC_FreeWork(proc);
  GFL_HEAP_DeleteHeap(HEAPID_TITLE_DEMO);

  if( mode == END_SELECT ){
    GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &StartMenuProcData, NULL);
  }else if( mode == END_BACKUP_ERASE ){
    GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &BACKUP_ERASE_ProcData, NULL);
  }else if( mode == END_MIC_TEST ){
    GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(mictest), &TitleMicTestProcData, NULL);
#ifdef PM_DEBUG // �f�o�b�O�p�X�L�b�v����
  }else if( mode == END_DEBUG_CALL ){
#ifdef DEBUG_ONLY_FOR_hudson
    if( HUDSON_IsSkipTitle() )
    {
      // HUDSON�Ŏ��s�����ꍇ�͒��Ńt�B�[���h��
      HudsonInit();
    }
    else
#endif // DEBUG_ONLY_FOR_hudson
    {
      GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(testmode), &TestMainProcData, NULL );
    }

#endif  // PM_DEBUG
  }

  return GFL_PROC_RES_FINISH;
}

//==============================================================================
//  
//  Init
//
//==============================================================================
//----------------------------------------------------------------------------------
/**
 * @brief ��ʏ�����
 *
 * @param   none    
 */
//----------------------------------------------------------------------------------
static void initIO(void)
{
  // �e����ʃ��W�X�^������
  GX_SetMasterBrightness(-16);
  GXS_SetMasterBrightness(-16);
  GFL_DISP_GX_SetVisibleControlDirect(0);   //�SBG&OBJ�̕\��OFF
  GFL_DISP_GXS_SetVisibleControlDirect(0);
  G2_BlendNone();
  G2S_BlendNone();
  GX_SetVisibleWnd(GX_WNDMASK_NONE);
  GXS_SetVisibleWnd(GX_WNDMASK_NONE);
}

//==============================================================================
//  VRAM
//==============================================================================
static const GFL_DISP_VRAM vramBank = {
  GX_VRAM_BG_32_FG,               // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,         // ���C��2D�G���W����BG�g���p���b�g
  GX_VRAM_SUB_BG_128_C,           // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,     // �T�u2D�G���W����BG�g���p���b�g
  GX_VRAM_OBJ_NONE,               // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,        // ���C��2D�G���W����OBJ�g���p���b�g
  GX_VRAM_SUB_OBJ_16_I,           // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,    // �T�u2D�G���W����OBJ�g���p���b�g
  GX_VRAM_TEX_01_AB,              // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_0123_E,          // �e�N�X�`���p���b�g�X���b�g
  GX_OBJVRAMMODE_CHAR_1D_64K,     // ���C��OBJ�}�b�s���O���[�h
  GX_OBJVRAMMODE_CHAR_1D_32K,     // �T�uOBJ�}�b�s���O���[�h
};

//----------------------------------------------------------------------------------
/**
 * @brief VRAM �ݒ�
 *
 * @param   none    
 */
//----------------------------------------------------------------------------------
static void initVRAM(void)
{
  static const GFL_BG_SYS_HEADER sysHeader = {
    GX_DISPMODE_VRAM_D, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
  };
  //VRAM�ݒ�
  GFL_DISP_SetBank( &vramBank );
  GFL_BG_SetBGMode( &sysHeader );
}

//==============================================================================
//  
//  Function
//
//==============================================================================
//----------------------------------------------------------------------------------
/**
 * @brief �풓����
 *
 * @param   tw    
 */
//----------------------------------------------------------------------------------
static void MainFunc(TITLE_WORK* tw)
{
  mainSystem(&tw->CSys, tw->heapID);
  mainG2Dcontrol(&tw->CG2d, tw->heapID);
  mainG3Dcontrol(&tw->CG3d, tw->heapID);
  mainOAMcontrol(&tw->COam, tw->heapID);
}


//----------------------------------------------------------------------------------
/**
 * @brief VBLANK����
 *
 * @param   tcb   
 * @param   work    
 */
//----------------------------------------------------------------------------------
static void VintrFunc(GFL_TCB *tcb, void *work)
{
  SYS_CONTROL* CSys = work;
  
  GFL_CLACT_SYS_VBlankFunc();


  GX_SetCapture(GX_CAPTURE_SIZE_256x192,  // Capture size
                      GX_CAPTURE_MODE_AB,        // Capture mode
                      GX_CAPTURE_SRCA_2D3D,            // Blend src A
                      GX_CAPTURE_SRCB_VRAM_0x00000,     // Blend src B
                      GX_CAPTURE_DEST_VRAM_D_0x00000,   // Output VRAM
                      CSys->mb_param_a,             // Blend parameter for src A
                      CSys->mb_param_b);            // Blend parameter for src B


#ifdef DEBUG_MOTION_BL_CONTROL
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP ){
    addSystemMbParam( CSys, 1 );
  }else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN ){
    addSystemMbParam( CSys, -1 );
  }
  
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L )
  {
    if( DEBUG_is_motion ){
      // OFF
      static const GFL_BG_SYS_HEADER sysHeader = {
        GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
      };
      GFL_BG_SetBGMode( &sysHeader );
      DEBUG_is_motion = FALSE;
    }else{

      // ON
      static const GFL_BG_SYS_HEADER sysHeader = {
        GX_DISPMODE_VRAM_D, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
      };
      GFL_BG_SetBGMode( &sysHeader );
      DEBUG_is_motion = TRUE;
    }
  }
#endif // DEBUG_MOTION_BL_CONTROL
}

//==============================================================================
//  
//  Setup & Release
//
//==============================================================================
//==============================================================================
//  System
//==============================================================================
#define DTCM_SIZE   (0x1000)

//------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 * @brief   ��{�V�X�e��������
 *
 * @param   CSys    SYS_CONTROL
 * @param   heapID  
 */
//----------------------------------------------------------------------------------
static void setupSystem(SYS_CONTROL* CSys, HEAPID heapID)
{
  GFL_BG_Init( heapID );
  GFL_BMPWIN_Init( heapID );
  GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K,
                GFL_G3D_VMANLNK, GFL_G3D_PLT32K,
                DTCM_SIZE, heapID, NULL );
  GFL_FONTSYS_Init();   // �������̂݃����[�X�Ȃ�

  CSys->tcbl = GFL_TCBL_Init( heapID, heapID, 4, 32 );
  CSys->vintr_tcb = GFUser_VIntr_CreateTCB(VintrFunc, CSys, 5);

  // �㉺��ʐݒ�
  GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );
  GFL_DISP_SetDispOn();

  // ���[�V�����u���[�W��
  CSys->mb_param_a = NORMAL_MOTION_BL_A;
  CSys->mb_param_b = NORMAL_MOTION_BL_B;
}

//------------------------------------------------------------------------------
static void mainSystem(SYS_CONTROL* CSys, HEAPID heapID)
{
  GFL_TCBL_Main( CSys->tcbl );
}

//------------------------------------------------------------------------------
static void releaseSystem(SYS_CONTROL* CSys)
{
  // �㉺��ʐݒ�
  GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
  GFL_DISP_SetDispOn();

  GFL_TCB_DeleteTask(CSys->vintr_tcb);
  GFL_TCBL_Exit(CSys->tcbl);

  GFL_G3D_Exit();
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�V�����u���[�W���̐ݒ�
 *
 *  @param  CSys      ���[�N
 *  @param  para_a    ���t���[���L���v�`���ʂ̌W��
 *  @param  para_b    �o�b�t�@�ʂ̌W��
 */
//-----------------------------------------------------------------------------
static void setSystemMbParam(SYS_CONTROL* CSys, u16 para_a, u16 para_b)
{
  CSys->mb_param_a = para_a;
  CSys->mb_param_b = para_b;
}

static void addSystemMbParam(SYS_CONTROL* CSys, s16 add)
{
  if( ((CSys->mb_param_a + add) >= 0) && ((CSys->mb_param_a + add) <= 16) ){
    CSys->mb_param_a += add;
    CSys->mb_param_b -= add;

#ifdef DEBUG_MOTION_BL_CONTROL
    OS_TPrintf( "parama a %d  parama b %d\n", CSys->mb_param_a, CSys->mb_param_b );
#endif
  }
}

//==============================================================================
//  G2DControl
//==============================================================================

//--------------------------------------------------------------
static void setupG2Dcontrol(G2D_CONTROL* CG2d, HEAPID heapID)
{
  //BG�t���[���ݒ�
  {
    static const GFL_BG_BGCNT_HEADER main_bkgr_frame = {  //���C�����BG�t���[��
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x04000, 0x04000,
      GX_BG_EXTPLTT_01, BGPRI_BKGR, 0, 0, FALSE
    };
    static const GFL_BG_BGCNT_HEADER sub_msg_frame = {  //�T�u���BG�t���[��
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000, 0x04000,
      GX_BG_EXTPLTT_01, BGPRI_MSG, 0, 0, FALSE
    };
    static const GFL_BG_BGCNT_HEADER sub_logo_frame = { //�T�u���BG�t���[��
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
      GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, 0x8000,
      GX_BG_EXTPLTT_01, BGPRI_TITLE_LOGO, 0, 0, FALSE
    };
    static const GFL_BG_BGCNT_HEADER sub_back_frame = { //�T�u���BG�t���[��
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
      GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x04000, 0x8000,
      GX_BG_EXTPLTT_01, BGPRI_TITLE_BACK, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( FRAME_BKGR, &main_bkgr_frame, GFL_BG_MODE_TEXT );
    GFL_BG_SetBGControl( FRAME_MSG,  &sub_msg_frame,   GFL_BG_MODE_TEXT );
    GFL_BG_SetBGControl( FRAME_LOGO, &sub_logo_frame,  GFL_BG_MODE_TEXT );
    GFL_BG_SetBGControl( FRAME_BACK, &sub_back_frame,  GFL_BG_MODE_TEXT );
  }
  //���C����ʔw�i�ݒ�
  {
    //GFL_BG_SetBackGroundColor(GFL_BG_FRAME0_M, GX_RGB(25, 31, 31));
    GFL_ARC_UTIL_TransVramBgCharacter(
      TITLE_RES_ARCID, TITLE_RES_DEV_NCGR, FRAME_BKGR, 0, 0, TRUE, heapID);
    GFL_ARC_UTIL_TransVramScreen(
      TITLE_RES_ARCID, TITLE_RES_DEV_NSCR, FRAME_BKGR, 0, 0, TRUE, heapID);
    GFL_ARC_UTIL_TransVramPalette(
      TITLE_RES_ARCID, TITLE_RES_DEV_NCLR, PALTYPE_MAIN_BG, 0, 0, heapID);

    GFL_BG_LoadScreenReq( FRAME_BKGR );
    GFL_BG_SetVisible( FRAME_BKGR, VISIBLE_ON );
  }
  //�O���t�B�b�N�f�[�^���[�h(LOGO)
  {
  GFL_ARC_UTIL_TransVramBgCharacter(
    TITLE_RES_ARCID, TITLE_RES_LOGO_NCGR, FRAME_LOGO, 0, 0x8000, 1, heapID);
  GFL_ARC_UTIL_TransVramScreen(
    TITLE_RES_ARCID, TITLE_RES_LOGO_NSCR, FRAME_LOGO, 0, 0, 1, heapID);
  GFL_ARC_UTIL_TransVramPalette(
    TITLE_RES_ARCID, TITLE_RES_LOGO_NCLR, PALTYPE_SUB_BG, 0, 0, heapID);
  
    GFL_BG_LoadScreenReq( FRAME_LOGO );
    GFL_BG_SetVisible(FRAME_LOGO, VISIBLE_ON);
  }

  // �O���t�B�b�N�f�[�^���[�h�iBACK)
  {
    GFL_ARC_UTIL_TransVramScreen(
      TITLE_RES_ARCID, TITLE_RES_BACK_NSCR, FRAME_BACK, 0, 0, 1, heapID);
#ifdef DELAY_CHECK
    GFL_BG_SetVisible( FRAME_BACK, VISIBLE_OFF );
#else
    GFL_BG_SetVisible( FRAME_BACK, VISIBLE_ON );
#endif
  }

  //���b�Z�[�W�֘A�쐬(MSG)
  {
    GFL_BG_FillCharacter( FRAME_MSG, 0x00, 1, 0 );
    GFL_BG_FillScreen( FRAME_MSG, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

    //BMPWIN�쐬
    CG2d->bmpwin = GFL_BMPWIN_Create
                    (FRAME_MSG, 0, 19, 32, 2, D_FONT_PALNO, GFL_BMP_CHRAREA_GET_F);
    CG2d->bmp = GFL_BMPWIN_GetBmp( CG2d->bmpwin );
    GFL_BMP_Clear( CG2d->bmp, 0x00 );
    GFL_BMPWIN_MakeScreen( CG2d->bmpwin );
    GFL_BMPWIN_TransVramCharacter( CG2d->bmpwin );
    PRINT_UTIL_Setup( &CG2d->printUtil, CG2d->bmpwin );
    GFL_BG_LoadScreenReq( FRAME_MSG );

    //�t�H���g�p���b�g�]��(�Ύ�)
//    GFL_STD_MemFill16((void*)(HW_DB_BG_PLTT + D_FONT_PALNO*0x20+2), GX_RGB(0, 20, 0), 0x20-2);
//    GFL_STD_MemFill16((void*)(HW_DB_BG_PLTT + D_FONT_PALNO*0x20+6), GX_RGB(0, 31, 0), 2);

    //�t�H���g&�v�����g�L���[�V�X�e���쐬
    CG2d->fontHandle = GFL_FONT_Create
      ( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
    CG2d->printQue = PRINTSYS_QUE_Create( heapID );

    //���b�Z�[�W�擾
    CG2d->mm = GFL_MSG_Create
              ( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_title_dat, heapID );
    CG2d->strbufENG = GFL_STR_CreateBuffer( 64, heapID );
    GFL_MSG_GetString(CG2d->mm, TITLE_STR_001, CG2d->strbufENG);
    {
      int length = PRINTSYS_GetStrWidth( CG2d->strbufENG, CG2d->fontHandle, 0 );
      //���b�Z�[�W�\��
      PRINTSYS_PrintQueColor(CG2d->printQue, CG2d->bmp, 128-(length/2), 0, CG2d->strbufENG, 
        CG2d->fontHandle, PRINTSYS_MACRO_LSB(1,2,0));
    }
    CG2d->message_req = TRUE;
  }
}

static void mainG2Dcontrol(G2D_CONTROL* CG2d, HEAPID heapID)
{
  GFL_BG_SetVisible(FRAME_MSG, FALSE);
  GFL_BG_SetScrollReq( FRAME_BACK, GFL_BG_SCROLL_X_SET, CG2d->scrol_work/2 );

  // BLACK�͉E�ɃX�N���[���EWHITE�͍��ɃX�N���[��
  if(GetVersion()==VERSION_BLACK){
    CG2d->scrol_work--;
  }else{
    CG2d->scrol_work++;
  }

#if 0
  // "PUSH START" �_�ŏ���
  CG2d->push_timer++;
  if(CG2d->push_timer > PUSH_TIMER_WAIT){
    CG2d->push_timer = 0;
    CG2d->push_visible ^= 1;
    GFL_BG_SetVisible(FRAME_MSG, CG2d->push_visible);
  }
  // ���b�Z�[�W�L���[����
  {
    BOOL que_ret = PRINTSYS_QUE_Main( CG2d->printQue );
  
    if( PRINT_UTIL_Trans( &CG2d->printUtil, CG2d->printQue ) != FALSE){
      if(que_ret == TRUE && CG2d->message_req == TRUE){
        GFL_BMPWIN_TransVramCharacter( CG2d->bmpwin );
        CG2d->message_req = FALSE;
      }
    }
  }
#endif
}

static void releaseG2Dcontrol(G2D_CONTROL* CG2d)
{
  GFL_STR_DeleteBuffer(CG2d->strbufENG);
  GFL_MSG_Delete(CG2d->mm);
  PRINTSYS_QUE_Delete(CG2d->printQue);
  GFL_FONT_Delete(CG2d->fontHandle);
  GFL_BMPWIN_Delete(CG2d->bmpwin);
}

//==============================================================================
//  G3DControl
//==============================================================================
//���C�g�����ݒ�f�[�^
static const GFL_G3D_LIGHT_DATA light0Tbl[] = {
  { 0, {{ (FX16_ONE-1), -(FX16_ONE-1)/2, -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
  { 1, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
  { 2, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
  { 3, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
};
static const GFL_G3D_LIGHTSET_SETUP light0Setup = { light0Tbl, NELEMS(light0Tbl) };

static const GFL_G3D_OBJSTATUS drawStatus = {
  { 0, 0, 0 },                                        //���W
  { FX32_ONE, FX32_ONE, FX32_ONE },                   //�X�P�[��
  { FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, //��]
};


enum{
  G3DRES_01_BMD = 0,
  G3DRES_01_BCA,
  G3DRES_02_BMD,
  G3DRES_02_BCA,
  G3DRES_03_BMD,
  G3DRES_03_BCA,
};

//�ǂݍ���3D���\�[�X
#if PM_VERSION == VERSION_BLACK
// ���F���f�����O�f�[�^
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
  { ARCID_DEMO3D_GRA, NARC_data_demo3d_title_b_01_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_DEMO3D_GRA, NARC_data_demo3d_title_b_01_nsbca, GFL_G3D_UTIL_RESARC },
  { ARCID_DEMO3D_GRA, NARC_data_demo3d_title_b_02_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_DEMO3D_GRA, NARC_data_demo3d_title_b_02_nsbca, GFL_G3D_UTIL_RESARC },
  { ARCID_DEMO3D_GRA, NARC_data_demo3d_title_b_03_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_DEMO3D_GRA, NARC_data_demo3d_title_b_03_nsbca, GFL_G3D_UTIL_RESARC },
};
// ���F�J�����f�[�^
#define CAMERA_DATA_BIN_NAME  NARC_data_demo3d_title_b_camera_bin

#else

// ���F���f�����O�f�[�^
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
  { ARCID_DEMO3D_GRA, NARC_data_demo3d_title_w_01_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_DEMO3D_GRA, NARC_data_demo3d_title_w_01_nsbca, GFL_G3D_UTIL_RESARC },
  { ARCID_DEMO3D_GRA, NARC_data_demo3d_title_w_02_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_DEMO3D_GRA, NARC_data_demo3d_title_w_02_nsbca, GFL_G3D_UTIL_RESARC },
  { ARCID_DEMO3D_GRA, NARC_data_demo3d_title_w_03_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_DEMO3D_GRA, NARC_data_demo3d_title_w_03_nsbca, GFL_G3D_UTIL_RESARC },
};

// ���F�J�����f�[�^
#define CAMERA_DATA_BIN_NAME  NARC_data_demo3d_title_w_camera_bin

#endif

//3D�A�j��
static const GFL_G3D_UTIL_ANM g3Dutil_anm1Tbl[] = {
  { G3DRES_01_BCA, 0  },
};
static const GFL_G3D_UTIL_ANM g3Dutil_anm2Tbl[] = {
  { G3DRES_02_BCA, 0  },
};
static const GFL_G3D_UTIL_ANM g3Dutil_anm3Tbl[] = {
  { G3DRES_03_BCA, 0  },
};

//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
  {
    G3DRES_01_BMD,          //���f�����\�[�XID
    0,                      //���f���f�[�^ID(���\�[�X����INDEX)
    G3DRES_01_BMD,          //�e�N�X�`�����\�[�XID
    g3Dutil_anm1Tbl,        //�A�j���e�[�u��(�����w��̂���)
    NELEMS(g3Dutil_anm1Tbl),//�A�j�����\�[�X��
  },
  {
    G3DRES_02_BMD,        //���f�����\�[�XID
    0,                    //���f���f�[�^ID(���\�[�X����INDEX)
    G3DRES_02_BMD,        //�e�N�X�`�����\�[�XID
    g3Dutil_anm2Tbl,      //�A�j���e�[�u��(�����w��̂���)
    NELEMS(g3Dutil_anm2Tbl),  //�A�j�����\�[�X��
  },
  {
    G3DRES_03_BMD,        //���f�����\�[�XID
    0,                    //���f���f�[�^ID(���\�[�X����INDEX)
    G3DRES_03_BMD,        //�e�N�X�`�����\�[�XID
    g3Dutil_anm3Tbl,      //�A�j���e�[�u��(�����w��̂���)
    NELEMS(g3Dutil_anm3Tbl),  //�A�j�����\�[�X��
  },
};

static const GFL_G3D_UTIL_SETUP g3Dutil_setup = {
  g3Dutil_resTbl,           //���\�[�X�e�[�u��
  NELEMS(g3Dutil_resTbl),   //���\�[�X��
  g3Dutil_objTbl,           //�I�u�W�F�N�g�ݒ�e�[�u��
  NELEMS(g3Dutil_objTbl),   //�I�u�W�F�N�g��
};

// ���\�[�X�e�[�u����
#define G3DUTIL_RESCOUNT  (NELEMS(g3Dutil_resTbl))
// �A�j��OBJ��
#define G3DUTIL_OBJCOUNT  (NELEMS(g3Dutil_objTbl))


static const VecFx32 sc_CAMERA_PER_POS    = { 0,0,FX32_CONST( 0 ) };  //�ʒu
static const VecFx32 sc_CAMERA_PER_UP     = { 0,FX32_ONE,0 };         //�����
static const VecFx32 sc_CAMERA_PER_TARGET = { 0,0,FX32_CONST( 0 ) };  //�^�[�Q�b�g

//--------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 * @brief 3D������
 *
 * @param   CG3d    
 * @param   heapID    
 */
//----------------------------------------------------------------------------------
static void setupG3Dcontrol(G3D_CONTROL* CG3d, HEAPID heapID)
{
  u16 objIdx;
  
  CG3d->g3Dutil = GFL_G3D_UTIL_Create( G3DUTIL_RESCOUNT, G3DUTIL_OBJCOUNT, heapID );
  CG3d->g3DutilUnitIdx = GFL_G3D_UTIL_AddUnit( CG3d->g3Dutil, &g3Dutil_setup );

  //�A�j���[�V������L���ɂ���
  objIdx = GFL_G3D_UTIL_GetUnitObjIdx( CG3d->g3Dutil, CG3d->g3DutilUnitIdx );
  GFL_G3D_OBJECT_EnableAnime( GFL_G3D_UTIL_GetObjHandle(CG3d->g3Dutil, objIdx + 0), 0); 
  GFL_G3D_OBJECT_EnableAnime( GFL_G3D_UTIL_GetObjHandle(CG3d->g3Dutil, objIdx + 1), 0); 
  GFL_G3D_OBJECT_EnableAnime( GFL_G3D_UTIL_GetObjHandle(CG3d->g3Dutil, objIdx + 2), 0); 


  // �J�����쐬
  {
    fx32 near = FX32_CONST(0.100);
    fx32 far  = FX32_ONE * 2048;
    fx32 fovySin, fovyCos;
    fx32 scale_w = FX32_CONST(0.1 );
    
    fovySin = FX_SinIdx( (FX32_CONST(60.0)>>FX32_SHIFT) / 2 * PERSPWAY_COEFFICIENT );
    fovyCos = FX_CosIdx( (FX32_CONST(60.0)>>FX32_SHIFT) / 2 * PERSPWAY_COEFFICIENT );

    CG3d->camera = GFL_G3D_CAMERA_CreateFrustumByPersPrmW( 
                  &sc_CAMERA_PER_POS, &sc_CAMERA_PER_UP, &sc_CAMERA_PER_TARGET,
                  fovySin, fovyCos, defaultCameraAspect, near, far, scale_w, heapID );
  }
  // �J����ica�f�[�^�����[�h
  CG3d->icaAnime = ICA_ANIME_CreateStreamingAlloc(
      heapID, ARCID_DEMO3D_GRA, CAMERA_DATA_BIN_NAME, 10 );


  //���C�g�Z�b�g
  CG3d->g3Dlightset = GFL_G3D_LIGHT_Create( &light0Setup, heapID );
  GFL_G3D_LIGHT_Switching(CG3d->g3Dlightset);

  GFL_BG_SetBGControl3D(BGPRI_3D);

  G3X_AntiAlias( TRUE );      //  �A���`�G�C���A�X
  G3X_AlphaTest( FALSE, 16 ); //  �A���t�@�e�X�g�@�@�I�t
  G3X_AlphaBlend( TRUE );     //  �A���t�@�u�����h�@�I��


}

//----------------------------------------------------------------------------------
/**
 * @brief 3D�`�惁�C��
 *
 * @param   CG3d    
 * @param   heapID    
 */
//----------------------------------------------------------------------------------
static void mainG3Dcontrol(G3D_CONTROL* CG3d, HEAPID heapID)
{
  GFL_G3D_OBJ*  g3Dobj;
  u16           objIdx;

  objIdx = GFL_G3D_UTIL_GetUnitObjIdx( CG3d->g3Dutil, CG3d->g3DutilUnitIdx );

  // ICA�J�����X�V
  ICA_ANIME_IncAnimeFrameNoLoop( CG3d->icaAnime, FX32_ONE );

  // ICA�J�������W��ݒ�
  ICA_CAMERA_SetCameraStatus( CG3d->camera, CG3d->icaAnime );

  // �J�����؂�ւ�
  GFL_G3D_CAMERA_Switching( CG3d->camera );


  // �`��
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();
  {
    int i;

    for(i=0; i<NELEMS(g3Dutil_objTbl); i++){
      g3Dobj = GFL_G3D_UTIL_GetObjHandle( CG3d->g3Dutil, objIdx + i );
      GFL_G3D_DRAW_DrawObject( g3Dobj, &drawStatus);  // �S���f�t�H�ʒu��OK
    }
  }
  GFL_G3D_DRAW_End();

  // �A�j���t���[���i�߂�
  {
    int i;
    for(i=0;i<NELEMS(g3Dutil_objTbl); i++){
      g3Dobj = GFL_G3D_UTIL_GetObjHandle( CG3d->g3Dutil, objIdx + i );
      GFL_G3D_OBJECT_LoopAnimeFrame( g3Dobj, 0, FX32_ONE );
    }
  }
}



//----------------------------------------------------------------------------------
/**
 * @brief 3D�֘A���
 *
 * @param   CG3d    
 */
//----------------------------------------------------------------------------------
static void releaseG3Dcontrol(G3D_CONTROL* CG3d)
{
  // �J����ICA���
  ICA_ANIME_Delete( CG3d->icaAnime );

  // �J�����j��
  GFL_G3D_CAMERA_Delete( CG3d->camera );
  GFL_G3D_LIGHT_Delete( CG3d->g3Dlightset );
  GFL_G3D_UTIL_DelUnit( CG3d->g3Dutil, CG3d->g3DutilUnitIdx );
  GFL_G3D_UTIL_Delete( CG3d->g3Dutil );
}


//----------------------------------------------------------------------------------
/**
 * @brief �`���|�P�����V�[���֔�΂�
 *
 * @param   CG3d    
 */
//----------------------------------------------------------------------------------
static void setLegendPokeScene( G3D_CONTROL *CG3d, int anime_frame )
{
  GFL_G3D_OBJ*  g3Dobj;
  u16           objIdx;
  int i;
  int anmFrm = anime_frame*FX32_ONE;

  ICA_ANIME_SetAnimeFrame( CG3d->icaAnime, anmFrm );
  objIdx = GFL_G3D_UTIL_GetUnitObjIdx( CG3d->g3Dutil, CG3d->g3DutilUnitIdx );
  for(i=0;i<NELEMS(g3Dutil_objTbl); i++){
    g3Dobj = GFL_G3D_UTIL_GetObjHandle( CG3d->g3Dutil, objIdx + i );
    GFL_G3D_OBJECT_SetAnimeFrame( g3Dobj, 0, &anmFrm );
  }
  
}



//==============================================================================
//  OAMControl
//==============================================================================
///�A�N�^�[�ő吔
#define ACT_MAX     (64)

//--------------------------------------------------------------
static void setupOAMcontrol(OAM_CONTROL* COam, HEAPID heapID)
{
  ARCHANDLE *handle = GFL_ARC_OpenDataHandle( TITLE_RES_ARCID, heapID );
  
  GFL_CLACT_SYS_Create(&GFL_CLSYSINIT_DEF_DIVSCREEN, &vramBank, heapID);
  
  COam->clunit = GFL_CLACT_UNIT_Create(ACT_MAX, 0, heapID);
  GFL_CLACT_UNIT_SetDefaultRend(COam->clunit);

  // ---PUSH START BUTTON---
  COam->clres[OBJ_RES_CHR]  = GFL_CLGRP_CGR_Register( handle, 
                                                      TITLE_RES_PUSH_NCGR, 1, 
                                                      CLSYS_DRAW_SUB, heapID );
  COam->clres[OBJ_RES_PAL] = GFL_CLGRP_PLTT_Register( handle, 
                                                      TITLE_RES_PUSH_NCLR, 
                                                      CLSYS_DRAW_SUB, 0, heapID );
  COam->clres[OBJ_RES_CEL] = GFL_CLGRP_CELLANIM_Register( handle, 
                                                         TITLE_RES_PUSH_NCER, 
                                                         TITLE_RES_PUSH_NANR, 
                                                         heapID );
  {
    GFL_CLWK_DATA add;
    add.pos_x   = 128;
    add.pos_y   = 96;
    add.anmseq  = 0;
    add.bgpri   = 0;
    add.softpri = 0;

    COam->clwk    = GFL_CLACT_WK_Create( COam->clunit,
                                       COam->clres[OBJ_RES_CHR],
                                       COam->clres[OBJ_RES_PAL],
                                       COam->clres[OBJ_RES_CEL],
                                       &add, CLSYS_DEFREND_SUB, heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( COam->clwk, TRUE );
  
  }
  

  //OBJ�\��ON
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

  GFL_ARC_CloseDataHandle( handle );
}

static void mainOAMcontrol(OAM_CONTROL* COam, HEAPID heapID)
{
  GFL_CLACT_SYS_Main();
}

static void releaseOAMcontrol(OAM_CONTROL* COam)
{
  GFL_CLGRP_CGR_Release(      COam->clres[OBJ_RES_CHR] );
  GFL_CLGRP_PLTT_Release(     COam->clres[OBJ_RES_PAL] );
  GFL_CLGRP_CELLANIM_Release( COam->clres[OBJ_RES_CEL] );

 GFL_CLACT_UNIT_Delete(COam->clunit);
  GFL_CLACT_SYS_Delete();
}


