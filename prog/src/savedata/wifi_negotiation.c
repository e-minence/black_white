//============================================================================================
/**
 * @file	  wifi_negotiation.c
 * @brief	  WIFI�l�S�V�G�[�V�����p�Ƃ����������f�[�^
 * @author	k.ohno
 * @date	  2009.10.16
 */
//============================================================================================

#include "gflib.h"

#include "savedata/save_tbl.h"
#include "savedata/wifi_negotiation.h"

//----------------------------------------------------------
/**
 * @brief  �\���̒�`
 */
//----------------------------------------------------------

struct _WIFI_NEGOTIATION_SAVEDATA{
  s32 aFriendData[WIFI_NEGOTIATION_DATAMAX];
  u32 count;
};


//--------------------------------------------------------------------------------------------
/**
 * @brief   �\���̃T�C�Y�𓾂�
 * @return	�T�C�Y
 */
//--------------------------------------------------------------------------------------------
int WIFI_NEGOTIATION_SV_GetWorkSize(void)
{
	return sizeof(WIFI_NEGOTIATION_SAVEDATA);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �\���̂𓾂�
 * @param   HEAPID
 * @return	�\����
 */
//--------------------------------------------------------------------------------------------

WIFI_NEGOTIATION_SAVEDATA* WIFI_NEGOTIATION_SV_AllocWork(HEAPID heapID)
{
	return GFL_HEAP_AllocClearMemory(heapID, WIFI_NEGOTIATION_SV_GetWorkSize());
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ���g������������
 * @return	�T�C�Y
 */
//--------------------------------------------------------------------------------------------

void WIFI_NEGOTIATION_SV_Init(WIFI_NEGOTIATION_SAVEDATA* pSV)
{
	GFL_STD_MemClear(pSV, WIFI_NEGOTIATION_SV_GetWorkSize());
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   Wi-Fi�l�S�V�G�[�V�����p�Ƃ������f�[�^�����[�v�ۑ�
 * @param   WIFI_NEGOTIATION_SAVEDATA�|�C���^
 * @param   profileID  ProfileID
 * @return	type  �p�l���^�C�v CGEAR_PANELTYPE_ENUM
 */
//--------------------------------------------------------------------------------------------
void WIFI_NEGOTIATION_SV_SetFriend(WIFI_NEGOTIATION_SAVEDATA* pSV,s32 profileID)
{
  if(pSV->count >= WIFI_NEGOTIATION_DATAMAX){
    pSV->count=0;
  }
  pSV->aFriendData[ pSV->count ] = profileID;
  pSV->count++;  //�J��Ԃ��ۑ�
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   Wi-Fi�l�S�V�G�[�V�����p�Ƃ������f�[�^�������o��
 * @param   WIFI_NEGOTIATION_SAVEDATA�|�C���^
 * @param   index �C���f�b�N�X WIFI_NEGOTIATION_DATAMAX�܂�
 * @return	profileID
 */
//--------------------------------------------------------------------------------------------
s32 WIFI_NEGOTIATION_SV_GetFriend(WIFI_NEGOTIATION_SAVEDATA* pSV,u32 index)
{
  GF_ASSERT(index < WIFI_NEGOTIATION_DATAMAX);
  if(index >= WIFI_NEGOTIATION_DATAMAX){
    return pSV->aFriendData[0];
  }
  return pSV->aFriendData[ index ];
}

//----------------------------------------------------------
//	�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
WIFI_NEGOTIATION_SAVEDATA* WIFI_NEGOTIATION_SV_GetSaveData(SAVE_CONTROL_WORK* pSave)
{
	WIFI_NEGOTIATION_SAVEDATA* pData;
	pData = SaveControl_DataPtrGet(pSave, GMDATA_ID_WIFI_NEGOTIATION);
	return pData;

}

