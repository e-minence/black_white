/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MI - include
  File:     uncompress.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $

 *---------------------------------------------------------------------------*/

#ifndef NITRO_MI_UNCOMPRESS_H_
#define NITRO_MI_UNCOMPRESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>


//---- compression type
typedef enum
{
    MI_COMPRESSION_LZ = 0x10,          // LZ77
    MI_COMPRESSION_HUFFMAN = 0x20,     // Huffman
    MI_COMPRESSION_RL = 0x30,          // Run Length
    MI_COMPRESSION_DIFF = 0x80,        // Differential filter

    MI_COMPRESSION_TYPE_MASK = 0xf0,
    MI_COMPRESSION_TYPE_EX_MASK = 0xff
}
MICompressionType;


//----------------------------------------------------------------
// 圧縮データヘッダー
//
typedef struct
{
    u32     compParam:4;
    u32     compType:4;
    u32     destSize:24;

}
MICompressionHeader;



//----------------------------------------------------------------
// Bit圧縮データ展開用パラメータ
//
typedef struct
{
    u16     srcNum;                    // ソースデータ・バイト数
    u16     srcBitNum:8;               // １ソースデータ・ビット数
    u16     destBitNum:8;              // １デスティネーションデータ・ビット数
    u32     destOffset:31;             // ソースデータに加算する数
    u32     destOffset0_on:1;          // ０のデータにオフセットを加算するか否かのフラグ
}
MIUnpackBitsParam;


//======================================================================
//          圧縮データ展開
//======================================================================

//----------------------------------------------------------------------
//          Ｂｉｔ圧縮データ展開
//
//・0固定のビットを詰めたデータを展開します。
//・デスティネーションアドレスは4Byte境界に合わせて下さい。
//
//・引数：
//             void *srcp     ソースアドレス
//             void *destp    デスティネーションアドレス
//  MIUnpackBitsParam *paramp   MIUnpackBitsParam構造体のアドレス
//
//・MIUnpackBitsParam構造体
//    u16 srcNum              ソースデータ・バイト数
//    u8  srcBitNum           １ソースデータ・ビット数
//    u8  destBitNum          １デスティネーションデータ・ビット数
//    u32 destOffset:31       ソースデータに加算するオフセット数
//        destOffset0_On:1    ０のデータにオフセットを加算するか否かのフラグ
//
//・戻り値：なし
//----------------------------------------------------------------------

void    MI_UnpackBits(const void *srcp, void *destp, MIUnpackBitsParam *paramp);


//----------------------------------------------------------------------
//          ＬＺ７７圧縮データ８ｂｉｔ展開
//
//・LZ77圧縮データを展開し、8bit単位で書き込みます。
//・VRAMに直接展開することはできません。
//・圧縮データのサイズが4の倍数にならなかった場合は
//  出来るだけ0で詰めて調整して下さい。
//・ソースアドレスは4Byte境界に合わせて下さい。
//
//・引数：
//    void *srcp              ソースアドレス
//    void *destp             デスティネーションアドレス
//
//・データヘッダ
//    u32 :4                  予約
//        compType:4          圧縮タイプ（ = 1）
//        destSize:24         展開後のデータサイズ
//
//・フラグデータフォーマット
//    u8  flags               圧縮／無圧縮フラグ
//                            （0, 1） = （無圧縮データ, 圧縮データ）
//・コードデータフォーマット（Big Endian）
//    u16 length:4            展開データ長 - 3（一致長3Byte以上時のみ圧縮）
//        offset:12           一致データオフセット - 1
//
//・戻り値：なし
//----------------------------------------------------------------------

void    MI_UncompressLZ8(const void *srcp, void *destp);


//----------------------------------------------------------------------
//          ＬＺ７７圧縮データ１６ｂｉｔ展開
//
//・LZ77圧縮データを展開し、16bit単位で書き込みます。
//・データTCMやメインメモリにも展開できますが、MI_UncompressLZ778()
//  より低速です。
//・圧縮データは一致文字列を2Byte以前より検索したものにして下さい。
//・圧縮データのサイズが4の倍数にならなかった場合は
//  出来るだけ0で詰めて調整して下さい。
//・ソースアドレスは4Byte境界に合わせて下さい。
//
//・引数：
//    void *srcp              ソースアドレス
//    void *destp             デスティネーションアドレス
//
//・データヘッダ
//    u32 :4                  予約
//        compType:4          圧縮タイプ（ = 1）
//        destSize:24         展開後のデータサイズ
//
//・フラグデータフォーマット
//    u8  flags               圧縮／無圧縮フラグ
//                            （0, 1） = （無圧縮データ, 圧縮データ）
//・コードデータフォーマット（Big Endian）
//    u16 length:4            展開データ長 - 3（一致長3Byte以上時のみ圧縮）
//        offset:12           一致データオフセット（ >= 2） - 1
//
//・戻り値：なし
//----------------------------------------------------------------------

void    MI_UncompressLZ16(const void *srcp, void *destp);


//----------------------------------------------------------------------
//          ハフマン圧縮データ展開
//
//・ハフマン圧縮データを展開し、32bit単位で書き込みます。
//・圧縮データのサイズが4の倍数にならなかった場合は
//  出来るだけ0で詰めて調整して下さい。
//・ソースアドレスは4Byte境界に合わせて下さい。
//
//・引数：
//    void *srcp              ソースアドレス
//    void *destp             デスティネーションアドレス
//
//・データヘッダ
//    u32 bitSize:4           １データ・ビットサイズ（通常 4|8）
//        compType:4          圧縮タイプ（ = 2）
//        destSize:24         展開後のデータサイズ
//
//・ツリーテーブル
//    u8           treeSize        ツリーテーブルサイズ/2 - 1
//    TreeNodeData nodeRoot        ルートノード
//
//    TreeNodeData nodeLeft        ルート左ノード
//    TreeNodeData nodeRight       ルート右ノード
//
//    TreeNodeData nodeLeftLeft    左左ノード
//    TreeNodeData nodeLeftRight   左右ノード
//
//    TreeNodeData nodeRightLeft   右左ノード
//    TreeNodeData nodeRightRight  右右ノード
//
//            ・
//            ・
//
//  この後に圧縮データ本体
//
//・TreeNodeData構造体
//    u8  nodeNextOffset:6    次ノードデータへのオフセット - 1（2Byte単位）
//        rightEndFlag:1      右ノード終了フラグ
//        leftEndzflag:1       左ノード終了フラグ
//                            終了フラグがセットされている場合
//                            次ノードにデータがある
//
//・戻り値：なし
//----------------------------------------------------------------------

void    MI_UncompressHuffman(const void *srcp, void *destp);


//----------------------------------------------------------------------
//          ランレングス圧縮データ８ｂｉｔ展開
//
//・ランレングス圧縮データを展開し、8bit単位で書き込みます。
//・NITROの場合VRAMに直接展開することはできません。
//・圧縮データのサイズが4の倍数にならなかった場合は
//  出来るだけ0で詰めて調整して下さい。
//・ソースアドレスは4Byte境界に合わせて下さい。
//
//・引数：
//    void *srcp              ソースアドレス
//    void *destp             デスティネーションアドレス
//
//・データヘッダ
//    u32 :4                  予約
//        compType:4          圧縮タイプ（ = 3）
//        destSize:24         展開後のデータサイズ
//
//・フラグデータフォーマット
//    u8  length:7            展開データ長 - 1（無圧縮時）
//                            展開データ長 - 3（連続長3Byte以上時のみ圧縮）
//        flag:1              （0, 1） = （無圧縮データ, 圧縮データ）
//
//・戻り値：なし
//----------------------------------------------------------------------

void    MI_UncompressRL8(const void *srcp, void *destp);


//----------------------------------------------------------------------
//          ランレングス圧縮データ１６ｂｉｔ展開
//
//・ランレングス圧縮データを展開し、16bit単位で書き込みます。
//・データTCMやVRAMにも展開できます
//・メインRAMへ展開する場合、MI_UncompressRL8() より低速です。
//・データTCMやVRAMへ展開する場合はMI_UncompressRL8() より高速です。
//・圧縮データのサイズが4の倍数にならなかった場合は
//  出来るだけ0で詰めて調整して下さい。
//・ソースアドレスは4Byte境界に合わせて下さい。
//
//・引数：
//    void *srcp              ソースアドレス
//    void *destp             デスティネーションアドレス
//
//・データヘッダ
//    u32 :4                  予約
//        compType:4          圧縮タイプ（ = 3）
//        destSize:24         展開後のデータサイズ
//
//・フラグデータフォーマット
//    u8  length:7            展開データ長 - 1（無圧縮時）
//                            展開データ長 - 3（連続長3Byte以上時のみ圧縮）
//        flag:1              （0, 1） = （無圧縮データ, 圧縮データ）
//
//・戻り値：なし
//----------------------------------------------------------------------

void    MI_UncompressRL16(const void *srcp, void *destp);


//----------------------------------------------------------------------
//          ランレングス圧縮データ３２ｂｉｔ展開
//
//・ランレングス圧縮データを展開し、32bit単位で書き込みます。
//・MI_UncompressRL8(), MI_UncompressRL16() より高速です。
//・圧縮データのサイズが4の倍数にならなかった場合は
//  出来るだけ0で詰めて調整して下さい。
//・ソースアドレスは4Byte境界に合わせて下さい。
//
//・引数：
//    void *srcp              ソースアドレス
//    void *destp             デスティネーションアドレス
//
//・データヘッダ
//    u32 :4                  予約
//        compType:4          圧縮タイプ（ = 3）
//        destSize:24         展開後のデータサイズ
//
//・フラグデータフォーマット
//    u8  length:7            展開データ長 - 1（無圧縮時）
//                            展開データ長 - 3（連続長3Byte以上時のみ圧縮）
//        flag:1              （0, 1） = （無圧縮データ, 圧縮データ）
//
//・戻り値：なし
//----------------------------------------------------------------------

void    MI_UncompressRL32(register const void *srcp,register void *destp);


//----------------------------------------------------------------------
//          差分フィルタ変換の復元 ８ｂｉｔ展開
//
//・差分フィルタを復元し、8bit単位で書き込みます。
//・NITROの場合VRAMに直接展開することはできません。
//・圧縮データのサイズが4の倍数にならなかった場合は
//  出来るだけ0で詰めて調整して下さい。
//・ソースアドレスは4Byte境界に合わせて下さい。
//
//・引数：
//    void *srcp              ソースアドレス
//    void *destp             デスティネーションアドレス
//
//・データヘッダ
//    u32 :4                  単位ビットサイズ
//        compType:4          圧縮タイプ（ = 3）
//        destSize:24         展開後のデータサイズ
//
//・戻り値：なし
//----------------------------------------------------------------------

void    MI_UnfilterDiff8(const void *srcp, void *destp);


//----------------------------------------------------------------------
//          差分フィルタ変換の復元 １６ｂｉｔ展開
//
//・差分フィルタを復元し、16bit単位で書き込みます。
//・データTCMやVRAMにも展開できます
//・MI_UnfilterDiff16()より高速です。
//・圧縮データのサイズが4の倍数にならなかった場合は
//  出来るだけ0で詰めて調整して下さい。
//・ソースアドレスは4Byte境界に合わせて下さい。
//
//・引数：
//    void *srcp              ソースアドレス
//    void *destp             デスティネーションアドレス
//
//・データヘッダ
//    u32 :4                  単位ビットサイズ
//        compType:4          圧縮タイプ（ = 3）
//        destSize:24         展開後のデータサイズ
//
//・戻り値：なし
//----------------------------------------------------------------------

void    MI_UnfilterDiff16(const void *srcp, void *destp);

//----------------------------------------------------------------------
//          差分フィルタ変換の復元 ３２ｂｉｔ展開
//
//・差分フィルタを復元し、32bit単位で書き込みます。
//・データTCMやVRAMにも展開できます。
//  MI_Uncompress8()より高速です。//---- 
//・圧縮データのサイズが4の倍数にならなかった場合は
//  出来るだけ0で詰めて調整して下さい。
//・ソースアドレスは4Byte境界に合わせて下さい。
//
//・引数：
//    void *srcp              ソースアドレス
//    void *destp             デスティネーションアドレス
//
//・データヘッダ
//    u32 :4                  単位ビットサイズ
//        compType:4          圧縮タイプ（ = 3）
//        destSize:24         展開後のデータサイズ
//
//・戻り値：なし
//----------------------------------------------------------------------

void    MI_UnfilterDiff32(register const void *srcp,register void *destp);


//----------------------------------------------------------------------
//          差分フィルタ変換 ８ｂｉｔ展開
//
//・差分フィルタ変換を行い、8bit単位で書き込みます。
//・NITROの場合VRAMに直接展開することはできません。
//・圧縮データのサイズが4の倍数にならなかった場合は
//  出来るだけ0で詰めて調整して下さい。
//・ソースアドレスは4Byte境界に合わせて下さい。
//
//・引数：
//    void *srcp              ソースアドレス
//    void *destp             デスティネーションアドレス
//    u32  size               ソースサイズ
//    BOOL bitsize            差分サイズ (TRUE: 16bit, FALSE: 8bit)
//
//・データヘッダ
//    u32 :4                  単位ビットサイズ
//        compType:4          圧縮タイプ（ = 3）
//        destSize:24         展開後のデータサイズ
//
//・戻り値：なし
//----------------------------------------------------------------------

void    MI_FilterDiff8(register const void *srcp, register void *destp, register u32 size, register BOOL bitsize);


//----------------------------------------------------------------------
//          差分フィルタ変換 １６ｂｉｔ展開
//
//・差分フィルタ変換を行い、16bit単位で書き込みます。
//・データTCMやVRAMにも展開できます
//・MI_FilterDiff8() より高速です。
//・圧縮データのサイズが4の倍数にならなかった場合は
//  出来るだけ0で詰めて調整して下さい。
//・ソースアドレスは4Byte境界に合わせて下さい。
//
//・引数：
//    void *srcp              ソースアドレス
//    void *destp             デスティネーションアドレス
//    u32  size               ソースサイズ
//    BOOL bitsize            差分サイズ (TRUE: 16bit, FALSE: 8bit)
//
//・データヘッダ
//    u32 :4                  単位ビットサイズ
//        compType:4          圧縮タイプ（ = 3）
//        destSize:24         展開後のデータサイズ
//
//・戻り値：なし
//----------------------------------------------------------------------

void    MI_FilterDiff16(register const void *srcp, register void *destp, register u32 size, register BOOL bitsize);


//----------------------------------------------------------------------
//          差分フィルタ変換 ３２ｂｉｔ展開
//
//・差分フィルタ変換を行い、32bit単位で書き込みます。
//・MI_FilterDiff16() よりさらに高速です。
//・圧縮データのサイズが4の倍数にならなかった場合は
//  出来るだけ0で詰めて調整して下さい。
//・ソースアドレスは4Byte境界に合わせて下さい。
//
//・引数：
//    void *srcp              ソースアドレス
//    void *destp             デスティネーションアドレス
//    u32  size               ソースサイズ
//    BOOL bitsize            差分サイズ (TRUE: 16bit, FALSE: 8bit)
//
//・データヘッダ
//    u32 :4                  単位ビットサイズ
//        compType:4          圧縮タイプ（ = 3）
//        destSize:24         展開後のデータサイズ
//
//・戻り値：なし
//----------------------------------------------------------------------

void    MI_FilterDiff32(register const void *srcp, register void *destp, register u32 size, register BOOL bitsize);


//================================================================================
/*---------------------------------------------------------------------------*
  Name:         MI_GetUncompressedSize

  Description:  get data size after uncompressing.
                this function can be used for all compress type
                (LZ8, LZ16, Huffman, RL8, RL16)

  Arguments:    srcp :  compressed data address

  Returns:      size
 *---------------------------------------------------------------------------*/
static inline u32 MI_GetUncompressedSize(const void *srcp)
{
    return (*(u32 *)srcp >> 8);
}

/*---------------------------------------------------------------------------*
  Name:         MI_GetCompressionType

  Description:  get compression type from compressed data

  Arguments:    srcp :  compressed data address

  Returns:      compression type.
                MI_COMPREESION_LZ      : mean compressed by LZ77
                MI_COMPREESION_HUFFMAN : mean compressed by Huffman
                MI_COMPREESION_RL      : mean compressed by Run Length
                MI_COMPRESSION_DIFF    : mean converted by Differential filter
 *---------------------------------------------------------------------------*/
static inline MICompressionType MI_GetCompressionType(const void *srcp)
{
    return (MICompressionType)(*(u32 *)srcp & MI_COMPRESSION_TYPE_MASK);
}



/*---------------------------------------------------------------------------*
  Name:         MIi_UncompressBackward

  Description:  Uncompress special archive for module compression

  Arguments:    bottom      = Bottom adrs of packed archive + 1
                bottom[-12] = offset for top    of compressed data
                                 inp_top = bottom + bottom[-12]
                bottom[ -8] = offset for bottom of compressed data
                                 inp     = bottom + bottom[ -8]
                bottom[ -4] = offset for bottom of original data
                                 outp    = bottom + bottom[ -4]
  
                typedef struct
                {
                    int         bufferTop;
                    int         compressBottom;
                    int         originalBottom;
                }   CompFooter;

  Returns:      None.
 *---------------------------------------------------------------------------*/
// !!!! Coded in libraries/init/ARM9/crt0.c
void    MIi_UncompressBackward(void *bottom);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_MI_UNCOMPRESS_H_ */
#endif
