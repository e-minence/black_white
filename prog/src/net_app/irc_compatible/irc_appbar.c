//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_appbar.c
 *	@brief	�ԊO���`�F�b�N�p����ʃo�[
 *	@author	Toru=Nagihashi
 *	@data		2009.07.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>

//mine
#include "net_app/irc_appbar.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	APPBAR
//=====================================
//���\�[�X
enum
{	
	APPBAR_RES_COMMON_PLT,
	APPBAR_RES_COMMON_CHR,
	APPBAR_RES_COMMON_CEL,

	APPBAR_RES_MAX
};

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	����ʃo�[(�A�v���P�[�V�����o�[�Ə���ɖ���)
//=====================================
struct _APPBAR_WORK
{
	GFL_CLWK	*p_clwk[APPBAR_ICON_MAX];
	u32				res[APPBAR_RES_MAX];
	u32				bg_frm;
	GFL_ARCUTIL_TRANSINFO				chr_pos;
	s32				trg;
	s32				cont;
	u32				tbl_len;
	APPBAR_SETUP	setup_tbl[0];
};


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//���\�[�X
static void AppBar_LoadResource( APPBAR_WORK *p_wk, CLSYS_DRAW_TYPE	clsys_draw_type, PALTYPE bgplttype, APP_COMMON_MAPPING mapping, u8 bg_plt, u8 obj_plt, HEAPID heapID );
static void AppBar_UnLoadResource( APPBAR_WORK *p_wk );
//obj
static void AppBar_CreateObj( APPBAR_WORK *p_wk, GFL_CLUNIT* p_unit, const APPBAR_SETUP *cp_setup_tbl, u16 tbl_len, CLSYS_DEFREND_TYPE	clsys_def_type, HEAPID heapID );
static void AppBar_DeleteObj( APPBAR_WORK *p_wk );
//etc
static void ARCHDL_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h,  u8 plt, BOOL compressedFlag, HEAPID heapID );

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	�A�j���V�[�P���X�e�[�u��
//	APPBAR_ICON�ƑΉ������e�[�u��
//=====================================
static const u16 sc_anmseq_tbl[]	=
{	
	APP_COMMON_BARICON_CLOSE,
	APP_COMMON_BARICON_RETURN,
	APP_COMMON_BARICON_CURSOR_DOWN,
	APP_COMMON_BARICON_CURSOR_UP,
	APP_COMMON_BARICON_CURSOR_LEFT,
	APP_COMMON_BARICON_CURSOR_RIGHT,
	APP_COMMON_BARICON_CHECK_OFF,
};

//-------------------------------------
///	�f�t�H���g���W
//=====================================
static const APPBAR_SETUP sc_appbar_default_setup[]		=
{
	{	
		APPBAR_ICON_CLOSE,
		APPBAR_ICON_CLOSE_X,
		APPBAR_ICON_Y
	},
	{	
		APPBAR_ICON_RETURN,
		APPBAR_ICON_RETURN_X,
		APPBAR_ICON_Y
	},
	{	
		APPBAR_ICON_CUR_D,
		APPBAR_ICON_CUR_D_X,
		APPBAR_ICON_Y
	},
	{	
		APPBAR_ICON_CUR_U,
		APPBAR_ICON_CUR_U_X,
		APPBAR_ICON_Y
	},
	{	
		APPBAR_ICON_CUR_L,
		APPBAR_ICON_CUR_L_X,
		APPBAR_ICON_Y
	},
	{	
		APPBAR_ICON_CUR_R,
		APPBAR_ICON_CUR_R_X,
		APPBAR_ICON_Y
	},
	{	
		APPBAR_ICON_CHECK,
		APPBAR_ICON_CHECK_X,
		APPBAR_ICON_Y
	}
};

//=============================================================================
/**
 *					GLOBAL
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	������
 *
 *	@param	APPBAR_WORK *p_wk	���[�N
 *	@param	mask							���[�h�}�X�N
 *	@param	p_unit						APPBAR��OBJ�����pp_unit
 *	@param	bar_frm						�g�p����BG�ʁi�����Ƀ��C�����T�u���𔻒肷��̂ɂ��g�p�j
 *	@param	bg_plt						�g�p����BG�p���b�g�ԍ�
 *	@param	obj_plt						�g�p����OBJ�p���b�g�ԍ�
 *	@param	heapID						�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
APPBAR_WORK * APPBAR_Init( APPBAR_OPTION_MASK mask, GFL_CLUNIT* p_unit, u8 bar_frm, u8 bg_plt, u8 obj_plt, APP_COMMON_MAPPING mapping, HEAPID heapID )
{	
	APPBAR_SETUP	*p_setup_tbl;
	APPBAR_WORK		*p_wk;
	u16 tbl_len;

	//�}�X�N�𐔂���
	{	
		int i;
		tbl_len	= 0;
		for( i = 0; i < APPBAR_ICON_MAX; i++ )
		{	
			if( mask & (1<<i) )
			{	
				tbl_len++;
			}
		}
	}
	
	//�ꎞ�ݒ�e�[�u�����������@
	p_setup_tbl	= GFL_HEAP_AllocMemoryLo( heapID, sizeof(APPBAR_SETUP)*tbl_len );
	GFL_STD_MemClear( p_setup_tbl, sizeof(APPBAR_SETUP)*tbl_len );

	//�ݒ�e�[�u��������
	{	
		int i;
		int cnt;
		cnt	= 0;
		for( i = 0; i < APPBAR_ICON_MAX; i++ )
		{	
			if( mask & (1<<i) )
			{
				p_setup_tbl[cnt]	= sc_appbar_default_setup[i];
				cnt++;
			}
		}
	}

	//���[�N�쐬
	p_wk	= APPBAR_InitEx( p_setup_tbl, tbl_len, p_unit, bar_frm, bg_plt, obj_plt, mapping, heapID );

	//�ݒ�e�[�u���j��
	GFL_HEAP_FreeMemory( p_setup_tbl );

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�������g���Łi�A�C�R���ʒu�w��Łj
 *
 *	@param	const APPBAR_SETUP *cp_setup_tbl	�ݒ�e�[�u��
 *	@param	tbl_len	�e�[�u����
 *	@param	p_unit	OBJ�ݒ�UNIT
 *	@param	bar_frm	�g�pBG�ԍ�	�i�����ɏ㉺��ʂ𒲂ׂĂ��܂��j
 *	@param	bg_plt	�g�pBG�pPLTNUM	
 *	@param	obj_plt	�g�pOBJ�pPLTNUM
 *	@param	mapping	OBJ�}�b�s���O���[�h
 *	@param	heapID	�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
APPBAR_WORK * APPBAR_InitEx( const APPBAR_SETUP *cp_setup_tbl, u16 tbl_len, GFL_CLUNIT* p_unit, u8 bar_frm, u8 bg_plt, u8 obj_plt, APP_COMMON_MAPPING mapping, HEAPID heapID )
{	
	APPBAR_WORK	*p_wk;

	GF_ASSERT( p_unit );

	//���[�N�쐬�A������
	{	
		u32 size;

		size	= sizeof(APPBAR_WORK) + sizeof(APPBAR_SETUP) * tbl_len;
		p_wk	= GFL_HEAP_AllocMemory( heapID, size );
		GFL_STD_MemClear( p_wk, size );
		p_wk->bg_frm	= bar_frm;
		p_wk->trg			= APPBAR_SELECT_NONE;
		p_wk->cont		= APPBAR_SELECT_NONE;
		p_wk->tbl_len	= tbl_len;

		GFL_STD_MemCopy( cp_setup_tbl, p_wk->setup_tbl, sizeof(APPBAR_SETUP)*tbl_len );
	}

	{	
		CLSYS_DRAW_TYPE			clsys_draw_type;
		CLSYS_DEFREND_TYPE	clsys_def_type;
		PALTYPE							bgplttype;

		//OBJ�ǂݍ��ޏꏊ���`�F�b�N
		if( bar_frm >= GFL_BG_FRAME0_S )
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
		AppBar_LoadResource( p_wk, clsys_draw_type, bgplttype, mapping, bg_plt, obj_plt, heapID );

		//CLWK�ǂݍ���
		AppBar_CreateObj( p_wk, p_unit, cp_setup_tbl, tbl_len, clsys_def_type, heapID );
	}

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	�j��
 *
 *	@param	APPBAR_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
void APPBAR_Exit( APPBAR_WORK *p_wk )
{	
	//CLWK
	AppBar_DeleteObj( p_wk );

	//���\�[�X�j��
	AppBar_UnLoadResource( p_wk );
	
	//�j��
	GFL_HEAP_FreeMemory( p_wk );

}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	���C������
 *
 *	@param	APPBAR_WORK *p_wk			���[�N
 *	@param	CURSOR_WORK	cp_cursor	�J�[�\���Ń{�^���������̂����������邽��
 *
 */
//-----------------------------------------------------------------------------
void APPBAR_Main( APPBAR_WORK *p_wk )
{	

	
	//�^�b�`�̓���
	int i;
	u32 x, y;

	p_wk->trg		= APPBAR_SELECT_NONE;
	p_wk->cont	= APPBAR_SELECT_NONE;
	for( i = 0; i < p_wk->tbl_len; i++ )
	{	
		const APPBAR_SETUP	*cp_setup	= &p_wk->setup_tbl[i];

		if( GFL_UI_TP_GetPointTrg( &x, &y ) )
		{	
			if( ((u32)( x - cp_setup->pos.x) <= (u32)(APPBAR_ICON_WIDTH))
					&	((u32)( y - cp_setup->pos.y) <= (u32)(APPBAR_ICON_HEIGHT)))
			{
				p_wk->trg	= cp_setup->icon;
			}
		}
		else if( GFL_UI_TP_GetPointCont( &x, &y ) )
		{	
			if( ((u32)( x - cp_setup->pos.x) <= (u32)(APPBAR_ICON_WIDTH))
					&	((u32)( y - cp_setup->pos.y) <= (u32)(APPBAR_ICON_HEIGHT)))
			{
				p_wk->cont	= cp_setup->icon;
			}
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	�I�����ꂽ���̂��擾
 *
 *	@param	const APPBAR_WORK *cp_wk ���[�N
 *
 *	@return	APPBAR_SELECT��
 */
//-----------------------------------------------------------------------------
APPBAR_ICON APPBAR_GetTrg( const APPBAR_WORK *cp_wk )
{	
	return cp_wk->trg;
}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	�I�����ꂽ���̂��擾
 *
 *	@param	const APPBAR_WORK *cp_wk ���[�N
 *
 *	@return	APPBAR_SELECT��
 */
//-----------------------------------------------------------------------------
APPBAR_ICON APPBAR_GetCont( const APPBAR_WORK *cp_wk )
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
 *	@param	APPBAR_WORK *p_wk	���[�N
 *	@param	clsys_draw_type		�Z���ǂݍ��݃^�C�v
 *	@param	PALTYPE bgplttype	�p���b�g�ǂݍ��ݏꏊ
 *	@param	mapping						�Z���}�b�s���O���[�h
 *	@param	heapID						�ǂݍ��݃e���|�����p�q�[�vID
 */
//-----------------------------------------------------------------------------
static void AppBar_LoadResource( APPBAR_WORK *p_wk, CLSYS_DRAW_TYPE	clsys_draw_type, PALTYPE bgplttype, APP_COMMON_MAPPING mapping, u8 bg_plt, u8 obj_plt, HEAPID heapID )
{	
	ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heapID );

	//BG
	//�̈�̊m��
	GFL_ARCHDL_UTIL_TransVramPalette( p_handle, APP_COMMON_GetBarPltArcIdx(),
			bgplttype, bg_plt*0x20, APPBAR_BG_PLT_NUM*0x20, heapID );
	//�m�ۂ����̈�ɓǂݍ���
	p_wk->chr_pos	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, APP_COMMON_GetBarCharArcIdx(), p_wk->bg_frm, APPBAR_BG_CHARAAREA_SIZE, FALSE, heapID );
	GF_ASSERT_MSG( p_wk->chr_pos != GFL_ARCUTIL_TRANSINFO_FAIL, "APPBAR:BG�L�����̈悪����܂���ł���\n" );
	//�X�N���[���̓�������ɒu���āA����32*3������������
	ARCHDL_UTIL_TransVramScreenEx( p_handle, APP_COMMON_GetBarScrnArcIdx(), p_wk->bg_frm,
			GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos), APPBAR_MENUBAR_X, APPBAR_MENUBAR_Y, 32, 24, 
			APPBAR_MENUBAR_X, APPBAR_MENUBAR_Y, APPBAR_MENUBAR_W, APPBAR_MENUBAR_H,
			bg_plt, FALSE, heapID );


	//OBJ
	//���ʃA�C�R�����\�[�X	
	p_wk->res[APPBAR_RES_COMMON_PLT]	= GFL_CLGRP_PLTT_RegisterEx( p_handle,
			APP_COMMON_GetBarIconPltArcIdx(), clsys_draw_type, obj_plt*0x20, 0, APPBAR_OBJ_PLT_NUM, heapID );	
	p_wk->res[APPBAR_RES_COMMON_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
			APP_COMMON_GetBarIconCharArcIdx(), FALSE, clsys_draw_type, heapID );

	p_wk->res[APPBAR_RES_COMMON_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
			APP_COMMON_GetBarIconCellArcIdx(mapping),
			APP_COMMON_GetBarIconAnimeArcIdx(mapping), heapID );

	GFL_ARC_CloseDataHandle( p_handle );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�j��
 *	
 *	@param	APPBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void AppBar_UnLoadResource( APPBAR_WORK *p_wk )
{	
	//OBJ
	GFL_CLGRP_CELLANIM_Release( p_wk->res[APPBAR_RES_COMMON_CEL] );
	GFL_CLGRP_CGR_Release( p_wk->res[APPBAR_RES_COMMON_CHR] );
	GFL_CLGRP_PLTT_Release( p_wk->res[APPBAR_RES_COMMON_PLT] );

	//BG
	GFL_BG_FreeCharacterArea(p_wk->bg_frm,
			GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos),
			GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->chr_pos));
}

//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�쐬
 *
 *	@param	APPBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void AppBar_CreateObj( APPBAR_WORK *p_wk, GFL_CLUNIT* p_unit, const APPBAR_SETUP *cp_setup_tbl, u16 tbl_len, CLSYS_DEFREND_TYPE	clsys_def_type, HEAPID heapID )
{	
	//CLWK�̍쐬
	int i;
	GFL_CLWK_DATA				cldata;
	const APPBAR_SETUP	*cp_setup;

	GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

	for( i = 0; i < tbl_len; i++ )
	{	
		cp_setup	= &cp_setup_tbl[i];
		GF_ASSERT( cp_setup->icon < APPBAR_ICON_MAX );

		cldata.pos_x	= cp_setup->pos.x;
		cldata.pos_y	= cp_setup->pos.y;
		cldata.anmseq	= sc_anmseq_tbl[ cp_setup->icon ];
		//clwk��APPBAR_ICON���ɂ��Ă���
		GF_ASSERT_MSG( p_wk->p_clwk[cp_setup->icon] == NULL, "APPBAR:SETUP�\���̂ɓ����A�C�R�����`���Ă��܂�\n" );
		p_wk->p_clwk[cp_setup->icon]	= GFL_CLACT_WK_Create( p_unit, 
				p_wk->res[APPBAR_RES_COMMON_CHR],
				p_wk->res[APPBAR_RES_COMMON_PLT],
				p_wk->res[APPBAR_RES_COMMON_CEL],
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
 *	@param	APPBAR_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void AppBar_DeleteObj( APPBAR_WORK *p_wk )
{	
	int i;
	for( i = 0; i < APPBAR_ICON_MAX; i++ )
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

