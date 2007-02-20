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
#include <dwc.h>
#include "gf_macro.h"
#include "gf_standard.h"
#include "assert.h"
#include "heapsys.h"
#include "net.h"
//#include "gflib.h"
//#include "dwc_rap.h"
#include "net_wireless.h"
#include "dwc_rapinitialize.h"

//==============================================================================
/**
 * @brief   Wi-Fi�ڑ������� ���������ɌĂяo���Ă����K�v������
            network�����ŌĂяo���̂�GFI����
 * @param   heapID  �������̈�
 * @retval  none
 */
//==============================================================================
void GFL_NET_startWiFi( int heapID )
{
	OSIntrMode old;
	void *work;

    
	GFL_NET_WLVRAMDInitialize(); // 
	// �A���[���V�X�e�������������Ă����Ȃ���DWC_StartUtility�ďo�����Panic����������

    while(GFL_NET_WLIsVRAMDInitialize()){
        ;
    }

	OS_InitTick();
	OS_InitAlarm();

	// ���荞�ݏ�������U�֎~��
	old = OS_DisableInterrupts();

	// WiFi�ݒ胁�j���[�Ăяo���i�I���܂ŋA���Ă��Ȃ�)
	work = GFL_HEAP_AllocMemory( heapID, DWC_UTILITY_WORK_SIZE );
	(void)DWC_StartUtility( work, DWC_LANGUAGE_JAPANESE, DWC_UTILITY_TOP_MENU_FOR_JPN );
	GFL_HEAP_FreeMemory( work );

	// �֎~�������荞�ݏ����𕜋A
	OS_RestoreInterrupts( old );

	//�}�X�^�[���荞�ݏ�����L����
	OS_EnableIrq(); 

}

