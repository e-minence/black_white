#pragma once
//============================================================================================
/**
 * @file	  wifi_status.h
 * @brief	  WIFI�ɗ������̊Ǘ�
 * @author	k.ohno
 * @date	  2009.6.6
 */
//============================================================================================
//#include "savedata/mystatus.h"
//#include "savedata/mystatus_local.h"

typedef struct _WIFI_STATUS WIFI_STATUS;


//---------------WIFISTATUS�f�[�^
typedef enum{
  WIFI_GAME_NONE,   // ��������	NONE�̂Ƃ��͏o�������܂���
  WIFI_GAME_LOGIN_WAIT,    // �ҋ@�� �����I��Ŗ���
  WIFI_GAME_UNIONMATCH,   // �q�������B�q������Ɋe�Q�[���ɐi��
  WIFI_GAME_VCT,      // VCT
  WIFI_GAME_TVT,          // TV�g�����V�[�o
  WIFI_GAME_TRADE,          // ����
  WIFI_GAME_BATTLE_SINGLE_ALL,      // �V���O���o�g��
  WIFI_GAME_BATTLE_SINGLE_FLAT,      // �V���O���o�g��
  WIFI_GAME_BATTLE_DOUBLE_ALL,      // �_�u��
  WIFI_GAME_BATTLE_DOUBLE_FLAT,      // �_�u��
  WIFI_GAME_BATTLE_TRIPLE_ALL,      // �g���v��
  WIFI_GAME_BATTLE_TRIPLE_FLAT,      // �g���v��
  WIFI_GAME_BATTLE_ROTATION_ALL,      // ���[�e�[�V����
  WIFI_GAME_BATTLE_ROTATION_FLAT,      // ���[�e�[�V����
  WIFI_GAME_UNKNOWN,   // �V���ɍ�����炱�̔ԍ��ȏ�ɂȂ�

} WIFI_GAME_e;



typedef enum{
  WIFI_STATUS_NONE,   // ��������	NONE�̂Ƃ��͏o�������܂���
  WIFI_STATUS_WAIT,   // �ҋ@��
  WIFI_STATUS_RECRUIT,    // ��W��  = �N�ł��q�������\
  WIFI_STATUS_CALL,   //���̐l�ɐڑ����悤�Ƃ��Ă���
  WIFI_STATUS_PLAYING,      // ���̐l�Ɛڑ���
  WIFI_STATUS_PLAY_AND_RECRUIT,      // ���̐l�Ɛڑ����ł���ɕ�W��
  WIFI_STATUS_UNKNOWN,   // �V���ɍ�����炱�̔ԍ��ȏ�ɂȂ�

} WIFI_STATUS_e;




// dwc_rap.h�ɂ����`�����₷�����\
// #define MYDWC_STATUS_DATA_SIZE_MAX (128)
// WIFI�t�����h���T�C�Y DWC��189�o�C�g�\�Ƃ��邪�A��`�������̂ōT���߃T�C�Y�Ō��߂���
// ���₵������ƑS�̂ɏd���Ȃ�

//#define _POKEMON_NUM   (6)

struct _WIFI_STATUS{
  u32 profileID;                                 //�v���t�@�C��ID
//	MYSTATUS aMyStatus;   // MYSTATUS                           ROM LANG�͂��̒��ɂ���
	u8 VChatMac[6];       // �Q�[�����Ăт�����l��Mac�A�h���X ������MAC�������ꍇ�J�n      
	u8 MyMac[6];       // ������Mac�A�h���X
  u8 trainer_view;	// ���j�I�����[�����ł̌�����
  u8 pm_version;	// �o�[�W����
	u8 pm_lang;		  // ����
	u8 GameMode;         // WIFI�ł̃Q�[�����̏��         WIFI_GAME_e
  u8 status;    // �����̏��                           WIFI_STATUS_e
  u8 nation;           //  ��
  u8 area;             //  �n��
  u8 shooter:1;    //�V���[�^�[�L�薳��
  u8 VChatStatus:1;       // VChat�̏��
  u8 sex:2;
  u8 dummybit:4;
  u8 dummy2;   //��ʃo�[�W�����p
  u8 dummy3;   //��ʃo�[�W�����p
  u8 dummy4;   //��ʃo�[�W�����p
};


//extern MYSTATUS* WIFI_STATUS_GetMyStatus(WIFI_STATUS* pStatus);
//extern void WIFI_STATUS_SetMyStatus(WIFI_STATUS* pStatus, const MYSTATUS* pMy);

extern u8 WIFI_STATUS_GetWifiStatus(const WIFI_STATUS* pStatus);
extern void WIFI_STATUS_SetWifiStatus( WIFI_STATUS* pStatus, u8 mode);

extern u8 WIFI_STATUS_GetGameMode(const WIFI_STATUS* pStatus);
extern void WIFI_STATUS_SetGameMode( WIFI_STATUS* pStatus, u8 mode);



extern u8 WIFI_STATUS_GetVChatStatus(const WIFI_STATUS* pStatus);
extern void WIFI_STATUS_SetVChatStatus(WIFI_STATUS* pStatus,u8 vct);

extern void WIFI_STATUS_SetMyNation(WIFI_STATUS* pStatus,u8 no);
extern void WIFI_STATUS_SetMyArea(WIFI_STATUS* pStatus,u8 no);

extern u8 WIFI_STATUS_GetActive(const WIFI_STATUS* pStatus);
extern BOOL WIFI_STATUS_IsVChatMac(const WIFI_STATUS* pMyStatus, const WIFI_STATUS* pFriendStatus);
extern void WIFI_STATUS_SetVChatMac(WIFI_STATUS* pStatus, const WIFI_STATUS* pFriendStatus);
extern void WIFI_STATUS_ResetVChatMac(WIFI_STATUS* pStatus);
extern void WIFI_STATUS_SetMyMac(WIFI_STATUS* pStatus);


static inline void WIFI_STATUS_SetTrainerView(WIFI_STATUS* pStatus,u8 trainer_view){ pStatus->trainer_view = trainer_view; }
static inline u8 WIFI_STATUS_GetTrainerView(WIFI_STATUS* pStatus){ return pStatus->trainer_view; }

static inline void WIFI_STATUS_SetPMVersion(WIFI_STATUS* pStatus,u8 pm_version){ pStatus->pm_version = pm_version; }
static inline void WIFI_STATUS_SetPMLang(WIFI_STATUS* pStatus,u8 pm_lang){ pStatus->pm_lang = pm_lang; }
static inline void WIFI_STATUS_SetSex(WIFI_STATUS* pStatus,u8 sex){ pStatus->sex = sex; }


//extern void WIFI_STATUS_SetTrainerView(WIFI_STATUS* pStatus,u8 trainer_view);
