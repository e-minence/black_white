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
#include "savedata/wifi_status.h"
#include "mystatus_local.h"

#define _POKEMON_NUM   (6)

// dwc_rap.h�ɂ����`�����₷�����\
// #define MYDWC_STATUS_DATA_SIZE_MAX (128)
// WIFI�t�����h���T�C�Y DWC��189�o�C�g�\�Ƃ��邪�A��`�������̂ōT���߃T�C�Y�Ō��߂���
// ���₵������ƑS�̂ɏd���Ȃ�


struct _WIFI_STATUS{
  u16 pokemonType[_POKEMON_NUM];                    //12
  u16 hasItemType[_POKEMON_NUM];                   //24
	MYSTATUS aMyStatus;   // MYSTATUS                          56
	u8 VChatMac[6];       // �Q�[�����Ăт�����l��Mac�A�h���X ������MAC�������ꍇ�J�n      62
	u8 MyMac[6];       // ������Mac�A�h���X 68
	u8 WifiMode;         // WIFI�ł̃Q�[�����̏��             69
	u8 VChatStatus;       // VChat�̏��                       70
	u8 Active;          // VChat�̏��                         71
  u8 nation;           //  72
  u8 area;             //  73
	u8 dummy[124-73];  // �����p ����0   128
};

//----------------------------------------------------------
/**
 * @brief	  �\���̃T�C�Y��Ԃ�
 * @return	int		�T�C�Y�i�o�C�g�P�ʁj
 */
//----------------------------------------------------------

int WIFI_STATUS_GetSize(void)
{
	return sizeof(WIFI_STATUS);

}

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
 * @brief	  WifiMode��Ԃ�
 * @return	,,
 */
//----------------------------------------------------------

u8 WIFI_STATUS_GetWifiMode(const WIFI_STATUS* pStatus)
{
	return pStatus->WifiMode;
}

//----------------------------------------------------------
/**
 * @brief	  WifiMode��Ԃ�
 * @return	,,
 */
//----------------------------------------------------------

void WIFI_STATUS_SetWifiMode( WIFI_STATUS* pStatus, u8 mode)
{
	 pStatus->WifiMode = mode;
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
 * @brief	  �Q�[�����ő��̐l���󂯓�����Ȃ����ǂ�����Ԃ�
 * @return	,,
 */
//----------------------------------------------------------

u8 WIFI_STATUS_GetActive(const WIFI_STATUS* pStatus)
{
	return pStatus->Active;
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


void WIFI_STATUS_SetMonsNo(WIFI_STATUS* pStatus,int index,u16 no)
{
	pStatus->pokemonType[index] = no;
}

void WIFI_STATUS_SetItemNo(WIFI_STATUS* pStatus,int index,u16 no)
{
	pStatus->hasItemType[index] = no;
}

u16 WIFI_STATUS_GetMonsNo(WIFI_STATUS* pStatus,int index)
{
	return pStatus->pokemonType[index];
}

u16 WIFI_STATUS_GetItemNo(WIFI_STATUS* pStatus,int index)
{
	return pStatus->hasItemType[index];
}


void WIFI_STATUS_SetMyNation(WIFI_STATUS* pStatus,u8 no)
{
	pStatus->nation = no;
}

void WIFI_STATUS_SetMyArea(WIFI_STATUS* pStatus,u8 no)
{
	pStatus->area = no;
}
