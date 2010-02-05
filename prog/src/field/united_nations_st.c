//======================================================================
/**
 * @file  united_nations_st.c
 * @brief   ���A�풓��
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "arc/fieldmap/fldmmdl_objcode.h"  //for OBJCODE
#include "savedata/un_savedata.h"
#include "savedata/un_savedata_local.h"
//--------------------------------------------------------------
/**
 * @brief�@�����̐l�������擾�@�n�a�i�R�[�h
 * @note    �]�[���`�F���W���ɃX�N���v�g����R�[�������
 * @param   work      ���[�N�|�C���^
 * @apram   inObjIdx �C���f�b�N�X�ԍ� 0�`UN_ROOM_OBJ_MAX
 * @retval  u32       �n�a�i�R�[�h
*/
//--------------------------------------------------------------
u32 UN_GetRoomObjCode(UNSV_WORK* work, const u32 inObjIdx)
{
  if (inObjIdx >= work->ObjNum)
  {
    if (inObjIdx >= UN_ROOM_OBJ_MAX)
    {
      GF_ASSERT_MSG(0,"idx error idx=%d",inObjIdx);
      return BOY2;
    }
    //������ȓ��ł���΁A�\���͂��Ȃ��͂������A�萔��Ԃ��Ă���
    return BOY2;
  }
  
  return work->UnObjCode[inObjIdx];
}

//--------------------------------------------------------------
/**
 * @brief ������OBJ����Ԃ�
 * @note    �]�[���`�F���W���ɃX�N���v�g����R�[�������
 * @param * work            ���A���[�N�|�C���^
 * @retval  u32     OBJ�� 0�`5
 */
//--------------------------------------------------------------
u32 UN_GetRoomObjNum(UNSV_WORK* work)
{
  return work->ObjNum;
}

