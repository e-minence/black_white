//======================================================================
/**
 * @file	fld_exp_obj.c
 * @brief	�t�B�[���h3�c�g���I�u�W�F�N�g
 * @author	Saito
 * @data	08.08.27
 *
 */
//======================================================================

#include "fld_exp_obj.h"

#define FLD_EXP_OBJ_UNIT_MAX  (4)

typedef struct EXP_UNIT_tag
{
  BOOL Valid;
  GFL_G3D_OBJSTATUS *ObjStatus;
  u16 UtilUnitIdx;
  u16 pading;
}EXP_UNIT;

typedef struct FLD_EXP_OBJ_CNT_tag
{
  int HeapID;
  GFL_G3D_UTIL* ObjUtil;
///  GFL_G3D_OBJSTATUS ObjStatus[FLD_EXP_OBJ_UNIT_MAX];
///  BOOL Valid[FLD_EXP_OBJ_UNIT_MAX];
  EXP_UNIT  Unit[FLD_EXP_OBJ_UNIT_MAX];

}FLD_EXP_OBJ_CNT;


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

  status = GFL_HEAP_AllocClearMemory(ptr->HeapID, sizeof(GFL_G3D_OBJSTATUS)*obj_num);
  ptr->Unit[inIndex].ObjStatus = status;
  for(i=0;i<obj_num;i++){
    VEC_Set( &status[i].scale, FX32_ONE, FX32_ONE, FX32_ONE );
	  MTX_Identity33( &status[i].rotate );
    VEC_Set( &status[i].trans, 0, 0, 0 );
  }
  ptr->Unit[inIndex].Valid = TRUE;
  ptr->Unit[inIndex].UtilUnitIdx = unitIdx;
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
  if (ptr->Unit[inUnitIdx].Valid == FALSE){
    GF_ASSERT_MSG(0,"%d UNIT INVALID\n",inUnitIdx);
    return;
  }
  if (inUnitIdx >= FLD_EXP_OBJ_UNIT_MAX){
    GF_ASSERT_MSG(0,"%d UNIT IDX OVER\n",inUnitIdx);
    return;
  }

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
  return &ptr->Unit[inUnitIdx].ObjStatus[inObjIdx];
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

