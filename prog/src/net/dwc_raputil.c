//=============================================================================
/**
 * @file	dwc_raputility.c
 * @brief	WIFI�ݒ��ʌĂяo���p
 * @author	k.ohno
 * @date	09/01/15
 */
//=============================================================================

#include <gflib.h>
#include <dwc.h>
#include "net/network_define.h"
#include "system/main.h"
#include "net/dwc_raputil.h"
#include "net/wih_dwc.h"

//==============================================================================
/**
 * @brief WiFi�ڑ����[�e�B���e�B���Ăяo��
 * @param   none
 * @retval  none
 */
//==============================================================================
static void mydwc_callWifiUtil( HEAPID HeapID )
{
	OSIntrMode old;
	void *work;

	// ���A���[���V�X�e�������������Ă����Ȃ���DWC_StartUtility�ďo�����Panic����������

	// ���荞�ݏ�������U�֎~��
	old = OS_DisableInterrupts();

	// WiFi�ݒ胁�j���[�Ăяo���i�I���܂ŋA���Ă��Ȃ�)
	work = GFL_HEAP_AllocClearMemory( HeapID, DWC_UTILITY_WORK_SIZE );

  if( !OS_IsRunOnTwl() ){//DSI�͌ĂԂ��Ƃ��o���Ȃ�
    DWC_StartUtility( work, DWC_LANGUAGE_JAPANESE, DWC_UTILITY_TOP_MENU_FOR_JPN );
  }
  
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

static GFL_PROC_RESULT WifiUtil2MainProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  NET_DeviceUnload(GFL_NET_TYPE_WIFI);
  GFL_HEAP_DeleteHeap( HEAPID_WIFI );
  WIH_DWC_ReloadCFG();
  return GFL_PROC_RES_FINISH;
}


const GFL_PROC_DATA WifiUtilProcData = {
	WifiUtilMainProcInit,
	WifiUtilMainProcMain,
	WifiUtilMainProcEnd,
};

const GFL_PROC_DATA WifiUtilGSyncProcData = {
	WifiUtilMainProcInit,
	WifiUtilMainProcMain,
	WifiUtil2MainProcEnd,
};

