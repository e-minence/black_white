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
#include "gfl_use.h"
#include "main.h"

#include "testmode.h"

static	void	SkeltonHBlankFunc(void);
static	void	SkeltonVBlankFunc(void);
static	void	GameInit(void);
static	void	GameMain(void);

extern	void	TestModeSet(void);
//------------------------------------------------------------------
/**
 * @brief	���C������
 */
//------------------------------------------------------------------
void NitroMain(void)
{

    // ���������āc
	GFLUser_Init();

    OS_InitTick();

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

	while(TRUE){
		// ���C���������āc
		GFLUser_Main();
		GameMain();

		// �`��ɕK�v�ȏ������āc
		GFLUser_Display();

		// �����_�����O�G���W�����Q�Ƃ���f�[�^�Q���X���b�v
		// ��gflib�ɓK�؂Ȋ֐����o������u�������Ă�������
		//G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);

        OS_TPrintf("timer %d\n",OS_TicksToMicroSeconds32( OS_GetTick()));

        
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

	GFLUser_VIntr();
}


#include "src/tomoya/debug_tomoya.h"
#include "src/ohno/fatal_error.h"
//------------------------------------------------------------------
/**
 * @brief		�Q�[�����Ƃ̏���������
 */
//------------------------------------------------------------------
static	void	GameInit(void)
{
	/* ���[�U�[���x���ŕK�v�ȏ������������ɋL�q���� */

    // �ʐM�u�[�g���� VBlank���荞�݌�ɍs�����߂����ɋL�q
    GFL_NET_Boot( GFL_HEAPID_APP, FatalError_Disp );
	/* �N���v���Z�X�̐ݒ� */
	TestModeSet();	//���T���v���f�o�b�O���[�h
}


//------------------------------------------------------------------
/**
 * @brief		�Q�[�����Ƃ̃��C������
 */
//------------------------------------------------------------------
static	void	GameMain(void)
{
	/* ���[�U�[���x���ŕK�v�ȃ��C�������������ɋL�q���� */
}


