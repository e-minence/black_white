//============================================================================
/**
 *  @file   zukan_detail_info.c
 *  @brief  �}�ӏڍ׉�ʂ̐������
 *  @author Koji Kawada
 *  @data   2010.02.02
 *  @note   
 *  ���W���[�����FZUKAN_DETAIL_INFO
 */
//============================================================================
//#define DEBUG_KAWADA


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "gamesystem/gamedata_def.h"
#include "savedata/zukan_savedata.h"
#include "sound/pm_sndsys.h"

#include "../info/zukan_info.h"

#include "zukan_detail_def.h"
#include "zukan_detail_common.h"
#include "zukan_detail_info.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "zukan_gra.naix"

// �T�E���h

// �I�[�o�[���C
FS_EXTERN_OVERLAY(ui_common);


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
// ���C��OBJ�p���b�g
/*
�^�b�`�o�[�œǂݍ��񂾃p���b�g�𗬗p����̂ŁA�����ł͓ǂݍ��܂Ȃ�
// �{��
enum
{
  OBJ_PAL_NUM_M_LANG_BTN             = 3,
};
// �ʒu
enum
{
  OBJ_PAL_POS_M_LANG_BTN             = 6,
};
*/

// ����{�^���̃p���b�g�A�j���p�Ƀp���b�g��ύX����
/*
�^�b�`�o�[�œǂݍ��񂾃p���b�g�𗬗p����̂ŁA�����ł͓ǂݍ��܂Ȃ�
#define OBJ_PAL_OFFSET_LANG_ANIME_NONE  (0)
#define OBJ_PAL_OFFSET_LANG_ANIME_EXEC  (2)
*/
#define OBJ_PAL_POS_M_LANG_ANIME_NONE   (ZKNDTL_OBJ_PAL_POS_M_TOUCHBAR + ZKND_TBAR_OBJ_PLT_NUM)  // zukan/detail/zukan_detail_touchbar.c OBJ_PAL_POS_M_CUSTOM ����  // �p���b�g�𒼂ŕς�����Ăł��Ȃ����ۂ�����A���ǂ���͎g�p�����Azukan/detail/zukan_detail_touchbar.c OBJ_PAL_OFFSET_GENERAL_ANIME_NONE�𗬗p���邱�Ƃɂ����B
#define OBJ_PAL_POS_M_LANG_ANIME_EXEC   (6)  // zukan/info/zukan_info.c ZUKAN_INFO_BG_PAL_POS_MAX ����  // �p���b�g�𒼂ŕς�����Ăł��Ȃ����ۂ�����A���ǂ���͎g�p�����Azukan/detail/zukan_detail_touchbar.c OBJ_PAL_OFFSET_GENERAL_ANIME_EXEC�𗬗p���邱�Ƃɂ����B

#define OBJ_PAL_OFFSET_LANG_ANIME_NONE  (0)  // zukan/detail/zukan_detail_touchbar.c OBJ_PAL_OFFSET_GENERAL_ANIME_NONE ����
#define OBJ_PAL_OFFSET_LANG_ANIME_EXEC  (2)  // zukan/detail/zukan_detail_touchbar.c OBJ_PAL_OFFSET_GENERAL_ANIME_EXEC ����
// zukan/detail/zukan_detail_touchbar.c OBJ_PAL_OFFSET_GENERAL_ANIME_NONE��OBJ_PAL_OFFSET_GENERAL_ANIME_EXEC�𗘗p����悤�ɂ����̂ŁA���̃\�[�X���̃p���b�g�A�j���֘A�̋L�q�͍폜���Ă��\��Ȃ��B

#define RES_PAL_POS_LANG_ANIME_START    (1)  // ���\�[�X�̃p���b�g�̗�ԍ�
#define RES_PAL_POS_LANG_ANIME_END      (3)  // ���\�[�X�̃p���b�g�̗�ԍ�
#define LANG_ANIME_ADD              (0x400)  // FX_CosIdx���g�p����̂�0<= <0x10000

// ProcMain�̃V�[�P���X
enum
{
  SEQ_START      = 0,
  SEQ_PREPARE,
  SEQ_FADE_IN,
  SEQ_MAIN,
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

// ����{�^��
#define LEFT_SPACE (2)
static const u8 lang_button_rect[ZUKAN_INFO_LANG_MAX][9] =
{
  // place_x,         place_y, touch_x,         touch_y, touch_w, touch_h, active_anmseq, push_anmseq, lang_code
  {  LEFT_SPACE+   0,      91, LEFT_SPACE+   0,      91,      16,      15,             8,          15, LANG_ENGLISH  },
  {  LEFT_SPACE+  15,      91, LEFT_SPACE+  17,      91,      20,      15,             9,          16, LANG_FRANCE   },
  {  LEFT_SPACE+  36,      91, LEFT_SPACE+  38,      91,      20,      15,            10,          27, LANG_GERMANY  },
  {  LEFT_SPACE+  57,      91, LEFT_SPACE+  59,      91,      20,      15,            11,          17, LANG_ITALY    },
  {  LEFT_SPACE+  78,      91, LEFT_SPACE+  80,      91,      20,      15,            12,          18, LANG_SPAIN    },
  {  LEFT_SPACE+  99,      91, LEFT_SPACE+ 101,      91,      20,      15,            13,          19, LANG_KOREA    },
};
#undef LEFT_SPACE

// ����{�^���̏��
typedef enum
{
  LANG_BTN_STATE_ACTIVE,      // �������Ƃ��ł���
  LANG_BTN_STATE_PUSH_START,  // �������u��(1�t���[��)
  LANG_BTN_STATE_PUSH_ANIME,  // �������A�j����
  LANG_BTN_STATE_PUSH_END,    // �������A�j�����I�������u��(1�t���[��)
}
LANG_BUTTON_STATE;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
// ����{�^��
typedef struct
{
  GFL_CLWK*           clwk;
  u8                  x;  // touch���W
  u8                  y;
  u8                  w;  // touch�T�C�Y
  u8                  h;
  u16                 active_anmseq;
  u16                 push_anmseq;
  u8                  lang_code;  // ����R�[�h(�J���g���[�R�[�h�A���R�[�h�ƌĂ�ł���Ƃ��������)  // pm_version.h��LANG_ENGLISH, LANG_FRANCE�Ȃ�
  LANG_BUTTON_STATE   state;
}
LANG_BUTTON;

//-------------------------------------
/// PROC ���[�N
//=====================================
typedef struct
{
  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // �}�ӏ��
  ZUKAN_INFO_WORK*            info_wk_m;
  ZUKAN_INFO_WORK*            info_wk_s;

  // �t�F�[�h
  ZKNDTL_COMMON_FADE_WORK*    fade_wk_m;
  ZKNDTL_COMMON_FADE_WORK*    fade_wk_s;

  // �I������
  END_CMD                     end_cmd;

  // ���͉s��
  BOOL                        input_enable;  // ���͉̂Ƃ�TRUE

  // ����
  LANG_BUTTON                 lang_btn[ZUKAN_INFO_LANG_MAX];
  ZUKAN_INFO_LANG             lang_btn_push;  // �������A�j�����Ă���{�^��(ZUKAN_INFO_LANG_NONE�̂Ƃ��Ȃ�)
  ZUKAN_INFO_LANG             lang;  // ���ʂɍ��\�����Ă��錾��
  u32                         lang_ncl;  // �^�b�`�o�[�œǂݍ��񂾃p���b�g�𗬗p����̂ŁA�����ł͓ǂݍ��܂Ȃ�
  u32                         lang_ncg;
  u32                         lang_nce;

  // ����{�^���̃p���b�g�A�j��
  //u16           pal_anime_lang_default[0x10];  // pal_anime_lang_default�����蓖�Ă�Ɖ����A�j�����Ȃ��Ȃ��Ă��܂����̂ŁApal_anime_lang_pltt_offs_req���g�����Ƃɂ����B
  u16           pal_anime_lang_start[0x10];
  u16           pal_anime_lang_end[0x10];
  u16           pal_anime_lang_now[0x10];
  int           pal_anime_lang_count;
  
  BOOL          pal_anime_lang_pltt_offs_req;  // �����A�j�����Ă���Ƃ��́A�p���b�g�A�j�������Ȃ��悤�ɁA�p���b�g�A�j���̊J�n��x�点��
}
ZUKAN_DETAIL_INFO_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Zukan_Detail_Info_VBlankFunc( GFL_TCB* tcb, void* wk );

// ����{�^���̃^�b�`����
static BOOL Zukan_Detail_Info_TouchLangButton( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn );
// ����{�^���̃L�[����
static BOOL Zukan_Detail_Info_KeyLangButton( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �|�P������ύX����
static void Zukan_Detail_Info_ChangePoke( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn );
// �����ύX����
static void Zukan_Detail_Info_ChangeLang( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn,
           ZUKAN_INFO_LANG lang);

// ����{�^��
static void Zukan_Detail_Info_CreateLangButton( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Info_DeleteLangButton( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Info_MainLangButton( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Info_SetupLangButtonDrawEnable( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                BOOL e, BOOL fra, BOOL ger, BOOL ita, BOOL spa, BOOL kor );

// ���ݕ\������|�P�����̏��𓾂�
static void Zukan_Detail_Info_GetCurrPokeInfo(
                ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                u16*  a_monsno,
                u16*  a_formno,
                u16*  a_sex,
                u16*  a_rare,
                u32*  a_personal_rnd,
                BOOL* a_b_get_flag,
                BOOL* a_lang_exist );  // a_lang_exist[ZUKAN_INFO_LANG_MAX]

// ����{�^���̃p���b�g�A�j��
static void Zukan_Detail_Info_AnimeBaseInitLang( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Info_AnimeBaseExitLang( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Info_AnimeInitLang( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Info_AnimeExitLang( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Info_AnimeMainLang( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn );


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Info_ProcInit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );
static ZKNDTL_PROC_RESULT Zukan_Detail_Info_ProcExit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );
static ZKNDTL_PROC_RESULT Zukan_Detail_Info_ProcMain( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Info_CommandFunc( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn, ZKNDTL_COMMAND cmd );

const ZKNDTL_PROC_DATA    ZUKAN_DETAIL_INFO_ProcData =
{
  Zukan_Detail_Info_ProcInit,
  Zukan_Detail_Info_ProcMain,
  Zukan_Detail_Info_ProcExit,
  Zukan_Detail_Info_CommandFunc,
  NULL,
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
void             ZUKAN_DETAIL_INFO_InitParam(
                            ZUKAN_DETAIL_INFO_PARAM*  param,
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
static ZKNDTL_PROC_RESULT Zukan_Detail_Info_ProcInit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_INFO_PARAM*    param    = (ZUKAN_DETAIL_INFO_PARAM*)pwk;
  ZUKAN_DETAIL_INFO_WORK*     work;

#ifdef DEBUG_KAWADA
  {
    OS_Printf( "Zukan_Detail_Info_ProcInit\n" );
  }
#endif

  // �I�[�o�[���C
  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );
  GFL_OVERLAY_Load( FS_OVERLAY_ID(zukan_info) );

  // �q�[�v
  {
    work = ZKNDTL_PROC_AllocWork(proc, sizeof(ZUKAN_DETAIL_INFO_WORK), param->heap_id);
    GFL_STD_MemClear( work, sizeof(ZUKAN_DETAIL_INFO_WORK) ); 
  }
  
  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Zukan_Detail_Info_VBlankFunc, work, 1 );

  // ����
  work->lang_btn_push = ZUKAN_INFO_LANG_NONE;
  work->lang = ZUKAN_INFO_LANG_NONE;

  work->pal_anime_lang_pltt_offs_req = FALSE;

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

  // ���͉s��
  work->input_enable = TRUE;

  return ZKNDTL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Info_ProcExit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_INFO_PARAM*    param    = (ZUKAN_DETAIL_INFO_PARAM*)pwk;
  ZUKAN_DETAIL_INFO_WORK*     work     = (ZUKAN_DETAIL_INFO_WORK*)mywk;

  // ����{�^���̃p���b�g�A�j��
  Zukan_Detail_Info_AnimeExitLang( param, work, cmn );
  Zukan_Detail_Info_AnimeBaseExitLang( param, work, cmn );

  // �}�ӏ��
  ZUKAN_INFO_Exit( work->info_wk_s );
  ZUKAN_INFO_Exit( work->info_wk_m );

  // ����{�^��
  Zukan_Detail_Info_DeleteLangButton( param, work, cmn );

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
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(zukan_info) );
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );

#ifdef DEBUG_KAWADA
  {
    OS_Printf( "Zukan_Detail_Info_ProcExit\n" );
  }
#endif

  return ZKNDTL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �又��
//=====================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Info_ProcMain( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_INFO_PARAM*    param    = (ZUKAN_DETAIL_INFO_PARAM*)pwk;
  ZUKAN_DETAIL_INFO_WORK*     work     = (ZUKAN_DETAIL_INFO_WORK*)mywk;

  ZUKAN_DETAIL_TOUCHBAR_WORK* touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
  ZUKAN_DETAIL_HEADBAR_WORK*  headbar  = ZKNDTL_COMMON_GetHeadbar(cmn);
  
  switch(*seq)
  {
  case SEQ_START:
    {
      *seq = SEQ_PREPARE;

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
      }

    // �^�b�`�o�[�œǂݍ��񂾃p���b�g�𗬗p����̂ŁA�����ł͓ǂݍ��܂Ȃ�
    // �Ƃ����̂�����̂ŁA�^�b�`�o�[���ɐ������Ă����B
    {
      // �^�b�`�o�[
      if( ZUKAN_DETAIL_TOUCHBAR_GetState( touchbar ) != ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR )
      {
        ZUKAN_DETAIL_TOUCHBAR_SetType(
            touchbar,
            ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL,
            ZUKAN_DETAIL_TOUCHBAR_DISP_INFO );
        ZUKAN_DETAIL_TOUCHBAR_Appear( touchbar, ZUKAN_DETAIL_TOUCHBAR_SPEED_OUTSIDE );
      }
      else
      {
        ZUKAN_DETAIL_TOUCHBAR_SetDispOfGeneral(
            touchbar,
            ZUKAN_DETAIL_TOUCHBAR_DISP_INFO );
      }
      ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, FALSE );  // ZUKAN_DETAIL_TOUCHBAR_SetType�̂Ƃ���Unlock��ԂȂ̂�
      {
        GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
        ZUKAN_DETAIL_TOUCHBAR_SetCheckFlipOfGeneral(
            touchbar,
            GAMEDATA_GetShortCut( gamedata, SHORTCUT_ID_ZUKAN_INFO ) );
      }
      // �^�C�g���o�[
      if( ZUKAN_DETAIL_HEADBAR_GetState( headbar ) != ZUKAN_DETAIL_HEADBAR_STATE_APPEAR )
      {
        ZUKAN_DETAIL_HEADBAR_SetType(
            headbar,
            ZUKAN_DETAIL_HEADBAR_TYPE_INFO );
        ZUKAN_DETAIL_HEADBAR_Appear( headbar );
      }
    }

      // ����{�^��
      Zukan_Detail_Info_CreateLangButton( param, work, cmn );

      // ��ʐ���
      {
        ZUKAN_SAVEDATA* zkn_sv = GAMEDATA_GetZukanSave( ZKNDTL_COMMON_GetGamedata(cmn) );
        GFL_CLUNIT*     clunit = ZUKAN_DETAIL_GRAPHIC_GetClunit( ZKNDTL_COMMON_GetGraphic(cmn) );
        
        BOOL b_zenkoku_flag = ZUKANSAVE_GetZukanMode( zkn_sv );  // TRUE�̂Ƃ��S���}��
        
        u16  monsno;
        u16  formno;
        u16  sex;
        u16  rare;
        u32  personal_rnd;
        BOOL b_get_flag;
        BOOL lang_exist[ZUKAN_INFO_LANG_MAX];

        // ���ݕ\������|�P�����̏��𓾂�
        Zukan_Detail_Info_GetCurrPokeInfo(
                param, work, cmn,
                &monsno,
                &formno,
                &sex,
                &rare,
                &personal_rnd,
                &b_get_flag,
                lang_exist );
        
        // �}�ӏ��
        {
          work->info_wk_m = ZUKAN_INFO_InitFromMonsno(
                        param->heap_id,
                        monsno,
                        formno,
                        sex,
                        rare,
                        personal_rnd,
                        b_zenkoku_flag,
                        b_get_flag,
                        ZUKAN_INFO_LAUNCH_LIST,
                        ZUKAN_INFO_DISP_M,
                        1,
                        clunit,
                        ZKNDTL_COMMON_GetFont(cmn),
                        ZKNDTL_COMMON_GetPrintQue(cmn) );

          work->info_wk_s = ZUKAN_INFO_InitFromMonsno(
                        param->heap_id,
                        monsno,
                        formno,
                        sex,
                        rare,
                        personal_rnd,
                        b_zenkoku_flag,
                        b_get_flag,
                        ZUKAN_INFO_LAUNCH_LIST,
                        ZUKAN_INFO_DISP_S,
                        1,
                        clunit,
                        ZKNDTL_COMMON_GetFont(cmn),
                        ZKNDTL_COMMON_GetPrintQue(cmn) );
        }
     
        // �����ɉ����\�����Ȃ��Ƃ��p�̔w�i�݂̂�\������
        ZUKAN_INFO_DisplayBackNone( work->info_wk_s );

        // ����{�^���̕\����\���؂�ւ�
        {
          Zukan_Detail_Info_SetupLangButtonDrawEnable( param, work, cmn,
              lang_exist[0], lang_exist[1], lang_exist[2], lang_exist[3], lang_exist[4], lang_exist[5] );
        }

        // ����{�^���̃p���b�g�A�j��
        Zukan_Detail_Info_AnimeBaseInitLang( param, work, cmn );
        Zukan_Detail_Info_AnimeInitLang( param, work, cmn );
      }
    }
    break;
  case SEQ_PREPARE:
    {
      *seq = SEQ_FADE_IN;

      // �t�F�[�h
      ZKNDTL_COMMON_FadeSetBlackToColorless( work->fade_wk_m );
      ZKNDTL_COMMON_FadeSetBlackToColorless( work->fade_wk_s );  // �����\�����Ȃ��Ƃ��p�̔w�i�݂̂�\�����郂�[�h��p�ӂ����̂�

/*
�^�b�`�o�[�œǂݍ��񂾃p���b�g�𗬗p����̂ŁA�����ł͓ǂݍ��܂Ȃ�
�Ƃ����̂�����̂ŁA�^�b�`�o�[���ɐ������Ă����B
      // �^�b�`�o�[
      if( ZUKAN_DETAIL_TOUCHBAR_GetState( touchbar ) != ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR )
      {
        ZUKAN_DETAIL_TOUCHBAR_SetType(
            touchbar,
            ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL,
            ZUKAN_DETAIL_TOUCHBAR_DISP_INFO );
        ZUKAN_DETAIL_TOUCHBAR_Appear( touchbar, ZUKAN_DETAIL_TOUCHBAR_SPEED_OUTSIDE );
      }
      else
      {
        ZUKAN_DETAIL_TOUCHBAR_SetDispOfGeneral(
            touchbar,
            ZUKAN_DETAIL_TOUCHBAR_DISP_INFO );
      }
      ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, FALSE );  // ZUKAN_DETAIL_TOUCHBAR_SetType�̂Ƃ���Unlock��ԂȂ̂�
      {
        GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
        ZUKAN_DETAIL_TOUCHBAR_SetCheckFlipOfGeneral(
            touchbar,
            GAMEDATA_GetShortCut( gamedata, SHORTCUT_ID_ZUKAN_INFO ) );
      }
      // �^�C�g���o�[
      if( ZUKAN_DETAIL_HEADBAR_GetState( headbar ) != ZUKAN_DETAIL_HEADBAR_STATE_APPEAR )
      {
        ZUKAN_DETAIL_HEADBAR_SetType(
            headbar,
            ZUKAN_DETAIL_HEADBAR_TYPE_INFO );
        ZUKAN_DETAIL_HEADBAR_Appear( headbar );
      }
*/

    }
    break;
  case SEQ_FADE_IN:
    {
      if(    (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_m ))
          && ZUKAN_DETAIL_TOUCHBAR_GetState( touchbar ) == ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR
          && ZUKAN_DETAIL_HEADBAR_GetState( headbar ) == ZUKAN_DETAIL_HEADBAR_STATE_APPEAR )
      {
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
        ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, TRUE );  // ZUKAN_DETAIL_TOUCHBAR_SetType�̂Ƃ���Unlock��ԂȂ̂�

        *seq = SEQ_MAIN;
      }
    }
    break;
  case SEQ_MAIN:
    {
      if( work->end_cmd != END_CMD_NONE )
      {
        *seq = SEQ_FADE_OUT;

        // �t�F�[�h
        {
          ZKNDTL_COMMON_FadeSetColorlessToBlack( work->fade_wk_m );

          //if( ZKNDTL_COMMON_FadeIsColorless( work->fade_wk_s ) )
          //{
          //  ZKNDTL_COMMON_FadeSetColorlessToBlack( work->fade_wk_s );
          //}
          ZKNDTL_COMMON_FadeSetColorlessToBlack( work->fade_wk_s );  // �����\�����Ȃ��Ƃ��p�̔w�i�݂̂�\�����郂�[�h��p�ӂ����̂�
        } 
        
        // �^�C�g���o�[
        ZUKAN_DETAIL_HEADBAR_Disappear( headbar );
        
        if( work->end_cmd == END_CMD_OUTSIDE )
        {
          // �^�b�`�o�[
          ZUKAN_DETAIL_TOUCHBAR_Disappear( touchbar, ZUKAN_DETAIL_TOUCHBAR_SPEED_OUTSIDE );
        }
      }
      else
      {
        if( work->input_enable )
        {
          // ����{�^���̃^�b�`����
          BOOL is_input = Zukan_Detail_Info_TouchLangButton( param, work, cmn ); 
        
          // ����{�^���̃L�[����
          if( !is_input )
          {
            Zukan_Detail_Info_KeyLangButton( param, work, cmn );
          }
        }
      }
    }
    break;
  case SEQ_FADE_OUT:
    {
      if(    (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_m ))
          && (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_s ))
          && ZUKAN_DETAIL_HEADBAR_GetState( headbar ) == ZUKAN_DETAIL_HEADBAR_STATE_DISAPPEAR )
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
      return ZKNDTL_PROC_RES_FINISH;
    }
    break;
  }

  if( *seq >= SEQ_FADE_IN )
  {
    // ����{�^��
    Zukan_Detail_Info_MainLangButton( param, work, cmn );
   
    // �}�ӏ��
    ZUKAN_INFO_Main( work->info_wk_m );
    ZUKAN_INFO_Main( work->info_wk_s );

    // ����{�^���̃p���b�g�A�j��
    if( work->lang_btn_push == ZUKAN_INFO_LANG_NONE )  // �����A�j�����Ă���Ƃ��́A�p���b�g�A�j�������Ȃ��悤��
    {
      Zukan_Detail_Info_AnimeMainLang( param, work, cmn );
    }
  }

  // �t�F�[�h
  ZKNDTL_COMMON_FadeMain( work->fade_wk_m, work->fade_wk_s );

  return ZKNDTL_PROC_RES_CONTINUE;
}

//-------------------------------------
/// PROC ���ߏ���
//=====================================
static void Zukan_Detail_Info_CommandFunc( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn, ZKNDTL_COMMAND cmd )
{
  if( mywk )
  {
    ZUKAN_DETAIL_INFO_PARAM*    param    = (ZUKAN_DETAIL_INFO_PARAM*)pwk;
    ZUKAN_DETAIL_INFO_WORK*     work     = (ZUKAN_DETAIL_INFO_WORK*)mywk;

    ZUKAN_DETAIL_TOUCHBAR_WORK* touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
 
    BOOL b_valid_cmd = FALSE;  // cmd��ZKNDTL_CMD_NONE�ȊO��ZKNDTL_CMD_???�̂Ƃ�TRUE�BZKNDTL_CMD_NONE��ZKNDTL_SCMD_???�̂Ƃ�FALSE�B

    // ���͕s��
    switch( cmd )
    {
    case ZKNDTL_SCMD_CLOSE:
    case ZKNDTL_SCMD_RETURN:
    case ZKNDTL_SCMD_MAP:
    case ZKNDTL_SCMD_VOICE:
    case ZKNDTL_SCMD_FORM:
    case ZKNDTL_SCMD_CUR_D:
    case ZKNDTL_SCMD_CUR_U:
    case ZKNDTL_SCMD_CHECK:
      {
        work->input_enable = FALSE;
      }
      break;
    }
    // ���͉�
    switch( cmd )
    {
    case ZKNDTL_CMD_CLOSE:
    case ZKNDTL_CMD_RETURN:
    case ZKNDTL_CMD_MAP:
    case ZKNDTL_CMD_VOICE:
    case ZKNDTL_CMD_FORM:
    case ZKNDTL_CMD_CUR_D:
    case ZKNDTL_CMD_CUR_U:
    case ZKNDTL_CMD_CHECK:
      {
        work->input_enable = TRUE;
        b_valid_cmd = TRUE;
      }
      break;
    }

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
    case ZKNDTL_CMD_MAP:
    case ZKNDTL_CMD_VOICE:
    case ZKNDTL_CMD_FORM:
      {
        work->end_cmd = END_CMD_INSIDE;
      }
      break;
    case ZKNDTL_CMD_CUR_D:
      {
        u16 monsno_curr;
        u16 monsno_go;
        monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
        ZKNDTL_COMMON_GoToNextPoke(cmn);
        monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
        if( monsno_curr != monsno_go )
        {
          Zukan_Detail_Info_ChangePoke(param, work, cmn);
        }
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_CUR_U:
      {
        u16 monsno_curr;
        u16 monsno_go;
        monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
        ZKNDTL_COMMON_GoToPrevPoke(cmn);
        monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
        if( monsno_curr != monsno_go )
        {
          Zukan_Detail_Info_ChangePoke(param, work, cmn);
        }
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_CHECK:
      {
        GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
        GAMEDATA_SetShortCut(
            gamedata,
            SHORTCUT_ID_ZUKAN_INFO,
            ZUKAN_DETAIL_TOUCHBAR_GetCheckFlipOfGeneral( touchbar ) );
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    default:
      {
        if( b_valid_cmd )
        {
          ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
        }
      }
      break;
    }
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
static void Zukan_Detail_Info_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  ZUKAN_DETAIL_INFO_WORK*     work     = (ZUKAN_DETAIL_INFO_WORK*)wk;
}

//-------------------------------------
/// ����{�^���̃^�b�`����
//=====================================
static BOOL Zukan_Detail_Info_TouchLangButton( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_TOUCHBAR_WORK* touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
  BOOL is_input = FALSE;

  if( work->lang_btn_push == ZUKAN_INFO_LANG_NONE )
  {
    u32 x, y;

    // �^�b�`����
    if( GFL_UI_TP_GetPointTrg(&x, &y) )
    {
      u8 i;
      for( i=0; i<ZUKAN_INFO_LANG_MAX; i++ )
      {
        if( GFL_CLACT_WK_GetDrawEnable( work->lang_btn[i].clwk ) )
        {
          if(    work->lang_btn[i].x <= x && x < work->lang_btn[i].x + work->lang_btn[i].w
              && work->lang_btn[i].y <= y && y < work->lang_btn[i].y + work->lang_btn[i].h )
          {
            if( work->lang != i )
            {
              work->lang_btn_push = i;
              is_input = TRUE;
            }
            break;
          }
        }
      }
    }
  }

  if( is_input )
  {
    // �����ύX����
    Zukan_Detail_Info_ChangeLang( param, work, cmn, work->lang_btn_push );
    
    work->lang_btn[work->lang_btn_push].state = LANG_BTN_STATE_PUSH_START;
    GFL_CLACT_WK_SetAnmSeq( work->lang_btn[work->lang_btn_push].clwk, work->lang_btn[work->lang_btn_push].push_anmseq );
    PMSND_PlaySE( SEQ_SE_SELECT3 );
    GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
    ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, FALSE );
  }

  return is_input;
}

//-------------------------------------
/// ����{�^���̃L�[����
//=====================================
static BOOL Zukan_Detail_Info_KeyLangButton( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_TOUCHBAR_WORK* touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
  BOOL is_input = FALSE;
  
  if( work->lang_btn_push == ZUKAN_INFO_LANG_NONE )
  {
    u8 i;

    // �L�[����
    int rept = GFL_UI_KEY_GetRepeat();
    int trg  = GFL_UI_KEY_GetTrg();
 
    if( work->lang == ZUKAN_INFO_LANG_NONE )  // �܂��ǂ���I��ł��Ȃ��Ƃ�
    {
      if(    ( rept & ( PAD_BUTTON_R | PAD_BUTTON_L ) )
          || ( trg & PAD_BUTTON_A ) )
      {
        for( i=0; i<ZUKAN_INFO_LANG_MAX; i++ )  // ��ԍ��ɂ��錾��{�^����I��
        {
          if( GFL_CLACT_WK_GetDrawEnable( work->lang_btn[i].clwk ) )
          {
            work->lang_btn_push = i;
            is_input = TRUE;
            break;
          }
        }
      }
    }
    else  // ���ɂǂꂩ��I��ł���Ƃ�
    {
      if(    ( rept & PAD_BUTTON_R )
          || ( trg & PAD_BUTTON_A ) )
      {
        i = 1;
        while( i < ZUKAN_INFO_LANG_MAX )
        {
          u8 j = ( work->lang + i ) % ZUKAN_INFO_LANG_MAX;
          if( GFL_CLACT_WK_GetDrawEnable( work->lang_btn[j].clwk ) )
          {
            work->lang_btn_push = j;
            is_input = TRUE;
            break;
          }
          i++;
        }
      }
      else if( rept & PAD_BUTTON_L )
      {
        i = 1;
        while( i < ZUKAN_INFO_LANG_MAX )
        {
          u8 j = ( work->lang + ZUKAN_INFO_LANG_MAX - i ) % ZUKAN_INFO_LANG_MAX;
          if( GFL_CLACT_WK_GetDrawEnable( work->lang_btn[j].clwk ) )
          {
            work->lang_btn_push = j;
            is_input = TRUE;
            break;
          }
          i++;
        }
      }
    }
  }

  if( is_input )
  {
    // �����ύX����
    Zukan_Detail_Info_ChangeLang( param, work, cmn, work->lang_btn_push );
    
    work->lang_btn[work->lang_btn_push].state = LANG_BTN_STATE_PUSH_START;
    GFL_CLACT_WK_SetAnmSeq( work->lang_btn[work->lang_btn_push].clwk, work->lang_btn[work->lang_btn_push].push_anmseq );
    PMSND_PlaySE( SEQ_SE_SELECT3 );
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, FALSE );
  }
  
  return is_input;
}

//-------------------------------------
/// �|�P������ύX����
//=====================================
static void Zukan_Detail_Info_ChangePoke( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u16  monsno;
  u16  formno;
  u16  sex;
  u16  rare;
  u32  personal_rnd;
  BOOL b_get_flag;
  BOOL lang_exist[ZUKAN_INFO_LANG_MAX];

  // ���ݕ\������|�P�����̏��𓾂�
  Zukan_Detail_Info_GetCurrPokeInfo(
                param, work, cmn,
                &monsno,
                &formno,
                &sex,
                &rare,
                &personal_rnd,
                &b_get_flag,
                lang_exist );

  // �}�ӏ��
  {
    BOOL b_sub_off = TRUE;
    ZUKAN_INFO_ChangePoke( work->info_wk_m, monsno, formno, sex, rare, personal_rnd, b_get_flag );

    if( work->lang != ZUKAN_INFO_LANG_NONE )
    {
      if( lang_exist[work->lang] )
      {
        b_sub_off = FALSE;
      }
      else
      {
        // ����{�^���̃p���b�g�A�j��
        Zukan_Detail_Info_AnimeExitLang( param, work, cmn );

        work->lang = ZUKAN_INFO_LANG_NONE;
      }
    }

    if( b_sub_off )
    {
      ZUKAN_INFO_ChangePoke( work->info_wk_s, monsno, formno, sex, rare, personal_rnd, b_get_flag );

      //// �����ɍ��ɂ���
      //ZKNDTL_COMMON_FadeSetBlackImmediately( ZKNDTL_COMMON_FADE_DISP_S, work->fade_wk_s );
      // �����ɉ����\�����Ȃ��Ƃ��p�̔w�i�݂̂�\������
      ZUKAN_INFO_DisplayBackNone( work->info_wk_s );
    }
    else
    {
      ZUKAN_INFO_ChangePokeAndLang( work->info_wk_s, monsno, formno, sex, rare, personal_rnd, b_get_flag, work->lang );
    }
  }

  // ����{�^���̕\����\���؂�ւ�
  {
    Zukan_Detail_Info_SetupLangButtonDrawEnable( param, work, cmn,
        lang_exist[0], lang_exist[1], lang_exist[2], lang_exist[3], lang_exist[4], lang_exist[5] );
  }
}

//-------------------------------------
/// �����ύX����
//=====================================
static void Zukan_Detail_Info_ChangeLang( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn,
           ZUKAN_INFO_LANG lang)
{
  if( work->lang == lang ) return;
 
  ZUKAN_INFO_ChangeLang( work->info_wk_s, lang );

  if( work->lang == ZUKAN_INFO_LANG_NONE )
  {
    //// �����Ɍ�����悤�ɂ���
    //ZKNDTL_COMMON_FadeSetColorlessImmediately( ZKNDTL_COMMON_FADE_DISP_S, work->fade_wk_s );
    // �����ɒʏ�̕\���ɂ���
    ZUKAN_INFO_DisplayNormal( work->info_wk_s );
  }

  // ����{�^���̃p���b�g�A�j��
  Zukan_Detail_Info_AnimeExitLang( param, work, cmn );

  work->lang = lang;
  
  // ����{�^���̃p���b�g�A�j��
  Zukan_Detail_Info_AnimeInitLang( param, work, cmn );
}

//-------------------------------------
/// ����{�^��
//=====================================
static void Zukan_Detail_Info_CreateLangButton( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_TOUCHBAR_WORK* touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);

  ZUKAN_DETAIL_GRAPHIC_WORK* graphic = ZKNDTL_COMMON_GetGraphic(cmn);
  GFL_CLUNIT* clunit = ZUKAN_DETAIL_GRAPHIC_GetClunit(graphic);

  // ���C�� or �T�u
  CLSYS_DRAW_TYPE draw_type = CLSYS_DRAW_MAIN;
  CLSYS_DEFREND_TYPE def_type = CLSYS_DEFREND_MAIN;

  // ���\�[�X�ǂݍ���
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, param->heap_id );
/*
�^�b�`�o�[�œǂݍ��񂾃p���b�g�𗬗p����̂ŁA�����ł͓ǂݍ��܂Ȃ�
    work->lang_ncl = GFL_CLGRP_PLTT_RegisterEx(
                            handle,
                            NARC_zukan_gra_info_info_obj_NCLR,
                            draw_type,
                            OBJ_PAL_POS_M_LANG_BTN * 0x20, 0, OBJ_PAL_NUM_M_LANG_BTN,
                            param->heap_id );
*/
    work->lang_ncl = ZUKAN_DETAIL_TOUCHBAR_GetCustomIconPlttRegIdx( touchbar );  // �^�b�`�o�[�œǂݍ��񂾃p���b�g�𗬗p����̂ŁA�����ł͓ǂݍ��܂Ȃ�
    work->lang_ncg = GFL_CLGRP_CGR_Register(
                            handle,
                            NARC_zukan_gra_info_info_obj_NCGR,
                            FALSE,
                            draw_type,
                            param->heap_id );
    work->lang_nce = GFL_CLGRP_CELLANIM_Register(
                            handle,
                            NARC_zukan_gra_info_info_obj_NCER,
                            NARC_zukan_gra_info_info_obj_NANR,
                            param->heap_id );
    GFL_ARC_CloseDataHandle( handle );
  }

  // CLWK�쐬
  {
    u8 i;
    for( i=0; i<ZUKAN_INFO_LANG_MAX; i++ )
    {
      GFL_CLWK_DATA   cldata;
		  GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
      
      work->lang_btn[i].x             = lang_button_rect[i][2];
      work->lang_btn[i].y             = lang_button_rect[i][3];
      work->lang_btn[i].w             = lang_button_rect[i][4];
      work->lang_btn[i].h             = lang_button_rect[i][5];
      work->lang_btn[i].active_anmseq = lang_button_rect[i][6];
      work->lang_btn[i].push_anmseq   = lang_button_rect[i][7];
      work->lang_btn[i].lang_code     = lang_button_rect[i][8];
      work->lang_btn[i].state         = LANG_BTN_STATE_ACTIVE;

      cldata.pos_x = lang_button_rect[i][0];
      cldata.pos_y = lang_button_rect[i][1];
      cldata.anmseq = work->lang_btn[i].active_anmseq;

		  work->lang_btn[i].clwk = GFL_CLACT_WK_Create( clunit,
                             work->lang_ncg, work->lang_ncl, work->lang_nce,
                             &cldata, def_type, param->heap_id );
		  GFL_CLACT_WK_SetAutoAnmFlag( work->lang_btn[i].clwk, TRUE );
      GFL_CLACT_WK_SetObjMode( work->lang_btn[i].clwk, GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
    }
  }
}
static void Zukan_Detail_Info_DeleteLangButton( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
	// CLWK�j��
  {
    u8 i;
    for( i=0; i<ZUKAN_INFO_LANG_MAX; i++ )
    {
	    GFL_CLACT_WK_Remove( work->lang_btn[i].clwk );
    }
  }

  // ���\�[�X�j��
  {
/*
�^�b�`�o�[�œǂݍ��񂾃p���b�g�𗬗p����̂ŁA�����ł͓ǂݍ��܂Ȃ�
    GFL_CLGRP_PLTT_Release( work->lang_ncl );
*/
    GFL_CLGRP_CGR_Release( work->lang_ncg );
    GFL_CLGRP_CELLANIM_Release( work->lang_nce );
  }
}
static void Zukan_Detail_Info_MainLangButton( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_TOUCHBAR_WORK* touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
  
  if( work->lang_btn_push != ZUKAN_INFO_LANG_NONE )
  {
    u8 i = work->lang_btn_push;

    switch( work->lang_btn[i].state )
    {
    case LANG_BTN_STATE_ACTIVE:
      {
        // �������Ȃ�
      }
      break;
    case LANG_BTN_STATE_PUSH_START:  // ���̊֐��̌Ăяo���ʒu�ɂ���ẮA���̏�Ԃɂ�1�t���[�����Ȃ��Ă��Ȃ������B
      {
        work->lang_btn[i].state = LANG_BTN_STATE_PUSH_ANIME;
      }
      break;
    case LANG_BTN_STATE_PUSH_ANIME:
      {
        if( !GFL_CLACT_WK_CheckAnmActive( work->lang_btn[i].clwk ) )
        {
          work->lang_btn[i].state = LANG_BTN_STATE_PUSH_END;
        } 
      }
      break;
    case LANG_BTN_STATE_PUSH_END:
      {
        GFL_CLACT_WK_SetAnmSeq( work->lang_btn[i].clwk, work->lang_btn[i].active_anmseq );
        work->lang_btn[i].state = LANG_BTN_STATE_ACTIVE;

        work->lang_btn_push = ZUKAN_INFO_LANG_NONE;
        ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, TRUE );
      }
      break;
    }
  }
}
static void Zukan_Detail_Info_SetupLangButtonDrawEnable( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                BOOL e, BOOL fra, BOOL ger, BOOL ita, BOOL spa, BOOL kor )
{
  u8 i;

  GFL_CLACT_WK_SetDrawEnable( work->lang_btn[ZUKAN_INFO_LANG_E].clwk,   e   );
  GFL_CLACT_WK_SetDrawEnable( work->lang_btn[ZUKAN_INFO_LANG_FRA].clwk, fra );
  GFL_CLACT_WK_SetDrawEnable( work->lang_btn[ZUKAN_INFO_LANG_GER].clwk, ger );
  GFL_CLACT_WK_SetDrawEnable( work->lang_btn[ZUKAN_INFO_LANG_ITA].clwk, ita );
  GFL_CLACT_WK_SetDrawEnable( work->lang_btn[ZUKAN_INFO_LANG_SPA].clwk, spa );
  GFL_CLACT_WK_SetDrawEnable( work->lang_btn[ZUKAN_INFO_LANG_KOR].clwk, kor );

  for( i=0; i<ZUKAN_INFO_LANG_MAX; i++ )
  {
    work->lang_btn[i].state         = LANG_BTN_STATE_ACTIVE;
    GFL_CLACT_WK_SetAnmSeq( work->lang_btn[i].clwk, work->lang_btn[i].active_anmseq );
  }
  
  work->lang_btn_push = ZUKAN_INFO_LANG_NONE;
}

//-------------------------------------
/// ���ݕ\������|�P�����̏��𓾂�
//=====================================
static void Zukan_Detail_Info_GetCurrPokeInfo(
                ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                u16*  a_monsno,
                u16*  a_formno,
                u16*  a_sex,
                u16*  a_rare,
                u32*  a_personal_rnd,
                BOOL* a_b_get_flag,
                BOOL* a_lang_exist )  // a_lang_exist[ZUKAN_INFO_LANG_MAX]
{
  ZUKAN_SAVEDATA* zkn_sv = GAMEDATA_GetZukanSave( ZKNDTL_COMMON_GetGamedata(cmn) );

  u16  monsno = ZKNDTL_COMMON_GetCurrPoke(cmn);

  u32  formno;
  u32  sex;
  BOOL rare;
  u32  personal_rnd = 0;

  BOOL b_get_flag;
  BOOL lang_exist[ZUKAN_INFO_LANG_MAX];

  // ���ݕ\������p
  ZUKANSAVE_GetDrawData(  zkn_sv, monsno, &sex, &rare, &formno, param->heap_id );
  if( monsno == MONSNO_PATTIIRU )  // personal_rnd��monsno==MONSNO_PATTIIRU�̂Ƃ��̂ݎg�p����
  {
    personal_rnd = ZUKANSAVE_GetPokeRandomFlag( zkn_sv, ZUKANSAVE_RANDOM_PACHI );
  }

  b_get_flag = ZUKANSAVE_GetPokeGetFlag( zkn_sv, monsno );

  {
    u8 i;
    for( i=0; i<ZUKAN_INFO_LANG_MAX; i++ )
    {
      lang_exist[i] = ZUKANSAVE_GetTextVersionUpFlag( zkn_sv, monsno, work->lang_btn[i].lang_code );  // �J���g���[�R�[�h��pm_version.h��LANG_JAPAN, LANG_ENGLISH�Ȃǂ̌���R�[�h�w��̒l
    }
  }
 
#if 0 
  // �S����`�F�b�N�p
  {
    u8 i;
    for( i=0; i<ZUKAN_INFO_LANG_MAX; i++ )
    {
      lang_exist[i] = TRUE;
    }
  }
#endif

  *a_monsno        = monsno;
  *a_formno        = (u16)formno;
  *a_sex           = (u16)sex;
  *a_rare          = (u16)rare;
  *a_personal_rnd  = personal_rnd;
  *a_b_get_flag      = b_get_flag;
  {
    u8 i;
    for( i=0; i<ZUKAN_INFO_LANG_MAX; i++ )
    {
      a_lang_exist[i] = lang_exist[i];
    }
  }
}

//-------------------------------------
/// ����{�^���̃p���b�g�A�j��
//=====================================
static void Zukan_Detail_Info_AnimeBaseInitLang( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  NNSG2dPaletteData* pal_data;
  void* buf = GFL_ARC_UTIL_LoadPalette( ARCID_ZUKAN_GRA, NARC_zukan_gra_info_info_obj_NCLR, &pal_data, param->heap_id );
  u16* raw_data = pal_data->pRawData;
  //GFL_STD_MemCopy( &raw_data[OBJ_PAL_OFFSET_LANG_ANIME_NONE*0x10], work->pal_anime_lang_default, 0x20 );
  GFL_STD_MemCopy( &raw_data[RES_PAL_POS_LANG_ANIME_START*0x10], work->pal_anime_lang_start, 0x20 );
  GFL_STD_MemCopy( &raw_data[RES_PAL_POS_LANG_ANIME_END*0x10], work->pal_anime_lang_end, 0x20 );
  GFL_HEAP_FreeMemory( buf );
  work->pal_anime_lang_count = 0;

  // ������
  {
    GFL_STD_MemCopy( work->pal_anime_lang_start, work->pal_anime_lang_now, 0x20 );

    NNS_GfdRegisterNewVramTransferTask(
        NNS_GFD_DST_2D_OBJ_PLTT_MAIN,
/*
�^�b�`�o�[�œǂݍ��񂾃p���b�g�𗬗p����̂ŁA�����ł͓ǂݍ��܂Ȃ�
        ( OBJ_PAL_POS_M_LANG_BTN + OBJ_PAL_OFFSET_LANG_ANIME_EXEC ) * 0x20,
*/
        OBJ_PAL_POS_M_LANG_ANIME_EXEC * 0x20,
        work->pal_anime_lang_now,
        0x20 );
  }
}
static void Zukan_Detail_Info_AnimeBaseExitLang( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �������Ȃ�
}
static void Zukan_Detail_Info_AnimeInitLang( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  if( work->lang != ZUKAN_INFO_LANG_NONE )
  {
    // pal_anime_lang_default�����蓖�Ă�Ɖ����A�j�����Ȃ��Ȃ��Ă��܂����̂ŁApal_anime_lang_pltt_offs_req���g�����Ƃɂ����B
    /*
    {
      // �����A�j�����Ă���Ƃ��́Adefault�̐F�ɂ��Ă���
      GFL_STD_MemCopy( work->pal_anime_lang_default, work->pal_anime_lang_now, 0x20 );
      NNS_GfdRegisterNewVramTransferTask(
          NNS_GFD_DST_2D_OBJ_PLTT_MAIN,
          ( OBJ_PAL_POS_M_LANG_BTN + OBJ_PAL_OFFSET_LANG_ANIME_EXEC ) * 0x20,
          work->pal_anime_lang_now,
          0x20 );
    }
    */
    //GFL_CLACT_WK_SetPlttOffs( work->lang_btn[work->lang].clwk, OBJ_PAL_OFFSET_LANG_ANIME_EXEC, CLWK_PLTTOFFS_MODE_OAM_COLOR );
    work->pal_anime_lang_count = 0;
    work->pal_anime_lang_pltt_offs_req = TRUE;  // �����A�j�����Ă���Ƃ��́A�p���b�g�A�j�������Ȃ��悤�ɁA�p���b�g�A�j���̊J�n��x�点��
  }
}
static void Zukan_Detail_Info_AnimeExitLang( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  if( work->lang != ZUKAN_INFO_LANG_NONE )
  {
/*
�^�b�`�o�[�œǂݍ��񂾃p���b�g�𗬗p����̂ŁA�����ł͓ǂݍ��܂Ȃ� 
    GFL_CLACT_WK_SetPlttOffs( work->lang_btn[work->lang].clwk, OBJ_PAL_OFFSET_LANG_ANIME_NONE, CLWK_PLTTOFFS_MODE_OAM_COLOR );
*/
    //GFL_CLACT_WK_SetPlttOffs( work->lang_btn[work->lang].clwk, OBJ_PAL_POS_M_LANG_ANIME_NONE, CLWK_PLTTOFFS_MODE_PLTT_TOP );�p���b�g�𒼂ŕς�����Ăł��Ȃ����ۂ�
    GFL_CLACT_WK_SetPlttOffs( work->lang_btn[work->lang].clwk, OBJ_PAL_OFFSET_LANG_ANIME_NONE, CLWK_PLTTOFFS_MODE_OAM_COLOR );
    work->pal_anime_lang_pltt_offs_req = FALSE;
  }
}
static void Zukan_Detail_Info_AnimeMainLang( ZUKAN_DETAIL_INFO_PARAM* param, ZUKAN_DETAIL_INFO_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 i;
  fx16 cos;

  if( work->pal_anime_lang_pltt_offs_req )  // �����A�j�����Ă���Ƃ��́A�p���b�g�A�j�������Ȃ��悤�ɁA�p���b�g�A�j���̊J�n��x�点��
  {
    if( work->lang != ZUKAN_INFO_LANG_NONE )
    {
/*
�^�b�`�o�[�œǂݍ��񂾃p���b�g�𗬗p����̂ŁA�����ł͓ǂݍ��܂Ȃ� 
      GFL_CLACT_WK_SetPlttOffs( work->lang_btn[work->lang].clwk, OBJ_PAL_OFFSET_LANG_ANIME_EXEC, CLWK_PLTTOFFS_MODE_OAM_COLOR );
*/
      //GFL_CLACT_WK_SetPlttOffs( work->lang_btn[work->lang].clwk, OBJ_PAL_POS_M_LANG_ANIME_EXEC, CLWK_PLTTOFFS_MODE_PLTT_TOP );�p���b�g�𒼂ŕς�����Ăł��Ȃ����ۂ�
      GFL_CLACT_WK_SetPlttOffs( work->lang_btn[work->lang].clwk, OBJ_PAL_OFFSET_LANG_ANIME_EXEC, CLWK_PLTTOFFS_MODE_OAM_COLOR );
    }
    work->pal_anime_lang_pltt_offs_req = FALSE;
  }

  if( work->pal_anime_lang_count + LANG_ANIME_ADD >= 0x10000 )
  {
    work->pal_anime_lang_count = work->pal_anime_lang_count + LANG_ANIME_ADD - 0x10000;
  }
  else
  {
    work->pal_anime_lang_count = work->pal_anime_lang_count + LANG_ANIME_ADD;
  }
  cos = ( FX_CosIdx( work->pal_anime_lang_count ) + FX16_ONE ) / 2;  // 0<= <=1�ɂ��Ă���

  for( i=0; i<0x10; i++ )
  {
    u8 s_r = ( work->pal_anime_lang_start[i] & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    u8 s_g = ( work->pal_anime_lang_start[i] & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    u8 s_b = ( work->pal_anime_lang_start[i] & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
    u8 e_r = ( work->pal_anime_lang_end[i]   & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    u8 e_g = ( work->pal_anime_lang_end[i]   & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    u8 e_b = ( work->pal_anime_lang_end[i]   & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

    u8 r = s_r + (((e_r-s_r)*cos)>>FX16_SHIFT);
    u8 g = s_g + (((e_g-s_g)*cos)>>FX16_SHIFT);
    u8 b = s_b + (((e_b-s_b)*cos)>>FX16_SHIFT);

    work->pal_anime_lang_now[i] = GX_RGB( r, g, b );
  }

  {
    NNS_GfdRegisterNewVramTransferTask(
        NNS_GFD_DST_2D_OBJ_PLTT_MAIN,
/*
�^�b�`�o�[�œǂݍ��񂾃p���b�g�𗬗p����̂ŁA�����ł͓ǂݍ��܂Ȃ�
        ( OBJ_PAL_POS_M_LANG_BTN + OBJ_PAL_OFFSET_LANG_ANIME_EXEC ) * 0x20,
*/
        OBJ_PAL_POS_M_LANG_ANIME_EXEC * 0x20,
        work->pal_anime_lang_now,
        0x20 );
  }
}

