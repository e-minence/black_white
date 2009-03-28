//=============================================================================================
/**
 * @file	machine_use.c
 * @author	GAME FREAK inc.
 * @brief	���[�U�[�̃}�V���ˑ��V�X�e���g�p�̂��߂̃X�P���g���\�[�X
 *
 */
//=============================================================================================
#include <nitro.h>
#include <nnsys.h>

#include "system\machine_use.h"
//=============================================================================================
//=============================================================================================
// �t�@�C���V�X�e���ŗ��p����DMA�ԍ��̒�`
#define FS_DMA_NUMBER	1

//VRAM�]���}�l�[�W����`(NNS�֐�)
#define VRAMTRANS_MAN_TASKNUM    (32)
NNSGfdVramTransferTask    VRAMtransManTaskArray[ VRAMTRANS_MAN_TASKNUM ];
//=============================================================================================
//
//			�֐�
//
//=============================================================================================
//------------------------------------------------------------------
/**
 * @brief	������
 */
//------------------------------------------------------------------
void MachineSystem_Init(void)
{
	// �V�X�e��������
	OS_Init();
	OS_InitTick();
	OS_InitAlarm();
	// RTC������
	RTC_Init();
	// GX������
	GX_Init();
	// �Œ菬���_API������
	FX_Init();
	// �t�@�C���V�X�e��������
	{
		u32 file_table_size;
		void* p_table;

		FS_Init(FS_DMA_NUMBER);

		// �t�@�C���Ǘ��e�[�u����RAM�֍ڂ��遨�t�@�C���ւ̍����A�N�Z�X���\
		file_table_size = FS_GetTableSize();
		p_table = OS_AllocFromMainArenaLo(file_table_size, 4);

		SDK_ASSERT(p_table != NULL);

		OS_EnableIrq();
		
		(void)FS_LoadTable(p_table, file_table_size);

		OS_TPrintf("FileTable Size     = 0x%08x bytes.\n", file_table_size);
		OS_TPrintf("remains of MainRAM = 0x%08x bytes.\n", 
						(u32)(OS_GetMainArenaHi())-(u32)(OS_GetMainArenaLo()));
	}
	// �u�q�`�l�]���֐�������
	NNS_GfdInitVramTransferManager( VRAMtransManTaskArray, VRAMTRANS_MAN_TASKNUM );

#ifdef PM_DEBUG
	//�X�^�b�N���`�F�b�N
	OS_SetThreadStackWarningOffset(OS_GetCurrentThread(), 0x30);  //VCHAT�Ŏg���Ă����̂ň�������0x100->0x30
    OS_CheckStack(OS_GetCurrentThread());
    OS_SetIrqStackWarningOffset(0x100);
#endif
}

//------------------------------------------------------------------
/**
 * @brief	���C���Ăяo��
 */
//------------------------------------------------------------------
void MachineSystem_Main(void)
{
}

//------------------------------------------------------------------
/**
 * @brief	�I������
 */
//------------------------------------------------------------------
void MachineSystem_Exit(void)
{
}

//------------------------------------------------------------------
/**
 * @brief	VBlank���荞�ݏ���
 */
//------------------------------------------------------------------
void MachineSystem_VIntr(void)
{
	NNS_GfdDoVramTransfer();
}

