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

#include "system/main.h" //for HEAPID_FIELDMAP

#define OVERLAY_STACK_MAX (3)
#define OVERLAY_NONE  (-1)

typedef GMEVENT* (*CREATE_FUNC)(GAMESYS_WORK *, FIELDMAP_WORK *, const BOOL);
typedef void (*DRAW_FUNC)(ENCEFF_CNT_PTR);

FS_EXTERN_OVERLAY(enceff_ci);
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
  void *UserWorkPtr;
}ENCEFF_CNT;

typedef struct {
  CREATE_FUNC CreateFunc;
  DRAW_FUNC DrawFunc;
  FSOverlayID OverlayID;
  BOOL IsFadeWhite;
  u32 UseMemSize;  //�G�t�F�N�g���g�p����t�B�[���h�q�[�v�@���\�[�X�Ɉˑ�����̂ŁA�X�V�����������ꍇ�͍Ē������K�v�ł�
}ENCEFF_TBL;

//�� enceffno_def.h �ƕ��т𓯂��ɂ��邱��
static const ENCEFF_TBL EncEffTbl[] = {
  {ENCEFF_MDL_Create1, ENCEFF_MDL_Draw, FS_OVERLAY_ID(enceff_mdl), TRUE, 0x13010},     //�쐶�@�ʏ�
  {ENCEFF_MDL_Create2, ENCEFF_MDL_Draw, FS_OVERLAY_ID(enceff_mdl), TRUE, 0x131a4},     //�쐶�@��
  {ENCEFF_WAV_Create, ENCEFF_WAV_Draw, FS_OVERLAY_ID(enceff_wav), TRUE, 0},      //�쐶�@���� �ނ�
  {ENCEFF_CI_CreateCave, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0x1e04},     //�쐶�@����
  {ENCEFF_CI_CreateDesert, NULL, FS_OVERLAY_ID(enceff_ci), TRUE, 0x14bc8},     //�쐶�@���n
  {ENCEFF_MDL_Create3, ENCEFF_MDL_Draw, FS_OVERLAY_ID(enceff_mdl), TRUE, 0x12fbc},     //�g���[�i�[�@�ʏ�
  {ENCEFF_MDL_Create4, ENCEFF_MDL_Draw, FS_OVERLAY_ID(enceff_mdl), TRUE, 0x13b20},      //�g���[�i�[�@����
  {ENCEFF_PNL1_Create, ENCEFF_PNL_Draw, FS_OVERLAY_ID(enceff_pnl1), FALSE, 0},    //�g���[�i�[�@����
  {ENCEFF_PNL3_Create, ENCEFF_PNL_Draw, FS_OVERLAY_ID(enceff_pnl3), TRUE, 0},    //�g���[�i�[�@���n

  {ENCEFF_PNL2_Create, ENCEFF_PNL_Draw, FS_OVERLAY_ID(enceff_pnl2), TRUE, 0},    //�o�g���T�u�E�F�C

  {ENCEFF_CI_CreateRival, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0xe570},    //���C�o��
  {ENCEFF_CI_CreateSupport, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0xe570},    //�T�|�[�^�[
  {ENCEFF_CI_CreateGym01A, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0xe570},    //C1�W�����[�_�[�`
  {ENCEFF_CI_CreateGym01B, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0xe570},    //C1�W�����[�_�[�a
  {ENCEFF_CI_CreateGym01C, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0xe570},    //C1�W�����[�_�[�b
  {ENCEFF_CI_CreateGym02, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0xe570},    //C2�W�����[�_�[
  {ENCEFF_CI_CreateGym03, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0xe570},    //C3�W�����[�_�[
  {ENCEFF_CI_CreateGym04, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0xe570},    //C4�W�����[�_�[
  {ENCEFF_CI_CreateGym05, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0xe570},    //C5�W�����[�_�[
  {ENCEFF_CI_CreateGym06, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0xe570},    //C6�W�����[�_�[
  {ENCEFF_CI_CreateGym07, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0xe570},    //C7�W�����[�_�[
  {ENCEFF_CI_CreateGym08A, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0xe570},    //C8�W�����[�_�[�`
  {ENCEFF_CI_CreateGym08B, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0xe570},    //C8�W�����[�_�[�a
  {ENCEFF_CI_CreateBigFour1, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0x12a68},    //�l�V��1
  {ENCEFF_CI_CreateBigFour2, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0x12a68},    //�l�V��2
  {ENCEFF_CI_CreateBigFour3, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0x12a68},    //�l�V��3
  {ENCEFF_CI_CreateBigFour4, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0x12a68},    //�l�V��4
  {ENCEFF_CI_CreateChamp, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0x12a68},    //�`�����v
  {ENCEFF_CI_CreateBoss, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0xd530},    //N
  {ENCEFF_CI_CreateSage, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0xd530},    //�Q�[�c�B�X
  {ENCEFF_CI_CreatePlasma, NULL, FS_OVERLAY_ID(enceff_ci), FALSE, 0xcf20},    //�v���Y�}�c

  {ENCEFF_MDL_Create5, ENCEFF_MDL_Draw, FS_OVERLAY_ID(enceff_mdl), TRUE, 0x132e8},    //�G�t�F�N�g
  {ENCEFF_CI_CreatePackage, NULL, FS_OVERLAY_ID(enceff_ci), TRUE, 0x80e0},    //�p�b�P�[�W�|�P
  {ENCEFF_CI_CreateMovePoke, NULL, FS_OVERLAY_ID(enceff_ci), TRUE, 0x619c},    //�ړ��|�P
  {ENCEFF_CI_CreateThree, NULL, FS_OVERLAY_ID(enceff_ci), TRUE, 0xac88},    //�O�e�m�|�P
  {ENCEFF_MDL_Create6, ENCEFF_MDL_Draw, FS_OVERLAY_ID(enceff_mdl), TRUE, 0x12dc0},    //�]���A�[�N

  {ENCEFF_WAV_Create2, ENCEFF_WAV_Draw, FS_OVERLAY_ID(enceff_wav), FALSE, 0},    //�p���X���[�v�i�G���J�E���g�ɂ͎g�p���Ȃ��j

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
  ENCEFF_FreeUserWork(ptr);
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
  no = inID;
  if (no >= ENCEFFID_MAX)
  {
    GF_ASSERT_MSG(0,"ID=%d",no);
    no = 0;
  }
#ifdef PM_DEBUG
#ifdef DEBUG_ONLY_FOR_saitou
//  no = ENCEFFID_MAX-1;
#endif
#endif

  //�q�[�v���m�ۂł��邩���`�F�b�N GFI�֐��͊�{�I�ɒ��ڎg�p�֎~�ł����A����͓��ʂɎg�p���܂��B
  if ( GFI_HEAP_GetHeapAllocatableSize(HEAPID_FIELDMAP) < EncEffTbl[no].UseMemSize )
  {
    GF_ASSERT_MSG(0,"EFF_CAN_NOT_ALLOC No:%d  Allocatable=%x total=%x",
        no, GFI_HEAP_GetHeapAllocatableSize(HEAPID_FIELDMAP),GFL_HEAP_GetHeapFreeSize(HEAPID_FIELDMAP));
    //�t�B�[���h�q�[�v���g�p���Ȃ��G�t�F�N�g�ɕύX
    no = ENCEFFID_TR_INNER;
  }
  
  //�I�[�o�[���C���[�h
  if (EncEffTbl[no].OverlayID != OVERLAY_NONE)
  {
    LoadOverlay(ptr, EncEffTbl[no].OverlayID);
  }

  call_event = EncEffTbl[no].CreateFunc(gsys, fieldmap, EncEffTbl[no].IsFadeWhite);
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

//--------------------------------------------------------------
/**
 * @brief   ���[�U�[���[�N�|�C���^���Z�b�g
 * @param ptr   �R���g���[���|�C���^
 * @retval  void*     ���[�N�|�C���^
 */
//--------------------------------------------------------------
void *ENCEFF_AllocUserWork(ENCEFF_CNT_PTR ptr, const int size, const HEAPID inHeapID)
{
  if (ptr->UserWorkPtr != NULL)
  {
    GF_ASSERT(0);
  }

  NOZOMU_Printf("user_work_size = %x\n",size); 
  
  ptr->UserWorkPtr = GFL_HEAP_AllocClearMemory(inHeapID, size);
  return ptr->UserWorkPtr;
}

//--------------------------------------------------------------
/**
 * @brief   ���[�U�[���[�N���
 * @param ptr   �R���g���[���|�C���^
 * @retval  none
 */
//--------------------------------------------------------------
void ENCEFF_FreeUserWork(ENCEFF_CNT_PTR ptr)
{
  if (ptr->UserWorkPtr != NULL)
  {
    GFL_HEAP_FreeMemory( ptr->UserWorkPtr );
    ptr->UserWorkPtr = NULL;
  }
}


//--------------------------------------------------------------
/**
 * @brief   ���[�U�[���[�N�|�C���^���擾
 * @param ptr   �R���g���[���|�C���^
 * @retval  void*     ���[�N�|�C���^
 */
//--------------------------------------------------------------
void *ENCEFF_GetUserWorkPtr(ENCEFF_CNT_PTR ptr)
{
  return ptr->UserWorkPtr;
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


