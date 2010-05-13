//======================================================================
/**
 * @file	fieldobj_draw.c
 * @brief	�t�B�[���h���샂�f�� �`��n
 * @author	kagaya
 * @date	05.07.25
 *
 */
//======================================================================
#include "fldmmdl.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	struct
//======================================================================

//======================================================================
//	proto
//======================================================================

//======================================================================
//	�t�B�[���h���샂�f���@�`��V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * FIELD OBJ �`��n������
 * @param	fos			MMDLSYS *
 * @param	tex_max		�e�N�X�`����o�^�ł���ő�l��
 * @param	reg_tex_max	tex_max���A��ɏ풓����ő�l��
 * @param	tex_tbl		��ɏ풓����l�����܂Ƃ߂��e�[�u�� BABYBOY1��
 * @param	frm_trans_max	1�t���[���œ]���ł���f�[�^�ő吔 add pl
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_InitDraw( MMDLSYS *fos )
{
	MMDLSYS_SetCompleteDrawInit( fos, TRUE );
}

//--------------------------------------------------------------
/**
 * FIELD OBJ �`�揈�����폜
 * @param	fos		MMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_DeleteDraw( MMDLSYS *fos )
{
  if( MMDLSYS_GetBlActCont(fos) != NULL ){
		MMDL_BLACTCONT_Release( fos );
	}

  if( MMDLSYS_GetG3dObjCont(fos) != NULL ){
    MMDL_G3DOBJCONT_Delete( fos );
  }
  
	MMDLSYS_SetCompleteDrawInit( fos, FALSE );
}

//======================================================================
//	�t�B�[���h���샂�f�� �`�揈��
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���`��
 * @param	fmmdl		MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_UpdateDraw( MMDL * fmmdl )
{
	const MMDLSYS *fos = MMDL_GetMMdlSys( fmmdl );
	
	if( MMDLSYS_CheckStatusBit(fos,MMDLSYS_STABIT_DRAW_PROC_STOP) ){
		return; //�V�X�e���ŕ`���~���������Ă���
	}
	
	if( MMDL_CheckMoveBitCompletedDrawInit(fmmdl) == FALSE ){
		return; //���������������Ă��Ȃ�
	}
  
  #if 0 //old dp	
  /*
   * �����A����|�[�Y���͕`�揈�����|�[�Y���Ă�����
   * �|�[�Y���Ăяo���C�x���g���Ƃ��Ă͐F�X�ƕs�s��������
   * �g�����肪���������ׂ����p�~�B
   * �`�揈���͏�ɌĂяo���A�|�[�Y�͊e�`�揈�����őΉ�����B
   */
	if( MMDL_CheckMoveBitMoveProcPause(fmmdl) == FALSE ||
		MMDL_CheckStatusBitAcmd(fmmdl) ){
		MMDL_CallDrawProc( fmmdl );
	}
  #else //new wb
	MMDL_CallDrawProc( fmmdl );
  #endif
}

//======================================================================
//	parts
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�� �`��|�[�Y�`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	int		TRUE=�`��|�[�Y
 */
//--------------------------------------------------------------
BOOL MMDL_CheckDrawPause( const MMDL * fmmdl )
{
  if( MMDL_CheckMoveBitMoveProcPause(fmmdl) ){
		if( MMDL_CheckMoveBitAcmd(fmmdl) == FALSE ){
			return( TRUE );
		}
	}
	
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_PAUSE_ANM) ){
		return( TRUE );
	}

	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���@��{���W�{�I�t�Z�b�g���擾
 * @param	fmmdl	MMDL *
 * @param	vec		���W�i�[��
 * @retval	void*	�擾�����f�[�^
 */
//--------------------------------------------------------------
void MMDL_GetDrawVectorPos( const MMDL * fmmdl, VecFx32 *vec )
{
	VecFx32 vec_pos,vec_offs,vec_out,vec_attr,vec_ctrl;
	
	MMDL_GetVectorPos( fmmdl, &vec_pos );
	MMDL_GetVectorDrawOffsetPos( fmmdl, &vec_offs );
	MMDL_GetVectorOuterDrawOffsetPos( fmmdl, &vec_out );
	MMDL_GetVectorAttrDrawOffsetPos( fmmdl, &vec_attr );
  MMDL_GetControlOffsetPos( fmmdl, &vec_ctrl );
	vec->x = vec_pos.x + vec_offs.x + vec_out.x + vec_attr.x + vec_ctrl.x;
	vec->y = vec_pos.y + vec_offs.y + vec_out.y + vec_attr.y + vec_ctrl.y;
	vec->z = vec_pos.z + vec_offs.z + vec_out.z + vec_attr.z + vec_ctrl.z;
}
