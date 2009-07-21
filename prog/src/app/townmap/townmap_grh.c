//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		townmap_grh.c
 *	@brief	�^�E���}�b�v�p��{�O���t�B�b�N
 *	@author	Toru=Nagihashi
 *	@data		2009.07.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//lib
#include <gflib.h>
#include "print/gf_font.h"
#include "system/gfl_use.h"

//	archive
#include "arc_def.h"
#include "townmap_gra.naix"
#include "font/font.naix"

//mine
#include "townmap_grh.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	BG���[�N
//=====================================
typedef struct 
{
	int dummy;
} GRAPHIC_BG_WORK;
//-------------------------------------
///	OBJ���[�N
//=====================================
typedef struct 
{
	GFL_CLUNIT *p_clunit;
} GRAPHIC_OBJ_WORK;
//-------------------------------------
///	�^�E���}�b�v�p��{�O���t�B�b�N���[�N
//=====================================
struct _TOWNMAP_GRAPHIC_SYS
{	
	GRAPHIC_BG_WORK		bg;
	GRAPHIC_OBJ_WORK	obj;
	GFL_TCB	*p_vblank_task;
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	GRAPHIC
//=====================================
static void TownMap_Graphic_Init( void );
static void TownMap_Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_wk_adrs );
//-------------------------------------
///	BG
//=====================================
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk );
//-------------------------------------
///	OBJ
//=====================================
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID );
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk );
static GFL_CLUNIT * GRAPHIC_OBJ_GetUnit( const GRAPHIC_OBJ_WORK *cp_wk );
//=============================================================================
/**
 *					DATA
 */
//=============================================================================
//-------------------------------------
///	VRAM�ݒ�
//=====================================
static const GFL_DISP_VRAM sc_vramSetTable =
{
	GX_VRAM_BG_256_AD,					// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_0123_E,     // ���C��2D�G���W����BG�g���p���b�g
	GX_VRAM_SUB_BG_128_C,				// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g
	GX_VRAM_OBJ_128_B,					// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
	GX_VRAM_SUB_OBJ_16_I,       // �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g
	GX_VRAM_TEX_NONE,						// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_NONE,				// �e�N�X�`���p���b�g�X���b�g
	GX_OBJVRAMMODE_CHAR_1D_128K,		
	GX_OBJVRAMMODE_CHAR_1D_128K,		
};


//-------------------------------------
///	BG�ʐݒ�
//=====================================
static const struct 
{
	u32									frame;
	GFL_BG_BGCNT_HEADER	bgcnt_header;
	u32									mode;
}	sc_bgsetup[TOWNMAP_BG_FRAME_MAX]	=
{	
	//BAR_M
	{	
		GFL_BG_FRAME0_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	//FONT_M
	{	
		GFL_BG_FRAME1_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	//ROAD_M
	{	
		GFL_BG_FRAME2_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		},
		GFL_BG_MODE_256X16
	},
	//MAP_M
	{	
		GFL_BG_FRAME3_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x18000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE	//slot3=0x6000
		},
		GFL_BG_MODE_256X16
	},

	//FONT_S
	{	
		GFL_BG_FRAME0_S,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	//WND_S
	{	
		GFL_BG_FRAME1_S,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
};
//�t���[���擾�p�}�N��
#define GRAPHIC_BG_GetFrame( x )	(sc_bgsetup[ x ].frame)

//=============================================================================
/**
 *					GLOBAL
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	��{�O���t�B�b�N�쐬
 *
 *	@param	HEAPID heapID		�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
TOWNMAP_GRAPHIC_SYS * TOWNMAP_GRAPHIC_Init( HEAPID heapID )
{	
	TOWNMAP_GRAPHIC_SYS *p_wk;

	//���[�N�쐬
	p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(TOWNMAP_GRAPHIC_SYS) );
	GFL_STD_MemClear( p_wk, sizeof(TOWNMAP_GRAPHIC_SYS) );

	//��{�O���t�B�b�N�ݒ�
	TownMap_Graphic_Init();

	//���W���[���쐬
	GRAPHIC_BG_Init( &p_wk->bg, heapID );
	GRAPHIC_OBJ_Init( &p_wk->obj, &sc_vramSetTable, heapID );


	//VBlankTask�o�^
	p_wk->p_vblank_task	= GFUser_VIntr_CreateTCB(TownMap_Graphic_VBlankTask, p_wk, 0 );

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	��{�O���t�B�b�N�j��
 *
 *	@param	TOWNMAP_GRAPHIC_SYS *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
void TOWNMAP_GRAPHIC_Exit( TOWNMAP_GRAPHIC_SYS *p_wk )
{	
	//VBLANKTask����
	GFL_TCB_DeleteTask( p_wk->p_vblank_task );

	//���W���[���j��
	GRAPHIC_OBJ_Exit( &p_wk->obj );
	GRAPHIC_BG_Exit( &p_wk->bg );

	//���[�N�j��
	GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	��{�O���t�B�b�N�`��
 *
 *	@param	TOWNMAP_GRAPHIC_SYS *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
void TOWNMAP_GRAPHIC_Draw( TOWNMAP_GRAPHIC_SYS *p_wk )
{
	GRAPHIC_OBJ_Main( &p_wk->obj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG�ʎ擾
 *
 *	@param	const TOWNMAP_GRAPHIC_SYS *cp_wk	���[�N
 *	@param	frame		BG�C���f�b�N�X
 *
 *	@return	BG��
 */
//-----------------------------------------------------------------------------
u8 TOWNMAP_GRAPHIC_GetFrame( const TOWNMAP_GRAPHIC_SYS *cp_wk, TOWNMAP_GRAPHIC_BG_FRAME frame )
{	
	return GRAPHIC_BG_GetFrame( frame );
}
//----------------------------------------------------------------------------
/**
 *	@brief	UNIT�擾
 *
 *	@param	const TOWNMAP_GRAPHIC_SYS *cp_wk	���[�N
 *
 *	@return	UNIT
 */
//-----------------------------------------------------------------------------
GFL_CLUNIT *TOWNMAP_GRAPHIC_GetUnit( const TOWNMAP_GRAPHIC_SYS *cp_wk )
{	
	return GRAPHIC_OBJ_GetUnit( &cp_wk->obj );
}
//=============================================================================
/**
 *						GRAPHIC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N������
 *
 */
//-----------------------------------------------------------------------------
static void TownMap_Graphic_Init( void )
{	
	//���W�X�^������
	G2_BlendNone();
	G2S_BlendNone();	
	GX_SetVisibleWnd( GX_WNDMASK_NONE );
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );

	//VRAM�N���A�[
	GFL_DISP_ClearVRAM( 0 );

	//VRAM�o���N�ݒ�
	GFL_DISP_SetBank( &sc_vramSetTable );

	// �f�B�X�v���CON
	GFL_DISP_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );
	GFL_DISP_SetDispOn();

	//	�\��
	GFL_DISP_GX_InitVisibleControl();

	//	�t�H���g������
	GFL_FONTSYS_Init();
}
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�NVBLANK�֐�
 *
 *	@param	p_tcb			�^�X�N�R���g���[���u���b�N
 *	@param	p_wk_adrs	���[�N�A�h���X
 *
 */
//-----------------------------------------------------------------------------
static void TownMap_Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_wk_adrs )
{	
	TOWNMAP_GRAPHIC_SYS *p_wk	= p_wk_adrs;
	GRAPHIC_BG_VBlankFunction( &p_wk->bg );
	GRAPHIC_OBJ_VBlankFunction( &p_wk->obj );
}
//=============================================================================
/**
 *						BG
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BG	������
 *
 *	@param	GRAPHIC_BG_WORK *p_wk	���[�N
 *	@param	heapID								�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_BG_WORK) );

	//������
	GFL_BG_Init( heapID );
	GFL_BMPWIN_Init( heapID );

	//�O���t�B�b�N���[�h�ݒ�
	{	
		static const GFL_BG_SYS_HEADER sc_bgsys_header	=
		{	
			GX_DISPMODE_GRAPHICS,GX_BGMODE_5,GX_BGMODE_0,GX_BG0_AS_2D
		};
		GFL_BG_SetBGMode( &sc_bgsys_header );
	}

	//BG�ʐݒ�
	{	
		int i;
		for( i = 0; i < TOWNMAP_BG_FRAME_MAX; i++ )
		{	
			GFL_BG_SetBGControl( sc_bgsetup[i].frame, &sc_bgsetup[i].bgcnt_header, sc_bgsetup[i].mode );
			GFL_BG_SetVisible( sc_bgsetup[i].frame, VISIBLE_ON );
		}
	}

	//�ǂݍ���
	{	
		ARCHANDLE	*p_handle;
		p_handle	= GFL_ARC_OpenDataHandle( ARCID_TOWNMAP_GRAPHIC, heapID );

		//�����(���C��)
		//2�ʂŎg�p�g���p���b�g�X���b�g���Ⴄ�̂�2��]�����Ă���
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_townmap_gra_tmap_bg_d_NCLR,
				PALTYPE_MAIN_BG_EX, 0x6000, 32*16*16, heapID );
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_townmap_gra_tmap_bg_d_NCLR,
				PALTYPE_MAIN_BG_EX, 0x4000, 32*16*16, heapID );

		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_townmap_gra_tmap_bg_d_NCGR,
				GRAPHIC_BG_GetFrame(TOWNMAP_BG_FRAME_ROAD_M), 0, 32*32*32*2, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_townmap_gra_tmap_map_d_NCGR,
				GRAPHIC_BG_GetFrame(TOWNMAP_BG_FRAME_MAP_M), 0, 32*32*32*2, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_townmap_gra_tmap_root_d_NSCR,
				GRAPHIC_BG_GetFrame(TOWNMAP_BG_FRAME_ROAD_M), 0, 0x800, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_townmap_gra_tmap_map_d_NSCR,
				GRAPHIC_BG_GetFrame(TOWNMAP_BG_FRAME_MAP_M), 0, 0x800, FALSE, heapID );

		//���ʁi�T�u�j
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_townmap_gra_tmap_bg_u_NCLR,
				PALTYPE_SUB_BG, TOWNMAP_BG_PAL_S_00*0x20, 0, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_townmap_gra_tmap_bg_u_NCGR,
				GRAPHIC_BG_GetFrame(TOWNMAP_BG_FRAME_WND_S), 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_townmap_gra_tmap_bg_u_NSCR,
				GRAPHIC_BG_GetFrame(TOWNMAP_BG_FRAME_WND_S), 0, 0, FALSE, heapID );

		//�t�H���g�p�p���b�g
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, TOWNMAP_BG_PAL_M_14*0x20, 0x20, heapID );
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, TOWNMAP_BG_PAL_S_14*0x20, 0x20, heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}


}
//----------------------------------------------------------------------------
/**
 *	@brief	BG	�j��
 *
 *	@param	GRAPHIC_BG_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk )
{	
	
	//BG�ʔj��
	{	
		int i;
		for( i = 0; i < TOWNMAP_BG_FRAME_MAX; i++ )
		{	
			GFL_BG_ClearFrame( sc_bgsetup[i].frame );
			GFL_BG_FreeBGControl( sc_bgsetup[i].frame );
		}
	}

	//�I��
	GFL_BMPWIN_Exit();
	GFL_BG_Exit();

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_BG_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	BG	VBlank�֐�
 *
 *	@param	GRAPHIC_BG_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk )
{	
	GFL_BG_VBlankFunc();
}
//=============================================================================
/**
 *				OBJ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	������
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk			���[�N
 *	@param	GFL_DISP_VRAM* cp_vram_bank	�o���N�e�[�u��
 *	@param	heapID											�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_OBJ_WORK) );

	//�V�X�e���쐬
	GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN, cp_vram_bank, heapID );
	p_wk->p_clunit	= GFL_CLACT_UNIT_Create( 128, 0, heapID );
	GFL_CLACT_UNIT_SetDefaultRend( p_wk->p_clunit );

	//�\��
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	�j��
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk )
{	

	//�V�X�e���j��
	GFL_CLACT_UNIT_Delete( p_wk->p_clunit );
	GFL_CLACT_SYS_Delete();
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_OBJ_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	���C������
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk )
{	
	GFL_CLACT_SYS_Main();

}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	V�u�����N����
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk )
{	
	GFL_CLACT_SYS_VBlankFunc();
}

//----------------------------------------------------------------------------
/**
 *	@brief	CL���j�b�g�̎擾
 *
 *	@param	const GRAPHIC__OBJ_WORK *cp_wk	���[�N
 *
 *	@return	GFL_CLUNIT
 */
//-----------------------------------------------------------------------------
static GFL_CLUNIT * GRAPHIC_OBJ_GetUnit( const GRAPHIC_OBJ_WORK *cp_wk )
{	
	return cp_wk->p_clunit;
}


