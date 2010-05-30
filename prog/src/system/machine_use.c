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

//VRAM�]���}�l�[�W����`(NNS�֐�)
#define VRAMTRANS_MAN_TASKNUM    (48)
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
}

//------------------------------------------------------------------
/**
 * @brief	���C���Ăяo��
 */
//------------------------------------------------------------------
void MachineSystem_Main(void)
{
#if (defined(SDK_TWL))
  if(bHardResetFlg){  //�n�[�h���Z�b�g�R�[���o�b�N���Ă΂ꂽ
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




// ���� ROM �A�[�J�C�u�\���́B
typedef struct MyRomArchive
{
    FSArchive   arc[1];
    u32         default_dma_no;
    u32         card_lock_id;
}
MyRomArchive;

// �񓯊���ROM�ǂݍ��݂����������Ƃ��̏����B
static void MyRom_OnReadDone(void *arc)
{
    // �A�[�J�C�u�֊����ʒm�B
    FS_NotifyArchiveAsyncEnd((FSArchive *)arc, FS_RESULT_SUCCESS);
}

// FS����A�[�J�C�u�ւ̃��[�h�A�N�Z�X�R�[���o�b�N�B
static FSResult MyRom_ReadCallback(FSArchive *arc, void *dst, u32 src, u32 len)
{
    MyRomArchive *const p_rom = (MyRomArchive *)arc;
    CARD_ReadRomAsync(p_rom->default_dma_no,
                      (const void *)(FS_GetArchiveBase(arc) + src), dst, len,
                      MyRom_OnReadDone, arc);
    return FS_RESULT_PROC_ASYNC;
}

// FS����A�[�J�C�u�ւ̃��C�g�R�[���o�b�N�B
// ���[�U�v���V�[�W����FS_RESULT_UNSUPPORTED��Ԃ��̂ŌĂ΂�Ȃ��B
static FSResult MyRom_WriteDummyCallback(FSArchive *arc, const void *src, u32 dst, u32 len)
{
    (void)arc;
    (void)src;
    (void)dst;
    (void)len;
    return FS_RESULT_FAILURE;
}

// ���[�U�v���V�[�W���B
// �ŏ��̃R�}���h�J�n�O����Ō�̃R�}���h������܂�ROM�����b�N�B
// ���C�g����̓T�|�[�g�O�Ƃ��ĉ�������B
// ����ȊO�̓f�t�H���g�̓���B
static FSResult MyRom_ArchiveProc(FSFile *file, FSCommandType cmd)
{
    MyRomArchive *const p_rom = (MyRomArchive *) FS_GetAttachedArchive(file);
    switch (cmd)
    {
    case FS_COMMAND_ACTIVATE:
        CARD_LockRom((u16)p_rom->card_lock_id);
        return FS_RESULT_SUCCESS;
    case FS_COMMAND_IDLE:
        CARD_UnlockRom((u16)p_rom->card_lock_id);
        return FS_RESULT_SUCCESS;
    case FS_COMMAND_WRITEFILE:
        return FS_RESULT_UNSUPPORTED;
    default:
        return FS_RESULT_PROC_UNKNOWN;
    }
}

static void MachineSystem_MbInitFile(void)
{
  //�����ł͏������Ȃ�
#if 0
  CARDRomHeader *headerData = (CARDRomHeader*)CARD_GetRomHeader();
#if PM_DEBUG
  if( STD_CompareString( headerData->game_name , "POKEMON D" ) == 0 ||
      STD_CompareString( headerData->game_name , "POKEMON P" ) == 0 ||
      STD_CompareString( headerData->game_name , "POKEMON PL" ) == 0 ||
      STD_CompareString( headerData->game_name , "POKEMON HG" ) == 0 ||
      STD_CompareString( headerData->game_name , "POKEMON SS" ) == 0 ||
      STD_CompareString( headerData->game_name , "NINTENDO    NTRJ01" ) == 0 ||
      STD_CompareString( headerData->game_name , "SKEL" ) == 0 ||
      STD_CompareString( headerData->game_name , "dlplay" ) == 0 ||
      STD_CompareString( headerData->game_name , "SYACHI_MB" ) == 0 )
#else
  if( STD_CompareString( headerData->game_name , "POKEMON D" ) == 0 ||
      STD_CompareString( headerData->game_name , "POKEMON P" ) == 0 ||
      STD_CompareString( headerData->game_name , "POKEMON PL" ) == 0 ||
      STD_CompareString( headerData->game_name , "POKEMON HG" ) == 0 ||
      STD_CompareString( headerData->game_name , "POKEMON SS" ) == 0 )
#endif
  {
    //�}���`�u�[�g�Ŏq�@ROM����t�@�C(�A�C�R��)��ǂނ��߂̏���
    
    u32 file_table_size;
    void* p_table;
    MBParam *multi_p = (MBParam *)MB_GetMultiBootParam();

    // ROM�A�N�Z�X����������B
    CARD_Enable(TRUE);

    multi_p->boot_type = MB_TYPE_NORMAL;	/* FS_Init()��ROM��enable�ɂ����邽�߁AMULTIBOOT�t���O����uOFF�ɂ��� */
    OS_EnableIrq();
    FS_Init(FS_DMA_NUMBER);
    multi_p->boot_type = MB_TYPE_MULTIBOOT;	/* MULTIBOOT�t���O���ăZ�b�g���� */

    {
      const u32 base = 0;
      const CARDRomRegion *fnt = &((CARDRomHeader*)CARD_GetRomHeader())->fnt;
      const CARDRomRegion *fat = &((CARDRomHeader*)CARD_GetRomHeader())->fat;
      const char *name = "child_rom";

      static MyRomArchive newRom;

      FS_InitArchive(newRom.arc);
      newRom.default_dma_no = FS_DMA_NUMBER;
      newRom.card_lock_id = (u32)OS_GetLockID();
      if (!FS_RegisterArchiveName(newRom.arc, name, (u32)STD_GetStringLength(name)))
      {
        OS_TPanic("error! FS_RegisterArchiveName(%s) failed.\n", name);
      }
      else
      {
        FS_SetArchiveProc(newRom.arc, MyRom_ArchiveProc,
                          FS_ARCHIVE_PROC_WRITEFILE | FS_ARCHIVE_PROC_ACTIVATE | FS_ARCHIVE_PROC_IDLE);
        if (!FS_LoadArchive(newRom.arc, base,
                          fat->offset, fat->length, fnt->offset, fnt->length,
                          MyRom_ReadCallback, MyRom_WriteDummyCallback))
        {
            OS_TPanic("error! FS_LoadArchive() failed.\n");
        }
      }
    }
  }
#endif
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

