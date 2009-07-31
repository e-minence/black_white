/**
 *  @file trcard_sys.c
 *  @brief  �g���[�i�[�J�[�h�V�X�e��
 *  @author Miyuki Iwasawa
 *  @date 08.01.16
 */

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "savedata/config.h"
#include "savedata/trainercard_data.h"
#include "savedata/mystatus.h"
#include "savedata/gametime.h"
#include "savedata/record.h"

#include "app/mysign.h"
#include "app/trainer_card.h"
#include "trcard_sys.h"

#include "test/ariizumi/ari_debug.h"


typedef struct _TR_CARD_SYS{
  int heapId;

  void  *app_wk;  ///<�ȈՉ�b���W���[�����[�N�̕ۑ�
  GFL_PROCSYS*  procSys;    ///<�T�u�v���Z�X���[�N

  TRCARD_CALL_PARAM* tcp;

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
    if (GFL_PROC_LOCAL_Main(*procSys) == FALSE ) {
      GFL_PROC_LOCAL_Exit(*procSys);
      *procSys = NULL;
      return TRUE;
    }
  }
  return FALSE;
}

/**
 *  @brief  �g���[�i�[�J�[�h�V�X�e���Ăяo���@������
 */
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
  wk->tcp = pp;
  wk->tcp->TrCardData = GFL_HEAP_AllocClearMemory( wk->heapId , sizeof( TR_CARD_DATA ) );
  TRAINERCARD_GetSelfData( wk->tcp->TrCardData , pp->gameData , FALSE);

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


/**
 *  @brief  �g���[�i�[�J�[�h�@�V�X�e�����C��
 */
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

/**
 *  @brief  �g���[�i�[�J�[�h�@�V�X�e���I��
 */
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
GFL_PROC_RESULT TrCardSysProc_EndComm( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  TR_CARD_SYS* wk = (TR_CARD_SYS*)mywk;

  GFL_HEAP_FreeMemory( wk->tcp );

  //���[�N�G���A���
  GFL_PROC_FreeWork(proc);
  
  GFL_HEAP_DeleteHeap(HEAPID_TRCARD_SYS);
  return GFL_PROC_RES_FINISH;
}

/**
 *  @brief  �g���[�i�[�J�[�h�Ăяo��
 */
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
  wk->procSys = GFL_PROC_LOCAL_boot( wk->heapId );
  GFL_PROC_LOCAL_CallProc( wk->procSys, NO_OVERLAY_ID, &TrCardProcData,(void*)wk->tcp);
  return CARD_WAIT;
}

/**
 *  @brief  �g���[�i�[�J�[�h�҂�
 */
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

/**
 *  @brief  �T�C���Ăяo��
 */
static int sub_SignInit(TR_CARD_SYS* wk)
{
//  FS_EXTERN_OVERLAY(mysign);
  // �v���Z�X��`�f�[�^
  static const GFL_PROC_DATA MySignProcData = {
    MySignProc_Init,
    MySignProc_Main,
    MySignProc_End,
  };
    
//  wk->proc = PROC_Create(&MySignProcData,(void*)wk->tcp->savedata,wk->heapId);
  wk->procSys = GFL_PROC_LOCAL_boot( wk->heapId );
  GFL_PROC_LOCAL_CallProc( wk->procSys, NO_OVERLAY_ID, &MySignProcData,wk->tcp);
  return SIGN_WAIT;
}

/**
 *  @brief  �T�C���҂�
 */
static int sub_SignWait(TR_CARD_SYS* wk)
{
  if(!TrCardSysProcCall(&wk->procSys)){
    return SIGN_WAIT;
  }
  //�T�C���f�[�^���Ăяo���e���|�����ɏ����߂�
  {
    TR_CARD_SV_PTR trc_ptr = TRCSave_GetSaveDataPtr(SaveControl_GetPointer());
    //�T�C���f�[�^�̗L��/�����t���O���擾(���⃍�[�J���ł̂ݗL��)
    wk->tcp->TrCardData->MySignValid = TRCSave_GetSigned(trc_ptr);
    //�T�C���f�[�^���Z�[�u�f�[�^����R�s�[
    MI_CpuCopy8(TRCSave_GetSignDataPtr(trc_ptr),
        wk->tcp->TrCardData->SignRawData, SIGN_SIZE_X*SIGN_SIZE_Y*8 );
  }
  return CARD_INIT;
}

void TRAINERCARD_GetSelfData( TR_CARD_DATA *cardData , GAMEDATA *gameData , const BOOL isSendData )
{
  u8 i,flag;
  TR_CARD_SV_PTR trc_ptr = TRCSave_GetSaveDataPtr(GAMEDATA_GetSaveControlWork(gameData));
  MYSTATUS *mystatus = GAMEDATA_GetMyStatus( gameData );
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
  
  cardData->TrSex = MyStatus_GetMySex( mystatus );
  cardData->TrainerID = MyStatus_GetID( mystatus );
  cardData->Money = MyStatus_GetGold( mystatus );
  cardData->BadgeFlag = 0;
  //FIXME �������o�b�a���̒�`��(8��)
  flag = 1;
  for( i=0; i<8; i++ )
  {
    if( MyStatus_GetBadgeFlag( mystatus , i ) == TRUE )
    {
      cardData->BadgeFlag &= flag;
    }
    flag <<= 1;
  }
  //�ʐM�p���ŕ���
  if( isSendData == TRUE )
  {
    PLAYTIME *playTime = SaveData_GetPlayTime( SaveControl_GetPointer() );
    cardData->UnionTrNo = MyStatus_GetTrainerView( mystatus );
    cardData->TimeUpdate = FALSE;
    cardData->PlayTime = NULL;
    cardData->PlayTime_h = PLAYTIME_GetHour( playTime );
    cardData->PlayTime_m = PLAYTIME_GetMinute( playTime );
  }
  else
  {
    cardData->UnionTrNo = UNION_TR_NONE;
    cardData->TimeUpdate = TRUE;
    cardData->PlayTime = SaveData_GetPlayTime( SaveControl_GetPointer() );
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
  //������  ���C�����X+WiFi
  cardData->CommBattleWin = RECORD_Get(rec, RECID_COMM_BTL_WIN)+RECORD_Get(rec, RECID_WIFI_BTL_WIN);
  //������  ���C�����X+WiFi
  cardData->CommBattleLose = RECORD_Get(rec, RECID_COMM_BTL_LOSE)+RECORD_Get(rec, RECID_WIFI_BTL_LOSE);
  //�ʐM����  ���C�����X+WiFi
  cardData->CommTrade = RECORD_Get(rec, RECID_COMM_TRADE)+RECORD_Get(rec, RECID_WIFI_TRADE);
  
  //�X�R�A
  cardData->Score = RECORD_Score_Get( rec );
  
  //FIXME �}�ӏ���
  cardData->PokeBookFlg = FALSE;

  //�T�C���f�[�^�̎擾
  //�T�C���f�[�^�̗L��/�����t���O���擾(���⃍�[�J���ł̂ݗL��)
  cardData->MySignValid = TRCSave_GetSigned(trc_ptr);
  //�T�C���f�[�^���Z�[�u�f�[�^����R�s�[
  MI_CpuCopy8(TRCSave_GetSignDataPtr(trc_ptr),
      cardData->SignRawData, SIGN_SIZE_X*SIGN_SIZE_Y*8 );
}
//CALL_PARAM�̍쐬
TRCARD_CALL_PARAM* TRAINERCASR_CreateCallParam_SelfData( GAMEDATA *gameData , HEAPID heapId )
{
  TRCARD_CALL_PARAM* callParam;
  callParam = GFL_HEAP_AllocMemory( heapId , sizeof( TRCARD_CALL_PARAM ) );
  callParam->TrCardData = NULL;
  callParam->gameData = gameData;
  callParam->value = 0;
  
  return callParam;
}

//�ʐM����CALL_PARAM�̍쐬
TRCARD_CALL_PARAM*  TRAINERCASR_CreateCallParam_CommData( GAMEDATA *gameData , void* pCardData , HEAPID heapId )
{
  TRCARD_CALL_PARAM* callParam;
  
  callParam = GFL_HEAP_AllocMemory( heapId , sizeof( TRCARD_CALL_PARAM ) );
  callParam->TrCardData = pCardData;
  callParam->gameData = gameData;
  callParam->value = 0;

  return callParam;
}
