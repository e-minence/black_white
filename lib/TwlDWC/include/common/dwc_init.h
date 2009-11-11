/*---------------------------------------------------------------------------*
  Project:  TwlDWC

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_INIT_H_
#define DWC_INIT_H_

#include <common/dwc_memfunc.h>

#ifdef __cplusplus
extern "C"
{
#endif


    /* -------------------------------------------------------------------------
            define
       ------------------------------------------------------------------------- */

// �������֐����[�N�������T�C�Y
#ifdef SDK_TWL
#define DWC_INIT_WORK_SIZE 0x1000
#define DWC_BM_INIT_WORK_SIZE 0x1000
#else
#define DWC_INIT_WORK_SIZE 0x700
#define DWC_BM_INIT_WORK_SIZE 0x700
#endif


    /* -------------------------------------------------------------------------
            enum
       ------------------------------------------------------------------------- */

    // DWC_Init�֐��̕Ԃ�l�������񋓎q
    enum
    {
        DWC_INIT_RESULT_NOERROR = 0,            // �����������B
        DWC_INIT_RESULT_CREATE_USERID,          // DS�{�̂ɉ��̃��[�UID�𐶐����܂����B
        DWC_INIT_RESULT_DESTROY_USERID,         // DS�{�̂̃��[�UID���j�󂳂�Ă����\��������̂ŁA���̃��[�UID�𐶐����܂����B
        DWC_INIT_RESULT_DESTROY_OTHER_SETTING,  // DS�{�̂̐ڑ���ݒ肪�j�󂳂ꂽ�\��������܂��B
        DWC_INIT_RESULT_MEMORY_FULL,            // ���[�N�������̊m�ۂɎ��s���܂����B(HYBRID ROM / LIMITED ROM ��0x1000�o�C�g�ANITRO ROM��0x700�o�C�g)
        DWC_INIT_RESULT_LAST
    };


    /* -------------------------------------------------------------------------
            function - external
       ------------------------------------------------------------------------- */

    int     DWC_InitForDevelopment( const char* gameName, u32 gameCode, DWCAllocEx alloc, DWCFreeEx free );
    int     DWC_InitForProduction( const char* gameName, u32 gameCode, DWCAllocEx alloc, DWCFreeEx free );

    u64     DWC_GetAuthenticatedUserId( void );

    void    DWC_Debug_DWCInitError( void* work, int dwc_init_error );


    /* -------------------------------------------------------------------------
            function - internal
       ------------------------------------------------------------------------- */
    u32    DWCi_GetGamecode         ( void );



#ifdef __cplusplus
}
#endif

#endif // DWC_INIT_H_
