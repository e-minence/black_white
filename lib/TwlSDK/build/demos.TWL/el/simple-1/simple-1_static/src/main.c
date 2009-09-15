/*---------------------------------------------------------------------------*
  Project:  TwlSDK - EL - demos - simple-1
  File:     main.c

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include <twl.h>
#include <twl/el.h>


#include "dllA.h"

/*---------------------------------------------------------------------------*
    static�ϐ�
 *---------------------------------------------------------------------------*/
static u32 lib_buf[8192];
int        fd;

typedef void (*global_func_p)( void);
global_func_p   global_func;

/*---------------------------------------------------------------------------*
  Name:         MY_Alloc

  Description:  el���C�u�����ɓn���������m�ۂ̃��[�UAPI

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void *MY_Alloc(size_t size)
{
    void* heap;
    heap = OS_Alloc( size);
    if( heap == NULL) { OS_TPanic( "OS_Alloc failed.\n");}
    return( heap);
}

/*---------------------------------------------------------------------------*
  Name:         MY_Free

  Description:  el���C�u�����ɓn���������J���̃��[�UAPI

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void MY_Free(void *ptr)
{
    OS_Free( ptr);
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  VBlank interrupt handler

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    //---- check interrupt flag
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         TwlMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void TwlMain(void)
{
    ELDlld my_dlld;

    /* OS�̏����� */
    OS_Init();
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)GX_VBlankIntr(TRUE);

    {
        void   *tempLo;
        OSHeapHandle hh;

        // OS_Init�͌Ă΂�Ă���Ƃ����O��
        tempLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
        OS_SetArenaLo(OS_ARENA_MAIN, tempLo);
        hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
        if (hh < 0)
        {
            OS_Panic("ARM9: Fail to create heap...\n");
        }
        hh = OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
    }


    OS_TPrintf("\n");
    OS_TPrintf("===================================\n");
    OS_TPrintf("EL Library test\n");
    OS_TPrintf("===================================\n");

    /* EL ���C�u�����̃e�X�g�J�n */
    {
        FS_Init( FS_DMA_NOT_USE );
        
        if( EL_Init( MY_Alloc, MY_Free) < 0)
        {
            OS_TPanic( "EL_Init failed.\n");
        }
        else
        {
            OS_TPrintf( "EL_Init success.\n");
        }

        // ���I���W���[����ROM�ɑ��݂���
        my_dlld = EL_LinkFileEx( "rom:/data/dllA.a", lib_buf, 8192);
        if(my_dlld == 0)
            OS_Panic("failed EL_LinkFileEx\n");
        
        OS_TPrintf( "dll loaded 0x%x - 0x%x\n", (u32)lib_buf, (u32)lib_buf + EL_GetLibSize( my_dlld));

        OS_TPrintf( "object resolved flag = %d\n", EL_IsResolved( my_dlld));

        /* �X�^�e�B�b�N���̃V���{�����G�N�X�|�[�g */
        EL_AddStaticSym();

        /* DLL�̃V���{������ */
        (void)EL_ResolveAll();

        OS_TPrintf( "object resolved flag = %d\n", EL_IsResolved( my_dlld));
        if(!EL_IsResolved( my_dlld))
            OS_Panic("failed EL_ResolveAll\n");

        OS_TPrintf( "LINK : dynamic\n");
        global_func = (global_func_p)EL_GetGlobalAdr( my_dlld, "global_func_A\0");
        OS_TPrintf( "global_func : 0x%x\n", global_func);
        if(global_func == 0)
            OS_Panic("failed EL_GetGlobalAdr\n");
        
        OS_TPrintf( "----- dll-func1 execution -----\n");
        (*global_func)();
        
        OS_TPrintf( "----- dll execution end -----\n");

        /* �A�������N */
        (void)EL_Unlink( my_dlld);
        OS_TPrintf( "EL_Unlink success.\n");
      
        OS_TPrintf( "object resolved flag = %d\n", EL_IsResolved( my_dlld));
        
    }

    OS_TPrintf("\n");
    OS_TPrintf("===================================\n");
    OS_TPrintf("Finish\n");
    OS_TPrintf("===================================\n");
    OS_Terminate();
}

/*====== End of main.c ======*/
