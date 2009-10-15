//============================================================================
/**
 *
 *	@file		nsbtx_to_clwk.h
 *	@brief  �e�N�X�`����CLWK�ɕϊ�
 *	@author	hosaka genya
 *	@data		2009.10.31
 *	@note   �t�B�[���h�}�b�v���f���̃��\�[�X�� resource\fldmmdl �ɂ���܂��B
 *
 */
//============================================================================
#pragma once



enum
{ 
  // �l���̃t�B�[���hOBJ�͊�{ 4 x 4
  NSBTX_DEF_SX = 4,
  NSBTX_DEF_SY = 4,
};

//-----------------------------------------------------------------------------
/**
 *	@brief  nsbtx���j�g���L�����N�^�A�j�g���p���b�g�ɕϊ�����CLWK�ɓ]��
 *
 *	@param	GFL_CLWK* act �A�N�^�[�ւ̃|�C���^
 *	@param  arc_idx   �A�[�J�C�u�h�c
 *	@param	tex_idx   �e�N�X�`����ARC���C���f�b�N�X
 *	@param  ptn_ofs   �e�N�X�`���I�t�Z�b�g�i�A�j���p�^�[���̃I�t�Z�b�g�j
 *	@param	sx        �]�����f�[�^�̃T�C�Y�w�i�L�����P�ʁj
 *	@param	sy        �]�����f�[�^�̃T�C�Y�x�i�L�����P�ʁj
 *	@param	VramOfs   �]����I�t�Z�b�g�iByte) (�o�^�ς�CGR���]������Ă���A�h���X����̃I�t�Z�b�g�j
 *  @param  vramType  �]����VramType  
 *	@param	HeapID    �q�[�v�h�c
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
extern void CLWK_TransNSBTX( GFL_CLWK* act, u32 arc_idx, u32 tex_idx, u8 ptn_ofs, u16 sx, u16 sy, u32 VramOfs, CLSYS_DRAW_TYPE vram_type, HEAPID HeapID );

