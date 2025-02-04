//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_fade.c
 *	@brief  バトルレコーダー用フェードシステム
 *	@author	Toru=Nagihashi
 *	@data		2009.12.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//ライブラリ
#include <gflib.h>

//システム
#include "system/palanm.h"

//外部公開
#include "br_fade.h"
//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
#define BR_FADE_DEFAULT_SYNC    (0)
#define BR_FADE_DEFAULT_EV      (16)
#define BR_FADE_DEFAULT_EV_MAX  (16/GFL_FADE_GetFadeSpeed())

#define BR_FADE_ALPHA_PLANEMASK_M_01  (GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 )
#define BR_FADE_ALPHA_PLANEMASK_M_02  (GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ)
#define BR_FADE_ALPHA_PLANEMASK_S_01  (GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 )
#define BR_FADE_ALPHA_PLANEMASK_S_02  (GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2)

#define BR_FADE_CHANGEFADE_NONE (0xFFFF)
//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================


//-------------------------------------
///	フェード実行関数
//=====================================
typedef BOOL (*BR_FADE_MAINFUNCTION)( BR_FADE_WORK * p_wk, u32 *p_seq );

//-------------------------------------
///	フェードシステム
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
 *					プロトタイプ宣言
*/
//=============================================================================
//実行関数
static BOOL Br_Fade_MainMasterBrightBlack( BR_FADE_WORK *p_wk, u32 *p_seq );
static BOOL Br_Fade_MainMasterBrightWhite( BR_FADE_WORK *p_wk, u32 *p_seq );
static BOOL Br_Fade_MainAlpha( BR_FADE_WORK *p_wk, u32 *p_seq );
static BOOL Br_Fade_MainPallete( BR_FADE_WORK *p_wk, u32 *p_seq );
static BOOL Br_Fade_MainMasterBrightAndAlpha( BR_FADE_WORK *p_wk, u32 *p_seq );

static BOOL Br_Fade_MainChangeFade( BR_FADE_WORK *p_wk, u32 *p_seq );
//その他
static BR_FADE_MAINFUNCTION Br_Fade_Factory( BR_FADE_TYPE type );

//=============================================================================
/**
 *    PUBLIC
 */
//=============================================================================
//----------------------------------------------------------------------------
/** 
 *	@brief  フェードシステム初期化
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
  PaletteFadeWorkAllocSet( p_wk->p_pfd, FADE_MAIN_OBJ, FADE_PAL_ONE_SIZE*14, heapID );
  PaletteFadeWorkAllocSet( p_wk->p_pfd, FADE_SUB_BG, FADE_PAL_ALL_SIZE, heapID );
  PaletteFadeWorkAllocSet( p_wk->p_pfd, FADE_SUB_OBJ, FADE_PAL_ONE_SIZE*14, heapID );
  PaletteTrans_AutoSet( p_wk->p_pfd, TRUE );

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  フェードシステム破棄
 *
 *	@param	BR_FADE_WORK *p_wk ワーク
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
 *	@brief  フェードメイン処理
 *
 *	@param	BR_FADE_WORK *p_wk ワーク
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
 *	@brief  フェード開始
 *
 *	@param	BR_FADE_WORK *p_wk  ワーク
 *	@param	type                フェードの種類
 *	@param  disp                画面
 *	@param  dir                 フェードの方向
 */
//-----------------------------------------------------------------------------
void BR_FADE_StartFade( BR_FADE_WORK *p_wk, BR_FADE_TYPE type, BR_FADE_DISPLAY disp, BR_FADE_DIR  dir )
{ 
  BR_FADE_StartFadeEx( p_wk, type, disp, dir, BR_FADE_DEFAULT_SYNC );
}

//----------------------------------------------------------------------------
/**
 *	@brief  フェード開始  シンク指定版
 *
 *	@param	BR_FADE_WORK *p_wk  ワーク
 *	@param	type  フェードの種類
 *	@param  disp  画面
 *	@param  dir   フェードの方向
 *	@param	sync  シンク
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
 *	@brief  フェード終了検知
 *
 *	@param	const BR_FADE_WORK *cp_wk   ワーク
 *
 *	@return TRUEならばフェード終了  FALSEならべフェード中
 */
//-----------------------------------------------------------------------------
BOOL BR_FADE_IsEnd( const BR_FADE_WORK *cp_wk )
{ 
  return cp_wk->is_end;
}

//----------------------------------------------------------------------------
/**
 *	@brief  画面切り替えように、  フェードイン→コールバックー＞フェードアウトする
 *
 *	@param	BR_FADE_WORK *p_wk                  ワーク
 *	@param	BR_FADE_CHANGEFADE_PARAM *cp_param  引数
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
 *	@brief  フェードで使用するパレットをコピー
 *
 *	@param	BR_FADE_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void BR_FADE_PALETTE_Copy( BR_FADE_WORK *p_wk )
{
  PaletteWorkSet_VramCopy( p_wk->p_pfd, FADE_MAIN_BG, 0, FADE_PAL_ALL_SIZE );
  PaletteWorkSet_VramCopy( p_wk->p_pfd, FADE_MAIN_OBJ, 0, FADE_PAL_ONE_SIZE*14 );
  PaletteWorkSet_VramCopy( p_wk->p_pfd, FADE_SUB_BG, 0, FADE_PAL_ALL_SIZE );
  PaletteWorkSet_VramCopy( p_wk->p_pfd, FADE_SUB_OBJ, 0, FADE_PAL_ONE_SIZE*14 );
}
//----------------------------------------------------------------------------
/**
 *	@brief  パレットフェードで使用する色を設定
 *
 *	@param	BR_FADE_WORK *p_wk  ワーク
 *	@param	rgb                 RGB
 */
//-----------------------------------------------------------------------------
void BR_FADE_PALETTE_SetColor( BR_FADE_WORK *p_wk, GXRgb rgb )
{ 
  p_wk->pfd_color = rgb;
}
//----------------------------------------------------------------------------
/**
 *	@brief  パレットフェードの指定色を転送
 *
 *	@param	BR_FADE_WORK *p_wk  ワーク
 *	@param  画面
 */
//-----------------------------------------------------------------------------
void BR_FADE_PALETTE_TransColor( BR_FADE_WORK *p_wk, BR_FADE_DISPLAY display )
{ 
  //画面による設定
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
 *	@brief  アルファ設定
 *
 *	@param	BR_FADE_WORK *p_wk  ワーク
 *	@param	ev                  アルファ値
 */
//-----------------------------------------------------------------------------
void BR_FADE_ALPHA_SetAlpha( BR_FADE_WORK *p_wk, BR_FADE_DISPLAY display, u8 ev )
{ 
  //上画面
  if( display & BR_FADE_DISPLAY_MAIN )
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
  //下画面
  if( display & BR_FADE_DISPLAY_SUB )
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
 *	@brief  アーカイブをパレットフェードに読み込み
 *
 *	@param	BR_FADE_WORK *p_wk  ワーク
 *	@param	*p_handle           ハンドル
 *	@param	datID               DATAID
 *	@param	type                読み込みタイプ
 *	@param	offset              オフセット  色単位
 *	@param	trans_size          サイズ      バイト単位
 *	@param	heapID              ヒープID
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
 *	@brief  輝度でのフェードメイン処理  黒くなる
 *
 *	@param	BR_FADE_WORK *p_wk  ワーク
 *	@param  u32 seq             シーケンス
 *
 *	@return TRUEならば処理終了  FALSEならば処理中
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
      //画面による設定
      if( p_wk->display & BR_FADE_DISPLAY_MAIN )
      { 
        mode  |= GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN;
      }
      if( p_wk->display & BR_FADE_DISPLAY_SUB )
      { 
        mode  |= GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB;
      }
      //方向による設定
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
 *	@brief  輝度でのフェードメイン処理  白くなる
 *
 *	@param	BR_FADE_WORK *p_wk  ワーク
 *	@param  u32 seq             シーケンス
 *
 *	@return TRUEならば処理終了  FALSEならば処理中
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
      //画面による設定
      if( p_wk->display & BR_FADE_DISPLAY_MAIN )
      { 
        mode  |= GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN;
      }
      if( p_wk->display & BR_FADE_DISPLAY_SUB )
      { 
        mode  |= GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB;
      }
      //方向による設定
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
 *	@brief  アルファでのフェードメイン処理
 *
 *	@param	BR_FADE_WORK *p_wk  ワーク
 *	@param  u32 seq             シーケンス
 *
 *	@return TRUEならば処理終了  FALSEならば処理中
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

      //フェード方向
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

      //上画面
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
      //下画面
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
 *	@brief  パレットでのフェードメイン処理
 *
 *	@param	BR_FADE_WORK *p_wk  ワーク
 *	@param  u32 seq             シーケンス
 *
 *	@return TRUEならば処理終了  FALSEならば処理中
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
      //フェード方向
      if( p_wk->dir == BR_FADE_DIR_IN )
      { 
        ev = BR_FADE_DEFAULT_EV - BR_FADE_DEFAULT_EV * p_wk->cnt / p_wk->max;
      }
      else if( p_wk->dir == BR_FADE_DIR_OUT )
      { 
        ev = 0 + BR_FADE_DEFAULT_EV * p_wk->cnt / p_wk->max;
      }

      //画面による設定
      if( p_wk->display == BR_FADE_DISPLAY_TOUCH_HERE )
      { 
        //TOUCH_HEREのみ
        ColorConceChangePfd( p_wk->p_pfd, FADE_SUB_BG, 0x3000, ev, 0xFFFF );
      }
      else
      { 
        //画面
        if( p_wk->display & BR_FADE_DISPLAY_MAIN )
        { 
          ColorConceChangePfd( p_wk->p_pfd, FADE_MAIN_OBJ, 0x3FFF, ev, p_wk->pfd_color );	///< main	oam
          ColorConceChangePfd( p_wk->p_pfd, FADE_MAIN_BG,  0xBFFF, ev, p_wk->pfd_color );	///< main	bg
        }
        if( p_wk->display & BR_FADE_DISPLAY_SUB )
        { 
          ColorConceChangePfd( p_wk->p_pfd, FADE_SUB_OBJ, 0x3FFF, ev, p_wk->pfd_color );	///< sub	oam
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
 *	@brief  上画面がマスターブライト、下画面がアルファのフェードメイン処理
 *
 *	@param	BR_FADE_WORK *p_wk  ワーク
 *	@param  u32 seq             シーケンス
 *
 *	@return TRUEならば処理終了  FALSEならば処理中
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

      //方向による設定
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

      //フェード方向
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

      //アルファの処理
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
 *	@brief  画面読み替えようのフェード
 *
 *	@param	BR_FADE_WORK *p_wk  ワーク
 *	@param  u32 seq             シーケンス
 *
 *	@return TRUEならば処理終了  FALSEならば処理中
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
 *	@brief  フェードの種類により実行関数を返す
 *
 *	@param	BR_FADE_TYPE  フェードの種類
 *
 *	@return 実行関数
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
