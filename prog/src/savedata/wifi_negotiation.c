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
#include "savedata/mystatus_local.h"
#include "system/pms_data.h"
#include "buflen.h"

//----------------------------------------------------------
/**
 * @brief  �\���̒�`
 */
//----------------------------------------------------------

struct _WIFI_NEGOTIATION_SAVEDATA{
  MYSTATUS aMyStatus[WIFI_NEGOTIATION_DATAMAX];
  u16 date[WIFI_NEGOTIATION_DATAMAX];         //��������
  PMS_DATA message;   //�ȈՕ� ���ȏЉ
  u16 count;   //�f�[�^�z�ׂ̈̃J�E���^
  u16 num;  //�����̌����|�P������
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
 * @param   pSV       WIFI_NEGOTIATION_SAVEDATA�|�C���^
 * @param   pMyStatus MYSTATUS
 */
//--------------------------------------------------------------------------------------------
void WIFI_NEGOTIATION_SV_SetFriend(WIFI_NEGOTIATION_SAVEDATA* pSV,const MYSTATUS* pMyStatus)
{
  int profileID = MyStatus_GetProfileID(pMyStatus);

  if(profileID==0){
    return;
  }
  if(WIFI_NEGOTIATION_SV_IsCheckFriend(pSV,profileID)){
    return;
  }
  if(pSV->count >= WIFI_NEGOTIATION_DATAMAX){
    pSV->count = 0;
  }
  MyStatus_Copy(pMyStatus, &pSV->aMyStatus[pSV->count]);
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
  int i = index;
  GF_ASSERT(index < WIFI_NEGOTIATION_DATAMAX);

  if(index >= WIFI_NEGOTIATION_DATAMAX){
    i=0;
  }
  return MyStatus_GetProfileID(&pSV->aMyStatus[ i ]);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   Wi-Fi�l�S�V�G�[�V�����pMYSTATUS�������o��
 * @param   WIFI_NEGOTIATION_SAVEDATA�|�C���^
 * @param   index �C���f�b�N�X WIFI_NEGOTIATION_DATAMAX�܂�
 * @return	profileID
 */
//--------------------------------------------------------------------------------------------
MYSTATUS* WIFI_NEGOTIATION_SV_GetMyStatus(WIFI_NEGOTIATION_SAVEDATA* pSV,u32 index)
{
  int i = index;
  GF_ASSERT(index < WIFI_NEGOTIATION_DATAMAX);

  if(index >= WIFI_NEGOTIATION_DATAMAX){
    return NULL;
  }
  if(0==WIFI_NEGOTIATION_SV_GetFriend(pSV,index)){
    return NULL;
  }
  return &pSV->aMyStatus[ i ];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �n�����v���t�@�C��ID�Ɉ�v���镨�����邩�ǂ���������
 * @param   WIFI_NEGOTIATION_SAVEDATA�|�C���^
 * @param   profile ����profileID
 * @return	��v������TRUE
 */
//--------------------------------------------------------------------------------------------
BOOL WIFI_NEGOTIATION_SV_IsCheckFriend(WIFI_NEGOTIATION_SAVEDATA* pSV,s32 profile)
{
  int i;

  for(i=0;i<WIFI_NEGOTIATION_DATAMAX;i++){
    if(MyStatus_GetProfileID(&pSV->aMyStatus[ i ]) == profile){
      return TRUE;
    }
  }
  return FALSE;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   �ۑ��ԍ���Ԃ�
 * @param   WIFI_NEGOTIATION_SAVEDATA�|�C���^
 * @return	����
 */
//--------------------------------------------------------------------------------------------
int WIFI_NEGOTIATION_SV_GetCount(WIFI_NEGOTIATION_SAVEDATA* pSV)
{
  return pSV->count;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   �n�����v���t�@�C��ID�Ɉ�v���镨�����邩�ǂ���������
 * @param   WIFI_NEGOTIATION_SAVEDATA�|�C���^
 * @param   profile ����profileID
 * @return	��v������TRUE
 */
//--------------------------------------------------------------------------------------------
#if 0
void WIFI_NEGOTIATION_SV_GetDate(WIFI_NEGOTIATION_SAVEDATA* pSV,RTCDate * date)
{
//  RTCDate * date
  int i;
}
#endif



//----------------------------------------------------------
//	�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
WIFI_NEGOTIATION_SAVEDATA* WIFI_NEGOTIATION_SV_GetSaveData(SAVE_CONTROL_WORK* pSave)
{
	WIFI_NEGOTIATION_SAVEDATA* pData;
	pData = SaveControl_DataPtrGet(pSave, GMDATA_ID_WIFI_NEGOTIATION);
	return pData;

}

