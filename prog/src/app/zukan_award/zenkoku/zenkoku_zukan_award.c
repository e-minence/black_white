//============================================================================
/**
 *  @file   zenkoku_zukan_award.c
 *  @brief  �n���}�ӕ\����
 *  @author Koji Kawada
 *  @data   2010.03.08
 *  @note   
 *  ���W���[�����FZENKOKU_ZUKAN_AWARD
 */
//============================================================================
//#define DEBUG_TEXT_MOVE  // ���ꂪ��`����Ă���Ƃ��A�e�L�X�g�𓮂�����
//#define USE_OBJ_GF  // ���ꂪ��`����Ă���Ƃ��AOBJ_GF���g��


#define HEAPID_ZENKOKU_ZUKAN_AWARD (HEAPID_AWARD)


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "gamesystem/gamedata_def.h"
#include "gamesystem/game_data.h"
#include "savedata/mystatus.h"
#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "app/app_nogear_subscreen.h"

#include "sound/wb_sound_data.sadl"
#include "sound/pm_sndsys.h"

#include "zenkoku_zukan_award_graphic.h"
#include "app/zenkoku_zukan_award.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "script_message.naix"
#include "msg/script/msg_award.h"
#include "shoujou.naix"

// �T�E���h

// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
#define HEAP_SIZE              (0x30000)               ///< �q�[�v�T�C�Y

// ���C��BG�t���[��
#define BG_FRAME_M_FRONT       (GFL_BG_FRAME1_M)
#define BG_FRAME_M_TEXT        (GFL_BG_FRAME2_M)

// ���C��BG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_M_FRONT   (1)
#define BG_FRAME_PRI_M_TEXT    (0)

// ���C��BG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_M_GRA_FRONT     = 15,
  BG_PAL_NUM_M_TEXT          = 1,
};
// �ʒu
enum
{
  BG_PAL_POS_M_GRA_FRONT    =  0,
  BG_PAL_POS_M_TEXT         = 15,
};

// ���C��OBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_M_BALL        = 2,
  OBJ_PAL_NUM_M_GF          = 1,
};
// �ʒu
enum
{
  OBJ_PAL_POS_M_BALL        = 0,
  OBJ_PAL_POS_M_GF          = 2,
};

// ProcMain�̃V�[�P���X
enum  // ���̕��я��𗘗p���Ă���Ƃ��낪����̂ŁA���я��͕ύX���Ȃ����ƁB
{
  SEQ_START          = 0,
  SEQ_FADE_IN,
  SEQ_SCROLL,      // work->param->b_fix��FALSE�̂Ƃ��������Ȃ�
  SEQ_SOUND_WAIT,  // work->param->b_fix��FALSE�̂Ƃ��������Ȃ�
  SEQ_MAIN,
  SEQ_FADE_OUT,
  SEQ_END,
};

// �e�L�X�g
enum
{
  TEXT_DUMMY,
  TEXT_BLACK,
  TEXT_NAME,
  TEXT_MAIN,
  TEXT_STAFF,
  TEXT_MAX,
};

// BG_PAL_POS_M_TEXT�̊��蓖��
#define TEXT_PAL_POS      (BG_PAL_POS_M_TEXT)
#define TEXT_COLOR_L      (1)  // ������F
#define TEXT_COLOR_S      (2)  // �����e�F
#define TEXT_COLOR_B      (0)  // �����w�i�F(����)
#define TEXT_COLOR_BLACK  (3)  // �X�N���[������ۂ̋󂫗̈�����ɂ��Ă���

// �����̐F
#define TEXT_COLOR_VALUE_L   (0x1D29)
#define TEXT_COLOR_VALUE_S   (0x3210)

static const u8 bmpwin_setup[TEXT_MAX][9] =  // MOJIMODE_HIRAGANA
{
  // frmnum           posx  posy  sizx  sizy  palnum                dir                    x  y (x,y�͖������ăZ���^�����O���邱�Ƃ�����)
  {  BG_FRAME_M_TEXT,    0,    0,    1,    1, TEXT_PAL_POS,         GFL_BMP_CHRAREA_GET_F, 0, 0 },
  {  BG_FRAME_M_TEXT,    0,   24,    1,    1, TEXT_PAL_POS,         GFL_BMP_CHRAREA_GET_F, 0, 0 },
  {  BG_FRAME_M_TEXT,    7,    4,   20,    3, TEXT_PAL_POS,         GFL_BMP_CHRAREA_GET_F, 0, 0 },  // �Z���^�����O�Ȃ� 6,    4,   20,    3,
  {  BG_FRAME_M_TEXT,    7,    8,   20,    9, TEXT_PAL_POS,         GFL_BMP_CHRAREA_GET_F, 0, 0 },  // �Z���^�����O�Ȃ� 6,    8,   20,    9,
  {  BG_FRAME_M_TEXT,    6,   17,   20,    5, TEXT_PAL_POS,         GFL_BMP_CHRAREA_GET_F, 0, 0 },
};

static const u8 bmpwin_setup_kanji[TEXT_MAX][4] =  // MOJIMODE_KANJI
{
  // posx  posy   x  y (x,y�͖������ăZ���^�����O���邱�Ƃ�����)
  {     0,    0,  0, 0 },
  {     0,   24,  0, 0 },
  {     7,    4,  0, 0 },  // �Z���^�����O�Ȃ�{     6,    4,  0, 0 },
  {     6,    8,  0, 0 },
  {     6,   17,  0, 0 },
};

// �t�F�[�h
#define FADE_IN_WAIT  (0)
#define FADE_OUT_WAIT (0)

// ������
#define TEXT_NAME_LEN_MAX    (32)  // EOM���܂߂�������

// �X�N���[��
#define SCROLL_START_POS_Y  (64)
#define SCROLL_WAIT         ( 0)  // 0�ȏ�̒l�B0�Ŗ��t���[���ړ��B1�ő҂��A�ړ��A�҂��A�ړ��B2�ő҂��A�҂��A�ړ��B
#define SCROLL_VALUE        ( 2)  // �ړ�����Ƃ��̈ړ��ʁB


// OBJ
enum
{
  OBJ_BALL_RES_NCG,
  OBJ_BALL_RES_NCL,
  OBJ_BALL_RES_NCE,
#ifdef USE_OBJ_GF
  OBJ_GF_RES_NCG,
  OBJ_GF_RES_NCL,
  OBJ_GF_RES_NCE,
#endif
  OBJ_RES_MAX,
};
// CELL
enum
{
  OBJ_BALL_CELL,
#ifdef USE_OBJ_GF
  OBJ_GF_CELL,
#endif
  OBJ_CELL_MAX,
};
#define OBJ_BALL_CELL_ANMSEQ  (0)
#ifdef USE_OBJ_GF
#define OBJ_GF_CELL_ANMSEQ    (0)
#endif
static const GFL_CLWK_DATA obj_cell_data[OBJ_CELL_MAX] =
{
  // pos_x, pos_y, anmseq,               softpri, bgpri
  {   18,    17,   OBJ_BALL_CELL_ANMSEQ, 0,       BG_FRAME_PRI_M_FRONT },
#ifdef USE_OBJ_GF
  {  175,   136,   OBJ_GF_CELL_ANMSEQ,   0,       BG_FRAME_PRI_M_FRONT },
#endif
};


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
  HEAPID                      heap_id;
  ZENKOKU_ZUKAN_AWARD_PARAM*   param;
  
  // �O���t�B�b�N�A�t�H���g�Ȃ�
  ZENKOKU_ZUKAN_AWARD_GRAPHIC_WORK*      graphic;
  GFL_FONT*                             font;
  PRINT_QUE*                            print_que;

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // �e�L�X�g
  GFL_MSGDATA*                msgdata;
  GFL_BMPWIN*                 text_bmpwin[TEXT_MAX];
  BOOL                        text_finish[TEXT_MAX];  // bmpwin�̓]�����ς�ł����TRUE

  // �X�N���[��
  u8                          scroll_wait_count;

  // OBJ
  u32                         obj_res[OBJ_RES_MAX];
  GFL_CLWK*                   obj_clwk[OBJ_CELL_MAX];
}
ZENKOKU_ZUKAN_AWARD_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Zenkoku_Zukan_Award_VBlankFunc( GFL_TCB* tcb, void* wk );

// BG
static void Zenkoku_Zukan_Award_BgInit( ZENKOKU_ZUKAN_AWARD_WORK* work );
static void Zenkoku_Zukan_Award_BgExit( ZENKOKU_ZUKAN_AWARD_WORK* work );

// �e�L�X�g
static void Zenkoku_Zukan_Award_TextInit( ZENKOKU_ZUKAN_AWARD_WORK* work );
static void Zenkoku_Zukan_Award_TextExit( ZENKOKU_ZUKAN_AWARD_WORK* work );
static void Zenkoku_Zukan_Award_TextMain( ZENKOKU_ZUKAN_AWARD_WORK* work );

// �����Ȃ��Ƃ���͍��ɂ��Ă���
static void Zenkoku_Zukan_Award_BlackInit( ZENKOKU_ZUKAN_AWARD_WORK* work );

// OBJ
static void Zenkoku_Zukan_Award_ObjInit( ZENKOKU_ZUKAN_AWARD_WORK* work );
static void Zenkoku_Zukan_Award_ObjExit( ZENKOKU_ZUKAN_AWARD_WORK* work );

// �X�N���[��
static void Zenkoku_Zukan_Award_ScrollInit( ZENKOKU_ZUKAN_AWARD_WORK* work );
static void Zenkoku_Zukan_Award_ScrollMain( ZENKOKU_ZUKAN_AWARD_WORK* work );
static BOOL Zenkoku_Zukan_Award_ScrollIsEnd( ZENKOKU_ZUKAN_AWARD_WORK* work );


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static GFL_PROC_RESULT Zenkoku_Zukan_Award_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Zenkoku_Zukan_Award_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Zenkoku_Zukan_Award_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk );

const GFL_PROC_DATA    ZENKOKU_ZUKAN_AWARD_ProcData =
{
  Zenkoku_Zukan_Award_ProcInit,
  Zenkoku_Zukan_Award_ProcMain,
  Zenkoku_Zukan_Award_ProcExit,
};


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief           �p�����[�^����
 *
 *  @param[in]       heap_id       �q�[�vID
 *  @param[in]       gamedata      ������ԃf�[�^MYSTATUS�𓾂��肷�邽�߂ɕK�v(���O�Ɛ��ʂ��g�p)
 *  @param[in]       b_fix         �����Ă���̂�����Ƃ�TRUE
 *
 *  @retval          ZENKOKU_ZUKAN_AWARD_PARAM
 */
//------------------------------------------------------------------
ZENKOKU_ZUKAN_AWARD_PARAM*  ZENKOKU_ZUKAN_AWARD_AllocParam(
                                HEAPID               heap_id,
                                const GAMEDATA*      gamedata,
                                BOOL                 b_fix
                           )
{
  ZENKOKU_ZUKAN_AWARD_PARAM* param = GFL_HEAP_AllocMemory( heap_id, sizeof( ZENKOKU_ZUKAN_AWARD_PARAM ) );
  ZENKOKU_ZUKAN_AWARD_InitParam(
      param,
      gamedata,
      b_fix );
  return param;
}

//------------------------------------------------------------------
/**
 *  @brief           �p�����[�^���
 *
 *  @param[in,out]   param      �p�����[�^�����Ő�����������
 *
 *  @retval          
 */
//------------------------------------------------------------------
void            ZENKOKU_ZUKAN_AWARD_FreeParam(
                            ZENKOKU_ZUKAN_AWARD_PARAM*  param )
{
  GFL_HEAP_FreeMemory( param );
}

//------------------------------------------------------------------
/**
 *  @brief           �p�����[�^��ݒ肷��
 *
 *  @param[in,out]   param         ZENKOKU_ZUKAN_AWARD_PARAM
 *  @param[in]       gamedata      ������ԃf�[�^MYSTATUS�𓾂��肷�邽�߂ɕK�v(���O�Ɛ��ʂ��g�p)
 *  @param[in]       b_fix         �����Ă���̂�����Ƃ�TRUE
 *
 *  @retval          
 */
//------------------------------------------------------------------
void  ZENKOKU_ZUKAN_AWARD_InitParam(
                  ZENKOKU_ZUKAN_AWARD_PARAM*      param,
                  const GAMEDATA*                 gamedata,
                  BOOL                            b_fix
                         )
{
  param->gamedata          = gamedata;
  param->b_fix             = b_fix;
}


//=============================================================================
/**
*  ���[�J���֐���`(PROC)
*/
//=============================================================================
//-------------------------------------
/// PROC ����������
//=====================================
static GFL_PROC_RESULT Zenkoku_Zukan_Award_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  ZENKOKU_ZUKAN_AWARD_WORK*     work;

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ZENKOKU_ZUKAN_AWARD, HEAP_SIZE );
    work = GFL_PROC_AllocWork( proc, sizeof(ZENKOKU_ZUKAN_AWARD_WORK), HEAPID_ZENKOKU_ZUKAN_AWARD );
    GFL_STD_MemClear( work, sizeof(ZENKOKU_ZUKAN_AWARD_WORK) );
    
    work->heap_id       = HEAPID_ZENKOKU_ZUKAN_AWARD;
    work->param         = (ZENKOKU_ZUKAN_AWARD_PARAM*)pwk;
  }

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    work->graphic       = ZENKOKU_ZUKAN_AWARD_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, work->heap_id );
    work->font          = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, work->heap_id );
    work->print_que     = PRINTSYS_QUE_Create( work->heap_id );
  }

  // ���C��BG
  GFL_BG_SetPriority( BG_FRAME_M_FRONT,  BG_FRAME_PRI_M_FRONT );
  GFL_BG_SetPriority( BG_FRAME_M_TEXT,   BG_FRAME_PRI_M_TEXT  );

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Zenkoku_Zukan_Award_VBlankFunc, work, 1 );

  // ����
  Zenkoku_Zukan_Award_BgInit( work );
  Zenkoku_Zukan_Award_TextInit( work );
  Zenkoku_Zukan_Award_BlackInit( work );  // �����Ȃ��Ƃ���͍��ɂ��Ă���
  Zenkoku_Zukan_Award_ObjInit( work );

  // �T�uBG
  {
    const MYSTATUS*  mystatus  = GAMEDATA_GetMyStatus( (GAMEDATA*)(work->param->gamedata) );  // ������ƃC������const�͂���
    APP_NOGEAR_SUBSCREEN_Init();
    APP_NOGEAR_SUBSCREEN_Trans( work->heap_id, mystatus->sex );  // PM_MALE or PM_FEMALE  // include/pm_version.h
  }

  // �X�N���[��
  Zenkoku_Zukan_Award_ScrollInit( work );
 
  // �o�b�N�O���E���h�J���[
  //GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, 0x0000 );
  //GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, 0x0000 );
  // �o�b�N�O���E���h�J���[(���ߐF)�܂ŐF�Ƃ��Ďg���Ă���̂ŁA�ύX���Ă̓_���B

  // �t�F�[�h�C��(�������ɐ^����)
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 16, 0 );

  // �ʐM�A�C�R��
  GFL_NET_ReloadIconTopOrBottom( FALSE, work->heap_id );

  // �T�E���h
  if( !(work->param->b_fix) )
  {
    PMSND_FadeOutBGM( PMSND_FADE_FAST );
  }

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static GFL_PROC_RESULT Zenkoku_Zukan_Award_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  ZENKOKU_ZUKAN_AWARD_WORK*     work     = (ZENKOKU_ZUKAN_AWARD_WORK*)mywk;

  // �ʐM�A�C�R��
  // �I������Ƃ��͒ʐM�A�C�R���ɑ΂��ĉ������Ȃ�
  
  // �T�uBG
  APP_NOGEAR_SUBSCREEN_Exit();

  // �j��
  Zenkoku_Zukan_Award_ObjExit( work );
  Zenkoku_Zukan_Award_TextExit( work );
  Zenkoku_Zukan_Award_BgExit( work );

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    PRINTSYS_QUE_Clear( work->print_que );
    PRINTSYS_QUE_Delete( work->print_que );
    GFL_FONT_Delete( work->font );
    ZENKOKU_ZUKAN_AWARD_GRAPHIC_Exit( work->graphic );
  }

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_PROC_FreeWork( proc );
    GFL_HEAP_DeleteHeap( HEAPID_ZENKOKU_ZUKAN_AWARD );
  }

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �又��
//=====================================
static GFL_PROC_RESULT Zenkoku_Zukan_Award_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  ZENKOKU_ZUKAN_AWARD_WORK*     work     = (ZENKOKU_ZUKAN_AWARD_WORK*)mywk;

  switch(*seq)
  {
  case SEQ_START:
    {
      BOOL  b_next  = TRUE;
      // �T�E���h
      if( !(work->param->b_fix) )
      {
        if( PMSND_CheckFadeOnBGM() )
        {
          b_next = FALSE;
        }
        else
        {
          PMSND_PauseBGM( TRUE );
          PMSND_PushBGM();
          PMSND_PlayBGM( SEQ_ME_HYOUKA6 );
        }
      }
      if( b_next )
      {
        // �t�F�[�h�C��(����������)
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, FADE_IN_WAIT );
        *seq = SEQ_FADE_IN;
      }
    }
    break;
  case SEQ_FADE_IN:
    {
      if( !GFL_FADE_CheckFade() )
      {
        if( work->param->b_fix ) *seq = SEQ_MAIN;
        else                     *seq = SEQ_SCROLL;
      }
    }
    break;
  case SEQ_SCROLL:  // work->param->b_fix��FALSE�̂Ƃ��������Ȃ�
    {
      if( Zenkoku_Zukan_Award_ScrollIsEnd( work ) )
      {
        *seq = SEQ_SOUND_WAIT;
      }
    }
    break;
  case SEQ_SOUND_WAIT:  // work->param->b_fix��FALSE�̂Ƃ��������Ȃ�
    {
      // �T�E���h
      if( !PMSND_CheckPlayBGM() )
      {
        PMSND_PopBGM();
        PMSND_PauseBGM( FALSE );
        PMSND_FadeInBGM( PMSND_FADE_FAST );
        
        *seq = SEQ_MAIN;
      }
    }
    break;
  case SEQ_MAIN:
    {
      BOOL b_end = FALSE;
      u32 x, y;
      if( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) )
      {
        GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
        b_end = TRUE;
      }
      else if( GFL_UI_TP_GetPointTrg( &x, &y ) )
      {
        GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
        b_end = TRUE;
      }

      if( b_end )
      {
        *seq = SEQ_FADE_OUT;
        // �t�F�[�h�A�E�g(�����遨��)
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, FADE_OUT_WAIT );
      }
    }
    break;
  case SEQ_FADE_OUT:
    {
      if( !GFL_FADE_CheckFade() )
      {
        *seq = SEQ_END;
      }
    }
    break;
  case SEQ_END:
    {
      return GFL_PROC_RES_FINISH;
    }
    break;
  }


#ifdef DEBUG_TEXT_MOVE
  {
    static int target = 0;
#ifdef USE_OBJ_GF
    static const int target_max = 5;
#else
    static const int target_max = 4;
#endif

    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L )
    {
      target--;
      if( target < 0 ) target = target_max -1;
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R )
    {
      target++;
      if( target >= target_max ) target = 0;
    }
    else
    {
      if( target >= 3 )
      {
        int obj_cell_idx = target - 3;
        GFL_CLACTPOS pos;
        GFL_CLACT_WK_GetPos( work->obj_clwk[obj_cell_idx], &pos, CLSYS_DEFREND_MAIN );

        if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
        {
          if( obj_cell_idx == OBJ_BALL_CELL )    OS_Printf( "Monster Ball (%d, %d)\n", pos.x, pos.y );
#ifdef USE_OBJ_GF
          else if( obj_cell_idx == OBJ_GF_CELL ) OS_Printf( "Logo (%d, %d)\n", pos.x, pos.y );
#endif
        }
        else if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
        {
          pos.y--;
        }
        else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
        {
          pos.y++;
        }
        else if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
        {
          pos.x--;
        }
        else if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT )
        {
          pos.x++;
        }
        
        GFL_CLACT_WK_SetPos( work->obj_clwk[obj_cell_idx], &pos, CLSYS_DEFREND_MAIN );
      }
      else
      {
        GFL_BMPWIN* target_bmpwin = work->text_bmpwin[target +2];
        u8 x = GFL_BMPWIN_GetPosX( target_bmpwin );
        u8 y = GFL_BMPWIN_GetPosY( target_bmpwin );
        BOOL b_change = FALSE;
      
        if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
        {
          switch(target)
          {
          case 0:
            OS_Printf( "Player Name (%d, %d)\n", x, y );
            break;
          case 1:
            OS_Printf( "Main Text (%d, %d)\n", x, y );
            break;
          case 2:
            OS_Printf( "Game Freak (%d, %d)\n", x, y );
            break;
          }
        }
        else if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
        {
          y--;
          b_change = TRUE;
        }
        else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
        {
          y++;
          b_change = TRUE;
        }
        else if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
        {
          x--;
          b_change = TRUE;
        }
        else if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT )
        {
          x++;
          b_change = TRUE;
        }

        if( b_change )
        {
          u8 i;

          //GFL_BG_ClearScreen( BG_FRAME_M_TEXT );  // ����BG�t���[���̋󂢂Ă���Ƃ�������ɂ��Ă���̂ŁA�S�N���A�͂܂���
          GFL_BG_FillScreen( BG_FRAME_M_TEXT, 0, 0, 0, 32, 24, GFL_BG_SCRWRT_PALNL );
       
          GFL_BMPWIN_SetPosX( target_bmpwin, x );
          GFL_BMPWIN_SetPosY( target_bmpwin, y );
        
          for( i=0; i<TEXT_MAX; i++ )
          {
            GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[i] );
          }
        }
      }
    }
  }
#endif


  PRINTSYS_QUE_Main( work->print_que );

  // ���C��
  Zenkoku_Zukan_Award_TextMain( work );

  if( SEQ_FADE_IN<=(*seq) && (*seq)<=SEQ_FADE_OUT )
  {
    Zenkoku_Zukan_Award_ScrollMain( work );
  }

  // 2D�`��
  ZENKOKU_ZUKAN_AWARD_GRAPHIC_2D_Draw( work->graphic );

  // 3D�`��
  //ZENKOKU_ZUKAN_AWARD_GRAPHIC_3D_StartDraw( work->graphic );
  //ZENKOKU_ZUKAN_AWARD_GRAPHIC_3D_EndDraw( work->graphic );

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
static void Zenkoku_Zukan_Award_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  ZENKOKU_ZUKAN_AWARD_WORK* work = (ZENKOKU_ZUKAN_AWARD_WORK*)wk;
}

//-------------------------------------
/// BG
//=====================================
static void Zenkoku_Zukan_Award_BgInit( ZENKOKU_ZUKAN_AWARD_WORK* work )
{
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_SHOUJOU, work->heap_id );

  // FRONT
  GFL_ARCHDL_UTIL_TransVramPalette(
      handle,
      NARC_shoujou_wb_syoujyou_z_NCLR,
      PALTYPE_MAIN_BG,
      BG_PAL_POS_M_GRA_FRONT * 0x20,
      BG_PAL_NUM_M_GRA_FRONT * 0x20,
      work->heap_id );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
      handle,
      NARC_shoujou_wb_syoujyou_z_NCGR,
      BG_FRAME_M_FRONT,
			0,
      0,  // �S�]��
      FALSE,
      work->heap_id );

  GFL_ARCHDL_UTIL_TransVramScreen(
      handle,
      NARC_shoujou_wb_syoujyou_z_NSCR,
      BG_FRAME_M_FRONT,
      0,
      0,  // �S�]��
      FALSE,
      work->heap_id );

  GFL_ARC_CloseDataHandle( handle );

  GFL_BG_LoadScreenReq( BG_FRAME_M_FRONT );
}
static void Zenkoku_Zukan_Award_BgExit( ZENKOKU_ZUKAN_AWARD_WORK* work )
{
  // �������Ȃ�
}

//-------------------------------------
/// �e�L�X�g
//=====================================
static void Zenkoku_Zukan_Award_TextInit( ZENKOKU_ZUKAN_AWARD_WORK* work )
{
  u8 i;
  const MYSTATUS*     mystatus  = GAMEDATA_GetMyStatus( (GAMEDATA*)(work->param->gamedata) );  // ������ƃC������const�͂���
  SAVE_CONTROL_WORK*  sv        = GAMEDATA_GetSaveControlWork( (GAMEDATA*)(work->param->gamedata) );  // ������ƃC������const�͂���
  CONFIG*             cfg       = SaveData_GetConfig( sv );
  MOJIMODE            mojimode  = CONFIG_GetMojiMode( cfg );

  // NULL�A�[��������
  work->msgdata = NULL;
  for( i=0; i<TEXT_MAX; i++ )
  {
    work->text_bmpwin[i] = NULL;
    work->text_finish[i] = FALSE;
  }
  work->text_finish[TEXT_DUMMY] = TRUE;  // �_�~�[�͍ς�ł��邱�Ƃɂ��Ă���
  work->text_finish[TEXT_BLACK] = TRUE;  // �_�~�[�͍ς�ł��邱�Ƃɂ��Ă���

  // �p���b�g
  GFL_ARC_UTIL_TransVramPaletteEx(
      ARCID_FONT,
      NARC_font_default_nclr,
      PALTYPE_MAIN_BG,
      0,
      BG_PAL_POS_M_TEXT * 0x20,
      BG_PAL_NUM_M_TEXT * 0x20,
      work->heap_id );

/*
  {
    // �p���b�g�̍쐬���]��
    u16 num = 3;
    u16 siz = sizeof(u16) * num;
    u16 ofs = BG_PAL_POS_M_TEXT * 0x20 + 0x20 - 2 * num;
    u16* pal = GFL_HEAP_AllocClearMemory( work->heap_id, siz );
    pal[0x00] = 0x294b;  // 13  // ��
    pal[0x01] = 0x5694;  // 14  // �D
    pal[0x02] = 0x7fff;  // 15  // �� 
    GFL_BG_LoadPalette( BG_PAL_POS_M_TEXT, pal, siz, ofs );
    GFL_HEAP_FreeMemory( pal );
  }
*/

  // ���b�Z�[�W
  work->msgdata = GFL_MSG_Create(
                      GFL_MSG_LOAD_NORMAL,
                      ARCID_SCRIPT_MESSAGE,
                      NARC_script_message_award_dat,
                      work->heap_id );

  // �r�b�g�}�b�v�E�B���h�E
  for( i=0; i<TEXT_MAX; i++ )
  {
    work->text_bmpwin[i] = GFL_BMPWIN_Create(
                                     bmpwin_setup[i][0],
                                     bmpwin_setup[i][1], bmpwin_setup[i][2], bmpwin_setup[i][3], bmpwin_setup[i][4],
                                     bmpwin_setup[i][5], bmpwin_setup[i][6] );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[i]), 0 );
    GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[i] );
  }

  // MOJIMODE_KANJI
  if( mojimode == MOJIMODE_KANJI )
  {
    for( i=0; i<TEXT_MAX; i++ )
    {
      GFL_BMPWIN_SetPosX( work->text_bmpwin[i], bmpwin_setup_kanji[i][0] );
      GFL_BMPWIN_SetPosY( work->text_bmpwin[i], bmpwin_setup_kanji[i][1] );
    }
  }

  // �e�L�X�g
  // �v���C���[���O����
  {
    WORDSET* wordset       = WORDSET_Create( work->heap_id );
    STRBUF*  src_strbuf    = GFL_MSG_CreateString( work->msgdata, msg_award_05 );
    STRBUF*  strbuf        = GFL_STR_CreateBuffer( TEXT_NAME_LEN_MAX, work->heap_id );

    u16 str_width;
    u16 bmp_width;
    s16 str_x     = ( mojimode == MOJIMODE_KANJI )?(bmpwin_setup_kanji[TEXT_NAME][2]):(bmpwin_setup[TEXT_NAME][7]);
    s16 str_y     = ( mojimode == MOJIMODE_KANJI )?(bmpwin_setup_kanji[TEXT_NAME][3]):(bmpwin_setup[TEXT_NAME][8]);

    WORDSET_RegisterPlayerName( wordset, 0, mystatus );
    WORDSET_ExpandStr( wordset, strbuf, src_strbuf );

    str_width = (u16)( PRINTSYS_GetStrWidth( strbuf, work->font, 0 ) );
    bmp_width = GFL_BMP_GetSizeX( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_NAME]) );
    if( bmp_width >= str_width )
    {
      //str_x = (s16)( ( bmp_width - str_width ) / 2 );  // ���̈����Z���^�����O  // ���񂹂Ȃ̂ŃR�����g�A�E�g
    }

    PRINTSYS_PrintQueColor(
        work->print_que,
        GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_NAME]),
        str_x, str_y,
        strbuf,
        work->font,
        PRINTSYS_LSB_Make(TEXT_COLOR_L,TEXT_COLOR_S,TEXT_COLOR_B) );
    GFL_STR_DeleteBuffer( strbuf );
    GFL_STR_DeleteBuffer( src_strbuf );
    WORDSET_Delete( wordset ); 
  }

  // �����ɏؖ����܂�
  {
#ifdef DEBUG_TEXT_MOVE
    STRBUF*  strbuf       = GFL_MSG_CreateString( work->msgdata, ( GFL_UI_KEY_GetCont() & ( PAD_BUTTON_Y ) ) ? ( msg_award_03 ) : ( msg_award_07 ) );
#else
    STRBUF*  strbuf       = GFL_MSG_CreateString( work->msgdata, msg_award_07 );
#endif

    u16 str_width;
    u16 bmp_width;
    s16 str_x     = ( mojimode == MOJIMODE_KANJI )?(bmpwin_setup_kanji[TEXT_MAIN][2]):(bmpwin_setup[TEXT_MAIN][7]);
    s16 str_y     = ( mojimode == MOJIMODE_KANJI )?(bmpwin_setup_kanji[TEXT_MAIN][3]):(bmpwin_setup[TEXT_MAIN][8]);

    str_width = (u16)( PRINTSYS_GetStrWidth( strbuf, work->font, 0 ) );
    bmp_width = GFL_BMP_GetSizeX( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_MAIN]) );
    if( bmp_width >= str_width )
    {
      if( mojimode == MOJIMODE_KANJI )
      {
        str_x = (s16)( ( bmp_width - str_width ) / 2 );  // ���̈����Z���^�����O
      }
      else
      {
        //str_x = (s16)( ( bmp_width - str_width ) / 2 );  // ���̈����Z���^�����O  // ���񂹂Ȃ̂ŃR�����g�A�E�g
      }
    }
    
    PRINTSYS_PrintQueColor(
        work->print_que,
        GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_MAIN]),
        str_x, str_y,
        strbuf,
        work->font,
        PRINTSYS_LSB_Make(TEXT_COLOR_L,TEXT_COLOR_S,TEXT_COLOR_B) );
    GFL_STR_DeleteBuffer( strbuf );
  }

  // �Q�[���t���[�N�X�^�b�t�ꓯ
  {
    STRBUF*  strbuf       = GFL_MSG_CreateString( work->msgdata, msg_award_06 );

    u16 str_width;
    u16 bmp_width;
    s16 str_x     = ( mojimode == MOJIMODE_KANJI )?(bmpwin_setup_kanji[TEXT_STAFF][2]):(bmpwin_setup[TEXT_STAFF][7]);
    s16 str_y     = ( mojimode == MOJIMODE_KANJI )?(bmpwin_setup_kanji[TEXT_STAFF][3]):(bmpwin_setup[TEXT_STAFF][8]);

    str_width = (u16)( PRINTSYS_GetStrWidth( strbuf, work->font, 0 ) );
    bmp_width = GFL_BMP_GetSizeX( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_STAFF]) );
    if( bmp_width >= str_width )
    {
      //str_x = (s16)( bmp_width - str_width );  // ���̈����E��  // gmm�ŉE�񂹎w�肵�Ă���̂ŃR�����g�A�E�g
    }

    PRINTSYS_PrintQueColor(
        work->print_que,
        GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_STAFF]),
        str_x, str_y,
        strbuf,
        work->font,
        PRINTSYS_LSB_Make(TEXT_COLOR_L,TEXT_COLOR_S,TEXT_COLOR_B) );
    GFL_STR_DeleteBuffer( strbuf );
  }

  // �ς�ł��Ȃ���������Ȃ����A1�x�Ă�ł���
  for( i=TEXT_DUMMY +1; i<TEXT_MAX; i++ )
  {
    GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[i] );
  }

  // ���ɍς�ł��邩������Ȃ��̂ŁAMain��1�x�Ă�ł���
  Zenkoku_Zukan_Award_TextMain( work );
}
static void Zenkoku_Zukan_Award_TextExit( ZENKOKU_ZUKAN_AWARD_WORK* work )
{
  u8 i;

  for( i=0; i<TEXT_MAX; i++ )
  {
    if( work->text_bmpwin[i] ) GFL_BMPWIN_Delete( work->text_bmpwin[i] );
  }
  
  if( work->msgdata ) GFL_MSG_Delete( work->msgdata );
}
static void Zenkoku_Zukan_Award_TextMain( ZENKOKU_ZUKAN_AWARD_WORK* work )
{
  u8 i;

  for( i=0; i<TEXT_MAX; i++ )
  {
    if( !(work->text_finish[i]) )
    {
      if( !PRINTSYS_QUE_IsExistTarget( work->print_que, GFL_BMPWIN_GetBmp(work->text_bmpwin[i]) ) )
      {
        GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[i] );
        work->text_finish[i] = TRUE;
      }
    }
  }
}

//-------------------------------------
/// �����Ȃ��Ƃ���͍��ɂ��Ă���
//=====================================
static void Zenkoku_Zukan_Award_BlackInit( ZENKOKU_ZUKAN_AWARD_WORK* work )
{
  // �K��Zenkoku_Zukan_Award_TextInit���ς�ł���ĂԂ���

  // �p���b�g�̍쐬���]��
  {
    u16* pal = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(u16) * 0x10 );
    pal[0x00]                = 0x0000;              // ����
    pal[TEXT_COLOR_L]        = TEXT_COLOR_VALUE_L;  // ������F
    pal[TEXT_COLOR_S]        = TEXT_COLOR_VALUE_S;  // �����e�F
    pal[TEXT_COLOR_BLACK]    = 0x0000;              // ��
    GFL_BG_LoadPalette( BG_FRAME_M_TEXT, pal, 0x20, TEXT_PAL_POS*0x20 );
    GFL_HEAP_FreeMemory( pal );
  }

  // �L�����̓h��ׂ����]��
  {
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_BLACK]), TEXT_COLOR_BLACK );  // ��
	  GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[TEXT_BLACK] );
  }

  // �X�N���[���̍쐬���]��
  {
    u16* scr = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(u16) * 32*24 );  // 1��ʕ����X�N���[�����Ȃ��̂ŁA���ꂾ���Ƃ��Ă����Ώ\���̂͂�
    u8 i, j;
    u16 h = 0;
    u16 chr_num = GFL_BMPWIN_GetChrNum( work->text_bmpwin[TEXT_BLACK] );
    for(j=0; j<24; j++)       // TWL_ProgramingManual.pdf TWL�v���O���~���O�}�j���A��
    {                         // 6.2.3.2.2 �X�N���[���f�[�^�̃A�h���X�}�b�s���O
      for(i=0; i<32; i++)     // �X�N���[���T�C�Y��256�~512 �h�b�g�̂Ƃ�
      {                       // ���Q�l�ɂ����B
        u16 chara_name = chr_num;
        u16 flip_h     = 0;
        u16 flip_v     = 0;
        u16 pal        = TEXT_PAL_POS;
        scr[h] = ( pal << 12 ) | ( flip_v << 11 ) | ( flip_h << 10 ) | ( chara_name << 0 );
        h++;
      }
    }
    GFL_BG_WriteScreen( BG_FRAME_M_TEXT, scr, 0, 24, 32, 24 );  // ������256�h�b�g�Ȃ̂ŁA�܂�Ԃ��͂Ȃ��Ǝv��
    GFL_BG_LoadScreenReq( BG_FRAME_M_TEXT );
    GFL_HEAP_FreeMemory( scr );
  }
}

//-------------------------------------
/// OBJ
//=====================================
static void Zenkoku_Zukan_Award_ObjInit( ZENKOKU_ZUKAN_AWARD_WORK* work )
{
  // ���\�[�X�̓ǂݍ���
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_SHOUJOU, work->heap_id );

  // BALL
  work->obj_res[OBJ_BALL_RES_NCL] = GFL_CLGRP_PLTT_RegisterEx(
      handle,
      NARC_shoujou_shoujou_obj_NCLR,
      CLSYS_DRAW_MAIN,
      OBJ_PAL_POS_M_BALL * 0x20,
      0,
      OBJ_PAL_NUM_M_BALL,
      work->heap_id );
  work->obj_res[OBJ_BALL_RES_NCG] = GFL_CLGRP_CGR_Register(
      handle,
      NARC_shoujou_shoujou_obj_NCGR,
      FALSE,
      CLSYS_DRAW_MAIN,
      work->heap_id );
  work->obj_res[OBJ_BALL_RES_NCE] = GFL_CLGRP_CELLANIM_Register(
      handle,
      NARC_shoujou_shoujou_obj_NCER,
      NARC_shoujou_shoujou_obj_NANR,
      work->heap_id );

#ifdef USE_OBJ_GF
  // GF
  work->obj_res[OBJ_GF_RES_NCL] = GFL_CLGRP_PLTT_RegisterEx(
      handle,
      NARC_shoujou_shoujou_logo_NCLR,
      CLSYS_DRAW_MAIN,
      OBJ_PAL_POS_M_GF * 0x20,
      0,
      OBJ_PAL_NUM_M_GF,
      work->heap_id );
  work->obj_res[OBJ_GF_RES_NCG] = GFL_CLGRP_CGR_Register(
      handle,
      NARC_shoujou_shoujou_logo_NCGR,
      FALSE,
      CLSYS_DRAW_MAIN,
      work->heap_id );
  work->obj_res[OBJ_GF_RES_NCE] = GFL_CLGRP_CELLANIM_Register(
      handle,
      NARC_shoujou_shoujou_logo_NCER,
      NARC_shoujou_shoujou_logo_NANR,
      work->heap_id );
#endif

  GFL_ARC_CloseDataHandle( handle );

  // CLWK�쐬
  // BALL
  work->obj_clwk[OBJ_BALL_CELL] = GFL_CLACT_WK_Create(
      ZENKOKU_ZUKAN_AWARD_GRAPHIC_GetClunit( work->graphic ),
      work->obj_res[OBJ_BALL_RES_NCG],
      work->obj_res[OBJ_BALL_RES_NCL],
      work->obj_res[OBJ_BALL_RES_NCE],
      &obj_cell_data[OBJ_BALL_CELL],
      CLSYS_DEFREND_MAIN,
      work->heap_id );
  GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_BALL_CELL], TRUE );
  GFL_CLACT_WK_SetAutoAnmFlag( work->obj_clwk[OBJ_BALL_CELL], TRUE );

#ifdef USE_OBJ_GF
  // GF
  work->obj_clwk[OBJ_GF_CELL] = GFL_CLACT_WK_Create(
      ZENKOKU_ZUKAN_AWARD_GRAPHIC_GetClunit( work->graphic ),
      work->obj_res[OBJ_GF_RES_NCG],
      work->obj_res[OBJ_GF_RES_NCL],
      work->obj_res[OBJ_GF_RES_NCE],
      &obj_cell_data[OBJ_GF_CELL],
      CLSYS_DEFREND_MAIN,
      work->heap_id );
  GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_GF_CELL], FALSE );
  GFL_CLACT_WK_SetAutoAnmFlag( work->obj_clwk[OBJ_GF_CELL], TRUE );
#endif
}
static void Zenkoku_Zukan_Award_ObjExit( ZENKOKU_ZUKAN_AWARD_WORK* work )
{
  // CLWK�j��
  u8 i;
  for( i=0; i<OBJ_CELL_MAX; i++ )
  {
    GFL_CLACT_WK_Remove( work->obj_clwk[i] );
  }

  // ���\�[�X�j��
#ifdef USE_OBJ_GF
  // GF
  GFL_CLGRP_CELLANIM_Release( work->obj_res[OBJ_GF_RES_NCE] );
  GFL_CLGRP_CGR_Release( work->obj_res[OBJ_GF_RES_NCG] );
  GFL_CLGRP_PLTT_Release( work->obj_res[OBJ_GF_RES_NCL] );
#endif
  // BALL
  GFL_CLGRP_CELLANIM_Release( work->obj_res[OBJ_BALL_RES_NCE] );
  GFL_CLGRP_CGR_Release( work->obj_res[OBJ_BALL_RES_NCG] );
  GFL_CLGRP_PLTT_Release( work->obj_res[OBJ_BALL_RES_NCL] );
}

//-------------------------------------
/// �X�N���[��
//=====================================
static void Zenkoku_Zukan_Award_ScrollInit( ZENKOKU_ZUKAN_AWARD_WORK* work )
{
  if( work->param->b_fix ) return;

  GFL_BG_SetScroll( BG_FRAME_M_FRONT, GFL_BG_SCROLL_Y_SET, SCROLL_START_POS_Y );
  GFL_BG_SetScroll( BG_FRAME_M_TEXT, GFL_BG_SCROLL_Y_SET, SCROLL_START_POS_Y );
 
  {
    u8 i;
    for( i=0; i<OBJ_CELL_MAX; i++ )
    {
      GFL_CLACTPOS pos;
      pos.x = obj_cell_data[i].pos_x;
      pos.y = obj_cell_data[i].pos_y - SCROLL_START_POS_Y;
      GFL_CLACT_WK_SetPos( work->obj_clwk[i], &pos, CLSYS_DEFREND_MAIN );
    }
  }

  work->scroll_wait_count = SCROLL_WAIT;
}
static void Zenkoku_Zukan_Award_ScrollMain( ZENKOKU_ZUKAN_AWARD_WORK* work )
{
  int curr_value;

  if( work->param->b_fix ) return;

  curr_value = GFL_BG_GetScrollY( BG_FRAME_M_FRONT );
  if( curr_value != 0 )
  {
    if( work->scroll_wait_count == 0 )
    {
      int value;
      if( curr_value < SCROLL_VALUE ) value = curr_value;
      else                            value = SCROLL_VALUE;
      GFL_BG_SetScrollReq( BG_FRAME_M_FRONT, GFL_BG_SCROLL_Y_DEC, value );
      GFL_BG_SetScrollReq( BG_FRAME_M_TEXT, GFL_BG_SCROLL_Y_DEC, value );

      {
        u8 i;
        for( i=0; i<OBJ_CELL_MAX; i++ )
        {
          GFL_CLACTPOS pos;
          GFL_CLACT_WK_GetPos( work->obj_clwk[i], &pos, CLSYS_DEFREND_MAIN );
          pos.y += (s16)value;
          GFL_CLACT_WK_SetPos( work->obj_clwk[i], &pos, CLSYS_DEFREND_MAIN );
        }
      }

      work->scroll_wait_count = SCROLL_WAIT;
    }
    else
    {
      work->scroll_wait_count--;
    }
  }
}
static BOOL Zenkoku_Zukan_Award_ScrollIsEnd( ZENKOKU_ZUKAN_AWARD_WORK* work )
{
  BOOL ret = FALSE;

  if( work->param->b_fix ) return TRUE;

  {
    BOOL ret_bg = ( GFL_BG_GetScrollY( BG_FRAME_M_FRONT ) == 0 );
    BOOL ret_obj = TRUE;
    //GFL_CLACTPOS pos;  // BG�̃X�N���[���݂̂Ŕ��f���邱�Ƃɂ����̂ŁA�R�����g�A�E�g�B
    //GFL_CLACT_WK_GetPos( work->obj_clwk[OBJ_BALL_CELL], &pos, CLSYS_DEFREND_MAIN );
    //if( pos.y == 0 ) ret_obj = TRUE;

    ret = ( ret_bg && ret_obj );
  }
  return ret;
}

