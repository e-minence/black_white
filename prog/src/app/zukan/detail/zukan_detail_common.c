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

#include "zukan_detail_def.h"
#include "zukan_detail_common.h"

// �A�[�J�C�u

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
u16   ZKNDTL_COMMON_GetCurrPoke( ZKNDTL_COMMON_WORK* cmn )
{
  return cmn->list[*(cmn->no)];
}
void  ZKNDTL_COMMON_GoToNextPoke( ZKNDTL_COMMON_WORK* cmn )
{
  *(cmn->no) += 1;
  if( *(cmn->no) >= cmn->num ) *(cmn->no) = 0;
}
void  ZKNDTL_COMMON_GoToPrevPoke( ZKNDTL_COMMON_WORK* cmn )
{
  if( *(cmn->no) == 0 ) *(cmn->no) = cmn->num -1;
  else                  *(cmn->no) -= 1;
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
#define ZKNDTL_COMMON_FADE_BRIGHT_DELTA (1)
#define ZKNDTL_COMMON_FADE_BRIGHT_WAIT  (1)

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

