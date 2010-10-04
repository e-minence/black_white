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

#include "trtype_gra.h"

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
static  inline  const u8 get_gra_index( int tr_type, int dir );

//=============================================================================
/**
 *		���\�[�X�擾�֐�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	POKEGRA�̃A�[�J�C�uID
 *
 *	@param[in]  dir ����
 *		
 *	@return	�A�[�J�C�uID
 */
//-----------------------------------------------------------------------------
ARCID TRGRA_GetArcID( int dir )
{	
  return 0;
/*  if( dir )
  { 
	  return ARCID_TRBGRA;
  }
  else
  { 
	  return ARCID_TRFGRA;
  }*/
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param[in]	tr_type   �g���[�i�[�^�C�v
 *	@param[in]  dir       ����
 *
 *	@return	�L�����A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetCgrArcIndex( int tr_type, int dir )
{	
	int	file_start = TRGRA_FILE_MAX * get_gra_index( tr_type, dir );	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z

	return file_start + TRGRA_NCGR;
}

//----------------------------------------------------------------------------
/**
 *	@brief	
 *
 *  @param[in]	tr_type   �g���[�i�[�^�C�v
 *	@param[in]  dir       ����
 *
 *	@return	�L�����r�b�g�}�b�v�A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetCbrArcIndex( int tr_type, int dir )
{	
	int	file_start = TRGRA_FILE_MAX * get_gra_index( tr_type, dir );	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z

	return file_start + TRGRA_NCBR;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param[in]	tr_type   �g���[�i�[�^�C�v
 *	@param[in]  dir       ����
 *
 *	@return	��گăA�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetPalArcIndex( int tr_type, int dir )
{	
	int	file_start = TRGRA_FILE_MAX * get_gra_index( tr_type, dir );	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z

	return file_start + TRGRA_NCLR;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param[in]	tr_type   �g���[�i�[�^�C�v
 *	@param[in]  dir       ����
 *
 *	@return	�Z���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetCelArcIndex( int tr_type, int dir )
{	
	int	file_start = TRGRA_FILE_MAX * get_gra_index( tr_type, dir );	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z

	return file_start + TRGRA_NCER;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param[in]	tr_type   �g���[�i�[�^�C�v
 *	@param[in]  dir       ����
 *
 *	@return	�Z���A�j���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetAnmArcIndex( int tr_type, int dir )
{	
	int	file_start = TRGRA_FILE_MAX * get_gra_index( tr_type, dir );	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z

	return file_start + TRGRA_NANR;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param[in]	tr_type   �g���[�i�[�^�C�v
 *	@param[in]  dir       ����
 *
 *	@return	�}���`�Z���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetMCelArcIndex( int tr_type, int dir )
{	
	int	file_start = TRGRA_FILE_MAX * get_gra_index( tr_type, dir );	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z

	return file_start + TRGRA_NMCR;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param[in]	tr_type   �g���[�i�[�^�C�v
 *	@param[in]  dir       ����
 *
 *	@return	�}���`�Z���A�j���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetMAnmArcIndex( int tr_type, int dir )
{	
	int	file_start = TRGRA_FILE_MAX * get_gra_index( tr_type, dir );	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z

	return file_start + TRGRA_NMAR;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param[in]	tr_type   �g���[�i�[�^�C�v
 *	@param[in]  dir       ����
 *
 *	@return	�A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetNcecArcIndex( int tr_type, int dir )
{	
	int	file_start = TRGRA_FILE_MAX * get_gra_index( tr_type, dir );	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z

	return file_start + TRGRA_NCEC;
}

static  inline  const u8 get_gra_index( int tr_type, int dir )
{ 
  //�w�ʂ͎擾����K�v�Ȃ�
  if( dir & 1 )
  { 
    return tr_type;
  }
  GF_ASSERT( NELEMS( TrTypeGraTable ) > tr_type );

  if( NELEMS( TrTypeGraTable ) < tr_type )
  { 
    return 0;
  }

  return TrTypeGraTable[ tr_type ];
}
