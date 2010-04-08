//======================================================================
/**
 * @file  field_gimmick_d12.c
 * @brief  �r�N�e�B�̓� �M�~�b�N
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "fieldmap.h" 
#include "field_gimmick_d12.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "system/gfl_use.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/arc_def.h"
#include "gmk_tmp_wk.h"

#define EXPOBJ_UNIT_IDX (0)
//#define ARCID (ARCID_H01_GIMMICK) // �M�~�b�N�f�[�^�̃A�[�J�C�uID
#define D12_TMP_ASSIGN_ID  (1)

//------------------------------------------------------------------------------------------
/**
 * @brief �������֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void D12_GIMMICK_Setup( FIELDMAP_WORK* fieldmap )
{
#if 0  
  u32* gmk_save;  // �M�~�b�N�̎��Z�[�u�f�[�^
  GMK_WORK* work;  // �M�~�b�N�Ǘ����[�N
  HEAPID                heapID = FIELDMAP_GetHeapID( fieldmap );
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
  GAMESYS_WORK*            gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*               gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*          gmkwork = GAMEDATA_GetGimmickWork(gdata);

  //�ėp���[�N�m��
  GMK_TMP_WK_AllocWork
      (fieldmap, R04D03_TMP_ASSIGN_ID, FIELDMAP_GetHeapID(fieldmap), sizeof(GMK_WORK));
  work = GMK_TMP_WK_GetWork(fieldmap, R04D03_TMP_ASSIGN_ID);
  // �g���I�u�W�F�N�g�̃��j�b�g��ǉ�
  FLD_EXP_OBJ_AddUnit( exobj_cnt, &setup, EXPOBJ_UNIT_IDX );

  // �M�~�b�N�Ǘ����[�N�������� 
  InitWork( work, fieldmap );
#endif  
}

//------------------------------------------------------------------------------------------
/**
 * @brief �I���֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void D12_GIMMICK_End( FIELDMAP_WORK* fieldmap )
{
#if 0  
  int i;
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  //���j�b�g�j��
  FLD_EXP_OBJ_DelUnit( exobj_cnt, EXPOBJ_UNIT_IDX );  
  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldmap, R04D03_TMP_ASSIGN_ID);
#endif
}

//------------------------------------------------------------------------------------------
/**
 * @brief ����֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void D12_GIMMICK_Move( FIELDMAP_WORK* fieldmap )
{
}

