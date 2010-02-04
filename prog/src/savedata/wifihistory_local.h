//============================================================================================
/**
 * @file	wifihistory_local.h
 * @brief	WiFi�ʐM�����f�[�^�\����`�w�b�_
          ���A�Z�[�u�f�[�^��ǉ�  ���֌W�Ƃ��Ă܂Ƃ߂������s�����ǂ���
 * @date	2006.03.25
 */
//============================================================================================

#ifndef	__WIFIHISTORY_LOCAL_H__
#define	__WIFIHISTORY_LOCAL_H__

#include "savedata/wifihistory.h"
#include "savedata/mystatus_local.h"

//============================================================================================
//============================================================================================

enum {
	///�����ƂɊm�ۂ���f�[�^�T�C�Y�i�o�C�g�P�ʁj
	NATION_DATA_SIZE = WIFI_AREA_MAX * 2 / 8,
	//���P�f�[�^������2�r�b�g�K�v�Ńo�C�g�P�ʂɒ������߂�8�Ŋ����Ă���
};


enum {
  //���A�̃t���A�̃T�C�Y
  BILL_DATA_SIZE = (WIFI_COUNTRY_MAX+7) / 8,
  //���A�ɂƂ��Ă����l�̐�
  UNITEDNATIONS_PEOPLE_MAX = 20,
};



typedef struct _UNITEDNATIONS_SAVE_tag{
  MYSTATUS aMyStatus;
  u16 recvPokemon;  //������|�P����
  u16 sendPokemon;  //�������|�P����
  u8 favorite;   //�
  u8 countryCount;  //�����������̉�
  u8 nature:3;   //���i
  u8 bTalk:1;   //�b�������ǂ���
  u8 valid:1;  //�L���f�[�^���H
  u8 dummy1:3;
  u8 dummy2;
}_UNITEDNATIONS_SAVE;

//----------------------------------------------------------
/**
 * @brief	WiFi�ʐM�����f�[�^�p�\���̒�`
 */
//----------------------------------------------------------
struct _WIFI_HISTORY{
  /// ���A�ɂƂ��Ă����l
  _UNITEDNATIONS_SAVE aUnitedPeople[UNITEDNATIONS_PEOPLE_MAX];
  u8 billopen[BILL_DATA_SIZE];  ///< ���������Ƃ�����r���̃t���A
	///���t
	u32	date;
	u8 world_flag;			///<�S���E�Ώۃ��[�h���ǂ����̃t���O
  u8 myCountryCount;  ///< ���J���ƌ�����������
  u8 myFavorite;     ///�����̎
  u8 myNature;       ///�����̐��i
	///�����f�[�^
	u8	data[(WIFI_NATION_MAX - 1) * NATION_DATA_SIZE];

};

#endif	/* __WIFIHISTORY_LOCAL_H__ */
