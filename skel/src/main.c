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

static	void	SkeltonVBlankFunc(void);
static	void	GameInit(void);
static	void	GameMain(void);

//------------------------------------------------------------------
/**
 * @brief	���C������
 */
//------------------------------------------------------------------
void NitroMain(void)
{
	// ���������āc
	GFLUser_Init();

	//VBLANK���荞�݋���
	OS_SetIrqFunction(OS_IE_V_BLANK,SkeltonVBlankFunc);

	(void)OS_EnableIrqMask(OS_IE_V_BLANK);
	(void)OS_EnableIrq();

	(void)GX_VBlankIntr(TRUE);

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

		// VBLANK�҂�
		// ��gflib�ɓK�؂Ȋ֐����o������u�������Ă�������
		OS_WaitIrq(TRUE,OS_IE_V_BLANK);
	}
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

extern void TetsuTestInit(void);
extern void TetsuTestMain(void);
//------------------------------------------------------------------
/**
 * @brief		�Q�[�����Ƃ̏���������
 */
//------------------------------------------------------------------
static	void	GameInit(void)
{
	/* ���[�U�[���x���ŕK�v�ȏ������������ɋL�q���� */
	TetsuTestInit();
}

//------------------------------------------------------------------
/**
 * @brief		�Q�[�����Ƃ̃��C������
 */
//------------------------------------------------------------------
static	void	GameMain(void)
{
	/* ���[�U�[���x���ŕK�v�ȃ��C�������������ɋL�q���� */
	TetsuTestMain();
}


