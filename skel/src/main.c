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
#include "main.h"

static const HEAP_INIT_HEADER hih[]={
	{ HEAPSIZE_SYSTEM,	OS_ARENA_MAIN },
	{ HEAPSIZE_APP,		OS_ARENA_MAIN },
};

void NitroMain(void)
{
  // ���������āc
  InitSystem();

  GFL_HEAP_sysInit(&hih[0],HEAPID_BASE_MAX,4,0);

  // �K�v��TCB�Ƃ��o�^���āc
  
  while(TRUE){
    // �L�[���͂��āc

    // ���C���������āc

    // �`��ɕK�v�ȏ������āc

    // �����_�����O�G���W�����Q�Ƃ���f�[�^�Q���X���b�v
    // ��gflib�ɓK�؂Ȋ֐����o������u�������Ă�������
    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);

    // VBLANK�҂�
    // ��gflib�ɓK�؂Ȋ֐����o������u�������Ă�������
    SVC_WaitVBlankIntr();
  }
}

/*  */
