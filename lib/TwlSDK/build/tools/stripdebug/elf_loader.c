/*---------------------------------------------------------------------------*
  Project:  TwlSDK - ELF Loader
  File:     elf_loader.c

  Copyright 2006-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include "types.h"
#include "elf.h"
#include "elf_loader.h"
#include "arch.h"
#include "loader_subset.h"
#include <string.h>
#include <stdlib.h>


//OSHeapHandle        EL_Heap;
ELAdrEntry*           ELAdrEntStart = NULL;
ELUnresolvedEntry*    ELUnrEntStart = NULL;

extern u16 dbg_print_flag;
extern u32 unresolved_table_block_flag;
extern u32 ELi_ALIGN( u32 addr, u32 align_size);

extern u16 ELi_LoadObject2( ELHandle* ElfHandle, void* obj_offset, void* buf);

     
#define MAKELST_DS_API        "    elAddAdrEntry"    //DS上でアドレステーブルに追加するAPI関数名

extern char    c_source_line_str[256];
extern FILE*   NewElfFilep;


/*------------------------------------------------------
  ローカル関数の宣言
 -----------------------------------------------------*/
// ELFオブジェクトまたはそのアーカイブをバッファに再配置する
u16 ELi_LoadLibrary( ELHandle* ElfHandle, void* obj_image, u32 obj_len, void* buf, u16 type);
// ELFオブジェクトをバッファに再配置するコア関数
u16 ELi_LoadObject( ELHandle* ElfHandle, void* obj_offset, void* buf);
// ELFオブジェクトからデータを読み出すスタブ関数
void ELi_ReadFile( void* buf, void* file_struct, u32 file_base, u32 file_offset, u32 size);
void ELi_ReadMem( void* buf, void* file_struct, u32 file_base, u32 file_offset, u32 size);



/*hex値を10進の文字列で表現する*/
void ELi_SetDecSize( char* dec, u32 hex)
{
    u16 i;
    u32 tmp = 1000000000;
    u16 ptr = 0;
  
    memset( dec, 0x20, 10);
    for( i=0; i<10; i++) {
        if( ((hex / tmp) != 0) || (ptr != 0)) {
            dec[ptr++] = 0x30 + (hex/tmp);
            hex -= (hex/tmp) * tmp;
        }
        tmp /= 10;
    }
}



/*---------------------------------------------------------
 ELFオブジェクトのサイズを求める
    
    buf : ELFイメージのアドレス
 --------------------------------------------------------*/
u32 EL_GetElfSize( const void* buf)
{
    Elf32_Ehdr    Ehdr;
    u32           size;
    
    if( ELF_LoadELFHeader( buf, &Ehdr) == NULL) {
        return 0;
    }
    size = (u32)(Ehdr.e_shoff + (Ehdr.e_shentsize * Ehdr.e_shnum));
    return size;
}


/*------------------------------------------------------
  ダイナミックリンクシステムを初期化する
 -----------------------------------------------------*/
void EL_Init( void)
{
//    void* heap_start;
    printf( "\n");
    /*--- メモリアロケーション関係の設定 ---*/
/*    OS_InitArena();
    heap_start = OS_InitAlloc( OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetMainArenaLo( heap_start );
    EL_Heap = OS_CreateHeap( OS_ARENA_MAIN, heap_start, (void*)((u32)(OS_GetMainArenaHi())+1));
    OS_SetCurrentHeap( OS_ARENA_MAIN, EL_Heap);*/
    /*--------------------------------------*/
}

/*------------------------------------------------------
  ELHandle構造体を初期化する
 -----------------------------------------------------*/
BOOL EL_InitHandle( ELHandle* ElfHandle)
{
    if( ElfHandle == NULL) {    /*NULLチェック*/
        return FALSE;
    }

    /*初期値の設定*/
    ElfHandle->ShdrEx   = NULL;
    ElfHandle->SymEx    = NULL;
    ElfHandle->SymExTbl = NULL;
    ElfHandle->SymExTarget = 0xFFFFFFFF;

    ElfHandle->process = EL_INITIALIZED;    /*フラグの設定*/

    return TRUE;
}

/*------------------------------------------------------
  ELFオブジェクトまたはそのアーカイブをバッファに再配置する
    
    ElfHandle : ヘッダ構造体
    ObjFile : OBJファイルまたはアーカイブファイルの構造体
    buf : ロードするバッファ
    type : 0:デバッグ情報削除版elf作成、1:デバッガ用elf作成
 -----------------------------------------------------*/
u16 EL_LoadLibraryfromFile( ELHandle* ElfHandle, FILE* ObjFile, void* buf, u16 type)
{
    u16 result;
    u32 len;

    /*リード関数の設定*/
    ElfHandle->ELi_ReadStub = ELi_ReadFile;
    ElfHandle->FileStruct = ObjFile;

    fseek( ObjFile, 0, SEEK_END);
    len = ftell( ObjFile);
    fseek( ObjFile, 0, SEEK_SET);
    result = ELi_LoadLibrary( ElfHandle, NULL, len, buf, type);

    return result;
}

/*------------------------------------------------------
  ELFオブジェクトまたはそのアーカイブをバッファに再配置する
    
    ElfHandle : ヘッダ構造体
    obj_image : OBJファイルまたはアーカイブファイルのRAM上イメージアドレス
    buf : ロードするバッファ
 -----------------------------------------------------*/
u16 EL_LoadLibraryfromMem( ELHandle* ElfHandle, void* obj_image, u32 obj_len, void* buf)
{
    u16 result;
    
    /*リード関数の設定*/
    ElfHandle->ELi_ReadStub = ELi_ReadMem;
    ElfHandle->FileStruct = NULL;
    
    result = ELi_LoadLibrary( ElfHandle, obj_image, obj_len, buf, 0);

    return result;
}

/*------------------------------------------------------
  ELFオブジェクトまたはそのアーカイブをバッファに再配置する
    
    ElfHandle : ヘッダ構造体
    obj_image : OBJファイルまたはアーカイブファイルのRAM上イメージアドレス
    buf : ロードするバッファ
 -----------------------------------------------------*/
u16 ELi_LoadLibrary( ELHandle* ElfHandle, void* obj_image, u32 obj_len, void* buf, u16 type)
{
    u16     result, all_result;
    u32     image_pointer;
    u32     new_image_pointer;
    u32     arch_size;
    u32     elf_num = 0;                /*ELFオブジェクトの数*/
    u32     newarch_size = 0;
    ArchHdr ArHdr;
    char    OBJMAG[8];
    char    ELFMAG[4] = { ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};


    all_result = EL_RELOCATED;
    ElfHandle->ar_head = obj_image;
    image_pointer = 0;

    
    ElfHandle->ELi_ReadStub( OBJMAG, ElfHandle->FileStruct, (u32)obj_image, 0, 8);    /*OBJの文字列を取得*/
    /*--------------- アーカイブファイルの場合 ---------------*/
    if( strncmp( OBJMAG, ARMAG, 8) == 0) {
        arch_size = sizeof( ArchHdr);
        image_pointer += 8;                /*最初のエントリへ*/


        new_image_pointer = (u32)buf;
        memcpy( buf, OBJMAG, 8); //stripped elfにコピー
        new_image_pointer += 8;
        newarch_size += 8;
        

        ElfHandle->buf_current = (void*)new_image_pointer;
        while( image_pointer < obj_len) {
            ElfHandle->ELi_ReadStub( &ArHdr, ElfHandle->FileStruct, (u32)(obj_image), image_pointer, arch_size);
            ElfHandle->ELi_ReadStub( OBJMAG, ElfHandle->FileStruct, (u32)(obj_image), (image_pointer+arch_size), 4);    /*OBJの文字列を取得*/
            if( strncmp( OBJMAG, ELFMAG, 4) == 0) {
                elf_num++;

                memcpy( (void*)new_image_pointer, (const void*)&ArHdr, arch_size); //arヘッダをstripped elfにコピー

                if( type == 0) {
                result = ELi_LoadObject( ElfHandle, (void*)(image_pointer+arch_size),
                                         (void*)(new_image_pointer + arch_size));
//                                         (ElfHandle->buf_current + arch_size));
                }else{
                result = ELi_LoadObject2( ElfHandle, (void*)(image_pointer+arch_size),
                                          (void*)(new_image_pointer + arch_size));
                }

                ELi_SetDecSize( (char*)(((ArchHdr*)new_image_pointer)->ar_size), ElfHandle->newelf_size); //archヘッダのサイズ更新
                new_image_pointer += (ElfHandle->newelf_size + arch_size);
                newarch_size += (ElfHandle->newelf_size + arch_size);
                
                if( result < all_result) {        /*悪い結果のときだけall_resultに反映*/
                    all_result = result;
                }
                /*初期値の設定*/
                ElfHandle->ShdrEx = NULL;
                ElfHandle->SymEx = NULL;
                ElfHandle->process = EL_INITIALIZED;    /*フラグの設定*/
            }else{
                memcpy( (void*)new_image_pointer, (const void*)&ArHdr, arch_size); //arヘッダをstripped elfにコピー

                /*arヘッダの次（エントリ本体）をstripped elfにコピー*/
                ElfHandle->ELi_ReadStub( (void*)(new_image_pointer+arch_size), ElfHandle->FileStruct,
                                         (u32)(obj_image),
                                         (image_pointer+arch_size), AR_GetEntrySize( &ArHdr));

                new_image_pointer += (AR_GetEntrySize( &ArHdr) + arch_size);
                newarch_size += (AR_GetEntrySize( &ArHdr) + arch_size);
            }
            /*次のエントリへ*/
            image_pointer += arch_size + AR_GetEntrySize( &ArHdr);
        }
        ElfHandle->newelf_size = newarch_size;
    }else{/*--------------- ELFファイルの場合 ---------------*/
        if( strncmp( OBJMAG, ELFMAG, 4) == 0) {
            elf_num++;
            if( type == 0) {
                all_result = ELi_LoadObject( ElfHandle, 0, buf);
            }else{
                all_result = ELi_LoadObject2( ElfHandle, 0, buf);
            }
        }
    }
    /*-------------------------------------------------------*/

    if( elf_num) {
        return all_result;
    }else{
        return EL_FAILED;
    }
}

/*------------------------------------------------------
  ELFオブジェクトをバッファに再配置する
    
    ElfHandle : ヘッダ構造体
    obj_offset : OBJファイルのRAM上イメージアドレスからのオフセット
    buf : ロードするバッファ
 -----------------------------------------------------*/
u16 ELi_LoadObject( ELHandle* ElfHandle, void* obj_offset, void* buf)
{
    u16         i, j;
//    u32         num_of_entry;
    ELShdrEx*   FwdShdrEx;
    ELShdrEx*   CurrentShdrEx;
    ELShdrEx    DmyShdrEx;
//    char        sym_str[128];     //デバッグプリント用
//    u32         offset;           //デバッグプリント用
    u32         newelf_shoff = 0; //stripped elfイメージへの書き込みポインタ
    u32         buf_shdr;
    u32         section_num = 0;
//    u32         newelf_size;
    u32         tmp_buf;
    u32         *shdr_table;      //セクションヘッダ新旧番号対応テーブル
//    u32         *sym_table;       //シンボルエントリ新旧番号対応テーブル
    u32         last_local_symbol_index = 0xFFFFFFFF;


    /* ELHandleの初期化チェック */
    if( ElfHandle->process != EL_INITIALIZED) {
        return EL_FAILED;
    }
    /* バッファのNULLチェック */
    if( buf == NULL) {
        return EL_FAILED;
    }
    /* ELFヘッダの取得 */
    ElfHandle->ELi_ReadStub( &(ElfHandle->CurrentEhdr), ElfHandle->FileStruct,
                             (u32)(ElfHandle->ar_head), (u32)(obj_offset), sizeof( Elf32_Ehdr));

    /* セクションハンドル構築 */
    ElfHandle->elf_offset = obj_offset;
    ElfHandle->buf_current = (void*)((u32)buf + sizeof( Elf32_Ehdr));
    ElfHandle->shentsize = ElfHandle->CurrentEhdr.e_shentsize;

    /*セクションヘッダテーブル構築*/
    shdr_table = (u32*)malloc( 4 * ElfHandle->CurrentEhdr.e_shnum);
  
    /*---------- ELShdrExのリストとshdr_tableを作る ----------*/
    CurrentShdrEx = &DmyShdrEx;
    for( i=0; i<(ElfHandle->CurrentEhdr.e_shnum); i++) {
        CurrentShdrEx->next = (void*)(malloc( sizeof(ELShdrEx)));
        CurrentShdrEx = (ELShdrEx*)(CurrentShdrEx->next);
        memset( CurrentShdrEx, 0, sizeof(ELShdrEx));     //ゼロクリア
        
        /*デバッグ情報かどうかを判別してフラグをセット*/
        if( ELi_ShdrIsDebug( ElfHandle, i) == TRUE) {    /*デバッグ情報の場合*/
            CurrentShdrEx->debug_flag = 1;
            shdr_table[i] = 0xFFFFFFFF;
        }else{                                           /*デバッグ情報でない場合*/
            /*セクションヘッダをコピー*/
            ELi_GetShdr( ElfHandle, i, &(CurrentShdrEx->Shdr));
            CurrentShdrEx->debug_flag = 0;
            shdr_table[i] = section_num;                 /*セクション新旧テーブル作成*/
//            printf( "shdr_table[0x%x] = 0x%x\n", i, section_num);
            section_num++;
            /*セクション文字列を取得しておく*/
#if 0 // セクション文字列バッファを動的に確保しない
            CurrentShdrEx->str = (char*)malloc( 128);    //128文字バッファ取得
            ELi_GetStrAdr( ElfHandle, ElfHandle->CurrentEhdr.e_shstrndx,
                           CurrentShdrEx->Shdr.sh_name,
                           CurrentShdrEx->str, 128);
#else
            {
                u32 length;
                
                length = ELi_GetStrLen( ElfHandle, ElfHandle->CurrentEhdr.e_shstrndx, CurrentShdrEx->Shdr.sh_name) + 1;
                
                CurrentShdrEx->str = (char*)malloc(length);
                if(CurrentShdrEx->str == 0)
                {
                    return EL_FAILED;
                }
                ELi_GetStrAdr( ElfHandle, ElfHandle->CurrentEhdr.e_shstrndx,
                               CurrentShdrEx->Shdr.sh_name,
                               CurrentShdrEx->str, length);
            }
#endif
        }
    }
#if 0 /*特殊セクション追加はadded-elf側で行うのでコメントアウト*/
    section_num++;                    //特殊セクション追加
    ElfHandle->CurrentEhdr.e_shnum++; //特殊セクション追加
    CurrentShdrEx->next = (void*)(malloc( sizeof(ELShdrEx)));
    CurrentShdrEx = (ELShdrEx*)(CurrentShdrEx->next);
    memset( CurrentShdrEx, 0, sizeof(ELShdrEx));    //ゼロクリア
//    CurrentShdrEx->Shdr.sh_name
    CurrentShdrEx->Shdr.sh_type = SPECIAL_SECTION_TYPE;
    CurrentShdrEx->Shdr.sh_flags = 0;
    CurrentShdrEx->Shdr.sh_addr = 0;
    CurrentShdrEx->Shdr.sh_offset = 0;
    CurrentShdrEx->Shdr.sh_size = (section_num - 1)*4; //(ElfHandle->CurrentEhdr.e_shnum)*4;(旧セクション番号がインデックスの場合)
    CurrentShdrEx->Shdr.sh_link = 0;
    CurrentShdrEx->Shdr.sh_info = 0;
    CurrentShdrEx->Shdr.sh_addralign = 4;
    CurrentShdrEx->Shdr.sh_entsize = 4;
#if 0 // セクション文字列バッファを動的に確保しない
    CurrentShdrEx->str = (char*)malloc( 128);
#else
    CurrentShdrEx->str = (char*)malloc( strlen(SPECIAL_SECTION_NAME) + 1 );
    if(CurrentShdrEx->str == 0)
    {
        return EL_FAILED;
    }
#endif
    strcpy( CurrentShdrEx->str, SPECIAL_SECTION_NAME);
#endif
    CurrentShdrEx->next = NULL;
    ElfHandle->ShdrEx = DmyShdrEx.next;
    /*--------------------------------------------------------*/

   
    /*---------- 全セクションを調べてコピーする ----------*/
//    printf( "\nLoad to RAM:\n");
    for( i=0; i<(ElfHandle->CurrentEhdr.e_shnum); i++) {
        //
        CurrentShdrEx = ELi_GetShdrExfromList( ElfHandle->ShdrEx, i);

        if( CurrentShdrEx->debug_flag == 1) {                 //デバッグ情報の場合
//            printf( "skip debug-section %02x\n", i);
        }else{                                                //デバッグ情報でない場合
            // .text section
            if( /*(CurrentShdrEx->Shdr.sh_flags == (SHF_ALLOC | SHF_EXECINSTR))&&*/
                (CurrentShdrEx->Shdr.sh_type == SHT_PROGBITS)) {
                //メモリにコピー
                CurrentShdrEx->loaded_adr = (u32)
                                ELi_CopySectionToBuffer( ElfHandle, &(CurrentShdrEx->Shdr));
            }
            // .data, .data1 section (初期化済みデータ)
/*            else if( (CurrentShdrEx->Shdr.sh_flags == (SHF_ALLOC | SHF_WRITE))&&
                (CurrentShdrEx->Shdr.sh_type == SHT_PROGBITS)) {
                //メモリにコピー
                CurrentShdrEx->loaded_adr = (u32)
                                ELi_CopySectionToBuffer( ElfHandle, &(CurrentShdrEx->Shdr));
            }*/
            // .bss section
            else if( (CurrentShdrEx->Shdr.sh_flags == (SHF_ALLOC | SHF_WRITE))&&
                (CurrentShdrEx->Shdr.sh_type == SHT_NOBITS)) {
                //コピーしない
                CurrentShdrEx->loaded_adr = ELi_ALIGN( (u32)(ElfHandle->buf_current), 4);
/*                CurrentShdrEx->loaded_adr = (u32)
                                ELi_AllocSectionToBuffer( ElfHandle, &(CurrentShdrEx->Shdr));*/
            }
            // .rodata, .rodata1 section
/*            else if( (CurrentShdrEx->Shdr.sh_flags == SHF_ALLOC)&&
                (CurrentShdrEx->Shdr.sh_type == SHT_PROGBITS)) {
                //メモリにコピー
                CurrentShdrEx->loaded_adr = (u32)
                                ELi_CopySectionToBuffer( ElfHandle, &(CurrentShdrEx->Shdr));
            }*/
#if 0 /*特殊セクション追加はadded-elf側で行うのでコメントアウト*/
            else if( CurrentShdrEx->Shdr.sh_type == SPECIAL_SECTION_TYPE) {
                //ELi_CopySectionToBufferに相当する一連の処理
                u32 load_start = ELi_ALIGN( ((u32)(ElfHandle->buf_current)), 4);
                u32 sh_size    = CurrentShdrEx->Shdr.sh_size;
                u32* reverse_shdr_table = (u32*)malloc( 4 * (section_num-1)); //-1は特殊セクションを除くため
                /*インデックスが新セクション番号の表に変換する*/
                for( j=0; j<(ElfHandle->CurrentEhdr.e_shnum - 1); j++) {
                    if( shdr_table[j] != 0xFFFFFFFF) {
                        reverse_shdr_table[shdr_table[j]] = j;
                    }
                }
                memcpy( (u32*)load_start, reverse_shdr_table, sh_size); //新旧セクション対応テーブルコピー
                free( reverse_shdr_table);
                CurrentShdrEx->loaded_adr = load_start;
                ElfHandle->buf_current = (void*)(load_start + sh_size);
            }
#endif
            //.arm_vfe_header, .ARM.exidx, .ARM.attributesなどもコピーしておく
            else if( CurrentShdrEx->Shdr.sh_type >= SHT_LOPROC) {
                //メモリにコピー
                CurrentShdrEx->loaded_adr = (u32)
                                ELi_CopySectionToBuffer( ElfHandle, &(CurrentShdrEx->Shdr));
            }
            //シンボルテーブルセクション
            else if( CurrentShdrEx->Shdr.sh_type == SHT_SYMTAB) {
                ELi_BuildSymList( ElfHandle, i, &(CurrentShdrEx->sym_table)); //シンボルリスト作成
                {
                    ELSymEx* CurrentSymEx;
                    ELShdrEx* StrShEx;    //文字列セクション
#if 0 // セクション文字列バッファを動的に確保しない
                    char     symstr[128];
#else
                    char     *symstr;
#endif
                    u32      symbol_num = 0;

                    /*--- シンボルエントリのデータを修正 ---*/
                    CurrentSymEx = ElfHandle->SymEx;
                    while( CurrentSymEx != NULL) {  //SymExをたどればデバッグシンボルは含まれない
                        /*文字列セクション取得*/
                        StrShEx = ELi_GetShdrExfromList( ElfHandle->ShdrEx,
                                                         CurrentShdrEx->Shdr.sh_link);
                        /*--- 文字列セクションの中身構築 ---*/
#if 0 // セクション文字列バッファを動的に確保しない
                        ELi_GetStrAdr( ElfHandle, CurrentShdrEx->Shdr.sh_link, //シンボル文字列取得
                                       CurrentSymEx->Sym.st_name,
                                       symstr, 128);
#else
                        {
                            u32 length;
                            
                            length = ELi_GetStrLen( ElfHandle, CurrentShdrEx->Shdr.sh_link, CurrentSymEx->Sym.st_name ) + 1;
                            
                            symstr = (char*)malloc(length);
                            if(symstr == 0)
                            {
                                return EL_FAILED;
                            }
                            ELi_GetStrAdr( ElfHandle, CurrentShdrEx->Shdr.sh_link,
                                           CurrentSymEx->Sym.st_name,
                                           symstr, length);
                        }
#endif
                        StrShEx->str_table = realloc( StrShEx->str_table,     //シンボル文字列追加
                                                      (StrShEx->str_table_size) +
                                                      strlen( symstr) + 1);
                        strcpy( (u8*)((u32)StrShEx->str_table + StrShEx->str_table_size),
                                symstr);

                        CurrentSymEx->Sym.st_name = StrShEx->str_table_size; //シンボルエントリのデータを修正

                        StrShEx->str_table_size += ( strlen( symstr) + 1);
#if 0 // セクション文字列バッファを動的に確保しない
#else
                        free(symstr);
#endif
                        /*---------------------------------*/
                        //最後のSTB_LOCALシンボルを見つける（SYMTABセクションヘッダのsh_infoの値になる）
                        if( ((ELF32_ST_BIND( (CurrentSymEx->Sym.st_info))) != STB_LOCAL)&&
                            (last_local_symbol_index == 0xFFFFFFFF)) {
                            last_local_symbol_index = symbol_num;
                        }
                        /*---------------------------------*/

                        symbol_num++;

                        /*---------------------------------*/
                        if( (CurrentSymEx->Sym.st_shndx != SHN_UNDEF) &&
                            (CurrentSymEx->Sym.st_shndx < SHN_LORESERVE)) {
                            CurrentSymEx->Sym.st_shndx = shdr_table[CurrentSymEx->Sym.st_shndx]; //シンボルエントリの対象セクション番号更新
                        }
                        /*---------------------------------*/
                        CurrentSymEx = CurrentSymEx->next;
                    }/*-------------------------------------*/
                    
                    /*--- シンボルテーブルセクションヘッダの更新 ---*/
                    CurrentShdrEx->loaded_adr = (u32)(ELi_CopySymToBuffer( ElfHandle));
                    if( (CurrentShdrEx->Shdr.sh_link != SHN_UNDEF) &&
                        (CurrentShdrEx->Shdr.sh_link < SHN_LORESERVE)) {
                            CurrentShdrEx->Shdr.sh_link = shdr_table[CurrentShdrEx->Shdr.sh_link]; //文字列セクション番号更新
                    }
                    CurrentShdrEx->Shdr.sh_size = symbol_num * sizeof( Elf32_Sym);
                    CurrentShdrEx->Shdr.sh_info = last_local_symbol_index; //ARM ELF資料参照
                    /*----------------------------------------------*/
                }
                ELi_FreeSymList( ElfHandle, (u32**)(CurrentShdrEx->sym_table));    //シンボルリスト開放
            }

/*            printf( "section %02x relocated at %08x\n",
                        i, CurrentShdrEx->loaded_adr);*/
        }
    }

    /*---------- REL, RELA, STRTABセクションの処理 ----------*/
    for( i=0; i<(ElfHandle->CurrentEhdr.e_shnum); i++) {
        //
        CurrentShdrEx = ELi_GetShdrExfromList( ElfHandle->ShdrEx, i);

        if( CurrentShdrEx->debug_flag == 1) {                //デバッグ情報の場合
        }else{                                               //デバッグ情報でない場合
            //RELセクション
            if( CurrentShdrEx->Shdr.sh_type == SHT_REL) {
                CurrentShdrEx->loaded_adr = (u32)
                                ELi_CopySectionToBuffer( ElfHandle, &(CurrentShdrEx->Shdr));
                {
                    Elf32_Rel*  CurrentRel;
                    ELShdrEx*   SymShdrEx;
                    u32         new_sym_num;
                    /*シンボルセクション取得*/
                    SymShdrEx = ELi_GetShdrExfromList( ElfHandle->ShdrEx,
                                                       CurrentShdrEx->Shdr.sh_link);
                    /*コピー先のRelセクションを修正*/
                    for( j=0; j<(CurrentShdrEx->Shdr.sh_size/CurrentShdrEx->Shdr.sh_entsize); j++) {
                        CurrentRel = (Elf32_Rel*)(CurrentShdrEx->loaded_adr + (j * sizeof( Elf32_Rel)));
                        new_sym_num = SymShdrEx->sym_table[ELF32_R_SYM(CurrentRel->r_info)];
                        CurrentRel->r_info = ELF32_R_INFO( new_sym_num,
                                                            ELF32_R_TYPE(CurrentRel->r_info));
                        }
                }
            }
            //RELAセクション
            else if( CurrentShdrEx->Shdr.sh_type == SHT_RELA) {
                CurrentShdrEx->loaded_adr = (u32)
                                ELi_CopySectionToBuffer( ElfHandle, &(CurrentShdrEx->Shdr));
                {
                    Elf32_Rela* CurrentRela;
                    ELShdrEx*   SymShdrEx;
                    u32         new_sym_num;
                    /*シンボルセクション取得*/
                    SymShdrEx = ELi_GetShdrExfromList( ElfHandle->ShdrEx,
                                                       CurrentShdrEx->Shdr.sh_link);
                    /*コピー先のRelaセクションを修正*/
                    for( j=0; j<(CurrentShdrEx->Shdr.sh_size/CurrentShdrEx->Shdr.sh_entsize); j++) {
                        CurrentRela = (Elf32_Rela*)(CurrentShdrEx->loaded_adr + (j * sizeof( Elf32_Rela)));
//                        printf( "symnum: 0x%x\n", ELF32_R_SYM(CurrentRela->r_info));
//                        printf( "sym_table:0x%x", (u32)(SymShdrEx->sym_table));
                        new_sym_num = SymShdrEx->sym_table[ELF32_R_SYM(CurrentRela->r_info)];
                        CurrentRela->r_info = ELF32_R_INFO( new_sym_num,
                                                            ELF32_R_TYPE(CurrentRela->r_info));
                        }
                }
            }
            //文字列テーブルセクション
            else if( CurrentShdrEx->Shdr.sh_type == SHT_STRTAB) {
                if( i == ElfHandle->CurrentEhdr.e_shstrndx) { //セクション名文字列テーブルセクション
                    CurrentShdrEx->loaded_adr = (u32)
                        ELi_CopyShStrToBuffer( ElfHandle, &(CurrentShdrEx->Shdr));
                }else{
                    CurrentShdrEx->loaded_adr = (u32)
                        ELi_CopySymStrToBuffer( ElfHandle, CurrentShdrEx);
//                    printf( "sym str section:0x%x, size:0x%x\n", i, CurrentShdrEx->str_table_size);
                }
            }
        }
    }
    /*-------------------------------------------------------*/

    
    /*ここまででセクションの中身だけがstripped elfにコピーされた*/

  
    /*---------- セクションヘッダを stripped elfにコピー ----------*/
    buf_shdr = ELi_ALIGN( ((u32)(ElfHandle->buf_current)), 4);
    ElfHandle->buf_current = (void*)buf_shdr;
//    printf( "buf_shdr = 0x%x\n", buf_shdr);
//    printf( "buf = 0x%x\n", (u32)buf);
    
    newelf_shoff = buf_shdr - ((u32)(buf));
//    printf( "newelf_shoff = 0x%x\n", newelf_shoff);

    section_num = 0;
    for( i=0; i<(ElfHandle->CurrentEhdr.e_shnum); i++) {
        //
        tmp_buf = buf_shdr + ( section_num * sizeof( Elf32_Shdr));
        
        CurrentShdrEx = ELi_GetShdrExfromList( ElfHandle->ShdrEx, i);
        if( CurrentShdrEx->debug_flag == 1) {                //デバッグ情報の場合はNULLセクションに変換
//            memset( (u8*)tmp_buf, '\0', sizeof( Elf32_Shdr));
        }else{
            /*オフセット更新*/
            if( CurrentShdrEx->loaded_adr != 0) {
                CurrentShdrEx->Shdr.sh_offset = (CurrentShdrEx->loaded_adr - (u32)buf);
            }
            /*セクションヘッダをstripped elfイメージへコピー*/
            if( CurrentShdrEx->Shdr.sh_type == SHT_SYMTAB) {
                /*シンボルテーブルのsh_typeはシンボルエントリのインデックスを表す*/
              //SYMTABの場合は406行目〜で既にsh_linkを変換済みなのでここでは必要なし
              //SYMTABの場合は428行目で既にsh_infoを変換済みなのでここでは必要なし
            }else{
                CurrentShdrEx->Shdr.sh_link = shdr_table[CurrentShdrEx->Shdr.sh_link]; //セクション番号更新
                CurrentShdrEx->Shdr.sh_info = shdr_table[CurrentShdrEx->Shdr.sh_info]; //セクション番号更新
            }
            memcpy( (u8*)tmp_buf, &(CurrentShdrEx->Shdr), 
                    sizeof( Elf32_Shdr));
            section_num++;                        /*セクション数更新*/
        }
    }

    // コピー終了後
    ElfHandle->process = EL_COPIED;
    /*------------------------------------------------------------*/

    ElfHandle->newelf_size = (buf_shdr - (u32)buf) + (section_num*sizeof( Elf32_Shdr));
//    printf( "newelf_size = 0x%x\n", ElfHandle->newelf_size);

    /*---------- ELFヘッダ更新 ----------*/
    ElfHandle->CurrentEhdr.e_shnum = section_num; /*減ったセクション数をELFヘッダに反映*/
    ElfHandle->CurrentEhdr.e_shstrndx = shdr_table[ElfHandle->CurrentEhdr.e_shstrndx];
    //セクションヘッダオフセット更新
    ElfHandle->CurrentEhdr.e_shoff = newelf_shoff;
    memcpy( (u8*)buf, &(ElfHandle->CurrentEhdr), sizeof( Elf32_Ehdr)); /*ELFヘッダをstripped elfイメージにコピー*/
    /*-----------------------------------*/


    /*---------- 再配置 ----------*/
/*    if( unresolved_table_block_flag == 0) {
        if( dbg_print_flag == 1) {
            printf( "\nRelocate Symbols:\n");
        }
    for( i=0; i<(ElfHandle->CurrentEhdr.e_shnum); i++) {
        //
        CurrentShdrEx = ELi_GetShdrExfromList( ElfHandle->ShdrEx, i);
        
        if( CurrentShdrEx->debug_flag == 1) {                //デバッグ情報の場合
        }else{                                                //デバッグ情報でない場合

            if( CurrentShdrEx->Shdr.sh_type == SHT_REL) {
                num_of_entry = (CurrentShdrEx->Shdr.sh_size) /
                                (CurrentShdrEx->Shdr.sh_entsize);
                if( dbg_print_flag == 1) {
                    printf( "num of REL = %x\n", num_of_entry);
                    printf( "Section Header Info.\n");
                    printf( "link   : %x\n", CurrentShdrEx->Shdr.sh_link);
                    printf( "info   : %x\n", CurrentShdrEx->Shdr.sh_info);
                    printf( " Offset     Info    Type            Sym.Value  Sym.Name\n");
                }
                offset = 0;
                for( j=0; j<num_of_entry; j++) {
                    ELi_GetSent( ElfHandle, i, &(ElfHandle->Rel), offset, sizeof(Elf32_Rel));
                    ELi_GetShdr( ElfHandle, CurrentShdrEx->Shdr.sh_link, &(ElfHandle->SymShdr));
                    ELi_GetSent( ElfHandle, CurrentShdrEx->Shdr.sh_link, &(ElfHandle->Sym),
                                 (u32)(ElfHandle->SymShdr.sh_entsize * ELF32_R_SYM( ElfHandle->Rel.r_info)), sizeof(Elf32_Sym));
                    ELi_GetStrAdr( ElfHandle, ElfHandle->SymShdr.sh_link, ElfHandle->Sym.st_name, sym_str, 128);

                    if( dbg_print_flag == 1) {
                        printf( "%08x  ", ElfHandle->Rel.r_offset);
                        printf( "%08x ", ElfHandle->Rel.r_info);
                        printf( "                  ");
                        printf( "%08x ", ElfHandle->Sym.st_value);
                        printf( sym_str);
                        printf( "\n");
                    }
                    //次のエントリへ
                    offset += (u32)(CurrentShdrEx->Shdr.sh_entsize);
                }
                if( dbg_print_flag == 1) {                
                    printf( "\n");
                }
                //リロケート
                ELi_RelocateSym( ElfHandle, i);
                if( dbg_print_flag == 1) {
                    printf( "\n");
                }
            }
            else if( CurrentShdrEx->Shdr.sh_type == SHT_RELA) {
                
                num_of_entry = (CurrentShdrEx->Shdr.sh_size) /
                                (CurrentShdrEx->Shdr.sh_entsize);
                if( dbg_print_flag == 1) {
                    printf( "num of RELA = %x\n", num_of_entry);
                    printf( "Section Header Info.\n");
                    printf( "link   : %x\n", CurrentShdrEx->Shdr.sh_link);
                    printf( "info   : %x\n", CurrentShdrEx->Shdr.sh_info);
                    printf( " Offset     Info    Type            Sym.Value  Sym.Name\n");
                }
                offset = 0;
                for( j=0; j<num_of_entry; j++) {
                    ELi_GetSent( ElfHandle, i, &(ElfHandle->Rela), offset, sizeof(Elf32_Rel));
                    ELi_GetShdr( ElfHandle, CurrentShdrEx->Shdr.sh_link, &(ElfHandle->SymShdr));
                    ELi_GetSent( ElfHandle, CurrentShdrEx->Shdr.sh_link, &(ElfHandle->Sym),
                                 (u32)(ElfHandle->SymShdr.sh_entsize * ELF32_R_SYM( ElfHandle->Rela.r_info)), sizeof(Elf32_Sym));
                    ELi_GetStrAdr( ElfHandle, ElfHandle->SymShdr.sh_link, ElfHandle->Sym.st_name, sym_str, 128);

                    if( dbg_print_flag == 1) {
                        printf( "%08x  ", ElfHandle->Rela.r_offset);
                        printf( "%08x ", ElfHandle->Rela.r_info);
                        printf( "                  ");
                        printf( "%08x ", ElfHandle->Sym.st_value);
                        printf( sym_str);
                        printf( "\n");
                    }
                    //次のエントリへ
                    offset += (u32)(CurrentShdrEx->Shdr.sh_entsize);
                }
                if( dbg_print_flag == 1) {                
                    printf( "\n");
                }
                //リロケート
                ELi_RelocateSym( ElfHandle, i);
                if( dbg_print_flag == 1) {
                    printf( "\n");
                }
            }
        }
    }
    }else{    //dllでなくてstaticモジュールの場合
        for( i=0; i<(ElfHandle->CurrentEhdr.e_shnum); i++) {
            CurrentShdrEx = ELi_GetShdrExfromList( ElfHandle->ShdrEx, i);
            if( CurrentShdrEx->debug_flag == 1) {                //デバッグ情報の場合
            }else{
                if( CurrentShdrEx->Shdr.sh_type == SHT_SYMTAB) {
                    ELi_DiscriminateGlobalSym( ElfHandle, i);
                }
            }
        }
    }*/

    /*------- ELShdrExのリストを解放する -------*/
    CurrentShdrEx = ElfHandle->ShdrEx;
    if( CurrentShdrEx) {
        do{
            FwdShdrEx = CurrentShdrEx;
            CurrentShdrEx = CurrentShdrEx->next;
            if( FwdShdrEx->debug_flag == 0) {
                free( FwdShdrEx->str);    //非デバッグセクションの場合
            }
            free( FwdShdrEx);
        }while( CurrentShdrEx != NULL);
        ElfHandle->ShdrEx = NULL;
    }
    /*-----------------------------------------*/

    /*セクションヘッダテーブル開放*/
    free( shdr_table);


  
    /*RAM上のDLLが呼ばれる前にキャッシュをフラッシュ*/
//    DC_FlushAll();
//    DC_WaitWriteBufferEmpty();
    
    return (ElfHandle->process);
}

/*------------------------------------------------------
  未解決のシンボルがあればアドレステーブルを使って解決する
 -----------------------------------------------------*/
u16 EL_ResolveAllLibrary( void)
{
    ELAdrEntry*           AdrEnt;
    ELUnresolvedEntry*    RemoveUnrEnt;
    ELUnresolvedEntry*    UnrEnt;
//    ELUnresolvedEntry*    CurrentUnrEnt;
//    ELUnresolvedEntry*    FwdUnrEnt;
//    u32                   relocation_val;
//    ELAdrEntry            AddAdrEnt;
#if 0 // セクション文字列バッファを動的に確保しない
    char                  sym_str[128];
#else
    char                  *sym_str;
#endif
    UnrEnt = ELUnrEntStart;
    if( dbg_print_flag == 1) {
        printf( "\nResolve all symbols:\n");
    }
    while( UnrEnt != NULL) {
        if( UnrEnt->remove_flag == 0) {
            AdrEnt = EL_GetAdrEntry( UnrEnt->sym_str);        /*アドレステーブルから検索*/
            if( AdrEnt) {                                    /*アドレステーブルから見つかった場合*/
                UnrEnt->S_ = (u32)(AdrEnt->adr);
                UnrEnt->T_ = (u32)(AdrEnt->thumb_flag);
                if( unresolved_table_block_flag == 0) {
                    if( dbg_print_flag == 1) {
//                        printf( "\n symbol found %s : %8x\n", UnrEnt->sym_str, UnrEnt->S_);
                    }
                    UnrEnt->remove_flag = 1;
    //                ELi_RemoveUnresolvedEntry( UnrEnt);    //解決したので未解決リストから削除
                }else{
                    if( dbg_print_flag == 1) {
                        printf( "\n static symbol found %s : %8x\n", UnrEnt->sym_str, (int)(UnrEnt->S_));
                    }
                    UnrEnt->AdrEnt = AdrEnt;            //見つけたアドレスエントリをセット
                    UnrEnt->remove_flag = 2;            //マーキング（makelstだけで使用する特別な値）
#if 0 // セクション文字列バッファを動的に確保しない
#else
                    sym_str = (char*)malloc( strlen( UnrEnt->sym_str ) + 1 );
                    if(sym_str == 0)
                    {
                        return EL_FAILED;
                    }
#endif
                    strcpy( sym_str, UnrEnt->sym_str);
                    while( 1) {
                        RemoveUnrEnt = ELi_GetUnresolvedEntry( sym_str);
                        if( RemoveUnrEnt == NULL) {
                            break;
                        }else{
                            RemoveUnrEnt->remove_flag = 1;
                        }
                    }
#if 0 // セクション文字列バッファを動的に確保しない
#else
                    free(sym_str);
#endif
                }
    /*            relocation_val = ELi_DoRelocate( UnrEnt);    //シンボル解決
                if( !relocation_val) {
                    return EL_FAILED;
                }*/
            
            }else{                                            /*アドレステーブルから見つからなかった場合*/
                if( unresolved_table_block_flag == 0) {
                    if( dbg_print_flag == 1) {
                        printf( "ERROR! cannot find symbol : %s\n", UnrEnt->sym_str);
                    }
                }else{
                    if( dbg_print_flag == 1) {
                        printf( "ERROR! cannot find static symbol : %s\n", UnrEnt->sym_str);
                    }
                }
    /*            AddAdrEnt.next = NULL;
                AddAdrEnt.name = UnrEnt->sym_str;
                AddAdrEnt.
                EL_AddAdrEntry( */
    //            return EL_FAILED;
            }
        }
        UnrEnt = (ELUnresolvedEntry*)(UnrEnt->next);                            /*次の未解決エントリへ*/
    }
    
    /*--- ELUnresolvedEntryのリストを解放する ---*/
/*    CurrentUnrEnt = ELUnrEntStart;
    if( CurrentUnrEnt) {
        while( CurrentUnrEnt->next != NULL) {
            FwdUnrEnt = CurrentUnrEnt;
            CurrentUnrEnt = CurrentUnrEnt->next;
            free( FwdUnrEnt->sym_str);    //シンボル名文字列
            free( FwdUnrEnt);            //構造体自身
        }
        ELUnrEntStart = NULL;
    }*/
    /*-------------------------------------------*/

    /*RAM上のDLLが呼ばれる前にキャッシュをフラッシュ*/
//    DC_FlushAll();
//    DC_WaitWriteBufferEmpty();
    
    return EL_RELOCATED;
}





/*------------------------------------------------------
  アドレステーブルからエントリを削除する
 -----------------------------------------------------*/
BOOL EL_RemoveAdrEntry( ELAdrEntry* AdrEnt)
{
    ELAdrEntry  DmyAdrEnt;
    ELAdrEntry* CurrentAdrEnt;

    DmyAdrEnt.next = ELAdrEntStart;
    CurrentAdrEnt  = &DmyAdrEnt;

    while( CurrentAdrEnt->next != AdrEnt) {
        if( CurrentAdrEnt->next == NULL) {
            return FALSE;
        }else{
            CurrentAdrEnt = (ELAdrEntry*)CurrentAdrEnt->next;
        }
    }

    CurrentAdrEnt->next = AdrEnt->next;
    ELAdrEntStart = DmyAdrEnt.next;
    
    return TRUE;
}

/*------------------------------------------------------
  アドレステーブルにエントリを追加する
 -----------------------------------------------------*/
void EL_AddAdrEntry( ELAdrEntry* AdrEnt)
{
    ELAdrEntry  DmyAdrEnt;
    ELAdrEntry* CurrentAdrEnt;

    if( !ELAdrEntStart) {
        ELAdrEntStart = AdrEnt;
    }else{
        DmyAdrEnt.next = ELAdrEntStart;
        CurrentAdrEnt = &DmyAdrEnt;

        while( CurrentAdrEnt->next != NULL) {
            CurrentAdrEnt = (ELAdrEntry*)CurrentAdrEnt->next;
        }
        CurrentAdrEnt->next = (void*)AdrEnt;
    }
    AdrEnt->next = NULL;
}

/*------------------------------------------------------
  アドレステーブルから指定文字列に該当するエントリを探す
 -----------------------------------------------------*/
ELAdrEntry* EL_GetAdrEntry( char* ent_name)
{
    ELAdrEntry* CurrentAdrEnt;

    CurrentAdrEnt = ELAdrEntStart;
    if( CurrentAdrEnt == NULL) {
        return NULL;
    }
    while( strcmp( CurrentAdrEnt->name, ent_name) != 0) {
        CurrentAdrEnt = (ELAdrEntry*)CurrentAdrEnt->next;
        if( CurrentAdrEnt == NULL) {
            break;
        }
    }
    return CurrentAdrEnt;
}

/*------------------------------------------------------
  アドレステーブルから指定文字列に該当するアドレスを返す
 -----------------------------------------------------*/
void* EL_GetGlobalAdr( char* ent_name)
{
    u32         adr;
    ELAdrEntry* CurrentAdrEnt;

    CurrentAdrEnt = EL_GetAdrEntry( ent_name);

    if( CurrentAdrEnt) {
        if( CurrentAdrEnt->thumb_flag) {
            adr = (u32)(CurrentAdrEnt->adr) + 1;
        }else{
            adr = (u32)(CurrentAdrEnt->adr);
        }
    }else{
        adr = 0;
    }

    return (void*)(adr);
}

#if 0
/*------------------------------------------------------
  アドレステーブルを解放する（アプリケーションが登録したエントリまで削除しようとするのでＮＧ）
 -----------------------------------------------------*/
void* EL_FreeAdrTbl( void)
{
    ELAdrEntry*    FwdAdrEnt;
    ELAdrEntry*    CurrentAdrEnt;
    
    /*--- ELAdrEntryのリストを解放する ---*/
    CurrentAdrEnt = ELAdrEntStart;
    if( CurrentAdrEnt) {
        while( CurrentAdrEnt->next != NULL) {
            FwdAdrEnt = CurrentAdrEnt;
            CurrentAdrEnt = CurrentAdrEnt->next;
            free( FwdAdrEnt->name);        //シンボル名文字列
            free( FwdAdrEnt);            //構造体自身
        }
        ELAdrEntStart = NULL;
    }
    /*------------------------------------*/
}
#endif

/*------------------------------------------------------
  ELFオブジェクトからデータを読み出すスタブ
 -----------------------------------------------------*/
void ELi_ReadFile( void* buf, void* file_struct, u32 file_base, u32 file_offset, u32 size)
{
//  printf( "0x%x, 0x%x\n", file_offset, size);
    fseek( file_struct, file_offset, SEEK_SET);
    fread( buf, 1, size, file_struct);
    
/*    FS_SeekFile( file_struct, (s32)(file_offset), FS_SEEK_SET);
    FS_ReadFile( file_struct, buf, (s32)(size));*/
}

void ELi_ReadMem( void* buf, void* file_struct, u32 file_base, u32 file_offset, u32 size)
{
/*    MI_CpuCopy8( (void*)(file_base + file_offset),
                buf,
                size);*/
    memcpy( buf,
            (void*)(file_base + file_offset),
            size);
}
