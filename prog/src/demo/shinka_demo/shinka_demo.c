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

//#define NOT_USE_STATUS_BATTLE  // ���ꂪ��`����Ă���Ƃ��A�o�g���X�e�[�^�X���g��Ȃ�


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
#include "gamesystem/game_data.h"
#include "savedata/record.h"
#include "savedata/mail.h"
#include "item/itemsym.h"
#include "poke_tool/shinka_check.h"
#include "print/global_msg.h"
#include "app/app_keycursor.h"

#include "ui/yesno_menu.h"

#include "savedata/save_control.h"
#include "app/p_status.h"

#include "sound/wb_sound_data.sadl"
#include "sound/sound_manager.h"
#include "sound/pm_sndsys.h"
#include "../../field/field_sound.h"

#include "shinka_demo_graphic.h"
#include "shinka_demo_view.h"
#include "shinka_demo_effect.h"


// �A�[�J�C�u
#include "shinka_demo_particle.naix"


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
#define BG_FRAME_M_POKEMON           (GFL_BG_FRAME0_M)        // �v���C�I���e�B2
#define BG_FRAME_M_BELT              (GFL_BG_FRAME2_M)        // �v���C�I���e�B1
#define BG_FRAME_M_TEXT              (GFL_BG_FRAME1_M)        // �v���C�I���e�B0

#define BG_FRAME_S_BACK              (GFL_BG_FRAME0_S)        // �v���C�I���e�B2
#define BG_FRAME_S_BUTTON            (GFL_BG_FRAME1_S)        // �v���C�I���e�B1
#define BG_FRAME_S_TEXT              (GFL_BG_FRAME2_S)        // �v���C�I���e�B0

// BG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_M_POKEMON    (2)
#define BG_FRAME_PRI_M_BELT       (1)
#define BG_FRAME_PRI_M_TEXT       (0)

#define BG_FRAME_PRI_S_BACK       (2)
#define BG_FRAME_PRI_S_BUTTON     (1)
#define BG_FRAME_PRI_S_TEXT       (0)

// BG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_M_BELT          = 1,
  BG_PAL_NUM_M_TEXT_FONT     = 1,
  BG_PAL_NUM_M_TEXT_FRAME    = 1,
};
// �ʒu
enum
{
  BG_PAL_POS_M_BELT          = 0,
  BG_PAL_POS_M_TEXT_FONT     = 1,
  BG_PAL_POS_M_TEXT_FRAME    = 2,
  BG_PAL_POS_M_MAX           = 3,  // ���������
};
// �{��
enum
{
  BG_PAL_NUM_S_BACK          = 1,
  BG_PAL_NUM_S_BUTTON        = 2,
  BG_PAL_NUM_S_TEXT          = 1,
};
// �ʒu
enum
{
  BG_PAL_POS_S_BACK          = 0,
  BG_PAL_POS_S_BUTTON        = 1, 
  BG_PAL_POS_S_TEXT          = 3, 
  BG_PAL_POS_S_MAX           = 4,  // ���������
};

// OBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_M_BELT         = 1,
};
// �ʒu
enum
{
  OBJ_PAL_POS_M_BELT         = 0,
  OBJ_PAL_POS_M_MAX          = 1,  // ���������
};
// �{��
enum
{
  OBJ_PAL_NUM_S_CURSOR        = 2,
};
// �ʒu
enum
{
  OBJ_PAL_POS_S_CURSOR        = 0,
  OBJ_PAL_POS_S_MAX           = 2,  // ���������
};

// OBJ��BG�v���C�I���e�B
#define OBJ_BG_PRI_M_BELT   (BG_FRAME_PRI_M_BELT)

// OBJ
enum
{
  OBJ_CELL_BELT_UP_01,
  OBJ_CELL_BELT_UP_02,
  OBJ_CELL_BELT_UP_03,
  OBJ_CELL_BELT_DOWN_01,
  OBJ_CELL_BELT_DOWN_02,
  OBJ_CELL_BELT_DOWN_03,
  OBJ_CELL_MAX,
};
enum
{
  OBJ_RES_BELT_NCG,
  OBJ_RES_BELT_NCL,
  OBJ_RES_BELT_NCE,
  OBJ_RES_MAX,
};
static const GFL_CLWK_DATA obj_cell_data[OBJ_CELL_MAX] =
{
  { 128, 96, 0, 0, OBJ_BG_PRI_M_BELT },
  { 128, 96, 1, 0, OBJ_BG_PRI_M_BELT },
  { 128, 96, 2, 0, OBJ_BG_PRI_M_BELT },
  { 128, 96, 3, 0, OBJ_BG_PRI_M_BELT },
  { 128, 96, 4, 0, OBJ_BG_PRI_M_BELT },
  { 128, 96, 5, 0, OBJ_BG_PRI_M_BELT },
};

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

#define TEXT_WININ_BACK_COLOR        (15)
#define TEXT_WININ_ZERO_COLOR        (0)

// �t�F�[�h
#define FADE_IN_WAIT           (-16)       ///< �t�F�[�h�C���̃X�s�[�h  // BG��OBJ��BELT���������Ă���̂Ńt�F�[�h�C���͈�u�ōs���̂Ń}�C�i�X
#define FADE_OUT_WAIT          (2)         ///< �t�F�[�h�A�E�g�̃X�s�[�h

#define STATUS_FADE_OUT_WAIT   (0)         ///< �Z�I���ֈڍs���邽�߂̃t�F�[�h�A�E�g�̃X�s�[�h
#define STATUS_FADE_IN_WAIT    (0)         ///< �Z�I������߂��Ă��邽�߂̃t�F�[�h�C���̃X�s�[�h

// �o�g���X�e�[�^�X�p�^�X�N
#define TCBSYS_TASK_MAX        (8)

// �X�e�b�v
typedef enum
{
  STEP_FADE_IN_BEFORE,              // �t�F�[�h�C���J�n�҂�
  STEP_FADE_IN,                     // �t�F�[�h�C����
  STEP_BELT_OPEN,                   // �x���g�J����
  STEP_TEXT_SIGN,                   // �u����I�H�@�悤�����c�c�I�v�\����
  STEP_DEMO_CRY,                    // �f������
  STEP_BGM_INTRO,                   // �C���g��BGM�Đ���
  STEP_DEMO_CHANGE,                 // �f���ω���  // �i��BGM�Đ��J�n  // �i��BGM�Đ����f

  // �i�������ꍇ
  STEP_BGM_CONGRATULATE,            // ���߂łƂ�BGM�Đ��J�n
  STEP_TEXT_CONGRATULATE,           // �u���߂łƂ��I�@���񂩂����I�v�\����

  // �i���L�����Z�������ꍇ
  STEP_BGM_CANCEL_SHINKA_POP,       // �i��BGM��POP���čĐ��J�n
  STEP_TEXT_CANCEL,                 // �u����c�c�H�@�ւ񂩂��@�Ƃ܂����I�v�\����

  // �i����������������
  STEP_SHINKA_COND_CHECK,           // �i����������������

  // �Z�o��
  STEP_WAZA_OBOE,                   // �Z�o���`�F�b�N
  STEP_WAZA_VACANT_MASTER,          // �u�����炵���@���ڂ����I�v�\����(�Z���R�ȉ��������ꍇ)
  STEP_WAZA_FULL_PREPARE,           // ����Q�̏���
  STEP_WAZA_FULL_Q,                 // �u�ق��́@�킴���@�킷�ꂳ���܂����H�v�\����
  STEP_WAZA_FULL_YN,                // �I��

  // �t�B�[���h�Z�I��
  STEP_WAZA_STATUS_FIELD_FADE_OUT,  // �Z�I���Ɉڍs���邽�߂̃t�F�[�h�A�E�g(�t�B�[���h)
  STEP_WAZA_STATUS_FIELD,           // �Z�I��PROC(�t�B�[���h)
  STEP_WAZA_STATUS_FIELD_OUT,       // �Z�I��PROC�𔲂�����̏���(�t�B�[���h)
  STEP_WAZA_STATUS_FIELD_FADE_IN,   // �Z�I������߂��Ă��邽�߂̃t�F�[�h�C��(�t�B�[���h)

  // �o�g���Z�I��
  STEP_WAZA_STATUS_BATTLE_FADE_OUT, // �Z�I���Ɉڍs���邽�߂̃t�F�[�h�A�E�g(�o�g��)
  STEP_WAZA_STATUS_BATTLE,          // �Z�I���̃t�F�[�h�C�����Z�I��(�o�g��)
  STEP_WAZA_STATUS_BATTLE_OUT,      // �Z�I���̃t�F�[�h�A�E�g(�o�g��)
  STEP_WAZA_STATUS_BATTLE_FADE_IN,  // �Z�I������߂��Ă��邽�߂̃t�F�[�h�C��(�o�g��)

  // �Z�Y��
  STEP_WAZA_CONFIRM_PREPARE,        // ����Q�̏���
  STEP_WAZA_FORGET,                 // �u�����������@���ꂢ�ɂ킷�ꂽ�I�v�\����
  STEP_WAZA_FORGET_MASTER,          // �u�����炵���@���ڂ����I�v�\����
  STEP_WAZA_ABANDON_PREPARE,        // ����Q�̏���
  STEP_WAZA_ABANDON_Q,              // �u���ڂ���̂��@������߂܂����H�v�\����
  STEP_WAZA_ABANDON_YN,             // �I��
  STEP_WAZA_NOT_MASTER,             // �u���ڂ����Ɂ@��������I�v�\����

  STEP_FADE_OUT_START,              // �t�F�[�h�A�E�g�J�n
  STEP_FADE_OUT_WAIT,               // �t�F�[�h�A�E�g����BGM�̃t�F�[�h�A�E�g�����҂�
  STEP_FADE_OUT_END,                // �t�F�[�h�A�E�g���ŉ�ʂ̃t�F�[�h�A�E�g�����҂�
  STEP_END,
}
STEP;

// �T�E���h�X�e�b�v
typedef enum
{
  SOUND_STEP_WAIT,
  SOUND_STEP_FIELD_FADE_OUT,
  SOUND_STEP_INTRO,
  SOUND_STEP_SHINKA,
  SOUND_STEP_SHINKA_PUSH,
  SOUND_STEP_SHINKA_FADE_OUT,
  SOUND_STEP_CONGRATULATE_LOAD,
  SOUND_STEP_CONGRATULATE,
  SOUND_STEP_WAZAOBOE,
}
SOUND_STEP;

/*
�T�E���h�q�[�v������Ȃ��̂Ŗ���
// �T�E���h�f�[�^�v���Z�b�g�e�[�u��
static const u32 preset_sound_table[] =
{
//  SEQ_BGM_SHINKA,
//  SEQ_BGM_KOUKAN,
  SEQ_ME_SHINKAOME,
};
*/


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
  // �O���t�B�b�N�A�t�H���g�Ȃ�
  HEAPID                      heap_id;
  SHINKADEMO_GRAPHIC_WORK*    graphic;
  GFL_FONT*                   font;
  PRINT_QUE*                  print_que;

  // �|�P����
  POKEMON_PARAM*              pp;
  // (�Â�)�j�b�N�l�[��
  STRBUF*                     poke_nick_name_strbuf;  // �|�P�����̎푰�����j�b�N�l�[���̏ꍇ�A�i����Ƀj�b�N�l�[�����ύX�����̂ŁB
  // �Y���Z
  WazaID                      wazawasure_no;
  u8                          wazawasure_pos;  // �Y���Z�̈ʒu
  // �o����Z
  WazaID                      wazaoboe_no;
  int                         wazaoboe_index;

  // �i���L�����Z��
  BOOL                        evo_cancel;  // �i���L�����Z�������Ƃ���TRUE

  // �X�e�b�v
  STEP                        step;
  u32                         wait_count;
  // �T�E���h�X�e�b�v
  SOUND_STEP                  sound_step;
/*
�T�E���h�q�[�v������Ȃ��̂Ŗ��� 
  SOUNDMAN_PRESET_HANDLE*     sound_preset_handle;  // �T�E���h�v���Z�b�g�p�n���h��
*/
  u32                         sound_div_seq;  // BGM�̕������[�h�̃V�[�P���X

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // OBJ
  u32                         obj_res[OBJ_RES_MAX];
  GFL_CLWK*                   obj_clwk[OBJ_CELL_MAX];

  // TEXT
  PRINT_STREAM*               text_stream;
  GFL_TCBLSYS*                text_tcblsys;
  GFL_BMPWIN*                 text_winin_bmpwin;
  GFL_BMPWIN*                 text_dummy_bmpwin;      ///< 0�Ԃ̃L�����N�^�[��1x1�ł����Ă���
  GFL_ARCUTIL_TRANSINFO       text_winfrm_tinfo;
  GFL_MSGDATA*                text_msgdata_shinka;
  GFL_MSGDATA*                text_msgdata_wazaoboe;
  STRBUF*                     text_strbuf;
  APP_KEYCURSOR_WORK*         text_keycursor_wk;  // ���b�Z�[�W����L�[�J�[�\���A�C�R��

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
  // �i���f���̃p�[�e�B�N���Ɣw�i
  SHINKADEMO_EFFECT_WORK*     efwk;

  // SE
  BOOL                        se_play;        // �f���̃��C���������Đ�����SE��炵�Ă������ǂ����̃t���O
  BOOL                        se_to_white;    // ������΂��Ƃ���SE��2�x�炳�Ȃ��悤�ɂ��邽�߂̃t���O
  BOOL                        se_from_white;  // ������߂�Ƃ���SE��2�x�炳�Ȃ��悤�ɂ��邽�߂̃t���O

  // ���[�J��PROC�V�X�e��
  GFL_PROCSYS*  local_procsys;
}
SHINKA_DEMO_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// ��PROC�֑J�ڂ���̂ŁA���ł�Init��Exit���ł���悤�ɂ��Ă���
static void ShinkaDemo_Init( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_Exit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );

// �T�E���h
static void ShinkaDemo_SoundInit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_SoundExit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_SoundMain( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );

static BOOL ShinkaDemo_SoundCheckFadeOutField( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_SoundFadeInField( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_SoundPlayIntro( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static BOOL ShinkaDemo_SoundCheckPlayIntro( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_SoundPlayShinka( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_SoundPushShinka( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_SoundPopShinka( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static BOOL ShinkaDemo_SoundCheckPlayShinka( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_SoundFadeOutShinka( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static BOOL ShinkaDemo_SoundCheckFadeOutShinka( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_SoundPlayCongratulate( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static BOOL ShinkaDemo_SoundCheckPlayCongratulate( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_SoundPlayWazaoboe( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );

// BG
static void ShinkaDemo_BgInit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_BgExit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );

// OBJ
static void ShinkaDemo_ObjInit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_ObjInitAfterEvolution( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_ObjExit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_ObjStartAnime( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static BOOL ShinkaDemo_ObjIsEndAnime( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );

// VBlank�֐�
static void ShinkaDemo_VBlankFunc( GFL_TCB* tcb, void* wk );

// �e�L�X�g
static void ShinkaDemo_TextInit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_TextExit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_TextMain( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static BOOL ShinkaDemo_TextStreamForgetCallBack( u32 arg );
static void ShinkaDemo_MakeTextStream( SHINKA_DEMO_WORK* work,
                                       GFL_MSGDATA* msgdata, u32 str_id, pPrintCallBack callback,
                                       TAG_REGIST_TYPE type0, const void* data0,
                                       TAG_REGIST_TYPE type1, const void* data1 );
static BOOL ShinkaDemo_WaitTextStream( SHINKA_DEMO_WORK* work );
static BOOL ShinkaDemo_TextWaitInput( SHINKA_DEMO_WORK* work );
static void ShinkaDemo_TextWininClear( SHINKA_DEMO_WORK* work, u8 col_code );
static void ShinkaDemo_TextWinfrmShow( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work, BOOL is_on );

// YESNO_MENU
static void ShinkaDemo_YesNoMenuInit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_YesNoMenuExit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );
static void ShinkaDemo_MakeYesNoMenuStrbuf( SHINKA_DEMO_WORK* work,
                                            GFL_MSGDATA* yes_msgdata, u32 yes_str_id,
                                            TAG_REGIST_TYPE yes_type0, const void* yes_data0,
                                            TAG_REGIST_TYPE yes_type1, const void* yes_data1,
                                            GFL_MSGDATA* no_msgdata, u32 no_str_id,
                                            TAG_REGIST_TYPE no_type0, const void* no_data0,
                                            TAG_REGIST_TYPE no_type1, const void* no_data1 );

// ������쐬
static STRBUF* MakeStr( HEAPID heap_id,
                        GFL_MSGDATA* msgdata, u32 str_id,
                        TAG_REGIST_TYPE type0, const void* data0,
                        TAG_REGIST_TYPE type1, const void* data1 );

// �i����������������
static void ShinkaDemo_ShinkaCondCheckSpecialLevelup( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work );


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
    work->step          = STEP_FADE_IN_BEFORE;
    work->wait_count    = 5;  // �ŏ���ʂ������̂ŁA���΂��^���Â̂܂ܑ҂�
    // �T�E���h�X�e�b�v
    work->sound_step    = SOUND_STEP_WAIT;
  }

  // �|�P����
  {
    // �|�P����
    work->pp            = PokeParty_GetMemberPointer( param->ppt, param->shinka_pos );
    // (�Â�)�j�b�N�l�[��
    work->poke_nick_name_strbuf = GFL_STR_CreateBuffer( STRBUF_LENGTH, work->heap_id );
    PP_Get( work->pp, ID_PARA_nickname, work->poke_nick_name_strbuf );
  }

  // �i���L�����Z��
  {
    work->evo_cancel = FALSE;
  }

  // �o�g���X�e�[�^�X
  {
    work->tcbsys           = NULL;
    work->tcbsys_work      = NULL;
    work->pfd              = NULL;
    work->cursor_flag      = 0;
  }

  // �t�B�[���h�X�e�[�^�X
  {
    work->psData      = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(PSTATUS_DATA) );
  }

  // �t�F�[�h�C��(������)
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 16, FADE_IN_WAIT );

  // �T�E���h
  ShinkaDemo_SoundInit( param, work );

  // VBlank��TCB
  {
    work->vblank_tcb = GFUser_VIntr_CreateTCB( ShinkaDemo_VBlankFunc, work, 1 );
  }

  // SE
  work->se_play       = FALSE;
  work->se_to_white   = FALSE;
  work->se_from_white = FALSE;

  // ���[�J��PROC�V�X�e�����쐬
  work->local_procsys = GFL_PROC_LOCAL_boot( work->heap_id );

  // ����������
  ShinkaDemo_Init( param, work );


#ifdef NOT_USE_STATUS_BATTLE
  {
    // �o�g���X�e�[�^�X���g��Ȃ��悤�ɂ���
    param->b_field  = TRUE;
  }
#endif


  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static GFL_PROC_RESULT ShinkaDemoProcExit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  SHINKA_DEMO_PARAM*    param    = (SHINKA_DEMO_PARAM*)pwk;
  SHINKA_DEMO_WORK*     work     = (SHINKA_DEMO_WORK*)mywk;

  // �I������
  ShinkaDemo_Exit( param, work );

  // ���[�J��PROC�V�X�e����j��
  GFL_PROC_LOCAL_Exit( work->local_procsys ); 

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �T�E���h
  ShinkaDemo_SoundExit( param, work );

  // �t�B�[���h�X�e�[�^�X
  {
    GFL_HEAP_FreeMemory( work->psData );
  }   

  // �o�g���X�e�[�^�X
  {
    // ProcMain���ōς܂��Ă���̂ŁA�����ł͓��ɂ�邱�ƂȂ�
  }

  // �|�P����
  {
    // (�Â�)�j�b�N�l�[��
    GFL_STR_DeleteBuffer( work->poke_nick_name_strbuf );
  }

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

  // ���[�J��PROC�̍X�V����
  GFL_PROC_MAIN_STATUS  local_proc_status   =  GFL_PROC_LOCAL_Main( work->local_procsys );
  if( local_proc_status == GFL_PROC_MAIN_VALID ) return GFL_PROC_RES_CONTINUE;

  switch( work->step )
  {
  //---------------------------
  case STEP_FADE_IN_BEFORE:
    {
      if( work->wait_count == 0 )
      {
        // ����
        work->step = STEP_FADE_IN;

        // �t�F�[�h�C��(����������)
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, FADE_IN_WAIT );
      }
      else
      {
        work->wait_count--;
      }
    }
    break;
  case STEP_FADE_IN:
    {
      if( !GFL_FADE_CheckFade() )
      {
        // ����
        work->step = STEP_BELT_OPEN;

        ShinkaDemo_ObjStartAnime( param, work );
      }
    }
    break;
  case STEP_BELT_OPEN:
    {
      if(    ( !ShinkaDemo_SoundCheckFadeOutField( param, work ) )
          && ShinkaDemo_ObjIsEndAnime( param, work ) )
      {
        // ����
        work->step = STEP_TEXT_SIGN;

        ShinkaDemo_TextWinfrmShow( param, work, TRUE );
        ShinkaDemo_MakeTextStream(
            work,
            work->text_msgdata_shinka, SHINKADEMO_ShinkaBeforeMsg, NULL,
            TAG_REGIST_TYPE_POKE_NICK_NAME, work->pp,
            TAG_REGIST_TYPE_NONE, NULL );
      }
    }
    break;
  case STEP_TEXT_SIGN:
    {
      if( ShinkaDemo_WaitTextStream( work ) )
      {
        if( ShinkaDemo_TextWaitInput( work ) )
        {
          ShinkaDemo_TextWininClear( work, TEXT_WININ_ZERO_COLOR );
          ShinkaDemo_TextWinfrmShow( param, work, FALSE );

          // ����
          work->step = STEP_DEMO_CRY;
        
          // �i���f�����J�n
          SHINKADEMO_VIEW_CryStart( work->view );
        }
      }
    }
    break;
  case STEP_DEMO_CRY:
    {
      // �i���f�������I��������
      if( SHINKADEMO_VIEW_CryIsEnd( work->view ) )
      {
        // ����
        work->step = STEP_BGM_INTRO;

        ShinkaDemo_SoundPlayIntro( param, work );
      }
    }
    break;
  case STEP_BGM_INTRO:
    {
      if( !ShinkaDemo_SoundCheckPlayIntro( param, work ) )
      {
        // ����
        work->step = STEP_DEMO_CHANGE;
        
        // �i���f���ω��J�n
        SHINKADEMO_VIEW_ChangeStart( work->view );
        // �i���f���̃p�[�e�B�N���Ɣw�i
        SHINKADEMO_EFFECT_Start( work->efwk );

        //PMSND_PlaySE( SEQ_SE_SHDEMO_01 );
        PMSND_PlaySE( SEQ_SE_EDEMO_01 );
        work->se_play = TRUE;
        work->wait_count = 0;
      }
    }
    break;
  case STEP_DEMO_CHANGE:
    {
      int key_trg = GFL_UI_KEY_GetTrg();
  
      // �i���f���ω����I��������
      if( SHINKADEMO_VIEW_ChangeIsEnd( work->view ) )
      {
        if( work->evo_cancel )
        {
          // �i���L�����Z�������ꍇ 
          work->step = STEP_BGM_CANCEL_SHINKA_POP;
        }
        else
        {
          // �i�������ꍇ
          work->step = STEP_BGM_CONGRATULATE;
        }
      }
      else
      {
        // �i��BGM�Đ��J�n���邩
        if( SHINKADEMO_VIEW_ChangeIsBgmShinkaStart( work->view ) )
        {
          ShinkaDemo_SoundPlayShinka( param, work ); 
        }
        // �i��BGM�Đ����f���邩
        if( SHINKADEMO_VIEW_ChangeIsBgmShinkaPush( work->view ) )
        {
          ShinkaDemo_SoundPushShinka( param, work );
        }
        // ��ʂ𔒂���΂���
        if( SHINKADEMO_VIEW_ChangeIsToWhite( work->view ) )
        {
          SHINKADEMO_EFFECT_StartWhite( work->efwk );

          if( !(work->se_to_white) )
          {
            PMSND_PlaySE( SEQ_SE_SHDEMO_04 );
            work->se_play = FALSE;
            work->se_to_white = TRUE;
          }
        }
        // INDEPENDENT��MCSS�ɓ���ւ��X�^�[�g
        if( SHINKADEMO_EFFECT_IsWhite( work->efwk ) )
        {
          SHINKADEMO_VIEW_ChangeReplaceStart( work->view );
        }
        // �|�P�����𔒂���߂��̂��X�^�[�g
        if( SHINKADEMO_EFFECT_IsFromWhite( work->efwk) )
        {
          SHINKADEMO_VIEW_ChangeFromWhiteStart( work->view );
          
          if( !(work->se_from_white) )
          {
            // �i�������ꍇ
            if( !(work->evo_cancel) )
            {
              work->se_play = TRUE;
            }
            work->se_from_white = TRUE;
          }
        }

        // SE
        if( work->se_play )
        {
          if( work->wait_count == 90 )
          {
            PMSND_PlaySE( SEQ_SE_SHDEMO_02 );
          }
          else if(    work->wait_count == 530
                   || work->wait_count == 585
                   || work->wait_count == 640
          )
          {
            //PMSND_PlaySE( SEQ_SE_SHDEMO_03 );
            PMSND_PlaySE( SEQ_SE_EDEMO_01 );
          }
          // ���̊ԏ���wait_count�͎~�܂��Ă��܂�
          else if( work->wait_count == 740 )
          {
            PMSND_PlaySE( SEQ_SE_SHDEMO_05 );
          }
          work->wait_count++;
        }

        // �i���L�����Z��������
        if(    ( param->b_enable_cancel )
            && ShinkaDemo_SoundCheckPlayShinka( param, work )
            && ( !(work->evo_cancel) ) )
        {
          if( key_trg & PAD_BUTTON_B )
          {
            BOOL success_cancel = SHINKADEMO_VIEW_ChangeCancel( work->view );
            // �i���L�����Z������
            if( success_cancel )
            {
              work->evo_cancel = TRUE;

              // �^�b�`or�L�[
              GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );

              // �i���f���̃p�[�e�B�N���Ɣw�i
              SHINKADEMO_EFFECT_Cancel( work->efwk );
            }
            // �i���L�����Z�����s
            else
            {
              // �������Ȃ�
            }
          }
        }
      }
    }
    break;

  //---------------------------
  // �i�������ꍇ
  case STEP_BGM_CONGRATULATE:
    {
      // POKEMON_PARAM��i��������
      {
        // �i��
        PP_ChangeMonsNo( work->pp, param->after_mons_no );

        // �}�ӓo�^�i�߂܂����j
        {
          if( param->gamedata )
          {
            ZUKAN_SAVEDATA* zukan_savedata = GAMEDATA_GetZukanSave( param->gamedata );
            ZUKANSAVE_SetPokeSee( zukan_savedata, work->pp );  // ����  // �}�Ӄt���O���Z�b�g����
            ZUKANSAVE_SetPokeGet( zukan_savedata, work->pp );  // �߂܂���  // �}�Ӄt���O���Z�b�g����
          }
        }
      }

      ShinkaDemo_SoundPlayCongratulate( param, work );

      ShinkaDemo_TextWinfrmShow( param, work, TRUE );
      ShinkaDemo_MakeTextStream(
          work,
          work->text_msgdata_shinka, SHINKADEMO_ShinkaMsg, NULL,
          TAG_REGIST_TYPE_WORD, work->poke_nick_name_strbuf,
          TAG_REGIST_TYPE_POKE_MONS_NAME, work->pp );

      // ����
      work->step = STEP_TEXT_CONGRATULATE;
    }
    break;
  case STEP_TEXT_CONGRATULATE:
    {
      if( !ShinkaDemo_SoundCheckPlayCongratulate( param, work ) )
      { 
        if( ShinkaDemo_WaitTextStream( work ) )
        {
          if( ShinkaDemo_TextWaitInput( work ) )
          {
            {
              // ����Ⴂ�p�f�[�^���Z�b�g
              { 
                GAMEBEACON_Set_PokemonEvolution( PP_Get( work->pp, ID_PARA_monsno, NULL ), work->poke_nick_name_strbuf );
                // (�Â�)�j�b�N�l�[����(�V����)�푰���ɐi������
              }

              // �i���������|�P�����̐�
              // 1���Ƀ|�P�����i����������
              {
                RECORD* rec = GAMEDATA_GetRecordPtr(param->gamedata); 
                RECORD_Inc(rec, RECID_POKE_EVOLUTION);    // �i���������|�P�����̐�
                RECORD_Inc(rec, RECID_DAYCNT_EVOLUTION);  // 1���Ƀ|�P�����i����������
              }
            }

            //// ����
            //work->step = STEP_WAZA_OBOE;
            //
            //work->wazaoboe_index = 0;
            //STEP_SHINKA_COND_CHECK��ʂ��Ă���STEP_WAZA_OBOE�ֈڍs����

            // ����
            work->step = STEP_SHINKA_COND_CHECK;
          }
        }
      }
    }
    break;

  //---------------------------
  // �i���L�����Z�������ꍇ 
  case STEP_BGM_CANCEL_SHINKA_POP:
    {
      ShinkaDemo_SoundPopShinka( param, work );
     
      ShinkaDemo_TextWinfrmShow( param, work, TRUE );
      ShinkaDemo_MakeTextStream(
          work,
          work->text_msgdata_shinka, SHINKADEMO_ShinkaCancelMsg, NULL,
          TAG_REGIST_TYPE_POKE_NICK_NAME, work->pp,
          TAG_REGIST_TYPE_NONE, NULL );

      // ����
      work->step = STEP_TEXT_CANCEL;
    }
    break;
  case STEP_TEXT_CANCEL:
    {
      if( ShinkaDemo_WaitTextStream( work ) )
      {
        if( ShinkaDemo_TextWaitInput( work ) )
        {
          // ����
          work->step = STEP_FADE_OUT_START;
        }
      }
    }
    break;

  //---------------------------
  // �i����������������
  case STEP_SHINKA_COND_CHECK:
    {
      switch( param->shinka_cond )
      {
      case SHINKA_COND_SPECIAL_LEVELUP:  // �k�P�j����a��������  // SHINKA_COND_SPECIAL_NUKENIN�ł͂Ȃ�SHINKA_COND_SPECIAL_LEVELUP(SHINKA_COND_SPECIAL_LEVELUP�̓c�`�j���ł��������Ȃ��l)
        {
          ShinkaDemo_ShinkaCondCheckSpecialLevelup( param, work );
        }
        break;
      }

      // ����
      work->step = STEP_WAZA_OBOE;
            
      work->wazaoboe_index = 0;
    }
    break;

  //---------------------------
  // �Z�o�� 
  case STEP_WAZA_OBOE:
    {
      {
        // �Z�o���`�F�b�N
        work->wazaoboe_no = PP_CheckWazaOboe( work->pp, &work->wazaoboe_index, work->heap_id );
        if( work->wazaoboe_no == PTL_WAZAOBOE_NONE )
        {
          // ����
          work->step = STEP_FADE_OUT_START;
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
          ShinkaDemo_MakeTextStream(
            work,
            work->text_msgdata_wazaoboe, msg_waza_oboe_04, NULL,
            TAG_REGIST_TYPE_POKE_NICK_NAME, work->pp,
            TAG_REGIST_TYPE_WAZA, &work->wazaoboe_no );
          
          ShinkaDemo_SoundPlayWazaoboe( param, work );
        }
      }
    }
    break;
  case STEP_WAZA_VACANT_MASTER:
    {
      if( ShinkaDemo_WaitTextStream( work ) )
      {
        if( ShinkaDemo_TextWaitInput( work ) )
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
      ShinkaDemo_MakeTextStream(
        work,
        work->text_msgdata_wazaoboe, msg_waza_oboe_05, NULL,
        TAG_REGIST_TYPE_POKE_NICK_NAME, work->pp,
        TAG_REGIST_TYPE_WAZA, &work->wazaoboe_no );
    }
    break;
  case STEP_WAZA_FULL_Q:
    {
      if( ShinkaDemo_WaitTextStream( work ) )
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

  //---------------------------
  // �t�B�[���h�Z�I�� 
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
          // ���[�J��PROC�Ăяo��
          GFL_PROC_LOCAL_CallProc( work->local_procsys, NO_OVERLAY_ID, &PokeStatus_ProcData, work->psData );
        }
      }
    }
    break;
  case STEP_WAZA_STATUS_FIELD:
    {
      // ���[�J��PROC���I������̂�҂�  // ����Main�̍ŏ���GFL_PROC_MAIN_VALID�Ȃ�return���Ă���̂ŁA�����ł͔��肵�Ȃ��Ă��悢���O�̂���
      if( local_proc_status != GFL_PROC_MAIN_VALID )
      {
        // ����
        work->step = STEP_WAZA_STATUS_FIELD_OUT;
      }
      else
      {
        return GFL_PROC_RES_CONTINUE;
      }
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

  //---------------------------
  // �o�g���Z�I�� 
  case STEP_WAZA_STATUS_BATTLE_FADE_OUT:
    {
      if( !GFL_FADE_CheckFade() )
      {
        // ����
        work->step = STEP_WAZA_STATUS_BATTLE;

        // ����ʂ̔j��
        YESNO_MENU_DeleteRes( work->yesno_menu_work );
        SHINKADEMO_GRAPHIC_ExitBGSub( work->graphic );

        // �L�[or�^�b�`
        {
          work->cursor_flag = ( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY );
        } 

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
        work->bplist_data.gamedata    = param->gamedata;
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

        // �L�[or�^�b�`
        {
          GFL_UI_SetTouchOrKey( (work->cursor_flag)?(GFL_APP_END_KEY):(GFL_APP_END_TOUCH) );
        } 
      }
    }
    break;

  //---------------------------
  // �Z�Y�� 
  case STEP_WAZA_CONFIRM_PREPARE:
    {
      // ����
      work->step = STEP_WAZA_FORGET;
      
      ShinkaDemo_TextWinfrmShow( param, work, TRUE );  // �t�B�[���h�Z�I���̌�͏����Ă���̂�
      ShinkaDemo_MakeTextStream(
        work,
        work->text_msgdata_wazaoboe, msg_waza_oboe_06, ShinkaDemo_TextStreamForgetCallBack,
        TAG_REGIST_TYPE_POKE_NICK_NAME, work->pp,
        TAG_REGIST_TYPE_WAZA, &work->wazawasure_no );
    }
    break;
  case STEP_WAZA_FORGET:
    {
      if( ShinkaDemo_WaitTextStream( work ) )
      {
        // �Z�o����
        PP_SetWazaPos( work->pp, work->wazaoboe_no, work->wazawasure_pos );
        
        // ����
        work->step = STEP_WAZA_FORGET_MASTER;

        ShinkaDemo_MakeTextStream(
          work,
          work->text_msgdata_wazaoboe, msg_waza_oboe_07, NULL,
          TAG_REGIST_TYPE_POKE_NICK_NAME, work->pp,
          TAG_REGIST_TYPE_WAZA, &work->wazaoboe_no );

        ShinkaDemo_SoundPlayWazaoboe( param, work );
      }
    }
    break;
  case STEP_WAZA_FORGET_MASTER:
    {
      if( ShinkaDemo_WaitTextStream( work ) )
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

      ShinkaDemo_TextWinfrmShow( param, work, TRUE );  // �t�B�[���h�Z�I���̌�͏����Ă���̂�
      ShinkaDemo_MakeTextStream(
        work,
        work->text_msgdata_wazaoboe, msg_waza_oboe_08, NULL,
        TAG_REGIST_TYPE_NONE, NULL,
        TAG_REGIST_TYPE_WAZA, &work->wazaoboe_no );
    }
    break;
  case STEP_WAZA_ABANDON_Q:
    {
      if( ShinkaDemo_WaitTextStream( work ) )
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

          ShinkaDemo_MakeTextStream(
            work,
            work->text_msgdata_wazaoboe, msg_waza_oboe_09, NULL,
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
      if( ShinkaDemo_WaitTextStream( work ) )
      {
        // ����
        work->step = STEP_WAZA_OBOE;
      }
    }
    break;

  //---------------------------
  case STEP_FADE_OUT_START:
    {
      if( ShinkaDemo_SoundCheckPlayShinka( param, work ) )
      {
        ShinkaDemo_SoundFadeOutShinka( param, work );
   
        // �t�F�[�h�A�E�g(�����遨��)
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, FADE_OUT_WAIT );

        // ����
        work->step = STEP_FADE_OUT_WAIT;
      }
    }
    break;
  case STEP_FADE_OUT_WAIT:
    {
      if( !ShinkaDemo_SoundCheckFadeOutShinka( param, work ) )
      {
        // ����
        work->step = STEP_FADE_OUT_END;
        
        ShinkaDemo_SoundFadeInField( param, work );
      }
    }
    break;
  case STEP_FADE_OUT_END:
    {
      if( !GFL_FADE_CheckFade() )
      {
        // ����
        work->step = STEP_END;

        return GFL_PROC_RES_FINISH;
      }
    }
    break;
  case STEP_END:
    {
      return GFL_PROC_RES_FINISH;
    }
    break;
  }

  ShinkaDemo_SoundMain( param, work );

  if(    work->step != STEP_WAZA_STATUS_FIELD
      && work->step != STEP_WAZA_STATUS_FIELD_OUT )
  {
    YESNO_MENU_Main( work->yesno_menu_work );

    ShinkaDemo_TextMain( param, work );

    PRINTSYS_QUE_Main( work->print_que );

    SHINKADEMO_VIEW_Main( work->view );
    // �i���f���̃p�[�e�B�N���Ɣw�i
    SHINKADEMO_EFFECT_Main( work->efwk );

    // 2D�`��
    SHINKADEMO_GRAPHIC_2D_Draw( work->graphic );
    // 3D�`��
    SHINKADEMO_GRAPHIC_3D_StartDraw( work->graphic );
#if 0 
    // �������|�P��������O�ɃG�t�F�N�g�Ɣw�i������ꍇ
    SHINKADEMO_VIEW_Draw( work->view );
    // �i���f���̃p�[�e�B�N���Ɣw�i
    SHINKADEMO_EFFECT_Draw( work->efwk );
#else
    // �G�t�F�N�g�Ɣw�i����O�ɔ������|�P����������ꍇ
    // �i���f���̃p�[�e�B�N���Ɣw�i
    SHINKADEMO_EFFECT_Draw( work->efwk );
    SHINKADEMO_VIEW_Draw( work->view );
#endif
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
    // �O���t�B�b�N�A�t�H���g�Ȃ�
    work->graphic       = SHINKADEMO_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, work->heap_id );
    SHINKADEMO_GRAPHIC_InitBGSub( work->graphic );
    work->font          = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, work->heap_id );
    work->print_que     = PRINTSYS_QUE_Create( work->heap_id );
  }

  // �e�L�X�g�̏���������
  ShinkaDemo_TextInit( param, work );
  // YESNO_MENU�̏���������
  ShinkaDemo_YesNoMenuInit( param, work );
  // BG
  ShinkaDemo_BgInit( param, work );
  // OBJ
  {
    if( work->step == STEP_FADE_IN_BEFORE )
    {
      ShinkaDemo_ObjInit( param, work );
    }
    else
    {
      ShinkaDemo_ObjInitAfterEvolution( param, work );
    }
  }

  // �v���C�I���e�B�A�\���A�w�i�F�Ȃ�
  {
    GFL_BG_SetPriority( BG_FRAME_M_POKEMON               , BG_FRAME_PRI_M_POKEMON );
    GFL_BG_SetPriority( BG_FRAME_M_BELT                  , BG_FRAME_PRI_M_BELT    );
    GFL_BG_SetPriority( BG_FRAME_M_TEXT                  , BG_FRAME_PRI_M_TEXT    );  // �őO��

    GFL_BG_SetPriority( BG_FRAME_S_BACK                  , BG_FRAME_PRI_S_BACK );
    //GFL_BG_SetPriority( BG_FRAME_S_BUTTON                , BG_FRAME_PRI_S_BUTTON );
    //GFL_BG_SetPriority( BG_FRAME_S_TEXT                  , BG_FRAME_PRI_S_TEXT );  // �őO��

    GFL_BG_SetVisible( BG_FRAME_M_POKEMON                , VISIBLE_ON );
    GFL_BG_SetVisible( BG_FRAME_M_BELT                   , VISIBLE_ON );
    GFL_BG_SetVisible( BG_FRAME_M_TEXT                   , VISIBLE_ON );
  
    GFL_BG_SetVisible( BG_FRAME_S_BACK                  , VISIBLE_ON );
    //GFL_BG_SetVisible( BG_FRAME_S_BUTTON                , VISIBLE_ON );
    //GFL_BG_SetVisible( BG_FRAME_S_TEXT                  , VISIBLE_ON );
  
    GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, 0x0000 );
    GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, 0x0000 );
  }

  // �i���f���̉��o
  {
    if( work->step == STEP_FADE_IN_BEFORE )
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

  // �i���f���̃p�[�e�B�N���Ɣw�i
  {
    if( work->step == STEP_FADE_IN_BEFORE )
    {
      work->efwk = SHINKADEMO_EFFECT_Init( work->heap_id, SHINKADEMO_EFFECT_LAUNCH_EVO );
    }
    else
    {
      work->efwk = SHINKADEMO_EFFECT_Init( work->heap_id, SHINKADEMO_EFFECT_LAUNCH_AFTER );
    }
  }

  // �p�[�e�B�N���Ή�
  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG0,
                    GX_BLEND_PLANEMASK_BD | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG1,
                    0, 0 );
    // �p�[�e�B�N���̃A���t�@�����ꂢ�ɏo������
    // ev1��ev2�͎g���Ȃ�  // TWL�v���O���~���O�}�j���A���u2D�ʂƂ̃��u�����f�B���O�v�Q�l

  // �ʐM�A�C�R��
  GFL_NET_WirelessIconEasy_HoldLCD( FALSE, work->heap_id );
  GFL_NET_ReloadIcon();
}

//-------------------------------------
/// �I������
//=====================================
static void ShinkaDemo_Exit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  // �i���f���̃p�[�e�B�N���Ɣw�i
  {
    SHINKADEMO_EFFECT_Exit( work->efwk );
  }

  // �i���f���̉��o
  {
    SHINKADEMO_VIEW_Exit( work->view );
  }

  // OBJ
  {
    ShinkaDemo_ObjExit( param, work );
  }
  // BG
  ShinkaDemo_BgExit( param, work );
  // YESNO_MENU�̏I������
  ShinkaDemo_YesNoMenuExit( param, work );
  // �e�L�X�g�̏I������
  ShinkaDemo_TextExit( param, work );

  // SHINKA_DEMO_WORK��Еt��
  {
    // �O���t�B�b�N�A�t�H���g�Ȃ�
    PRINTSYS_QUE_Clear( work->print_que );
    PRINTSYS_QUE_Delete( work->print_que );
    GFL_FONT_Delete( work->font );
    SHINKADEMO_GRAPHIC_ExitBGSub( work->graphic );
    SHINKADEMO_GRAPHIC_Exit( work->graphic );
  }
  
  // �I�[�o�[���C
  GFL_OVERLAY_Unload( FS_OVERLAY_ID( ui_common ) );

  // �ʐM�A�C�R��
  GFL_NET_WirelessIconEasy_DefaultLCD();
}

//-------------------------------------
/// �T�E���h����������
//=====================================
static void ShinkaDemo_SoundInit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  PMSND_FadeOutBGM( FSND_FADE_SHORT );
  work->sound_step = SOUND_STEP_FIELD_FADE_OUT;
}
//-------------------------------------
/// �T�E���h�I������
//=====================================
static void ShinkaDemo_SoundExit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
/*
�T�E���h�q�[�v������Ȃ��̂Ŗ���
  // �T�E���h�f�[�^�v���Z�b�g
  SOUNDMAN_ReleasePresetData( work->sound_preset_handle );
*/
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
  case SOUND_STEP_FIELD_FADE_OUT:
    {
      if( !PMSND_CheckFadeOnBGM() )
      {
        PMSND_PauseBGM( TRUE );
        PMSND_PushBGM();
        work->sound_step = SOUND_STEP_WAIT;

/*
�T�E���h�q�[�v������Ȃ��̂Ŗ���
        // �T�E���h�f�[�^�v���Z�b�g
        work->sound_preset_handle = SOUNDMAN_PresetSoundTbl( preset_sound_table, NELEMS(preset_sound_table) );
*/
      }
    }
    break;
  case SOUND_STEP_INTRO:
    {
      if( !PMSND_CheckPlayBGM() )
      {
        work->sound_step = SOUND_STEP_WAIT;
      }
    }
    break;
  case SOUND_STEP_SHINKA:
    {
    }
    break;
  case SOUND_STEP_SHINKA_PUSH:  // SHINKA�Ȃ�push���Ă��艽�����Ă��Ȃ�
    {
    }
    break;
  case SOUND_STEP_SHINKA_FADE_OUT:  // SHINKA�Ȃ��~����
    {
      if( !PMSND_CheckFadeOnBGM() )
      {
        PMSND_StopBGM();
        work->sound_step = SOUND_STEP_WAIT;
      }
    }
    break;
  case SOUND_STEP_CONGRATULATE_LOAD:
    {
      // BGM�̕������[�h�𗘗p���Ă݂�
      BOOL play_start = PMSND_PlayBGMdiv( SEQ_ME_SHINKAOME, &(work->sound_div_seq), FALSE );
      if( play_start )
      {
        work->sound_step = SOUND_STEP_CONGRATULATE;
      }
    }
    break;
  case SOUND_STEP_CONGRATULATE:  // SOUND_STEP_SHINKA_PUSH���o�Ă���
    {
      if( !PMSND_CheckPlayBGM() )
      {
        ShinkaDemo_SoundPopShinka( param, work );
      }
    }
    break;
  case SOUND_STEP_WAZAOBOE:  // SOUND_STEP_SHINKA_PUSH���o�Ă���
    {
      if( !PMSND_CheckPlayBGM() )
      {
        ShinkaDemo_SoundPopShinka( param, work );
      }
    }
    break;
  }
}
//-------------------------------------
/// �T�E���h
//=====================================
static BOOL ShinkaDemo_SoundCheckFadeOutField( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  return ( work->sound_step == SOUND_STEP_FIELD_FADE_OUT );
}
static void ShinkaDemo_SoundFadeInField( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  if( work->sound_step == SOUND_STEP_WAIT )
  {
    // �t�B�[���hBGM
    PMSND_PopBGM();
    PMSND_PauseBGM( FALSE );
    PMSND_FadeInBGM( FSND_FADE_NORMAL );
  }
}
static void ShinkaDemo_SoundPlayIntro( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  if( work->sound_step == SOUND_STEP_WAIT )
  {
    PMSND_PlayBGM(SEQ_BGM_SHINKA);  // Intro�̋Ȗ���SHINKA
    work->sound_step = SOUND_STEP_INTRO;
  }
}
static BOOL ShinkaDemo_SoundCheckPlayIntro( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  return ( work->sound_step == SOUND_STEP_INTRO );
}
static void ShinkaDemo_SoundPlayShinka( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  if( work->sound_step == SOUND_STEP_WAIT )
  {
    PMSND_PlayBGM(SEQ_BGM_KOUKAN);  // Shinka�̋Ȗ���KOUKAN
    work->sound_step = SOUND_STEP_SHINKA;
  }
}
static void ShinkaDemo_SoundPushShinka( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  if( work->sound_step == SOUND_STEP_SHINKA )
  {
    // �����̓t�F�[�h���Ȃ��Ă���
    PMSND_PauseBGM( TRUE );
    PMSND_PushBGM();
    work->sound_step = SOUND_STEP_SHINKA_PUSH;
  }
}
static void ShinkaDemo_SoundPopShinka( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  if(    work->sound_step == SOUND_STEP_SHINKA_PUSH
      || work->sound_step == SOUND_STEP_CONGRATULATE
      || work->sound_step == SOUND_STEP_WAZAOBOE )
  {
    PMSND_PopBGM();
    PMSND_PauseBGM( FALSE );
    PMSND_FadeInBGM( FSND_FADE_FAST );
    work->sound_step = SOUND_STEP_SHINKA;
  }
}
static BOOL ShinkaDemo_SoundCheckPlayShinka( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  return ( work->sound_step == SOUND_STEP_SHINKA );
}
static void ShinkaDemo_SoundFadeOutShinka( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  if( work->sound_step == SOUND_STEP_SHINKA )
  {
    PMSND_FadeOutBGM( FSND_FADE_NORMAL );
    work->sound_step = SOUND_STEP_SHINKA_FADE_OUT;
  }
}
static BOOL ShinkaDemo_SoundCheckFadeOutShinka( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  return ( work->sound_step == SOUND_STEP_SHINKA_FADE_OUT );
}
static void ShinkaDemo_SoundPlayCongratulate( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  if( work->sound_step == SOUND_STEP_SHINKA_PUSH )
  {
/*
BGM�̕������[�h�𗘗p���Ă݂邱�Ƃɂ����̂ŃR�����g�A�E�g
    PMSND_PlayBGM(SEQ_ME_SHINKAOME);
    work->sound_step = SOUND_STEP_CONGRATULATE;
*/
    // BGM�̕������[�h�𗘗p���Ă݂�
    work->sound_div_seq = 0;
    PMSND_PlayBGMdiv( SEQ_ME_SHINKAOME, &(work->sound_div_seq), TRUE );
    work->sound_step = SOUND_STEP_CONGRATULATE_LOAD;
  }
}
static BOOL ShinkaDemo_SoundCheckPlayCongratulate( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  return ( work->sound_step == SOUND_STEP_CONGRATULATE_LOAD || work->sound_step == SOUND_STEP_CONGRATULATE );
}
static void ShinkaDemo_SoundPlayWazaoboe( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  if( work->sound_step == SOUND_STEP_SHINKA )
  {
    ShinkaDemo_SoundPushShinka( param, work );
    //PMSND_PlaySE( SEQ_SE_LVUP );  // btl_client.c���Q�l�ɂ���
    PMSND_PlayBGM( SEQ_ME_LVUP );  // btl_client.c���Q�l�ɂ���  // ME�̏ꍇ�́A�����I��push, pop���Ă����炵�����A���̏I����m�肽���̂Ŏ蓮��push, pop���邱�Ƃɂ����B
    work->sound_step = SOUND_STEP_WAZAOBOE;
  }
}

//-------------------------------------
/// BG
//=====================================
static void ShinkaDemo_BgInit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_SHINKA_DEMO, work->heap_id );

  // BELT
  GFL_ARCHDL_UTIL_TransVramPalette(
      handle,
      NARC_shinka_demo_particle_demo_egg_NCLR,
      PALTYPE_MAIN_BG,
      BG_PAL_POS_M_BELT * 0x20,
      BG_PAL_NUM_M_BELT * 0x20,
      work->heap_id );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
      handle,
      NARC_shinka_demo_particle_demo_egg_bg_NCGR,
      BG_FRAME_M_BELT,
			0,
      0,  // �S�]��
      FALSE,
      work->heap_id );

  GFL_ARCHDL_UTIL_TransVramScreen(
      handle,
      NARC_shinka_demo_particle_demo_egg_NSCR,
      BG_FRAME_M_BELT,
      0,
      0,  // �S�]��
      FALSE,
      work->heap_id );

  GFL_ARC_CloseDataHandle( handle );

  GFL_BG_LoadScreenReq( BG_FRAME_M_BELT );
}
static void ShinkaDemo_BgExit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  // �������Ȃ�
}

//-------------------------------------
/// OBJ
//=====================================
static void ShinkaDemo_ObjInit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  u8 i;

  // ���\�[�X�ǂݍ���
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_SHINKA_DEMO, work->heap_id );

  work->obj_res[OBJ_RES_BELT_NCL] = GFL_CLGRP_PLTT_RegisterEx( 
          handle,
          NARC_shinka_demo_particle_demo_egg_NCLR,
          CLSYS_DRAW_MAIN,
          OBJ_PAL_POS_M_BELT * 0x20,
          0,
          OBJ_PAL_NUM_M_BELT,
          work->heap_id );
      
  work->obj_res[OBJ_RES_BELT_NCG] = GFL_CLGRP_CGR_Register(
          handle,
          NARC_shinka_demo_particle_demo_egg_NCGR,
          FALSE,
          CLSYS_DRAW_MAIN,
          work->heap_id );

  work->obj_res[OBJ_RES_BELT_NCE] = GFL_CLGRP_CELLANIM_Register(
          handle,
          NARC_shinka_demo_particle_demo_egg_NCER,
          NARC_shinka_demo_particle_demo_egg_NANR,
          work->heap_id );
  
  GFL_ARC_CloseDataHandle( handle );

  // CLWK�쐬
  for( i=0; i<OBJ_CELL_MAX; i++ )
  {
    work->obj_clwk[i] = GFL_CLACT_WK_Create(
        SHINKADEMO_GRAPHIC_GetClunit( work->graphic ),
        work->obj_res[OBJ_RES_BELT_NCG],
        work->obj_res[OBJ_RES_BELT_NCL],
        work->obj_res[OBJ_RES_BELT_NCE],
        &obj_cell_data[i],
        CLSYS_DEFREND_MAIN,
        work->heap_id );
    GFL_CLACT_WK_SetAutoAnmFlag( work->obj_clwk[i], FALSE );
  }
}
static void ShinkaDemo_ObjInitAfterEvolution( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  ShinkaDemo_ObjInit( param, work );

  {
    u8 i;
    for( i=0; i<OBJ_CELL_MAX; i++ )
    {
      GFL_CLACT_WK_AddAnmFrame( work->obj_clwk[i], FX32_CONST(10000) );  // 1��ŃA�j�����I���悤�ɁA�\���傫�Ȑ�����^����
      GFL_CLACT_WK_SetAutoAnmFlag( work->obj_clwk[i], TRUE );
    }
  }
}
static void ShinkaDemo_ObjExit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  // CLWK�j��
  u8 i;
  for( i=0; i<OBJ_CELL_MAX; i++ )
  {
    GFL_CLACT_WK_Remove( work->obj_clwk[i] );
  }

  // ���\�[�X�j��
  GFL_CLGRP_CELLANIM_Release( work->obj_res[OBJ_RES_BELT_NCE] );
  GFL_CLGRP_CGR_Release( work->obj_res[OBJ_RES_BELT_NCG] );
  GFL_CLGRP_PLTT_Release( work->obj_res[OBJ_RES_BELT_NCL] );
}
static void ShinkaDemo_ObjStartAnime( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  u8 i;
  for( i=0; i<OBJ_CELL_MAX; i++ )
  {
    GFL_CLACT_WK_SetAnmIndex( work->obj_clwk[i], 0 );
    GFL_CLACT_WK_SetAutoAnmFlag( work->obj_clwk[i], TRUE );
  }
}
static BOOL ShinkaDemo_ObjIsEndAnime( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  BOOL b_end = TRUE;
  u8 i;
  for( i=0; i<OBJ_CELL_MAX; i++ )
  {
    if( GFL_CLACT_WK_CheckAnmActive( work->obj_clwk[i] ) )
    {
      b_end = FALSE;
      break;
    }
  }
  return b_end;
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
}

//-------------------------------------
/// �e�L�X�g����������
//=====================================
static void ShinkaDemo_TextInit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  // TEXT
  {
    // �p���b�g
    GFL_ARC_UTIL_TransVramPalette(
        ARCID_FONT,
        NARC_font_default_nclr,
        PALTYPE_MAIN_BG,
        BG_PAL_POS_M_TEXT_FONT * 0x20,
        BG_PAL_NUM_M_TEXT_FONT * 0x20,
        work->heap_id );

    // BG�t���[���̃X�N���[���̋󂢂Ă���ӏ��ɉ����\��������Ȃ��悤�ɂ��Ă���
    work->text_dummy_bmpwin = GFL_BMPWIN_Create(
        BG_FRAME_M_TEXT,
        0, 0, 1, 1,
        BG_PAL_POS_M_TEXT_FONT,
        GFL_BMP_CHRAREA_GET_F );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_dummy_bmpwin), 0 );
    GFL_BMPWIN_TransVramCharacter(work->text_dummy_bmpwin);

    // �E�B���h�E��
    work->text_winin_bmpwin = GFL_BMPWIN_Create(
        BG_FRAME_M_TEXT,
        1, 19, 30, 4,
        BG_PAL_POS_M_TEXT_FONT,
        GFL_BMP_CHRAREA_GET_B );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_winin_bmpwin), TEXT_WININ_ZERO_COLOR );
    GFL_BMPWIN_MakeTransWindow_VBlank( work->text_winin_bmpwin );
    
    // �E�B���h�E�g
    work->text_winfrm_tinfo = BmpWinFrame_GraphicSetAreaMan(
        BG_FRAME_M_TEXT,
        BG_PAL_POS_M_TEXT_FRAME,
        MENU_TYPE_SYSTEM,
        work->heap_id );

    // ���b�Z�[�W
    work->text_msgdata_shinka     = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_shinka_demo_dat, work->heap_id );
    work->text_msgdata_wazaoboe   = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_waza_oboe_dat, work->heap_id );

    // TCBL�A�t�H���g�J���[�A�]���Ȃ�
    work->text_tcblsys = GFL_TCBL_Init( work->heap_id, work->heap_id, 1, 0 );
    GFL_FONTSYS_SetColor( 1, 2, TEXT_WININ_BACK_COLOR );

    // NULL������
    work->text_strbuf       = NULL;
    work->text_stream       = NULL;
  }

  // ���b�Z�[�W����L�[�J�[�\���A�C�R��
  work->text_keycursor_wk = APP_KEYCURSOR_Create( TEXT_WININ_BACK_COLOR, TRUE, TRUE, work->heap_id );
}

//-------------------------------------
/// �e�L�X�g�I������
//=====================================
static void ShinkaDemo_TextExit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  // ���b�Z�[�W����L�[�J�[�\���A�C�R��
  APP_KEYCURSOR_Delete( work->text_keycursor_wk );

  // TEXT
  {
    if( work->text_stream ) PRINTSYS_PrintStreamDelete( work->text_stream );
    GFL_TCBL_Exit( work->text_tcblsys );
    if( work->text_strbuf ) GFL_STR_DeleteBuffer( work->text_strbuf );
    GFL_MSG_Delete( work->text_msgdata_shinka );
    GFL_MSG_Delete( work->text_msgdata_wazaoboe );
    GFL_BG_FreeCharacterArea(
        BG_FRAME_M_TEXT,
        GFL_ARCUTIL_TRANSINFO_GetPos(work->text_winfrm_tinfo),
        GFL_ARCUTIL_TRANSINFO_GetSize(work->text_winfrm_tinfo) );
    GFL_BMPWIN_Delete( work->text_winin_bmpwin );
    GFL_BMPWIN_Delete( work->text_dummy_bmpwin );
  }
}

//-------------------------------------
/// �e�L�X�g�又��
//=====================================
static void ShinkaDemo_TextMain( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  GFL_TCBL_Main( work->text_tcblsys );
  
  if( work->text_stream ) 
  {
    APP_KEYCURSOR_Main( work->text_keycursor_wk, work->text_stream, work->text_winin_bmpwin );
  }
}

//-------------------------------------
/// �u�����������@���ꂢ�ɂ킷�ꂽ�I�v���b�Z�[�W�̃R�[���o�b�N�֐�
//=====================================
static BOOL ShinkaDemo_TextStreamForgetCallBack( u32 arg )
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
/// TEXT�̃X�g���[���쐬
//=====================================
static void ShinkaDemo_MakeTextStream( SHINKA_DEMO_WORK* work,
                                       GFL_MSGDATA* msgdata, u32 str_id, pPrintCallBack callback,
                                       TAG_REGIST_TYPE type0, const void* data0,
                                       TAG_REGIST_TYPE type1, const void* data1 )
{
  // ����
  ShinkaDemo_TextWininClear( work, TEXT_WININ_BACK_COLOR );

  // ������쐬
  work->text_strbuf = MakeStr( work->heap_id,
                               msgdata, str_id,
                               type0, data0,
                               type1, data1 );

  // �X�g���[���J�n
  work->text_stream = PRINTSYS_PrintStreamCallBack(
                              work->text_winin_bmpwin,
                              0, 0,
                              work->text_strbuf,
                              work->font, MSGSPEED_GetWait(),
                              work->text_tcblsys, 2,
                              work->heap_id,
                              TEXT_WININ_BACK_COLOR,
                              callback );
}

//-------------------------------------
/// TEXT�̃X�g���[���҂�
//=====================================
static BOOL ShinkaDemo_WaitTextStream( SHINKA_DEMO_WORK* work )
{
  BOOL ret = FALSE;

  switch( PRINTSYS_PrintStreamGetState( work->text_stream ) )
  { 
  case PRINTSTREAM_STATE_RUNNING:
    if( ( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) ) || GFL_UI_TP_GetTrg() )
    {
      PRINTSYS_PrintStreamShortWait( work->text_stream, 0 );

      if( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) )
      {
        // �^�b�`or�L�[
        GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      }
      else
      {
        // �^�b�`or�L�[
        GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
      }
    }
    break;
  case PRINTSTREAM_STATE_PAUSE:
    if( ( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) ) || GFL_UI_TP_GetTrg() )
    {
      PMSND_PlaySystemSE( SEQ_SE_MESSAGE );
      PRINTSYS_PrintStreamReleasePause( work->text_stream );

      if( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) )
      {
        // �^�b�`or�L�[
        GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      }
      else
      {
        // �^�b�`or�L�[
        GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
      }
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
/// �e�L�X�g��̓��͑҂�
//=====================================
static BOOL ShinkaDemo_TextWaitInput( SHINKA_DEMO_WORK* work )
{
  if( ( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) ) || GFL_UI_TP_GetTrg() )
  { 
    if( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) )
    {
      // �^�b�`or�L�[
      GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    }
    else
    {
      // �^�b�`or�L�[
      GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
    }

    return TRUE;
  }

  return FALSE;
}

//-------------------------------------
/// �e�L�X�g�̒��g������
//=====================================
static void ShinkaDemo_TextWininClear( SHINKA_DEMO_WORK* work, u8 col_code )
{
  // ��U����
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_winin_bmpwin), col_code );
	//GFL_BMPWIN_TransVramCharacter(work->text_winin_bmpwin);      // GFL_BMPWIN_TransVramCharacter�ł�2��ڃE�B���h�E���������̂܂܂������̂ŁA
  GFL_BMPWIN_MakeTransWindow_VBlank( work->text_winin_bmpwin );  // GFL_BMPWIN_MakeTransWindow_VBlank�ɂ����B

  // �O�̂�����
  if( work->text_stream ) PRINTSYS_PrintStreamDelete( work->text_stream );  // ���̊֐��Ń^�X�N���폜���Ă����̂ŁA�����ɓ����^�X�N��1�ōς�
  if( work->text_strbuf ) GFL_STR_DeleteBuffer( work->text_strbuf );

  work->text_stream = NULL;
  work->text_strbuf = NULL;
}

//-------------------------------------
/// �e�L�X�g�̘g��`��/����
//=====================================
static void ShinkaDemo_TextWinfrmShow( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work, BOOL is_on )
{
  if( is_on )
  {
    BmpWinFrame_Write(
        work->text_winin_bmpwin,
        WINDOW_TRANS_ON_V,
        GFL_ARCUTIL_TRANSINFO_GetPos(work->text_winfrm_tinfo),
        BG_PAL_POS_M_TEXT_FRAME );
  }
  else
  {
    BmpWinFrame_Clear( work->text_winin_bmpwin, WINDOW_TRANS_ON_V );
  }
}


//-------------------------------------
/// YESNO_MENU�̏���������
//=====================================
static void ShinkaDemo_YesNoMenuInit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
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
}

//-------------------------------------
/// YESNO_MENU�̏I������
//=====================================
static void ShinkaDemo_YesNoMenuExit( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  // YESNO_MENU
  {
    YESNO_MENU_DeleteRes( work->yesno_menu_work );
    if( work->yesno_menu_strbuf_yes ) GFL_STR_DeleteBuffer( work->yesno_menu_strbuf_yes );
    if( work->yesno_menu_strbuf_no ) GFL_STR_DeleteBuffer( work->yesno_menu_strbuf_no );
    GFL_MSG_Delete( work->yesno_menu_msgdata );
  }
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

//-------------------------------------
/// �i����������������
//=====================================
static void ShinkaDemo_ShinkaCondCheckSpecialLevelup( SHINKA_DEMO_PARAM* param, SHINKA_DEMO_WORK* work )
{
  // �u�c�`�j���v����ł͂Ȃ��u�Z���o����O�̃e�b�J�j���v����a��������
  // �p�[�e�B�ɋ󂫂������āA���ʂ̃����X�^�[�{�[���������Ă���ꍇ�A�k�P�j�����a������
  
  // ���̊֐����Ăяo�����_�ł�work->pp�́A���Ɂu�Z���o����O�̃e�b�J�j���v�ɂȂ��Ă��邱��
  
  {
    MYITEM_PTR myitem_ptr = GAMEDATA_GetMyItem( param->gamedata );

    if(    ( PokeParty_GetPokeCount(param->ppt) < PokeParty_GetPokeCountMax(param->ppt) )  // �p�[�e�B�ɋ󂫂�����
        && ( MYITEM_CheckItem( myitem_ptr, ITEM_MONSUTAABOORU, 1, work->heap_id ) ) )      // ���ʂ̃����X�^�[�{�[���������Ă���
    {
      BOOL ret;
      POKEMON_PARAM* nukenin_pp;

      // �k�P�j���a��
      nukenin_pp = GFL_HEAP_AllocClearMemory( work->heap_id, POKETOOL_GetWorkSize() );
      POKETOOL_CopyPPtoPP( (POKEMON_PARAM*)(work->pp), nukenin_pp );
      PP_ChangeMonsNo( nukenin_pp, MONSNO_NUKENIN );  // �i��


      //-------------// ���p�����[�^��ݒ肷�遫 //-------------//
      
      //�擾�{�[���������X�^�[�{�[����
      PP_Put( nukenin_pp, ID_PARA_get_ball, ITEM_MONSUTAABOORU );
      
      //�����A�C�e�����Ȃ���
      PP_Put( nukenin_pp, ID_PARA_item, 0 );
      
      //�|�P�����ɂ���}�[�N������
      PP_Put( nukenin_pp, ID_PARA_mark, 0 );
      
      //���{���n���N���A
      {
        int i;
        for( i=ID_PARA_sinou_champ_ribbon; i<ID_PARA_sinou_amari_ribbon+1; i++ )
        {
          PP_Put( nukenin_pp, i, 0 );
        }
        for( i=ID_PARA_stylemedal_normal; i<ID_PARA_world_ribbon+1; i++ )
        {
          PP_Put( nukenin_pp, i, 0 );
        }
        for( i=ID_PARA_trial_stylemedal_normal; i<ID_PARA_amari_ribbon+1; i++ )
        {
          PP_Put( nukenin_pp, i, 0 );
        }
      }
      
      //�j�b�N�l�[�����f�t�H���g����
      {
        u32 nukenin_monsno = PP_Get( nukenin_pp, ID_PARA_monsno, NULL );
        STRBUF* strbuf = GFL_MSG_CreateString( GlobalMsg_PokeName, nukenin_monsno );
        PP_Put( nukenin_pp, ID_PARA_nickname, (u32)strbuf );
        GFL_STR_DeleteBuffer( strbuf );
      }
      
      //��Ԉُ�𒼂�
      PP_Put( nukenin_pp, ID_PARA_condition, 0 );

      //���[���f�[�^
      {
        MAIL_DATA* mail_data = MailData_CreateWork( work->heap_id );
        PP_Put( nukenin_pp, ID_PARA_mail_data, (u32)mail_data );
        GFL_HEAP_FreeMemory( mail_data );
      }

      //�J�X�^���{�[��ID
      PP_Put( nukenin_pp, ID_PARA_cb_id, 0 );

/*
�J�X�^���{�[���Ȃ����ۂ�
      //�J�X�^���{�[���f�[�^
      {
        CB_CORE cb_core;
        MI_CpuClearFast( &cb_core, sizeof(CB_CORE) );
        PP_Put( nukenin_pp, ID_PARA_cb_core, cb_core );
      }
 */

      //-------------// ���p�����[�^��ݒ肷�遪 //-------------//


      // �p�[�e�B�ɓ����
      PokeParty_Add( (POKEPARTY*)(param->ppt), nukenin_pp );  // const�͂���

      // ���ʂ̃����X�^�[�{�[�������炷
      ret = MYITEM_SubItem( myitem_ptr, ITEM_MONSUTAABOORU, 1, work->heap_id );
      GF_ASSERT_MSG( ret, "SHINKADEMO : �����X�^�[�{�[�������点�܂���ł����B\n" );

      // ��n��
      GFL_HEAP_FreeMemory( nukenin_pp );

      // �}�ӓo�^
      {
        ZUKAN_SAVEDATA* zukan_savedata = GAMEDATA_GetZukanSave( param->gamedata );
        ZUKANSAVE_SetPokeSee( zukan_savedata, nukenin_pp );  // ����  // �}�Ӄt���O���Z�b�g����
        ZUKANSAVE_SetPokeGet( zukan_savedata, nukenin_pp );  // �߂܂���  // �}�Ӄt���O���Z�b�g����
      }

      {
        // �k�P�j���͂���Ⴂ�p�f�[�^���Z�b�g���Ȃ��Ă���
        //// ����Ⴂ�p�f�[�^���Z�b�g
        //{ 
        //  GAMEBEACON_Set_PokemonEvolution( PP_Get( nukenin_pp, ID_PARA_monsno, NULL ), work->poke_nick_name_strbuf );
        //  // (�Â�)�j�b�N�l�[����(�V����)�푰���ɐi������
        //}

        // �i���������|�P�����̐�
        // 1���Ƀ|�P�����i����������
        {
          RECORD* rec = GAMEDATA_GetRecordPtr(param->gamedata); 
          RECORD_Inc(rec, RECID_POKE_EVOLUTION);    // �i���������|�P�����̐�
          RECORD_Inc(rec, RECID_DAYCNT_EVOLUTION);  // 1���Ƀ|�P�����i����������
        }
      }
    }
  }
}

