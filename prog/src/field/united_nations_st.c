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
#include "un_roominfo_def.h"

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
 * @brief�@�������擾
 * @note  �@�]�[���`�F���W���ɃX�N���v�g����R�[���ŕ����̂��q�̐����擾����̂ŏ풓�ɒu��
 * @note    �擾�ł�����́A�q���A�����̍��R�[�h�A�t���A
 * @param   work      ���[�N�|�C���^
 * @param   inType    �擾���^�C�v  un_roominfo_def.h �Q��
 * @retval  u32       �擾����
*/
//--------------------------------------------------------------
u32 UN_GetRoomInfo(UNSV_WORK* work, const u32 inType)
{
  u32 val;
  switch(inType){
  case UN_ROOMINFO_TYPE_OBJNUM:
    val = work->ObjNum;
    break;
  case UN_ROOMINFO_TYPE_COUNTRY:
    val = work->TargetCountryCode;
    break;
  case UN_ROOMINFO_TYPE_FLOOR:
    val = work->Floor;
    break;
  default:
    GF_ASSERT_MSG(0,"info code error %d",inType);
    val = 0;
  }
  return val;
}
