//=============================================================================
/**
 *
 *	@file		ui_easy_clwk.h
 *	@brief  CLWK���\�[�X�́AGFL_CLWK�̊ȈՐ������J�� ���[�e�B���e�B�[�ł��B
 *	@author	hosaka genya
 *	@data		2009.10.08
 *
 *	@note   NCL�ENCG�ENCE ����Έ�Ή��̂��̂ɂ����g���܂���B
 *	@note   nsbtx_to_clwk �̃_�~�[�f�[�^��ǂݍ��ލۂȂǂɂ����p���������B
 *
 */
//=============================================================================
#pragma once

#include <gflib.h>

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

//--------------------------------------------------------------
///	OBJ���\�[�X�Ǘ����[�N
//==============================================================
typedef struct {
  u32               res_ncg;
  u32               res_ncl;
  u32               res_nce;
  CLSYS_DRAW_TYPE   draw_type;
} UI_EASY_CLWK_RES;

//--------------------------------------------------------------
///	���k�t���O�r�b�g
//==============================================================
typedef enum{
  UI_EAYSY_CLWK_RES_COMP_NONE = 0,     ///< ���k�Ȃ�
  UI_EAYSY_CLWK_RES_COMP_NCLR = 0x1,   ///< �p���b�g���k
  UI_EAYSY_CLWK_RES_COMP_NCGR = 0x2,   ///< �L�����N�^���k
} UI_EAYSY_CLWK_RES_COMP;

//--------------------------------------------------------------
///	CLWK�������p�����[�^
//==============================================================
typedef struct {
  CLSYS_DRAW_TYPE draw_type; ///< �`���^�C�v
  UI_EAYSY_CLWK_RES_COMP   comp_flg;  ///< ���k�t���O
  u32 arc_id;   ///< �A�[�J�C�uNO
  u32 pltt_id;  ///< �p���b�g���\�[�XNO
  u32 ncg_id;   ///< �L�����N�^���\�[�XNO
  u32 cell_id;  ///< �Z�����\�[�XNO
  u32 anm_id;   ///< �Z���A�j�����\�[�XNO
  u8 pltt_line; ///< �p���b�g�]����NO
  u8 padding[3];
} UI_EAYSY_CLWK_RES_PARAM;

//=============================================================================
/**
 *								EXTERN
 */
//=============================================================================
extern void UI_EASY_CLWK_LoadResource( UI_EASY_CLWK_RES* res, UI_EAYSY_CLWK_RES_PARAM* prm, GFL_CLUNIT* unit, HEAPID heapID );
extern void UI_EASY_CLWK_UnLoadResource( UI_EASY_CLWK_RES* res );
extern GFL_CLWK* UI_EASY_CLWK_CreateCLWK( UI_EASY_CLWK_RES* res, GFL_CLUNIT* unit, u8 px, u8 py, u8 anim, HEAPID heapID );

