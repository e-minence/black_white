//============================================================================
/**
 *  @file   shinka_demo.c
 *  @brief  �i���f��
 *  @author Koji Kawada
 *  @data   2010.01.13
 *  @note   
 *
 *  ���W���[�����FSHINKADEMO
 */
//============================================================================
// �C���N���[�h
#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>

#include "demo/shinka_demo.h"

#include "gamesystem/game_beacon.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "arc_def.h"
#include "msg/msg_shinka_demo.h"
#include "msg/msg_waza_oboe.h"
#include "msg/msg_yesnomenu.h"
#include "system/main.h"
#include "system/gfl_use.h"
#include "font/font.naix"
#include "battle/app/b_plist.h"
#include "message.naix"

// �I�[�o�[���C
FS_EXTERN_OVERLAY(battle_b_app);
FS_EXTERN_OVERLAY(battle_plist);


#include "gamesystem/msgspeed.h"
#include "system/bmp_winframe.h"

#include "ui/yesno_menu.h"

#include "savedata/save_control.h"
#include "app/p_status.h"

#include "sound/wb_sound_data.sadl"
#include "sound/pm_sndsys.h"

#include "shinka_demo_graphic.h"
#include "shinka_demo_view.h"

// �I�[�o�[���C
FS_EXTERN_OVERLAY(ui_common);
FS_EXTERN_OVERLAY(poke_status);

//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
#define HEAP_SIZE              (0x80000)               ///< �q�[�v�T�C�Y

// BG�t���[��
#define BG_FRAME_M_BACK              (GFL_BG_FRAME2_M)        // �v���C�I���e�B2
#define BG_FRAME_M_POKEMON           (GFL_BG_FRAME0_M)        // �v���C�I���e�B1
#define BG_FRAME_M_STM_TEXT          (GFL_BG_FRAME1_M)        // �v���C�I���e�B0

#define BG_FRAME_S_BACK              (GFL_BG_FRAME0_S)        // �v���C�I���e�B2
#define BG_FRAME_S_BUTTON            (GFL_BG_FRAME1_S)        // �v���C�I���e�B1
#define BG_FRAME_S_BTN_TEXT          (GFL_BG_FRAME2_S)        // �v���C�I���e�B0

// BG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_M_BACKDROP          = 1,
  BG_PAL_NUM_M_STM_TEXT_FONT     = 1,
  BG_PAL_NUM_M_STM_TEXT_FRAME    = 1,
  BG_PAL_NUM_M_BACK              = 1,
};
// �ʒu
enum
{
  BG_PAL_POS_M_BACKDROP          = 0,                                                                // 0
  BG_PAL_POS_M_STM_TEXT_FONT     = BG_PAL_POS_M_BACKDROP        + BG_PAL_NUM_M_BACKDROP            , // 1
  BG_PAL_POS_M_STM_TEXT_FRAME    = BG_PAL_POS_M_STM_TEXT_FONT   + BG_PAL_NUM_M_STM_TEXT_FONT       , // 2
  BG_PAL_POS_M_BACK              = BG_PAL_POS_M_STM_TEXT_FRAME  + BG_PAL_NUM_M_STM_TEXT_FRAME      , // 3
  BG_PAL_POS_M_MAX               = BG_PAL_POS_M_BACK            + BG_PAL_NUM_M_BACK                , // 4  // ���������
};
// �{��
enum
{
  BG_PAL_NUM_S_BUTTON            = 2,
  BG_PAL_NUM_S_BTN_TEXT          = 1,
  BG_PAL_NUM_S_BACK              = 1,
};
/*
// �ʒu
enum
{
  BG_PAL_POS_S_BUTTON            = 0                                                              ,  // 0
  BG_PAL_POS_S_BTN_TEXT          = BG_PAL_POS_S_BUTTON          + BG_PAL_NUM_S_BUTTON             ,  // 2 
  BG_PAL_POS_S_BACK              = BG_PAL_POS_S_BTN_TEXT        + BG_PAL_NUM_S_BTN_TEXT           ,  // 3
  BG_PAL_POS_S_MAX               = BG_PAL_POS_S_BACK            + BG_PAL_NUM_S_BACK               ,  // 4  // ���������
};
*/
// �ʒu
enum
{
  BG_PAL_POS_S_BACK              = 0                                                              ,  // 0
  BG_PAL_POS_S_BUTTON            = BG_PAL_POS_S_BACK            + BG_PAL_NUM_S_BACK               ,  // 1 
  BG_PAL_POS_S_BTN_TEXT          = BG_PAL_POS_S_BUTTON          + BG_PAL_NUM_S_BUTTON             ,  // 3 
  BG_PAL_POS_S_MAX               = BG_PAL_POS_S_BTN_TEXT        + BG_PAL_NUM_S_BTN_TEXT           ,  // 4  // ���������
};

// OBJ�p���b�g
// �{��
// �ʒu
enum
{
  OBJ_PAL_POS_M_MAX              = 0                                                            ,         // ���������
};
// �{��
enum
{
  OBJ_PAL_NUM_S_CURSOR           = 2,
};
// �ʒu
enum
{
  OBJ_PAL_POS_S_CURSOR           = 0,
  OBJ_PAL_POS_S_MAX              = OBJ_PAL_POS_S_CURSOR        + OBJ_PAL_NUM_S_CURSOR            ,  // 2  // ���������
};

// BTN_TEXT
#define BTM_TEXT_WININ_BACK_COLOR        (15)

// �t�F�[�h
#define FADE_IN_WAIT           (2)         ///< �t�F�[�h�C���̃X�s�[�h
#define FADE_OUT_WAIT          (2)         ///< �t�F�[�h�A�E�g�̃X�s�[�h

#define STATUS_FADE_OUT_WAIT   (0)         ///< �Z�I���ֈڍs���邽�߂̃t�F�[�h�A�E�g�̃X�s�[�h
#define STATUS_FADE_IN_WAIT    (0)         ///< �Z�I������߂��Ă��邽�߂̃t�F�[�h�C���̃X�s�[�h

// 
#define TCBSYS_TASK_MAX        (8)

// ������΂����o�̂��߂̃p���b�g�t�F�[�h
#define PFADE_TCBSYS_TASK_MAX  (8)
#define PFADE_WAIT_TO_WHITE    (0)
#define PFADE_WAIT_FROM_WHITE  (2)

// �X�e�b�v
typedef enum
{
  STEP_FADE_IN,                     // �t�F�[�h�C��
  STEP_EVO_BEFORE,                  // ����I�H�@�悤�����c�c�I
  STEP_EVO_DEMO_BEFORE,             // �f��(BGM�̍Đ����J�n����Ă��Ȃ��Ƃ�)
  STEP_EVO_DEMO,                    // �f��
  STEP_EVO_DEMO_AFTER,              // �f��(BGM��push���Ă���Ƃ�)
  STEP_EVO_CANCEL,                  // ����c�c�H�@�ւ񂩂��@�Ƃ܂����I
  STEP_EVO_AFTER,                   // ���߂łƂ��I�@���񂩂����I
  STEP_WAZA_OBOE,                   // �Z�o���`�F�b�N
  STEP_WAZA_VACANT_MASTER,          // �����炵���@���ڂ����I(�Z���R�ȉ��������ꍇ)
  STEP_WAZA_FULL_PREPARE,           // ����Q�̏���
  STEP_WAZA_FULL_Q,                 // �ق��́@�킴���@�킷�ꂳ���܂����H
  STEP_WAZA_FULL_YN,                // �I��
  
  STEP_WAZA_STATUS_FIELD_FADE_OUT,        // �Z�I���Ɉڍs���邽�߂̃t�F�[�h�A�E�g(�t�B�[���h)
  STEP_WAZA_STATUS_FIELD,                 // �Z�I��PROC(�t�B�[���h)
  STEP_WAZA_STATUS_FIELD_OUT,             // �Z�I��PROC�𔲂�����̏���(�t�B�[���h)
  STEP_WAZA_STATUS_FIELD_FADE_IN,         // �Z�I������߂��Ă��邽�߂̃t�F�[�h�C��(�t�B�[���h)

  STEP_WAZA_STATUS_BATTLE_FADE_OUT,        // �Z�I���Ɉڍs���邽�߂̃t�F�[�h�A�E�g(�o�g��)
  STEP_WAZA_STATUS_BATTLE,                 // �Z�I���̃t�F�[�h�C�����Z�I��(�o�g��)
  STEP_WAZA_STATUS_BATTLE_OUT,             // �Z�I���̃t�F�[�h�A�E�g(�o�g��)
  STEP_WAZA_STATUS_BATTLE_FADE_IN,         // �Z�I������߂��Ă��邽�߂̃t�F�[�h�C��(�o�g��)

  STEP_WAZA_CONFIRM_PREPARE,        // ����Q�̏���
  STEP_WAZA_CONFIRM_Q,              // �킷�ꂳ���ā@��낵���ł��ˁH
  STEP_WAZA_CONFIRM_YN,             // �I��
  STEP_WAZA_FORGET,                 // �����������@���ꂢ�ɂ킷�ꂽ�I
  STEP_WAZA_FORGET_MASTER,          // �����炵���@���ڂ����I
  STEP_WAZA_ABANDON_PREPARE,        // ����Q�̏���
  STEP_WAZA_ABANDON_Q,              // ���ڂ���̂��@������߂܂����H
  STEP_WAZA_ABANDON_YN,             // �I��
  STEP_WAZA_NOT_MASTER,             // ���ڂ����Ɂ@��������I
  STEP_FADE_OUT_BEFORE,             // �t�F�[�h�A�E�g�O�ɏ����҂ۂ̊�
  STEP_FADE_OUT,                    // �t�F�[�h�A�E�g
  STEP_END,
}
STEP;

// �T�E���h�X�e�b�v
typedef enum
{
  SOUND_STEP_WAIT,
  SOUND_STEP_PLAY_SHINKA,
  SOUND_STEP_PLAYING_SHINKA,
  SOUND_STEP_PUSH_SHINKA,
  SOUND_STEP_POP_SHINKA,
  SOUND_STEP_PLAY_CONGRATULATE,
  SOUND_STEP_PLAYING_CONGRATULATE,
  SOUND_STEP_PLAY_WAZAOBOE,
  SOUND_STEP_PLAYING_WAZAOBOE,
  SOUND_STEP_FADE_OUT_SHINKA,
  SOUND_STEP_FADING_OUT_SHINKA,
}
SOUND_STEP;

// �����񒆂̃^�O�ɓo�^�������
typedef enum
{
  TAG_REGIST_TYPE_WORD,               // ������(STRBUF��n��)
  TAG_REGIST_TYPE_POKE_MONS_NAME,     // �|�P�����̎푰��(POKEMON_PARAM��n��)
  TAG_REGIST_TYPE_POKE_NICK_NAME,     // �|�P�����̃j�b�N�l�[��(POKEMON_PARAM��n��)
  TAG_REGIST_TYPE_WAZA,               // ���U��(���UID��n��)
  TAG_REGIST_TYPE_NONE,               // �Ȃ�(NULL��n��)
}
TAG_REGIST_TYPE;

#define STRBUF_LENGTH       (256)  // ���̕������ő���邩buflen.h�ŗv�m�F

//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// PROC ���[�N
//=====================================
typedef struct
{
  // 
  HEAPID                      heap_id;
  SHINKADEMO_GRAPHIC_WORK*    graphic;
  GFL_FONT*                   font;
  PRINT_QUE*                  print_que;

  // 
  POKEMON_PARAM*              pp;
  BOOL                        evo_cancel;

  // �X�e�b�v
  STEP                        step;
  // �T�E���h�X�e�b�v
  SOUND_STEP                  sound_step;
  BOOL                        sound_none;

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // STM_TEXT
  PRINT_STREAM*               stm_text_stream;
  GFL_TCBLSYS*                stm_text_tcblsys;
  GFL_BMPWIN*                 stm_text_winin_bmpwin;
  GFL_BMPWIN*                 stm_text_dummy_bmpwin;      ///< 0�Ԃ̃L�����N�^�[��1x1�ł����Ă���
  GFL_ARCUTIL_TRANSINFO       stm_text_winfrm_tinfo;
  GFL_MSGDATA*                stm_text_msgdata_shinka;
  GFL_MSGDATA*                stm_text_msgdata_wazaoboe;
  STRBUF*                     stm_text_strbuf;

  // (�Â�)�j�b�N�l�[���A�Y���Z
  STRBUF*                     poke_nick_name_strbuf;  // �|�P�����̎푰�����j�b�N�l�[���̏ꍇ�A�i����Ƀj�b�N�l�[�����ύX�����̂ŁB
  WazaID                      wazawasure_no;
  u8                          wazawasure_pos;  // �Y���Z�̈ʒu

  // wazaoboe
  WazaID                      wazaoboe_no;
  int                         wazaoboe_index;

  // YESNO_MENU
  GFL_MSGDATA*                yesno_menu_msgdata;
  STRBUF*                     yesno_menu_strbuf_yes;
  STRBUF*                     yesno_menu_strbuf_no;
  YESNO_MENU_WORK*            yesno_menu_work;

  // �o�g���X�e�[�^�X
  GFL_TCBSYS*                 tcbsys;
  void*                       tcbsys_work;
  PALETTE_FADE_PTR            pfd;  // �p���b�g�t�F�[�h
  u8                          cursor_flag;
  BPLIST_DATA                 bplist_data;

  // �t�B�[���h�X�e�[�^�X
  PSTATUS_DATA*               psData;

  // �i���f���̉��o
  SHINKADEMO_VIEW_WORK*       view;

  // �P��F�A�P��L������BG
  GFL_ARCUTIL_TRANSINFO       sim_transinfo;  // ARCUTIL�͎g�p���Ă��Ȃ����A�ʒu�ƃT�C�Y���ЂƂ܂Ƃ߂ɂ����������̂ŁB
  // �㉺�ɍ��т�\�����邽�߂�wnd
  u8                          wnd_up_y;    // wnd_up_y <= ������s�N�Z�� < wnd_down_y
  u8                          wnd_down_y;  //        0 <= ������s�N�Z�� < 192
  // ������΂����o�̂��߂̃p���b�g�t�F�[�h
  GFL_TCBSYS*                 pfade_tcbsys;
  void*                       pfade_tcbsys_work;
  PALETTE_FADE_PTR            pfade_ptr;  // �p���b�g�t�F�[�h
  s32                         pfade_step;
}
SHINKA_DEMO_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
static void ShinkaDemo_Init( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_Exit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );

static void ShinkaDemo_SoundMain( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );

static void ShinkaDemo_CreateSimpleBG( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_DeleteSimpleBG( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );

static void ShinkaDemo_InitWnd( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_ExitWnd( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_MainWnd( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );

static void ShinkaDemo_ZeroPFade( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_InitPFade( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );  // �p���b�g������������̂ŁA�S�p���b�g�̗p�ӂ��ς�ł���ĂԂ���
static void ShinkaDemo_ExitPFade( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_MainPFade( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_ToWhitePFade( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_FromWhitePFade( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static BOOL ShinkaDemo_CheckPFade( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );  // TRUE�������AFALSE�I��
static void ShinkaDemo_ToFromWhitePFade( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );

static void ShinkaDemo_VBlankFunc( GFL_TCB* tcb, void* wk );

static BOOL ShinkaDemo_StmTextStreamForgetCallBack( u32 arg );

static void ShinkaDemo_MakeStmTextStream( SHINKA_DEMO_WORK* work,
                                          GFL_MSGDATA* msgdata, u32 str_id, pPrintCallBack callback,
                                          TAG_REGIST_TYPE type0, const void* data0,
                                          TAG_REGIST_TYPE type1, const void* data1 );
static BOOL ShinkaDemo_WaitStmTextStream( SHINKA_DEMO_WORK* work );
static void ShinkaDemo_MakeYesNoMenuStrbuf( SHINKA_DEMO_WORK* work,
                                            GFL_MSGDATA* yes_msgdata, u32 yes_str_id,
                                            TAG_REGIST_TYPE yes_type0, const void* yes_data0,
                                            TAG_REGIST_TYPE yes_type1, const void* yes_data1,
                                            GFL_MSGDATA* no_msgdata, u32 no_str_id,
                                            TAG_REGIST_TYPE no_type0, const void* no_data0,
                                            TAG_REGIST_TYPE no_type1, const void* no_data1 );

static STRBUF* MakeStr( HEAPID heap_id,
                        GFL_MSGDATA* msgdata, u32 str_id,
                        TAG_REGIST_TYPE type0, const void* data0,
                        TAG_REGIST_TYPE type1, const void* data1 );

//=============================================================================
/**
*  PROC
*/
//=============================================================================
static GFL_PROC_RESULT ShinkaDemoProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT ShinkaDemoProcExit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT ShinkaDemoProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

const GFL_PROC_DATA   ShinkaDemoProcData = {
  ShinkaDemoProcInit,
  ShinkaDemoProcMain,
  ShinkaDemoProcExit,
};


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief       �i���f���p�p�����[�^�\���̐���
 *
 *  @param[in]   heapID        �q�[�vID
 *  @param[in]   gamedata      GAMEDATA
 *  @param[in]   ppt           POKEPARTY�\����
 *  @param[in]   after_mons_no �i����̃|�P�����i���o�[
 *  @param[in]   pos           �i������|�P������POKEPARTY���̃C���f�b�N�X
 *  @param[in]   cond          �i�������i�k�P�j���`�F�b�N�Ɏg�p�j
 *  @param[in]   b_field       �t�B�[���h����Ăяo���Ƃ���TRUE�A�o�g����ɌĂяo���Ƃ���FALSE
 *
 *  @retval      SHINKA_DEMO_PARAM
 */
//------------------------------------------------------------------
SHINKA_DEMO_PARAM*  SHINKADEMO_AllocParam( HEAPID heapID, GAMEDATA* gamedata, const POKEPARTY* ppt, u16 after_mons_no, u8 pos, u8 cond, BOOL b_field )
{ 
  SHINKA_DEMO_PARAM*  sdp = GFL_HEAP_AllocMemory( heapID, sizeof( SHINKA_DEMO_PARAM ) );

  SHINKADEMO_InitParam( sdp,
                        gamedata, ppt,
                        after_mons_no, pos, cond, b_field, TRUE );

  return sdp;
}

//------------------------------------------------------------------
/**
 *  @brief           �i���f���p�p�����[�^���[�N���
 *
 *  @param[in,out]   sdp           �p�����[�^���[�N�\����
 *
 *  @retval          
 */
//------------------------------------------------------------------
void  SHINKADEMO_FreeParam( SHINKA_DEMO_PARAM* sdp )
{ 
  GFL_HEAP_FreeMemory( sdp );
}

//------------------------------------------------------------------
/**
 *  @brief       �i���f���p�p�����[�^��ݒ肷��
 *
 *  @param[in,out] param             SHINKA_DEMO_PARAM
 *  @param[in]     gamedata          GAMEDATA
 *  @param[in]     ppt               POKEPARTY�\����
 *  @param[in]     after_mons_no     �i����̃|�P�����i���o�[
 *  @param[in]     pos               �i������|�P������POKEPARTY���̃C���f�b�N�X
 *  @param[in]     cond              �i�������i�k�P�j���`�F�b�N�Ɏg�p�j
 *  @param[in]     b_field           �t�B�[���h����Ăяo���Ƃ���TRUE�A�o�g����ɌĂяo���Ƃ���FALSE
 *  @param[in]     b_enable_cancel   �i���L�����Z���ł���Ƃ���TRUE�A�ł��Ȃ��Ƃ���FALSE
 *
 *  @retval      
 */
//------------------------------------------------------------------
void SHINKADEMO_InitParam( SHINKA_DEMO_PARAM* param,
                           GAMEDATA* gamedata, const POKEPARTY* ppt,
                           u16 after_mons_no, u8 pos, u8 cond, BOOL b_field, BOOL b_enable_cancel )
{
  param->gamedata          = gamedata;
  param->ppt               = ppt;
  param->after_mons_no     = after_mons_no;
  param->shinka_pos        = pos;
  param->shinka_cond       = cond;
  param->b_field           = b_field;
  param->b_enable_cancel   = b_enable_cancel;
}


//=============================================================================
/**
*  ���[�J���֐���`(PROC)
*/
//=============================================================================
//-------------------------------------
/// PROC ����������
//=====================================
static GFL_PROC_RESULT ShinkaDemoProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  SHINKA_DEMO_PARAM*    param    = (SHINKA_DEMO_PARAM*)pwk;
  SHINKA_DEMO_WORK*     work;

  // �q�[�v
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_SHINKA_DEMO, HEAP_SIZE );
    work = GFL_PROC_AllocWork( proc, sizeof(SHINKA_DEMO_WORK), HEAPID_SHINKA_DEMO );
    GFL_STD_MemClear( work, sizeof(SHINKA_DEMO_WORK) );
    
    work->heap_id       = HEAPID_SHINKA_DEMO;
  }

  // �X�e�b�v
  {
    // �X�e�b�v
    work->step          = STEP_FADE_IN;
    // �T�E���h�X�e�b�v
    work->sound_step    = SOUND_STEP_WAIT;
    work->sound_none    = FALSE;
  }

  // ���̑�
  {
    work->pp            = PokeParty_GetMemberPointer( param->ppt, param->shinka_pos );
  }

  // ����������
  ShinkaDemo_Init( param, work );

  // (�Â�)�j�b�N�l�[��
  {
    work->poke_nick_name_strbuf = GFL_STR_CreateBuffer( STRBUF_LENGTH, work->heap_id );
  }

  // �o�g���X�e�[�^�X
  {
    // ������
    work->tcbsys           = NULL;
    work->tcbsys_work      = NULL;
    work->pfd              = NULL;

    work->cursor_flag      = 0;
 }

  // �t�B�[���h�X�e�[�^�X
  {
     work->psData      = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(PSTATUS_DATA) );
  }

  // �t�F�[�h�C��(����������)
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, FADE_IN_WAIT );

  // �T�E���h
  if( param->b_field )
    PMSND_PushBGM();
  else
    PMSND_FadeOutBGM( 16 * FADE_IN_WAIT / 2 );

  // VBlank��TCB
  {
    work->vblank_tcb = GFUser_VIntr_CreateTCB( ShinkaDemo_VBlankFunc, work, 1 );
  }

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static GFL_PROC_RESULT ShinkaDemoProcExit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  SHINKA_DEMO_PARAM*    param    = (SHINKA_DEMO_PARAM*)pwk;
  SHINKA_DEMO_WORK*     work     = (SHINKA_DEMO_WORK*)mywk;

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �T�E���h
  PMSND_StopBGM();
  if( param->b_field )
    PMSND_PopBGM();

  // �t�B�[���h�X�e�[�^�X
  {
     GFL_HEAP_FreeMemory( work->psData );
  }   

  // �o�g���X�e�[�^�X
  {
    // ProcMain���ōς܂��Ă���̂ŁA�����ł͓��ɂ�邱�ƂȂ�
  }

  // (�Â�)�j�b�N�l�[��
  {
    GFL_STR_DeleteBuffer( work->poke_nick_name_strbuf );
  }

  // �I������
  ShinkaDemo_Exit( param, work );
 
  // �q�[�v
  {
    GFL_PROC_FreeWork( proc );
    GFL_HEAP_DeleteHeap( HEAPID_SHINKA_DEMO );
  }

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �又��
//=====================================
static GFL_PROC_RESULT ShinkaDemoProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  SHINKA_DEMO_PARAM*    param    = (SHINKA_DEMO_PARAM*)pwk;
  SHINKA_DEMO_WORK*     work     = (SHINKA_DEMO_WORK*)mywk;

  int key_trg = GFL_UI_KEY_GetTrg();

  switch( work->step )
  {
  case STEP_FADE_IN:
    {
      if( !GFL_FADE_CheckFade() )
      {
        // ����
        work->step = STEP_EVO_BEFORE;
        
        ShinkaDemo_MakeStmTextStream(
          work,
          work->stm_text_msgdata_shinka, SHINKADEMO_ShinkaBeforeMsg, NULL,
          TAG_REGIST_TYPE_POKE_NICK_NAME, work->pp,
          TAG_REGIST_TYPE_NONE, NULL );
      }
    }
    break;
  case STEP_EVO_BEFORE:
    {
      if( ShinkaDemo_WaitStmTextStream( work ) )
      {
        // ����
        work->step = STEP_EVO_DEMO_BEFORE;
        
        work->evo_cancel = FALSE;
        SHINKADEMO_VIEW_StartShinka( work->view );
      }
    }
    break;
  case STEP_EVO_DEMO_BEFORE:
    {
      if( SHINKADEMO_VIEW_IsBGMPlay( work->view ) )
      {
        // ����
        work->step = STEP_EVO_DEMO;
        
        work->sound_step = SOUND_STEP_PLAY_SHINKA;
      }
    }
    break;
  case STEP_EVO_DEMO:
    {
      if( SHINKADEMO_VIEW_ToFromWhite( work->view ) )
      {
        // ������΂����o�̂��߂̃p���b�g�t�F�[�h
        ShinkaDemo_ToFromWhitePFade( param, work );
      }

      if( !SHINKADEMO_VIEW_IsBGMPlay( work->view ) )
      {
        // ����
        work->step = STEP_EVO_DEMO_AFTER;

        work->sound_step = SOUND_STEP_PUSH_SHINKA;
      }
      else
      {
        if( param->b_enable_cancel )
        {
          if( key_trg & PAD_BUTTON_B )
          {
            BOOL success_cancel = SHINKADEMO_VIEW_CancelShinka( work->view );
            if( success_cancel )
            {
              work->evo_cancel = TRUE;
            }
          }
        }
      }
    }
    break;
  case STEP_EVO_DEMO_AFTER:
    {
      if( SHINKADEMO_VIEW_IsEndShinka( work->view ) )
      {
        if( work->evo_cancel )
        {
          // ����
          work->step = STEP_EVO_CANCEL;

          ShinkaDemo_MakeStmTextStream(
            work,
            work->stm_text_msgdata_shinka, SHINKADEMO_ShinkaCancelMsg, NULL,
            TAG_REGIST_TYPE_POKE_NICK_NAME, work->pp,
            TAG_REGIST_TYPE_NONE, NULL );

          // �T�E���h
          work->sound_step = SOUND_STEP_POP_SHINKA;
        }
        else
        {
          // (�Â�)�j�b�N�l�[��
          PP_Get( work->pp, ID_PARA_nickname, work->poke_nick_name_strbuf );

          // �i��
          PP_ChangeMonsNo( work->pp, param->after_mons_no );

          // �}�ӓo�^�i�߂܂����j
          {
            if( param->gamedata )
            {
              ZUKAN_SAVEDATA* zukan_savedata = GAMEDATA_GetZukanSave( param->gamedata );
              ZUKANSAVE_SetPokeGet( zukan_savedata, work->pp );  // �}�Ӄt���O���Z�b�g����
            }
          }

          // ����
          work->step = STEP_EVO_AFTER;

          ShinkaDemo_MakeStmTextStream(
            work,
            work->stm_text_msgdata_shinka, SHINKADEMO_ShinkaMsg, NULL,
            TAG_REGIST_TYPE_WORD, work->poke_nick_name_strbuf,
            TAG_REGIST_TYPE_POKE_MONS_NAME, work->pp );
         
          // �T�E���h
          work->sound_step = SOUND_STEP_PLAY_CONGRATULATE;
        }
      }
    }
    break;
  case STEP_EVO_CANCEL:
    {
      if( ShinkaDemo_WaitStmTextStream( work ) )
      {
        // ����
        work->step = STEP_FADE_OUT_BEFORE;
      }
    }
    break;
  case STEP_EVO_AFTER:
    {
      if( ShinkaDemo_WaitStmTextStream( work ) )
      {
        // ����Ⴂ�p�f�[�^���Z�b�g
        { 
          STRBUF* strbuf = GFL_STR_CreateBuffer( STRBUF_LENGTH, work->heap_id );
          PP_Get( work->pp, ID_PARA_nickname, strbuf );  // �����͐V�����j�b�N�l�[���ł����̂��낤���H
          GAMEBEACON_Set_PokemonEvolution( strbuf );
          GFL_STR_DeleteBuffer( strbuf );
        }

        // ����
        work->step = STEP_WAZA_OBOE;

        work->wazaoboe_index = 0;
      }
    }
    break;
  case STEP_WAZA_OBOE:
    {
      if( ShinkaDemo_WaitStmTextStream( work ) )
      {
        // �Z�o���`�F�b�N
        work->wazaoboe_no = PP_CheckWazaOboe( work->pp, &work->wazaoboe_index, work->heap_id );
        if( work->wazaoboe_no == PTL_WAZAOBOE_NONE )
        {
          // ����
          work->step = STEP_FADE_OUT_BEFORE;
        }
        else if( work->wazaoboe_no == PTL_WAZASET_SAME )
        {
          // ���̃C���f�b�N�X�ŋZ�o���`�F�b�N
          break;
        }
        else if( work->wazaoboe_no & PTL_WAZAOBOE_FULL )
        {
          work->wazaoboe_no &= ( PTL_WAZAOBOE_FULL ^ 0xffff );

          // ����
          work->step = STEP_WAZA_FULL_PREPARE;
        }
        else
        {
          // ����
          work->step = STEP_WAZA_VACANT_MASTER;
          
          // �Z�o����
          ShinkaDemo_MakeStmTextStream(
            work,
            work->stm_text_msgdata_wazaoboe, msg_waza_oboe_04, NULL,
            TAG_REGIST_TYPE_POKE_NICK_NAME, work->pp,
            TAG_REGIST_TYPE_WAZA, &work->wazaoboe_no );

          // �T�E���h
          work->sound_step = SOUND_STEP_PLAY_WAZAOBOE; 
        }
      }
    }
    break;
  case STEP_WAZA_VACANT_MASTER:
    {
      if( ShinkaDemo_WaitStmTextStream( work ) )
      {
        if( key_trg & PAD_BUTTON_A )
        {
          // ����
          work->step = STEP_WAZA_OBOE;
        }
      }
    }
    break;
  case STEP_WAZA_FULL_PREPARE:
    {
      // ����
      work->step = STEP_WAZA_FULL_Q;
          
      // �Z�̎莝���������ς�
      ShinkaDemo_MakeStmTextStream(
        work,
        work->stm_text_msgdata_wazaoboe, msg_waza_oboe_05, NULL,
        TAG_REGIST_TYPE_POKE_NICK_NAME, work->pp,
        TAG_REGIST_TYPE_WAZA, &work->wazaoboe_no );
    }
    break;
  case STEP_WAZA_FULL_Q:
    {
      if( ShinkaDemo_WaitStmTextStream( work ) )
      {
        // ����
        work->step = STEP_WAZA_FULL_YN;

        // �Z�Y��m�F����
        ShinkaDemo_MakeYesNoMenuStrbuf( work,
                                        work->yesno_menu_msgdata, msgid_yesno_wazawasureru,
                                        TAG_REGIST_TYPE_NONE, NULL,
                                        TAG_REGIST_TYPE_NONE, NULL,
                                        work->yesno_menu_msgdata, msgid_yesno_wazawasurenai,
                                        TAG_REGIST_TYPE_NONE, NULL,
                                        TAG_REGIST_TYPE_NONE, NULL );
        YESNO_MENU_OpenMenu( work->yesno_menu_work, work->yesno_menu_strbuf_yes, work->yesno_menu_strbuf_no );
      }
    }
    break;
  case STEP_WAZA_FULL_YN:
    {
      YESNO_MENU_SEL sel = YESNO_MENU_GetSelect( work->yesno_menu_work );
      if( sel != YESNO_MENU_SEL_SEL )
      {
        if( sel == YESNO_MENU_SEL_YES )
        {
          if( param->b_field )
          {
            // ����
            work->step = STEP_WAZA_STATUS_FIELD_FADE_OUT;

            // �t�F�[�h�A�E�g(�����遨��)
            GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, STATUS_FADE_OUT_WAIT );
          }
          else
          {
            // ����
            work->step = STEP_WAZA_STATUS_BATTLE_FADE_OUT;

            // �t�F�[�h�A�E�g(�����遨��)
            GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 0, 16, STATUS_FADE_OUT_WAIT );
          }
        }
        else if( sel == YESNO_MENU_SEL_NO )
        {
          // ����
          work->step = STEP_WAZA_ABANDON_PREPARE;
        }
        YESNO_MENU_CloseMenu( work->yesno_menu_work );
      }
    }
    break;

  case STEP_WAZA_STATUS_FIELD_FADE_OUT:
    {
      if( !GFL_FADE_CheckFade() )
      {
        // �I������
        ShinkaDemo_Exit( param, work );
        
        // ����
        work->step = STEP_WAZA_STATUS_FIELD;

        {
          // �t�B�[���h�X�e�[�^�X
          SAVE_CONTROL_WORK* svWork = GAMEDATA_GetSaveControlWork( param->gamedata );
          
          work->psData->ppt               = PST_PP_TYPE_POKEPARTY;
          work->psData->game_data         = param->gamedata;
          work->psData->isExitRequest     = FALSE;

          work->psData->ppd               = (void*)(param->ppt);
          work->psData->cfg               = SaveData_GetConfig( svWork );
          work->psData->max               = PokeParty_GetPokeCount( param->ppt );
          work->psData->pos               = param->shinka_pos;

          work->psData->waza              = work->wazaoboe_no;
          work->psData->mode              = PST_MODE_WAZAADD;
          work->psData->page              = PPT_SKILL;
          work->psData->canExitButton     = FALSE;

          GFL_OVERLAY_Load( FS_OVERLAY_ID(poke_status) );
          GFL_PROC_SysCallProc( NO_OVERLAY_ID, &PokeStatus_ProcData, work->psData );
        }
      }
    }
    break;
  case STEP_WAZA_STATUS_FIELD:
    {
      // ����
      work->step = STEP_WAZA_STATUS_FIELD_OUT;
    }
    break;
  case STEP_WAZA_STATUS_FIELD_OUT:
    {
      {
        // �t�B�[���h�X�e�[�^�X
        GFL_OVERLAY_Unload( FS_OVERLAY_ID(poke_status) );
      }

      // ����
      work->step = STEP_WAZA_STATUS_FIELD_FADE_IN;
 
      // ����������
      ShinkaDemo_Init( param, work );

      // �t�F�[�h�C��(����������)
      GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, STATUS_FADE_IN_WAIT );
    }
    break;
  case STEP_WAZA_STATUS_FIELD_FADE_IN:
    {
      if( !GFL_FADE_CheckFade() )
      {
        switch( work->psData->ret_mode )
        {
        case PST_RET_DECIDE:
          {
            work->wazawasure_pos  = work->psData->ret_sel;

            // �Y���Z
            work->wazawasure_no = PP_Get( work->pp, ID_PARA_waza1 + work->wazawasure_pos, NULL );

            // ����
            work->step = STEP_WAZA_CONFIRM_PREPARE;
          }
          break;
        case PST_RET_CANCEL:
        case PST_RET_EXIT:
        default:
          {
            // ����
            work->step = STEP_WAZA_ABANDON_PREPARE;
          }
          break;
        }
      }
    }
    break;

  case STEP_WAZA_STATUS_BATTLE_FADE_OUT:
    {
      if( !GFL_FADE_CheckFade() )
      {
        // ����
        work->step = STEP_WAZA_STATUS_BATTLE;

        // ����ʂ̔j��
        YESNO_MENU_DeleteRes( work->yesno_menu_work );
        SHINKADEMO_GRAPHIC_ExitBGSub( work->graphic );

        // �o�g���X�e�[�^�X�p�^�X�N
        work->tcbsys_work = GFL_HEAP_AllocMemory( work->heap_id, GFL_TCB_CalcSystemWorkSize(TCBSYS_TASK_MAX) );
        GFL_STD_MemClear( work->tcbsys_work, GFL_TCB_CalcSystemWorkSize(TCBSYS_TASK_MAX) );
        work->tcbsys = GFL_TCB_Init( TCBSYS_TASK_MAX, work->tcbsys_work );
 
        // �p���b�g�t�F�[�h
        work->pfd = PaletteFadeInit( work->heap_id );
        PaletteTrans_AutoSet( work->pfd, TRUE );
        PaletteFadeWorkAllocSet( work->pfd, FADE_SUB_BG, 0x1e0, work->heap_id );
        PaletteFadeWorkAllocSet( work->pfd, FADE_SUB_OBJ, 0x1e0, work->heap_id );

        // �o�g���X�e�[�^�X
        work->bplist_data.pp          = (POKEPARTY*)param->ppt;
        work->bplist_data.font        = work->font;
        work->bplist_data.heap        = work->heap_id;
        work->bplist_data.mode        = BPL_MODE_WAZASET;
        work->bplist_data.end_flg     = FALSE;
        work->bplist_data.sel_poke    = param->shinka_pos;
        work->bplist_data.chg_waza    = work->wazaoboe_no;
        work->bplist_data.rule        = 0;
        work->bplist_data.cursor_flg  = &work->cursor_flag;
        work->bplist_data.tcb_sys     = work->tcbsys;
        work->bplist_data.pfd         = work->pfd;

        GFL_OVERLAY_Load( FS_OVERLAY_ID( battle_b_app ) );
        GFL_OVERLAY_Load( FS_OVERLAY_ID( battle_plist ) );
        
        BattlePokeList_TaskAdd( &work->bplist_data );

        // �t�F�[�h�C��(����������)
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, STATUS_FADE_IN_WAIT );
      }
    }
    break;
  case STEP_WAZA_STATUS_BATTLE:
    {
      // �o�g���X�e�[�^�X
      GFL_TCB_Main( work->tcbsys );

      if( work->bplist_data.end_flg )
      {
        // ����
        work->step = STEP_WAZA_STATUS_BATTLE_OUT;

        // �t�F�[�h�A�E�g(�����遨��)
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 0, 16, STATUS_FADE_OUT_WAIT );
      }
    }
    break;
  case STEP_WAZA_STATUS_BATTLE_OUT:
    {
      if( !GFL_FADE_CheckFade() )
      {
        // ����
        work->step = STEP_WAZA_STATUS_BATTLE_FADE_IN;

        // �o�g���X�e�[�^�X
        GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle_b_app ) );
        GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle_plist ) );

        // �p���b�g�t�F�[�h
        PaletteFadeWorkAllocFree( work->pfd, FADE_SUB_BG );
        PaletteFadeWorkAllocFree( work->pfd, FADE_SUB_OBJ );
        PaletteFadeFree( work->pfd );

        // �o�g���X�e�[�^�X�p�^�X�N
        GFL_TCB_Exit( work->tcbsys );
        GFL_HEAP_FreeMemory( work->tcbsys_work );

        // ������
        work->tcbsys           = NULL;
        work->tcbsys_work      = NULL;
        work->pfd              = NULL;

        // ����ʂ̐���
        SHINKADEMO_GRAPHIC_InitBGSub( work->graphic );
        work->yesno_menu_work = YESNO_MENU_CreateRes( work->heap_id,
                                  BG_FRAME_S_BUTTON, 0, BG_PAL_POS_S_BUTTON,
                                  OBJ_PAL_POS_S_CURSOR,
                                  SHINKADEMO_GRAPHIC_GetClunit(work->graphic),
                                  work->font,
                                  work->print_que,
                                  FALSE );

        // �t�F�[�h�C��(����������)
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, STATUS_FADE_IN_WAIT );
      }
    }
    break;
  case STEP_WAZA_STATUS_BATTLE_FADE_IN:
    {
      if( !GFL_FADE_CheckFade() )
      {
        // �o�g���X�e�[�^�X
        work->wazawasure_pos = work->bplist_data.sel_wp;
        if( work->wazawasure_pos == BPL_SEL_WP_CANCEL )
        {
          // ����
          work->step = STEP_WAZA_ABANDON_PREPARE;
        }
        else
        {
          // �Y���Z
          work->wazawasure_no = PP_Get( work->pp, ID_PARA_waza1 + work->wazawasure_pos, NULL );

          // ����
          work->step = STEP_WAZA_CONFIRM_PREPARE;
        }
      }
    }
    break;
  
  case STEP_WAZA_CONFIRM_PREPARE:
    {
    /*
      �Y���Z�̊m�F�͂��Ȃ��Ă����̂ŁASTEP_WAZA_CONFIRM_Q��STEP_WAZA_CONFIRM_YN�͔�΂�
      // ����
      work->step = STEP_WAZA_CONFIRM_Q;

      ShinkaDemo_MakeStmTextStream(
        work,
        work->stm_text_msgdata_wazaoboe, msg_waza_oboe_11, NULL,
        TAG_REGIST_TYPE_WAZA, &work->wazawasure_no,
        TAG_REGIST_TYPE_NONE, NULL );
    */
      // ����
      work->step = STEP_WAZA_FORGET;

      ShinkaDemo_MakeStmTextStream(
        work,
        work->stm_text_msgdata_wazaoboe, msg_waza_oboe_06, ShinkaDemo_StmTextStreamForgetCallBack,
        TAG_REGIST_TYPE_POKE_NICK_NAME, work->pp,
        TAG_REGIST_TYPE_WAZA, &work->wazawasure_no );
    }
    break;
  case STEP_WAZA_CONFIRM_Q:
    {
      if( ShinkaDemo_WaitStmTextStream( work ) )
      {
        // ����
        work->step = STEP_WAZA_CONFIRM_YN;

        ShinkaDemo_MakeYesNoMenuStrbuf( work,
                                        work->yesno_menu_msgdata, msgid_yesno_wazawasureru,
                                        TAG_REGIST_TYPE_NONE, NULL,
                                        TAG_REGIST_TYPE_NONE, NULL,
                                        work->yesno_menu_msgdata, msgid_yesno_wazawasurenai,
                                        TAG_REGIST_TYPE_NONE, NULL,
                                        TAG_REGIST_TYPE_NONE, NULL );
        YESNO_MENU_OpenMenu( work->yesno_menu_work, work->yesno_menu_strbuf_yes, work->yesno_menu_strbuf_no );
      }
    }
    break;
  case STEP_WAZA_CONFIRM_YN:
    {
      YESNO_MENU_SEL sel = YESNO_MENU_GetSelect( work->yesno_menu_work );
      if( sel != YESNO_MENU_SEL_SEL )
      {
        if( sel == YESNO_MENU_SEL_YES )
        {
          // ����
          work->step = STEP_WAZA_FORGET;

          ShinkaDemo_MakeStmTextStream(
            work,
            work->stm_text_msgdata_wazaoboe, msg_waza_oboe_06, ShinkaDemo_StmTextStreamForgetCallBack,
            TAG_REGIST_TYPE_POKE_NICK_NAME, work->pp,
            TAG_REGIST_TYPE_WAZA, &work->wazawasure_no );
        }
        else if( sel == YESNO_MENU_SEL_NO )
        {
          // ����
          work->step = STEP_WAZA_FULL_PREPARE;
        }
        YESNO_MENU_CloseMenu( work->yesno_menu_work );
      }
    }
    break;
  case STEP_WAZA_FORGET:
    {
      if( ShinkaDemo_WaitStmTextStream( work ) )
      {
        // �Z�o����
        PP_SetWazaPos( work->pp, work->wazaoboe_no, work->wazawasure_pos );
        
        // ����
        work->step = STEP_WAZA_FORGET_MASTER;

        ShinkaDemo_MakeStmTextStream(
          work,
          work->stm_text_msgdata_wazaoboe, msg_waza_oboe_07, NULL,
          TAG_REGIST_TYPE_POKE_NICK_NAME, work->pp,
          TAG_REGIST_TYPE_WAZA, &work->wazaoboe_no );

        // �T�E���h
        work->sound_step = SOUND_STEP_PLAY_WAZAOBOE; 
      }
    }
    break;
  case STEP_WAZA_FORGET_MASTER:
    {
      if( ShinkaDemo_WaitStmTextStream( work ) )
      {
         // ����
        work->step = STEP_WAZA_OBOE;
      }
    }
    break;
  case STEP_WAZA_ABANDON_PREPARE:
    {
      // ����
      work->step = STEP_WAZA_ABANDON_Q;

      ShinkaDemo_MakeStmTextStream(
        work,
        work->stm_text_msgdata_wazaoboe, msg_waza_oboe_08, NULL,
        TAG_REGIST_TYPE_NONE, NULL,
        TAG_REGIST_TYPE_WAZA, &work->wazaoboe_no );
    }
    break;
  case STEP_WAZA_ABANDON_Q:
    {
      if( ShinkaDemo_WaitStmTextStream( work ) )
      {
        // ����
        work->step = STEP_WAZA_ABANDON_YN;

        // �Z������ߊm�F����
        ShinkaDemo_MakeYesNoMenuStrbuf( work,
                                        work->yesno_menu_msgdata, msgid_yesno_wazaakirameru,
                                        TAG_REGIST_TYPE_WAZA, &work->wazaoboe_no,
                                        TAG_REGIST_TYPE_NONE, NULL,
                                        work->yesno_menu_msgdata, msgid_yesno_wazaakiramenai,
                                        TAG_REGIST_TYPE_WAZA, &work->wazaoboe_no,
                                        TAG_REGIST_TYPE_NONE, NULL );
        YESNO_MENU_OpenMenu( work->yesno_menu_work, work->yesno_menu_strbuf_yes, work->yesno_menu_strbuf_no );
      }
    }
    break;
  case STEP_WAZA_ABANDON_YN:
    {
      YESNO_MENU_SEL sel = YESNO_MENU_GetSelect( work->yesno_menu_work );
      if( sel != YESNO_MENU_SEL_SEL )
      {
        if( sel == YESNO_MENU_SEL_YES )
        {
          // ����
          work->step = STEP_WAZA_NOT_MASTER;

          ShinkaDemo_MakeStmTextStream(
            work,
            work->stm_text_msgdata_wazaoboe, msg_waza_oboe_09, NULL,
            TAG_REGIST_TYPE_POKE_NICK_NAME, work->pp,
            TAG_REGIST_TYPE_WAZA, &work->wazaoboe_no );
        }
        else if( sel == YESNO_MENU_SEL_NO )
        {
          // ����
          work->step = STEP_WAZA_FULL_PREPARE;
        }
        YESNO_MENU_CloseMenu( work->yesno_menu_work );
      }
    }
    break;
  case STEP_WAZA_NOT_MASTER:
    {
      if( ShinkaDemo_WaitStmTextStream( work ) )
      {
        // ����
        work->step = STEP_WAZA_OBOE;
      }
    }
    break;
  case STEP_FADE_OUT_BEFORE:
    {
      work->sound_none = TRUE;

      if( work->sound_step == SOUND_STEP_PLAYING_SHINKA )
      {
        // ����
        work->step = STEP_FADE_OUT;

        // �t�F�[�h�A�E�g(�����遨��)
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, FADE_OUT_WAIT );

        // �T�E���h
        work->sound_step = SOUND_STEP_FADE_OUT_SHINKA;
      }
    }
    break;
  case STEP_FADE_OUT:
    {
      if( (!GFL_FADE_CheckFade()) && (work->sound_step == SOUND_STEP_WAIT) )
      {
        // ����
        work->step = STEP_END;

        return GFL_PROC_RES_FINISH;
      }
    }
    break;
  default:
    break;
  }

  ShinkaDemo_SoundMain( param, work );
  
  // ������΂����o�̂��߂̃p���b�g�t�F�[�h
  ShinkaDemo_MainPFade( param, work );

  if(    work->step != STEP_WAZA_STATUS_FIELD
      && work->step != STEP_WAZA_STATUS_FIELD_OUT )
  {
    YESNO_MENU_Main( work->yesno_menu_work );

    GFL_TCBL_Main( work->stm_text_tcblsys );
    
    PRINTSYS_QUE_Main( work->print_que );

    SHINKADEMO_VIEW_Main( work->view );

    // 2D�`��
    SHINKADEMO_GRAPHIC_2D_Draw( work->graphic );
    // 3D�`��
    SHINKADEMO_GRAPHIC_3D_StartDraw( work->graphic );
    SHINKADEMO_VIEW_Draw( work->view );
    SHINKADEMO_GRAPHIC_3D_EndDraw( work->graphic );
  }

  return GFL_PROC_RES_CONTINUE;
}


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// ����������
//=====================================
static void ShinkaDemo_Init( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  // �I�[�o�[���C
  GFL_OVERLAY_Load( FS_OVERLAY_ID( ui_common ) );

  // ��������ɍs��������
  {
    // ��\��
    u8 i;
    for(i=GFL_BG_FRAME0_M; i<=GFL_BG_FRAME3_S; i++)
    {
      GFL_BG_SetVisible( i, VISIBLE_OFF );
    }
  } 

  // SHINKA_DEMO_WORK����
  {
    // 
    work->graphic       = SHINKADEMO_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, work->heap_id );
    SHINKADEMO_GRAPHIC_InitBGSub( work->graphic );
    work->font          = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, work->heap_id );
    work->print_que     = PRINTSYS_QUE_Create( work->heap_id );
  }

  // STM_TEXT
  {
    // �p���b�g
    GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
      BG_PAL_POS_M_STM_TEXT_FONT * 0x20, BG_PAL_NUM_M_STM_TEXT_FONT * 0x20, work->heap_id );

    // BG�t���[���̃X�N���[���̋󂢂Ă���ӏ��ɉ����\��������Ȃ��悤�ɂ��Ă���
    work->stm_text_dummy_bmpwin = GFL_BMPWIN_Create( BG_FRAME_M_STM_TEXT, 0, 0, 1, 1,
                                    BG_PAL_POS_M_STM_TEXT_FONT, GFL_BMP_CHRAREA_GET_F );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->stm_text_dummy_bmpwin), 0 );
    GFL_BMPWIN_TransVramCharacter(work->stm_text_dummy_bmpwin);

    // �E�B���h�E��
    work->stm_text_winin_bmpwin = GFL_BMPWIN_Create( BG_FRAME_M_STM_TEXT, 1, 19, 30, 4,
                                     BG_PAL_POS_M_STM_TEXT_FONT, GFL_BMP_CHRAREA_GET_B );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->stm_text_winin_bmpwin), BTM_TEXT_WININ_BACK_COLOR );
    GFL_BMPWIN_TransVramCharacter(work->stm_text_winin_bmpwin);
    
    // �E�B���h�E�g
    work->stm_text_winfrm_tinfo = BmpWinFrame_GraphicSetAreaMan( BG_FRAME_M_STM_TEXT,
                                    BG_PAL_POS_M_STM_TEXT_FRAME,
                                    MENU_TYPE_SYSTEM, work->heap_id );
    BmpWinFrame_Write( work->stm_text_winin_bmpwin, WINDOW_TRANS_ON_V,
                       GFL_ARCUTIL_TRANSINFO_GetPos(work->stm_text_winfrm_tinfo),
                       BG_PAL_POS_M_STM_TEXT_FRAME );
    GFL_BMPWIN_MakeTransWindow_VBlank( work->stm_text_winin_bmpwin );

    // ���b�Z�[�W
    work->stm_text_msgdata_shinka     = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_shinka_demo_dat, work->heap_id );
    work->stm_text_msgdata_wazaoboe   = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_waza_oboe_dat, work->heap_id );

    // TCBL�A�t�H���g�J���[�A�]���Ȃ�
    work->stm_text_tcblsys = GFL_TCBL_Init( work->heap_id, work->heap_id, 1, 0 );
    GFL_FONTSYS_SetColor( 1, 2, BTM_TEXT_WININ_BACK_COLOR );
    GFL_BG_LoadScreenV_Req( BG_FRAME_M_STM_TEXT );

    // NULL������
    work->stm_text_strbuf       = NULL;
    work->stm_text_stream       = NULL;
  }

  // YESNO_MENU
  {
    // ���b�Z�[�W
    work->yesno_menu_msgdata     = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_yesnomenu_dat, work->heap_id );

    // ���[�N
    work->yesno_menu_work        = YESNO_MENU_CreateRes( work->heap_id,
                                      BG_FRAME_S_BUTTON, 0, BG_PAL_POS_S_BUTTON,
                                      OBJ_PAL_POS_S_CURSOR,
                                      SHINKADEMO_GRAPHIC_GetClunit(work->graphic),
                                      work->font,
                                      work->print_que,
                                      FALSE );

    // NULL������
    work->yesno_menu_strbuf_yes = NULL;
    work->yesno_menu_strbuf_no  = NULL;
  }

  // �P��F�A�P��L������BG�𐶐�����
  ShinkaDemo_CreateSimpleBG( param, work );

  // �㉺�ɍ��т�\�����邽�߂�wnd
  ShinkaDemo_InitWnd( param, work );

  // �v���C�I���e�B�A�\���A�w�i�F�Ȃ�
  {
    GFL_BG_SetPriority( BG_FRAME_M_BACK                  , 2 );
    GFL_BG_SetPriority( BG_FRAME_M_POKEMON               , 1 );
    GFL_BG_SetPriority( BG_FRAME_M_STM_TEXT              , 0 );  // �őO��

    GFL_BG_SetPriority( BG_FRAME_S_BACK                  , 2 );
    //GFL_BG_SetPriority( BG_FRAME_S_BUTTON                , 1 );
    //GFL_BG_SetPriority( BG_FRAME_S_BTN_TEXT              , 0 );  // �őO��

    GFL_BG_SetVisible( BG_FRAME_M_BACK                  , VISIBLE_ON );
    GFL_BG_SetVisible( BG_FRAME_M_POKEMON               , VISIBLE_ON );
    GFL_BG_SetVisible( BG_FRAME_M_STM_TEXT              , VISIBLE_ON );
  
    GFL_BG_SetVisible( BG_FRAME_S_BACK                  , VISIBLE_ON );
    //GFL_BG_SetVisible( BG_FRAME_S_BUTTON                , VISIBLE_ON );
    //GFL_BG_SetVisible( BG_FRAME_S_BTN_TEXT              , VISIBLE_ON );
  
    GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, 0x0000 );
    GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, 0x0000 );
  }

  // �i���f���̉��o
  {
    if( work->step == STEP_FADE_IN )
    {
      work->view = SHINKADEMO_VIEW_Init(
                     work->heap_id,
                     SHINKADEMO_VIEW_LAUNCH_EVO,
                     work->pp,
                     param->after_mons_no
                   );
    }
    else
    {
      work->view = SHINKADEMO_VIEW_Init(
                     work->heap_id,
                     SHINKADEMO_VIEW_LAUNCH_AFTER,
                     work->pp,
                     param->after_mons_no
                   );
    }
  }

  // �p�[�e�B�N���Ή�
  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG0,
                    GX_BLEND_PLANEMASK_BD | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG1,
                    0, 0 );
    // �p�[�e�B�N���̃A���t�@�����ꂢ�ɏo������
    // ev1��ev2�͎g���Ȃ�  // TWL�v���O���~���O�}�j���A���u2D�ʂƂ̃��u�����f�B���O�v�Q�l

  // ������΂����o�̂��߂̃p���b�g�t�F�[�h
  ShinkaDemo_InitPFade( param, work );  // �p���b�g������������̂ŁA�S�p���b�g�̗p�ӂ��ς�ł���ĂԂ���
}

//-------------------------------------
/// �I������
//=====================================
static void ShinkaDemo_Exit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  // ������΂����o�̂��߂̃p���b�g�t�F�[�h
  ShinkaDemo_ExitPFade( param, work );

  // �㉺�ɍ��т�\�����邽�߂�wnd
  ShinkaDemo_ExitWnd( param, work );

  // �P��F�A�P��L������BG��j������
  ShinkaDemo_DeleteSimpleBG( param, work );

  // �i���f���̉��o
  {
    SHINKADEMO_VIEW_Exit( work->view );
  }

  // YESNO_MENU
  {
    YESNO_MENU_DeleteRes( work->yesno_menu_work );
    if( work->yesno_menu_strbuf_yes ) GFL_STR_DeleteBuffer( work->yesno_menu_strbuf_yes );
    if( work->yesno_menu_strbuf_no ) GFL_STR_DeleteBuffer( work->yesno_menu_strbuf_no );
    GFL_MSG_Delete( work->yesno_menu_msgdata );
  }

  // STM_TEXT
  {
    if( work->stm_text_stream ) PRINTSYS_PrintStreamDelete( work->stm_text_stream );
    GFL_TCBL_Exit( work->stm_text_tcblsys );
    if( work->stm_text_strbuf ) GFL_STR_DeleteBuffer( work->stm_text_strbuf );
    GFL_MSG_Delete( work->stm_text_msgdata_shinka );
    GFL_MSG_Delete( work->stm_text_msgdata_wazaoboe );
    GFL_BG_FreeCharacterArea( BG_FRAME_M_STM_TEXT,
      GFL_ARCUTIL_TRANSINFO_GetPos(work->stm_text_winfrm_tinfo),
      GFL_ARCUTIL_TRANSINFO_GetSize(work->stm_text_winfrm_tinfo) );
    GFL_BMPWIN_Delete( work->stm_text_winin_bmpwin );
    GFL_BMPWIN_Delete( work->stm_text_dummy_bmpwin );
  }

  // SHINKA_DEMO_WORK��Еt��
  {
    PRINTSYS_QUE_Delete( work->print_que );
    GFL_FONT_Delete( work->font );
    SHINKADEMO_GRAPHIC_ExitBGSub( work->graphic );
    SHINKADEMO_GRAPHIC_Exit( work->graphic );
  }
  
  // �I�[�o�[���C
  GFL_OVERLAY_Unload( FS_OVERLAY_ID( ui_common ) );
}

//-------------------------------------
/// �T�E���h�又��
//=====================================
static void ShinkaDemo_SoundMain( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  switch( work->sound_step )
  {
  case SOUND_STEP_WAIT:
    {
    }
    break;
  case SOUND_STEP_PLAY_SHINKA:
    {
      PMSND_PlayBGM(SEQ_BGM_SHINKA);
      work->sound_step = SOUND_STEP_PLAYING_SHINKA;
    }
    break;
  case SOUND_STEP_PLAYING_SHINKA:
    {
    }
    break;
  case SOUND_STEP_PUSH_SHINKA:
    {
      PMSND_PushBGM();
      work->sound_step = SOUND_STEP_PLAYING_SHINKA;
    }
    break;
  case SOUND_STEP_POP_SHINKA:
    {
      PMSND_PopBGM();
      work->sound_step = SOUND_STEP_PLAYING_SHINKA;
    }
    break;
  case SOUND_STEP_PLAY_CONGRATULATE:
    {
      PMSND_PlayBGM(SEQ_ME_SHINKAOME);
      work->sound_step = SOUND_STEP_PLAYING_CONGRATULATE;
    }
    break;
  case SOUND_STEP_PLAYING_CONGRATULATE:
    {
      if( !PMSND_CheckPlayBGM() )
      {
        PMSND_PopBGM();
        work->sound_step = SOUND_STEP_PLAYING_SHINKA;
        
        if( work->sound_none )
        {
          PMSND_PauseBGM( TRUE );
        }
      }
    }
    break;
  case SOUND_STEP_PLAY_WAZAOBOE:
    {
      PMSND_PauseBGM( TRUE );
      PMSND_PlaySE( SEQ_SE_LVUP );  // btl_client.c����scProc_ACT_ExpLvup���Q�l�ɂ���
      work->sound_step = SOUND_STEP_PLAYING_WAZAOBOE;
    }
    break;
  case SOUND_STEP_PLAYING_WAZAOBOE:
    {
      if( !PMSND_CheckPlaySE() )
      {
        PMSND_PauseBGM( FALSE );
        work->sound_step = SOUND_STEP_PLAYING_SHINKA;
      }
    }
    break;
  case SOUND_STEP_FADE_OUT_SHINKA:
    {
      if( !(work->sound_none) )
        PMSND_FadeOutBGM( 16 * FADE_OUT_WAIT / 2 );
      work->sound_step = SOUND_STEP_FADING_OUT_SHINKA;
    }
    break;
  case SOUND_STEP_FADING_OUT_SHINKA:
    {
      if( !(work->sound_none) )
      {
        if( !PMSND_CheckFadeOnBGM() )
        {
          work->sound_step = SOUND_STEP_WAIT;
        }
      }
      else
        work->sound_step = SOUND_STEP_WAIT;
    }
    break;
  }
}

//-------------------------------------
/// �P��F�A�P��L������BG�𐶐�����
//=====================================
static void ShinkaDemo_CreateSimpleBG( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  // �p���b�g�̍쐬���]��
  {
    u16* pal = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(u16) * 0x10 );
    pal[0x00] = 0x0000;  // ����
    pal[0x01] = 0x0000;  // ��
    pal[0x02] = 0x7fff;  // ��
    pal[0x03] = 0x2108;  // �D

    GFL_BG_LoadPalette( BG_FRAME_M_BACK, pal, 0x20, BG_PAL_POS_M_BACK*0x20 );
    GFL_HEAP_FreeMemory( pal );
  }

  // �L�����̍쐬���]��
  {
    u32 sim_bmp_pos;
    u32 sim_bmp_size;

    GFL_BMP_DATA* sim_bmp = GFL_BMP_Create( 1, 1, GFL_BMP_16_COLOR, work->heap_id );
    GFL_BMP_Fill( sim_bmp, 0, 0, 8, 8, 0x03 );

    sim_bmp_size = GFL_BMP_GetBmpDataSize(sim_bmp);
    sim_bmp_pos = GFL_BG_LoadCharacterAreaMan(
                    BG_FRAME_M_BACK,
                    GFL_BMP_GetCharacterAdrs(sim_bmp),
                    sim_bmp_size );

    GF_ASSERT_MSG( sim_bmp_pos != AREAMAN_POS_NOTFOUND, "SHINKADEMO : BG�L�����̈悪����܂���ł����B\n" );  // gflib��arc_util.c��
    GF_ASSERT_MSG( sim_bmp_pos < 0xffff, "SHINKADEMO : BG�L�����̈ʒu���悭����܂���B\n" );                 // _TransVramBgCharacterAreaMan
    GF_ASSERT_MSG( sim_bmp_size < 0xffff, "SHINKADEMO : BG�L�����̃T�C�Y���悭����܂���B\n" );              // ���Q�l�ɂ����B

    work->sim_transinfo = GFL_ARCUTIL_TRANSINFO_Make( sim_bmp_pos, sim_bmp_size );
    
    GFL_BMP_Delete( sim_bmp );
  }

  // �X�N���[���̍쐬���]��
  {
    u16* sim_scr = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(u16) * 32*24 );
    u8 i, j;
    u16 h = 0;
    for(i=0; i<32; i++)
    {
      for(j=0; j<24; j++)
      {
        u16 chara_name = GFL_ARCUTIL_TRANSINFO_GetPos(work->sim_transinfo);
        u16 flip_h     = 0;
        u16 flip_v     = 0;
        u16 pal        = BG_PAL_POS_M_BACK;
        sim_scr[h] = ( pal << 12 ) | ( flip_v << 11 ) | ( flip_h << 10 ) | ( chara_name << 0 );
        
        h++;
      }
    }

    GFL_BG_WriteScreen( BG_FRAME_M_BACK, sim_scr, 0, 0, 32, 24 );
    GFL_BG_LoadScreenReq( BG_FRAME_M_BACK );

    GFL_HEAP_FreeMemory( sim_scr );
  }
}

//-------------------------------------
/// �P��F�A�P��L������BG��j������
//=====================================
static void ShinkaDemo_DeleteSimpleBG( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  GFL_BG_FreeCharacterArea( BG_FRAME_M_BACK,
                            GFL_ARCUTIL_TRANSINFO_GetPos(work->sim_transinfo),
                            GFL_ARCUTIL_TRANSINFO_GetSize(work->sim_transinfo) );
}

//-------------------------------------
/// �㉺�ɍ��т�\�����邽�߂�wnd
//=====================================
static void ShinkaDemo_InitWnd( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  work->wnd_up_y     =  20;
  work->wnd_down_y   = 130;

  GX_SetVisibleWnd( GX_WNDMASK_W0 | GX_WNDMASK_W1 );

  G2_SetWnd0Position(   0, work->wnd_up_y,      128, work->wnd_down_y );
  G2_SetWnd1Position( 128, work->wnd_up_y, 0/*256*/, work->wnd_down_y );

  G2_SetWnd0InsidePlane(
    GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ,
    TRUE );
  G2_SetWnd1InsidePlane(
    GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ,
    TRUE );

  G2_SetWndOutsidePlane(
    GX_WND_PLANEMASK_BG1,
    TRUE );
}
static void ShinkaDemo_ExitWnd( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  GX_SetVisibleWnd( GX_WNDMASK_NONE );
}
static void ShinkaDemo_MainWnd( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
}

//-------------------------------------
/// ������΂����o�̂��߂̃p���b�g�t�F�[�h
//=====================================
static void ShinkaDemo_ZeroPFade( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  work->pfade_tcbsys       = NULL;
  work->pfade_tcbsys_work  = NULL;
  work->pfade_ptr          = NULL;
}

static void ShinkaDemo_InitPFade( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )  // �p���b�g������������̂ŁA�S�p���b�g�̗p�ӂ��ς�ł���ĂԂ���
{
  ShinkaDemo_ZeroPFade( param, work );

  // �^�X�N
  work->pfade_tcbsys_work = GFL_HEAP_AllocClearMemory( work->heap_id, GFL_TCB_CalcSystemWorkSize(PFADE_TCBSYS_TASK_MAX) );
  work->pfade_tcbsys = GFL_TCB_Init( PFADE_TCBSYS_TASK_MAX, work->pfade_tcbsys_work );

  // �p���b�g�t�F�[�h
  work->pfade_ptr = PaletteFadeInit( work->heap_id );
  PaletteTrans_AutoSet( work->pfade_ptr, TRUE );
  PaletteFadeWorkAllocSet( work->pfade_ptr, FADE_MAIN_BG, 0x1e0, work->heap_id );
  PaletteFadeWorkAllocSet( work->pfade_ptr, FADE_MAIN_OBJ, 0x1e0, work->heap_id );

  // ����VRAM�ɂ���p���b�g���󂳂Ȃ��悤�ɁAVRAM����p���b�g���e���R�s�[����
  PaletteWorkSet_VramCopy( work->pfade_ptr, FADE_MAIN_BG, 0, 0x1e0 );
  PaletteWorkSet_VramCopy( work->pfade_ptr, FADE_MAIN_OBJ, 0, 0x1e0 );

  // �X�e�b�v
  work->pfade_step = 0;
}

static void ShinkaDemo_ExitPFade( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  // �p���b�g�t�F�[�h
  PaletteFadeWorkAllocFree( work->pfade_ptr, FADE_MAIN_BG );
  PaletteFadeWorkAllocFree( work->pfade_ptr, FADE_MAIN_OBJ );
  PaletteFadeFree( work->pfade_ptr );

  // �^�X�N
  GFL_TCB_Exit( work->pfade_tcbsys );
  GFL_HEAP_FreeMemory( work->pfade_tcbsys_work );

  ShinkaDemo_ZeroPFade( param, work );
}

static void ShinkaDemo_MainPFade( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  // �^�X�N
  if( work->pfade_tcbsys ) GFL_TCB_Main( work->pfade_tcbsys );

  // �X�e�b�v
  switch( work->pfade_step )
  {
  case 0:
    {
    }
    break;
  case 1:
    {
      ShinkaDemo_ToWhitePFade( param, work );
      work->pfade_step++;
    }
    break;
  case 2:
    {
      if( !ShinkaDemo_CheckPFade( param, work ) )
        work->pfade_step++;
    }
    break;
  case 3:
    {
      ShinkaDemo_FromWhitePFade( param, work );
      work->pfade_step++;
    }
    break;
  case 4:
    {
      if( !ShinkaDemo_CheckPFade( param, work ) )
        work->pfade_step = 0;
    }
    break;
  }
}

static void ShinkaDemo_ToWhitePFade( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  // ������΂�
  u16 req_bit = (1<<BG_PAL_POS_M_BACKDROP) | (1<<BG_PAL_POS_M_BACK);
  PaletteFadeReq(
    work->pfade_ptr, PF_BIT_MAIN_BG, req_bit, PFADE_WAIT_TO_WHITE, 0, 16, 0x7fff, work->pfade_tcbsys
  );
}

static void ShinkaDemo_FromWhitePFade( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  // ������߂�
  u16 req_bit = (1<<BG_PAL_POS_M_BACKDROP) | (1<<BG_PAL_POS_M_BACK);
  PaletteFadeReq(
    work->pfade_ptr, PF_BIT_MAIN_BG, req_bit, PFADE_WAIT_FROM_WHITE, 16, 0, 0x7fff, work->pfade_tcbsys
  );
}

static BOOL ShinkaDemo_CheckPFade( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )  // TRUE�������AFALSE�I��
{
  if( PaletteFadeCheck(work->pfade_ptr) == 0 )
    return FALSE;
  else
    return TRUE;
}

static void ShinkaDemo_ToFromWhitePFade( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  work->pfade_step = 1;
}

//-------------------------------------
/// VBlank�֐�
//=====================================
static void ShinkaDemo_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  SHINKA_DEMO_WORK* work = (SHINKA_DEMO_WORK*)wk;

  // �o�g���X�e�[�^�X
  {
    // �p���b�g�t�F�[�h
    if( work->pfd ) PaletteFadeTrans( work->pfd );
  }

  // ������΂����o�̂��߂̃p���b�g�t�F�[�h
  {
    // �p���b�g�t�F�[�h
    if( work->pfade_ptr ) PaletteFadeTrans( work->pfade_ptr );
  }
}

//-------------------------------------
/// �u�����������@���ꂢ�ɂ킷�ꂽ�I�v���b�Z�[�W�̃R�[���o�b�N�֐�
//=====================================
static BOOL ShinkaDemo_StmTextStreamForgetCallBack( u32 arg )
{
  switch( arg )
  {
  case 3:  // "�|�J��"
    {
      PMSND_PlaySE( SEQ_SE_KON );
    }
    break;
  case 5:  // "�|�J��"��SE�I���҂�
    {
      return PMSND_CheckPlaySE();
    }
    break;
  }

  return FALSE;
}

//-------------------------------------
/// STM_TEXT�̃X�g���[���쐬
//=====================================
static void ShinkaDemo_MakeStmTextStream( SHINKA_DEMO_WORK* work,
                                          GFL_MSGDATA* msgdata, u32 str_id, pPrintCallBack callback,
                                          TAG_REGIST_TYPE type0, const void* data0,
                                          TAG_REGIST_TYPE type1, const void* data1 )
{
  // ��U����
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->stm_text_winin_bmpwin), BTM_TEXT_WININ_BACK_COLOR );
  
  // �O�̂�����
  if( work->stm_text_stream ) PRINTSYS_PrintStreamDelete( work->stm_text_stream );  // ���̊֐��Ń^�X�N���폜���Ă����̂ŁA�����ɓ����^�X�N��1�ōς�
  if( work->stm_text_strbuf ) GFL_STR_DeleteBuffer( work->stm_text_strbuf );

  // ������쐬
  work->stm_text_strbuf = MakeStr( work->heap_id,
                                   msgdata, str_id,
                                   type0, data0,
                                   type1, data1 );

  // �X�g���[���J�n
  work->stm_text_stream = PRINTSYS_PrintStreamCallBack(
                              work->stm_text_winin_bmpwin,
                              0, 0,
                              work->stm_text_strbuf,
                              work->font, MSGSPEED_GetWait(),
                              work->stm_text_tcblsys, 2,
                              work->heap_id,
                              BTM_TEXT_WININ_BACK_COLOR,
                              callback );
}

//-------------------------------------
/// STM_TEXT�̃X�g���[���҂�
//=====================================
static BOOL ShinkaDemo_WaitStmTextStream( SHINKA_DEMO_WORK* work )
{
  BOOL ret = FALSE;

  switch( PRINTSYS_PrintStreamGetState( work->stm_text_stream ) )
  { 
  case PRINTSTREAM_STATE_PAUSE:
    if( ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ) || GFL_UI_TP_GetTrg() )
    { 
      PRINTSYS_PrintStreamReleasePause( work->stm_text_stream );
    }
    break;
  case PRINTSTREAM_STATE_DONE:
    {
      ret = TRUE;
    }
    break;
  }
  
  return ret;
}

//-------------------------------------
/// YESNO_MENU�̕�����쐬
//=====================================
static void ShinkaDemo_MakeYesNoMenuStrbuf( SHINKA_DEMO_WORK* work,
                                            GFL_MSGDATA* yes_msgdata, u32 yes_str_id,
                                            TAG_REGIST_TYPE yes_type0, const void* yes_data0,
                                            TAG_REGIST_TYPE yes_type1, const void* yes_data1,
                                            GFL_MSGDATA* no_msgdata, u32 no_str_id,
                                            TAG_REGIST_TYPE no_type0, const void* no_data0,
                                            TAG_REGIST_TYPE no_type1, const void* no_data1 )
{
  // �O�̂�����
  if( work->yesno_menu_strbuf_yes ) GFL_STR_DeleteBuffer( work->yesno_menu_strbuf_yes );
  if( work->yesno_menu_strbuf_no  ) GFL_STR_DeleteBuffer( work->yesno_menu_strbuf_no  );

  // ������쐬
  work->yesno_menu_strbuf_yes = MakeStr(
                                  work->heap_id,
                                  yes_msgdata, yes_str_id,
                                  yes_type0, yes_data0,
                                  yes_type1, yes_data1 );
  work->yesno_menu_strbuf_no  = MakeStr(
                                  work->heap_id,
                                  no_msgdata, no_str_id,
                                  no_type0, no_data0,
                                  no_type1, no_data1 );
}

//-------------------------------------
/// ������쐬
//=====================================
static STRBUF* MakeStr( HEAPID heap_id,
                        GFL_MSGDATA* msgdata, u32 str_id,
                        TAG_REGIST_TYPE type0, const void* data0,
                        TAG_REGIST_TYPE type1, const void* data1 )
{
  STRBUF* strbuf;

  if(    type0 == TAG_REGIST_TYPE_NONE
      && type1 == TAG_REGIST_TYPE_NONE )
  {
    strbuf = GFL_MSG_CreateString( msgdata, str_id );
  }
  else
  {
    typedef struct
    {
      TAG_REGIST_TYPE type;
      const void*     data;
    }
    TYPE_DATA_SET;
    TYPE_DATA_SET type_data_set[2];

    STRBUF* src_strbuf = GFL_MSG_CreateString( msgdata, str_id );
    WORDSET* wordset = WORDSET_Create( heap_id );

    u32 i;

    type_data_set[0].type = type0;
    type_data_set[0].data = data0;
    type_data_set[1].type = type1;
    type_data_set[1].data = data1;

    for(i=0; i<2; i++)
    {
      switch( type_data_set[i].type )
      {
      case TAG_REGIST_TYPE_WORD:
        {
          WORDSET_RegisterWord( wordset, i, type_data_set[i].data, 0, TRUE, 0 );  // �|�P�����̃j�b�N�l�[�������n���\�肪�Ȃ��̂ŁA�p�����[�^�͌Œ�l�ɂ��Ă��܂�
        }
        break;
      case TAG_REGIST_TYPE_POKE_MONS_NAME:
        {
          WORDSET_RegisterPokeMonsName( wordset, i, type_data_set[i].data );
        }
        break;
      case TAG_REGIST_TYPE_POKE_NICK_NAME:
        {
          WORDSET_RegisterPokeNickName( wordset, i, type_data_set[i].data );
        }
        break;
      case TAG_REGIST_TYPE_WAZA:
        {
          WORDSET_RegisterWazaName( wordset, i, *((WazaID*)(type_data_set[i].data)) );
        }
        break;
      default:
        break;
      }
    }
    
    strbuf = GFL_STR_CreateBuffer( STRBUF_LENGTH, heap_id );
    WORDSET_ExpandStr( wordset, strbuf, src_strbuf );
    
    GFL_STR_DeleteBuffer( src_strbuf );
    WORDSET_Delete( wordset );
  }

  return strbuf;
}

