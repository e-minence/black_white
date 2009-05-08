//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		texanm_sys.c
 *	@brief		ITP�A�j���f�[�^�̕K�v�ȂƂ���݂̂𔲂��o���e�N�X�`�����A�j���[�V�����f�[�^���擾����V�X�e��
 *	@author	 
 *	@data		2005.10.31
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "texanm_sys.h"

//-----------------------------------------------------------------------------
/**
 *					�R�[�f�B���O�K��
 *		���֐���
 *				�P�����ڂ͑啶������ȍ~�͏������ɂ���
 *		���ϐ���
 *				�E�ϐ�����
 *						const�ɂ�c_��t����
 *						static�ɂ�s_��t����
 *						�|�C���^�ɂ�p_��t����
 *						�S�č��킳���csp_�ƂȂ�
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

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *
 *	@brief	frame���ɑΉ�����e�N�X�`���C���f�b�N�X�@�p���b�g�C���f�b�N�X�̎擾
 *
 *	@param	cp_texanm	�e�N�X�`���A�j���[�V�����f�[�^�e�[�u��
 *	@param	c_frame		���݃t���[����
 *
 *	@return	�t���[�����ɑΉ������e�N�X�`���C���f�b�N�X�@�p���b�g�C���f�b�N�X
 *	
		typedef struct {
			u8	tex_idx;		// �e�N�X�`���C���f�b�N�X	
			u8	pltt_idx;		// �p���b�g�C���f�b�N�X
		} TEXANM_DATA;
 *
 *
 */
//-----------------------------------------------------------------------------
TEXANM_DATA TEXANM_GetFrameData( const TEXANM_DATATBL* cp_texanm, u32 anime_index, u32 frame )
{
	int i;					// ���[�v�p
	u32 anime_data_head;
	u32 anime_data_tail;
	TEXANM_DATA	o_tex_anm;	// �߂�l

	GF_ASSERT( cp_texanm->anime_num > anime_index );

	// �A�j���[�f�[�^�̃w�b�h�C���f�b�N�X�擾
	// anime_data_head >= anime_data < anime_data_tail
	anime_data_head = cp_texanm->cp_data_head[ anime_index ];
	if( (anime_index+1) < cp_texanm->anime_num ){
		anime_data_tail	= cp_texanm->cp_data_head[ anime_index+1 ];
	}else{
		anime_data_tail	= cp_texanm->tbl_num;
	}

	// �t���[�����ɑΉ�����e�[�u���v�f���i i �j���擾
	for( i = anime_data_head; i < anime_data_tail - 1; i++ ){

		// i + 1�̒l���v�f�����ς��t���[�����ł��B
		if( cp_texanm->cp_frame[ i + 1 ] > frame ){

			break;
		}
	}
	
	// �t���[�����ɑΉ�����e�N�X�`���C���f�b�N�X
	// �p���b�g�C���f�b�N�X���
	o_tex_anm.tex_idx = cp_texanm->cp_tex_idx[ i ];
	o_tex_anm.pltt_idx = cp_texanm->cp_pltt_idx[ i ];

	return o_tex_anm;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�������̎擾
 *
 *	@param	cp_texanm		�A�j���[�V�����f�[�^
 *
 *	@return	�A�j���[�V������
 */	
//-----------------------------------------------------------------------------
u32 TEXANM_GetAnimeNum( const TEXANM_DATATBL* cp_texanm )
{
	return cp_texanm->anime_num;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����̃t���[���ő吔���擾
 *
 *	@param	cp_texanm			�A�j���[�V�������[�N
 *	@param	anime_index		�C���f�b�N�X
 *
 *	@return	�t���[���ő�l
 */
//-----------------------------------------------------------------------------
u32 TEXANM_GetFrameMax( const TEXANM_DATATBL* cp_texanm )
{
	return cp_texanm->frame_max;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����̍ŏIKey�ݒ�t���[�����擾
 *
 *	@param	cp_texanm				�A�j���[�V�������[�N
 *	@param	anime_index			�A�j���[�V�����C���f�b�N�X
 *
 *	@return	�ŏIKey�ݒ�t���[��
 */
//-----------------------------------------------------------------------------
u32 TEXANM_GetLastKeyFrame( const TEXANM_DATATBL* cp_texanm, u32 anime_index )
{
	u32 anime_data_tail;
	if( (anime_index+1) < cp_texanm->anime_num ){
		anime_data_tail	= cp_texanm->cp_data_head[ anime_index+1 ];
	}else{
		anime_data_tail	= cp_texanm->tbl_num;
	}

	return cp_texanm->cp_frame[ anime_data_tail - 1 ];
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief	ROM����ǂݍ���ł����f�[�^���e�N�X�`���A�j���[�V�����f�[�^�e�[�u���`���ɃA���p�b�N����
 *
 *	@param	cp_buff			�ǂݍ���ł����f�[�^
 *	@param	op_texanm		�A���p�b�N��e�N�X�`���A�j���[�V�����f�[�^�e�[�u��
 *
 *	@return	none
 *
 * ��op_texanm���g�p���Ă�����Ԃł�cp_buff�̃������͊m�ۂ����܂܂ɂ��Ă����Ă�������
 *
 */
//-----------------------------------------------------------------------------
void TEXANM_UnPackLoadFile( const void* cp_buff, TEXANM_DATATBL* op_texanm )
{	
	const u8* work;

	work = cp_buff;
	
	// �e�[�u���v�f���擾
	// �ŏ���4byte�̓e�[�u���v�f��
	op_texanm->tbl_num = *((u32*)work);
	work = work + sizeof(u32);		// 4byte�ړ�
	
	// �t���[���f�[�^�e�[�u���擪�|�C���^�ݒ�
	op_texanm->cp_frame = (u16*)work;
	work = work + (sizeof(u16)*op_texanm->tbl_num);			// �e�[�u�����ړ�
	if( ((sizeof(u16)*op_texanm->tbl_num)%4) ){
		work = work + (4-((sizeof(u16)*op_texanm->tbl_num)%4));	// 4byte�A���C�����g
	}

	// �e�N�X�`���C���f�b�N�X�f�[�^�e�[�u���擪�|�C���^�ݒ�
	op_texanm->cp_tex_idx = (u8*)work;
	work = work + (sizeof(u8) * op_texanm->tbl_num);	// �e�[�u�����ړ�
	if( ((sizeof(u8)*op_texanm->tbl_num)%4) ){
		work = work + (4-((sizeof(u8)*op_texanm->tbl_num)%4));	// 4byte�A���C�����g
	}

	// �p���b�g�C���f�b�N�X�f�[�^�e�[�u���擪�|�C���^�ݒ�
	op_texanm->cp_pltt_idx = (u8*)work;
	work = work + (sizeof(u8) * op_texanm->tbl_num);	// �e�[�u�����ړ�
	if( ((sizeof(u8)*op_texanm->tbl_num)%4) ){
		work = work + (4-((sizeof(u8)*op_texanm->tbl_num)%4));	// 4byte�A���C�����g
	}

	// �A�j���[�V������
	op_texanm->anime_num = *((u32*)work);
	work = work + sizeof(u32);		// 4byte�ړ�

	// �A�j���[�V�����w�b�h
	op_texanm->cp_data_head = (u8*)work;
	work = work + (sizeof(u8) * op_texanm->anime_num);	// �e�[�u�����ړ�
	if( ((sizeof(u8)*op_texanm->anime_num)%4) ){
			work = work + (4-((sizeof(u8)*op_texanm->anime_num)%4));	// 4byte�A���C�����g
	}

	// �A�j���[�V�����t���[����
	op_texanm->frame_max = *((u32*)work);

#ifdef DEBUG_TEXANM_SYS_PRINT_LOAD_DATA
	TEXANM_DEBUG_PrintData( op_texanm );
#endif
}


#ifdef DEBUG_TEXANM_SYS_PRINT_LOAD_DATA
void TEXANM_DEBUG_PrintData( const TEXANM_DATATBL* cp_texanm )
{
	int i;
	
	OS_TPrintf( "frame_max = %d\n", cp_texanm->frame_max );
	OS_TPrintf( "anime_num = %d\n", cp_texanm->anime_num );
	OS_TPrintf( "anime_start[\n" );
	for( i=0; i<cp_texanm->anime_num; i++ ){
		OS_TPrintf( " %d,", cp_texanm->cp_data_head[i] );
	}
	OS_TPrintf( "]\n\n" );
	
	OS_TPrintf( "tbl_num = %d\n", cp_texanm->tbl_num );
	OS_TPrintf( "frame_tbl[\n" );
	for( i=0; i<cp_texanm->tbl_num; i++ ){
		OS_TPrintf( " %d,", cp_texanm->cp_frame[i] );
	}
	OS_TPrintf( "]\n" );
	OS_TPrintf( "tex_tbl[\n" );
	for( i=0; i<cp_texanm->tbl_num; i++ ){
		OS_TPrintf( " %d,", cp_texanm->cp_tex_idx[i] );
	}
	OS_TPrintf( "]\n" );
	OS_TPrintf( "pltt_tbl[\n" );
	for( i=0; i<cp_texanm->tbl_num; i++ ){
		OS_TPrintf( " %d,", cp_texanm->cp_pltt_idx[i] );
	}
	OS_TPrintf( "]\n" );
	
}
#endif

