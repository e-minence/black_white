//======================================================================
/**
 * @file  trial_house.c
 * @brief �g���C�A���n�E�X
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"   //for GFUser_GetPublicRand
#include "trial_house.h"
#include "trial_house_def.h"
#include "system/main.h"  //for HEAPID_APP_CONTROL
#include "fld_btl_inst_tool.h"

//#include "../../../resource/fldmapdata/script/c03r0801_def.h"  //for SCRID_�`
//#include "script.h"     //for SCRIPT_SetEventScript

//#include "pl_boat_def.h"

//#include "pl_boat_setup.h"


//--------------------------------------------------------------
/**
 * @brief	//�g���C�A���n�E�X���[�N�m�ۊ֐�
 * @retval	ptr      TRIAL_HOUSE_WORK_PTR
*/
//--------------------------------------------------------------
TRIAL_HOUSE_WORK_PTR TRIAL_HOUSE_Start(void)
{
  TRIAL_HOUSE_WORK_PTR ptr;
  int size = sizeof(TRIAL_HOUSE_WORK);
  NOZOMU_Printf("TrialHouseWorkSize = 0x%x\n",size);
  ptr = GFL_HEAP_AllocClearMemory(HEAPID_APP_CONTROL, size);
  ptr->HeapID = HEAPID_APP_CONTROL;//HEAPID_PROC;

  return ptr;
}

//--------------------------------------------------------------
/**
 * @brief	�g���C�A���n�E�X���[�N����֐�
 * @param	ptr      TRIAL_HOUSE_WORK_PTR
 * @retval		none
*/
//--------------------------------------------------------------
void TRIAL_HOUSE_End( TRIAL_HOUSE_WORK_PTR *ptr )
{
  if ( *ptr != NULL )
  {
    GFL_HEAP_FreeMemory( *ptr );
    *ptr = NULL;
  }
  else GF_ASSERT(0);
}

//--------------------------------------------------------------
/**
 * @brief	�g���C�A���n�E�X �v���C���[�h�̃Z�b�g
 * @param	ptr      TRIAL_HOUSE_WORK_PTR
 * @param inPlayMode     
 * @retval		none
*/
//--------------------------------------------------------------
void TRIAL_HOUSE_SetPlayMode( TRIAL_HOUSE_WORK_PTR ptr, const BSWAY_PLAYMODE inPlayMode )
{
  if ( ptr == NULL )
  {
    GF_ASSERT(0);
    return;
  }

  if ( (inPlayMode == BSWAY_PLAYMODE_SINGLE)||(inPlayMode == BSWAY_PLAYMODE_DOUBLE) )
  {
    //�V���O��3�@����ȊO�i�_�u���j4
    if ( inPlayMode == BSWAY_PLAYMODE_SINGLE ) ptr->MemberNum = 3;
    else ptr->MemberNum = 4;

    ptr->PlayMode = inPlayMode;
  }
  else
  {
    GF_ASSERT_MSG(0,"mode error %d",inPlayMode);
    ptr->PlayMode = BSWAY_PLAYMODE_SINGLE;
    ptr->MemberNum = 3;
  }
}


//--------------------------------------------------------------
/**
 * @brief	�ΐ푊��̒��I
 * @param	ptr      TRIAL_HOUSE_WORK_PTR
 * @param inBtlCouont   �ΐ�񐔁@0�`4�i�ő�ܐ�j
 * @retval		none
*/
//--------------------------------------------------------------
int TRIAL_HOUSE_MakeTrainer( TRIAL_HOUSE_WORK_PTR ptr, const int inBtlCount )
{
  int obj_id;
  int tr_no;
  int base,band;

  switch(inBtlCount){
  case 0:
    base = 0;
    band = 100;
    break;
  case 1:
    base = 140;
    band = 20;
    break;
  case 2:
    base = 180;
    band = 20;
    break;
  case 3:
    base = 220;
    band = 80;
    break;
  case 4:
    base = 220;
    band = 80;
    break;
  default:
    GF_ASSERT_MSG(0,"count error %d",inBtlCount);
    base = 0;
    band = 100;
  }
  //�g���[�i�[���I
  tr_no = base + GFUser_GetPublicRand(band);

  //�f�[�^���g���C�A���n�E�X���[�N�ɃZ�b�g
  {
    BOOL ret;
    ret = FBI_TOOL_MakeRomTrainerData(
        &ptr->TrData,
        tr_no, ptr->MemberNum,
        NULL,NULL,NULL, GFL_HEAP_LOWID(ptr->HeapID));

    if (!ret)
    {
      GF_ASSERT(0);
    }
  }

  //�����ڂ��擾���ĕԂ�
  obj_id = TRIAL_HOUSE_GetTrainerOBJCode( ptr );
  
  return obj_id;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�^�C�v����OBJ�R�[�h���擾���Ă���
 * @param ptr   �g���C�A���n�E�X�|�C���^
 * @param idx �g���[�i�[�f�[�^�C���f�b�N�X
 * @retval u16 OBJ�R�[�h
 */
//--------------------------------------------------------------
u16 TRIAL_HOUSE_GetTrainerOBJCode( TRIAL_HOUSE_WORK_PTR ptr )
{
  return FBI_TOOL_GetTrainerOBJCode( ptr->TrData.bt_trd.tr_type );
}


//--------------------------------------------------------------
/**
 * @brief	�ΐ푊��̐擪�O���b�Z�[�W�\��
 * @param	ptr      TRIAL_HOUSE_WORK_PTR
 * @retval		none
*/
//--------------------------------------------------------------
void TRIAL_HOUSE_DispBtlBeforeMsg( TRIAL_HOUSE_WORK_PTR ptr )
{
  ;
}
