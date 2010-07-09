//============================================================================
/**
 *  @file   d_test.c
 *  @brief  �e�X�g
 *  @author Koji Kawada
 *  @data   2010.03.31
 *  @note   
 *  ���W���[�����FD_KAWADA_TEST, D_TEST
 */
//============================================================================
#define DEBUG_KAWADA  // ���ꂪ��`����Ă���Ƃ��A�f�o�b�O�o�͂��������肷��


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/mcss.h"
#include "system/mcss_tool.h"
#include "poke_tool/monsno_def.h"

#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_msg.h"

#include "../../demo/shinka_demo/shinka_demo_graphic.h"
#include "../../demo/egg_demo/egg_demo_graphic.h"
#include "../../app/zukan/detail/zukan_detail_graphic.h"
#include "d_test.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "debug_message.naix"
#include "msg/debug/msg_d_kawada.h"

// �T�E���h

// �I�[�o�[���C
FS_EXTERN_OVERLAY(shinka_demo);
FS_EXTERN_OVERLAY(egg_demo);
FS_EXTERN_OVERLAY(zukan_detail);

#ifdef PM_DEBUG

//=============================================================================
/**
*  �萔��`
*/
//=============================================================================

// ���C��BG�t���[��
#define BG_FRAME_M_3D      (GFL_BG_FRAME0_M)

// �T�uBG�t���[��
#define BG_FRAME_S_TEXT    (GFL_BG_FRAME0_S)

// �T�uBG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_S_FONT_DEFAULT  = 1,
};
// �ʒu
enum
{
  BG_PAL_POS_S_FONT_DEFAULT  = 0,
};




//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
/**
*  �i���f���J�����ƃ^�}�S�z���f���J�����ŋ��ʎg�p�ł��鎖��
*/
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================

// mons_no����gra_no�𓾂邽�߂̃e�[�u��
static const u16 d_test_cmn_mons_no_to_gra_no_tbl[MONSNO_END +1] =
{
  #include "d_test_mons_no_to_gra_no.h" 
};
/*
d_test_mons_no_to_gra_no.h
��
pokemon_wb/prog/include/poke_tool/monsnum_def.h
��
MONSNO_???
��
???
�����o���Ď�Â��肵���t�@�C���B

gra_no = d_test_cmn_mons_no_to_gra_no_tbl[mons_no];
�Ƃ����ӂ��Ɏg�p�ł���B

0<=mons_no<=MONSNO_END

0
MONSNO_HUSIGIDANE
  .
  .
  .
MONSNO_INSEKUTA
*/

// gra_no����mons_no����ׂ��e�[�u��
static u16 d_test_cmn_mons_no_tbl_gra_no_order[MONSNO_END +1];
/*
��ԎႢgra_no��mons_no��
mons_no = d_test_cmn_mons_no_tbl_gra_no_order[1];
�Ƃ����ӂ��Ɏg�p����B

d_test_cmn_mons_no_tbl_gra_no_order[0]�͎g�p���Ȃ����ƁB
*/


// �o�b�N�O���E���h�J���[
typedef enum
{
  D_TEST_CMN_BG_COLOR_ENUM_BLACK,
  D_TEST_CMN_BG_COLOR_ENUM_LBLACK,
  D_TEST_CMN_BG_COLOR_ENUM_GRAY,
  D_TEST_CMN_BG_COLOR_ENUM_DWHITE,
  D_TEST_CMN_BG_COLOR_ENUM_WHITE,
  D_TEST_CMN_BG_COLOR_ENUM_RED,
  D_TEST_CMN_BG_COLOR_ENUM_GREEN,
  D_TEST_CMN_BG_COLOR_ENUM_BLUE,
  D_TEST_CMN_BG_COLOR_ENUM_MAX,
}
D_TEST_CMN_BG_COLOR_ENUM;

static const u16 d_test_cmn_bg_color_enum[D_TEST_CMN_BG_COLOR_ENUM_MAX] =
{
  0x0000,
  0x2108,
  0x4210,
  0x6318,
  0x7fff,
  0x0f3e,
  0x2790,
  0x7b10,
};

// �e�L�X�g
enum
{
  D_TEST_CMN_TEXT_DUMMY,
  
  D_TEST_CMN_TEXT_LABEL_POKE_NAME,
  D_TEST_CMN_TEXT_LABEL_POKE_MONS_NO,
  D_TEST_CMN_TEXT_LABEL_POKE_GRA_NO,
  D_TEST_CMN_TEXT_LABEL_POKE_FORM_NO,
  D_TEST_CMN_TEXT_LABEL_POKE_SEX,
  D_TEST_CMN_TEXT_LABEL_POKE_COLOR,
  
  D_TEST_CMN_TEXT_POKE_NAME,
  D_TEST_CMN_TEXT_POKE_MONS_NO,
  D_TEST_CMN_TEXT_POKE_GRA_NO,
  D_TEST_CMN_TEXT_POKE_FORM_NO,
  D_TEST_CMN_TEXT_POKE_SEX,
  D_TEST_CMN_TEXT_POKE_COLOR,

  D_TEST_CMN_TEXT_MAX,
};

static const u8 d_test_cmn_bmpwin_setup[D_TEST_CMN_TEXT_MAX][9] =
{
  // frmnum           posx  posy  sizx  sizy  palnum                     dir                    x  y (x,y�͖������ăZ���^�����O���邱�Ƃ�����)
  {  BG_FRAME_S_TEXT,    4,    2,   12,    2, BG_PAL_POS_S_FONT_DEFAULT, GFL_BMP_CHRAREA_GET_F, 0, 0 },
  
  {  BG_FRAME_S_TEXT,    4,    2,   12,    2, BG_PAL_POS_S_FONT_DEFAULT, GFL_BMP_CHRAREA_GET_F, 0, 0 },
  {  BG_FRAME_S_TEXT,    4,    4,   12,    2, BG_PAL_POS_S_FONT_DEFAULT, GFL_BMP_CHRAREA_GET_F, 0, 0 },
  {  BG_FRAME_S_TEXT,    4,    6,   12,    2, BG_PAL_POS_S_FONT_DEFAULT, GFL_BMP_CHRAREA_GET_F, 0, 0 },
  {  BG_FRAME_S_TEXT,    4,    8,   12,    2, BG_PAL_POS_S_FONT_DEFAULT, GFL_BMP_CHRAREA_GET_F, 0, 0 },
  {  BG_FRAME_S_TEXT,    4,   10,   12,    2, BG_PAL_POS_S_FONT_DEFAULT, GFL_BMP_CHRAREA_GET_F, 0, 0 },
  {  BG_FRAME_S_TEXT,    4,   12,   12,    2, BG_PAL_POS_S_FONT_DEFAULT, GFL_BMP_CHRAREA_GET_F, 0, 0 },
  
  {  BG_FRAME_S_TEXT,   18,    2,   12,    2, BG_PAL_POS_S_FONT_DEFAULT, GFL_BMP_CHRAREA_GET_F, 0, 0 },
  {  BG_FRAME_S_TEXT,   18,    4,   12,    2, BG_PAL_POS_S_FONT_DEFAULT, GFL_BMP_CHRAREA_GET_F, 0, 0 },
  {  BG_FRAME_S_TEXT,   18,    6,   12,    2, BG_PAL_POS_S_FONT_DEFAULT, GFL_BMP_CHRAREA_GET_F, 0, 0 },
  {  BG_FRAME_S_TEXT,   18,    8,   12,    2, BG_PAL_POS_S_FONT_DEFAULT, GFL_BMP_CHRAREA_GET_F, 0, 0 },
  {  BG_FRAME_S_TEXT,   18,   10,   12,    2, BG_PAL_POS_S_FONT_DEFAULT, GFL_BMP_CHRAREA_GET_F, 0, 0 },
  {  BG_FRAME_S_TEXT,   18,   12,   12,    2, BG_PAL_POS_S_FONT_DEFAULT, GFL_BMP_CHRAREA_GET_F, 0, 0 },
};

#define D_TEST_CMN_STRBUF_LEN (64)  // ������(���ꂾ������Α����ł��傤)

// �I�t�Z�b�g��ݒ肷��֐�
typedef void (D_Test_CmnMcssSetOfsPositionFunc)(void* d_test_cmn_wk);  // d_test_cmn_wk��(D_TEST_COMMON_WORK*)�ɃL���X�g���Ďg�p����


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
typedef struct
{
  // �q�[�v�A�p�����[�^�Ȃ�
  HEAPID                             heap_id;

  // �t�H���g�Ȃ�
  GFL_FONT*                          font;
  PRINT_QUE*                         print_que;

  // �o�b�N�O���E���h�J���[
  D_TEST_CMN_BG_COLOR_ENUM           bg_color_enum;

  // �e�L�X�g
  GFL_MSGDATA*                       msgdata;
  GFL_BMPWIN*                        text_bmpwin[D_TEST_CMN_TEXT_MAX];
  BOOL                               text_trans[D_TEST_CMN_TEXT_MAX];  // TRUE�̂Ƃ��]������K�v����

  // MCSS
  MCSS_SYS_WORK*                     mcss_sys_wk;
  MCSS_WORK*                         mcss_wk;

  VecFx32                            mcss_pos;
  VecFx32                            mcss_scale;
  D_Test_CmnMcssSetOfsPositionFunc*  mcss_set_ofs_position_func;

  // ���
  BOOL                               order_mons_no;   // mons_no���̂Ƃ�TRUE�Agra_no���̂Ƃ�FALSE
  BOOL                               mcss_anm_play;   // TRUE�̂Ƃ��A�j���Đ�����AFALSE�̂Ƃ��A�j���Đ����Ȃ�
  BOOL                               mcss_dir_front;  // TRUE�̂Ƃ��O�����AFALSE�̂Ƃ������

  // �|�P����
  int                                mons_no;
  int                                form_no;
  int                                sex;
  int                                rare;
  BOOL                               egg;
  int                                dir;
  u32                                personal_rnd;  // personal_rnd��mons_no==MONSNO_PATTIIRU�̂Ƃ��̂ݎg�p�����
}
D_TEST_COMMON_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// ���ʎg�p�ł��鎖��
static D_TEST_COMMON_WORK* D_Test_CmnInit( HEAPID heap_id );
static void D_Test_CmnExit( D_TEST_COMMON_WORK* work );
static void D_Test_CmnMain( D_TEST_COMMON_WORK* work );
static void D_Test_CmnDraw3D( D_TEST_COMMON_WORK* work );
static BOOL D_Test_CmnInput( D_TEST_COMMON_WORK* work );  // �I�����͂��������Ƃ�TRUE�A�p������Ƃ�FALSE��Ԃ�

static void D_Test_CmnTextInit( D_TEST_COMMON_WORK* work );
static void D_Test_CmnTextExit( D_TEST_COMMON_WORK* work );
static void D_Test_CmnTextMain( D_TEST_COMMON_WORK* work );


// gra_no����mons_no����ׂ��e�[�u��������
static void D_Test_CmnMakeMonsNoTblGraNoOrder(void);

// ����mons_no��n������mons_no�𓾂�B���̏��Ԃ�order_mons_no�ɂ�邪gra_no�����Ƃ��Ă�gra_no��Ԃ����Ƃ͂Ȃ��B
static u16 D_Test_CmnGetNextMonsNo( u16 mons_no_curr, BOOL order_mons_no );  // mons_no���̂Ƃ�TRUE�Agra_no���̂Ƃ�FALSE  // (MONSNO_END+1)��Ԃ����Ƃ����Ȃ�
static u16 D_Test_CmnGetPrevMonsNo( u16 mons_no_curr, BOOL order_mons_no );  // mons_no���̂Ƃ�TRUE�Agra_no���̂Ƃ�FALSE  // (0)��Ԃ����Ƃ��O�Ȃ�

static u16 D_Test_CmnGetFirstMonsNo( BOOL order_mons_no );  // mons_no���̂Ƃ�TRUE�Agra_no���̂Ƃ�FALSE
static u16 D_Test_CmnGetLastMonsNo( BOOL order_mons_no );  // mons_no���̂Ƃ�TRUE�Agra_no���̂Ƃ�FALSE


static void D_Test_CmnBgColorEnumSetNext(
    D_TEST_COMMON_WORK*     work );

static void D_Test_CmnMcssSet(
    D_TEST_COMMON_WORK*                work,
    const VecFx32*                     mcss_pos,
    const VecFx32*                     mcss_scale,
    D_Test_CmnMcssSetOfsPositionFunc*  mcss_set_ofs_position_func );

// �|�P�����؂�ւ�
static void D_Test_CmnPokeChange(
    D_TEST_COMMON_WORK* work,
    int mons_no, int form_no, int sex, int rare, BOOL egg, int dir,
    u32 personal_rnd );
static void D_Test_CmnPokeChangeOnlyData(
    D_TEST_COMMON_WORK* work,
    int mons_no, int form_no, int sex, int rare, BOOL egg, int dir,
    u32 personal_rnd );
static void D_Test_CmnPokeUpdateText( D_TEST_COMMON_WORK* work );
static void D_Test_CmnPokeAnmFlip( D_TEST_COMMON_WORK* work );
static void D_Test_CmnPokeDirFlip( D_TEST_COMMON_WORK* work );

// ���̃t�H�������𓾂�(���̃|�P�����̎��̃t�H�������Ȃ��Ƃ�FALSE��Ԃ��A���̂Ƃ��̈������߂�l�ɂ̓S�~�������Ă���)
static BOOL D_Test_CmnPokeGetNextForm(
    D_TEST_COMMON_WORK* work,
    int* mons_no, int* form_no, int* sex, int* rare, BOOL* egg, int* dir,  // �������߂�l
    u32* personal_rnd );                                                   // �������߂�l
// �O�̃t�H�������𓾂�(���̃|�P�����̑O�̃t�H�������Ȃ��Ƃ�FALSE��Ԃ��A���̂Ƃ��̈������߂�l�ɂ̓S�~�������Ă���)
static BOOL D_Test_CmnPokeGetPrevForm(
    D_TEST_COMMON_WORK* work,
    int* mons_no, int* form_no, int* sex, int* rare, BOOL* egg, int* dir,  // �������߂�l
    u32* personal_rnd );                                                   // �������߂�l

// ���̃|�P�������𓾂�(���̃|�P�������Ȃ��Ƃ�FALSE��Ԃ��A���̂Ƃ��̈������߂�l�ɂ̓S�~�������Ă���)
static BOOL D_Test_CmnPokeGetNextPoke(
    D_TEST_COMMON_WORK* work,
    int* mons_no, int* form_no, int* sex, int* rare, BOOL* egg, int* dir,  // �������߂�l
    u32* personal_rnd );                                                   // �������߂�l
// �O�̃|�P�������𓾂�(�O�̃|�P�������Ȃ��Ƃ�FALSE��Ԃ��A���̂Ƃ��̈������߂�l�ɂ̓S�~�������Ă���)
static BOOL D_Test_CmnPokeGetPrevPoke(
    D_TEST_COMMON_WORK* work,
    int* mons_no, int* form_no, int* sex, int* rare, BOOL* egg, int* dir,  // �������߂�l
    u32* personal_rnd );                                                   // �������߂�l
// �O�̃|�P�����̍Ō�̃t�H�������𓾂�(�O�̃|�P�����Ȃ��Ƃ�FALSE��Ԃ��A���̂Ƃ��̈������߂�l�ɂ̓S�~�������Ă���)
static BOOL D_Test_CmnPokeGetPrevPokeLastForm(
    D_TEST_COMMON_WORK* work,
    int* mons_no, int* form_no, int* sex, int* rare, BOOL* egg, int* dir,  // �������߂�l
    u32* personal_rnd );                                                   // �������߂�l

// �ŏ��̃|�P�����̍ŏ��̃t�H�������𓾂�
static void D_Test_CmnPokeGetFirstPokeFirstForm(
    D_TEST_COMMON_WORK* work,
    int* mons_no, int* form_no, int* sex, int* rare, BOOL* egg, int* dir,  // �������߂�l
    u32* personal_rnd );                                                   // �������߂�l
// �Ō�̃|�P�����̍Ō�̃t�H�������𓾂�
static void D_Test_CmnPokeGetLastPokeLastForm(
    D_TEST_COMMON_WORK* work,
    int* mons_no, int* form_no, int* sex, int* rare, BOOL* egg, int* dir,  // �������߂�l
    u32* personal_rnd );                                                   // �������߂�l
// �Ō�̃|�P�����̍ŏ��̃t�H�������𓾂�
static void D_Test_CmnPokeGetLastPokeFirstForm(
    D_TEST_COMMON_WORK* work,
    int* mons_no, int* form_no, int* sex, int* rare, BOOL* egg, int* dir,  // �������߂�l
    u32* personal_rnd );                                                   // �������߂�l

// �|�P�����p�[�\�i���f�[�^�̏�񂩂�A�t�H�������Ɛ��ʃx�N�g���𓾂�
static void D_Test_CmnPokeGetPersonalData(
    D_TEST_COMMON_WORK* work,
    u16 mons_no,
    u32* ppd_form_max, u32* ppd_sex );  // �������߂�l


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// ���ʎg�p�ł��鎖��
//=====================================
static D_TEST_COMMON_WORK* D_Test_CmnInit( HEAPID heap_id )
{
  // �q�[�v�A�p�����[�^�Ȃ�
  D_TEST_COMMON_WORK* work = GFL_HEAP_AllocClearMemory( heap_id, sizeof(D_TEST_COMMON_WORK) );
  work->heap_id = heap_id;

  // �t�H���g�Ȃ�
  {
    work->font          = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, work->heap_id );
    work->print_que     = PRINTSYS_QUE_Create( work->heap_id );

    // �p���b�g
    GFL_ARC_UTIL_TransVramPaletteEx(
        ARCID_FONT,
        NARC_font_default_nclr,
        PALTYPE_SUB_BG,
        0,
        BG_PAL_POS_S_FONT_DEFAULT * 0x20,
        BG_PAL_NUM_S_FONT_DEFAULT * 0x20,
        work->heap_id );
  }

  // �o�b�N�O���E���h�J���[
  work->bg_color_enum = D_TEST_CMN_BG_COLOR_ENUM_BLACK;
  D_Test_CmnBgColorEnumSetNext( work );

  // MCSS
  work->mcss_sys_wk = MCSS_Init( 1, work->heap_id );
  MCSS_SetTextureTransAdrs( work->mcss_sys_wk, 0 );  // ���Ɉꏏ�ɏo��3D�͂Ȃ��̂�
  MCSS_SetOrthoMode( work->mcss_sys_wk );
  MCSS_OpenHandle( work->mcss_sys_wk, POKEGRA_GetArcID() );

  // 0, NULL������
  {
    VecFx32  mcss_pos    = { 0, 0, 0 };
    VecFx32  mcss_scale  = { 0, 0, 0 };

    work->mcss_wk = NULL;

    D_Test_CmnMcssSet(
      work,
      &mcss_pos,
      &mcss_scale,
      NULL );
  }

  // ���
  work->order_mons_no  = FALSE;//TRUE;
  work->mcss_anm_play  = TRUE;
  work->mcss_dir_front = TRUE;

  // �e�L�X�g
  D_Test_CmnTextInit( work );

  // gra_no����mons_no����ׂ��e�[�u��������
  D_Test_CmnMakeMonsNoTblGraNoOrder();

  return work;
}
static void D_Test_CmnExit( D_TEST_COMMON_WORK* work )
{
  // �e�L�X�g
  D_Test_CmnTextExit( work );

  // MCSS
  if( work->mcss_wk )
  {
    MCSS_SetVanishFlag( work->mcss_wk );
    MCSS_Del( work->mcss_sys_wk, work->mcss_wk );
  }
 
  MCSS_CloseHandle( work->mcss_sys_wk );
  MCSS_Exit( work->mcss_sys_wk );

  // �t�H���g�Ȃ�
  {
    PRINTSYS_QUE_Clear( work->print_que );
    PRINTSYS_QUE_Delete( work->print_que );
    GFL_FONT_Delete( work->font );
  }

  // �q�[�v�A�p�����[�^�Ȃ�
  GFL_HEAP_FreeMemory( work );
}
static void D_Test_CmnMain( D_TEST_COMMON_WORK* work )
{
  D_Test_CmnTextMain( work );

  PRINTSYS_QUE_Main( work->print_que );

  // MCSS
  MCSS_Main( work->mcss_sys_wk );
}
static void D_Test_CmnDraw3D( D_TEST_COMMON_WORK* work )
{
  // MCSS
  MCSS_Draw( work->mcss_sys_wk );
}
static BOOL D_Test_CmnInput( D_TEST_COMMON_WORK* work )  // �I�����͂��������Ƃ�TRUE�A�p������Ƃ�FALSE��Ԃ�
{
  int trg    = GFL_UI_KEY_GetTrg();
  int repeat = GFL_UI_KEY_GetRepeat();

  int  mons_no;
  int  form_no;
  int  sex;
  int  rare;
  BOOL egg;
  int  dir;
  u32  personal_rnd;

  BOOL ret = FALSE;

  u8 i;

  // ���̃t�H����
  if( repeat & PAD_KEY_DOWN )
  {
    ret = D_Test_CmnPokeGetNextForm( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
    if( !ret )
    {
      ret = D_Test_CmnPokeGetNextPoke( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
      if( !ret ) D_Test_CmnPokeGetFirstPokeFirstForm( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
    }
    D_Test_CmnPokeChange( work, mons_no, form_no, sex, rare, egg, dir, personal_rnd );
  }
  else if( repeat & PAD_KEY_UP )
  {
    ret = D_Test_CmnPokeGetPrevForm( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
    if( !ret )
    {
      ret = D_Test_CmnPokeGetPrevPokeLastForm( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
      if( !ret ) D_Test_CmnPokeGetLastPokeLastForm( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
    }
    D_Test_CmnPokeChange( work, mons_no, form_no, sex, rare, egg, dir, personal_rnd );
  }

  // ���̃|�P����
  else if( repeat & PAD_KEY_RIGHT )
  {
    ret = D_Test_CmnPokeGetNextPoke( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
    if( !ret ) D_Test_CmnPokeGetFirstPokeFirstForm( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
    D_Test_CmnPokeChange( work, mons_no, form_no, sex, rare, egg, dir, personal_rnd );
  }
  else if( repeat & PAD_KEY_LEFT )
  {
    ret = D_Test_CmnPokeGetPrevPoke( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
    if( !ret ) D_Test_CmnPokeGetLastPokeFirstForm( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
    D_Test_CmnPokeChange( work, mons_no, form_no, sex, rare, egg, dir, personal_rnd );
  }

  // 10��̃|�P����
  else if( repeat & PAD_BUTTON_R )
  {
    for( i=0; i<10; i++ )
    {
      ret = D_Test_CmnPokeGetNextPoke( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
      if( ret )
      {
        D_Test_CmnPokeChangeOnlyData( work, mons_no, form_no, sex, rare, egg, dir, personal_rnd );
      }
      else
      {
        D_Test_CmnPokeGetFirstPokeFirstForm( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
        break;
      }
    } 
    D_Test_CmnPokeChange( work, mons_no, form_no, sex, rare, egg, dir, personal_rnd );
  }
  else if( repeat & PAD_BUTTON_L )
  {
    for( i=0; i<10; i++ )
    {
      ret = D_Test_CmnPokeGetPrevPoke( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
      if( ret )
      {
        D_Test_CmnPokeChangeOnlyData( work, mons_no, form_no, sex, rare, egg, dir, personal_rnd );
      }
      else
      {
        D_Test_CmnPokeGetLastPokeFirstForm( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
        break;
      }
    }
    D_Test_CmnPokeChange( work, mons_no, form_no, sex, rare, egg, dir, personal_rnd );
  }

  // 50��̃|�P����
  else if( repeat & PAD_BUTTON_A )
  {
    for( i=0; i<50; i++ )
    {
      ret = D_Test_CmnPokeGetNextPoke( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
      if( ret )
      {
        D_Test_CmnPokeChangeOnlyData( work, mons_no, form_no, sex, rare, egg, dir, personal_rnd );
      }
      else
      {
        D_Test_CmnPokeGetFirstPokeFirstForm( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
        break;
      }
    } 
    D_Test_CmnPokeChange( work, mons_no, form_no, sex, rare, egg, dir, personal_rnd );
  }
  else if( repeat & PAD_BUTTON_Y )
  {
    for( i=0; i<50; i++ )
    {
      ret = D_Test_CmnPokeGetPrevPoke( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
      if( ret )
      {
        D_Test_CmnPokeChangeOnlyData( work, mons_no, form_no, sex, rare, egg, dir, personal_rnd );
      }
      else
      {
        D_Test_CmnPokeGetLastPokeFirstForm( work, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
        break;
      }
    }
    D_Test_CmnPokeChange( work, mons_no, form_no, sex, rare, egg, dir, personal_rnd );
  }

  // �A�j���[�V�����̍Đ��A��~
  else if( trg & PAD_BUTTON_X )
  {
    D_Test_CmnPokeAnmFlip( work );
  }

  // �O��̕ύX
  else if( trg & PAD_BUTTON_START )
  {
    D_Test_CmnPokeDirFlip( work );
  }

  // �o�b�N�O���E���h�J���[�̕ύX
  else if( trg & PAD_BUTTON_SELECT )
  {
    D_Test_CmnBgColorEnumSetNext( work );
  }

  // �I��
  else if( trg & PAD_BUTTON_B )
  {
    return TRUE;
  }

  return FALSE;
}

static void D_Test_CmnTextInit( D_TEST_COMMON_WORK* work )
{
  u8 i;

  // �o�b�N�O���E���h�J���[
  GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, 0x7fff );

  // ���b�Z�[�W
  work->msgdata = GFL_MSG_Create(
                      GFL_MSG_LOAD_NORMAL,
                      ARCID_DEBUG_MESSAGE,
                      NARC_debug_message_d_kawada_dat,
                      work->heap_id );

  // �r�b�g�}�b�v�E�B���h�E
  for( i=0; i<D_TEST_CMN_TEXT_MAX; i++ )
  {
    work->text_bmpwin[i] = GFL_BMPWIN_Create(
                               d_test_cmn_bmpwin_setup[i][0],
                               d_test_cmn_bmpwin_setup[i][1], d_test_cmn_bmpwin_setup[i][2], d_test_cmn_bmpwin_setup[i][3], d_test_cmn_bmpwin_setup[i][4],
                               d_test_cmn_bmpwin_setup[i][5], d_test_cmn_bmpwin_setup[i][6] );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[i]), 0 );
    GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[i] );

    work->text_trans[i] = FALSE;
  }

  // �Œ�e�L�X�g
  {
    u8 i;
    for( i=D_TEST_CMN_TEXT_LABEL_POKE_NAME; i<=D_TEST_CMN_TEXT_LABEL_POKE_COLOR; i++ )
    {
      STRBUF* src_strbuf = GFL_MSG_CreateString( work->msgdata, label_poke_name +i -D_TEST_CMN_TEXT_LABEL_POKE_NAME );
      PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp( work->text_bmpwin[i] ),
                              d_test_cmn_bmpwin_setup[i][7], d_test_cmn_bmpwin_setup[i][8],
                              src_strbuf, work->font, PRINTSYS_LSB_Make(1,2,0) );  
      GFL_STR_DeleteBuffer( src_strbuf );
      work->text_trans[i] = TRUE;
    }
    D_Test_CmnTextMain( work );
  }
}
static void D_Test_CmnTextExit( D_TEST_COMMON_WORK* work )
{
  u8 i;
 
  // print_que
  PRINTSYS_QUE_Clear( work->print_que );

  // �r�b�g�}�b�v�E�B���h�E
  for( i=0; i<D_TEST_CMN_TEXT_MAX; i++ )
  {
    GFL_BMPWIN_Delete( work->text_bmpwin[i] );
    work->text_trans[i] = FALSE;
  }

  // ���b�Z�[�W
  GFL_MSG_Delete( work->msgdata );
}
static void D_Test_CmnTextMain( D_TEST_COMMON_WORK* work )
{
  u8 i;

  for( i=0; i<D_TEST_CMN_TEXT_MAX; i++ )
  {
    if( work->text_trans[i] )
    {
      if( !PRINTSYS_QUE_IsExistTarget( work->print_que, GFL_BMPWIN_GetBmp(work->text_bmpwin[i]) ) )
      {
        GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[i] );
        work->text_trans[i] = FALSE;
      }
    }
  }
}

// gra_no����mons_no����ׂ��e�[�u��������
static void D_Test_CmnMakeMonsNoTblGraNoOrder(void)
{
  // �o�u���\�[�g
  u16 mons_no_tbl[MONSNO_END +1];
  s16 i, j;

  // ��������
  for( i=0; i<=MONSNO_END; i++ )
  {
    mons_no_tbl[i] = i;
  }

  // �o�u���\�[�g���s
  for( i=MONSNO_END; i>=1; i-- )
  {
    for( j=i-1; j>=0; j-- )
    {
      if( d_test_cmn_mons_no_to_gra_no_tbl[ mons_no_tbl[i] ] < d_test_cmn_mons_no_to_gra_no_tbl[ mons_no_tbl[j] ] )
      {
        u16 no = mons_no_tbl[i];
        mons_no_tbl[i] = mons_no_tbl[j];
        mons_no_tbl[j] = no;
      }
    }
  }

  // ����
  for( i=0; i<=MONSNO_END; i++ )
  {
    d_test_cmn_mons_no_tbl_gra_no_order[i] = mons_no_tbl[i];
    //OS_Printf( "[%3d] mons_no=%3d, gra_no=%3d\n",
    //    i,
    //    d_test_cmn_mons_no_tbl_gra_no_order[i],
    //    d_test_cmn_mons_no_to_gra_no_tbl[ d_test_cmn_mons_no_tbl_gra_no_order[i] ] );
  }
}

// ����mons_no��n������mons_no�𓾂�B���̏��Ԃ�order_mons_no�ɂ�邪gra_no�����Ƃ��Ă�gra_no��Ԃ����Ƃ͂Ȃ��B
static u16 D_Test_CmnGetNextMonsNo( u16 mons_no_curr, BOOL order_mons_no )  // mons_no���̂Ƃ�TRUE�Agra_no���̂Ƃ�FALSE  // (MONSNO_END+1)��Ԃ����Ƃ����Ȃ�
{
  if( mons_no_curr <= 0 || MONSNO_END < mons_no_curr )
  {
    return (MONSNO_END+1);
  }
  
  if( order_mons_no )
  {
    return (mons_no_curr+1);
  }
  else
  {
    s16 i;

    for( i=0; i<=MONSNO_END; i++ )
    {
      if( d_test_cmn_mons_no_tbl_gra_no_order[i] == mons_no_curr )
      {
        break;
      }
    }

    if( i<=0 || MONSNO_END<=i )  // i==MONSNO_END�̂Ƃ��͎���MONSNO_END+1�Ȃ̂łȂ�
    {
      return (MONSNO_END+1);
    }
    else
    {
      return d_test_cmn_mons_no_tbl_gra_no_order[i+1];
    }
  }
}
static u16 D_Test_CmnGetPrevMonsNo( u16 mons_no_curr, BOOL order_mons_no )  // mons_no���̂Ƃ�TRUE�Agra_no���̂Ƃ�FALSE  // (0)��Ԃ����Ƃ��O�Ȃ�
{ 
  if( mons_no_curr <= 0 || MONSNO_END < mons_no_curr )
  {
    return (0);
  }

  if( order_mons_no )
  {
    return (mons_no_curr-1);
  }
  else
  {
    s16 i;
    for( i=0; i<=MONSNO_END; i++ )
    {
      if( d_test_cmn_mons_no_tbl_gra_no_order[i] == mons_no_curr )
      {
        break;
      }
    }

    if( i<=1 || MONSNO_END<i )  // i==1�̂Ƃ��͑O��0�Ȃ̂łȂ�
    {
      return (0);
    }
    else
    {
      return d_test_cmn_mons_no_tbl_gra_no_order[i-1];
    }
  }
}

static u16 D_Test_CmnGetFirstMonsNo( BOOL order_mons_no )  // mons_no���̂Ƃ�TRUE�Agra_no���̂Ƃ�FALSE
{
  if( order_mons_no )
  {
    return MONSNO_HUSIGIDANE;
  }
  else
  {
    return d_test_cmn_mons_no_tbl_gra_no_order[1];
  }
}
static u16 D_Test_CmnGetLastMonsNo( BOOL order_mons_no )  // mons_no���̂Ƃ�TRUE�Agra_no���̂Ƃ�FALSE
{
  if( order_mons_no )
  {
    return MONSNO_END;
  }
  else
  {
    return d_test_cmn_mons_no_tbl_gra_no_order[MONSNO_END];
  }
}




static void D_Test_CmnBgColorEnumSetNext(
    D_TEST_COMMON_WORK*     work )
{
  work->bg_color_enum++;
  if( work->bg_color_enum >= D_TEST_CMN_BG_COLOR_ENUM_MAX )
  {
    work->bg_color_enum = D_TEST_CMN_BG_COLOR_ENUM_BLACK;
  }
  GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, d_test_cmn_bg_color_enum[work->bg_color_enum] );
}

static void D_Test_CmnMcssSet(
    D_TEST_COMMON_WORK*                work,
    const VecFx32*                     mcss_pos,
    const VecFx32*                     mcss_scale,
    D_Test_CmnMcssSetOfsPositionFunc*  mcss_set_ofs_position_func )
{
  work->mcss_pos                     = *mcss_pos;
  work->mcss_scale                   = *mcss_scale;
  work->mcss_set_ofs_position_func   = mcss_set_ofs_position_func;
}


//-------------------------------------
/// �|�P�����؂�ւ�
//=====================================
static void D_Test_CmnPokeChange(
    D_TEST_COMMON_WORK* work,
    int mons_no, int form_no, int sex, int rare, BOOL egg, int dir,
    u32 personal_rnd )
{
  // �O�̂��폜
  {
    // MCSS
    if( work->mcss_wk )
    {
      MCSS_SetVanishFlag( work->mcss_wk );
      MCSS_Del( work->mcss_sys_wk, work->mcss_wk );
    }
  }

  // ���̂𐶐�
  {
    // MCSS
    MCSS_ADD_WORK add_wk;

    if( mons_no == MONSNO_PATTIIRU )  // MCSS_TOOL_AddPokeMcss�̒��g�ɍ��킹�Ă���
    {
      MCSS_TOOL_SetMakeBuchiCallback( work->mcss_sys_wk, personal_rnd );
    }
    MCSS_TOOL_MakeMAWParam( mons_no, form_no, sex, rare, egg, &add_wk, dir );
    work->mcss_wk = MCSS_Add( work->mcss_sys_wk, work->mcss_pos.x, work->mcss_pos.y, work->mcss_pos.z, &add_wk );
    MCSS_SetShadowVanishFlag( work->mcss_wk, TRUE );  // �e�������Ă����Ȃ��ƁA�����ȓ_�Ƃ��ĉe���\������Ă��܂��B
    MCSS_SetScale( work->mcss_wk, &work->mcss_scale );
    if( work->mcss_set_ofs_position_func )
    {
      work->mcss_set_ofs_position_func( work );
    }

    if( work->mcss_anm_play )
    {
      MCSS_SetAnimeIndex( work->mcss_wk, 0 );
      MCSS_ResetAnmStopFlag( work->mcss_wk );
      MCSS_TOOL_SetAnmRestartCallback( work->mcss_wk );  // 1���[�v������A�j���[�V�������Z�b�g���ĂԂ��߂̃R�[���o�b�N�Z�b�g
    }
    else
    {
      MCSS_SetAnmStopFlag( work->mcss_wk );
    }
  }

  // �\����ύX�����̂ŁA�f�[�^���ύX���Ă���
  {
    D_Test_CmnPokeChangeOnlyData(
      work,
      mons_no, form_no, sex, rare, egg, dir,
      personal_rnd );
  }

  // �e�L�X�g���ύX���Ă���
  D_Test_CmnPokeUpdateText( work );
}

static void D_Test_CmnPokeChangeOnlyData(
    D_TEST_COMMON_WORK* work,
    int mons_no, int form_no, int sex, int rare, BOOL egg, int dir,
    u32 personal_rnd )
{
  // �\���͕ύX�����f�[�^�����ύX����
  {
    work->mons_no        = mons_no;
    work->form_no        = form_no;
    work->sex            = sex;
    work->rare           = rare;
    work->egg            = egg;
    work->dir            = dir;
    work->personal_rnd   = personal_rnd;
  }
}

static void D_Test_CmnPokeUpdateText( D_TEST_COMMON_WORK* work )
{
  // ���݂̏�ԂŃe�L�X�g���X�V����

  // �|�P������ �s�J�`���E
  {
    STRBUF* src_strbuf = GFL_MSG_CreateString( GlobalMsg_PokeName, work->mons_no );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[D_TEST_CMN_TEXT_POKE_NAME]), 0 );
    PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp( work->text_bmpwin[D_TEST_CMN_TEXT_POKE_NAME] ),
                            d_test_cmn_bmpwin_setup[D_TEST_CMN_TEXT_POKE_NAME][7], d_test_cmn_bmpwin_setup[D_TEST_CMN_TEXT_POKE_NAME][8],
                            src_strbuf, work->font, PRINTSYS_LSB_Make(1,2,0) );  
    GFL_STR_DeleteBuffer( src_strbuf );
    work->text_trans[D_TEST_CMN_TEXT_POKE_NAME] = TRUE;
  }

  // �S���}��No 025
  {
    STRBUF* src_strbuf = GFL_MSG_CreateString( work->msgdata, poke_mons_no );
    STRBUF* dst_strbuf = GFL_STR_CreateBuffer( D_TEST_CMN_STRBUF_LEN, work->heap_id );
    WORDSET* wordset = WORDSET_Create( work->heap_id );
    WORDSET_RegisterNumber( wordset, 0, work->mons_no, 3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_ExpandStr( wordset, dst_strbuf, src_strbuf );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[D_TEST_CMN_TEXT_POKE_MONS_NO]), 0 );
    PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp( work->text_bmpwin[D_TEST_CMN_TEXT_POKE_MONS_NO] ),
                            d_test_cmn_bmpwin_setup[D_TEST_CMN_TEXT_POKE_MONS_NO][7], d_test_cmn_bmpwin_setup[D_TEST_CMN_TEXT_POKE_MONS_NO][8],
                            dst_strbuf, work->font, PRINTSYS_LSB_Make(1,2,0) );  
    WORDSET_Delete( wordset );
    GFL_STR_DeleteBuffer( dst_strbuf );
    GFL_STR_DeleteBuffer( src_strbuf );
    work->text_trans[D_TEST_CMN_TEXT_POKE_MONS_NO] = TRUE;
  }

  // �O���t�B�b�NNo 025
  {
    STRBUF* src_strbuf = GFL_MSG_CreateString( work->msgdata, poke_gra_no );
    STRBUF* dst_strbuf = GFL_STR_CreateBuffer( D_TEST_CMN_STRBUF_LEN, work->heap_id );
    WORDSET* wordset = WORDSET_Create( work->heap_id );
    WORDSET_RegisterNumber( wordset, 0, d_test_cmn_mons_no_to_gra_no_tbl[work->mons_no], 3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_ExpandStr( wordset, dst_strbuf, src_strbuf );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[D_TEST_CMN_TEXT_POKE_GRA_NO]), 0 );
    PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp( work->text_bmpwin[D_TEST_CMN_TEXT_POKE_GRA_NO] ),
                            d_test_cmn_bmpwin_setup[D_TEST_CMN_TEXT_POKE_GRA_NO][7], d_test_cmn_bmpwin_setup[D_TEST_CMN_TEXT_POKE_GRA_NO][8],
                            dst_strbuf, work->font, PRINTSYS_LSB_Make(1,2,0) );  
    WORDSET_Delete( wordset );
    GFL_STR_DeleteBuffer( dst_strbuf );
    GFL_STR_DeleteBuffer( src_strbuf );
    work->text_trans[D_TEST_CMN_TEXT_POKE_GRA_NO] = TRUE;
  }

  // �t�H����No 00
  {
    STRBUF* src_strbuf = GFL_MSG_CreateString( work->msgdata, poke_form_no );
    STRBUF* dst_strbuf = GFL_STR_CreateBuffer( D_TEST_CMN_STRBUF_LEN, work->heap_id );
    WORDSET* wordset = WORDSET_Create( work->heap_id );
    WORDSET_RegisterNumber( wordset, 0, work->form_no, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_ExpandStr( wordset, dst_strbuf, src_strbuf );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[D_TEST_CMN_TEXT_POKE_FORM_NO]), 0 );
    PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp( work->text_bmpwin[D_TEST_CMN_TEXT_POKE_FORM_NO] ),
                            d_test_cmn_bmpwin_setup[D_TEST_CMN_TEXT_POKE_FORM_NO][7], d_test_cmn_bmpwin_setup[D_TEST_CMN_TEXT_POKE_FORM_NO][8],
                            dst_strbuf, work->font, PRINTSYS_LSB_Make(1,2,0) );  
    WORDSET_Delete( wordset );
    GFL_STR_DeleteBuffer( dst_strbuf );
    GFL_STR_DeleteBuffer( src_strbuf );
    work->text_trans[D_TEST_CMN_TEXT_POKE_FORM_NO] = TRUE;
  }

  // ����
  {
    STRBUF* src_strbuf;
    switch( work->sex )
    {
    case PTL_SEX_MALE:
      {
        src_strbuf = GFL_MSG_CreateString( work->msgdata, poke_sex_male );
      }
      break;
    case PTL_SEX_FEMALE:
      {
        src_strbuf = GFL_MSG_CreateString( work->msgdata, poke_sex_female );
      }
      break;
    case PTL_SEX_UNKNOWN:
      {
        src_strbuf = GFL_MSG_CreateString( work->msgdata, poke_sex_none );
      }
      break;
    }
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[D_TEST_CMN_TEXT_POKE_SEX]), 0 );
    PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp( work->text_bmpwin[D_TEST_CMN_TEXT_POKE_SEX] ),
                            d_test_cmn_bmpwin_setup[D_TEST_CMN_TEXT_POKE_SEX][7], d_test_cmn_bmpwin_setup[D_TEST_CMN_TEXT_POKE_SEX][8],
                            src_strbuf, work->font, PRINTSYS_LSB_Make(1,2,0) );  
    GFL_STR_DeleteBuffer( src_strbuf );
    work->text_trans[D_TEST_CMN_TEXT_POKE_SEX] = TRUE;
  }

  // �F
  {
    STRBUF* src_strbuf;
    if( work->rare == 0 )  // �m�[�}��
    {
      src_strbuf = GFL_MSG_CreateString( work->msgdata, poke_color_normal );
    }
    else                   // ���A
    {
      src_strbuf = GFL_MSG_CreateString( work->msgdata, poke_color_rare );
    }
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[D_TEST_CMN_TEXT_POKE_COLOR]), 0 );
    PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp( work->text_bmpwin[D_TEST_CMN_TEXT_POKE_COLOR] ),
                            d_test_cmn_bmpwin_setup[D_TEST_CMN_TEXT_POKE_COLOR][7], d_test_cmn_bmpwin_setup[D_TEST_CMN_TEXT_POKE_COLOR][8],
                            src_strbuf, work->font, PRINTSYS_LSB_Make(1,2,0) );  
    GFL_STR_DeleteBuffer( src_strbuf );
    work->text_trans[D_TEST_CMN_TEXT_POKE_COLOR] = TRUE;
  }

  D_Test_CmnTextMain( work );
}

static void D_Test_CmnPokeAnmFlip( D_TEST_COMMON_WORK* work )
{
  if( work->mcss_anm_play )
  {
    work->mcss_anm_play = FALSE;
  }
  else
  {
    work->mcss_anm_play = TRUE;
  }

  if( work->mcss_wk )
  {
    if( work->mcss_anm_play )
    {
      MCSS_SetAnimeIndex( work->mcss_wk, 0 );
      MCSS_ResetAnmStopFlag( work->mcss_wk );
      MCSS_TOOL_SetAnmRestartCallback( work->mcss_wk );  // 1���[�v������A�j���[�V�������Z�b�g���ĂԂ��߂̃R�[���o�b�N�Z�b�g
    }
    else
    {
      MCSS_SetAnmStopFlag( work->mcss_wk );
    }
  }
}

static void D_Test_CmnPokeDirFlip( D_TEST_COMMON_WORK* work )
{
  if( work->mcss_dir_front )
  {
    work->mcss_dir_front = FALSE;
    work->dir = MCSS_DIR_BACK;
  }
  else
  {
    work->mcss_dir_front = TRUE;
    work->dir = MCSS_DIR_FRONT;
  }

  D_Test_CmnPokeChange(
    work,
    work->mons_no, work->form_no, work->sex, work->rare, work->egg, work->dir,
    work->personal_rnd );
}


//-------------------------------------
/// ���̃t�H�������𓾂�(���̃|�P�����̎��̃t�H�������Ȃ��Ƃ�FALSE��Ԃ��A���̂Ƃ��̈������߂�l�ɂ̓S�~�������Ă���)
//=====================================
static BOOL D_Test_CmnPokeGetNextForm(
    D_TEST_COMMON_WORK* work,
    int* mons_no, int* form_no, int* sex, int* rare, BOOL* egg, int* dir,
    u32* personal_rnd )
{
  u32 ppd_form_max;
  u32 ppd_sex;

  D_Test_CmnPokeGetPersonalData(
      work, 
      work->mons_no,
      &ppd_form_max,
      &ppd_sex );

  if( ppd_form_max <= 1 )  // �t�H�����Ⴂ�Ȃ�
  {
    if( work->rare == 0 )  // ���m�[�}���J���[�Ȃ玟�̓��A�J���[�ɂ���
    {
      *mons_no       = work->mons_no;
      *form_no       = work->form_no;
      *sex           = work->sex;
      *rare          = 1;
      *egg           = work->egg;
      *dir           = work->dir;
      *personal_rnd  = work->personal_rnd;
      return TRUE;
    }
    else  // �����A�J���[�Ȃ玟�͎��̐��ʂ̃m�[�}���J���[�ɂ���
    {
      switch( ppd_sex )
      {
      case POKEPER_SEX_MALE:      // ���ʌŒ�
      case POKEPER_SEX_FEMALE:
      case POKEPER_SEX_UNKNOWN:
        {
          return FALSE;
        }
        break;
      default:  // 1�`253        // �I�X�ƃ��X�A�ǂ�������݂���
        {
          if( work->sex == PTL_SEX_FEMALE )  // �����X�Ȃ̂Ŏ��̐��ʂȂ�
          {
            return FALSE;
          }
          else  // ���I�X�Ȃ̂Ŏ��̓��X�ɂ���
          {
            *mons_no       = work->mons_no;
            *form_no       = work->form_no;
            *sex           = PTL_SEX_FEMALE;
            *rare          = 0;
            *egg           = work->egg;
            *dir           = work->dir;
            *personal_rnd  = work->personal_rnd;
            return TRUE;
          }
        }
        break;
      }
    }
  }
  else  // �t�H�����Ⴂ����
  {
    if( work->rare == 0 )  // ���m�[�}���J���[�Ȃ玟�̓��A�J���[�ɂ���
    {
      *mons_no       = work->mons_no;
      *form_no       = work->form_no;
      *sex           = work->sex;
      *rare          = 1;
      *egg           = work->egg;
      *dir           = work->dir;
      *personal_rnd  = work->personal_rnd;
      return TRUE;
    }
    else  // �����A�J���[�Ȃ玟�͎��̃t�H�����̃m�[�}���J���[�ɂ���
    {
      if( work->form_no +1 >= ppd_form_max )  // ���̃t�H�����Ȃ�
      {
        return FALSE;
      }
      else
      {
        *mons_no       = work->mons_no;
        *form_no       = work->form_no +1;
        *sex           = work->sex;
        *rare          = 0;
        *egg           = work->egg;
        *dir           = work->dir;
        *personal_rnd  = work->personal_rnd;
        return TRUE;
      }
    }
  }
}
//-------------------------------------
/// �O�̃t�H�������𓾂�(���̃|�P�����̑O�̃t�H�������Ȃ��Ƃ�FALSE��Ԃ��A���̂Ƃ��̈������߂�l�ɂ̓S�~�������Ă���)
//=====================================
static BOOL D_Test_CmnPokeGetPrevForm(
    D_TEST_COMMON_WORK* work,
    int* mons_no, int* form_no, int* sex, int* rare, BOOL* egg, int* dir,  // �������߂�l
    u32* personal_rnd )                                                    // �������߂�l
{
  u32 ppd_form_max;
  u32 ppd_sex;

  D_Test_CmnPokeGetPersonalData(
      work, 
      work->mons_no,
      &ppd_form_max,
      &ppd_sex );

  if( ppd_form_max <= 1 )  // �t�H�����Ⴂ�Ȃ�
  {
    if( work->rare == 1 )  // �����A�J���[�Ȃ玟�̓m�[�}���J���[�ɂ���
    {
      *mons_no       = work->mons_no;
      *form_no       = work->form_no;
      *sex           = work->sex;
      *rare          = 0;
      *egg           = work->egg;
      *dir           = work->dir;
      *personal_rnd  = work->personal_rnd;
      return TRUE;
    }
    else  // ���m�[�}���J���[�Ȃ玟�͑O�̐��ʂ̃��A�J���[�ɂ���
    {
      switch( ppd_sex )
      {
      case POKEPER_SEX_MALE:      // ���ʌŒ�
      case POKEPER_SEX_FEMALE:
      case POKEPER_SEX_UNKNOWN:
        {
          return FALSE;
        }
        break;
      default:  // 1�`253        // �I�X�ƃ��X�A�ǂ�������݂���
        {
          if( work->sex == PTL_SEX_MALE )  // ���I�X�Ȃ̂őO�̐��ʂȂ�
          {
            return FALSE;
          }
          else  // �����X�Ȃ̂Ŏ��̓I�X�ɂ���
          {
            *mons_no       = work->mons_no;
            *form_no       = work->form_no;
            *sex           = PTL_SEX_MALE;
            *rare          = 1;
            *egg           = work->egg;
            *dir           = work->dir;
            *personal_rnd  = work->personal_rnd;
            return TRUE;
          }
        }
        break;
      }
    }
  }
  else  // �t�H�����Ⴂ����
  {
    if( work->rare == 1 )  // �����A�J���[�Ȃ玟�̓m�[�}���J���[�ɂ���
    {
      *mons_no       = work->mons_no;
      *form_no       = work->form_no;
      *sex           = work->sex;
      *rare          = 0;
      *egg           = work->egg;
      *dir           = work->dir;
      *personal_rnd  = work->personal_rnd;
      return TRUE;
    }
    else  // ���m�[�}���J���[�Ȃ玟�͑O�̃t�H�����̃��A�J���[�ɂ���
    {
      if( work->form_no <= 0 )  // �O�̃t�H�����Ȃ�
      {
        return FALSE;
      }
      else
      {
        *mons_no       = work->mons_no;
        *form_no       = work->form_no -1;
        *sex           = work->sex;
        *rare          = 1;
        *egg           = work->egg;
        *dir           = work->dir;
        *personal_rnd  = work->personal_rnd;
        return TRUE;
      }
    }
  }
}

//-------------------------------------
/// ���̃|�P�������𓾂�(���̃|�P�������Ȃ��Ƃ�FALSE��Ԃ��A���̂Ƃ��̈������߂�l�ɂ̓S�~�������Ă���)
//=====================================
static BOOL D_Test_CmnPokeGetNextPoke(
    D_TEST_COMMON_WORK* work,
    int* mons_no, int* form_no, int* sex, int* rare, BOOL* egg, int* dir,  // �������߂�l
    u32* personal_rnd )                                                    // �������߂�l
{
  *mons_no = D_Test_CmnGetNextMonsNo( work->mons_no, work->order_mons_no );

  if( *mons_no > MONSNO_END )
  {
    return FALSE;
  }
  else
  {
    u32 ppd_form_max;
    u32 ppd_sex;

    D_Test_CmnPokeGetPersonalData(
        work, 
        *mons_no,
        &ppd_form_max,
        &ppd_sex );

    *form_no = 0;

    switch( ppd_sex )
    {
    case POKEPER_SEX_MALE:
      {
        *sex = PTL_SEX_MALE;
      }
      break;
    case POKEPER_SEX_FEMALE:
      {
        *sex = PTL_SEX_FEMALE;
      }
      break;
    case POKEPER_SEX_UNKNOWN:
      {
        *sex = PTL_SEX_UNKNOWN;
      }
      break;
    default:  // 1�`253
      {
        *sex = PTL_SEX_MALE;
      }
      break;
    }

    *rare         = 0;
    *egg          = FALSE;
    *dir          = (work->mcss_dir_front)?(MCSS_DIR_FRONT):(MCSS_DIR_BACK);
    *personal_rnd = 0;

    return TRUE;
  }
}
//-------------------------------------
/// �O�̃|�P�������𓾂�(�O�̃|�P�������Ȃ��Ƃ�FALSE��Ԃ��A���̂Ƃ��̈������߂�l�ɂ̓S�~�������Ă���)
//=====================================
static BOOL D_Test_CmnPokeGetPrevPoke(
    D_TEST_COMMON_WORK* work,
    int* mons_no, int* form_no, int* sex, int* rare, BOOL* egg, int* dir,  // �������߂�l
    u32* personal_rnd )                                                    // �������߂�l
{
  *mons_no = D_Test_CmnGetPrevMonsNo( work->mons_no, work->order_mons_no );

  if( *mons_no <= 0 )
  {
    return FALSE;
  }
  else
  {
    u32 ppd_form_max;
    u32 ppd_sex;

    D_Test_CmnPokeGetPersonalData(
        work, 
        *mons_no,
        &ppd_form_max,
        &ppd_sex );

    *form_no = 0;

    switch( ppd_sex )
    {
    case POKEPER_SEX_MALE:
      {
        *sex = PTL_SEX_MALE;
      }
      break;
    case POKEPER_SEX_FEMALE:
      {
        *sex = PTL_SEX_FEMALE;
      }
      break;
    case POKEPER_SEX_UNKNOWN:
      {
        *sex = PTL_SEX_UNKNOWN;
      }
      break;
    default:  // 1�`253
      {
        *sex = PTL_SEX_MALE;
      }
      break;
    }

    *rare         = 0;
    *egg          = FALSE;
    *dir          = (work->mcss_dir_front)?(MCSS_DIR_FRONT):(MCSS_DIR_BACK);
    *personal_rnd = 0;

    return TRUE;
  }
}
//-------------------------------------
/// �O�̃|�P�����̍Ō�̃t�H�������𓾂�(�O�̃|�P�����Ȃ��Ƃ�FALSE��Ԃ��A���̂Ƃ��̈������߂�l�ɂ̓S�~�������Ă���)
//=====================================
static BOOL D_Test_CmnPokeGetPrevPokeLastForm(
    D_TEST_COMMON_WORK* work,
    int* mons_no, int* form_no, int* sex, int* rare, BOOL* egg, int* dir,  // �������߂�l
    u32* personal_rnd )                                                    // �������߂�l
{
  *mons_no = D_Test_CmnGetPrevMonsNo( work->mons_no, work->order_mons_no );

  if( *mons_no <= 0 )
  {
    return FALSE;
  }
  else
  {
    u32 ppd_form_max;
    u32 ppd_sex;

    D_Test_CmnPokeGetPersonalData(
        work, 
        *mons_no,
        &ppd_form_max,
        &ppd_sex );

    *form_no = ppd_form_max -1;
    
    switch( ppd_sex )
    {
    case POKEPER_SEX_MALE:      // ���ʌŒ�
      {
        *sex = PTL_SEX_MALE;
      }
      break;
    case POKEPER_SEX_FEMALE:
      {
        *sex = PTL_SEX_FEMALE;
      }
      break;
    case POKEPER_SEX_UNKNOWN:
      {
        *sex = PTL_SEX_UNKNOWN;
      }
      break;
    default:  // 1�`253        // �I�X�ƃ��X�A�ǂ�������݂���
      {
        *sex = PTL_SEX_FEMALE;
      }
      break;
    }

    *rare         = 1;
    *egg          = FALSE;
    *dir          = (work->mcss_dir_front)?(MCSS_DIR_FRONT):(MCSS_DIR_BACK);
    *personal_rnd = 0;
    
    return TRUE;
  }
}

//-------------------------------------
/// �ŏ��̃|�P�����̍ŏ��̃t�H�������𓾂�
//=====================================
static void D_Test_CmnPokeGetFirstPokeFirstForm(
    D_TEST_COMMON_WORK* work,
    int* mons_no, int* form_no, int* sex, int* rare, BOOL* egg, int* dir,  // �������߂�l
    u32* personal_rnd )                                                    // �������߂�l
{
  u32 ppd_form_max;
  u32 ppd_sex;
  
  *mons_no = D_Test_CmnGetFirstMonsNo( work->order_mons_no );  // MONSNO_HUSIGIDANE<= <=MONSNO_END  // prog/include/poke_tool/monsno_def.h
  
  D_Test_CmnPokeGetPersonalData(
      work,
      *mons_no,
      &ppd_form_max,
      &ppd_sex );

  *form_no      = 0;

  switch( ppd_sex )
  {
  case POKEPER_SEX_MALE:
    {
      *sex = PTL_SEX_MALE;  // PTL_SEX_MALE, PTL_SEX_FEMALE, PTL_SEX_UNKNOWN  // prog/include/poke_tool/poke_tool.h
    }
    break;
  case POKEPER_SEX_FEMALE:
    {
      *sex = PTL_SEX_FEMALE;
    }
    break;
  case POKEPER_SEX_UNKNOWN:
    {
      *sex = PTL_SEX_UNKNOWN;
    }
    break;
  default:  // 1�`253
    {
      *sex = PTL_SEX_MALE;
    }
    break;
  }

  *rare         = 0;
  *egg          = FALSE;
  *dir          = (work->mcss_dir_front)?(MCSS_DIR_FRONT):(MCSS_DIR_BACK);
  *personal_rnd = 0;
}
//-------------------------------------
/// �Ō�̃|�P�����̍Ō�̃t�H�������𓾂�
//=====================================
static void D_Test_CmnPokeGetLastPokeLastForm(
    D_TEST_COMMON_WORK* work,
    int* mons_no, int* form_no, int* sex, int* rare, BOOL* egg, int* dir,  // �������߂�l
    u32* personal_rnd )                                                    // �������߂�l
{
  u32 ppd_form_max;
  u32 ppd_sex;
 
  *mons_no = D_Test_CmnGetLastMonsNo( work->order_mons_no );

  D_Test_CmnPokeGetPersonalData(
      work,
      *mons_no,
      &ppd_form_max,
      &ppd_sex );

  *form_no      = ppd_form_max -1;

  switch( ppd_sex )
  {
  case POKEPER_SEX_MALE:
    {
      *sex = PTL_SEX_MALE;
    }
    break;
  case POKEPER_SEX_FEMALE:
    {
      *sex = PTL_SEX_FEMALE;
    }
    break;
  case POKEPER_SEX_UNKNOWN:
    {
      *sex = PTL_SEX_UNKNOWN;
    }
    break;
  default:  // 1�`253
    {
      *sex = PTL_SEX_FEMALE;
    }
    break;
  }

  *rare         = 1;
  *egg          = FALSE;
  *dir          = (work->mcss_dir_front)?(MCSS_DIR_FRONT):(MCSS_DIR_BACK);
  *personal_rnd = 0;
}
//-------------------------------------
/// �Ō�̃|�P�����̍ŏ��̃t�H�������𓾂�
//=====================================
static void D_Test_CmnPokeGetLastPokeFirstForm(
    D_TEST_COMMON_WORK* work,
    int* mons_no, int* form_no, int* sex, int* rare, BOOL* egg, int* dir,  // �������߂�l
    u32* personal_rnd )                                                    // �������߂�l
{
  u32 ppd_form_max;
  u32 ppd_sex;
  
  *mons_no = D_Test_CmnGetLastMonsNo( work->order_mons_no );
  
  D_Test_CmnPokeGetPersonalData(
      work,
      *mons_no,
      &ppd_form_max,
      &ppd_sex );

  *form_no      = 0;

  switch( ppd_sex )
  {
  case POKEPER_SEX_MALE:
    {
      *sex = PTL_SEX_MALE;
    }
    break;
  case POKEPER_SEX_FEMALE:
    {
      *sex = PTL_SEX_FEMALE;
    }
    break;
  case POKEPER_SEX_UNKNOWN:
    {
      *sex = PTL_SEX_UNKNOWN;
    }
    break;
  default:  // 1�`253
    {
      *sex = PTL_SEX_MALE;
    }
    break;
  }

  *rare         = 0;
  *egg          = FALSE;
  *dir          = (work->mcss_dir_front)?(MCSS_DIR_FRONT):(MCSS_DIR_BACK);
  *personal_rnd = 0;
} 

//-------------------------------------
/// �|�P�����p�[�\�i���f�[�^�̏�񂩂�A�t�H�������Ɛ��ʃx�N�g���𓾂�
//=====================================
static void D_Test_CmnPokeGetPersonalData(
    D_TEST_COMMON_WORK* work,
    u16 mons_no,
    u32* ppd_form_max, u32* ppd_sex )  // �������߂�l
{
  POKEMON_PERSONAL_DATA* ppd = POKE_PERSONAL_OpenHandle( mons_no, 0, work->heap_id );

  *ppd_form_max = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_form_max );  // �ʃt�H�����Ȃ��̂Ƃ���1
  *ppd_sex      = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_sex );       // POKEPER_SEX_MALE, 1�`253, POKEPER_SEX_FEMALE, POKEPER_SEX_UNKNOWN  // prog/include/poke_tool/poke_personal.h

  POKE_PERSONAL_CloseHandle( ppd );
} 




//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
/**
*  �i���f���J����
*/
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================

#define HEAPID_D_TEST_SHINKA (HEAPID_SHINKA_DEMO)
#define HEAP_SIZE_D_TEST_SHINKA  (0x70000)


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================


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
  // �q�[�v�A�p�����[�^�Ȃ�
  HEAPID                             heap_id;
  D_KAWADA_TEST_SHINKA_PARAM*        param;

  // �O���t�B�b�N�Ȃ�
  SHINKADEMO_GRAPHIC_WORK*           graphic;

  // VBlank��TCB
  GFL_TCB*                           vblank_tcb;

  // ���ʎg�p�ł��鎖��
  D_TEST_COMMON_WORK*                cmn_wk;
}
D_TEST_SHINKA_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void D_Test_ShinkaVBlankFunc( GFL_TCB* tcb, void* wk );

// �I�t�Z�b�g��ݒ肷��֐�
static void D_Test_ShinkaMcssSetOfsPositionFunc(void* d_test_cmn_wk);


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static GFL_PROC_RESULT D_Test_ShinkaProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT D_Test_ShinkaProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT D_Test_ShinkaProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk );

const GFL_PROC_DATA    D_KAWADA_TEST_ShinkaProcData =
{
  D_Test_ShinkaProcInit,
  D_Test_ShinkaProcMain,
  D_Test_ShinkaProcExit,
};


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief           
 *
 *  @param[in]       
 *
 *  @retval          
 */
//------------------------------------------------------------------


//=============================================================================
/**
*  ���[�J���֐���`(PROC)
*/
//=============================================================================
//-------------------------------------
/// PROC ����������
//=====================================
static GFL_PROC_RESULT D_Test_ShinkaProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  D_TEST_SHINKA_WORK*  work;

  // �I�[�o�[���C
  GFL_OVERLAY_Load( FS_OVERLAY_ID(shinka_demo) );

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_D_TEST_SHINKA, HEAP_SIZE_D_TEST_SHINKA );
    work = GFL_PROC_AllocWork( proc, sizeof(D_TEST_SHINKA_WORK), HEAPID_D_TEST_SHINKA );
    GFL_STD_MemClear( work, sizeof(D_TEST_SHINKA_WORK) );
    
    work->heap_id  = HEAPID_D_TEST_SHINKA;
    work->param    = (D_KAWADA_TEST_SHINKA_PARAM*)pwk;
  }

  // �O���t�B�b�N�Ȃ�
  {
    work->graphic    = SHINKADEMO_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, work->heap_id );
    SHINKADEMO_GRAPHIC_InitBGSub( work->graphic );
  }

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( D_Test_ShinkaVBlankFunc, work, 1 );

  // ���ʎg�p�ł��鎖��
  {
    VecFx32  mcss_pos    = { FX_F32_TO_FX32(0.0f), FX_F32_TO_FX32(-18.0f), 0 };
    VecFx32  mcss_scale  = { FX_F32_TO_FX32(16.0f), FX_F32_TO_FX32(16.0f), FX32_ONE };

    work->cmn_wk = D_Test_CmnInit( work->heap_id );
    D_Test_CmnMcssSet(
      work->cmn_wk,
      &mcss_pos,
      &mcss_scale,
      D_Test_ShinkaMcssSetOfsPositionFunc );
  }

  // �ŏ��̃|�P����
  {
    int  mons_no;
    int  form_no;
    int  sex;
    int  rare;
    BOOL egg;
    int  dir;
    u32  personal_rnd;
    D_Test_CmnPokeGetFirstPokeFirstForm( work->cmn_wk, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
    D_Test_CmnPokeChange( work->cmn_wk, mons_no, form_no, sex, rare, egg, dir, personal_rnd );
  }

  // �t�F�[�h�C��(�������Ɍ�����悤��)
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 0, 0 );

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static GFL_PROC_RESULT D_Test_ShinkaProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  D_TEST_SHINKA_WORK*  work  = (D_TEST_SHINKA_WORK*)mywk;

  // ���ʎg�p�ł��鎖��
  D_Test_CmnExit( work->cmn_wk );

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �O���t�B�b�N�Ȃ�
  {
    SHINKADEMO_GRAPHIC_ExitBGSub( work->graphic );
    SHINKADEMO_GRAPHIC_Exit( work->graphic );
  }

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_PROC_FreeWork( proc );
    GFL_HEAP_DeleteHeap( HEAPID_D_TEST_SHINKA );
  }

  // �I�[�o�[���C
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(shinka_demo) );

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �又��
//=====================================
static GFL_PROC_RESULT D_Test_ShinkaProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  D_TEST_SHINKA_WORK*     work     = (D_TEST_SHINKA_WORK*)mywk;

  BOOL res = D_Test_CmnInput( work->cmn_wk );

  if( res )
  {
    return GFL_PROC_RES_FINISH;
  }
  
  // ���C��
  D_Test_CmnMain( work->cmn_wk );
 
  // 3D�`��
  SHINKADEMO_GRAPHIC_3D_StartDraw( work->graphic );
  D_Test_CmnDraw3D( work->cmn_wk );  // ���ʎg�p�ł��鎖��
  SHINKADEMO_GRAPHIC_3D_EndDraw( work->graphic );

  return GFL_PROC_RES_CONTINUE;
}

//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// VBlank�֐�
//=====================================
static void D_Test_ShinkaVBlankFunc( GFL_TCB* tcb, void* wk )
{
  D_TEST_SHINKA_WORK* work = (D_TEST_SHINKA_WORK*)wk;
}

//-------------------------------------
/// �I�t�Z�b�g��ݒ肷��֐�
//=====================================
static void D_Test_ShinkaMcssSetOfsPositionFunc(void* d_test_cmn_wk)
{
  D_TEST_COMMON_WORK* cmn_wk = (D_TEST_COMMON_WORK*)d_test_cmn_wk;

  {
    f32      size_y   = (f32)MCSS_GetSizeY( cmn_wk->mcss_wk );
    f32      offset_y = (f32)MCSS_GetOffsetY( cmn_wk->mcss_wk );  // �����Ă���Ƃ�-, ����ł���Ƃ�+
    f32      offset_x = (f32)MCSS_GetOffsetX( cmn_wk->mcss_wk );  // �E�ɂ���Ă���Ƃ�+, ���ɂ���Ă���Ƃ�-
    f32      ofs_position_y;
    f32      ofs_position_x;
    VecFx32  ofs_position;

    //size_y   *= 16.0f / 16.0f;  // �Ӗ��Ȃ��̂ŃR�����g�A�E�g����
    if( size_y > 96.0f ) size_y = 96.0f;

    //offset_y *= 16.0f / 16.0f;  // �Ӗ��Ȃ��̂ŃR�����g�A�E�g����
    //offset_x *= 16.0f / 16.0f;  // �Ӗ��Ȃ��̂ŃR�����g�A�E�g����

#if 1  // ���������ĉ��{�s�v���H
    ofs_position_y = ( ( 96.0f - size_y ) / 2.0f + offset_y ) * (0.33f);
    ofs_position_x = - offset_x * (0.33f);
#else
    ofs_position_y = ( ( 96.0f - size_y ) / 2.0f + offset_y );
    ofs_position_x = - offset_x;
#endif

    ofs_position.x = FX_F32_TO_FX32(ofs_position_x);  ofs_position.y = FX_F32_TO_FX32(ofs_position_y);  ofs_position.z = 0;
    MCSS_SetOfsPosition( cmn_wk->mcss_wk, &ofs_position );
  }
}




//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
/**
*  �^�}�S�z���f���J����
*/
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================

#define HEAPID_D_TEST_EGG     (HEAPID_EGG_DEMO)
#define HEAP_SIZE_D_TEST_EGG  (0x50000)


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================


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
  // �q�[�v�A�p�����[�^�Ȃ�
  HEAPID                             heap_id;
  D_KAWADA_TEST_EGG_PARAM*           param;

  // �O���t�B�b�N�Ȃ�
  EGG_DEMO_GRAPHIC_WORK*             graphic;

  // VBlank��TCB
  GFL_TCB*                           vblank_tcb;

  // ���ʎg�p�ł��鎖��
  D_TEST_COMMON_WORK*                cmn_wk;
}
D_TEST_EGG_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void D_Test_EggVBlankFunc( GFL_TCB* tcb, void* wk );

// �I�t�Z�b�g��ݒ肷��֐�
static void D_Test_EggMcssSetOfsPositionFunc(void* d_test_cmn_wk);


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static GFL_PROC_RESULT D_Test_EggProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT D_Test_EggProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT D_Test_EggProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk );

const GFL_PROC_DATA    D_KAWADA_TEST_EggProcData =
{
  D_Test_EggProcInit,
  D_Test_EggProcMain,
  D_Test_EggProcExit,
};


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief           
 *
 *  @param[in]       
 *
 *  @retval          
 */
//------------------------------------------------------------------


//=============================================================================
/**
*  ���[�J���֐���`(PROC)
*/
//=============================================================================
//-------------------------------------
/// PROC ����������
//=====================================
static GFL_PROC_RESULT D_Test_EggProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  D_TEST_EGG_WORK*  work;

  // �I�[�o�[���C
  GFL_OVERLAY_Load( FS_OVERLAY_ID(egg_demo) );

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_D_TEST_EGG, HEAP_SIZE_D_TEST_EGG );
    work = GFL_PROC_AllocWork( proc, sizeof(D_TEST_EGG_WORK), HEAPID_D_TEST_EGG );
    GFL_STD_MemClear( work, sizeof(D_TEST_EGG_WORK) );
    
    work->heap_id  = HEAPID_D_TEST_EGG;
    work->param    = (D_KAWADA_TEST_EGG_PARAM*)pwk;
  }

  // �O���t�B�b�N�Ȃ�
  {
    work->graphic    = EGG_DEMO_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, work->heap_id );
  }

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( D_Test_EggVBlankFunc, work, 1 );

  // ���ʎg�p�ł��鎖��
  {
    VecFx32  mcss_pos    = { 0, FX_F32_TO_FX32(-190.0f), FX_F32_TO_FX32(-800.0f) };
    VecFx32  mcss_scale  = { FX_F32_TO_FX32(16.0f), FX_F32_TO_FX32(16.0f), FX32_ONE };

    work->cmn_wk = D_Test_CmnInit( work->heap_id );
    D_Test_CmnMcssSet(
      work->cmn_wk,
      &mcss_pos,
      &mcss_scale,
      D_Test_EggMcssSetOfsPositionFunc );
  }

  // �ŏ��̃|�P����
  {
    int  mons_no;
    int  form_no;
    int  sex;
    int  rare;
    BOOL egg;
    int  dir;
    u32  personal_rnd;
    D_Test_CmnPokeGetFirstPokeFirstForm( work->cmn_wk, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
    D_Test_CmnPokeChange( work->cmn_wk, mons_no, form_no, sex, rare, egg, dir, personal_rnd );
  }

  // �t�F�[�h�C��(�������Ɍ�����悤��)
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 0, 0 );

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static GFL_PROC_RESULT D_Test_EggProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  D_TEST_EGG_WORK*  work  = (D_TEST_EGG_WORK*)mywk;

  // ���ʎg�p�ł��鎖��
  D_Test_CmnExit( work->cmn_wk );

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �O���t�B�b�N�Ȃ�
  {
    EGG_DEMO_GRAPHIC_Exit( work->graphic );
  }

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_PROC_FreeWork( proc );
    GFL_HEAP_DeleteHeap( HEAPID_D_TEST_EGG );
  }

  // �I�[�o�[���C
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(egg_demo) );

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �又��
//=====================================
static GFL_PROC_RESULT D_Test_EggProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  D_TEST_EGG_WORK*     work     = (D_TEST_EGG_WORK*)mywk;

  BOOL res = D_Test_CmnInput( work->cmn_wk );

  if( res )
  {
    return GFL_PROC_RES_FINISH;
  }
  
  // ���C��
  D_Test_CmnMain( work->cmn_wk );
 
  // 3D�`��
  EGG_DEMO_GRAPHIC_3D_StartDraw( work->graphic );
  D_Test_CmnDraw3D( work->cmn_wk );  // ���ʎg�p�ł��鎖��
  EGG_DEMO_GRAPHIC_3D_EndDraw( work->graphic );

  return GFL_PROC_RES_CONTINUE;
}

//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// VBlank�֐�
//=====================================
static void D_Test_EggVBlankFunc( GFL_TCB* tcb, void* wk )
{
  D_TEST_EGG_WORK* work = (D_TEST_EGG_WORK*)wk;
}

//-------------------------------------
/// �I�t�Z�b�g��ݒ肷��֐�
//=====================================
static void D_Test_EggMcssSetOfsPositionFunc(void* d_test_cmn_wk)
{
  D_TEST_COMMON_WORK* cmn_wk = (D_TEST_COMMON_WORK*)d_test_cmn_wk;

  {
    f32      size_y   = (f32)MCSS_GetSizeY( cmn_wk->mcss_wk );
    f32      offset_y = (f32)MCSS_GetOffsetY( cmn_wk->mcss_wk );  // �����Ă���Ƃ�-, ����ł���Ƃ�+
    f32      offset_x = (f32)MCSS_GetOffsetX( cmn_wk->mcss_wk );  // �E�ɂ���Ă���Ƃ�+, ���ɂ���Ă���Ƃ�-
    f32      ofs_position_y;
    f32      ofs_position_x;
    VecFx32  ofs_position;

    OS_Printf( "MCSS_GetOffsetX=%d\n", MCSS_GetOffsetX(cmn_wk->mcss_wk) );

    if( size_y > 96.0f ) size_y = 96.0f;

    ofs_position_y = ( 96.0f - size_y ) / 2.0f + offset_y;
    ofs_position_x = - offset_x;
      
    ofs_position.x = FX_F32_TO_FX32(ofs_position_x);  ofs_position.y = FX_F32_TO_FX32(ofs_position_y);  ofs_position.z = 0;
    MCSS_SetOfsPosition( cmn_wk->mcss_wk, &ofs_position );
  }
}








//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
/**
*  �}�Ӄt�H�����J����
*/
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================

#define HEAPID_D_TEST_ZUKAN_FORM     (HEAPID_SHINKA_DEMO)
#define HEAP_SIZE_D_TEST_ZUKAN_FORM  (0x90000)


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================


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
  // �q�[�v�A�p�����[�^�Ȃ�
  HEAPID                             heap_id;
  D_KAWADA_TEST_ZUKAN_FORM_PARAM*    param;

  // �O���t�B�b�N�Ȃ�
  ZUKAN_DETAIL_GRAPHIC_WORK*         graphic;

  // VBlank��TCB
  GFL_TCB*                           vblank_tcb;

  // ���ʎg�p�ł��鎖��
  D_TEST_COMMON_WORK*                cmn_wk;
}
D_TEST_ZUKAN_FORM_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void D_Test_ZukanFormVBlankFunc( GFL_TCB* tcb, void* wk );

// �I�t�Z�b�g��ݒ肷��֐�
static void D_Test_ZukanFormMcssSetOfsPositionFunc(void* d_test_cmn_wk);


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static GFL_PROC_RESULT D_Test_ZukanFormProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT D_Test_ZukanFormProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT D_Test_ZukanFormProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk );

const GFL_PROC_DATA    D_KAWADA_TEST_ZukanFormProcData =
{
  D_Test_ZukanFormProcInit,
  D_Test_ZukanFormProcMain,
  D_Test_ZukanFormProcExit,
};


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief           
 *
 *  @param[in]       
 *
 *  @retval          
 */
//------------------------------------------------------------------


//=============================================================================
/**
*  ���[�J���֐���`(PROC)
*/
//=============================================================================
//-------------------------------------
/// PROC ����������
//=====================================
static GFL_PROC_RESULT D_Test_ZukanFormProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  D_TEST_ZUKAN_FORM_WORK*  work;

  // �I�[�o�[���C
  GFL_OVERLAY_Load( FS_OVERLAY_ID(zukan_detail) );

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_D_TEST_ZUKAN_FORM, HEAP_SIZE_D_TEST_ZUKAN_FORM );
    work = GFL_PROC_AllocWork( proc, sizeof(D_TEST_ZUKAN_FORM_WORK), HEAPID_D_TEST_ZUKAN_FORM );
    GFL_STD_MemClear( work, sizeof(D_TEST_ZUKAN_FORM_WORK) );
    
    work->heap_id  = HEAPID_D_TEST_ZUKAN_FORM;
    work->param    = (D_KAWADA_TEST_ZUKAN_FORM_PARAM*)pwk;
  }

  // �O���t�B�b�N�Ȃ�
  {
    work->graphic    = ZUKAN_DETAIL_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, work->heap_id, TRUE );

    // �؂�ւ�
    {
      // �O���t�B�b�N�X���[�h�ݒ�
      GX_SetGraphicsMode( GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D );
    }
  }

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( D_Test_ZukanFormVBlankFunc, work, 1 );

  // ���ʎg�p�ł��鎖��
  {
    VecFx32  mcss_pos;
    VecFx32  mcss_scale;

    switch( work->param->mode )
    {
    case 0:
      {
        mcss_pos.x   = FX_F32_TO_FX32(0.0f);   mcss_pos.y   = FX_F32_TO_FX32(-13.9f);  mcss_pos.z   = FX_F32_TO_FX32(0.0f);
        mcss_scale.x = FX_F32_TO_FX32(16.0f);  mcss_scale.y = FX_F32_TO_FX32(16.0f);   mcss_scale.z = FX32_ONE;
      }
      break;
    case 1:
      {
        mcss_pos.x   = FX_F32_TO_FX32(-16.0f);  mcss_pos.y   = FX_F32_TO_FX32(-13.9f);  mcss_pos.z   = FX_F32_TO_FX32(0.0f);
        mcss_scale.x = FX_F32_TO_FX32(16.0f);   mcss_scale.y = FX_F32_TO_FX32(16.0f);   mcss_scale.z = FX32_ONE;
      }
      break;
    case 2:
      {
        mcss_pos.x   = FX_F32_TO_FX32(16.0f);  mcss_pos.y   = FX_F32_TO_FX32(-13.9f);  mcss_pos.z   = FX_F32_TO_FX32(0.0f);
        mcss_scale.x = FX_F32_TO_FX32(16.0f);  mcss_scale.y = FX_F32_TO_FX32(16.0f);   mcss_scale.z = FX32_ONE;
      }
      break;
    }

    work->cmn_wk = D_Test_CmnInit( work->heap_id );
    D_Test_CmnMcssSet(
      work->cmn_wk,
      &mcss_pos,
      &mcss_scale,
      D_Test_ZukanFormMcssSetOfsPositionFunc );
  }

  // �ŏ��̃|�P����
  {
    int  mons_no;
    int  form_no;
    int  sex;
    int  rare;
    BOOL egg;
    int  dir;
    u32  personal_rnd;
    D_Test_CmnPokeGetFirstPokeFirstForm( work->cmn_wk, &mons_no, &form_no, &sex, &rare, &egg, &dir, &personal_rnd );
    D_Test_CmnPokeChange( work->cmn_wk, mons_no, form_no, sex, rare, egg, dir, personal_rnd );
  }

  // �t�F�[�h�C��(�������Ɍ�����悤��)
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 0, 0 );

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static GFL_PROC_RESULT D_Test_ZukanFormProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  D_TEST_ZUKAN_FORM_WORK*  work  = (D_TEST_ZUKAN_FORM_WORK*)mywk;

  // ���ʎg�p�ł��鎖��
  D_Test_CmnExit( work->cmn_wk );

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �O���t�B�b�N�Ȃ�
  {
    // �؂�ւ�
    {
      // �O���t�B�b�N�X���[�h�ݒ�
      GX_SetGraphicsMode( GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D );
    }

    ZUKAN_DETAIL_GRAPHIC_Exit( work->graphic );
  }

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_PROC_FreeWork( proc );
    GFL_HEAP_DeleteHeap( HEAPID_D_TEST_ZUKAN_FORM );
  }

  // �I�[�o�[���C
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(zukan_detail) );

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �又��
//=====================================
static GFL_PROC_RESULT D_Test_ZukanFormProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  D_TEST_ZUKAN_FORM_WORK*     work     = (D_TEST_ZUKAN_FORM_WORK*)mywk;

  BOOL res = D_Test_CmnInput( work->cmn_wk );

  if( res )
  {
    return GFL_PROC_RES_FINISH;
  }
  
  // ���C��
  D_Test_CmnMain( work->cmn_wk );
 
  // 3D�`��
  ZUKAN_DETAIL_GRAPHIC_3D_StartDraw( work->graphic );
  D_Test_CmnDraw3D( work->cmn_wk );  // ���ʎg�p�ł��鎖��
  ZUKAN_DETAIL_GRAPHIC_3D_EndDraw( work->graphic );

  return GFL_PROC_RES_CONTINUE;
}

//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// VBlank�֐�
//=====================================
static void D_Test_ZukanFormVBlankFunc( GFL_TCB* tcb, void* wk )
{
  D_TEST_ZUKAN_FORM_WORK* work = (D_TEST_ZUKAN_FORM_WORK*)wk;
}

//-------------------------------------
/// �I�t�Z�b�g��ݒ肷��֐�
//=====================================
static void D_Test_ZukanFormMcssSetOfsPositionFunc(void* d_test_cmn_wk)
{
  D_TEST_COMMON_WORK* cmn_wk = (D_TEST_COMMON_WORK*)d_test_cmn_wk;

  {
    f32      offset_x = (f32)MCSS_GetOffsetX( cmn_wk->mcss_wk );  // �E�ɂ���Ă���Ƃ�+, ���ɂ���Ă���Ƃ�-
    f32      ofs_position_x;
    VecFx32  ofs_position;

#if 1  // ���������ĉ��{�s�v���H
    ofs_position_x = - offset_x * (0.25f);
#else 
    ofs_position_x = - offset_x * (0.25f);
#endif

    ofs_position.x = FX_F32_TO_FX32(ofs_position_x);  ofs_position.y = 0;  ofs_position.z = 0;
    MCSS_SetOfsPosition( cmn_wk->mcss_wk, &ofs_position );
  }
}


#endif
