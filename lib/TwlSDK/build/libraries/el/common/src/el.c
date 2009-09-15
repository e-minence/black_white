/*---------------------------------------------------------------------------*
  Project:  TwlSDK - ELF Loader
  File:     el.c

  Copyright 2006-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include "el_config.h"

#include <twl/el.h>


#include "elf.h"
#include "arch.h"
#include "elf_loader.h"
#include "loader_subset.h"


/*------------------------------------------------------
  extern �ϐ�
 -----------------------------------------------------*/
extern ELAlloc     i_elAlloc;
extern ELFree      i_elFree;


/*------------------------------------------------------
  extern �֐�
 -----------------------------------------------------*/
extern BOOL elRemoveObjEntry( ELObject** StartEnt, ELObject* ObjEnt);


/*------------------------------------------------------
  �O���[�o���ϐ�
 -----------------------------------------------------*/
ELDesc* i_eldesc     = NULL;    // for link
ELDesc* i_eldesc_sim = NULL;    // for simulation link


/*---------------------------------------------------------------------------*
  Name:         EL_Init

  Description:  �_�C�i�~�b�N�����N�V�X�e��������������

  Arguments:    alloc    �q�[�v�擾�֐�
                free     �q�[�v�J���֐�

  Returns:      ��������� 0, ���s����� -1
 *---------------------------------------------------------------------------*/
s32 EL_Init( ELAlloc alloc, ELFree free)
{
    if( i_eldesc != NULL) {
        return( -1);    //Init�ς�
    }
  
    ELi_Init( alloc, free);

    i_eldesc = ELi_Malloc( NULL, NULL, ((sizeof( ELDesc))*2)); //for link and simulation-link
    if( i_eldesc == NULL) {
        return( -1);
    }

    (void)ELi_InitDesc( i_eldesc);
    i_eldesc_sim = &(i_eldesc[1]);
    (void)ELi_InitDesc( i_eldesc_sim);

    return( 0);
}

#if 1
/*---------------------------------------------------------------------------*
  Name:         EL_CalcEnoughBufferSizeforLink*

  Description:  ���I�����N���e�X�g���čĔz�u����̂ɏ\���ȃo�b�t�@�T�C�Y��Ԃ��B
                ���I�I�u�W�F�N�g���O���Q�Ƃ��Ă���ꍇ�A���傫�ڂ̃T�C�Y��
                �Ԃ��ꍇ������܂�

  Arguments:    FilePath    ���I�I�u�W�F�N�g�̃p�X��
                buf         �Ĕz�u��ƂȂ�o�b�t�@�A�h���X�i���ۂɏ������݂͍s���܂���j
                readfunc    �A�v�����p�ӂ������[�h�֐�
                len         ���I�I�u�W�F�N�g�̃T�C�Y
                obj_image   ���I�I�u�W�F�N�g�����݂��郁������̃A�h���X
                obj_len     ���I�I�u�W�F�N�g�̃T�C�Y
                link_mode   ���I�����N���e�X�g����ۂ̃��[�h

  Returns:      ��������΃o�b�t�@�T�C�Y, ���s����� -1
 *---------------------------------------------------------------------------*/
s32 EL_CalcEnoughBufferSizeforLinkFile( const char* FilePath, const void* buf, ELLinkMode link_mode)
{
    ELDlld dlld;
    u32    size;

    if( link_mode != EL_LINKMODE_ONESHOT) {
        return( -1);
    }
    dlld = EL_LoadLibraryfromFile( i_eldesc_sim, FilePath, (void*)buf, 0xFFFFFFFF);
    if( dlld) {
        if( ELi_ResolveAllLibrary( i_eldesc_sim) != EL_PROC_RELOCATED) {
            return( -1);
        }
        size = ((ELObject*)dlld)->lib_size; //ELi_ResolveAllLibrary��Ȃ̂�veneer���݂̃T�C�Y

        (void)ELi_Unlink( i_eldesc, dlld);
        (void)elRemoveObjEntry( &(i_eldesc_sim->ELObjectStart), (ELObject*)dlld);
        return( (s32)size);
    }
    return( -1);
}

/*------------------------------------------------------*/
s32 EL_CalcEnoughBufferSizeforLink( ELReadImage readfunc, u32 len, const void* buf, ELLinkMode link_mode)
{
    ELDlld dlld;
    u32    size;
  
    if( link_mode != EL_LINKMODE_ONESHOT) {
        return( -1);
    }
    dlld = EL_LoadLibrary( i_eldesc_sim, readfunc, len, (void*)buf, 0xFFFFFFFF);
    if( dlld) {
        if( ELi_ResolveAllLibrary( i_eldesc_sim) != EL_PROC_RELOCATED) {
            return( -1);
        }
        size = ((ELObject*)dlld)->lib_size; //ELi_ResolveAllLibrary��Ȃ̂�veneer���݂̃T�C�Y

        (void)ELi_Unlink( i_eldesc, dlld);
        (void)elRemoveObjEntry( &(i_eldesc_sim->ELObjectStart), (ELObject*)dlld);
        return( (s32)size);
    }
    return( -1);
}

/*------------------------------------------------------*/
s32 EL_CalcEnoughBufferSizeforLinkImage( void* obj_image, u32 obj_len, const void* buf, ELLinkMode link_mode)
{
    ELDlld dlld;
    u32    size;
  
    if( link_mode != EL_LINKMODE_ONESHOT) {
        return( -1);
    }
    dlld = EL_LoadLibraryfromMem( i_eldesc_sim, obj_image, obj_len, (void*)buf, 0xFFFFFFFF);
    if( dlld) {
        if( ELi_ResolveAllLibrary( i_eldesc_sim) != EL_PROC_RELOCATED) {
            return( -1);
        }
        size = ((ELObject*)dlld)->lib_size; //ELi_ResolveAllLibrary��Ȃ̂�veneer���݂̃T�C�Y

        (void)ELi_Unlink( i_eldesc, dlld);
        (void)elRemoveObjEntry( &(i_eldesc_sim->ELObjectStart), (ELObject*)dlld);
        return( (s32)size);
    }
    return( -1);
}
#endif


/*---------------------------------------------------------------------------*
  Name:         EL_Link*

  Description:  ���I�I�u�W�F�N�g���i�t�@�C��/���[�UAPI/����������j�Ĕz�u����

  Arguments:    FilePath    ���I�I�u�W�F�N�g�̃p�X��
                readfunc    �A�v�����p�ӂ������[�h�֐�
                len         ���I�I�u�W�F�N�g�̃T�C�Y
                obj_image   ���I�I�u�W�F�N�g�����݂��郁������̃A�h���X
                obj_len     ���I�I�u�W�F�N�g�̃T�C�Y
                buf         �Ĕz�u��ƂȂ�o�b�t�@�A�h���X

  Returns:      ��������Γ��I���W���[���̃n���h���A���s����� 0
 *---------------------------------------------------------------------------*/
ELDlld EL_LinkFileEx( const char* FilePath, void* buf, u32 buf_size)
{
    return( EL_LoadLibraryfromFile( i_eldesc, FilePath, buf, buf_size));
}

/*------------------------------------------------------*/
ELDlld EL_LinkEx( ELReadImage readfunc, u32 len, void* buf, u32 buf_size)
{
    return( EL_LoadLibrary( i_eldesc, readfunc, len, buf, buf_size));
}

/*------------------------------------------------------*/
ELDlld EL_LinkImageEx( void* obj_image, u32 obj_len, void* buf, u32 buf_size)
{
    return( EL_LoadLibraryfromMem( i_eldesc, obj_image, obj_len, buf, buf_size));
}

/*---------------------------------------------------------------------------*
  Name:         EL_Link*

  Description:  ���I�I�u�W�F�N�g���i�t�@�C��/���[�UAPI/����������j�Ĕz�u����

  Arguments:    FilePath    ���I�I�u�W�F�N�g�̃p�X��
                readfunc    �A�v�����p�ӂ������[�h�֐�
                len         ���I�I�u�W�F�N�g�̃T�C�Y
                obj_image   ���I�I�u�W�F�N�g�����݂��郁������̃A�h���X
                obj_len     ���I�I�u�W�F�N�g�̃T�C�Y
                buf         �Ĕz�u��ƂȂ�o�b�t�@�A�h���X

  Returns:      ��������Γ��I���W���[���̃n���h���A���s����� 0
 *---------------------------------------------------------------------------*/
ELDlld EL_LinkFile( const char* FilePath, void* buf)
{
    return( EL_LoadLibraryfromFile( i_eldesc, FilePath, buf, 0xFFFFFFFF));
}

/*------------------------------------------------------*/
ELDlld EL_Link( ELReadImage readfunc, u32 len, void* buf)
{
    return( EL_LoadLibrary( i_eldesc, readfunc, len, buf, 0xFFFFFFFF));
}

/*------------------------------------------------------*/
ELDlld EL_LinkImage( void* obj_image, u32 obj_len, void* buf)
{
    return( EL_LoadLibraryfromMem( i_eldesc, obj_image, obj_len, buf, 0xFFFFFFFF));
}


/*---------------------------------------------------------------------------*
  Name:         EL_ResolveAll

  Description:  �������̃V���{������������

  Arguments:    None.

  Returns:      ��������� EL_RELOCATED, ���s����� EL_FAILED
 *---------------------------------------------------------------------------*/
u16 EL_ResolveAll( void)
{
    if( ELi_ResolveAllLibrary( i_eldesc) == EL_PROC_RELOCATED) {
        return( EL_RELOCATED);
    }
    return( EL_FAILED);
}


/*---------------------------------------------------------------------------*
  Name:         EL_Export

  Description:  DLL�V�X�e���ɃV���{������o�^����

  Arguments:    None.

  Returns:      ��������� TRUE
 *---------------------------------------------------------------------------*/
BOOL EL_Export( ELAdrEntry* AdrEnt)
{
    /* (TODO)i_eldesc_sim�ɒǉ�����Ȃ��̂�simulation�ɂ͔��f����Ȃ� */
    return ELi_Export( i_eldesc, AdrEnt);
}


/*---------------------------------------------------------------------------*
  Name:         EL_GetGlobalAdr

  Description:  ���I���W���[������V���{����T���ăA�h���X��Ԃ�

  Arguments:    my_dlld     �����ΏۂƂ��铮�I���W���[���̃n���h��
                ent_name    �V���{����

  Returns:      ��������΃V���{���̃A�h���X�A���s����� 0
 *---------------------------------------------------------------------------*/
void* EL_GetGlobalAdr( ELDlld my_dlld, const char* ent_name)
{
    return( ELi_GetGlobalAdr( i_eldesc, my_dlld, ent_name));
}


/*---------------------------------------------------------------------------*
  Name:         EL_Unlink

  Description:  ���I���W���[�����A�������N����

  Arguments:    my_dlld     �A�������N���铮�I���W���[���̃n���h��

  Returns:      ��������� EL_SUCCESS, ���s����� EL_FAILED
 *---------------------------------------------------------------------------*/
u16 EL_Unlink( ELDlld my_dlld)
{
    if( ELi_Unlink( i_eldesc, my_dlld) == TRUE) {
        /* �Y���I�u�W�F�N�g�\���̂�j������̂� ReLink�s�� */
        (void)elRemoveObjEntry( &(i_eldesc->ELObjectStart), (ELObject*)my_dlld); // ���s�ł����Ȃ�
        return( EL_SUCCESS);
    }
    return( EL_FAILED);
}


/*---------------------------------------------------------------------------*
  Name:         EL_GetResultCode

  Description:  �Ō�ɍs���������̏ڍׂȌ��ʂ�Ԃ�

  Arguments:    None.

  Returns:      ELResult�^�̃G���[�R�[�h
 *---------------------------------------------------------------------------*/
ELResult EL_GetResultCode( void)
{
    if( i_eldesc == NULL) {
        return( EL_RESULT_FAILURE);
    }
    return( (ELResult)(i_eldesc->result));
}

#if 0
ELResult EL_GetResultCode( ELDlld my_dlld)
{
    ELObject*      MYObject;
  
    if( my_dlld == 0) {
        return( EL_RESULT_INVALID_PARAMETER);
    }
  
    // ���ʂ��Q�Ƃ���I�u�W�F�N�g
    MYObject = (ELObject*)my_dlld;

    return( MYObject->result);
}
#endif
