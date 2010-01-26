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
#include "poke_tool/monsno_def.h"
#include "poke_tool/poke_tool.h"

//�O���Q��
#include "system/pokegra.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
static void PokeGra_GetFileOffset( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg, u32 *p_mons_offset, u32 *p_dir_offset, u32 *p_sex_offset, u32 *p_rare_offset );

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
ARCDATID POKEGRA_GetCgrArcIndex( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg )
{	
	u32 mons_offset;
	u32 dir_offset;
	u32 sex_offset;

  PokeGra_GetFileOffset( mons_no, form_no, sex, rare, dir, egg, &mons_offset, &dir_offset, &sex_offset, NULL );
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
ARCDATID POKEGRA_GetCbrArcIndex( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg )
{	
	u32 mons_offset;
	u32 dir_offset;
	u32 sex_offset;

  PokeGra_GetFileOffset( mons_no, form_no, sex, rare, dir, egg, &mons_offset, &dir_offset, &sex_offset, NULL );
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
ARCDATID POKEGRA_GetPalArcIndex( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg )
{	
  u32 mons_offset;
  u32 rare_offset;
  PokeGra_GetFileOffset( mons_no, form_no, sex, rare, dir, egg, &mons_offset, NULL, NULL, &rare_offset );
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
ARCDATID POKEGRA_GetCelArcIndex( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg )
{	
  u32 mons_offset;
  u32 dir_offset;

  PokeGra_GetFileOffset( mons_no, form_no, sex, rare, dir, egg, &mons_offset, &dir_offset, NULL, NULL );
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
ARCDATID POKEGRA_GetAnmArcIndex( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg )
{	
	u32 mons_offset;
	u32 dir_offset;

	PokeGra_GetFileOffset( mons_no, form_no, sex, rare, dir, egg, &mons_offset, &dir_offset, NULL, NULL );

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
ARCDATID POKEGRA_GetMCelArcIndex( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg )
{	
	u32 mons_offset;
	u32 dir_offset;

	PokeGra_GetFileOffset( mons_no, form_no, sex, rare, dir, egg, &mons_offset, &dir_offset, NULL, NULL );

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
ARCDATID POKEGRA_GetMAnmArcIndex( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg )
{	
	u32 mons_offset;
	u32 dir_offset;

	PokeGra_GetFileOffset( mons_no, form_no, sex, rare, dir, egg, &mons_offset, &dir_offset, NULL, NULL );

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
ARCDATID POKEGRA_GetNcecArcIndex( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg )
{	
	u32 mons_offset;
	u32 dir_offset;

	PokeGra_GetFileOffset( mons_no, form_no, sex, rare, dir, egg, &mons_offset, &dir_offset, NULL, NULL );

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
 *	@param	dir		    �|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *	@param  egg       �^�}�S���ǂ���
 *	@param	*p_mons_offset				�|�P�����ԍ��t�@�C���I�t�Z�b�g
 *	@param	*p_dir_offset			�w�ʃI�t�Z�b�g
 *	@param	*p_sex_offset				���ʃI�t�Z�b�g
 *	@param	u32 *p_rare_offset	���A�I�t�Z�b�g
 */
//-----------------------------------------------------------------------------
static void PokeGra_GetFileOffset( int mons_no, int form_no, int sex, int rare, int dir, BOOL egg, u32 *p_mons_offset, u32 *p_dir_offset, u32 *p_sex_offset, u32 *p_rare_offset )
{	
	u32 file_start;
	u32 file_offset;
	u32 mf_ratio;

  file_start	= POKEGRA_FILE_MAX * mons_no;
  file_offset	= (dir == POKEGRA_DIR_FRONT) ? POKEGRA_FRONT_M_NCGR: POKEGRA_BACK_M_NCGR;
  mf_ratio	= (dir == POKEGRA_DIR_FRONT) ? POKEGRA_FRONT_M_NCBR: POKEGRA_BACK_M_NCBR;

  //�^�}�S�`�F�b�N
  if( egg )
  { 
    if( form_no > 2 )
    { 
      form_no = 0;
    }
    file_start = POKEGRA_FILE_MAX * ( MONSNO_TAMAGO + form_no );
  }
  //�ʃt�H��������
  else if( form_no )
  { 
    int gra_index = POKETOOL_GetPersonalParam( mons_no, 0, POKEPER_ID_form_gra_index );
    int form_max = POKETOOL_GetPersonalParam( mons_no, 0, POKEPER_ID_form_max );
    if( form_no >= form_max )
    { 
      form_no = 0;
    }
    file_start = POKEGRA_FILE_MAX * ( MONSNO_MAX + 1 ) + POKEGRA_FILE_MAX * ( gra_index + form_no - 1 );
  }

  //���ʂ̃`�F�b�N
  switch( sex ){
  case PTL_SEX_MALE:
    break;
  case PTL_SEX_FEMALE:
    //�I�X���X�����������Ă��邩�`�F�b�N����i�T�C�Y���O�Ȃ珑�������Ȃ��j
    sex = ( GFL_ARC_GetDataSize( ARCID_POKEGRA, file_start + mf_ratio + 1 ) == 0 ) ? PTL_SEX_MALE : PTL_SEX_FEMALE;
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
