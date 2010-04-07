//============================================================================================
/**
 * @file	  wifi_status.c
 * @brief	  WIFI�ɗ������̊Ǘ�
            �Z�[�u����f�[�^�ł͂Ȃ��� mystatus�\���̂̓���̂��߂����ɂ���
 * @author	k.ohno
 * @date	  2009.6.6
 */
//============================================================================================

#include "gflib.h"
#include "wifi_status.h"


//----------------------------------------------------------
/**
 * @brief	  MYSTATUS�|�C���^��Ԃ�
 * @return	,,
 */
//----------------------------------------------------------

MYSTATUS* WIFI_STATUS_GetMyStatus(WIFI_STATUS* pStatus)
{
	return &pStatus->aMyStatus;
}


//----------------------------------------------------------
/**
 * @brief	  MYSTATUS�|�C���^��Ԃ�
 * @return	,,
 */
//----------------------------------------------------------

extern void WIFI_STATUS_SetMyStatus(WIFI_STATUS* pStatus, const MYSTATUS* pMy)
{
	MyStatus_Copy(pMy,&pStatus->aMyStatus);
}

//----------------------------------------------------------
/**
 * @brief	  WifiStatus��Ԃ�
 * @return	,,
 */
//----------------------------------------------------------

u8 WIFI_STATUS_GetWifiStatus(const WIFI_STATUS* pStatus)
{
	return pStatus->status;
}

//----------------------------------------------------------
/**
 * @brief	  WifiStatus��Ԃ�
 * @return	,,
 */
//----------------------------------------------------------

void WIFI_STATUS_SetWifiStatus( WIFI_STATUS* pStatus, u8 mode)
{
	 pStatus->status = mode;
}

//----------------------------------------------------------
/**
 * @brief	  GameMode��Ԃ�
 * @return	,,
 */
//----------------------------------------------------------

u8 WIFI_STATUS_GetGameMode(const WIFI_STATUS* pStatus)
{
	return pStatus->GameMode;
}

//----------------------------------------------------------
/**
 * @brief	  GameMode��Ԃ�
 * @return	,,
 */
//----------------------------------------------------------

void WIFI_STATUS_SetGameMode( WIFI_STATUS* pStatus, u8 mode)
{
	 pStatus->GameMode = mode;
}


//----------------------------------------------------------
/**
 * @brief	  �{�C�X�`���b�g����Ԃ��ǂ�����Ԃ�
 * @return	,,
 */
//----------------------------------------------------------

u8 WIFI_STATUS_GetVChatStatus(const WIFI_STATUS* pStatus)
{
	return pStatus->VChatStatus;
}

//----------------------------------------------------------
/**
 * @brief	  �{�C�X�`���b�g����Ԃ��ǂ�����ݒ�
 * @return	,,
 */
//----------------------------------------------------------

void WIFI_STATUS_SetVChatStatus(WIFI_STATUS* pStatus, u8 vct)
{
	pStatus->VChatStatus= vct;
}

//----------------------------------------------------------
/**
 * @brief	  ���̐l��VCHAT�������Ƃ���MAC�A�h���X����v���Ă邩�ǂ�����Ԃ�
 * @param   ������WIFI_STATUS
 * @param   ��r����WIFI_STATUS
 * @return	,,
 */
//----------------------------------------------------------

BOOL WIFI_STATUS_IsVChatMac(const WIFI_STATUS* pMyStatus, const WIFI_STATUS* pFriendStatus)
{
	if(0==GFL_STD_MemComp(pMyStatus->MyMac,pFriendStatus->VChatMac, 6))
	{
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------
/**
 * @brief	  �q�������l��MACADDRESS����
 * @param   ������WIFI_STATUS
 * @param   �q�������l��WIFI_STATUS
 * @return	�Ȃ�
 */
//----------------------------------------------------------

void WIFI_STATUS_SetVChatMac(WIFI_STATUS* pStatus, const WIFI_STATUS* pFriendStatus)
{
  pStatus->callcounter++;  //���̐ڑ��ł��鎖���ؖ�����ׂ̃J�E���^�������C���N�������g
  GFL_STD_MemCopy(pFriendStatus->MyMac,pStatus->VChatMac, 6);
}

//----------------------------------------------------------
/**
 * @brief	  �q�������l��MACADDRESS����
 * @param   ������WIFI_STATUS
 * @return	�Ȃ�
 */
//----------------------------------------------------------

void WIFI_STATUS_ResetVChatMac(WIFI_STATUS* pStatus)
{
  u8 macnull[]={0,0,0,0,0,0};
  GFL_STD_MemCopy(macnull,pStatus->VChatMac, 6);
  OS_TPrintf("WIFI_STATUS_ResetVChatMac\n");
}

//----------------------------------------------------------
/**
 * @brief	  ������MACADDRESS����
 * @param   ������WIFI_STATUS
 * @return	,,
 */
//----------------------------------------------------------

void WIFI_STATUS_SetMyMac(WIFI_STATUS* pStatus)
{
  OS_GetMacAddress(pStatus->MyMac);  
}



#if 0
void WIFI_STATUS_SetMyNation(WIFI_STATUS* pStatus,u8 no)
{
	pStatus->nation = no;
}

void WIFI_STATUS_SetMyArea(WIFI_STATUS* pStatus,u8 no)
{
	pStatus->area = no;
}
#endif
//----------------------------------------------------------
/**
 * @brief	�����̃R�[���J�E���^�[���擾
 * @param	WIFI_STATUS
 * @return	�J�E���^��
 */
//----------------------------------------------------------
u8 WIFI_STATUS_GetCallCounter(WIFI_STATUS* pStatus)
{
	return pStatus->callcounter;
}

u8 WIFI_STATUS_GetTrainerView(WIFI_STATUS* pStatus)
{
  return MyStatus_GetTrainerView(&pStatus->aMyStatus);
}

/*
static inline void WIFI_STATUS_SetTrainerView(WIFI_STATUS* pStatus,u8 trainer_view){ pStatus->trainer_view = trainer_view; }

static inline void WIFI_STATUS_SetPMVersion(WIFI_STATUS* pStatus,u8 pm_version){ pStatus->pm_version = pm_version; }
static inline void WIFI_STATUS_SetPMLang(WIFI_STATUS* pStatus,u8 pm_lang){ pStatus->pm_lang = pm_lang; }
static inline void WIFI_STATUS_SetSex(WIFI_STATUS* pStatus,u8 sex){ pStatus->sex = sex; }
static inline u8 WIFI_STATUS_GetSex(WIFI_STATUS* pStatus){ return pStatus->sex; }
*/
