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
#include "savedata/th_rank_def.h"
#include "savedata/save_tbl.h"    //for SAVESIZE_EXTRA_BATTLE_EXAMINATION

#include "net/delivery_beacon.h"

//�r�[�R���T�[�`���[�N
typedef struct
{
  DELIVERY_BEACON_INIT BeaconInit;
  DELIVERY_BEACON_WORK *BeaconWork;
  GAMESYS_WORK *gsys;
  TRIAL_HOUSE_WORK_PTR Ptr;
  u16 *Ret;
  int Time;
}EVENT_WORK_BEACON_SEARCH;

static void MakeTrainer(TRIAL_HOUSE_WORK_PTR ptr, const int inBtlCount);
static void SetDownLoadData( GAMESYS_WORK *gsys, TRIAL_HOUSE_WORK_PTR ptr, const int inBtlCount);
static u16 GetTrainerOBJCode( TRIAL_HOUSE_WORK_PTR ptr );

static GMEVENT_RESULT BeaconSearchEvt( GMEVENT *event, int *seq, void *wk );

typedef enum
{
  BEACON_SEQ_START = 0,
  BEACON_SEQ_SEARCH,
  BEACON_SEQ_CHECK,
  BEACON_SEQ_MAIN,
  BEACON_SEQ_END
}BEACON_SEQ;

//--------------------------------------------------------------
/**
 * @brief	//�g���C�A���n�E�X���[�N�m�ۊ֐�
 * @param   gsys      �Q�[���V�X�e���|�C���^
 * @retval	ptr      TRIAL_HOUSE_WORK_PTR
*/
//--------------------------------------------------------------
TRIAL_HOUSE_WORK_PTR TRIAL_HOUSE_Start(GAMESYS_WORK * gsys)
{
  TRIAL_HOUSE_WORK_PTR ptr;
  int size = sizeof(TRIAL_HOUSE_WORK);
  int comm_size = BATTLE_EXAMINATION_SAVE_GetWorkSize();
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gamedata);

  NOZOMU_Printf("TrialHouseWorkSize = 0x%x\n",size);
  NOZOMU_Printf("ExaminationSize = 0x%x\n",comm_size);
  ptr = GFL_HEAP_AllocClearMemory(HEAPID_APP_CONTROL, size);
  ptr->HeapID = HEAPID_APP_CONTROL;//HEAPID_PROC;
  ptr->CommBuf = GFL_HEAP_AllocClearMemory(HEAPID_APP_CONTROL, comm_size);
  ptr->ThSaveWork = GFL_HEAP_AllocClearMemory(GFL_HEAP_LOWID(HEAPID_PROC), SAVESIZE_EXTRA_BATTLE_EXAMINATION);

  if (sv != NULL)
  {
    //�O���f�[�^�����[�h
    if ( LOAD_RESULT_OK == SaveControl_Extra_LoadWork(sv, SAVE_EXTRA_ID_BATTLE_EXAMINATION, GFL_HEAP_LOWID(HEAPID_PROC),
                                                     ptr->ThSaveWork, SAVESIZE_EXTRA_BATTLE_EXAMINATION) )
    {
      OS_Printf("�g���C�A���n�E�X�O���f�[�^���[�h\n");
    }
    else GF_ASSERT_MSG(0,"extra_data_load fail");
  }
  //�p�[�e�B�쐬
  ptr->Party = PokeParty_AllocPartyWork( ptr->HeapID );
  return ptr;
}

//--------------------------------------------------------------
/**
 * @brief	�g���C�A���n�E�X���[�N����֐�
 * @param   gsys  �Q�[���V�X�e���|�C���^
 * @param	ptr      TRIAL_HOUSE_WORK_PTR
 * @retval		none
*/
//--------------------------------------------------------------
void TRIAL_HOUSE_End( GAMESYS_WORK * gsys, TRIAL_HOUSE_WORK_PTR *ptr )
{
  if ( *ptr != NULL )
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
    SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gamedata);
    SaveControl_Extra_UnloadWork(sv, SAVE_EXTRA_ID_BATTLE_EXAMINATION);
    GFL_HEAP_FreeMemory( (*ptr)->ThSaveWork );
    GFL_HEAP_FreeMemory( (*ptr)->CommBuf );
    GFL_HEAP_FreeMemory( (*ptr)->Party );
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
 * @param gsys      �Q�[���V�X�e���|�C���^
 * @param	ptr      TRIAL_HOUSE_WORK_PTR
 * @param inBtlCouont   �ΐ�񐔁@0�`4�i�ő�ܐ�j
 * @retval	obj_id      OBJ������     
*/
//--------------------------------------------------------------
int TRIAL_HOUSE_MakeTrainer( GAMESYS_WORK *gsys, TRIAL_HOUSE_WORK_PTR ptr, const int inBtlCount )
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
 * @param	ptr      TRIAL_HOUSE_WORK_PTR
 * @param inBtlCouont   �ΐ�񐔁@0�`4�i�ő�ܐ�j
 * @retval	none    
*/
//--------------------------------------------------------------
static void SetDownLoadData( GAMESYS_WORK *gsys, TRIAL_HOUSE_WORK_PTR ptr, const int inBtlCount)
{
  BATTLE_EXAMINATION_SAVEDATA *exa;
  BSUBWAY_PARTNER_DATA *data;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gamedata);
  exa = SaveControl_Extra_DataPtrGet( sv, SAVE_EXTRA_ID_BATTLE_EXAMINATION, 0);
  
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

//--------------------------------------------------------------
/**
 * @brief	�r�[�R���T�[�`
 * @param	gsys        �Q�[���V�X�e���|�C���^
 * @param ptr         �g���C�A���n�E�X���[�N�|�C���^
 * @param outRet      �T�[�`���ʊi�[�o�b�t�@
 * @retval	event     �C�x���g�|�C���^
*/
//--------------------------------------------------------------
GMEVENT *TRIAL_HOUSE_CreateBeaconSearchEvt( GAMESYS_WORK *gsys, TRIAL_HOUSE_WORK_PTR ptr, u16* outRet )
{
  GMEVENT *event;
  EVENT_WORK_BEACON_SEARCH *evt_wk;
  event = GMEVENT_Create( gsys, NULL,
      BeaconSearchEvt, sizeof(EVENT_WORK_BEACON_SEARCH) );
  evt_wk = GMEVENT_GetEventWork( event );
  evt_wk->gsys = gsys;
  evt_wk->Ret = outRet;
  evt_wk->Ptr = ptr;
  evt_wk->Time = 0;
  //�r�[�R���\���̏�����
  {
    DELIVERY_BEACON_INIT *init;
    init = &evt_wk->BeaconInit;
    init->NetDevID = WB_NET_BATTLE_EXAMINATION;   // //�ʐM���
    init->datasize = BATTLE_EXAMINATION_SAVE_GetWorkSize();  //�f�[�^�S�̃T�C�Y
    init->pData = ptr->CommBuf;     // ��M�o�b�t�@�f�[�^
    init->ConfusionID = 0;   //�������Ȃ����߂�ID
    init->heapID = HEAPID_APP_CONTROL;
  }

  return event;
}

//--------------------------------------------------------------
/**
 * �r�[�R���T�[�`  @todo�܂�������
 * @param event GMEVENT
 * @param seq event sequence
 * @param wk event work
 * @retval GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT BeaconSearchEvt( GMEVENT *event, int *seq, void *wk )
{
  EVENT_WORK_BEACON_SEARCH *evt_wk = wk;
  GAMESYS_WORK *gsys = evt_wk->gsys;

  switch( *seq ){
  case BEACON_SEQ_START:
    {
      BOOL rc;
      //�r�[�R���T�[�`�J�n
      evt_wk->BeaconWork = DELIVERY_BEACON_Init(&evt_wk->BeaconInit);
      rc = DELIVERY_BEACON_RecvStart(evt_wk->BeaconWork);
      if (!rc)
      { //�J�n���s
        GF_ASSERT_MSG(0,"beacon_start_error");
        //�󂯎��̌��ʂ��Z�b�g
        *(evt_wk->Ret) = FALSE;   //���s
        (*seq) = BEACON_SEQ_END;
      }
      else (*seq) = BEACON_SEQ_SEARCH;
    }
    break;
  case BEACON_SEQ_SEARCH:
    //�z�M�T�[�r�X���o�̂��߁A�Q�b�Ԉʔz�����C�����܂킷
    {
      //�z�����C��
      DELIVERY_BEACON_Main( evt_wk->BeaconWork );
    }
    evt_wk->Time++;
    if ( evt_wk->Time > 30*2 ) (*seq) = BEACON_SEQ_CHECK;
    break;
  case BEACON_SEQ_CHECK:
    //�z�����C��
    DELIVERY_BEACON_Main( evt_wk->BeaconWork );
    {
      BOOL rc;
      //�r�[�R�����P�ł����邩�ǂ����i �z�M�T�[�r�X������Ă��邩�ǂ����j
      rc = DELIVERY_BEACON_RecvSingleCheck( evt_wk->BeaconWork );
      if (rc) (*seq) = BEACON_SEQ_MAIN;
      else
      {   //�z�M���Ă��Ȃ�
        //�󂯎��̌��ʂ��Z�b�g
        *(evt_wk->Ret) = FALSE;   //���s
        (*seq) = BEACON_SEQ_END;
      }
    }
    break;
  case BEACON_SEQ_MAIN:
    //�z�����C��
    DELIVERY_BEACON_Main( evt_wk->BeaconWork );
    //�f�[�^�󂯎��҂�
    if ( DELIVERY_BEACON_RecvCheck( evt_wk->BeaconWork )  )
    {
      BATTLE_EXAMINATION_SAVEDATA *exa;
      GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
      SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gamedata); 
      exa = SaveControl_Extra_DataPtrGet( sv, SAVE_EXTRA_ID_BATTLE_EXAMINATION, 0);
      //�o�b�t�@����Z�[�u�փf�[�^�R�s�[
      GFL_STD_MemCopy( evt_wk->Ptr->CommBuf, exa, BATTLE_EXAMINATION_SAVE_GetWorkSize() );
      NOZOMU_Printf("�f�[�^�󂯎�萬��\n");
      //�󂯎��̌��ʂ��Z�b�g
      *(evt_wk->Ret) = TRUE;    //����
      (*seq) = BEACON_SEQ_END;
    }
    break;
  case BEACON_SEQ_END:
    //�r�[�R���I��
    DELIVERY_BEACON_End( evt_wk->BeaconWork );
    //�I��
    return( GMEVENT_RES_FINISH );
  }
  
  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * �����N�v�Z
 * @param   ptr      TRIAL_HOUSE_WORK_PTR
 * @param   outRank   �Z�o�����N�i�[�o�b�t�@
 * @param   outPoint  ���_�i�[�o�b�t�@
 * @retval  none
 */
//--------------------------------------------------------------
void TRIAL_HOUSE_CalcBtlResult( TRIAL_HOUSE_WORK_PTR ptr, u16 *outRank, u16 *outPoint )
{
  int val;
  u16 rank;
  TH_POINT_WORK *point;
  point = &ptr->PointWork;
  val = 0;
  val += (point->WinTrainerNum * 1000);     //�|�����g���[�i�[�� x 1000
  val += (point->WinPokeNum * 100);         //�|�����|�P������ x 100
  val += (point->PokeChgNum * 5);           //���� x 5
  val += (point->WeakAtcNum * 1);           //�΂���̋Z���o������ x 1
  val += (point->VoidNum * 5);              //���ʂ��Ȃ��Z���󂯂��� x 5
  val += (point->ResistNum * 2);            //���܂ЂƂ̋Z���󂯂��� x 2
  val += (point->UseWazaNum * 1);           //�g�p�����Z�̐� x 1

  val -= (point->TurnNum * 10);              //���������^�[���� x 10
  val -= (point->VoidAtcNum * 10);          //���ʂ��Ȃ��Z���o������ x 10
  val -= (point->ResistAtcNum * 2);         //���܂ЂƂ̋Z���o������ x 2
  val -= (point->LosePokeNum * 100);        //�|���ꂽ�|�P������ x 100
  val -= ( ((500 - point->RestHpPer) * 100) / 500 );     //�c��g�o����(�ő�-100)

  if (val < 0) val = 0;

  if (val >= 6000) rank = TH_RANK_MASTER;
  else if (val >= 5000) rank = TH_RANK_ELITE;
  else if (val >= 4000) rank = TH_RANK_HYPER;
  else if (val >= 3000) rank = TH_RANK_SUPER;
  else if (val >= 2000) rank = TH_RANK_NORMAL;
  else if (val >= 1000) rank = TH_RANK_NOVICE;
  else rank = TH_RANK_BEGINNER;

  *outRank = rank;
  *outPoint = val;
}

//--------------------------------------------------------------
/**
 * �_�E�����[�h�f�[�^�̎�ނ��擾
 * @param   gsys      �Q�[���V�X�e���|�C���^
 * @param   ptr       �g���C�A���n�E�X���[�N�|�C���^
 * @retval  u32       �f�[�^�^�C�v
 */
//--------------------------------------------------------------
u32 TRIAL_HOUSE_GetDLDataType( GAMESYS_WORK *gsys, TRIAL_HOUSE_WORK_PTR ptr )
{
  BOOL rc;
  u32 type;
  u32 monsno;
  BATTLE_EXAMINATION_SAVEDATA *exa;
  BSUBWAY_PARTNER_DATA *data;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gamedata);
  exa = SaveControl_Extra_DataPtrGet( sv, SAVE_EXTRA_ID_BATTLE_EXAMINATION, 0);
  //�f�[�^�̗L�������`�F�b�N
  rc = BATTLE_EXAMINATION_SAVE_IsInData(exa);
  if (!rc) type = TH_DL_DATA_TYPE_NONE;
  else
  {
    //��l�ڂ̃g���[�i�[�̎莝���|�P�����̂S�Ԗڂ��擾
    data = BATTLE_EXAMINATION_SAVE_GetData(exa, 0);
    //3�C�ڎ擾
    monsno = data->btpwd[2].mons_no;
    //3�C�ڂ��Ȃ���΁A�f�[�^����
    if ( (0<monsno)&&(monsno<=MONSNO_END) )
    {
      //4�C�ڎ擾
      monsno = data->btpwd[3].mons_no;
      //�S�C�ڂ��Ȃ���΃V���O���A����΃_�u��
      if ( (0<monsno)&&(monsno<=MONSNO_END) ) type = TH_DL_DATA_TYPE_DOUBLE;
      else type = TH_DL_DATA_TYPE_SINGLE;
    }  
    else type = TH_DL_DATA_TYPE_NONE;
    
  }

  NOZOMU_Printf("DL_data_type = %d\n",type);
  return type;
}

//--------------------------------------------------------------
/**
 * ����f�[�^�𖳌���Ԃɂ��ăZ�[�u����
 * @param   gsys      �Q�[���V�X�e���|�C���^
 * @param   ptr       �g���C�A���n�E�X���[�N�|�C���^
 * @retval  none
 */
//--------------------------------------------------------------
void TRIAL_HOUSE_InvalidDLData( GAMESYS_WORK *gsys, TRIAL_HOUSE_WORK_PTR ptr )
{
  BATTLE_EXAMINATION_SAVEDATA exa_data;
  BSUBWAY_PARTNER_DATA *data;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gamedata);
  int size = BATTLE_EXAMINATION_SAVE_GetWorkSize();

  NOZOMU_Printf("����f�[�^�@%dbyte ���N���A\n",size);

  GFL_STD_MemClear( &exa_data, size );
  BATTLE_EXAMINATION_SAVE_Write(sv, &exa_data, ptr->HeapID);
}







