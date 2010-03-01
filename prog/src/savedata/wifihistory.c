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
//	�Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	WiFi�ʐM�������[�N�̃T�C�Y�擾
 * @return	int		�T�C�Y�i�o�C�g�P�ʁj
 */
//----------------------------------------------------------
int WIFIHISTORY_GetWorkSize(void)
{
	return sizeof(WIFI_HISTORY);
}

//----------------------------------------------------------
/**
 * @brief	WiFi�ʐM�������[�N�̏�����
 * @param	hist
 * @return	WIFI_HISTORY	�擾�������[�N�ւ̃|�C���^
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
 * @brief	WiFi�ʐM�����Z�[�u�f�[�^�̎擾
 * @param	sv		�Z�[�u�f�[�^�ւ̃|�C���^
 * @return	WIFI_HISTORY�f�[�^�ւ̃|�C���^
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
 * @brief	�����̍��A�n����Z�b�g
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @param	nation		���w��R�[�h
 * @param	area		�n��w��R�[�h
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
 * @brief	��ԃR�[�h�̎擾
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @param	nation		���w��R�[�h
 * @param	area		�n��w��R�[�h
 * @return	WIFIHIST_STAT	��Ԏw��ID
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
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @param	nation		���w��R�[�h
 * @param	area		�n��w��R�[�h
 * @param	stat		��Ԏw��ID
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
 * @brief	���E���[�h���ǂ����̃t���O�擾
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @return	BOOL		TRUE�̂Ƃ��A���E���[�h
 */
//----------------------------------------------------------
BOOL WIFIHISTORY_GetWorldFlag(const WIFI_HISTORY * wh)
{
	return wh->world_flag;
}


//----------------------------------------------------------
/**
 * @brief	���E���[�h�̃t���O�Z�b�g
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @param	flag		�ݒ肷��l�iTRUE/FALSE�j
 */
//----------------------------------------------------------
void WIFIHISTORY_SetWorldFlag(WIFI_HISTORY * wh, BOOL flag)
{
	wh->world_flag = flag;
}


//----------------------------------------------------------
/**
 * @brief	���t�X�V�œ��e�X�V
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
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
 * @brief	�����������̐����擾
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @return u8     ����
 */
//----------------------------------------------------------
u8 WIFIHISTORY_GetMyCountryCount(WIFI_HISTORY * wh)
{
  return wh->myCountryCount;
}

//----------------------------------------------------------
/**
 * @brief	���A�L���f�[�^����Ԃ�0�`20
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @return u8     �f�[�^��
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
 * @brief	�����������̐������Z
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @return none
 */
//----------------------------------------------------------
void WIFIHISTORY_AddMyCountryCount(WIFI_HISTORY * wh)
{
  if ( wh->myCountryCount < WIFI_COUNTRY_MAX ) wh->myCountryCount++;
}

//----------------------------------------------------------
/**
 * @brief	�����̐��i���Z�b�g
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @param inNature    ���i�R�[�h�@0�`4
 * @return none
 */
//----------------------------------------------------------
void WIFIHISTORY_SetMyNature(WIFI_HISTORY * wh, const NATURE_TYPE inType)
{
  if ( inType < NATURE_MAX ) wh->myNature = inType;
}

//----------------------------------------------------------
/**
 * @brief	�����̎���Z�b�g
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @param inFavorite    ��R�[�h
 * @return none
 */
//----------------------------------------------------------
void WIFIHISTORY_SetMyFavorite(WIFI_HISTORY * wh, const int inFavorite)
{
  if ( inFavorite < HOBBY_ID_NUM ) wh->myFavorite = inFavorite;
}

//----------------------------------------------------------
/**
 * @brief	�����̎���Z�b�g
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @param inFavorite    ��R�[�h
 * @return none
 */
//----------------------------------------------------------
u8 WIFIHISTORY_GetMyFavorite(WIFI_HISTORY * wh )
{
  return wh->myFavorite;
}

//----------------------------------------------------------
/**
 * @brief	�����̏������Ă��鍑�A�f�[�^���擾
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @param inIdx   �擾�������A�f�[�^�C���f�b�N�X
 * @param inType  �擾���������̎�� wifihistory.h �Q��
 * @return int    �擾���
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
  case UN_INFO_RECV_POKE:       //��������|�P����
    info = un->recvPokemon;
    break;
  case UN_INFO_SEND_POKE:      //�������|�P����
    info = un->sendPokemon;
    break;
  case UN_INFO_FAVORITE:       //�
    info = un->favorite;
    break;
  case UN_INFO_COUNTRY_NUM:    //�����������̐�
    info = un->countryCount;
    break;
  case UN_INFO_NATURE:         //���i
    info = un->nature;
    break;
  case UN_INFO_TALK:           //�b�������Ƃ��邩�H
    info = un->bTalk;
    break;
  case UN_INFO_VALID:          //�f�[�^�͗L�����H
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
 * @brief	�����̏������Ă��鍑�A�f�[�^���擾
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @param inIdx   �X�V�������A�f�[�^�C���f�b�N�X
 * @param inType  �X�V���������̎�� wifihistory.h �Q��
 * @param inInfo �@�X�V���
 * @return none
 */
//----------------------------------------------------------
void WIFIHISTORY_SetUnInfo(WIFI_HISTORY * wh, const int inIdx, const UN_INFO_TYPE inType, const u32 inInfo)
{
  UNITEDNATIONS_SAVE *un;

  if (inIdx >= UNITEDNATIONS_PEOPLE_MAX)
  {
    GF_ASSERT_MSG(0,"INDEX_OVER idx=%d",inIdx);
    return;
  }
  un = &wh->aUnitedPeople[inIdx];
  switch(inType){
  case UN_INFO_RECV_POKE:       //��������|�P����
    //@todo 2010/02/02 ���̂Ƃ��떢�g�p
/**    
    if ( (0 < inInfo) && (inInfo <= MONSNO_END) ) un->recvPokemon = inInfo;
    else GF_ASSERT_MSG(0,"monsno error %d", inInfo);
*/    
    break;
  case UN_INFO_SEND_POKE:      //�������|�P����
    //@todo 2010/02/02 ���̂Ƃ��떢�g�p
/**
    if ( (0 < inInfo) && (inInfo <= MONSNO_END) ) un->sendPokemon = inInfo;
    else GF_ASSERT_MSG(0,"monsno error %d", inInfo);
*/    
    break;
  case UN_INFO_FAVORITE:       //�
    //@todo 2010/02/02 ���̂Ƃ��떢�g�p
/**    
    if (inInfo < 5) un->favorite = inInfo;
    else GF_ASSERT_MSG(0,"favorite error %d", inInfo);
*/    
    break;
  case UN_INFO_COUNTRY_NUM:    //�����������̐�
    //@todo 2010/02/02 ���̂Ƃ��떢�g�p
/**    
    if (inInfo <= WIFI_COUNTRY_MAX) un->countryCount = inInfo;
    else GF_ASSEERT_MSG(0,"contry error %d", inInfo);
*/    
    break;
  case UN_INFO_NATURE:         //���i
    if (inInfo < 5 ) un->nature = inInfo;
    else GF_ASSERT_MSG(0,"nature error %d", inInfo);
    break;
  case UN_INFO_TALK:           //�b�������Ƃ��邩�H
    if (inInfo) un->bTalk = 1;
    else un->bTalk = 0;
    break;
  case UN_INFO_VALID:          //�f�[�^�͗L�����H
    if (inInfo) un->valid = 1;
    else un->valid = 0;
    break;
  default:
    GF_ASSERT_MSG(0,"TYPE ERROR %d",inType);
  }
}


//----------------------------------------------------------
/**
 * @brief	�����̏������Ă��鍑�A�f�[�^���擾
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @param inIdx   �擾�������A�f�[�^�C���f�b�N�X
 * @param inType  �擾���������̎�� wifihistory.h �Q��
 * @return int    �擾���
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
 * @brief	���A�f�[�^�擪�|�C���^�擾
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @return adr    �擪�A�h���X
 */
//----------------------------------------------------------
UNITEDNATIONS_SAVE *WIFIHISTORY_GetUNDataPtr(WIFI_HISTORY * wh)
{
  return wh->aUnitedPeople;
}

//----------------------------------------------------------
/**
 * @brief	�w��C���f�b�N�X�̍��r�b�g�𗧂Ă�
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @param inCountryCode
 * @return BOOL �V�K�r�b�g���Đ���
 */
//----------------------------------------------------------
BOOL WIFIHISTORY_SetCountryBit(WIFI_HISTORY * wh, const u32 inCountryCode)
{
  BOOL rc;
  rc = FALSE;
  //�w��C���f�b�N�I�[�o�[�`�F�b�N
  if (inCountryCode < WIFI_COUNTRY_MAX)
  {
    u32 idx;
    u8 data;
    u8 bit_pos; //0�`7
    

    if ( !WIFIHISTORY_CheckCountryBit(wh, inCountryCode) ) rc = TRUE;

    //�w�荑�̃r�b�g���i�[����Ă���1�o�C�g�f�[�^��z�񂩂�擾
    idx = inCountryCode / 8;
    data = wh->billopen[idx];
    //�Ή�����r�b�g�ʒu���Z�o
    bit_pos = inCountryCode % 8;
    //�r�b�gON
    data |= (1 << bit_pos);
    //�z��Ɋi�[���Ȃ���
    wh->billopen[idx] = data;
  }

  return rc;
}

//----------------------------------------------------------
/**
 * @brief	�w��C���f�b�N�X�̍��r�b�g���`�F�b�N����
 * @param	wh			WIFI�����f�[�^�ւ̃|�C���^
 * @param inCountryCode
 * @return BOOL     �r�b�g�������Ă�����TRUE
 */
//----------------------------------------------------------
BOOL WIFIHISTORY_CheckCountryBit(WIFI_HISTORY * wh, const u32 inCountryCode)
{
  BOOL rc = FALSE;
  //�w��C���f�b�N�I�[�o�[�`�F�b�N
  if (inCountryCode < WIFI_COUNTRY_MAX)
  {
    u32 idx;
    u8 data;
    u8 bit_pos; //0�`7
    u8 bit; //0 or 1
    //�w�荑�̃r�b�g���i�[����Ă���1�o�C�g�f�[�^��z�񂩂�擾
    idx = inCountryCode / 8;
    data = wh->billopen[idx];
    //�Ή�����r�b�g�ʒu���Z�o
    bit_pos = inCountryCode % 8;
    //�r�b�g�`�F�b�N
    bit = (data >> bit_pos) & 0x1;
    if (bit) rc = TRUE;
  }
  return rc;
}

