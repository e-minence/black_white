//============================================================================================
/**
 * @file	fss_tower_sub.c
 * @bfief	�t�����e�B�A�V�X�e���X�N���v�g�R�}���h�T�u�F�^���[
 * @author	Satoshi Nohara
 * @date	07.05.25
 *
 * b_tower_scr.c�̏������ړ�
 *
 * �֘A�\�[�X	field/scr_btower.c		�풓
 *				field/b_tower_scr.c		�t�B�[���h�T�u
 *				field/b_tower_ev.c		�풓
 *				field/b_tower_wifi.c	�풓
 *				field/b_tower_fld.c		�t�B�[���h�T�u
 */
//============================================================================================
#include "common.h"
#include "savedata/savedata_def.h"
#include "savedata/record.h"
#include "savedata/frontier_savedata.h"
#include "savedata/fnote_mem.h"
#include "system/buflen.h"
#include "savedata/tv_work.h"
#include "poketool/poke_tool.h"
#include "poketool/pokeparty.h"
#include "system/procsys.h"
#include "system/gamedata.h"
#include "gflib/strbuf_family.h"
#include "application/pokelist.h"
#include "../field/fieldsys.h"
#include "field/situation.h"
#include "field/location.h"
#include "../field/tv_topic.h"
#include "field/tvtopic_extern.h"
#include "field/field_encount.h"
#include "savedata/undergrounddata.h"
#include "savedata/b_tower.h"
#include "field/eventflag.h"
#include "field/evwkdef.h"
#include "../field/sysflag.h"
#include "../field/syswork.h"
//�t�����e�B�A�V�X�e��
#include "frontier_tool.h"
#include "../field/scr_tool.h"				//FactoryScr_GetWinRecordID
#include "itemtool/itemsym.h"
//�ʐM
//#include "communication/comm_system.h"
//#include "communication/comm_tool.h"
//#include "communication/comm_def.h"
//�o�g���^���[
#include "../field/b_tower_scr.h"			//BTOWER_SCRWORK�̐錾������
#include "../field/b_tower_ev.h"
#include "tower_tool.h"


//============================================================================================
//
//	�v���g�^�C�v�錾
//
//	�֐��̓���FS������(�t�����e�B�A�V�X�e���̗�)
//
//	(b_tower_scr.c��Tools�����ɂ��Ă���̂Ƃ��Ă��Ȃ��̂�����̂́H)
//
//============================================================================================
u16	FSTowerScrTools_GetMemberNum(u16 mode);
void FSTowerScrTools_SystemReset(void);
void FSTowerScrTools_ClearPlaySaveData(BTLTOWER_PLAYWORK* playSave);
BOOL FSTowerScrTools_IsSaveDataEnable(BTLTOWER_PLAYWORK* playSave);
u16	FSTowerScrTools_GetRenshouRecord(SAVEDATA* sv,u16 play_mode);
void FSTowerScrTools_SetWifiUploadFlag(SAVEDATA* sv,u8 flag);
u16 FSTowerScrTools_GetWifiUploadFlag(SAVEDATA* sv);
u16 FSTowerScrTools_SetNGScore(SAVEDATA* savedata);
u16	FSTowerScrTools_IsPlayerDataEnable(SAVEDATA* sv);
//����Tools�����Ă��Ȃ�
u16	FSTowerScr_SetWifiRank(BTOWER_SCRWORK* wk,SAVEDATA* sv,u8 mode);
u16	FSTowerScr_IncRound(BTOWER_SCRWORK* wk);
BOOL FSTowerScr_IsClear(BTOWER_SCRWORK* wk);
u16	FSTowerScr_GetPlayMode(BTOWER_SCRWORK* wk);
void FSTowerScr_ChoiceBtlPartner(BTOWER_SCRWORK* wk,SAVEDATA* sv);
u16 FSTowerScr_GetEnemyObj(BTOWER_SCRWORK* wk,u16 idx);
//void FSTowerScr_LocalBattleCall(GMEVENT_CONTROL* event,BTOWER_SCRWORK* wk,BOOL* win_flag);
//void FSTowerScr_SetLoseScore(BTOWER_SCRWORK* wk,FIELDSYS_WORK* fsys);
void FSTowerScr_SetLoseScore(BTOWER_SCRWORK* wk,SAVEDATA* savedata);
void FSTowerScr_SetClearScore(BTOWER_SCRWORK* wk,SAVEDATA* savedata,FNOTE_DATA* fnote);
void FSTowerScr_WorkRelease(BTOWER_SCRWORK* wk);
static u16 towerscr_IfRenshouPrizeGet(BTOWER_SCRWORK* wk);
static void towerscr_SetCommonScore(BTOWER_SCRWORK* wk,SAVEDATA* sv,u8 win_f,u16 now_renshou);
static void towerscr_SaveMemberPokeData(BTOWER_SCRWORK* wk,SAVEDATA* sv,BTWR_SCORE_POKE_DATA mode);
//static void towerscr_MakeTVRenshouMaxUpdate(BTOWER_SCRWORK* wk,FIELDSYS_WORK* fsys,u16 renshou);
static void towerscr_MakeTVRenshouMaxUpdate(BTOWER_SCRWORK* wk,SAVEDATA* savedata,u16 renshou);
static void towerscr_PokeDataPack(B_TOWER_POKEMON* dat,POKEMON_PARAM* pp);
u16 FSTowerScr_GetRenshouCount(BTOWER_SCRWORK* wk);
u16	FSTowerScr_GetNowRound(BTOWER_SCRWORK* wk);
void FSTowerScr_SaveRestPlayData(BTOWER_SCRWORK* wk);
void FSTowerScr_SetLeaderClearFlag(BTOWER_SCRWORK* wk,u16 mode);


//============================================================================================
//
//	�֐�
//
//============================================================================================

//--------------------------------------------------------------
/**
 *	@brief	�v���C���[�h�ʂ̃����o�[�����擾����
 *
 *	@param	mode	BTWR_MODE_�`
 */
//--------------------------------------------------------------
u16	FSTowerScrTools_GetMemberNum(u16 mode)
{
	switch(mode){
	case BTWR_MODE_SINGLE:
	case BTWR_MODE_WIFI:
	case BTWR_MODE_RETRY:
		return 3;
	case BTWR_MODE_DOUBLE:
		return 4;
	case BTWR_MODE_MULTI:
	case BTWR_MODE_COMM_MULTI:
	case BTWR_MODE_WIFI_MULTI:
		return 2;
	}
	return 0;
}

//--------------------------------------------------------------
/**
 *	@brief	���Z�b�g���ă^�C�g���ɖ߂�
 */
//--------------------------------------------------------------
void FSTowerScrTools_SystemReset(void)
{
	OS_ResetSystem(0);
}

//--------------------------------------------------------------
/**
 *	@brief	new game���Ƀv���C�f�[�^���N���A����
 */
//--------------------------------------------------------------
void FSTowerScrTools_ClearPlaySaveData(BTLTOWER_PLAYWORK* playSave)
{
	TowerPlayData_Clear(playSave);
}

//--------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^���L�����ǂ����Ԃ�
 */
//--------------------------------------------------------------
BOOL FSTowerScrTools_IsSaveDataEnable(BTLTOWER_PLAYWORK* playSave)
{
	return TowerPlayData_GetSaveFlag(playSave);
}

//--------------------------------------------------------------
/**
 *	@brief	�v���C���[�h�ʃ��R�[�h�Z�[�u�f�[�^�A������Ԃ�
 */
//--------------------------------------------------------------
u16	FSTowerScrTools_GetRenshouRecord(SAVEDATA* sv,u16 play_mode)
{
	u32 id;
	u16 val;
	if(play_mode == BTWR_MODE_RETRY){
		return 0;
	}

	if(play_mode == BTWR_MODE_WIFI_MULTI){
		id = FRID_TOWER_MULTI_WIFI_RENSHOU_CNT, 
		val= FrontierRecord_Get(SaveData_GetFrontier(sv), id, Frontier_GetFriendIndex(id) );
		return val;
	}

	id = FRID_TOWER_SINGLE_RENSHOU_CNT+play_mode*2, 
	val= FrontierRecord_Get(SaveData_GetFrontier(sv), id, Frontier_GetFriendIndex(id));

	return val;
}

//--------------------------------------------------------------
/**
 *	@brief	���݂�WIFI�����N�𑀍삵�ĕԂ�
 */
//--------------------------------------------------------------
u16	FSTowerScr_SetWifiRank(BTOWER_SCRWORK* wk,SAVEDATA* sv,u8 mode)
{
	u8	ct,rank;
	BTLTOWER_SCOREWORK* score = SaveData_GetTowerScoreData(sv);
	
	static const u8	btower_wifi_rankdown[] = {
	 0,5,4,4,3,3,2,2,1,1,
//	 0,2,1,1,1,1,1,1,1,1,
	};

	switch(mode){
	//Get
	case 0:
		return (u16)TowerScoreData_SetWifiRank(score,BTWR_DATA_get);
	case 1:	//Inc
		//�A���s��t���O�𗎂Ƃ�
		TowerScoreData_SetFlags(score,BTWR_SFLAG_WIFI_LOSE_F,BTWR_DATA_reset);
		//���݂̃����N���擾
		rank = TowerScoreData_SetWifiRank(score,BTWR_DATA_get);

		if(rank == 10){	
			return 0;	//�����オ��Ȃ�
		}
		//�����N�A�b�v����
		TowerScoreData_SetWifiRank(score,BTWR_DATA_inc);

		//�����N5�ȏ�ɃA�b�v���Ă��烊�{�������炦��
		if(rank+1 >= 5){
			wk->prize_f = 1;
		}
		return 1;
	case 2:	//dec
		//���݂̘A���s�퐔���J�E���g
		ct = TowerScoreData_SetWifiLoseCount(score,BTWR_DATA_inc);
		rank = TowerScoreData_SetWifiRank(score,BTWR_DATA_get);

		if(rank == 1){
			return 0;
		}
		//�����N�ʔs��J�E���g�`�F�b�N
		if(ct >= btower_wifi_rankdown[rank-1] ){
			//�����N�_�E��
			TowerScoreData_SetWifiRank(score,BTWR_DATA_dec);
			//�A���s�퐔�ƘA���s��t���O�����Z�b�g
			TowerScoreData_SetWifiLoseCount(score,BTWR_DATA_reset);
			TowerScoreData_SetFlags(score,BTWR_SFLAG_WIFI_LOSE_F,BTWR_DATA_reset);
			return 1;
		}
		return 0;
	}
	return 0;
}

//--------------------------------------------------------------
/**
 *	@brief	Wifi���т̖��A�b�v���[�h�t���O�𐧌䂷��
 */
//--------------------------------------------------------------
void FSTowerScrTools_SetWifiUploadFlag(SAVEDATA* sv,u8 flag)
{
	BTLTOWER_SCOREWORK	*score = SaveData_GetTowerScoreData(sv);
	
	if(flag == 0){	//���Z�b�g
		TowerScoreData_SetFlags(score,BTWR_SFLAG_WIFI_UPLOAD,BTWR_DATA_reset);
	}else{	//�Z�b�g
		TowerScoreData_SetFlags(score,BTWR_SFLAG_WIFI_UPLOAD,BTWR_DATA_set);
	}
}

//--------------------------------------------------------------
/**
 *	@brief	WiFi�v���C���т̖��A�b�v���[�h�t���O���擾����
 */
//--------------------------------------------------------------
u16 FSTowerScrTools_GetWifiUploadFlag(SAVEDATA* sv)
{
	BTLTOWER_SCOREWORK	*score = SaveData_GetTowerScoreData(sv);
	return (u16)TowerScoreData_SetFlags(score,BTWR_SFLAG_WIFI_UPLOAD,BTWR_DATA_get);
}

//--------------------------------------------------------------
/**
 *	@brief	�Z�[�u�����Ɏ~�߂��Ƃ��̃G���[����
 *
 *	@return	�v���C���Ă������[�h��Ԃ�
 */
//--------------------------------------------------------------
u16 FSTowerScrTools_SetNGScore(SAVEDATA* savedata)
{
	u8	play_mode;
	int id;
	BTLTOWER_PLAYWORK* playSave;
	BTLTOWER_SCOREWORK* scoreSave;
//	RECORD*  record;
	
	//�Z�[�u�f�[�^�擾
	playSave = SaveData_GetTowerPlayData(savedata);
	scoreSave = SaveData_GetTowerScoreData(savedata);

	//�ǂ̃��[�h���v���C���Ă������H
	play_mode = (u8)TowerPlayData_Get(playSave,BTWR_PSD_playmode,NULL);
	
	if(play_mode == BTWR_MODE_RETRY){
		return play_mode;
	}

	//���R�[�h���풆�t���O�𗎂Ƃ�
	if(play_mode == BTWR_MODE_WIFI_MULTI){
#if 0
		TowerScoreData_SetFlags(scoreSave,BTWR_SFLAG_WIFI_MULTI_RECORD,BTWR_DATA_reset);
#else
		FrontierRecord_Set(	SaveData_GetFrontier(savedata), 
							FRID_TOWER_MULTI_WIFI_CLEAR_BIT,
							Frontier_GetFriendIndex(FRID_TOWER_MULTI_WIFI_CLEAR_BIT), 0 );
#endif
	}else{
		TowerScoreData_SetFlags(scoreSave,BTWR_SFLAG_SINGLE_RECORD+play_mode,BTWR_DATA_reset);
	}

	//���݂̎��񐔃��Z�b�g
	TowerScoreData_SetStage(scoreSave,play_mode,BTWR_DATA_reset);
	if(play_mode != BTWR_MODE_WIFI){
		//�v���C�����_���V�[�h���ЂƂ����Ői�߂�
		BtlTower_UpdatePlayRndSeed(savedata);
	}

	OS_Printf("TowerNGScoreSet -> mode = %d\n",play_mode);
	return play_mode;	
}

//--------------------------------------------------------------
/**
 *	@brief	WIFI�̃v���C���[DL�f�[�^�����邩�ǂ����`�F�b�N
 */
//--------------------------------------------------------------
u16	FSTowerScrTools_IsPlayerDataEnable(SAVEDATA* sv)
{
	BTLTOWER_WIFI_DATA	*wifiSave;
	wifiSave = SaveData_GetTowerWifiData(sv);

	return (u16)TowerWifiData_IsPlayerDataEnable(wifiSave);
}

//--------------------------------------------------------------
/**
 *	@brief	���E���h�����C���N�������g
 */
//--------------------------------------------------------------
u16	FSTowerScr_IncRound(BTOWER_SCRWORK* wk)
{
	++wk->now_win;	//���������C���N�������g

	//�����_���̎�X�V
	wk->play_rnd_seed = BtlTower_PlayFixRand(wk->play_rnd_seed);

	OS_Printf("TowerRoundInc = %d\n",wk->play_rnd_seed);

	return ++wk->now_round;
}

//--------------------------------------------------------------
/**
 *	@brief	7�A�����Ă��邩�ǂ����`�F�b�N
 */
//--------------------------------------------------------------
BOOL FSTowerScr_IsClear(BTOWER_SCRWORK* wk)
{
	if(wk->clear_f){
		return TRUE;
	}
	if(wk->now_round > BTWR_CLEAR_WINCNT){
		//�N���A�t���Oon
		wk->clear_f = 1;
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 *	@brief	���݂̃v���C���[�h��Ԃ�
 */
//--------------------------------------------------------------
u16	FSTowerScr_GetPlayMode(BTOWER_SCRWORK* wk)
{
	//OS_Printf( "wk->play_mode = %d\n", wk->play_mode );
	return wk->play_mode;
}

//--------------------------------------------------------------
/**
 *	@brief	�ΐ�g���[�i�[���I
 */
//--------------------------------------------------------------
void FSTowerScr_ChoiceBtlPartner(BTOWER_SCRWORK* wk,SAVEDATA* sv)
{
	int i;
	u16	monsno[2];
	u16	itemno[2];
	
	switch(wk->play_mode){
	case BTWR_MODE_WIFI:
	case BTWR_MODE_RETRY:
		btltower_BtlPartnerSelectWifi(sv,wk->tr_data,wk->now_round-1);
		break;
	case BTWR_MODE_MULTI:
	case BTWR_MODE_COMM_MULTI:
	case BTWR_MODE_WIFI_MULTI:
		FSRomBattleTowerTrainerDataMake(wk,&(wk->tr_data[0]),
			wk->trainer[(wk->now_round-1)*2+0],wk->member_num,NULL,NULL,NULL,wk->heapID);
		
		//�����X�^�[No�̏d���`�F�b�N
		for(i = 0;i < wk->member_num;i++){
			monsno[i] = wk->tr_data[0].btpwd[i].mons_no;
			itemno[i] = wk->tr_data[0].btpwd[i].item_no;
		}
		FSRomBattleTowerTrainerDataMake(wk,&(wk->tr_data[1]),
			wk->trainer[(wk->now_round-1)*2+1],wk->member_num,monsno,itemno,NULL,wk->heapID);
		break;
	case BTWR_MODE_DOUBLE:
	case BTWR_MODE_SINGLE:
	default:
		FSRomBattleTowerTrainerDataMake(wk,&(wk->tr_data[0]),
			wk->trainer[wk->now_round-1],wk->member_num,NULL,NULL,NULL,wk->heapID);
		break;
	}
}

//--------------------------------------------------------------
/**
 *	@brief	�ΐ�g���[�i�[OBJ�R�[�h�擾
 */
//--------------------------------------------------------------
u16 FSTowerScr_GetEnemyObj(BTOWER_SCRWORK* wk,u16 idx)
{
	//�g���[�i�[�^�C�v����OBJ�R�[�h���擾���Ă���
	return Frontier_TrType2ObjCode(wk->tr_data[idx].bt_trd.tr_type);
}

#if 0
//--------------------------------------------------------------
/**
 *	@brief	�퓬�Ăяo��
 */
//--------------------------------------------------------------
void FSTowerScr_LocalBattleCall(GMEVENT_CONTROL* event,BTOWER_SCRWORK* wk,BOOL* win_flag)
{
	EventCmd_TowerLocalBattle(event,wk,win_flag);
}
#endif

//--------------------------------------------------------------
/**
 *	@brief	�s�폈��	
 */
//--------------------------------------------------------------
void FSTowerScr_SetLoseScore(BTOWER_SCRWORK* wk,SAVEDATA* savedata)
{
	u32	ret = 0;
	int	id;
	u16	before,after,chg_flg;
	RECORD* record = SaveData_GetRecord(savedata);
	FRONTIER_SAVEWORK *frontier = SaveData_GetFrontier(savedata);
	
	if(wk->play_mode == BTWR_MODE_RETRY){
		return;
	}
	
	OS_Printf("TowerLoseScoreSet -> mode = %d\n",wk->play_mode);

	if(wk->play_mode == BTWR_MODE_WIFI_MULTI){
		id = FRID_TOWER_MULTI_WIFI_RENSHOU_CNT;
	}else{
		id = wk->play_mode*2+FRID_TOWER_SINGLE_RENSHOU_CNT;
	}

	//���݂̍ő�A�����擾
	before = FrontierRecord_Get(frontier,id,Frontier_GetFriendIndex(id));
	//�ő�A�����X�V
	after = FrontierRecord_SetIfLarge(
		frontier,id,Frontier_GetFriendIndex(id),wk->renshou+wk->now_win);

	//�X�V���Ă���||(�L�^��7�̔{��&&�O��̒l������)�Ȃ�ԑg�쐬
	if(after > 1){
		if(	(before < after) ||
			((before == after) && (after%7==0)) ){
			//towerscr_MakeTVRenshouMaxUpdate(wk,fsys,after);
			towerscr_MakeTVRenshouMaxUpdate(wk,savedata,after);
		}
	}

	//���у��j�^�p�Ɍ��݂̘A�����������o���Ă���
	if(wk->play_mode == BTWR_MODE_WIFI_MULTI){
#if 0
		chg_flg = TowerScoreData_SetFlags(wk->scoreSave,
			BTWR_SFLAG_WIFI_MULTI_RECORD,BTWR_DATA_get);
#else
		chg_flg = FrontierRecord_Get(SaveData_GetFrontier(savedata), 
									FRID_TOWER_MULTI_WIFI_CLEAR_BIT,
									Frontier_GetFriendIndex(FRID_TOWER_MULTI_WIFI_CLEAR_BIT) );
#endif
	}else{
		chg_flg = TowerScoreData_SetFlags(wk->scoreSave,
			BTWR_SFLAG_SINGLE_RECORD+wk->play_mode,BTWR_DATA_get);
	}

	if(chg_flg){	//�A���L�^���풆�Ȃ���Z
		ret = FrontierRecord_Add(frontier,id+1,Frontier_GetFriendIndex(id+1),wk->now_win);
	}else{	//�O�񕉂��Ă���΃Z�b�g
		ret = FrontierRecord_Set(frontier,id+1,Frontier_GetFriendIndex(id+1),wk->now_win);
	}

	//���R�[�h���풆�t���O�𗎂Ƃ�
	if(wk->play_mode == BTWR_MODE_WIFI_MULTI){
#if 0
		TowerScoreData_SetFlags(wk->scoreSave,BTWR_SFLAG_WIFI_MULTI_RECORD,
								BTWR_DATA_reset);
#else
		FrontierRecord_Set(	SaveData_GetFrontier(savedata), 
							FRID_TOWER_MULTI_WIFI_CLEAR_BIT,
							Frontier_GetFriendIndex(FRID_TOWER_MULTI_WIFI_CLEAR_BIT), 0 );
#endif
	}else{
		TowerScoreData_SetFlags(wk->scoreSave,BTWR_SFLAG_SINGLE_RECORD+wk->play_mode,
								BTWR_DATA_reset);
	}

	//���׏������X�V
	RECORD_Add(record,RECID_BTOWER_WIN,wk->now_win);
	//���݂̎��񐔃��Z�b�g
	TowerScoreData_SetStage(wk->scoreSave,wk->play_mode,BTWR_DATA_reset);
	//�o�g���^���[�ւ̃`�������W���ǉ�
	RECORD_Add(SaveData_GetRecord(savedata),RECID_BTOWER_CHALLENGE,1);
	
	//�A�����{�������炦�邩�ǂ����̃t���O���Z�b�g
	towerscr_IfRenshouPrizeGet(wk);

	//�����������ʃf�[�^����
	ret+=1;
	if(ret > 9999){
		ret = 9999;	
	}
	towerscr_SetCommonScore(wk,savedata,FALSE,ret);
}

//--------------------------------------------------------------
/**
 *	@brief	�N���A����
 */
//--------------------------------------------------------------
void FSTowerScr_SetClearScore(BTOWER_SCRWORK* wk,SAVEDATA* savedata,FNOTE_DATA* fnote)
{
	u32	ret = 0;
	int	id;
	void* note;
	u16	before,after,chg_flg;
	RECORD*  record;
	FRONTIER_SAVEWORK *frontier;
		
	if(wk->play_mode == BTWR_MODE_RETRY){
		return;
	}
	OS_Printf("TowerClearScoreSet -> mode = %d\n",wk->play_mode);
	
	record = SaveData_GetRecord(savedata);
	frontier = SaveData_GetFrontier(savedata);

	if(wk->play_mode == BTWR_MODE_WIFI_MULTI){
		id = FRID_TOWER_MULTI_WIFI_RENSHOU;
	}else{
		id = wk->play_mode*2+FRID_TOWER_SINGLE_RENSHOU;
	}
	
	//���݂̘A�����������o��
	if(wk->play_mode == BTWR_MODE_WIFI_MULTI){
#if 0
		chg_flg = TowerScoreData_SetFlags(wk->scoreSave,
				BTWR_SFLAG_WIFI_MULTI_RECORD,BTWR_DATA_get);
#else
		chg_flg = FrontierRecord_Get(SaveData_GetFrontier(savedata), 
									FRID_TOWER_MULTI_WIFI_CLEAR_BIT,
									Frontier_GetFriendIndex(FRID_TOWER_MULTI_WIFI_CLEAR_BIT) );
#endif
	}else{
		chg_flg = TowerScoreData_SetFlags(wk->scoreSave,
				BTWR_SFLAG_SINGLE_RECORD+wk->play_mode,BTWR_DATA_get);
	}

	if(chg_flg){	//���݂��A���L�^���풆�Ȃ���Z
		ret = RECORD_Add(record,id+1,BTWR_CLEAR_WINCNT);
	}else{	//�O�񕉂��Ă��ꍇ�͈�U���Z�b�g
		ret = RECORD_Set(record,id+1,BTWR_CLEAR_WINCNT);
	}

	//���R�[�h���풆�t���O�𗧂Ă�
	if(wk->play_mode == BTWR_MODE_WIFI_MULTI){
#if 0
		TowerScoreData_SetFlags(wk->scoreSave,BTWR_SFLAG_WIFI_MULTI_RECORD,BTWR_DATA_set);
#else
		FrontierRecord_Set(	SaveData_GetFrontier(savedata), 
							FRID_TOWER_MULTI_WIFI_CLEAR_BIT,
							Frontier_GetFriendIndex(FRID_TOWER_MULTI_WIFI_CLEAR_BIT), 1 );
#endif
	}else{
		TowerScoreData_SetFlags(wk->scoreSave,BTWR_SFLAG_SINGLE_RECORD+wk->play_mode,BTWR_DATA_set);
	}

	//���݂̍ő�A�����擾
	before = FrontierRecord_Get(frontier,id,Frontier_GetFriendIndex(id));
	//�ő�A�����X�V
	after = FrontierRecord_SetIfLarge(frontier,id,Frontier_GetFriendIndex(id),ret);

	//�X�V���Ă���ԑg�쐬
#if 0
	if(before < after){
		//towerscr_MakeTVRenshouMaxUpdate(wk,fsys,after);
		towerscr_MakeTVRenshouMaxUpdate(wk,savedata,after);
	}
#endif
	//���׏������X�V
	RECORD_Add(record,RECID_BTOWER_WIN,BTWR_CLEAR_WINCNT);
	//���񐔃v���X
	TowerScoreData_SetStage(wk->scoreSave,wk->play_mode,BTWR_DATA_inc);
	//�o�g���^���[�ւ̃`�������W���ǉ�
	RECORD_Add(record,RECID_BTOWER_CHALLENGE,1);
	//�X�R�A���Z
	RECORD_Score_Add(record,SCORE_ID_BTOWER_7WIN);
	
	//�A�����{�������炦�邩�ǂ����̃t���O���Z�b�g
	towerscr_IfRenshouPrizeGet(wk);

	//�����������ʃf�[�^����
	towerscr_SetCommonScore(wk,savedata,TRUE,ret);

	//�`���m�[�g(�ʐM�}���`&wifi�̂�)
	if(	wk->play_mode == BTWR_MODE_COMM_MULTI ||
		wk->play_mode == BTWR_MODE_WIFI_MULTI ||
		wk->play_mode == BTWR_MODE_WIFI){
		note = FNOTE_SioBattleTowerDataMake(wk->heapID);
		FNOTE_DataSave( fnote, note, FNOTE_TYPE_SIO );
	}
}

//--------------------------------------------------------------
/**
 *	@brief	���[�N�G���A���J������
 */
//--------------------------------------------------------------
void FSTowerScr_WorkRelease(BTOWER_SCRWORK* wk)
{
	int	heap;

	if(wk == NULL){
		return;
	}
	GF_ASSERT(wk->magicNo == BTOWER_SCRWORK_MAGIC && "btower word is crash");
	heap = wk->heapID;

	MI_CpuClear8(wk,sizeof(BTOWER_SCRWORK));
	sys_FreeMemoryEz(wk);
	wk = NULL;

	//sys_DeleteHeap(heap);
}

//--------------------------------------------------------------
/**
 *	@brief	�A���̂��J�����{�������炦�邩�ǂ����̃t���O�𗧂ĂĂ���
 */
//--------------------------------------------------------------
static u16 towerscr_IfRenshouPrizeGet(BTOWER_SCRWORK* wk)
{
	u16 win;
	if(	wk->play_mode == BTWR_MODE_RETRY ||
		wk->play_mode == BTWR_MODE_SINGLE ||
		wk->play_mode == BTWR_MODE_WIFI){
		return 0;
	}
	win = wk->renshou+wk->now_win;

	//50�A���ȏ�ł��炦��
	if(win < BTWR_50_RENSHOU_CNT){
		return 0;
	}
	//prize get�t���O�𗧂ĂĂ���
	wk->prize_f = 1;
	return 1;
}

//--------------------------------------------------------------
/**
 *	@brief	7�l���������܂��͕��������̋��ʃf�[�^�Z�[�u����
 *
 *	@param	now_renshou	���݂̘A����
 */
//--------------------------------------------------------------
static void towerscr_SetCommonScore(BTOWER_SCRWORK* wk,SAVEDATA* sv,u8 win_f,u16 now_renshou)
{
	u8	buf8;

	switch(wk->play_mode){
	case BTWR_MODE_SINGLE:
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

//--------------------------------------------------------------
/**
 *	@brief	�Q�������|�P�����̃p�����[�^��ۑ�����
 */
//--------------------------------------------------------------
static void towerscr_SaveMemberPokeData(BTOWER_SCRWORK* wk,SAVEDATA* sv,BTWR_SCORE_POKE_DATA mode)
{
	int i = 0;
	B_TOWER_POKEMON* dat;
	POKEPARTY	*party;
	POKEMON_PARAM* pp;
	
	dat = sys_AllocMemoryLo(wk->heapID,sizeof(B_TOWER_POKEMON)*3);
	MI_CpuClear8(dat,sizeof(B_TOWER_POKEMON)*3);

	party = SaveData_GetTemotiPokemon(sv);
	for(i = 0;i < 3;i++){
		pp = PokeParty_GetMemberPointer(party,wk->member[i]);
		towerscr_PokeDataPack(&(dat[i]),pp);	
	}
	TowerScoreData_SetUsePokeData(wk->scoreSave,mode,dat);

	MI_CpuClear8(dat,sizeof(B_TOWER_POKEMON)*3);
	sys_FreeMemoryEz(dat);
}

//--------------------------------------------------------------
/**
 *	@brief	�ő�A���L�^�X�V�ԑg�쐬�֐�
 */
//--------------------------------------------------------------
static void towerscr_MakeTVRenshouMaxUpdate(BTOWER_SCRWORK* wk,SAVEDATA* savedata,u16 renshou)
{
	POKEPARTY *party;

	if(	wk->play_mode != BTWR_MODE_SINGLE &&
		wk->play_mode != BTWR_MODE_DOUBLE){
		return;
	}
	party = SaveData_GetTemotiPokemon(savedata);

	if(wk->play_mode == BTWR_MODE_SINGLE){
		//TVTOPIC_Entry_Record_BTower(fsys,
		TVTOPIC_Entry_Record_BTowerEx(savedata,
			renshou,PokeParty_GetMemberPointer(party,wk->member[0]), TRUE);
	}else{
		//TVTOPIC_Entry_Record_BTower(fsys,
		TVTOPIC_Entry_Record_BTowerEx(savedata,
			renshou,PokeParty_GetMemberPointer(party,wk->member[0]), FALSE);
	}
}

//--------------------------------------------------------------
/**
 *	@brief	�Q�������|�P�����̃p�����[�^��B_TOWER_POKEMON�^�Ƀp�b�N����
 */
//--------------------------------------------------------------
static void towerscr_PokeDataPack(B_TOWER_POKEMON* dat,POKEMON_PARAM* pp)
{
	int i;
	
	//mons_no/form_no/item_no
	dat->mons_no = PokeParaGet(pp,ID_PARA_monsno,NULL);
	dat->form_no = PokeParaGet(pp,ID_PARA_form_no,NULL);
	dat->item_no = PokeParaGet(pp,ID_PARA_item,NULL);

	//waza/pp_count
	for(i = 0;i < WAZA_TEMOTI_MAX;i++){
		dat->waza[i] = PokeParaGet(pp,ID_PARA_waza1+i,NULL);
		dat->pp_count |= ((PokeParaGet(pp,ID_PARA_pp_count1+i,NULL)) << (i*2));
	}
	//country,id,personal
	dat->country_code = PokeParaGet(pp,ID_PARA_country_code,NULL);
	dat->id_no = PokeParaGet(pp,ID_PARA_id_no,NULL);
	dat->personal_rnd = PokeParaGet(pp,ID_PARA_personal_rnd,NULL);

	//power_rnd
	dat->power_rnd = PokeParaGet(pp,ID_PARA_power_rnd,NULL);

	//exp
	for(i = 0;i < 6;i++){
		dat->exp[i] = PokeParaGet(pp,ID_PARA_hp_exp+i,NULL);
	}
	//tokusei,natukido
	dat->tokusei = PokeParaGet(pp,ID_PARA_speabino,NULL);
	dat->natuki = PokeParaGet(pp,ID_PARA_friend,NULL);
	
	//�j�b�N�l�[��
	PokeParaGet(pp,ID_PARA_nickname,dat->nickname);
}

//--------------------------------------------------------------
/**
 *	@brief	�v���C���̘A�������擾����
 */
//--------------------------------------------------------------
u16 FSTowerScr_GetRenshouCount(BTOWER_SCRWORK* wk)
{
	if((u32)wk->renshou+wk->now_win > 0xFFFF){
		return 0xFFFF;
	}else{
		return wk->renshou+wk->now_win;
	}
}

//--------------------------------------------------------------
/**
 *	@brief	���݂̃��E���h�����X�N���v�g���[�N�Ɏ擾����
 */
//--------------------------------------------------------------
u16	FSTowerScr_GetNowRound(BTOWER_SCRWORK* wk)
{
	return wk->now_round;
}

//--------------------------------------------------------------
/**
 *	@brief	�x�ނƂ��Ɍ��݂̃v���C�󋵂��Z�[�u�ɏ����o��
 */
//--------------------------------------------------------------
void FSTowerScr_SaveRestPlayData(BTOWER_SCRWORK* wk)
{
	u16	i;
	u8	buf8[4];

	//�v���C���[�h�����o��
	buf8[0] = wk->play_mode;
	TowerPlayData_Put(wk->playSave,BTWR_PSD_playmode,buf8);
	
	//���E���h�������o��
	buf8[0] = wk->now_round;
	TowerPlayData_Put(wk->playSave,BTWR_PSD_round,buf8);

	//�I�񂾃|�P����No
	TowerPlayData_Put(wk->playSave,BTWR_PSD_pokeno,wk->member);
	
	//�o�g�����я����o��
	TowerPlayData_WifiRecordAdd(wk->playSave,wk->rec_down,wk->rec_turn,wk->rec_damage);

	//���I���ꂽ�g���[�i�[No�����o��
	TowerPlayData_Put(wk->playSave,BTWR_PSD_trainer,wk->trainer);
	
	//�v���C�����_���V�[�h�ۑ�
	TowerPlayData_Put(wk->playSave,BTWR_PSD_rnd_seed,&(wk->play_rnd_seed));
	OS_Printf("TowerRestRndSeed = %d\n",wk->play_rnd_seed);
	
	//�Z�[�u�t���O��L����ԂɃ��Z�b�g
	TowerPlayData_SetSaveFlag(wk->playSave,TRUE);

	if(wk->play_mode != BTWR_MODE_MULTI){
		return;
	}
	//AI�}���`���[�h�Ȃ�p�[�g�i�[���o���Ă���
	buf8[0] = wk->partner;
	TowerPlayData_Put(wk->playSave,BTWR_PSD_partner,buf8);

	//�p�[�g�i�[�̃|�P�����p�����[�^�������Ă���
	TowerPlayData_Put(wk->playSave,BTWR_PSD_pare_poke,&(wk->five_poke[wk->partner]));
	//�A�C�e�����Œ肾�������ǂ��������Ă���
	TowerPlayData_Put(wk->playSave,BTWR_PSD_pare_itemfix,&(wk->five_item[wk->partner]));
	//�v���C�����_���V�[�h�������Ă���
	
}

//--------------------------------------------------------------
/**
 *	@brief	���[�_�[�N���A�t���O�𗧂Ă�
 */
//--------------------------------------------------------------
void FSTowerScr_SetLeaderClearFlag(BTOWER_SCRWORK* wk,u16 mode)
{
	wk->leader_f = mode;
}


