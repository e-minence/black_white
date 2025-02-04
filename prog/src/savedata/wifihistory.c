//============================================================================================
/**
 * @file	wifihistory.c
 * @brief
 * @date	2006.03.25
 */
//============================================================================================

#include <gflib.h>
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/wifihistory.h"
#include "savedata/mystatus.h"
#include "wifihistory_local.h"
#include "savedata/unitednations_def.h"

#include "../../../resource/research_radar/data/hobby_id.h"  // for HOBBY_ID_xxxx

//============================================================================================
//============================================================================================


//============================================================================================
//
//	セーブデータシステムが依存する関数
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	WiFi通信履歴ワークのサイズ取得
 * @return	int		サイズ（バイト単位）
 */
//----------------------------------------------------------
int WIFIHISTORY_GetWorkSize(void)
{
	return sizeof(WIFI_HISTORY);
}

//----------------------------------------------------------
/**
 * @brief	WiFi通信履歴ワークの初期化
 * @param	hist
 * @return	WIFI_HISTORY	取得したワークへのポインタ
 */
//----------------------------------------------------------
void WIFIHISTORY_Init(WIFI_HISTORY * hist)
{
	GFL_STD_MemClear32(hist, sizeof(WIFI_HISTORY));
}

//============================================================================================
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	WiFi通信履歴セーブデータの取得
 * @param	sv		セーブデータへのポインタ
 * @return	WIFI_HISTORYデータへのポインタ
 */
//----------------------------------------------------------
WIFI_HISTORY * SaveData_GetWifiHistory(SAVE_CONTROL_WORK * sv)
{
	return (WIFI_HISTORY*)SaveControl_DataPtrGet(sv, GMDATA_ID_WIFIHISTORY);
}

//============================================================================================
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	自分の国、地域をセット
 * @param	wh			WIFI履歴データへのポインタ
 * @param	nation		国指定コード
 * @param	area		地域指定コード
 */
//----------------------------------------------------------
void WIFIHISTORY_SetMyNationArea(WIFI_HISTORY * wh, MYSTATUS* my, int nation, int area)
{
	GF_ASSERT(nation < WIFI_NATION_MAX);
	GF_ASSERT(area < WIFI_AREA_MAX);

  MyStatus_SetMyNationArea(my,nation,area);
	WIFIHISTORY_SetStat(wh, nation, area, WIFIHIST_STAT_MINE);
}

//----------------------------------------------------------
/**
 * @brief	状態コードの取得
 * @param	wh			WIFI履歴データへのポインタ
 * @param	nation		国指定コード
 * @param	area		地域指定コード
 * @return	WIFIHIST_STAT	状態指定ID
 */
//----------------------------------------------------------
WIFIHIST_STAT WIFIHISTORY_GetStat(const WIFI_HISTORY * wh, int nation, int area)
{
	WIFIHIST_STAT stat;
	GF_ASSERT(nation < WIFI_NATION_MAX);
	GF_ASSERT(area < WIFI_AREA_MAX);

	if (nation == 0) {
		return WIFIHIST_STAT_NODATA;
	}
	stat = (wh->data[(nation - 1) * NATION_DATA_SIZE + area / 4] >> ((area % 4) * 2)) & 3;

	return stat;
}

//----------------------------------------------------------
/**
 * @param	wh			WIFI履歴データへのポインタ
 * @param	nation		国指定コード
 * @param	area		地域指定コード
 * @param	stat		状態指定ID
 */
//----------------------------------------------------------
void WIFIHISTORY_SetStat(WIFI_HISTORY * wh, int nation, int area, WIFIHIST_STAT stat)
{
	u8 * p;
	u8 mask = 3;
	u8 data;

	GF_ASSERT(stat < WIFIHIST_STAT_MAX);
	GF_ASSERT(nation < WIFI_NATION_MAX);
	GF_ASSERT(area < WIFI_AREA_MAX);

	if (nation == 0) {
		return;
	}
	p = &wh->data[(nation - 1) * NATION_DATA_SIZE + area / 4];

	*p &= ((mask << ((area % 4) * 2))^0xff);
	*p |= stat << ((area % 4) * 2);
	
	if (nation != WIFI_NATION_JAPAN) {
		WIFIHISTORY_SetWorldFlag(wh, TRUE);
	}
}


//----------------------------------------------------------
/**
 * @brief	世界モードかどうかのフラグ取得
 * @param	wh			WIFI履歴データへのポインタ
 * @return	BOOL		TRUEのとき、世界モード
 */
//----------------------------------------------------------
BOOL WIFIHISTORY_GetWorldFlag(const WIFI_HISTORY * wh)
{
	return wh->world_flag;
}


//----------------------------------------------------------
/**
 * @brief	世界モードのフラグセット
 * @param	wh			WIFI履歴データへのポインタ
 * @param	flag		設定する値（TRUE/FALSE）
 */
//----------------------------------------------------------
void WIFIHISTORY_SetWorldFlag(WIFI_HISTORY * wh, BOOL flag)
{
	wh->world_flag = flag;
}


//----------------------------------------------------------
/**
 * @brief	日付更新で内容更新
 * @param	wh			WIFI履歴データへのポインタ
 */
//----------------------------------------------------------
void WIFIHISTORY_Update(WIFI_HISTORY * wh)
{
	int i, j;
	u8 data;
	for (i = 0; i < (WIFI_NATION_MAX - 1) * NATION_DATA_SIZE; i++) {
		data = wh->data[i];
		for (j = 0; j < 8; j+=2) {
			if (((data >> j) & 3) == WIFIHIST_STAT_NEW) {
				data &= ((3 << j)^0xff);
				data |= (WIFIHIST_STAT_EXIST << j);
			}
		}
		wh->data[i] = data;
	}
}

//----------------------------------------------------------
/**
 * @brief	交換した国の数を取得
 * @param	wh			WIFI履歴データへのポインタ
 * @return u8     国数
 */
//----------------------------------------------------------
u8 WIFIHISTORY_GetMyCountryCount(WIFI_HISTORY * wh)
{
  return wh->myCountryCount;
}

//----------------------------------------------------------------------------
/**
 *	@brief  交換した国の数を取得  拡張版
 *
 *	@param	* wh  WIFI履歴データへのポインタ
 *	@param	my_country_flg TRUEで自分の国を含む　FALSEで含まない
 *
 *	@return 国数
 */
//-----------------------------------------------------------------------------
u8 WIFIHISTORY_GetMyCountryCountEx(WIFI_HISTORY * wh, const MYSTATUS *cp_mystats, BOOL my_country_flg )
{
  u8 num  = WIFIHISTORY_GetMyCountryCount(wh);
  NOZOMU_Printf("交換国数 %d\n",num);

  if ( !my_country_flg )
  {
    int nation = MyStatus_GetMyNation(cp_mystats);
    //国コードがセットされていない場合は0を返すようにする
    if (!nation) num = 0;
    else
    {
      //自分の国の交換フラグが成立している場合、交換国数を-1する
      if ( WIFIHISTORY_CheckCountryBit(wh, nation) )
      {
        if (num) num--; //１以上のとき減算
      }
    }
  }

  return num;
}

//----------------------------------------------------------
/**
 * @brief	国連有効データ数を返す0〜20
 * @param	wh			WIFI履歴データへのポインタ
 * @return u8     データ数
 */
//----------------------------------------------------------
u8 WIFIHISTORY_GetValidUnDataNum(WIFI_HISTORY * wh)
{
  u8 num;
  int i;
  for (i=0;i<UNITEDNATIONS_PEOPLE_MAX;i++)
  {
    if ( !wh->aUnitedPeople[i].valid ) break;
  }
  num = i;
  return num;
}

//----------------------------------------------------------
/**
 * @brief	交換した国の数を加算
 * @param	wh			WIFI履歴データへのポインタ
 * @return none
 */
//----------------------------------------------------------
void WIFIHISTORY_AddMyCountryCount(WIFI_HISTORY * wh)
{
  if ( wh->myCountryCount < WIFI_COUNTRY_MAX ) wh->myCountryCount++;
}

//----------------------------------------------------------
/**
 * @brief	自分の性格をセット
 * @param	wh			WIFI履歴データへのポインタ
 * @param inNature    性格コード　0〜4
 * @return none
 */
//----------------------------------------------------------
void WIFIHISTORY_SetMyNature(WIFI_HISTORY * wh, const NATURE_TYPE inType)
{
  if ( inType < NATURE_MAX ) wh->myNature = inType;
}

//----------------------------------------------------------
/**
 * @brief	自分の性格を得る
 * @param	wh			WIFI履歴データへのポインタ
 * @return 性格
 */
//----------------------------------------------------------
u8 WIFIHISTORY_GetMyNature(WIFI_HISTORY * wh)
{
  return wh->myNature;
}

//----------------------------------------------------------
/**
 * @brief	自分の趣味をセット
 * @param	wh			WIFI履歴データへのポインタ
 * @param inFavorite    趣味コード
 * @return none
 */
//----------------------------------------------------------
void WIFIHISTORY_SetMyFavorite(WIFI_HISTORY * wh, const int inFavorite)
{
  if ( inFavorite < HOBBY_ID_NUM ) wh->myFavorite = inFavorite;
}

//----------------------------------------------------------
/**
 * @brief	自分の趣味をセット
 * @param	wh			WIFI履歴データへのポインタ
 * @param inFavorite    趣味コード
 * @return none
 */
//----------------------------------------------------------
u8 WIFIHISTORY_GetMyFavorite(WIFI_HISTORY * wh )
{
  return wh->myFavorite;
}

//----------------------------------------------------------
/**
 * @brief	自分の所持している国連データを取得
 * @param	wh			WIFI履歴データへのポインタ
 * @param inIdx   取得したい、データインデックス
 * @param inType  取得したい情報の種類 wifihistory.h 参照
 * @return int    取得情報
 */
//----------------------------------------------------------
u32 WIFIHISTORY_GetUnInfo(WIFI_HISTORY * wh, const u32 inIdx, const UN_INFO_TYPE inType)
{
  u32 info;
  UNITEDNATIONS_SAVE *un;

  if (inIdx >= UNITEDNATIONS_PEOPLE_MAX)
  {
    GF_ASSERT_MSG(0,"INDEX_OVER idx=%d",inIdx);
    return 0;
  }

  un = &wh->aUnitedPeople[inIdx];
  switch(inType){
  case UN_INFO_RECV_POKE:       //もらったポケモン
    info = un->recvPokemon;
    break;
  case UN_INFO_SEND_POKE:      //あげたポケモン
    info = un->sendPokemon;
    break;
  case UN_INFO_FAVORITE:       //趣味
    info = un->favorite;
    break;
  case UN_INFO_COUNTRY_NUM:    //交換した国の数
    info = un->countryCount;
    break;
  case UN_INFO_NATURE:         //性格
    info = un->nature;
    break;
  case UN_INFO_TALK:           //話したことあるか？
    info = un->bTalk;
    break;
  case UN_INFO_VALID:          //データは有効か？
    info = un->valid;
    break;
  default:
    GF_ASSERT_MSG(0,"TYPE ERROR %d",inType);
    info = 0;
  }
  return info;
}

//----------------------------------------------------------
/**
 * @brief	自分の所持している国連データを取得
 * @param	wh			WIFI履歴データへのポインタ
 * @param inIdx   更新したい、データインデックス
 * @param inType  更新したい情報の種類 wifihistory.h 参照
 * @param inInfo 　更新情報
 * @return none
 */
//----------------------------------------------------------
void WIFIHISTORY_SetUnInfo(WIFI_HISTORY * wh, const int inIdx, const UN_INFO_TYPE inType, const u32 inInfo)
{
  UNITEDNATIONS_SAVE *un;

  NOZOMU_Printf( "idx = %d type = %d info = %d\n", inIdx, inType, inInfo );

  if (inIdx >= UNITEDNATIONS_PEOPLE_MAX)
  {
    GF_ASSERT_MSG(0,"INDEX_OVER idx=%d",inIdx);
    return;
  }
  un = &wh->aUnitedPeople[inIdx];
  switch(inType){
  case UN_INFO_RECV_POKE:       //もらったポケモン
    break;
  case UN_INFO_SEND_POKE:      //あげたポケモン
    break;
  case UN_INFO_FAVORITE:       //趣味
    break;
  case UN_INFO_COUNTRY_NUM:    //交換した国の数
    break;
  case UN_INFO_NATURE:         //性格
    if (inInfo < 5 ) un->nature = inInfo;
    else GF_ASSERT_MSG(0,"nature error %d", inInfo);
    break;
  case UN_INFO_TALK:           //話したことあるか？
    if (inInfo)
    {
      un->bTalk = 1;
      NOZOMU_Printf("set %d\n",un->bTalk);
    }
    else un->bTalk = 0;
    break;
  case UN_INFO_VALID:          //データは有効か？
    if (inInfo) un->valid = 1;
    else un->valid = 0;
    break;
  default:
    GF_ASSERT_MSG(0,"TYPE ERROR %d",inType);
  }
}


//----------------------------------------------------------
/**
 * @brief	自分の所持している国連データを取得
 * @param	wh			WIFI履歴データへのポインタ
 * @param inIdx   取得したい、データインデックス
 * @param inType  取得したい情報の種類 wifihistory.h 参照
 * @return int    取得情報
 */
//----------------------------------------------------------
MYSTATUS *WIFIHISTORY_GetUnMyStatus(WIFI_HISTORY * wh, const int inIdx)
{
  int info;
  UNITEDNATIONS_SAVE *un;

  if (inIdx >= UNITEDNATIONS_PEOPLE_MAX)
  {
    GF_ASSERT_MSG(0,"INDEX_OVER idx=%d",inIdx);
    return NULL;
  }
  un = &wh->aUnitedPeople[inIdx];
  return &un->aMyStatus;
}

//----------------------------------------------------------
/**
 * @brief	国連データ先頭ポインタ取得
 * @param	wh			WIFI履歴データへのポインタ
 * @return adr    先頭アドレス
 */
//----------------------------------------------------------
UNITEDNATIONS_SAVE *WIFIHISTORY_GetUNDataPtr(WIFI_HISTORY * wh)
{
  return wh->aUnitedPeople;
}

//----------------------------------------------------------
/**
 * @brief	指定インデックスの国ビットを立てる
 * @param	wh			WIFI履歴データへのポインタ
 * @param inCountryCode
 * @return BOOL 新規ビット立て成功
 */
//----------------------------------------------------------
BOOL WIFIHISTORY_SetCountryBit(WIFI_HISTORY * wh, const u32 inCountryCode)
{
  BOOL rc;
  rc = FALSE;

  if (inCountryCode == 0) return rc;

  //指定インデックオーバーチェック
  if (inCountryCode < WIFI_COUNTRY_MAX)
  {
    u32 idx;
    u8 data;
    u8 bit_pos; //0〜7
    

    if ( !WIFIHISTORY_CheckCountryBit(wh, inCountryCode) ) rc = TRUE;

    //指定国のビットが格納されている1バイトデータを配列から取得
    idx = inCountryCode / 8;
    data = wh->billopen[idx];
    //対応するビット位置を算出
    bit_pos = inCountryCode % 8;
    //ビットON
    data |= (1 << bit_pos);
    //配列に格納しなおし
    wh->billopen[idx] = data;
  }

  return rc;
}

//----------------------------------------------------------
/**
 * @brief	指定インデックスの国ビットをチェックする
 * @param	wh			WIFI履歴データへのポインタ
 * @param inCountryCode
 * @return BOOL     ビットが立っていたらTRUE
 */
//----------------------------------------------------------
BOOL WIFIHISTORY_CheckCountryBit(WIFI_HISTORY * wh, const u32 inCountryCode)
{
  BOOL rc = FALSE;
  //指定インデックオーバーチェック
  if (inCountryCode < WIFI_COUNTRY_MAX)
  {
    u32 idx;
    u8 data;
    u8 bit_pos; //0〜7
    u8 bit; //0 or 1
    //指定国のビットが格納されている1バイトデータを配列から取得
    idx = inCountryCode / 8;
    data = wh->billopen[idx];
    //対応するビット位置を算出
    bit_pos = inCountryCode % 8;
    //ビットチェック
    bit = (data >> bit_pos) & 0x1;
    if (bit) rc = TRUE;
  }
  return rc;
}

