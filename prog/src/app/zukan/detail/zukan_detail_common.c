//============================================================================
/**
 *  @file   zukan_detail_common.c
 *  @brief  �}�ӏڍ׉�ʋ��ʂ̃f�[�^���܂Ƃ߂�����
 *  @author Koji Kawada
 *  @data   2010.02.02
 *  @note   
 *  ���W���[�����FZUKAN_DETAIL_COMMON
 */
//============================================================================
// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "gamesystem/gamedata_def.h"
#include "savedata/zukan_savedata.h"

#include "zukan_detail_def.h"
#include "zukan_detail_common.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "zukan_gra.naix"

// �T�E���h

// �I�[�o�[���C


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
/// ���[�N
//=====================================
struct _ZKNDTL_COMMON_WORK
{
  GAMEDATA*                      gamedata;
  ZUKAN_DETAIL_GRAPHIC_WORK*     graphic;
  GFL_FONT*                      font;
  PRINT_QUE*                     print_que;
  ZUKAN_DETAIL_TOUCHBAR_WORK*    touchbar;
  ZUKAN_DETAIL_HEADBAR_WORK*     headbar;
  u16*                           list;
  u16                            num;
  u16*                           no;
};


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================


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
ZKNDTL_COMMON_WORK* ZKNDTL_COMMON_Init(
                HEAPID                       heap_id,
                GAMEDATA*                    gamedata,
                ZUKAN_DETAIL_GRAPHIC_WORK*   graphic,
                GFL_FONT*                    font,
                PRINT_QUE*                   print_que,
                ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar,
                ZUKAN_DETAIL_HEADBAR_WORK*   headbar,
                u16*                         list,
                u16                          num,
                u16*                         no )
{
  ZKNDTL_COMMON_WORK*   cmn;

  // ���[�N����
  cmn = GFL_HEAP_AllocClearMemory( heap_id, sizeof(ZKNDTL_COMMON_WORK) );

  // �Z�b�g
  cmn->gamedata   = gamedata;
  cmn->graphic    = graphic;
  cmn->font       = font;
  cmn->print_que  = print_que;
  cmn->touchbar   = touchbar;
  cmn->headbar    = headbar;
  cmn->list       = list;
  cmn->num        = num;
  cmn->no         = no;

  return cmn;
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void ZKNDTL_COMMON_Exit( ZKNDTL_COMMON_WORK* cmn )
{
  // ���[�N�j��
  GFL_HEAP_FreeMemory( cmn );
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
GAMEDATA*                    ZKNDTL_COMMON_GetGamedata( ZKNDTL_COMMON_WORK* cmn )
{
  return cmn->gamedata;
}
ZUKAN_DETAIL_GRAPHIC_WORK*   ZKNDTL_COMMON_GetGraphic( ZKNDTL_COMMON_WORK* cmn )
{
  return cmn->graphic;
}
GFL_FONT*                    ZKNDTL_COMMON_GetFont( ZKNDTL_COMMON_WORK* cmn )
{
  return cmn->font;
}
PRINT_QUE*                   ZKNDTL_COMMON_GetPrintQue( ZKNDTL_COMMON_WORK* cmn )
{
  return cmn->print_que;
}
ZUKAN_DETAIL_TOUCHBAR_WORK*  ZKNDTL_COMMON_GetTouchbar( ZKNDTL_COMMON_WORK* cmn )
{
  return cmn->touchbar;
}
ZUKAN_DETAIL_HEADBAR_WORK*   ZKNDTL_COMMON_GetHeadbar( ZKNDTL_COMMON_WORK* cmn )
{
  return cmn->headbar;
}

//------------------------------------------------------------------
/**
 *  @brief        
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
u16   ZKNDTL_COMMON_GetCurrPoke( ZKNDTL_COMMON_WORK* cmn )  // monsno��Ԃ�
{
  return cmn->list[*(cmn->no)];
}
void  ZKNDTL_COMMON_GoToNextPoke( ZKNDTL_COMMON_WORK* cmn )
{
  u16   start_no   = *(cmn->no);
  BOOL  see_flag   = FALSE;

  ZUKAN_SAVEDATA* zukan_savedata = GAMEDATA_GetZukanSave( cmn->gamedata );
  
  do
  {
    *(cmn->no) += 1;
    if( *(cmn->no) >= cmn->num ) *(cmn->no) = 0;

    if( *(cmn->no) == start_no )
    {
      break;
    }
    else
    {
      u16 monsno = cmn->list[*(cmn->no)];
      see_flag = ZUKANSAVE_GetPokeSeeFlag( zukan_savedata, monsno );
    }
  }
  while( !see_flag );
}
void  ZKNDTL_COMMON_GoToPrevPoke( ZKNDTL_COMMON_WORK* cmn )
{
  u16   start_no   = *(cmn->no);
  BOOL  see_flag   = FALSE;

  ZUKAN_SAVEDATA* zukan_savedata = GAMEDATA_GetZukanSave( cmn->gamedata );
 
  do
  {
    if( *(cmn->no) == 0 ) *(cmn->no) = cmn->num -1;
    else                  *(cmn->no) -= 1;
    
    if( *(cmn->no) == start_no )
    {
      break;
    }
    else
    {
      u16 monsno = cmn->list[*(cmn->no)];
      see_flag = ZUKANSAVE_GetPokeSeeFlag( zukan_savedata, monsno );
    }
  }
  while( !see_flag );
}


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// 
//=====================================




// �ꕔ���t�F�[�h
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
struct _ZKNDTL_COMMON_FADE_WORK
{
  int plane1;
  int plane2;
  s32 bright;
  s32 bright_delta;
  s32 bright_wait;
};

//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
static void Zkndtl_Common_FadeImmediately( ZKNDTL_COMMON_FADE_DISP fade_disp, ZKNDTL_COMMON_FADE_WORK* fade_wk );

//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
ZKNDTL_COMMON_FADE_WORK* ZKNDTL_COMMON_FadeInit( HEAPID heap_id )
{
  ZKNDTL_COMMON_FADE_WORK* fade_wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof(ZKNDTL_COMMON_FADE_WORK) );
  return fade_wk;
}
void ZKNDTL_COMMON_FadeExit( ZKNDTL_COMMON_FADE_WORK* fade_wk )
{
  GFL_HEAP_FreeMemory( fade_wk );
}

void ZKNDTL_COMMON_FadeMain( ZKNDTL_COMMON_FADE_WORK* fade_wk_m, ZKNDTL_COMMON_FADE_WORK* fade_wk_s )
{
  // (��)0<=bright<=16(������)                        // bright�Ō��݃t�F�[�h���������Ă���̂��ۂ�����ł���
  
  // bright_delta==0 �t�F�[�h�A�j�����Ă��Ȃ�         // bright_delta�Ō��݃t�F�[�h�A�j�����Ă��邩�ۂ�����ł���
  // bright_delta >0 �t�F�[�h�C��(����������)
  // bright_delta <0 �t�F�[�h�A�E�g(�����遨��)
  
  // bright_wait 0�̂Ƃ����t���[���A�j�����i�ށA4�̂Ƃ��A�j���A�ҁA�ҁA�ҁA�ҁA�A�j���A�ҁA�ҁA�ҁA�ҁA�A�j���ƂȂ�

  u8 i;
  
  ZKNDTL_COMMON_FADE_WORK* fade_wk_tbl[ZKNDTL_COMMON_FADE_DISP_MAX];
  
  fade_wk_tbl[ZKNDTL_COMMON_FADE_DISP_M] = fade_wk_m;
  fade_wk_tbl[ZKNDTL_COMMON_FADE_DISP_S] = fade_wk_s;

  for( i=0; i<ZKNDTL_COMMON_FADE_DISP_MAX; i++ )
  {
    ZKNDTL_COMMON_FADE_WORK* fade_wk = fade_wk_tbl[i];

    if( fade_wk->bright_delta != 0 )
    {
      if( fade_wk->bright_wait == 0 )
      {
        fade_wk->bright_wait = ZKNDTL_COMMON_FADE_BRIGHT_WAIT;
        fade_wk->bright += fade_wk->bright_delta;
        if( fade_wk->bright_delta > 0 )  // �t�F�[�h�C��
        {
          if( fade_wk->bright >= 16 )
          {
            fade_wk->bright = 16;
            fade_wk->bright_delta = 0;
          }
        }
        else                             // �t�F�[�h�A�E�g
        {
          if( fade_wk->bright <= 0 )
          {
            fade_wk->bright = 0;
            fade_wk->bright_delta = 0;
          }
        }

        Zkndtl_Common_FadeImmediately( i, fade_wk );
      }
      else
      {
        fade_wk->bright_wait--;
      }
    }
  }
}

BOOL ZKNDTL_COMMON_FadeIsExecute( ZKNDTL_COMMON_FADE_WORK* fade_wk )
{
  return ( fade_wk->bright_delta != 0 );
}
BOOL ZKNDTL_COMMON_FadeIsColorless( ZKNDTL_COMMON_FADE_WORK* fade_wk )
{
  return ( fade_wk->bright == 16 );
}
BOOL ZKNDTL_COMMON_FadeIsBlack( ZKNDTL_COMMON_FADE_WORK* fade_wk )
{
  return ( fade_wk->bright ==  0 );
}

void ZKNDTL_COMMON_FadeSetBlackToColorless( ZKNDTL_COMMON_FADE_WORK* fade_wk )
{
  // ���ŏ�����
  fade_wk->bright       = 0;
  fade_wk->bright_delta = ZKNDTL_COMMON_FADE_BRIGHT_DELTA;
  fade_wk->bright_wait  = ZKNDTL_COMMON_FADE_BRIGHT_WAIT;
}
void ZKNDTL_COMMON_FadeSetColorlessToBlack( ZKNDTL_COMMON_FADE_WORK* fade_wk )
{
  // ������ŏ�����
  fade_wk->bright       = 16;
  fade_wk->bright_delta = - ZKNDTL_COMMON_FADE_BRIGHT_DELTA;
  fade_wk->bright_wait  = ZKNDTL_COMMON_FADE_BRIGHT_WAIT;
}

void ZKNDTL_COMMON_FadeSetColorlessImmediately( ZKNDTL_COMMON_FADE_DISP fade_disp, ZKNDTL_COMMON_FADE_WORK* fade_wk )
{
  // ������ŏ�����
  fade_wk->bright       = 16;
  fade_wk->bright_delta = 0;
  fade_wk->bright_wait  = 0;
  
  Zkndtl_Common_FadeImmediately( fade_disp, fade_wk );
}
void ZKNDTL_COMMON_FadeSetBlackImmediately( ZKNDTL_COMMON_FADE_DISP fade_disp, ZKNDTL_COMMON_FADE_WORK* fade_wk )
{
  // ���ŏ�����
  fade_wk->bright       = 0;
  fade_wk->bright_delta = 0;
  fade_wk->bright_wait  = 0;
 
  Zkndtl_Common_FadeImmediately( fade_disp, fade_wk );
}

void ZKNDTL_COMMON_FadeSetPlane( ZKNDTL_COMMON_FADE_WORK* fade_wk, int plane1, int plane2 )
{
  fade_wk->plane1 = plane1;
  fade_wk->plane2 = plane2;
}

void ZKNDTL_COMMON_FadeSetPlaneDefault( ZKNDTL_COMMON_FADE_WORK* fade_wk )
{
  fade_wk->plane1 = GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BD;
  fade_wk->plane2 = GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BD;
}


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
static void Zkndtl_Common_FadeImmediately( ZKNDTL_COMMON_FADE_DISP fade_disp, ZKNDTL_COMMON_FADE_WORK* fade_wk )
{
  if( fade_disp == ZKNDTL_COMMON_FADE_DISP_M )
  {
    G2_SetBlendBrightnessExt(  fade_wk->plane1,
                               fade_wk->plane2,
                               fade_wk->bright,
                               0,
                               fade_wk->bright - 16 );
  }
  else
  {
    G2S_SetBlendBrightnessExt( fade_wk->plane1,
                               fade_wk->plane2,
                               fade_wk->bright,
                               0,
                               fade_wk->bright - 16 );
  }
}




// �ꕔ���p���b�g�t�F�[�h
//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
#define PF_TCBSYS_TASK_MAX               (2)
#define PF_MAIN_BG_REQ_BIT               (0x1FFF)  // 0001 1111 1111 1111  // 0<= <13
#define PF_MAIN_OBJ_REQ_BIT              (0x00FF)  // 0000 0000 1111 1111  // 0<= <8
#define PF_SUB_BG_REQ_BIT                (0x1FFF)  // 0001 1111 1111 1111  // 0<= <13
#define PF_SUB_OBJ_REQ_BIT               (0x3FFF)  // 0011 1111 1111 1111  // 0<= <14
#define PF_BLACK_TO_COLORLESS_START_EVY  (16)
#define PF_BLACK_TO_COLORLESS_END_EVY    (0)
#define PF_COLORLESS_TO_BLACK_START_EVY  (0)
#define PF_COLORLESS_TO_BLACK_END_EVY    (16)
#define PF_BLACK_RGB                     (0x0000)

#define PF_MAIN_BG_DATA_SIZE   (ZKNDTL_BG_PAL_POS_M_TOUCHBAR*0x20)   // �o�C�g�P��  // PaletteFadeWorkAllocSet��PaletteWorkSet_VramCopy�ɓn���T�C�Y
#define PF_MAIN_OBJ_DATA_SIZE  (ZKNDTL_OBJ_PAL_POS_M_TOUCHBAR*0x20)  // ZKNDTL_BG_PAL_POS_M_TOUCHBAR����
#define PF_SUB_BG_DATA_SIZE    (ZKNDTL_BG_PAL_POS_S_HEADBAR*0x20)    // �t�F�[�h���Ă͂Ȃ�Ȃ����̂��g�p�Ă���̂ŁA
#define PF_SUB_OBJ_DATA_SIZE   (ZKNDTL_OBJ_PAL_POS_S_HEADBAR*0x20)   // �p���b�g�t�F�[�h���Ă͂Ȃ�Ȃ��B
 
typedef enum
{
  PF_STATE_COLORLESS,           // ������ŗ��������Ă���
  PF_STATE_BLACK,               // ���ŗ��������Ă���
  PF_STATE_BLACK_TO_COLORLESS,  // �p���b�g�t�F�[�h���s���Ȃ̂ŁA���̂Ƃ����߂��Ă����������
  PF_STATE_COLORLESS_TO_BLACK,  // �p���b�g�t�F�[�h���s���Ȃ̂ŁA���̂Ƃ����߂��Ă����������
}
PF_STATE;

//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
struct _ZKNDTL_COMMON_PF_WORK
{
  GFL_TCBSYS*          tcbsys;
  void*                tcbsys_wk;
  PALETTE_FADE_PTR     pfp;
  u16                  req_bit;
  PF_STATE             state;
};

//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
static void Zkndtl_Common_PfSetReq( ZKNDTL_COMMON_PF_WORK* pf_wk,
    s8 wait, u8 start_evy, u8 end_evy );  // prog/src/system/palanm.c��PaletteFadeReq�Q�ƁB

//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
ZKNDTL_COMMON_PF_WORK* ZKNDTL_COMMON_PfInitPart( HEAPID heap_id, u16 req_bit )  // req_bit��PF_BIT_MAIN_BG,PF_BIT_MAIN_OBJ,PF_BIT_SUB_BG,PF_BIT_SUB_OBJ��|�łȂ�������
{
  ZKNDTL_COMMON_PF_WORK* pf_wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof(ZKNDTL_COMMON_PF_WORK) );
 
  // �^�X�N
  pf_wk->tcbsys_wk    = GFL_HEAP_AllocClearMemory( heap_id, GFL_TCB_CalcSystemWorkSize(PF_TCBSYS_TASK_MAX) );
  pf_wk->tcbsys       = GFL_TCB_Init( PF_TCBSYS_TASK_MAX, pf_wk->tcbsys_wk );

  // �p���b�g�t�F�[�h
  pf_wk->pfp = PaletteFadeInit( heap_id );
  PaletteTrans_AutoSet( pf_wk->pfp, TRUE );

  // �Ώ�
  pf_wk->req_bit = req_bit;

  if( pf_wk->req_bit & PF_BIT_MAIN_BG  ) PaletteFadeWorkAllocSet( pf_wk->pfp, FADE_MAIN_BG,  PF_MAIN_BG_DATA_SIZE,  heap_id ); 
  if( pf_wk->req_bit & PF_BIT_MAIN_OBJ ) PaletteFadeWorkAllocSet( pf_wk->pfp, FADE_MAIN_OBJ, PF_MAIN_OBJ_DATA_SIZE, heap_id ); 
  if( pf_wk->req_bit & PF_BIT_SUB_BG   ) PaletteFadeWorkAllocSet( pf_wk->pfp, FADE_SUB_BG,   PF_SUB_BG_DATA_SIZE,   heap_id );
  if( pf_wk->req_bit & PF_BIT_SUB_OBJ  ) PaletteFadeWorkAllocSet( pf_wk->pfp, FADE_SUB_OBJ,  PF_SUB_OBJ_DATA_SIZE,  heap_id );

  // ���
  pf_wk->state = PF_STATE_COLORLESS;

  return pf_wk;
}

ZKNDTL_COMMON_PF_WORK* ZKNDTL_COMMON_PfInit( HEAPID heap_id )
{
  return ZKNDTL_COMMON_PfInitPart( heap_id, PF_BIT_MAIN_BG | PF_BIT_MAIN_OBJ | PF_BIT_SUB_BG | PF_BIT_SUB_OBJ );
}
void ZKNDTL_COMMON_PfExit( ZKNDTL_COMMON_PF_WORK* pf_wk )
{
  // �p���b�g�t�F�[�h
  if( pf_wk->req_bit & PF_BIT_SUB_OBJ  ) PaletteFadeWorkAllocFree( pf_wk->pfp, FADE_SUB_OBJ );
  if( pf_wk->req_bit & PF_BIT_SUB_BG   ) PaletteFadeWorkAllocFree( pf_wk->pfp, FADE_SUB_BG );
  if( pf_wk->req_bit & PF_BIT_MAIN_OBJ ) PaletteFadeWorkAllocFree( pf_wk->pfp, FADE_MAIN_OBJ );
  if( pf_wk->req_bit & PF_BIT_MAIN_BG  ) PaletteFadeWorkAllocFree( pf_wk->pfp, FADE_MAIN_BG );
  
  PaletteFadeFree( pf_wk->pfp );

  // �^�X�N
  GFL_TCB_Exit( pf_wk->tcbsys );
  GFL_HEAP_FreeMemory( pf_wk->tcbsys_wk );
  
  GFL_HEAP_FreeMemory( pf_wk );
}

void ZKNDTL_COMMON_PfMain( ZKNDTL_COMMON_PF_WORK* pf_wk )
{
  // �^�X�N
  GFL_TCB_Main( pf_wk->tcbsys );

  switch( pf_wk->state )
  {
  case PF_STATE_COLORLESS:
    {
      // �������Ȃ�
    }
    break;
  case PF_STATE_BLACK:
    {
      // �������Ȃ�
    }
    break;
  case PF_STATE_BLACK_TO_COLORLESS:
    {
      if( PaletteFadeCheck(pf_wk->pfp) == 0 )
      {
        pf_wk->state = PF_STATE_COLORLESS;
      }
    }
    break;
  case PF_STATE_COLORLESS_TO_BLACK:
    {
      if( PaletteFadeCheck(pf_wk->pfp) == 0 )
      {
        pf_wk->state = PF_STATE_BLACK;
      }
    }
    break;
  }
}

void ZKNDTL_COMMON_PfTrans( ZKNDTL_COMMON_PF_WORK* pf_wk )  // VBlank���ŌĂԂ���
{
  PaletteFadeTrans( pf_wk->pfp );  // VBlank���ŌĂԂ���
}

BOOL ZKNDTL_COMMON_PfIsExecute( ZKNDTL_COMMON_PF_WORK* pf_wk )
{
  if(    pf_wk->state == PF_STATE_BLACK_TO_COLORLESS
      || pf_wk->state == PF_STATE_COLORLESS_TO_BLACK )
  {
    return TRUE;
  }
  return FALSE;
}
BOOL ZKNDTL_COMMON_PfIsColorless( ZKNDTL_COMMON_PF_WORK* pf_wk )
{
  if( pf_wk->state == PF_STATE_COLORLESS )
    return TRUE;
  else
    return FALSE;
}
BOOL ZKNDTL_COMMON_PfIsBlack( ZKNDTL_COMMON_PF_WORK* pf_wk )
{
  if( pf_wk->state == PF_STATE_BLACK )
    return TRUE;
  else
    return FALSE;
}

void ZKNDTL_COMMON_PfSetBlackToColorless( ZKNDTL_COMMON_PF_WORK* pf_wk )
{
  if( !ZKNDTL_COMMON_PfIsExecute( pf_wk ) )
  {
    Zkndtl_Common_PfSetReq( pf_wk,
        ZKNDTL_COMMON_PF_BRIGHT_WAIT, PF_BLACK_TO_COLORLESS_START_EVY, PF_BLACK_TO_COLORLESS_END_EVY );

    pf_wk->state = PF_STATE_BLACK_TO_COLORLESS;
  }
}
void ZKNDTL_COMMON_PfSetColorlessToBlack( ZKNDTL_COMMON_PF_WORK* pf_wk )
{
  if( !ZKNDTL_COMMON_PfIsExecute( pf_wk ) )
  {
    Zkndtl_Common_PfSetReq( pf_wk,
        ZKNDTL_COMMON_PF_BRIGHT_WAIT, PF_COLORLESS_TO_BLACK_START_EVY, PF_COLORLESS_TO_BLACK_END_EVY );

    pf_wk->state = PF_STATE_COLORLESS_TO_BLACK;
  }
}

void ZKNDTL_COMMON_PfSetColorlessImmediately( ZKNDTL_COMMON_PF_WORK* pf_wk )
{
  if( !ZKNDTL_COMMON_PfIsExecute( pf_wk ) )
  {
    Zkndtl_Common_PfSetReq( pf_wk,
        0, PF_COLORLESS_TO_BLACK_START_EVY, PF_COLORLESS_TO_BLACK_START_EVY );

    pf_wk->state = PF_STATE_COLORLESS;
  }
}
void ZKNDTL_COMMON_PfSetBlackImmediately( ZKNDTL_COMMON_PF_WORK* pf_wk )
{
  if( !ZKNDTL_COMMON_PfIsExecute( pf_wk ) )
  {
    Zkndtl_Common_PfSetReq( pf_wk,
        0, PF_BLACK_TO_COLORLESS_START_EVY, PF_BLACK_TO_COLORLESS_START_EVY );

    pf_wk->state = PF_STATE_BLACK;
  }
}

void ZKNDTL_COMMON_PfSetPaletteData( ZKNDTL_COMMON_PF_WORK* pf_wk, const void* dat, FADEREQ req, u16 pos, u16 siz )  // prog/src/system/palanm.c��PaletteWorkSet�Q�ƁB
{
  PaletteWorkSet( pf_wk->pfp, dat, req, pos, siz );
}
void ZKNDTL_COMMON_PfSetPaletteDataFromArchandle(
    ZKNDTL_COMMON_PF_WORK* pf_wk,
    ARCHANDLE*             handle,
    ARCDATID               dataIdx,
    HEAPID                 heap,
    FADEREQ                req,
    u32                    trans_size,
    u16                    pos,
    u16                    read_pos )  // prog/src/system/palanm.c��PaletteWorkSetEx_ArcHandle�Q�ƁB
{
  PaletteWorkSetEx_ArcHandle(
      pf_wk->pfp,
      handle,
      dataIdx,
      heap,
      req,
      trans_size,
      pos,
      read_pos );
}
void ZKNDTL_COMMON_PfSetPaletteDataFromVram( ZKNDTL_COMMON_PF_WORK* pf_wk )
{
  if( pf_wk->req_bit & PF_BIT_MAIN_BG  ) PaletteWorkSet_VramCopy( pf_wk->pfp, FADE_MAIN_BG,  0, PF_MAIN_BG_DATA_SIZE  );
  if( pf_wk->req_bit & PF_BIT_MAIN_OBJ ) PaletteWorkSet_VramCopy( pf_wk->pfp, FADE_MAIN_OBJ, 0, PF_MAIN_OBJ_DATA_SIZE );
  if( pf_wk->req_bit & PF_BIT_SUB_BG   ) PaletteWorkSet_VramCopy( pf_wk->pfp, FADE_SUB_BG,   0, PF_SUB_BG_DATA_SIZE   );
  if( pf_wk->req_bit & PF_BIT_SUB_OBJ  ) PaletteWorkSet_VramCopy( pf_wk->pfp, FADE_SUB_OBJ,  0, PF_SUB_OBJ_DATA_SIZE  );
}


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
static void Zkndtl_Common_PfSetReq( ZKNDTL_COMMON_PF_WORK* pf_wk,
    s8 wait, u8 start_evy, u8 end_evy )  // prog/src/system/palanm.c��PaletteFadeReq�Q�ƁB
{
  if( pf_wk->req_bit & PF_BIT_MAIN_BG  )
    PaletteFadeReq(
        pf_wk->pfp,
        PF_BIT_MAIN_BG,
        PF_MAIN_BG_REQ_BIT,
        wait,
        start_evy,
        end_evy,
        PF_BLACK_RGB,
        pf_wk->tcbsys
    );
  if( pf_wk->req_bit & PF_BIT_MAIN_OBJ )
    PaletteFadeReq(
        pf_wk->pfp,
        PF_BIT_MAIN_OBJ,
        PF_MAIN_OBJ_REQ_BIT,
        wait,
        start_evy,
        end_evy,
        PF_BLACK_RGB,
        pf_wk->tcbsys
    );
  if( pf_wk->req_bit & PF_BIT_SUB_BG   )
    PaletteFadeReq(
        pf_wk->pfp,
        PF_BIT_SUB_BG,
        PF_SUB_BG_REQ_BIT,
        wait,
        start_evy,
        end_evy,
        PF_BLACK_RGB,
        pf_wk->tcbsys
    );
  if( pf_wk->req_bit & PF_BIT_SUB_OBJ  )
    PaletteFadeReq(
        pf_wk->pfp,
        PF_BIT_SUB_OBJ,
        PF_SUB_OBJ_REQ_BIT,
        wait,
        start_evy,
        end_evy,
        PF_BLACK_RGB,
        pf_wk->tcbsys
    );
}




// �Ŕw��
//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
typedef enum
{
  REAR_DISP_M,
  REAR_DISP_S,
  REAR_DISP_MAX,
}
REAR_DISP;

enum
{
  REAR_DATA_NCL,
  REAR_DATA_NCG,
  REAR_DATA_NSC,
  REAR_DATA_MAX,
};

static const u8 rear_pal_ofs_low[REAR_DISP_MAX][ZKNDTL_COMMON_REAR_TYPE_MAX] =
{
  { 0, 3, 2, 4, },  // main
  { 1, 1, 1, 1, },  // sub
};
static const u8 rear_pal_ofs_high[REAR_DISP_MAX][ZKNDTL_COMMON_REAR_TYPE_MAX] =
{
  { 0, 3, 2, 4, },  // main
  { 0, 3, 2, 4, },  // sub
};
static const ARCDATID rear_data[REAR_DISP_MAX][REAR_DATA_MAX] =
{
  // main
  {
    NARC_zukan_gra_info_info_bgd_NCLR,
    NARC_zukan_gra_info_info_bgd_NCGR,
    NARC_zukan_gra_info_voicebase_bgd_NSCR,
  },
  // sub
  {
    NARC_zukan_gra_info_info_bgu_NCLR,
    NARC_zukan_gra_info_info_bgu_NCGR,
    NARC_zukan_gra_info_infobase_bgu_NSCR,
  },
};
#define REAR_DATA_FORM_M_NSC (NARC_zukan_gra_info_formebase_bgd_NSCR)  // ���ʂɍ����ւ�


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
struct _ZKNDTL_COMMON_REAR_WORK
{
  GFL_ARCUTIL_TRANSINFO       tinfo;
  u8                          bg_frame;
  u8                          wait;
};


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
ZKNDTL_COMMON_REAR_WORK* ZKNDTL_COMMON_RearInit(
                                     HEAPID                    heap_id,
                                     ZKNDTL_COMMON_REAR_TYPE   type,
                                     u8                        bg_frame,
                                     u8                        pal0,
                                     u8                        pal1
                                )
{
  ZKNDTL_COMMON_REAR_WORK* rear_wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof(ZKNDTL_COMMON_REAR_WORK) );

  ARCHANDLE* handle;
  ARCDATID   data_nsc;

  // BG�t���[�����烁�C�����T�u�����肷��
  REAR_DISP disp;
  PALTYPE   paltype;
  
  if( bg_frame < GFL_BG_FRAME0_S )
  {
    disp    = REAR_DISP_M;
    paltype = PALTYPE_MAIN_BG;
  }
  else
  {
    disp    = REAR_DISP_S;
    paltype = PALTYPE_SUB_BG;
  }

  // �ǂݍ���œ]��
  handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, heap_id );

  GFL_ARCHDL_UTIL_TransVramPaletteEx(
                               handle,
                               rear_data[disp][REAR_DATA_NCL],
                               paltype,
                               rear_pal_ofs_low[disp][type] * 0x20,
                               pal0 * 0x20,
                               1 * 0x20,
                               heap_id );

  GFL_ARCHDL_UTIL_TransVramPaletteEx(
                               handle,
                               rear_data[disp][REAR_DATA_NCL],
                               paltype,
                               rear_pal_ofs_high[disp][type] * 0x20,
                               pal1 * 0x20,
                               1 * 0x20,
                               heap_id );

  rear_wk->tinfo = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
                            handle,
                            rear_data[disp][REAR_DATA_NCG],
                            bg_frame,
                            0,
                            FALSE,
                            heap_id );
  GF_ASSERT_MSG( rear_wk->tinfo != GFL_ARCUTIL_TRANSINFO_FAIL, "ZUKAN_DETAIL_COMMON : BG�L�����̈悪����܂���ł����B\n" );

  data_nsc = rear_data[disp][REAR_DATA_NSC];
  if( type == ZKNDTL_COMMON_REAR_TYPE_FORM && disp == REAR_DISP_M )  // ���ʂɍ����ւ�
      data_nsc = REAR_DATA_FORM_M_NSC;

  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(
        handle,
        data_nsc,
        bg_frame,
        0,
        GFL_ARCUTIL_TRANSINFO_GetPos( rear_wk->tinfo ),
        0,
        FALSE,
        heap_id );
  GFL_BG_ChangeScreenPalette( bg_frame, 0, 21, 32, 3, pal0 );
  GFL_BG_ChangeScreenPalette( bg_frame, 0, 0, 32, 21, pal1 );

  GFL_ARC_CloseDataHandle( handle );

  GFL_BG_LoadScreenV_Req( bg_frame );

  rear_wk->bg_frame = bg_frame;
  rear_wk->wait = ZKNDTL_SCROLL_WAIT;

  return rear_wk;
}

void ZKNDTL_COMMON_RearExit( ZKNDTL_COMMON_REAR_WORK* rear_wk )
{
  GFL_BG_FreeCharacterArea( rear_wk->bg_frame,
                            GFL_ARCUTIL_TRANSINFO_GetPos( rear_wk->tinfo ),
                            GFL_ARCUTIL_TRANSINFO_GetSize( rear_wk->tinfo ) );

  GFL_HEAP_FreeMemory( rear_wk );
}

void ZKNDTL_COMMON_RearMain( ZKNDTL_COMMON_REAR_WORK* rear_wk )
{
  if( rear_wk->wait == 0 )
  {
    GFL_BG_SetScrollReq( rear_wk->bg_frame, GFL_BG_SCROLL_X_INC, 1 );
    rear_wk->wait = ZKNDTL_SCROLL_WAIT;
  }
  else
  {
    rear_wk->wait--;
  }
}




// BG�p�l��
//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
                                             // type          TRANS  EXIST

GFL_ARCUTIL_TRANSINFO ZKNDTL_COMMON_PanelCreate(           //  o      x
             ZKNDTL_COMMON_PANEL_CREATE_TYPE    type,      //  o      o
             HEAPID                             heap_id,   //  o      o
             u8                                 bg_frame,  //  o      o
             u8                                 pal_num,   //  o      x
             u8                                 pal_pos,   //  o      o
             u8                                 pal_ofs,   //  o      x
             u32                                arc_id,    //  o      o
             ARCDATID                           data_ncl,  //  o      x
             ARCDATID                           data_ncg,  //  o      x 
             ARCDATID                           data_nsc,  //  o      o
             GFL_ARCUTIL_TRANSINFO              tinfo      //  x      o
       )
{
  ARCHANDLE*  handle;

  // BG�t���[�����烁�C�����T�u�����肷��
  PALTYPE   paltype;
  
  if( bg_frame < GFL_BG_FRAME0_S )
  {
    paltype = PALTYPE_MAIN_BG;
  }
  else
  {
    paltype = PALTYPE_SUB_BG;
  }

  // �ǂݍ���œ]��
  handle = GFL_ARC_OpenDataHandle( arc_id, heap_id );

  if( type == ZKNDTL_COMMON_PANEL_CREATE_TYPE_TRANS_PAL_CHARA )
  {
    GFL_ARCHDL_UTIL_TransVramPaletteEx(
                               handle,
                               data_ncl,
                               paltype,
                               pal_ofs * 0x20,
                               pal_pos * 0x20,
                               pal_num * 0x20,
                               heap_id );
  }

  if( type == ZKNDTL_COMMON_PANEL_CREATE_TYPE_TRANS_PAL_CHARA )
  {
    tinfo = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
                            handle,
                            data_ncg,
                            bg_frame,
                            0,
                            FALSE,
                            heap_id );
    GF_ASSERT_MSG( tinfo != GFL_ARCUTIL_TRANSINFO_FAIL, "ZUKAN_DETAIL_COMMON : BG�L�����̈悪����܂���ł����B\n" );
  }

  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(
        handle,
        data_nsc,
        bg_frame,
        0,
        GFL_ARCUTIL_TRANSINFO_GetPos( tinfo ),
        0,
        FALSE,
        heap_id );
  GFL_BG_ChangeScreenPalette( bg_frame, 0, 0, 32, 24, pal_pos );

  GFL_ARC_CloseDataHandle( handle );

  GFL_BG_LoadScreenV_Req( bg_frame );
  
  return tinfo;
}

void ZKNDTL_COMMON_PanelDelete(
                       u8                      bg_frame,
                       GFL_ARCUTIL_TRANSINFO   tinfo
        )
{
  GFL_BG_FreeCharacterArea( bg_frame,
                            GFL_ARCUTIL_TRANSINFO_GetPos( tinfo ),
                            GFL_ARCUTIL_TRANSINFO_GetSize( tinfo ) );
}

