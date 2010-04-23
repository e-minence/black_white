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
#include "savedata/musical_save.h"
#include "savedata/intrude_save.h"
#include "savedata/my_pms_data.h"
#include "savedata/zukan_savedata.h"
#include "savedata/c_gear_data.h"
#include "gamesystem/game_beacon.h"

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
  int               TrainerView;      ///< �g���[�i�[�J�[�h�J�n���̃��j�I��������

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
  CARD_OR_BADGE=0,
  CARD_INIT,
  CARD_WAIT,
  PMSINPUT_INIT,
  PMSINPUT_WAIT,
  BADGE_INIT,
  BADGE_WAIT,
  CARDSYS_END,
};

static int sub_CardInit(TR_CARD_SYS* wk);
static int sub_CardWait(TR_CARD_SYS* wk);
static int sub_PmsInputInit(TR_CARD_SYS* wk);
static int sub_PmsInputWait(TR_CARD_SYS* wk);
static int sub_BadgeInit(TR_CARD_SYS* wk);
static int sub_BadgeWait(TR_CARD_SYS* wk);


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
  TRAINERCARD_GetSelfData( wk->tcp->TrCardData , pp->gameData , FALSE, wk->tcp->edit_possible, wk->heapId);

  // ���j�I�������ڂ�ۑ�
  wk->TrainerView = wk->tcp->TrCardData->UnionTrNo;
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
  case CARD_OR_BADGE:
    // ���N���A�Ȃ�o�b�W�ցA�a�����肵�Ă���΃J�[�h��
    if(wk->tcp->TrCardData->Clear_m==0 && wk->tcp->edit_possible==1){
      *seq = BADGE_INIT;
    }else{
      *seq = CARD_INIT;
    }
    break;
  case CARD_INIT:
    *seq = sub_CardInit(wk);
    break;
  case CARD_WAIT:
    *seq = sub_CardWait(wk);
    break;
  case PMSINPUT_INIT:
    *seq = sub_PmsInputInit(wk);
    break;
  case PMSINPUT_WAIT:
    *seq = sub_PmsInputWait(wk);
    break;
  case BADGE_INIT:
    *seq = sub_BadgeInit(wk);
    break;
  case BADGE_WAIT:
    *seq = sub_BadgeWait(wk);
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
  TRCARD_CALL_PARAM* param = (TRCARD_CALL_PARAM*)pwk;

  // ���j�I�������ڂ��ύX����Ă����炷��Ⴂ�����X�V����
  if(param->edit_possible)
  {
    MYSTATUS *my     = GAMEDATA_GetMyStatus(param->gameData);
    int trainer_view = MyStatus_GetTrainerView( my );
    if(wk->TrainerView!=trainer_view)
    {
      OS_Printf("����Ⴂ���[������]���X�V����\n");
      GAMEBEACON_SendDataUpdate_TrainerView(trainer_view);
    }
  }
  
  // �I�������������p��
  param->next_proc = wk->tcp->next_proc;

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
  // �v���Z�X��`�f�[�^
  static const GFL_PROC_DATA TrCardProcData = {
    TrCardProc_Init,
    TrCardProc_Main,
    TrCardProc_End,
  };

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
  
  if(wk->tcp->value==CALL_PMSINPUT){
    return PMSINPUT_INIT;
  }else if(wk->tcp->value==CALL_BADGE){
    return BADGE_INIT;
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
static int sub_PmsInputInit(TR_CARD_SYS* wk)
{
  if( wk->procSys == NULL )
  { 
    wk->procSys = GFL_PROC_LOCAL_boot( wk->heapId );
  }
  wk->PmsParam.save_ctrl = GAMEDATA_GetSaveControlWork( wk->tcp->gameData );
  GFL_PROC_LOCAL_CallProc( wk->procSys, FS_OVERLAY_ID(pmsinput), &ProcData_PMSSelect, &wk->PmsParam);
  return PMSINPUT_WAIT;
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
static int sub_PmsInputWait(TR_CARD_SYS* wk)
{
  if(!TrCardSysProcCall(&wk->procSys)){
    return PMSINPUT_WAIT;
  }
  // �쐬�����ȈՉ�b������Ώ�������
  if(wk->PmsParam.out_pms_data!=NULL){
    MYPMS_DATA *p_wk = SaveData_GetMyPmsData(GAMEDATA_GetSaveControlWork(wk->tcp->gameData));
    wk->tcp->TrCardData->Pms = *wk->PmsParam.out_pms_data;
//    TRCSave_SetPmsData( trc_ptr, wk->PmsParam.out_pms_data ); // �Z�[�u�f�[�^�ɂ����f
    MYPMS_SetPms( p_wk, MYPMS_PMS_TYPE_INTRODUCTION, wk->PmsParam.out_pms_data );
    GAMEBEACON_SendDataUpdate_TrCardIntroduction( wk->PmsParam.out_pms_data );
  }
  
  return CARD_INIT;
}

extern const GFL_PROC_DATA BadgeViewProcData;

//----------------------------------------------------------------------------------
/**
 * @brief �ȈՉ�b�Ăяo��
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int sub_BadgeInit(TR_CARD_SYS* wk)
{
  if( wk->procSys == NULL )
  { 
    wk->procSys = GFL_PROC_LOCAL_boot( wk->heapId );
  }
  wk->PmsParam.save_ctrl = GAMEDATA_GetSaveControlWork( wk->tcp->gameData );
  GFL_PROC_LOCAL_CallProc( wk->procSys, FS_OVERLAY_ID(pmsinput), &BadgeViewProcData, wk->tcp );
  return BADGE_WAIT;
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
static int sub_BadgeWait(TR_CARD_SYS* wk)
{
  if(!TrCardSysProcCall(&wk->procSys)){
    return BADGE_WAIT;
  }
  
  // �J�[�h�ɖ߂�Ȃ�J�[�h��������
  if(wk->tcp->value==CALL_CARD){
  return CARD_INIT;
  }

  return CARDSYS_END;
}


//=============================================================================================
/**
 * @brief �����̃g���[�i�[�J�[�h�f�[�^�����W����
 *
 * @param   cardData    �i�[����J�[�h�f�[�^�|�C���^
 * @param   gameData    GAMEDATA
 * @param   isSendData  �ʐM�p�f�[�^���ǂ����iTRUE:�ʐM�f�[�^�j
 * @param   edit        �ҏW�\�f�[�^���ǂ����iTRUE:�ҏW�\�@FALSE:�s�\�j
 * @param   heapId      �q�[�v�h�c
 */
//=============================================================================================
void TRAINERCARD_GetSelfData( TR_CARD_DATA *cardData , GAMEDATA *gameData , const BOOL isSendData, BOOL edit, HEAPID heapId )
{
  u8 i,flag;
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gameData);
  TR_CARD_SV_PTR trc_ptr = TRCSave_GetSaveDataPtr(sv);
  MYSTATUS *mystatus = GAMEDATA_GetMyStatus( gameData );
  MISC *misc = GAMEDATA_GetMiscWork( gameData );
  GMTIME *gameTime = SaveData_GetGameTime( sv );
  RECORD *rec = SaveData_GetRecord( sv );
  const MYPMS_DATA *p_wk = SaveData_GetMyPmsDataConst(sv);
  ZUKAN_SAVEDATA *zukan = GAMEDATA_GetZukanSave( gameData );

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
      cardData->BadgeFlag |= flag;
    }
    flag <<= 1;
  }
  //�ʐM�p���ŕ���
  if( isSendData == TRUE )
  {
    PLAYTIME *playTime = SaveData_GetPlayTime( sv );
    cardData->UnionTrNo = MyStatus_GetTrainerView( mystatus );
    cardData->TimeUpdate = FALSE;
    cardData->PlayTime = NULL;
    cardData->PlayTime_h = PLAYTIME_GetHour( playTime );
    cardData->PlayTime_m = PLAYTIME_GetMinute( playTime );
  }
  else
  {
    cardData->UnionTrNo   = MyStatus_GetTrainerView( mystatus );
    cardData->TimeUpdate  = TRUE;
    cardData->PlayTime    = SaveData_GetPlayTime( sv );
    cardData->Personality = TRCSave_GetPersonarity(  trc_ptr );
    cardData->SignAnimeOn = TRCSave_GetSignAnime(  trc_ptr );
    MYPMS_GetPms( p_wk, MYPMS_PMS_TYPE_INTRODUCTION, &cardData->Pms );

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
  cardData->CommNum = RECORD_Get(rec, RECID_COMM_TRADE)+RECORD_Get(rec, RECID_WIFI_TRADE)+
                      RECORD_Get(rec, RECID_COMM_BATTLE)+RECORD_Get(rec, RECID_WIFI_BATTLE);
                      
  // �ʐM�ΐ��
  cardData->CommBattleNum = RECORD_Get(rec, RECID_COMM_BATTLE) + 
                            RECORD_Get(rec, RECID_WIFI_BATTLE) +
                            RECORD_Get(rec, RECID_IRC_BATTLE);
  //������  ���C�����X+WiFi
  cardData->CommBattleWin = RECORD_Get(rec, RECID_COMM_BTL_WIN)+RECORD_Get(rec, RECID_WIFI_BTL_WIN);
  //������  ���C�����X+WiFi
  cardData->CommBattleLose = RECORD_Get(rec, RECID_COMM_BTL_LOSE)+RECORD_Get(rec, RECID_WIFI_BTL_LOSE);
  //�ʐM����  ���C�����X+WiFi
  cardData->CommTrade = RECORD_Get(rec, RECID_COMM_TRADE)+RECORD_Get(rec, RECID_WIFI_TRADE);

  // �쐶�G���J�E���g��
  cardData->EncountNum        = RECORD_Get(rec, RECID_BTL_ENCOUNT);        
  // �g���[�i�[���������
  cardData->TrainerEncountNum = RECORD_Get(rec, RECID_BTL_TRAINER);

  // ����Ⴂ�ʐM��������
  cardData->SuretigaiNum      = RECORD_Get(rec, RECID_SURECHIGAI_NUM);      
  // �t�B�[�����O�`�F�b�N��������
  cardData->FeelingCheckNum = RECORD_Get(rec, RECID_AFFINITY_CHECK_NUM);   
  // ���m���X���x��
  {
    OCCUPY_INFO *occupy     = GAMEDATA_GetMyOccupyInfo(gameData);
    int level = OccupyInfo_GetWhiteLevel(occupy) + OccupyInfo_GetBlackLevel(occupy);
    cardData->MonolithLevel = level;
  }
  // �~���[�W�J����������
  cardData->MusicalNum        = RECORD_Get( rec, RECID_MUSICAL_PLAY_NUM );     
  // �|�P�V�t�^�[�̃n�C�X�R�A
  {
    cardData->PokeshifterHigh = MISC_GetPalparkHighscore(misc);
  }
  // �g���C�A�E�n�E�X�̍ō����_
  cardData->TrialHouseHigh = RECORD_GetThScore(rec);    
  // �g���C�A���n�E�X�̃����N
  cardData->TrialHouseRank = RECORD_GetThRank(rec);

  // C�M�A�������Ă��邩
  {
    CGEAR_SAVEDATA *cgear = GAMEDATA_GetCGearSaveData( gameData );
    cardData->CgearGetFlag  = CGEAR_SV_GetCGearONOFF(cgear);    
  }
  
  // �p���X�ɍs�����������邩
  {
    INTRUDE_SAVE_WORK *intrude_sv = SaveData_GetIntrude( sv );
    if(ISC_SAVE_GetPalaceSojournTime(intrude_sv)){
      cardData->PaleceFlag      = 1;    
    } 
  }
  // �~���[�W�J���������������邩
  if(cardData->MusicalNum){
    cardData->MusicalFlag   = 1;    
  }

  // �|�P�V�t�^�[�������������邩
  cardData->PokeshifterFlag = 1;    
  // �g���C�A���n�E�X�ɎQ�������������邩
  cardData->TrianHouseFlag  = 1;    
  
  //�}�ӏ���
  cardData->PokeBookFlg = TRUE;
  cardData->PokeBook    = ZUKANSAVE_GetZukanPokeGetCount( zukan, heapId );

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


// �u���b�N�V�e�B�E�z���C�g�t�H���X�g�̃g���[�i�[�J�[�h�����N�A�b�v����
#define OCCUPY_QUALIFY  ( 30 )

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
  BSUBWAY_SCOREDATA *bs_score   = GAMEDATA_GetBSubwayScoreData( gameData );
  MUSICAL_SAVE      *musical_sv = GAMEDATA_GetMusicalSavePtr( gameData );
  OCCUPY_INFO       *occupy_sv  = GAMEDATA_GetMyOccupyInfo( gameData );

  //�X�g�[���[�N���A�i�a������Ƃ͈Ⴄ�j
  if (EVENTWORK_CheckEventFlag(GAMEDATA_GetEventWork( gameData ),SYS_FLAG_GAME_CLEAR)){
    OS_Printf( "�X�g�[���[�N���A\n");
    rank++;
  }
  //�S���}�ӊ����i�C�x���g�n�|�P�����������|�P�������Q�b�g���Ă��邩�j
  if ( ZUKANSAVE_CheckZenkokuComp(GAMEDATA_GetZukanSave(gameData)) ){
    OS_Printf( "�S���}�ӊ���\n");
    rank++;
  }

  // �o�g���T�u�E�F�C�A�X�[�p�[�V���O���A�X�[�p�[�_�u���Ń{�X���j
  if(BSUBWAY_SCOREDATA_SetFlag( bs_score,  BSWAY_SCOREDATA_FLAG_BOSS_CLEAR_S_SINGLE, BSWAY_SETMODE_get) 
  && BSUBWAY_SCOREDATA_SetFlag( bs_score,  BSWAY_SCOREDATA_FLAG_BOSS_CLEAR_S_DOUBLE, BSWAY_SETMODE_get) 
//  && BSUBWAY_SCOREDATA_SetFlag( bs_score,  BSWAY_SCOREDATA_FLAG_BOSS_CLEAR_S_MULTI, BSWAY_SETMODE_get)    // �K�v�Ȃ�
  ){
    OS_Printf( "�o�g���T�u�E�F�C�{�X���j\n");
    rank++;
  }

  // �~���[�W�J���O�b�Y���R���v���[�g���Ă��邩
  if( MUSICAL_SAVE_IsCompleteItem( musical_sv ) ){
    OS_Printf( "�~���[�W�J���O�b�Y�R���v\n");
    rank++;
  }
  
  // �u���b�N�V�e�B�E�z���C�g�t�H���X�g�̃o�����X���l�����ɂႤ�ɂ�
  if(OccupyInfo_GetWhiteLevel(occupy_sv)>=OCCUPY_QUALIFY
  && OccupyInfo_GetBlackLevel(occupy_sv)>=OCCUPY_QUALIFY
  ){
    OS_Printf( "BC/WF�̃o�����X���l���������R�O�ȏ�\n");
    rank++;
  }

  return rank;
}