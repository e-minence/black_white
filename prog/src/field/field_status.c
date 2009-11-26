//======================================================================
/**
 * @file	field_status.c
 * @brief �t�B�[���h�}�b�v�Ǘ����
 * @author  tamada GAMEFREAK inc.
 * @date    2009.10.03
 */
//======================================================================

#include <gflib.h>
#include "field/field_status.h"


#include "field_status_local.h"   //MAPMODE
//--------------------------------------------------------------
//--------------------------------------------------------------

//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
struct _FIELD_STATUS{
  MAPMODE map_mode;
  u8 isFieldInitFlag;

  // �t�B�[���h�Z�֘A
  u8  fs_flash;      // �t���b�V��ON
  u16 fs_mapeffect;  // �����ڂ̏��

  // �G�ߕ\��
  BOOL seasonDispFlag;  // �\���t���O
  u8   seasonDispLast;  // �Ō�ɕ\�������G�� PMSEASON_xxxx
};

//======================================================================
//======================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
FIELD_STATUS * FIELD_STATUS_Create(HEAPID heapID)
{
  FIELD_STATUS * fldstatus;
  fldstatus = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_STATUS) );
  return fldstatus;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_STATUS_Delete(FIELD_STATUS * fldstatus)
{
  GFL_HEAP_FreeMemory( fldstatus );
}


//======================================================================
//======================================================================
//------------------------------------------------------------------
/**
 * �}�b�v���[�h�擾
 *
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 *
 * @retval  MAPMODE		�}�b�v���[�h
 */
//------------------------------------------------------------------
MAPMODE FIELD_STATUS_GetMapMode(const FIELD_STATUS * fldstatus)
{
  return fldstatus->map_mode;
}

//------------------------------------------------------------------
/**
 * �}�b�v���[�h�ݒ�
 *
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @param   map_mode		�}�b�v���[�h
 */
//------------------------------------------------------------------
void FIELD_STATUS_SetMapMode(FIELD_STATUS * fldstatus, MAPMODE map_mode)
{
  fldstatus->map_mode = map_mode;
}

//======================================================================
//======================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_STATUS_SetFieldInitFlag( FIELD_STATUS * fldstatus, BOOL flag )
{
  fldstatus->isFieldInitFlag = flag;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL FIELD_STATUS_GetFieldInitFlag( const FIELD_STATUS * fldstatus )
{
  return fldstatus->isFieldInitFlag;
}

//------------------------------------------------------------------
/**
 * @brief �G�ߕ\���t���O�̐ݒ�
 */
//------------------------------------------------------------------
void FIELD_STATUS_SetSeasonDispFlag( FIELD_STATUS * fldstatus, BOOL flag )
{
  fldstatus->seasonDispFlag = flag;
}

//------------------------------------------------------------------
/**
 * @brief �G�ߕ\���t���O�̎擾
 */
//------------------------------------------------------------------
BOOL FIELD_STATUS_GetSeasonDispFlag( const FIELD_STATUS * fldstatus )
{
  return fldstatus->seasonDispFlag;
}

//------------------------------------------------------------------
/**
 * @brief �Ō�ɕ\�������G�߂̐ݒ�
 */
//------------------------------------------------------------------
void FIELD_STATUS_SetSeasonDispLast( FIELD_STATUS * fldstatus, u8 season )
{
  fldstatus->seasonDispLast = season;
}

//------------------------------------------------------------------
/**
 * @brief �Ō�ɕ\�������G�߂̎擾
 */
//------------------------------------------------------------------
u8 FIELD_STATUS_GetSeasonDispLast( const FIELD_STATUS * fldstatus )
{
  return fldstatus->seasonDispLast;
}


//------------------------------------------------------------------
// �t�B�[���h�Z�@�t���b�V����ON�EOFF
//------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h�Z�@�t���b�V���@ON�EOFF
 *
 *	@param	fldstatus   ���[�N
 *	@param	flag        TRUE:ON   FALSE�FOFF
 */
//-----------------------------------------------------------------------------
void FIELD_STATUS_SetFieldSkillFlash( FIELD_STATUS * fldstatus, BOOL flag )
{
  GF_ASSERT( fldstatus );
  fldstatus->fs_flash = flag;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h�Z�@�t���b�V���@�`�F�b�N
 *
 *	@param	fldstatus   ���[�N
 *
 *	@retval TRUE    �t���b�V�����
 *	@retval FALSE   �t���b�V�����ĂȂ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_STATUS_IsFieldSkillFlash( const FIELD_STATUS * fldstatus )
{
  GF_ASSERT( fldstatus );
  return fldstatus->fs_flash;
}

//------------------------------------------------------------------
// �t�B�[���h�Z�@�}�b�v���ʏ��
//------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h�Z�@�}�b�v���ʏ�ԃ}�X�N�@��ݒ�
 *
 *	@param	fldstatus   ���[�N
 *	@param	msk         ��ԃ}�X�N
 */
//-----------------------------------------------------------------------------
void FIELD_STATUS_SetFieldSkillMapEffectMsk( FIELD_STATUS * fldstatus, u32 msk )  
{
  GF_ASSERT( fldstatus );
  fldstatus->fs_mapeffect = msk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h�Z�@�}�b�v���ʏ�ԃ}�X�N���@�擾
 *
 *	@param	fldstatus   ���[�N
 *
 *	@return �}�X�N
 */
//-----------------------------------------------------------------------------
u32 FIELD_STATUS_GetFieldSkillMapEffectMsk( const FIELD_STATUS * fldstatus )
{
  GF_ASSERT( fldstatus );
  return fldstatus->fs_mapeffect;
}




