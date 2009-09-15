/*---------------------------------------------------------------------------*
  Project:  TwlSDK - ELF Loader
  File:     elf_loader.h

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
#ifndef _ELF_LOADER_H_
#define _ELF_LOADER_H_




#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif
//#include <stdio.h>
#include "elf.h"
#include <twl/el.h>



#ifdef __cplusplus
extern "C" {
#endif


/*------------------------------------------------------
  �Z�N�V�����w�b�_�g��(���[�h�A�h���X���ɑΉ�)
 -----------------------------------------------------*/
typedef struct {
  void*         next;
  u16           index;
  u16           debug_flag;    /*0:�f�o�b�O���łȂ��A1:�f�o�b�O���*/
  u32           loaded_adr;
  u32           alloc_adr;
  u32           loaded_size;
  Elf32_Shdr    Shdr;
}ELShdrEx;


/*------------------------------------------------------
  �V���{���G���g���g��(���[�h�A�h���X���ɑΉ�)
 -----------------------------------------------------*/
typedef struct {
  void*      next;
  u16        debug_flag;       /*0:�f�o�b�O���łȂ��A1:�f�o�b�O���*/
  u16        thumb_flag;
  u32        relocation_val;
  Elf32_Sym  Sym;
}ELSymEx;


/*------------------------------------------------------
  �x�j���̃����N���X�g(ELi_DoRelocate�Ŏg�p)
 -----------------------------------------------------*/
typedef struct {
  void* next;
  u32   adr;     /* �x�j���R�[�h�̐擪�A�h���X */
  u32   data;    /* �x�j���̃��e�����v�[���Ɋi�[����Ă����ѐ�̃A�h���X�l */
}ELVeneer;


/*------------------------------------------------------
  �C���|�[�g�p�̍Ĕz�u���e�[�u��

  �ォ��A�h���X�e�[�u�����Q�Ƃ���Ύ��̂悤�ɉ�������B
  S_ = AdrEntry.adr;
  T_ = (u32)(AdrEntry.thumb_flag);
 -----------------------------------------------------*/
typedef struct {
  void*       next;             /*���̃G���g��*/
  char*       sym_str;          /*�������̊O���Q�ƃV���{����*/
  u32         r_type;           /*�����P�[�V�����^�C�v�iELF32_R_TYPE( Rela.r_info)�j*/
  u32         S_;               /*�������̊O���Q�ƃV���{���A�h���X*/
  s32         A_;               /*�����ς�*/
  u32         P_;               /*�����ς�*/
  u32         T_;               /*�������̊O���Q�ƃV���{����ARM/Thumb�t���O*/
  u32         sh_type;          /*SHT_REL or SHT_RELA*/
  struct ELObject*   Dlld;      /*�������ɃC���|�[�g�����G���g�������I�u�W�F�N�g�B����������NULL*/
}ELImportEntry;



/* ELDesc �� process�l */
typedef enum ELProcess {
  EL_PROC_NOTHING      = 0,
  EL_PROC_INITIALIZED = 0x5A,
  EL_PROC_COPIED      = 0xF0,
  EL_PROC_RELOCATED
} ELProcess;


/*------------------------------------------------------
  ELF�I�u�W�F�N�g�ʂ̊Ǘ�
 -----------------------------------------------------*/
typedef struct {
  void*          next;                   /* ���̃I�u�W�F�N�g�G���g�� */
  void*          lib_start;              /* ���C�u�����Ƃ��čĔz�u���ꂽ�A�h���X */
  u32            lib_size;               /* ���C�u�����ɂȂ�����̃T�C�Y */
  u32            buf_limit_addr;         /* �Ĕz�u�p�ɗ^����ꂽ�o�b�t�@�̍ŏI�A�h���X+1 */
  void*          buf_current;            /* �o�b�t�@�|�C���^ */
  ELAdrEntry*    ExportAdrEnt;           /* �G�N�X�|�[�g��� */
  ELAdrEntry*    HiddenAdrEnt;           /* �A�������N���̃G�N�X�|�[�g���ޔ��ꏊ */
  ELImportEntry* ResolvedImportAdrEnt;   /* ���������C���|�[�g��� */
  ELImportEntry* UnresolvedImportAdrEnt; /* �������Ă��Ȃ��C���|�[�g��� */
  ELVeneer*      ELVenEntStart;          /* �x�j���̃����N���X�g */
  ELVeneer*      ELV4tVenEntStart;       /* v4t�x�j���̃����N���X�g */
  u32            stat;                   /* �� */
  u32            file_id;                /* ROM�o�R��DLL��z�u�����ꍇ�̃t�@�C��ID */
  u32            process;                /* ELProcess�^���L���X�g���Ċi�[ */
  u32            result;                 /* ELResult�^���L���X�g���Ċi�[ */
}ELObject;


/*------------------------------------------------------
  ELF�I�u�W�F�N�g�S�̂̊Ǘ�
 -----------------------------------------------------*/
typedef BOOL (*ELi_ReadFunc)( void* buf, void* file_struct, u32 file_base, u32 file_offset, u32 size);
typedef struct {
  void*         ar_head;        /* AR�܂���ELF�t�@�C���̐擪�A�h���X */
  void*         elf_offset;     /* ELF�I�u�W�F�N�g�̐擪�ւ̃I�t�Z�b�g */

  u16           reserve;
  u16           shentsize;      /* 1�Z�N�V�����w�b�_�̃T�C�Y */
  u32           process;        /* ELProcess�^���L���X�g���Ċi�[ */
  u32           result;         /* ELResult�^���L���X�g���Ċi�[ */

  ELShdrEx*     ShdrEx;         /* ShdrEx���X�g�̐擪 */

  Elf32_Ehdr    CurrentEhdr;    /* ELF�w�b�_ */

  Elf32_Rel     Rel;            /* �Ĕz�u�G���g�� */
  Elf32_Rela    Rela;
  Elf32_Sym     Sym;            /* �V���{���G���g�� */
  Elf32_Shdr    SymShdr;

  ELSymEx*      SymEx;          /* SymEx���X�g�̐擪�i��f�o�b�O�V���{���̂݌q����j */
  ELSymEx**     SymExTbl;       /* SymEx�A�h���X�̃e�[�u���i�S�V���{�����Ԃ�j*/
  u32           SymExTarget;    /* SymEx���X�g���\�z�����V���{���Z�N�V�����̃Z�N�V�����ԍ� */

  ELi_ReadFunc  i_elReadStub;   /* ���[�h�X�^�u�֐� */
  void*         FileStruct;     /* �t�@�C���\���� */
    
  u32           entry_adr;      /* �G���g���A�h���X */
  
  ELObject*     ELObjectStart;  /* �I�u�W�F�N�g�̃����N���X�g */
  ELObject*     ELStaticObj;    /* �����N���X�g�Ɍq�����Ă���Static�p�\���̂ւ̃|�C���^ */
}ELDesc;


/*---------------------------------------------------------
 ELF�I�u�W�F�N�g�̃T�C�Y�����߂�
 --------------------------------------------------------*/
u32 EL_GetElfSize( const void* buf);

/*---------------------------------------------------------
 �����N���ꂽ���C�u�����̃T�C�Y�����߂�
 --------------------------------------------------------*/
u32 EL_GetLibSize( ELDlld my_dlld);


/*------------------------------------------------------
  �_�C�i�~�b�N�����N�V�X�e��������������
 -----------------------------------------------------*/
#if 0
//#ifndef SDK_TWL
void ELi_Init( void);
#else
void ELi_Init( ELAlloc alloc, ELFree free);
void* ELi_Malloc( ELDesc* elElfDesc, ELObject* MYObject, size_t size);
#endif

/*------------------------------------------------------
  ELDesc�\���̂�����������
 -----------------------------------------------------*/
BOOL ELi_InitDesc( ELDesc* elElfDesc);

/*------------------------------------------------------
  ELF�I�u�W�F�N�g�܂��͂��̃A�[�J�C�u���t�@�C������o�b�t�@�ɍĔz�u����
 -----------------------------------------------------*/
ELDlld EL_LoadLibraryfromFile( ELDesc* elElfDesc, const char* FilePath, void* buf, u32 buf_size);

/*------------------------------------------------------
  ELF�I�u�W�F�N�g�܂��͂��̃A�[�J�C�u�����[�U�̃��[�hAPI��ʂ��čĔz�u����
 -----------------------------------------------------*/
ELDlld EL_LoadLibrary( ELDesc* elElfDesc, ELReadImage readfunc, u32 len, void* buf, u32 buf_size);

/*------------------------------------------------------
  ELF�I�u�W�F�N�g�܂��͂��̃A�[�J�C�u������������o�b�t�@�ɍĔz�u����
 -----------------------------------------------------*/
ELDlld EL_LoadLibraryfromMem( ELDesc* elElfDesc, void* obj_image, u32 obj_len, void* buf, u32 buf_size);

/*------------------------------------------------------
  �A�h���X�e�[�u�����g���Ė������̃V���{������������
 -----------------------------------------------------*/
ELProcess ELi_ResolveAllLibrary( ELDesc* elElfDesc);


/*------------------------------------------------------
  �A�v���P�[�V��������A�h���X�e�[�u���ɃG���g����ǉ�����
 -----------------------------------------------------*/
BOOL ELi_Export( ELDesc* elElfDesc, ELAdrEntry* AdrEnt);

/*------------------------------------------------------
  �A�h���X�e�[�u���ɃX�^�e�B�b�N���̃G���g����ǉ�����
  �iEL���C�u��������WEAK�V���{���Ƃ��Ē�`����Ă���A
    makelst�����������t�@�C���̒�`�ŏ㏑�������j
 -----------------------------------------------------*/
void EL_AddStaticSym( void);


/*------------------------------------------------------
  �A�h���X�e�[�u������w�蕶����ɊY������A�h���X��Ԃ�
 -----------------------------------------------------*/
void* ELi_GetGlobalAdr( ELDesc* elElfDesc, ELDlld my_dlld, const char* ent_name);


/*------------------------------------------------------
  �I�u�W�F�N�g���A�������N����
 -----------------------------------------------------*/
BOOL ELi_Unlink( ELDesc* elElfDesc, ELDlld my_dlld);


/*------------------------------------------------------
  �I�u�W�F�N�g�ɖ������ȊO���Q�Ƃ��c���Ă��Ȃ����𒲂ׂ�
 -----------------------------------------------------*/
BOOL EL_IsResolved( ELDlld my_dlld);


/*------------------------------------------------------
  �G���[�R�[�h/�v���Z�X�R�[�h�̃Z�b�g
 -----------------------------------------------------*/
void ELi_SetResultCode( ELDesc* elElfDesc, ELObject* MYObject, ELResult result);
void ELi_SetProcCode( ELDesc* elElfDesc, ELObject* MYObject, ELProcess process);


/*------------------------------------------------------
  �q�[�v���m�ۂ���
 -----------------------------------------------------*/
void* ELi_Malloc( ELDesc* elElfDesc, ELObject* MYObject, size_t size);


#ifdef __cplusplus
}    /* extern "C" */
#endif


#endif    /*_ELF_LOADER_H_*/
