//==============================================================================
/**
 * @file    comm_player_support.c
 * @brief   �ʐM�v���C���[����̃T�|�[�g
 * @author  matsuda
 * @date    2009.12.10(��)
 */
//==============================================================================
#include <gflib.h>
#include "gamesystem/comm_player_support.h"
#include "savedata/mystatus_local.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
///�ʐM���肩��̃T�|�[�g
struct _COMM_PLAYER_SUPPORT{
  SUPPORT_TYPE type;            ///<�T�|�[�g�̎��
  SUPPORT_TYPE used_type;       ///<���ۂɐ퓬�Ŏg�p���ꂽ�T�|�[�g�̎��
  SUPPORT_TYPE battle_end_type; ///<�퓬�I����T�|�[�g�̎�ޏ�

  MYSTATUS mystatus;            ///<�T�|�[�g���Ă��ꂽ�ʐM�����MYSTATUS
  MYSTATUS used_mystatus;       ///<���ۂɐ퓬��ʂŔ��f���ꂽ�����MYSTATUS
};


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �T�|�[�g�f�[�^Alloc
 *
 * @param   cps		
 */
//==================================================================
COMM_PLAYER_SUPPORT * COMM_PLAYER_SUPPORT_Alloc(HEAPID heap_id)
{
  COMM_PLAYER_SUPPORT *cps;
  
  cps = GFL_HEAP_AllocClearMemory(heap_id, sizeof(COMM_PLAYER_SUPPORT));
  COMM_PLAYER_SUPPORT_Init(cps);
  return cps;
}

//==================================================================
/**
 * �T�|�[�g�f�[�^FreeMemory
 *
 * @param   cps		
 */
//==================================================================
void COMM_PLAYER_SUPPORT_Free(COMM_PLAYER_SUPPORT *cps)
{
  GFL_HEAP_FreeMemory(cps);
}

//==================================================================
/**
 * �T�|�[�g�f�[�^������
 *
 * @param   cps		
 */
//==================================================================
void COMM_PLAYER_SUPPORT_Init(COMM_PLAYER_SUPPORT *cps)
{
  GFL_STD_MemClear(cps, sizeof(COMM_PLAYER_SUPPORT));
  cps->type = SUPPORT_TYPE_NULL;
  cps->used_type = SUPPORT_TYPE_NULL;
  cps->battle_end_type = SUPPORT_TYPE_NULL;
  MyStatus_Init(&cps->mystatus);
  MyStatus_Init(&cps->used_mystatus);
}

//==================================================================
/**
 * �T�|�[�g�p�����[�^���Z�b�g����
 *
 * @param   dest_cps		
 * @param   type		    
 * @param   mystatus		
 */
//==================================================================
void COMM_PLAYER_SUPPORT_SetParam(COMM_PLAYER_SUPPORT *dest_cps, SUPPORT_TYPE type, const MYSTATUS *mystatus)
{
  dest_cps->type = type;
  dest_cps->mystatus = *mystatus;
}

//==================================================================
/**
 * �T�|�[�g�f�[�^����MYSTATUS���擾
 *
 * @param   cps		
 *
 * @retval  const MYSTATUS *		
 */
//==================================================================
const MYSTATUS * COMM_PLAYER_SUPPORT_GetMyStatus(const COMM_PLAYER_SUPPORT *cps)
{
  return &cps->mystatus;
}

//==================================================================
/**
 * �T�|�[�g�f�[�^����T�|�[�g�^�C�v���擾
 *
 * @param   cps		
 *
 * @retval  SUPPORT_TYPE		
 */
//==================================================================
SUPPORT_TYPE COMM_PLAYER_SUPPORT_GetSupportType(const COMM_PLAYER_SUPPORT *cps)
{
  return cps->type;
}

//==================================================================
/**
 * �T�|�[�g�f�[�^���g�p�ςݏ�Ԃɂ���
 *
 * @param   cps		
 */
//==================================================================
void COMM_PLAYER_SUPPORT_SetUsed(COMM_PLAYER_SUPPORT *cps)
{
  cps->used_type = cps->type;
  cps->used_mystatus = cps->mystatus;

  cps->type = SUPPORT_TYPE_USED;
}

//==================================================================
/**
 * �퓬��̏󋵂̔��f
 *
 * @param   cps		
 */
//==================================================================
void COMM_PLAYER_SUPPORT_SetBattleEnd(COMM_PLAYER_SUPPORT *cps)
{
  cps->battle_end_type = cps->type;
  cps->type = SUPPORT_TYPE_NULL;
}

//==================================================================
/**
 * �퓬��A�T�|�[�g���������Ă������`�F�b�N
 *
 * @param   cps		
 *
 * @retval  BOOL		TRUE:�T�|�[�g���������Ă���
 */
//==================================================================
BOOL COMM_PLAYER_SUPPORT_GetBattleEndSupportType(const COMM_PLAYER_SUPPORT *cps)
{
  if(cps->used_type != SUPPORT_TYPE_NULL || cps->battle_end_type != SUPPORT_TYPE_NULL){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �퓬��Ɏg�p����c�[���F���ۂɏ����Ă��ꂽ�l��MYSTATUS���擾����
 *
 * @param   cps		
 *
 * @retval  const MYSTATUS *		
 *
 * �퓬��ʂŔ��f���ꂽ�T�|�[�g������Ȃ炻�̐l�D��
 * ���f���ꂽ�T�|�[�g���Ȃ��Ȃ�Ō�Ɏ�M���Ă���l�̃f�[�^��Ԃ�
 */
//==================================================================
const MYSTATUS * COMM_PLAYER_SUPPORT_GetSupportedMyStatus(const COMM_PLAYER_SUPPORT *cps)
{
  GF_ASSERT(COMM_PLAYER_SUPPORT_GetBattleEndSupportType(cps) == TRUE);
  
  if(cps->used_type != SUPPORT_TYPE_NULL){
    return &cps->used_mystatus;
  }
  return &cps->mystatus;
}


