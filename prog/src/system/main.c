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
#include "print/printsys.h"
#include "print/global_font.h"
#include "print/global_msg.h"
#include "poke_tool/poke_tool.h"
#include "system/net_err.h"
#include "savedata/config.h"
#include "system/wipe.h"

#include "sound\pm_sndsys.h"
#ifdef PM_DEBUG
#include "test/performance.h"
#endif //PM_DEBUG

#include "title/title.h"

static	void	SkeltonHBlankFunc(void);
static	void	SkeltonVBlankFunc(void);
static	void	GameInit(void);
static	void	GameMain(void);
static	void	GameExit(void);

#ifdef PM_DEBUG
static void DEBUG_StackOverCheck(void);
#endif

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
	DEBUG_PerformanceInit();
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
#endif //PM_DEBUG

        // VBLANK�҂�
		// ��gflib�ɓK�؂Ȋ֐����o������u�������Ă�������
		OS_WaitIrq(TRUE,OS_IE_V_BLANK);
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
	CPContext context;
	CP_SaveContext( &context );	// ���Z��̏�Ԃ�ۑ�

	OS_SetIrqCheckFlag(OS_IE_V_BLANK);

	MI_WaitDma(GX_DEFAULT_DMAID);

#ifdef PM_DEBUG
	DEBUG_PerformanceStartLine(PERFORMANCE_ID_VBLANK);
#endif

	MachineSystem_VIntr();
	GFLUser_VIntr();

#ifdef PM_DEBUG
	DEBUG_PerformanceEndLine(PERFORMANCE_ID_VBLANK);
#endif
	CP_RestoreContext( &context );	// ���Z��̏�Ԃ𕜌�
}

//------------------------------------------------------------------
/**
 * @brief		�Q�[�����Ƃ̏���������
 */
//------------------------------------------------------------------
static	void	GameInit(void)
{
	/* ���[�U�[���x���ŕK�v�ȏ������������ɋL�q���� */

	//�Z�[�u�֘A������
	SaveControl_SystemInit(GFL_HEAPID_APP);	//��check �g�p���Ă���q�[�vID�͎b��
	
    // �ʐM�u�[�g���� VBlank���荞�݌�ɍs�����߂����ɋL�q�A�������͕\���p�֐��|�C���^
    GFL_NET_Boot( GFL_HEAPID_APP, NULL, GFL_HEAPID_APP, HEAPID_NETWORK_FIX);
	/* �N���v���Z�X�̐ݒ� */
#if 0
	TestModeSet();	//���T���v���f�o�b�O���[�h
#else
	GFL_PROC_SysCallProc(NO_OVERLAY_ID, &TitleControlProcData, NULL);
#endif

	/* �����`��V�X�e�������� */
	PRINTSYS_Init( GFL_HEAPID_SYSTEM );
	
	//�ʐM�G���[��ʊǗ��V�X�e��������
	NetErr_SystemInit();
	NetErr_SystemCreate(GFL_HEAPID_APP);//��check�@�Ƃ肠�����Q�[�����A�����ƃV�X�e�������݂���悤�ɂ��Ă���

	/* �O���[�o�����b�Z�[�W�f�[�^���t�H���g������ */
	GLOBALMSG_Init( GFL_HEAPID_SYSTEM );

	/* poketool�V�X�e�������� */
	POKETOOL_InitSystem( GFL_HEAPID_SYSTEM );

    //�T�E���h�̐ݒ�
	PMSNDSYS_Init();
	//�T�E���h�X�g���[�~���O�Đ��V�X�e��
	SND_STRM_Init(GFL_HEAPID_SYSTEM);

	//�L�[�R���g���[�����[�h�ݒ�
	CONFIG_SYSTEM_KyeControlTblSetting();

}

//------------------------------------------------------------------
/**
 * @brief		�Q�[�����Ƃ̃��C������
 */
//------------------------------------------------------------------
static	void	GameMain(void)
{
	/* ���[�U�[���x���ŕK�v�ȃ��C�������������ɋL�q���� */

	WIPE_SYS_Main();

	NetErr_Main();
	NetErr_App_ErrorCheck();
	if((GFL_UI_KEY_GetCont() & PAD_BUTTON_L) && (GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG)){
		NetErr_ErrorSet();
	}
	SND_STRM_Main();
	PMSNDSYS_Main();
}

//------------------------------------------------------------------
/**
 * @brief		�Q�[�����Ƃ̏I������
 */
//------------------------------------------------------------------
static	void	GameExit(void)
{
	SND_STRM_Exit();
	PMSNDSYS_Exit();
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
