//============================================================================================
/**
 * @file  wifi_country.c
 * @brief WI-FI�n����ʂŎg�p���镶���񓙂̃f�[�^���O�����J���邽�߂̎d�g��
 * @author  taya
 * @date  2006.04.22
 */
//============================================================================================
#include <gflib.h>

#include "message.naix"
#include "msg/msg_wifi_place_msg_world.h"
#include "wifi_earth_place.naix"

#include "net_app\wifi_country.h"

#include "msg/msg_wifi_place_msg_ARG.h"
#include "msg/msg_wifi_place_msg_AUS.h"
#include "msg/msg_wifi_place_msg_BRA.h"
#include "msg/msg_wifi_place_msg_CAN.h"
#include "msg/msg_wifi_place_msg_CHN.h"
#include "msg/msg_wifi_place_msg_DEU.h"
#include "msg/msg_wifi_place_msg_ESP.h"
#include "msg/msg_wifi_place_msg_FIN.h"
#include "msg/msg_wifi_place_msg_FRA.h"
#include "msg/msg_wifi_place_msg_GBR.h"
#include "msg/msg_wifi_place_msg_IND.h"
#include "msg/msg_wifi_place_msg_ITA.h"
#include "msg/msg_wifi_place_msg_JPN.h"
#include "msg/msg_wifi_place_msg_NOR.h"
#include "msg/msg_wifi_place_msg_POL.h"
#include "msg/msg_wifi_place_msg_RUS.h"
#include "msg/msg_wifi_place_msg_SWE.h"
#include "msg/msg_wifi_place_msg_USA.h"

//�f�[�^�\���́i���n��e�[�u���f�[�^�j
typedef struct EARTH_AREATABLE_tag
{
  u8    nationID;
  u8    place_dataID;
  u16   msg_arcID;
  
  u8    place_index_max;    ///<�n�於�̍ő吔
  u8    padding[3];   //�p�f�B���O
}EARTH_AREATABLE;

static const EARTH_AREATABLE NationFlag_to_AreaID[] = {
  {//�_�~�[
    country000,
    NARC_wifi_earth_place_place_pos_wrd_dat,
    NARC_message_wifi_place_msg_world_dat,
    0
  },
  { // �A���[���`��
    country009,
    NARC_wifi_earth_place_place_pos_ARG_dat,
    NARC_message_wifi_place_msg_ARG_dat,
    ARG24
  },
  { // �I�[�X�g�����A
    country012,
    NARC_wifi_earth_place_place_pos_AUS_dat,
    NARC_message_wifi_place_msg_AUS_dat,
    AUS07
  },
  { // �u���W��
    country028,
    NARC_wifi_earth_place_place_pos_BRA_dat,
    NARC_message_wifi_place_msg_BRA_dat,
    BRA27
  },
  { // �J�i�_
    country036,
    NARC_wifi_earth_place_place_pos_CAN_dat,
    NARC_message_wifi_place_msg_CAN_dat,
    CAN13
  },
  { // ����
    country043,
    NARC_wifi_earth_place_place_pos_CHN_dat,
    NARC_message_wifi_place_msg_CHN_dat,
    CHN33
  },
  { // �h�C�c
    country079,
    NARC_wifi_earth_place_place_pos_DEU_dat,
    NARC_message_wifi_place_msg_DEU_dat,
    DEU16
  },
  { // �X�y�C��
    country195,
    NARC_wifi_earth_place_place_pos_ESP_dat,
    NARC_message_wifi_place_msg_ESP_dat,
    ESP17
  },
  { // �t�B�������h
    country072,
    NARC_wifi_earth_place_place_pos_FIN_dat,
    NARC_message_wifi_place_msg_FIN_dat,
    FIN06
  },
  { // �t�����X
    country073,
    NARC_wifi_earth_place_place_pos_FRA_dat,
    NARC_message_wifi_place_msg_FRA_dat,
    FRA22
  },
  { // �C�M���X
    country218,
    NARC_wifi_earth_place_place_pos_GBR_dat,
    NARC_message_wifi_place_msg_GBR_dat,
    GBR12
  },
  { // �C���h
    country095,
    NARC_wifi_earth_place_place_pos_IND_dat,
    NARC_message_wifi_place_msg_IND_dat,
    IND35
  },
  { // �C�^���A
    country102,
    NARC_wifi_earth_place_place_pos_ITA_dat,
    NARC_message_wifi_place_msg_ITA_dat,
    ITA20
  },
  { // ���{
    country105,
    NARC_wifi_earth_place_place_pos_JPN_dat,
    NARC_message_wifi_place_msg_JPN_dat,
    JPN50
  },
  { // �m���E�F�[
    country155,
    NARC_wifi_earth_place_place_pos_NOR_dat,
    NARC_message_wifi_place_msg_NOR_dat,
    NOR20
  },
  { // �|�[�����h
    country166,
    NARC_wifi_earth_place_place_pos_POL_dat,
    NARC_message_wifi_place_msg_POL_dat,
    POL16
  },
  { // ���V�A
    country174,
    NARC_wifi_earth_place_place_pos_RUS_dat,
    NARC_message_wifi_place_msg_RUS_dat,
    RUS07
  },
  { // �X�E�F�[�f��
    country200,
    NARC_wifi_earth_place_place_pos_SWE_dat,
    NARC_message_wifi_place_msg_SWE_dat,
    SWE22
  },
  { // �A�����J
    country220,
    NARC_wifi_earth_place_place_pos_USA_dat,
    NARC_message_wifi_place_msg_USA_dat,
    USA51
  },
};




//------------------------------------------------------------------
/**
 * �f�[�^�����擾
 *
 * @retval  u32   
 */
//------------------------------------------------------------------
u32 WIFI_COUNTRY_GetDataLen( void )
{
  return NELEMS(NationFlag_to_AreaID);
}

//------------------------------------------------------------------
/**
 * ���R�[�h���f�[�^�C���f�b�N�X�ɕϊ�
 *
 * �f�[�^�C���f�b�N�X�́A�e��t���f�[�^���������邽�߂Ɏg�p����B
 * �f�[�^�C���f�b�N�X���O�̎��A���̍��̃��[�J���n��f�[�^�͑��݂��Ȃ��B
 *
 * @param   countryCode   ���R�[�h
 *
 * @retval  u32   �����f�[�^�C���f�b�N�X
 */
//------------------------------------------------------------------
u32 WIFI_COUNTRY_CountryCodeToDataIndex( u32 countryCode )
{
  u32 i;

  for(i=0; i<NELEMS(NationFlag_to_AreaID); i++)
  {
    if(NationFlag_to_AreaID[i].nationID == countryCode)
    {
      return i;
    }
  }

  return 0;

}

//------------------------------------------------------------------
/**
 * ���R�[�h����n�搔���擾
 *
 * @param   countryCode   ���R�[�h
 *
 * @retval  �n�搔(�n�悪���݂��Ȃ����̏ꍇ��0)
 */
//------------------------------------------------------------------
u32 WIFI_COUNTRY_CountryCodeToPlaceIndexMax( u32 countryCode )
{
  u32 i;

  for(i=0; i<NELEMS(NationFlag_to_AreaID); i++)
  {
    if(NationFlag_to_AreaID[i].nationID == countryCode)
    {
      return NationFlag_to_AreaID[i].place_index_max;
    }
  }

  return 0;

}

//------------------------------------------------------------------
/**
 * ���R�[�h���烍�[�J���n�於������̃��b�Z�[�W�f�[�^ID���擾
 *
 * @param   countryCode   ���R�[�h
 *
 * @retval  u32   �n�於������̃��b�Z�[�W�f�[�^ID�iARC_MSG���j
 */
//------------------------------------------------------------------
u32 WIFI_COUNTRY_CountryCodeToPlaceMsgDataID( u32 countryCode )
{
  u32 dataIndex = WIFI_COUNTRY_CountryCodeToDataIndex( countryCode );
  return WIFI_COUNTRY_DataIndexToPlaceMsgDataID( dataIndex );
}


//------------------------------------------------------------------
/**
 * �f�[�^�C���f�b�N�X���烍�[�J���n�於������̃��b�Z�[�W�f�[�^ID���擾
 *
 * @param   datID �f�[�^�C���f�b�N�X
 *
 * @retval  u32   �n�於������̃��b�Z�[�W�f�[�^ID�iARC_MSG���j
 */
//------------------------------------------------------------------
u32 WIFI_COUNTRY_DataIndexToPlaceMsgDataID( u32 dataIndex )
{
  GF_ASSERT(dataIndex < NELEMS(NationFlag_to_AreaID));

  return NationFlag_to_AreaID[dataIndex].msg_arcID;
}

//------------------------------------------------------------------
/**
 * �f�[�^�C���f�b�N�X���獑�R�[�h���擾
 *
 * @param   dataIndex   �f�[�^�C���f�b�N�X
 *
 * @retval  u32   ���R�[�h
 */
//------------------------------------------------------------------
u32 WIFI_COUNTRY_DataIndexToCountryCode( u32 dataIndex )
{
  GF_ASSERT(dataIndex < NELEMS(NationFlag_to_AreaID));

  return NationFlag_to_AreaID[dataIndex].nationID;
}


//------------------------------------------------------------------
/**
 * �f�[�^�C���f�b�N�X����n��f�[�^ID���擾
 *
 * @param   dataIndex   �f�[�^�C���f�b�N�X
 *
 * @retval  u32   �n��f�[�^ID
 */
//------------------------------------------------------------------
u32 WIFI_COUNTRY_DataIndexToPlaceDataID( u32 dataIndex )
{
  GF_ASSERT(dataIndex < NELEMS(NationFlag_to_AreaID));

  return NationFlag_to_AreaID[dataIndex].place_dataID;
}

