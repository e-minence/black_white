//======================================================================
/**
 * @file bsubway_scr.c
 * @brief �o�g���T�u�E�F�C�@�X�N���v�g�֘A
 * @authaor kagaya
 * @date 2008.12.11
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "savedata/save_tbl.h"
#include "savedata/record.h"

#include "system/main.h"  //HEAPID_PROC

#include "poke_tool/poke_tool.h"
#include "app/pokelist.h"

#include "savedata/bsubway_savedata.h"
#include "savedata/bsubway_savedata_def.h"
#include "battle/bsubway_battle_data.h"
#include "bsubway_scr_def.h"
#include "bsubway_scr.h"
#include "bsubway_scr_common.h"
#include "bsubway_tr.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================
static u8 bswScr_GetMemberNum( u16 mode );

static BOOL is_trainer_conflict( u16* trainer,u16 id,u16 num )
{
  u16 i;
  for( i = 0;i < num;i++){
    if( trainer[i] == id ){
      return TRUE;
    }
  }
  return FALSE;
}

//======================================================================
//  �o�g���T�u�E�F�C�@�X�N���v�g���[�N�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * BSUBWAY_SCRWORK ���[�N�|�C���^���N���A
 * @param bsw_scr BSUBWAY_SCRWORK*
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_ClearWork( GAMESYS_WORK *gsys )
{
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  GAMEDATA_SetBSubwayScrWork( gdata, NULL );
}

//--------------------------------------------------------------
/**
 * BSUBWAY_SCRWORK ���[�N�G���A���擾���ď���������
 * @param gsys GAMESYS_WORK*
 * @param init ���������[�h BSWAY_PLAY_NEW:�n�߁ABSWAY_PLAY_CONTINE:����
 * @param playmode �v���C���[�h�w��:BSWAY_MODE_�`*
 * @retval BSUBWAY_SCRWORK*
 */
//--------------------------------------------------------------
BSUBWAY_SCRWORK * BSUBWAY_SCRWORK_CreateWork(
    GAMESYS_WORK *gsys, u16 init, u16 playmode )
{
  int i;
  BSUBWAY_SCRWORK *bsw_scr;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  MYSTATUS *mystatus = GAMEDATA_GetMyStatus( gdata );
  SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( gdata );
  
  bsw_scr = GFL_HEAP_AllocClearMemory(
      HEAPID_PROC, sizeof(BSUBWAY_SCRWORK) );
  
  bsw_scr->heapID = HEAPID_PROC;
  bsw_scr->magicNo = BSUBWAY_SCRWORK_MAGIC;
  
  bsw_scr->playData = SaveControl_DataPtrGet(
      save, GMDATA_ID_BSUBWAY_PLAYDATA );
  bsw_scr->scoreData = SaveControl_DataPtrGet(
      save, GMDATA_ID_BSUBWAY_SCOREDATA );
  
  BSUBWAY_PLAYDATA_SetSaveFlag( bsw_scr->playData, FALSE ); //�Z�[�u�Ȃ���
  
  GF_ASSERT( GAMEDATA_GetBSubwayScrWork(gdata) == NULL );
  GAMEDATA_SetBSubwayScrWork( gdata, bsw_scr );
  
  if( init == BSWAY_PLAY_NEW ){ //�V�K
    u8 buf8;
    
    bsw_scr->play_mode = playmode;
    bsw_scr->member_num = bswScr_GetMemberNum( bsw_scr->play_mode );
    bsw_scr->now_round = 1;
    bsw_scr->now_win = 0;
    
    for(i = 0;i < BSUBWAY_STOCK_MEMBER_MAX;i++){
      bsw_scr->member[i] = BSWAY_NULL_POKE;
    }
    
    for(i = 0;i < BSUBWAY_STOCK_TRAINER_MAX;i++){
      bsw_scr->trainer[i] = BSWAY_NULL_TRAINER;
    }
    
    //�v���C�f�[�^������
    BSUBWAY_PLAYDATA_Init( bsw_scr->playData );
    buf8 = bsw_scr->play_mode; //�v���C���[�h�����͂��̎��_�ŃZ�[�u���f
    BSUBWAY_PLAYDATA_SetData(
        bsw_scr->playData, BSWAY_PLAYDATA_ID_playmode, &buf8 );
    
    /* //wb null
    #ifdef BTOWER_AUTO_DEB
    if(wk->play_mode == BSWAY_MODE_WIFI){
      OS_Printf(" #WifiTowerStage=%d\n",DebugBTowerAutoStage++);
    }
    #endif
    */
  }else{ //�����@�f�[�^���[�h
    bsw_scr->play_mode = (u8)BSUBWAY_PLAYDATA_GetData(
        bsw_scr->playData, BSWAY_PLAYDATA_ID_playmode, NULL );
    bsw_scr->now_round = (u8)BSUBWAY_PLAYDATA_GetData(
        bsw_scr->playData, BSWAY_PLAYDATA_ID_round, NULL );
    bsw_scr->now_win = bsw_scr->now_round-1;
    
    bsw_scr->member_num = bswScr_GetMemberNum( bsw_scr->play_mode );
    
    //�I�΂�Ă���|�P����No
    BSUBWAY_PLAYDATA_GetData( bsw_scr->playData,
        BSWAY_PLAYDATA_ID_pokeno, bsw_scr->member );
    
    //���I�ς݂̃g���[�i�[No
    BSUBWAY_PLAYDATA_GetData( bsw_scr->playData,
        BSWAY_PLAYDATA_ID_trainer, bsw_scr->trainer);
    
    #if 0 //wb null
    //�v���C�����_���V�[�h�擾
    wk->play_rnd_seed = TowerPlayData_Get(
        wk->playSave,BSWAY_PSD_rnd_seed,NULL);
    OS_Printf("TowerContinueRndSeed = %d\n",wk->play_rnd_seed);
    #endif
    
    #if 0 //wb null
    if( bsw_scr->play_mode == BSWAY_MODE_MULTI ){
      wk->partner = (u8)TowerPlayData_Get(
          wk->playSave,BSWAY_PSD_partner,NULL);

      //�p�[�g�i�[�g���[�i�[�f�[�^�Đ���
      TowerPlayData_Get(wk->playSave,
          BSWAY_PSD_pare_poke,&(wk->five_poke[wk->partner]));
      RomBattleTowerPartnerDataMake(wk,&wk->five_data[wk->partner],
        TOWER_FIVE_FIRST+wk->partner,
        TowerPlayData_Get(wk->playSave,BSWAY_PSD_pare_itemfix,NULL),
        &(wk->five_poke[wk->partner]),wk->heapID);
    }
    #endif
  }

#if 0  
  bsw_scr->renshou = BSUBWAY_SCOREDATA_SetRenshou(
      bsw_scr->scoreData, BSWAY_SETMODE_get );
#endif

  bsw_scr->my_sex = MyStatus_GetMySex( mystatus );
  
  //���݂̃��R�[�h���擾
#if 0 //wb null
  if( bsw_scr->play_mode != BSWAY_MODE_RETRY ){
    frontier = SaveData_GetFrontier(savedata);
    record = SaveData_GetRecord(savedata);
    
    //���݂̘A�����͒��풆�t���O��on�̂Ƃ����������p��
    if(wk->play_mode == BSWAY_MODE_WIFI_MULTI){
#if 0
      chg_flg = TowerScoreData_SetFlags(wk->scoreSave,
            BSWAY_SFLAG_WIFI_MULTI_RECORD,BSWAY_DATA_get);
#else
      //chg_flg = FrontierRecord_Get(SaveData_GetFrontier(savedata), 
      //            FRID_TOWER_MULTI_WIFI_CLEAR_BIT,
      //            Frontier_GetFriendIndex(FRID_TOWER_MULTI_WIFI_CLEAR_BIT) );
      chg_flg = SysWork_WifiFrClearFlagGet( SaveData_GetEventWork(savedata) );
      OS_Printf( "WIFI chg_flg = %d\n", chg_flg );
#endif
    }else{
      chg_flg = TowerScoreData_SetFlags(wk->scoreSave,
            BSWAY_SFLAG_SINGLE_RECORD+wk->play_mode,BSWAY_DATA_get);
    }

    if(chg_flg){
      if(wk->play_mode == BSWAY_MODE_WIFI_MULTI){
        wk->renshou = FrontierRecord_Get(
        frontier,FRID_TOWER_MULTI_WIFI_RENSHOU_CNT,
        Frontier_GetFriendIndex(FRID_TOWER_MULTI_WIFI_RENSHOU_CNT) );
      }else{
        wk->renshou = FrontierRecord_Get(
        frontier,FRID_TOWER_SINGLE_RENSHOU_CNT+wk->play_mode*2,
        FRONTIER_RECORD_NOT_FRIEND);
      }

      wk->stage = TowerScoreData_SetStage(
          wk->scoreSave,wk->play_mode,BSWAY_DATA_get);
    }

    wk->win_cnt = RECORD_Get(record,RECID_BTOWER_WIN);

    //wk->stage��chg_flg�Ɋ֌W�Ȃ��擾���Ă����̂ŕύX(08.05.20)
    //wifi�̂��߂ɕύX������wifi�ł͘A���L�^��DP�Ƃ͕ʂ̂��̂��g�p���Ă���̂ŁA
    //���񐔂̕ϐ��͌��Ă��Ȃ����畽�C�Ǝv���邪�ꉞ�B
    //wk->stage = TowerScoreData_SetStage(
        wk->scoreSave,wk->play_mode,BSWAY_DATA_get);
  }
#endif

#if 0 //wb null
  //WIFI(32�l�f�[�^������̂ŁA���񐔃��[�N������Ȃ����߁A
  //�A����������񐔂��Z�o���ăZ�b�g
  if(wk->play_mode == BSWAY_MODE_WIFI_MULTI){
    wk->stage = TowerScoreData_SetStageValue(
        wk->scoreSave, BSWAY_MODE_WIFI_MULTI, 
        (wk->renshou / 7) );
    OS_Printf( "** wk->renshou = %d\n", wk->renshou );
    OS_Printf( "** stage = %d\n", TowerScoreData_SetStage(wk->scoreSave,
                              BSWAY_MODE_WIFI_MULTI,BSWAY_DATA_get) );
  }

  OS_Printf( "stage = %d\n", wk->stage );
  return wk;
#endif
  return( bsw_scr );
}

//--------------------------------------------------------------
/**
 * BSUBWAY_SCRWORK ���[�N�G���A���J������
 * @param bsw_scr BSUBWAY_SCRWORK*
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_ReleaseWork( GAMESYS_WORK *gsys, BSUBWAY_SCRWORK *bsw_scr )
{
  if( bsw_scr != NULL ){
    GF_ASSERT( bsw_scr->magicNo == BSUBWAY_SCRWORK_MAGIC );
    MI_CpuClear8( bsw_scr, sizeof(BSUBWAY_SCRWORK) );
    GFL_HEAP_FreeMemory( bsw_scr );
    
  }
  
  {
    GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
    GAMEDATA_SetBSubwayScrWork( gdata, NULL );
  }
}

//--------------------------------------------------------------
/**
 * BSUBWAY_SCRWORK �x�ނƂ��Ɍ��݂̃v���C�󋵂��Z�[�u�ɏ����o��
 * @param bsw_scr BSUBWAY_SCRWORK*
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_SaveRestPlayData( BSUBWAY_SCRWORK *bsw_scr )
{
  u16  i;
  u8  buf8[4];

  //�v���C���[�h�����o��
  buf8[0] = bsw_scr->play_mode;
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_playmode, buf8 );

  //���E���h�������o��
  buf8[0] = bsw_scr->now_round;
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_round, buf8 );

  //�I�񂾃|�P����No
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_pokeno, bsw_scr->member );
  
  //�o�g�����я����o��
  BSUBWAY_PLAYDATA_AddWifiRecord( bsw_scr->playData,
    bsw_scr->rec_down, bsw_scr->rec_turn, bsw_scr->rec_damage );
  
  //���I���ꂽ�g���[�i�[No�����o��
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_trainer, bsw_scr->trainer );
  
  //�v���C�����_���V�[�h�ۑ�
  #if 0 //wb null
  TowerPlayData_Put(wk->playSave,BSWAY_PSD_rnd_seed,&(wk->play_rnd_seed));
  OS_Printf("TowerRestRndSeed = %d\n",wk->play_rnd_seed);
  #endif
  
  //�Z�[�u�t���O��L����ԂɃ��Z�b�g
  BSUBWAY_PLAYDATA_SetSaveFlag( bsw_scr->playData, TRUE );
  
  if( bsw_scr->play_mode != BSWAY_MODE_MULTI ){
    return;
  }
  
  //AI�}���`���[�h�Ȃ�p�[�g�i�[���o���Ă���
  buf8[0] = bsw_scr->partner;
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData, BSWAY_PLAYDATA_ID_partner, buf8 );
  
  //�p�[�g�i�[�̃|�P�����p�����[�^�������Ă���
  #if 0 //wb null
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_pare_poke,
      &(bsw_scr->five_poke[bsw_scr->partner]) );
  #endif

  //�A�C�e�����Œ肾�������ǂ��������Ă���
  #if 0 //wb null
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_pare_itemfix,
      &(bsw_scr->five_item[bsw_scr->partner]) );
  #endif
}

u16 BSUBWAY_SCRWORK_SetNGScore( GAMESYS_WORK *gsys )
{
  int id;
  u8  play_mode;
  BSUBWAY_PLAYDATA *playData;
  BSUBWAY_SCOREDATA *scoreData;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( gdata );
  
  //�Z�[�u�f�[�^�擾
  playData = SaveControl_DataPtrGet( save, GMDATA_ID_BSUBWAY_PLAYDATA );
  scoreData = SaveControl_DataPtrGet( save, GMDATA_ID_BSUBWAY_SCOREDATA );

  //�ǂ̃��[�h���v���C���Ă������H
  play_mode = BSUBWAY_PLAYDATA_GetData( playData,
      BSWAY_PLAYDATA_ID_playmode, NULL );
  
  if( play_mode == BSWAY_MODE_RETRY ){
    return play_mode;
  }
  
  //���R�[�h���풆�t���O�𗎂Ƃ�
  BSUBWAY_SCOREDATA_SetFlag( scoreData,
    BSWAY_SCOREDATA_FLAG_SINGLE_RECORD + play_mode,
    BSWAY_SETMODE_reset );

  //���݂̎��񐔃��Z�b�g
  BSUBWAY_SCOREDATA_SetStage( scoreData,
    play_mode, BSWAY_SETMODE_reset );
  
  #if 0 //wb null
  if( play_mode != BSWAY_MODE_WIFI ){
    //�v���C�����_���V�[�h���ЂƂ����Ői�߂�
    BtlTower_UpdatePlayRndSeed(savedata);
  }
  #endif
  
  OS_Printf("TowerNGScoreSet -> mode = %d\n",play_mode);
  return play_mode;  
}

//--------------------------------------------------------------
//  @brief  �s�폈��  
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_SetLoseScore(
    GAMESYS_WORK *gsys, BSUBWAY_SCRWORK *bsw_scr )
{
  u32  ret = 0;
  int  id;
  u16  before,after,chg_flg;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( gdata );
  RECORD *record = SaveControl_DataPtrGet( save, GMDATA_ID_RECORD );
  
  if( bsw_scr->play_mode == BSWAY_MODE_RETRY ){
    return;
  }
  
  OS_Printf( "TowerLoseScoreSet -> mode = %d\n", bsw_scr->play_mode );
  
  id = bsw_scr->play_mode * 2 + RECID_RENSHOU_SINGLE;
  
  bsw_scr->renshou = 0;

#if 0
  BSUBWAY_SCOREDATA_SetRenshou(
     bsw_scr->scoreData, BSWAY_SETMODE_reset );
#endif

#if 0 //wb null
  //���݂̍ő�A�����擾
  before = RECORD_Get( record, id );
  
  //�ő�A�����X�V
  after = RECORD_SetIfLarge(record,id,wk->renshou+wk->now_win);

  //�X�V���Ă���||(�L�^��7�̔{��&&�O��̒l������)�Ȃ�ԑg�쐬
  if(after > 1){
    if(  (before < after) ||
      ((before == after) && (after%7==0)) ){
      towerscr_MakeTVRenshouMaxUpdate(wk,fsys,after);
    }
  }
  //���у��j�^�p�Ɍ��݂̘A�����������o���Ă���
  chg_flg = TowerScoreData_SetFlags(wk->scoreSave,
      BSWAY_SFLAG_SINGLE_RECORD+wk->play_mode,BSWAY_DATA_get);
  if(chg_flg){  //�A���L�^���풆�Ȃ���Z
    ret = RECORD_Add(record,id+1,wk->now_win);
  }else{  //�O�񕉂��Ă���΃Z�b�g
    ret = RECORD_Set(record,id+1,wk->now_win);
  }

  //���R�[�h���풆�t���O�𗎂Ƃ�
  TowerScoreData_SetFlags(
      wk->scoreSave,
      BSWAY_SFLAG_SINGLE_RECORD+wk->play_mode,BSWAY_DATA_reset);
  
  //���׏������X�V
  RECORD_Add(record,RECID_BTOWER_WIN,wk->now_win);
#endif

  //���݂̎��񐔃��Z�b�g
  BSUBWAY_SCOREDATA_SetStage( bsw_scr->scoreData,
      bsw_scr->play_mode, BSWAY_SETMODE_reset );

  #if 0  //wb null
  //�o�g���^���[�ւ̃`�������W���ǉ�
  RECORD_Add(SaveData_GetRecord(savedata),RECID_BTOWER_CHALLENGE,1);
  
  //�A�����{�������炦�邩�ǂ����̃t���O���Z�b�g
  towerscr_IfRenshouPrizeGet(wk);
  #endif

  //�����������ʃf�[�^����
  ret+=1;
  
  if(ret > 9999){
    ret = 9999;  
  }

#if 0 //wb null
  towerscr_SetCommonScore(wk,savedata,FALSE,ret);
#endif
}

#if 0
static void towerscr_SetCommonScore(BTOWER_SCRWORK* wk,SAVEDATA* sv,u8 win_f,u16 now_renshou)
{
	u8	buf8;

	switch(wk->play_mode){
	case BSWAY_MODE_SINGLE:
#if 0
		//�|�P�����f�[�^�Z�b�g
		towerscr_SaveMemberPokeData(wk,sv,BTWR_SCORE_POKE_SINGLE);
#endif
	case BTWR_MODE_DOUBLE:
#if 0
		if(now_renshou >= 7){
			//TV�C���^�r���[�f�[�^�Z�b�g(�V���O���ƃ_�u���Ŏ��s)
			TVTOPIC_BTowerTemp_Set(SaveData_GetTvWork(sv),win_f,now_renshou);
		}
#endif
		break;
#if 0
	case BTWR_MODE_WIFI:
		//�|�P�����f�[�^�Z�b�g
		towerscr_SaveMemberPokeData(wk,sv,BTWR_SCORE_POKE_WIFI);
		//�X�R�A�����o��
		TowerPlayData_WifiRecordAdd(wk->playSave,wk->rec_down,wk->rec_turn,wk->rec_damage);

		//�v���C���[�h�����o��
		buf8 = wk->play_mode;
		TowerPlayData_Put(wk->playSave,BTWR_PSD_playmode,&buf8);
		//���E���h�������o��
		buf8 = wk->now_round;
		TowerPlayData_Put(wk->playSave,BTWR_PSD_round,&buf8);
		
		TowerScoreData_SetWifiScore(wk->scoreSave,wk->playSave);
		break;
#endif
	default:
		break;
	}
}
#endif

u16  BSUBWAY_SCRWORK_AddBattlePoint( BSUBWAY_SCRWORK *bsw_scr )
{
  u16  stage;
  u16  point = 0;
  static const u8  bpoint_wifi[] = {0,
    BTLPOINT_VAL_BSUBWAY_WIFI1,BTLPOINT_VAL_BSUBWAY_WIFI2,
    BTLPOINT_VAL_BSUBWAY_WIFI3,BTLPOINT_VAL_BSUBWAY_WIFI4,
    BTLPOINT_VAL_BSUBWAY_WIFI5,BTLPOINT_VAL_BSUBWAY_WIFI6,
    BTLPOINT_VAL_BSUBWAY_WIFI7,BTLPOINT_VAL_BSUBWAY_WIFI8,
    BTLPOINT_VAL_BSUBWAY_WIFI9,BTLPOINT_VAL_BSUBWAY_WIFI10,
  };
  static const u8 bpoint_normal[] = {0,
    BTLPOINT_VAL_BSUBWAY_STAGE1,BTLPOINT_VAL_BSUBWAY_STAGE2,
    BTLPOINT_VAL_BSUBWAY_STAGE3,BTLPOINT_VAL_BSUBWAY_STAGE4,
    BTLPOINT_VAL_BSUBWAY_STAGE5,BTLPOINT_VAL_BSUBWAY_STAGE6,
    BTLPOINT_VAL_BSUBWAY_STAGE7,
  };
  
  if( bsw_scr->play_mode == BSWAY_MODE_RETRY ){
    return 0;
  }
  
  if( bsw_scr->play_mode == BSWAY_MODE_WIFI ){ //�����N����
    u8 rank = BSUBWAY_SCOREDATA_SetWifiRank(
        bsw_scr->scoreData, BSWAY_SETMODE_get );
    point = bpoint_wifi[rank];
  }else{ //���񐔂���
    stage = BSUBWAY_SCOREDATA_SetStage( bsw_scr->scoreData,
      bsw_scr->play_mode, BSWAY_SETMODE_get );
    
    if( bsw_scr->leader_f ){
      point = BTLPOINT_VAL_BSUBWAY_LEADER;
    }else if( stage >= 7 ){
      point = BTLPOINT_VAL_BSUBWAY_STAGE8;
    }else{
      point = bpoint_normal[stage];
    }
  }
  
  BSUBWAY_SCOREDATA_SetBattlePoint( //�o�g���|�C���g�����Z����
      bsw_scr->scoreData, point, BSWAY_SETMODE_add );
  return point;
}

void BSUBWAY_SCRWORK_SetBtlTrainerNo( BSUBWAY_SCRWORK *wk )
{
  int i;
  u16  no,stage;
  
  if(  wk->play_mode == BSWAY_MODE_MULTI ||
      wk->play_mode == BSWAY_MODE_COMM_MULTI){

  if( wk->play_mode == BSWAY_MODE_COMM_MULTI &&
          wk->pare_stage > wk->stage){
        stage = wk->pare_stage;  //�ʐM���ɂ͎��񐔂̑����ق��Œ��I
    }else{
      stage = wk->stage;
    }
    
    for( i = 0;i < BSUBWAY_STOCK_TRAINER_MAX; i++ ){
      do{
        no = BattleTowerTrainerNoGet( wk, stage,i/2, wk->play_mode );
      }while(is_trainer_conflict(wk->trainer,no,i));
      wk->trainer[i] = no;
    }
  }else{
    for(i = 0;i < (BSUBWAY_STOCK_TRAINER_MAX/2);i++){
      do{
        no = BattleTowerTrainerNoGet(wk,wk->stage,i,wk->play_mode);
      }while(is_trainer_conflict(wk->trainer,no,i));
      wk->trainer[i] = no;
    }
  }
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * �v���C���[�h�ʃ����o�[��
 * @param play_mode 
 * @retval  
 */
//--------------------------------------------------------------
static u8 bswScr_GetMemberNum( u16 mode )
{
  switch(mode){
  case BSWAY_MODE_SINGLE:
  case BSWAY_MODE_WIFI:
  case BSWAY_MODE_RETRY:
    return 3;
  case BSWAY_MODE_DOUBLE:
    return 4;
  case BSWAY_MODE_MULTI:
  case BSWAY_MODE_COMM_MULTI:
  case BSWAY_MODE_WIFI_MULTI:
    return 2;
  }
  return 0;
}

//======================================================================
//  �o�g���T�u�E�F�C�@�X�N���v�g�R�}���h�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * ����ڑ���Ɍ��݂̃��P�[�V�������Z�b�g
 * @param
 * @retval
 */
//--------------------------------------------------------------
/*
void BSUBWAY_SCRWORK_PushNowLocation( GAMEDATA *gdata )
{
  GAMEDATA_SetSpecialLocation( gdata, &loc );
}
*/

//--------------------------------------------------------------
/**
 *  @brief  �o�g���^���[�p�|�P�����I����ʌĂяo���ďo��
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_SelectPoke(
    BSUBWAY_SCRWORK *wk, GMEVENT *ev, void **proc_wk )
{
#if 0 //wb
  EventCmd_BTowerPokeSelectCall( ev_work,proc_wk,
    PL_MODE_BATTLE_TOWER,PST_MODE_NORMAL,
    wk->member_num,wk->member_num,100,0);
#endif
}

//--------------------------------------------------------------
/**
 *  @brief  �I�������|�P�������擾
 *  @retval  TRUE  �I������
 *  @retval  FALSE  �I�������ɂ�߂�
 */
//--------------------------------------------------------------
BOOL BSUBWAY_SCRWORK_GetEntryPoke(
    BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys )
{
  u16  i = 0;
  POKEPARTY *party;
  POKEMON_PARAM *pp;
  
  //�f�[�^�擾
  if( wk->pokelist_return_mode != PL_RET_NORMAL ||
      wk->pokelist_result_select == PL_SEL_POS_EXIT ||
      wk->pokelist_result_select == PL_SEL_POS_EXIT2 ){
    return FALSE;
  }
  
  party = GAMEDATA_GetMyPokemon( GAMESYSTEM_GetGameData(gsys) );

#ifdef DEBUG_ONLY_FOR_kagaya  
  for( i = 0;i < wk->member_num;i++){
    KAGAYA_Printf( "�I�������|�P���� No.%d = Pos %d\n", i,
        wk->pokelist_select_num[i] );
  }
#endif

  for( i = 0;i < wk->member_num;i++){
    //�|�P�����I���Ŏ擾�����莝��No
    if( (wk->pokelist_select_num[i]-1) >= 6 ){
      GF_ASSERT( 0 );
      wk->pokelist_select_num[i] = 1;
    }
    
    wk->member[i] = wk->pokelist_select_num[i] - 1;
    pp = PokeParty_GetMemberPointer( party, wk->member[i] );
    wk->mem_poke[i] = PP_Get( pp, ID_PARA_monsno, NULL );  
    wk->mem_item[i] = PP_Get( pp, ID_PARA_item, NULL );  
  }
  
  return TRUE;
}

//--------------------------------------------------------------
/**
 *  @brief  �Q���w�肵���莝���|�P�����̏����`�F�b�N
 *  @retval  0  �Q��OK
 *  @retval  1  �����|�P����������
 *  @retval 2  �����A�C�e���������Ă���
 */
//--------------------------------------------------------------
int BSUBWAY_SCRWORK_CheckEntryPoke(
    BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys ) //SAVEDATA *savedata )
{
#if 0 //wb
#if 1
  u16  i = 0,j = 0;
  u16  monsno[BSUBWAY_STOCK_MEMBER_MAX],itemno[BSUBWAY_STOCK_MEMBER_MAX];
  POKEPARTY *party;
  POKEMON_PARAM *poke;

  party = SaveData_GetTemotiPokemon( savedata );

  for( i = 0;i < wk->member_num;i++){
    poke = PokeParty_GetMemberPointer( party,wk->member[i]);
    monsno[i] = PokeParaGet( poke,ID_PARA_monsno,NULL );
    itemno[i] = PokeParaGet( poke,ID_PARA_item,NULL );

    if( i == 0){
      continue;
    }
    for( j = 0;j < i;j++){
      if( monsno[i] == monsno[j]){
        //�����|�P����������
        return 1;
      }
      if(( itemno[i] != 0) && ( itemno[i] == itemno[j])){
        //�����A�C�e���������Ă���
        return 2;
      }
    }
  }
  return 0;
#else
  return BSUBWAY_SCRWORK_CheckEntryPokeSub( wk->member, wk->member_num, savedata );
#endif
#else
  return 0;
#endif
}

//--------------------------------------------------------------
/**
 *  @brief�@�ΐ�g���[�i�[No���I
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_BtlTrainerNoSet( BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys )
{
#if 0 //wb
  int i;
  u16  no,stage;
  
  if(  wk->play_mode == BSWAY_MODE_MULTI ||
    wk->play_mode == BSWAY_MODE_WIFI_MULTI ||
    wk->play_mode == BSWAY_MODE_COMM_MULTI ){
    //if( wk->play_mode == BSWAY_MODE_COMM_MULTI && wk->pare_stage > wk->stage ){
    if( ( wk->play_mode == BSWAY_MODE_COMM_MULTI && wk->pare_stage > wk->stage ) ||
      ( wk->play_mode == BSWAY_MODE_WIFI_MULTI && wk->pare_stage > wk->stage ) ){
      stage = wk->pare_stage;  //�ʐM���ɂ͎��񐔂̑����ق��Œ��I
    }else{
      stage = wk->stage;
    }
    for( i = 0;i < BSUBWAY_STOCK_TRAINER_MAX;i++){
      do{
        no = BattleTowerTrainerNoGet( wk,stage,i/2,wk->play_mode );
      }while( is_trainer_conflict( wk->trainer,no,i ));
#ifdef BSUBWAY_AUTO_DEB
#if 0
      wk->trainer[i] = DebugBTowerAutoTrainer;
      OS_Printf(" #TowerStage = %d,%d\n",DebugBTowerAutoStage,DebugBTowerAutoTrainer );
      if( DebugBTowerAutoTrainer++ >= 299){
        DebugBTowerAutoTrainer = 0;
        DebugBTowerAutoStage++;
      }
#endif
      wk->trainer[i] = no;
      if( DebugBTowerAutoTrainer++ >= 299){
        DebugBTowerAutoStage++;
      }
      OS_Printf(" #TowerStage = %d,%d\n",DebugBTowerAutoStage,DebugBTowerAutoTrainer );
#else    
      wk->trainer[i] = no;
#endif
    }
  }else{
    for( i = 0;i < ( BSUBWAY_STOCK_TRAINER_MAX/2);i++){
      do{
        no = BattleTowerTrainerNoGet( wk,wk->stage,i,wk->play_mode );
      }while( is_trainer_conflict( wk->trainer,no,i ));
#ifdef BSUBWAY_AUTO_DEB
#if 0
      wk->trainer[i] = DebugBTowerAutoTrainer;
      OS_Printf(" #TowerStage = %d,%d\n",DebugBTowerAutoStage,DebugBTowerAutoTrainer );
      if( DebugBTowerAutoTrainer++ >= 299){
        DebugBTowerAutoTrainer = 0;
        DebugBTowerAutoStage++;
      }
#endif
      if( DebugBTowerAutoTrainer++ >= 299){
        DebugBTowerAutoStage++;
      }
      OS_Printf(" #TowerStage = %d,%d\n",DebugBTowerAutoStage,DebugBTowerAutoTrainer );
      wk->trainer[i] = 25;//no;
#else    
      wk->trainer[i] = no;
#endif
    }
  }
#endif
}

//--------------------------------------------------------------
/**
 *  @brief  ���݂̃��E���h�����X�N���v�g���[�N�Ɏ擾����
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCRWORK_GetNowRound( BSUBWAY_SCRWORK *wk )
{
  return wk->now_round;
}

//--------------------------------------------------------------
/**
 *  @brief  7�A�����Ă��邩�ǂ����`�F�b�N
 */
//--------------------------------------------------------------
BOOL BSUBWAY_SCRWORK_IsClear( BSUBWAY_SCRWORK *wk )
{
  if( wk->clear_f ){
    return TRUE;
  }
  
  if( wk->now_round > BSWAY_CLEAR_WINCNT ){
    //�N���A�t���Oon
    wk->clear_f = 1;
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 *  @brief  �ő�A���L�^�X�V�ԑg�쐬�֐�
 */
//--------------------------------------------------------------
#if 0 //wb
static void bswayscr_MakeTVRenshouMaxUpdate(
    BSUBWAY_SCRWORK *wk,SAVEDATA *savedata,u16 renshou )
{
  POKEPARTY *party;

  if(  wk->play_mode != BSWAY_MODE_SINGLE &&
    wk->play_mode != BSWAY_MODE_DOUBLE ){
    return;
  }
  party = SaveData_GetTemotiPokemon( savedata );

  if( wk->play_mode == BSWAY_MODE_SINGLE ){
    //TVTOPIC_Entry_Record_BTower( fsys,
    TVTOPIC_Entry_Record_BTowerEx( savedata,
      renshou,PokeParty_GetMemberPointer( party,wk->member[0]), TRUE );
  }else{
    //TVTOPIC_Entry_Record_BTower( fsys,
    TVTOPIC_Entry_Record_BTowerEx( savedata,
      renshou,PokeParty_GetMemberPointer( party,wk->member[0]), FALSE );
  }
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  7�l���������܂��͕��������̋��ʃf�[�^�Z�[�u����
 *  @param  now_renshou  ���݂̘A����
 */
//--------------------------------------------------------------
#if 0 //wb
static void bswayscr_SetCommonScore(
    BSUBWAY_SCRWORK *wk, SAVEDATA *sv, u8 win_f, u16 now_renshou )
{
  u8  buf8;

  switch( wk->play_mode ){
  case BSWAY_MODE_SINGLE:
    //�|�P�����f�[�^�Z�b�g
    bswayscr_SaveMemberPokeData( wk,sv,BSWAY_SCORE_POKE_SINGLE );
  case BSWAY_MODE_DOUBLE:
    if( now_renshou >= 7){
      //TV�C���^�r���[�f�[�^�Z�b�g(�V���O���ƃ_�u���Ŏ��s)
      TVTOPIC_BTowerTemp_Set( SaveData_GetTvWork( sv ),win_f,now_renshou );
    }
    break;
  case BSWAY_MODE_WIFI:
    //�|�P�����f�[�^�Z�b�g
    bswayscr_SaveMemberPokeData( wk,sv,BSWAY_SCORE_POKE_WIFI );
    //�X�R�A�����o��
    TowerPlayData_WifiRecordAdd( wk->playSave,wk->rec_down,wk->rec_turn,wk->rec_damage );

    //�v���C���[�h�����o��
    buf8 = wk->play_mode;
    TowerPlayData_Put( wk->playSave,BSWAY_PSD_playmode,&buf8);
    //���E���h�������o��
    buf8 = wk->now_round;
    TowerPlayData_Put( wk->playSave,BSWAY_PSD_round,&buf8);
    
    TowerScoreData_SetWifiScore( wk->scoreSave,wk->playSave );
    break;
  default:
    break;
  }
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  �s�폈��  
 */
//--------------------------------------------------------------

//--------------------------------------------------------------
/**
 *  @brief  �N���A����
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_SetClearScore(
    BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys )
{
/*
  u32  ret = 0;
  int  id;
//  void *note;
  u16  before,after,chg_flg;
//  RECORD * record;
//  FRONTIER_SAVEWORK *frontier;
    
  if( wk->play_mode == BSWAY_MODE_RETRY ){
    return;
  }
  
//  OS_Printf("TowerClearScoreSet -> mode = %d\n",wk->play_mode );
//
#if 0  
  record = SaveData_GetRecord( savedata );
  frontier = SaveData_GetFrontier( savedata );
#endif

#if 0
  if( wk->play_mode == BSWAY_MODE_WIFI_MULTI ){
    id = FRID_TOWER_MULTI_WIFI_RENSHOU;
  }else{
    id = wk->play_mode*2+FRID_TOWER_SINGLE_RENSHOU;
  }
#endif

  //���݂̘A�����������o��
  if( wk->play_mode == BSWAY_MODE_WIFI_MULTI ){
    #if 0 //wb null
    chg_flg = FrontierRecord_Get( SaveData_GetFrontier( savedata ), 
          FRID_TOWER_MULTI_WIFI_CLEAR_BIT,
          Frontier_GetFriendIndex( FRID_TOWER_MULTI_WIFI_CLEAR_BIT ) );
    #else
    ch_flg = 0;
    #endif
  }else{
    chg_flg = BSUBWAY_SCOREDATA_SetFlag( wk->scoreData,
        BSWAY_SCOREDATA_FLAG_SINGLE_RECORD + wk->play_mode,
        BSWAY_DATA_get );
  }

  if( chg_flg ){  //���݂��A���L�^���풆�Ȃ���Z
    #if 0 //wb null
    ret = FrontierRecord_Get(  SaveData_GetFrontier( savedata ), 
                  ( id+1),
                  Frontier_GetFriendIndex( id+1) );

    FrontierRecord_Set(  SaveData_GetFrontier( savedata ), 
              ( id+1),
              Frontier_GetFriendIndex( id+1), ( ret+BSWAY_CLEAR_WINCNT ) );

    ret = FrontierRecord_Get(  SaveData_GetFrontier( savedata ), 
                  ( id+1),
                  Frontier_GetFriendIndex( id+1) );
    #endif
  }else{  //�O�񕉂��Ă��ꍇ�͈�U���Z�b�g
    #if 0 //wb null
    ret = FrontierRecord_Set(
        frontier,id+1,Frontier_GetFriendIndex( id+1),BSWAY_CLEAR_WINCNT );
    #endif
  }

#if 0
  //�A�����Ă���A���������ɁAwk->renshou���K��0�ɂȂ��Ă���΁A
  //( renshou + now_win )���Z�b�g�ł����͂�(08.05.25)
  //7+7=14����(�����I���)
  //7+3=10����(�����I���)
  //0+3=3����(�����I���)
  OS_Printf( "wk->renshou = %d\n", wk->renshou );
  OS_Printf( "wk->now_win = %d\n", wk->now_win );
  ret = FrontierRecord_Set( frontier,id+1,Frontier_GetFriendIndex( id+1),( wk->renshou+wk->now_win ));
#endif

  //���R�[�h���풆�t���O�𗧂Ă�
#if 0
  if( wk->play_mode == BSWAY_MODE_WIFI_MULTI ){
#if 0
    TowerScoreData_SetFlags( wk->scoreSave,BSWAY_SFLAG_WIFI_MULTI_RECORD,BSWAY_DATA_set );
#else
    FrontierRecord_Set(  SaveData_GetFrontier( savedata ), 
              FRID_TOWER_MULTI_WIFI_CLEAR_BIT,
              Frontier_GetFriendIndex( FRID_TOWER_MULTI_WIFI_CLEAR_BIT ), 1 );
#endif
  }else{
    TowerScoreData_SetFlags( wk->scoreSave,BSWAY_SFLAG_SINGLE_RECORD+wk->play_mode,BSWAY_DATA_set );
  }
#endif

  //���݂̍ő�A�����擾
  before = FrontierRecord_Get( frontier,id,Frontier_GetFriendIndex( id ));
  //�ő�A�����X�V
  after = FrontierRecord_SetIfLarge( frontier,id,Frontier_GetFriendIndex( id ),ret );

  //�X�V���Ă���ԑg�쐬
#if 0
  if( before < after ){
    bswayscr_MakeTVRenshouMaxUpdate( wk,fsys->savedata,after );
  }
#endif

#if 0
  //���׏������X�V
  RECORD_Add( record,RECID_BSUBWAY_WIN,BSWAY_CLEAR_WINCNT );
  //���񐔃v���X
  TowerScoreData_SetStage( wk->scoreSave,wk->play_mode,BSWAY_DATA_inc );
#endif

#if 0
  //�o�g���^���[�ւ̃`�������W���ǉ�
  if( wk->play_mode != BSWAY_MODE_WIFI_MULTI ){
    RECORD_Add( record,RECID_BSUBWAY_CHALLENGE,1);
  }

  //�X�R�A���Z
  RECORD_Score_Add( record,SCORE_ID_BSUBWAY_7WIN );
  
  //�A�����{�������炦�邩�ǂ����̃t���O���Z�b�g
  bswayscr_IfRenshouPrizeGet( wk );

  //�����������ʃf�[�^����
  bswayscr_SetCommonScore( wk,savedata,TRUE,ret );
#endif

#if 0
  //�`���m�[�g(�ʐM�}���`&wifi�̂�)
  if(  wk->play_mode == BSWAY_MODE_COMM_MULTI ||
    wk->play_mode == BSWAY_MODE_WIFI_MULTI ||
    wk->play_mode == BSWAY_MODE_WIFI ){
#else
  //�`���m�[�g( WIFI_DL�̂�)
  if( wk->play_mode == BSWAY_MODE_WIFI ){
#endif
    note = FNOTE_SioBattleTowerDataMake( wk->heapID );
    FNOTE_DataSave( fnote, note, FNOTE_TYPE_SIO );
  }
*/
  u8 buf8;
  
  //�v���C���[�h�����o��
  buf8 = wk->play_mode;
  BSUBWAY_PLAYDATA_SetData( wk->playData,
      BSWAY_PLAYDATA_ID_playmode, &buf8 );

  //���E���h�������o��
  buf8 = wk->now_round;
	BSUBWAY_PLAYDATA_SetData( wk->playData,
      BSWAY_PLAYDATA_ID_round, &buf8 );
}

/**
 *  @brief  �ΐ�g���[�i�[���I
 */
void BSUBWAY_SCRWORK_ChoiceBattlePartner(BSUBWAY_SCRWORK *wk )
{
  int i;
  u16  monsno[2];
  u16  itemno[2];
  
  switch(wk->play_mode){
  case BSWAY_MODE_WIFI:
  case BSWAY_MODE_RETRY:
#if 0
    btltower_BtlPartnerSelectWifi(sv,wk->tr_data,wk->now_round-1);
#endif
    break;
  case BSWAY_MODE_MULTI:
  case BSWAY_MODE_COMM_MULTI:
#if 0
    RomBattleTowerTrainerDataMake(wk,&(wk->tr_data[0]),
      wk->trainer[(wk->now_round-1)*2+0],
      wk->member_num,NULL,NULL,NULL,wk->heapID);
    
    //�����X�^�[No�̏d���`�F�b�N
    for(i = 0;i < wk->member_num;i++){
      monsno[i] = wk->tr_data[0].btpwd[i].mons_no;
      itemno[i] = wk->tr_data[0].btpwd[i].item_no;
    }
    RomBattleTowerTrainerDataMake(wk,&(wk->tr_data[1]),
      wk->trainer[(wk->now_round-1)*2+1],wk->member_num,monsno,itemno,NULL,wk->heapID);
#endif
    break;
  case BSWAY_MODE_DOUBLE:
  case BSWAY_MODE_SINGLE:
  default:
    RomBattleTowerTrainerDataMake(wk,&(wk->tr_data[0]),
      wk->trainer[wk->now_round-1],
      wk->member_num,NULL,NULL,NULL,wk->heapID);
    break;
  }
}

/**
 *  @brief  AI�}���`�y�A�|�P�������I
 */
#if 0
void TowerScr_ChoiceBtlSeven(BTOWER_SCRWORK* wk)
{
  int i;

  for(i = 0;i < BSWAY_FIVE_NUM;i++){
    wk->five_item[i] = (u8)RomBattleTowerTrainerDataMake(wk,&(wk->five_data[i]),
      TOWER_FIVE_FIRST+i,wk->member_num,wk->mem_poke,wk->mem_item,&(wk->five_poke[i]),wk->heapID);
  }
}
#endif

//--------------------------------------------------------------
/**
 * @brief  AI�}���`�y�A�|�P�������I
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_ChoiceBtlSeven( BSUBWAY_SCRWORK *wk )
{
#if 0 //wb
  int i;
  
  for( i = 0;i < BSWAY_FIVE_NUM;i++){
    wk->five_item[i] = ( u8)RomBattleTowerTrainerDataMake( wk,&( wk->five_data[i]),
      TOWER_FIVE_FIRST+i,wk->member_num,wk->mem_poke,wk->mem_item,&( wk->five_poke[i]),wk->heapID );
  }
#endif
}

//--------------------------------------------------------------
/*
 * @brief  �ΐ�g���[�i�[OBJ�R�[�h�擾
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCRWORK_GetEnemyObj( BSUBWAY_SCRWORK *wk, u16 idx )
{
  //�g���[�i�[�^�C�v����OBJ�R�[�h���擾���Ă���
  return BtlTower_TrType2ObjCode( wk->tr_data[idx].bt_trd.tr_type );
}

//--------------------------------------------------------------
/**
 *  @brief  �퓬�Ăяo��
 */
//--------------------------------------------------------------
#if 0 //wb
void BSUBWAY_SCRWORK_LocalBattleCall(
    GMEVENT_CONTROL *event,BSUBWAY_SCRWORK *wk,BOOL *win_flag )
{
  EventCmd_TowerLocalBattle( event,wk,win_flag );
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  ���݂̃v���C���[�h��Ԃ�
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCRWORK_GetPlayMode( BSUBWAY_SCRWORK *wk )
{
  return wk->play_mode;
}

//--------------------------------------------------------------
/**
 *  @brief  ���[�_�[�N���A�t���O���擾����
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCRWORK_GetLeaderClearFlag( BSUBWAY_SCRWORK *wk )
{
  return (u16)wk->leader_f;
}

//--------------------------------------------------------------
/**
 *  @brief  �^���[�N���A���Ƀo�g���|�C���g�����Z����
 *  @return  �V���Ɏ擾�����o�g���|�C���g
 */
//--------------------------------------------------------------
u16  BSUBWAY_SCRWORK_AddBtlPoint( BSUBWAY_SCRWORK *wk )
{
#if 0 //wb
  u16  stage;
  u16  point = 0;
  static const u8  bpoint_wifi[] = {0,
    BTLPOINT_VAL_TOWER_WIFI1,BTLPOINT_VAL_TOWER_WIFI2,BTLPOINT_VAL_TOWER_WIFI3,
    BTLPOINT_VAL_TOWER_WIFI4,BTLPOINT_VAL_TOWER_WIFI5,BTLPOINT_VAL_TOWER_WIFI6,
    BTLPOINT_VAL_TOWER_WIFI7,BTLPOINT_VAL_TOWER_WIFI8,BTLPOINT_VAL_TOWER_WIFI9,
    BTLPOINT_VAL_TOWER_WIFI10,
  };
  static const u8 bpoint_normal[] = {0,
    BTLPOINT_VAL_TOWER_STAGE1,BTLPOINT_VAL_TOWER_STAGE2,BTLPOINT_VAL_TOWER_STAGE3,
    BTLPOINT_VAL_TOWER_STAGE4,BTLPOINT_VAL_TOWER_STAGE5,BTLPOINT_VAL_TOWER_STAGE6,
    BTLPOINT_VAL_TOWER_STAGE7,
  };
  static const u8 bpoint_comm[] = {0,
    BTLPOINT_VAL_TOWER_COMM1,BTLPOINT_VAL_TOWER_COMM2,BTLPOINT_VAL_TOWER_COMM3,
    BTLPOINT_VAL_TOWER_COMM4,BTLPOINT_VAL_TOWER_COMM5,BTLPOINT_VAL_TOWER_COMM6,
    BTLPOINT_VAL_TOWER_COMM7,
  };

  if( wk->play_mode == BSWAY_MODE_RETRY ){
    return 0;
  }
  if( wk->play_mode == BSWAY_MODE_WIFI ){
    //�����N����
    point = bpoint_wifi[TowerScoreData_SetWifiRank( wk->scoreSave,BSWAY_DATA_get )];
  }else{

    //���C�A���X�}���`�AWIFI
    //if( wk->play_mode == BSWAY_MODE_COMM_MULTI ){
    if( ( wk->play_mode == BSWAY_MODE_COMM_MULTI ) || ( wk->play_mode == BSWAY_MODE_WIFI_MULTI ) ){

      //���񐔂���
      stage = TowerScoreData_SetStage( wk->scoreSave,wk->play_mode,BSWAY_DATA_get );
      if( stage >= 7){                //���̔�r����DP�ƍ��킹��
        point = BTLPOINT_VAL_TOWER_COMM8;
      }else{
        point = bpoint_comm[stage];
      }

#if 0
    //WIFI(32�l�f�[�^������̂ŁA���񐔃��[�N������Ȃ����߁A�A����������񐔂��Z�o����
    }else if( wk->play_mode == BSWAY_MODE_WIFI_MULTI ){

      OS_Printf( "wk->renshou = %d\n", wk->renshou );
      OS_Printf( "wk->now_win = %d\n", wk->now_win );
      stage = ( wk->renshou / 7 );
      if( stage >= 7){                //���̔�r����DP�ƍ��킹��
        point = BTLPOINT_VAL_TOWER_COMM8;
      }else{
        point = bpoint_comm[stage];
      }
#endif

    //�V���O���A�_�u���AAI�}���`
    }else{

      //���񐔂���
      stage = TowerScoreData_SetStage( wk->scoreSave,wk->play_mode,BSWAY_DATA_get );
      if( wk->leader_f ){
        point = BTLPOINT_VAL_TOWER_LEADER;
      }else if( stage >= 7){
        point = BTLPOINT_VAL_TOWER_STAGE8;
      }else{
        point = bpoint_normal[stage];
      }
    }
  }
  //�o�g���|�C���g�����Z����
  TowerScoreData_SetBattlePoint( wk->scoreSave,point,BSWAY_DATA_add );
  return point;
}

/**
 *  @brief  �V���O���ŘA���������̂��J���̃g���t�B�[��������t���O�𗧂ĂĂ���
 */
u16  BSUBWAY_SCRWORK_GoodsFlagSet( BSUBWAY_SCRWORK *wk,SAVEDATA *sv )
{
  u8  id,fid;
  u16  record,goods,ret;

  record = TowerScrTools_GetRenshouRecord( sv,wk->play_mode );

  if( record < BSWAY_50_RENSHOU_CNT ){
    return 0;
  }
  if( record >= BSWAY_100_RENSHOU_CNT ){
    if( TowerScoreData_SetFlags( wk->scoreSave,BSWAY_SFLAG_GOLD_GET,BSWAY_DATA_get )){
      return 0;
    }
  }else{
    if( TowerScoreData_SetFlags( wk->scoreSave,BSWAY_SFLAG_SILVER_GET,BSWAY_DATA_get )){
      return 0;
    }
  }
#endif
  return 1;
}

//--------------------------------------------------------------
/**
 *  @brief  ���݂�WIFI�����N�𑀍삵�ĕԂ�
 */
//--------------------------------------------------------------
#if 0 //wb
u16 BSUBWAY_SCRWORK_SetWifiRank(
    BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys, u8 mode )
{
  u8  ct,rank;
  BTLTOWER_SCOREWORK *score = SaveData_GetTowerScoreData( sv );
  
  static const u8  btower_wifi_rankdown[] = {
   0,5,4,4,3,3,2,2,1,1,
//   0,2,1,1,1,1,1,1,1,1,
  };

  switch( mode ){
  //Get
  case 0:
    return ( u16)TowerScoreData_SetWifiRank( score,BSWAY_DATA_get );
  case 1:  //Inc
    //�A���s��t���O�𗎂Ƃ�
    TowerScoreData_SetFlags( score,BSWAY_SFLAG_WIFI_LOSE_F,BSWAY_DATA_reset );
    //���݂̃����N���擾
    rank = TowerScoreData_SetWifiRank( score,BSWAY_DATA_get );

    if( rank == 10){  
      wk->prize_f = 1;        //�����N10�Ȃ�A���{����Ⴆ������̓N���A(08.06.01)
      return 0;  //�����オ��Ȃ�
    }
#if 0
    //rank10�̎����A���{�������炦��t���O�𗧂Ă�K�v������
    //�����N�A�b�v�����͌Ă�ł͂����Ȃ�
    //�߂�l�́A�����N�A�b�v�������A�����N�A�b�v���Ă��Ȃ�����Ԃ��Ă���
    //�����N�A�b�v�����������A���{����n������ɍs���Ă��Ȃ��̂͐������̂��H���Ă�����
    //
    //msg_tower_56
    //�u�����ā�����́@�����N���Ɂ@�Ȃ�܂����I�v
    //
    //�����āA���{����n�������
    //���{�������łɎ����Ă���Ƃ��A�S�������Ă���Ƃ��́A�R�����g���������`�F�b�N����Ă���
#endif

    //�����N�A�b�v����
    TowerScoreData_SetWifiRank( score,BSWAY_DATA_inc );

    //�����N5�ȏ�ɃA�b�v���Ă��烊�{�������炦��
    if( rank+1 >= 5){
      wk->prize_f = 1;
    }
    return 1;
  case 2:  //dec
    //���݂̘A���s�퐔���J�E���g
    ct = TowerScoreData_SetWifiLoseCount( score,BSWAY_DATA_inc );
    rank = TowerScoreData_SetWifiRank( score,BSWAY_DATA_get );

    if( rank == 1){
      return 0;
    }
    //�����N�ʔs��J�E���g�`�F�b�N
    if( ct >= btower_wifi_rankdown[rank-1] ){
      //�����N�_�E��
      TowerScoreData_SetWifiRank( score,BSWAY_DATA_dec );
      //�A���s�퐔�ƘA���s��t���O�����Z�b�g
      TowerScoreData_SetWifiLoseCount( score,BSWAY_DATA_reset );
      TowerScoreData_SetFlags( score,BSWAY_SFLAG_WIFI_LOSE_F,BSWAY_DATA_reset );
      return 1;
    }
    return 0;
  }
  return 0;
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  ���[�_�[��|�������J�����{����������
 */
//--------------------------------------------------------------
#if 0 //wb
u16 BSUBWAY_SCRWORK_LeaderRibbonSet( BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys )
{
  if( wk->play_mode != BSWAY_MODE_SINGLE ){
    return 0;
  }
  
  //�����o�[�Ƀ��{���Z�b�g
  switch( wk->leader_f ){
  case 1:
    return bswayscr_PokeRibbonSet( sv,ID_PARA_sinou_battle_tower_ttwin_first,wk );
  case 2:
    return bswayscr_PokeRibbonSet( sv,ID_PARA_sinou_battle_tower_ttwin_second,wk );
  }
  return 0;
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  �t���O�������Ă�����A���̂��J�����{����������  
 */
//--------------------------------------------------------------
#if 0 //wb
u16 BSUBWAY_SCRWORK_RenshouRibbonSet( BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys )
{
  u8  id,fid;
  u16  record,goods,ret;
  UNDERGROUNDDATA *gSave;

  if( wk->play_mode == BSWAY_MODE_RETRY ){
    return 0;
  }

  /////////////////////////////////////////
  //�Ƃ肠�����d�l�����܂��Ă��Ȃ��̂ŕۗ�
  /////////////////////////////////////////
  if( wk->play_mode == BSWAY_MODE_WIFI_MULTI ){
    return 0;
  }

  if(!wk->prize_f ){
    return 0;
  }
  switch( wk->play_mode ){
  case BSWAY_MODE_DOUBLE:
    id = ID_PARA_sinou_battle_tower_2vs2_win50;
    break;
  case BSWAY_MODE_MULTI:
    id = ID_PARA_sinou_battle_tower_aimulti_win50;
    break;
  case BSWAY_MODE_COMM_MULTI:
    id = ID_PARA_sinou_battle_tower_siomulti_win50;
    break;
  case BSWAY_MODE_WIFI:
    id = ID_PARA_sinou_battle_tower_wifi_rank5;
    break;
  }
  //�����o�[�Ƀ��{���Z�b�g
  return bswayscr_PokeRibbonSet( sv,id,wk );
}
#endif

//--------------------------------------------------------------
/**
 *  @biref  �v���C�����_���V�[�h���X�V����
 *  d31r0201.ev
 *  case BSWAY_SUB_UPDATE_RANDOM �ŌĂ΂��
 *  �����́AWIFI�}���`�͒ʉ߂��Ȃ��̂ŁA���̂܂܂ł悢
 */
//--------------------------------------------------------------
#if 0 //wb
u16 BSUBWAY_SCRWORK_PlayRandUpdate( BSUBWAY_SCRWORK *wk,SAVEDATA *sv )
{
  u8  chg_flg;

  //���݃`�������W�p�������ǂ����H
  if( wk->play_mode == BSWAY_MODE_WIFI_MULTI ){
#if 0
    chg_flg = TowerScoreData_SetFlags( wk->scoreSave,
        BSWAY_SFLAG_WIFI_MULTI_RECORD,BSWAY_DATA_get );
#else
    chg_flg = FrontierRecord_Get( SaveData_GetFrontier( sv ), 
                  FRID_TOWER_MULTI_WIFI_CLEAR_BIT,
                  Frontier_GetFriendIndex( FRID_TOWER_MULTI_WIFI_CLEAR_BIT ) );
#endif
  }else{
    chg_flg = TowerScoreData_SetFlags( wk->scoreSave,
        BSWAY_SFLAG_SINGLE_RECORD+wk->play_mode,BSWAY_DATA_get );
  }
  
  if(!chg_flg ){
    //�v���C�X�V�����_���V�[�h�����l�擾&���t�X�V�V�[�h�X�V
    wk->play_rnd_seed = BtlTower_UpdatePlayRndSeed( sv );
  }else{
    //�L�^����J�n����day�V�[�h����_�ɁA���݂�play�V�[�h���擾����
    wk->play_rnd_seed = BtlTower_GetContinuePlayRndSeed( sv );
  }
  return ( wk->play_rnd_seed/65535);
}
#endif

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 *  @brief  �|�P�����Ƀ��{��������@�T�u
 */
//--------------------------------------------------------------
#if 0 //wb
static u16 bswayscr_PokeRibbonSet( SAVEDATA *sv,u8 ribbon,BSUBWAY_SCRWORK *wk )
{
  u8  flag = 1;
  u8 ct;
  int i;
  POKEPARTY  *party;
  POKEMON_PARAM *pp;
  
  party = SaveData_GetTemotiPokemon( sv );

  ct = 0;
  for( i = 0;i < wk->member_num;i++){
    pp = PokeParty_GetMemberPointer( party,wk->member[i]);
    if( PokeParaGet( pp,ribbon,NULL )){
      continue;  //���Ɏ����Ă�
    }

    PokeParaPut( pp,ribbon,&flag );

    //TV�g�s�b�N�쐬�F���{���R���N�^�[
    TVTOPIC_Entry_Record_Ribbon( sv, pp, ribbon );

    ++ct;
  }
  if( ct == 0){
    //�݂�Ȃ��������Ă�
    return FALSE;
  }
  return TRUE;
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  �A���̂��J�����{�������炦�邩�ǂ����̃t���O�𗧂ĂĂ���
 */
//--------------------------------------------------------------
#if 0
static u16 bswayscr_IfRenshouPrizeGet( BSUBWAY_SCRWORK *wk )
{
  u16 win;
  if(  wk->play_mode == BSWAY_MODE_RETRY ||
    wk->play_mode == BSWAY_MODE_SINGLE ||
    wk->play_mode == BSWAY_MODE_WIFI_MULTI ||
    wk->play_mode == BSWAY_MODE_WIFI ){
    return 0;
  }
  win = wk->renshou+wk->now_win;

  //50�A���ȏ�ł��炦��
  if( win < BSWAY_50_RENSHOU_CNT ){
    return 0;
  }
  //prize get�t���O�𗧂ĂĂ���
  wk->prize_f = 1;
  return 1;
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  �Q�������|�P�����̃p�����[�^��B_TOWER_POKEMON�^�Ƀp�b�N����
 */
//--------------------------------------------------------------
#if 0
static void bswayscr_PokeDataPack( B_TOWER_POKEMON *dat,POKEMON_PARAM *pp )
{
  int i;
  
  //mons_no/form_no/item_no
  dat->mons_no = PokeParaGet( pp,ID_PARA_monsno,NULL );
  dat->form_no = PokeParaGet( pp,ID_PARA_form_no,NULL );
  dat->item_no = PokeParaGet( pp,ID_PARA_item,NULL );

  //waza/pp_count
  for( i = 0;i < WAZA_TEMOTI_MAX;i++){
    dat->waza[i] = PokeParaGet( pp,ID_PARA_waza1+i,NULL );
    dat->pp_count |= (( PokeParaGet( pp,ID_PARA_pp_count1+i,NULL )) << ( i*2));
  }
  //country,id,personal
  dat->country_code = PokeParaGet( pp,ID_PARA_country_code,NULL );
  dat->id_no = PokeParaGet( pp,ID_PARA_id_no,NULL );
  dat->personal_rnd = PokeParaGet( pp,ID_PARA_personal_rnd,NULL );

  //power_rnd
  dat->power_rnd = PokeParaGet( pp,ID_PARA_power_rnd,NULL );

  //exp
  for( i = 0;i < 6;i++){
    dat->exp[i] = PokeParaGet( pp,ID_PARA_hp_exp+i,NULL );
  }
  //tokusei,natukido
  dat->tokusei = PokeParaGet( pp,ID_PARA_speabino,NULL );
  dat->natuki = PokeParaGet( pp,ID_PARA_friend,NULL );
  
  //�j�b�N�l�[��
  PokeParaGet( pp,ID_PARA_nickname,dat->nickname );
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  �Q�������|�P�����̃p�����[�^��ۑ�����
 */
//--------------------------------------------------------------
#if 0
static void bswayscr_SaveMemberPokeData( BSUBWAY_SCRWORK *wk,SAVEDATA *sv,BSWAY_SCORE_POKE_DATA mode )
{
  int i = 0;
  B_TOWER_POKEMON *dat;
  POKEPARTY  *party;
  POKEMON_PARAM *pp;
  
  dat = sys_AllocMemoryLo( wk->heapID,sizeof( B_TOWER_POKEMON )*3);
  MI_CpuClear8( dat,sizeof( B_TOWER_POKEMON )*3);

  party = SaveData_GetTemotiPokemon( sv );
  for( i = 0;i < 3;i++){
    pp = PokeParty_GetMemberPointer( party,wk->member[i]);
    bswayscr_PokeDataPack(&( dat[i]),pp );  
  }
  TowerScoreData_SetUsePokeData( wk->scoreSave,mode,dat );

  MI_CpuClear8( dat,sizeof( B_TOWER_POKEMON )*3);
  sys_FreeMemoryEz( dat );
}
#endif

//  �t�����e�B�A�ƃt�B�[���h�ŋ��ʂŎg�p������̂��ړ�
//--------------------------------------------------------------
/**
 * �o�g���^���[�g���[�i�[�̎����|�P�����̃p���[���������肷��
 * @param  tr_no  �g���[�i�[�i���o�[
 * @return  �p���[����
 * b_tower_fld.c��b_tower_ev�Ɉړ�
 */
//--------------------------------------------------------------
#if 0
u8  BattleTowerPowRndGet( u16 tr_no )
{
  u8  pow_rnd;

  if( tr_no<100){
    pow_rnd=(0x1f/8)*1;
  }
  else if( tr_no<120){
    pow_rnd=(0x1f/8)*2;
  }
  else if( tr_no<140){
    pow_rnd=(0x1f/8)*3;
  }
  else if( tr_no<160){
    pow_rnd=(0x1f/8)*4;
  }
  else if( tr_no<180){
    pow_rnd=(0x1f/8)*5;
  }
  else if( tr_no<200){
    pow_rnd=(0x1f/8)*6;
  }
  else if( tr_no<220){
    pow_rnd=(0x1f/8)*7;
  }
  else{
    pow_rnd=0x1f;
  }
  return pow_rnd;
}
#endif

//--------------------------------------------------------------
/**
 * @brief  �^���[�v���C�X�V�����_���V�[�h�擾 ���E���h�X�V���ɌĂ�
 * b_tower_fld.c��b_tower_ev�Ɉړ�
 */
//--------------------------------------------------------------
#if 0
u16  btower_rand( BSUBWAY_SCRWORK *wk )
{
  //�v���`�i�Œǉ����ꂽWIFI�}���`�͒ʏ�̃����_�����g�p
  if( wk->play_mode == BSWAY_MODE_WIFI_MULTI ){
    return ( gf_rand() );
  }

  wk->play_rnd_seed = BtlTower_PlayFixRand( wk->play_rnd_seed );

  OS_Printf("btower_rand = %d\n",wk->play_rnd_seed );
  return ( wk->play_rnd_seed/65535);
}
#endif
