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
#include "savedata/save_control.h"
#include "print/printsys.h"
#include "print/global_font.h"
#include "print/global_msg.h"
#include "poke_tool/poke_tool.h"
#include "system/net_err.h"

#ifdef PM_DEBUG
#include "test/performance.h"
#include "test/goto/comm_error.h"
#endif //PM_DEBUG

#include "title/title.h"

static	void	SkeltonHBlankFunc(void);
static	void	SkeltonVBlankFunc(void);
static	void	GameInit(void);
static	void	GameMain(void);

//==============================================================================
//	�^�C�g�����PROC�Ăяo���悤�̃f�[�^
//==============================================================================
const GFL_PROC_DATA TitleProcData = {
	TitleProcInit,
	TitleProcMain,
	TitleProcEnd,
};


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
}

//------------------------------------------------------------------
/**
 * @brief		�Q�[�����Ƃ̏���������
 */
//------------------------------------------------------------------
extern void TestModeSet( void );	//���T���v���f�o�b�O���[�h

static	void	GameInit(void)
{
	/* ���[�U�[���x���ŕK�v�ȏ������������ɋL�q���� */

	//�Z�[�u�֘A������
	SaveControl_SystemInit(GFL_HEAPID_APP);	//��check �g�p���Ă���q�[�vID�͎b��
	
    // �ʐM�u�[�g���� VBlank���荞�݌�ɍs�����߂����ɋL�q�A�������͕\���p�֐��|�C���^
    GFL_NET_Boot( GFL_HEAPID_APP, NULL );
	/* �N���v���Z�X�̐ݒ� */
#if 0
	TestModeSet();	//���T���v���f�o�b�O���[�h
#else
	GFL_PROC_SysCallProc(NO_OVERLAY_ID, &TitleProcData, NULL);
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

}

//------------------------------------------------------------------
/**
 * @brief		�Q�[�����Ƃ̃��C������
 */
//------------------------------------------------------------------
static	void	GameMain(void)
{
	/* ���[�U�[���x���ŕK�v�ȃ��C�������������ɋL�q���� */

#ifdef PM_DEBUG

	CommErrorSys_SampleView();
	
#endif	// PM_DEBUG

	NetErr_Main();
	NetErr_App_ErrorCheck();
	if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DEBUG){
		NetErr_ErrorSet();
	}
}
