/*---------------------------------------------------------------------------*
  Project:  TwlSDK - ELF Loader
  File:     el.h

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
#ifndef TWL_COMMON_EL_H_
#define TWL_COMMON_EL_H_

#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/*------------------------------------------------------
  typedef
 -----------------------------------------------------*/
typedef u32   ELDlld;
typedef void *(*ELAlloc)(size_t size);
typedef void  (*ELFree)(void* ptr);
typedef u32   (*ELReadImage)( u32 offset, void* buf, u32 size);


/*------------------------------------------------------
  EL_CalcEnoughBufferSizeforLink* �Ŏw�肷��萔
 -----------------------------------------------------*/
typedef enum ELLinkMode {
  EL_LINKMODE_ROUGH = 0,
  EL_LINKMODE_ONESHOT,
  EL_LINKMODE_RELATION
} ELLinkMode;

/*------------------------------------------------------
  �G���[�R�[�h
 -----------------------------------------------------*/
#define EL_SUCCESS   0
#define EL_FAILED    1
#define EL_RELOCATED 0xF1    //for EL_ResolveAll only

/*------------------------------------------------------
  EL_GetResultCode�Ŏ擾����ڍׂȃG���[�R�[�h
 -----------------------------------------------------*/
typedef enum ELResult {
  EL_RESULT_SUCCESS = 0,
  EL_RESULT_FAILURE = 1,
  EL_RESULT_INVALID_PARAMETER,
  EL_RESULT_INVALID_ELF,
  EL_RESULT_UNSUPPORTED_ELF,
  EL_RESULT_CANNOT_ACCESS_ELF, //ELF�t�@�C���� open/read�G���[
  EL_RESULT_NO_MORE_RESOURCE   //malloc���s
} ELResult;


/*------------------------------------------------------
  �G�N�X�|�[�g�p�̃A�h���X�e�[�u��
 -----------------------------------------------------*/
typedef struct {
  void*      next;              /*���̃A�h���X�G���g��*/
  char*      name;              /*������*/
  void*      adr;               /*�A�h���X*/
  u16        func_flag;         /*0:�f�[�^�A1:�֐�*/
  u16        thumb_flag;        /*0:arm�R�[�h�A1:thumb�R�[�h*/
}ELAdrEntry;


/*---------------------------------------------------------------------------*
  Name:         EL_Init

  Description:  �_�C�i�~�b�N�����N�V�X�e��������������

  Arguments:    alloc    �q�[�v�擾�֐�
                free     �q�[�v�J���֐�

  Returns:      ��������� 0, ���s����� -1
 *---------------------------------------------------------------------------*/
s32 EL_Init( ELAlloc alloc, ELFree free);


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
                            �i�������ԂƐ��x�̃g���[�h�I�t��I�����邽�߂̒萔�j

  Returns:      ��������΃o�b�t�@�T�C�Y, ���s����� -1
 *---------------------------------------------------------------------------*/
s32 EL_CalcEnoughBufferSizeforLinkFile( const char* FilePath, const void* buf, ELLinkMode link_mode);
s32 EL_CalcEnoughBufferSizeforLink( ELReadImage readfunc, u32 len, const void* buf, ELLinkMode link_mode);
s32 EL_CalcEnoughBufferSizeforLinkImage( void* obj_image, u32 obj_len, const void* buf, ELLinkMode link_mode);


/*---------------------------------------------------------------------------*
  Name:         EL_Link*Ex

  Description:  �^����ꂽ�o�b�t�@�Ɏ��܂邩���`�F�b�N���Ȃ���A
                ���I�I�u�W�F�N�g���i�t�@�C��/���[�UAPI/����������j�Ĕz�u����

  Arguments:    FilePath    ���I�I�u�W�F�N�g�̃p�X��
                readfunc    �A�v�����p�ӂ������[�h�֐�
                len         ���I�I�u�W�F�N�g�̃T�C�Y
                obj_image   ���I�I�u�W�F�N�g�����݂��郁������̃A�h���X
                obj_len     ���I�I�u�W�F�N�g�̃T�C�Y
                buf         �Ĕz�u��ƂȂ�o�b�t�@�A�h���X
                buf_size    �Ĕz�u��ƂȂ�o�b�t�@�̃T�C�Y

  Returns:      ��������Γ��I���W���[���̃n���h���A���s����� 0
 *---------------------------------------------------------------------------*/
ELDlld EL_LinkFileEx( const char* FilePath, void* buf, u32 buf_size);
ELDlld EL_LinkEx( ELReadImage readfunc, u32 len, void* buf, u32 buf_size);
ELDlld EL_LinkImageEx( void* obj_image, u32 obj_len, void* buf, u32 buf_size);

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
ELDlld EL_LinkFile( const char* FilePath, void* buf);
ELDlld EL_Link( ELReadImage readfunc, u32 len, void* buf);
ELDlld EL_LinkImage( void* obj_image, u32 obj_len, void* buf);


/*---------------------------------------------------------------------------*
  Name:         EL_ResolveAll

  Description:  �������̃V���{������������

  Arguments:    None.

  Returns:      ��������� EL_RELOCATED, ���s����� EL_FAILED
 *---------------------------------------------------------------------------*/
u16 EL_ResolveAll( void);


/*---------------------------------------------------------------------------*
  Name:         EL_Export

  Description:  DLL�V�X�e���ɃV���{������o�^����

  Arguments:    None.

  Returns:      ��������� TRUE
 *---------------------------------------------------------------------------*/
BOOL EL_Export( ELAdrEntry* AdrEnt);


/*---------------------------------------------------------------------------*
  Name:         EL_AddStaticSym

  Description:  DLL�V�X�e���ɐÓI���W���[���̃V���{������o�^����
                �iEL���C�u��������WEAK�V���{���Ƃ��Ē�`����Ă���A
                  makelst�����������t�@�C���̒�`�ŏ㏑�������j
  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void EL_AddStaticSym( void);


/*---------------------------------------------------------------------------*
  Name:         EL_GetGlobalAdr

  Description:  ���I���W���[������V���{����T���ăA�h���X��Ԃ�

  Arguments:    my_dlld     �����ΏۂƂ��铮�I���W���[���̃n���h��
                ent_name    �V���{����

  Returns:      ��������΃V���{���̃A�h���X�A���s����� 0
 *---------------------------------------------------------------------------*/
void* EL_GetGlobalAdr( ELDlld my_dlld, const char* ent_name);


/*---------------------------------------------------------------------------*
  Name:         EL_Unlink

  Description:  ���I���W���[�����A�������N����

  Arguments:    my_dlld     �A�������N���铮�I���W���[���̃n���h��

  Returns:      ��������� EL_SUCCESS, ���s����� EL_FAILED
 *---------------------------------------------------------------------------*/
u16 EL_Unlink( ELDlld my_dlld);


/*---------------------------------------------------------------------------*
  Name:         EL_IsResolved

  Description:  ���I���W���[���̊O���Q�Ƃ��S�ĉ����ς݂��ǂ����𒲂ׂ�

  Arguments:    my_dlld     ���ׂ�ΏۂƂ��铮�I���W���[���̃n���h��

  Returns:      �������ȊO���Q�Ƃ��c���Ă��Ȃ���� TRUE
 *---------------------------------------------------------------------------*/
BOOL EL_IsResolved( ELDlld my_dlld);


/*---------------------------------------------------------------------------*
  Name:         EL_GetLibSize

  Description:  �����N���ꂽ���I���W���[���̃T�C�Y��Ԃ�
                ���I���W���[���ɂ܂��������ȊO���Q�Ƃ��c���Ă��Ă����̎��_�ł�
                �T�C�Y��Ԃ��܂��B
                EL_ResolveAll��EL_RELOCATED��Ԃ��̂��m�F���Ă���ł���΁A
                �Ĕz�u������������̍ŏI�I�ȃT�C�Y�𓾂邱�Ƃ��ł��܂��B

  Arguments:    my_dlld     �T�C�Y�𒲂ׂ�ΏۂƂ��铮�I���W���[���̃n���h��

  Returns:      ��������΃T�C�Y�A���s����� 0
 *---------------------------------------------------------------------------*/
u32 EL_GetLibSize( ELDlld my_dlld);


/*---------------------------------------------------------------------------*
  Name:         EL_GetResultCode

  Description:  �Ō�ɍs���������̏ڍׂȌ��ʂ�Ԃ�

  Arguments:    None.

  Returns:      ELResult�^�̃G���[�R�[�h
 *---------------------------------------------------------------------------*/
ELResult EL_GetResultCode( void);


#ifdef __cplusplus
}    /* extern "C" */
#endif

#endif    /*TWL_COMMON_EL_H_*/
