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

#define OVERLAY_STACK_MAX (3)
#define OVERLAY_NONE  (-1)

typedef GMEVENT* (*CREATE_FUNC)(GAMESYS_WORK *, FIELDMAP_WORK *);
typedef void (*DRAW_FUNC)(ENCEFF_CNT_PTR);

FS_EXTERN_OVERLAY(enceff_mdl);
FS_EXTERN_OVERLAY(enceff_prg);
FS_EXTERN_OVERLAY(enceff_wav);
FS_EXTERN_OVERLAY(enceff_pnl);
FS_EXTERN_OVERLAY(enceff_pnl1);
FS_EXTERN_OVERLAY(enceff_pnl2);
FS_EXTERN_OVERLAY(enceff_pnl3);

typedef struct ENCEFF_CNT_tag
{
  void *Work;
  DRAW_FUNC DrawFunc;
  FIELDMAP_WORK *FieldMapWork;
  int OverlayStack[OVERLAY_STACK_MAX];
  int OverlayStackNum;
}ENCEFF_CNT;

typedef struct {
  CREATE_FUNC CreateFunc;
  DRAW_FUNC DrawFunc;
  FSOverlayID OverlayID;
}ENCEFF_TBL;

static const ENCEFF_TBL EncEffTbl[] = {
  {ENCEFF_MDL_Create1, ENCEFF_MDL_Draw, FS_OVERLAY_ID(enceff_mdl)},
  {ENCEFF_WAV_Create, ENCEFF_WAV_Draw, FS_OVERLAY_ID(enceff_wav)},
  {ENCEFF_PNL1_Create, ENCEFF_PNL_Draw, FS_OVERLAY_ID(enceff_pnl1)},
  {ENCEFF_PNL2_Create, ENCEFF_PNL_Draw, FS_OVERLAY_ID(enceff_pnl2)},
  {ENCEFF_PNL3_Create, ENCEFF_PNL_Draw, FS_OVERLAY_ID(enceff_pnl3)},
};


static void LoadOverlay(ENCEFF_CNT_PTR ptr, const inFSOverlayID);


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
  ptr->OverlayStackNum = 0;
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
  //�I�[�o�[���C�X�^�b�N������ꍇ�͂����ŃA�����[�h
  ENCEFF_UnloadEffOverlay(ptr);
  
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
  no = GFUser_GetPublicRand(ENCEFFID_MAX);
#ifdef PM_DEBUG
#ifdef DEBUG_ONLY_FOR_saitou
  no = ENCEFFID_MAX-1;
  no = 0;
#endif
#endif
  //�I�[�o�[���C���[�h
  if (EncEffTbl[no].OverlayID != OVERLAY_NONE)
  {
    LoadOverlay(ptr, EncEffTbl[no].OverlayID);
  }

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

//--------------------------------------------------------------------------------------------
/**
 * �I�[�o�[���C�����[�h���ăX�^�b�N����
 *
 * @param   ptr     �R���g���[���|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void LoadOverlay(ENCEFF_CNT_PTR ptr, const inFSOverlayID)
{
  GFL_OVERLAY_Load( inFSOverlayID );
  ptr->OverlayStack[ ptr->OverlayStackNum++ ] = inFSOverlayID;
  GF_ASSERT( ptr->OverlayStackNum <= OVERLAY_STACK_MAX );
}

//--------------------------------------------------------------------------------------------
/**
 * �p�l���G�t�F�N�g�p�I�[�o�[���C���[�h
 *
 * @param   ptr     �R���g���[���|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ENCEFF_LoadPanelEffOverlay(ENCEFF_CNT_PTR ptr)
{
  //�v���O��������G�t�F�N�g�I�[�o�[���C���[�h
  LoadOverlay(ptr, FS_OVERLAY_ID(enceff_prg));
  //�p�l���G�t�F�N�g�I�[�o�[���C���[�h
  LoadOverlay(ptr, FS_OVERLAY_ID(enceff_pnl));
}

//--------------------------------------------------------------------------------------------
/**
 * ���_�G�t�F�N�g�p�I�[�o�[���C���[�h
 *
 * @param   ptr     �R���g���[���|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ENCEFF_LoadVertexEffOverlay(ENCEFF_CNT_PTR ptr)
{
  //�v���O��������G�t�F�N�g�I�[�o�[���C���[�h
  LoadOverlay(ptr, FS_OVERLAY_ID(enceff_prg));
}

//--------------------------------------------------------------------------------------------
/**
 * �G�t�F�N�g�p�I�[�o�[���C�A�����[�h
 *
 * @param   
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ENCEFF_UnloadEffOverlay(ENCEFF_CNT_PTR ptr)
{
  if (ptr->OverlayStackNum>0)
  {
    int idx;
    while(ptr->OverlayStackNum>0)
    {
      idx = ptr->OverlayStackNum-1;
      //�A�����[�h
      if ( ptr->OverlayStack[ idx ] != NULL )
      {
        GFL_OVERLAY_Unload( ptr->OverlayStack[ idx ] );
      }
      else GF_ASSERT_MSG(0,"Can not unload idx=%d\n", idx);

      ptr->OverlayStackNum--;
    }
  }
}


