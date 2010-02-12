//===================================================================
/**
 * @file	main.c
 * @brief	�A�v���P�[�V�����@�T���v���X�P���g��
 * @author	GAME FREAK Inc.
 * @date	06.11.08
 *
 * $LastChangedDate$
 * $Id$
 */
//===================================================================
#include <nitro.h>
#include <nnsys.h>
#include "gflib.h"
#include "system\machine_use.h"
#include "system\gfl_use.h"
#include "system\main.h"
#include "sound\snd_strm.h"
#include "savedata/save_control.h"
#include "net/wih_dwc.h"
#include "print/printsys.h"
#include "print/global_font.h"
#include "print/global_msg.h"
#include "poke_tool/poke_tool.h"
#include "system/net_err.h"
#include "savedata/config.h"
#include "system/wipe.h"
#include "system/brightness.h"
#include "gamesystem\msgspeed.h"
#include "gamesystem\game_beacon.h"
#include "gamesystem/g_power.h"

#include "sound\pm_sndsys.h"
#include "sound\pm_voice.h"
#include "sound\pm_wb_voice.h"
#ifdef PM_DEBUG
#include "debug/debug_sd_print.h"
#include "test/performance.h"
#include "test/debug_pause.h"
#include "debug/debugwin_sys.h"
#endif //PM_DEBUG

#include "title/title.h"

// �T�E���h�ǂݍ��݃X���b�h
extern OSThread soundLoadThread;

static	void	SkeltonHBlankFunc(void);
static	void	SkeltonVBlankFunc(void);
static	void	GameInit(void);
static	void	GameMain(void);
static	void	GameExit(void);
static  void  GameVBlankFunc(void);

#ifdef PM_DEBUG
static void DEBUG_StackOverCheck(void);
#endif
//------------------------------------------------------------------
//	�O���֐��錾
//------------------------------------------------------------------
//extern const	GFL_PROC_DATA CorpProcData;
//FS_EXTERN_OVERLAY(title);
FS_EXTERN_OVERLAY(notwifi);

//------------------------------------------------------------------
/**
 * @brief	���C������
 */
//------------------------------------------------------------------
void NitroMain(void)
{
	// �n�[�h���V�X�e���S������������֐�
	MachineSystem_Init();
	// �f�e�k������
	GFLUser_Init();

	//HBLANK���荞�݋���
	OS_SetIrqFunction(OS_IE_H_BLANK,SkeltonHBlankFunc);
	//VBLANK���荞�݋���
	OS_SetIrqFunction(OS_IE_V_BLANK,SkeltonVBlankFunc);

	(void)OS_EnableIrqMask(OS_IE_H_BLANK|OS_IE_V_BLANK);
	(void)OS_EnableIrq();

	(void)GX_HBlankIntr(TRUE);
	(void)GX_VBlankIntr(TRUE);
	(void)OS_EnableInterrupts();

	// �K�v��TCB�Ƃ��o�^���āc
	GameInit();

#ifdef PM_DEBUG
  DEB_SD_PRINT_InitSystem( GFL_HEAPID_APP );
	DEBUG_PerformanceInit();
	DEBUG_PAUSE_Init();
	{
		//�f�o�b�O���̂Ă̏������{�A�h���X��n��
		u8 *charArea;
		u16 *scrnArea,*plttArea;
		NetErr_GetTempArea( &charArea , &scrnArea , &plttArea );
		DEBUGWIN_InitSystem(charArea , scrnArea , plttArea);
	}
#endif

	while(TRUE){
#ifdef PM_DEBUG
		//	DEBUG_PerformanceStart();
		DEBUG_PerformanceMain();
		DEBUG_PerformanceStartLine(PERFORMANCE_ID_MAIN);
#endif //PM_DEBUG

#ifdef PM_DEBUG
		DEBUG_StackOverCheck();
#endif	//PM_DEBUG

		MachineSystem_Main();
		// ���C���������āc
		GFLUser_Main();
		GameMain();

		// �`��ɕK�v�ȏ������āc
		GFLUser_Display();

		// �����_�����O�G���W�����Q�Ƃ���f�[�^�Q���X���b�v
		// ��gflib�ɓK�؂Ȋ֐����o������u�������Ă�������
		//G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);

#ifdef PM_DEBUG
		//DEBUG_PerformanceDisp();
		DEBUG_PerformanceEndLine(PERFORMANCE_ID_MAIN);
		DEB_SD_PRINT_UpdateSystem();
#endif //PM_DEBUG

		// VBLANK�҂�
		GFL_G3D_SwapBuffers();
		if(GFL_NET_SystemGetConnectNum() > 1){
			OS_WaitIrq(TRUE, OS_IE_V_BLANK);
			GameVBlankFunc();
		}
		else{
			MI_SetMainMemoryPriority(MI_PROCESSOR_ARM9);
      if( OS_IsThreadTerminated( &soundLoadThread ) == FALSE )
      { // �T�E���h�ǂݍ��݃X���b�h�x�~
        OS_SleepThreadDirect( &soundLoadThread, NULL );
      }
			OS_WaitIrq(TRUE, OS_IE_V_BLANK);
			GameVBlankFunc();
      if( OS_IsThreadTerminated( &soundLoadThread ) == FALSE )
      { // �T�E���h�ǂݍ��݃X���b�h�N��
        OS_WakeupThreadDirect( &soundLoadThread );
      }
			MI_SetMainMemoryPriority(MI_PROCESSOR_ARM7);
		}
	}

	GameExit();
}

//------------------------------------------------------------------
/**
 * brief	HBlank���荞�ݏ���
 */
//------------------------------------------------------------------
static	void	SkeltonHBlankFunc(void)
{
	GFLUser_HIntr();
}

//------------------------------------------------------------------
/**
 * brief	VBlank���荞�ݏ���
 */
//------------------------------------------------------------------
static	void	SkeltonVBlankFunc(void)
{
	GFLUser_VIntr();

	OS_SetIrqCheckFlag(OS_IE_V_BLANK);
	MI_WaitDma(GX_DEFAULT_DMAID);
}

//--------------------------------------------------------------
/**
 * VBlank���Ԓ��ɍs������
 */
//--------------------------------------------------------------
static void GameVBlankFunc(void)
{
#ifdef PM_DEBUG
	DEBUG_PerformanceStartLine(PERFORMANCE_ID_VBLANK);
#endif

	MachineSystem_VIntr();
	GFLUser_VTiming();

#ifdef PM_DEBUG
	DEBUG_PerformanceEndLine(PERFORMANCE_ID_VBLANK);
#endif
}

//------------------------------------------------------------------
/**
 * @brief		�Q�[�����Ƃ̏���������
 */
//------------------------------------------------------------------
static	void	GameInit(void)
{
	/* ���[�U�[���x���ŕK�v�ȏ������������ɋL�q���� */
  //WIFI�ŕK�v�Ȃ��v���O�����I�[�o�[���C�̍ŏ��̃��[�h
  GFL_OVERLAY_Load( FS_OVERLAY_ID( notwifi ) );

	//�Z�[�u�֘A������
	SaveControl_SystemInit(HEAPID_SAVE);

	// �ʐM�̃f�o�b�O�v�����g���s����`
#ifdef PM_DEBUG
#if defined(DEBUG_ONLY_FOR_ohno)||defined(DEBUG_ONLY_FOR_toru_nagihashi)||defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko)
	GFL_NET_DebugPrintOn();
#endif
#endif  //PM_DEBUG
	// �ʐM�u�[�g���� VBlank���荞�݌�ɍs�����߂����ɋL�q�A�������͕\���p�֐��|�C���^
  GFL_NET_Boot( GFL_HEAPID_APP, NULL, GFL_HEAPID_APP, HEAPID_NETWORK_FIX);
  // AP���̎擾
  WIH_DWC_CreateCFG(HEAPID_NETWORK_FIX);
  WIH_DWC_ReloadCFG();

  /* �N���v���Z�X�̐ݒ� */
#if 0
	TestModeSet();	//���T���v���f�o�b�O���[�h
#else
	GFL_PROC_SysCallProc(NO_OVERLAY_ID, &TitleControlProcData, NULL);
//	GFL_PROC_SysCallProc(FS_OVERLAY_ID(title), &CorpProcData, NULL);
#endif

	/* �����`��V�X�e�������� */
	PRINTSYS_Init( GFL_HEAPID_SYSTEM );
	MSGSPEED_InitSystem( SaveControl_GetPointer() );

	//�ʐM�G���[��ʊǗ��V�X�e��������
	NetErr_SystemInit();
	NetErr_SystemCreate(HEAPID_NET_ERR);//��check�@�Ƃ肠�����Q�[�����A�����ƃV�X�e�������݂���悤�ɂ��Ă���

	/* �O���[�o�����b�Z�[�W�f�[�^���t�H���g������ */
	GLOBALMSG_Init( GFL_HEAPID_SYSTEM );

	/* poketool�V�X�e�������� */
	POKETOOL_InitSystem( GFL_HEAPID_SYSTEM );

	//�T�E���h�̐ݒ�
	PMSND_Init();
	//�����V�X�e���̐ݒ�
	PMVOICE_Init(GFL_HEAPID_APP, PMV_GetVoiceWaveIdx, PMV_CustomVoiceWave);

	//�L�[�R���g���[�����[�h�ݒ�
	CONFIG_SYSTEM_KyeControlTblSetting();
	
	//�Q�[�����r�[�R���X�L��������
  GAMEBEACON_Init(GFL_HEAPID_APP);
  
  //G�p���[
  GPower_SYSTEM_Init();
}

//------------------------------------------------------------------
/**
 * @brief		�Q�[�����Ƃ̃��C������
 */
//------------------------------------------------------------------
static	void	GameMain(void)
{
	/* ���[�U�[���x���ŕK�v�ȃ��C�������������ɋL�q���� */

	BrightnessChgMain();
	WIPE_SYS_Main();

	NetErr_Main();
	if((GFL_UI_KEY_GetCont() & PAD_BUTTON_L) && (GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG)){
		NetErr_DEBUG_ErrorSet();
	}
	PMSND_Main();
	PMVOICE_Main();
	
	GAMEBEACON_Update();
	GPOWER_SYSTEM_Update();
}

//------------------------------------------------------------------
/**
 * @brief		�Q�[�����Ƃ̏I������
 */
//------------------------------------------------------------------
static	void	GameExit(void)
{
#ifdef PM_DEBUG
	//�f�o�b�O�V�X�e��
	DEBUGWIN_ExitSystem();
#endif PM_DEBUG

  GAMEBEACON_Exit();
  WIH_DWC_DeleteCFG();
	PMVOICE_Exit();
	PMSND_Exit();
}

//--------------------------------------------------------------
/**
 * @brief   �X�^�b�N���`�F�b�N
 */
//--------------------------------------------------------------
#ifdef PM_DEBUG
static void DEBUG_StackOverCheck(void)
{
	OSStackStatus stack_status;

	stack_status = OS_GetStackStatus(OS_GetCurrentThread());
	switch(stack_status){
	case OS_STACK_OVERFLOW:
		GF_ASSERT("�X�^�b�N�ŉ��ʂ̃}�W�b�N�i���o�[�������������Ă��܂�\n");
		break;
	case OS_STACK_UNDERFLOW:
		GF_ASSERT("�X�^�b�N�ŏ�ʂ̃}�W�b�N�i���o�[�������������Ă��܂�\n");
		break;
	case OS_STACK_ABOUT_TO_OVERFLOW:
		OS_TPrintf("�X�^�b�N���̌x�������ɒB���Ă��܂�\n");
		break;
	}

	stack_status = OS_GetIrqStackStatus();
	switch(stack_status){
	case OS_STACK_OVERFLOW:
		GF_ASSERT("IRQ�X�^�b�N�ŉ��ʂ̃}�W�b�N�i���o�[�������������Ă��܂�\n");
		break;
	case OS_STACK_UNDERFLOW:
		GF_ASSERT("IRQ�X�^�b�N�ŏ�ʂ̃}�W�b�N�i���o�[�������������Ă��܂�\n");
		break;
	case OS_STACK_ABOUT_TO_OVERFLOW:
		OS_TPrintf("IRQ�X�^�b�N���̌x�������ɒB���Ă��܂�\n");
		break;
	}
}
#endif	//PM_DEBUG
