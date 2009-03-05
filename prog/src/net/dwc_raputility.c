//=============================================================================
/**
 * @file	dwc_raputility.c
 * @bfief	WIFI�ݒ��ʌĂяo���p
 * @author	k.ohnop
 * @date	09/01/15
 */
//=============================================================================

#include <gflib.h>

//==============================================================================
/**
 * WiFi�ڑ����[�e�B���e�B���Ăяo��
 * @param   none
 * @retval  none
 */
//==============================================================================
void mydwc_callUtility( HEAPID HeapID )
{
	OSIntrMode old;
	void *work;

	// �A���[���V�X�e�������������Ă����Ȃ���DWC_StartUtility�ďo�����Panic����������
	OS_InitTick();
	OS_InitAlarm();


	// ���荞�ݏ�������U�֎~��
	old = OS_DisableInterrupts();

	// WiFi�ݒ胁�j���[�Ăяo���i�I���܂ŋA���Ă��Ȃ�)
	work = GFL_HEAP_AllocClearMemory( HeapID, DWC_UTILITY_WORK_SIZE );
	(void)DWC_StartUtility( work, DWC_LANGUAGE_JAPANESE, DWC_UTILITY_TOP_MENU_FOR_JPN );
	GFL_HEAP_FreeMemory( work );

	// �֎~�������荞�ݏ����𕜋A
	OS_RestoreInterrupts( old );

	//�}�X�^�[���荞�ݏ�����L����
	OS_EnableIrq( ); 


}