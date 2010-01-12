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
static void MMdlSys_ArcHandleOpen( MMDLSYS *fos );
static void MMdlSys_ArcHandleClose( MMDLSYS *fos );

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
	#ifndef MMDL_PL_NULL
	int max,pri;
	MMDL_BLACT_CONT *cont;
	
	GF_ASSERT( MMDLSYS_CheckCompleteDrawInit(fos) == FALSE );
	MMdlSys_ArcHandleOpen( fos );
	
	max = MMDLSYS_GetMMdlMax( fos );
	pri = MMDLSYS_GetTCBPriority( fos ) - 1; //�ǉ������͕W������
	cont = MMDLSYS_BlActContGet( fos );
	
	MMDL_BlActCont_Init(
		cont, fos, max, pri, tex_max, reg_tex_max, tex_tbl, frm_trans_max );
	MMDLSYS_StatusBit_ON( fos, MMDLSYS_STABIT_DRAW_INIT_COMP );
	#else
	MMDLSYS_SetCompleteDrawInit( fos, TRUE );
	#endif
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
	#ifndef MMDL_PL_NULL
	GF_ASSERT( MMDLSYS_CheckCompleteDrawInit(fos) == TRUE );
	
	MMDL_BlActCont_Delete( MMDLSYS_BlActContGet(fos) );
	
	MMDLSYS_StatusBit_OFF( fos, MMDLSYS_STABIT_DRAW_INIT_COMP );
	MMdlSys_ArcHandleClose( fos );
	#else
  if( MMDLSYS_GetBlActCont(fos) != NULL ){
		MMDL_BLACTCONT_Release( fos );
	}

  if( MMDLSYS_GetG3dObjCont(fos) != NULL ){
    MMDL_G3DOBJCONT_Delete( fos );
  }
  
	MMDLSYS_SetCompleteDrawInit( fos, FALSE );
	#endif
}

//======================================================================
//	�A�[�J�C�u
//======================================================================
//--------------------------------------------------------------
/**
 * �A�[�J�C�u�n���h���Z�b�g
 * @param	fos		MMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdlSys_ArcHandleOpen( MMDLSYS *fos )
{
#ifndef MMDL_PL_NULL
	ARCHANDLE *handle = ArchiveDataHandleOpen( ARC_MMODEL, HEAPID_FIELD );
	MMDLSYS_SetArcHandle( fos, handle );
#endif
}

//--------------------------------------------------------------
/**
 * �A�[�J�C�u�n���h���N���[�Y
 * @param	fos		MMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdlSys_ArcHandleClose( MMDLSYS *fos )
{
#ifndef MMDL_PL_NULL
	ARCHANDLE *handle = MMDLSYS_GetArcHandle( fos );
	ArchiveDataHandleClose( handle );
#endif
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
//	�t�B�[���h���샂�f�� �`��X�e�[�^�X
//======================================================================
#ifndef MMDL_PL_NULL
//--------------------------------------------------------------
/**
 * OBJ�R�[�h����`��X�e�[�^�X�擾
 * @param	code	�\���R�[�h HERO��
 * @retval	OBJCODE_STATE*
 */
//--------------------------------------------------------------
const OBJCODE_STATE * MMDL_TOOL_GetOBJCodeState( u16 code )
{
	const OBJCODE_STATE *state = DATA_FieldOBJCodeDrawStateTbl;
	
	do{
		if( state->code == code ){
			return( state );
		}
		state++;
	}while( state->code != OBJCODEMAX );
	
	GF_ASSERT( 0 );
	return( NULL );
}

//--------------------------------------------------------------
/**
 * ���샂�f��OBJ�R�[�h����`��X�e�[�^�X�擾
 * @param	code	�\���R�[�h HERO��
 * @retval	OBJCODE_STATE*
 */
//--------------------------------------------------------------
const OBJCODE_STATE * MMDL_GetOBJCodeState( const MMDL *fmmdl )
{
	return( MMDL_TOOL_GetOBJCodeState(MMDL_GetOBJCode(fmmdl)) );
}
#endif

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
 * �A�[�J�C�u�f�[�^���[�h
 * @param	fos		MMDLSYS *
 * @param	datID	�A�[�J�C�u�f�[�^ID
 * @param	fb TRUE=GFL_HEAP_AllocClearMemory() FALSE=GFL_HEAP_AllocClearMemoryLo()
 * @retval	void*	�擾�����f�[�^
 */
//--------------------------------------------------------------
void * MMDL_DrawArcDataAlloc( const MMDLSYS *fos, u32 datID, int fb )
{
#ifndef MMDL_PL_NULL
	void *buf;
	ARCHANDLE *handle = MMDLSYS_GetArcHandle( fos );
	u32 size = ArchiveDataSizeGetByHandle( handle, datID );
	
	if( fb == TRUE ){
		buf = GFL_HEAP_AllocClearMemory( HEAPID_FIELD, size );
	}else{
		buf = GFL_HEAP_AllocClearMemoryLo( HEAPID_FIELD, size );
	}
	
	#ifdef DEBUG_ONLY_FOR_kagaya
	{
	 u32 free = sys_GetHeapFreeSize( HEAPID_FIELD );
	 OS_Printf( "fmmdl DrawArcData datID(%d) FIELD HEAP FreeSize %xH\n",
			 datID, free );
	}
	#endif
	
	ArchiveDataLoadByHandle( handle, datID, buf );
	return( buf );
#else
	return( NULL );
#endif
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
