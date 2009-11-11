//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_res.c
 *	@brief	�o�g�����R�[�_�[���\�[�X�Ǘ�
 *	@author	Toru=Nagihashi
 *	@data		2009.11.11
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID


//�A�[�J�C�u
#include "arc_def.h"
#include "battle_recorder_gra.naix"

//�O���Q��
#include "br_res.h"
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
///	���\�[�X���[�N
//=====================================
struct _BR_RES_WORK
{	
	BR_RES_OBJ_DATA	obj[BR_RES_OBJ_MAX];
};

//=============================================================================
/**
 *					���\�[�X�Ǘ�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�Ǘ�������
 *
 *	@param	HEAPID heapID		�q�[�vID
 *
 *	@return	���\�[�X���[�N
 */
//-----------------------------------------------------------------------------
BR_RES_WORK *BR_RES_Init( HEAPID heapID )
{	
	BR_RES_WORK *p_wk;

	p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(BR_RES_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(BR_RES_WORK) );

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�Ǘ��j��
 *
 *	@param	BR_RES_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_RES_Exit( BR_RES_WORK *p_wk )
{	
	GFL_HEAP_FreeMemory( p_wk );
}

//=============================================================================
/**
 *					���\�[�X�ǂݍ���
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BG�ǂݍ���	
 *
 *	@param	BR_RES_WORK *p_wk	���[�N
 *	@param	procID						�v���Z�X��ID
 */
//-----------------------------------------------------------------------------
void BR_RES_LoadBG( BR_RES_WORK *p_wk, BR_PROCID procID, HEAPID heapID )
{	
	ARCHANDLE * p_handle  = GFL_ARC_OpenDataHandle( ARCID_BATTLE_RECORDER_GRA, heapID );

	switch( procID )
	{	
	case BR_PROCID_START:
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_battle_recorder_gra_batt_rec_browse_bg5_NCLR,
        PALTYPE_MAIN_BG, PLT_BG_M_BACK*0x20, 0, heapID );

    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_browse_bg_NCGR,
        BG_FRAME_M_BACK, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_browse_bg0u_NSCR,
				BG_FRAME_M_BACK, 0, 0, FALSE, heapID );

		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_battle_recorder_gra_batt_rec_browse_bg5_NCLR,
        PALTYPE_SUB_BG, PLT_BG_S_BACK*0x20, 0, heapID );

    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_browse_bg_NCGR,
        BG_FRAME_S_BACK, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_browse_bg0d_NSCR,
				BG_FRAME_S_BACK, 0, 0, FALSE, heapID );
		break;
	}

	GFL_ARC_CloseDataHandle( p_handle );
}
//----------------------------------------------------------------------------
/**
 *	@brief	BG�j��
 *
 *	@param	BR_RES_WORK *p_wk	���[�N
 *	@param	procID						�v���Z�X��ID
 */
//-----------------------------------------------------------------------------
void BR_RES_UnLoadBG( BR_RES_WORK *p_wk, BR_PROCID procID )
{	
	switch( procID )
	{	
	case BR_PROCID_START:

		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�ǂݍ���
 *
 *	@param	BR_RES_WORK *p_wk	���[�N
 *	@param	objID							OBJID
 *	@param	heapID						�q�[�vID
 *	@param	*p_res						���\�[�X�󂯎��
 */
//-----------------------------------------------------------------------------
void BR_RES_LoadOBJ( BR_RES_WORK *p_wk, BR_RES_OBJID objID, HEAPID heapID, BR_RES_OBJ_DATA *p_res )
{	

}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�j��
 *
 *	@param	BR_RES_WORK *p_wk		���[�N
 *	@param	objID								OBJID
 */
//-----------------------------------------------------------------------------
void BR_RES_UnLoadOBJ( BR_RES_WORK *p_wk, BR_RES_OBJID objID )
{	

}

