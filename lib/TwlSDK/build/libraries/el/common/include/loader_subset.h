/*---------------------------------------------------------------------------*
  Project:  TwlSDK - ELF Loader
  File:     loader_subset.h

  Copyright 2006-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-12-10#$
  $Rev: 9620 $
  $Author: shirait $
 *---------------------------------------------------------------------------*/

#ifndef _LOADER_SUBSET_H_
#define _LOADER_SUBSET_H_

#include "elf.h"
#include "elf_loader.h"


/*------------------------------------------------------
  �x�j�����o�b�t�@�ɃR�s�[����
    start : �Ăяo�����A�h���X
    data : ��ѐ�A�h���X
 -----------------------------------------------------*/
void* ELi_CopyVeneerToBuffer( ELDesc* elElfDesc, ELObject* MYObject, u32 start, u32 data, s32 threshold);


/*------------------------------------------------------
  �x�j�����o�b�t�@�ɃR�s�[����
    start : �Ăяo����
    data : ��ѐ�
    threshold : ���͈͓̔��Ɋ��Ƀx�j��������Ύg���܂킷
 -----------------------------------------------------*/
void* ELi_CopyV4tVeneerToBuffer( ELDesc* elElfDesc, ELObject* MYObject, u32 start, u32 data, s32 threshold);


/*------------------------------------------------------
  �Z�O�����g���o�b�t�@�ɃR�s�[����
 -----------------------------------------------------*/
void* ELi_CopySegmentToBuffer( ELDesc* elElfDesc, ELObject* MYObject, Elf32_Phdr* Phdr);


/*------------------------------------------------------
  �Z�N�V�������o�b�t�@�ɃR�s�[����
 -----------------------------------------------------*/
void* ELi_CopySectionToBuffer( ELDesc* elElfDesc, ELObject* MYObject, Elf32_Shdr* Shdr);


/*------------------------------------------------------
  �Z�N�V�������o�b�t�@�Ɋm�ۂ���i�R�s�[���Ȃ��j
 -----------------------------------------------------*/
void* ELi_AllocSectionToBuffer( ELDesc* elElfDesc, ELObject* MYObject, Elf32_Shdr* Shdr);
    

/*------------------------------------------------------
  �w��C���f�b�N�X�̃v���O�����w�b�_���o�b�t�@�Ɏ擾����
 -----------------------------------------------------*/
void ELi_GetPhdr( ELDesc* elElfDesc, u32 index, Elf32_Phdr* Phdr);


/*------------------------------------------------------
  �w��C���f�b�N�X�̃Z�N�V�����w�b�_���o�b�t�@�Ɏ擾����
 -----------------------------------------------------*/
void ELi_GetShdr( ELDesc* elElfDesc, u32 index, Elf32_Shdr* Shdr);


/*------------------------------------------------------
  �w��C���f�b�N�X�̃Z�N�V�����̃G���g�����o�b�t�@�Ɏ擾����
 -----------------------------------------------------*/
void ELi_GetSent( ELDesc* elElfDesc, u32 index, void* entry_buf, u32 offset, u32 size);


/*------------------------------------------------------
  �w��Z�N�V�����w�b�_�̎w��C���f�b�N�X�̃G���g�����o�b�t�@�Ɏ擾����
  �i�G���g���T�C�Y���Œ�̃Z�N�V�����̂݁j
 -----------------------------------------------------*/
void ELi_GetEntry( ELDesc* elElfDesc, Elf32_Shdr* Shdr, u32 index, void* entry_buf);


/*------------------------------------------------------
  STR�Z�N�V�����w�b�_�̎w��C���f�b�N�X�̕�������擾����
 -----------------------------------------------------*/
void ELi_GetStrAdr( ELDesc* elElfDesc, u32 strsh_index, u32 ent_index, char* str, u32 len);


/*------------------------------------------------------
  �V���{�����Ē�`����
 -----------------------------------------------------*/
BOOL ELi_RelocateSym( ELDesc* elElfDesc, ELObject* MYObject, u32 relsh_index);

/*------------------------------------------------------
  �O���[�o���V���{�����A�h���X�e�[�u���ɓ����
 -----------------------------------------------------*/
BOOL ELi_GoPublicGlobalSym( ELDesc* elElfDesc, ELObject* MYObject, u32 symtblsh_index);

/*------------------------------------------------------
  ELi_RelocateSym��ELi_GoPublicGlobalSym�̒��ō쐬�����V���{�����X�g��
  �J������i�ǂ�����ĂяI�������Ō�ɂ���API���Ă�ł��������j
 -----------------------------------------------------*/
void ELi_FreeSymList( ELDesc* elElfDesc);

/*------------------------------------------------------
  �������������ƂɃV���{������������
 -----------------------------------------------------*/
BOOL ELi_DoRelocate( ELDesc* elElfDesc, ELObject* MYObject, ELImportEntry* UnresolvedInfo);


/*------------------------------------------------------
  ���X�g����w��C���f�b�N�X��ELSymEx�����o��
 -----------------------------------------------------*/
//ELSymEx* ELi_GetSymExfromList( ELSymEx* SymExStart, u32 index);


/*------------------------------------------------------
  ���X�g����w��C���f�b�N�X��ELShdrEx�����o��
 -----------------------------------------------------*/
ELShdrEx* ELi_GetShdrExfromList( ELShdrEx* ShdrExStart, u32 index);


/*------------------------------------------------------
  �w��C���f�b�N�X�̃Z�N�V�������f�o�b�O��񂩂ǂ������肷��
 -----------------------------------------------------*/
BOOL ELi_ShdrIsDebug( ELDesc* elElfDesc, u32 index);


/*------------------------------------------------------
  elElfDesc��SymEx�e�[�u���𒲂ׁA�w��C���f�b�N�X��
�@�w��I�t�Z�b�g�ɂ���R�[�h��ARM��THUMB���𔻒肷��
 -----------------------------------------------------*/
u32 ELi_CodeIsThumb( ELDesc* elElfDesc, u16 sh_index, u32 offset);


/*---------------------------------------------------------
 �C���|�[�g���G���g��������������
 --------------------------------------------------------*/
static void ELi_InitImport( ELImportEntry* ImportInfo);


/*------------------------------------------------------
  �C���|�[�g���e�[�u������G���g���𔲂��o���iImpEnt�͍폜���Ȃ��I�j
 -----------------------------------------------------*/
BOOL ELi_ExtractImportEntry( ELImportEntry** StartEnt, ELImportEntry* ImpEnt);


/*---------------------------------------------------------
 �C���|�[�g���e�[�u���ɃG���g����ǉ�����
 --------------------------------------------------------*/
void ELi_AddImportEntry( ELImportEntry** ELUnrEntStart, ELImportEntry* UnrEnt);


/*------------------------------------------------------
  �C���|�[�g���e�[�u����S���������
 -----------------------------------------------------*/
void ELi_FreeImportTbl( ELImportEntry** ELImpEntStart);


/*------------------------------------------------------
  �x�j���e�[�u�����������
 -----------------------------------------------------*/
void* ELi_FreeVenTbl( ELDesc* elElfDesc, ELObject* MYObject);



#endif /*_LOADER_SUBSET_H_*/
