//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_view.c
 *	@brief	�����_���}�b�`�\���֌W
 *	@author	Toru=Nagihashi
 *	@data		2009.11.04
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	lib
#include <gflib.h>

//	SYSTEM
#include "system/gf_date.h"
#include "system/bmp_winframe.h"
#include "sound/pm_sndsys.h"

//	module
#include "pokeicon/pokeicon.h"
#include "ui/print_tool.h"
#include "system/pms_draw.h"
#include "system/nsbtx_to_clwk.h"

//	archive
#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"
#include "wifimatch_gra.naix"
#include "msg/msg_wifi_match.h"
#include "app/app_menu_common.h"
#include "wifi_unionobj_plt.cdat"
#include "wifi_unionobj.naix"
#include "msg/msg_print_tool.h"


//	print
#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"

// mine
#include "wifibattlematch_snd.h"
#include "wifibattlematch_view.h"

//-------------------------------------
///	�g���[�i�[OBJ
//=====================================
#define WBM_CARD_TRAINER_RES_NCGR(x)      (NARC_wifi_unionobj_front00_NCGR+x)
#define WBM_CARD_TRAINER_RES_NCER         (NARC_wifi_unionobj_front00_NCER)
#define WBM_CARD_TRAINER_RES_NANR         (NARC_wifi_unionobj_front00_NANR)
#define WBM_CARD_TRAINER_RES_NCLR         (NARC_wifi_unionobj_wifi_union_obj_NCLR)
#define WBM_CARD_TRAINER_RES_PLT_OFS(x)   (sc_wifi_unionobj_plt[x])

#define WBM_CARD_INIT_POS_X (256)

//-------------------------------------
///	���ʂŎg��
//    �����N�����̐��ɂȂ��Ă���
//=====================================
typedef enum
{ 
  WBM_CARD_RANK_NORMAL,
  WBM_CARD_RANK_COPPER,
  WBM_CARD_RANK_BRONZE,
  WBM_CARD_RANK_SILVER,
  WBM_CARD_RANK_GOLD,
  WBM_CARD_RANK_BLACK,
}WBM_CARD_RANK;

//-------------------------------------
///	�v���g�^�C�v
//=====================================
static WBM_CARD_RANK CalcRank( u32 btl_cnt, u32 win, u32 lose );
static void BG_MainPltAnm( NNS_GFD_DST_TYPE type, u16 *p_buff, u16 *p_cnt, u16 add, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end );

static void Wbm_View_PrintFraction(
					PRINT_UTIL * wk, PRINT_QUE * que, GFL_FONT * font,
					u32 nowPrm, u32 maxPrm, HEAPID heapID );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					���\�[�X�ǂݍ���
 *					  ���ʑf�ނ�ǂݍ��݂܂�
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�萔
//=====================================

//-------------------------------------
///	���\�[�X�Ǘ����[�N
//=====================================
struct _WIFIBATTLEMATCH_VIEW_RESOURCE
{ 
  u32 res[WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_MAX];
  u16 plt[0x10];
  u16 plt_original[0x10];
  u16 plt_next[0x10];
  u16 cnt;
};

//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���\�[�X�ǂݍ���
 *
 *	@param	GFL_CLUNIT *p_unit  CLUNIT
 *	@param  mode                �ǂݍ��݃��[�h
 *	@param	heapID              �q�[�vID 
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_VIEW_RESOURCE *WIFIBATTLEMATCH_VIEW_LoadResource( GFL_CLUNIT *p_unit, WIFIBATTLEMATCH_VIEW_RES_MODE mode, HEAPID heapID )
{ 
  WIFIBATTLEMATCH_VIEW_RESOURCE	*	p_wk;

  //���[�N�쐬
	p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_VIEW_RESOURCE) );
	GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_VIEW_RESOURCE) );
  //�����_���}�b�`���\�[�X�t�@�C���[�[�[�[�[�[�[�[�[�[
	{	
		ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_WIFIMATCH_GRA, GFL_HEAP_LOWID( heapID ) );


    //��--BG--

		//PLT
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifimatch_gra_bg_NCLR,
				PALTYPE_MAIN_BG, 0, 0, GFL_HEAP_LOWID( heapID ) );
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifimatch_gra_bg_NCLR,
				PALTYPE_SUB_BG, 0, 0, GFL_HEAP_LOWID( heapID ) );

		//CHR
    //  CARD,BACK�͋��ʃL����
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_wifimatch_gra_bg_back_NCGR, 
				BG_FRAME_M_BACK, 0, 0, FALSE, GFL_HEAP_LOWID( heapID ) );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_wifimatch_gra_bg_back_NCGR, 
				BG_FRAME_S_BACK, 0, 0, FALSE, GFL_HEAP_LOWID( heapID ) );

		//SCR�ǂݍ���
		WIFIBATTLEMATCH_VIEW_LoadScreen( p_wk, mode, heapID );

    //��--OBJ--
    p_wk->res[ WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_PLT_M ] = 
      GFL_CLGRP_PLTT_Register( p_handle, NARC_wifimatch_gra_obj_NCLR, CLSYS_DRAW_MAIN, PLT_OBJ_M * 0x20, GFL_HEAP_LOWID( heapID ) );
    p_wk->res[ WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CHR_M ] = 
      GFL_CLGRP_CGR_Register( p_handle, NARC_wifimatch_gra_obj_NCGR, FALSE, CLSYS_DRAW_MAIN, GFL_HEAP_LOWID( heapID ) );
    p_wk->res[ WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CEL_M ] = 
      GFL_CLGRP_CELLANIM_Register( p_handle, NARC_wifimatch_gra_obj_NCER, NARC_wifimatch_gra_obj_NANR,
          GFL_HEAP_LOWID( heapID ) );
    p_wk->res[ WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_PLT_S ] = 
      GFL_CLGRP_PLTT_Register( p_handle, NARC_wifimatch_gra_obj_NCLR, CLSYS_DRAW_SUB, PLT_OBJ_S * 0x20, GFL_HEAP_LOWID( heapID ) );
    p_wk->res[ WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CHR_S ] = 
      GFL_CLGRP_CGR_Register( p_handle, NARC_wifimatch_gra_obj_NCGR, FALSE, CLSYS_DRAW_SUB, GFL_HEAP_LOWID( heapID ) );
    p_wk->res[ WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CEL_S ] = 
      GFL_CLGRP_CELLANIM_Register( p_handle, NARC_wifimatch_gra_obj_NCER, NARC_wifimatch_gra_obj_NANR,
          GFL_HEAP_LOWID( heapID ) );


    //�p���b�g�t�F�[�h����F��ۑ����Ă���
    { 
      void *p_buff;
      NNSG2dPaletteData *p_plt;
      const u16 *cp_plt_adrs;

      //���Ƃ̃p���b�g����F����ۑ�
      p_buff  = GFL_ARCHDL_UTIL_LoadPalette( p_handle, NARC_wifimatch_gra_bg_NCLR, &p_plt, heapID );
      cp_plt_adrs = p_plt->pRawData;
      GFL_STD_MemCopy( cp_plt_adrs, p_wk->plt_original, sizeof(u16) * 0x10 );
      GFL_STD_MemCopy( (u8*)cp_plt_adrs + 9 * 0x20, p_wk->plt_next, sizeof(u16) * 0x10 );

      //�p���b�g�j��
      GFL_HEAP_FreeMemory( p_buff );
    }

		GFL_ARC_CloseDataHandle( p_handle );
  }

  //���ʃ��\�[�X�t�@�C���[�[�[�[�[�[�[�[�[�[
  { 
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr,
				PALTYPE_MAIN_BG, PLT_FONT_M*0x20, 0x20, GFL_HEAP_LOWID( heapID ) );
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr,
				PALTYPE_SUB_BG, PLT_FONT_S*0x20, 0x20, GFL_HEAP_LOWID( heapID ) );

    
    GFL_BG_FillCharacter( BG_FRAME_M_TEXT, 0, 1,  CGR_OFS_M_CLEAR );
    BmpWinFrame_GraphicSet( BG_FRAME_M_TEXT, CGR_OFS_M_LIST, PLT_LIST_M, MENU_TYPE_SYSTEM, GFL_HEAP_LOWID( heapID ) );
    BmpWinFrame_GraphicSet( BG_FRAME_M_TEXT, CGR_OFS_M_TEXT, PLT_TEXT_M, MENU_TYPE_SYSTEM, GFL_HEAP_LOWID( heapID ) );
	}


  //�J�[�h�͉��ɂ��炵�Ă���
  GFL_BG_SetScroll( BG_FRAME_M_CARD, GFL_BG_SCROLL_X_SET, -WBM_CARD_INIT_POS_X );
  GFL_BG_SetScroll( BG_FRAME_M_FONT, GFL_BG_SCROLL_X_SET, -WBM_CARD_INIT_POS_X );
  GFL_BG_SetScroll( BG_FRAME_S_CARD, GFL_BG_SCROLL_X_SET, -WBM_CARD_INIT_POS_X );
  GFL_BG_SetScroll( BG_FRAME_S_FONT, GFL_BG_SCROLL_X_SET, -WBM_CARD_INIT_POS_X );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���\�[�X�j��
 *
 *	@param	WIFIBATTLEMATCH_VIEW_RESOURCE *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_VIEW_UnLoadResource( WIFIBATTLEMATCH_VIEW_RESOURCE *p_wk )
{ 
  //���\�[�X���
  { 
    GFL_CLGRP_PLTT_Release( p_wk->res[ WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_PLT_M ] );
    GFL_CLGRP_CGR_Release( p_wk->res[ WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CHR_M ]  );
    GFL_CLGRP_CELLANIM_Release( p_wk->res[ WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CEL_M ] );
    GFL_CLGRP_PLTT_Release( p_wk->res[ WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_PLT_S ] );
    GFL_CLGRP_CGR_Release( p_wk->res[ WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CHR_S ]  );
    GFL_CLGRP_CELLANIM_Release( p_wk->res[ WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CEL_S ] );

    GFL_BG_FillCharacterRelease(BG_FRAME_M_TEXT, 1, CGR_OFS_M_CLEAR ); 
  }

  //���[�N�j��
	GFL_HEAP_FreeMemory( p_wk ); 
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�N���[���ǂݍ���
 *	        �L�����͋��ʂȂ̂ŃX�N���[���ǂݍ��݂����Ŕw�i�̍����ւ����ł���
 *
 *	@param	WIFIBATTLEMATCH_VIEW_RESOURCE *p_wk   ���[�N
 *	@param	mode                                  �ǂݍ��ރ^�C�v
 *	@param	heapID                                �q�[�vID
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_VIEW_LoadScreen( WIFIBATTLEMATCH_VIEW_RESOURCE *p_wk, WIFIBATTLEMATCH_VIEW_RES_MODE mode, HEAPID heapID )
{ 
  ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_WIFIMATCH_GRA, GFL_HEAP_LOWID( heapID ) );

  //SCR
  GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_wifimatch_gra_back_NSCR,
      BG_FRAME_M_BACK, 0, 0, FALSE, GFL_HEAP_LOWID( heapID ) );
  GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_wifimatch_gra_back2_NSCR,
      BG_FRAME_S_BACK, 0, 0, FALSE, GFL_HEAP_LOWID( heapID ) );



  switch( mode )
  { 
  case WIFIBATTLEMATCH_VIEW_RES_MODE_RANDOM:
    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_wifimatch_gra_random_card_NSCR,
        BG_FRAME_S_CARD, 0, 0, FALSE, GFL_HEAP_LOWID( heapID ) );
    break;

  case WIFIBATTLEMATCH_VIEW_RES_MODE_DIGITALCARD:
    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_wifimatch_gra_wcs_card_NSCR,
        BG_FRAME_M_CARD, 0, 0, FALSE, GFL_HEAP_LOWID( heapID ) );
    break;

  case WIFIBATTLEMATCH_VIEW_RES_MODE_WIFI:
    /* fallthr */
  case WIFIBATTLEMATCH_VIEW_RES_MODE_LIVE:
    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_wifimatch_gra_wcs_card_NSCR,
        BG_FRAME_S_CARD, 0, 0, FALSE, GFL_HEAP_LOWID( heapID ) );
    break;
  }

  //�J�[�h�͉��ɂ��炵�Ă���
  GFL_BG_SetScroll( BG_FRAME_M_CARD, GFL_BG_SCROLL_X_SET, -WBM_CARD_INIT_POS_X );
  GFL_BG_SetScroll( BG_FRAME_M_FONT, GFL_BG_SCROLL_X_SET, -WBM_CARD_INIT_POS_X );
  GFL_BG_SetScroll( BG_FRAME_S_CARD, GFL_BG_SCROLL_X_SET, -WBM_CARD_INIT_POS_X );
  GFL_BG_SetScroll( BG_FRAME_S_FONT, GFL_BG_SCROLL_X_SET, -WBM_CARD_INIT_POS_X );


  GFL_ARC_CloseDataHandle( p_handle );
}
 

//----------------------------------------------------------------------------
/**
 *	@brief  ���\�[�X�擾
 *
 *	@param	const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_wk  ���[�N
 *	@param	type  
 *
 *	@return
 */
//-----------------------------------------------------------------------------
u32 WIFIBATTLEMATCH_VIEW_GetResource( const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_wk, WIFIBATTLEMATCH_VIEW_RES_TYPE type )
{ 
  return cp_wk->res[ type ];
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���C��
 *
 *	@param	WIFIBATTLEMATCH_VIEW_RESOURCE *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_VIEW_Main( WIFIBATTLEMATCH_VIEW_RESOURCE *p_wk )
{ 
  int i;
  const u16 add = 0x200;

  //�p���b�g�A�j��
  if( p_wk->cnt + add >= 0x10000 )
  {
    p_wk->cnt = p_wk->cnt+add-0x10000;
  }
  else
  {
    p_wk->cnt += add;
  }

  for( i = 0; i < 0x10; i++ )
  { 
    BG_MainPltAnm( NNS_GFD_DST_2D_BG_PLTT_MAIN, &p_wk->plt[i], &p_wk->cnt, 0x0, PLT_BG_M, i, p_wk->plt_original[i], p_wk->plt_next[i] );
    BG_MainPltAnm( NNS_GFD_DST_2D_BG_PLTT_SUB, &p_wk->plt[i], &p_wk->cnt, 0x0, PLT_BG_S, i, p_wk->plt_original[i], p_wk->plt_next[i] );
    BG_MainPltAnm( NNS_GFD_DST_2D_OBJ_PLTT_MAIN, &p_wk->plt[i], &p_wk->cnt, 0x0, 2, i, p_wk->plt_original[i], p_wk->plt_next[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  BG�p���b�g�t�F�[�h
 *
 *  @param  u16 *p_buff �F�ۑ��o�b�t�@
 *  @param  *p_cnt      �J�E���^�o�b�t�@
 *  @param  add         �J�E���^���Z�l
 *  @param  plt_num     �p���b�g�c�ԍ�
 *  @param  plt_col     �p���b�g���ԍ�
 *  @param  start       �J�n�F
 *  @param  end         �I���F
 */
//-----------------------------------------------------------------------------
static void BG_MainPltAnm( NNS_GFD_DST_TYPE type, u16 *p_buff, u16 *p_cnt, u16 add, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end )
{ 

  {
    //1�`0�ɕϊ�
    const fx16 cos = (FX_CosIdx(*p_cnt)+FX16_ONE)/2;
    const u8 start_r  = (start & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    const u8 start_g  = (start & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    const u8 start_b  = (start & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
    const u8 end_r  = (end & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    const u8 end_g  = (end & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    const u8 end_b  = (end & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

    const u8 r = start_r + (((end_r-start_r)*cos)>>FX16_SHIFT);
    const u8 g = start_g + (((end_g-start_g)*cos)>>FX16_SHIFT);
    const u8 b = start_b + (((end_b-start_b)*cos)>>FX16_SHIFT);
    
    *p_buff = GX_RGB(r, g, b);
    
    NNS_GfdRegisterNewVramTransferTask( type,
                                        plt_num * 32 + plt_col * 2,
                                        p_buff, 2 );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ?? / ?? ��\��
 *
 *	@param	PRINT_UTIL *p_util  PRINT_UTIL
 *	@param	*p_que              PRINT_QUE
 *	@param	*p_font             �t�H���g
 *	@param	now                 ���ݒl
 *	@param	max                 �ő�l
 *	@param	heapID              �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Wbm_View_PrintFraction(
					PRINT_UTIL * wk, PRINT_QUE * que, GFL_FONT * font,
					u32 nowPrm, u32 maxPrm, HEAPID heapID )
{ 
	GFL_MSGDATA * mman;
	WORDSET * wset;
	STRBUF * str;
	STRBUF * buf;
	u16	slash_size, now_size, max_size;
  u16 x, y;

	mman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_print_tool_dat, heapID );
	wset = WORDSET_Create( heapID );
	buf  = GFL_STR_CreateBuffer( 32, heapID );

  x = GFL_BMPWIN_GetSizeX( wk->win ) * 8;
  y = 0;

	// maxPrm
	str = GFL_MSG_CreateString( mman, str_max_param );
	WORDSET_RegisterNumber( wset, 0, maxPrm, 8, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	WORDSET_ExpandStr( wset, buf, str );
	max_size = PRINTSYS_GetStrWidth( buf, font, 0 );
	PRINT_UTIL_Print( wk, que, x-max_size, y, buf, font );
	GFL_STR_DeleteBuffer( str );

	//�u�^�v
	str = GFL_MSG_CreateString( mman, str_slash );
	slash_size = PRINTSYS_GetStrWidth( str, font, 0 );
	PRINT_UTIL_Print( wk, que, x-max_size-slash_size, y, str, font );
	GFL_STR_DeleteBuffer( str );

	// nowPrm
	str = GFL_MSG_CreateString( mman, str_now_param );
	WORDSET_RegisterNumber( wset, 0, nowPrm, 8, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	WORDSET_ExpandStr( wset, buf, str );
	now_size = PRINTSYS_GetStrWidth( buf, font, 0 );
	PRINT_UTIL_Print( wk, que, x-max_size-slash_size-now_size, y, buf, font );
	GFL_STR_DeleteBuffer( str );

	GFL_STR_DeleteBuffer( buf );
	WORDSET_Delete( wset );
	GFL_MSG_Delete( mman );

}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�v���C���[���\��
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
enum
{	
	PLAYERINFO_BMPWIN_MAX	= 30,
	PLAYERINFO_STAR_MAX	  = 5,
};
enum
{	
	PLAYERINFO_CLWK_POKEICON_TOP,
	PLAYERINFO_CLWK_POKEICON_END	= PLAYERINFO_CLWK_POKEICON_TOP + TEMOTI_POKEMAX,
	PLAYERINFO_CLWK_TRAINER,
	PLAYERINFO_CLWK_LOCK,
  PLAYERINFO_CLWK_STAR_TOP,
  PLAYERINFO_CLWK_STAR_END = PLAYERINFO_CLWK_STAR_TOP + PLAYERINFO_STAR_MAX,
	PLAYERINFO_CLWK_ITEM_TOP,
	PLAYERINFO_CLWK_ITEM_END  = PLAYERINFO_CLWK_ITEM_TOP + TEMOTI_POKEMAX,
	PLAYERINFO_CLWK_MAX,
};
enum
{	
	PLAYERINFO_RESID_POKEICON_CGR_TOP,
	PLAYERINFO_RESID_POKEICON_CGR_END	= PLAYERINFO_RESID_POKEICON_CGR_TOP+TEMOTI_POKEMAX,
	PLAYERINFO_RESID_POKEICON_CELL,
	PLAYERINFO_RESID_POKEICON_PLT,
	PLAYERINFO_RESID_TRAINER_PLT,
	PLAYERINFO_RESID_TRAINER_CGR,
	PLAYERINFO_RESID_TRAINER_CELL,

	PLAYERINFO_RESID_ITEM_PLT,
	PLAYERINFO_RESID_ITEM_CGR,
	PLAYERINFO_RESID_ITEM_CELL,
	PLAYERINFO_RESID_MAX,
};

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�萔
//=====================================
#define CARD_WAIT_SYNC (20)

//-------------------------------------
///	�v���C���[���\�����[�N
//=====================================
struct _PLAYERINFO_WORK
{	
	u32					bmpwin_max;
	GFL_BMPWIN	*p_bmpwin[ PLAYERINFO_BMPWIN_MAX ];
	PRINT_UTIL	print_util[ PLAYERINFO_BMPWIN_MAX ];
	GFL_CLWK		*p_clwk[ PLAYERINFO_CLWK_MAX ];
	u32					res[PLAYERINFO_RESID_MAX];
  u32         cnt;
  s16         clwk_x[ PLAYERINFO_CLWK_MAX ];
  u8          frm;
  u8          plt;
  u8          pokemonicon_plt;
  u8          trainer_plt;
  CLSYS_DRAW_TYPE cl_draw_type;
  REGULATION_VIEWDATA * p_reg_view;
  HEAPID      heapID;
  GFL_CLUNIT *p_unit;
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	BMPWIN�쐬
//=====================================
static void PlayerInfo_Bmpwin_Rnd_Create( PLAYERINFO_WORK * p_wk, BOOL is_rate, const PLAYERINFO_RANDOMMATCH_DATA *cp_data, const MYSTATUS* p_my, GFL_FONT *p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, BOOL is_black, HEAPID heapID );
static void PlayerInfo_Bmpwin_Wifi_Create( PLAYERINFO_WORK * p_wk, BOOL is_limit, const PLAYERINFO_WIFICUP_DATA *cp_data, const MYSTATUS* p_my, GFL_FONT *p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, HEAPID heapID );
static void PlayerInfo_Bmpwin_Live_Create( PLAYERINFO_WORK * p_wk, const PLAYERINFO_LIVECUP_DATA *cp_data,const MYSTATUS* p_my, GFL_FONT *p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, HEAPID heapID );
static BOOL PlayerInfo_Bmpwin_PrintMain( PLAYERINFO_WORK * p_wk, PRINT_QUE *p_que );
static void PlayerInfo_Bmpwin_Delete( PLAYERINFO_WORK * p_wk );
//-------------------------------------
///	CLWK�쐬
//=====================================
static void PlayerInfo_POKEICON_Create( PLAYERINFO_WORK * p_wk, GFL_CLUNIT *p_unit, REGULATION_VIEWDATA * p_reg_view, HEAPID heapID );
static void PlayerInfo_POKEICON_Delete( PLAYERINFO_WORK * p_wk );

static void PlayerInfo_TRAINER_Cleate( PLAYERINFO_WORK * p_wk, u32 trainerID, GFL_CLUNIT *p_unit, HEAPID heapID );
static void PlayerInfo_TRAINER_Delete( PLAYERINFO_WORK * p_wk );

static void PlayerInfo_LOCK_Cleate( PLAYERINFO_WORK * p_wk, GFL_CLUNIT *p_unit, const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, HEAPID heapID );
static void PlayerInfo_LOCK_Delete( PLAYERINFO_WORK * p_wk );

static void PlayerInfo_STAR_Cleate( PLAYERINFO_WORK * p_wk, const PLAYERINFO_RANDOMMATCH_DATA *cp_data, GFL_CLUNIT *p_unit, const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, HEAPID heapID );
static void PlayerInfo_STAR_Delete( PLAYERINFO_WORK * p_wk );
//=============================================================================
/**
 *					�����_���}�b�`�O�����J�֐�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�����_���}�b�`�v���C���[���\��	������
 *
 *	@param	cp_data							      ���\����
 *	@param	*p_unit										�Z���쐬�̂��߂̃��j�b�g
 *	@param	PRINT_QUE *p_que					�����\���p�L���[
 *	@param	*p_msg										�����\���p���b�Z�[�W�f�[�^
 *	@param	*p_word										�����\���p�P��o�^
 *	@param	heapID										�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
PLAYERINFO_WORK *PLAYERINFO_RND_Init( const PLAYERINFO_RANDOMMATCH_DATA *cp_data, BOOL is_rate, const MYSTATUS* p_my, GFL_CLUNIT *p_unit, const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, GFL_FONT	*p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, HEAPID heapID )
{
  PLAYERINFO_WORK	*	p_wk;
  WBM_CARD_RANK rank;

  //���[�N�쐬
	p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(PLAYERINFO_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(PLAYERINFO_WORK) );

  p_wk->frm = PLAYERINFO_BG_FRAME;
  p_wk->plt = PLAYERINFO_PLT_BG_FONT;
  p_wk->pokemonicon_plt = PLAYERINFO_PLT_OBJ_POKEICON;
  p_wk->trainer_plt = PLAYERINFO_PLT_OBJ_TRAINER;
  p_wk->cl_draw_type  = CLSYS_DRAW_SUB;

  G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, PLAYERINFO_ALPHA_EV1, PLAYERINFO_ALPHA_EV2);

  //BMPWIN�쐬
  rank  = CalcRank( cp_data->btl_cnt, cp_data->win_cnt, cp_data->lose_cnt );
  PlayerInfo_Bmpwin_Rnd_Create( p_wk, is_rate, cp_data, p_my, p_font, p_que, p_msg, p_word, rank == WBM_CARD_RANK_BLACK, heapID );
  //�g���[�i�[�쐬
  PlayerInfo_TRAINER_Cleate( p_wk, cp_data->trainerID, p_unit, heapID );
  {	
    GFL_CLACTPOS	pos;
    pos.x	= PLAYERINFO_TRAINER_RND_X + WBM_CARD_INIT_POS_X;
    pos.y = PLAYERINFO_TRAINER_RND_Y;
    GFL_CLACT_WK_SetPos( p_wk->p_clwk[ PLAYERINFO_CLWK_TRAINER ], &pos, CLSYS_DRAW_SUB );
  }
  //���쐬
  PlayerInfo_STAR_Cleate( p_wk, cp_data, p_unit, cp_res, heapID );

  { 
    GFL_BG_ChangeScreenPalette( BG_FRAME_S_CARD, 0, 0, 32, 6, 1 );
    GFL_BG_ChangeScreenPalette( BG_FRAME_S_CARD, 0, 6, 32, 18, 3 + rank );
    GFL_BG_LoadScreenReq( BG_FRAME_S_CARD );
  }

  G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, PLAYERINFO_ALPHA_EV1, PLAYERINFO_ALPHA_EV2);

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`�v���C���[���\��  �j��
 *
 *	@param	PLAYERINFO_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void PLAYERINFO_RND_Exit( PLAYERINFO_WORK *p_wk )
{ 
  G2S_BlendNone();

  //���j��
  PlayerInfo_STAR_Delete( p_wk );

  //�g���[�i�[�j��
  PlayerInfo_TRAINER_Delete( p_wk );

  //BMPWIN�j��
  PlayerInfo_Bmpwin_Delete( p_wk );

  //���[�N�j��
	GFL_HEAP_FreeMemory( p_wk );
}

//=============================================================================
/**
 *					WIFI���O�����J�֐�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	WIFI���v���C���[���\��	������
 *
 *	@param	cp_data							      ���\����
 *	@param	*p_unit										�Z���쐬�̂��߂̃��j�b�g
 *	@param	PRINT_QUE *p_que					�����\���p�L���[
 *	@param	*p_msg										�����\���p���b�Z�[�W�f�[�^
 *	@param	*p_word										�����\���p�P��o�^
 *	@param  p_btl_box                 �o�g���{�b�N�X�Z�[�u�f�[�^
 *	@param	heapID										�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
PLAYERINFO_WORK *PLAYERINFO_WIFI_Init( const PLAYERINFO_WIFICUP_DATA *cp_data, BOOL is_limit, const MYSTATUS* p_my, GFL_CLUNIT *p_unit, const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, GFL_FONT	*p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, REGULATION_VIEWDATA * p_reg_view, BOOL is_main, HEAPID heapID )
{ 
  u16 card_frm;

  PLAYERINFO_WORK	*	p_wk;

  //���[�N�쐬
	p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(PLAYERINFO_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(PLAYERINFO_WORK) );
  p_wk->p_reg_view  = p_reg_view;
  p_wk->heapID      = heapID;
  p_wk->p_unit      = p_unit;

  //���C���ƃT�u�œǂݍ��ݐ�ύX
  if( is_main )
  { 
    card_frm  = BG_FRAME_M_CARD;

    p_wk->frm = PLAYERINFO_BG_FRAME_MAIN;
    p_wk->plt = PLAYERINFO_PLT_BG_FONT_MAIN;
    p_wk->pokemonicon_plt = PLAYERINFO_PLT_OBJ_POKEICON_MAIN;
    p_wk->trainer_plt = PLAYERINFO_PLT_OBJ_TRAINER_MAIN;
    p_wk->cl_draw_type  = CLSYS_DRAW_MAIN;

    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, PLAYERINFO_ALPHA_EV1, PLAYERINFO_ALPHA_EV2);
  }
  else
  { 
    card_frm  = BG_FRAME_S_CARD;

    p_wk->frm = PLAYERINFO_BG_FRAME;
    p_wk->plt = PLAYERINFO_PLT_BG_FONT;
    p_wk->pokemonicon_plt = PLAYERINFO_PLT_OBJ_POKEICON;
    p_wk->trainer_plt = PLAYERINFO_PLT_OBJ_TRAINER;
    p_wk->cl_draw_type  = CLSYS_DRAW_SUB;

    G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, PLAYERINFO_ALPHA_EV1, PLAYERINFO_ALPHA_EV2);
  }

  switch( cp_data->bgm_no )
  { 
  case REGULATION_CARD_BGM_TRAINER:
    GFL_BG_ChangeScreenPalette( card_frm, 0, 0, 32, 5, 1 );
    GFL_BG_ChangeScreenPalette( card_frm, 0, 5, 32, 18, 3 );
    GFL_BG_LoadScreenReq( card_frm );
    break;
  case REGULATION_CARD_BGM_WCS:
    GFL_BG_ChangeScreenPalette( card_frm, 0, 0, 32, 5, 1 );
    GFL_BG_ChangeScreenPalette( card_frm, 0, 5, 32, 18, 7 );
    GFL_BG_LoadScreenReq( card_frm );
    break;
  }


  //BMPWIN�쐬
  PlayerInfo_Bmpwin_Wifi_Create( p_wk, is_limit, cp_data, p_my, p_font, p_que, p_msg, p_word, heapID );

  //�|�P�A�C�R���쐬
  PlayerInfo_POKEICON_Create( p_wk, p_unit, p_reg_view, heapID );

  //�g���[�i�[�쐬
  PlayerInfo_TRAINER_Cleate( p_wk, cp_data->trainerID, p_unit, heapID );
  {	
    GFL_CLACTPOS	pos;
    pos.x	= PLAYERINFO_TRAINER_CUP_X + WBM_CARD_INIT_POS_X;
    pos.y = PLAYERINFO_TRAINER_CUP_Y;
    GFL_CLACT_WK_SetPos( p_wk->p_clwk[ PLAYERINFO_CLWK_TRAINER ], &pos, p_wk->cl_draw_type );
  }

  //���쐬
  PlayerInfo_LOCK_Cleate( p_wk, p_unit, cp_res, heapID );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���v���C���[���\��  �j��
 *
 *	@param	PLAYERINFO_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void PLAYERINFO_WIFI_Exit( PLAYERINFO_WORK *p_wk )
{ 
  G2S_BlendNone();

  PlayerInfo_LOCK_Delete( p_wk );

  PlayerInfo_TRAINER_Delete( p_wk );

	PlayerInfo_POKEICON_Delete( p_wk );

	PlayerInfo_Bmpwin_Delete( p_wk );

	GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���v���C���[���  �f�[�^�X�V
 *
 *	@param	PLAYERINFO_WORK *p_wk ���[�N
 *	@param	type                  �X�V�^�C�v
 *
 */
//-----------------------------------------------------------------------------
void PLAYERINFO_WIFI_RenewalData( PLAYERINFO_WORK *p_wk, PLAYERINFO_WIFI_UPDATE_TYPE type, GFL_MSGDATA *p_msg, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID )
{ 
  int i;
  STRBUF	*p_strbuf;

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_wk->p_bmpwin[7]), 0 );

  PlayerInfo_POKEICON_Delete( p_wk );
  PlayerInfo_POKEICON_Create( p_wk, p_wk->p_unit, p_wk->p_reg_view, p_wk->heapID );

  { 
    GFL_CLACTPOS  pos;
    int x = GFL_BG_GetScrollX( p_wk->frm );
    for( i = 0; i < TEMOTI_POKEMAX; i++ )
    {
      pos.y = PLAYERINFO_POKEICON_Y;
      pos.x = -x + PLAYERINFO_POKEICON_START_X + PLAYERINFO_POKEICON_DIFF_X*i;
      if( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i] )
      { 
        GFL_CLACT_WK_SetPos( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i], &pos, p_wk->cl_draw_type );
      }
      if( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP+i] )
      { 
        pos.x += PLAYERINFO_POKEITEM_X_OFS;
        pos.y += PLAYERINFO_POKEITEM_Y_OFS;
        GFL_CLACT_WK_SetPos( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP+i], &pos, p_wk->cl_draw_type );
      }
    }
  }

  switch( type )
  { 
  case PLAYERINFO_WIFI_UPDATE_TYPE_LOCK:
    for( i = 0; i < TEMOTI_POKEMAX; i++ )
		{	
      if( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP+i] )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP+i], TRUE );
      }
      if( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i] )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i], TRUE );
      }
		}
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_LOCK], TRUE );

    p_strbuf  = GFL_MSG_CreateString( p_msg, WIFIMATCH_STR_017 );
    PRINT_UTIL_PrintColor( &p_wk->print_util[7], p_que, 0, 0, p_strbuf, p_font, PLAYERINFO_STR_COLOR_WHITE );
    break;

  case PLAYERINFO_WIFI_UPDATE_TYPE_UNLOCK:
    for( i = 0; i < TEMOTI_POKEMAX; i++ )
		{	
      if( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP+i] )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP+i], TRUE );
      }
      if( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i] )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i], TRUE );
      }
		}
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_LOCK], FALSE );

    p_strbuf  = GFL_MSG_CreateString( p_msg, WIFIMATCH_STR_017_01 );
    PRINT_UTIL_PrintColor( &p_wk->print_util[7], p_que, 0, 0, p_strbuf, p_font, PLAYERINFO_STR_COLOR_WHITE );
    break;

  case PLAYERINFO_WIFI_UPDATE_TYPE_UNREGISTER:
    for( i = 0; i < TEMOTI_POKEMAX; i++ )
		{	
      if( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP+i] )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP+i], FALSE );
      }
      if( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i] )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i], FALSE );
      }
		}
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_LOCK], FALSE );

    p_strbuf  = GFL_MSG_CreateString( p_msg, WIFIMATCH_STR_017_02 );
    PRINT_UTIL_PrintColor( &p_wk->print_util[7], p_que, 0, 0, p_strbuf, p_font, PLAYERINFO_STR_COLOR_WHITE );
    break;
  }

  GFL_STR_DeleteBuffer( p_strbuf );
}
//=============================================================================
/**
 *					LIVE���O�����J�֐�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	LIVE���v���C���[���\��	������
 *
 *	@param	cp_data							      ���\����
 *	@param	*p_unit										�Z���쐬�̂��߂̃��j�b�g
 *	@param	PRINT_QUE *p_que					�����\���p�L���[
 *	@param	*p_msg										�����\���p���b�Z�[�W�f�[�^
 *	@param	*p_word										�����\���p�P��o�^
 *	@param	heapID										�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
PLAYERINFO_WORK *PLAYERINFO_LIVE_Init( const PLAYERINFO_LIVECUP_DATA *cp_data, const MYSTATUS* p_my, GFL_CLUNIT *p_unit, const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, GFL_FONT *p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, REGULATION_VIEWDATA * p_reg_view, BOOL is_main, HEAPID heapID )
{ 
  u16 card_frm;

  PLAYERINFO_WORK	*	p_wk;

  //���[�N�쐬
	p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(PLAYERINFO_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(PLAYERINFO_WORK) );
  p_wk->p_reg_view  = p_reg_view; 
  p_wk->heapID      = heapID;
  p_wk->p_unit      = p_unit;

  //���C���ƃT�u�œǂݍ��ݐ�ύX
  if( is_main )
  { 
    card_frm  = BG_FRAME_M_CARD;

    p_wk->frm = PLAYERINFO_BG_FRAME_MAIN;
    p_wk->plt = PLAYERINFO_PLT_BG_FONT_MAIN;
    p_wk->pokemonicon_plt = PLAYERINFO_PLT_OBJ_POKEICON_MAIN;
    p_wk->trainer_plt = PLAYERINFO_PLT_OBJ_TRAINER_MAIN;
    p_wk->cl_draw_type  = CLSYS_DRAW_MAIN;

    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, PLAYERINFO_ALPHA_EV1, PLAYERINFO_ALPHA_EV2);
  }
  else
  { 
    card_frm  = BG_FRAME_S_CARD;

    p_wk->frm = PLAYERINFO_BG_FRAME;
    p_wk->plt = PLAYERINFO_PLT_BG_FONT;
    p_wk->pokemonicon_plt = PLAYERINFO_PLT_OBJ_POKEICON;
    p_wk->trainer_plt = PLAYERINFO_PLT_OBJ_TRAINER;
    p_wk->cl_draw_type  = CLSYS_DRAW_SUB;

    G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, PLAYERINFO_ALPHA_EV1, PLAYERINFO_ALPHA_EV2);
  }

  switch( cp_data->bgm_no )
  { 
  case REGULATION_CARD_BGM_TRAINER:
    GFL_BG_ChangeScreenPalette( card_frm, 0, 0, 32, 5, 1 );
    GFL_BG_ChangeScreenPalette( card_frm, 0, 5, 32, 18, 3 );
    GFL_BG_LoadScreenReq( card_frm );
    break;
  case REGULATION_CARD_BGM_WCS:
    GFL_BG_ChangeScreenPalette( card_frm, 0, 0, 32, 5, 1 );
    GFL_BG_ChangeScreenPalette( card_frm, 0, 5, 32, 18, 7 );
    GFL_BG_LoadScreenReq( card_frm );
    break;
  }



  //BMPWIN�쐬
  PlayerInfo_Bmpwin_Live_Create( p_wk, cp_data, p_my, p_font, p_que, p_msg, p_word, heapID );

  //�|�P�A�C�R���쐬
  PlayerInfo_POKEICON_Create( p_wk, p_unit, p_reg_view, heapID );

  //�g���[�i�[�쐬
  PlayerInfo_TRAINER_Cleate( p_wk, cp_data->trainerID, p_unit, heapID );
  {	
    GFL_CLACTPOS	pos;
    pos.x	= PLAYERINFO_TRAINER_CUP_X + WBM_CARD_INIT_POS_X;
    pos.y = PLAYERINFO_TRAINER_CUP_Y;
    GFL_CLACT_WK_SetPos( p_wk->p_clwk[ PLAYERINFO_CLWK_TRAINER ], &pos, p_wk->cl_draw_type );
  }

  //���쐬
  PlayerInfo_LOCK_Cleate( p_wk, p_unit, cp_res, heapID );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  LIVE���v���C���[���\��  �j��
 *
 *	@param	PLAYERINFO_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void PLAYERINFO_LIVE_Exit( PLAYERINFO_WORK *p_wk )
{ 
  G2S_BlendNone();

  PlayerInfo_LOCK_Delete( p_wk );

  PlayerInfo_TRAINER_Delete( p_wk );

	PlayerInfo_POKEICON_Delete( p_wk );

	PlayerInfo_Bmpwin_Delete( p_wk );

	GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  LIVE���v���C���[���  �f�[�^�X�V
 *
 *	@param	PLAYERINFO_WORK *p_wk ���[�N
 *	@param	type                  �X�V�^�C�v
 *
 */
//-----------------------------------------------------------------------------
void PLAYERINFO_LIVE_RenewalData( PLAYERINFO_WORK *p_wk, PLAYERINFO_WIFI_UPDATE_TYPE type, GFL_MSGDATA *p_msg, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID )
{ 
  int i;
  STRBUF	*p_strbuf;

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_wk->p_bmpwin[9]), 0 );

  PlayerInfo_POKEICON_Delete( p_wk );
  PlayerInfo_POKEICON_Create( p_wk, p_wk->p_unit, p_wk->p_reg_view, p_wk->heapID );


  { 
    GFL_CLACTPOS  pos;
    int x = GFL_BG_GetScrollX( p_wk->frm );
    for( i = 0; i < TEMOTI_POKEMAX; i++ )
    {
      pos.y = PLAYERINFO_POKEICON_Y;
      pos.x = -x + PLAYERINFO_POKEICON_START_X + PLAYERINFO_POKEICON_DIFF_X*i;
      if( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i] )
      { 
        GFL_CLACT_WK_SetPos( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i], &pos, p_wk->cl_draw_type );
      }
      if( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP+i] )
      { 
        pos.x += PLAYERINFO_POKEITEM_X_OFS;
        pos.y += PLAYERINFO_POKEITEM_Y_OFS;
        GFL_CLACT_WK_SetPos( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP+i], &pos, p_wk->cl_draw_type );
      }
    }
  }

  switch( type )
  { 
  case PLAYERINFO_WIFI_UPDATE_TYPE_LOCK:
    for( i = 0; i < TEMOTI_POKEMAX; i++ )
		{	
      if( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP+i] )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP+i], TRUE );
      }
      if( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i] )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i], TRUE );
      }
		}
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_LOCK], TRUE );

    p_strbuf  = GFL_MSG_CreateString( p_msg, WIFIMATCH_STR_017 );
    PRINT_UTIL_PrintColor( &p_wk->print_util[9], p_que, 0, 0, p_strbuf, p_font, PLAYERINFO_STR_COLOR_WHITE );
    break;

  case PLAYERINFO_WIFI_UPDATE_TYPE_UNLOCK:
    for( i = 0; i < TEMOTI_POKEMAX; i++ )
		{	
      if( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP+i] )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP+i], TRUE );
      }
      if( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i] )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i], TRUE );
      }
		}
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_LOCK], FALSE );

    p_strbuf  = GFL_MSG_CreateString( p_msg, WIFIMATCH_STR_017_01 );
    PRINT_UTIL_PrintColor( &p_wk->print_util[9], p_que, 0, 0, p_strbuf, p_font, PLAYERINFO_STR_COLOR_WHITE );
    break;

  case PLAYERINFO_WIFI_UPDATE_TYPE_UNREGISTER:
    for( i = 0; i < TEMOTI_POKEMAX; i++ )
		{	
      if( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP+i] )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP+i], FALSE );
      }
      if( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i] )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i], FALSE );
      }
		}
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[PLAYERINFO_CLWK_LOCK], FALSE );

    p_strbuf  = GFL_MSG_CreateString( p_msg, WIFIMATCH_STR_017_02 );
    PRINT_UTIL_PrintColor( &p_wk->print_util[9], p_que, 0, 0, p_strbuf, p_font, PLAYERINFO_STR_COLOR_WHITE );
    break;
  }

  GFL_STR_DeleteBuffer( p_strbuf );
}
//=============================================================================
/**
 *					���ʊO�����J�֐�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�v�����g���C��
 *
 *	@param	PLAYERINFO_WORK * p_wk	���[�N
 *	@param	PRINT_QUE	*p_que				�v�����g�L���[
 *
 *	@return	TRUE�őS�Ẵv�����g�I��	FALSE�ł܂�
 */
//-----------------------------------------------------------------------------
BOOL PLAYERINFO_PrintMain( PLAYERINFO_WORK * p_wk, PRINT_QUE *p_que )
{	
 	return PlayerInfo_Bmpwin_PrintMain( p_wk, p_que );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �J�[�h���X���C�h�C��
 *
 *	@param	PLAYERINFO_WORK *p_wk  ���[�N
 *
 *	@return TRUE�Ŋ���  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL PLAYERINFO_MoveMain( PLAYERINFO_WORK * p_wk )
{ 
  if( GFL_BG_GetScrollX( p_wk->frm ) == 0 )
  { 
    return TRUE;
  }
  else
  { 
    BOOL ret  = FALSE;
  
    s32 x;

    if( p_wk->cnt == 0 )
    { 
      int i;
      GFL_CLACTPOS  pos;
      for( i = 0; i < PLAYERINFO_CLWK_MAX; i++ )
      { 
        if( p_wk->p_clwk[i] )
        { 
          GFL_CLACT_WK_GetPos( p_wk->p_clwk[i], &pos, p_wk->cl_draw_type );
          p_wk->clwk_x[i]  = pos.x;
        }
      }

      PMSND_PlaySE( WBM_SND_SE_CARD_SLIDE );
    }

    x = -WBM_CARD_INIT_POS_X + WBM_CARD_INIT_POS_X * p_wk->cnt / CARD_WAIT_SYNC;

    { 
      int i;
      GFL_CLACTPOS  pos;
      for( i = 0; i < PLAYERINFO_CLWK_MAX; i++ )
      { 
        if( p_wk->p_clwk[i] )
        { 
          GFL_CLACT_WK_GetPos( p_wk->p_clwk[i], &pos, p_wk->cl_draw_type );
          pos.x = p_wk->clwk_x[i] - WBM_CARD_INIT_POS_X * p_wk->cnt / CARD_WAIT_SYNC;
          GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &pos, p_wk->cl_draw_type );
        }
      }
    }

    if( p_wk->cnt++ >= CARD_WAIT_SYNC )
    { 
      x = 0;
      p_wk->cnt = 0;
      ret = TRUE;
    }

    GFL_BG_SetScrollReq( p_wk->frm+1, GFL_BG_SCROLL_X_SET, x );
    GFL_BG_SetScrollReq( p_wk->frm, GFL_BG_SCROLL_X_SET, x );
    return ret;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �J�[�h���X���C�h�A�E�g
 *
 *	@param	PLAYERINFO_WORK *p_wk  ���[�N
 *
 *	@return TRUE�Ŋ���  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL PLAYERINFO_MoveOutMain( PLAYERINFO_WORK * p_wk )
{ 

  { 
    BOOL ret  = FALSE;
  
    s32 x;

    if( p_wk->cnt == 0 )
    { 
      int i;
      GFL_CLACTPOS  pos;
      for( i = 0; i < PLAYERINFO_CLWK_MAX; i++ )
      { 
        if( p_wk->p_clwk[i] )
        { 
          GFL_CLACT_WK_GetPos( p_wk->p_clwk[i], &pos, p_wk->cl_draw_type );
          p_wk->clwk_x[i]  = pos.x;
        }
      }

      PMSND_PlaySE( WBM_SND_SE_CARD_SLIDE );
    }

    x =  WBM_CARD_INIT_POS_X * p_wk->cnt / CARD_WAIT_SYNC;

    { 
      int i;
      GFL_CLACTPOS  pos;
      for( i = 0; i < PLAYERINFO_CLWK_MAX; i++ )
      { 
        if( p_wk->p_clwk[i] )
        { 
          GFL_CLACT_WK_GetPos( p_wk->p_clwk[i], &pos, p_wk->cl_draw_type );
          pos.x = p_wk->clwk_x[i] - WBM_CARD_INIT_POS_X * p_wk->cnt / CARD_WAIT_SYNC;
          GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &pos, p_wk->cl_draw_type );
        }
      }
    }

    if( p_wk->cnt++ >= CARD_WAIT_SYNC )
    { 
      x = WBM_CARD_INIT_POS_X;
      p_wk->cnt = 0;
      ret = TRUE;
    }

    GFL_BG_SetScrollReq( p_wk->frm+1, GFL_BG_SCROLL_X_SET, x );
    GFL_BG_SetScrollReq( p_wk->frm, GFL_BG_SCROLL_X_SET, x );
    return ret;
  }
}
//=============================================================================
/**
 *					BMPWIN
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BMPWIN���쐬���A�����\��	�����_���}�b�`��
 *
 *	@param	PLAYERINFO_WORK * p_wk	���[�N
 *	@param	mode										���[�h
 *	@param	PLAYERINFO_RANDOMMATCH_DATA *cp_data	�\�����邽�߂̃f�[�^
 *	@param	MYSTATUS* p_my												�v���C���[�̃f�[�^
 *	@param	*p_font																�t�H���g
 *	@param	*p_que																�L���[
 *	@param	*p_msg																���b�Z�[�W
 *	@param	*p_word																�P��o�^
 *	@param	heapID																�q�[�vID
 */
//-----------------------------------------------------------------------------
static void PlayerInfo_Bmpwin_Rnd_Create( PLAYERINFO_WORK * p_wk, BOOL is_rate, const PLAYERINFO_RANDOMMATCH_DATA *cp_data, const MYSTATUS* p_my, GFL_FONT *p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, BOOL is_black, HEAPID heapID )
{	
	enum
	{	
		PLAYERINFO_RND_BMPWIN_MODE,
		PLAYERINFO_RND_BMPWIN_RULE,
		PLAYERINFO_RND_BMPWIN_PLAYERSCORE,
		PLAYERINFO_RND_BMPWIN_RATE_LABEL,
		PLAYERINFO_RND_BMPWIN_RATE_NUM,
		PLAYERINFO_RND_BMPWIN_BTLCNT_LABEL,
		PLAYERINFO_RND_BMPWIN_BTLCNT_NUM,
		PLAYERINFO_RND_BMPWIN_WIN_LABEL,
		PLAYERINFO_RND_BMPWIN_WIN_NUM,
		PLAYERINFO_RND_BMPWIN_LOSE_LABEL,
		PLAYERINFO_RND_BMPWIN_LOSE_NUM,
		PLAYERINFO_RND_BMPWIN_MAX,
	};
	
	static const struct
	{	
		u8 x;
		u8 y;
		u8 w;
		u8 h;
	}sc_bmpwin_range[PLAYERINFO_RND_BMPWIN_MAX]	=
	{	
		//���[�h�\��
		{	
			2,4,26,2,
		},
		//�o�g�����[��
		{	
			9,7,21,2,
		},
		//�����̐���
		{	
			9,9,21,2,
		},
		//���[�e�B���O
		{	
			2,12,20,2,
		},
		//���[�e�B���O�̐��l
		{	
			25, 12, 5, 2,
		},
		//�ΐ��
		{	
			2,15,20,2,
		},
		//�ΐ�񐔂̐��l
		{	
			25, 15, 5, 2,
		},
		//��������
		{	
			2,18,20,2,
		},
		//���������̐��l
		{	
			25, 18, 5, 2,
		},
		//��������
		{	
			2,21,20,2,
		},
		//���������̐��l
		{	
			25, 21, 5, 2,
		},
	};
	//BMPWIN�o�^��
	p_wk->bmpwin_max	= PLAYERINFO_RND_BMPWIN_MAX;

	//BMPWIN�쐬
	{	
		int i;
		for( i = 0; i < p_wk->bmpwin_max; i++ )
		{	
			p_wk->p_bmpwin[i]	= GFL_BMPWIN_Create( PLAYERINFO_BG_FRAME, 
					sc_bmpwin_range[i].x, sc_bmpwin_range[i].y, sc_bmpwin_range[i].w, sc_bmpwin_range[i].h,
					PLAYERINFO_PLT_BG_FONT, GFL_BMP_CHRAREA_GET_B );

			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0 );

			GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin[i] );

			if( p_wk->p_bmpwin[i] )
			{	
				PRINT_UTIL_Setup( &p_wk->print_util[i], p_wk->p_bmpwin[i] );
			}
		}
	}

	//�����\��
	{	
		int i;
		STRBUF	*p_src;
		STRBUF	*p_str;
		BOOL is_print;
    PRINTSYS_LSB color;

		p_src	= GFL_STR_CreateBuffer( 128, heapID );
		p_str	= GFL_STR_CreateBuffer( 128, heapID );

		for( i = 0; i < PLAYERINFO_RND_BMPWIN_MAX; i++ )
		{	
      color = is_black == TRUE ? PLAYERINFO_STR_COLOR_WHITE:PLAYERINFO_STR_COLOR_BLACK;

			is_print	= TRUE;
			switch( i )
			{	
			case PLAYERINFO_RND_BMPWIN_MODE:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_000 + is_rate, p_str );
        color = PLAYERINFO_STR_COLOR_WHITE;
				break;
			case PLAYERINFO_RND_BMPWIN_RULE:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_002 + cp_data->btl_rule, p_str );
        color = PLAYERINFO_STR_COLOR_WHITE;
				break;
			case PLAYERINFO_RND_BMPWIN_PLAYERSCORE:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_007, p_src );
				WORDSET_RegisterPlayerName( p_word, 0, p_my );
				WORDSET_ExpandStr( p_word, p_str, p_src );
        color = PLAYERINFO_STR_COLOR_WHITE;
				break;
			case PLAYERINFO_RND_BMPWIN_RATE_LABEL:
				if( is_rate )
				{	
					GFL_MSG_GetString( p_msg, WIFIMATCH_STR_008, p_str );
				}
				else
				{	
					is_print	= FALSE;
				}
				break;
			case PLAYERINFO_RND_BMPWIN_RATE_NUM:
				if( is_rate )
				{	
					GFL_MSG_GetString( p_msg, WIFIMATCH_STR_009, p_src );
					WORDSET_RegisterNumber( p_word, 0, cp_data->rate, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
					WORDSET_ExpandStr( p_word, p_str, p_src );
				}
				else
				{	
					is_print	= FALSE;
				}
				break;
			case PLAYERINFO_RND_BMPWIN_BTLCNT_LABEL:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_010, p_str );
				break;
			case PLAYERINFO_RND_BMPWIN_BTLCNT_NUM:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_011, p_src );
				WORDSET_RegisterNumber( p_word, 0, cp_data->btl_cnt, 5, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
				WORDSET_ExpandStr( p_word, p_str, p_src );
				break;
			case PLAYERINFO_RND_BMPWIN_WIN_LABEL:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_012, p_str );
				break;
			case PLAYERINFO_RND_BMPWIN_WIN_NUM:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_013, p_src );
				WORDSET_RegisterNumber( p_word, 0, cp_data->win_cnt, 5, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
				WORDSET_ExpandStr( p_word, p_str, p_src );
				break;
			case PLAYERINFO_RND_BMPWIN_LOSE_LABEL:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_014, p_str );
				break;
			case PLAYERINFO_RND_BMPWIN_LOSE_NUM:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_015, p_src );
				WORDSET_RegisterNumber( p_word, 0, cp_data->lose_cnt, 5, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
				WORDSET_ExpandStr( p_word, p_str, p_src );
				break;
			}
			if( is_print )
			{	
				PRINT_UTIL_PrintColor( &p_wk->print_util[i], p_que, 0, 0, p_str,  p_font, color );
			}

		}

		GFL_STR_DeleteBuffer( p_src );
		GFL_STR_DeleteBuffer( p_str );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	BMPWIN���쐬���A�����\��	WIFI����
 *
 *	@param	PLAYERINFO_WORK * p_wk	���[�N
 *	@param	mode										���[�h
 *	@param	PLAYERINFO_RANDOMMATCH_DATA *cp_data	�\�����邽�߂̃f�[�^
 *	@param	MYSTATUS* p_my												�v���C���[�̃f�[�^
 *	@param	*p_font																�t�H���g
 *	@param	*p_que																�L���[
 *	@param	*p_msg																���b�Z�[�W
 *	@param	*p_word																�P��o�^
 *	@param	heapID																�q�[�vID
 */
//-----------------------------------------------------------------------------
static void PlayerInfo_Bmpwin_Wifi_Create( PLAYERINFO_WORK * p_wk, BOOL is_limit, const PLAYERINFO_WIFICUP_DATA *cp_data, const MYSTATUS* p_my, GFL_FONT *p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, HEAPID heapID )
{	
	enum
	{	
		PLAYERINFO_CUP_BMPWIN_CUPNAME,
		PLAYERINFO_CUP_BMPWIN_TIME,
		PLAYERINFO_CUP_BMPWIN_PLAYERNAME,
		PLAYERINFO_CUP_BMPWIN_RATE_LABEL,
		PLAYERINFO_CUP_BMPWIN_RATE_NUM,
		PLAYERINFO_CUP_BMPWIN_BTLCNT_LABEL,
		PLAYERINFO_CUP_BMPWIN_BTLCNT_NUM,
		PLAYERINFO_CUP_BMPWIN_REGPOKE,
		PLAYERINFO_CUP_BMPWIN_MAX,
	};
	
	static const struct
	{	
		u8 x;
		u8 y;
		u8 w;
		u8 h;
	}sc_bmpwin_range[PLAYERINFO_CUP_BMPWIN_MAX]	=
	{	
		//���\��
		{	
			2,1,28,4,
		},
		//�J�Ó���
		{	
			2,6,28,2,
		},
		//�v���C���[��
		{	
			2,9,9,2,
		},
		//���[�e�B���O
		{	
			10,9,15,2,
		},
		//���[�e�B���O�̐��l
		{	
			25, 9, 5, 2,
		},
		//�ΐ��
		{	
			10,12,15,2,
		},
		//�ΐ�񐔂̐��l
		{	
			25, 12, 5, 2,
		},
		//�o�^�|�P����
		{	
			5,18,28,2,
		},
	};


	//BMPWIN�o�^��
	p_wk->bmpwin_max	= PLAYERINFO_CUP_BMPWIN_MAX;

	//BMPWIN�쐬
	{	
		int i;
		for( i = 0; i < p_wk->bmpwin_max; i++ )
		{	
			p_wk->p_bmpwin[i]	= GFL_BMPWIN_Create( p_wk->frm, 
					sc_bmpwin_range[i].x, sc_bmpwin_range[i].y, sc_bmpwin_range[i].w, sc_bmpwin_range[i].h,
					p_wk->plt, GFL_BMP_CHRAREA_GET_B );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0 );
			GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin[i] );

			if( p_wk->p_bmpwin[i] )
			{	
				PRINT_UTIL_Setup( &p_wk->print_util[i], p_wk->p_bmpwin[i] );
			}
		}
	}

	//�����\��
	{	
		int i;
		STRBUF	*p_src;
		STRBUF	*p_str;
		BOOL	is_print;
    PRINTSYS_LSB color;

		p_src	= GFL_STR_CreateBuffer( 128, heapID );
		p_str	= GFL_STR_CreateBuffer( 128, heapID );

		for( i = 0; i < PLAYERINFO_CUP_BMPWIN_MAX; i++ )
		{	
      color = PLAYERINFO_STR_COLOR_BLACK;

			is_print	= TRUE;
			switch( i )
			{	
			case PLAYERINFO_CUP_BMPWIN_CUPNAME:
				GFL_STR_SetStringCode( p_str, cp_data->cup_name );
        color = PLAYERINFO_STR_COLOR_WHITE;
				break;
			case PLAYERINFO_CUP_BMPWIN_TIME:
				{	
					u16 s_y;
					u8 s_m, s_d;
					u16 e_y;
					u8 e_m, e_d;

					s_y	= 2000 + GFDATE_GetYear( cp_data->start_date );
					s_m	= GFDATE_GetMonth( cp_data->start_date );
					s_d	= GFDATE_GetDay( cp_data->start_date );
					e_y	= 2000 + GFDATE_GetYear( cp_data->end_date );
					e_m	= GFDATE_GetMonth( cp_data->end_date );
					e_d	= GFDATE_GetDay( cp_data->end_date );

					GFL_MSG_GetString( p_msg, WIFIMATCH_STR_016, p_src );
					WORDSET_RegisterNumber( p_word, 0, s_y, 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
					WORDSET_RegisterNumber( p_word, 1, s_m, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
					WORDSET_RegisterNumber( p_word, 2, s_d, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
					WORDSET_RegisterNumber( p_word, 3, e_y, 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
					WORDSET_RegisterNumber( p_word, 4, e_m, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
					WORDSET_RegisterNumber( p_word, 5, e_d, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
					WORDSET_ExpandStr( p_word, p_str, p_src );
          color = PLAYERINFO_STR_COLOR_WHITE;
				}
				break;
			case PLAYERINFO_CUP_BMPWIN_PLAYERNAME:
				MyStatus_CopyNameString( p_my, p_str );
        color = PLAYERINFO_STR_COLOR_WHITE;
				break;
			case PLAYERINFO_CUP_BMPWIN_RATE_LABEL:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_008, p_str );
				break;
			case PLAYERINFO_CUP_BMPWIN_RATE_NUM:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_009, p_src );
				WORDSET_RegisterNumber( p_word, 0, cp_data->rate, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
				WORDSET_ExpandStr( p_word, p_str, p_src );
				break;
			case PLAYERINFO_CUP_BMPWIN_BTLCNT_LABEL:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_010, p_str );
				break;
			case PLAYERINFO_CUP_BMPWIN_BTLCNT_NUM:
				if( is_limit )
				{	
					Wbm_View_PrintFraction(
					&p_wk->print_util[i], p_que, p_font,
					cp_data->btl_cnt, cp_data->btl_max, heapID );
					is_print =FALSE;
				}
				else
				{	
					GFL_MSG_GetString( p_msg, WIFIMATCH_STR_011, p_src );
					WORDSET_RegisterNumber( p_word, 0, cp_data->btl_cnt, 5, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
					WORDSET_ExpandStr( p_word, p_str, p_src );
				}
				break;
			case PLAYERINFO_CUP_BMPWIN_REGPOKE:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_017, p_str );
        color = PLAYERINFO_STR_COLOR_WHITE;
				break;
			}
			if( is_print )
			{	
				PRINT_UTIL_PrintColor( &p_wk->print_util[i], p_que, 0, 0, p_str,  p_font, color );
			}
		}

		GFL_STR_DeleteBuffer( p_src );
		GFL_STR_DeleteBuffer( p_str );
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	BMPWIN���쐬���A�����\��	���C�u����
 *
 *	@param	PLAYERINFO_WORK * p_wk	���[�N
 *	@param	mode										���[�h
 *	@param	PLAYERINFO_RANDOMMATCH_DATA *cp_data	�\�����邽�߂̃f�[�^
 *	@param	MYSTATUS* p_my												�v���C���[�̃f�[�^
 *	@param	*p_font																�t�H���g
 *	@param	*p_que																�L���[
 *	@param	*p_msg																���b�Z�[�W
 *	@param	*p_word																�P��o�^
 *	@param	heapID																�q�[�vID
 */
//-----------------------------------------------------------------------------
static void PlayerInfo_Bmpwin_Live_Create( PLAYERINFO_WORK * p_wk, const PLAYERINFO_LIVECUP_DATA *cp_data,const MYSTATUS* p_my, GFL_FONT *p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, HEAPID heapID )
{	
	enum
	{	
		PLAYERINFO_LIVE_BMPWIN_CUPNAME,
		PLAYERINFO_LIVE_BMPWIN_TIME,
		PLAYERINFO_LIVE_BMPWIN_PLAYERNAME,
		PLAYERINFO_LIVE_BMPWIN_WIN_LABEL,
		PLAYERINFO_LIVE_BMPWIN_WIN_NUM,
		PLAYERINFO_LIVE_BMPWIN_LOSE_LABEL,
		PLAYERINFO_LIVE_BMPWIN_LOSE_NUM,
		PLAYERINFO_LIVE_BMPWIN_BTLCNT_LABEL,
		PLAYERINFO_LIVE_BMPWIN_BTLCNT_NUM,
		PLAYERINFO_LIVE_BMPWIN_REGPOKE,
		PLAYERINFO_LIVE_BMPWIN_MAX,
	};
	
	static const struct
	{	
		u8 x;
		u8 y;
		u8 w;
		u8 h;
	}sc_bmpwin_range[PLAYERINFO_LIVE_BMPWIN_MAX]	=
	{	
		//���\��
		{	
			2,1,28,4,
		},
		//�J�Ó���
		{	
			2,6,28,2,
		},
		//�v���C���[��
		{
			2,9,9,2,
		},
		//��������
		{
			10,9,12,2,
		},
		//���������̐��l
		{	
			25,9, 5, 2,
		},
		//��������
		{	
			10,12,12,2,
		},
		//���������̐��l
		{	
			25,12, 5, 2,
		},
		//�ΐ��
		{	
			8,15,14,2,
		},
		//�ΐ�񐔂̐��l
		{	
			25,15,5, 2,
		},
		//�o�^�|�P����
		{	
			5,18,28,2,
		},
	};
	//BMPWIN�o�^��
	p_wk->bmpwin_max	= PLAYERINFO_LIVE_BMPWIN_MAX;

	//BMPWIN�쐬
	{	
		int i;
		for( i = 0; i < p_wk->bmpwin_max; i++ )
		{	
			p_wk->p_bmpwin[i]	= GFL_BMPWIN_Create( p_wk->frm, 
					sc_bmpwin_range[i].x, sc_bmpwin_range[i].y, sc_bmpwin_range[i].w, sc_bmpwin_range[i].h,
					p_wk->plt, GFL_BMP_CHRAREA_GET_B );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0 );
			GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin[i] );

			if( p_wk->p_bmpwin[i] )
			{	
				PRINT_UTIL_Setup( &p_wk->print_util[i], p_wk->p_bmpwin[i] );
			}
		}
	}

	//�����\��
	{	
		int i;
		STRBUF	*p_src;
		STRBUF	*p_str;
    PRINTSYS_LSB color;

		BOOL	is_print;

		p_src	= GFL_STR_CreateBuffer( 128, heapID );
		p_str	= GFL_STR_CreateBuffer( 128, heapID );

		for( i = 0; i < PLAYERINFO_LIVE_BMPWIN_MAX; i++ )
		{	
      color = PLAYERINFO_STR_COLOR_BLACK;

			is_print	= TRUE;
			switch( i )
			{	
			case PLAYERINFO_LIVE_BMPWIN_CUPNAME:
				GFL_STR_SetStringCode( p_str, cp_data->cup_name );
        color = PLAYERINFO_STR_COLOR_WHITE;
				break;
			case PLAYERINFO_LIVE_BMPWIN_TIME:
				{	
					u16 s_y;
					u8 s_m, s_d;
					u16 e_y;
					u8 e_m, e_d;

					s_y	= 2000 + GFDATE_GetYear( cp_data->start_date );
					s_m	= GFDATE_GetMonth( cp_data->start_date );
					s_d	= GFDATE_GetDay( cp_data->start_date );
					e_y	= 2000 + GFDATE_GetYear( cp_data->end_date );
					e_m	= GFDATE_GetMonth( cp_data->end_date );
					e_d	= GFDATE_GetDay( cp_data->end_date );

					GFL_MSG_GetString( p_msg, WIFIMATCH_STR_016, p_src );
					WORDSET_RegisterNumber( p_word, 0, s_y, 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
					WORDSET_RegisterNumber( p_word, 1, s_m, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
					WORDSET_RegisterNumber( p_word, 2, s_d, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
					WORDSET_RegisterNumber( p_word, 3, e_y, 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
					WORDSET_RegisterNumber( p_word, 4, e_m, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
					WORDSET_RegisterNumber( p_word, 5, e_d, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
					WORDSET_ExpandStr( p_word, p_str, p_src );
          color = PLAYERINFO_STR_COLOR_WHITE;
				}
				break;
			case PLAYERINFO_LIVE_BMPWIN_PLAYERNAME:
        MyStatus_CopyNameString( p_my, p_str );
        color = PLAYERINFO_STR_COLOR_WHITE;
				break;
			case PLAYERINFO_LIVE_BMPWIN_WIN_LABEL:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_012, p_str );
				break;
			case PLAYERINFO_LIVE_BMPWIN_WIN_NUM:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_013, p_src );
				WORDSET_RegisterNumber( p_word, 0, cp_data->win_cnt, 5, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
				WORDSET_ExpandStr( p_word, p_str, p_src );
				break;
			case PLAYERINFO_LIVE_BMPWIN_LOSE_LABEL:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_014, p_str );
				break;
			case PLAYERINFO_LIVE_BMPWIN_LOSE_NUM:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_015, p_src );
				WORDSET_RegisterNumber( p_word, 0, cp_data->lose_cnt, 5, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
				WORDSET_ExpandStr( p_word, p_str, p_src );
				break;
			case PLAYERINFO_LIVE_BMPWIN_BTLCNT_LABEL:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_010, p_str );
				break;
			case PLAYERINFO_LIVE_BMPWIN_BTLCNT_NUM:
				Wbm_View_PrintFraction(
						&p_wk->print_util[i], p_que, p_font,
						cp_data->btl_cnt, cp_data->btl_max, heapID );
				is_print =FALSE;
				break;
			case PLAYERINFO_LIVE_BMPWIN_REGPOKE:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_017, p_str );
        color = PLAYERINFO_STR_COLOR_WHITE;
				break;
			}
			if( is_print )
			{	
				PRINT_UTIL_PrintColor( &p_wk->print_util[i], p_que, 0, 0, p_str,  p_font, color );
			}
		}

		GFL_STR_DeleteBuffer( p_src );
		GFL_STR_DeleteBuffer( p_str );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v�����g���C��
 *
 *	@param	PLAYERINFO_WORK * p_wk	���[�N
 *	@param	PRINT_QUE	*p_que				�v�����g�L���[
 *
 *	@return	TRUE�őS�Ẵv�����g�I��	FALSE�ł܂�
 */
//-----------------------------------------------------------------------------
static BOOL PlayerInfo_Bmpwin_PrintMain( PLAYERINFO_WORK * p_wk, PRINT_QUE *p_que )
{	
	int i;
	BOOL ret;
	ret	= TRUE;
	for( i = 0; i < p_wk->bmpwin_max; i++ )
	{	
		if( p_wk->p_bmpwin[i] )
		{	
			ret	&= PRINT_UTIL_Trans( &p_wk->print_util[i], p_que );
		}
	}

	return ret;
}
//----------------------------------------------------------------------------
/**
 *	@brief	BMPWIN�j��
 *
 *	@param	PLAYERINFO_WORK * p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void PlayerInfo_Bmpwin_Delete( PLAYERINFO_WORK * p_wk )
{	
	int i;
	for( i = 0; i < p_wk->bmpwin_max; i++ )
	{	
		if( p_wk->p_bmpwin[i] )
		{	
			GFL_BMPWIN_Delete( p_wk->p_bmpwin[i] );
		}
	}
}
//=============================================================================
/**
 *					CLWK
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�A�C�R���ǂ݂���
 *
 *	@param	PLAYERINFO_WORK * p_wk	���[�N
 *	@param	mode					���[�h
 *	@param	u16 *cp_poke	�����X�^�[�ԍ����������z��6��
 *	@param	u16 *cp_form	�t�H�����z��6��
 *	@param	*p_unit				CLWK�ǂ݂��ݗp
 *	@param	heapID				�q�[�v�h�c
 */
//-----------------------------------------------------------------------------
static void PlayerInfo_POKEICON_Create( PLAYERINFO_WORK * p_wk, GFL_CLUNIT *p_unit,REGULATION_VIEWDATA * p_reg_view, HEAPID heapID )
{	
  u16 mons_no;
  u8 form_no;
  u8  sex;
  u8  item;

	//���\�[�X�ǂ݂���
	{	
		int i;
		ARCHANDLE	*p_handle;

		p_handle	= GFL_ARC_OpenDataHandle( ARCID_POKEICON, heapID );

		p_wk->res[ PLAYERINFO_RESID_POKEICON_PLT ]	= GFL_CLGRP_PLTT_RegisterComp( p_handle,
        POKEICON_GetPalArcIndex(), p_wk->cl_draw_type, p_wk->pokemonicon_plt*0x20, GFL_HEAP_LOWID(heapID) );
    p_wk->res[ PLAYERINFO_RESID_POKEICON_CELL ]	= GFL_CLGRP_CELLANIM_Register( 
						p_handle, POKEICON_GetCellArcIndex(), POKEICON_GetAnmArcIndex(), heapID ); 

		for( i = 0; i < TEMOTI_POKEMAX; i++ )
		{
      mons_no = RegulationView_GetParam( p_reg_view, REGULATION_VIEW_MONS_NO, i );
      form_no = RegulationView_GetParam( p_reg_view, REGULATION_VIEW_FROM_NO, i );
      sex     = RegulationView_GetParam( p_reg_view, REGULATION_VIEW_SEX, i );
      mons_no = (mons_no == 0) ? 1: mons_no;

      p_wk->res[ PLAYERINFO_RESID_POKEICON_CGR_TOP + i ]		=
        GFL_CLGRP_CGR_Register( 
            p_handle, POKEICON_GetCgxArcIndexByMonsNumber( mons_no, form_no, sex, FALSE ),
            FALSE, p_wk->cl_draw_type, GFL_HEAP_LOWID(heapID) ); 
		}
		GFL_ARC_CloseDataHandle( p_handle );
	}

  { 
		ARCHANDLE	*p_handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heapID );
    p_wk->res[ PLAYERINFO_RESID_ITEM_PLT ] = GFL_CLGRP_PLTT_RegisterEx( p_handle,
          APP_COMMON_GetPokeItemIconPltArcIdx(), p_wk->cl_draw_type, 
          (p_wk->pokemonicon_plt+POKEICON_PAL_MAX)*0x20, 0, 1, GFL_HEAP_LOWID(heapID) );
    p_wk->res[ PLAYERINFO_RESID_ITEM_CGR ] = GFL_CLGRP_CGR_Register( 
        p_handle, APP_COMMON_GetPokeItemIconCharArcIdx(),
        FALSE, p_wk->cl_draw_type, GFL_HEAP_LOWID(heapID) ); 
    p_wk->res[ PLAYERINFO_RESID_ITEM_CELL ]  = GFL_CLGRP_CELLANIM_Register( 
        p_handle, APP_COMMON_GetPokeItemIconCellArcIdx( APP_COMMON_MAPPING_128K),
        APP_COMMON_GetPokeItemIconAnimeArcIdx( APP_COMMON_MAPPING_128K ), heapID ); 
		GFL_ARC_CloseDataHandle( p_handle );
  }

	//CLWK�쐬
	{	
		GFL_CLWK_DATA	clwk_data;
		int i;

		GFL_STD_MemClear( &clwk_data, sizeof(GFL_CLWK_DATA) );
		clwk_data.pos_y		= PLAYERINFO_POKEICON_Y;
		clwk_data.anmseq	= POKEICON_ANM_HPMAX;
    clwk_data.bgpri   = 1;
    clwk_data.softpri = 1;
		for( i = 0; i < TEMOTI_POKEMAX; i++ )
		{	
      mons_no = RegulationView_GetParam( p_reg_view, REGULATION_VIEW_MONS_NO, i );
      form_no = RegulationView_GetParam( p_reg_view, REGULATION_VIEW_FROM_NO, i );
      sex     = RegulationView_GetParam( p_reg_view, REGULATION_VIEW_SEX, i );
      item    = RegulationView_GetParam( p_reg_view, REGULATION_VIEW_ITEM_FLAG, i ); 

      NAGI_Printf( "�����X�^�[%d %d %d %d\n", mons_no, form_no, sex, item );

      if( mons_no != 0 )
      { 
        clwk_data.pos_x	= WBM_CARD_INIT_POS_X + PLAYERINFO_POKEICON_START_X + PLAYERINFO_POKEICON_DIFF_X*i;
        p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i]	= GFL_CLACT_WK_Create( p_unit,
            p_wk->res[ PLAYERINFO_RESID_POKEICON_CGR_TOP + i ],
            p_wk->res[ PLAYERINFO_RESID_POKEICON_PLT ],
            p_wk->res[ PLAYERINFO_RESID_POKEICON_CELL ],
            &clwk_data,
            p_wk->cl_draw_type,
            heapID );

        GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk[i],
            POKEICON_GetPalNum( mons_no, form_no, sex, FALSE ), 
            CLWK_PLTTOFFS_MODE_OAM_COLOR );

        //����A�C�R��
        if( item )
        { 
          GFL_CLWK_DATA clwk_item_data;
          clwk_item_data  = clwk_data;
          clwk_item_data.pos_x  += PLAYERINFO_POKEITEM_X_OFS;
          clwk_item_data.pos_y  += PLAYERINFO_POKEITEM_Y_OFS;
          clwk_item_data.softpri  = 0;
          clwk_item_data.anmseq   = 0;
          p_wk->p_clwk[ PLAYERINFO_CLWK_ITEM_TOP + i]	= GFL_CLACT_WK_Create( p_unit,
              p_wk->res[ PLAYERINFO_RESID_ITEM_CGR ],
              p_wk->res[ PLAYERINFO_RESID_ITEM_PLT ],
              p_wk->res[ PLAYERINFO_RESID_ITEM_CELL ],
              &clwk_item_data,
              p_wk->cl_draw_type,
              heapID );
        }
        else
        { 
          p_wk->p_clwk[ PLAYERINFO_CLWK_ITEM_TOP + i] = NULL;
        }
      }
      else
      { 
        p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i] = NULL;
        p_wk->p_clwk[ PLAYERINFO_CLWK_ITEM_TOP + i] = NULL;
      }
    }
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����A�C�R���j��
 *
 *	@param	PLAYERINFO_WORK * p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void PlayerInfo_POKEICON_Delete( PLAYERINFO_WORK * p_wk )
{	
	
	//CLWK�j��
	{	
		int i;
		for( i = 0; i < TEMOTI_POKEMAX; i++ )
		{	
      if( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP +i] )
      { 
        GFL_CLACT_WK_Remove( p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP +i] );
        p_wk->p_clwk[PLAYERINFO_CLWK_POKEICON_TOP+i] = NULL;
      }
      if( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP +i] )
      { 
        GFL_CLACT_WK_Remove( p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP +i] );
        p_wk->p_clwk[PLAYERINFO_CLWK_ITEM_TOP +i] = NULL;
      }
		}
	}

	//���\�[�X�j��
	{	
		int i;


		for( i = 0; i < TEMOTI_POKEMAX; i++ )
		{	
			GFL_CLGRP_CGR_Release( p_wk->res[ PLAYERINFO_RESID_POKEICON_CGR_TOP + i ] );
		}	

    GFL_CLGRP_CELLANIM_Release( p_wk->res[ PLAYERINFO_RESID_POKEICON_CELL ] );
		GFL_CLGRP_PLTT_Release( p_wk->res[ PLAYERINFO_RESID_POKEICON_PLT ] );
	
    GFL_CLGRP_CGR_Release( p_wk->res[ PLAYERINFO_RESID_ITEM_CGR ] );
    GFL_CLGRP_PLTT_Release( p_wk->res[ PLAYERINFO_RESID_ITEM_PLT ] );
    GFL_CLGRP_CELLANIM_Release( p_wk->res[ PLAYERINFO_RESID_ITEM_CELL ] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����̌����ڂ��쐬
 *
 *	@param	PLAYERINFO_WORK * p_wk	���[�N
 *	@param	trainerID		������
 *	@param	*p_unit			���j�b�g
 *	@param	heapID			�q�[�vID
 */
//-----------------------------------------------------------------------------
static void PlayerInfo_TRAINER_Cleate( PLAYERINFO_WORK * p_wk, u32 trainerID, GFL_CLUNIT *p_unit, HEAPID heapID )
{	
	//���\�[�X�ǂ݂���
	{	
		ARCHANDLE	*p_handle;

		p_handle	= GFL_ARC_OpenDataHandle( ARCID_WIFIUNIONCHAR, heapID );

		p_wk->res[ PLAYERINFO_RESID_TRAINER_PLT ]	= GFL_CLGRP_PLTT_RegisterEx( p_handle,
        WBM_CARD_TRAINER_RES_NCLR, p_wk->cl_draw_type, p_wk->trainer_plt*0x20,
        WBM_CARD_TRAINER_RES_PLT_OFS(trainerID), 1, heapID );

		p_wk->res[ PLAYERINFO_RESID_TRAINER_CGR ]		= GFL_CLGRP_CGR_Register( 
				p_handle, WBM_CARD_TRAINER_RES_NCGR(trainerID),
				FALSE, p_wk->cl_draw_type, heapID ); 

		p_wk->res[ PLAYERINFO_RESID_TRAINER_CELL ]	= GFL_CLGRP_CELLANIM_Register( 
				p_handle, WBM_CARD_TRAINER_RES_NCER, 
				WBM_CARD_TRAINER_RES_NANR, heapID ); 

		GFL_ARC_CloseDataHandle( p_handle );
	}


	//CLWK
	{	
		GFL_CLWK_DATA	clwk_data;

		GFL_STD_MemClear( &clwk_data, sizeof(GFL_CLWK_DATA) );
		clwk_data.pos_y		= 0;
		clwk_data.pos_x		= 0;
    clwk_data.bgpri   = 1;
		p_wk->p_clwk[PLAYERINFO_CLWK_TRAINER]	= GFL_CLACT_WK_Create( p_unit,
				p_wk->res[ PLAYERINFO_RESID_TRAINER_CGR ],
				p_wk->res[ PLAYERINFO_RESID_TRAINER_PLT ],
				p_wk->res[ PLAYERINFO_RESID_TRAINER_CELL ],
				&clwk_data,
				p_wk->cl_draw_type,
				heapID );

    GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk[PLAYERINFO_CLWK_TRAINER], 0, CLWK_PLTTOFFS_MODE_PLTT_TOP );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����̌����ڂ�j��
 *
 *	@param	PLAYERINFO_WORK * p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void PlayerInfo_TRAINER_Delete( PLAYERINFO_WORK * p_wk )
{	
	//CLWK�j��
	{	
		GFL_CLACT_WK_Remove( p_wk->p_clwk[PLAYERINFO_CLWK_TRAINER] );
	}

	//���\�[�X�j��
	{	
		GFL_CLGRP_CGR_Release( p_wk->res[ PLAYERINFO_RESID_TRAINER_CGR ] );
		GFL_CLGRP_CELLANIM_Release( p_wk->res[ PLAYERINFO_RESID_TRAINER_CELL ] );
		GFL_CLGRP_PLTT_Release( p_wk->res[ PLAYERINFO_RESID_TRAINER_PLT ] );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���쐬
 *
 *	@param	PLAYERINFO_WORK * p_wk  ���[�N
 *	@param	*p_unit                 OBJ�쐬�p���j�b�g
 *	@param	WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res ���\�[�X
 *	@param	heapID                  �q�[�vID
 */
//-----------------------------------------------------------------------------
static void PlayerInfo_LOCK_Cleate( PLAYERINFO_WORK * p_wk, GFL_CLUNIT *p_unit, const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, HEAPID heapID )
{ 
	//CLWK
  GFL_CLWK_DATA	clwk_data;

  GFL_STD_MemClear( &clwk_data, sizeof(GFL_CLWK_DATA) );
  clwk_data.pos_x		= WBM_CARD_LOCK_POS_X + WBM_CARD_INIT_POS_X;
  clwk_data.pos_y		= WBM_CARD_LOCK_POS_Y;
  clwk_data.anmseq	= 7;
  clwk_data.bgpri   = 1;

  p_wk->p_clwk[PLAYERINFO_CLWK_LOCK]	= GFL_CLACT_WK_Create( p_unit,
      WIFIBATTLEMATCH_VIEW_GetResource( cp_res, WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CHR_M + p_wk->cl_draw_type ),
      WIFIBATTLEMATCH_VIEW_GetResource( cp_res, WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_PLT_M + p_wk->cl_draw_type ),
      WIFIBATTLEMATCH_VIEW_GetResource( cp_res, WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CEL_M + p_wk->cl_draw_type ),
      &clwk_data,
      p_wk->cl_draw_type,
      heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���j��
 *
 *	@param	PLAYERINFO_WORK * p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void PlayerInfo_LOCK_Delete( PLAYERINFO_WORK * p_wk )
{ 
	//CLWK�j��
	{	
		GFL_CLACT_WK_Remove( p_wk->p_clwk[PLAYERINFO_CLWK_LOCK] );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����쐬
 *
 *	@param	PLAYERINFO_WORK * p_wk    ���[�N
 *	@param	PLAYERINFO_RANDOMMATCH_DATA *cp_data  �f�[�^
 *	@param	*p_unit OBJ�쐬�p���j�b�g
 *	@param	heapID  �q�[�vID
 */
//-----------------------------------------------------------------------------
static void PlayerInfo_STAR_Cleate( PLAYERINFO_WORK * p_wk, const PLAYERINFO_RANDOMMATCH_DATA *cp_data, GFL_CLUNIT *p_unit, const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, HEAPID heapID )
{ 
  u32 star_num;

  //���̌v�Z
  star_num  = CalcRank( cp_data->btl_cnt, cp_data->win_cnt, cp_data->lose_cnt );

	//CLWK
	{	
    int i;
		GFL_CLWK_DATA	clwk_data;

		GFL_STD_MemClear( &clwk_data, sizeof(GFL_CLWK_DATA) );
		clwk_data.pos_x		= WBM_CARD_INIT_POS_X;
		clwk_data.pos_y		= 16;

    for( i = 0; i < star_num; i++ )
    { 
      clwk_data.pos_x	  	+=  16;
      clwk_data.anmseq		=   2+i;
      p_wk->p_clwk[PLAYERINFO_CLWK_STAR_TOP + i]	= GFL_CLACT_WK_Create( p_unit,
				WIFIBATTLEMATCH_VIEW_GetResource( cp_res, WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CHR_S ),
				WIFIBATTLEMATCH_VIEW_GetResource( cp_res, WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_PLT_S ),
				WIFIBATTLEMATCH_VIEW_GetResource( cp_res, WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CEL_S ),
				&clwk_data,
				CLSYS_DRAW_SUB,
				heapID );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ����j��
 *
 *	@param	PLAYERINFO_WORK * p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void PlayerInfo_STAR_Delete( PLAYERINFO_WORK * p_wk )
{   
  int i;
  for( i = PLAYERINFO_CLWK_STAR_TOP; i < PLAYERINFO_CLWK_STAR_END; i++ )
  { 
    if( p_wk->p_clwk[i] )
    { 
      GFL_CLACT_WK_Remove( p_wk->p_clwk[i] );
      p_wk->p_clwk[i] = NULL;
    }
  }
}

#ifdef DEBUG_DATA_CREATE
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�f�[�^���쐬
 *
 *	@param	WIFIBATTLEMATCH_MODE mode	���[�h
 *	@param	*p_data										�󂯎�胏�[�N
 */
//-----------------------------------------------------------------------------
void PLAYERINFO_DEBUG_CreateRndData( WIFIBATTLEMATCH_TYPE mode, void *p_data_adrs )
{	
		{	
			PLAYERINFO_RANDOMMATCH_DATA	*p_data	= p_data_adrs;
			GFL_STD_MemClear( p_data, sizeof(PLAYERINFO_RANDOMMATCH_DATA) );
			p_data->btl_rule	= 0;
			p_data->rate			= 1234;
			p_data->btl_cnt		= 99;
			p_data->win_cnt		= 70;
			p_data->lose_cnt	= 29;	
		}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�f�[�^���쐬
 *
 *	@param	WIFIBATTLEMATCH_MODE mode	���[�h
 *	@param	*p_data										�󂯎�胏�[�N
 */
//-----------------------------------------------------------------------------
void PLAYERINFO_DEBUG_CreateWifiData( WIFIBATTLEMATCH_TYPE mode, void *p_data_adrs )
{ 
		{	
			PLAYERINFO_WIFICUP_DATA	*p_data	= p_data_adrs;
			GFL_STD_MemClear( p_data, sizeof(PLAYERINFO_WIFICUP_DATA) );
			p_data->cup_name[0]	= L'�f';
			p_data->cup_name[1]	= L'�o';
			p_data->cup_name[2]	= L'�b';
			p_data->cup_name[3]	= L'�O';
			p_data->cup_name[4]	= L'W';
			p_data->cup_name[5]	= L'i';
			p_data->cup_name[6]	= L'-';
			p_data->cup_name[7]	= L'F';
			p_data->cup_name[8]	= L'i';
			p_data->cup_name[9]	= L'��';
			p_data->cup_name[10]	= L'��';
			p_data->cup_name[11]	= L'��';
			p_data->cup_name[12]	= L'��';
			p_data->cup_name[13]	= GFL_STR_GetEOMCode();
			p_data->start_date	= GFDATE_Set( 10, 11, 12, 0);
			p_data->end_date		= GFDATE_Set( 10, 12, 24, 0);
			p_data->rate				= 53;
			p_data->btl_cnt			= 2;
			p_data->btl_max			= 15;
		}

}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�f�[�^���쐬
 *
 *	@param	WIFIBATTLEMATCH_MODE mode	���[�h
 *	@param	*p_data										�󂯎�胏�[�N
 */
//-----------------------------------------------------------------------------
void PLAYERINFO_DEBUG_CreateLiveData( WIFIBATTLEMATCH_TYPE mode, void *p_data_adrs )
{ 
		{	
			PLAYERINFO_LIVECUP_DATA	*p_data	= p_data_adrs;
			GFL_STD_MemClear( p_data, sizeof(PLAYERINFO_LIVECUP_DATA) );
			p_data->cup_name[0]	= L'�f';
			p_data->cup_name[1]	= L'�o';
			p_data->cup_name[2]	= L'�b';
			p_data->cup_name[3]	= L'�O';
			p_data->cup_name[4]	= L'��';
			p_data->cup_name[5]	= L'�C';
			p_data->cup_name[6]	= L'�u';
			p_data->cup_name[7]	= L'��';
			p_data->cup_name[8]	= L'��';
			p_data->cup_name[9]	= L'��';
			p_data->cup_name[10]	= L'��';
			p_data->cup_name[11]	= GFL_STR_GetEOMCode();
			p_data->start_date	= GFDATE_Set( 11, 11, 12, 0);
			p_data->end_date		= GFDATE_Set( 11, 12, 24, 0);
			p_data->win_cnt			= 20;
			p_data->lose_cnt		= 17;
			p_data->btl_cnt			= 2;
			p_data->btl_max			= 15;
		}

}

#endif


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�ΐ�ҏ��\��
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
enum
{	
	MATCHINFO_BMPWIN_PLAYER,
	MATCHINFO_BMPWIN_RATE_LABEL,
	MATCHINFO_BMPWIN_RATE_NUM,
	MATCHINFO_BMPWIN_COMEFROM,
	MATCHINFO_BMPWIN_CONTRY,
	MATCHINFO_BMPWIN_PLACE,
	MATCHINFO_BMPWIN_GREET,
	MATCHINFO_BMPWIN_PMS,
	MATCHINFO_BMPWIN_MAX,
};
enum
{	
	MATCHINFO_RESID_TRAINER_PLT,
	MATCHINFO_RESID_TRAINER_CGR,
	MATCHINFO_RESID_TRAINER_CELL,
	MATCHINFO_RESID_MAX,
};
//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�ΐ�ҏ��\�����[�N
//=====================================
struct _MATCHINFO_WORK
{	
  BOOL          is_rate;
	GFL_BMPWIN		*p_bmpwin[ MATCHINFO_BMPWIN_MAX ];
	PRINT_UTIL		print_util[ MATCHINFO_BMPWIN_MAX ];
	GFL_CLWK			*p_clwk;
	GFL_CLWK			*p_star[ PLAYERINFO_STAR_MAX ];
	PMS_DRAW_WORK	*p_pms;
	u16 res[MATCHINFO_RESID_MAX];
  u32 cnt;

  u16 clwk_x;
  u16 star_x[PLAYERINFO_STAR_MAX];

  WIFIBATTLEMATCH_TYPE mode;
};
//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	BMPWIN
//=====================================
static void MatchInfo_Bmpwin_Create( MATCHINFO_WORK * p_wk, const WIFIBATTLEMATCH_ENEMYDATA *cp_data, GFL_FONT *p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, BOOL is_black, HEAPID heapID );
static BOOL MatchInfo_Bmpwin_PrintMain( MATCHINFO_WORK * p_wk, PRINT_QUE *p_que );
static void MatchInfo_Bmpwin_Delete( MATCHINFO_WORK * p_wk );
//-------------------------------------
///	CLWK
//=====================================
static void MatchInfo_TRAINER_Create( MATCHINFO_WORK * p_wk, u32 trainerID, GFL_CLUNIT *p_unit, HEAPID heapID );
static void MatchInfo_TRAINER_Delete( MATCHINFO_WORK * p_wk );

static void MatchInfo_STAR_Cleate( MATCHINFO_WORK * p_wk, const WIFIBATTLEMATCH_ENEMYDATA *cp_data, GFL_CLUNIT *p_unit, const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, HEAPID heapID );
static void MatchInfo_STAR_Delete( MATCHINFO_WORK * p_wk );

//=============================================================================
/**
 *					�f�[�^
*/
//=============================================================================

//=============================================================================
/**
 *					�O�����J�֐�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�ΐ�ҏ��	�쐬
 *
 *	@param	const MATCHINFO_DATA *cp_data	�f�[�^
 *	@param	PRINT_QUE *p_que							�L���[
 *	@param	*p_msg												���b�Z�[�W
 *	@param	*p_word												�P��o�^
 *	@param	heapID												�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
MATCHINFO_WORK * MATCHINFO_Init( const WIFIBATTLEMATCH_ENEMYDATA *cp_data, GFL_CLUNIT *p_unit, const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, GFL_FONT *p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, WIFIBATTLEMATCH_TYPE mode, BOOL is_rate, REGULATION_CARD_BGM_TYPE type, HEAPID heapID )
{	
	MATCHINFO_WORK	*	p_wk;
  WBM_CARD_RANK rank;
	p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(MATCHINFO_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(MATCHINFO_WORK) );
  p_wk->is_rate = is_rate;
  p_wk->mode    = mode;
	p_wk->p_pms	= PMS_DRAW_Init( p_unit, CLSYS_DRAW_MAIN, p_que, p_font, MATCHINFO_PLT_OBJ_PMS, 1, heapID );
  PMS_DRAW_SetCLWKAutoScrollFlag( p_wk->p_pms, TRUE );

  PMS_DRAW_SetNullColorPallet( p_wk->p_pms, 0 );
	
  rank = CalcRank( cp_data->btl_cnt, cp_data->win_cnt, cp_data->lose_cnt );


  {
    BOOL is_black = FALSE;
    if( mode == WIFIBATTLEMATCH_TYPE_RNDRATE
      || mode == WIFIBATTLEMATCH_TYPE_RNDFREE )
    {
      is_black  = (rank == WBM_CARD_RANK_BLACK);
    }
    MatchInfo_Bmpwin_Create( p_wk, cp_data, p_font, p_que, p_msg, p_word, is_black, heapID );
  }
	MatchInfo_TRAINER_Create( p_wk, MyStatus_GetTrainerView( (MYSTATUS*)cp_data->mystatus ), p_unit, heapID );


  {
    GFL_ARC_UTIL_TransVramScreen( ARCID_WIFIMATCH_GRA, NARC_wifimatch_gra_standby_card_NSCR,
				BG_FRAME_M_CARD, 0, 0, FALSE, GFL_HEAP_LOWID( heapID ) );

    switch( mode )
    { 
    case WIFIBATTLEMATCH_TYPE_RNDRATE:
    case WIFIBATTLEMATCH_TYPE_RNDFREE:
      MatchInfo_STAR_Cleate( p_wk, cp_data, p_unit, cp_res, heapID );


      GFL_BG_ChangeScreenPalette( BG_FRAME_M_CARD, 0, 0, 32, 2, 1 );
      GFL_BG_ChangeScreenPalette( BG_FRAME_M_CARD, 0, 3, 32, 32, 3 + rank );
      GFL_BG_ChangeScreenPalette( BG_FRAME_M_CARD, 0, 3, 1, 2, 1 );
      GFL_BG_ChangeScreenPalette( BG_FRAME_M_CARD, 31, 3, 1, 2, 1 );
      break;

    case WIFIBATTLEMATCH_TYPE_WIFICUP:
      /* fallthr */
    case WIFIBATTLEMATCH_TYPE_LIVECUP:
      GFL_BG_WriteScreenExpand( BG_FRAME_M_CARD, 0, 0, 32, 2,
        GFL_BG_GetScreenBufferAdrs(BG_FRAME_M_CARD), 0, 24, 32, 32 );

      switch(type)
      { 
      case REGULATION_CARD_BGM_TRAINER: //SEQ_BGM_VS_TRAINER_WIFI
        GFL_BG_ChangeScreenPalette( BG_FRAME_M_CARD, 0, 0, 32, 2, 1 );
        GFL_BG_ChangeScreenPalette( BG_FRAME_M_CARD, 0, 3, 32, 32, 3);
        GFL_BG_ChangeScreenPalette( BG_FRAME_M_CARD, 0, 3, 1, 2, 1 );
        GFL_BG_ChangeScreenPalette( BG_FRAME_M_CARD, 31, 3, 1, 2, 1 );
        break;
      case REGULATION_CARD_BGM_WCS:  //SEQ_BGM_WCS
        GFL_BG_ChangeScreenPalette( BG_FRAME_M_CARD, 0, 0, 32, 2, 1 );
        GFL_BG_ChangeScreenPalette( BG_FRAME_M_CARD, 0, 3, 32, 32, 7 );
        GFL_BG_ChangeScreenPalette( BG_FRAME_M_CARD, 0, 3, 1, 2, 1 );
        GFL_BG_ChangeScreenPalette( BG_FRAME_M_CARD, 31, 3, 1, 2, 1 );
        break;
      }

    }

    GFL_BG_LoadScreenReq( BG_FRAME_M_CARD );
  }

  GFL_BG_SetScroll( BG_FRAME_M_CARD, GFL_BG_SCROLL_X_SET, -WBM_CARD_INIT_POS_X );
  GFL_BG_SetScroll( BG_FRAME_M_FONT, GFL_BG_SCROLL_X_SET, -WBM_CARD_INIT_POS_X );

  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, PLAYERINFO_ALPHA_EV1, PLAYERINFO_ALPHA_EV2);
	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ΐ�ҏ��j��
 *
 *	@param	MATCHINFO_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void MATCHINFO_Exit( MATCHINFO_WORK *p_wk )
{	
  G2_BlendNone();

  switch( p_wk->mode )
  { 
  case WIFIBATTLEMATCH_TYPE_RNDRATE:
  case WIFIBATTLEMATCH_TYPE_RNDFREE:
    MatchInfo_STAR_Delete( p_wk );
    break;

  case WIFIBATTLEMATCH_TYPE_WIFICUP:
    /* fallthr */
  case WIFIBATTLEMATCH_TYPE_LIVECUP:
    break;
  }

	MatchInfo_TRAINER_Delete( p_wk );
	MatchInfo_Bmpwin_Delete( p_wk );

	PMS_DRAW_Exit( p_wk->p_pms );

  GFL_BG_SetScroll( BG_FRAME_M_CARD, GFL_BG_SCROLL_X_SET, -WBM_CARD_INIT_POS_X );
  GFL_BG_SetScroll( BG_FRAME_M_FONT, GFL_BG_SCROLL_X_SET, -WBM_CARD_INIT_POS_X );

	GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ΐ�ҏ��	�v�����g
 *
 *	@param	MATCHINFO_WORK *p_wk	���[�N
 *	@param	*p_que								�L���[
 *
 *	@return	TRUE�Ńv�����g�I��	FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL MATCHINFO_PrintMain( MATCHINFO_WORK *p_wk, PRINT_QUE *p_que )
{	
	return MatchInfo_Bmpwin_PrintMain( p_wk, p_que );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ�ҏ��  �J�[�h����
 *
 *	@param	MATCHINFO_WORK * p_wk ���[�N
 *
 *	@return TRUE  FALSE
 */
//-----------------------------------------------------------------------------
BOOL MATCHINFO_MoveMain( MATCHINFO_WORK * p_wk )
{ 
  BOOL ret  = FALSE;
  
  s32 x;
  
  if( p_wk->cnt == 0 )
  { 
    int i;
    GFL_CLACTPOS  pos;

    if( p_wk->p_clwk )
    { 
      GFL_CLACT_WK_GetPos( p_wk->p_clwk, &pos, CLSYS_DRAW_MAIN );
      p_wk->clwk_x  = pos.x;
    }
    for( i = 0; i < PLAYERINFO_STAR_MAX; i++ )
    { 
      if( p_wk->p_star[i] )
      { 
        GFL_CLACT_WK_GetPos( p_wk->p_star[i], &pos, CLSYS_DRAW_MAIN );
        p_wk->star_x[i] = pos.x;
      }
    }

    //�X���C�h�C���̉�
    PMSND_PlaySE( WBM_SND_SE_CARD_SLIDE );
  }


  x = -WBM_CARD_INIT_POS_X + WBM_CARD_INIT_POS_X * p_wk->cnt / CARD_WAIT_SYNC;

  { 
    int i;
    GFL_CLACTPOS  pos;
    if( p_wk->p_clwk )
    { 
      GFL_CLACT_WK_GetPos( p_wk->p_clwk, &pos, CLSYS_DRAW_MAIN );
      pos.x = p_wk->clwk_x - WBM_CARD_INIT_POS_X * p_wk->cnt / CARD_WAIT_SYNC;
      GFL_CLACT_WK_SetPos( p_wk->p_clwk, &pos, CLSYS_DRAW_MAIN );
    }
    for( i = 0; i < PLAYERINFO_STAR_MAX; i++ )
    { 
      if( p_wk->p_star[i] )
      { 
        GFL_CLACT_WK_GetPos( p_wk->p_star[i], &pos, CLSYS_DRAW_MAIN );
        pos.x = p_wk->star_x[i] - WBM_CARD_INIT_POS_X * p_wk->cnt / CARD_WAIT_SYNC;
        GFL_CLACT_WK_SetPos( p_wk->p_star[i], &pos, CLSYS_DRAW_MAIN );
      }
    }
  }

  if( p_wk->cnt++ >= CARD_WAIT_SYNC )
  { 
    x = 0;
    p_wk->cnt = 0;
    ret = TRUE;
  }

  GFL_BG_SetScrollReq( BG_FRAME_M_CARD, GFL_BG_SCROLL_X_SET, x );
  GFL_BG_SetScrollReq( BG_FRAME_M_FONT, GFL_BG_SCROLL_X_SET, x );


  return ret;
}

#ifdef DEBUG_DATA_CREATE
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�f�[�^�쐬
 *
 *	@param	MATCHINFO_DATA *p_data_adrs �f�[�^�󂯎��
 */
//-----------------------------------------------------------------------------
void MATCHINFO_DEBUG_CreateData( MATCHINFO_DATA *p_data )
{	
	GFL_STD_MemClear( p_data, sizeof(MATCHINFO_DATA) );
	p_data->name[0]	= L'��';
	p_data->name[1]	= L'��';
	p_data->name[2]	= L'��';
	p_data->name[3]	= L'��';
	p_data->name[4]	= L'��';
	p_data->name[5]	= L'��';
	p_data->name[6]	= GFL_STR_GetEOMCode();
	p_data->rate		= 999;
	p_data->area		= 0;
	p_data->country	= 0;
	p_data->pms.sentence_type	= 0;
	p_data->pms.sentence_id		= 0;
	p_data->pms.word[0]				= 1;
	p_data->pms.word[1]				= 2;
}
#endif //DEBUG_DATA_CREATE

//=============================================================================
/**
 *				BMPWIN
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BMPWIN�쐬
 *
 *	@param	MATCHINFO_WORK * p_wk		���[�N
 *	@param	MATCHINFO_DATA *cp_data	�f�[�^
 *	@param	*p_font	�t�H���g
 *	@param	*p_que	�L���[
 *	@param	*p_msg	���b�Z�[�W
 *	@param	*p_word	�P��o�^
 *	@param	heapID	�q�[�vID
 */
//-----------------------------------------------------------------------------
static void MatchInfo_Bmpwin_Create( MATCHINFO_WORK * p_wk, const WIFIBATTLEMATCH_ENEMYDATA *cp_data, GFL_FONT *p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, BOOL is_black, HEAPID heapID )
{	
	static const struct
	{	
		u8 x;
		u8 y;
		u8 w;
		u8 h;
	}sc_bmpwin_range[MATCHINFO_BMPWIN_MAX]	=
	{	
		//�v���C���[��
		{	
			8,5,8,2,
		},

		//���[�e�B���O
		{	
			16,5,9,2,
		},
		//���[�e�B���O�̐��l
		{	
			25, 5, 5, 2,
		},
		//�Z��ł���Ƃ���
		{	
			2,9,28,2,
		},
		//��
		{	
			2, 11, 28, 2,
		},
		//�ꏊ
		{	
			2, 13, 28, 2,
		},
		//���A
		{	
			2, 16, 28, 2,
		},
		//PMS
		{	
			2, 18, 28, 4,
		},
	};

	//BMPWIN�쐬
	{	
		int i;
		for( i = 0; i < MATCHINFO_BMPWIN_MAX; i++ )
		{	
			p_wk->p_bmpwin[i]	= GFL_BMPWIN_Create( MATCHINFO_BG_FRAME, 
					sc_bmpwin_range[i].x, sc_bmpwin_range[i].y, sc_bmpwin_range[i].w, sc_bmpwin_range[i].h,
					MATCHINFO_PLT_BG_FONT, GFL_BMP_CHRAREA_GET_B );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0 );
			GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin[i] );

			if( i != MATCHINFO_BMPWIN_PMS )
			{	
				PRINT_UTIL_Setup( &p_wk->print_util[i], p_wk->p_bmpwin[i] );
			}
		}
	}

	//�����\��
	{	
		int i;
		STRBUF	*p_src;
		STRBUF	*p_str;
    PRINTSYS_LSB color;

		BOOL	is_print;

		p_src	= GFL_STR_CreateBuffer( 128, heapID );
		p_str	= GFL_STR_CreateBuffer( 128, heapID );

		for( i = 0; i < MATCHINFO_BMPWIN_MAX; i++ )
		{	
      color = is_black == TRUE ? PLAYERINFO_STR_COLOR_WHITE:PLAYERINFO_STR_COLOR_BLACK;
      is_print	= TRUE;		
			switch( i )
			{	
			case MATCHINFO_BMPWIN_PLAYER:
				GFL_STR_SetStringCode( p_str, MyStatus_GetMyName( (MYSTATUS*)cp_data->mystatus ) );

        color = PLAYERINFO_STR_COLOR_WHITE;
				break;
			case MATCHINFO_BMPWIN_RATE_LABEL:
        if( p_wk->mode == WIFIBATTLEMATCH_TYPE_RNDFREE )
        { 
          GFL_MSG_GetString( p_msg, WIFIMATCH_STR_012, p_str );
          color = PLAYERINFO_STR_COLOR_WHITE;
        }
        else
        {
          if( p_wk->is_rate )
          { 
            GFL_MSG_GetString( p_msg, WIFIMATCH_STR_008, p_str );
            color = PLAYERINFO_STR_COLOR_WHITE;
          }
          else
          { 
            is_print  = FALSE;
          }
        }
				break;
			case MATCHINFO_BMPWIN_RATE_NUM:
        if( p_wk->mode == WIFIBATTLEMATCH_TYPE_RNDFREE )
        { 
          GFL_MSG_GetString( p_msg, WIFIMATCH_STR_013, p_src );
          WORDSET_RegisterNumber( p_word, 0, cp_data->win_cnt, 5, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
          WORDSET_ExpandStr( p_word, p_str, p_src );
          color = PLAYERINFO_STR_COLOR_WHITE;
        }
        else
        {
          if( p_wk->is_rate )
          { 
            GFL_MSG_GetString( p_msg, WIFIMATCH_STR_009, p_src );
            WORDSET_RegisterNumber( p_word, 0, cp_data->rate, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
            WORDSET_ExpandStr( p_word, p_str, p_src );
            color = PLAYERINFO_STR_COLOR_WHITE;
          }
          else
          { 
            is_print  = FALSE;
          }
        }
				break;
			case MATCHINFO_BMPWIN_COMEFROM:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_018, p_str );
        color = PLAYERINFO_STR_COLOR_WHITE;
				break;
			case MATCHINFO_BMPWIN_CONTRY:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_020, p_src );
				WORDSET_RegisterCountryName( p_word, 0, MyStatus_GetMyNation( (MYSTATUS*)cp_data->mystatus ) );
				WORDSET_ExpandStr( p_word, p_str, p_src );
				break;
			case MATCHINFO_BMPWIN_PLACE:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_021, p_src );
				WORDSET_RegisterLocalPlaceName( p_word, 0, MyStatus_GetMyNation( (MYSTATUS*)cp_data->mystatus ), MyStatus_GetMyArea( (MYSTATUS*)cp_data->mystatus ) );
				WORDSET_ExpandStr( p_word, p_str, p_src );
				break;
			case MATCHINFO_BMPWIN_GREET:
				GFL_MSG_GetString( p_msg, WIFIMATCH_STR_019, p_str );
        color = PLAYERINFO_STR_COLOR_WHITE;
				break;
			case MATCHINFO_BMPWIN_PMS:
        PMS_DRAW_SetPrintColor( p_wk->p_pms, color );
				PMS_DRAW_Print( p_wk->p_pms, p_wk->p_bmpwin[i], (PMS_DATA*)&cp_data->pms, 0 );
				is_print	= FALSE;
				break;
			}
			if( is_print )
			{	
				PRINT_UTIL_PrintColor( &p_wk->print_util[i], p_que, 0, 0, p_str,  p_font, color );
			}
		}

		GFL_STR_DeleteBuffer( p_src );
		GFL_STR_DeleteBuffer( p_str );
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	BMPWIN�v�����g�`�F�b�N
 *
 *	@param	MATCHINFO_WORK * p_wk	���[�N
 *	@param	*p_que									�L���[
 *
 *	@return	TRIE�Ńv�����g�I��	FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
static BOOL MatchInfo_Bmpwin_PrintMain( MATCHINFO_WORK * p_wk, PRINT_QUE *p_que )
{	
	int i;
	BOOL ret;
	ret	= TRUE;
	for( i = 0; i < MATCHINFO_BMPWIN_MAX; i++ )
	{	
		if( i != MATCHINFO_BMPWIN_PMS )
		{	
			ret	&= PRINT_UTIL_Trans( &p_wk->print_util[i], p_que );
		}
		else
		{	
			PMS_DRAW_Main( p_wk->p_pms );
			ret &= PMS_DRAW_IsPrintEnd( p_wk->p_pms );
		}
	}

	return ret;
}
//----------------------------------------------------------------------------
/**
 *	@brief	BMPWI���j��
 *
 *	@param	MATCHINFO_WORK * p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void MatchInfo_Bmpwin_Delete( MATCHINFO_WORK * p_wk )
{	
	int i;
	for( i = 0; i < MATCHINFO_BMPWIN_MAX; i++ )
	{	
		if( p_wk->p_bmpwin[i] )
		{	
			GFL_BMPWIN_Delete( p_wk->p_bmpwin[i] );
      GFL_BMPWIN_ClearScreen( p_wk->p_bmpwin[i] );
		}
	}
}
//=============================================================================
/**
 *					CLWK
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�ΐ푊��̌����ڂ��쐬
 *
 *	@param	MATCHINFO_WORK * p_wk	���[�N
 *	@param	trainerID			������
 *	@param	*p_unit				���j�b�g
 *	@param	heapID				�q�[�vID
 */
//-----------------------------------------------------------------------------
static void MatchInfo_TRAINER_Create( MATCHINFO_WORK * p_wk, u32 trainerID, GFL_CLUNIT *p_unit, HEAPID heapID )
{	
	//���\�[�X�ǂ݂���
	{	
		ARCHANDLE	*p_handle;

		p_handle	= GFL_ARC_OpenDataHandle( ARCID_WIFIUNIONCHAR, heapID );

		p_wk->res[ MATCHINFO_RESID_TRAINER_PLT ]	= GFL_CLGRP_PLTT_RegisterEx( p_handle,
        WBM_CARD_TRAINER_RES_NCLR, CLSYS_DRAW_MAIN, MATCHINFO_PLT_OBJ_TRAINER*0x20,
        WBM_CARD_TRAINER_RES_PLT_OFS(trainerID), 1, heapID );

		p_wk->res[ MATCHINFO_RESID_TRAINER_CGR ]		= GFL_CLGRP_CGR_Register( 
				p_handle, WBM_CARD_TRAINER_RES_NCGR(trainerID),
				FALSE, CLSYS_DRAW_MAIN, heapID ); 

		p_wk->res[ MATCHINFO_RESID_TRAINER_CELL ]	= GFL_CLGRP_CELLANIM_Register( 
				p_handle, WBM_CARD_TRAINER_RES_NCER, 
				WBM_CARD_TRAINER_RES_NANR, heapID ); 

		GFL_ARC_CloseDataHandle( p_handle );
	}

	//CLWK
	{	
		GFL_CLWK_DATA	clwk_data;

		GFL_STD_MemClear( &clwk_data, sizeof(GFL_CLWK_DATA) );
		clwk_data.pos_x		= MATCHINFO_TRAINER_X + WBM_CARD_INIT_POS_X;
		clwk_data.pos_y		= MATCHINFO_TRAINER_Y;
		p_wk->p_clwk	= GFL_CLACT_WK_Create( p_unit,
				p_wk->res[ MATCHINFO_RESID_TRAINER_CGR ],
				p_wk->res[ MATCHINFO_RESID_TRAINER_PLT ],
				p_wk->res[ MATCHINFO_RESID_TRAINER_CELL ],
				&clwk_data,
				CLSYS_DRAW_MAIN,
				heapID );

    GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk, 0, CLWK_PLTTOFFS_MODE_PLTT_TOP );

  }

}
//----------------------------------------------------------------------------
/**
 *	@brief	�ΐ푊��̌����ڂ�j��
 *
 *	@param	MATCHINFO_WORK * p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void MatchInfo_TRAINER_Delete( MATCHINFO_WORK * p_wk )
{	

	//CLWK�j��
	{	
		GFL_CLACT_WK_Remove( p_wk->p_clwk );
	}

	//���\�[�X�j��
	{	
		GFL_CLGRP_CGR_Release( p_wk->res[ MATCHINFO_RESID_TRAINER_CGR ] );
		GFL_CLGRP_CELLANIM_Release( p_wk->res[ MATCHINFO_RESID_TRAINER_CELL ] );
		GFL_CLGRP_PLTT_Release( p_wk->res[ MATCHINFO_RESID_TRAINER_PLT ] );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ�҂̐���j��
 *
 *	@param	MATCHINFO_WORK * p_wk                 ���[�N
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *cp_data    ���
 *	@param	*p_unit                               OBJ�쐬�p���j�b�g
 *	@param	WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res ���\�[�X
 *	@param	heapID                                �q�[�vID
 */
//-----------------------------------------------------------------------------
static void MatchInfo_STAR_Cleate( MATCHINFO_WORK * p_wk, const WIFIBATTLEMATCH_ENEMYDATA *cp_data, GFL_CLUNIT *p_unit, const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, HEAPID heapID )
{ 
  u32 star_num;

  //���̌v�Z
  star_num  = CalcRank( cp_data->btl_cnt, cp_data->win_cnt, cp_data->lose_cnt );

	//CLWK
	{	
    int i;
		GFL_CLWK_DATA	clwk_data;

		GFL_STD_MemClear( &clwk_data, sizeof(GFL_CLWK_DATA) );
		clwk_data.pos_x		= WBM_CARD_INIT_POS_X;
		clwk_data.pos_y		= 16;

    for( i = 0; i < star_num; i++ )
    { 
      clwk_data.pos_x	  	+= 16;
      clwk_data.anmseq		= 2 + i;
      p_wk->p_star[ i ]	= GFL_CLACT_WK_Create( p_unit,
				WIFIBATTLEMATCH_VIEW_GetResource( cp_res, WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CHR_M ),
				WIFIBATTLEMATCH_VIEW_GetResource( cp_res, WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_PLT_M ),
				WIFIBATTLEMATCH_VIEW_GetResource( cp_res, WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CEL_M ),
				&clwk_data,
				CLSYS_DRAW_MAIN,
				heapID );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ�҂̐���j��
 *
 *	@param	MATCHINFO_WORK * p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void MatchInfo_STAR_Delete( MATCHINFO_WORK * p_wk )
{ 
  int i;
  for( i = 0; i < PLAYERINFO_STAR_MAX; i++ )
  { 
    if( p_wk->p_star[i] != NULL )
    { 
      GFL_CLACT_WK_Remove( p_wk->p_star[i] );
      p_wk->p_star[i] = NULL;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���̐����v�Z
 *
 *	@param	u32 btl_cnt �퓬��
 *	@param	win         ������
 *	@param	lose        ������
 *
 *	@return ���̐�
 */
//-----------------------------------------------------------------------------
static WBM_CARD_RANK CalcRank( u32 btl_cnt, u32 win, u32 lose )
{ 
  if( win >= WBM_CARD_WIN_BLACK )
  { 
    return WBM_CARD_RANK_BLACK;
  }
  else if( win >= WBM_CARD_WIN_GOLD )
  { 
    return WBM_CARD_RANK_GOLD;
  }
  else if( win >= WBM_CARD_WIN_SILVER )
  { 
    return WBM_CARD_RANK_SILVER;
  }
  else if( win >= WBM_CARD_WIN_BRONZE )
  { 
    return WBM_CARD_RANK_BRONZE;
  }
  else if( win >= WBM_CARD_WIN_COPPER )
  { 
    return WBM_CARD_RANK_COPPER;
  }
  else
  { 
    return WBM_CARD_RANK_NORMAL;
  }
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�ҋ@��OBJ
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�ΐ�ҏ��\�����[�N
//=====================================
struct _WBM_WAITICON_WORK
{ 
  GFL_CLWK  *p_light;
  GFL_CLWK  *p_wall;
};

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �ҋ@��OBJ�쐬
 *
 *	@param	GFL_CLUNIT *p_unit                    OBJ�������j�b�g
 *	@param	WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res ���\�[�X
 *	@param	heapID                                �q�[�vID 
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
WBM_WAITICON_WORK	* WBM_WAITICON_Init( GFL_CLUNIT *p_unit,  const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, HEAPID heapID )
{ 
  WBM_WAITICON_WORK	*p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(WBM_WAITICON_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(WBM_WAITICON_WORK) );

  //CLWK�쐬
	{	
		GFL_CLWK_DATA	clwk_data;

		GFL_STD_MemClear( &clwk_data, sizeof(GFL_CLWK_DATA) );
		clwk_data.pos_x		= 128;
		clwk_data.pos_y		= 96;

    clwk_data.anmseq  = 1;
    clwk_data.softpri = 1;
    clwk_data.bgpri   = 1;
		p_wk->p_light	= GFL_CLACT_WK_Create( p_unit,
				WIFIBATTLEMATCH_VIEW_GetResource( cp_res, WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CHR_M ),
				WIFIBATTLEMATCH_VIEW_GetResource( cp_res, WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_PLT_M ),
				WIFIBATTLEMATCH_VIEW_GetResource( cp_res, WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CEL_M ),
				&clwk_data,
				CLSYS_DRAW_MAIN,
				heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_light, TRUE );

    clwk_data.anmseq  = 0;
    clwk_data.softpri = 0;
    clwk_data.bgpri   = 1;
		p_wk->p_wall	= GFL_CLACT_WK_Create( p_unit,
				WIFIBATTLEMATCH_VIEW_GetResource( cp_res, WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CHR_M ),
				WIFIBATTLEMATCH_VIEW_GetResource( cp_res, WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_PLT_M ),
				WIFIBATTLEMATCH_VIEW_GetResource( cp_res, WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CEL_M ),
				&clwk_data,
				CLSYS_DRAW_MAIN,
				heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_wall, TRUE );
  }


  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ҋ@��OBJ�̔j��
 *
 *	@param	WBM_WAITICON_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WBM_WAITICON_Exit( WBM_WAITICON_WORK *p_wk )
{ 
  GFL_CLACT_WK_Remove( p_wk->p_wall );
  GFL_CLACT_WK_Remove( p_wk->p_light );

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/** 
 *	@brief  �ҋ@��OBJ�̃��C������
 *
 *	@param	WBM_WAITICON_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WBM_WAITICON_Main( WBM_WAITICON_WORK *p_wk )
{ 

}
//----------------------------------------------------------------------------
/**
 *	@brief  �ҋ@��OBJ�̕\���ݒ�
 *
 *	@param	WBM_WAITICON_WORK *p_wk ���[�N
 *	@param	on_off                  TRUE�Ȃ�Ε\��  FALSE�Ȃ�Δ�\��
 */
//-----------------------------------------------------------------------------
void WBM_WAITICON_SetDrawEnable( WBM_WAITICON_WORK *p_wk, BOOL on_off )
{ 
  if( on_off == TRUE )
  { 
    PMSND_PlaySE( WBM_SND_SE_MATCHING );
  }
  else
  { 
    PMSND_StopSE_byPlayerID( SEPLAYER_SE2 );
  }
  GFL_CLACT_WK_SetDrawEnable( p_wk->p_light, on_off );
  GFL_CLACT_WK_SetDrawEnable( p_wk->p_wall, on_off );
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  �o�g���{�b�N�X�\��
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�萔
//=====================================
//���\�[�X
typedef enum
{
  WBM_BTLBOX_RESID_POKEICON_PLT,
  WBM_BTLBOX_RESID_POKEICON_CGR_TOP,
  WBM_BTLBOX_RESID_POKEICON_CGR_END = WBM_BTLBOX_RESID_POKEICON_CGR_TOP + TEMOTI_POKEMAX,
  WBM_BTLBOX_RESID_POKEICON_CEL,
  WBM_BTLBOX_RESID_POKEITEM_PLT,
  WBM_BTLBOX_RESID_POKEITEM_CGR,
  WBM_BTLBOX_RESID_POKEITEM_CEL,
  WBM_BTLBOX_RESID_MAX,
} WBM_BTLBOX_RESID;

//CLWK
typedef enum
{
  WBM_BTLBOX_CLWCKID_POKE_TOP,
  WBM_BTLBOX_CLWCKID_POKE_END = WBM_BTLBOX_CLWCKID_POKE_TOP + TEMOTI_POKEMAX,
  WBM_BTLBOX_CLWCKID_ITEM_TOP,
  WBM_BTLBOX_CLWCKID_ITEM_END = WBM_BTLBOX_CLWCKID_ITEM_TOP + TEMOTI_POKEMAX,
  WBM_BTLBOX_CLWCKID_MAX,
} WBM_BTLBOX_CLWCKID;

//-------------------------------------
///	�ΐ�ҏ��\�����[�N
//=====================================
struct _WBM_BTLBOX_WORK
{ 
  u32         res[ WBM_BTLBOX_RESID_MAX ];
  GFL_CLWK    *p_clwk[ WBM_BTLBOX_CLWCKID_MAX ];
  GFL_BMPWIN  *p_bmpwin[ TEMOTI_POKEMAX ];
  PRINT_UTIL  print_util[ TEMOTI_POKEMAX ];
  u32         cnt;
  s16         clwk_x[ WBM_BTLBOX_CLWCKID_MAX ];
};

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���{�b�N�X�̒��g��\��  ������
 *
 *	@param	const BATTLE_BOX_SAVE * cp_btl_box    �o�g���{�b�N�X  
 *	@param	*p_unit                               OBJ�o�^�p���j�b�g
 *	@param	WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res ���\�[�X
 *	@param	*p_font                               �t�H���g
 *	@param	*p_que                                �����`��L���[
 *	@param	*p_msg                                ���b�Z�[�W
 *	@param	heapID                                �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
WBM_BTLBOX_WORK	* WBM_BTLBOX_Init( BATTLE_BOX_SAVE * cp_btl_box, GFL_CLUNIT *p_unit, const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, GFL_FONT *p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, HEAPID heapID )
{ 
  WBM_BTLBOX_WORK	*p_wk;
  POKEPARTY       *p_party;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(WBM_BTLBOX_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(WBM_BTLBOX_WORK) );

  p_party = BATTLE_BOX_SAVE_MakePokeParty( cp_btl_box, GFL_HEAP_LOWID(heapID) );

  {
    GFL_ARC_UTIL_TransVramScreen( ARCID_WIFIMATCH_GRA, NARC_wifimatch_gra_pokesel_NSCR,
				BG_FRAME_M_CARD, 0, 0, FALSE, GFL_HEAP_LOWID( heapID ) );
  }



  //���\�[�X�ǂݍ���
  { 
    //�|�P����
    { 
      int i;
      ARCHANDLE	*p_handle;
      POKEMON_PARAM *p_pp;
      p_handle	= GFL_ARC_OpenDataHandle( ARCID_POKEICON, heapID );

      p_wk->res[ WBM_BTLBOX_RESID_POKEICON_PLT ]	= GFL_CLGRP_PLTT_RegisterComp( p_handle,
          POKEICON_GetPalArcIndex(), CLSYS_DRAW_MAIN, PLT_OBJ_POKEMON_M*0x20, GFL_HEAP_LOWID(heapID) );

      p_wk->res[ WBM_BTLBOX_RESID_POKEICON_CEL ]	= GFL_CLGRP_CELLANIM_Register( 
          p_handle, POKEICON_GetCellArcIndex(), POKEICON_GetAnmArcIndex(), GFL_HEAP_LOWID(heapID) ); 

      for( i = 0; i < PokeParty_GetPokeCount( p_party ); i++ )
      {
        p_pp  = PokeParty_GetMemberPointer( p_party, i );
        if( PP_Get( p_pp, ID_PARA_poke_exist, NULL ) )
        { 
          p_wk->res[ WBM_BTLBOX_RESID_POKEICON_CGR_TOP + i ]		= GFL_CLGRP_CGR_Register( 
              p_handle, POKEICON_GetCgxArcIndex( PP_GetPPPPointerConst(p_pp) ),
              FALSE, CLSYS_DRAW_MAIN, GFL_HEAP_LOWID(heapID) ); 
        }
      }
      GFL_ARC_CloseDataHandle( p_handle );
    }

    //�A�C�e��
    { 
      int i;
      ARCHANDLE	*p_handle;
      p_handle	= GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heapID );

      p_wk->res[ WBM_BTLBOX_RESID_POKEITEM_PLT ]	= GFL_CLGRP_PLTT_RegisterEx( p_handle,
          APP_COMMON_GetPokeItemIconPltArcIdx(), CLSYS_DRAW_MAIN, PLT_OBJ_POKEITEM_M*0x20, 0, 1, GFL_HEAP_LOWID(heapID) );

      p_wk->res[ WBM_BTLBOX_RESID_POKEITEM_CEL ]	= GFL_CLGRP_CELLANIM_Register( 
          p_handle, APP_COMMON_GetPokeItemIconCellArcIdx( APP_COMMON_MAPPING_128K ),
          APP_COMMON_GetPokeItemIconAnimeArcIdx( APP_COMMON_MAPPING_128K ), GFL_HEAP_LOWID(heapID) ); 

      p_wk->res[ WBM_BTLBOX_RESID_POKEITEM_CGR ]  = GFL_CLGRP_CGR_Register( 
            p_handle, APP_COMMON_GetPokeItemIconCharArcIdx(),
            FALSE, CLSYS_DRAW_MAIN, GFL_HEAP_LOWID(heapID) );


      GFL_ARC_CloseDataHandle( p_handle );
    }
  }

  //CLWK�쐬
  { 
    int i;
    POKEMON_PARAM *p_pp;
    GFL_CLWK_DATA clwk_data;
    GFL_CLWK_DATA clwk_item_data;
    GFL_STD_MemClear( &clwk_data, sizeof(GFL_CLWK_DATA) );

    clwk_data.pos_x		= 24;
    clwk_data.pos_y		= 48 - 4;
		clwk_data.anmseq	= POKEICON_ANM_HPMAX;
    clwk_data.softpri  = 1;
		for( i = 0; i < PokeParty_GetPokeCount( p_party ); i++ )
		{	
      //�|�P����
      p_pp  = PokeParty_GetMemberPointer( p_party, i );
      if( PP_Get( p_pp, ID_PARA_poke_exist, NULL ) )
      { 
        clwk_data.pos_x	= 24 + i * 40;
        p_wk->p_clwk[ WBM_BTLBOX_CLWCKID_POKE_TOP + i]	= GFL_CLACT_WK_Create( p_unit,
            p_wk->res[ WBM_BTLBOX_RESID_POKEICON_CGR_TOP + i ],
            p_wk->res[ WBM_BTLBOX_RESID_POKEICON_PLT ],
            p_wk->res[ WBM_BTLBOX_RESID_POKEICON_CEL ],
            &clwk_data,
            CLSYS_DRAW_MAIN,
            heapID );

        GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk[ WBM_BTLBOX_CLWCKID_POKE_TOP + i],
            POKEICON_GetPalNumGetByPPP( PP_GetPPPPointerConst(p_pp) ), 
            CLWK_PLTTOFFS_MODE_OAM_COLOR );

        //����A�C�R��
        if( PP_Get( p_pp, ID_PARA_item, NULL) != 0 )
        { 
          clwk_item_data  = clwk_data;
          clwk_item_data.pos_x  += PLAYERINFO_POKEITEM_X_OFS;
          clwk_item_data.pos_y  += PLAYERINFO_POKEITEM_Y_OFS;
          clwk_item_data.softpri  = 0;
          clwk_item_data.anmseq   = 0;
          p_wk->p_clwk[ WBM_BTLBOX_CLWCKID_ITEM_TOP + i]	= GFL_CLACT_WK_Create( p_unit,
              p_wk->res[ WBM_BTLBOX_RESID_POKEITEM_CGR ],
              p_wk->res[ WBM_BTLBOX_RESID_POKEITEM_PLT ],
              p_wk->res[ WBM_BTLBOX_RESID_POKEITEM_CEL ],
					&clwk_item_data,
          CLSYS_DRAW_MAIN,
          heapID );
        }
      }
    }
  }

  //�����`��i���ʂƃ��x���j
  {
    int i;
    u8  x;
    u8  w;
    u8  lv;
    POKEMON_PARAM *p_pp;
    WORDSET *p_word;
    STRBUF  *p_strbuf;
    STRBUF  *p_lv_src;
    STRBUF  *p_male_src;
    STRBUF  *p_female_src;

    p_word    = WORDSET_Create( GFL_HEAP_LOWID(heapID) );
    p_strbuf  = GFL_STR_CreateBuffer( 128, GFL_HEAP_LOWID(heapID) );
    p_lv_src  = GFL_MSG_CreateString( p_msg, WIFIMATCH_WIFI_POKE_00 );
    p_male_src  = GFL_MSG_CreateString( p_msg, WIFIMATCH_WIFI_POKE_01 );
    p_female_src  = GFL_MSG_CreateString( p_msg, WIFIMATCH_WIFI_POKE_02 );

    for( i = 0; i < PokeParty_GetPokeCount( p_party ); i++ )
    {
      p_pp  = PokeParty_GetMemberPointer( p_party, i );

      if( PP_Get( p_pp, ID_PARA_poke_exist, NULL ) )
      { 
        //BMPWIN�쐬
        w = 5;
        x = 1 + i * w; 
        p_wk->p_bmpwin[i]  = GFL_BMPWIN_Create( BG_FRAME_M_FONT, x, 4, w, 6, PLT_FONT_M, GFL_BMP_CHRAREA_GET_B );
        GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin[i] );
        PRINT_UTIL_Setup( &p_wk->print_util[i], p_wk->p_bmpwin[i] );

        //���ʕ`��
        if( PTL_SEX_MALE == PP_Get( p_pp, ID_PARA_sex, NULL ) )
        { 
          PRINT_UTIL_PrintColor( &p_wk->print_util[i], p_que, 32, 0, p_male_src, p_font, PRINTSYS_LSB_Make( 5, 6, 0 ) );
        }
        else if( PTL_SEX_FEMALE == PP_Get( p_pp, ID_PARA_sex, NULL ) )
        { 
          PRINT_UTIL_PrintColor( &p_wk->print_util[i], p_que, 32, 0, p_female_src, p_font, PRINTSYS_LSB_Make( 3, 4, 0 ) );
        }

        //���x���`��
        lv  = PP_Get( p_pp, ID_PARA_level, NULL );
        WORDSET_RegisterNumber( p_word, 0, lv, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
        WORDSET_ExpandStr( p_word, p_strbuf, p_lv_src );
        x = 40 /2 - PRINTSYS_GetStrWidth( p_strbuf, p_font, 0 )/2;
        PRINT_UTIL_PrintColor( &p_wk->print_util[i], p_que, x, 36, p_strbuf, p_font, PRINTSYS_LSB_Make( 1, 2, 0 ) );
      }
    }

    GFL_STR_DeleteBuffer( p_female_src );
    GFL_STR_DeleteBuffer( p_male_src );
    GFL_STR_DeleteBuffer( p_lv_src );
    GFL_STR_DeleteBuffer( p_strbuf );
    WORDSET_Delete( p_word );
  }

  //������X���C�h�C�����Ă��邽�߁A�ʒu�����炵�Ă���
  { 
    int i;
    GFL_CLACTPOS  pos;

    for( i = 0; i < WBM_BTLBOX_CLWCKID_MAX; i++ )
    { 
      if( p_wk->p_clwk[i] )
      { 
        GFL_CLACT_WK_GetPos( p_wk->p_clwk[i], &pos, CLSYS_DRAW_MAIN );
        pos.x +=  WBM_CARD_INIT_POS_X;
        GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &pos, CLSYS_DRAW_MAIN );
      }
    }

    GFL_BG_SetScroll( BG_FRAME_M_CARD, GFL_BG_SCROLL_X_SET, -WBM_CARD_INIT_POS_X );
    GFL_BG_SetScroll( BG_FRAME_M_FONT, GFL_BG_SCROLL_X_SET, -WBM_CARD_INIT_POS_X );
  }

  GFL_HEAP_FreeMemory( p_party );

  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, PLAYERINFO_ALPHA_EV1, PLAYERINFO_ALPHA_EV2); 

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���{�b�N�X�̒��g��\��  �j��
 *
 *	@param	WBM_BTLBOX_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WBM_BTLBOX_Exit( WBM_BTLBOX_WORK *p_wk )
{ 
  G2_BlendNone();
  //BMPWIN�j��
  { 
    int i;
    for( i = 0; i < TEMOTI_POKEMAX; i++ )
    { 
      if( p_wk->p_bmpwin[i] )
      { 
        GFL_BMPWIN_Delete( p_wk->p_bmpwin[i] );
        GFL_BMPWIN_ClearScreen( p_wk->p_bmpwin[i] );
      }
    }
  }

  //CLWK�j��
  { 
    int i;
    for( i = 0; i < WBM_BTLBOX_CLWCKID_MAX; i++ )
    { 
      if( p_wk->p_clwk[i] )
      { 
        GFL_CLACT_WK_Remove( p_wk->p_clwk[i] );
      }
    }
  }


  //���\�[�X�j��
  { 
    int i;  
		GFL_CLGRP_CGR_Release( p_wk->res[ WBM_BTLBOX_RESID_POKEITEM_CGR ] );
		GFL_CLGRP_CELLANIM_Release( p_wk->res[ WBM_BTLBOX_RESID_POKEITEM_CEL ] );
		GFL_CLGRP_PLTT_Release( p_wk->res[ WBM_BTLBOX_RESID_POKEITEM_PLT ] );

    for( i = 0; i < TEMOTI_POKEMAX; i++ )
    { 
      if( p_wk->p_clwk[i] )
      { 
        GFL_CLGRP_CGR_Release( p_wk->res[ WBM_BTLBOX_RESID_POKEICON_CGR_TOP + i ] );
      }
    }
    GFL_CLGRP_CELLANIM_Release( p_wk->res[ WBM_BTLBOX_RESID_POKEICON_CEL ] );
		GFL_CLGRP_PLTT_Release( p_wk->res[ WBM_BTLBOX_RESID_POKEICON_PLT ] );
  }

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���{�b�N�X�̒��g��\��  �ړ�
 *
 *	@param	WBM_BTLBOX_WORK *p_wk   ���[�N
 *
 *	@return TRUE�ňړ��I��  FALSE�ňړ���
 */
//-----------------------------------------------------------------------------
BOOL WBM_BTLBOX_MoveInMain( WBM_BTLBOX_WORK *p_wk )
{ 

  if( GFL_BG_GetScrollX( BG_FRAME_M_CARD ) == 0 )
  { 
    return TRUE;
  }
  else
  { 
    BOOL ret  = FALSE;
  
    s32 x;

    if( p_wk->cnt == 0 )
    { 
      int i;
      GFL_CLACTPOS  pos;
      for( i = 0; i < WBM_BTLBOX_CLWCKID_MAX; i++ )
      { 
        if( p_wk->p_clwk[i] )
        { 
          GFL_CLACT_WK_GetPos( p_wk->p_clwk[i], &pos, CLSYS_DRAW_MAIN );
          p_wk->clwk_x[i]  = pos.x;
        }
      }
    }

    x = -WBM_CARD_INIT_POS_X + WBM_CARD_INIT_POS_X * p_wk->cnt / CARD_WAIT_SYNC;

    { 
      int i;
      GFL_CLACTPOS  pos;
      for( i = 0; i < WBM_BTLBOX_CLWCKID_MAX; i++ )
      { 
        if( p_wk->p_clwk[i] )
        { 
          GFL_CLACT_WK_GetPos( p_wk->p_clwk[i], &pos, CLSYS_DRAW_MAIN );
          pos.x = p_wk->clwk_x[i] - WBM_CARD_INIT_POS_X * p_wk->cnt / CARD_WAIT_SYNC;
          GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &pos, CLSYS_DRAW_MAIN );
        }
      }
    }

    if( p_wk->cnt++ >= CARD_WAIT_SYNC )
    { 
      x = 0;
      p_wk->cnt = 0;
      ret = TRUE;
    }

    GFL_BG_SetScrollReq( BG_FRAME_M_CARD, GFL_BG_SCROLL_X_SET, x );
    GFL_BG_SetScrollReq( BG_FRAME_M_FONT, GFL_BG_SCROLL_X_SET, x );

    return ret;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���{�b�N�X�̒��g��\��  �ړ�
 *
 *	@param	WBM_BTLBOX_WORK *p_wk   ���[�N
 *
 *	@return TRUE�ňړ��I��  FALSE�ňړ���
 */
//-----------------------------------------------------------------------------
BOOL WBM_BTLBOX_MoveOutMain( WBM_BTLBOX_WORK *p_wk )
{ 
  BOOL ret  = FALSE;
  
  s32 x;

  if( p_wk->cnt == 0 )
  { 
    int i;
    GFL_CLACTPOS  pos;
    for( i = 0; i < WBM_BTLBOX_CLWCKID_MAX; i++ )
    { 
      if( p_wk->p_clwk[i] )
      { 
        GFL_CLACT_WK_GetPos( p_wk->p_clwk[i], &pos, CLSYS_DRAW_MAIN );
        p_wk->clwk_x[i]  = pos.x;
      }
    }
  }

  x = WBM_CARD_INIT_POS_X * p_wk->cnt / CARD_WAIT_SYNC;

  { 
    int i;
    GFL_CLACTPOS  pos;
    for( i = 0; i < WBM_BTLBOX_CLWCKID_MAX; i++ )
    { 
      if( p_wk->p_clwk[i] )
      { 
        GFL_CLACT_WK_GetPos( p_wk->p_clwk[i], &pos, CLSYS_DRAW_MAIN );
        pos.x = p_wk->clwk_x[i] - WBM_CARD_INIT_POS_X * p_wk->cnt / CARD_WAIT_SYNC;
        GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &pos, CLSYS_DRAW_MAIN );
      }
    }
  }

  if( p_wk->cnt++ >= CARD_WAIT_SYNC )
  { 
    x = WBM_CARD_INIT_POS_X;
    p_wk->cnt = 0;
    ret = TRUE;
  }

  GFL_BG_SetScrollReq( BG_FRAME_M_CARD, GFL_BG_SCROLL_X_SET, x );
  GFL_BG_SetScrollReq( BG_FRAME_M_FONT, GFL_BG_SCROLL_X_SET, x );

  return ret;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���{�b�N�X�̒��g��\��  �����`�惁�C��
 *
 *	@param	WBM_BTLBOX_WORK *p_wk   ���[�N
 *
 *	@return TRUE�ŕ����`�抮��  FALSE�ŕ����`�撆
 */
//-----------------------------------------------------------------------------
BOOL WBM_BTLBOX_PrintMain( WBM_BTLBOX_WORK *p_wk, PRINT_QUE *p_que )
{ 
  BOOL ret = TRUE;

  //BMPWIN�j��
  { 
    int i;
    for( i = 0; i < TEMOTI_POKEMAX; i++ )
    { 
      if( p_wk->p_bmpwin[i] )
      {
        ret &= PRINT_UTIL_Trans( &p_wk->print_util[i], p_que );
      }
    }
  }

  return ret;
}

