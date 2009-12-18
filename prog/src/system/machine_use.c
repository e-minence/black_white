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


//TWL��NITRO���[�h�Ƃ�HEAP�̍����̈׊m�ۂ��Ă���܂�
//
#if !defined(SDK_TWL)
static u8 dummybufferTWL[0x8d00];
#endif

//VRAM�]���}�l�[�W����`(NNS�֐�)
#define VRAMTRANS_MAN_TASKNUM    (32)
NNSGfdVramTransferTask    VRAMtransManTaskArray[ VRAMTRANS_MAN_TASKNUM ];
//=============================================================================================
//
//			�֐�
//
//=============================================================================================

static void MachineSystem_MbInitFile(void);

//------------------------------------------------------------------
/**
 * @brief	������
 */
//------------------------------------------------------------------
void MachineSystem_Init(void)
{
#ifdef PM_DEBUG
#ifndef MULTI_BOOT_MAKE
  OS_EnableMainExArena();
#endif //MULTI_BOOT_MAKE
#endif PM_DEBUG
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
#ifndef MULTI_BOOT_MAKE
	{
		u32 file_table_size;
		void* p_table;

		OS_EnableIrq();
		FS_Init(FS_DMA_NUMBER);

		// �t�@�C���Ǘ��e�[�u����RAM�֍ڂ��遨�t�@�C���ւ̍����A�N�Z�X���\
		file_table_size = FS_GetTableSize();
		p_table = OS_AllocFromMainArenaLo(file_table_size, 4);

		SDK_ASSERT(p_table != NULL);

		
		(void)FS_LoadTable(p_table, file_table_size);

		OS_TPrintf("FileTable Size     = 0x%08x bytes.\n", file_table_size);
		OS_TPrintf("remains of MainRAM = 0x%08x bytes.\n", 
						(u32)(OS_GetMainArenaHi())-(u32)(OS_GetMainArenaLo()));
	}
#else
  MachineSystem_MbInitFile();
#endif
	// �u�q�`�l�]���֐�������
	NNS_GfdInitVramTransferManager( VRAMtransManTaskArray, VRAMTRANS_MAN_TASKNUM );

#ifdef PM_DEBUG
	//�X�^�b�N���`�F�b�N
	OS_SetThreadStackWarningOffset(OS_GetCurrentThread(), 0x30);  //VCHAT�Ŏg���Ă����̂ň�������0x100->0x30
    OS_CheckStack(OS_GetCurrentThread());
    OS_SetIrqStackWarningOffset(0x100);
#endif

#if !defined(SDK_TWL)
  dummybufferTWL[0]=0;
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

//------------------------------------------------------------------
/**
 * @brief	�J�[�g���b�W���C�u���������� ���������팸�̂���
 */
//------------------------------------------------------------------
void CTRDG_Init(void)
{
  CTRDG_DummyInit();
}


static void MachineSystem_MbInitFile(void)
{
  u32 file_table_size;
  void* p_table;
  MBParam *multi_p = (MBParam *)MB_GetMultiBootParam();

  // ROM�A�N�Z�X����������B
  CARD_Enable(TRUE);

  multi_p->boot_type = MB_TYPE_NORMAL;	/* FS_Init()��ROM��enable�ɂ����邽�߁AMULTIBOOT�t���O����uOFF�ɂ��� */
  OS_EnableIrq();
  FS_Init(FS_DMA_NUMBER);
  multi_p->boot_type = MB_TYPE_MULTIBOOT;	/* MULTIBOOT�t���O���ăZ�b�g���� */

#if 0
  if (!FS_IsAvailable())
  {
      OS_TPanic("no archive to replace!");
  }
  else
  {
    static const char name[] = "rom";
    static const int name_len = sizeof(name) - 1;
    const CARDRomHeader* header;
    FSArchive *rom;
    {
      CARDRomHeader * const arg_buffer = (CARDRomHeader *)0x027FF000/*HW_MAIN_MEM_SHARED*/;
      CARDRomHeader * const app_header = (CARDRomHeader *)HW_ROM_HEADER_BUF;
      CARDRomHeader * const org_header = (CARDRomHeader *)HW_CARD_ROM_HEADER;
      if (arg_buffer->game_code == 0)
      {
        // ROM�w�b�_�̓��e��ޔ�̈�ɃR�s�[���܂��B
        CARD_Init();
        MI_CpuCopy8(app_header, arg_buffer, HW_CARD_ROM_HEADER_SIZE);
        MI_CpuCopy8(app_header, org_header, HW_CARD_ROM_HEADER_SIZE);
        /*
         * ���̎��_��arg_buffer->game_code ��NITRO�J�[�h�̃C�j�V�����R�[�h���̂��̂������Ă���
         */
      }
      header = arg_buffer;
    }
    rom = FS_FindArchive(name, name_len);
    rom->fat = header->fat.offset;
    rom->fat_size = header->fat.length;
    rom->fnt = header->fnt.offset;
    rom->fnt_size = header->fnt.length;
  }
#endif
  {
    //����
    //OSBootInfo *bootInfo = ((OSBootInfo *)HW_WM_BOOT_BUF);
    //bootInfo->boot_type = OS_BOOTTYPE_ROM;
  }
/*  
  // �t�@�C���Ǘ��e�[�u����RAM�֍ڂ��遨�t�@�C���ւ̍����A�N�Z�X���\
  file_table_size = FS_GetTableSize();
  p_table = OS_AllocFromMainArenaLo(file_table_size, 4);

  SDK_ASSERT(p_table != NULL);


  (void)FS_LoadTable(p_table, file_table_size);

  OS_TPrintf("FileTable Size     = 0x%08x bytes.\n", file_table_size);
  OS_TPrintf("remains of MainRAM = 0x%08x bytes.\n", 
              (u32)(OS_GetMainArenaHi())-(u32)(OS_GetMainArenaLo()));
*/
}