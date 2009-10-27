//=============================================================================
/**
 * @file	dwc_raputility.c
 * @bfief	WIFI�ݒ��ʌĂяo���p
 * @author	k.ohnop
 * @date	09/01/15
 */
//=============================================================================

#include <gflib.h>
#include <dwc.h>
#include "system/main.h"
#include "net/dwc_raputil.h"

//==============================================================================
/**
 * WiFi�ڑ����[�e�B���e�B���Ăяo��
 * @param   none
 * @retval  none
 */
//==============================================================================
void mydwc_callWifiUtil( HEAPID HeapID )
{
	OSIntrMode old;
	void *work;

	// �A���[���V�X�e�������������Ă����Ȃ���DWC_StartUtility�ďo�����Panic����������
	OS_InitTick();
	OS_InitAlarm();

  DWC_SetAuthServer(DWC_CONNECTINET_AUTH_TEST);

	// ���荞�ݏ�������U�֎~��
	old = OS_DisableInterrupts();

	// WiFi�ݒ胁�j���[�Ăяo���i�I���܂ŋA���Ă��Ȃ�)
	work = GFL_HEAP_AllocClearMemory( HeapID, DWC_UTILITY_WORK_SIZE );
	GF_ASSERT(DWC_UTIL_RESULT_SUCCESS==DWC_StartUtility( work, DWC_LANGUAGE_JAPANESE, DWC_UTILITY_TOP_MENU_FOR_JPN ));
	GFL_HEAP_FreeMemory( work );

	// �֎~�������荞�ݏ����𕜋A
	OS_RestoreInterrupts( old );

	//�}�X�^�[���荞�ݏ�����L����
	OS_EnableIrq( ); 


}



static GFL_PROC_RESULT WifiUtilMainProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFI, DWC_UTILITY_WORK_SIZE+0x100 );
    NET_DeviceLoad(GFL_NET_TYPE_WIFI);
	return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT WifiUtilMainProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
    mydwc_callWifiUtil(HEAPID_WIFI);
    return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT WifiUtilMainProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
    OS_ResetSystem(0);
    return GFL_PROC_RES_FINISH;
}


const GFL_PROC_DATA WifiUtilProcData = {
	WifiUtilMainProcInit,
	WifiUtilMainProcMain,
	WifiUtilMainProcEnd,
};

