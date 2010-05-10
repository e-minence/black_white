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

#include "gamesystem\btl_setup.h"

#include "savedata/bsubway_savedata.h"
#include "savedata/bsubway_savedata_def.h"
#include "battle/bsubway_battle_data.h"
#include "bsubway_scr_def.h"
#include "bsubway_scr.h"
#include "bsubway_scrwork.h"
#include "bsubway_tr.h"

#include "savedata/battle_box_save.h"
#include "fld_btl_inst_tool.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================
static BOOL is_ConflictTrainer( u16* trainer,u16 id,u16 num );

static void bsw_SetCommonScore(
    BSUBWAY_SCRWORK *wk, SAVE_CONTROL_WORK *sv, u8 win_f, u16 now_renshou );
static void bsw_SaveMemberPokeData(
    BSUBWAY_SCRWORK *wk, SAVE_CONTROL_WORK *sv, BSWAY_SCORE_POKE_DATA mode );

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
 * @param init ���������[�h BSWAY_PLAY_NEW:�n�߁ABSWAY_PLAY_CONTINUE:����
 * @param playmode �v���C���[�h�w��:BSWAY_MODE_�`*
 * @retval BSUBWAY_SCRWORK*
 */
//--------------------------------------------------------------
BSUBWAY_SCRWORK * BSUBWAY_SCRWORK_CreateWork(
    GAMESYS_WORK *gsys, u16 init, u16 playmode )
{
  int i;
  u8 buf8;
  BSUBWAY_SCRWORK *bsw_scr;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  MYSTATUS *mystatus = GAMEDATA_GetMyStatus( gdata );
  SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( gdata );
  
  bsw_scr = GFL_HEAP_AllocClearMemory(
      HEAPID_PROC, sizeof(BSUBWAY_SCRWORK) );
  
  bsw_scr->heapID = HEAPID_PROC;
  bsw_scr->magicNo = BSUBWAY_SCRWORK_MAGIC;
  
  bsw_scr->gdata = gdata;
  bsw_scr->my_sex = MyStatus_GetMySex( mystatus );
  
  bsw_scr->playData = SaveControl_DataPtrGet(
      save, GMDATA_ID_BSUBWAY_PLAYDATA );
  
  bsw_scr->scoreData = SaveControl_DataPtrGet(
      save, GMDATA_ID_BSUBWAY_SCOREDATA );

  bsw_scr->wifiData = SaveControl_DataPtrGet(
      save, GMDATA_ID_BSUBWAY_WIFIDATA );
  
  BSUBWAY_PLAYDATA_SetSaveFlag( bsw_scr->playData, FALSE ); //�Z�[�u�Ȃ���
  
  GF_ASSERT( GAMEDATA_GetBSubwayScrWork(gdata) == NULL );
  GF_ASSERT( bsw_scr->playData != NULL );
  GF_ASSERT( bsw_scr->scoreData != NULL );
  
  GAMEDATA_SetBSubwayScrWork( gdata, bsw_scr );
  
  if( init == BSWAY_PLAY_NEW ){ //�V�K
    bsw_scr->play_mode = playmode;
    bsw_scr->member_num =
      BSUBWAY_SCRWORK_GetPlayModeMemberNum( bsw_scr->play_mode );
    
    for(i = 0;i < BSUBWAY_STOCK_MEMBER_MAX;i++){
      bsw_scr->member[i] = BSWAY_NULL_POKE;
    }
    
    for(i = 0;i < BSUBWAY_STOCK_TRAINER_MAX;i++){
      bsw_scr->trainer[i] = BSWAY_NULL_TRAINER;
    }
    
    BSUBWAY_PLAYDATA_Init( bsw_scr->playData ); //�v���C�f�[�^������
    BSUBWAY_PLAYDATA_ResetRoundNo( bsw_scr->playData );
    
    if( BSUBWAY_SCOREDATA_CheckExistStageNo( //�X�e�[�W���݂��邩�H
        bsw_scr->scoreData,bsw_scr->play_mode) == FALSE ){
      BSUBWAY_SCOREDATA_InitStageNo(  //�X�e�[�W������
          bsw_scr->scoreData, bsw_scr->play_mode );
      BSUBWAY_SCOREDATA_ResetRenshou( //�A�������Z�b�g
          bsw_scr->scoreData, bsw_scr->play_mode );
    }
    
    buf8 = bsw_scr->play_mode; //�v���C���[�h
    
    BSUBWAY_PLAYDATA_SetData(
        bsw_scr->playData, BSWAY_PLAYDATA_ID_playmode, &buf8 );
  }else{ //BSWAY_PLAY_CONTINUE ����
    bsw_scr->play_mode = (u8)BSUBWAY_PLAYDATA_GetData(
        bsw_scr->playData, BSWAY_PLAYDATA_ID_playmode, NULL );
    
    bsw_scr->member_num =
      BSUBWAY_SCRWORK_GetPlayModeMemberNum( bsw_scr->play_mode );
    
    //�o�g���{�b�N�X�g�p�ł���Ώ���
    if( (u32)BSUBWAY_PLAYDATA_GetData( bsw_scr->playData,
          BSWAY_PLAYDATA_ID_use_battle_box,NULL) != FALSE ){
      BSUBWAY_SCRWORK_PreparBattleBox( bsw_scr );
    }
    
    //�I�΂�Ă���|�P����No
    BSUBWAY_PLAYDATA_GetData( bsw_scr->playData,
        BSWAY_PLAYDATA_ID_pokeno, bsw_scr->member );
    
    //���I�ς݂̃g���[�i�[No
    BSUBWAY_PLAYDATA_GetData( bsw_scr->playData,
        BSWAY_PLAYDATA_ID_trainer, bsw_scr->trainer );
    
    //AI�}���`�Ȃ�p�[�g�i�[���A
    if( bsw_scr->play_mode == BSWAY_MODE_MULTI ||
        bsw_scr->play_mode == BSWAY_MODE_S_MULTI ){
      bsw_scr->partner = (u8)BSUBWAY_PLAYDATA_GetData(
          bsw_scr->playData, BSWAY_PLAYDATA_ID_partner, NULL );
      
      //�p�[�g�i�[�g���[�i�[�f�[�^�Đ���
      BSUBWAY_PLAYDATA_GetData( bsw_scr->playData,
          BSWAY_PLAYDATA_ID_pare_poke,
          &(bsw_scr->five_poke[bsw_scr->partner]) );
      
      {
        int i;
        int start_no = BSWAY_PARTNER_DATA_START_NO_WOMAN;
        PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
        u32 sex = MyStatus_GetMySex( &player->mystatus );
        
        if( sex ){ //woman
          start_no = BSWAY_PARTNER_DATA_START_NO_MAN;
        }
        
        BSUBWAY_SCRWORK_MakePartnerRomData(
          bsw_scr, &bsw_scr->five_data[bsw_scr->partner],
          start_no + bsw_scr->partner,
          BSUBWAY_PLAYDATA_GetData(bsw_scr->playData,
            BSWAY_PLAYDATA_ID_pare_itemfix, NULL ),
          &(bsw_scr->five_poke[bsw_scr->partner]),
          bsw_scr->heapID );
      }
    }
  }
  
  return( bsw_scr );
}

//--------------------------------------------------------------
/**
 * BSUBWAY_SCRWORK ���[�N�G���A���J������
 * @param bsw_scr BSUBWAY_SCRWORK*
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_ReleaseWork(
    GAMESYS_WORK *gsys, BSUBWAY_SCRWORK *bsw_scr )
{
  if( bsw_scr != NULL ){
    GF_ASSERT( bsw_scr->magicNo == BSUBWAY_SCRWORK_MAGIC );
    
    if( bsw_scr->btl_box_party != NULL ){
      GFL_HEAP_FreeMemory( bsw_scr->btl_box_party );
      bsw_scr->btl_box_party = NULL;
    }
    
    if( bsw_scr->btl_setup_param != NULL ){
      BATTLE_PARAM_Delete( bsw_scr->btl_setup_param );
      bsw_scr->btl_setup_param = NULL;
    }
    
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
 * BSUBWAY_SCRWORK�@�ʐM�}���`���[�h�֕ύX����
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_ChangeCommMultiMode( BSUBWAY_SCRWORK *bsw_scr )
{
  u8 buf[4];

  if( bsw_scr->play_mode == BSWAY_MODE_MULTI ){
    buf[0] = BSWAY_MODE_COMM_MULTI;
  }else if( bsw_scr->play_mode == BSWAY_MODE_S_MULTI ){
    buf[0] = BSWAY_MODE_S_COMM_MULTI;
  }
  
  bsw_scr->play_mode = buf[0];

  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_playmode, buf );
  
  BSUBWAY_SCOREDATA_InitStageNo( bsw_scr->scoreData, bsw_scr->play_mode );
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
  u16 i;
  u8 buf8[4];
  
  //�v���C���[�h�����o��
  buf8[0] = bsw_scr->play_mode;
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_playmode, buf8 );
  
  //�I�񂾃|�P����No
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_pokeno, bsw_scr->member );
  
#if 0  
  //�o�g�����я����o��
  BSUBWAY_PLAYDATA_AddWifiRecord( bsw_scr->playData,
    bsw_scr->rec_down, bsw_scr->rec_turn, bsw_scr->rec_damage );
#endif
  
  //���I���ꂽ�g���[�i�[No�����o��
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_trainer, bsw_scr->trainer );
  
  //�Z�[�u�t���O��L����ԂɃ��Z�b�g
  BSUBWAY_PLAYDATA_SetSaveFlag( bsw_scr->playData, TRUE );
  
  if( bsw_scr->play_mode == BSWAY_MODE_MULTI ||
      bsw_scr->play_mode == BSWAY_MODE_S_MULTI ){
    //AI�}���`���[�h�Ȃ�p�[�g�i�[���o���Ă���
    buf8[0] = bsw_scr->partner;
    BSUBWAY_PLAYDATA_SetData(
        bsw_scr->playData, BSWAY_PLAYDATA_ID_partner, buf8 );
    
    //�p�[�g�i�[�̃|�P�����p�����[�^�������Ă���
    BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
        BSWAY_PLAYDATA_ID_pare_poke,
        &(bsw_scr->five_poke[bsw_scr->partner]) );
    
    //�A�C�e�����Œ肾�������ǂ��������Ă���
    BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
        BSWAY_PLAYDATA_ID_pare_itemfix,
        &(bsw_scr->five_item[bsw_scr->partner]) );
  }
}

//--------------------------------------------------------------
/**
 * BSUBWAY_SCRWORK �|�P���������o�[�����[�h
 * @param bsw_scr BSUBWAY_SCRWORK*
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_LoadPokemonMember(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys )
{
  int i;
  const POKEPARTY *party;
  POKEMON_PARAM *pp;
  
  bsw_scr->member_num =
      BSUBWAY_SCRWORK_GetPlayModeMemberNum( bsw_scr->play_mode );
  
  BSUBWAY_PLAYDATA_GetData( bsw_scr->playData, //�I�񂾃|�P����No
      BSWAY_PLAYDATA_ID_pokeno, bsw_scr->member );
  
  party = BSUBWAY_SCRWORK_GetPokePartyUse( bsw_scr );
  
  for( i = 0; i < bsw_scr->member_num; i++ ){ //�I���|�P�����莝��No
    KAGAYA_Printf( "BSW LOAD POKEMON MEMBER [%d] = %d\n",
        i, bsw_scr->member[i] );
    pp = PokeParty_GetMemberPointer(
        (POKEPARTY *)party, bsw_scr->member[i] );
    bsw_scr->mem_poke[i] = PP_Get( pp, ID_PARA_monsno, NULL );  
    bsw_scr->mem_item[i] = PP_Get( pp, ID_PARA_item, NULL );  
  }
}

//--------------------------------------------------------------
/**
 * BSUBWAY_SCRWORK �Q�[���N���A���Ɍ��݂̃v���C�󋵂��Z�[�u�ɏ����o��
 * @param bsw_scr BSUBWAY_SCRWORK*
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_SaveGameClearPlayData( BSUBWAY_SCRWORK *bsw_scr )
{
  u16  i;
  u8  buf8[4];
  
  //�v���C���[�h�����o��
  buf8[0] = bsw_scr->play_mode;
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_playmode, buf8 );
  
  //���E���h�����Z�b�g
  BSUBWAY_PLAYDATA_ResetRoundNo( bsw_scr->playData );
  
  //�I�񂾃|�P����No
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_pokeno, bsw_scr->member );
  
#if 0  
  //�o�g�����я����o��
  BSUBWAY_PLAYDATA_AddWifiRecord( bsw_scr->playData,
    bsw_scr->rec_down, bsw_scr->rec_turn, bsw_scr->rec_damage );
#endif
  
  //�Z�[�u�t���O��L����ԂɃ��Z�b�g
  BSUBWAY_PLAYDATA_SetSaveFlag( bsw_scr->playData, TRUE );
  
  if( bsw_scr->play_mode == BSWAY_MODE_MULTI ||
      bsw_scr->play_mode == BSWAY_MODE_S_MULTI ){
    //AI�}���`���[�h�Ȃ�p�[�g�i�[���o���Ă���
    buf8[0] = bsw_scr->partner;
    BSUBWAY_PLAYDATA_SetData(
        bsw_scr->playData, BSWAY_PLAYDATA_ID_partner, buf8 );
  
    //�p�[�g�i�[�̃|�P�����p�����[�^�������Ă���
    BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
        BSWAY_PLAYDATA_ID_pare_poke,
        &(bsw_scr->five_poke[bsw_scr->partner]) );
    
    //�A�C�e�����Œ肾�������ǂ��������Ă���
    BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
        BSWAY_PLAYDATA_ID_pare_itemfix,
        &(bsw_scr->five_item[bsw_scr->partner]) );
  }
}

//--------------------------------------------------------------
/**
 * �G���[���̃��[�N�Z�b�g
 * @param GAMESYS_WORK *gsys
 * @retval u16 �v���C���Ă������[�h
 */
//--------------------------------------------------------------
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
  play_mode = BSUBWAY_PLAYDATA_GetData(
      playData, BSWAY_PLAYDATA_ID_playmode, NULL );
  
  //���݂̎��񐔃��Z�b�g
  BSUBWAY_PLAYDATA_ResetRoundNo( playData );
  
  //�X�e�[�W�����G���[��
  BSUBWAY_SCOREDATA_ErrorStageNo( scoreData, play_mode );
  
  //���R�[�h���풆�t���O�𗎂Ƃ�
#if 0 //wb null
  BSUBWAY_SCOREDATA_SetFlag( scoreData,
    BSWAY_SCOREDATA_FLAG_SINGLE_RECORD + play_mode,
    BSWAY_SETMODE_reset );
#endif

  //�A���L�^�N���A
  BSUBWAY_SCOREDATA_ResetRenshou( scoreData, play_mode );
  
  return play_mode;  
}

//--------------------------------------------------------------
/**
 * �s�펞�̃��[�N�Z�b�g
 * @param GAMESYS_WORK *gsys
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_SetLoseScore(
    GAMESYS_WORK *gsys, BSUBWAY_SCRWORK *bsw_scr )
{
  u16 play_mode = bsw_scr->play_mode;
  BSUBWAY_SCOREDATA *scoreData = bsw_scr->scoreData;
  
  //�A���L�^�X�V
  BSUBWAY_SCOREDATA_UpdateRenshouMax( scoreData, play_mode,
      BSUBWAY_SCOREDATA_GetRenshou(scoreData,play_mode) );
  
  { //�����������ʃf�[�^�쐬
    SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( bsw_scr->gdata );
    u16 renshou = BSUBWAY_SCOREDATA_GetRenshou(
        bsw_scr->scoreData, play_mode );
    bsw_SetCommonScore( bsw_scr, save, TRUE, renshou );
  }

  //���񐔃��Z�b�g
  BSUBWAY_PLAYDATA_ResetRoundNo( bsw_scr->playData );
  
  //�X�e�[�W�����G���[��
  BSUBWAY_SCOREDATA_ErrorStageNo( bsw_scr->scoreData, play_mode );
}

#if 0 //wb null
void BSUBWAY_SCRWORK_SetLoseScore(
    GAMESYS_WORK *gsys, BSUBWAY_SCRWORK *bsw_scr )
{
  u32  ret = 0;
  int  id;
  u16  before,after,chg_flg;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( gdata );
  RECORD *record = SaveControl_DataPtrGet( save, GMDATA_ID_RECORD );
  
  OS_Printf( "TowerLoseScoreSet -> mode = %d\n", bsw_scr->play_mode );
  
  id = bsw_scr->play_mode * 2 + RECID_RENSHOU_SINGLE;
  
  BSUBWAY_SCOREDATA_ResetRenshou( bsw_scr->scoreData, bsw_scr->play_mode );
  
  BSUBWAY_SCOREDATA_SetRenshou(
     bsw_scr->scoreData, BSWAY_SETMODE_reset );

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

  //���݂̎��񐔃��Z�b�g
  BSUBWAY_SCOREDATA_ResetStageCount(
      bsw_scr->scoreData, bsw_scr->play_mode );

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

  towerscr_SetCommonScore(wk,savedata,FALSE,ret);
}
#endif

#if 0
static void towerscr_SetCommonScore(BTOWER_SCRWORK* wk,SAVEDATA* sv,u8 win_f,u16 now_renshou)
{
	u8	buf8;

	switch(wk->play_mode){
	case BSWAY_MODE_SINGLE:
		//�|�P�����f�[�^�Z�b�g
		towerscr_SaveMemberPokeData(wk,sv,BTWR_SCORE_POKE_SINGLE);
	case BTWR_MODE_DOUBLE:
		if(now_renshou >= 7){
			//TV�C���^�r���[�f�[�^�Z�b�g(�V���O���ƃ_�u���Ŏ��s)
			TVTOPIC_BTowerTemp_Set(SaveData_GetTvWork(sv),win_f,now_renshou);
		}
		break;
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
	default:
		break;
	}
}
#endif


//--------------------------------------------------------------
//
//--------------------------------------------------------------
enum
{
  BTP_SINGLE,
  BTP_S_SINGLE,
  BTP_DOUBLE,
  BTP_S_DOUBLE,
  BTP_MULTI,
  BTP_S_MULTI,
  BTP_WIFI,
  BTP_TYPE_MAX,
};

static const u8 bpoint_tbl[BTP_TYPE_MAX][10] =
{
  //�V���O��
  {
    3, 3, 3, 0, 0,
    0, 0, 0, 0, 0
  },
  //�X�[�p�[�V���O��
  {
    5, 6, 7, 8, 9,
    10, 10, 10, 10, 10
  },
  //�_�u��
  {
    3, 3, 3, 0, 0,
    0, 0, 0, 0, 0
  },
  //�X�[�p�[�_�u��
  {
    5, 6, 7, 8, 9,
    10, 10, 10, 10, 10
  },
  //�}���`
  {
    3, 3, 3, 0, 0,
    0, 0, 0, 0, 0
  },
  //�X�[�p�[�}���`
  {
    5, 6, 7, 8, 9,
    10, 10, 10, 10, 10
  },
  //WiFi �����N��
  {
    10, 10, 10, 10, 10,
    10, 10, 10, 10, 10
  },
};

//--------------------------------------------------------------
/**
 * �o�g���|�C���g�ǉ�
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
u16  BSUBWAY_SCRWORK_AddBattlePoint( BSUBWAY_SCRWORK *bsw_scr )
{
  u16 point = 0;
  
  if( bsw_scr->play_mode == BSWAY_MODE_WIFI ){ //�����N����
    s8 rank = BSUBWAY_SCOREDATA_GetWifiRank( bsw_scr->scoreData );
    if( rank < 0 ){
      rank = 0;
    }else if( rank >= 10 ){
      rank = 9;
    }
    point = bpoint_tbl[BTP_WIFI][rank];
  }else{ //���񐔂���
    u16 type;
    BOOL super = FALSE;
    u16 stage = BSUBWAY_SCOREDATA_GetStageNo_Org0(
        bsw_scr->scoreData, bsw_scr->play_mode );
    
    switch( bsw_scr->play_mode ){
    case BSWAY_MODE_SINGLE:
      type = BTP_SINGLE;
      break;
    case BSWAY_MODE_S_SINGLE:
      type = BTP_S_SINGLE;
      super = TRUE;
      break;
    case BSWAY_MODE_DOUBLE:
      type = BTP_DOUBLE;
      break;
    case BSWAY_MODE_S_DOUBLE:
      type = BTP_S_DOUBLE;
      super = TRUE;
      break;
    case BSWAY_MODE_MULTI:
      type = BTP_MULTI;
      break;
    case BSWAY_MODE_COMM_MULTI:
      type = BTP_MULTI;
      break;
    case BSWAY_MODE_S_MULTI:
      type = BTP_S_MULTI;
      super = TRUE;
      break;
    case BSWAY_MODE_S_COMM_MULTI:
      type = BTP_S_MULTI;
      super = TRUE;
      break;
    case BSWAY_MODE_WIFI:
      type = BTP_WIFI;
      break;
    default:
      GF_ASSERT( 0 );
      type = BTP_SINGLE;
    }

    stage--;
    
    if( (s16)stage < 0 ){
      stage = 0;
    }else if( stage >= 10 ){
      stage = 9;
    }
    
    if( bsw_scr->boss_f ){
      if( super == TRUE ){
        point = 30;
      }else{
        point = 10;
      }
    }else{
      point = bpoint_tbl[type][stage];
    }
  }
  
  if( point == 0 ){
    point = 1;
  }
  
  BSUBWAY_SCOREDATA_AddBattlePoint( bsw_scr->scoreData, point );
  return point;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�i���o�[���Z�b�g
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_SetBtlTrainerNo( BSUBWAY_SCRWORK *bsw_scr )
{
  int i;
  u8 play_mode;
  u16 no,stage,round;
  
  play_mode = bsw_scr->play_mode;
  stage = BSUBWAY_SCOREDATA_GetStageNo_Org0( bsw_scr->scoreData, play_mode );
  
  if( play_mode == BSWAY_MODE_MULTI ||
      play_mode == BSWAY_MODE_COMM_MULTI ||
      play_mode == BSWAY_MODE_S_MULTI ||
      play_mode == BSWAY_MODE_S_COMM_MULTI )
  {
    if( stage < bsw_scr->pare_stage_no )
    {
      stage = bsw_scr->pare_stage_no;  //�ʐM���ɂ͎��񐔂̑����ق��Œ��I
    }
    
    for( i = 0; i < BSUBWAY_STOCK_TRAINER_MAX; i++ )
    {
      do
      {
        no = BSUBWAY_SCRWORK_GetTrainerNo(
            bsw_scr, stage, i/2, play_mode, (i)&0x01 );
      }while( is_ConflictTrainer(bsw_scr->trainer,no,i) );
      
      bsw_scr->trainer[i] = no;
    }
  }
  else
  {
    for( i = 0; i < (BSUBWAY_STOCK_TRAINER_MAX/2); i++ )
    {
      do
      {
        no = BSUBWAY_SCRWORK_GetTrainerNo(
            bsw_scr, stage, i, bsw_scr->play_mode, 0 );
      }while( is_ConflictTrainer(bsw_scr->trainer,no,i) );
      
      bsw_scr->trainer[i] = no;
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief  �I�������|�P�������擾
 * @param wk BSUBWAY_SCRWORK
 * @param gsys GAMESYS_WORK
 * @retval  BOOL TRUE  �I������ FALSE  �I�������ɂ�߂�
 */
//--------------------------------------------------------------
BOOL BSUBWAY_SCRWORK_GetEntryPoke( BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys )
{
  if( bsw_scr->pokelist_return_mode != PL_RET_NORMAL ||
      bsw_scr->pokelist_result_select == PL_SEL_POS_EXIT ||
      bsw_scr->pokelist_result_select == PL_SEL_POS_EXIT2 )
  {
    return FALSE;
  }
  else
  {
    u16  i = 0;
    const POKEPARTY *party;
    POKEMON_PARAM *pp;

    party = BSUBWAY_SCRWORK_GetPokePartyUse( bsw_scr );
    
    for( i = 0; i < bsw_scr->member_num; i++ ){ //�|�P�����I���̎莝��No
      if( (bsw_scr->pokelist_select_num[i]-1) >= 6 ){
        GF_ASSERT( 0 );
        bsw_scr->pokelist_select_num[i] = 1;
      }
      
      bsw_scr->member[i] = bsw_scr->pokelist_select_num[i] - 1;
      pp = PokeParty_GetMemberPointer(
          (POKEPARTY*)party, bsw_scr->member[i] );
      bsw_scr->mem_poke[i] = PP_Get( pp, ID_PARA_monsno, NULL );  
      bsw_scr->mem_item[i] = PP_Get( pp, ID_PARA_item, NULL );  
    }
    
    return TRUE;
  }
}

//--------------------------------------------------------------
/**
 *  @brief  7�A�����Ă��邩�ǂ����`�F�b�N
 *  @param bsw_scr BSUBWAY_SCRWORK
 *  @retval BOOL TRUE=�V�A�����Ă���
 */
//--------------------------------------------------------------
BOOL BSUBWAY_SCRWORK_IsClear( BSUBWAY_SCRWORK *bsw_scr )
{
#ifdef DEBUG_BSW_NORMAL_CLEAR
  switch( bsw_scr->play_mode ){
  case BSWAY_MODE_SINGLE:
  case BSWAY_MODE_DOUBLE:
  case BSWAY_MODE_MULTI:
  case BSWAY_MODE_COMM_MULTI:
    BSUBWAY_PLAYDATA_SetRoundNo( bsw_scr->playData, 7 );
    BSUBWAY_SCOREDATA_SetStageNo( bsw_scr->scoreData, bsw_scr->play_mode, 2 );
    BSUBWAY_SCOREDATA_SetRenshou(
        bsw_scr->scoreData, bsw_scr->play_mode, 7*3 );
    break;
  }
#endif

  if( bsw_scr->clear_f == FALSE ){
    u32 round = BSUBWAY_PLAYDATA_GetRoundNo( bsw_scr->playData );
    
#ifdef DEBUG_BSW_CLEAR_1
    if( round < 1 ){
      return( FALSE );
    }
#else
    if( round < BSWAY_CLEAR_WINCNT ){
      return( FALSE );
    }
#endif
    
    bsw_scr->clear_f = 1; //�N���A�t���Oon
  }
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * @brief  �N���A����
 * @param bsw_scr BSUBWAY_SCRWORK
 * @param gsys GAMESYS_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_SetClearScore(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys )
{
  u32 ret = 0;
  u16 before,after,chg_flg,play_mode;
  play_mode = bsw_scr->play_mode;
  
  //�ő�A�����X�V
  BSUBWAY_SCOREDATA_UpdateRenshouMax(
      bsw_scr->scoreData, play_mode,
      BSUBWAY_SCOREDATA_GetRenshou(bsw_scr->scoreData,play_mode) );
  
  //���񐔃v���X
  BSUBWAY_SCOREDATA_IncStageNo( bsw_scr->scoreData, play_mode );
  
  { //�����������ʃf�[�^�쐬
    SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( bsw_scr->gdata );
    u16 renshou = BSUBWAY_SCOREDATA_GetRenshou(
        bsw_scr->scoreData, play_mode );
    bsw_SetCommonScore( bsw_scr, save, TRUE, renshou );
  }
  
  //���E���h�����Z�b�g
  BSUBWAY_PLAYDATA_ResetRoundNo( bsw_scr->playData );
}

//--------------------------------------------------------------
/**
 * @brief  �z�[���ɒ������ۂ̃��[�N����
 * @param bsw_scr BSUBWAY_SCRWORK
 * @param gsys GAMESYS_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_SetHomeWork(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys )
{
  bsw_scr->clear_f = 0;
  bsw_scr->boss_f = 0;
}

//--------------------------------------------------------------
/**
 * �ΐ�g���[�i�[���I
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_ChoiceBattlePartner( BSUBWAY_SCRWORK *bsw_scr )
{
  int i;
  u16 round;
  u16 monsno[2];
  u16 itemno[2];
   
  round = BSUBWAY_PLAYDATA_GetRoundNo( bsw_scr->playData );
  
  switch( bsw_scr->play_mode ){
  case BSWAY_MODE_WIFI:
    BSUBWAY_WIFIDATA_GetBtlPlayerData( bsw_scr->wifiData,
        &bsw_scr->tr_data[0], round );
    break;
  case BSWAY_MODE_MULTI:
  case BSWAY_MODE_S_MULTI:
  case BSWAY_MODE_COMM_MULTI:
  case BSWAY_MODE_S_COMM_MULTI:
    BSUBWAY_SCRWORK_MakeRomTrainerData(
        bsw_scr, &(bsw_scr->tr_data[0]),
        bsw_scr->trainer[round*2+0],
        bsw_scr->member_num, NULL, NULL, NULL, bsw_scr->heapID );
    
    for( i = 0; i < bsw_scr->member_num; i++ ){ //�����X�^�[No�d���`�F�b�N
      monsno[i] = bsw_scr->tr_data[0].btpwd[i].mons_no;
      itemno[i] = bsw_scr->tr_data[0].btpwd[i].item_no;
    }
    
    BSUBWAY_SCRWORK_MakeRomTrainerData(
        bsw_scr, &(bsw_scr->tr_data[1]),
        bsw_scr->trainer[round*2+1],
        bsw_scr->member_num, monsno, itemno, NULL, bsw_scr->heapID );
    break;
  case BSWAY_MODE_SINGLE:
  case BSWAY_MODE_DOUBLE:
  case BSWAY_MODE_S_SINGLE:
  case BSWAY_MODE_S_DOUBLE:
  default:
    BSUBWAY_SCRWORK_MakeRomTrainerData(
      bsw_scr, &(bsw_scr->tr_data[0]),
      bsw_scr->trainer[round],
      bsw_scr->member_num, NULL, NULL, NULL, bsw_scr->heapID );
    break;
  }
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�^�C�v����OBJ�R�[�h���擾���Ă���
 * @param bsw_scr BSUBWAY_SCR
 * @param idx �g���[�i�[�f�[�^�C���f�b�N�X
 * @retval u16 OBJ�R�[�h
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCRWORK_GetTrainerOBJCode( BSUBWAY_SCRWORK *bsw_scr, u16 idx )
{
//  return BSUBWAY_GetTrainerOBJCode( bsw_scr->tr_data[idx].bt_trd.tr_type );   //100424del saito
    return  FBI_TOOL_GetTrainerOBJCode( bsw_scr->tr_data[idx].bt_trd.tr_type );
}

//--------------------------------------------------------------
/**
 * @brief  ���݂�WIFI�����N�𑀍삵�ĕԂ�
 * @param bsw_scr BSUBWAY_SCRWORK*
 * @param gsys GAMESYS_WORK
 * @param mode BSWAY_SETMODE
 * @retval u16 �����̃����N
 */
//--------------------------------------------------------------
#if 0
u16 BSUBWAY_SCRWORK_SetWifiRank(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys, BSWAY_SETMODE mode )
{
  u8 ct,rank;
  BSUBWAY_SCOREDATA *score = bsw_scr->scoreData;
  static const u8 btower_wifi_rankdown[] = {
   0,5,4,4,3,3,2,2,1,1,
  };

  switch( mode ){
  case BSWAY_SETMODE_get:
    return (u16)BSUBWAY_SCOREDATA_GetWifiRank( score );
  case BSWAY_SETMODE_inc:  //Inc
    BSUBWAY_SCOREDATA_SetFlag( score,  //�A���s��t���O�𗎂Ƃ�
      BSWAY_SCOREDATA_FLAG_WIFI_LOSE_F, BSWAY_SETMODE_reset);
    rank = BSUBWAY_SCOREDATA_GetWifiRank( score );
    
    if( rank == 10 ){  
      bsw_scr->prize_f = 1; //�����N10�Ȃ烊�{����Ⴆ������̓N���A(08.06.01)
      return 0;  //�����オ��Ȃ�
    }
    
    //�����N�A�b�v����
    BSUBWAY_SCOREDATA_IncWifiRank( score );
    
    //�����N5�ȏ�ɃA�b�v���Ă��烊�{�������炦��
    if( rank+1 >= 5 ){
      bsw_scr->prize_f = 1;
    }
    return 1;
  case BSWAY_SETMODE_dec:  //dec
    //���݂̘A���s�퐔���J�E���g
    ct = BSUBWAY_SCOREDATA_SetWifiLoseCount( score, BSWAY_SETMODE_inc );
    rank = BSUBWAY_SCOREDATA_GetWifiRank( score );
    
    if( rank == 1 ){
      return 0;
    }
    
    //�����N�ʔs��J�E���g�`�F�b�N
    if( ct >= btower_wifi_rankdown[rank-1] ){
      BSUBWAY_SCOREDATA_DecWifiRank( score ); //�����N�_�E��
      //�A���s�퐔�ƘA���s��t���O�����Z�b�g
      BSUBWAY_SCOREDATA_SetWifiLoseCount( score, BSWAY_SETMODE_reset );
      BSUBWAY_SCOREDATA_SetFlag( score,
        BSWAY_SCOREDATA_FLAG_WIFI_LOSE_F, BSWAY_SETMODE_reset );
      return 1;
    }
    return 0;
  }
  return 0;
}
#else
u16 BSUBWAY_SCRWORK_SetWifiRank(
    BSUBWAY_SCOREDATA *score, GAMESYS_WORK *gsys, BSWAY_SETMODE mode )
{
  u8 ct,rank;
  static const u8 btower_wifi_rankdown[10] = {
   0,5,4,4,3,3,2,2,1,1,
  };
  
  rank = (u16)BSUBWAY_SCOREDATA_GetWifiRank( score );
  
  switch( mode ){
  case BSWAY_SETMODE_get:
    return rank;
  case BSWAY_SETMODE_inc:  //Inc
    BSUBWAY_SCOREDATA_SetFlag( score,  //�A���s��t���O�𗎂Ƃ�
      BSWAY_SCOREDATA_FLAG_WIFI_LOSE_F, BSWAY_SETMODE_reset);
    
    if( rank == 10 ){  
      return FALSE;  //�����オ��Ȃ�
    }
    
    //�����N�A�b�v����
    BSUBWAY_SCOREDATA_IncWifiRank( score );
    return TRUE; //�����N�A�b�v����
  case BSWAY_SETMODE_dec:  //dec
    //���݂̘A���s�퐔���J�E���g
    ct = BSUBWAY_SCOREDATA_SetWifiLoseCount( score, BSWAY_SETMODE_inc );

    if( rank == 1 ){
      return FALSE;
    }
    
    //�����N�ʔs��J�E���g�`�F�b�N
    if( ct >= btower_wifi_rankdown[rank-1] ){
      BSUBWAY_SCOREDATA_DecWifiRank( score ); //�����N�_�E��
      
      //�A���s�퐔�ƘA���s��t���O�����Z�b�g
      BSUBWAY_SCOREDATA_SetWifiLoseCount( score, BSWAY_SETMODE_reset );
      BSUBWAY_SCOREDATA_SetFlag( score,
        BSWAY_SCOREDATA_FLAG_WIFI_LOSE_F, BSWAY_SETMODE_reset );
      return TRUE; //�����N�_�E������
    }
    
    return FALSE;
  }
  return 0;
}
#endif

//--------------------------------------------------------------
/**
 * �o�g���{�b�N�X�g�p����
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_PreparBattleBox( BSUBWAY_SCRWORK *bsw_scr )
{
  GF_ASSERT( BSUBWAY_PLAYDATA_GetData(
        bsw_scr->playData,BSWAY_PLAYDATA_ID_use_battle_box, NULL ) );

  if( bsw_scr->btl_box_party != NULL ){
    GF_ASSERT( 0 );
  }else{
    SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( bsw_scr->gdata );
    BATTLE_BOX_SAVE *bb_save = BATTLE_BOX_SAVE_GetBattleBoxSave( save );
    bsw_scr->btl_box_party =
      BATTLE_BOX_SAVE_MakePokeParty( bb_save, HEAPID_PROC );
  }
}

//--------------------------------------------------------------
/**
 * �g�p����POKEPARTY�擾
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval POKEPARTY*
 */
//--------------------------------------------------------------
const POKEPARTY * BSUBWAY_SCRWORK_GetPokePartyUse(
    const BSUBWAY_SCRWORK *bsw_scr )
{
  const POKEPARTY *party;
  u32 use_bbox = (u32)BSUBWAY_PLAYDATA_GetData(
      bsw_scr->playData, BSWAY_PLAYDATA_ID_use_battle_box, NULL );
  
  if( use_bbox == FALSE ){
    party = GAMEDATA_GetMyPokemon( bsw_scr->gdata ); 
  }else{
    party = bsw_scr->btl_box_party; //�o�g���{�b�N�X�pPOKEPARTY
  }
  
  GF_ASSERT( party != NULL );
  return( party );
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
u32 BSUBWAY_SCRWORK_GetPlayModeMemberNum( u16 mode )
{
  switch(mode){
  case BSWAY_MODE_SINGLE:
  case BSWAY_MODE_S_SINGLE:
  case BSWAY_MODE_WIFI:
    return 3;
  case BSWAY_MODE_DOUBLE:
  case BSWAY_MODE_S_DOUBLE:
    return 4;
  case BSWAY_MODE_MULTI:
  case BSWAY_MODE_S_MULTI:
  case BSWAY_MODE_COMM_MULTI:
  case BSWAY_MODE_S_COMM_MULTI:
    return 2;
  }
  GF_ASSERT( 0 );
  return 0;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[ID�d���`�F�b�N
 * @param trainer �g���[�i�[ID���i�[���ꂽ�e�[�u��
 * @param id �`�F�b�N����ID
 * @param num trainer�v�f��
 * @retval BOOL TRUE=�d�����Ă���
 */
//--------------------------------------------------------------
static BOOL is_ConflictTrainer( u16 * trainer, u16 id, u16 num )
{
  u16 i;

  for( i = 0;i < num;i++){
    if( trainer[i] == id ){
      OS_Printf( "BSW is_ConflictTrainer() CONFLICT!! : " );
      OS_Printf( "TR NO = %d : DATA NO = %d : HIT NO = %d\n", id, num, i );
      return TRUE;
    }
  }
  return FALSE;
}

//======================================================================
//  wb null
//======================================================================
//--------------------------------------------------------------
/**
 * ����ڑ���Ɍ��݂̃��P�[�V�������Z�b�g
 * @param
 * @retval
 */
//--------------------------------------------------------------
#if 0 //wb null
void BSUBWAY_SCRWORK_PushNowLocation( GAMEDATA *gdata )
{
  GAMEDATA_SetSpecialLocation( gdata, &loc );
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  �o�g���^���[�p�|�P�����I����ʌĂяo���ďo��
 */
//--------------------------------------------------------------
#if 0 //wb null
void BSUBWAY_SCRWORK_SelectPoke(
    BSUBWAY_SCRWORK *wk, GMEVENT *ev, void **proc_wk )
{
  EventCmd_BTowerPokeSelectCall( ev_work,proc_wk,
    PL_MODE_BATTLE_TOWER,PST_MODE_NORMAL,
    wk->member_num,wk->member_num,100,0);
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  �Q���w�肵���莝���|�P�����̏����`�F�b�N
 *  @retval  0  �Q��OK
 *  @retval  1  �����|�P����������
 *  @retval 2  �����A�C�e���������Ă���
 */
//--------------------------------------------------------------
#if 0 //wb null
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
#endif

//--------------------------------------------------------------
/**
 *  @brief�@�ΐ�g���[�i�[No���I
 */
//--------------------------------------------------------------
#if 0 //wb null
void BSUBWAY_SCRWORK_BtlTrainerNoSet( BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys )
{
#if 0 //wb
  int i;
  u16  no,stage;
  
  if(  wk->play_mode == BSWAY_MODE_MULTI ||
    wk->play_mode == BSWAY_MODE_WIFI_MULTI ||
    wk->play_mode == BSWAY_MODE_COMM_MULTI ){
    //if( wk->play_mode == BSWAY_MODE_COMM_MULTI && wk->pare_round > wk->stage ){
    if( ( wk->play_mode == BSWAY_MODE_COMM_MULTI && wk->pare_round > wk->stage ) ||
      ( wk->play_mode == BSWAY_MODE_WIFI_MULTI && wk->pare_round > wk->stage ) ){
      stage = wk->pare_round;  //�ʐM���ɂ͎��񐔂̑����ق��Œ��I
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
#endif

//--------------------------------------------------------------
/**
 *  @brief  �ő�A���L�^�X�V�ԑg�쐬�֐�
 */
//--------------------------------------------------------------
#if 0 //wb null
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
static void bsw_SetCommonScore(
    BSUBWAY_SCRWORK *wk, SAVE_CONTROL_WORK *sv, u8 win_f, u16 now_renshou )
{
  u8  buf8;
  
  switch( wk->play_mode ){
  case BSWAY_MODE_SINGLE:
    //�|�P�����f�[�^�Z�b�g
    bsw_SaveMemberPokeData( wk,sv,BSWAY_SCORE_POKE_SINGLE );
  case BSWAY_MODE_DOUBLE:
#if 0 //wb null
    if( now_renshou >= 7){
      //TV�C���^�r���[�f�[�^�Z�b�g(�V���O���ƃ_�u���Ŏ��s)
      TVTOPIC_BTowerTemp_Set(
          SaveData_GetTvWork( sv ),win_f,now_renshou );
    }
#endif
    break;
  case BSWAY_MODE_WIFI:
    //�|�P�����f�[�^�Z�b�g
    bsw_SaveMemberPokeData( wk, sv, BSWAY_SCORE_POKE_WIFI );

#if 0    
    //�X�R�A�����o��
    BSUBWAY_PLAYDATA_AddWifiRecord(
        wk->playData, wk->rec_down, wk->rec_turn, wk->rec_damage );
#endif
  
    //�v���C���[�h�����o��
    buf8 = wk->play_mode;
    BSUBWAY_PLAYDATA_SetData(
        wk->playData, BSWAY_PLAYDATA_ID_playmode, &buf8 );
    //���E���h�������o��
    buf8 = BSUBWAY_PLAYDATA_GetRoundNo( wk->playData ) + 1;
    BSUBWAY_PLAYDATA_SetData( wk->playData, BSWAY_PLAYDATA_ID_round, &buf8 );
    BSUBWAY_SCOREDATA_SetWifiScore( wk->scoreData, wk->playData );
    break;
  default:
    break;
  }
}

#if 0 //wb null
void BSUBWAY_SCRWORK_SetClearScore( BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys )
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
  
  //�A���L�^�����o��
  {
    u32 renshou = BSUBWAY_SCOREDATA_GetRenshou(
        wk->scoreData, wk->play_mode );
    
    if( renshou + wk->now_win > 0xffff ){
      renshou = 0xffff;
    }else{
      renshou += wk->now_win;
    }
#if 0   
    BSUBWAY_SCOREDATA_SetRenshouCount(
          wk->scoreData, wk->play_mode, renshou );
#endif
  }
}
#endif

void BSUBWAY_SCRWORK_ChoiceBtlSeven( BSUBWAY_SCRWORK *wk, u8 sex )
{
  int i;
  int start_no = BSWAY_PARTNER_DATA_START_NO_MAN;
  
  if( sex ){
    start_no = BSWAY_PARTNER_DATA_START_NO_WOMAN;
  }
  
  for(i = 0;i < BSWAY_PARTNER_NUM;i++){
    wk->five_item[i] = BSUBWAY_SCRWORK_MakeRomTrainerData(
        wk, &(wk->five_data[i]),
        start_no + i, wk->member_num,
        wk->mem_poke, wk->mem_item, &(wk->five_poke[i]), wk->heapID );
  }
}

// @brief  AI�}���`�y�A�|�P�������I
#if 0 //wb
void BSUBWAY_SCRWORK_ChoiceBtlSeven( BSUBWAY_SCRWORK *wk )
{
  int i;
  
  for( i = 0;i < BSWAY_FIVE_NUM;i++){
    wk->five_item[i] = ( u8)RomBattleTowerTrainerDataMake( wk,&( wk->five_data[i]),
      TOWER_FIVE_FIRST+i,wk->member_num,wk->mem_poke,wk->mem_item,&( wk->five_poke[i]),wk->heapID );
  }
}
#endif

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
#if 0 // wb
u16 BSUBWAY_SCRWORK_GetPlayMode( BSUBWAY_SCRWORK *wk )
{
  return wk->play_mode;
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  ���[�_�[�N���A�t���O���擾����
 */
//--------------------------------------------------------------
#if 0 // wb
u16 BSUBWAY_SCRWORK_GetLeaderClearFlag( BSUBWAY_SCRWORK *wk )
{
  return (u16)wk->leader_f;
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  �^���[�N���A���Ƀo�g���|�C���g�����Z����
 *  @return  �V���Ɏ擾�����o�g���|�C���g
 */
//--------------------------------------------------------------
#if 0 //wb
u16 BSUBWAY_SCRWORK_AddBtlPoint( BSUBWAY_SCRWORK *wk )
{
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
  return 1;
}
#endif

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
static void bsw_PokeDataPack( BSUBWAY_POKEMON *dat, const POKEMON_PARAM *pp )
{
  int i;
  
  //mons_no/form_no/item_no
  dat->mons_no = PP_Get( pp,ID_PARA_monsno,NULL );
  dat->form_no = PP_Get( pp,ID_PARA_form_no,NULL );
  dat->item_no = PP_Get( pp,ID_PARA_item,NULL );
  
  //waza/pp_count
  for( i = 0;i < WAZA_TEMOTI_MAX;i++){
    dat->waza[i] = PP_Get( pp,ID_PARA_waza1+i,NULL );
    dat->pp_count |= ((PP_Get( pp, ID_PARA_pp_count1+i, NULL )) << (i*2));
  }
  //country,id,personal
  dat->country_code = PP_Get( pp,ID_PARA_country_code,NULL );
  dat->id_no = PP_Get( pp,ID_PARA_id_no,NULL );
  dat->personal_rnd = PP_Get( pp,ID_PARA_personal_rnd,NULL );

  //power_rnd
  dat->power_rnd = PP_Get( pp,ID_PARA_power_rnd,NULL );

  //exp
  for( i = 0;i < 6;i++){
    dat->exp[i] = PP_Get( pp,ID_PARA_hp_exp+i,NULL );
  }
  //tokusei,natukido
  dat->tokusei = PP_Get( pp,ID_PARA_speabino,NULL );
  dat->natuki = PP_Get( pp,ID_PARA_friend,NULL );
  
  //�j�b�N�l�[��
  PP_Get( pp,ID_PARA_nickname_raw,dat->nickname );
}

//--------------------------------------------------------------
/**
 *  @brief  �Q�������|�P�����̃p�����[�^��ۑ�����
 */
//--------------------------------------------------------------
static void bsw_SaveMemberPokeData(
    BSUBWAY_SCRWORK *wk, SAVE_CONTROL_WORK *sv, BSWAY_SCORE_POKE_DATA mode )
{
  int i = 0;
  BSUBWAY_POKEMON *dat;
  const POKEPARTY  *party;
  POKEMON_PARAM *pp;
  
  dat = GFL_HEAP_AllocMemoryLo( wk->heapID, sizeof(BSUBWAY_POKEMON)*3 );
  MI_CpuClear8( dat,sizeof(BSUBWAY_POKEMON)*3);

#if 0  
  party = SaveData_GetTemotiPokemon( sv );
#else
  party = BSUBWAY_SCRWORK_GetPokePartyUse( wk );
#endif

  for( i = 0;i < 3; i++ ){
    pp = PokeParty_GetMemberPointer( party, wk->member[i] );
    bsw_PokeDataPack( &(dat[i]), pp );  
  }
  
  BSUBWAY_SCOREDATA_SetUsePokeData( wk->scoreData,mode, dat );

  MI_CpuClear8( dat,sizeof( BSUBWAY_POKEMON )*3);
  GFL_HEAP_FreeMemory( dat );
}

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
