//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		poke2dgra.c
 *	@brief	�|�P����OBJ,BG�O���t�B�b�N
 *	@author	Toru=Nagihashi
 *	@data		2009.10.05
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//���\�[�X
#include "system/pokegra.h"
#include "app/app_menu_common.h"

//�O���Q��
#include "system/poke2dgra.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	NNSG2dCharacterData **ncg_data
 *	@param	POKEMON_PASO_PARAM* ppp
 *	@param	dir
 *	@param	heapID 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
void* POKE2DGRA_LoadCharacter( NNSG2dCharacterData **ncg_data, const POKEMON_PASO_PARAM* ppp, int dir, HEAPID heapID )
{	
	u32 cgr;
	void *p_buf;

	//���\�[�X�󂯎��
	{	
		int	mons_no = PPP_Get( ppp, ID_PARA_monsno,	NULL );
		int	form_no = PPP_Get( ppp, ID_PARA_form_no,NULL );
		int	sex		= PPP_Get( ppp, ID_PARA_sex,	NULL );
		int	rare	= PPP_CheckRare( ppp );
		cgr	= POKEGRA_GetCgrArcIndex( mons_no, form_no, sex, rare, dir );
	}
	//���\�[�X��OBJ�Ƃ��č���Ă���̂ŁALoadOBJ����Ȃ��Ɠǂݍ��߂Ȃ�
	p_buf = GFL_ARC_UTIL_LoadOBJCharacter( POKEGRA_GetArcID(), cgr, FALSE, ncg_data, heapID );

	return p_buf;
}


//=============================================================================
/**
 *					BG�֌W
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BG�\��t���p�ɁA�L�����A��گēǂݍ���
 *
 *	@param	const POKEMON_PASO_PARAM* ppp	�|�P�����p�[�\�i��
 *	@param	dir														�G�̕���
 *	@param	frm														�Ǎ���t���[��
 *	@param	chr_offs											�L�����I�t�Z�b�g
 *	@param	plt_offs											�p���b�g�I�t�Z�b�g
 *	@param	heapID												�q�[�vID
 */
//-----------------------------------------------------------------------------
void POKE2DGRA_BG_TransResource( const POKEMON_PASO_PARAM* ppp, int dir, u32 frm, u32 chr_offs, u32 plt_offs, HEAPID heapID )
{
	PALTYPE	paltype;
	ARCID		arc;
	ARCDATID	cgr, plt;

	//�p���b�g�Ǎ���擾
	if( frm < GFL_BG_FRAME0_S )
	{	
		paltype	= PALTYPE_MAIN_BG;
	}
	else
	{	
		paltype	= PALTYPE_SUB_BG;
	}
	
	//���\�[�X�󂯎��
	{	
		int	mons_no = PPP_Get( ppp, ID_PARA_monsno,	NULL );
		int	form_no = PPP_Get( ppp, ID_PARA_form_no,NULL );
		int	sex		= PPP_Get( ppp, ID_PARA_sex,	NULL );
		int	rare	= PPP_CheckRare( ppp );

		arc	= POKEGRA_GetArcID();
		cgr	= POKEGRA_GetCgrArcIndex( mons_no, form_no, sex, rare, dir );
		plt	= POKEGRA_GetPalArcIndex( mons_no, form_no, sex, rare, dir );
	}

	//���\�[�X�ǂݍ���
	{	
		ARCHANDLE *p_handle;
		void *p_buff;
		NNSG2dCharacterData *ncg_data;

		p_handle	= GFL_ARC_OpenDataHandle( arc, heapID );

		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, plt,
				paltype, plt_offs, POKEGRA_POKEMON_PLT_SIZE, heapID );

		p_buff	= POKE2DGRA_LoadCharacter( &ncg_data, ppp, dir, heapID );
		GFL_BG_LoadCharacter( frm, ncg_data->pRawData, POKEGRA_POKEMON_CHARA_SIZE, chr_offs );

		GFL_ARC_CloseDataHandle( p_handle );

		GFL_HEAP_FreeMemory( p_buff );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG�\��t���p�ɁA�L�����A��گēǂݍ��݃G���A�}�l�[�W����
 *
 *	@param	const POKEMON_PASO_PARAM* ppp	�|�P�����p�[�\�i��
 *	@param	dir														�G�̕���
 *	@param	frm														�Ǎ���t���[��
 *	@param	chr_offs											�L�����I�t�Z�b�g
 *	@param	plt														�p���b�g�I�t�Z�b�g
 *	@param	heapID												�q�[�vID
 *
 *
 */
//-----------------------------------------------------------------------------
GFL_ARCUTIL_TRANSINFO POKE2DGRA_BG_TransResourceByAreaMan( const POKEMON_PASO_PARAM* ppp, int dir, u32 frm, u32 plt_offs, HEAPID heapID )
{	
	PALTYPE	paltype;
	ARCID		arc;
	ARCDATID	cgr, plt;
	GFL_ARCUTIL_TRANSINFO info;

	//�p���b�g�Ǎ���擾
	if( frm < GFL_BG_FRAME0_S )
	{	
		paltype	= PALTYPE_MAIN_BG;
	}
	else
	{	
		paltype	= PALTYPE_SUB_BG;
	}
	
	//���\�[�X�󂯎��
	{	
		int	mons_no = PPP_Get( ppp, ID_PARA_monsno,	NULL );
		int	form_no = PPP_Get( ppp, ID_PARA_form_no,NULL );
		int	sex		= PPP_Get( ppp, ID_PARA_sex,	NULL );
		int	rare	= PPP_CheckRare( ppp );

		arc	= POKEGRA_GetArcID();
		cgr	= POKEGRA_GetCgrArcIndex( mons_no, form_no, sex, rare, dir );
		plt	= POKEGRA_GetPalArcIndex( mons_no, form_no, sex, rare, dir );
	}

	//���\�[�X�ǂݍ���
	{	
		ARCHANDLE *p_handle;
		u32 pos;
		void *p_buff;
		NNSG2dCharacterData *ncg_data;

		p_handle	= GFL_ARC_OpenDataHandle( arc, heapID );

		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, plt,
				paltype, plt_offs, POKEGRA_POKEMON_PLT_SIZE, heapID );

		p_buff	= POKE2DGRA_LoadCharacter( &ncg_data, ppp, dir, heapID );
		pos = GFL_BG_LoadCharacterAreaMan( frm, ncg_data->pRawData, POKEGRA_POKEMON_CHARA_SIZE );

		if(  pos == AREAMAN_POS_NOTFOUND )
		{	
			info	= GFL_ARCUTIL_TRANSINFO_FAIL;
		}
		else
		{	
			info	= GFL_ARCUTIL_TRANSINFO_Make(pos, POKEGRA_POKEMON_CHARA_SIZE);
		}

		GFL_ARC_CloseDataHandle( p_handle );
	}

	return info;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[���ɏ�������
 *
 *	@param	u32 frm		�t���[��
 *	@param	chr_offs	�L�����I�t�Z�b�g
 *	@param	pal_offs	�p���b�g�I�t�Z�b�g
 *	@param	x					�������݊J�n�ʒuX
 *	@param	y					�������݊J�n�ʒuY
 *
 */
//-----------------------------------------------------------------------------
void POKE2DGRA_BG_WriteScreen( u32 frm, u32 chr_offs, u32 pal_offs, u16 x, u16 y )
{	
	int i, j;
	int cnt;

	cnt	= 0;
	//OAM��8x8,4x8,8x4,4x4�̏��ɕ���ł���
	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 8; j++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + j, y + i, 1, 1, pal_offs );
			cnt++;
		}
	}
	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 4; j++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + j + 8, y + i, 1, 1, pal_offs );
			cnt++;
		}
	}
	for( i = 0; i < 4; i++ ){
		for( j = 0; j < 8; j++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + j, y + i + 8, 1, 1, pal_offs );
			cnt++;
		}
	}
	for( i = 0; i < 4; i++ ){
		for( j = 0; j < 4; j++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + j + 8, y + i + 8, 1, 1, pal_offs );
			cnt++;
		}
	}
}

//=============================================================================
/**
 *					OBJ�֌W
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�n���h���I�[�v��
 *	@param	HEAPID	�q�[�vID
 *	@return	�n���h��
 */
//-----------------------------------------------------------------------------
ARCHANDLE *POKE2DGRA_OpenHandle( HEAPID heapID )
{	
	return GFL_ARC_OpenDataHandle( POKEGRA_GetArcID(), heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����̃p���b�g�ǂݍ���
 *
 *	@param	ARCHANDLE *p_handle			�n���h��
 *	@param	POKEMON_PASO_PARAM* ppp	�|�P�����p�[�\�i���p�����[�^
 *	@param	dir				�|�P�����̕���
 *	@param	vramType	�ǂݍ��݃^�C�v
 *	@param	byteOffs	�p���b�g�ǂݍ��݃I�t�Z�b�g
 *	@param	heapID		�q�[�vID 
 *
 *	@return	�o�^ID
 */
//-----------------------------------------------------------------------------
u32 POKE2DGRA_OBJ_PLTT_Register( ARCHANDLE *p_handle, const POKEMON_PASO_PARAM* ppp, int dir, CLSYS_DRAW_TYPE vramType, u16 byteOffs, HEAPID heapID )
{	
	u32 plt;
	//���\�[�X�󂯎��
	{	
		int	mons_no = PPP_Get( ppp, ID_PARA_monsno,	NULL );
		int	form_no = PPP_Get( ppp, ID_PARA_form_no,NULL );
		int	sex		= PPP_Get( ppp, ID_PARA_sex,	NULL );
		int	rare	= PPP_CheckRare( ppp );

		plt	= POKEGRA_GetPalArcIndex( mons_no, form_no, sex, rare, dir );
	}

	//�ǂݍ���
	return GFL_CLGRP_PLTT_RegisterEx( p_handle, plt, vramType, byteOffs, 0, POKEGRA_POKEMON_PLT_NUM, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����L�����N�^�[�ǂݍ���
 *
 *	@param	ARCHANDLE *p_handle			�n���h��
 *	@param	POKEMON_PASO_PARAM* ppp	�|�P�����p�[�\�i���p�����[�^
 *	@param	dir											�|�P�����̕���
 *	@param	vramType								�ǂݍ��݃^�C�v
 *	@param	heapID									�q�[�vID
 *
 *	@return	�o�^�ԍ�
 */
//-----------------------------------------------------------------------------
u32 POKE2DGRA_OBJ_CGR_Register( ARCHANDLE *p_handle, const POKEMON_PASO_PARAM* ppp, int dir, CLSYS_DRAW_TYPE vramType, HEAPID heapID )
{	
	u32 cgr;
	//���\�[�X�󂯎��
	{	
		int	mons_no = PPP_Get( ppp, ID_PARA_monsno,	NULL );
		int	form_no = PPP_Get( ppp, ID_PARA_form_no,NULL );
		int	sex		= PPP_Get( ppp, ID_PARA_sex,	NULL );
		int	rare	= PPP_CheckRare( ppp );

		cgr	= POKEGRA_GetCgrArcIndex( mons_no, form_no, sex, rare, dir );
	}

	//�ǂݍ���
	return GFL_CLGRP_CGR_Register( p_handle, cgr, FALSE, vramType, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����Z���ǂݍ���
 *
 *	@param	const POKEMON_PASO_PARAM* ppp	�|�P�����p�[�\�i���p�����[�^
 *	@param	dir														�|�P�����̕���
 *	@param	mapping												�}�b�s���O���[�h
 *	@param	vramType											�ǂݍ��݃^�C�v
 *	@param	heapID												�q�[�vID
 *
 *	@return
 */
//-----------------------------------------------------------------------------
extern u32 POKE2DGRA_OBJ_CELLANM_Register( const POKEMON_PASO_PARAM* ppp, int dir, APP_COMMON_MAPPING mapping, CLSYS_DRAW_TYPE vramType, HEAPID heapID )
{	
	ARCHANDLE *p_handle;
	u32 cel, anm;
	u32 ret;

	{	
		cel	= APP_COMMON_GetDummyCellArcIdx( mapping );
		anm	= APP_COMMON_GetDummyAnimeArcIdx( mapping );
	}

	p_handle	= GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heapID );
	ret	= GFL_CLGRP_CELLANIM_Register( p_handle, cel, anm, heapID );
	GFL_ARC_CloseDataHandle( p_handle );

	return ret;
}
