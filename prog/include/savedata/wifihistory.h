//============================================================================================
/**
 * @file	wifihistory.h
 * @brief
 * @date	2006.03.25
 */
//============================================================================================

#ifndef	__WIFIHISTORY_H__
#define	__WIFIHISTORY_H__

#include "savedata/save_control.h"
#include "savedata/mystatus.h"

//============================================================================================
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	WiFi�ʐM�����f�[�^�̕s���S�^�錾
 */
//----------------------------------------------------------
typedef struct _WIFI_HISTORY WIFI_HISTORY;

///WiFi�ʐM�������c�����̐�
#define	WIFI_NATION_MAX		(256)

///WiFi�ʐM�������c�����̂��ꂼ��̒n��R�[�h�̍ő吔
#define	WIFI_AREA_MAX		(64)


///���{�̒n���V��`(�{����country103�Ȃ񂾂��Q�Ƃł��Ȃ��̂Łj
#define WIFI_NATION_JAPAN	( 103 ) 

///�����̂��鍑�̐��iwifi_earth/wifi_place_msg_world.gmm�j
#define	WIFI_COUNTRY_MAX		(234)

///�n�於�̂���n��̐��iwifi_earth/wifi_place_msg_USA.gmm�j
#define	WIFI_AREADATA_MAX		(52)

//----------------------------------------------------------
/**
 * @brief	WiFi�ʐM�����f�[�^�̏�Ԓ�`
 */
//----------------------------------------------------------
typedef enum {
	WIFIHIST_STAT_NODATA = 0,	///<�܂��ʐM�������Ƃ��Ȃ�
	WIFIHIST_STAT_NEW = 1,		///<�{�����߂ĒʐM����
	WIFIHIST_STAT_EXIST = 2,	///<�ʐM�������Ƃ�����
	WIFIHIST_STAT_MINE = 3,		///<�����̏ꏊ

	WIFIHIST_STAT_MAX,
}WIFIHIST_STAT;

typedef enum {
  UN_INFO_RECV_POKE,       //��������|�P����
  UN_INFO_SEND_POKE,      //�������|�P����
  UN_INFO_FAVORITE,       //�
  UN_INFO_COUNTRY_NUM,    //�����������̐�
  UN_INFO_NATURE,         //���i
  UN_INFO_TALK,           //�b�������Ƃ��邩�H
  UN_INFO_VALID,          //�f�[�^�͗L�����H
}UN_INFO_TYPE;

//----------------------------------------------------------
//----------------------------------------------------------
//���[�N�T�C�Y�擾�i�Z�[�u�V�X�e������Ă΂��j
extern int WIFIHISTORY_GetWorkSize(void);
//�������i�Z�[�u�V�X�e������Ă΂��j
extern void WIFIHISTORY_Init(WIFI_HISTORY * hist);
//�Z�[�u�f�[�^�擾�i�g���ӏ��ŌĂԁj
extern WIFI_HISTORY * SaveData_GetWifiHistory(SAVE_CONTROL_WORK * sv);

//�����̍��ƒn��o�^
extern void WIFIHISTORY_SetMyNationArea(WIFI_HISTORY * wh, MYSTATUS* my, int nation, int area);

extern void WIFIHISTORY_SetStat(WIFI_HISTORY * wh, int nation, int area, WIFIHIST_STAT stat);
extern WIFIHIST_STAT WIFIHISTORY_GetStat(const WIFI_HISTORY * wh, int nation, int area);

extern BOOL WIFIHISTORY_GetWorldFlag(const WIFI_HISTORY * wh);
extern void WIFIHISTORY_SetWorldFlag(WIFI_HISTORY * wh, BOOL flag);

//���t�ɂ��X�V����
extern void WIFIHISTORY_Update(WIFI_HISTORY * wh);

extern u8 WIFIHISTORY_GetMyCountryCount(WIFI_HISTORY * wh);

extern int WIFIHISTORY_GetUnInfo(WIFI_HISTORY * wh, const int inIdx, const UN_INFO_TYPE inType);

#endif	/* __WIFIHISTORY_H__ */

