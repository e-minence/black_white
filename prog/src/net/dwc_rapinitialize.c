//=============================================================================
/**
 * @file	dwc_rapinitialize.c
 * @bfief	���������ɕK�v��Wi-Fi�̏���
 * @author	k.ohno
 * @date	06/03/25
 */
//=============================================================================

#include <nitro.h>
#include <nnsys.h>
#include "gflib.h"

#if GFL_NET_WIFI

//#include "net_wireless.h"
#include "net/dwc_rapcommon.h"
#include "dwc_rapinitialize.h"



//==============================================================================
/**
 * @brief   Wi-Fi�ڑ������� ���������ɌĂяo���Ă����K�v������
 * @param   heapID  �������̈�
 * @param   �G���[���ɌĂяo���֐�
 * @retval  none
 */
//==============================================================================
void GFL_NET_WifiStart( int heapID , NetErrorFunc errorFunc)
{
    if( DWC_INIT_RESULT_DESTROY_OTHER_SETTING == mydwc_init(heapID) ){ //dwc������
        errorFunc(NULL, 0, NULL);
    }
}


//==============================================================================
/**
 * @brief   Wi-Fi�ݒ��ʂ��Ăяo��
 * @param   heapID  �������̈�
 * @retval  none
 */
//==============================================================================
void GFL_NET_WifiUtility( int heapID )
{
	OSIntrMode old;
	void *work;

//	GFL_NET_InitIchneumon(); // 
	// �A���[���V�X�e�������������Ă����Ȃ���DWC_StartUtility�ďo�����Panic����������

  //  while(GFL_NET_IsInitIchneumon()){
    //    ;
//    }

	OS_InitTick();
	OS_InitAlarm();

	// ���荞�ݏ�������U�֎~��
	old = OS_DisableInterrupts();

	// WiFi�ݒ胁�j���[�Ăяo���i�I���܂ŋA���Ă��Ȃ�)
	work = GFL_HEAP_AllocClearMemory( heapID, DWC_UTILITY_WORK_SIZE );
	(void)DWC_StartUtility( work, DWC_LANGUAGE_JAPANESE, DWC_UTILITY_TOP_MENU_FOR_JPN );
	GFL_HEAP_FreeMemory( work );

	// �֎~�������荞�ݏ����𕜋A
	OS_RestoreInterrupts( old );

	//�}�X�^�[���荞�ݏ�����L����
	OS_EnableIrq(); 

}

#endif //GFL_NET_WIFI
