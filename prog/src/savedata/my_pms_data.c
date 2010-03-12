//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		my_pms_data.c
 *	@brief  ���ȏЉ��ΐ�O���A�Ȃǂ́APMS���Z�[�u�f�[�^
 *	@author	Toru=Nagihashi
 *	@data		2010.01.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>
#include "savedata/save_tbl.h"

#include "savedata/my_pms_data.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	PMS���Z�[�u�f�[�^
//=====================================
struct _MYPMS_DATA
{
  PMS_DATA  pms[MYPMS_PMS_TYPE_MAX];
} ;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================

//=============================================================================
/**
 *          �O�����J
 */
//=============================================================================
//----------------------------------------------------------
//�Z�[�u�f�[�^�V�X�e���Ɉˑ�����֐�
//----------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �\���̃T�C�Y���擾
 *
 *	@param	void 
 *
 *	@return �\���̃T�C�Y
 */
//-----------------------------------------------------------------------------
int MYPMS_GetWorkSize( void )
{ 
  return sizeof( MYPMS_DATA );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �\���̂�������
 *
 *	@param	MYPMS_DATA *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void MYPMS_Init( MYPMS_DATA *p_wk )
{
  PMSDAT_SetupDefaultIntroductionMessage( &p_wk->pms[MYPMS_PMS_TYPE_INTRODUCTION] );
  PMSDAT_SetupDefaultBattleMessage( &p_wk->pms[MYPMS_PMS_TYPE_BATTLE_READY], PMS_BATTLE_TYPE_PLAYER_READY );
  PMSDAT_SetupDefaultBattleMessage( &p_wk->pms[MYPMS_PMS_TYPE_BATTLE_WON], PMS_BATTLE_TYPE_PLAYER_WIN );
  PMSDAT_SetupDefaultBattleMessage( &p_wk->pms[MYPMS_PMS_TYPE_BATTLE_LOST], PMS_BATTLE_TYPE_PLAYER_LOSE );
  PMSDAT_SetupDefaultBattleMessage( &p_wk->pms[MYPMS_PMS_TYPE_BATTLE_TOP], PMS_BATTLE_TYPE_LEADER );

  /*
  PMSDAT_Init( &p_wk->pms[MYPMS_PMS_TYPE_INTRODUCTION], PMS_TYPE_UNION );
  PMSDAT_Init( &p_wk->pms[MYPMS_PMS_TYPE_BATTLE_READY], PMS_TYPE_BATTLE_READY );
  PMSDAT_Init( &p_wk->pms[MYPMS_PMS_TYPE_BATTLE_WON], PMS_TYPE_BATTLE_WON );
  PMSDAT_Init( &p_wk->pms[MYPMS_PMS_TYPE_BATTLE_LOST], PMS_TYPE_BATTLE_LOST );
  PMSDAT_Init( &p_wk->pms[MYPMS_PMS_TYPE_BATTLE_TOP], PMS_TYPE_BATTLE_WON );
  */
}

//----------------------------------------------------------
//�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  ���[�N�擾  CONST��
 *
 *	@param	const SAVE_CONTROL_WORK * cp_sv �Z�[�u�f�[�^�|�C���^
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
const MYPMS_DATA * SaveData_GetMyPmsDataConst( const SAVE_CONTROL_WORK * cp_sv)
{ 
	return (const MYPMS_DATA *)SaveData_GetMyPmsData( (SAVE_CONTROL_WORK *)cp_sv);
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���[�N�擾
 *
 *	@param	SAVE_CONTROL_WORK * p_sv  �Z�[�u�f�[�^�|�C���^
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
MYPMS_DATA * SaveData_GetMyPmsData( SAVE_CONTROL_WORK * p_sv)
{ 
  return SaveControl_DataPtrGet(p_sv, GMDATA_ID_MYPMS);
}

//----------------------------------------------------------
//���ꂼ��̎擾�֐�
//----------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  PMS���擾
 *
 *	@param	const MYPMS_DATA *cp_wk ���[�N
 *	@param	type                    ���A���̎��
 *	@param	*p_pms                  �󂯎��ϐ�
 */
//-----------------------------------------------------------------------------
void MYPMS_GetPms( const MYPMS_DATA *cp_wk, MYPMS_PMS_TYPE type, PMS_DATA *p_pms )
{ 
  GF_ASSERT( type < MYPMS_PMS_TYPE_MAX );
  *p_pms  = cp_wk->pms[ type ];
}
//----------------------------------------------------------------------------
/**
 *	@brief  PMS��ݒ�
 *
 *	@param	MYPMS_DATA *p_wk  ���[�N
 *	@param	type              ���A���̎��
 *	@param	PMS_DATA *cp_pms  �ݒ肷��l
 *
 */
//-----------------------------------------------------------------------------
void MYPMS_SetPms( MYPMS_DATA *p_wk, MYPMS_PMS_TYPE type, const PMS_DATA *cp_pms )
{ 
  GF_ASSERT( type < MYPMS_PMS_TYPE_MAX );
  p_wk->pms[ type ] = *cp_pms;
}
