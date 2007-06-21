//===================================================================
/**
 * @file	initialize.c
 * @brief	������
 * @author	GAME FREAK Inc.
 * @date	06.11.07
 *
 * $LastChangedDate$
 * $Id$
 */
//===================================================================
#include <nitro.h>
#include "gflib.h"
#include "gflib_inter.h"


//------------------------------------------------------------------
/**
 * @brief	�V�X�e��������
 * @param	NONE
 * @return	NONE
 */
//------------------------------------------------------------------
void InitSystem(void)
{
#ifdef USE_MAINEXMEMORY
  OS_EnableMainExArena();
#endif

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
  InitFileSystem();

  OS_EnableIrq();  // ���̊֐��͉�c�ő��k������ňړ�����  @@OO
}


/*  */
