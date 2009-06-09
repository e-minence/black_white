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


struct _WIFI_STATUS{
	MYSTATUS aMyStatus;   // MYSTATUS                          32
	u8 VChatMac[6];       // VChat���������l��Mac�A�h���X      38
	u8 WifiMode;         // WIFI�ł̃Q�[�����̏��             39
	u8 VChatStatus;       // VChat�̏��                       40
	u8 Active;          // VChat�̏��                         41
	u8 dummy[64-41];  // �����p ����0
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

const MYSTATUS* WIFI_STATUS_GetMyStatus(const WIFI_STATUS* pStatus)
{
	return &pStatus->aMyStatus;
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
 * @param   ��r�����WIFI_STATUS
 * @param   ��r����MAC�A�h���X
 * @return	,,
 */
//----------------------------------------------------------

BOOL WIFI_STATUS_IsVChatMac(const WIFI_STATUS* pStatus, const u8* SearchMacAddress)
{
	if(0==GFL_STD_MemComp(SearchMacAddress,pStatus->VChatMac, 6))
	{
		return TRUE;
	}
	return FALSE;
}


