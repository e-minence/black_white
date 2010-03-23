//============================================================================
/**
 *  @file   zukan_info.c
 *  @brief  �}�ӏ��
 *  @author Koji Kawada
 *  @data   2009.12.03
 *  @note   infowin.c, touchbar.c, ui_template.c���Q�l�ɂ��č쐬���܂����B
 *
 *  ���W���[�����FZUKAN_INFO
 */
//============================================================================
// lib
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

// �C���N���[�h
#include "print/printsys.h"
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/global_msg.h"

#include "ui/ui_easy_clwk.h"

#include "system/poke2dgra.h"

#include "poke_tool/pokefoot.h"

// ����
#include "sound/pm_voice.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"
#include "zukan_gra.naix"

// ���b�Z�[�W
#include "msg/msg_zkn.h"
#include "msg/msg_zkn_type.h"
#include "msg/msg_zkn_height.h"
#include "msg/msg_zkn_weight.h"
#include "msg/msg_zkn_comment_00.h"
#include "msg/msg_zkn_comment_e.h"

// zukan_info
#include "zukan_info.h"

//=============================================================================
/**
 *  �萔��`
 */
//=============================================================================
// BG�t���[��
#define ZUKAN_INFO_BG_FRAME_M_MSG    (GFL_BG_FRAME3_M)
#define ZUKAN_INFO_BG_FRAME_M_FORE   (GFL_BG_FRAME2_M)
#define ZUKAN_INFO_BG_FRAME_M_BACK   (GFL_BG_FRAME0_M)

#define ZUKAN_INFO_BG_FRAME_S_MSG    (GFL_BG_FRAME3_S)
#define ZUKAN_INFO_BG_FRAME_S_FORE   (GFL_BG_FRAME2_S)
#define ZUKAN_INFO_BG_FRAME_S_BACK   (GFL_BG_FRAME0_S)

// BG�p���b�g
// �{��
enum
{
  ZUKAN_INFO_BG_PAL_NUM_FORE_BACK    =  3,
  ZUKAN_INFO_BG_PAL_NUM_MSG          =  1,
  ZUKAN_INFO_BG_PAL_NUM_BACK_NONE    =  2,  // �����\�����Ȃ��Ƃ��p�̔w�i
};
// �ʒu
enum
{
  ZUKAN_INFO_BG_PAL_POS_FORE_BACK    = 0,
  ZUKAN_INFO_BG_PAL_POS_MSG          = ZUKAN_INFO_BG_PAL_POS_FORE_BACK   + ZUKAN_INFO_BG_PAL_NUM_FORE_BACK   ,  // 3
  ZUKAN_INFO_BG_PAL_POS_BACK_NONE    = ZUKAN_INFO_BG_PAL_POS_MSG         + ZUKAN_INFO_BG_PAL_NUM_MSG         ,  // 4
  ZUKAN_INFO_BG_PAL_POS_MAX          = ZUKAN_INFO_BG_PAL_POS_BACK_NONE   + ZUKAN_INFO_BG_PAL_NUM_BACK_NONE   ,  // 6  // ���������
};

// OBJ�p���b�g
// �{��
enum
{
  ZUKAN_INFO_OBJ_PAL_NUM_POKE2D    =  1,
  ZUKAN_INFO_OBJ_PAL_NUM_TYPEICON  =  3,
  ZUKAN_INFO_OBJ_PAL_NUM_POKEFOOT  =  1,
};
// �ʒu
enum
{
  ZUKAN_INFO_OBJ_PAL_POS_POKE2D    = 0,
  ZUKAN_INFO_OBJ_PAL_POS_TYPEICON  = ZUKAN_INFO_OBJ_PAL_POS_POKE2D   + ZUKAN_INFO_OBJ_PAL_NUM_POKE2D   ,  // 1
  ZUKAN_INFO_OBJ_PAL_POS_POKEFOOT  = ZUKAN_INFO_OBJ_PAL_POS_TYPEICON + ZUKAN_INFO_OBJ_PAL_NUM_TYPEICON ,  // 4
  ZUKAN_INFO_OBJ_PAL_POS_MAX       = ZUKAN_INFO_OBJ_PAL_POS_POKEFOOT + ZUKAN_INFO_OBJ_PAL_NUM_POKEFOOT ,  // 5  // ���������
};

// �F�ɂ���
#define ZUKAN_INFO_COLOR_SIZE           (2)         // 1�F�̃T�C�Y��2�o�C�g
#define ZUKAN_INFO_PAL_LINE_COLOR_NUM   (0x10)      // 1�{�̐F��
#define ZUKAN_INFO_PAL_LINE_SIZE        ( ZUKAN_INFO_COLOR_SIZE * ZUKAN_INFO_PAL_LINE_COLOR_NUM )  // 1�{�̃T�C�Y(32�o�C�g)

// BG�ɂ���
#define ZUKAN_INFO_BG_CHARA_SIZE           ( 32 * 4 * GFL_BG_1CHRDATASIZ )  // �o�C�g
#define ZUKAN_INFO_BG_SCREEN_W             (32)  // �L����
#define ZUKAN_INFO_BG_SCREEN_H             (32)//(24)  // �L����
#define ZUKAN_INFO_BG_SCREEN_SIZE          ( ZUKAN_INFO_BG_SCREEN_W * ZUKAN_INFO_BG_SCREEN_H * GFL_BG_1SCRDATASIZ )  // �o�C�g

// BG�̃X�N���[��
#define ZUKAN_INFO_BACK_BG_SCROLL_X_VALUE (1)
#define ZUKAN_INFO_BACK_BG_SCROLL_WAIT    (4)  // 4�t���[����1�񓮂�  // 4���傫���Ɠ������J�N�J�N���Ă��܂�

// TCB�ƍX�V
#define ZUKAN_INFO_VBLANK_TCB_PRI               (1)
#define ZUKAN_INFO_BG_UPDATE_BIT_RESET          (0)
#define ZUKAN_INFO_BACK_BG_UPDATE_BIT_SCROLL    (1<<0)

// MSG�ɂ���
enum
{
  ZUKAN_INFO_MSG_TOROKU,
  ZUKAN_INFO_MSG_NAME,
  ZUKAN_INFO_MSG_PHYSICAL,
  ZUKAN_INFO_MSG_EXPLAIN,

  ZUKAN_INFO_MSG_MAX,
};

typedef enum
{
  ZUKAN_INFO_ALIGN_LEFT,
  ZUKAN_INFO_ALIGN_RIGHT,
  ZUKAN_INFO_ALIGN_CENTER,
}
ZUKAN_INFO_ALIGN;

#define ZUKAN_INFO_STRBUF_LEN (128)  // ���̕������ő���邩buflen.h�ŗv�m�F

// �X�e�b�v
typedef enum
{
  ZUKAN_INFO_STEP_COLOR_WAIT,    // TOROKU
  ZUKAN_INFO_STEP_COLOR_WHILE,   // TOROKU
  ZUKAN_INFO_STEP_COLOR,         // TOROKU
  ZUKAN_INFO_STEP_MOVE_WAIT,     // TOROKU
  ZUKAN_INFO_STEP_MOVE,          // TOROKU
  ZUKAN_INFO_STEP_CENTER,        // NICKNAME
  ZUKAN_INFO_STEP_CENTER_STILL,  // TOROKU or NICKNAME
  ZUKAN_INFO_STEP_START_STILL,   // LIST
}
ZUKAN_INFO_STEP;

// �|�P����2D�ɂ���
#define ZUKAN_INFO_START_POKEMON_POS_X   (8*6+2)//(8*6)  // �|�P����2D��12chara x 12chara
                                                       // ����̃s�N�Z�����W��(0, 24)��������(2,24)�ɂȂ�悤�ɁB
#define ZUKAN_INFO_START_POKEMON_POS_Y   (8*(3+6))

#define ZUKAN_INFO_CENTER_POKEMON_POS_X  (8*16)
#define ZUKAN_INFO_CENTER_POKEMON_POS_Y  (8*(3+6))

// �|�P����2D�ɏ��WND
#define ZUKAN_INFO_WND_DOWN_Y_START    (8*17)
#define ZUKAN_INFO_WND_DOWN_Y_GOAL     (8*2)
#define ZUKAN_INFO_WND_UP_Y            (8*1)
#define ZUKAN_INFO_WND_LEFT_X          (0)
#define ZUKAN_INFO_WND_RIGHT_X         (100)

#define ZUKAN_INFO_COLOR_WHILE         (15)  // �|�P����2D�ɏ��WND�������o���܂ł̑҂��t���[����

// �p���b�g�A�j���[�V����
#define ZUKAN_INFO_PALETTE_ANIME_LINE          (0x1)        // 16�{���̂ǂ���p���b�g�A�j���[�V�������邩
#define ZUKAN_INFO_PALETTE_ANIME_LINE_S        (0x1)        // 16�{���̂ǂꂪ�p���b�g�A�j���[�V�����̃X�^�[�g�F�̂��郉�C����
#define ZUKAN_INFO_PALETTE_ANIME_LINE_E        (0x2)        // 16�{���̂ǂꂪ�p���b�g�A�j���[�V�����̃G���h�F�̂��郉�C����
#define ZUKAN_INFO_PALETTE_ANIME_NO            (0x1)        // 16�F���̂ǂ�����p���b�g�A�j���[�V�����̑ΏۂƂ��ĊJ�n���邩
#define ZUKAN_INFO_PALETTE_ANIME_NUM           (5)          // �p���b�g�A�j���[�V�����̑ΏۂƂ��ĉ��F���邩
#define ZUKAN_INFO_PALETTE_ANIME_VALUE         (0x400)      // cos�g���̂�0�`0xFFFF�̃��[�v
#define ZUKAN_INFO_PALETTE_ANIME_VALUE_MAX     (0x10000)

// Y�I�t�Z�b�g
#define ZUKAN_INFO_Y_OFFSET      (-8*3)      // �}�ӂ̐�����ʂƂ��ĉ��̃f�B�X�v���C(���C��)�ɕ\������Ƃ�

// �O����}��
#define FOREIGN_MONSNO_MAX  (493)  // 0�̓|�P�����ł͂Ȃ����f�[�^����A1���t�V�M�_�l�A493���A���Z�E�X�܂ő��݂���A�C�b�V������ǉ����ꂽ494���瑶�݂��Ȃ�

static const u16 narc_msg_explain[ZUKAN_INFO_LANG_MAX] =
{
  NARC_message_zkn_comment_e_dat,
  NARC_message_zkn_comment_fra_dat,
  NARC_message_zkn_comment_ger_dat,
  NARC_message_zkn_comment_ita_dat,
  NARC_message_zkn_comment_spa_dat,
  NARC_message_zkn_comment_kor_dat,
};


//=============================================================================
/**
 *  �\���̐錾
 */
//=============================================================================
struct _ZUKAN_INFO_WORK
{
  // ����
  HEAPID                  heap_id;         // ���̂Ƃ���̂��o���Ă��邾���Ő�����j���͂��Ȃ��B
  BOOL                    zenkoku_flag;
  BOOL                    get_flag;
  u16                     monsno;
  u16                     formno;          // 0<=formno<POKETOOL_GetPersonalParam( monsno, 0, POKEPER_ID_form_max )
  u16                     sex;             // PTL_SEX_MALE, PTL_SEX_FEMALE, PTL_SEX_UNKNOWN
  u16                     rare;            // 0(���A����Ȃ�)��1(���A)  // PP_CheckRare( pp ) 
  u32                     personal_rnd;    // ?  // personal_rnd��monsno==MONSNO_PATTIIRU�̂Ƃ��͕K�{
  ZUKAN_INFO_LAUNCH       launch;
  ZUKAN_INFO_DISP         disp;
  u8                      bg_priority;
  GFL_CLUNIT*             clunit;          // ���̂Ƃ���̂��o���Ă��邾���Ő�����j���͂��Ȃ��B
  GFL_FONT*               font;            // ���̂Ƃ���̂��o���Ă��邾���Ő�����j���͂��Ȃ��B
  PRINT_QUE*              print_que;       // ���̂Ƃ���̂��o���Ă��邾���Ő�����j���͂��Ȃ��B

  // BG
  // fore = setumei, back = base
  GFL_ARCUTIL_TRANSINFO     fore_bg_chara_info;    ///< fore��BG�L�����̈�
  GFL_ARCUTIL_TRANSINFO     back_bg_chara_info;    ///< back��BG�L�����̈�
  u8                        fore_bg_update;        ///< fore��BG�X�V�r�b�g�t���O
  u8                        back_bg_update;        ///< back��BG�X�V�r�b�g�t���O

  // �����\�����Ȃ��Ƃ��p�̔w�i
  GFL_ARCUTIL_TRANSINFO     back_none_bg_chara_info;    // �����\�����Ȃ��Ƃ��p�̔w�i��BG�L�����̈�
  BOOL                      back_none_bg_need_free;     // �ǂݍ���ł�����TRUE
  BOOL                      back_none_bg_display_mode;  // �����\�����Ȃ��Ƃ��p�̔w�i�݂̂�\�����Ă��郂�[�h�̂Ƃ�TRUE

  PALTYPE    bg_paltype;
  u8         msg_bg_frame;
  u8         fore_bg_frame;
  u8         back_bg_frame;

  // TCB
  GFL_TCB*     vblank_tcb;      ///< VBlank����TCB

  // �X�e�b�v��^�C�~���O
  ZUKAN_INFO_STEP      step;
  BOOL                 move_flag;
  BOOL                 cry_flag;
  u32                  color_while_count;
  u32                  back_bg_scroll_wait_count;

  // WND
  int wnd_down_y;  // �E�B���h�E�̉E��Y���W

  //MSG�n
  GFL_BMPWIN*  bmpwin[ZUKAN_INFO_MSG_MAX];

  // �����X�^�[�{�[���̃}�[�N
  GFL_BMPWIN*  ball_yes_bmpwin;
  GFL_BMPWIN*  ball_no_bmpwin;

  // �|�P����2D
  u32          ncg_poke2d;
  u32          ncl_poke2d;
  u32          nce_poke2d;
  GFL_CLWK*    clwk_poke2d;

  // �^�C�v�A�C�R��
  u32       typeicon_cg_idx[2];
  u32       typeicon_cl_idx;
  u32       typeicon_cean_idx;
  GFL_CLWK* typeicon_clwk[2];

  // �|�P�����̑���
  UI_EASY_CLWK_RES clres_pokefoot;
  GFL_CLWK* clwk_pokefoot;

  // �p���b�g�A�j���[�V����
  u16 anm_cnt;
  u16 trans_buf[ZUKAN_INFO_PALETTE_ANIME_NUM];  // �^�̃T�C�Y��ZUKAN_INFO_COLOR_SIZE
  u16 palette_anime_s[ZUKAN_INFO_PALETTE_ANIME_NUM];
  u16 palette_anime_e[ZUKAN_INFO_PALETTE_ANIME_NUM];

  // Y�I�t�Z�b�g
  s32 y_offset;
};

//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
static void Zukan_Info_VBlankFunc( GFL_TCB* tcb, void* wk );

// Message
static void Zukan_Info_DrawStr( HEAPID heap_id, GFL_BMPWIN* bmpwin, GFL_MSGDATA* msgdata, PRINT_QUE* print_que, GFL_FONT* font,
                                u32 str_id, u16 x, u16 y, u16 color, ZUKAN_INFO_ALIGN align, WORDSET* wordset );
static void Zukan_Info_CreateMessage( ZUKAN_INFO_WORK* work );
static void Zukan_Info_CreateForeignMessage( ZUKAN_INFO_WORK* work, ZUKAN_INFO_LANG lang );
static void Zukan_Info_DeleteMessage( ZUKAN_INFO_WORK* work );

// �����X�^�[�{�[���̃}�[�N
static void Zukan_Info_CreateBall( ZUKAN_INFO_WORK* work );
static void Zukan_Info_DeleteBall( ZUKAN_INFO_WORK* work );
static void Zukan_Info_TransBall_VBlank( ZUKAN_INFO_WORK* work );

//�|�P����2D
static void Zukan_Info_Poke2dLoadResourceObj( ZUKAN_INFO_WORK* work );
static void Zukan_Info_Poke2dUnloadResourceObj( ZUKAN_INFO_WORK* work );
static void Zukan_Info_Poke2dCreateCLWK( ZUKAN_INFO_WORK* work, u16 pos_x, u16 pos_y );
static void Zukan_Info_Poke2dDeleteCLWK( ZUKAN_INFO_WORK* work );

// �^�C�v�A�C�R��
static void Zukan_Info_CreateTypeicon( ZUKAN_INFO_WORK* work, PokeType type1, PokeType type2 );
static void Zukan_Info_DeleteTypeicon( ZUKAN_INFO_WORK* work );

// �|�P�����̑���
static void Zukan_Info_CreatePokefoot( ZUKAN_INFO_WORK* work, u32 monsno );
static void Zukan_Info_DeletePokefoot( ZUKAN_INFO_WORK* work );

// �|�P����2D�ȊO���܂Ƃ߂�
static void Zukan_Info_CreateOthers( ZUKAN_INFO_WORK* work );

// �p���b�g�A�j���[�V����
static void Zukan_Info_UpdatePaletteAnime( ZUKAN_INFO_WORK* work );

// �SOBJ�̕\��/��\����ݒ肷��
void ZUKAN_INFO_SetDrawEnableAllObj( ZUKAN_INFO_WORK* work, BOOL on_off );


//=============================================================================
/**
 *  �O�����J�֐���`
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief     ����������
 *
 *  @param[in] a_heap_id         �q�[�vID
 *  @param[in] a_pp              �|�P�����p�����[�^
 *  @param[in] a_zenkoku_flag    �S���}�ӂɂȂ��Ă�����TRUE
 *  @param[in] a_get_flag        �Q�b�g���Ă�����TRUE
 *  @param[in] a_launch          �N�����@
 *  @param[in] a_disp            �\����
 *  @param[in] a_bg_priority     0��1(0�̂Ƃ���012,1�̂Ƃ���123�Ƃ����v���C�I���e�B�ɂȂ�)
 *  @param[in] a_clunit          �Z���A�N�^�[���j�b�g
 *  @param[in] a_font            �t�H���g
 *  @param[in] a_print_que       �v�����g�L���[
 *
 *  @retval    �����������[�N
 */
//-----------------------------------------------------------------------------
ZUKAN_INFO_WORK* ZUKAN_INFO_Init(
                     HEAPID                   a_heap_id,
                     const POKEMON_PARAM*     a_pp,
                     BOOL                     a_zenkoku_flag,
                     BOOL                     a_get_flag,
                     ZUKAN_INFO_LAUNCH        a_launch,
                     ZUKAN_INFO_DISP          a_disp,
                     u8                       a_bg_priority,
                     GFL_CLUNIT*              a_clunit,
                     GFL_FONT*                a_font,
                     PRINT_QUE*               a_print_que )
{
  //BOOL fast         = PP_FastModeOn( a_pp );  // ���������[�h��const�͂����Ȃ��Ƃ����Ȃ��̂ŁA�~�߂�
  u16  monsno       = (u16)PP_Get( a_pp, ID_PARA_monsno, NULL );
  u16  formno       = (u16)PP_Get( a_pp, ID_PARA_form_no, NULL );
  u16  sex          = PP_GetSex( a_pp );
  u16  rare         = PP_CheckRare( a_pp );
  u32  personal_rnd = (u16)PP_Get( a_pp, ID_PARA_personal_rnd, NULL );
  //if( fast ) PP_FastModeOff( a_pp, fast );

  return ZUKAN_INFO_InitFromMonsno(
             a_heap_id,
             monsno,
             formno,
             sex,
             rare,
             personal_rnd,
             a_zenkoku_flag,
             a_get_flag,
             a_launch,
             a_disp,
             a_bg_priority,
             a_clunit,
             a_font,
             a_print_que );
}

ZUKAN_INFO_WORK* ZUKAN_INFO_InitFromMonsno(
                            HEAPID                 a_heap_id,
                            u16                    a_monsno,
                            u16                    a_formno,        // 0<=formno<POKETOOL_GetPersonalParam( monsno, 0, POKEPER_ID_form_max )
                            u16                    a_sex,           // PTL_SEX_MALE, PTL_SEX_FEMALE, PTL_SEX_UNKNOWN
                            u16                    a_rare,          // 0(���A����Ȃ�)��1(���A)  // PP_CheckRare( pp ) 
                            u32                    a_personal_rnd,  // ?  // personal_rnd��monsno==MONSNO_PATTIIRU�̂Ƃ��͕K�{
                            BOOL                   a_zenkoku_flag,
                            BOOL                   a_get_flag,
                            ZUKAN_INFO_LAUNCH      a_launch,
                            ZUKAN_INFO_DISP        a_disp,
                            u8                     a_bg_priority,
                            GFL_CLUNIT*            a_clunit,
                            GFL_FONT*              a_font,
                            PRINT_QUE*             a_print_que )
{
  ZUKAN_INFO_WORK* work;

  // ���[�N�����A������
  {
    u32 size = sizeof(ZUKAN_INFO_WORK);
    work = GFL_HEAP_AllocMemory( a_heap_id, size );
    GFL_STD_MemClear( work, size );
  }

  // ����
  {
    work->heap_id             = a_heap_id;
    work->monsno              = a_monsno;
    work->formno              = a_formno;
    work->sex                 = a_sex;   
    work->rare                = a_rare; 
    work->personal_rnd        = a_personal_rnd;
    work->zenkoku_flag        = a_zenkoku_flag;
    work->get_flag            = a_get_flag;
    work->launch              = a_launch;
    work->disp                = a_disp;
    work->bg_priority         = a_bg_priority;
    work->clunit              = a_clunit;
    work->font                = a_font;
    work->print_que           = a_print_que;
  }

  // ������
  {
    // BG
    if( work->disp == ZUKAN_INFO_DISP_M )
    {
      work->bg_paltype    = PALTYPE_MAIN_BG;
      work->msg_bg_frame  = ZUKAN_INFO_BG_FRAME_M_MSG; 
      work->fore_bg_frame = ZUKAN_INFO_BG_FRAME_M_FORE; 
      work->back_bg_frame = ZUKAN_INFO_BG_FRAME_M_BACK; 
    }
    else
    {
      work->bg_paltype    = PALTYPE_SUB_BG;
      work->msg_bg_frame  = ZUKAN_INFO_BG_FRAME_S_MSG; 
      work->fore_bg_frame = ZUKAN_INFO_BG_FRAME_S_FORE; 
      work->back_bg_frame = ZUKAN_INFO_BG_FRAME_S_BACK; 
    }
    GFL_BG_SetPriority( work->msg_bg_frame, work->bg_priority );  // �őO��
    GFL_BG_SetPriority( work->fore_bg_frame, work->bg_priority +1 );
    GFL_BG_SetPriority( work->back_bg_frame, work->bg_priority +2 );

    work->fore_bg_update = ZUKAN_INFO_BG_UPDATE_BIT_RESET;
    work->back_bg_update = ZUKAN_INFO_BG_UPDATE_BIT_RESET;
 
    // �X�e�b�v��^�C�~���O
    // �N�����@
    switch( work->launch )
    {
    case ZUKAN_INFO_LAUNCH_TOROKU:
      {
        work->step = ZUKAN_INFO_STEP_COLOR_WAIT;
      }
      break;
    case ZUKAN_INFO_LAUNCH_NICKNAME:
      {
        work->step = ZUKAN_INFO_STEP_CENTER;
      }
      break;
    case ZUKAN_INFO_LAUNCH_LIST:
      {
        work->step = ZUKAN_INFO_STEP_START_STILL;
      }
      break;
    }
    work->move_flag                 = FALSE;
    work->cry_flag                  = FALSE;
    work->color_while_count         = 0;
    work->back_bg_scroll_wait_count = 0;

    // �p���b�g�A�j���[�V����
    work->anm_cnt = 0;
        
    // Y�I�t�Z�b�g
    if( work->launch == ZUKAN_INFO_LAUNCH_LIST && work->disp == ZUKAN_INFO_DISP_M )
    {
      work->y_offset = ZUKAN_INFO_Y_OFFSET;
    }
    else
    {
      work->y_offset = 0;
    }
  }

  // ���\�[�X�ǂݍ���
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, work->heap_id );
    GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_zukan_gra_info_zukan_bgu_NCLR, work->bg_paltype,
                                      ZUKAN_INFO_BG_PAL_POS_FORE_BACK * ZUKAN_INFO_PAL_LINE_SIZE,
                                      ZUKAN_INFO_BG_PAL_NUM_FORE_BACK * ZUKAN_INFO_PAL_LINE_SIZE,
                                      work->heap_id );

    // fore
    work->fore_bg_chara_info = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( handle, NARC_zukan_gra_info_zukan_bgu_NCGR,
                                                                            work->fore_bg_frame,
                                                                            ZUKAN_INFO_BG_CHARA_SIZE, FALSE,
                                                                            work->heap_id );
    GF_ASSERT_MSG( work->fore_bg_chara_info != GFL_ARCUTIL_TRANSINFO_FAIL, "ZUKAN_INFO : BG�L�����̈悪����܂���ł����B\n" );
    
    GFL_ARCHDL_UTIL_TransVramScreen( handle, NARC_zukan_gra_info_setumei_bgu_NSCR, work->fore_bg_frame,
                                     GFL_ARCUTIL_TRANSINFO_GetPos( work->fore_bg_chara_info ),
                                     ZUKAN_INFO_BG_SCREEN_SIZE, FALSE, work->heap_id );

    // back
    work->back_bg_chara_info = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( handle, NARC_zukan_gra_info_zukan_bgu_NCGR,
                                                                            work->back_bg_frame,
                                                                            ZUKAN_INFO_BG_CHARA_SIZE, FALSE, work->heap_id );
    GF_ASSERT_MSG( work->back_bg_chara_info != GFL_ARCUTIL_TRANSINFO_FAIL, "ZUKAN_INFO : BG�L�����̈悪����܂���ł����B\n" );
        
    GFL_ARCHDL_UTIL_TransVramScreen( handle, NARC_zukan_gra_info_base_bgu_NSCR, work->back_bg_frame,
                                     GFL_ARCUTIL_TRANSINFO_GetPos( work->back_bg_chara_info ),
                                     ZUKAN_INFO_BG_SCREEN_SIZE, FALSE, work->heap_id );
    
    GFL_ARC_CloseDataHandle( handle );
  }

  // �w�i�̉��H
  {
    // ������
    work->back_none_bg_need_free = FALSE;
    work->back_none_bg_display_mode = FALSE;
    
    if( work->launch == ZUKAN_INFO_LAUNCH_LIST && work->disp == ZUKAN_INFO_DISP_S )
    {
      // �|�P����������@�Ƃ��낭�@�����傤�I
      // �̏ꏊ������
      GFL_BG_FillScreen( work->fore_bg_frame, 0, 0, 0, 32, 3, GFL_BG_SCRWRT_PALIN );

      // �w�i��ǉ�����
      {
        // �����\�����Ȃ��Ƃ��p�̔w�i
        ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, work->heap_id );

        GFL_ARCHDL_UTIL_TransVramPaletteEx(
            handle,
            NARC_zukan_gra_info_info_bgu_NCLR,
            work->bg_paltype,
            0 * 0x20,
            ZUKAN_INFO_BG_PAL_POS_BACK_NONE * 0x20,
            ZUKAN_INFO_BG_PAL_NUM_BACK_NONE * 0x20,
            work->heap_id );

        work->back_none_bg_chara_info = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
                                            handle,
                                            NARC_zukan_gra_info_info_bgu_NCGR,
                                            work->back_bg_frame,
                                            32 * 8 * GFL_BG_1CHRDATASIZ,
                                            FALSE,
                                            work->heap_id );
        GF_ASSERT_MSG( work->back_none_bg_chara_info != GFL_ARCUTIL_TRANSINFO_FAIL, "ZUKAN_INFO : BG�L�����̈悪����܂���ł����B\n" );

        work->back_none_bg_need_free = TRUE;

        GFL_ARCHDL_UTIL_TransVramScreenCharOfs(
            handle,
            NARC_zukan_gra_info_infobase_bgu_NSCR,
            work->back_bg_frame,
            32 * 32,
            GFL_ARCUTIL_TRANSINFO_GetPos( work->back_none_bg_chara_info ),
            32 * 32 * GFL_BG_1SCRDATASIZ,
            FALSE,
            work->heap_id );

        GFL_BG_ChangeScreenPalette( work->back_bg_frame, 0,    32, 32, 21, ZUKAN_INFO_BG_PAL_POS_BACK_NONE    );
        GFL_BG_ChangeScreenPalette( work->back_bg_frame, 0, 32+21, 32,  3, ZUKAN_INFO_BG_PAL_POS_BACK_NONE +1 );

        GFL_ARC_CloseDataHandle( handle );
      }
    }
  }

  // �p���b�g�A�j���[�V����
  {
    NNSG2dPaletteData* pal_data;
    u16* pal_raw;
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, work->heap_id );
    u32 size = GFL_ARC_GetDataSizeByHandle( handle, NARC_zukan_gra_info_zukan_bgu_NCLR );  // BG�̃p���b�g
    void* pal_src = GFL_HEAP_AllocMemory( work->heap_id, size );
    GFL_ARC_LoadDataByHandle( handle, NARC_zukan_gra_info_zukan_bgu_NCLR, pal_src );
    NNS_G2dGetUnpackedPaletteData( pal_src, &pal_data );
    pal_raw = (u16*)(pal_data->pRawData);
    {
      u8 i;
      for( i=0; i<ZUKAN_INFO_PALETTE_ANIME_NUM; i++ )
      {
        work->palette_anime_s[i] = pal_raw[ ZUKAN_INFO_PAL_LINE_COLOR_NUM*ZUKAN_INFO_PALETTE_ANIME_LINE_S + ZUKAN_INFO_PALETTE_ANIME_NO + i];
        work->palette_anime_e[i] = pal_raw[ ZUKAN_INFO_PAL_LINE_COLOR_NUM*ZUKAN_INFO_PALETTE_ANIME_LINE_E + ZUKAN_INFO_PALETTE_ANIME_NO + i];
      }
    }
    GFL_HEAP_FreeMemory( pal_src );
    GFL_ARC_CloseDataHandle( handle );
  }

  // �����X�^�[�{�[���̃}�[�N
  Zukan_Info_CreateBall( work );
  Zukan_Info_TransBall_VBlank( work );

  // TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Zukan_Info_VBlankFunc, work, ZUKAN_INFO_VBLANK_TCB_PRI );

  // �|�P����2D�ȊO
  if(    work->launch == ZUKAN_INFO_LAUNCH_TOROKU
      || work->launch == ZUKAN_INFO_LAUNCH_LIST )
  {
    Zukan_Info_CreateOthers( work );
  }

  //�|�P����2D
  {
    u16 pos_x, pos_y;
    switch(work->launch)
    {
    case ZUKAN_INFO_LAUNCH_TOROKU:
    case ZUKAN_INFO_LAUNCH_LIST:
      {
        pos_x = ZUKAN_INFO_START_POKEMON_POS_X;
        pos_y = ZUKAN_INFO_START_POKEMON_POS_Y;
      }
      break;
    case ZUKAN_INFO_LAUNCH_NICKNAME:
      {
        pos_x = ZUKAN_INFO_CENTER_POKEMON_POS_X;
        pos_y = ZUKAN_INFO_CENTER_POKEMON_POS_Y;
      }
      break;
    }
    Zukan_Info_Poke2dLoadResourceObj( work );
    Zukan_Info_Poke2dCreateCLWK( work, pos_x, (u16)( pos_y + work->y_offset ) );
  }

  // WND
  if( work->launch == ZUKAN_INFO_LAUNCH_TOROKU )
  {
    work->wnd_down_y = ZUKAN_INFO_WND_DOWN_Y_START;
    GX_SetVisibleWnd( GX_WNDMASK_W0 );
    G2_SetWnd0Position( ZUKAN_INFO_WND_LEFT_X, ZUKAN_INFO_WND_UP_Y, ZUKAN_INFO_WND_RIGHT_X, work->wnd_down_y );
    G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ,
                           TRUE );
    G2_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ,
                           FALSE );
    G2_SetBlendBrightness( GX_BLEND_PLANEMASK_OBJ,
                           -16 );
  }

  // BG��Y�I�t�Z�b�g
  GFL_BG_SetScroll( work->fore_bg_frame, GFL_BG_SCROLL_Y_SET, - work->y_offset );
  GFL_BG_SetScroll( work->back_bg_frame, GFL_BG_SCROLL_Y_SET, - work->y_offset );
  GFL_BG_SetScroll( work->msg_bg_frame, GFL_BG_SCROLL_Y_SET, - work->y_offset );

  // BG
  GFL_BG_LoadScreenReq( work->fore_bg_frame );
  GFL_BG_LoadScreenReq( work->back_bg_frame );

  return work;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �I������ 
 *
 *  @param[in,out] work  ZUKAN_INFO_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void ZUKAN_INFO_Exit( ZUKAN_INFO_WORK* work )
{
  // �����\�����Ȃ��Ƃ��p�̔w�i
  {
    // �����\�����Ȃ��Ƃ��p�̔w�i����ʏ�̕\���ɂ��Ă���
    ZUKAN_INFO_DisplayNormal( work );
    work->back_none_bg_display_mode = FALSE;
  }

  // �|�P����2D
  Zukan_Info_Poke2dDeleteCLWK( work );
  Zukan_Info_Poke2dUnloadResourceObj( work );

  // �|�P����2D�ȊO
  if( work->launch == ZUKAN_INFO_LAUNCH_LIST )  // TOROKU�̏ꍇ�͊��ɔj������Ă���
  {
    ZUKAN_INFO_DeleteOthers( work );
  }

  // TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �����X�^�[�{�[���̃}�[�N
  Zukan_Info_DeleteBall( work );

  // �ǂݍ��񂾃��\�[�X�̔j��
  {
    // �����\�����Ȃ��Ƃ��p�̔w�i
    if( work->back_none_bg_need_free )
    {
      GFL_BG_FreeCharacterArea( work->back_bg_frame, GFL_ARCUTIL_TRANSINFO_GetPos( work->back_none_bg_chara_info ),
                                GFL_ARCUTIL_TRANSINFO_GetSize( work->back_none_bg_chara_info ) );
      work->back_none_bg_need_free = FALSE;
    }

    // back
    GFL_BG_FreeCharacterArea( work->back_bg_frame, GFL_ARCUTIL_TRANSINFO_GetPos( work->back_bg_chara_info ),
                              GFL_ARCUTIL_TRANSINFO_GetSize( work->back_bg_chara_info ) );
      
    // fore
    GFL_BG_FreeCharacterArea( work->fore_bg_frame, GFL_ARCUTIL_TRANSINFO_GetPos( work->fore_bg_chara_info ),
                              GFL_ARCUTIL_TRANSINFO_GetSize( work->fore_bg_chara_info ) );
  }

  // ���[�N�j��
  {
    GFL_HEAP_FreeMemory( work );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �又�� 
 *
 *  @param[in,out] work  ZUKAN_INFO_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void ZUKAN_INFO_Main( ZUKAN_INFO_WORK* work )
{
  // �p���b�g�A�j���[�V����
  Zukan_Info_UpdatePaletteAnime( work );

  // BG�X�N���[��
  {
    work->back_bg_scroll_wait_count++;
    if( work->back_bg_scroll_wait_count == ZUKAN_INFO_BACK_BG_SCROLL_WAIT )
    {
      work->back_bg_update |= ZUKAN_INFO_BACK_BG_UPDATE_BIT_SCROLL;
      work->back_bg_scroll_wait_count = 0;
    }
  }

  // �X�e�b�v
  switch(work->step)
  {
  case ZUKAN_INFO_STEP_COLOR_WAIT:
    {
    }
    break;
  case ZUKAN_INFO_STEP_COLOR_WHILE:
    {
      work->color_while_count++;
      if( work->color_while_count == ZUKAN_INFO_COLOR_WHILE )
      {
        work->step = ZUKAN_INFO_STEP_COLOR;
      }
    }
    break;
  case ZUKAN_INFO_STEP_COLOR:
    {
      work->wnd_down_y--;
      G2_SetWnd0Position( ZUKAN_INFO_WND_LEFT_X, ZUKAN_INFO_WND_UP_Y, ZUKAN_INFO_WND_RIGHT_X, work->wnd_down_y );
      if( work->wnd_down_y == ZUKAN_INFO_WND_DOWN_Y_GOAL )
      {
        GX_SetVisibleWnd( GX_WNDMASK_NONE );
        G2_SetBlendBrightness( GX_BLEND_PLANEMASK_OBJ,
                               0 );
        PMVOICE_Play( work->monsno, work->formno, 64, FALSE, 0, 0, FALSE, 0 );
        work->step = ZUKAN_INFO_STEP_MOVE_WAIT;
      }
    }
    break;
  case ZUKAN_INFO_STEP_MOVE_WAIT:
    {
      if(work->move_flag)
      {
        work->step = ZUKAN_INFO_STEP_MOVE;
      }
    }
    break;
  case ZUKAN_INFO_STEP_MOVE:
    {
      // work->y_offset���g�p����Ƃ��́A�����ɂ͗��Ȃ�

      BOOL goal_x = FALSE;
      BOOL goal_y = FALSE;

      // �|�P����2D
      GFL_CLACTPOS pos;
      GFL_CLACT_WK_GetWldPos( work->clwk_poke2d, &pos );
      pos.x += 1;
      pos.y += 1;
      if( pos.x > ZUKAN_INFO_CENTER_POKEMON_POS_X )
      {
        pos.x = ZUKAN_INFO_CENTER_POKEMON_POS_X;
        goal_x = TRUE;
      }
      if( pos.y > ZUKAN_INFO_CENTER_POKEMON_POS_Y )
      {
        pos.y = ZUKAN_INFO_CENTER_POKEMON_POS_Y;
        goal_y = TRUE;
      }
      GFL_CLACT_WK_SetWldPos( work->clwk_poke2d, &pos );
            
      if(goal_x && goal_y)
      {
        work->step = ZUKAN_INFO_STEP_CENTER_STILL;
      }
    }
    break;
  case ZUKAN_INFO_STEP_CENTER:
    {
      if(work->cry_flag)
      {
        PMVOICE_Play( work->monsno, work->formno, 64, FALSE, 0, 0, FALSE, 0 );
        work->step = ZUKAN_INFO_STEP_CENTER_STILL;
      }
    }
    break;
  case ZUKAN_INFO_STEP_CENTER_STILL:
    {
    }
    break;
  case ZUKAN_INFO_STEP_START_STILL:
    {
    }
    break;
  }

}

//-----------------------------------------------------------------------------
/**
 *  @brief         �|�P����2D�ȊO�̂��̂�j������ 
 *
 *  @param[in,out] work  ZUKAN_INFO_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void ZUKAN_INFO_DeleteOthers( ZUKAN_INFO_WORK* work )
{
  // �|�P�����̑���
  Zukan_Info_DeletePokefoot( work );
  // �^�C�v�A�C�R��
  Zukan_Info_DeleteTypeicon( work ); 
  // Message
  Zukan_Info_DeleteMessage( work );
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �|�P����2D�̈ړ��J�n���t���O�𗧂Ă� 
 *
 *  @param[in,out] work  ZUKAN_INFO_Init�Ő����������[�N
 *
 *  @retval        �Ȃ�
 *
 *  @note          �����ֈړ��ł���i�K�ɂȂ�����ړ����Ȃ�����A�Ƃ����t���O�𗧂Ă邾���Ȃ̂ŁA�����Ɉړ�����Ƃ͌���Ȃ��B
 */
//-----------------------------------------------------------------------------
void ZUKAN_INFO_Move( ZUKAN_INFO_WORK* work )
{
  work->move_flag = TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �|�P�����̖����t���O�𗧂Ă�
 *
 *  @param[in,out] work  ZUKAN_INFO_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void ZUKAN_INFO_Cry( ZUKAN_INFO_WORK* work )
{
  work->cry_flag = TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief     �|�P����2D���ړ������ۂ� 
 *
 *  @param[in] work  ZUKAN_INFO_Init�Ő����������[�N
 *
 *  @retval    �ړ����J�n���Ă�����TRUE��Ԃ��B�ړ����J�n���Ă��Ȃ��Ƃ���FALSE��Ԃ��B�ړ����I�����Ă�����FALSE��Ԃ��B 
 */
//-----------------------------------------------------------------------------
BOOL ZUKAN_INFO_IsMove( ZUKAN_INFO_WORK* work )
{
  BOOL ret = FALSE;
  if( work->step == ZUKAN_INFO_STEP_MOVE ) ret = TRUE;
  return ret;
}

//-----------------------------------------------------------------------------
/**
 *  @brief     �|�P����2D�������ł����Ƃ��Ă��邩�ۂ�
 *
 *  @param[in] work  ZUKAN_INFO_Init�Ő����������[�N
 *
 *  @retval    �����ŐÎ~���Ă�����TRUE 
 */
//-----------------------------------------------------------------------------
BOOL ZUKAN_INFO_IsCenterStill( ZUKAN_INFO_WORK* work )
{
  BOOL ret = FALSE;
  if( work->step == ZUKAN_INFO_STEP_CENTER_STILL ) ret = TRUE;
  return ret;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �������J�n������
 *
 *  @param[in,out] work  ZUKAN_INFO_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void ZUKAN_INFO_Start( ZUKAN_INFO_WORK* work )
{
  if( work->step == ZUKAN_INFO_STEP_COLOR_WAIT )
  {
    work->step = ZUKAN_INFO_STEP_COLOR_WHILE;
  }
}

//-------------------------------------
/// �|�P������ύX����
//=====================================
void ZUKAN_INFO_ChangePoke(
                ZUKAN_INFO_WORK* work,
                u16              monsno,
                u16              formno,
                u16              sex,
                u16              rare,
                u32              personal_rnd,
                BOOL             get_flag )
{
  // �O�̂��폜����
  // �|�P����2D
  Zukan_Info_Poke2dDeleteCLWK( work );
  Zukan_Info_Poke2dUnloadResourceObj( work );
  // �|�P����2D�ȊO
  ZUKAN_INFO_DeleteOthers( work );

  // ���̂𐶐�����
  work->monsno         = monsno;
  work->formno         = formno;
  work->sex            = sex;   
  work->rare           = rare; 
  work->personal_rnd   = personal_rnd;
  work->get_flag       = get_flag;

  // �|�P����2D�ȊO
  Zukan_Info_CreateOthers( work );
  //�|�P����2D
  {
    u16 pos_x, pos_y;
    switch(work->launch)
    {
    case ZUKAN_INFO_LAUNCH_TOROKU:
    case ZUKAN_INFO_LAUNCH_LIST:
      {
        pos_x = ZUKAN_INFO_START_POKEMON_POS_X;
        pos_y = ZUKAN_INFO_START_POKEMON_POS_Y;
      }
      break;
    case ZUKAN_INFO_LAUNCH_NICKNAME:
      {
        pos_x = ZUKAN_INFO_CENTER_POKEMON_POS_X;
        pos_y = ZUKAN_INFO_CENTER_POKEMON_POS_Y;
      }
      break;
    }
    Zukan_Info_Poke2dLoadResourceObj( work );
    Zukan_Info_Poke2dCreateCLWK( work, pos_x, (u16)( pos_y + work->y_offset ) );
  }

  // �SOBJ�̕\��/��\����ݒ肷��
  ZUKAN_INFO_SetDrawEnableAllObj( work, !work->back_none_bg_display_mode );
}

//-------------------------------------
/// �����ύX����
//=====================================
void ZUKAN_INFO_ChangeLang( ZUKAN_INFO_WORK* work,
                ZUKAN_INFO_LANG lang )
{
  // �O�̂��폜����
  Zukan_Info_DeleteMessage( work );
  
  // ���̂𐶐�����
  Zukan_Info_CreateForeignMessage( work, lang );
}

//-------------------------------------
/// �|�P�����ƌ����ύX����
//=====================================
void ZUKAN_INFO_ChangePokeAndLang(
                ZUKAN_INFO_WORK* work,
                u16              monsno,
                u16              formno,
                u16              sex,
                u16              rare,
                u32              personal_rnd,
                BOOL             get_flag,
                ZUKAN_INFO_LANG  lang )
{
  ZUKAN_INFO_ChangePoke( work, monsno, formno, sex, rare, personal_rnd, get_flag );
  ZUKAN_INFO_ChangeLang( work, lang );
}

//-------------------------------------
/// �����\�����Ȃ��Ƃ��p�̔w�i�݂̂�\������
//=====================================
void ZUKAN_INFO_DisplayBackNone( ZUKAN_INFO_WORK* work )
{
  u8 i;
  
  if( work->back_none_bg_display_mode ) return;  // ���ɁA�����\�����Ȃ��Ƃ��p�̔w�i�݂̂�\�����Ă���

  work->back_none_bg_display_mode = TRUE;

  // BG
  GFL_BG_SetVisible( work->msg_bg_frame,  VISIBLE_OFF );
  GFL_BG_SetVisible( work->fore_bg_frame, VISIBLE_OFF );

  GFL_BG_SetScrollReq( work->back_bg_frame, GFL_BG_SCROLL_Y_SET, -32*8 );

  // �SOBJ�̕\��/��\����ݒ肷��
  ZUKAN_INFO_SetDrawEnableAllObj( work, !work->back_none_bg_display_mode );
}

//-------------------------------------
/// �ʏ�̕\���ɂ���
//=====================================
void ZUKAN_INFO_DisplayNormal( ZUKAN_INFO_WORK* work )
{
  u8 i;
  
  if( !(work->back_none_bg_display_mode) ) return;  // ���ɁA�ʏ�̕\��

  work->back_none_bg_display_mode = FALSE;
  
  // BG
  GFL_BG_SetVisible( work->msg_bg_frame,  VISIBLE_ON );
  GFL_BG_SetVisible( work->fore_bg_frame, VISIBLE_ON );

  GFL_BG_SetScrollReq( work->back_bg_frame, GFL_BG_SCROLL_Y_SET, - work->y_offset );

  // �SOBJ�̕\��/��\����ݒ肷��
  ZUKAN_INFO_SetDrawEnableAllObj( work, !work->back_none_bg_display_mode );
}


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// VBlank���ɌĂяo�����֐�
//=====================================
static void Zukan_Info_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  ZUKAN_INFO_WORK* work = (ZUKAN_INFO_WORK*)wk;

  if( work->back_bg_update & ZUKAN_INFO_BACK_BG_UPDATE_BIT_SCROLL )
  {
    GFL_BG_SetScrollReq( work->back_bg_frame, GFL_BG_SCROLL_X_INC, ZUKAN_INFO_BACK_BG_SCROLL_X_VALUE );
  }

  work->fore_bg_update = ZUKAN_INFO_BG_UPDATE_BIT_RESET;
  work->back_bg_update = ZUKAN_INFO_BG_UPDATE_BIT_RESET;
}

//-------------------------------------
/// ����������
//=====================================
static void Zukan_Info_DrawStr( HEAPID heap_id, GFL_BMPWIN* bmpwin, GFL_MSGDATA* msgdata, PRINT_QUE* print_que, GFL_FONT* font,
                                u32 str_id, u16 x, u16 y, u16 color, ZUKAN_INFO_ALIGN align, WORDSET* wordset )
{
  // LEFT�̂Ƃ���(x,y)������ɂȂ�悤�ɏ���
  // RIGHT�̂Ƃ���(x,y)���E��ɂȂ�悤�ɏ���
  // CENTER�̂Ƃ���(bmpwin�̍��E�����ɂȂ�ʒu,y)�ɂȂ�悤�ɏ���(x�͎g�p���Ȃ�)

  GFL_BMP_DATA* bmp_data = GFL_BMPWIN_GetBmp( bmpwin );
  u16 ax;
  STRBUF* strbuf;
  u16 str_width;
  u16 bmp_width;

  if( wordset )
  {
    STRBUF* src_strbuf = GFL_MSG_CreateString( msgdata, str_id );
    strbuf = GFL_STR_CreateBuffer( ZUKAN_INFO_STRBUF_LEN, heap_id );
    WORDSET_ExpandStr( wordset, strbuf, src_strbuf );
    GFL_STR_DeleteBuffer( src_strbuf );
  }
  else
  {
    strbuf = GFL_MSG_CreateString( msgdata, str_id );
  }

  switch( align )
  {
  case ZUKAN_INFO_ALIGN_LEFT:
    {
      ax = x;
    }
    break;
  case ZUKAN_INFO_ALIGN_RIGHT:
    {
      str_width = (u16)( PRINTSYS_GetStrWidth( strbuf, font, 0 ) );
      ax = x - str_width;
    }
    break;
  case ZUKAN_INFO_ALIGN_CENTER:
    {
      str_width = (u16)( PRINTSYS_GetStrWidth( strbuf, font, 0 ) );
      bmp_width = GFL_BMP_GetSizeX( bmp_data );
      ax = ( bmp_width - str_width ) / 2;
    }
    break;
  }
 
  PRINTSYS_PrintQueColor( print_que, bmp_data, 
                          ax, y, strbuf, font, color );
        
  GFL_STR_DeleteBuffer( strbuf );
}

//-------------------------------------
/// BG�ɕ���������
//=====================================
static void Zukan_Info_CreateMessage( ZUKAN_INFO_WORK* work )
{
  GFL_MSGDATA* msgdata_common;
  GFL_MSGDATA* msgdata_kind;
  GFL_MSGDATA* msgdata_height;
  GFL_MSGDATA* msgdata_weight;
  GFL_MSGDATA* msgdata_explain;

  PALTYPE paltype = (work->disp==ZUKAN_INFO_DISP_M)?(PALTYPE_MAIN_BG):(PALTYPE_SUB_BG);

  msgdata_common = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_dat, work->heap_id );
  msgdata_kind = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_type_dat, work->heap_id );
  msgdata_height = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_height_dat, work->heap_id );
  msgdata_weight = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_weight_dat, work->heap_id );
  msgdata_explain = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_comment_00_dat, work->heap_id );

  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, paltype,
                                 ZUKAN_INFO_BG_PAL_POS_MSG * ZUKAN_INFO_PAL_LINE_SIZE,
                                 ZUKAN_INFO_BG_PAL_NUM_MSG * ZUKAN_INFO_PAL_LINE_SIZE,
                                 work->heap_id );

  {
    s32 i;

    // �L�����N�^�[�P�ʂ�X���W�AY���W�AX�T�C�Y�AY�T�C�Y���w�肷��
    const u8 pos_siz[ZUKAN_INFO_MSG_MAX][4] =
    {
      {  2,  0, 28,  3 },
      { 16,  4, 15,  5 },
      { 18, 12, 13,  5 },
      {  2, 17, 28,  7 },
    };

    for( i=0; i<ZUKAN_INFO_MSG_MAX; i++ )
    {
      // bmpwin�̃r�b�g�}�b�v�𓧖��F�œh��ׂ��Ă������ق��������H��GFL_BMP_Clear���邱�Ƃɂ���
      work->bmpwin[i] = GFL_BMPWIN_Create( work->msg_bg_frame,
                                           pos_siz[i][0], pos_siz[i][1], pos_siz[i][2], pos_siz[i][3],
                                           ZUKAN_INFO_BG_PAL_POS_MSG, GFL_BMP_CHRAREA_GET_B );
      // �N���A
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->bmpwin[i] ), 0 );
    }

    if( work->launch == ZUKAN_INFO_LAUNCH_LIST && work->disp == ZUKAN_INFO_DISP_S )
    {
      // �|�P����������@�Ƃ��낭�@�����傤�I
      // �̏ꏊ�ɉ��������Ȃ��悤�ɂ���
    }
    else
    {
      // �|�P����������@�Ƃ��낭�@�����傤�I
      Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_TOROKU], msgdata_common,
                          work->print_que, work->font,
                          ZKN_POKEGET_00, 0, 5, PRINTSYS_LSB_Make(0xF,2,0), ZUKAN_INFO_ALIGN_CENTER, NULL );
    }

    // 025(�|�P�����̔ԍ�)
    {
      WORDSET* wordset = WORDSET_Create( work->heap_id );
      WORDSET_RegisterNumber( wordset, 0, work->monsno, 3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
      Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_NAME], msgdata_common,
                          work->print_que, work->font,
                          ZNK_POKELIST_17, 8, 7, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, wordset );
      WORDSET_Delete( wordset );
    }
    
    // ��F�s�J�`���E
    {
      STRBUF* strbuf;
      strbuf = GFL_MSG_CreateString( GlobalMsg_PokeName, work->monsno );
      PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp( work->bmpwin[ZUKAN_INFO_MSG_NAME] ),
                              48, 7, strbuf, work->font, PRINTSYS_LSB_Make(1,2,0) );  
      GFL_STR_DeleteBuffer( strbuf );
    }

    // ��F�˂��݃|�P����
    Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_NAME], msgdata_kind, work->print_que, work->font,
                        ZKN_TYPE_000 + work->monsno, 16, 23, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );
    
    // ������
    Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_PHYSICAL], msgdata_common, work->print_que, work->font,
                        ZNK_POKELIST_19, 0, 4, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );      
    
    // ������
    Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_PHYSICAL], msgdata_common, work->print_que, work->font,
                        ZNK_POKELIST_20, 0, 20, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );      
    
    // �H�H�H.�Hm  // ����w��Ȃ�(40, 4)  // m��kg�ŏ����_�������悤��
    Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_PHYSICAL], msgdata_height, work->print_que, work->font,
                        ZKN_HEIGHT_000_000 + work->monsno, 94, 4, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_RIGHT, NULL );
    
    // �H�H�H.�Hkg  // ����w��Ȃ�(40,20)
    Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_PHYSICAL], msgdata_weight, work->print_que, work->font,
                        ZKN_WEIGHT_000_000 + work->monsno, 100, 20, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_RIGHT, NULL );
    
    // ��F�������@���݂̂��@�ł񂰂���
    Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_EXPLAIN], msgdata_explain, work->print_que, work->font,
                        ZKN_COMMENT_00_000_000 + work->monsno, 4, 5, PRINTSYS_LSB_Make(0xF,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );      

    for( i=0; i<ZUKAN_INFO_MSG_MAX; i++ )
    {
      GFL_BMPWIN_MakeTransWindow_VBlank( work->bmpwin[i] );
    }
  }

  GFL_MSG_Delete(msgdata_explain);
  GFL_MSG_Delete(msgdata_weight);
  GFL_MSG_Delete(msgdata_height);
  GFL_MSG_Delete(msgdata_kind);
  GFL_MSG_Delete(msgdata_common);
}

static void Zukan_Info_CreateForeignMessage( ZUKAN_INFO_WORK* work, ZUKAN_INFO_LANG lang )
{
  GFL_MSGDATA* msgdata_common;
  GFL_MSGDATA* msgdata_kind;
  GFL_MSGDATA* msgdata_height;
  GFL_MSGDATA* msgdata_weight;
  GFL_MSGDATA* msgdata_explain;

  PALTYPE paltype = (work->disp==ZUKAN_INFO_DISP_M)?(PALTYPE_MAIN_BG):(PALTYPE_SUB_BG);

  if( lang == ZUKAN_INFO_LANG_NONE || work->monsno > FOREIGN_MONSNO_MAX )
  {
    Zukan_Info_CreateMessage( work );
    return;
  }

  msgdata_common = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_dat, work->heap_id );
  msgdata_kind = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_type_dat, work->heap_id );
  msgdata_height = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_height_dat, work->heap_id );
  msgdata_weight = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_weight_dat, work->heap_id );
  msgdata_explain = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, narc_msg_explain[lang], work->heap_id );

  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, paltype,
                                 ZUKAN_INFO_BG_PAL_POS_MSG * ZUKAN_INFO_PAL_LINE_SIZE,
                                 ZUKAN_INFO_BG_PAL_NUM_MSG * ZUKAN_INFO_PAL_LINE_SIZE,
                                 work->heap_id );

  {
    s32 i;

    // �L�����N�^�[�P�ʂ�X���W�AY���W�AX�T�C�Y�AY�T�C�Y���w�肷��
    const u8 pos_siz[ZUKAN_INFO_MSG_MAX][4] =
    {
      {  2,  0, 28,  3 },
      { 16,  4, 15,  5 },
      { 18, 12, 13,  5 },
      {  2, 17, 28,  7 },
    };

    for( i=0; i<ZUKAN_INFO_MSG_MAX; i++ )
    {
      // bmpwin�̃r�b�g�}�b�v�𓧖��F�œh��ׂ��Ă������ق��������H��GFL_BMP_Clear���邱�Ƃɂ���
      work->bmpwin[i] = GFL_BMPWIN_Create( work->msg_bg_frame,
                                           pos_siz[i][0], pos_siz[i][1], pos_siz[i][2], pos_siz[i][3],
                                           ZUKAN_INFO_BG_PAL_POS_MSG, GFL_BMP_CHRAREA_GET_B );
      // �N���A
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->bmpwin[i] ), 0 );
    }

    if( work->launch == ZUKAN_INFO_LAUNCH_LIST && work->disp == ZUKAN_INFO_DISP_S )
    {
      // �|�P����������@�Ƃ��낭�@�����傤�I
      // �̏ꏊ�ɉ��������Ȃ��悤�ɂ���
    }
    else
    {
      // �|�P����������@�Ƃ��낭�@�����傤�I
      Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_TOROKU], msgdata_common,
                          work->print_que, work->font,
                          ZKN_POKEGET_00, 0, 5, PRINTSYS_LSB_Make(0xF,2,0), ZUKAN_INFO_ALIGN_CENTER, NULL );
    }

    // 025(�|�P�����̔ԍ�)
    {
      WORDSET* wordset = WORDSET_Create( work->heap_id );
      WORDSET_RegisterNumber( wordset, 0, work->monsno, 3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
      Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_NAME], msgdata_common,
                          work->print_que, work->font,
                          ZNK_POKELIST_17, 8, 7, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, wordset );
      WORDSET_Delete( wordset );
    }
    
    // ��F�s�J�`���E
    {
      STRBUF* strbuf;
      strbuf = GFL_MSG_CreateString( GlobalMsg_PokeName, work->monsno );
      PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp( work->bmpwin[ZUKAN_INFO_MSG_NAME] ),
                              48, 7, strbuf, work->font, PRINTSYS_LSB_Make(1,2,0) );  
      GFL_STR_DeleteBuffer( strbuf );
    }

    // ��F�˂��݃|�P����
    Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_NAME], msgdata_kind, work->print_que, work->font,
                        ZKN_TYPE_000 + work->monsno, 16, 23, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );
    
    // ������
    Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_PHYSICAL], msgdata_common, work->print_que, work->font,
                        ZNK_POKELIST_19, 0, 4, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );      
    
    // ������
    Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_PHYSICAL], msgdata_common, work->print_que, work->font,
                        ZNK_POKELIST_20, 0, 20, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );      
    
    // �H�H�H.�Hm  // ����w��Ȃ�(40, 4)  // m��kg�ŏ����_�������悤��
    Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_PHYSICAL], msgdata_height, work->print_que, work->font,
                        ZKN_HEIGHT_000_000 + work->monsno, 94, 4, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_RIGHT, NULL );
    
    // �H�H�H.�Hkg  // ����w��Ȃ�(40,20)
    Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_PHYSICAL], msgdata_weight, work->print_que, work->font,
                        ZKN_WEIGHT_000_000 + work->monsno, 100, 20, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_RIGHT, NULL );
    
    // ��F�������@���݂̂��@�ł񂰂���
    Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_EXPLAIN], msgdata_explain, work->print_que, work->font,
                        ZKN_COMMENT_E_00_000 + work->monsno, 4, 5, PRINTSYS_LSB_Make(0xF,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );      
    
    for( i=0; i<ZUKAN_INFO_MSG_MAX; i++ )
    {
      GFL_BMPWIN_MakeTransWindow_VBlank( work->bmpwin[i] );
    }
  }

  GFL_MSG_Delete(msgdata_explain);
  GFL_MSG_Delete(msgdata_weight);
  GFL_MSG_Delete(msgdata_height);
  GFL_MSG_Delete(msgdata_kind);
  GFL_MSG_Delete(msgdata_common);
}

//-------------------------------------
/// BG�ɏ�����������j������
//=====================================
static void Zukan_Info_DeleteMessage( ZUKAN_INFO_WORK* work )
{
  s32 i;
  for( i=0; i<ZUKAN_INFO_MSG_MAX; i++ )
  {
    GFL_BMPWIN_Delete( work->bmpwin[i] );
  }
}

//-------------------------------------
/// �����X�^�[�{�[���̃}�[�N
//=====================================
static void Zukan_Info_CreateBall( ZUKAN_INFO_WORK* work )
{
  GFL_BMP_DATA* bmp_data;
  bmp_data = GFL_BMP_LoadCharacter( ARCID_ZUKAN_GRA, NARC_zukan_gra_info_zukan_bgu_NCGR, 0, work->heap_id );
  
  // �����X�^�[�{�[���̃}�[�N����
  work->ball_yes_bmpwin = GFL_BMPWIN_Create( work->fore_bg_frame, 12, 4, 4, 3, 0x0, GFL_BMP_CHRAREA_GET_F );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_yes_bmpwin), 8*1, 8*0, 0, 0, 8*4, 8*3, 0x0 );

  // �����X�^�[�{�[���̃}�[�N�Ȃ�
  work->ball_no_bmpwin = GFL_BMPWIN_Create( work->fore_bg_frame, 12, 4, 4, 3, 0x0, GFL_BMP_CHRAREA_GET_F );
/*
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8*26, 8*0,   0,   0, 8*4, 8*1, 0x0 );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8*11, 8*1,   0, 8*1, 8*4, 8*1, 0x0 );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8*18, 8*1, 8*1, 8*2, 8*2, 8*1, 0x0 );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8* 1, 8*2, 8*0, 8*2, 8*1, 8*1, 0x0 );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8* 4, 8*2, 8*3, 8*2, 8*1, 8*1, 0x0 );
*/
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8*0x15, 8*0x01,   0,   0, 8*4, 8*1, 0x0 );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8*0x1F, 8*0x01,   0, 8*1, 8*1, 8*1, 0x0 );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8*0x05, 8*0x02, 8*1, 8*1, 8*3, 8*1, 0x0 );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8*0x10, 8*0x00, 8*0, 8*2, 8*1, 8*1, 0x0 );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8*0x0B, 8*0x02, 8*1, 8*2, 8*3, 8*1, 0x0 );

  GFL_BMP_Delete( bmp_data );
}
static void Zukan_Info_DeleteBall( ZUKAN_INFO_WORK* work )
{
  GFL_BMPWIN_Delete( work->ball_yes_bmpwin );
  GFL_BMPWIN_Delete( work->ball_no_bmpwin );
}
static void Zukan_Info_TransBall_VBlank( ZUKAN_INFO_WORK* work )
{
  // �����X�^�[�{�[���̃}�[�N��t����
  if( work->get_flag )
  {
    GFL_BMPWIN_MakeTransWindow( work->ball_yes_bmpwin );
  }
  else
  {
    GFL_BMPWIN_MakeTransWindow( work->ball_no_bmpwin );
  }
}

//-------------------------------------
/// �|�P����2D�̃��\�[�X��ǂݍ���
//=====================================
static void Zukan_Info_Poke2dLoadResourceObj( ZUKAN_INFO_WORK* work )
{
  POKEMON_PASO_PARAM*   ppp;
  ARCHANDLE*            handle;

  CLSYS_DRAW_TYPE draw_type = (work->disp==ZUKAN_INFO_DISP_M)?(CLSYS_DRAW_MAIN):(CLSYS_DRAW_SUB);

  // PPP�쐬
  ppp = (POKEMON_PASO_PARAM*)PP_Create( (u16)work->monsno, 0, 0, work->heap_id );

  // �n���h��
  handle = POKE2DGRA_OpenHandle( work->heap_id );
  // ���\�[�X�ǂ݂���
  work->ncg_poke2d = POKE2DGRA_OBJ_CGR_RegisterPPP( handle, ppp, POKEGRA_DIR_FRONT, draw_type, work->heap_id );
  work->ncl_poke2d = POKE2DGRA_OBJ_PLTT_RegisterPPP( handle, ppp, POKEGRA_DIR_FRONT ,draw_type,
                                                     ZUKAN_INFO_OBJ_PAL_POS_POKE2D * ZUKAN_INFO_PAL_LINE_SIZE,  work->heap_id );
  work->nce_poke2d = POKE2DGRA_OBJ_CELLANM_RegisterPPP( ppp, POKEGRA_DIR_FRONT, APP_COMMON_MAPPING_128K, draw_type, work->heap_id );
  GFL_ARC_CloseDataHandle( handle );

  // PP�j��
  GFL_HEAP_FreeMemory( ppp );
}

//-------------------------------------
/// �|�P����2D�̃��\�[�X�j��
//=====================================
static void Zukan_Info_Poke2dUnloadResourceObj( ZUKAN_INFO_WORK* work )
{
  //���\�[�X�j��
  GFL_CLGRP_PLTT_Release( work->ncl_poke2d );
  GFL_CLGRP_CGR_Release( work->ncg_poke2d );
  GFL_CLGRP_CELLANIM_Release( work->nce_poke2d );
}

//-------------------------------------
/// �|�P����2D��OBJ�𐶐�����
//=====================================
static void Zukan_Info_Poke2dCreateCLWK( ZUKAN_INFO_WORK* work, u16 pos_x, u16 pos_y )
{
  GFL_CLWK_DATA cldata;

  CLSYS_DEFREND_TYPE defrend_type = (work->disp==ZUKAN_INFO_DISP_M)?(CLSYS_DEFREND_MAIN):(CLSYS_DEFREND_SUB);

  GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
  cldata.pos_x = pos_x;
  cldata.pos_y = pos_y;
  work->clwk_poke2d = GFL_CLACT_WK_Create( work->clunit, 
                                           work->ncg_poke2d,
                                           work->ncl_poke2d,
                                           work->nce_poke2d,
                                           &cldata, 
                                           defrend_type, work->heap_id );
  
  GFL_CLACT_WK_SetSoftPri( work->clwk_poke2d, 0 );  // ��O > �|�P����2D > ���� > �����A�C�R�� > ��
  
  if( work->launch == ZUKAN_INFO_LAUNCH_LIST )
  {
    GFL_CLACT_WK_SetObjMode( work->clwk_poke2d, GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
  }
}

//-------------------------------------
/// �|�P����2D��OBJ��j������
//=====================================
static void Zukan_Info_Poke2dDeleteCLWK( ZUKAN_INFO_WORK* work )
{
  GFL_CLACT_WK_Remove( work->clwk_poke2d );
}

//-------------------------------------
/// �^�C�v�A�C�R��OBJ�𐶐�����
//=====================================
static void Zukan_Info_CreateTypeicon( ZUKAN_INFO_WORK* work, PokeType type1, PokeType type2 )
{
  s32 i;
  PokeType type[2];
  GFL_CLWK_DATA data[2] = { 0 };
  
  CLSYS_DRAW_TYPE draw_type = (work->disp==ZUKAN_INFO_DISP_M)?(CLSYS_DRAW_MAIN):(CLSYS_DRAW_SUB);
  CLSYS_DEFREND_TYPE defrend_type = (work->disp==ZUKAN_INFO_DISP_M)?(CLSYS_DEFREND_MAIN):(CLSYS_DEFREND_SUB);

  type[0] = type1;
  type[1] = type2;

  data[0].pos_x = 8*21;
  data[0].pos_y = 8*11 -4 + work->y_offset;
  data[1].pos_x = 8*26;
  data[1].pos_y = 8*11 -4 + work->y_offset;
  
  // ���\�[�X�ǂݍ���
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), work->heap_id );

    for( i=0; i<2; i++ )
    {
      work->typeicon_cg_idx[i] = GFL_CLGRP_REGISTER_FAILED;
      if( type[i] == POKETYPE_NULL )
      {
        continue;
      }
      work->typeicon_cg_idx[i] = GFL_CLGRP_CGR_Register( handle,
                                                         APP_COMMON_GetPokeTypeCharArcIdx(type[i]),
                                                         FALSE, draw_type, work->heap_id );
    }

    work->typeicon_cl_idx = GFL_CLGRP_PLTT_RegisterEx( handle,
                                                       APP_COMMON_GetPokeTypePltArcIdx(),
                                                       draw_type,
                                                       ZUKAN_INFO_OBJ_PAL_POS_TYPEICON * ZUKAN_INFO_PAL_LINE_SIZE,
                                                       0, ZUKAN_INFO_OBJ_PAL_NUM_TYPEICON, work->heap_id );

    work->typeicon_cean_idx = GFL_CLGRP_CELLANIM_Register( handle,
                                                           APP_COMMON_GetPokeTypeCellArcIdx( APP_COMMON_MAPPING_128K ),
                                                           APP_COMMON_GetPokeTypeAnimeArcIdx( APP_COMMON_MAPPING_128K ),
                                                           work->heap_id );
  
    GFL_ARC_CloseDataHandle( handle );
  }

  // CLWK�쐬
  {
    for( i=0; i<2; i++ )
    {
      if( work->typeicon_cg_idx[i] == GFL_CLGRP_REGISTER_FAILED )
      {
        continue;
      }
      work->typeicon_clwk[i] = GFL_CLACT_WK_Create( work->clunit,
                                                    work->typeicon_cg_idx[i],
                                                    work->typeicon_cl_idx,
                                                    work->typeicon_cean_idx,
                                                    &(data[i]),
                                                    defrend_type, work->heap_id );
      GFL_CLACT_WK_SetPlttOffs( work->typeicon_clwk[i], APP_COMMON_GetPokeTypePltOffset(type[i]),
                                CLWK_PLTTOFFS_MODE_PLTT_TOP );
      GFL_CLACT_WK_SetSoftPri( work->typeicon_clwk[i], 2 );  // ��O > �|�P����2D > ���� > �����A�C�R�� > ��
      GFL_CLACT_WK_SetObjMode( work->typeicon_clwk[i], GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
    }
  }
}

//-------------------------------------
/// �^�C�v�A�C�R��OBJ��j������
//=====================================
static void Zukan_Info_DeleteTypeicon( ZUKAN_INFO_WORK* work )
{
  s32 i=0;

  // CLWK�j��
  for( i=0; i<2; i++ )
  {
    if( work->typeicon_cg_idx[i] == GFL_CLGRP_REGISTER_FAILED )
    {
      continue;
    }
    GFL_CLACT_WK_Remove( work->typeicon_clwk[i] );
  }

  // ���\�[�X�j��
  GFL_CLGRP_PLTT_Release( work->typeicon_cl_idx );
  for( i=0; i<2; i++ )
  {
    if( work->typeicon_cg_idx[i] == GFL_CLGRP_REGISTER_FAILED )
    {
      continue;
    }
    GFL_CLGRP_CGR_Release( work->typeicon_cg_idx[i] );
  }
  GFL_CLGRP_CELLANIM_Release( work->typeicon_cean_idx );
}

//-------------------------------------
/// �|�P�����̑���OBJ�𐶐�����
//=====================================
static void Zukan_Info_CreatePokefoot( ZUKAN_INFO_WORK* work, u32 monsno )
{
  UI_EASY_CLWK_RES_PARAM prm;
  CLSYS_DRAW_TYPE draw_type = (work->disp==ZUKAN_INFO_DISP_M)?(CLSYS_DRAW_MAIN):(CLSYS_DRAW_SUB);

  if( monsno > MONSNO_ARUSEUSU ) monsno = 1;  // �J���������̏���

  prm.draw_type = draw_type;
  prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NCGR;  // NCLR�񈳏k�ANCGR���k�ANCER�񈳏k�ANANR�񈳏k
  prm.arc_id    = PokeFootArcFileGet();
  prm.pltt_id   = PokeFootPlttDataIdxGet();
  prm.ncg_id    = PokeFootCharDataIdxGet((int)monsno);
  prm.cell_id   = PokeFootCellDataIdxGet();
  prm.anm_id    = PokeFootCellAnmDataIdxGet();
  prm.pltt_line = ZUKAN_INFO_OBJ_PAL_POS_POKEFOOT;
  prm.pltt_src_ofs = 0;
  prm.pltt_src_num = ZUKAN_INFO_OBJ_PAL_NUM_POKEFOOT;
  
  UI_EASY_CLWK_LoadResource( &work->clres_pokefoot, &prm, work->clunit, work->heap_id );

  work->clwk_pokefoot = UI_EASY_CLWK_CreateCLWK( &work->clres_pokefoot, work->clunit, 8*15, (u8)( 8*11 + work->y_offset ), 0, work->heap_id );
  GFL_CLACT_WK_SetSoftPri( work->clwk_pokefoot, 1 );  // ��O > �|�P����2D > ���� > �����A�C�R�� > ��
  GFL_CLACT_WK_SetObjMode( work->clwk_pokefoot, GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
}

//-------------------------------------
/// �|�P�����̑���OBJ��j������
//=====================================
static void Zukan_Info_DeletePokefoot( ZUKAN_INFO_WORK* work )
{
  //CLWK�j��
  GFL_CLACT_WK_Remove( work->clwk_pokefoot );
  //���\�[�X�j��
  UI_EASY_CLWK_UnLoadResource( &work->clres_pokefoot );
}

//-------------------------------------
/// �|�P����2D�ȊO�̂��̂𐶐�����
//=====================================
static void Zukan_Info_CreateOthers( ZUKAN_INFO_WORK* work )
{
  // Message
  Zukan_Info_CreateMessage( work );

  // �^�C�v�A�C�R��
  {
    //PokeType type1 = (PokeType)( PP_Get( work->pp, ID_PARA_type1, NULL ) );
    //PokeType type2 = (PokeType)( PP_Get( work->pp, ID_PARA_type2, NULL ) );
    
    POKEMON_PERSONAL_DATA* ppd = POKE_PERSONAL_OpenHandle( work->monsno, work->formno, work->heap_id );
    PokeType type1 = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_type1 );
    PokeType type2 = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_type2 );
    POKE_PERSONAL_CloseHandle( ppd );

    if( type1 == type2 )
    {
      type2 = POKETYPE_NULL;
    }
    Zukan_Info_CreateTypeicon( work, type1, type2 );
  }

  // �|�P�����̑���
  {
    Zukan_Info_CreatePokefoot( work, work->monsno );
  }
}

//-------------------------------------
/// �p���b�g�A�j���[�V�����̍X�V
//=====================================
static void Zukan_Info_UpdatePaletteAnime( ZUKAN_INFO_WORK* work )
{
  // �A�j���[�V�����J�E���g��i�߂�
  if( work->anm_cnt + ZUKAN_INFO_PALETTE_ANIME_VALUE >= ZUKAN_INFO_PALETTE_ANIME_VALUE_MAX )
  {
    work->anm_cnt = work->anm_cnt + ZUKAN_INFO_PALETTE_ANIME_VALUE - ZUKAN_INFO_PALETTE_ANIME_VALUE_MAX;
  }
  else
  {
    work->anm_cnt += ZUKAN_INFO_PALETTE_ANIME_VALUE;
  }

  // �p���b�g�̐F���X�V����
  {
#define ZUKAN_INFO_GET_R(color) ( ( (color) & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT )
#define ZUKAN_INFO_GET_G(color) ( ( (color) & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT )
#define ZUKAN_INFO_GET_B(color) ( ( (color) & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT )

    // 0�`1�̒l�ɕϊ�
    const fx16 cos = ( FX_CosIdx(work->anm_cnt) + FX16_ONE ) / 2;
    u8 i;
    for( i=0; i<ZUKAN_INFO_PALETTE_ANIME_NUM; i++ )
    {
      const u8 rs = ZUKAN_INFO_GET_R( work->palette_anime_s[i] );
      const u8 gs = ZUKAN_INFO_GET_G( work->palette_anime_s[i] );
      const u8 bs = ZUKAN_INFO_GET_B( work->palette_anime_s[i] );
      const u8 re = ZUKAN_INFO_GET_R( work->palette_anime_e[i] );
      const u8 ge = ZUKAN_INFO_GET_G( work->palette_anime_e[i] );
      const u8 be = ZUKAN_INFO_GET_B( work->palette_anime_e[i] );
      const u8 r = rs + ( ((re-rs)*cos) >> FX16_SHIFT );
      const u8 g = gs + ( ((ge-gs)*cos) >> FX16_SHIFT );
      const u8 b = bs + ( ((be-bs)*cos) >> FX16_SHIFT );
      work->trans_buf[i] = GX_RGB(r, g, b);
    }

    {
      NNS_GFD_DST_TYPE dst_type =(work->disp==ZUKAN_INFO_DISP_M)?(NNS_GFD_DST_2D_BG_PLTT_MAIN):(NNS_GFD_DST_2D_BG_PLTT_SUB); 
      NNS_GfdRegisterNewVramTransferTask( dst_type,
          ZUKAN_INFO_PALETTE_ANIME_LINE * ZUKAN_INFO_PAL_LINE_SIZE + ZUKAN_INFO_PALETTE_ANIME_NO * ZUKAN_INFO_COLOR_SIZE,
          work->trans_buf, ZUKAN_INFO_COLOR_SIZE * ZUKAN_INFO_PALETTE_ANIME_NUM );
    }

#undef ZUKAN_INFO_GET_R
#undef ZUKAN_INFO_GET_G
#undef ZUKAN_INFO_GET_B
  }
}

//-------------------------------------
/// �SOBJ�̕\��/��\����ݒ肷��
//=====================================
void ZUKAN_INFO_SetDrawEnableAllObj( ZUKAN_INFO_WORK* work, BOOL on_off )
{
  u8 i;

  // �|�P����2D
  GFL_CLACT_WK_SetDrawEnable( work->clwk_poke2d, on_off );
  
  // �^�C�v�A�C�R��
  GFL_CLACT_WK_SetDrawEnable( work->clwk_pokefoot, on_off );
 
  // �|�P�����̑���
  for( i=0; i<2; i++ )
  {
    if( work->typeicon_cg_idx[i] == GFL_CLGRP_REGISTER_FAILED )
    {
      continue;
    }
    GFL_CLACT_WK_SetDrawEnable( work->typeicon_clwk[i], on_off );
  }
}
