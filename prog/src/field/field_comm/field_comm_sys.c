//==============================================================================
/**
 * @file  field_comm_sys.c
 * @brief
 * @author  matsuda
 * @date  2009.04.30(��)
 */
//==============================================================================
#include <gflib.h>
#include "field/field_comm/field_comm_sys.h"
#include "field_comm_func.h"
#include "field_comm_data.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
typedef struct _COMM_FIELD_SYS{
  FIELD_COMM_FUNC *commFunc_;
  FIELD_COMM_DATA *commData_;
  
  GAME_COMM_SYS_PTR game_comm;
  
  BOOL comm_act_vanish[FIELD_COMM_MEMBER_MAX];   ///<TRUE:��\��
  u8 invalid_netid;           ///<�N����ROM��net_id
  u8 exit_recv;               ///<TRUE:�I���t���O
  u8 recv_profile;            ///<�v���t�B�[����M�t���O(bit�Ǘ�)
  u8 padding;
}COMM_FIELD_SYS;



//==================================================================
/**
 * �t�B�[���h�ʐM�Ď����[�N�𐶐�����(�ʐM�J�n���ɐ�������΂悢)
 *
 * @param   commHeapID
 *
 * @retval  COMM_FIELD_SYS_PTR    ���������t�B�[���h�ʐM�Ď����[�N�ւ̃|�C���^
 */
//==================================================================
COMM_FIELD_SYS_PTR FIELD_COMM_SYS_Alloc(HEAPID commHeapID, GAME_COMM_SYS_PTR game_comm)
{
  COMM_FIELD_SYS_PTR comm_field;

  comm_field = GFL_HEAP_AllocClearMemory(commHeapID, sizeof(COMM_FIELD_SYS));
  comm_field->commFunc_ = FIELD_COMM_FUNC_InitSystem( commHeapID );
  comm_field->commData_ = FIELD_COMM_DATA_InitSystem( commHeapID );
  comm_field->game_comm = game_comm;
  return comm_field;
}

//==================================================================
/**
 * �t�B�[���h�ʐM�Ď����[�N��j��(�ʐM�I�����ɔj��)
 *
 * @param   comm_field   �t�B�[���h�ʐM�Ď����[�N�ւ̃|�C���^
 */
//==================================================================
void FIELD_COMM_SYS_Free(COMM_FIELD_SYS_PTR comm_field)
{
  FIELD_COMM_DATA_TermSystem(comm_field->commData_);
  FIELD_COMM_FUNC_TermSystem(comm_field->commFunc_);
  GFL_HEAP_FreeMemory(comm_field);
}

FIELD_COMM_FUNC* FIELD_COMM_SYS_GetCommFuncWork( COMM_FIELD_SYS_PTR commField )
{
  return commField->commFunc_;
}

FIELD_COMM_DATA* FIELD_COMM_SYS_GetCommDataWork( COMM_FIELD_SYS_PTR commField )
{
  return commField->commData_;
}

GAME_COMM_SYS_PTR FIELD_COMM_SYS_GetGameCommSys( COMM_FIELD_SYS_PTR commField )
{
  return commField->game_comm;
}

//==================================================================
/**
 * �A�N�^�[�\���E��\���t���O�ւ̃|�C���^���擾
 *
 * @param   palace		
 * @param   net_id		
 *
 * @retval  BOOL *		
 */
//==================================================================
BOOL * FIELD_COMM_SYS_GetCommActorVanishFlag(COMM_FIELD_SYS_PTR commField, int net_id)
{
  return &commField->comm_act_vanish[net_id];
}

//==================================================================
/**
 * �N����ROM�̃l�b�gID���Z�b�g����
 *
 * @param   commField		    
 * @param   invalid_netid		�N����ROM�̃l�b�gID
 */
//==================================================================
void FIELD_COMM_SYS_SetInvalidNetID(COMM_FIELD_SYS_PTR commField, int invalid_netid)
{
  commField->invalid_netid = invalid_netid;
}

//==================================================================
/**
 * �N����ROM�̃l�b�gID���擾����
 *
 * @param   commField		
 *
 * @retval  int		�N����ROM�̃l�b�gID
 */
//==================================================================
int FIELD_COMM_SYS_GetInvalidNetID(COMM_FIELD_SYS_PTR commField)
{
  return commField->invalid_netid;
}

//==================================================================
/**
 * �I���t���O���Z�b�g����
 *
 * @param   commField		
 */
//==================================================================
void FIELD_COMM_SYS_SetExitReq(COMM_FIELD_SYS_PTR commField)
{
  commField->exit_recv = TRUE;
}

//==================================================================
/**
 * �I���t���O���擾����
 *
 * @param   commField		
 *
 * @retval  BOOL		TRUE:�I���t���O�������Ă���
 */
//==================================================================
BOOL FIELD_COMM_SYS_GetExitReq(COMM_FIELD_SYS_PTR commField)
{
  return commField->exit_recv;
}

//==================================================================
/**
 * �v���t�B�[����M�t���O���Z�b�g����
 *
 * @param   commField		
 * @param   net_id		
 */
//==================================================================
void FIELD_COMM_SYS_SetRecvProfile(COMM_FIELD_SYS_PTR commField, int net_id)
{
  commField->recv_profile |= 1 << net_id;
}

//==================================================================
/**
 * �v���t�B�[����M�t���O���擾����
 *
 * @param   commField		
 * @param   net_id		
 *
 * @retval  BOOL		TRUE:��M���Ă���
 */
//==================================================================
BOOL FIELD_COMM_SYS_GetRecvProfile(COMM_FIELD_SYS_PTR commField, int net_id)
{
  if(commField->recv_profile & (1 << net_id)){
    return TRUE;
  }
  return FALSE;
}
