/**
 *  @file trcard_sys.c
 *  @brief  �g���[�i�[�J�[�h�V�X�e��
 *  @author Miyuki Iwasawa
 *  @date 08.01.16
 *
 *  @todo
 *    ����ł̓g���[�i�[�J�[�h�̓T�C�������A�v�����Ăяo���Ă������AWB�ł͒��ڃJ�[�h����
 *    �T�C����������悤�ɂȂ��Ă���Bmysign�A�v�������ŗ��p����Ȃ��̂ł���΍폜���Ă悢
 */

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "gamesystem/game_data.h"
#include "savedata/config.h"
#include "savedata/trainercard_data.h"
#include "savedata/mystatus.h"
#include "savedata/gametime.h"
#include "system/pms_data.h"
#include "app/pms_select.h"
#include "savedata/record.h"
#include "savedata/bsubway_savedata.h"
#include "savedata/zukan_savedata.h"
#include "field/eventwork.h"
#include "field/eventwork_def.h"


//#include "app/mysign.h"
#include "app/trainer_card.h"
#include "trcard_sys.h"

#include "test/ariizumi/ari_debug.h"


typedef struct _TR_CARD_SYS{
  int heapId;

  void              *app_wk;          ///< �ȈՉ�b���W���[�����[�N�̕ۑ�
  GFL_PROCSYS       *procSys;         ///< �T�u�v���Z�X���[�N
  PMS_SELECT_PARAM  PmsParam;         ///< �ȈՉ�b�A�v���Ăяo���p���[�N

  TRCARD_CALL_PARAM *tcp;

}TR_CARD_SYS;

//�v���g�^�C�v�錾�@���[�J��
//================================================================
///�I�[�o�[���C�v���Z�X
GFL_PROC_RESULT TrCardSysProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
GFL_PROC_RESULT TrCardSysProc_InitComm( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
GFL_PROC_RESULT TrCardSysProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
GFL_PROC_RESULT TrCardSysProc_End( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
GFL_PROC_RESULT TrCardSysProc_EndComm( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
const GFL_PROC_DATA TrCardSysProcData = {
  TrCardSysProc_Init,
  TrCardSysProc_Main,
  TrCardSysProc_End,
};
//�������E�J�����Ⴄ�ʐM�pProc�f�[�^
//�g���[�i�[�J�[�h�f�[�^���O������󂯎��̂ŊJ�����Ȃ�
const GFL_PROC_DATA TrCardSysCommProcData = {
  TrCardSysProc_InitComm,
  TrCardSysProc_Main,
  TrCardSysProc_EndComm,
};

//================================================================
///�f�[�^��`�G���A
//================================================================
///�I�[�o�[���C�v���Z�X��`�f�[�^
enum{
  CARD_INIT,
  CARD_WAIT,
  SIGN_INIT,
  SIGN_WAIT,
  CARDSYS_END,
};

static int sub_CardInit(TR_CARD_SYS* wk);
static int sub_CardWait(TR_CARD_SYS* wk);
static int sub_SignInit(TR_CARD_SYS* wk);
static int sub_SignWait(TR_CARD_SYS* wk);

//------------------------------------------------------------------
/**
 * @brief �T�u�v���Z�X�Ăяo������
 * @param proc  �T�u�v���Z�X�|�C���^��ێ����郏�[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
static BOOL TrCardSysProcCall(GFL_PROCSYS ** procSys)
{
  if (*procSys) {
    if (GFL_PROC_LOCAL_Main(*procSys) == GFL_PROC_MAIN_NULL ) {
      GFL_PROC_LOCAL_Exit(*procSys);
      *procSys = NULL;
      return TRUE;
    }
  }
  return FALSE;
}


//=============================================================================================
/**
 * @brief  �g���[�i�[�J�[�h�V�X�e���Ăяo���@������
 *
 * @param   proc    
 * @param   seq   
 * @param   pwk   
 * @param   mywk    
 *
 * @retval  GFL_PROC_RESULT   
 */
//=============================================================================================
GFL_PROC_RESULT TrCardSysProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  TR_CARD_SYS* wk = NULL;
  TRCARD_CALL_PARAM* pp = (TRCARD_CALL_PARAM*)pwk;
  
  //�q�[�v�쐬
  GFL_HEAP_CreateHeap(GFL_HEAPID_APP,HEAPID_TRCARD_SYS,0x18000);
  wk = GFL_PROC_AllocWork(proc,sizeof(TR_CARD_SYS),HEAPID_TRCARD_SYS);
  MI_CpuClear8(wk,sizeof(TR_CARD_SYS));

  //�q�[�vID�ۑ�
  wk->heapId = HEAPID_TRCARD_SYS;

  //�f�[�^�e���|�����쐬
  wk->tcp = GFL_HEAP_AllocClearMemory( wk->heapId , sizeof( TRCARD_CALL_PARAM ) );
  *wk->tcp = *pp;
  wk->tcp->TrCardData = GFL_HEAP_AllocClearMemory( wk->heapId , sizeof( TR_CARD_DATA ) );
  TRAINERCARD_GetSelfData( wk->tcp->TrCardData , pp->gameData , FALSE, wk->tcp->edit_possible);

  return GFL_PROC_RES_FINISH;
}

GFL_PROC_RESULT TrCardSysProc_InitComm( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  TR_CARD_SYS* wk = NULL;
  TRCARD_CALL_PARAM* callParam = (TRCARD_CALL_PARAM*)pwk;
  
  //�q�[�v�쐬
  GFL_HEAP_CreateHeap(GFL_HEAPID_APP,HEAPID_TRCARD_SYS,0x18000);
  wk = GFL_PROC_AllocWork(proc,sizeof(TR_CARD_SYS),HEAPID_TRCARD_SYS);
  MI_CpuClear8(wk,sizeof(TR_CARD_SYS));

  //�q�[�vID�ۑ�
  wk->heapId = HEAPID_TRCARD_SYS;

  //�f�[�^�e���|�����쐬
  wk->tcp = callParam;

  return GFL_PROC_RES_FINISH;
}


//=============================================================================================
/**
 * @brief  �g���[�i�[�J�[�h�@�V�X�e�����C��
 *
 * @param   proc    
 * @param   seq   
 * @param   pwk   
 * @param   mywk    
 *
 * @retval  GFL_PROC_RESULT   
 */
//=============================================================================================
GFL_PROC_RESULT TrCardSysProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  TR_CARD_SYS* wk = (TR_CARD_SYS*)mywk;
  switch(*seq){
  case CARD_INIT:
    *seq = sub_CardInit(wk);
    break;
  case CARD_WAIT:
    *seq = sub_CardWait(wk);
    break;
  case SIGN_INIT:
    *seq = sub_SignInit(wk);
    break;
  case SIGN_WAIT:
    *seq = sub_SignWait(wk);
    break;
  case CARDSYS_END:
  default:
    return GFL_PROC_RES_FINISH;
  }
  return GFL_PROC_RES_CONTINUE;
}


//=============================================================================================
/**
 * @brief �g���[�i�[�J�[�h�@�V�X�e���I��
 *
 * @param   proc    
 * @param   seq   
 * @param   pwk   
 * @param   mywk    
 *
 * @retval  GFL_PROC_RESULT   
 */
//=============================================================================================
GFL_PROC_RESULT TrCardSysProc_End( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  TR_CARD_SYS* wk = (TR_CARD_SYS*)mywk;

  GFL_HEAP_FreeMemory( wk->tcp->TrCardData );
  GFL_HEAP_FreeMemory( wk->tcp );

  //���[�N�G���A���
  GFL_PROC_FreeWork(proc);
  
  GFL_HEAP_DeleteHeap(HEAPID_TRCARD_SYS);
  return GFL_PROC_RES_FINISH;
}


//=============================================================================================
/**
 * @brief �ʐM�g���[�i�[�J�[�h�{���I����
 *
 * @param   proc    
 * @param   seq   
 * @param   pwk   
 * @param   mywk    
 *
 * @retval  GFL_PROC_RESULT   
 */
//=============================================================================================
GFL_PROC_RESULT TrCardSysProc_EndComm( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  TR_CARD_SYS* wk = (TR_CARD_SYS*)mywk;

  GFL_HEAP_FreeMemory( wk->tcp );

  //���[�N�G���A���
  GFL_PROC_FreeWork(proc);
  
  GFL_HEAP_DeleteHeap(HEAPID_TRCARD_SYS);
  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------------
/**
 * @brief �g���[�i�[�J�[�h�Ăяo��
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int sub_CardInit(TR_CARD_SYS* wk)
{
  // �I�[�o�[���CID�錾
//  FS_EXTERN_OVERLAY(trainer_card);

  // �v���Z�X��`�f�[�^
  static const GFL_PROC_DATA TrCardProcData = {
    TrCardProc_Init,
    TrCardProc_Main,
    TrCardProc_End,
  };
//  wk->proc = PROC_Create(&TrCardProcData,wk->tcp,wk->heapId);
  if( wk->procSys == NULL )
  { 
    wk->procSys = GFL_PROC_LOCAL_boot( wk->heapId );
  }
  GFL_PROC_LOCAL_CallProc( wk->procSys, NO_OVERLAY_ID, &TrCardProcData,(void*)wk->tcp);
  return CARD_WAIT;
}


//----------------------------------------------------------------------------------
/**
 * @brief �g���[�i�[�J�[�h�I���҂�
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int sub_CardWait(TR_CARD_SYS* wk)
{
  if(!TrCardSysProcCall(&wk->procSys)){
    return CARD_WAIT;
  }
  
  if(wk->tcp->value){
    return SIGN_INIT;
  }
  return CARDSYS_END;
}

FS_EXTERN_OVERLAY(pmsinput);
//----------------------------------------------------------------------------------
/**
 * @brief �ȈՉ�b�Ăяo��
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int sub_SignInit(TR_CARD_SYS* wk)
{
//  FS_EXTERN_OVERLAY(mysign);
  // �v���Z�X��`�f�[�^
//  static const GFL_PROC_DATA MySignProcData = {
//    MySignProc_Init,
//    MySignProc_Main,
//    MySignProc_End,
//  };
    
//  wk->proc = PROC_Create(&MySignProcData,(void*)wk->tcp->savedata,wk->heapId);
  if( wk->procSys == NULL )
  { 
    wk->procSys = GFL_PROC_LOCAL_boot( wk->heapId );
  }
//  GFL_PROC_LOCAL_CallProc( wk->procSys, NO_OVERLAY_ID, &MySignProcData,wk->tcp);
  wk->PmsParam.save_ctrl = GAMEDATA_GetSaveControlWork( wk->tcp->gameData );
  GFL_PROC_LOCAL_CallProc( wk->procSys, FS_OVERLAY_ID(pmsinput), &ProcData_PMSSelect, &wk->PmsParam );
  return SIGN_WAIT;
}

//----------------------------------------------------------------------------------
/**
 * @brief �ȈՉ�b�I���҂�
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int sub_SignWait(TR_CARD_SYS* wk)
{
  if(!TrCardSysProcCall(&wk->procSys)){
    return SIGN_WAIT;
  }
  
  // �쐬�����ȈՉ�b������Ώ�������
  if(wk->PmsParam.out_pms_data!=NULL){
    wk->tcp->TrCardData->Pms = *wk->PmsParam.out_pms_data;
  }

  return CARD_INIT;
}

//=============================================================================================
/**
 * @brief �����̃g���[�i�[�J�[�h�f�[�^�����W����
 *
 * @param   cardData    
 * @param   gameData    
 * @param   isSendData    
 */
//=============================================================================================
void TRAINERCARD_GetSelfData( TR_CARD_DATA *cardData , GAMEDATA *gameData , const BOOL isSendData, BOOL edit )
{
  u8 i,flag;
  TR_CARD_SV_PTR trc_ptr = TRCSave_GetSaveDataPtr(GAMEDATA_GetSaveControlWork(gameData));
  MYSTATUS *mystatus = GAMEDATA_GetMyStatus( gameData );
  MISC *misc = GAMEDATA_GetMiscWork( gameData );
  GMTIME *gameTime = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork(gameData) );
  RECORD *rec = SaveData_GetRecord( GAMEDATA_GetSaveControlWork(gameData) );

  //���O
  if( MyStatus_CheckNameClear( mystatus ) == FALSE )
  {
    const STRCODE *baseName = MyStatus_GetMyName( mystatus );
    GFL_STD_MemCopy16( baseName , cardData->TrainerName , sizeof(STRCODE)*(PERSON_NAME_SIZE+EOM_SIZE) );
  }
  else
  {
    //TODO �O�̂��ߖ��O�������ĂȂ��Ƃ��ɗ����Ȃ��悤�ɂ��Ă���
    cardData->TrainerName[0] = L'N';
    cardData->TrainerName[1] = L'o';
    cardData->TrainerName[2] = L'N';
    cardData->TrainerName[3] = L'a';
    cardData->TrainerName[4] = L'm';
    cardData->TrainerName[5] = L'e';
    cardData->TrainerName[6] = GFL_STR_GetEOMCode();
  }
  
  // �J�[�h�����N�擾
  cardData->CardRank = TRAINERCARD_GetCardRank( gameData );

  cardData->TrSex     = MyStatus_GetMySex( mystatus );
  cardData->TrainerID = MyStatus_GetID( mystatus );
  cardData->Money     = MISC_GetGold( misc );
  cardData->BadgeFlag = 0;
  cardData->Version   = PM_VERSION;

  //FIXME �������o�b�a���̒�`��(8��)
  flag = 1;
  for( i=0; i<8; i++ )
  {
    if( MISC_GetBadgeFlag( misc , i ) == TRUE )
    {
      cardData->BadgeFlag &= flag;
    }
    flag <<= 1;
  }
  //�ʐM�p���ŕ���
  if( isSendData == TRUE )
  {
    PLAYTIME *playTime = SaveData_GetPlayTime( GAMEDATA_GetSaveControlWork(gameData) );
    cardData->UnionTrNo = MyStatus_GetTrainerView( mystatus );
    cardData->TimeUpdate = FALSE;
    cardData->PlayTime = NULL;
    cardData->PlayTime_h = PLAYTIME_GetHour( playTime );
    cardData->PlayTime_m = PLAYTIME_GetMinute( playTime );
  }
  else
  {
    cardData->UnionTrNo = MyStatus_GetTrainerView( mystatus );
    cardData->TimeUpdate = TRUE;
    cardData->PlayTime = SaveData_GetPlayTime( GAMEDATA_GetSaveControlWork(gameData) );
    cardData->Personality = TRCSave_GetPersonarity(  trc_ptr );

  }
  //�N���A�����ƃv���C�J�n����
  {
    RTCDate workDate;
    RTCTime workTime;
    
    RTC_ConvertSecondToDateTime( &workDate , &workTime , gameTime->start_sec );
    cardData->Start_y = workDate.year;
    cardData->Start_m = workDate.month;
    cardData->Start_d = workDate.day;

    if( gameTime->clear_sec == 0 )
    {
      //���N���A
      cardData->Clear_y = 0;
      cardData->Clear_m = 0;
      cardData->Clear_d = 0;
      cardData->ClearTime_h = 0;
      cardData->ClearTime_m = 0;
    }
    else
    {
      RTC_ConvertSecondToDateTime( &workDate , &workTime , gameTime->clear_sec );
      cardData->Clear_y = workDate.year;
      cardData->Clear_m = workDate.month;
      cardData->Clear_d = workDate.day;
      cardData->ClearTime_h = workTime.hour;
      cardData->ClearTime_m = workTime.minute;
    }
  }
  //�ʐM��  ���C�����X�R���e�X�g+���C�����X����+WiFi����+���C�����X�ΐ�+WiFi�ΐ�+���C�����X�|���g
  cardData->CommNum = RECORD_Get(rec, RECID_CONTEST_COMM_ENTRY)+
            RECORD_Get(rec, RECID_COMM_TRADE)+RECORD_Get(rec, RECID_WIFI_TRADE)+
            RECORD_Get(rec, RECID_COMM_BATTLE)+RECORD_Get(rec, RECID_WIFI_BATTLE)+
            RECORD_Get(rec, RECID_PORUTO_COMM);
  // �ʐM�ΐ��
  cardData->CommBattleNum = RECORD_Get(rec, RECID_BATTLE_COUNT);
  //������  ���C�����X+WiFi
  cardData->CommBattleWin = RECORD_Get(rec, RECID_COMM_BTL_WIN)+RECORD_Get(rec, RECID_WIFI_BTL_WIN);
  //������  ���C�����X+WiFi
  cardData->CommBattleLose = RECORD_Get(rec, RECID_COMM_BTL_LOSE)+RECORD_Get(rec, RECID_WIFI_BTL_LOSE);
  //�ʐM����  ���C�����X+WiFi
  cardData->CommTrade = RECORD_Get(rec, RECID_COMM_TRADE)+RECORD_Get(rec, RECID_WIFI_TRADE);
  
  //�X�R�A
  cardData->Score = RECORD_Score_Get( rec );
  
  //FIXME �}�ӏ���
  cardData->PokeBookFlg = TRUE;

  //�T�C���f�[�^�̎擾
  //�T�C���f�[�^�̗L��/�����t���O���擾(���⃍�[�J���ł̂ݗL��)
  cardData->MySignValid = TRCSave_GetSigned(trc_ptr);

  //�T�C���f�[�^���Z�[�u�f�[�^����R�s�[
  MI_CpuCopy8(TRCSave_GetSignDataPtr(trc_ptr),
      cardData->SignRawData, SIGN_SIZE_X*SIGN_SIZE_Y*8 );

  // �ҏW�\�t���O�̊i�[
  cardData->EditPossible = edit;

}

//=============================================================================================
/**
 * @brief CALL_PARAM�̍쐬
 *
 * @param   gameData    
 * @param   heapId    
 * @param   edit      �ҏW�\��(TRUE:OK FALSE:NG)
 *
 * @retval  TRCARD_CALL_PARAM*    
 */
//=============================================================================================
TRCARD_CALL_PARAM* TRAINERCASR_CreateCallParam_SelfData( GAMEDATA *gameData , HEAPID heapId, BOOL edit )
{
  TRCARD_CALL_PARAM* callParam;
  callParam = GFL_HEAP_AllocClearMemory( heapId , sizeof( TRCARD_CALL_PARAM ) );
  GFL_STD_MemClear( callParam, sizeof(TRCARD_CALL_PARAM) );
  callParam->TrCardData    = NULL;
  callParam->gameData      = gameData;
  callParam->value         = 0;
  callParam->edit_possible = edit;
  
  return callParam;
}

//=============================================================================================
/**
 * @brief �ʐM����CALL_PARAM�̍쐬
 *
 * @param   gameData    
 * @param   pCardData   
 * @param   heapId    
 *
 * @retval  TRCARD_CALL_PARAM*    
 */
//=============================================================================================
TRCARD_CALL_PARAM*  TRAINERCASR_CreateCallParam_CommData( GAMEDATA *gameData , void* pCardData , HEAPID heapId )
{
  TRCARD_CALL_PARAM* callParam;
  
  callParam = GFL_HEAP_AllocClearMemory( heapId , sizeof( TRCARD_CALL_PARAM ) );
  callParam->TrCardData = pCardData;
  callParam->gameData = gameData;
  callParam->value = 0;
  callParam->TrCardData->EditPossible = FALSE;   // �K���ҏW�s�\
  callParam->TrCardData->OtherTrCard  = TRUE;

  return callParam;
}


// �o�g���T�u�E�F�C�Ń����N�A�b�v�ɂȂ����(�V���O��49�A���j
#define BSUBWAY_BADGE_TERMS ( 49 )

//=============================================================================================
/**
 * @brief �g���[�i�[�J�[�h�̃����N���擾����
 *
 * @param   gameData    
 *
 * @retval  int   
 */
//=============================================================================================
int TRAINERCARD_GetCardRank( GAMEDATA *gameData )
{
  int rank=0;
  //�a������i�N���A�t���O�j
  if (EVENTWORK_CheckEventFlag(GAMEDATA_GetEventWork( gameData ),SYS_FLAG_GAME_CLEAR)){
    rank++;
  }
  //�S���}�ӊ����i�C�x���g�n�|�P�����������|�P�������Q�b�g���Ă��邩�j
  if ( ZUKANSAVE_CheckZenkokuComp(GAMEDATA_GetZukanSave(gameData)) ){
       
    rank++;
  }

#if 0
  // �o�g���T�u�E�F�C��49�A��
  bs_save = SaveControl_DataPtrGet( save, GMDATA_ID_BSUBWAY_SCOREDATA );    /// @@TODO �܂��o�g���T�u�E�F�C�̘A��������
  if(BSUBWAY_SCOREDATA_GetMaxRenshouCount( bs_save, BSWAY_PLAYMODE_SINGLE) > BSUBWAY_BADGE_TERMS){
    rank++;
  }
#endif

  return rank;
}