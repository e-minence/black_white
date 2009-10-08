//=============================================================================
/**
 *
 *	@file		ui_easy_clwk.c
 *	@brief  CLWK���\�[�X�́AGFL_CLWK�̊ȈՐ������J�� ���[�e�B���e�B�[�ł��B
 *	@author	hosaka genya
 *	@data		2009.10.08
 *
 *	@note   NCL�ENCG�ENCE ����Έ�Ή��̂��̂ɂ����g���܂���B
 *	@note   nsbtx_to_clwk �̃_�~�[�f�[�^��ǂݍ��ލۂȂǂɂ����p���������B
 *
 */
//=============================================================================
#include <gflib.h>

#include "ui/ui_easy_clwk.h"


//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  OBJ���\�[�X�ǂݍ���
 *
 *	@param	UI_EASY_CLWK_RES* res   ���\�[�X�Z�b�g
 *	@param	prm �������p�����[�^
 *	@param	GFL_CLUNIT* unit ���j�b�g
 *	@param	heapID  �q�[�v�h�c
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void UI_EASY_CLWK_LoadResource( UI_EASY_CLWK_RES* res, UI_EAYSY_CLWK_RES_PARAM* prm, GFL_CLUNIT* unit, HEAPID heapID )
{
  ARCHANDLE *handle;

  BOOL comp_ncg = ( prm->comp_flg & UI_EAYSY_CLWK_RES_COMP_NCGR );
  
  handle	= GFL_ARC_OpenDataHandle( prm->arc_id, heapID );

  if( prm->comp_flg & UI_EAYSY_CLWK_RES_COMP_NCLR )
  {
    res->res_ncl	= GFL_CLGRP_PLTT_RegisterComp( handle,
        prm->pltt_id, prm->draw_type, prm->pltt_line*0x20, heapID );
  }
  else
  {
    res->res_ncl	= GFL_CLGRP_PLTT_Register( handle,
        prm->pltt_id, prm->draw_type, prm->pltt_line*0x20, heapID );
  }

  res->res_ncg	= GFL_CLGRP_CGR_Register( handle,
      prm->ncg_id, comp_ncg, prm->draw_type, heapID );

  res->res_nce	= GFL_CLGRP_CELLANIM_Register( handle,
      prm->cell_id,	prm->anm_id, heapID );

  // �`����ێ�
  res->draw_type = prm->draw_type;

  GFL_ARC_CloseDataHandle( handle );	

}

//-----------------------------------------------------------------------------
/**
 *	@brief  OBJ ���\�[�X�j��
 *
 *	@param	UI_EASY_CLWK_RES* res ���\�[�X�Z�b�g
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void UI_EASY_CLWK_UnLoadResource( UI_EASY_CLWK_RES* res )
{
	GFL_CLGRP_PLTT_Release( res->res_ncl );
	GFL_CLGRP_CGR_Release( res->res_ncg );
	GFL_CLGRP_CELLANIM_Release( res->res_nce );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  OBJ CLWK����
 *
 *	@param	UI_EASY_CLWK_RES* res �����ς݂̃��\�[�X�f�[�^
 *	@param	unit    GFL_CLUNIT*
 *	@param	px      ����X���W
 *	@param	py      ����Y���W
 *	@param	anim    �����A�j���V�[�P���X
 *	@param	heapID  �q�[�v�h�c
 *
 *	@retval clwk    �A�N�^�[�|�C���^
 */
//-----------------------------------------------------------------------------
GFL_CLWK* UI_EASY_CLWK_CreateCLWK( UI_EASY_CLWK_RES* res, GFL_CLUNIT* unit, u8 px, u8 py, u8 anim, HEAPID heapID )
{
  GFL_CLWK_DATA	cldata = {0};
  GFL_CLWK* clwk;

  GF_ASSERT( res );
  GF_ASSERT( res );

  cldata.pos_x	= px;
  cldata.pos_y	= py;

  clwk = GFL_CLACT_WK_Create( unit,
      res->res_ncg,
      res->res_ncl,
      res->res_nce,
      &cldata,
      res->draw_type,
      heapID );

  GFL_CLACT_WK_SetAnmSeq( clwk, anim );

  return clwk;
}


