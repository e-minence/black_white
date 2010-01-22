//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		trgra.c
 *	@brief	�g���[�i�[�O���t�B�b�N�f�ނ̎擾
 *	@author	HisashiSogabe
 *	@data		2010.01.22
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
#include "system/trgra.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//�g���[�i�[��̂��\������MCSS�p�t�@�C���̍\��
enum{
	TRGRA_NCGR,
	TRGRA_NCBR,
	TRGRA_NCER,
	TRGRA_NANR,
	TRGRA_NMCR,
	TRGRA_NMAR,
	TRGRA_NCEC,
	TRGRA_NCLR,

	TRGRA_FILE_MAX,			//�g���[�i�[��̂��\������MCSS�p�t�@�C���̑���
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================

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
ARCID TRGRA_GetArcID( void )
{	
	return ARCID_TRFGRA;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param	tr_type  �g���[�i�[�^�C�v
 *
 *	@return	�L�����A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetCgrArcIndex( int tr_type )
{	
	int	file_start = TRGRA_FILE_MAX * tr_type;	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z

	return file_start + TRGRA_NCGR;
}

//----------------------------------------------------------------------------
/**
 *	@brief	
 *
 *  @param	tr_type  �g���[�i�[�^�C�v
 *
 *	@return	�L�����r�b�g�}�b�v�A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetCbrArcIndex( int tr_type )
{	
	int	file_start = TRGRA_FILE_MAX * tr_type;	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z

	return file_start + TRGRA_NCBR;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param	tr_type  �g���[�i�[�^�C�v
 *
 *	@return	��گăA�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetPalArcIndex( int tr_type )
{	
	int	file_start = TRGRA_FILE_MAX * tr_type;	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z

	return file_start + TRGRA_NCLR;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param	tr_type  �g���[�i�[�^�C�v
 *
 *	@return	�Z���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetCelArcIndex( int tr_type )
{	
	int	file_start = TRGRA_FILE_MAX * tr_type;	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z

	return file_start + TRGRA_NCER;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param	tr_type  �g���[�i�[�^�C�v
 *
 *	@return	�Z���A�j���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetAnmArcIndex( int tr_type )
{	
	int	file_start = TRGRA_FILE_MAX * tr_type;	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z

	return file_start + TRGRA_NANR;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param	tr_type  �g���[�i�[�^�C�v
 *
 *	@return	�}���`�Z���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetMCelArcIndex( int tr_type )
{	
	int	file_start = TRGRA_FILE_MAX * tr_type;	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z

	return file_start + TRGRA_NMCR;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param	tr_type  �g���[�i�[�^�C�v
 *
 *	@return	�}���`�Z���A�j���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetMAnmArcIndex( int tr_type )
{	
	int	file_start = TRGRA_FILE_MAX * tr_type;	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z

	return file_start + TRGRA_NMCR;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param	tr_type  �g���[�i�[�^�C�v
 *
 *	@return	�A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetNcecArcIndex( int tr_type )
{	
	int	file_start = TRGRA_FILE_MAX * tr_type;	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z

	return file_start + TRGRA_NCEC;
}

