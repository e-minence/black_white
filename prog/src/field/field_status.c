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

#include "script.h" //SCRID_NULL

//--------------------------------------------------------------
//--------------------------------------------------------------

//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
struct _FIELD_STATUS{
  MAPMODE map_mode;
  u8 isFieldInitFlag;
  u8 isContinueFlag;

  // �t�B�[���h�Z�֘A
  u8  fs_flash;      // �t���b�V��ON
  u16 fs_mapeffect;  // �����ڂ̏��

  // �G�ߕ\��
  BOOL seasonDispFlag;  // �\���t���O
  u8   seasonDispLast;  // �Ō�ɕ\�������G�� PMSEASON_xxxx

  u8 proc_action;       // �t�B�[���h�����A�퓬�����A�A�� PROC_ACTION_xxxx
  
  ///�\��X�N���v�gID
  u16 reserved_script_id;
};

//======================================================================
//======================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
FIELD_STATUS * FIELD_STATUS_Create(HEAPID heapID)
{
  FIELD_STATUS * fldstatus;
  fldstatus = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_STATUS) );
  fldstatus->reserved_script_id = SCRID_NULL;
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
//------------------------------------------------------------------
void FIELD_STATUS_SetContinueFlag( FIELD_STATUS * fldstatus, BOOL flag )
{
  fldstatus->isContinueFlag = flag;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL FIELD_STATUS_GetContinueFlag( const FIELD_STATUS * fldstatus )
{
  return fldstatus->isContinueFlag;
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

//------------------------------------------------------------------
/**
 * @brief �\��X�N���v�gID�̓o�^
 * @param fldstatus   ���[�N
 * @param scr_id    �o�^����X�N���v�g��ID
 */
//------------------------------------------------------------------
void FIELD_STATUS_SetReserveScript( FIELD_STATUS * fldstatus, u16 scr_id )
{
  fldstatus->reserved_script_id = scr_id;
}

//------------------------------------------------------------------
/**
 * @brief �\��X�N���v�gID�̎擾
 * @param fldstatus
 * @retval  u16   �o�^�����X�N���v�g��ID
 */
//------------------------------------------------------------------
u16 FIELD_STATUS_GetReserveScript( const FIELD_STATUS * fldstatus )
{
  return fldstatus->reserved_script_id;
}

//==================================================================
/**
 * PROC�A�N�V�������Z�b�g
 *
 * @param   fldstatus		
 * @param   action		    PROC_ACTION_xxxx
 *
 * @retval  PROC_ACTION		�Z�b�g�O��PROC�A�N�V����
 */
//==================================================================
PROC_ACTION FIELD_STATUS_SetProcAction( FIELD_STATUS * fldstatus, PROC_ACTION action)
{
  PROC_ACTION old_action = fldstatus->proc_action;
  fldstatus->proc_action = action;
  return old_action;
}

//==================================================================
/**
 * PROC�A�N�V�������擾
 *
 * @param   fldstatus		
 *
 * @retval  PROC_ACTION		PROC_ACTION_xxxx
 */
//==================================================================
PROC_ACTION FIELD_STATUS_GetProcAction( const FIELD_STATUS * fldstatus )
{
  return fldstatus->proc_action;
}

