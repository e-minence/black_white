//======================================================================
/**
 * @file  pleasure_boat_st.c
 * @brief   �V���D�풓��
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "pleasure_boat.h"
#include "pleasure_boat_def.h"

#include "arc/fieldmap/fldmmdl_objcode.h"  //for OBJCODE

//--------------------------------------------------------------
/**
 * @brief�@�����̐l�������擾�@�n�a�i�R�[�h
 * @note    �]�[���`�F���W���ɃX�N���v�g����R�[�������
 * @param   work      PL_BOAT_WORK_PTR
 * @apram   inRoomIdx �����ԍ�
 * @retval  int       �n�a�i�R�[�h
*/
//--------------------------------------------------------------
int PL_BOAT_GetObjCode(PL_BOAT_WORK_PTR work, const int inRoomIdx)
{
  if ( work==NULL ) return BOY1;
  return work->RoomParam[inRoomIdx].ObjCode;
}

//--------------------------------------------------------------
/**
 * @brief�@�����̐l�����_�u���o�g�����邩���`�F�b�N
 * @param   work      PL_BOAT_WORK_PTR
 * @apram   inRoomIdx �����ԍ�
 * @retval  BOOL       TRUE�Ń_�u���o�g��
*/
//--------------------------------------------------------------
BOOL PL_BOAT_CheckDblBtl(PL_BOAT_WORK_PTR work, const int inRoomIdx)
{
  BOOL rc = FALSE;

  if ( work==NULL ) return rc;

  if ( (inRoomIdx == 2)&&(work->DblBtl[0]) ) rc = TRUE;
  else if ( (inRoomIdx == 7)&&(work->DblBtl[1]) ) rc = TRUE;
    
  return rc;
}

