//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		touchbar.c
 *	@brief	�A�v���p����ʃ^�b�`�o�[
 *	@author	Toru=Nagihashi
 *	@data		2009.09.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>

//mine
#include "touchbar.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	TOUCHBAR
//=====================================
//���\�[�X
enum
{	
	TOUCHBAR_RES_COMMON_PLT,
	TOUCHBAR_RES_COMMON_CHR,
	TOUCHBAR_RES_COMMON_CEL,

	TOUCHBAR_RES_MAX
};

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	����ʃo�[(�A�v���P�[�V�����o�[�Ə���ɖ���)
//=====================================
struct _TOUCHBAR_WORK
{
	GFL_CLWK	*p_clwk[APBAR_ICON_REGISTER_MAX];
	u32				res[TOUCHBAR_RES_MAX];
	u32				bg_frm;
	GFL_ARCUTIL_TRANSINFO				chr_pos;
	s32				trg;
	s32				cont;
	u32				tbl_len;
	TOUCHBAR_ITEM_ICON	setup_tbl[0];
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//���\�[�X
static void TouchBar_LoadResource( TOUCHBAR_WORK *p_wk, CLSYS_DRAW_TYPE	clsys_draw_type, PALTYPE bgplttype, APP_COMMON_MAPPING mapping, u8 bg_plt, u8 obj_plt, HEAPID heapID );
static void TouchBar_UnLoadResource( TOUCHBAR_WORK *p_wk );
//obj
static void TouchBar_CreateObj( TOUCHBAR_WORK *p_wk, GFL_CLUNIT* p_unit, const TOUCHBAR_ITEM_ICON *cp_setup_tbl, u16 tbl_len, CLSYS_DEFREND_TYPE	clsys_def_type, HEAPID heapID );
static void TouchBar_DeleteObj( TOUCHBAR_WORK *p_wk );
//etc
static void ARCHDL_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h,  u8 plt, BOOL compressedFlag, HEAPID heapID );

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	�A�j���V�[�P���X�e�[�u��
//	TOUCHBAR_ICON�ƑΉ������e�[�u��
//=====================================
static const u16 sc_anmseq_tbl[]	=
{	
	APP_COMMON_BARICON_EXIT,
	APP_COMMON_BARICON_RETURN,
	APP_COMMON_BARICON_CURSOR_DOWN,
	APP_COMMON_BARICON_CURSOR_UP,
	APP_COMMON_BARICON_CURSOR_LEFT,
	APP_COMMON_BARICON_CURSOR_RIGHT,
	APP_COMMON_BARICON_CHECK_OFF,
};

//=============================================================================
/**
 *					GLOBAL
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�������g���Łi�A�C�R���ʒu�w��Łj
 *
 *	@param	const TOUCHBAR_SETUP *cp_setup	�ݒ���
 *	@param	heapID	�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
extern TOUCHBAR_WORK * TOUCHBAR_Init( TOUCHBAR_SETUP *p_setup, HEAPID heapID )
{	
	TOUCHBAR_WORK	*p_wk;


	//���[�N�쐬�A������
	{	
		u32 size;

		size	= sizeof(TOUCHBAR_WORK) + sizeof(TOUCHBAR_ITEM_ICON) * p_setup->item_num;
		p_wk	= GFL_HEAP_AllocMemory( heapID, size );
		GFL_STD_MemClear( p_wk, size );
		p_wk->bg_frm	= p_setup->bar_frm;
		p_wk->trg			= TOUCHBAR_SELECT_NONE;
		p_wk->cont		= TOUCHBAR_SELECT_NONE;
		p_wk->tbl_len	= p_setup->item_num;

		GFL_STD_MemCopy( p_setup->p_item, p_wk->setup_tbl, sizeof(TOUCHBAR_ITEM_ICON)*p_setup->item_num );
	}

	{	
		CLSYS_DRAW_TYPE			clsys_draw_type;
		CLSYS_DEFREND_TYPE	clsys_def_type;
		PALTYPE							bgplttype;

		//OBJ�ǂݍ��ޏꏊ���`�F�b�N
		if( p_setup->bar_frm >= GFL_BG_FRAME0_S )
		{	
			clsys_draw_type	= CLSYS_DRAW_SUB;
			clsys_def_type	= CLSYS_DEFREND_SUB;
			bgplttype				= PALTYPE_SUB_BG;
		}
		else
		{	
			clsys_draw_type	= CLSYS_DRAW_MAIN;
			clsys_def_type	= CLSYS_DEFREND_MAIN;
			bgplttype				= PALTYPE_MAIN_BG;
		}

		//���\�[�X�ǂݍ���
		TouchBar_LoadResource( p_wk, clsys_draw_type, bgplttype, 
				p_setup->mapping, p_setup->bg_plt, p_setup->obj_plt, heapID );

		//CLWK�ǂݍ���
		TouchBar_CreateObj( p_wk, p_setup->p_unit, 
				p_setup->p_item, p_setup->item_num, clsys_def_type, heapID );
	}

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�j��
 *
 *	@param	TOUCHBAR_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_Exit( TOUCHBAR_WORK *p_wk )
{	
	//CLWK
	TouchBar_DeleteObj( p_wk );

	//���\�[�X�j��
	TouchBar_UnLoadResource( p_wk );
	
	//�j��
	GFL_HEAP_FreeMemory( p_wk );

}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	���C������
 *
 *	@param	TOUCHBAR_WORK *p_wk			���[�N
 *	@param	CURSOR_WORK	cp_cursor	�J�[�\���Ń{�^���������̂����������邽��
 *
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_Main( TOUCHBAR_WORK *p_wk )
{	

	
	//�^�b�`�̓���
	int i;
	u32 x, y;

	p_wk->trg		= TOUCHBAR_SELECT_NONE;
	p_wk->cont	= TOUCHBAR_SELECT_NONE;
	for( i = 0; i < p_wk->tbl_len; i++ )
	{	
		const TOUCHBAR_ITEM_ICON *cp_setup	= &p_wk->setup_tbl[i];

		if( GFL_UI_TP_GetPointTrg( &x, &y ) )
		{	
			if( ((u32)( x - cp_setup->pos.x) <= (u32)(TOUCHBAR_ICON_WIDTH))
					&	((u32)( y - cp_setup->pos.y) <= (u32)(TOUCHBAR_ICON_HEIGHT)))
			{
				p_wk->trg	= cp_setup->icon;
			}
		}
		else if( GFL_UI_TP_GetPointCont( &x, &y ) )
		{	
			if( ((u32)( x - cp_setup->pos.x) <= (u32)(TOUCHBAR_ICON_WIDTH))
					&	((u32)( y - cp_setup->pos.y) <= (u32)(TOUCHBAR_ICON_HEIGHT)))
			{
				p_wk->cont	= cp_setup->icon;
			}
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�I�����ꂽ���̂��擾
 *
 *	@param	const TOUCHBAR_WORK *cp_wk ���[�N
 *
 *	@return	TOUCHBAR_SELECT��
 */
//-----------------------------------------------------------------------------
TOUCHBAR_ICON TOUCHBAR_GetTrg( const TOUCHBAR_WORK *cp_wk )
{	
	return cp_wk->trg;
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�I�����ꂽ���̂��擾
 *
 *	@param	const TOUCHBAR_WORK *cp_wk ���[�N
 *
 *	@return	TOUCHBAR_SELECT��
 */
//-----------------------------------------------------------------------------
TOUCHBAR_ICON TOUCHBAR_GetCont( const TOUCHBAR_WORK *cp_wk )
{	
	return cp_wk->cont;
}
//=============================================================================
/**
 *			EXTERN
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�ǂݍ���
 *
 *	@param	TOUCHBAR_WORK *p_wk	���[�N
 *	@param	clsys_draw_type		�Z���ǂݍ��݃^�C�v
 *	@param	PALTYPE bgplttype	�p���b�g�ǂݍ��ݏꏊ
 *	@param	mapping						�Z���}�b�s���O���[�h
 *	@param	heapID						�ǂݍ��݃e���|�����p�q�[�vID
 */
//-----------------------------------------------------------------------------
static void TouchBar_LoadResource( TOUCHBAR_WORK *p_wk, CLSYS_DRAW_TYPE	clsys_draw_type, PALTYPE bgplttype, APP_COMMON_MAPPING mapping, u8 bg_plt, u8 obj_plt, HEAPID heapID )
{	
	ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heapID );

	//BG
	//�̈�̊m��
	GFL_ARCHDL_UTIL_TransVramPalette( p_handle, APP_COMMON_GetBarPltArcIdx(),
			bgplttype, bg_plt*0x20, TOUCHBAR_BG_PLT_NUM*0x20, heapID );
	//�m�ۂ����̈�ɓǂݍ���
	p_wk->chr_pos	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, APP_COMMON_GetBarCharArcIdx(), p_wk->bg_frm, TOUCHBAR_BG_CHARAAREA_SIZE, FALSE, heapID );
	GF_ASSERT_MSG( p_wk->chr_pos != GFL_ARCUTIL_TRANSINFO_FAIL, "TOUCHBAR:BG�L�����̈悪����܂���ł���\n" );
	//�X�N���[���̓�������ɒu���āA����32*3������������
	ARCHDL_UTIL_TransVramScreenEx( p_handle, APP_COMMON_GetBarScrnArcIdx(), p_wk->bg_frm,
			GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos), TOUCHBAR_MENUBAR_X, TOUCHBAR_MENUBAR_Y, 32, 24, 
			TOUCHBAR_MENUBAR_X, TOUCHBAR_MENUBAR_Y, TOUCHBAR_MENUBAR_W, TOUCHBAR_MENUBAR_H,
			bg_plt, FALSE, heapID );


	//OBJ
	//���ʃA�C�R�����\�[�X	
	p_wk->res[TOUCHBAR_RES_COMMON_PLT]	= GFL_CLGRP_PLTT_RegisterEx( p_handle,
			APP_COMMON_GetBarIconPltArcIdx(), clsys_draw_type, obj_plt*0x20, 0, TOUCHBAR_OBJ_PLT_NUM, heapID );	
	p_wk->res[TOUCHBAR_RES_COMMON_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
			APP_COMMON_GetBarIconCharArcIdx(), FALSE, clsys_draw_type, heapID );

	p_wk->res[TOUCHBAR_RES_COMMON_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
			APP_COMMON_GetBarIconCellArcIdx(mapping),
			APP_COMMON_GetBarIconAnimeArcIdx(mapping), heapID );

	GFL_ARC_CloseDataHandle( p_handle );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�j��
 *	
 *	@param	TOUCHBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void TouchBar_UnLoadResource( TOUCHBAR_WORK *p_wk )
{	
	//OBJ
	GFL_CLGRP_CELLANIM_Release( p_wk->res[TOUCHBAR_RES_COMMON_CEL] );
	GFL_CLGRP_CGR_Release( p_wk->res[TOUCHBAR_RES_COMMON_CHR] );
	GFL_CLGRP_PLTT_Release( p_wk->res[TOUCHBAR_RES_COMMON_PLT] );

	//BG
	GFL_BG_FreeCharacterArea(p_wk->bg_frm,
			GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos),
			GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->chr_pos));
}

//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�쐬
 *
 *	@param	TOUCHBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void TouchBar_CreateObj( TOUCHBAR_WORK *p_wk, GFL_CLUNIT* p_unit, const TOUCHBAR_ITEM_ICON *cp_setup_tbl, u16 tbl_len, CLSYS_DEFREND_TYPE	clsys_def_type, HEAPID heapID )
{	
	//CLWK�̍쐬
	int i;
	GFL_CLWK_DATA				cldata;
	const TOUCHBAR_ITEM_ICON	*cp_setup;

	GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

	for( i = 0; i < tbl_len; i++ )
	{	
		cp_setup	= &cp_setup_tbl[i];
		GF_ASSERT( cp_setup->icon < TOUCHBAR_ICON_MAX );
		GF_ASSERT_MSG( i < APBAR_ICON_REGISTER_MAX, "TOUCHBAR�F�A�C�R���o�^���������Ă��܂� now%d max%d\n", i, APBAR_ICON_REGISTER_MAX );

		cldata.pos_x	= cp_setup->pos.x;
		cldata.pos_y	= cp_setup->pos.y;
		cldata.anmseq	= sc_anmseq_tbl[ cp_setup->icon ];
		//clwk��TOUCHBAR_ICON���ɂ��Ă���
		GF_ASSERT_MSG( p_wk->p_clwk[cp_setup->icon] == NULL, "TOUCHBAR:SETUP�\���̂ɓ����A�C�R�����`���Ă��܂�\n" );
		p_wk->p_clwk[cp_setup->icon]	= GFL_CLACT_WK_Create( p_unit, 
				p_wk->res[TOUCHBAR_RES_COMMON_CHR],
				p_wk->res[TOUCHBAR_RES_COMMON_PLT],
				p_wk->res[TOUCHBAR_RES_COMMON_CEL],
				&cldata,
				clsys_def_type,
				heapID
				);
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�j��
 *
 *	@param	TOUCHBAR_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void TouchBar_DeleteObj( TOUCHBAR_WORK *p_wk )
{	
	int i;
	for( i = 0; i < APBAR_ICON_REGISTER_MAX; i++ )
	{	
		if( p_wk->p_clwk[i] )
		{	
			GFL_CLACT_WK_Remove( p_wk->p_clwk[i] );
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	Screen�f�[�^�́@VRAM�]���g���Łi�ǂݍ��񂾃X�N���[���̈ꕔ�͈͂��o�b�t�@�ɒ������j
 *
 *	@param	ARCHANDLE *handle	�n���h��
 *	@param	datID							�f�[�^ID
 *	@param	frm								�t���[��
 *	@param	chr_ofs						�L�����I�t�Z�b�g
 *	@param	src_x							�]���������W
 *	@param	src_y							�]����Y���W
 *	@param	src_w							�]������			//�f�[�^�̑S�̂̑傫��
 *	@param	src_h							�]��������		//�f�[�^�̑S�̂̑傫��
 *	@param	dst_x							�]����X���W
 *	@param	dst_y							�]����Y���W
 *	@param	dst_w							�]���敝
 *	@param	dst_h							�]���捂��
 *	@param	plt								�p���b�g�ԍ�
 *	@param	compressedFlag		���k�t���O
 *	@param	heapID						�ꎞ�o�b�t�@�p�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void ARCHDL_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h, u8 plt, BOOL compressedFlag, HEAPID heapID )
{	
	void *p_src_arc;
	NNSG2dScreenData* p_scr_data;

	//�ǂݍ���
	p_src_arc = GFL_ARCHDL_UTIL_Load( handle, datID, compressedFlag, GetHeapLowID(heapID) );

	//�A���p�b�N
	if(!NNS_G2dGetUnpackedScreenData( p_src_arc, &p_scr_data ) )
	{	
		GF_ASSERT(0);	//�X�N���[���f�[�^����Ȃ���
	}

	//�G���[
	GF_ASSERT( src_w * src_h * 2 <= p_scr_data->szByte );

	//�L�����I�t�Z�b�g�v�Z
	if( chr_ofs )
	{	
		int i;
		if( GFL_BG_GetScreenColorMode( frm ) == GX_BG_COLORMODE_16 )
		{
			u16 *p_scr16;
			
			p_scr16	=	(u16 *)&p_scr_data->rawData;
			for( i = 0; i < src_w * src_h ; i++ )
			{
				p_scr16[i]	+= chr_ofs;
			}	
		}
		else
		{	
			GF_ASSERT(0);	//256�ł͍���ĂȂ�
		}
	}

	//��������
	if( GFL_BG_GetScreenBufferAdrs( frm ) != NULL )
	{	
		GFL_BG_WriteScreenExpand( frm, dst_x, dst_y, dst_w, dst_h,
				&p_scr_data->rawData, src_x, src_y, src_w, src_h );	
		GFL_BG_ChangeScreenPalette( frm, dst_x, dst_y, dst_w, dst_h, plt );
		GFL_BG_LoadScreenReq( frm );
	}
	else
	{	
		GF_ASSERT(0);	//�o�b�t�@���Ȃ�
	}

	GFL_HEAP_FreeMemory( p_src_arc );
}	


