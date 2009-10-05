//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		pokegra.c
 *	@brief	�|�P�����O���t�B�b�N�f�ނ̎擾
 *	@author	Toru=Nagihashi
 *	@data		2009.10.05
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�A�[�J�C�u
#include "arc_def.h"
#include "pokegra/pokegra_wb.naix"

//�|�P�c�[��
#include "poke_tool/poke_tool.h"

//�O���Q��
#include "system/pokegra.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//�|�P������̂��\������MCSS�p�t�@�C���̍\��
enum{
	POKEGRA_FRONT_M_NCGR = 0,
	POKEGRA_FRONT_F_NCGR,
	POKEGRA_FRONT_M_NCBR,
	POKEGRA_FRONT_F_NCBR,
	POKEGRA_FRONT_NCER,
	POKEGRA_FRONT_NANR,
	POKEGRA_FRONT_NMCR,
	POKEGRA_FRONT_NMAR,
	POKEGRA_FRONT_NCEC,
	POKEGRA_BACK_M_NCGR,
	POKEGRA_BACK_F_NCGR,
	POKEGRA_BACK_M_NCBR,
	POKEGRA_BACK_F_NCBR,
	POKEGRA_BACK_NCER,
	POKEGRA_BACK_NANR,
	POKEGRA_BACK_NMCR,
	POKEGRA_BACK_NMAR,
	POKEGRA_BACK_NCEC,
	POKEGRA_NORMAL_NCLR,
	POKEGRA_RARE_NCLR,

	POKEGRA_FILE_MAX,			//�|�P������̂��\������MCSS�p�t�@�C���̑���

	POKEGRA_M_NCGR = 0,
	POKEGRA_F_NCGR,
	POKEGRA_M_NCBR,
	POKEGRA_F_NCBR,
	POKEGRA_NCER,
	POKEGRA_NANR,
	POKEGRA_NMCR,
	POKEGRA_NMAR,
	POKEGRA_NCEC
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
static void PokeGra_GetFileOffset( int mons_no, int form_no, int sex, int rare, int dir, u32 *p_mons_offset, u32 *p_dir_offset, u32 *p_sex_offset, u32 *p_rare_offset );

//=============================================================================
/**
 *		���\�[�X�擾�֐�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	POKEGRA�̃A�[�J�C�uID
 *		
 *	@return	�A�[�J�C�uID
 */
//-----------------------------------------------------------------------------
ARCID POKEGRA_GetArcID( void )
{	
	return ARCID_POKEGRA;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 * @param	mons_no �����X�^�[�i���o�[
 * @param	form_no �t�H�����i���o�[
 * @param	sex		����
 * @param	rare	���A���ǂ���
 * @param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *
 *	@return	�L�����A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID POKEGRA_GetCgrArcIndex( int mons_no, int form_no, int sex, int rare, int dir )
{	
	u32 mons_offset;
	u32 dir_offset;
	u32 sex_offset;

	PokeGra_GetFileOffset( mons_no, form_no, sex, rare, dir, &mons_offset, &dir_offset, &sex_offset, NULL );
	return mons_offset + dir_offset + POKEGRA_M_NCGR + sex_offset;
}
//----------------------------------------------------------------------------
/**
 *	@brief	
 *
 * @param	mons_no �����X�^�[�i���o�[
 * @param	form_no �t�H�����i���o�[
 * @param	sex		����
 * @param	rare	���A���ǂ���
 * @param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *
 *	@return	�L�����r�b�g�}�b�v�A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID POKEGRA_GetCbrArcIndex( int mons_no, int form_no, int sex, int rare, int dir )
{	
	u32 mons_offset;
	u32 dir_offset;
	u32 sex_offset;

	PokeGra_GetFileOffset( mons_no, form_no, sex, rare, dir, &mons_offset, &dir_offset, &sex_offset, NULL );
	return mons_offset + dir_offset + POKEGRA_M_NCBR + sex_offset;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 * @param	mons_no �����X�^�[�i���o�[
 * @param	form_no �t�H�����i���o�[
 * @param	sex		����
 * @param	rare	���A���ǂ���
 * @param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *
 *	@return	��گăA�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID POKEGRA_GetPalArcIndex( int mons_no, int form_no, int sex, int rare, int dir )
{	
	u32 mons_offset;
	u32 rare_offset;

	PokeGra_GetFileOffset( mons_no, form_no, sex, rare, dir, &mons_offset, NULL, NULL, &rare_offset );

	return mons_offset + POKEGRA_NORMAL_NCLR + rare_offset;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 * @param	mons_no �����X�^�[�i���o�[
 * @param	form_no �t�H�����i���o�[
 * @param	sex		����
 * @param	rare	���A���ǂ���
 * @param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *
 *	@return	�Z���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID POKEGRA_GetCelArcIndex( int mons_no, int form_no, int sex, int rare, int dir )
{	
	u32 mons_offset;
	u32 dir_offset;

	PokeGra_GetFileOffset( mons_no, form_no, sex, rare, dir, &mons_offset, &dir_offset, NULL, NULL );

	return mons_offset + dir_offset + POKEGRA_NCER;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 * @param	mons_no �����X�^�[�i���o�[
 * @param	form_no �t�H�����i���o�[
 * @param	sex		����
 * @param	rare	���A���ǂ���
 * @param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *
 *	@return	�Z���A�j���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID POKEGRA_GetAnmArcIndex( int mons_no, int form_no, int sex, int rare, int dir )
{	
	u32 mons_offset;
	u32 dir_offset;

	PokeGra_GetFileOffset( mons_no, form_no, sex, rare, dir, &mons_offset, &dir_offset, NULL, NULL );

	return mons_offset + dir_offset + POKEGRA_NANR;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 * @param	mons_no �����X�^�[�i���o�[
 * @param	form_no �t�H�����i���o�[
 * @param	sex		����
 * @param	rare	���A���ǂ���
 * @param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *
 *	@return	�}���`�Z���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID POKEGRA_GetMCelArcIndex( int mons_no, int form_no, int sex, int rare, int dir )
{	
	u32 mons_offset;
	u32 dir_offset;

	PokeGra_GetFileOffset( mons_no, form_no, sex, rare, dir, &mons_offset, &dir_offset, NULL, NULL );

	return mons_offset + dir_offset + POKEGRA_NMCR;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 * @param	mons_no �����X�^�[�i���o�[
 * @param	form_no �t�H�����i���o�[
 * @param	sex		����
 * @param	rare	���A���ǂ���
 * @param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *
 *	@return	�}���`�Z���A�j���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID POKEGRA_GetMAnmArcIndex( int mons_no, int form_no, int sex, int rare, int dir )
{	
	u32 mons_offset;
	u32 dir_offset;

	PokeGra_GetFileOffset( mons_no, form_no, sex, rare, dir, &mons_offset, &dir_offset, NULL, NULL );

	return mons_offset + dir_offset + POKEGRA_NMAR;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 * @param	mons_no �����X�^�[�i���o�[
 * @param	form_no �t�H�����i���o�[
 * @param	sex		����
 * @param	rare	���A���ǂ���
 * @param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *
 *	@return	�A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID POKEGRA_GetNcecArcIndex( int mons_no, int form_no, int sex, int rare, int dir )
{	
	u32 mons_offset;
	u32 dir_offset;

	PokeGra_GetFileOffset( mons_no, form_no, sex, rare, dir, &mons_offset, &dir_offset, NULL, NULL );

	return mons_offset + dir_offset + POKEGRA_NCEC;
}

//=============================================================================
/**
 *	private
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	 mons_no	�����X�^�[�i���o�[
 *	@param	form_no		�t�H�����i���o�[
 *	@param	sex				����
 *	@param	rare			���A���ǂ���
 *	@param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *	@param	*p_mons_offset				�|�P�����ԍ��t�@�C���I�t�Z�b�g
 *	@param	*p_dir_offset			�w�ʃI�t�Z�b�g
 *	@param	*p_sex_offset				���ʃI�t�Z�b�g
 *	@param	u32 *p_rare_offset	���A�I�t�Z�b�g
 */
//-----------------------------------------------------------------------------
static void PokeGra_GetFileOffset( int mons_no, int form_no, int sex, int rare, int dir, u32 *p_mons_offset, u32 *p_dir_offset, u32 *p_sex_offset, u32 *p_rare_offset )
{	
	u32 file_start;
	u32 file_offset;

	//�����X�^�[�i���o�[�͂P�I���W��
	//���\�[�X�͂O�I���W���̂��߁A�|�P
	mons_no	-= 1;

	file_start	= POKEGRA_FILE_MAX * mons_no;
	file_offset	= (dir == POKEGRA_DIR_FRONT) ? 0: POKEGRA_BACK_M_NCGR;

	//�{���͕ʃt�H��������������@todo
#if defined(DEBUG_ONLY_FOR_sogabe) || defined(DEBUG_ONLY_FOR_toru_nagihashi)
#warning Another Form Nothing
#endif

	//���ʂ̃`�F�b�N
	switch( sex ){
	case PTL_SEX_MALE:
		break;
	case PTL_SEX_FEMALE:
		//�I�X���X�����������Ă��邩�`�F�b�N����i�T�C�Y���O�Ȃ珑�������Ȃ��j
		sex = ( GFL_ARC_GetDataSize( ARCID_POKEGRA, file_start + file_offset + 1 ) == 0 ) ? PTL_SEX_MALE : PTL_SEX_FEMALE;
		break;
	case PTL_SEX_UNKNOWN:
		//���ʂȂ��́A�I�X�����ɂ���
		sex = PTL_SEX_MALE;
		break;
	default:
		//���肦�Ȃ�����
		GF_ASSERT(0);
		break;
	}


	//�󂯎��
	if( p_mons_offset )
	{	
		*p_mons_offset	= file_start;
	}
	if( p_dir_offset )
	{	
		*p_dir_offset		= file_offset;
	}
	if( p_sex_offset )
	{	
		*p_sex_offset		= sex;
	}
	if( p_rare_offset )
	{	
		*p_rare_offset	= rare;
	}
}
