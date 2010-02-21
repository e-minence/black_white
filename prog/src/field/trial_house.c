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

#include  "trial_house_scr_def.h"
#include "savedata/battle_examination.h"

static void MakeTrainer(TRIAL_HOUSE_WORK_PTR ptr, const int inBtlCount);
static void SetDownLoadData(GAMESYS_WORK * gsys, TRIAL_HOUSE_WORK_PTR ptr, const int inBtlCount);
static u16 GetTrainerOBJCode( TRIAL_HOUSE_WORK_PTR ptr );

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
  //�p�[�e�B�쐬
  ptr->Party = PokeParty_AllocPartyWork( ptr->HeapID );
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
    GFL_HEAP_FreeMemory((*ptr)->Party);
    GFL_HEAP_FreeMemory( *ptr );
    *ptr = NULL;
  }
  else GF_ASSERT(0);
}

//--------------------------------------------------------------
/**
 * @brief	�g���C�A���n�E�X �v���C���[�h�̃Z�b�g
 * @param	ptr      TRIAL_HOUSE_WORK_PTR
 * @param inPlayMode     �v���C���[�h�@ TH_PLAYMODE_SINGLE or TH_PLAYMODE_DOUBLE
 * @note  trial_house_scr_def.h�Q��
 * @retval		none
*/
//--------------------------------------------------------------
void TRIAL_HOUSE_SetPlayMode( TRIAL_HOUSE_WORK_PTR ptr, const u32 inPlayMode )
{
  BSWAY_PLAYMODE mode;
  if ( ptr == NULL )
  {
    GF_ASSERT(0);
    return;
  }

  if ( (inPlayMode == TH_PLAYMODE_SINGLE)||(inPlayMode == TH_PLAYMODE_DOUBLE) )
  {
    //�V���O��3�@����ȊO�i�_�u���j4
    if ( inPlayMode == TH_PLAYMODE_SINGLE )
    {
      mode = BSWAY_PLAYMODE_SINGLE; //�o�g���T�u�E�F�C�̃V���O�����[�h���Z�b�g
      ptr->MemberNum = 3;
    }
    else
    {
      mode = BSWAY_PLAYMODE_DOUBLE; //�o�g���T�u�E�F�C�̃_�u�����[�h���Z�b�g
      ptr->MemberNum = 4;
    }

    ptr->PlayMode = mode;
  }
  else
  {
    GF_ASSERT_MSG(0,"mode error %d",inPlayMode);
    ptr->PlayMode = BSWAY_PLAYMODE_SINGLE;
    ptr->MemberNum = 3;
  }

  //�p�[�e�B�̍ő�C�����Z�b�g���ď�����
  PokeParty_Init( ptr->Party, ptr->MemberNum );
}

//--------------------------------------------------------------
/**
 * @brief	�g���C�A���n�E�X �v���C���[�h�̃Z�b�g
 * @param	ptr      TRIAL_HOUSE_WORK_PTR
 * @param inDLFlg   �_�E�����[�h�f�[�^�ŗV�Ԃ��H�@�@TRUE:�_�E�����[�h�f�[�^�@FALSE:ROM�f�[�^
 * @retval		none
*/
//--------------------------------------------------------------
void TRIAL_HOUSE_SetDLFlg( TRIAL_HOUSE_WORK_PTR ptr, const BOOL inDLFlg )
{
  ptr->DownLoad = inDLFlg;
}

//--------------------------------------------------------------
/**
 * @brief	�ΐ푊��̒��I
 * @param gsys        �Q�[���V�X�e���|�C���^
 * @param	ptr      TRIAL_HOUSE_WORK_PTR
 * @param inBtlCouont   �ΐ�񐔁@0�`4�i�ő�ܐ�j
 * @retval	obj_id      OBJ������     
*/
//--------------------------------------------------------------
int TRIAL_HOUSE_MakeTrainer( GAMESYS_WORK * gsys, TRIAL_HOUSE_WORK_PTR ptr, const int inBtlCount )
{
  int obj_id;

  if ( ptr->DownLoad ) SetDownLoadData(gsys, ptr, inBtlCount);
  else MakeTrainer(ptr, inBtlCount);

  //�����ڂ��擾���ĕԂ�
  obj_id = GetTrainerOBJCode( ptr );
  NOZOMU_Printf("obj_id = %d\n",obj_id); 
  return obj_id;
}

//--------------------------------------------------------------
/**
 * @brief	�ΐ푊��̒��I(�q�n�l�f�[�^)
 * @param	ptr      TRIAL_HOUSE_WORK_PTR
 * @param inBtlCouont   �ΐ�񐔁@0�`4�i�ő�ܐ�j
 * @retval	none  
*/
//--------------------------------------------------------------
static void MakeTrainer(TRIAL_HOUSE_WORK_PTR ptr, const int inBtlCount)
{
  int obj_id;
  int tr_no;
  int base, band;

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
  NOZOMU_Printf("entry_tr_no = %d\n",tr_no);

  //�f�[�^���g���C�A���n�E�X���[�N�ɃZ�b�g
  FBI_TOOL_MakeRomTrainerData(
      &ptr->TrData,
      tr_no, ptr->MemberNum,
      NULL,NULL,NULL, GFL_HEAP_LOWID(ptr->HeapID));
}

//--------------------------------------------------------------
/**
 * @brief	�ΐ푊��̃Z�b�g(�_�E�����[�h�f�[�^)
 * @param gsys    �Q�[���V�X�e���|�C���^
 * @param	ptr      TRIAL_HOUSE_WORK_PTR
 * @param inBtlCouont   �ΐ�񐔁@0�`4�i�ő�ܐ�j
 * @retval	none    
*/
//--------------------------------------------------------------
static void SetDownLoadData(GAMESYS_WORK * gsys, TRIAL_HOUSE_WORK_PTR ptr, const int inBtlCount)
{
  SAVE_CONTROL_WORK * sv;
  BATTLE_EXAMINATION_SAVEDATA *exa;
  BSUBWAY_PARTNER_DATA *data;
  {
    GAMEDATA *gdata = GAMESYSTEM_GetGameData(gsys);
    sv = GAMEDATA_GetSaveControlWork( gdata );
  }
  //�Z�[�u�f�[�^�ɃA�N�Z�X
  exa = BATTLE_EXAMINATION_SAVE_GetSvPtr(sv);
  //�f�[�^�擾
  data = BATTLE_EXAMINATION_SAVE_GetData(exa, inBtlCount);
  //�g���C�A���n�E�X���[�N�Ƀf�[�^���Z�b�g
  ptr->TrData = *data;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�^�C�v����OBJ�R�[�h���擾���Ă���
 * @param ptr   �g���C�A���n�E�X�|�C���^
 * @param idx �g���[�i�[�f�[�^�C���f�b�N�X
 * @retval u16 OBJ�R�[�h
 */
//--------------------------------------------------------------
static u16 GetTrainerOBJCode( TRIAL_HOUSE_WORK_PTR ptr )
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
GMEVENT *TRIAL_HOUSE_CreateBeforeMsgEvt( GAMESYS_WORK *gsys, TRIAL_HOUSE_WORK_PTR ptr )
{
  GMEVENT *event;

  event = FBI_TOOL_CreateTrainerBeforeMsgEvt(gsys, &ptr->TrData );

  return event;
}
