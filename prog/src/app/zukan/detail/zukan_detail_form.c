//============================================================================
/**
 *  @file   zukan_detail_form.c
 *  @brief  �}�ӏڍ׉�ʂ̎p���
 *  @author Koji Kawada
 *  @data   2010.02.02
 *  @note   
 *  ���W���[�����FZUKAN_DETAIL_FORM
 */
//============================================================================
#define DEBUG_KAWADA


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/mcss.h"
#include "system/mcss_tool.h"
#include "ui/ui_easy_clwk.h"
#include "pokeicon/pokeicon.h"
#include "gamesystem/gamedata_def.h"
#include "savedata/zukan_savedata.h"
#include "poke_tool/poke_personal.h"
#include "system/bmp_winframe.h"
#include "print/printsys.h"
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/global_msg.h"
#include "sound/pm_sndsys.h"

#include "zukan_detail_def.h"
#include "zukan_detail_common.h"
#include "zukan_detail_graphic.h"
#include "zukan_detail_form.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_zkn.h"
#include "zukan_gra.naix"

// �T�E���h

// �I�[�o�[���C
FS_EXTERN_OVERLAY(ui_common);


/*
�\�����[��


1 �t�H�����Ⴂ���Ȃ��āA�I�X���X���Ȃ��Ƃ�

�~���E�̂�����
�~���E�̂�����  (���낿����)


2 �t�H�����Ⴂ���Ȃ��āA�I�X���X(�I�X�ƃ��X�ŃO���t�B�b�N��
  �Ⴂ�����낤�ƂȂ��낤�ƁA�I�X�ƃ��X��\������)������Ƃ�

�I�X�̂�����
�I�X�̂�����  (���낿����)
���X�̂�����
���X�̂�����  (���낿����)

���ӁI�F�I�X���X�ǂ��炩�Е��������Ȃ��Ƃ�������B
�Ⴆ�΁A�I�X�������Ȃ��Ƃ���

�I�X�̂�����
�I�X�̂�����  (���낿����)

�ƂȂ�B


3 �t�H�����Ⴂ������Ƃ�(�I�X���X�͂��낤�ƂȂ��낤�ƁA��������)

�m�[�}���t�H����
�m�[�}���t�H����  (���낿����)
�A�^�b�N�t�H����
�A�^�b�N�t�H����  (���낿����)
�f�B�t�F���X�t�H����
�f�B�t�F���X�t�H����  (���낿����)
�X�s�[�h�t�H����
�X�s�[�h�t�H����  (���낿����)


*/


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
// ���C��BG�t���[��
#define BG_FRAME_M_POKE          (GFL_BG_FRAME0_M)
#define BG_FRAME_M_TEXT          (GFL_BG_FRAME2_M)
#define BG_FRAME_M_REAR          (GFL_BG_FRAME3_M)
// ���C��BG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_M_POKE      (1)
#define BG_FRAME_PRI_M_TEXT      (2)
#define BG_FRAME_PRI_M_REAR      (3)

// ���C��BG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_M_TEXT              = 1,
  BG_PAL_NUM_M_REAR              = ZKNDTL_COMMON_REAR_BG_PAL_NUM,
};
// �ʒu
enum
{
  BG_PAL_POS_M_TEXT              = 0,
  BG_PAL_POS_M_REAR              = 1,
};

// ���C��OBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_M_             = 0,
};
// �ʒu
enum
{
  OBJ_PAL_POS_M_             = 0,
};


// �T�uBG�t���[��
#define BG_FRAME_S_TEXT           (GFL_BG_FRAME2_S)
#define BG_FRAME_S_PANEL          (GFL_BG_FRAME3_S)
#define BG_FRAME_S_REAR           (GFL_BG_FRAME0_S)
// �T�uBG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_S_TEXT       (1)
#define BG_FRAME_PRI_S_PANEL      (2)
#define BG_FRAME_PRI_S_REAR       (3)

// �T�uBG�p���b�g
enum
{
  BG_PAL_NUM_S_TEXT              = 1,
  BG_PAL_NUM_S_PANEL             = 1,
  BG_PAL_NUM_S_REAR              = ZKNDTL_COMMON_REAR_BG_PAL_NUM,
};
// �ʒu
enum
{
  BG_PAL_POS_S_TEXT              = 0,
  BG_PAL_POS_S_PANEL             = 1,
  BG_PAL_POS_S_REAR              = 2,
};

// �T�uOBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_S_POKEICON         = 3,
};
// �ʒu
enum
{
  OBJ_PAL_POS_S_POKEICON         = 0,
};


// ProcMain�̃V�[�P���X
enum
{
  SEQ_START      = 0,
  SEQ_PREPARE_0,
  SEQ_PREPARE,
  SEQ_FADE_IN,
  SEQ_MAIN,
  SEQ_POST,
  SEQ_FADE_OUT,
  SEQ_END,
};

// �I������
typedef enum
{
  END_CMD_NONE,
  END_CMD_INSIDE,
  END_CMD_OUTSIDE,
}
END_CMD;

// ���
typedef enum
{
  STATE_TOP,
//  STATE_TOP_TO_EXCHANGE,
  STATE_EXCHANGE,
//  STATE_EXCHANGE_TO_TOP,
}
STATE;

// MCSS�|�P����
typedef enum
{
  POKE_CURR_F,    // ���̃t�H����(�O����)
  POKE_CURR_B,    // ���̃t�H����(�����)
  POKE_COMP_F,    // ��r�t�H����(�O����)
  POKE_COMP_B,    // ��r�t�H����(�����)
  POKE_MAX,
}
POKE_INDEX;

// �|�P�����̑傫��
#define POKE_SCALE    (16.0f)
#define POKE_SIZE_MAX (96.0f)

// �|�P�����̈ʒu
typedef enum
{
  POKE_CURR_POS_CENTER,  // ���̃t�H�����̃|�P�����̈ʒu
  POKE_CURR_POS_LEFT, 
  POKE_COMP_RPOS,        // ��r�t�H�����̃|�P�����̈ʒu(POKE_CURR_POS_???����Ƃ������Έʒu)
  POKE_POS_MAX,
}
POKE_POS_INDEX;

static VecFx32 poke_pos[POKE_POS_MAX] =
{
  { FX_F32_TO_FX32(  0.0f), FX_F32_TO_FX32(-24.0f), FX_F32_TO_FX32(0.0f) },
  { FX_F32_TO_FX32(-64.0f), FX_F32_TO_FX32(-24.0f), FX_F32_TO_FX32(0.0f) },
  { FX_F32_TO_FX32(128.0f), FX_F32_TO_FX32(  0.0f), FX_F32_TO_FX32(0.0f) },
};

// �|�P�A�C�R���̈ʒu
static const u8 pokeicon_pos[2] = { 128, 128 };

// �Ⴄ�p
typedef enum
{
  LOOKS_DIFF_ONE,   // ���ʂȂ��A�t�H�����Ȃ�
  LOOKS_DIFF_SEX,   // ���ʂ���A�t�H�����Ȃ�
  LOOKS_DIFF_FORM,  // ���ʕs��A�t�H��������
}
LOOKS_DIFF;

typedef enum
{
  COLOR_DIFF_NONE,     // �F�Ⴂ�Ȃ�
  COLOR_DIFF_NORMAL,   // �F�Ⴂ����Œʏ�F
  COLOR_DIFF_SPECIAL,  // �F�Ⴂ����œ��ʐF
}
COLOR_DIFF;

enum
{
  SEX_MALE,
  SEX_FEMALE,
};

// �Ⴄ�p�̐�
#define DIFF_MAX (64)  // ����ő���邩�H

// �e�L�X�g
enum
{
  TEXT_POKENAME,
  TEXT_UP_LABEL,
  TEXT_UP_NUM,
  TEXT_POKE_CURR,
  TEXT_POKE_COMP,
  TEXT_MAX,
};
// ������
#define STRBUF_NUM_LENGTH       (  8 )  // ??  // ��F�������p�̐�  99  �����̗��99�̕�����
#define STRBUF_POKENAME_LENGTH  ( 64 )  // [�|�P�����푰��]�̂�����  // buflen.h�Ŋm�F����K�v����I

#define TEXT_POKE_POS_Y_1_LINE   (  9 )
#define TEXT_POKE_POS_Y_2_LINE_0 (  1 )
#define TEXT_POKE_POS_Y_2_LINE_1 ( 17 )


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
// �Ⴄ�p
typedef struct
{
  LOOKS_DIFF looks_diff;
  COLOR_DIFF color_diff;
  u16        other_diff;  // LOOKS_DIFF_ONE�̂Ƃ��A�g�p���Ȃ�
                          // LOOKS_DIFF_SEX�̂Ƃ��ASEX_MALE or SEX_FEMALE
                          // LOOKS_DIFF_FORM�̂Ƃ��Aformno
}
DIFF_INFO;


//-------------------------------------
/// PROC ���[�N
//=====================================
typedef struct
{
  // ���̂Ƃ��납��ؗp�������
  GFL_CLUNIT*                 clunit;
  GFL_FONT*                   font;
  PRINT_QUE*                  print_que;

  // �����ō쐬�������
  MCSS_SYS_WORK*              mcss_sys_wk;
  MCSS_WORK*                  poke_wk[POKE_MAX];  // NULL�̂Ƃ��Ȃ�

  UI_EASY_CLWK_RES            pokeicon_res;   // pokeicon_clwk��NULL�̂Ƃ��A�g�p���Ă��Ȃ�
  GFL_CLWK*                   pokeicon_clwk;  // NULL�̂Ƃ��Ȃ�

  DIFF_INFO                   diff_info_list[DIFF_MAX];  // �����ɂ͍��̃t�H�������܂�ł���
  u16                         diff_num;  // �ő�DIFF_MAX
  u16                         diff_sugata_num;  // diff_num = diff_sugata_num + diff_irochigai_num
  u16                         diff_irochigai_num;
  u16                         diff_curr_no;  // diff_info_list[diff_curr_no]�����̃t�H����
  u16                         diff_comp_no;  // diff_info_list[diff_comp_no]����r�t�H����

  GFL_BMPWIN*                 text_bmpwin[TEXT_MAX];
  GFL_MSGDATA*                text_msgdata;

  GFL_ARCUTIL_TRANSINFO       panel_s_tinfo;

  ZKNDTL_COMMON_REAR_WORK*    rear_wk_m;
  ZKNDTL_COMMON_REAR_WORK*    rear_wk_s;

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // �t�F�[�h
  ZKNDTL_COMMON_FADE_WORK*    fade_wk_m;
  ZKNDTL_COMMON_FADE_WORK*    fade_wk_s;

  // �I������
  END_CMD                     end_cmd;
  
  // ���
  STATE                       state;
}
ZUKAN_DETAIL_FORM_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Zukan_Detail_Form_VBlankFunc( GFL_TCB* tcb, void* wk );

// �e�L�X�g
static void Zukan_Detail_Form_CreateTextBase( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_DeleteTextBase( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_WriteUpText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_WritePokeText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                GFL_BMPWIN* bmpwin, u16 diff_no );
static void Zukan_Detail_Form_WritePokeCurrText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_WritePokeCompText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_ScrollPokeText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// MCSS�|�P����
static void Zukan_Detail_Form_McssSysInit( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_McssSysExit( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static MCSS_WORK* PokeInit( MCSS_SYS_WORK* mcss_sys_wk,
                      int mons_no, int form_no, int sex, int rare, BOOL egg, int dir );
static void PokeExit( MCSS_SYS_WORK* mcss_sys_wk, MCSS_WORK* poke_wk );
// �}�N��
#define BLOCK_POKE_EXIT(sys,wk)                     \
    {                                               \
      if( wk ) PokeExit( sys, wk );                 \
      wk = NULL;                                    \
    }
// NULL�������Y��Ȃ��悤�Ƀ}�N�����g���悤�ɂ��Ă���
static void PokeAdjustOfsPos( MCSS_WORK* poke_wk );
static void PokeGetCompareRelativePosition( MCSS_WORK* poke_wk, VecFx32* pos );

// �|�P�A�C�R��
static GFL_CLWK* PokeiconInit( UI_EASY_CLWK_RES* res, GFL_CLUNIT* clunit, HEAPID heap_id, u32 mons, u32 form_no, BOOL egg );
static void PokeiconExit( UI_EASY_CLWK_RES* res, GFL_CLWK* clwk );
// �}�N��
#define BLOCK_POKEICON_EXIT(res,clwk)                     \
    {                                                     \
      if( clwk ) PokeiconExit( res, clwk );               \
      clwk = NULL;                                        \
    }
// NULL�������Y��Ȃ��悤�Ƀ}�N�����g���悤�ɂ��Ă���

// �Ⴄ�p�����擾����
static void Zukan_Detail_Form_GetDiffInfo( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �Ⴄ�p��񂩂�MCSS�|�P�����𐶐�����
static void Zukan_Detail_Form_PokeInitFromDiffInfo( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                POKE_INDEX poke_f, POKE_INDEX poke_b, POKE_POS_INDEX pos, u16 diff_no );
// �Ⴄ�p��񂩂�|�P�A�C�R���𐶐�����
static void Zukan_Detail_Form_PokeiconInitFromDiffInfo( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                u16 diff_no );
// �Ⴄ�p��񂩂�|�P�����̎p�̐���STRBUF�𓾂�
static void Zukan_Detail_Form_GetPokeTextStrbufFromDiffInfo( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                STRBUF** a_strbuf0, STRBUF** a_strbuf1, u16 diff_no );

// �|�P�����ύX
static void Zukan_Detail_Form_ChangePoke( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
// ��r�t�H�����ύX
static void Zukan_Detail_Form_ChangeCompareForm( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �|�P�A�C�R���ύX
static void Zukan_Detail_Form_ChangePokeicon( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// ����
static void Zukan_Detail_Form_Input( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �X�N���[���o�[���ړ�������
static void Zukan_Detail_Form_MoveScrollBar( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// ��Ԃ�J�ڂ�����
static void Zukan_Detail_Form_ChangeState( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                STATE state );


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Form_ProcInit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );
static ZKNDTL_PROC_RESULT Zukan_Detail_Form_ProcExit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );
static ZKNDTL_PROC_RESULT Zukan_Detail_Form_ProcMain( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_CommandFunc( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn, ZKNDTL_COMMAND cmd );
static void Zukan_Detail_Form_Draw3DFunc( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );

const ZKNDTL_PROC_DATA    ZUKAN_DETAIL_FORM_ProcData =
{
  Zukan_Detail_Form_ProcInit,
  Zukan_Detail_Form_ProcMain,
  Zukan_Detail_Form_ProcExit,
  Zukan_Detail_Form_CommandFunc,
  Zukan_Detail_Form_Draw3DFunc,
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
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void             ZUKAN_DETAIL_FORM_InitParam(
                            ZUKAN_DETAIL_FORM_PARAM*  param,
                            HEAPID                    heap_id )
{
  param->heap_id = heap_id;
}


//=============================================================================
/**
*  ���[�J���֐���`(PROC)
*/
//=============================================================================
//-------------------------------------
/// PROC ����������
//=====================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Form_ProcInit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_FORM_PARAM*    param    = (ZUKAN_DETAIL_FORM_PARAM*)pwk;
  ZUKAN_DETAIL_FORM_WORK*     work;

#ifdef DEBUG_KAWADA
  {
    OS_Printf( "Zukan_Detail_Form_ProcInit\n" );
  }
#endif

  // �I�[�o�[���C
  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );

  // �q�[�v
  {
    work = ZKNDTL_PROC_AllocWork(proc, sizeof(ZUKAN_DETAIL_FORM_WORK), param->heap_id);
    GFL_STD_MemClear( work, sizeof(ZUKAN_DETAIL_FORM_WORK) ); 
  }

  // ���̂Ƃ��납��ؗp�������
  {
    ZUKAN_DETAIL_GRAPHIC_WORK* graphic = ZKNDTL_COMMON_GetGraphic(cmn);
    work->clunit      = ZUKAN_DETAIL_GRAPHIC_GetClunit(graphic);
    work->font        = ZKNDTL_COMMON_GetFont(cmn);
    work->print_que   = ZKNDTL_COMMON_GetPrintQue(cmn);
  }

  // NULL������
  {
    u8 i;
    for( i=0; i<POKE_MAX; i++ )
    {
      work->poke_wk[i] = NULL;
    }

    work->pokeicon_clwk = NULL;
  }

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Zukan_Detail_Form_VBlankFunc, work, 1 );

  // �t�F�[�h
  {
    work->fade_wk_m = ZKNDTL_COMMON_FadeInit( param->heap_id );
    work->fade_wk_s = ZKNDTL_COMMON_FadeInit( param->heap_id );
   
    ZKNDTL_COMMON_FadeSetPlaneDefault( work->fade_wk_m );
    ZKNDTL_COMMON_FadeSetPlaneDefault( work->fade_wk_s );

    ZKNDTL_COMMON_FadeSetBlackImmediately( ZKNDTL_COMMON_FADE_DISP_M, work->fade_wk_m );
    ZKNDTL_COMMON_FadeSetBlackImmediately( ZKNDTL_COMMON_FADE_DISP_S, work->fade_wk_s );
  }

  // �I�����
  work->end_cmd = END_CMD_NONE;

  // ���
  work->state = STATE_TOP;

  return ZKNDTL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Form_ProcExit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_FORM_PARAM*    param    = (ZUKAN_DETAIL_FORM_PARAM*)pwk;
  ZUKAN_DETAIL_FORM_WORK*     work     = (ZUKAN_DETAIL_FORM_WORK*)mywk;

  // �Ŕw��
  ZKNDTL_COMMON_RearExit( work->rear_wk_s );
  ZKNDTL_COMMON_RearExit( work->rear_wk_m );

  // BG�p�l�� 
  ZKNDTL_COMMON_PanelDelete(
           BG_FRAME_S_PANEL,
           work->panel_s_tinfo );

  // �|�P�A�C�R��
  BLOCK_POKEICON_EXIT( &work->pokeicon_res, work->pokeicon_clwk )

  // �e�L�X�g
  Zukan_Detail_Form_DeleteTextBase( param, work, cmn );

  // �t�F�[�h
  {
    ZKNDTL_COMMON_FadeExit( work->fade_wk_s );
    ZKNDTL_COMMON_FadeExit( work->fade_wk_m );
  }

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �q�[�v
  ZKNDTL_PROC_FreeWork( proc );

  // �I�[�o�[���C
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );

#ifdef DEBUG_KAWADA
  {
    OS_Printf( "Zukan_Detail_Form_ProcExit\n" );
  }
#endif

  return ZKNDTL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �又��
//=====================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Form_ProcMain( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_FORM_PARAM*    param    = (ZUKAN_DETAIL_FORM_PARAM*)pwk;
  ZUKAN_DETAIL_FORM_WORK*     work     = (ZUKAN_DETAIL_FORM_WORK*)mywk;

  ZUKAN_DETAIL_GRAPHIC_WORK*   graphic  = ZKNDTL_COMMON_GetGraphic(cmn);

  ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
  ZUKAN_DETAIL_HEADBAR_WORK*   headbar  = ZKNDTL_COMMON_GetHeadbar(cmn);
 
  switch(*seq)
  {
  case SEQ_START:
    {
      *seq = SEQ_PREPARE_0;

      // �؂�ւ�
      {
        // �O���t�B�b�N�X���[�h�ݒ�
        GX_SetGraphicsMode( GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D );
      }

      // BG
      {
        // �N���A
        u8 i;
        for( i=GFL_BG_FRAME0_M; i<=GFL_BG_FRAME3_S; i++ )
        {
          if(    i != ZKNDTL_BG_FRAME_M_TOUCHBAR
              && i != ZKNDTL_BG_FRAME_S_HEADBAR )
          {
            GFL_BG_SetScroll( i, GFL_BG_SCROLL_X_SET, 0 );
            GFL_BG_SetScroll( i, GFL_BG_SCROLL_Y_SET, 0 );
            GFL_BG_ClearFrame(i);
          }
        }

        // ���C��BG
        GFL_BG_SetPriority( BG_FRAME_M_POKE,  BG_FRAME_PRI_M_POKE );
        GFL_BG_SetPriority( BG_FRAME_M_TEXT,  BG_FRAME_PRI_M_TEXT );
        GFL_BG_SetPriority( BG_FRAME_M_REAR,  BG_FRAME_PRI_M_REAR );
        
        // �T�uBG
        GFL_BG_SetPriority( BG_FRAME_S_TEXT,  BG_FRAME_PRI_S_TEXT  );
        GFL_BG_SetPriority( BG_FRAME_S_REAR,  BG_FRAME_PRI_S_REAR  );
        GFL_BG_SetPriority( BG_FRAME_S_PANEL, BG_FRAME_PRI_S_PANEL );
      }

      // 3D
      ZUKAN_DETAIL_GRAPHIC_Init3D( graphic, param->heap_id );

      // MCSS�|�P����
      Zukan_Detail_Form_McssSysInit( param, work, cmn );

      // �e�L�X�g
      Zukan_Detail_Form_CreateTextBase( param, work, cmn );

      // �Ⴄ�p�����擾����
      Zukan_Detail_Form_GetDiffInfo( param, work, cmn );
     
      // �ŏ��̃|�P������\������
      Zukan_Detail_Form_ChangePoke( param, work, cmn );

      // �|�P�������\���ɂ����ɂ���
      MCSS_SetVanishFlag( work->poke_wk[POKE_CURR_F] );
      MCSS_SetPaletteFade( work->poke_wk[POKE_CURR_F], 16, 16, 0, 0x0000 );
      if( work->poke_wk[POKE_COMP_F] )
      {
        MCSS_SetVanishFlag( work->poke_wk[POKE_COMP_F] );
        MCSS_SetPaletteFade( work->poke_wk[POKE_COMP_F], 16, 16, 0, 0x0000 );
      }

      // BG�p�l�� 
      work->panel_s_tinfo = ZKNDTL_COMMON_PanelCreate(
                              ZKNDTL_COMMON_PANEL_CREATE_TYPE_TRANS_PAL_CHARA,
                              param->heap_id,
                              BG_FRAME_S_PANEL,
                              BG_PAL_NUM_S_PANEL,
                              BG_PAL_POS_S_PANEL,
                              0,
                              ARCID_ZUKAN_GRA,
                              NARC_zukan_gra_info_info_bgu_NCLR,
                              NARC_zukan_gra_info_info_bgu_NCGR,
                              NARC_zukan_gra_info_formewin_bgu_NSCR,
                              0 );

      // �Ŕw��
      work->rear_wk_m = ZKNDTL_COMMON_RearInit( param->heap_id, ZKNDTL_COMMON_REAR_TYPE_FORM,
          BG_FRAME_M_REAR, BG_PAL_POS_M_REAR +0, BG_PAL_POS_M_REAR +1 );
      work->rear_wk_s = ZKNDTL_COMMON_RearInit( param->heap_id, ZKNDTL_COMMON_REAR_TYPE_FORM,
          BG_FRAME_S_REAR, BG_PAL_POS_S_REAR +0, BG_PAL_POS_S_REAR +1 );
    }
    break;
  case SEQ_PREPARE_0:
    {
      // MCSS�|�P����
      // �|�P���������ɂȂ�̂�҂��߂ɁA1�t���[���]�v�ɑ҂�
      *seq = SEQ_PREPARE;
    }
    break;
  case SEQ_PREPARE:
    {
      // MCSS�|�P����
      // �|�P���������ɂȂ�̂�҂�
      BOOL poke_fade = TRUE;
      if( MCSS_CheckExecutePaletteFade( work->poke_wk[POKE_CURR_F] ) )
      {
        poke_fade = FALSE;
      }
      if( work->poke_wk[POKE_COMP_F] )
      {
        if( MCSS_CheckExecutePaletteFade( work->poke_wk[POKE_COMP_F] ) )
        {
          poke_fade = FALSE;
        }
      }

      if( poke_fade )
      {
        *seq = SEQ_FADE_IN;

        // �t�F�[�h
        ZKNDTL_COMMON_FadeSetBlackToColorless( work->fade_wk_m );
        ZKNDTL_COMMON_FadeSetBlackToColorless( work->fade_wk_s );

        // �^�b�`�o�[
        if( ZUKAN_DETAIL_TOUCHBAR_GetState( touchbar ) != ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR )
        {
          ZUKAN_DETAIL_TOUCHBAR_SetType(
              touchbar,
              ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL,
              ZUKAN_DETAIL_TOUCHBAR_DISP_FORM );
          ZUKAN_DETAIL_TOUCHBAR_Appear( touchbar, ZUKAN_DETAIL_TOUCHBAR_SPEED_OUTSIDE );
        }
        // �^�C�g���o�[
        if( ZUKAN_DETAIL_HEADBAR_GetState( headbar ) != ZUKAN_DETAIL_HEADBAR_STATE_APPEAR )
        {
          ZUKAN_DETAIL_HEADBAR_SetType(
              headbar,
              ZUKAN_DETAIL_HEADBAR_TYPE_FORM );
          ZUKAN_DETAIL_HEADBAR_Appear( headbar );
        }

        // MCSS�|�P����
        // �|�P������\�����t�F�[�h�C��(�����J���[)
        MCSS_ResetVanishFlag( work->poke_wk[POKE_CURR_F] );
        MCSS_SetPaletteFade( work->poke_wk[POKE_CURR_F], 16, 0, ZKNDTL_COMMON_FADE_BRIGHT_WAIT, 0x0000 );
        if( work->poke_wk[POKE_COMP_F] )
        {
          MCSS_ResetVanishFlag( work->poke_wk[POKE_COMP_F] );
          MCSS_SetPaletteFade( work->poke_wk[POKE_COMP_F], 16, 0, ZKNDTL_COMMON_FADE_BRIGHT_WAIT, 0x0000 );
        }
      }
    }
    break;
  case SEQ_FADE_IN:
    {
      // MCSS�|�P����
      // �|�P�������J���[�ɂȂ�̂�҂�
      BOOL poke_fade = TRUE;
      if( MCSS_CheckExecutePaletteFade( work->poke_wk[POKE_CURR_F] ) )
      {
        poke_fade = FALSE;
      }
      if( work->poke_wk[POKE_COMP_F] )
      {
        if( MCSS_CheckExecutePaletteFade( work->poke_wk[POKE_COMP_F] ) )
        {
          poke_fade = FALSE;
        }
      }

      if(    (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_m ))
          && (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_s ))
          && ZUKAN_DETAIL_TOUCHBAR_GetState( touchbar ) == ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR
          && ZUKAN_DETAIL_HEADBAR_GetState( headbar ) == ZUKAN_DETAIL_HEADBAR_STATE_APPEAR
          && poke_fade )
      {
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );

        *seq = SEQ_MAIN;
      }
    }
    break;
  case SEQ_MAIN:
    {
      if( work->end_cmd != END_CMD_NONE )
      {
        *seq = SEQ_POST;

        // �|�P�������J���[�ɂ���
        {
          u8 i;
          for( i=0; i<POKE_MAX; i++ )
          {
            if( work->poke_wk[i] )
              MCSS_SetPaletteFade( work->poke_wk[i], 0, 0, 0, 0x0000 );
          }
        }
      }
      else
      {
        // ����
        Zukan_Detail_Form_Input( param, work, cmn );
      }
    }
    break;
  case SEQ_POST:
    {
      // MCSS�|�P����
      // �|�P�������J���[�ɂȂ�̂�҂�
      BOOL poke_fade = TRUE;
      {
        u8 i;
        for( i=0; i<POKE_MAX; i++ )
        {
          if( work->poke_wk[i] )
          {
            if( MCSS_CheckExecutePaletteFade( work->poke_wk[i] ) )
            {
              poke_fade = FALSE;
              break;
            }
          }
        }
      }

      if( poke_fade )
      {
        *seq = SEQ_FADE_OUT;

        // �t�F�[�h
        ZKNDTL_COMMON_FadeSetColorlessToBlack( work->fade_wk_m );
        ZKNDTL_COMMON_FadeSetColorlessToBlack( work->fade_wk_s );
        
        // �^�C�g���o�[
        ZUKAN_DETAIL_HEADBAR_Disappear( headbar );
        
        if( work->end_cmd == END_CMD_OUTSIDE )
        {
          // �^�b�`�o�[
          ZUKAN_DETAIL_TOUCHBAR_Disappear( touchbar, ZUKAN_DETAIL_TOUCHBAR_SPEED_OUTSIDE );
        }

        // MCSS�|�P����
        // �|�P������\�����t�F�[�h�A�E�g(�J���[����)
        {
          u8 i;
          for( i=0; i<POKE_MAX; i++ )
          {
            if( work->poke_wk[i] )
            {
              MCSS_SetPaletteFade( work->poke_wk[i], 0, 16, ZKNDTL_COMMON_FADE_BRIGHT_WAIT, 0x0000 );
            }
          }
        }
      }
    }
    break;
  case SEQ_FADE_OUT:
    {
      // MCSS�|�P����
      // �|�P���������ɂȂ�̂�҂�
      BOOL poke_fade = TRUE;
      {
        u8 i;
        for( i=0; i<POKE_MAX; i++ )
        {
          if( work->poke_wk[i] )
          {
            if( MCSS_CheckExecutePaletteFade( work->poke_wk[i] ) )
            {
              poke_fade = FALSE;
              break;
            }
          }
        }
      }

      if(    (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_m ))
          && (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_s ))
          && ZUKAN_DETAIL_HEADBAR_GetState( headbar ) == ZUKAN_DETAIL_HEADBAR_STATE_DISAPPEAR
          && poke_fade )
      {
        if( work->end_cmd == END_CMD_OUTSIDE )
        {
          if( ZUKAN_DETAIL_TOUCHBAR_GetState( touchbar ) == ZUKAN_DETAIL_TOUCHBAR_STATE_DISAPPEAR )
          {
            *seq = SEQ_END;
          }
        }
        else
        {
          *seq = SEQ_END;
        }
      }
    }
    break;
  case SEQ_END:
    {
      // MCSS�|�P����
      {
        u8 i;
        for( i=0; i<POKE_MAX; i++ )
        {
          BLOCK_POKE_EXIT( work->mcss_sys_wk, work->poke_wk[i] )
        }
      }
      Zukan_Detail_Form_McssSysExit( param, work, cmn );

      // 3D
      ZUKAN_DETAIL_GRAPHIC_Exit3D( graphic );

      // �؂�ւ�
      {
        // �O���t�B�b�N�X���[�h�ݒ�
        GX_SetGraphicsMode( GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D );
      }

      return ZKNDTL_PROC_RES_FINISH;
    }
    break;
  }

  // MCSS�|�P����
  if( work->mcss_sys_wk )
  {
    MCSS_Main( work->mcss_sys_wk );
  } 

  // �Ŕw��
  if( *seq >= SEQ_PREPARE )
  {
    ZKNDTL_COMMON_RearMain( work->rear_wk_m );
    ZKNDTL_COMMON_RearMain( work->rear_wk_s );
  }

  // �t�F�[�h
  ZKNDTL_COMMON_FadeMain( work->fade_wk_m, work->fade_wk_s );

  return ZKNDTL_PROC_RES_CONTINUE;
}

//-------------------------------------
/// PROC ���ߏ���
//=====================================
static void Zukan_Detail_Form_CommandFunc( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn, ZKNDTL_COMMAND cmd )
{
  if( mywk )
  {
    ZUKAN_DETAIL_FORM_PARAM*    param    = (ZUKAN_DETAIL_FORM_PARAM*)pwk;
    ZUKAN_DETAIL_FORM_WORK*     work     = (ZUKAN_DETAIL_FORM_WORK*)mywk;

    ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
  
    switch( cmd )
    {
    case ZKNDTL_CMD_NONE:
      {
      }
      break;
    case ZKNDTL_CMD_CLOSE:
    case ZKNDTL_CMD_RETURN:
      {
        work->end_cmd = END_CMD_OUTSIDE;
      }
      break;
    case ZKNDTL_CMD_INFO:
    case ZKNDTL_CMD_MAP:
    case ZKNDTL_CMD_VOICE:
      {
        work->end_cmd = END_CMD_INSIDE;
      }
      break;
    case ZKNDTL_CMD_CUR_D:
    case ZKNDTL_CMD_FORM_CUR_D:
      {
        u16 monsno_curr;
        u16 monsno_go;
        monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
        ZKNDTL_COMMON_GoToNextPoke(cmn);
        monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
        if( monsno_curr != monsno_go )
        {
          Zukan_Detail_Form_ChangePoke(param, work, cmn);
        }
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_CUR_U:
    case ZKNDTL_CMD_FORM_CUR_U:
      {
        u16 monsno_curr;
        u16 monsno_go;
        monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
        ZKNDTL_COMMON_GoToPrevPoke(cmn);
        monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
        if( monsno_curr != monsno_go )
        {
          Zukan_Detail_Form_ChangePoke(param, work, cmn);
        }
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_CHECK:
      {
        GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
        ZUKAN_SAVEDATA* zukan_savedata = GAMEDATA_GetZukanSave( gamedata );
        u16 monsno_curr;
        monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
        ZUKANSAVE_SetShortcutMons( zukan_savedata, monsno_curr );
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_FORM_CUR_R:
      {
        if( work->diff_num >= 3 )
        {
          work->diff_comp_no++;
          work->diff_comp_no %= work->diff_num;
          if( work->diff_curr_no == work->diff_comp_no )
          {
            work->diff_comp_no++;
            work->diff_comp_no %= work->diff_num;
          }

          Zukan_Detail_Form_ChangeCompareForm( param, work, cmn );
          Zukan_Detail_Form_MoveScrollBar( param, work, cmn );
        }
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_FORM_RETURN:
      {
        Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_TOP );
      }
      break;
    case ZKNDTL_CMD_FORM_CUR_L:
      {
        if( work->diff_num >= 3 )
        {
          work->diff_comp_no += work->diff_num -1;
          work->diff_comp_no %= work->diff_num;
          if( work->diff_curr_no == work->diff_comp_no )
          {
            work->diff_comp_no += work->diff_num -1;
            work->diff_comp_no %= work->diff_num;
          }

          Zukan_Detail_Form_ChangeCompareForm( param, work, cmn );
          Zukan_Detail_Form_MoveScrollBar( param, work, cmn );
        }
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_FORM_EXCHANGE:
      {
        if( work->diff_num >= 2 )
        {
          u16 no;
          MCSS_WORK* mw;

          // �ʒu����ւ�
          MCSS_SetPosition( work->poke_wk[POKE_COMP_F], &poke_pos[POKE_CURR_POS_LEFT] );
          MCSS_SetPosition( work->poke_wk[POKE_COMP_B], &poke_pos[POKE_CURR_POS_LEFT] );
          {
            VecFx32 p;
            PokeGetCompareRelativePosition( work->poke_wk[POKE_COMP_F], &p );
            
            MCSS_SetPosition( work->poke_wk[POKE_CURR_F], &p );
            MCSS_SetPosition( work->poke_wk[POKE_CURR_B], &p );
          }
          
          // �ԍ��A�|�C���^����ւ�
          no = work->diff_curr_no;
          work->diff_curr_no = work->diff_comp_no;
          work->diff_comp_no = no;

          mw = work->poke_wk[POKE_CURR_F];
          work->poke_wk[POKE_CURR_F] = work->poke_wk[POKE_COMP_F];
          work->poke_wk[POKE_COMP_F] = mw;

          mw = work->poke_wk[POKE_CURR_B];
          work->poke_wk[POKE_CURR_B] = work->poke_wk[POKE_COMP_B];
          work->poke_wk[POKE_COMP_B] = mw;

          // �|�P�A�C�R���ύX
          Zukan_Detail_Form_ChangePokeicon( param, work, cmn );

          // �e�L�X�g
          // �e�L�X�g��OBJ�ł͂Ȃ�BG�Ȃ̂ŁA����ւ��ł��Ȃ��̂ŊہX��������
          Zukan_Detail_Form_WritePokeCurrText( param, work, cmn );
          Zukan_Detail_Form_WritePokeCompText( param, work, cmn );
        }
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    default:
      {
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    }
  }
}

//-------------------------------------
/// PROC 3D�`��
//=====================================
static void Zukan_Detail_Form_Draw3DFunc( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  if( mywk )
  {
    ZUKAN_DETAIL_FORM_PARAM*    param    = (ZUKAN_DETAIL_FORM_PARAM*)pwk;
    ZUKAN_DETAIL_FORM_WORK*     work     = (ZUKAN_DETAIL_FORM_WORK*)mywk;

    if( work->mcss_sys_wk )
      MCSS_Draw( work->mcss_sys_wk );
  }
}


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// VBlank�֐�
//=====================================
static void Zukan_Detail_Form_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  ZUKAN_DETAIL_FORM_WORK*     work     = (ZUKAN_DETAIL_FORM_WORK*)wk;
}

//-------------------------------------
/// �Ⴄ�p��񂩂�|�P�����̎p�̐���STRBUF�𓾂�
    // �Ăяo�����ŉ�����邱��(NULL�̂Ƃ��͂Ȃ�)
//=====================================
static void Zukan_Detail_Form_GetPokeTextStrbufFromDiffInfo( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                STRBUF** a_strbuf0, STRBUF** a_strbuf1, u16 diff_no )
{
  STRBUF* strbuf0 = NULL;
  STRBUF* strbuf1 = NULL;
  
  u16 monsno = ZKNDTL_COMMON_GetCurrPoke(cmn);

  switch( work->diff_info_list[diff_no].looks_diff )
  {
  case LOOKS_DIFF_ONE:
    {
      WORDSET* wordset;
      STRBUF* src_strbuf;
      wordset = WORDSET_Create( param->heap_id );
      src_strbuf = GFL_MSG_CreateString( work->text_msgdata, ZKN_FORMNAME_10 );
      strbuf0 = GFL_STR_CreateBuffer( STRBUF_POKENAME_LENGTH, param->heap_id );
      WORDSET_RegisterPokeMonsNameNo( wordset, 0, monsno );
      WORDSET_ExpandStr( wordset, strbuf0, src_strbuf );
      GFL_STR_DeleteBuffer( src_strbuf );
      WORDSET_Delete( wordset );
    }
    break;
  case LOOKS_DIFF_SEX:
    {
      switch( work->diff_info_list[diff_no].other_diff )
      {
      case SEX_MALE:
        {
          strbuf0 = GFL_MSG_CreateString( work->text_msgdata, ZNK_ZUKAN_GRAPH_00 );
        }
        break;
      case SEX_FEMALE:
        {
          strbuf0 = GFL_MSG_CreateString( work->text_msgdata, ZNK_ZUKAN_GRAPH_01 );
        }
        break;
      }
    }
    break;
  case LOOKS_DIFF_FORM:
    {
      u16 form_no = work->diff_info_list[diff_no].other_diff;

      strbuf0 = GFL_MSG_CreateString( work->text_msgdata, ZNK_ZUKAN_GRAPH_07 );
    }
    break;
  }

  switch( work->diff_info_list[diff_no].color_diff )
  {
  case COLOR_DIFF_NONE:
    {
      // �������Ȃ�
    }
    break;
  case COLOR_DIFF_NORMAL:
    {
      // �������Ȃ�
    }
    break;
  case COLOR_DIFF_SPECIAL:
    {
      strbuf1 = GFL_MSG_CreateString( work->text_msgdata, ZKN_FORME_TEXT_05 );
    }
    break;
  }

  *a_strbuf0 = strbuf0;
  *a_strbuf1 = strbuf1;
}

//-------------------------------------
/// �e�L�X�g
//=====================================
static void Zukan_Detail_Form_CreateTextBase( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 bmpwin_setup[TEXT_MAX][7] =
  {
    { BG_FRAME_S_TEXT, 19, 16,  8,  2, BG_PAL_POS_S_TEXT, GFL_BMP_CHRAREA_GET_B },
    { BG_FRAME_S_TEXT,  3, 19, 23,  4, BG_PAL_POS_S_TEXT, GFL_BMP_CHRAREA_GET_B },
    { BG_FRAME_S_TEXT, 27, 19,  2,  4, BG_PAL_POS_S_TEXT, GFL_BMP_CHRAREA_GET_B },
    { BG_FRAME_M_TEXT,  0,  1, 16,  4, BG_PAL_POS_M_TEXT, GFL_BMP_CHRAREA_GET_B },
    { BG_FRAME_M_TEXT, 16,  1, 16,  4, BG_PAL_POS_M_TEXT, GFL_BMP_CHRAREA_GET_B },
  };

  // �p���b�g
  GFL_ARC_UTIL_TransVramPaletteEx( ARCID_FONT, NARC_font_default_nclr,
       PALTYPE_SUB_BG,
       0,
       BG_PAL_POS_S_TEXT * 0x20,
       BG_PAL_NUM_S_TEXT * 0x20,
       param->heap_id );

  GFL_ARC_UTIL_TransVramPaletteEx( ARCID_FONT, NARC_font_default_nclr,
       PALTYPE_MAIN_BG,
       0,
       BG_PAL_POS_S_TEXT * 0x20,
       BG_PAL_NUM_S_TEXT * 0x20,
       param->heap_id );

  // �r�b�g�}�b�v�E�B���h�E
  {
    u8 i;
    for( i=0; i<TEXT_MAX; i++ )
    {
      work->text_bmpwin[i] = GFL_BMPWIN_Create( bmpwin_setup[i][0],
                                         bmpwin_setup[i][1], bmpwin_setup[i][2], bmpwin_setup[i][3], bmpwin_setup[i][4],
                                         bmpwin_setup[i][5],
                                         bmpwin_setup[i][6] );

      // �N���A
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->text_bmpwin[i] ), 0 );
      GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[i] );
    }
  }

  // ���b�Z�[�W
  work->text_msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
                           ARCID_MESSAGE, NARC_message_zkn_dat,
                           param->heap_id );
}
static void Zukan_Detail_Form_DeleteTextBase( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 i;
  for( i=0; i<TEXT_MAX; i++ )
  {
    GFL_BMPWIN_Delete( work->text_bmpwin[i] );
  }

  GFL_MSG_Delete( work->text_msgdata );
}
static void Zukan_Detail_Form_WriteUpText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 i;

  // �O��̃e�L�X�g���N���A
  {
    for( i=TEXT_POKENAME; i<=TEXT_UP_NUM; i++ )
    {
      // �N���A
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->text_bmpwin[i] ), 0 );
      GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[i] );
    }
  }

  // ����̃e�L�X�g��`��
  {
    u16 monsno = ZKNDTL_COMMON_GetCurrPoke(cmn);
    u16 label_num_y[2] = { 1, 17 };
    
    {
      GFL_BMP_DATA*	bmp_data = GFL_BMPWIN_GetBmp( work->text_bmpwin[TEXT_POKENAME] );
      
      STRBUF* strbuf = GFL_MSG_CreateString( GlobalMsg_PokeName, monsno );

      u16 str_width = (u16)( PRINTSYS_GetStrWidth( strbuf, work->font, 0 ) );
      u16 bmp_width = GFL_BMP_GetSizeX( bmp_data );
      u16 x = ( bmp_width - str_width ) / 2;

/*
      u16 str_height = (u16)( PRINTSYS_GetStrHeight( strbuf, work->font ) );
      u16 bmp_height = GFL_BMP_GetSizeY( bmp_data );
      u16 y = ( bmp_height - str_height ) / 2;
*/
      u16 y = 1;  // �v�Z�ŏo�����ʒu�͒��S����Ɍ�����̂�

      PRINTSYS_PrintQueColor( work->print_que, bmp_data,
                              x, y, strbuf, work->font, PRINTSYS_LSB_Make(15,2,0) );

      GFL_STR_DeleteBuffer( strbuf );
    }

    {
      STRBUF* strbuf;
      GFL_BMP_DATA*	bmp_data = GFL_BMPWIN_GetBmp( work->text_bmpwin[TEXT_UP_LABEL] );
      
      strbuf = GFL_MSG_CreateString( work->text_msgdata, ZKN_FORME_TEXT_01 );
      PRINTSYS_PrintQueColor( work->print_que, bmp_data,
                              0, label_num_y[0], strbuf, work->font, PRINTSYS_LSB_Make(15,2,0) );
      GFL_STR_DeleteBuffer( strbuf );

      strbuf = GFL_MSG_CreateString( work->text_msgdata, ZKN_FORME_TEXT_02 );
      PRINTSYS_PrintQueColor( work->print_que, bmp_data,
                              0, label_num_y[1], strbuf, work->font, PRINTSYS_LSB_Make(15,2,0) );
      GFL_STR_DeleteBuffer( strbuf );
    }

    {
      WORDSET* wordset;
      STRBUF* src_strbuf;
      STRBUF* strbuf;
      GFL_BMP_DATA*	bmp_data = GFL_BMPWIN_GetBmp( work->text_bmpwin[TEXT_UP_NUM] );

      wordset = WORDSET_Create( param->heap_id );
      src_strbuf = GFL_MSG_CreateString( work->text_msgdata, ZKN_FORME_TEXT_03 );
      strbuf = GFL_STR_CreateBuffer( STRBUF_NUM_LENGTH, param->heap_id );
      WORDSET_RegisterNumber( wordset, 0, work->diff_sugata_num, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
      WORDSET_ExpandStr( wordset, strbuf, src_strbuf );
      PRINTSYS_PrintQueColor( work->print_que, bmp_data,
                              0, label_num_y[0], strbuf, work->font, PRINTSYS_LSB_Make(15,2,0) );
      GFL_STR_DeleteBuffer( strbuf );
      GFL_STR_DeleteBuffer( src_strbuf );
      WORDSET_Delete( wordset );

      wordset = WORDSET_Create( param->heap_id );
      src_strbuf = GFL_MSG_CreateString( work->text_msgdata, ZKN_FORME_TEXT_04 );
      strbuf = GFL_STR_CreateBuffer( STRBUF_NUM_LENGTH, param->heap_id );
      WORDSET_RegisterNumber( wordset, 0, work->diff_irochigai_num, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
      WORDSET_ExpandStr( wordset, strbuf, src_strbuf );
      PRINTSYS_PrintQueColor( work->print_que, bmp_data,
                              0, label_num_y[1], strbuf, work->font, PRINTSYS_LSB_Make(15,2,0) );
      GFL_STR_DeleteBuffer( strbuf );
      GFL_STR_DeleteBuffer( src_strbuf );
      WORDSET_Delete( wordset );
    }

    for( i=TEXT_POKENAME; i<=TEXT_UP_NUM; i++ )
    {
      // �X�N���[���]��
      GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[i] );
    }
  }
}

static void Zukan_Detail_Form_WritePokeText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                GFL_BMPWIN* bmpwin, u16 diff_no )
{
  GFL_BMP_DATA*	bmp_data = GFL_BMPWIN_GetBmp( bmpwin );
  u16 bmp_width = GFL_BMP_GetSizeX( bmp_data );

  // ����̃e�L�X�g��`��
  {
    STRBUF* strbuf0;
    STRBUF* strbuf1;

    u16 str_width;
    u16 x;
    u16 y[2];

    Zukan_Detail_Form_GetPokeTextStrbufFromDiffInfo( param, work, cmn,
         &strbuf0, &strbuf1, diff_no );

    if( strbuf1 )
    {
      y[0] = TEXT_POKE_POS_Y_2_LINE_0;
      y[1] = TEXT_POKE_POS_Y_2_LINE_1;
    }
    else
    {
      y[0] = TEXT_POKE_POS_Y_1_LINE;
    }

    str_width = (u16)( PRINTSYS_GetStrWidth( strbuf0, work->font, 0 ) );
    x = ( bmp_width - str_width ) / 2;
    PRINTSYS_PrintQueColor( work->print_que, bmp_data,
                            x, y[0], strbuf0, work->font, PRINTSYS_LSB_Make(15,2,0) );

    if( strbuf1 )
    {
      str_width = (u16)( PRINTSYS_GetStrWidth( strbuf1, work->font, 0 ) );
      x = ( bmp_width - str_width ) / 2;
      PRINTSYS_PrintQueColor( work->print_que, bmp_data,
                              x, y[1], strbuf1, work->font, PRINTSYS_LSB_Make(15,2,0) );
    }

    if( strbuf0 ) GFL_STR_DeleteBuffer( strbuf0 );
    if( strbuf1 ) GFL_STR_DeleteBuffer( strbuf1 );

    // �X�N���[���]��
    GFL_BMPWIN_MakeTransWindow_VBlank( bmpwin );
  }
}
static void Zukan_Detail_Form_WritePokeCurrText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �O��̃e�L�X�g���N���A
  {
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->text_bmpwin[TEXT_POKE_CURR] ), 0 );
    GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[TEXT_POKE_CURR] );
  }

  // ����̃e�L�X�g��`��
  Zukan_Detail_Form_WritePokeText( param, work, cmn,
               work->text_bmpwin[TEXT_POKE_CURR], work->diff_curr_no );
 }
static void Zukan_Detail_Form_WritePokeCompText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �O��̃e�L�X�g���N���A
  {
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->text_bmpwin[TEXT_POKE_COMP] ), 0 );
    GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[TEXT_POKE_COMP] );
  }

  // ����̃e�L�X�g��`��
  switch(work->state)
  {
  case STATE_TOP:
    {
      // �`�悵�Ȃ�
    }
    break;
  case STATE_EXCHANGE:
    {
      if( work->diff_num >= 2 )
      {
        Zukan_Detail_Form_WritePokeText( param, work, cmn,
               work->text_bmpwin[TEXT_POKE_COMP], work->diff_comp_no );
      }
      // 2�p�ȏ�Ȃ���΁A�`�悵�Ȃ�
    }
    break;
  }
}
static void Zukan_Detail_Form_ScrollPokeText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  switch(work->state)
  {
  case STATE_TOP:
    {
      GFL_BG_SetScroll( BG_FRAME_M_TEXT, GFL_BG_SCROLL_X_SET, -64 );
    }
    break;
  case STATE_EXCHANGE:
    {
      if( work->diff_num >= 2 )
      {
        GFL_BG_SetScroll( BG_FRAME_M_TEXT, GFL_BG_SCROLL_X_SET, 0 );
      }
      else
      {
        GFL_BG_SetScroll( BG_FRAME_M_TEXT, GFL_BG_SCROLL_X_SET, -64 );
      }
    }
    break;
  }
}

//-------------------------------------
/// MCSS�|�P����
//=====================================
static void Zukan_Detail_Form_McssSysInit( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  work->mcss_sys_wk = MCSS_Init( POKE_MAX, param->heap_id );
  MCSS_SetTextureTransAdrs( work->mcss_sys_wk, 0 );  // ���Ɉꏏ�ɏo��3D�͂Ȃ��̂�
  MCSS_SetOrthoMode( work->mcss_sys_wk );
}
static void Zukan_Detail_Form_McssSysExit( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  MCSS_Exit( work->mcss_sys_wk );
}

static MCSS_WORK* PokeInit( MCSS_SYS_WORK* mcss_sys_wk,
                      int mons_no, int form_no, int sex, int rare, BOOL egg, int dir )
{
  VecFx32 scale =
  {
    FX_F32_TO_FX32(POKE_SCALE),
    FX_F32_TO_FX32(POKE_SCALE),
    FX32_ONE, 
  };

  MCSS_ADD_WORK add_wk;
  MCSS_WORK*    poke_wk;

  MCSS_TOOL_MakeMAWParam( mons_no, form_no, sex, rare, egg, &add_wk, dir );
  poke_wk = MCSS_Add( mcss_sys_wk, 0, 0, 0, &add_wk );
  
  MCSS_SetAnmStopFlag( poke_wk );
  MCSS_SetScale( poke_wk, &scale );
  PokeAdjustOfsPos( poke_wk );

  return poke_wk;
}
static void PokeExit( MCSS_SYS_WORK* mcss_sys_wk, MCSS_WORK* poke_wk )
{
  MCSS_SetVanishFlag( poke_wk );
  MCSS_Del( mcss_sys_wk, poke_wk );
}
static void PokeAdjustOfsPos( MCSS_WORK* poke_wk )
{
  f32 size_y = (f32)MCSS_GetSizeY( poke_wk );
  f32 ofs_y;
  VecFx32 ofs;
  if( size_y > POKE_SIZE_MAX ) size_y = POKE_SIZE_MAX;
  ofs_y = ( POKE_SIZE_MAX - size_y ) / 2.0f;
  ofs.x = 0;  ofs.y = FX_F32_TO_FX32(ofs_y);  ofs.z = 0;
  MCSS_SetOfsPosition( poke_wk, &ofs );
}

static void PokeGetCompareRelativePosition( MCSS_WORK* poke_wk, VecFx32* pos )
{
  VecFx32 origin;
  MCSS_GetPosition( poke_wk, &origin );

  pos->x = origin.x + poke_pos[POKE_COMP_RPOS].x;
  pos->y = origin.y + poke_pos[POKE_COMP_RPOS].y;
  pos->z = origin.z + poke_pos[POKE_COMP_RPOS].z;
}

//-------------------------------------
/// �|�P�A�C�R��
//=====================================
static GFL_CLWK* PokeiconInit( UI_EASY_CLWK_RES* res, GFL_CLUNIT* clunit, HEAPID heap_id, u32 mons, u32 form_no, BOOL egg )
{
  GFL_CLWK* clwk;
  
  UI_EASY_CLWK_RES_PARAM prm;
  prm.draw_type    = CLSYS_DRAW_SUB;
  prm.comp_flg     = UI_EASY_CLWK_RES_COMP_NCLR;
  prm.arc_id       = ARCID_POKEICON;
  prm.pltt_id      = POKEICON_GetPalArcIndex();
  prm.ncg_id       = POKEICON_GetCgxArcIndexByMonsNumber( mons, form_no, egg );
  prm.cell_id      = POKEICON_GetCellArcIndex(); 
  prm.anm_id       = POKEICON_GetAnmArcIndex();
  prm.pltt_line    = OBJ_PAL_POS_S_POKEICON;
  prm.pltt_src_ofs = 0;
  prm.pltt_src_num = OBJ_PAL_NUM_S_POKEICON;
 
  UI_EASY_CLWK_LoadResource( res, &prm, clunit, heap_id );
  
  // �A�j���V�[�P���X�Ŏw��( 0=�m��, 1=HP�ő�, 2=HP��, 3=HP��, 4=HP��, 5=��Ԉُ� )
  clwk = UI_EASY_CLWK_CreateCLWK( res, clunit, pokeicon_pos[0], pokeicon_pos[1], 1, heap_id );

  // ��ɃA�C�e���A�C�R����`�悷��̂ŗD��x�������Ă���
  GFL_CLACT_WK_SetSoftPri( clwk, 1 );

  // �I�[�g�A�j�� OFF
  GFL_CLACT_WK_SetAutoAnmFlag( clwk, FALSE );

  {
    u8 pal_num = POKEICON_GetPalNum( mons, form_no, egg );
    GFL_CLACT_WK_SetPlttOffs( clwk, pal_num, CLWK_PLTTOFFS_MODE_OAM_COLOR );
  }

  GFL_CLACT_WK_SetObjMode( clwk, GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�

  return clwk;
}
static void PokeiconExit( UI_EASY_CLWK_RES* res, GFL_CLWK* clwk )
{
  // CLWK�j��
  GFL_CLACT_WK_Remove( clwk );
  // ���\�[�X�J��
  UI_EASY_CLWK_UnLoadResource( res );
}

//-------------------------------------
/// �Ⴄ�p�����擾����
//=====================================
static void Zukan_Detail_Form_GetDiffInfo( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u16 monsno = ZKNDTL_COMMON_GetCurrPoke(cmn);

  GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
  ZUKAN_SAVEDATA* zkn_sv = GAMEDATA_GetZukanSave( gamedata );

  // �������̃_�~�[�f�[�^�쐬
  {
    u16 diff_count = 0;
    
    POKEMON_PERSONAL_DATA* ppd = POKE_PERSONAL_OpenHandle( monsno, 0, param->heap_id );
    u16 form_max = (u16)POKE_PERSONAL_GetParam( ppd, POKEPER_ID_form_max );
    u32 sex      = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_sex );
    POKE_PERSONAL_CloseHandle( ppd );

    if( monsno == 151 )
    {
      work->diff_info_list[diff_count].looks_diff = LOOKS_DIFF_ONE;
      work->diff_info_list[diff_count].color_diff = COLOR_DIFF_NONE;
      diff_count++;
    }
    else
    {
      if( form_max <= 1 && sex != 0 )
      {
        work->diff_info_list[diff_count].looks_diff = LOOKS_DIFF_SEX;
        work->diff_info_list[diff_count].color_diff = COLOR_DIFF_NORMAL;
        work->diff_info_list[diff_count].other_diff = SEX_MALE;
        diff_count++;

        work->diff_info_list[diff_count].looks_diff = LOOKS_DIFF_SEX;
        work->diff_info_list[diff_count].color_diff = COLOR_DIFF_SPECIAL;
        work->diff_info_list[diff_count].other_diff = SEX_MALE;
        diff_count++;

        work->diff_info_list[diff_count].looks_diff = LOOKS_DIFF_SEX;
        work->diff_info_list[diff_count].color_diff = COLOR_DIFF_NORMAL;
        work->diff_info_list[diff_count].other_diff = SEX_FEMALE;
        diff_count++;

        work->diff_info_list[diff_count].looks_diff = LOOKS_DIFF_SEX;
        work->diff_info_list[diff_count].color_diff = COLOR_DIFF_SPECIAL;
        work->diff_info_list[diff_count].other_diff = SEX_FEMALE;
        diff_count++;
      }
      else if( form_max > 1 )
      {
        u16 i;
        for( i=0; i<form_max; i++ )
        {
          GF_ASSERT_MSG( diff_count < DIFF_MAX,  "ZUKAN_DETAIL_FORM : �Ⴄ�p�̐��������ł��B\n" );
  
          work->diff_info_list[diff_count].looks_diff = LOOKS_DIFF_FORM;
          work->diff_info_list[diff_count].color_diff = COLOR_DIFF_NORMAL;
          work->diff_info_list[diff_count].other_diff = i;
          diff_count++;

          work->diff_info_list[diff_count].looks_diff = LOOKS_DIFF_FORM;
          work->diff_info_list[diff_count].color_diff = COLOR_DIFF_SPECIAL;
          work->diff_info_list[diff_count].other_diff = i;
          diff_count++;
        }
      }
      else
      {
        work->diff_info_list[diff_count].looks_diff = LOOKS_DIFF_ONE;
        work->diff_info_list[diff_count].color_diff = COLOR_DIFF_NORMAL;
        diff_count++;

        work->diff_info_list[diff_count].looks_diff = LOOKS_DIFF_ONE;
        work->diff_info_list[diff_count].color_diff = COLOR_DIFF_SPECIAL;
        diff_count++;
      }
    }

    work->diff_num = diff_count;
    work->diff_curr_no = 0;
    work->diff_comp_no = 1 % diff_count;
  }

  // �p�̐��A�F�Ⴂ�̐�
  {
    u8 i;

    work->diff_sugata_num = 0;
    work->diff_irochigai_num = 0;

    for( i=0; i<work->diff_num; i++ )
    {
      if( work->diff_info_list[i].color_diff == COLOR_DIFF_SPECIAL )
      {
        work->diff_irochigai_num++;
      }
    }

    work->diff_sugata_num = work->diff_num - work->diff_irochigai_num;
  }
}

//-------------------------------------
/// �Ⴄ�p��񂩂�MCSS�|�P�����𐶐�����
//=====================================
static void Zukan_Detail_Form_PokeInitFromDiffInfo( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                POKE_INDEX poke_f, POKE_INDEX poke_b, POKE_POS_INDEX pos, u16 diff_no )
{
  int mons_no;
  int form_no;
  int sex;
  int rare;

  mons_no = ZKNDTL_COMMON_GetCurrPoke(cmn);

  switch( work->diff_info_list[diff_no].looks_diff )
  {
  case LOOKS_DIFF_ONE:
    {
      form_no = 0;
      sex = 0;
    }
    break;
  case LOOKS_DIFF_SEX:
    {
      form_no = 0;

      switch( work->diff_info_list[diff_no].other_diff )
      {
      case SEX_MALE:
        {
          sex = 0;
        }
        break;
      case SEX_FEMALE:
        {
          //sex = 1;
          sex = 0;
        }
        break;
      }
    }
    break;
  case LOOKS_DIFF_FORM:
    {
      sex = 0;

      form_no = work->diff_info_list[diff_no].other_diff;
    }
    break;
  }

  switch( work->diff_info_list[diff_no].color_diff )
  {
  case COLOR_DIFF_NONE:
    {
      rare = 0;
    }
    break;
  case COLOR_DIFF_NORMAL:
    {
      rare = 0;
    }
    break;
  case COLOR_DIFF_SPECIAL:
    {
      //rare = 1;
      rare = 0;
    }
    break;
  }

  work->poke_wk[poke_f] = PokeInit( work->mcss_sys_wk,
                              mons_no, form_no, sex, rare, FALSE, MCSS_DIR_FRONT );
  work->poke_wk[poke_b] = PokeInit( work->mcss_sys_wk,
                              mons_no, form_no, sex, rare, FALSE, MCSS_DIR_BACK );

  {
    VecFx32 p;
    if( pos == POKE_COMP_RPOS )
    {
      PokeGetCompareRelativePosition( work->poke_wk[POKE_CURR_F], &p );
    }
    else
    {
      p = poke_pos[pos];
    }
    MCSS_SetPosition( work->poke_wk[poke_f], &p );
    MCSS_SetPosition( work->poke_wk[poke_b], &p );
  }

  MCSS_SetVanishFlag( work->poke_wk[poke_b] );
}

//-------------------------------------
/// �Ⴄ�p��񂩂�|�P�A�C�R���𐶐�����
//=====================================
static void Zukan_Detail_Form_PokeiconInitFromDiffInfo( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                u16 diff_no )
{
  u32 mons;
  u32 form_no;

  mons = ZKNDTL_COMMON_GetCurrPoke(cmn);

  switch( work->diff_info_list[diff_no].looks_diff )
  {
  case LOOKS_DIFF_ONE:
    {
      form_no = 0;
    }
    break;
  case LOOKS_DIFF_SEX:
    {
      form_no = 0;
    }
    break;
  case LOOKS_DIFF_FORM:
    {
      form_no = work->diff_info_list[diff_no].other_diff;
    }
    break;
  }

  work->pokeicon_clwk = PokeiconInit( &work->pokeicon_res, work->clunit, param->heap_id, mons, form_no, 0 );
}

//-------------------------------------
/// �|�P�����ύX
//=====================================
static void Zukan_Detail_Form_ChangePoke( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �O�̂�j��
  u8 i;
  for( i=0; i<POKE_MAX; i++ )
  {
    BLOCK_POKE_EXIT( work->mcss_sys_wk, work->poke_wk[i] )
  }

  // ���̂𐶐�
  Zukan_Detail_Form_GetDiffInfo( param, work, cmn );

  {
    POKE_POS_INDEX pos = POKE_CURR_POS_CENTER;
    if( work->state == STATE_EXCHANGE && work->diff_num >= 2 ) 
    {
      pos = POKE_CURR_POS_LEFT;
    }
    Zukan_Detail_Form_PokeInitFromDiffInfo( param, work, cmn,
        POKE_CURR_F, POKE_CURR_B, pos, work->diff_curr_no );
  }
  if( work->diff_num >= 2 )
  {
    Zukan_Detail_Form_PokeInitFromDiffInfo( param, work, cmn,
        POKE_COMP_F, POKE_COMP_B, POKE_COMP_RPOS, work->diff_comp_no );
  }

  // �|�P�A�C�R���ύX
  Zukan_Detail_Form_ChangePokeicon( param, work, cmn );

  // �e�L�X�g
  // �ύX���ꂽ�|�P�����ŁA���ʂ̃e�L�X�g������
  Zukan_Detail_Form_WriteUpText( param, work, cmn );
  // �ύX���ꂽ�|�P�����ŁA�e�L�X�g���X�N���[�������āA���̃t�H�����̃e�L�X�g�������A��r�t�H�����̃e�L�X�g������or����
  Zukan_Detail_Form_ScrollPokeText( param, work, cmn );
  Zukan_Detail_Form_WritePokeCurrText( param, work, cmn );
  Zukan_Detail_Form_WritePokeCompText( param, work, cmn );
}

//-------------------------------------
/// ��r�t�H�����ύX
//=====================================
static void Zukan_Detail_Form_ChangeCompareForm( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �O�̂�j��
  BLOCK_POKE_EXIT( work->mcss_sys_wk, work->poke_wk[POKE_COMP_F] )
  BLOCK_POKE_EXIT( work->mcss_sys_wk, work->poke_wk[POKE_COMP_B] )

  // ���̂𐶐�
  Zukan_Detail_Form_PokeInitFromDiffInfo( param, work, cmn,
      POKE_COMP_F, POKE_COMP_B, POKE_COMP_RPOS, work->diff_comp_no );

  // �e�L�X�g
  // �ύX���ꂽ��r�t�H�����Ńe�L�X�g������
  Zukan_Detail_Form_WritePokeCompText( param, work, cmn );
}

//-------------------------------------
/// �|�P�A�C�R���ύX
//=====================================
static void Zukan_Detail_Form_ChangePokeicon( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �O�̂�j��
  BLOCK_POKEICON_EXIT( &work->pokeicon_res, work->pokeicon_clwk )
  
  // ���̂𐶐�
  Zukan_Detail_Form_PokeiconInitFromDiffInfo( param, work, cmn, work->diff_curr_no );
}

//-------------------------------------
/// ����
//=====================================
static void Zukan_Detail_Form_Input( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  switch(work->state)
  {
  case STATE_TOP:
    {
      u32 x, y;
      BOOL change_state = FALSE;
      if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
      {
        change_state = TRUE;
      }
      else
      {
		    if( GFL_UI_TP_GetPointTrg( &x, &y ) )
        {
          if( 0<=y&&y<168) change_state = TRUE;
        }
      }
      if( change_state )
      {
        Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_EXCHANGE );
        PMSND_PlaySE( SEQ_SE_DECIDE1 );
      }
    }
    break;
  case STATE_EXCHANGE:
    {
    }
    break;
  }
}

//-------------------------------------
/// �X�N���[���o�[���ړ�������
//=====================================
static void Zukan_Detail_Form_MoveScrollBar( param, work, cmn )
{
}

//-------------------------------------
/// ��Ԃ�J�ڂ�����
//=====================================
static void Zukan_Detail_Form_ChangeState( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                STATE state )
{
  ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
 
  // �J�ڑO�̏�� 
  switch(work->state)
  {
  case STATE_TOP:
    {
      if( work->diff_num >= 2 )
      {
        VecFx32 p;

        MCSS_SetPosition( work->poke_wk[POKE_CURR_F], &poke_pos[POKE_CURR_POS_LEFT] );
        MCSS_SetPosition( work->poke_wk[POKE_CURR_B], &poke_pos[POKE_CURR_POS_LEFT] );

        PokeGetCompareRelativePosition( work->poke_wk[POKE_CURR_F], &p );
        MCSS_SetPosition( work->poke_wk[POKE_COMP_F], &p );
        MCSS_SetPosition( work->poke_wk[POKE_COMP_B], &p );
      }

      // �^�b�`�o�[
      ZUKAN_DETAIL_TOUCHBAR_SetType(
          touchbar,
          ZUKAN_DETAIL_TOUCHBAR_TYPE_FORM,
          ZUKAN_DETAIL_TOUCHBAR_DISP_FORM );
    }
    break;
  case STATE_EXCHANGE:
    {
      if( work->diff_num >= 2 )
      {
        VecFx32 p;

        MCSS_SetPosition( work->poke_wk[POKE_CURR_F], &poke_pos[POKE_CURR_POS_CENTER] );
        MCSS_SetPosition( work->poke_wk[POKE_CURR_B], &poke_pos[POKE_CURR_POS_CENTER] );
      
        PokeGetCompareRelativePosition( work->poke_wk[POKE_CURR_F], &p );
        MCSS_SetPosition( work->poke_wk[POKE_COMP_F], &p );
        MCSS_SetPosition( work->poke_wk[POKE_COMP_B], &p );
      }

      // �^�b�`�o�[
      ZUKAN_DETAIL_TOUCHBAR_SetType(
          touchbar,
          ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL,
          ZUKAN_DETAIL_TOUCHBAR_DISP_FORM );
    }
    break;
  }

  // �J�ڌ�̏��
  work->state = state;

  // �e�L�X�g
  // �J�ڂ��ꂽ��ԂŁA�e�L�X�g���X�N���[�������āA��r�t�H�����̃e�L�X�g������or����
  Zukan_Detail_Form_ScrollPokeText( param, work, cmn );
  Zukan_Detail_Form_WritePokeCompText( param, work, cmn );
}

//=============================================================================
/**
 * 
 */
//=============================================================================
// �}�N��
#undef BLOCK_POKE_EXIT
#undef BLOCK_POKEICON_EXIT

