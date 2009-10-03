//======================================================================
/**
 * @file	fld_exp_obj.c
 * @brief	�t�B�[���h3�c�g���I�u�W�F�N�g
 * @author	Saito
 * @date	08.08.27
 *
 */
//======================================================================

#include "fld_exp_obj.h"

#define FLD_EXP_OBJ_UNIT_MAX  (4)

typedef struct EXP_OBJ_ANM_CNT_tag
{
  u8 Stop;
  u8 Loop;
  u8 Valid;
  u8 dummy;
  fx32 AddFrm;
  BOOL Continue;
}EXP_OBJ_ANM_CNT;

typedef struct ANM_LIST_tag
{
  int AnmNum;
  EXP_OBJ_ANM_CNT *AnmCnt;
}ANM_LIST;


typedef struct EXP_UNIT_tag
{
  BOOL Valid;
  GFL_G3D_OBJSTATUS *ObjStatus;
  ANM_LIST  *AnmList;
  u16 UtilUnitIdx;
  u16 ObjNum;
}EXP_UNIT;

typedef struct FLD_EXP_OBJ_CNT_tag
{
  int HeapID;
  GFL_G3D_UTIL* ObjUtil;
///  GFL_G3D_OBJSTATUS ObjStatus[FLD_EXP_OBJ_UNIT_MAX];
///  BOOL Valid[FLD_EXP_OBJ_UNIT_MAX];
  EXP_UNIT  Unit[FLD_EXP_OBJ_UNIT_MAX];

}FLD_EXP_OBJ_CNT;

static void AnmCntInit(ANM_LIST *lst, const int inAnmNum, const int inHeapID);


//--------------------------------------------------------------------------------------------
/**
 * ���W���[���쐬
 *
 * @param   inResCountMax			�ݒ胊�\�[�X�ő吔��
 * @param   inObjCountMax			�ݒ�I�u�W�F�N�g�ő吔
 * @param   inHeapID				�q�[�v�h�c
 *
 * @return	FLD_EXP_OBJ_CNT_PTR   ���W���[���|�C���^
 */
//--------------------------------------------------------------------------------------------
FLD_EXP_OBJ_CNT_PTR FLD_EXP_OBJ_Create( const int inResCountMax,
                                        const int inObjCountMax,
                                        const int inHeapID )
{
  FLD_EXP_OBJ_CNT_PTR ptr;
  ptr = GFL_HEAP_AllocClearMemory(inHeapID, sizeof(FLD_EXP_OBJ_CNT));
  ptr->ObjUtil = GFL_G3D_UTIL_Create( inResCountMax, inObjCountMax, inHeapID );
  ptr->HeapID = inHeapID;

  return ptr;
}

//--------------------------------------------------------------------------------------------
/**
 * ���W���[���j��
 *
 * @param   ptr     ���W���[���|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FLD_EXP_OBJ_Delete(FLD_EXP_OBJ_CNT_PTR ptr)
{
  u16 i;
  //����Y�ꂪ���邩������Ȃ��̂ŁA���j�b�g�p�������Ă���
  for (i=0;i<FLD_EXP_OBJ_UNIT_MAX;i++){
    if ( ptr->Unit[i].Valid){
      FLD_EXP_OBJ_DelUnit( ptr, i );
    }
  }
  GFL_G3D_UTIL_Delete( ptr->ObjUtil );
  GFL_HEAP_FreeMemory( ptr );
}

//--------------------------------------------------------------------------------------------
/**
 * ���j�b�g�ǉ�
 *
 * @param	ptr     ���W���[���|�C���^
 * @param	inSetup		�ݒ�f�[�^
 * @param inIndex   �o�^����C���f�b�N�X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FLD_EXP_OBJ_AddUnit(  FLD_EXP_OBJ_CNT_PTR ptr,
                          const GFL_G3D_UTIL_SETUP *inSetup,
                          const u16 inIndex)
{
  u16 i;
  u16 unitIdx;
  GFL_G3D_OBJSTATUS *status;
  u16 obj_num;

  if ( ptr->Unit[inIndex].Valid == TRUE ){
    GF_ASSERT_MSG(0,"CAN NOT ADD UNIT\n");
  }

  unitIdx = GFL_G3D_UTIL_AddUnit( ptr->ObjUtil, inSetup );
  obj_num = inSetup->objCount;
  if (obj_num == 0){
    GF_ASSERT_MSG(0,"OBJ NOTHING\n");
  }
  ptr->Unit[inIndex].ObjNum = obj_num;
  ptr->Unit[inIndex].Valid = TRUE;
  ptr->Unit[inIndex].UtilUnitIdx = unitIdx;

  status = GFL_HEAP_AllocClearMemory(ptr->HeapID, sizeof(GFL_G3D_OBJSTATUS)*obj_num);
  ptr->Unit[inIndex].ObjStatus = status;
  ptr->Unit[inIndex].AnmList = GFL_HEAP_AllocClearMemory(ptr->HeapID, sizeof(ANM_LIST)*obj_num);
  for(i=0;i<obj_num;i++){
    const GFL_G3D_UTIL_OBJ * objTbl = inSetup->objTbl;
    u16 anm_num = objTbl[i].anmCount;
    AnmCntInit(&ptr->Unit[inIndex].AnmList[i], anm_num, ptr->HeapID);

    VEC_Set( &status[i].scale, FX32_ONE, FX32_ONE, FX32_ONE );
	  MTX_Identity33( &status[i].rotate );
    VEC_Set( &status[i].trans, 0, 0, 0 );
#if 0
    {
      int j;
      GFL_G3D_OBJ *g3Dobj = FLD_EXP_OBJ_GetUnitObj(ptr, inIndex, i);
      EXP_OBJ_ANM_CNT *anm_cnt;
      ANM_LIST *lst;
      lst = &ptr->Unit[inIndex].AnmList[i];
      for (j=0;j<lst->AnmNum;j++){
        GFL_G3D_OBJECT_EnableAnime( g3Dobj, j );
      }
    }
#endif    
  }
  GF_ASSERT(GFL_HEAP_CheckHeapSafe(ptr->HeapID) == TRUE);
}

//--------------------------------------------------------------------------------------------
/**
 * ���j�b�g�j��
 *
 * @param	ptr         ���W���[���|�C���^
 * @param	inUnitIdx   ���j�b�g�C���f�b�N�X 
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FLD_EXP_OBJ_DelUnit( FLD_EXP_OBJ_CNT_PTR ptr, const u16 inUnitIdx )
{
  u16 i;
  if (ptr->Unit[inUnitIdx].Valid == FALSE){
    GF_ASSERT_MSG(0,"%d UNIT INVALID\n",inUnitIdx);
    return;
  }
  if (inUnitIdx >= FLD_EXP_OBJ_UNIT_MAX){
    GF_ASSERT_MSG(0,"%d UNIT IDX OVER\n",inUnitIdx);
    return;
  }
  GF_ASSERT(GFL_HEAP_CheckHeapSafe(ptr->HeapID) == TRUE);

  for(i=0;i<ptr->Unit[inUnitIdx].ObjNum;i++){
    GFL_HEAP_FreeMemory( ptr->Unit[inUnitIdx].AnmList[i].AnmCnt );
  }
  GFL_HEAP_FreeMemory( ptr->Unit[inUnitIdx].AnmList );

  GFL_HEAP_FreeMemory( ptr->Unit[inUnitIdx].ObjStatus );
  GFL_G3D_UTIL_DelUnit( ptr->ObjUtil, ptr->Unit[inUnitIdx].UtilUnitIdx );

  ptr->Unit[inUnitIdx].Valid = FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 *  ���[�e�B���e�B�|�C���^�擾 
 *
 * @param	ptr               ���W���[���|�C���^
 *
 * @return	GFL_G3D_UTIL*   ���[�e�B���e�B�|�C���^
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_UTIL *FLD_EXP_OBJ_GetUtil(FLD_EXP_OBJ_CNT_PTR ptr)
{
  GF_ASSERT(ptr != NULL);
  return ptr->ObjUtil;
}

//--------------------------------------------------------------------------------------------
/**
 *  ���[�e�B���e�B���Ǘ����郆�j�b�g�C���f�b�N�X���擾
 *
 * @param	ptr               ���W���[���|�C���^
 * @param inUnitIdx         ���W���[�����Ǘ����郆�j�b�g�̃C���f�b�N�X
 *
 * @return	u16   ���[�e�B���e�B�̎����j�b�g�̃C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
u16 FLD_EXP_OBJ_GetUtilUnitIdx(FLD_EXP_OBJ_CNT_PTR ptr, const u16 inUnitIdx)
{
  GF_ASSERT(ptr != NULL);
  return ptr->Unit[inUnitIdx].UtilUnitIdx;
}

//--------------------------------------------------------------------------------------------
/**
 *  ���j�b�g�X�e�[�^�X�|�C���^�擾
 *
 * @param   ptr               ���W���[���|�C���^
 * @param   inUnitIdx         ���j�b�g�C���f�b�N�X
 * @param   inObjIdx          ���j�b�g���n�a�i�C���f�b�N�X
 *
 * @return	GFL_G3D_OBJSTATUS*    �X�e�[�^�X�|�C���^
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_OBJSTATUS *FLD_EXP_OBJ_GetUnitObjStatus(FLD_EXP_OBJ_CNT_PTR ptr,
                                                const u16 inUnitIdx,
                                                const u16 inObjIdx)
{
  GF_ASSERT(ptr != NULL);
  if (ptr->Unit[inUnitIdx].Valid == FALSE){
    GF_ASSERT_MSG(0,"%d UNIT INVALID\n",inUnitIdx);
    return NULL;
  }
  if (inUnitIdx >= FLD_EXP_OBJ_UNIT_MAX){
    GF_ASSERT_MSG(0,"%d UNIT IDX OVER\n",inUnitIdx);
    return NULL;
  }
  GF_ASSERT(GFL_HEAP_CheckHeapSafe(ptr->HeapID) == TRUE);
  return &ptr->Unit[inUnitIdx].ObjStatus[inObjIdx];
}

//--------------------------------------------------------------------------------------------
/**
 *  ���j�b�gOBJ�|�C���^�擾
 *
 * @param   ptr               ���W���[���|�C���^
 * @param   inUnitIdx         ���j�b�g�C���f�b�N�X
 * @param   inObjIdx          ���j�b�g���n�a�i�C���f�b�N�X
 *
 * @return	GFL_G3D_OBJSTATUS*    �X�e�[�^�X�|�C���^
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_OBJ *FLD_EXP_OBJ_GetUnitObj(FLD_EXP_OBJ_CNT_PTR ptr,
                                    const u16 inUnitIdx,
                                    const u16 inObjIdx)
{
  u16 obj_idx;
  GFL_G3D_OBJ *pObj;

  GF_ASSERT(ptr != NULL);
  if (ptr->Unit[inUnitIdx].Valid == FALSE){
    GF_ASSERT_MSG(0,"%d UNIT INVALID\n",inUnitIdx);
    return NULL;
  }
  if (inUnitIdx >= FLD_EXP_OBJ_UNIT_MAX){
    GF_ASSERT_MSG(0,"%d UNIT IDX OVER\n",inUnitIdx);
    return NULL;
  }

  obj_idx = GFL_G3D_UTIL_GetUnitObjIdx( ptr->ObjUtil, ptr->Unit[inUnitIdx].UtilUnitIdx );
  pObj = GFL_G3D_UTIL_GetObjHandle(ptr->ObjUtil, obj_idx+inObjIdx);
  
  return pObj;
}


//--------------------------------------------------------------------------------------------
/**
 *  ���j�b�g�`��
 *
 * @param   ptr               ���W���[���|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FLD_EXP_OBJ_Draw( FLD_EXP_OBJ_CNT_PTR ptr )
{
  u8 i,j;
  //���j�b�g�������[�v
  for (i=0;i<FLD_EXP_OBJ_UNIT_MAX;i++){
    if (ptr->Unit[i].Valid == TRUE){
      u16 obj_idx = GFL_G3D_UTIL_GetUnitObjIdx( ptr->ObjUtil, ptr->Unit[i].UtilUnitIdx );
      u16 obj_count = GFL_G3D_UTIL_GetUnitObjCount( ptr->ObjUtil, ptr->Unit[i].UtilUnitIdx );
      for (j=0;j<obj_count;j++){
        GFL_G3D_OBJ* pObj;
        pObj = GFL_G3D_UTIL_GetObjHandle(ptr->ObjUtil, obj_idx+j);
        GFL_G3D_DRAW_DrawObject( pObj, &ptr->Unit[i].ObjStatus[j] );
      }
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 *  �A�j���[�V�����Đ��R���g���[��
 *
 * @param   ptr               ���W���[���|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FLD_EXP_OBJ_PlayAnime( FLD_EXP_OBJ_CNT_PTR ptr )
{
  u8 i,j,k;
  //���j�b�g�������[�v
  for (i=0;i<FLD_EXP_OBJ_UNIT_MAX;i++){
    if (ptr->Unit[i].Valid == TRUE){
      u16 obj_idx = GFL_G3D_UTIL_GetUnitObjIdx( ptr->ObjUtil, ptr->Unit[i].UtilUnitIdx );
      u16 obj_count = GFL_G3D_UTIL_GetUnitObjCount( ptr->ObjUtil, ptr->Unit[i].UtilUnitIdx );
      for (j=0;j<obj_count;j++){
        GFL_G3D_OBJ *pObj = FLD_EXP_OBJ_GetUnitObj(ptr, i, j);
        int anm_num = ptr->Unit[i].AnmList[j].AnmNum;
        for (k=0;k<anm_num;k++){
          EXP_OBJ_ANM_CNT *anm = &ptr->Unit[i].AnmList[j].AnmCnt[k];
          if ( !anm->Valid ){
            continue;
          }
          if ( !anm->Stop ){
            if (anm->Loop){      //���[�v�Đ�
              anm->Continue = GFL_G3D_OBJECT_LoopAnimeFrame( pObj, k, anm->AddFrm );
            }else{          //1��Đ�
              anm->Continue = GFL_G3D_OBJECT_IncAnimeFrame( pObj, k, anm->AddFrm );
            }
          }
        }
      }
    }
  }  
}

static void AnmCntInit(ANM_LIST *lst, const int inAnmNum, const int inHeapID)
{
  int i;
  EXP_OBJ_ANM_CNT *anm_cnt;
  lst->AnmCnt = GFL_HEAP_AllocClearMemory(inHeapID, sizeof(EXP_OBJ_ANM_CNT)*inAnmNum);
  anm_cnt = lst->AnmCnt;
  lst->AnmNum = inAnmNum;
  for (i=0;i<inAnmNum;i++){
    anm_cnt[i].Stop = 0;
    anm_cnt[i].Loop = 1;
    anm_cnt[i].Valid = 0;
    anm_cnt[i].AddFrm = FX32_ONE;
  }  
}

EXP_OBJ_ANM_CNT_PTR FLD_EXP_OBJ_GetAnmCnt(
    FLD_EXP_OBJ_CNT_PTR ptr, const u16 inUnitIdx, const u16 inObjIdx, const u16 inAnmIdx)
{
  EXP_OBJ_ANM_CNT *anm_cnt;
  if ( !ptr->Unit[inUnitIdx].Valid ){
    GF_ASSERT(0);
    return NULL;
  }
  if (inObjIdx >= ptr->Unit[inUnitIdx].ObjNum){
    GF_ASSERT(0);
    return NULL;
  }
  anm_cnt = ptr->Unit[inUnitIdx].AnmList[inObjIdx].AnmCnt;

  if (inAnmIdx >= ptr->Unit[inUnitIdx].AnmList[inObjIdx].AnmNum){
    GF_ASSERT(0);
    return NULL;
  }
  return &anm_cnt[inAnmIdx];
}

void FLD_EXP_OBJ_ChgAnmStopFlg(EXP_OBJ_ANM_CNT_PTR ptr, const u8 inStop)
{
  ptr->Stop = inStop;
}

u8 FLD_EXP_OBJ_GetAnmStopFlg(EXP_OBJ_ANM_CNT_PTR ptr)
{
  return ptr->Stop;
}

void FLD_EXP_OBJ_ChgAnmLoopFlg(EXP_OBJ_ANM_CNT_PTR ptr, const u8 inLoop)
{
  ptr->Loop = inLoop;
}

BOOL FLD_EXP_OBJ_ChkAnmEnd(EXP_OBJ_ANM_CNT_PTR ptr)
{
  return (!ptr->Continue);
}

void FLD_EXP_OBJ_SetObjAnmFrm(
                        FLD_EXP_OBJ_CNT_PTR ptr,
                        const u16 inUnitIdx,
                        const u16 inObjIdx,
                        const u16 inAnmIdx,
                        const fx32 inFrm  )
{
  int anmFrm = inFrm;
  GFL_G3D_OBJ *g3Dobj = FLD_EXP_OBJ_GetUnitObj(ptr, inUnitIdx, inObjIdx);
  GFL_G3D_OBJECT_SetAnimeFrame( g3Dobj, inAnmIdx, &anmFrm );
}

fx32 FLD_EXP_OBJ_GetObjAnmFrm(
                        FLD_EXP_OBJ_CNT_PTR ptr,
                        const u16 inUnitIdx,
                        const u16 inObjIdx,
                        const u16 inAnmIdx )
{
  int anmFrm;
  GFL_G3D_OBJ *g3Dobj = FLD_EXP_OBJ_GetUnitObj(ptr, inUnitIdx, inObjIdx);
  GFL_G3D_OBJECT_GetAnimeFrame( g3Dobj, inAnmIdx, &anmFrm );
  return anmFrm;
}


void FLD_EXP_OBJ_ValidCntAnm(
                        FLD_EXP_OBJ_CNT_PTR ptr,
                        const u16 inUnitIdx,
                        const u16 inObjIdx,
                        const u16 inAnmIdx,
                        const BOOL inValid)
{
  GFL_G3D_OBJ *g3Dobj = FLD_EXP_OBJ_GetUnitObj(ptr, inUnitIdx, inObjIdx);
  EXP_OBJ_ANM_CNT_PTR anm = FLD_EXP_OBJ_GetAnmCnt( ptr, inUnitIdx, inObjIdx, inAnmIdx );
  
  if (anm == NULL){
    return;
  }

  if (inValid){
    if (anm->Valid){
      return;
    }else{
      GFL_G3D_OBJECT_EnableAnime( g3Dobj, inAnmIdx );
    }
  }else{
    if (!anm->Valid){
      return;
    }
    GFL_G3D_OBJECT_DisableAnime( g3Dobj, inAnmIdx );
  }

  anm->Valid = inValid;

}
