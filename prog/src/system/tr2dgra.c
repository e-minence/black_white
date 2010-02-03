//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		tr2dgra.c
 *	@brief	�g���[�i�[OBJ,BG�O���t�B�b�N�i�i�M�[�������poke2dgra.c���ۃp�N���j
 *	@author	sogabe
 *	@data		2010.02.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�A�[�J�C�u
#include "arc_def.h"
#include "pokegra/pokegra_wb.naix"

//���\�[�X
#include "system/pokegra.h"
#include "app/app_menu_common.h"

//�O���Q��
#include "system/tr2dgra.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//=============================================================================
/**
 *					�v���g�^�C�v
 */
//=============================================================================
static void* TR2DGRA_LoadCharacter( NNSG2dCharacterData **ncg_data, int trtype, HEAPID heapID );
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
 *	@param	trtype
 *	@param	heapID 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void* TR2DGRA_LoadCharacter( NNSG2dCharacterData **ncg_data, int trtype, HEAPID heapID )
{	
	u32 cgr;
	void *p_buf;

	//���\�[�X�󂯎��
	cgr	= TRGRA_GetCgrArcIndex( trtype );
	//���\�[�X��OBJ�Ƃ��č���Ă���̂ŁALoadOBJ����Ȃ��Ɠǂݍ��߂Ȃ�
	p_buf = GFL_ARC_UTIL_LoadOBJCharacter( TRGRA_GetArcID(), cgr, FALSE, ncg_data, heapID );

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
 *	@param	trtype		�g���[�i�[�^�C�v
 *	@param	frm				�Ǎ���t���[��
 *	@param	chr_offs	�L�����I�t�Z�b�g
 *	@param	plt_offs	�p���b�g�I�t�Z�b�g(0�`15)
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
void TR2DGRA_BG_TransResource( int trtype, u32 frm, u32 chr_offs, u32 plt_offs, HEAPID heapID )
{
	PALTYPE	paltype;
	ARCID		arc;
	ARCDATID	plt;

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
		arc	= TRGRA_GetArcID();
		plt	= TRGRA_GetPalArcIndex( trtype );
	}

	//���\�[�X�ǂݍ���
	{	
		ARCHANDLE *p_handle;
		void *p_buff;
		NNSG2dCharacterData *ncg_data;

		p_handle	= GFL_ARC_OpenDataHandle( arc, heapID );

		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, plt,
				paltype, plt_offs*0x20, TRGRA_TRAINER_PLT_SIZE, heapID );

		p_buff	= TR2DGRA_LoadCharacter( &ncg_data, trtype, heapID );
		GFL_BG_LoadCharacter( frm, ncg_data->pRawData, TRGRA_TRAINER_CHARA_SIZE, chr_offs );

		GFL_ARC_CloseDataHandle( p_handle );

		GFL_HEAP_FreeMemory( p_buff );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG�\��t���p�ɁA�L�����A��گēǂݍ��݃G���A�}�l�[�W����
 *
 *	@param	trtype		�g���[�i�[�^�C�v
 *	@param	frm				�Ǎ���t���[��
 *	@param	chr_offs	�L�����I�t�Z�b�g
 *	@param	plt				�p���b�g�I�t�Z�b�g(0�`15)
 *	@param	heapID		�q�[�vID
 *
 *
 */
//-----------------------------------------------------------------------------
GFL_ARCUTIL_TRANSINFO TR2DGRA_BG_TransResourceByAreaMan( int trtype, u32 frm, u32 plt_offs, HEAPID heapID )
{	
	PALTYPE	paltype;
	ARCID		arc;
	ARCDATID	plt;
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
		arc	= TRGRA_GetArcID();
		plt	= TRGRA_GetPalArcIndex( trtype );
	}

	//���\�[�X�ǂݍ���
	{	
		ARCHANDLE *p_handle;
		u32 pos;
		void *p_buff;
		NNSG2dCharacterData *ncg_data;

		p_handle	= GFL_ARC_OpenDataHandle( arc, heapID );

		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, plt,
				paltype, plt_offs, TRGRA_TRAINER_PLT_SIZE, heapID );

		p_buff	= TR2DGRA_LoadCharacter( &ncg_data, trtype, heapID );
		pos = GFL_BG_LoadCharacterAreaMan( frm, ncg_data->pRawData, TRGRA_TRAINER_CHARA_SIZE );

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
void TR2DGRA_BG_WriteScreen( u32 frm, u32 chr_offs, u32 pal_offs, u16 x, u16 y )
{	
	int xx, yy;
	int cnt;

	cnt	= 0;
	//OAM��8x8,2x4,2x4,4x2,4x2,2x2�̏��ɕ���ł���
	for( yy = 0; yy < 8; yy++ ){
		for( xx = 0; xx < 8; xx++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + xx, y + yy, 1, 1, pal_offs );
			cnt++;
		}
	}

	for( yy = 0; yy < 4; yy++ ){
		for( xx = 0; xx < 2; xx++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + xx + 8, y + yy, 1, 1, pal_offs );
			cnt++;
		}
	}

	for( yy = 0; yy < 4; yy++ ){
		for( xx = 0; xx < 2; xx++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + xx + 8, y + yy + 4, 1, 1, pal_offs );
			cnt++;
		}
	}

	for( yy = 0; yy < 2; yy++ ){
		for( xx = 0; xx < 4; xx++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + xx, y + yy + 8, 1, 1, pal_offs );
			cnt++;
		}
	}

	for( yy = 0; yy < 2; yy++ ){
		for( xx = 0; xx < 4; xx++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + xx + 4, y + yy + 8, 1, 1, pal_offs );
			cnt++;
		}
	}

	for( yy = 0; yy < 2; yy++ ){
		for( xx = 0; xx < 2; xx++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + xx + 8, y + yy + 8, 1, 1, pal_offs );
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
ARCHANDLE *TR2DGRA_OpenHandle( HEAPID heapID )
{	
	return GFL_ARC_OpenDataHandle( TRGRA_GetArcID(), heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�g���[�i�[�̃p���b�g�ǂݍ���
 *
 *	@param	ARCHANDLE *p_handle			�n���h��
 *	@param	trtype									�g���[�i�[�^�C�v
 *	@param	vramType	              �ǂݍ��݃^�C�v
 *	@param	byteOffs	              �p���b�g�ǂݍ��݃I�t�Z�b�g
 *	@param	heapID		              �q�[�vID 
 *
 *	@return	�o�^ID
 */
//-----------------------------------------------------------------------------
u32 TR2DGRA_OBJ_PLTT_Register( ARCHANDLE *p_handle, int trtype, CLSYS_DRAW_TYPE vramType, u16 byteOffs, HEAPID heapID )
{	
	u32 plt;
	plt	= TRGRA_GetPalArcIndex( trtype );

	//�ǂݍ���
	return GFL_CLGRP_PLTT_RegisterEx( p_handle, plt, vramType, byteOffs, 0, TRGRA_TRAINER_PLT_NUM, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�g���[�i�[�L�����N�^�[�ǂݍ���
 *
 *	@param	p_handle								�A�[�N�n���h��
 *	@param	trtype									�����X�^�[�ԍ�
 *	@param	vramType								�ǂݍ��݃^�C�v
 *	@param	heapID									�q�[�vID
 *
 *	@return	�o�^�ԍ�
 */
//-----------------------------------------------------------------------------
u32 TR2DGRA_OBJ_CGR_Register( ARCHANDLE *p_handle, int trtype, CLSYS_DRAW_TYPE vramType, HEAPID heapID )
{	
	u32 cgr;
	cgr	= TRGRA_GetCgrArcIndex( trtype );
	//�ǂݍ���
	return GFL_CLGRP_CGR_Register( p_handle, cgr, FALSE, vramType, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�g���[�i�[�Z���ǂݍ���
 *
 *	@param	trtype		�g���[�i�[�^�C�v
 *	@param	mapping		�}�b�s���O���[�h
 *	@param	vramType	�ǂݍ��݃^�C�v
 *	@param	heapID		�q�[�vID
 *
 *	@return
 */
//-----------------------------------------------------------------------------
u32 TR2DGRA_OBJ_CELLANM_Register( int trtype, APP_COMMON_MAPPING mapping, CLSYS_DRAW_TYPE vramType, HEAPID heapID )
{	
	ARCHANDLE *p_handle;
	u32 cel, anm;
	u32 ret;

  {	
    cel	= APP_COMMON_GetTrDummyCellArcIdx( mapping );
    anm	= APP_COMMON_GetTrDummyAnimeArcIdx( mapping );
  }

  p_handle	= GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heapID );
  ret	= GFL_CLGRP_CELLANIM_Register( p_handle, cel, anm, heapID );
  GFL_ARC_CloseDataHandle( p_handle );

	return ret;
}
