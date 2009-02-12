//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wf2dmap_map.c
 *	@brief		�}�b�v�f�[�^�Ǘ��V�X�e��
 *	@author		tomoya takahashi
 *	@data		2007.03.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "net_app/wifi2dmap/wf2dmap_map.h"

//-----------------------------------------------------------------------------
/**
 *					�R�[�f�B���O�K��
 *		���֐���
 *				�P�����ڂ͑啶������ȍ~�͏������ɂ���
 *		���ϐ���
 *				�E�ϐ�����
 *						const�ɂ� c_ ��t����
 *						static�ɂ� s_ ��t����
 *						�|�C���^�ɂ� p_ ��t����
 *						�S�č��킳��� csp_ �ƂȂ�
 *				�E�O���[�o���ϐ�
 *						�P�����ڂ͑啶��
 *				�E�֐����ϐ�
 *						�������Ɓh�Q�h�Ɛ������g�p���� �֐��̈���������Ɠ���
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�}�b�v�f�[�^�Ǘ��V�X�e�����[�N
//=====================================
struct _WF2DMAP_MAPSYS {
	u16 xgrid;
	u16 ygrid;
	WF2DMAP_MAP* p_buff;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�f�[�^�Ǘ��V�X�e��������
 *
 *	@param	xgrid		���O���b�h��
 *	@param	ygrid		���O���b�h��
 *	@param	heapID		�g�p�q�[�v
 *
 *	@return	�쐬�����}�b�v�V�X�e��
 */
//-----------------------------------------------------------------------------
WF2DMAP_MAPSYS* WF2DMAP_MAPSysInit( u16 xgrid, u16 ygrid, u32 heapID )
{
	WF2DMAP_MAPSYS* p_sys;

	// �V�X�e���쐬
	p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(WF2DMAP_MAPSYS) );
	GF_ASSERT( p_sys );

	// �V�X�e���f�[�^�쐬
	p_sys->xgrid = xgrid;
	p_sys->ygrid = ygrid;

	p_sys->p_buff = GFL_HEAP_AllocClearMemory( heapID, sizeof(WF2DMAP_MAP)*(p_sys->xgrid*p_sys->ygrid) );
	GF_ASSERT( p_sys->p_buff );
	
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�f�[�^�Ǘ��V�X�e���j��
 *	
 *	@param	p_sys	�}�b�v�Ǘ��V�X�e��
 */
//-----------------------------------------------------------------------------
void WF2DMAP_MAPSysExit( WF2DMAP_MAPSYS* p_sys )
{
	GF_ASSERT( p_sys );
	GFL_HEAP_FreeMemory( p_sys->p_buff );
	GFL_HEAP_FreeMemory( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	X�O���b�h�T�C�Y�擾
 *
 *	@param	cp_sys	�}�b�v�Ǘ��V�X�e��
 *	
 *	@return	���O���b�h�T�C�Y
 */
//-----------------------------------------------------------------------------
u16 WF2DMAP_MAPSysGridXGet( const WF2DMAP_MAPSYS* cp_sys )
{
	return cp_sys->xgrid;
}

//----------------------------------------------------------------------------
/**
 *	@brief	Y�O���b�h�T�C�Y�擾
 *
 *	@param	cp_sys	�}�b�v�Ǘ��V�X�e��
 *	
 *	@return	Y�O���b�h�T�C�Y
 */
//-----------------------------------------------------------------------------
u16 WF2DMAP_MAPSysGridYGet( const WF2DMAP_MAPSYS* cp_sys )
{
	return cp_sys->ygrid;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�f�[�^�ݒ�
 *
 *	@param	p_sys		�}�b�v�Ǘ��V�X�e��
 *	@param	cp_buff		�ݒ�}�b�v�f�[�^�o�b�t�@
 */
//-----------------------------------------------------------------------------
void WF2DMAP_MAPSysDataSet( WF2DMAP_MAPSYS* p_sys, const WF2DMAP_MAP* cp_buff )
{
	GF_ASSERT( p_sys );
	GFL_STD_MemCopy(  cp_buff,p_sys->p_buff, sizeof(WF2DMAP_MAP)*(p_sys->xgrid*p_sys->ygrid) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�f�[�^�擾
 *
 *	@param	cp_sys		�}�b�v�Ǘ��V�X�e��
 *	@param	xgrid		���O���b�h
 *	@param	ygrid		���O���b�h
 *
 *	@retval	�}�b�v�f�[�^
 *	@retval	WF2DMAP_MAPDATA_NONE	�}�b�v�͈͊O
 */
//-----------------------------------------------------------------------------
WF2DMAP_MAP WF2DMAP_MAPSysDataGet( const WF2DMAP_MAPSYS* cp_sys, u16 xgrid, u16 ygrid )
{
	WF2DMAP_MAP retmap = {WF2DMAP_MAPDATA_NONE};
	
	GF_ASSERT( cp_sys );
	if( (cp_sys->xgrid > xgrid) && (cp_sys->ygrid > ygrid) ){
		retmap = cp_sys->p_buff[ (cp_sys->xgrid*ygrid)+xgrid ];
	}
	return retmap;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�����蔻��f�[�^�擾
 *
 *	@param	cp_sys	�}�b�v�Ǘ��V�X�e��
 *	@param	xgrid	���O���b�h
 *	@param	ygrid	���O���b�h
 *
 *	@retval	TRUE	�����蔻�肠��
 *	@retval	FALSE	�����蔻��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL WF2DMAP_MAPSysHitGet( const WF2DMAP_MAPSYS* cp_sys, u16 xgrid, u16 ygrid )
{
	WF2DMAP_MAP mapdata;

	GF_ASSERT( cp_sys );
	
	mapdata = WF2DMAP_MAPSysDataGet( cp_sys, xgrid, ygrid );
	if( mapdata.data == WF2DMAP_MAPDATA_NONE ){
		return TRUE;	// �͈͊O�͓����蔻�肠��ɂ���
	}
	return mapdata.hit;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�p�����[�^�f�[�^�擾
 *
 *	@param	cp_sys	�}�b�v�Ǘ��V�X�e��
 *	@param	xgrid	���O���b�h
 *	@param	ygrid	���O���b�h
 *
 *	@return	�p�����[�^�f�[�^
 */
//-----------------------------------------------------------------------------
u32 WF2DMAP_MAPSysParamGet( const WF2DMAP_MAPSYS* cp_sys, u16 xgrid, u16 ygrid )
{
	WF2DMAP_MAP mapdata;

	GF_ASSERT( cp_sys );
	
	mapdata = WF2DMAP_MAPSysDataGet( cp_sys, xgrid, ygrid );
	if( mapdata.data == WF2DMAP_MAPDATA_NONE ){
		return 0;
	}
	return mapdata.param;
}

