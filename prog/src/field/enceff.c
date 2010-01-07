//======================================================================
/*
 * @file	enceff.c
 * @brief	�G���J�E���g�G�t�F�N�g
 * @author saito
 */
//======================================================================
#include "enceff.h"
#include "gamesystem/gamesystem.h"

#include "fieldmap.h"

typedef GMEVENT* (*CREATE_FUNC)(GAMESYS_WORK *, FIELDMAP_WORK *);
typedef void (*DRAW_FUNC)(ENCEFF_CNT_PTR);


typedef struct ENCEFF_CNT_tag
{
  void *Work;
  DRAW_FUNC DrawFunc;
  FIELDMAP_WORK *FieldMapWork;
}ENCEFF_CNT;

typedef struct {
  CREATE_FUNC CreateFunc;
  DRAW_FUNC DrawFunc;
}ENCEFF_TBL;

static const ENCEFF_TBL EncEffTbl[] = {
  {ENCEFF_CreateEff1, ENCEFF_DrawEff1},
  {ENCEFF_CreateEff2, ENCEFF_DrawEff2},
  {ENCEFF_CreateEff3, ENCEFF_DrawEff3},
};

//--------------------------------------------------------------
/**
 * @brief �G���J�E���g�G�t�F�N�g�R���g���[���쐬
 * @param inHeapID    �q�[�v�h�c
 * @retval  ptr   �R���g���[���|�C���^
 */
//--------------------------------------------------------------
ENCEFF_CNT_PTR ENCEFF_CreateCntPtr(const HEAPID inHeapID, FIELDMAP_WORK * fieldmap)
{
  ENCEFF_CNT_PTR ptr;
  ptr = GFL_HEAP_AllocClearMemory(inHeapID, sizeof(ENCEFF_CNT));
  ptr->FieldMapWork = fieldmap;
  return ptr;
}

//--------------------------------------------------------------
/**
 * @brief   �G���J�E���g�G�t�F�N�g�R���g���[���j��
 * @param ptr     �R���g���[���|�C���^
 * @retval  none
 */
//--------------------------------------------------------------
void ENCEFF_DeleteCntPtr(ENCEFF_CNT_PTR ptr)
{
  GFL_HEAP_FreeMemory( ptr );
}

//--------------------------------------------------------------
/**
 * @brief   �G���J�E���g�G�t�F�N�g�Z�b�g
 * @param   ptr       �R���g���[���|�C���^
 * @param   event     �C�x���g�|�C���^
 * @param   inID      �G�t�F�N�g�h�c
 * @retval  none
 */
//--------------------------------------------------------------
void ENCEFF_SetEncEff(ENCEFF_CNT_PTR ptr, GMEVENT * event, const ENCEFF_ID inID)
{
  GMEVENT *call_event;
  FIELDMAP_WORK * fieldmap = ptr->FieldMapWork;
  GAMESYS_WORK * gsys = FIELDMAP_GetGameSysWork( ptr->FieldMapWork );

  int no;
  no = GFUser_GetPublicRand(2);

  call_event = EncEffTbl[no].CreateFunc(gsys,fieldmap);
  //�C�x���g�R�[��
  GMEVENT_CallEvent( event, call_event );
  //���[�N�|�C���^�Z�b�g
  ptr->Work = GMEVENT_GetEventWork( call_event );
  //�`��֐��Z�b�g
  ptr->DrawFunc = EncEffTbl[no].DrawFunc;
}

//--------------------------------------------------------------
/**
 * @brief   ���[�N�|�C���^��Ԃ�
 * @param ptr   �R���g���[���|�C���^
 * @retval  void*     ���[�N�|�C���^
 */
//--------------------------------------------------------------
void *ENCEFF_GetWork(ENCEFF_CNT_PTR ptr)
{
  if (ptr->Work == NULL) GF_ASSERT(0);

  return ptr->Work;
}

//--------------------------------------------------------------
/**
 * @brief   �`��֐�
 * @param   ptr     �R���g���[���|�C���^
 * @retval  none
 */
//--------------------------------------------------------------
void ENCEFF_Draw(ENCEFF_CNT_PTR ptr)
{
  if (ptr->DrawFunc != NULL)
  {
    ptr->DrawFunc(ptr);
  }
}

