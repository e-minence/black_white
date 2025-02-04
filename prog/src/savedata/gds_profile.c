//==============================================================================
/**
 * @file	gds_profile.c
 * @brief	GDSプロフィール(GDSのWifiデータに添付される)
 * @author	matsuda
 * @date	2007.07.23(月)
 */
//==============================================================================
#include <gflib.h>
#include "buflen.h"
#include "savedata\save_control.h"
#include "savedata\mystatus.h"
#include "savedata\wifihistory.h"
#include "system/pms_data.h"
#include "poke_tool/monsno_def.h"
#include "savedata/gds_profile.h"
#include "savedata/box_savedata.h"
#include "savedata/misc.h"
#include "savedata/my_pms_data.h"

#include "savedata/gds_local_common.h"
#include "net_app/gds/gds_profile_local.h"

#include "net_app/union/union_beacon_tool.h"

#include "message.naix"
#include "system/pms_data.h"
#include "system/pms_word.h"
#include "msg\msg_pmss_union.h"
#include "msg\msg_pmss_ready.h"
#include "msg\msg_pmss_won.h"
#include "msg\msg_pmss_lost.h"
#include "msg\msg_pms_word06.h"
#include "msg\msg_pms_word07.h"
#include "msg\msg_pms_word08.h"
#include "msg\msg_pms_word09.h"
#include "msg\msg_pms_word10.h"
#include "msg\msg_pms_word11.h"
#include "msg\msg_pms_word12.h"
#include "msg\msg_wifi_place_msg_world.h"

#include "net_app/wifi_country.h"


// あいさつの「よろしく」を選んでおく
#define YOROSHIKU_GMM_ID   (NARC_message_pms_word08_dat)
#define YOROSHIKU_WORD_ID  (pms_word08_48)


//--------------------------------------------------------------
/**
 * @brief   GDSプロフィールワークをAllocする
 *
 * @param   heap_id		ヒープID
 *
 * @retval  GDSプロフィールワークへのポインタ
 */
//--------------------------------------------------------------
GDS_PROFILE_PTR GDS_Profile_AllocMemory(int heap_id)
{
	GDS_PROFILE_PTR gpp;
	
	gpp = GFL_HEAP_AllocMemory(heap_id, sizeof(GDS_PROFILE));
	GFL_STD_MemClear(gpp, sizeof(GDS_PROFILE));
	
	return gpp;
}

//--------------------------------------------------------------
/**
 * @brief   GDSプロフィールワークを解放
 *
 * @param   gpp		GDSプロフィールへのポインタ
 */
//--------------------------------------------------------------
void GDS_Profile_FreeMemory(GDS_PROFILE_PTR gpp)
{
	GFL_HEAP_FreeMemory(gpp);
}

//--------------------------------------------------------------
/**
 * @brief   自分のGDSプロフィールデータを作成する
 *
 * @param   gpp			データ代入先
 * @param   gamedata  ゲームデータ
 *
 */
//--------------------------------------------------------------
void GDS_Profile_MyDataSet(GDS_PROFILE_PTR gpp, GAMEDATA *gamedata)
{
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork( gamedata );
	MYSTATUS *my = GAMEDATA_GetMyStatus(gamedata);
	WIFI_HISTORY *wh = SaveData_GetWifiHistory(sv);
	const MISC * misc = GAMEDATA_GetMiscWork(gamedata);
  const MYPMS_DATA * mypms  = SaveData_GetMyPmsDataConst(sv);
	int monsno, form_no, egg_flag, mons_sex;
	int i;
	OSOwnerInfo info;

	OS_GetOwnerInfo(&info);
	
	MISC_GetFavoriteMonsno(misc, &monsno, &form_no, &egg_flag, &mons_sex );

	GFL_STD_MemClear(gpp, sizeof(GDS_PROFILE));

	STRTOOL_Copy( MyStatus_GetMyName(my), gpp->name, PLW_TRAINER_NAME_SIZE );
	
	gpp->player_id = MyStatus_GetID(my);
	gpp->player_sex = MyStatus_GetMySex(my);
	gpp->monsno = monsno;
	gpp->form_no = form_no;
	gpp->egg_flag = egg_flag;
  gpp->mons_sex  = mons_sex;
	gpp->country_code = MyStatus_GetMyNation(my);
	gpp->local_code = MyStatus_GetMyArea(my);
	
	for(i = 0; i < EVENT_SELF_INTRO; i++){
		gpp->event_self_introduction[i] = GFL_STR_GetEOMCode();
	}
	MYPMS_GetPms(mypms, MYPMS_PMS_TYPE_INTRODUCTION, &gpp->self_introduction);

	gpp->birthday_month = info.birthday.month;
	gpp->trainer_view = MyStatus_GetTrainerView(my);
	gpp->version_code = PM_VERSION;
	gpp->language = PM_LANG;

	//CRC作成
	gpp->crc.crc16ccitt_hash = GFL_STD_CrcCalc(gpp, sizeof(GDS_PROFILE) - GDS_CRC_SIZE);
}

//----------------------------------------------------------
/**
 * @brief	名前取得（STRBUFを生成）
 * @param	gpp		GDSプロフィールへのポインタ
 * @param	heapID	STRBUFを生成するヒープのID
 * @return	STRBUF	名前を格納したSTRBUFへのポインタ
 */
//----------------------------------------------------------
STRBUF * GDS_Profile_CreateNameString(const GDS_PROFILE_PTR gpp, int heapID)
{
	STRBUF * tmpBuf = GFL_STR_CreateBuffer( ( PERSON_NAME_SIZE * 2 ) + EOM_SIZE, heapID);
	GFL_STR_SetStringCodeOrderLength(tmpBuf, gpp->name, ( PERSON_NAME_SIZE * 2 ) + EOM_SIZE);
	return tmpBuf;
}

u32 GDS_Profile_GetID(const GDS_PROFILE_PTR gpp)
{
	return gpp->player_id;
}

u32 GDS_Profile_GetSex(const GDS_PROFILE_PTR gpp)
{
	if(gpp->player_sex != PM_MALE && gpp->player_sex != PM_FEMALE){
		return PM_MALE;	//エラー処理
	}
	return gpp->player_sex;
}

int GDS_Profile_GetMonsNo(const GDS_PROFILE_PTR gpp)
{
	if(gpp->monsno >= MONSNO_MAX){
		return 0;	//エラー処理
	}
	return gpp->monsno;
}

int GDS_Profile_GetFormNo(const GDS_PROFILE_PTR gpp)
{
	if(gpp->monsno >= MONSNO_MAX){
		return 0;
	}
	return POKETOOL_CheckPokeFormNo_Field(gpp->monsno, gpp->form_no);
}

int GDS_Profile_GetPokeSex(const GDS_PROFILE_PTR gpp)
{
	if(gpp->mons_sex != PM_MALE && gpp->mons_sex != PM_FEMALE){
		return PM_MALE;	//エラー処理
	}
	return gpp->mons_sex;
}

int GDS_Profile_GetEggFlag(const GDS_PROFILE_PTR gpp)
{
	if(gpp->egg_flag > 1){
		return 1;	//エラー処理
	}
	return gpp->egg_flag;
}

//--------------------------------------------------------------
/**
 * @brief   国コードを取得
 *
 * @param   gpp		
 *
 * @retval  国コード(msg_wifi_place_msg_world.h)
 */
//--------------------------------------------------------------
int GDS_Profile_GetNation(const GDS_PROFILE_PTR gpp)
{
  return WIFI_COUNTRY_GetNGTestCountryCode(gpp->country_code, gpp->local_code, GDS_Profile_GetLanguage(gpp));
}

//--------------------------------------------------------------
/**
 * @brief   地域コードを取得
 *
 * @param   gpp		
 *
 * @retval  地域コード(msg_wifi_place_msg_ARG.h, JPN, BRA等)
 */
//--------------------------------------------------------------
int GDS_Profile_GetArea(const GDS_PROFILE_PTR gpp)
{
  return WIFI_COUNTRY_GetNGTestLocalCode(gpp->country_code, gpp->local_code, GDS_Profile_GetLanguage(gpp));
}

//--------------------------------------------------------------
/**
 * @brief   自己紹介メッセージ取得
 *
 * @param   gpp				
 * @param   pms_dest		簡易会話代入先
 * @param   heap_id			ヒープID(イベント用フリーワード時、中でSTRBUFを生成)
 *
 * @retval  NULLの場合はpms_destに簡易会話文が入っている
 * 			非NULLの場合はイベント用フリーワードメッセージ。
 *			内部でSTRBUFを作成し返している
 */
//--------------------------------------------------------------
STRBUF * GDS_Profile_GetSelfIntroduction(const GDS_PROFILE_PTR gpp, PMS_DATA *pms_dest, int heap_id)
{
	int error_flg = 0;
	
	if(gpp->message_flag == MESSAGE_FLAG_NORMAL){
		*pms_dest = gpp->self_introduction;
		if(pms_dest->sentence_type >= PMS_TYPE_USER_MAX){
			error_flg++;
		}
		else if(pms_dest->sentence_id > pmss_union_20){
			error_flg++;
		}
		else{
			u32 file_id, word_id;
			if((pms_dest->word[0] != PMS_WORD_NULL 
					&& GetWordSorceID(pms_dest->word[0], &file_id, &word_id) == FALSE)
					|| (pms_dest->word[1] != PMS_WORD_NULL 
					&& GetWordSorceID(pms_dest->word[1], &file_id, &word_id) == FALSE)){
				error_flg++;
			}
		}
		
		if(error_flg > 0){
			PMSDAT_Init( pms_dest, PMS_TYPE_UNION );
			pms_dest->sentence_id = pmss_union_01;
			pms_dest->word[0] 
				= PMSW_GetWordNumberByGmmID( YOROSHIKU_GMM_ID, YOROSHIKU_WORD_ID );
			pms_dest->word[1] = PMS_WORD_NULL;
		}
		return NULL;
	}
	else{
		STRBUF * tmpBuf = GFL_STR_CreateBuffer(EVENT_SELF_INTRO, heap_id);
		GFL_STR_SetStringCodeOrderLength(tmpBuf, gpp->event_self_introduction, EVENT_SELF_INTRO);
		return tmpBuf;
	}
}

int GDS_Profile_GetMonthBirthday(const GDS_PROFILE_PTR gpp)
{
	if(gpp->birthday_month >= 1 && gpp->birthday_month <= 12){
		return gpp->birthday_month;
	}
	return 1;
}

int GDS_Profile_GetTrainerView(const GDS_PROFILE_PTR gpp)
{
	if(gpp->trainer_view > UNION_VIEW_INDEX_MAX){	//ランキングのと一緒なので
		return 0;
	}
	return gpp->trainer_view;
}

int GDS_Profile_GetRomCode(const GDS_PROFILE_PTR gpp)
{
	switch(gpp->version_code){
	case VERSION_SAPPHIRE:
	case VERSION_RUBY:
	case VERSION_EMERALD:
	case VERSION_RED:
	case VERSION_GREEN:
	case VERSION_GOLD:
	case VERSION_SILVER:
	case VERSION_DIAMOND:
	case VERSION_PEARL:
	case VERSION_PLATINUM:
	case VERSION_COLOSSEUM:
	case VERSION_WHITE:
	case VERSION_BLACK:
		return gpp->version_code;
	}
	return PM_VERSION;
}

int GDS_Profile_GetLanguage(const GDS_PROFILE_PTR gpp)
{
	switch(gpp->language){
	case LANG_JAPAN:
	case LANG_ENGLISH:
	case LANG_FRANCE:
	case LANG_ITALY:
	case LANG_GERMANY:
	case LANG_SPAIN:
	case LANG_KOREA:
		return gpp->language;
	}
	return PM_LANG;
}

