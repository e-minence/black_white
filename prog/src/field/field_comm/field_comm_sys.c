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
