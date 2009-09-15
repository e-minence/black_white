/*---------------------------------------------------------------------------*
  Project:  TwlSDK - ELF Loader
  File:     loader_subset.c

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
#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif

#include "loader_subset.h"
#include <istdbglibpriv.h>


/*------------------------------------------------------
  extern�ϐ�
 -----------------------------------------------------*/
//#ifndef SDK_TWL
//#else
extern ELAlloc   i_elAlloc;
extern ELFree    i_elFree;
//#endif
extern ELDesc* i_eldesc_sim;


/*------------------------------------------------------
  extern�֐�
 -----------------------------------------------------*/
extern BOOL        elRemoveAdrEntry( ELAdrEntry* AdrEnt);
extern void        elAddAdrEntry( ELAdrEntry** ELAdrEntStart, ELAdrEntry* AdrEnt);
extern ELAdrEntry* elGetAdrEntry( ELDesc* elElfDesc, const char* ent_name, ELObject** ExpObjEnt);


/*------------------------------------------------------
  static�֐�
 -----------------------------------------------------*/
static u32 el_veneer[3] = { //ARM�R�[�h
    0xE59FC000,    //(LDR r12,[PC])
    0xE12FFF1C,    //(BX  r12)
    0x00000000,    //(data)
};

static u16 el_veneer_t[10] = { //v4t�pThumb�R�[�h
    0xB580,  //(PUSH {R7,LR})
    0x46FE,  //(NOP)
    0x46FE,  //(MOV LR, PC)
    0x2707,  //(MOV R7, 7)
    0x44BE,  //(ADD LR,R7)
    0x4F01,  //(LDR R7,[PC+4])
    0x4738,  //(BX R7) ---> ARM routine
    0xBD80,  //(POP {R7,PC} <---LR(v4t�ł͖�������Thumb����)
    0x0000,  //(data) ARM routine address
    0x0000,  //(data)
};

static BOOL ELi_CheckBufRest( ELDesc* elElfDesc, ELObject* MYObject, void* start, u32 size);

static BOOL     ELi_BuildSymList( ELDesc* elElfDesc, u32 symsh_index);
static ELSymEx* ELi_GetSymExfromList( ELSymEx* SymExStart, u32 index);
static ELSymEx* ELi_GetSymExfromTbl( ELSymEx** SymExTbl, u32 index);
static void     ELi_InitImport( ELImportEntry* ImportInfo);

//�C���|�[�g���e�[�u������G���g���𔲂��o���iImpEnt�͍폜���Ȃ��I�j
BOOL ELi_ExtractImportEntry( ELImportEntry** StartEnt, ELImportEntry* ImpEnt);
void ELi_AddImportEntry( ELImportEntry** ELUnrEntStart, ELImportEntry* UnrEnt);

static void      ELi_AddVeneerEntry( ELVeneer** start, ELVeneer* VenEnt);
static BOOL      ELi_RemoveVenEntry( ELVeneer** start, ELVeneer* VenEnt);
static ELVeneer* ELi_GetVenEntry( ELVeneer** start, u32 data);
static BOOL      ELi_IsFar( u32 P, u32 S, s32 threshold);


/*------------------------------------------------------
  �A�h���X���A���C�������g����
 -----------------------------------------------------*/
#define ALIGN( addr, align_size ) (((addr)  & ~((align_size) - 1)) + (align_size))

static u32 ELi_ALIGN( u32 addr, u32 align_size);
u32 ELi_ALIGN( u32 addr, u32 align_size)
{
    u32 aligned_addr;
    
    if( (addr % align_size) == 0) {
        aligned_addr = addr;
    }else{
        aligned_addr = (((addr) & ~((align_size) - 1)) + (align_size));
    }
    
    return aligned_addr;
}


/*------------------------------------------------------
  IS-TWL_DEBUGGER�ւ̒ʒm�֐�
 -----------------------------------------------------*/
SDK_WEAK_SYMBOL BOOL _ISTDbgLib_RegistVeneerInfo( ISTDOVERLAYPROC nProc, ISTDVENEERTYPE vType, u32 nVeneerAddress, u32 nJumpAddress)
{
#pragma unused( nProc)
#pragma unused( vType)
#pragma unused( nVeneerAddress)
#pragma unused( nJumpAddress)
    return( TRUE);
}

SDK_WEAK_SYMBOL BOOL _ISTDbgLib_UnregistVeneerInfo( ISTDOVERLAYPROC nProc, ISTDVENEERTYPE vType, u32 nVeneerAddress)
{
#pragma unused( nProc, vType, nVeneerAddress)
    return( TRUE);
}

static BOOL ELi_REGISTER_VENEER_INFO( ISTDOVERLAYPROC nProc, ISTDVENEERTYPE vType, u32 nVeneerAddress, u32 nJumpAddress, BOOL enable);
static BOOL ELi_UNREGISTER_VENEER_INFO( ISTDOVERLAYPROC nProc, ISTDVENEERTYPE vType, u32 nVeneerAddress, BOOL enable);

static BOOL ELi_REGISTER_VENEER_INFO( ISTDOVERLAYPROC nProc, ISTDVENEERTYPE vType, u32 nVeneerAddress, u32 nJumpAddress, BOOL enable)
{
    if( enable) {
        return( _ISTDbgLib_RegistVeneerInfo( nProc, vType, nVeneerAddress, nJumpAddress));
    }
    return( FALSE);
}

static BOOL ELi_UNREGISTER_VENEER_INFO( ISTDOVERLAYPROC nProc, ISTDVENEERTYPE vType, u32 nVeneerAddress, BOOL enable)
{
    if( enable) {
        return( _ISTDbgLib_UnregistVeneerInfo( nProc, vType, nVeneerAddress));
    }
    return( FALSE);
}

/*------------------------------------------------------
  �R�s�[���������e���o�b�t�@�̗]���Ɏ��܂邩�`�F�b�N����
    start : ��΃A�h���X
    size  : �R�s�[���悤�Ƃ���T�C�Y
 -----------------------------------------------------*/
static BOOL ELi_CheckBufRest( ELDesc* elElfDesc, ELObject* MYObject, void* start, u32 size)
{
    if( ((u32)start + size) > (MYObject->buf_limit_addr)) {
        ELi_SetResultCode( elElfDesc, MYObject, EL_RESULT_NO_MORE_RESOURCE);
        return( FALSE);
    }else{
        return( TRUE);
    }
}

/*------------------------------------------------------
  �x�j�����o�b�t�@�ɃR�s�[����
    start : �Ăяo����
    data : ��ѐ�
    threshold : ���͈͓̔��Ɋ��Ƀx�j��������Ύg���܂킷
 -----------------------------------------------------*/
void* ELi_CopyVeneerToBuffer( ELDesc* elElfDesc, ELObject* MYObject, u32 start, u32 data, s32 threshold)
{
#pragma unused(elElfDesc)
    u32*        el_veneer_dest;
    u32         load_start;
    Elf32_Addr  sh_size;
    ELVeneer*   elVenEnt;

    if( (elElfDesc == i_eldesc_sim)&&(data == 0)) {
    }else{
        /*--- ���łɃx�j��������΂�����g�� ---*/
        elVenEnt = ELi_GetVenEntry( &(MYObject->ELVenEntStart), data);
        if( elVenEnt != NULL) {
            if( ELi_IsFar( start, elVenEnt->adr, threshold) == FALSE) {
                PRINTDEBUG( "Veneer Hit!\n");
                return( (void*)elVenEnt->adr);
            }else{
                /*�x�j�����������Ďg���Ȃ��̂Ń����N���X�g����폜*/
                (void)ELi_RemoveVenEntry( &(MYObject->ELVenEntStart), elVenEnt);
            }
        }
        /*--------------------------------------*/
    }

    /*�A���C�������g���Ƃ�*/
    load_start = ELi_ALIGN( ((u32)(MYObject->buf_current)), 4);
    /*�T�C�Y�ݒ�*/
    sh_size = 12;//el_veneer

    /* �o�b�t�@���z���Ȃ����`�F�b�N */
    if( ELi_CheckBufRest( elElfDesc, MYObject, (void*)load_start, sh_size) == FALSE) {
        return( NULL);
    }
  
    if( elElfDesc != i_eldesc_sim) {
        /* simulation�łȂ���Ύ��ۂɃR�s�[ */
        OSAPI_CPUCOPY8( el_veneer, (u32*)load_start, sh_size);
        el_veneer_dest = (u32*)load_start;
        el_veneer_dest[2] = data;
    }

    /*�o�b�t�@�|�C���^���ړ�*/
    MYObject->buf_current = (void*)(load_start + sh_size);

    /*--- �x�j�����X�g�ɒǉ� ---*/
    elVenEnt = ELi_Malloc( elElfDesc, MYObject, sizeof( ELVeneer));
    if(elVenEnt == NULL)
        return 0;
    elVenEnt->adr = load_start;
    elVenEnt->data = data;
    ELi_AddVeneerEntry( &(MYObject->ELVenEntStart), elVenEnt);
    (void)ELi_REGISTER_VENEER_INFO( ISTDRELOCATIONPROC_AUTO, ISTDVENEERTYPE_AUTO, elVenEnt->adr, elVenEnt->data, (elElfDesc != i_eldesc_sim));
    /*--------------------------*/

    /*���[�h�����擪�A�h���X��Ԃ�*/
    return (void*)load_start;
}


/*------------------------------------------------------
  �x�j�����o�b�t�@�ɃR�s�[����
    start : �Ăяo����
    data : ��ѐ�
    threshold : ���͈͓̔��Ɋ��Ƀx�j��������Ύg���܂킷
 -----------------------------------------------------*/
void* ELi_CopyV4tVeneerToBuffer( ELDesc* elElfDesc, ELObject* MYObject, u32 start, u32 data, s32 threshold)
{
#pragma unused(elElfDesc)
    u32*        el_veneer_dest;
    u32         load_start;
    Elf32_Addr  sh_size;
    ELVeneer*   elVenEnt;

    if( (elElfDesc == i_eldesc_sim)&&(data == 0)) {
        /*--- ���łɃx�j��������΂�����g�� ---*/
        elVenEnt = ELi_GetVenEntry( &(MYObject->ELV4tVenEntStart), data);
        if( elVenEnt != NULL) {
            if( ELi_IsFar( start, elVenEnt->adr, threshold) == FALSE) {
                PRINTDEBUG( "Veneer Hit!\n");
                return( (void*)elVenEnt->adr);
            }else{
                /*�x�j�����������Ďg���Ȃ��̂Ń����N���X�g����폜*/
                (void)ELi_RemoveVenEntry( &(MYObject->ELV4tVenEntStart), elVenEnt);
            }
        }
        /*--------------------------------------*/
    }

    /*�A���C�������g���Ƃ�*/
    load_start = ELi_ALIGN( ((u32)(MYObject->buf_current)), 4);
    /*�T�C�Y�ݒ�*/
    sh_size = 20;//el_veneer_t

    /* �o�b�t�@���z���Ȃ����`�F�b�N */
    if( ELi_CheckBufRest( elElfDesc, MYObject, (void*)load_start, sh_size) == FALSE) {
        return( NULL);
    }
  
    if( elElfDesc != i_eldesc_sim) {
        /* simulation�łȂ���Ύ��ۂɃR�s�[ */
        OSAPI_CPUCOPY8( el_veneer_t, (u32*)load_start, sh_size);
        el_veneer_dest = (u32*)load_start;
        el_veneer_dest[4] = data;
    }

    /*�o�b�t�@�|�C���^���ړ�*/
    MYObject->buf_current = (void*)(load_start + sh_size);

    /*--- �x�j�����X�g�ɒǉ� ---*/
    elVenEnt = ELi_Malloc( elElfDesc, MYObject, sizeof( ELVeneer));
    if(elVenEnt == NULL)
        return 0;
    elVenEnt->adr = load_start;
    elVenEnt->data = data;
    ELi_AddVeneerEntry( &(MYObject->ELV4tVenEntStart), elVenEnt);
    (void)ELi_REGISTER_VENEER_INFO( ISTDRELOCATIONPROC_AUTO, ISTDVENEERTYPE_AUTO, elVenEnt->adr, elVenEnt->data, (elElfDesc != i_eldesc_sim));
    /*--------------------------*/

    /*���[�h�����擪�A�h���X��Ԃ�*/
    return (void*)load_start;
}


/*------------------------------------------------------
  �Z�O�����g���o�b�t�@�ɃR�s�[����
 -----------------------------------------------------*/
void* ELi_CopySegmentToBuffer( ELDesc* elElfDesc, ELObject* MYObject, Elf32_Phdr* Phdr)
{
    u32 load_start;
    
    load_start = ELi_ALIGN( Phdr->p_vaddr, Phdr->p_align);
    
    if( elElfDesc != i_eldesc_sim) {
        /* simulation�łȂ���Ύ��ۂɃR�s�[ */
        if( elElfDesc->i_elReadStub( (void*)load_start,
                                    elElfDesc->FileStruct,
                                    (u32)(elElfDesc->ar_head),
                                    (u32)(elElfDesc->elf_offset)+(u32)(Phdr->p_offset),
                                    Phdr->p_filesz) == FALSE)
        {
            ELi_SetResultCode( elElfDesc, MYObject, EL_RESULT_CANNOT_ACCESS_ELF);
            return( NULL);
        }
    }
    
    return( (void*)load_start);
}

/*------------------------------------------------------
  �Z�N�V�������o�b�t�@�ɃR�s�[����
 -----------------------------------------------------*/
void* ELi_CopySectionToBuffer( ELDesc* elElfDesc, ELObject* MYObject, Elf32_Shdr* Shdr)
{
    u32         load_start;
    Elf32_Addr  sh_size;

    /*�A���C�������g���Ƃ�*/
    load_start = ELi_ALIGN( ((u32)(MYObject->buf_current)), (Shdr->sh_addralign));
    /*�T�C�Y�ݒ�*/
    sh_size = Shdr->sh_size;

    /* �o�b�t�@���z���Ȃ����`�F�b�N */
    if( ELi_CheckBufRest( elElfDesc, MYObject, (void*)load_start, sh_size) == FALSE) {
        return( NULL);
    }

    if( elElfDesc != i_eldesc_sim) {
        /* simulation�łȂ���Ύ��ۂɃR�s�[ */
        if( elElfDesc->i_elReadStub( (void*)load_start,
                                    elElfDesc->FileStruct,
                                    (u32)(elElfDesc->ar_head),
                                    (u32)(elElfDesc->elf_offset)+(u32)(Shdr->sh_offset),
                                    sh_size) == FALSE)
        {
            ELi_SetResultCode( elElfDesc, MYObject, EL_RESULT_CANNOT_ACCESS_ELF);
            return( NULL);
        }
    }
    /*�o�b�t�@�|�C���^���ړ�*/
    MYObject->buf_current = (void*)(load_start + sh_size);

    /*���[�h�����擪�A�h���X��Ԃ�*/
    return (void*)load_start;
}


/*------------------------------------------------------
  �Z�N�V�������o�b�t�@�Ɋm�ہi�R�s�[���Ȃ��j���A
  ���̗̈��0�Ŗ��߂�
 -----------------------------------------------------*/
void* ELi_AllocSectionToBuffer( ELDesc* elElfDesc, ELObject* MYObject, Elf32_Shdr* Shdr)
{
#pragma unused(elElfDesc)
    u32         load_start;
    Elf32_Addr  sh_size;

    /*�A���C�������g���Ƃ�*/
    load_start = ELi_ALIGN( ((u32)(MYObject->buf_current)), (Shdr->sh_addralign));
    /*�T�C�Y�ݒ�*/
    sh_size = Shdr->sh_size;

    /* �o�b�t�@���z���Ȃ����`�F�b�N */
    if( ELi_CheckBufRest( elElfDesc, MYObject, (void*)load_start, sh_size) == FALSE) {
        return( NULL);
    }
  
    /*�o�b�t�@�|�C���^���ړ�*/
    MYObject->buf_current = (void*)(load_start + sh_size);

    /*0�Ŗ��߂�i.bss�Z�N�V������z�肵�Ă��邽�߁j*/
    OSAPI_CPUFILL8( (void*)load_start, 0, sh_size);
    
    /*�m�ۂ����擪�A�h���X��Ԃ�*/
    return (void*)load_start;
}


/*------------------------------------------------------
  �w��C���f�b�N�X�̃v���O�����w�b�_���o�b�t�@�Ɏ擾����
 -----------------------------------------------------*/
void ELi_GetPhdr( ELDesc* elElfDesc, u32 index, Elf32_Phdr* Phdr)
{
    u32 offset;
    
    offset = (elElfDesc->CurrentEhdr.e_phoff) +
             ((u32)(elElfDesc->CurrentEhdr.e_phentsize) * index);
    
    (void)elElfDesc->i_elReadStub( Phdr,
                             elElfDesc->FileStruct,
                             (u32)(elElfDesc->ar_head),
                             (u32)(elElfDesc->elf_offset) + offset,
                             sizeof( Elf32_Shdr));
}

/*------------------------------------------------------
  �w��C���f�b�N�X�̃Z�N�V�����w�b�_���o�b�t�@�Ɏ擾����
 -----------------------------------------------------*/
void ELi_GetShdr( ELDesc* elElfDesc, u32 index, Elf32_Shdr* Shdr)
{
    u32 offset;
    
    offset = (elElfDesc->CurrentEhdr.e_shoff) + ((u32)(elElfDesc->shentsize) * index);
    
    (void)elElfDesc->i_elReadStub( Shdr,
                             elElfDesc->FileStruct,
                             (u32)(elElfDesc->ar_head),
                             (u32)(elElfDesc->elf_offset) + offset,
                             sizeof( Elf32_Shdr));
}

/*------------------------------------------------------
  �w��C���f�b�N�X�̃Z�N�V�����̃G���g�����o�b�t�@�Ɏ擾����
 -----------------------------------------------------*/
void ELi_GetSent( ELDesc* elElfDesc, u32 index, void* entry_buf, u32 offset, u32 size)
{
    Elf32_Shdr  Shdr;

    ELi_GetShdr( elElfDesc, index, &Shdr);
    
    (void)elElfDesc->i_elReadStub( entry_buf,
                             elElfDesc->FileStruct,
                             (u32)(elElfDesc->ar_head),
                             (u32)(elElfDesc->elf_offset) + (u32)(Shdr.sh_offset) + offset,
                             size);
}


/*------------------------------------------------------
  �w��Z�N�V�����w�b�_�̎w��C���f�b�N�X�̃G���g�����o�b�t�@�Ɏ擾����
    �iRel,Rela,Sym�ȂǃG���g���T�C�Y���Œ�̃Z�N�V�����̂݁j
    Shdr : �w��Z�N�V�����w�b�_
    index : �G���g���ԍ�(0�`)
 -----------------------------------------------------*/
void ELi_GetEntry( ELDesc* elElfDesc, Elf32_Shdr* Shdr, u32 index, void* entry_buf)
{
    u32 offset;

    offset = (u32)(Shdr->sh_offset) + ((Shdr->sh_entsize) * index);
    
    (void)elElfDesc->i_elReadStub( entry_buf,
                             elElfDesc->FileStruct,
                             (u32)(elElfDesc->ar_head),
                             (u32)(elElfDesc->elf_offset) + offset,
                             Shdr->sh_entsize);
}

/*------------------------------------------------------
  STR�Z�N�V�����w�b�_�̎w��C���f�b�N�X�̕�������擾����

    Shdr : �w��Z�N�V�����w�b�_
    index : �G���g���C���f�b�N�X(0�`)
 -----------------------------------------------------*/
void ELi_GetStrAdr( ELDesc* elElfDesc, u32 strsh_index, u32 ent_index, char* str, u32 len)
{
    /*������G���g���̐擪�A�h���X*/
    ELi_GetSent( elElfDesc, strsh_index, str, ent_index, len);
}

/*------------------------------------------------------
  �V���{�����Ē�`����

    <Rel�Z�N�V�����w�b�_>
    RelShdr->sh_link : �V���{���Z�N�V�����ԍ�
    RelShdr->sh_info : �^�[�Q�b�g�Z�N�V�����ԍ�(��Frel.text��.text��\��)
    
    <Rel�G���g��>
    Rel->r_offset : �^�[�Q�b�g�Z�N�V�������̃I�t�Z�b�g�A�h���X
    ELF32_R_SYM( Rel->r_info) : �V���{���G���g���ԍ�
    ELF32_R_TYPE( Rel->r_info) : �����P�[�g�^�C�v

    <Sym�Z�N�V�����w�b�_>
    SymShdr->sh_link : �V���{���̕�����e�[�u���Z�N�V�����ԍ�
    
    <Sym�G���g��>
    Sym->st_name : �V���{���̕�����G���g���ԍ�
    Sym->st_value : �V���{���̏�������Z�N�V�������̃I�t�Z�b�g�A�h���X
    Sym->st_size : �V���{���̏�������Z�N�V�������ł̃T�C�Y
    Sym->st_shndx : �V���{���̏�������Z�N�V�����̔ԍ�
    ELF32_ST_BIND( Sym->st_info) : �o�C���h(LOCAL or GLOBAL)
    ELF32_ST_TYPE( Sym->st_info) : �^�C�v(�V���{�����֘A�t�����Ă���Ώ�)
 -----------------------------------------------------*/
BOOL ELi_RelocateSym( ELDesc* elElfDesc, ELObject* MYObject, u32 relsh_index)
{
    u32                 i;
    u32                 num_of_rel;     //�Ē�`���ׂ��V���{���̐�
    Elf32_Shdr          RelOrRelaShdr;  //REL�܂���RELA�Z�N�V�����w�b�_
    Elf32_Rela          CurrentRela;    //REL�܂���RELA�G���g���̃R�s�[��
    Elf32_Shdr*         SymShdr;
    ELSymEx*            CurrentSymEx;
    ELShdrEx*           TargetShdrEx;
    ELShdrEx*           CurrentShdrEx;
    u32                 relocation_adr;
    char                sym_str[128];
    u32                 copy_size;
    ELAdrEntry*         CurrentAdrEntry;
    u32                 sym_loaded_adr = 0;
    u32                 thumb_func_flag = 0;
    ELImportEntry       UnresolvedInfo;
    ELImportEntry*      UnrEnt;
    ELObject*           ExpObjEnt;
    u32                 unresolved_num = 0;

    /*REL or RELA�Z�N�V�����w�b�_�擾*/
    ELi_GetShdr( elElfDesc, relsh_index, &RelOrRelaShdr);

    /*�^�[�Q�b�g�Z�N�V������EX�w�b�_*/
    TargetShdrEx = ELi_GetShdrExfromList( elElfDesc->ShdrEx, RelOrRelaShdr.sh_info);
    
    num_of_rel = (RelOrRelaShdr.sh_size) / (RelOrRelaShdr.sh_entsize);    //�Ē�`���ׂ��V���{���̐�

    /*�V���{�����X�g���쐬����*/
    if(ELi_BuildSymList( elElfDesc, RelOrRelaShdr.sh_link) == FALSE)
        return FALSE;
    /*ELi_BuildSymList���ĂԂ�elElfDesc->SymShdr���Z�b�g�����*/
    SymShdr = &(elElfDesc->SymShdr);
    PRINTDEBUG( "SymShdr->link:%02x, SymShdr->info:%02x\n", SymShdr->sh_link, SymShdr->sh_info);


    /*--- �Ē�`���K�v�ȃV���{���̍Ē�` ---*/
    for( i=0; i<num_of_rel; i++) {
        
        /*- Rel�܂���Rela�G���g���擾 -*/
        ELi_GetEntry( elElfDesc, &RelOrRelaShdr, i, &CurrentRela);
        
        if( RelOrRelaShdr.sh_type == SHT_REL) {
            CurrentRela.r_addend = 0;
        }
        /*-----------------------------*/

        /*�V���{��Ex�G���g���擾(ELi_GetSymExfromTbl�ł��悢)*/
        CurrentSymEx = (ELSymEx*)(elElfDesc->SymExTbl[ELF32_R_SYM( CurrentRela.r_info)]);

//        if( CurrentSymEx->debug_flag == 1) {            /*�f�o�b�O���̏ꍇ*/
        if( CurrentSymEx == NULL) {
        }else{                                            /*�f�o�b�O���łȂ��ꍇ*/
            /**/
            ELi_InitImport( &UnresolvedInfo);
            /*����������A�h���X�i�d�l���ł����uP�v�j*/
            relocation_adr = (TargetShdrEx->loaded_adr) + (CurrentRela.r_offset);
            UnresolvedInfo.r_type = ELF32_R_TYPE( CurrentRela.r_info);
            UnresolvedInfo.A_ = (CurrentRela.r_addend);
            UnresolvedInfo.P_ = (relocation_adr);
            UnresolvedInfo.sh_type = (RelOrRelaShdr.sh_type);
            
            /*�V���{���̃A�h���X��˂��~�߂�*/
            if( CurrentSymEx->Sym.st_shndx == SHN_UNDEF) {
                /*�A�h���X�e�[�u�����猟��*/
                ELi_GetStrAdr( elElfDesc, SymShdr->sh_link, CurrentSymEx->Sym.st_name, sym_str, 128);
                CurrentAdrEntry = elGetAdrEntry( elElfDesc, sym_str, &ExpObjEnt);
              
                /*�V���{��������m��*/
                copy_size = (u32)OSAPI_STRLEN( sym_str) + 1;
                UnresolvedInfo.sym_str = ELi_Malloc( elElfDesc, MYObject, copy_size);
                if(UnresolvedInfo.sym_str == NULL)
                    return FALSE;
                OSAPI_CPUCOPY8( sym_str, UnresolvedInfo.sym_str, copy_size);

                if( CurrentAdrEntry) {    /*�A�h���X�e�[�u�����猩�������Ƃ�*/
                    sym_loaded_adr = (u32)(CurrentAdrEntry->adr);
                    /*THUMB�֐��t���O�i�d�l���ł����uT�v�jTHUMB or ARM �̔���*/
                    thumb_func_flag = CurrentAdrEntry->thumb_flag;
                    PRINTDEBUG( "\n symbol found %s : %8x\n", sym_str, sym_loaded_adr);
                }else{                    /*�A�h���X�e�[�u�����猩����Ȃ��Ƃ��͉������Ȃ�*/
                    /*�������e�[�u���ɒǉ��o�^*/
                    copy_size = sizeof( ELImportEntry);
                    UnrEnt = ELi_Malloc( elElfDesc, MYObject, copy_size);
                    if(UnrEnt == NULL)
                        return FALSE;
                    OSAPI_CPUCOPY8( &UnresolvedInfo, UnrEnt, copy_size);
                    ELi_AddImportEntry( &(MYObject->UnresolvedImportAdrEnt), UnrEnt);

                    unresolved_num++;    /*�������V���{�������J�E���g*/
                    PRINTDEBUG( "\n WARNING! cannot find symbol : %s\n", sym_str);
                }
            }else{ /* �������g�ŉ�������Ƃ� */
                /*�V���{���̏�������Z�N�V������Ex�w�b�_�擾*/
                CurrentShdrEx = ELi_GetShdrExfromList( elElfDesc->ShdrEx, CurrentSymEx->Sym.st_shndx);
                sym_loaded_adr = CurrentShdrEx->loaded_adr;
                sym_loaded_adr += CurrentSymEx->Sym.st_value;    //sym_loaded_adr�͎d�l���ł����uS�v
                /*THUMB�֐��t���O�i�d�l���ł����uT�v�jTHUMB or ARM �̔���*/
                thumb_func_flag = CurrentSymEx->thumb_flag;
                ExpObjEnt = MYObject;
            }

            if( ExpObjEnt) {        /* �N�����������G�N�X�|�[�g���Ă����ꍇ�͉������� */
                /*�d�l���Ɠ����ϐ����ɂ���*/
                UnresolvedInfo.S_ = (sym_loaded_adr);
                UnresolvedInfo.T_ = (thumb_func_flag);

                /*�����ς݃e�[�u���ɒǉ��o�^(�ʃI�u�W�F�N�g���Q�Ƃ���ꍇ�̂�)*/
                if( ExpObjEnt != MYObject) {
                    copy_size = sizeof( ELImportEntry);
                    UnrEnt = ELi_Malloc( elElfDesc, MYObject, copy_size);
                    if(UnrEnt == NULL)
                        return FALSE;
                    OSAPI_CPUCOPY8( &UnresolvedInfo, UnrEnt, copy_size);
                    UnrEnt->Dlld = (struct ELObject*)ExpObjEnt;    /* �����Ɏg�p�����I�u�W�F�N�g���L�^ */
                    ELi_AddImportEntry( &(MYObject->ResolvedImportAdrEnt), UnrEnt);
                }
              
                /*--------------- �V���{���̉���(�Ĕz�u�̎��s) ---------------*/
                /*CurrentSymEx->relocation_val =*/
                if( ELi_DoRelocate( elElfDesc, MYObject, &UnresolvedInfo) == FALSE) {
                    return( FALSE);
                }
                /*------------------------------------------------------------*/
            }
        }
    }
    /*-----------------------------------*/


    /*�V���{�����X�g���J������*/
//    ELi_FreeSymList( elElfDesc);

    
    /* �Ĕz�u������ */
    if( unresolved_num == 0) {
        ELi_SetProcCode( elElfDesc, NULL, EL_PROC_RELOCATED);
    }
    return TRUE;
}


/*------------------------------------------------------
  �O���[�o���V���{�����A�h���X�e�[�u���ɓ����

    <Sym�Z�N�V�����w�b�_>
    SymShdr->sh_link : �V���{���̕�����e�[�u���Z�N�V�����ԍ�
    
    <Sym�G���g��>
    Sym->st_name : �V���{���̕�����G���g���ԍ�
    Sym->st_value : �V���{���̏�������Z�N�V�������̃I�t�Z�b�g�A�h���X
    Sym->st_size : �V���{���̏�������Z�N�V�������ł̃T�C�Y
    Sym->st_shndx : �V���{���̏�������Z�N�V�����̔ԍ�
    ELF32_ST_BIND( Sym->st_info) : �o�C���h(LOCAL or GLOBAL)
    ELF32_ST_TYPE( Sym->st_info) : �^�C�v(�V���{�����֘A�t�����Ă���Ώ�)
 -----------------------------------------------------*/
BOOL ELi_GoPublicGlobalSym( ELDesc* elElfDesc, ELObject* MYObject, u32 symtblsh_index)
{
    Elf32_Shdr   SymShdr;
    ELSymEx*     CurrentSymEx;
    ELShdrEx*    CurrentShdrEx;
    ELAdrEntry*  ExportAdrEntry;
    ELObject*    DmyObjEnt;
    char         sym_str[128];
    u32          copy_size;
    
    /*SYMTAB�Z�N�V�����w�b�_�擾*/
    ELi_GetShdr( elElfDesc, symtblsh_index, &SymShdr);
    
    /*�V���{�����X�g���쐬����*/
    if(ELi_BuildSymList( elElfDesc, symtblsh_index) == FALSE)
        return FALSE;
    
    /*--- ���C�u��������GLOBAL�V���{�����A�h���X�e�[�u���Ɍ��J���� ---*/
    CurrentSymEx = elElfDesc->SymEx; //Tbl����łȂ��A�������炽�ǂ��debug���͊܂܂�Ȃ�
    while( CurrentSymEx != NULL) {
//        CurrentSymEx = ELi_GetSymExfromList( elElfDesc->SymExTbl, i);
//        if( CurrentSymEx != NULL) {
        /*GLOBAL�ŁA���֘A����Z�N�V���������C�u�������ɑ��݂���ꍇ*/
        if( (ELF32_ST_BIND( CurrentSymEx->Sym.st_info) == STB_GLOBAL) &&
            (CurrentSymEx->Sym.st_shndx != SHN_UNDEF)) {
            
            ExportAdrEntry = ELi_Malloc( elElfDesc, MYObject, sizeof(ELAdrEntry));    /*�������m��*/
            if(ExportAdrEntry == NULL)
                return FALSE;
            ExportAdrEntry->next = NULL;
            
            ELi_GetStrAdr( elElfDesc, SymShdr.sh_link, CurrentSymEx->Sym.st_name, sym_str, 128);
            copy_size = (u32)OSAPI_STRLEN( sym_str) + 1;
            ExportAdrEntry->name = ELi_Malloc( elElfDesc, MYObject, copy_size);
            if(ExportAdrEntry->name == NULL)
                return FALSE;
            OSAPI_CPUCOPY8( sym_str, ExportAdrEntry->name, copy_size);

            CurrentShdrEx = ELi_GetShdrExfromList( elElfDesc->ShdrEx, CurrentSymEx->Sym.st_shndx);
            //Sym.st_value�͋���/���ARM/Thumb�𔻕ʂł���悤�ɒ�������Ă���ꍇ������̂ŁA���̒������폜���Đ����̒l���o��
            ExportAdrEntry->adr = (void*)(CurrentShdrEx->loaded_adr + ((CurrentSymEx->Sym.st_value)&0xFFFFFFFE));
            ExportAdrEntry->func_flag = (u16)(ELF32_ST_TYPE( CurrentSymEx->Sym.st_info));
            ExportAdrEntry->thumb_flag = CurrentSymEx->thumb_flag;

            if( elGetAdrEntry( elElfDesc, ExportAdrEntry->name, &DmyObjEnt) == NULL) {    //�����ĂȂ�������
                elAddAdrEntry( &(MYObject->ExportAdrEnt), ExportAdrEntry);    /*�o�^*/
            }
        }
        CurrentSymEx = CurrentSymEx->next;
//        }
    }
    /*----------------------------------------------------------------*/

    /*�V���{�����X�g���J������*/
//    ELi_FreeSymList( elElfDesc);
    return TRUE;
}


/*------------------------------------------------------
  �V���{�����X�g���쐬����

 -----------------------------------------------------*/
static BOOL ELi_BuildSymList( ELDesc* elElfDesc, u32 symsh_index)
{
    u32         i;
    u32         num_of_sym;        //�V���{���̑S�̐�
    u16         debug_flag;
    Elf32_Sym   TestSym;
    ELSymEx*    CurrentSymEx;
    ELShdrEx*   CurrentShdrEx;
    Elf32_Shdr* SymShdr;
    ELSymEx     DmySymEx;

    if( elElfDesc->SymExTarget == symsh_index) {
        PRINTDEBUG( "%s skip.\n", __FUNCTION__);
        return TRUE;                              //���Ƀ��X�g�쐬�ς�
    }else{
        ELi_FreeSymList( elElfDesc); /*�V���{�����X�g���J������*/
    }
    
    PRINTDEBUG( "%s build\n", __FUNCTION__);
    
    /*SYMTAB�Z�N�V�����w�b�_�擾*/
    ELi_GetShdr( elElfDesc, symsh_index, &(elElfDesc->SymShdr));
    SymShdr = &(elElfDesc->SymShdr);
    
    num_of_sym = (SymShdr->sh_size) / (SymShdr->sh_entsize);    //�V���{���̑S�̐�
    elElfDesc->SymExTbl = ELi_Malloc( elElfDesc, NULL, num_of_sym * 4);
    if(elElfDesc->SymExTbl == NULL)
        return FALSE;
    /*---------- ELSymEx�̃��X�g����� ----------*/
    CurrentSymEx = &DmySymEx;
    for( i=0; i<num_of_sym; i++) {
        
        /*�V���{���G���g�����R�s�[*/
        ELi_GetEntry( elElfDesc, (Elf32_Shdr*)SymShdr, i, &TestSym);
        /*-- �f�o�b�O���t���O���Z�b�g --*/
        CurrentShdrEx = ELi_GetShdrExfromList( elElfDesc->ShdrEx, TestSym.st_shndx);
        if( CurrentShdrEx) {
            debug_flag = CurrentShdrEx->debug_flag;
        }else{
            debug_flag = 0;
        }/*-------------------------------*/

        if( debug_flag == 1) {
            elElfDesc->SymExTbl[i] = NULL;
        }else{
            CurrentSymEx->next = ELi_Malloc( elElfDesc, NULL, sizeof(ELSymEx));
            if(CurrentSymEx->next == NULL)
                return FALSE;
            CurrentSymEx = (ELSymEx*)(CurrentSymEx->next);
            
            OSAPI_CPUCOPY8( &TestSym, &(CurrentSymEx->Sym), sizeof(TestSym));
            
            elElfDesc->SymExTbl[i] = CurrentSymEx;
            
            PRINTDEBUG( "sym_no: %02x ... st_shndx: %04x\n", i, CurrentSymEx->Sym.st_shndx);
        }
    }
    
    CurrentSymEx->next = NULL;
    elElfDesc->SymEx = DmySymEx.next;
    /*-------------------------------------------*/
    

    /*-------- ARM or Thumb ����(ELi_CodeIsThumb��SymEx���o���ĂȂ��Ɠ����Ȃ�����) --------*/
    CurrentSymEx = elElfDesc->SymEx;
    while( CurrentSymEx != NULL) { //�������炽�ǂ��debug���͖���
        /*-- ELSymEx��Thumb�t���O���Z�b�g�i�֐��V���{�������K�v�j--*/
        if( ELF32_ST_TYPE( CurrentSymEx->Sym.st_info) == STT_FUNC) {
            CurrentSymEx->thumb_flag = (u16)(ELi_CodeIsThumb( elElfDesc,
                                                              CurrentSymEx->Sym.st_shndx,
                                                              CurrentSymEx->Sym.st_value));
        }else{
            CurrentSymEx->thumb_flag = 0;
        }/*--------------------------------------------------------*/
        
        CurrentSymEx = CurrentSymEx->next;
    }
    /*-------------------------------------------------------------------------------------*/


    elElfDesc->SymExTarget = symsh_index;
    return TRUE;
}


/*------------------------------------------------------
  �V���{�����X�g���J������

 -----------------------------------------------------*/
void ELi_FreeSymList( ELDesc* elElfDesc)
{
    ELSymEx*  CurrentSymEx;
    ELSymEx*  FwdSymEx;

    /*--- �܂�SymExTbl���J������ ---*/
    if( elElfDesc->SymExTbl != NULL) {
        OSAPI_FREE( elElfDesc->SymExTbl);
        elElfDesc->SymExTbl = NULL;
    }
    
    /*------- ELSymEx�̃��X�g��������� -------*/
    CurrentSymEx = elElfDesc->SymEx;
    if( CurrentSymEx) {
        do {
            FwdSymEx = CurrentSymEx;
            CurrentSymEx = CurrentSymEx->next;
            OSAPI_FREE( FwdSymEx);
        }while( CurrentSymEx != NULL);
        elElfDesc->SymEx = NULL;
    }
    /*-----------------------------------------*/

    elElfDesc->SymExTarget = 0xFFFFFFFF;
}


/*------------------------------------------------------
  �������������ƂɃV���{������������

    r_type,S_,A_,P_,T_���S�ĕ������Ă���K�v������B
 -----------------------------------------------------*/
#define _S_    (UnresolvedInfo->S_)
#define _A_    (UnresolvedInfo->A_)
#define _P_    (UnresolvedInfo->P_)
#define _T_    (UnresolvedInfo->T_)
BOOL    ELi_DoRelocate( ELDesc* elElfDesc, ELObject* MYObject, ELImportEntry* UnresolvedInfo)
{
    s32    signed_val;
    u32    relocation_val = 0;
    u32    relocation_adr;
    BOOL   ret_val;
    BOOL   veneer_flag = TRUE;

    ret_val = TRUE;
    relocation_adr = _P_;

    switch( (UnresolvedInfo->r_type)) {
      case R_ARM_PC24:
      case R_ARM_PLT32:
      case R_ARM_CALL:
      case R_ARM_JUMP24:
        if( UnresolvedInfo->sh_type == SHT_REL) {
            _A_ = (s32)(((*(vu32*)relocation_adr)|0xFF800000) << 2);    //��ʓI�ɂ�-8�ɂȂ��Ă���͂�
        }
        /*----------------- veneer -----------------*/
        if( (elElfDesc == i_eldesc_sim)&&(_S_ == 0)) {
            veneer_flag = TRUE; //simulation�ŉ����s�\�ȏꍇ��veneer�O��Ƃ���
        }else{
            veneer_flag = ELi_IsFar( _P_, _S_|_T_, 0x2000000); //+-32MBytes����?
        }
#if (TARGET_ARM_V5 == 1)
        if( veneer_flag == TRUE) {
#else //(TARGET_ARM_V4 == 1)
        //ARM->Thumb�����ARM->ARM�Ń����O�u�����`�̂Ƃ��� veneer �g�p
        if( (_T_) || (veneer_flag == TRUE)) {
#endif
            //_A_��PC���Ε���̂Ƃ��̒����l�Ȃ̂ŁA��Ε���̏ꍇ�͊֌W�Ȃ�
            _S_ = (u32)ELi_CopyVeneerToBuffer( elElfDesc,
                                               MYObject,
                                               _P_,
                                               (u32)(( (s32)(_S_) /*+ _A_*/) | (s32)(_T_)),
                                               0x2000000);
            if(_S_ == 0)
                return FALSE;
            _T_ = 0; //veneer��ARM�R�[�h�̂���
        }/*-----------------------------------------*/
        if( elElfDesc == i_eldesc_sim) {  //simulation���͎��ۂɃV���{���Ē�`���Ȃ�
            break;
        }
        signed_val = (( (s32)(_S_) + _A_) | (s32)(_T_)) - (s32)(_P_);
        if( _T_) {        /*BLX���߂�ARM����Thumb�ɔ��(v5��������BL���x�j�A��BX�Ƃ����d�g�݂��K�v)*/
            relocation_val = (0xFA000000) | ((signed_val>>2) & 0x00FFFFFF) | (((signed_val>>1) & 0x1)<<24);
        }else{            /*BL���߂�ARM����ARM�ɔ��*/
            signed_val >>= 2;
            relocation_val = (*(vu32*)relocation_adr & 0xFF000000) | (signed_val & 0x00FFFFFF);
        }
        *(vu32*)relocation_adr = relocation_val;
        break;
      case R_ARM_ABS32:
        if( elElfDesc == i_eldesc_sim) { //simulation���͎��ۂɃV���{���Ē�`���Ȃ�
            break;
        }
        relocation_val = (( _S_ + _A_) | _T_);
        *(vu32*)relocation_adr = relocation_val;
        break;
      case R_ARM_REL32:
      case R_ARM_PREL31:
        if( elElfDesc == i_eldesc_sim) { //simulation���͎��ۂɃV���{���Ē�`���Ȃ�
            break;
        }
        relocation_val = (( _S_ + _A_) | _T_) - _P_;
        *(vu32*)relocation_adr = relocation_val;
        break;
      case R_ARM_LDR_PC_G0:
        /*----------------- veneer -----------------*/
        if( (elElfDesc == i_eldesc_sim)&&(_S_ == 0)) {
            veneer_flag = TRUE; //simulation�ŉ����s�\�ȏꍇ��veneer�O��Ƃ���
        }else{
            veneer_flag = ELi_IsFar( _P_, _S_|_T_, 0x2000000); //+-32MBytes����?
        }
#if (TARGET_ARM_V5 == 1)
        if( veneer_flag == TRUE) {
#else //(TARGET_ARM_V4 == 1)
        //ARM->Thumb�����ARM->ARM�Ń����O�u�����`�̂Ƃ��� veneer �g�p
        if( (_T_) || (veneer_flag == TRUE)) {
#endif
            //_A_��PC���Ε���̂Ƃ��̒����l�Ȃ̂ŁA��Ε���̏ꍇ�͊֌W�Ȃ�
            _S_ = (u32)ELi_CopyVeneerToBuffer( elElfDesc,
                                               MYObject,
                                               _P_,
                                               (u32)(( (s32)(_S_) /*+ _A_*/) | (s32)(_T_)),
                                               0x2000000);
            if(_S_ == 0)
                return FALSE;
            _T_ = 0; //veneer��ARM�R�[�h�̂���
        }/*-----------------------------------------*/
        if( elElfDesc == i_eldesc_sim) {  //simulation���͎��ۂɃV���{���Ē�`���Ȃ�
            break;
        }
        signed_val = ( (s32)(_S_) + _A_) - (s32)(_P_);
        signed_val >>= 2;
        relocation_val = (*(vu32*)relocation_adr & 0xFF000000) | (signed_val & 0x00FFFFFF);
        *(vu32*)relocation_adr = relocation_val;
        break;
      case R_ARM_ABS16:
      case R_ARM_ABS12:
      case R_ARM_THM_ABS5:
      case R_ARM_ABS8:
        if( elElfDesc == i_eldesc_sim) { //simulation���͎��ۂɃV���{���Ē�`���Ȃ�
            break;
        }
        relocation_val = _S_ + _A_;
        *(vu32*)relocation_adr = relocation_val;
        break;
      case R_ARM_THM_PC22:/*�ʖ��FR_ARM_THM_CALL*/
        if( UnresolvedInfo->sh_type == SHT_REL) {
            _A_ = (s32)(((*(vu16*)relocation_adr & 0x07FF)<<11) + ((*((vu16*)(relocation_adr)+1)) & 0x07FF));
            _A_ = (s32)((_A_ | 0xFFC00000) << 1);    //��ʓI�ɂ�-4�ɂȂ��Ă���͂�(PC�͌����߃A�h���X+4�Ȃ̂�)
        }
        /*----------------- veneer -----------------*/
        if( (elElfDesc == i_eldesc_sim)&&(_S_ == 0)) {
            veneer_flag = TRUE; //simulation�ŉ����s�\�ȏꍇ��veneer�O��Ƃ���
        }else{
            veneer_flag = ELi_IsFar( _P_, _S_|_T_, 0x400000); //+-4MBytes����?
        }
#if (TARGET_ARM_V5 == 1)
        if( veneer_flag == TRUE) {
            //_A_��PC���Ε���̂Ƃ��̒����l�Ȃ̂ŁA��Ε���̏ꍇ�͊֌W�Ȃ�
            _S_ = (u32)ELi_CopyVeneerToBuffer( elElfDesc,
                                               MYObject,
                                               _P_,
                                               (u32)(( (s32)(_S_) /*+ _A_*/) | (s32)(_T_)),
                                               0x400000);
            if(_S_ == 0)
                return FALSE;
            _T_ = 0; //veneer��ARM�R�[�h�̂���
        }/*-----------------------------------------*/
#else //(TARGET_ARM_V4 == 1)
        /*----------------- v4t veneer -----------------*/
        //Thumb->ARM�����Thumb->Thumb�Ń����O�u�����`�̂Ƃ��� v4T veneer �g�p
        if( (_T_ == 0) || (veneer_flag == TRUE)) {
            _S_ = (u32)ELi_CopyV4tVeneerToBuffer( elElfDesc,
                                                  MYObject,
                                                  _P_,
                                                  (u32)(( (s32)(_S_)) | (s32)(_T_)),
                                                  0x400000);
            if(_S_ == 0)
                return FALSE;
            _T_ = 1; //v4t veneer��Thumb�R�[�h�̂���
        }
        /*---------------------------------------------*/
#endif
        if( elElfDesc == i_eldesc_sim) { //simulation���͎��ۂɃV���{���Ē�`���Ȃ�
            break;
        }
        signed_val = (( (s32)(_S_) + _A_) | (s32)(_T_)) - (s32)(_P_);
        signed_val >>= 1;
        if( _T_) {    /*BL���߂�Thumb����Thumb�ɔ��*/
            relocation_val = (u32)(*(vu16*)relocation_adr & 0xF800) | ((signed_val>>11) & 0x07FF) +
                                   ((((*((vu16*)(relocation_adr)+1)) & 0xF800) | (signed_val & 0x07FF)) << 16);
        }else{        /*BLX���߂�Thumb����ARM�ɔ��(v5��������BL���x�j�A��BX�Ƃ����d�g�݂��K�v)*/
            if( (signed_val & 0x1)) {    //_P_��4�o�C�g�A���C������Ă��Ȃ��Ƃ����ɗ���
                signed_val += 1;
            }
            relocation_val = (u32)(*(vu16*)relocation_adr & 0xF800) | ((signed_val>>11) & 0x07FF) +
                                   ((((*((vu16*)(relocation_adr)+1)) & 0xE800) | (signed_val & 0x07FF)) << 16);
        }
        *(vu16*)relocation_adr = (vu16)relocation_val;
        *((vu16*)relocation_adr+1) = (vu16)((u32)(relocation_val) >> 16);
        break;
      case R_ARM_NONE:
        /*R_ARM_NONE�̓^�[�Q�b�g�̃V���{����ێ����A�����J�Ƀf�b�h�X�g���b�v
          ����Ȃ��悤�ɂ��܂�*/
        break;
      case R_ARM_THM_JUMP24:
      default:
        ELi_SetResultCode( elElfDesc, MYObject, EL_RESULT_UNSUPPORTED_ELF);
        ret_val = FALSE;
        PRINTDEBUG( "ERROR! : unsupported relocation type (0x%x)!\n", (UnresolvedInfo->r_type));
        PRINTDEBUG( "S = 0x%x\n", _S_);
        PRINTDEBUG( "A = 0x%x\n", _A_);
        PRINTDEBUG( "P = 0x%x\n", _P_);
        PRINTDEBUG( "T = 0x%x\n", _T_);
        break;
    }
    
    return ret_val;//relocation_val;
}
#undef _S_
#undef _A_
#undef _P_
#undef _T_

/*------------------------------------------------------
  ���X�g����w��C���f�b�N�X��ELSymEx�����o��
 -----------------------------------------------------*/
static ELSymEx* ELi_GetSymExfromList( ELSymEx* SymExStart, u32 index)
{
    u32         i;
    ELSymEx*    SymEx;

    SymEx = SymExStart;
    for( i=0; i<index; i++) {
        SymEx = (ELSymEx*)(SymEx->next);
        if( SymEx == NULL) {
            break;
        }
    }
    return SymEx;
}

/*------------------------------------------------------
  �e�[�u������w��C���f�b�N�X��ELSymEx�����o��
 -----------------------------------------------------*/
static ELSymEx* ELi_GetSymExfromTbl( ELSymEx** SymExTbl, u32 index)
{
    return( (ELSymEx*)(SymExTbl[index]));
}

/*------------------------------------------------------
  ���X�g����w��C���f�b�N�X��ELShdrEx�����o��
 -----------------------------------------------------*/
ELShdrEx* ELi_GetShdrExfromList( ELShdrEx* ShdrExStart, u32 index)
{
    u32         i;
    ELShdrEx*   ShdrEx;

    ShdrEx = ShdrExStart;
    for( i=0; i<index; i++) {
        ShdrEx = (ELShdrEx*)(ShdrEx->next);
        if( ShdrEx == NULL) {
            break;
        }
    }
    return ShdrEx;
}



/*------------------------------------------------------
  �w��C���f�b�N�X�̃Z�N�V�������f�o�b�O��񂩂ǂ������肷��

���f�o�b�O���̒�`��
�E�Z�N�V��������".debug"����n�܂�Z�N�V����

�E.rel.debug�` �Z�N�V�����ȂǁAsh_info ���f�o�b�O���Z�N�V�����ԍ���
�@�����Ă���Z�N�V����
 -----------------------------------------------------*/
BOOL ELi_ShdrIsDebug( ELDesc* elElfDesc, u32 index)
{
    Elf32_Shdr  TmpShdr;
    char        shstr[6];

    /*-- �Z�N�V�������̕������6�����擾 --*/
    ELi_GetShdr( elElfDesc, index, &TmpShdr);
    ELi_GetStrAdr( elElfDesc, elElfDesc->CurrentEhdr.e_shstrndx,
                   TmpShdr.sh_name, shstr, 6);
    /*-------------------------------------*/
    
    if( OSAPI_STRNCMP( shstr, ".debug", 6) == 0) {    /*�f�o�b�O�Z�N�V�����̏ꍇ*/
        return TRUE;
    }else{                        /*�f�o�b�O�Z�N�V�����Ɋւ���Ĕz�u�Z�N�V�����̏ꍇ*/
        if( (TmpShdr.sh_type == SHT_REL) || (TmpShdr.sh_type == SHT_RELA)) {
            if( ELi_ShdrIsDebug( elElfDesc, TmpShdr.sh_info) == TRUE) {
                return TRUE;
            }
        }
    }

    return FALSE;
}



/*------------------------------------------------------
  elElfDesc��SymEx�e�[�u���𒲂ׁA�w��C���f�b�N�X��
�@�w��I�t�Z�b�g�ɂ���R�[�h��ARM��THUMB���𔻒肷��B
  �i�\�� elElfDesc->SymShdr �� elElfDesc->SymEx ��ݒ肵�Ă������Ɓj
    
    sh_index : ���ׂ����Z�N�V�����C���f�b�N�X
    offset : ���ׂ����Z�N�V�������̃I�t�Z�b�g
 -----------------------------------------------------*/
u32 ELi_CodeIsThumb( ELDesc* elElfDesc, u16 sh_index, u32 offset)
{
    u32            i;
    u32            thumb_flag;
    Elf32_Shdr*    SymShdr;
    char           str_adr[3];
    ELSymEx*       CurrentSymEx;

    /*�V���{���̃Z�N�V�����w�b�_��SymEx���X�g�擾*/
    SymShdr = &(elElfDesc->SymShdr);
    CurrentSymEx = elElfDesc->SymEx;

    i = 0;
    thumb_flag = 0;
    while( CurrentSymEx != NULL) {
        
        if( CurrentSymEx->Sym.st_shndx == sh_index) {
            ELi_GetStrAdr( elElfDesc, SymShdr->sh_link, CurrentSymEx->Sym.st_name, str_adr, 3);
            if( OSAPI_STRNCMP( str_adr, "$a\0", OSAPI_STRLEN("$a\0")) == 0) {
                thumb_flag = 0;
            }else if( OSAPI_STRNCMP( str_adr, "$t\0", OSAPI_STRLEN("$t\0")) == 0) {
                thumb_flag = 1;
            }
            if( CurrentSymEx->Sym.st_value > offset) {
                break;
            }
        }
        
        CurrentSymEx = CurrentSymEx->next;
        i++;
    }

    return thumb_flag;
}


/*---------------------------------------------------------
 �C���|�[�g���G���g��������������
 --------------------------------------------------------*/
static void ELi_InitImport( ELImportEntry* ImportInfo)
{
    ImportInfo->sym_str = NULL;
    ImportInfo->r_type = 0;
    ImportInfo->S_ = 0;
    ImportInfo->A_ = 0;
    ImportInfo->P_ = 0;
    ImportInfo->T_ = 0;
    ImportInfo->Dlld = NULL;
}

/*------------------------------------------------------
  �C���|�[�g���e�[�u������G���g���𔲂��o���iImpEnt�͍폜���Ȃ��I�j
 -----------------------------------------------------*/
BOOL ELi_ExtractImportEntry( ELImportEntry** StartEnt, ELImportEntry* ImpEnt)
{
    ELImportEntry  DmyImpEnt;
    ELImportEntry* CurImpEnt;

    DmyImpEnt.next = (*StartEnt);
    CurImpEnt      = &DmyImpEnt;

    while( CurImpEnt->next != ImpEnt) {
        if( CurImpEnt->next == NULL) {
            return FALSE;
        }else{
            CurImpEnt = (ELImportEntry*)CurImpEnt->next;
        }
    }

    /*�����N���X�g�̌q������*/
    CurImpEnt->next = ImpEnt->next;
    (*StartEnt) = DmyImpEnt.next;

    /*�J��*/
//    OSAPI_FREE( ImpEnt);
    
    return TRUE;
}

/*---------------------------------------------------------
 �C���|�[�g���e�[�u���ɃG���g����ǉ�����
 --------------------------------------------------------*/
void ELi_AddImportEntry( ELImportEntry** ELUnrEntStart, ELImportEntry* UnrEnt)
{
    ELImportEntry    DmyUnrEnt;
    ELImportEntry*   CurrentUnrEnt;

    if( !(*ELUnrEntStart)) {
        (*ELUnrEntStart) = UnrEnt;
    }else{
        DmyUnrEnt.next = (*ELUnrEntStart);
        CurrentUnrEnt = &DmyUnrEnt;

        while( CurrentUnrEnt->next != NULL) {
            CurrentUnrEnt = CurrentUnrEnt->next;
        }
        CurrentUnrEnt->next = (void*)UnrEnt;
    }
    UnrEnt->next = NULL;
}

/*------------------------------------------------------
  �C���|�[�g���e�[�u����S���������
 -----------------------------------------------------*/
void ELi_FreeImportTbl( ELImportEntry** ELImpEntStart)
{
    ELImportEntry*    FwdImpEnt;
    ELImportEntry*    CurrentImpEnt;
    
    /*--- ELImportEntry�̃��X�g��������� ---*/
    CurrentImpEnt = (*ELImpEntStart);
    if( CurrentImpEnt) {
        do {
            FwdImpEnt = CurrentImpEnt;
            CurrentImpEnt = CurrentImpEnt->next;
            OSAPI_FREE( FwdImpEnt->sym_str);        //�V���{����������
            OSAPI_FREE( FwdImpEnt);              //�\���̎��g
        }while( CurrentImpEnt != NULL);
        (*ELImpEntStart) = NULL;
    }
    /*------------------------------------*/
}


/*---------------------------------------------------------
 �x�j���e�[�u���ɃG���g����ǉ�����
 --------------------------------------------------------*/
static void ELi_AddVeneerEntry( ELVeneer** start, ELVeneer* VenEnt)
{
    ELVeneer    DmyVenEnt;
    ELVeneer*   CurrentVenEnt;

    if( !(*start)) {
        (*start) = VenEnt;
    }else{
        DmyVenEnt.next = (*start);
        CurrentVenEnt = &DmyVenEnt;

        while( CurrentVenEnt->next != NULL) {
            CurrentVenEnt = CurrentVenEnt->next;
        }
        CurrentVenEnt->next = (void*)VenEnt;
    }
    VenEnt->next = NULL;
}

/*------------------------------------------------------
  �x�j���e�[�u������G���g�����폜����
 -----------------------------------------------------*/
static BOOL ELi_RemoveVenEntry( ELVeneer** start, ELVeneer* VenEnt)
{
    ELVeneer  DmyVenEnt;
    ELVeneer* CurrentVenEnt;

    DmyVenEnt.next = (*start);
    CurrentVenEnt = &DmyVenEnt;

    while( CurrentVenEnt->next != VenEnt) {
        if( CurrentVenEnt->next == NULL) {
            return FALSE;
        }else{
            CurrentVenEnt = (ELVeneer*)CurrentVenEnt->next;
        }
    }

    /*�����N���X�g�̌q������*/
    CurrentVenEnt->next = VenEnt->next;
    (*start) = DmyVenEnt.next;

    /*�J��*/
    OSAPI_FREE( VenEnt);
    
     return TRUE;
}

/*------------------------------------------------------
  �x�j���e�[�u������w��f�[�^�ɊY������G���g����T��
 -----------------------------------------------------*/
static ELVeneer* ELi_GetVenEntry( ELVeneer** start, u32 data)
{
    ELVeneer* CurrentVenEnt;

    CurrentVenEnt = (*start);
    if( CurrentVenEnt == NULL) {
        return NULL;
    }
    while( CurrentVenEnt->data != data) {
        CurrentVenEnt = (ELVeneer*)CurrentVenEnt->next;
        if( CurrentVenEnt == NULL) {
            break;
        }
    }
    return CurrentVenEnt;
}

/*------------------------------------------------------
  �x�j���e�[�u�����������
 -----------------------------------------------------*/
void* ELi_FreeVenTbl( ELDesc* elElfDesc, ELObject* MYObject)
{
    ELVeneer*    FwdVenEnt;
    ELVeneer*    CurrentVenEnt;
    
    /*--- ELVenEntry�̃��X�g��������� ---*/
    CurrentVenEnt = MYObject->ELVenEntStart;
    while( CurrentVenEnt != NULL) {
        FwdVenEnt = CurrentVenEnt;
        CurrentVenEnt = CurrentVenEnt->next;
        (void)ELi_UNREGISTER_VENEER_INFO( ISTDRELOCATIONPROC_AUTO, ISTDVENEERTYPE_AUTO, FwdVenEnt->adr, (elElfDesc != i_eldesc_sim));
        OSAPI_FREE( FwdVenEnt);              //�\���̎��g
    }
    MYObject->ELVenEntStart = NULL;
    /*------------------------------------*/

    /*--- ELVenEntry�̃��X�g��������� ---*/
    CurrentVenEnt = MYObject->ELV4tVenEntStart;
    while( CurrentVenEnt != NULL) {
        FwdVenEnt = CurrentVenEnt;
        CurrentVenEnt = CurrentVenEnt->next;
        (void)ELi_UNREGISTER_VENEER_INFO( ISTDRELOCATIONPROC_AUTO, ISTDVENEERTYPE_AUTO, FwdVenEnt->adr, (elElfDesc != i_eldesc_sim));
        OSAPI_FREE( FwdVenEnt);              //�\���̎��g
    }
    MYObject->ELV4tVenEntStart = NULL;
    /*------------------------------------*/
    
    return NULL;
}

/*------------------------------------------------------
  2�̃A�h���X�l�̍��� (+threshold)�`(-threshold)����
  ���܂��Ă��邩�𒲂ׂ�
 -----------------------------------------------------*/
static BOOL ELi_IsFar( u32 P, u32 S, s32 threshold)
{
    s32 diff;

    diff = (s32)S - (s32)(P);
    if( (diff < threshold)&&( diff > -threshold)) {
        return( FALSE); //Near
    }else{
        return( TRUE);  //Far
    }
}

