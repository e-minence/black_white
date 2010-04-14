//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_fade.c
 *	@brief  �o�g�����R�[�_�[�p�t�F�[�h�V�X�e��
 *	@author	Toru=Nagihashi
 *	@data		2009.12.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/palanm.h"

//�O�����J
#include "br_fade.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define BR_FADE_DEFAULT_SYNC    (0)
#define BR_FADE_DEFAULT_EV      (16)
#define BR_FADE_DEFAULT_EV_MAX  (16/GFL_FADE_GetFadeSpeed())

#define BR_FADE_ALPHA_PLANEMASK_M_01  (GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 )
#define BR_FADE_ALPHA_PLANEMASK_M_02  (GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3)
#define BR_FADE_ALPHA_PLANEMASK_S_01  (GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 )
#define BR_FADE_ALPHA_PLANEMASK_S_02  (GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2)

#define BR_FADE_CHANGEFADE_NONE (0xFFFF)
//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================


//-------------------------------------
///	�t�F�[�h���s�֐�
//=====================================
typedef BOOL (*BR_FADE_MAINFUNCTION)( BR_FADE_WORK * p_wk, u32 *p_seq );

//-------------------------------------
///	�t�F�[�h�V�X�e��
//=====================================
struct _BR_FADE_WORK
{ 
  BOOL            is_end;
  BR_FADE_DISPLAY display;
  BR_FADE_DIR     dir;
  u32             seq;
  u32             cnt;
  u32             max;
  u32             sync;
  BR_FADE_MAINFUNCTION  MainFunction;
  PALETTE_FADE_PTR      p_pfd;
  GXRgb           pfd_color;
  u16             dummy;

  BR_FADE_CHANGEFADE_PARAM  changefade_param;
  u32                       sub_seq;
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//���s�֐�
static BOOL Br_Fade_MainMasterBrightBlack( BR_FADE_WORK *p_wk, u32 *p_seq );
static BOOL Br_Fade_MainMasterBrightWhite( BR_FADE_WORK *p_wk, u32 *p_seq );
static BOOL Br_Fade_MainAlpha( BR_FADE_WORK *p_wk, u32 *p_seq );
static BOOL Br_Fade_MainPallete( BR_FADE_WORK *p_wk, u32 *p_seq );
static BOOL Br_Fade_MainMasterBrightAndAlpha( BR_FADE_WORK *p_wk, u32 *p_seq );

static BOOL Br_Fade_MainChangeFade( BR_FADE_WORK *p_wk, u32 *p_seq );
//���̑�
static BR_FADE_MAINFUNCTION Br_Fade_Factory( BR_FADE_TYPE type );

//=============================================================================
/**
 *    PUBLIC
 */
//=============================================================================
//----------------------------------------------------------------------------
/** 
 *	@brief  �t�F�[�h�V�X�e��������
 *
 *	@param	HEAPID heapID 
 */
//-----------------------------------------------------------------------------
BR_FADE_WORK * BR_FADE_Init( HEAPID heapID )
{ 
  BR_FADE_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(BR_FADE_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(BR_FADE_WORK) );
  p_wk->sub_seq = BR_FADE_CHANGEFADE_NONE;
  p_wk->p_pfd   = PaletteFadeInit( heapID );

  PaletteFadeWorkAllocSet( p_wk->p_pfd, FADE_MAIN_BG, FADE_PAL_ALL_SIZE, heapID );
  PaletteFadeWorkAllocSet( p_wk->p_pfd, FADE_MAIN_OBJ, FADE_PAL_ALL_SIZE, heapID );
  PaletteFadeWorkAllocSet( p_wk->p_pfd, FADE_SUB_BG, FADE_PAL_ALL_SIZE, heapID );
  PaletteFadeWorkAllocSet( p_wk->p_pfd, FADE_SUB_OBJ, FADE_PAL_ALL_SIZE, heapID );
  PaletteTrans_AutoSet( p_wk->p_pfd, TRUE );

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�V�X�e���j��
 *
 *	@param	BR_FADE_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_FADE_Exit( BR_FADE_WORK *p_wk )
{ 
  PaletteFadeWorkAllocFree( p_wk->p_pfd, FADE_MAIN_OBJ );	///< main	oam
  PaletteFadeWorkAllocFree( p_wk->p_pfd, FADE_MAIN_BG );	///< main	bg
  PaletteFadeWorkAllocFree( p_wk->p_pfd, FADE_SUB_OBJ );	///< main	oam
  PaletteFadeWorkAllocFree( p_wk->p_pfd, FADE_SUB_BG );	///< main	bg
  
  PaletteFadeFree( p_wk->p_pfd );
  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h���C������
 *
 *	@param	BR_FADE_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_FADE_Main( BR_FADE_WORK *p_wk )
{
  if( p_wk->MainFunction )
  { 
    p_wk->is_end = p_wk->MainFunction( p_wk, &p_wk->seq );
    if( p_wk->is_end )
    { 
      p_wk->MainFunction  = NULL;
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�J�n
 *
 *	@param	BR_FADE_WORK *p_wk  ���[�N
 *	@param	type                �t�F�[�h�̎��
 *	@param  disp                ���
 *	@param  dir                 �t�F�[�h�̕���
 */
//-----------------------------------------------------------------------------
void BR_FADE_StartFade( BR_FADE_WORK *p_wk, BR_FADE_TYPE type, BR_FADE_DISPLAY disp, BR_FADE_DIR  dir )
{ 
  BR_FADE_StartFadeEx( p_wk, type, disp, dir, BR_FADE_DEFAULT_SYNC );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�J�n  �V���N�w���
 *
 *	@param	BR_FADE_WORK *p_wk  ���[�N
 *	@param	type  �t�F�[�h�̎��
 *	@param  disp  ���
 *	@param  dir   �t�F�[�h�̕���
 *	@param	sync  �V���N
 */
//-----------------------------------------------------------------------------
void BR_FADE_StartFadeEx( BR_FADE_WORK *p_wk, BR_FADE_TYPE type, BR_FADE_DISPLAY disp, BR_FADE_DIR dir, u32 sync )
{ 
  p_wk->display       = disp;
  p_wk->dir           = dir;
  p_wk->sync          = sync;

  p_wk->MainFunction  = Br_Fade_Factory( type );
  p_wk->is_end        = FALSE;
  p_wk->seq           = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�I�����m
 *
 *	@param	const BR_FADE_WORK *cp_wk   ���[�N
 *
 *	@return TRUE�Ȃ�΃t�F�[�h�I��  FALSE�Ȃ�׃t�F�[�h��
 */
//-----------------------------------------------------------------------------
BOOL BR_FADE_IsEnd( const BR_FADE_WORK *cp_wk )
{ 
  return cp_wk->is_end;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��ʐ؂�ւ��悤�ɁA  �t�F�[�h�C�����R�[���o�b�N�[���t�F�[�h�A�E�g����
 *
 *	@param	BR_FADE_WORK *p_wk                  ���[�N
 *	@param	BR_FADE_CHANGEFADE_PARAM *cp_param  ����
 */
//-----------------------------------------------------------------------------
void BR_FADE_StartChangeFade( BR_FADE_WORK *p_wk, const BR_FADE_CHANGEFADE_PARAM *cp_param )
{ 
  p_wk->changefade_param  = *cp_param;
  p_wk->sub_seq       = 0;
  p_wk->sync          = BR_FADE_DEFAULT_SYNC;

  p_wk->MainFunction  = Br_Fade_MainChangeFade;
  p_wk->is_end        = FALSE;
  p_wk->seq           = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�Ŏg�p����p���b�g���R�s�[
 *
 *	@param	BR_FADE_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_FADE_PALETTE_Copy( BR_FADE_WORK *p_wk )
{
  PaletteWorkSet_VramCopy( p_wk->p_pfd, FADE_MAIN_BG, 0, FADE_PAL_ALL_SIZE );
  PaletteWorkSet_VramCopy( p_wk->p_pfd, FADE_MAIN_OBJ, 0, FADE_PAL_ALL_SIZE );
  PaletteWorkSet_VramCopy( p_wk->p_pfd, FADE_SUB_BG, 0, FADE_PAL_ALL_SIZE );
  PaletteWorkSet_VramCopy( p_wk->p_pfd, FADE_SUB_OBJ, 0, FADE_PAL_ALL_SIZE );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �p���b�g�t�F�[�h�Ŏg�p����F��ݒ�
 *
 *	@param	BR_FADE_WORK *p_wk  ���[�N
 *	@param	rgb                 RGB
 */
//-----------------------------------------------------------------------------
void BR_FADE_PALETTE_SetColor( BR_FADE_WORK *p_wk, GXRgb rgb )
{ 
  p_wk->pfd_color = rgb;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �p���b�g�t�F�[�h�̎w��F��]��
 *
 *	@param	BR_FADE_WORK *p_wk  ���[�N
 *	@param  ���
 */
//-----------------------------------------------------------------------------
void BR_FADE_PALETTE_TransColor( BR_FADE_WORK *p_wk, BR_FADE_DISPLAY display )
{ 
  //��ʂɂ��ݒ�
  if( display & BR_FADE_DISPLAY_MAIN )
  { 
//    ColorConceChangePfd( p_wk->p_pfd, FADE_MAIN_OBJ, 0x3FFE, BR_FADE_DEFAULT_EV, p_wk->pfd_color );	///< main	oam
    ColorConceChangePfd( p_wk->p_pfd, FADE_MAIN_BG,  0xBFFF, BR_FADE_DEFAULT_EV, p_wk->pfd_color );	///< main	bg
  }
  if( display & BR_FADE_DISPLAY_SUB )
  { 
//    ColorConceChangePfd( p_wk->p_pfd, FADE_SUB_OBJ, 0xFFFE, BR_FADE_DEFAULT_EV, p_wk->pfd_color );	///< sub	oam
    ColorConceChangePfd( p_wk->p_pfd, FADE_SUB_BG,  0xBFFF, BR_FADE_DEFAULT_EV, p_wk->pfd_color );	///< sub	bg
  }

  PaletteTransSwitch( p_wk->p_pfd, TRUE );
  PaletteFadeTrans( p_wk->p_pfd );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �A���t�@�ݒ�
 *
 *	@param	BR_FADE_WORK *p_wk  ���[�N
 *	@param	ev                  �A���t�@�l
 */
//-----------------------------------------------------------------------------
void BR_FADE_ALPHA_SetAlpha( BR_FADE_WORK *p_wk, BR_FADE_DISPLAY display, u8 ev )
{ 
  //����
  if( p_wk->display & BR_FADE_DISPLAY_MAIN )
  { 
    G2_SetBlendAlpha(
        BR_FADE_ALPHA_PLANEMASK_M_01,
        BR_FADE_ALPHA_PLANEMASK_M_02,
        ev,
        BR_FADE_DEFAULT_EV-ev
        );
    if( ev == 0 )
    { 
      GFL_DISP_GX_SetVisibleControl( GX_BLEND_PLANEMASK_BG0 , FALSE );
      GFL_DISP_GX_SetVisibleControl( GX_BLEND_PLANEMASK_BG1 , FALSE );
      GFL_DISP_GX_SetVisibleControl( GX_BLEND_PLANEMASK_BG2 , FALSE );
    }
    else
    { 
      GFL_DISP_GX_SetVisibleControl( GX_BLEND_PLANEMASK_BG0 , TRUE );
      GFL_DISP_GX_SetVisibleControl( GX_BLEND_PLANEMASK_BG1 , TRUE );
      GFL_DISP_GX_SetVisibleControl( GX_BLEND_PLANEMASK_BG2 , TRUE );
    }
  }
  //�����
  if( p_wk->display & BR_FADE_DISPLAY_SUB )
  { 
    G2S_SetBlendAlpha(
        BR_FADE_ALPHA_PLANEMASK_S_01,
        BR_FADE_ALPHA_PLANEMASK_S_02,
        ev,
        BR_FADE_DEFAULT_EV-ev
        );
    if( ev == 0 )
    { 
      GFL_DISP_GXS_SetVisibleControl( GX_BLEND_PLANEMASK_BG0 , FALSE );
      GFL_DISP_GXS_SetVisibleControl( GX_BLEND_PLANEMASK_BG1 , FALSE );
    }
    else
    { 
      GFL_DISP_GXS_SetVisibleControl( GX_BLEND_PLANEMASK_BG0 , TRUE );
      GFL_DISP_GXS_SetVisibleControl( GX_BLEND_PLANEMASK_BG1 , TRUE );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�[�J�C�u���p���b�g�t�F�[�h�ɓǂݍ���
 *
 *	@param	BR_FADE_WORK *p_wk  ���[�N
 *	@param	*p_handle           �n���h��
 *	@param	datID               DATAID
 *	@param	type                �ǂݍ��݃^�C�v
 *	@param	offset              �I�t�Z�b�g  �F�P��
 *	@param	trans_size          �T�C�Y      �o�C�g�P��
 *	@param	heapID              �q�[�vID
 */
//-----------------------------------------------------------------------------
void BR_FADE_PALETTE_LoadPalette( BR_FADE_WORK *p_wk, ARCHANDLE *p_handle, ARCDATID datID, BR_FADE_PALETTE_LOADTYPE type, u32 offset, u32 trans_size, HEAPID heapID )
{ 
  PaletteWorkSetEx_ArcHandle( p_wk->p_pfd, p_handle, datID, heapID,
      type, trans_size, offset, 0 );
}

//=============================================================================
/**
 *    PRIVATE
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �P�x�ł̃t�F�[�h���C������  �����Ȃ�
 *
 *	@param	BR_FADE_WORK *p_wk  ���[�N
 *	@param  u32 seq             �V�[�P���X
 *
 *	@return TRUE�Ȃ�Ώ����I��  FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
static BOOL Br_Fade_MainMasterBrightBlack( BR_FADE_WORK *p_wk, u32 *p_seq )
{ 
  enum
  { 
    SEQ_MBB_INIT,
    SEQ_MBB_MAIN,
    SEQ_MBB_EXIT,
  };

  switch( *p_seq )
  { 
  case SEQ_MBB_INIT:
    { 
      u32 mode  = 0;
      u32 start, end;
      //��ʂɂ��ݒ�
      if( p_wk->display & BR_FADE_DISPLAY_MAIN )
      { 
        mode  |= GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN;
      }
      if( p_wk->display & BR_FADE_DISPLAY_SUB )
      { 
        mode  |= GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB;
      }
      //�����ɂ��ݒ�
      if( p_wk->dir == BR_FADE_DIR_IN )
      { 
        start = BR_FADE_DEFAULT_EV;
        end   = 0;
      }
      else if( p_wk->dir == BR_FADE_DIR_OUT )
      { 
        start = 0;
        end   = BR_FADE_DEFAULT_EV;
      }

      GFL_FADE_SetMasterBrightReq( mode, start, end, p_wk->sync );
      *p_seq  = SEQ_MBB_MAIN;
    }
    break;

  case SEQ_MBB_MAIN:
    if( !GFL_FADE_CheckFade() )
    { 
      *p_seq  = SEQ_MBB_EXIT;
    }
    break;

  case SEQ_MBB_EXIT:
    return TRUE;
  }


  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �P�x�ł̃t�F�[�h���C������  �����Ȃ�
 *
 *	@param	BR_FADE_WORK *p_wk  ���[�N
 *	@param  u32 seq             �V�[�P���X
 *
 *	@return TRUE�Ȃ�Ώ����I��  FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
static BOOL Br_Fade_MainMasterBrightWhite( BR_FADE_WORK *p_wk, u32 *p_seq )
{ 
  enum
  { 
    SEQ_MBB_INIT,
    SEQ_MBB_MAIN,
    SEQ_MBB_EXIT,
  };

  switch( *p_seq )
  { 
  case SEQ_MBB_INIT:
    { 
      u32 mode  = 0;
      u32 start, end;
      //��ʂɂ��ݒ�
      if( p_wk->display & BR_FADE_DISPLAY_MAIN )
      { 
        mode  |= GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN;
      }
      if( p_wk->display & BR_FADE_DISPLAY_SUB )
      { 
        mode  |= GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB;
      }
      //�����ɂ��ݒ�
      if( p_wk->dir == BR_FADE_DIR_IN )
      { 
        start = BR_FADE_DEFAULT_EV;
        end   = 0;
      }
      else if( p_wk->dir == BR_FADE_DIR_OUT )
      { 
        start = 0;
        end   = BR_FADE_DEFAULT_EV;
      }

      GFL_FADE_SetMasterBrightReq( mode, start, end, p_wk->sync );
      *p_seq  = SEQ_MBB_MAIN;
    }
    break;

  case SEQ_MBB_MAIN:
    if( !GFL_FADE_CheckFade() )
    { 
      *p_seq  = SEQ_MBB_EXIT;
    }
    break;

  case SEQ_MBB_EXIT:
    return TRUE;
  }


  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �A���t�@�ł̃t�F�[�h���C������
 *
 *	@param	BR_FADE_WORK *p_wk  ���[�N
 *	@param  u32 seq             �V�[�P���X
 *
 *	@return TRUE�Ȃ�Ώ����I��  FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
static BOOL Br_Fade_MainAlpha( BR_FADE_WORK *p_wk, u32 *p_seq )
{ 
  enum
  { 
    SEQ_AL_INIT,
    SEQ_AL_MAIN,
    SEQ_AL_EXIT,
  };

  switch( *p_seq )
  { 
  case SEQ_AL_INIT:
    {
      int ev1, ev2;

      if( p_wk->dir == BR_FADE_DIR_IN )
      { 
        ev1 = 0;
        ev2 = BR_FADE_DEFAULT_EV;
      }
      else if( p_wk->dir == BR_FADE_DIR_OUT )
      { 
        ev1 = BR_FADE_DEFAULT_EV;
        ev2 = 0;
      }

      if( p_wk->display & BR_FADE_DISPLAY_MAIN )
      { 
        G2_SetBlendAlpha(
          BR_FADE_ALPHA_PLANEMASK_M_01,
          BR_FADE_ALPHA_PLANEMASK_M_02,
          ev1,
          ev2
          );
      }
      if( p_wk->display & BR_FADE_DISPLAY_SUB )
      { 
        G2S_SetBlendAlpha(
          BR_FADE_ALPHA_PLANEMASK_S_01,
          BR_FADE_ALPHA_PLANEMASK_S_02,
          ev1,
          ev2
          );
      }

      p_wk->cnt = 0;
      p_wk->max = p_wk->sync == 0? BR_FADE_DEFAULT_EV_MAX: p_wk->sync;
      *p_seq    = SEQ_AL_MAIN;
    }
    break;

  case SEQ_AL_MAIN:
    { 
      int ev1, ev2;

      //�t�F�[�h����
      if( p_wk->dir == BR_FADE_DIR_IN )
      { 
        ev1 = 0 + BR_FADE_DEFAULT_EV * p_wk->cnt / p_wk->max;
        ev2 = BR_FADE_DEFAULT_EV - BR_FADE_DEFAULT_EV * p_wk->cnt / p_wk->max;
      }
      else if( p_wk->dir == BR_FADE_DIR_OUT )
      { 
        ev1 = BR_FADE_DEFAULT_EV - BR_FADE_DEFAULT_EV * p_wk->cnt / p_wk->max;
        ev2 = 0 + BR_FADE_DEFAULT_EV * p_wk->cnt / p_wk->max;
      }

      //����
      if( p_wk->display & BR_FADE_DISPLAY_MAIN )
      { 
        G2_ChangeBlendAlpha( ev1, ev2 );
        if( ev1 == 0 )
        { 
          GFL_DISP_GX_SetVisibleControl( GX_BLEND_PLANEMASK_BG0 , FALSE );
          GFL_DISP_GX_SetVisibleControl( GX_BLEND_PLANEMASK_BG1 , FALSE );
          GFL_DISP_GX_SetVisibleControl( GX_BLEND_PLANEMASK_BG2 , FALSE );
        }
        else
        { 
          GFL_DISP_GX_SetVisibleControl( GX_BLEND_PLANEMASK_BG0 , TRUE );
          GFL_DISP_GX_SetVisibleControl( GX_BLEND_PLANEMASK_BG1 , TRUE );
          GFL_DISP_GX_SetVisibleControl( GX_BLEND_PLANEMASK_BG2 , TRUE );
        }
      }
      //�����
      if( p_wk->display & BR_FADE_DISPLAY_SUB )
      { 
        G2S_ChangeBlendAlpha( ev1, ev2 );
        if( ev1 == 0 )
        { 
          GFL_DISP_GXS_SetVisibleControl( GX_BLEND_PLANEMASK_BG0 , FALSE );
          GFL_DISP_GXS_SetVisibleControl( GX_BLEND_PLANEMASK_BG1 , FALSE );
        }
        else
        { 
          GFL_DISP_GXS_SetVisibleControl( GX_BLEND_PLANEMASK_BG0 , TRUE );
          GFL_DISP_GXS_SetVisibleControl( GX_BLEND_PLANEMASK_BG1 , TRUE );
        }
      }
    }
    if( p_wk->cnt++ >= p_wk->max )
    { 
      *p_seq  = SEQ_AL_EXIT;
    }
    break;

  case SEQ_AL_EXIT:
    return TRUE;
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �p���b�g�ł̃t�F�[�h���C������
 *
 *	@param	BR_FADE_WORK *p_wk  ���[�N
 *	@param  u32 seq             �V�[�P���X
 *
 *	@return TRUE�Ȃ�Ώ����I��  FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
static BOOL Br_Fade_MainPallete( BR_FADE_WORK *p_wk, u32 *p_seq )
{ 
  enum
  { 
    SEQ_PFD_INIT,
    SEQ_PFD_MAIN,
    SEQ_PFD_EXIT,
  };

  switch( *p_seq )
  { 
  case SEQ_PFD_INIT:
    { 
      int i;

      p_wk->cnt = 0;
      p_wk->max = p_wk->sync == 0? BR_FADE_DEFAULT_EV_MAX: p_wk->sync;

      PaletteTransSwitch( p_wk->p_pfd, TRUE );

      *p_seq  = SEQ_PFD_MAIN;
    }
    break;

  case SEQ_PFD_MAIN:
    { 
      u8 ev;
      //�t�F�[�h����
      if( p_wk->dir == BR_FADE_DIR_IN )
      { 
        ev = BR_FADE_DEFAULT_EV - BR_FADE_DEFAULT_EV * p_wk->cnt / p_wk->max;
      }
      else if( p_wk->dir == BR_FADE_DIR_OUT )
      { 
        ev = 0 + BR_FADE_DEFAULT_EV * p_wk->cnt / p_wk->max;
      }

      //��ʂɂ��ݒ�
      if( p_wk->display == BR_FADE_DISPLAY_TOUCH_HERE )
      { 
        //TOUCH_HERE�̂�
        ColorConceChangePfd( p_wk->p_pfd, FADE_SUB_BG, 0xF000, ev, 0xFFFF );
      }
      else
      { 
        //���
        if( p_wk->display & BR_FADE_DISPLAY_MAIN )
        { 
          ColorConceChangePfd( p_wk->p_pfd, FADE_MAIN_OBJ, 0x3FFF, ev, p_wk->pfd_color );	///< main	oam
          ColorConceChangePfd( p_wk->p_pfd, FADE_MAIN_BG,  0xBFFF, ev, p_wk->pfd_color );	///< main	bg
        }
        if( p_wk->display & BR_FADE_DISPLAY_SUB )
        { 
          ColorConceChangePfd( p_wk->p_pfd, FADE_SUB_OBJ, 0xFFFF, ev, p_wk->pfd_color );	///< sub	oam
          ColorConceChangePfd( p_wk->p_pfd, FADE_SUB_BG,  0xBFFF, ev, p_wk->pfd_color );	///< sub	bg
        }
      }

      PaletteFadeTrans( p_wk->p_pfd );

      if( p_wk->cnt++ >= p_wk->max )
      { 
        *p_seq  = SEQ_PFD_EXIT;
      }
    }
    break;

  case SEQ_PFD_EXIT:
    return TRUE;
  }


  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���ʂ��}�X�^�[�u���C�g�A����ʂ��A���t�@�̃t�F�[�h���C������
 *
 *	@param	BR_FADE_WORK *p_wk  ���[�N
 *	@param  u32 seq             �V�[�P���X
 *
 *	@return TRUE�Ȃ�Ώ����I��  FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
static BOOL Br_Fade_MainMasterBrightAndAlpha( BR_FADE_WORK *p_wk, u32 *p_seq )
{ 
   enum
  { 
    SEQ_MBA_INIT,
    SEQ_MBA_MAIN,
    SEQ_MBA_EXIT,
  };

  switch( *p_seq )
  { 
  case SEQ_MBA_INIT:
    { 
      u32 start, end;
      int ev1, ev2;

      //�����ɂ��ݒ�
      if( p_wk->dir == BR_FADE_DIR_IN )
      { 
        start = BR_FADE_DEFAULT_EV;
        end   = 0;
      }
      else if( p_wk->dir == BR_FADE_DIR_OUT )
      { 
        start = 0;
        end   = BR_FADE_DEFAULT_EV;
      }
      if( p_wk->dir == BR_FADE_DIR_IN )
      { 
        ev1 = 0;
        ev2 = BR_FADE_DEFAULT_EV;
      }
      else if( p_wk->dir == BR_FADE_DIR_OUT )
      { 
        ev1 = BR_FADE_DEFAULT_EV;
        ev2 = 0;
      }

      GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN, start, end, p_wk->sync );

      G2S_SetBlendAlpha(
          BR_FADE_ALPHA_PLANEMASK_S_01,
          BR_FADE_ALPHA_PLANEMASK_S_02,
          ev1,
          ev2
          );
      p_wk->cnt = 0;
      p_wk->max = p_wk->sync == 0? BR_FADE_DEFAULT_EV_MAX: p_wk->sync;

      *p_seq  = SEQ_MBA_MAIN;
    }
    break;

  case SEQ_MBA_MAIN:
    { 
      int ev1, ev2;
      BOOL is_end = TRUE;

      //�t�F�[�h����
      if( p_wk->dir == BR_FADE_DIR_IN )
      { 
        ev1 = 0 + BR_FADE_DEFAULT_EV * p_wk->cnt / p_wk->max;
        ev2 = BR_FADE_DEFAULT_EV - BR_FADE_DEFAULT_EV * p_wk->cnt / p_wk->max;
      }
      else if( p_wk->dir == BR_FADE_DIR_OUT )
      { 
        ev1 = BR_FADE_DEFAULT_EV - BR_FADE_DEFAULT_EV * p_wk->cnt / p_wk->max;
        ev2 = 0 + BR_FADE_DEFAULT_EV * p_wk->cnt / p_wk->max;
      }

      //�A���t�@�̏���
      { 
        G2S_ChangeBlendAlpha( ev1, ev2 );
        if( ev1 == 0 )
        { 
          GFL_DISP_GXS_SetVisibleControl( BR_FADE_ALPHA_PLANEMASK_S_01 , FALSE );
        }
        else
        { 
          GFL_DISP_GXS_SetVisibleControl( BR_FADE_ALPHA_PLANEMASK_S_01 , TRUE );
        }
      }

      is_end  &= ( p_wk->cnt++ >= p_wk->max );
      is_end  &= ( !GFL_FADE_CheckFade() );

      if( is_end )
      { 
        *p_seq  = SEQ_MBA_EXIT;
      }
    }
    break;

  case SEQ_MBA_EXIT:
    return TRUE;
  }


  return FALSE; 
}
//----------------------------------------------------------------------------
/**
 *	@brief  ��ʓǂݑւ��悤�̃t�F�[�h
 *
 *	@param	BR_FADE_WORK *p_wk  ���[�N
 *	@param  u32 seq             �V�[�P���X
 *
 *	@return TRUE�Ȃ�Ώ����I��  FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
static BOOL Br_Fade_MainChangeFade( BR_FADE_WORK *p_wk, u32 *p_seq )
{ 
  enum
  { 
    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_CHANGE,
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_END,
  };

  switch( *p_seq )
  { 
  case SEQ_FADEOUT_START:
    BR_FADE_StartFade( p_wk, p_wk->changefade_param.fadeout_type,
        p_wk->changefade_param.disp, BR_FADE_DIR_OUT );
    p_wk->sub_seq = 0;
    (*p_seq)++;
    break;
  case SEQ_FADEOUT_WAIT:
    if( p_wk->MainFunction( p_wk, &p_wk->sub_seq ) )
    { 
      (*p_seq)++;
    }
    break;
  case SEQ_CHANGE:
    if( p_wk->changefade_param.callback )
    { 
      if( p_wk->changefade_param.callback( p_wk->changefade_param.p_wk_adrs ) )
      { 
        (*p_seq)++;
      }
    }
    else
    { 
      (*p_seq)++;
    }
    break;
  case SEQ_FADEIN_START:
    BR_FADE_StartFade( p_wk, p_wk->changefade_param.fadein_type,
        p_wk->changefade_param.disp, BR_FADE_DIR_IN );
    p_wk->sub_seq = 0;
    (*p_seq)++;
    break;
  case SEQ_FADEIN_WAIT:
    if( p_wk->MainFunction( p_wk, &p_wk->sub_seq ) )
    { 
      (*p_seq)++;
    }
    break;
  case SEQ_END:
    return TRUE;
    break;
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�̎�ނɂ����s�֐���Ԃ�
 *
 *	@param	BR_FADE_TYPE  �t�F�[�h�̎��
 *
 *	@return ���s�֐�
 */
//-----------------------------------------------------------------------------
static BR_FADE_MAINFUNCTION Br_Fade_Factory( BR_FADE_TYPE type )
{ 
  switch( type )
  { 
  case BR_FADE_TYPE_MASTERBRIGHT_BLACK:
    return Br_Fade_MainMasterBrightBlack;

  case BR_FADE_TYPE_MASTERBRIGHT_WHITE:
    return Br_Fade_MainMasterBrightWhite;

  case BR_FADE_TYPE_ALPHA_BG012OBJ:
    return Br_Fade_MainAlpha;

  case BR_FADE_TYPE_PALLETE:
    return Br_Fade_MainPallete;

  case BR_FADE_TYPE_MASTERBRIGHT_AND_ALPHA:
    return Br_Fade_MainMasterBrightAndAlpha;

  default:
    GF_ASSERT( 0 );
    return NULL;
  }
}
