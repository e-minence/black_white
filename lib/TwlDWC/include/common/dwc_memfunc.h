/*---------------------------------------------------------------------------*
  Project:  TwlDWC

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_MEMFUNC_H_
#define DWC_MEMFUNC_H_

#ifdef __cplusplus
extern "C"
{
#endif


    /* --------------------------------------------------------------------
      enums
      ----------------------------------------------------------------------*/

    typedef enum
    {
        DWC_ALLOCTYPE_AUTH,
        DWC_ALLOCTYPE_AC,
        DWC_ALLOCTYPE_BM,
        DWC_ALLOCTYPE_UTIL,
        DWC_ALLOCTYPE_BASE,
        DWC_ALLOCTYPE_LANMATCH,
        DWC_ALLOCTYPE_GHTTP,
        DWC_ALLOCTYPE_RANKING,
        DWC_ALLOCTYPE_ENC,
        DWC_ALLOCTYPE_GS,
        DWC_ALLOCTYPE_ND,
        DWC_ALLOCTYPE_TAC,
        DWC_ALLOCTYPE_OPTION_CF,
        DWC_ALLOCTYPE_NHTTP,
        DWC_ALLOCTYPE_MAIL,
        DWC_ALLOCTYPE_LAST,

        DWC_ALLOCTYPE_NUM = DWC_ALLOCTYPE_LAST
    } DWCAllocType;



    /* --------------------------------------------------------------------
      typedefs
      ----------------------------------------------------------------------*/

    /**
     * DWC���C�u�����S�̂Ŏg�p���郁�����m�ۊ֐�
     *
     * DWC���C�u��������g�p����郁�����m�ۊ֐��ł��B�X���b�h�r���������s���K�v������܂��B
     * TWL���[�h�œ��삳����ꍇ�A���C����������̊g���A���[�i����̈���m�ۂ���֐��͎w��ł��܂���B
     *
     * Param:    align   �o�C�g�A���C�����g���ꂽsize�o�C�g���̃o�b�t�@�ւ̃|
     *                  �C���^��Ԃ��Ă��������B
     * Param:    name    DWC���C�u�����J���p�̏�񂪊܂܂�Ă��܂��̂ŁA����
     *                  ���Ă��������B
     *
     * Return value:  �������m�ۂɐ��������ꍇ�Aalign�o�C�g�A���C�����g���ꂽsize�o�C�g���̃o�b�t�@�ւ̃|�C���^
     * �������m�ۂɎ��s�����ꍇ�ANULL
     *
     * See also:  DWCFreeEx
     * 
     */
    typedef void* (*DWCAllocEx)( DWCAllocType name, u32   size, int align );

    /**
     * DWC���C�u�����S�̂Ŏg�p���郁�����J���֐�
     *
     * DWC���C�u��������g�p����郁��������֐��ł��B�X���b�h�r���������s���K�v������܂��B
     *
     * DWCAllocEx�Ŋm�ۂ��ꂽ�o�b�t�@���J�����܂��BNULL�|�C���^���n���ꂽ�Ƃ��ɂ͉��������ɏ������I�����Ă��������B
     *
     * Param:    ptr     �������m�ۊ֐��Ŋm�ۂ��ꂽ�o�b�t�@�̃|�C���^�B
     * Param:    name    DWC���C�u�����J���p�̏�񂪊܂܂�Ă��܂��̂ŁA����
     *                  ���Ă��������B
     * Param:    size    DWC���C�u�����J���p�̏��Ő��m�Ȓl���i�[����Ă���
     *                  ���ꍇ������܂��̂ŁA�������Ă��������B
     *
     * See also:  DWCAllocEx
     *
     */
    typedef void  (*DWCFreeEx )( DWCAllocType name, void* ptr,  u32 size  );


    /* --------------------------------------------------------------------
      functions
      ----------------------------------------------------------------------*/
    void* DWC_Alloc     ( DWCAllocType name, u32 size );
    void* DWC_AllocEx   ( DWCAllocType name, u32 size, int align );
    void  DWC_Free      ( DWCAllocType name, void* ptr, u32 size );
    void* DWC_Realloc   ( DWCAllocType name, void* ptr, u32 oldsize, u32 newsize );
    void* DWC_ReallocEx ( DWCAllocType name, void* ptr, u32 oldsize, u32 newsize, int align );

    void* DWCi_GsMalloc  (size_t size);
    void* DWCi_GsRealloc (void* ptr, size_t size);
    void  DWCi_GsFree    (void* ptr);
    void* DWCi_GsMemalign(size_t boundary, size_t size);



#ifdef __cplusplus
}
#endif

#endif // DWC_MEMFUNC_H_
