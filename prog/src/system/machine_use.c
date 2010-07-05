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
#include <gflib.h> // gflib/include/dma.h���Q�Ƃ��邽��

#include "system\machine_use.h"
#include "system\ds_system.h"
//=============================================================================================
//=============================================================================================
// �t�@�C���V�X�e���ŗ��p����DMA�ԍ��̒�`
#define FS_DMA_NUMBER	(GFL_DMA_FS_NO)

//ARM9�D��̏ꍇ�ɕK�v�ȃ��Z�b�g�ׂ̈̏���
#if (defined(SDK_TWL))
static PMExitCallbackInfo myInfo;
static void myExitCallback(void *arg);
static BOOL bHardResetFlg=FALSE;
#endif

//�J�[�h���������o�����Ƃ��ɌĂ΂��R�[���o�b�N
#ifdef PM_DEBUG
static  BOOL  card_pull_callback( void );
#endif

//VRAM�]���}�l�[�W����`(NNS�֐�)
#define VRAMTRANS_MAN_TASKNUM    (48)
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
	
#if (defined(SDK_TWL))
  if( OS_GetConsoleType() & (OS_CONSOLE_TWL|OS_CONSOLE_TWLDEBUGGER) )
  {
    //TWL�g���@�\��OFF
  	SCFG_SetCpuSpeed( SCFG_CPU_SPEED_2X );
  	SCFG_SetVramExpanded( FALSE );
  }
#endif

	// �t�@�C���V�X�e��������
#ifndef MULTI_BOOT_MAKE
  //�}���`�u�[�g�͕ʂ̂Ƃ���ŏ���������
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
#endif
	// �u�q�`�l�]���֐�������
	NNS_GfdInitVramTransferManager( VRAMtransManTaskArray, VRAMTRANS_MAN_TASKNUM );

#ifdef PM_DEBUG
	//�X�^�b�N���`�F�b�N
	OS_SetThreadStackWarningOffset(OS_GetCurrentThread(), 0x30);  //VCHAT�Ŏg���Ă����̂ň�������0x100->0x30
    OS_CheckStack(OS_GetCurrentThread());
    OS_SetIrqStackWarningOffset(0x100);
#endif

#if (defined(SDK_TWL))
#ifndef MULTI_BOOT_MAKE
  if(DS_SYSTEM_IsRunOnTwl()){
#else
  if(OS_IsRunOnTwl()){
#endif
    //ARM9�D��̏ꍇ�ɕK�v�ȃn�[�h���Z�b�g�ׂ̈̏���
    bHardResetFlg=FALSE;
    PM_SetExitCallbackInfo( &myInfo, myExitCallback, (void*)0 );
    PM_AppendPreExitCallback( &myInfo );
  }
#endif//

#ifndef MULTI_BOOT_MAKE
  //�\�t�g���Z�b�g���}�C�N��OFF�ɂȂ�Ȃ������̂��C��
  {
    PMAmpSwitch swBuf;
    PM_GetAmp( &swBuf );
    if( swBuf == PM_AMP_ON )
    {
      PM_SetAmp( PM_AMP_OFF );
    }
  }
#endif//

#ifdef PM_DEBUG
  //�J�[�h���������o�����Ƃ��ɌĂяo�����R�[���o�b�N��ݒ�
  CARD_SetPulledOutCallback( &card_pull_callback );
#endif
}

//------------------------------------------------------------------
/**
 * @brief	���C���Ăяo��
 */
//------------------------------------------------------------------
void MachineSystem_Main(void)
{
#if (defined(SDK_TWL))
  //�n�[�h���Z�b�g�R�[���o�b�N���Ă΂ꂽ
  if(bHardResetFlg && GFL_UI_CheckSoftResetDisable(GFL_UI_SOFTRESET_SVLD) == FALSE){
    if(!PM_GetAutoExit()){  //�������������Ȃ���΂����Ȃ������m�F����
      MI_SetMainMemoryPriority(MI_PROCESSOR_ARM7);  //ARM7�D��ɐ؂�ւ���
      PM_ReadyToExit();  //�n�[�h���Z�b�g�㏈���J�n�i���ǂ��Ă��Ȃ��j
    }
  }
#endif
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

//------------------------------------------------------------------
/**
 * @brief	�n�[�h���Z�b�g���ɌĂ΂��֐�
 */
//------------------------------------------------------------------
#if (defined(SDK_TWL))
static void myExitCallback(void *arg)
{
#pragma unused(arg)
  PM_SetAutoExit(FALSE); //�n�[�h���Z�b�g��C���邱�ƂȂ� ���̌㏈������
  bHardResetFlg=TRUE;   //�n�[�h���Z�b�g�t���OON
}
#endif

#ifdef PM_DEBUG
//------------------------------------------------------------------
/**
 * @brief	�J�[�h���������o�������ɌĂ΂��֐�
 */
//------------------------------------------------------------------
static  BOOL  card_pull_callback( void )
{ 
  GF_ASSERT_MSG( 0, "card pull!\n" );

  return TRUE;
}
#endif

