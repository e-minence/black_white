/*---------------------------------------------------------------------------*
  Project:  TWL-System - libraries - g2d - src - load
  File:     g2d_NOB_load.c

  Copyright 2004-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1155 $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/g2d/load/g2d_NCE_load.h>
#include <nnsys/g2d/g2d_Load.h>
#include "g2di_Debug.h"


//------------------------------------------------------------------------------
static void* GetPtrOamArrayHead_( NNSG2dCellDataBank* pCellBank )
{
    NNS_G2D_NULL_ASSERT( pCellBank );
    
    if( NNSi_G2dCellBankHasBR( pCellBank ) ) 
    {
        return (NNSG2dCellDataWithBR*)(pCellBank->pCellDataArrayHead) + pCellBank->numCells;
    }else{
        return pCellBank->pCellDataArrayHead + pCellBank->numCells;
    }   
}



//------------------------------------------------------------------------------
// ユーザ拡張領域の情報を展開します
// 現在は単数ですが、実際には複数のブロックがユーザ拡張領域に格納される場合がありえます。
static void UnPackExtendedData_( void* pExData )
{
    NNS_G2D_NULL_ASSERT( pExData );
    {    
        NNSG2dUserExDataBlock*    pBlk          = (NNSG2dUserExDataBlock*)pExData;
        NNSG2dUserExCellAttrBank* pCellAttrBank = (NNSG2dUserExCellAttrBank*)(pBlk + 1);
    
        NNSi_G2dUnpackUserExCellAttrBank( pCellAttrBank );        
    }
}


//------------------------------------------------------------------------------
BOOL NNS_G2dGetUnpackedCellBank( void* pNcerFile, NNSG2dCellDataBank** ppCellBank )
{
    NNS_G2D_NULL_ASSERT( pNcerFile );
    NNS_G2D_NULL_ASSERT( ppCellBank );
    
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileSignatureValid( pNcerFile, 
                                                        NNS_G2D_BINFILE_SIG_CELL ),
                       "Input file signature is invalid for this method." );
                     
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileVersionValid( pNcerFile, 
                                                      BIN_FILE_VERSION( NCER ) ), 
                       "Input file is obsolete. Please use the new g2dcvtr.exe." );
                                                     
    {
        NNSG2dBinaryFileHeader*     pBinFile    = (NNSG2dBinaryFileHeader*)pNcerFile;
        {
            NNSG2dCellDataBankBlock* pBinBlk   =
                (NNSG2dCellDataBankBlock*)NNS_G2dFindBinaryBlock( pBinFile, 
                                                                   NNS_G2D_BLKSIG_CELLBANK );
            if( pBinBlk )
            {
                NNS_G2dUnpackNCE( (void*)&pBinBlk->cellDataBank );
                //
                // OK 
                //
                *ppCellBank = &pBinBlk->cellDataBank;
                return TRUE;
            }else{
                //
                // NG
                //
                *ppCellBank = NULL;
                return FALSE;
                
            }
        }
    }
}

//------------------------------------------------------------------------------
const NNSG2dCellData* NNS_G2dGetCellDataByIdx( const NNSG2dCellDataBank* pCellData, u16 idx )
{
    NNS_G2D_NULL_ASSERT( pCellData );
    
    // 
    // 不正なインデックスはNULLを返す
    //
    if( idx >= pCellData->numCells )
    {
        return NULL;
    }
    
    //
    // 種類によってアクセス方法を変化させる必要があります
    //
    if( NNSi_G2dCellBankHasBR( pCellData ) )
    {
        const NNSG2dCellDataWithBR* pCellBR = 
            (const NNSG2dCellDataWithBR*)(pCellData->pCellDataArrayHead) + idx;
        return &pCellBR->cellData;
    }else{
        return pCellData->pCellDataArrayHead + idx;
    }
}




//------------------------------------------------------------------------------
void NNS_G2dUnpackNCE( NNSG2dCellDataBank* pCellData )
{
    NNS_G2D_NULL_ASSERT( pCellData );
    {
        //
        // Unpack offset pointer to the cell data. 
        //
        pCellData->pCellDataArrayHead 
            = NNS_G2D_UNPACK_OFFSET_PTR( pCellData->pCellDataArrayHead, pCellData );
        
        //
        // Unpack offset pointer to the OBJ data. 
        //
        {
            void* pHeadOfOAMData = GetPtrOamArrayHead_( pCellData );
            
            u16 i;
            NNSG2dCellData* pCell = NULL;
            for( i = 0; i < pCellData->numCells; i++ )
            {
                pCell = (NNSG2dCellData*)(NNS_G2dGetCellDataByIdx( pCellData, i ));
                pCell->pOamAttrArray 
                    = NNS_G2D_UNPACK_OFFSET_PTR( pCell->pOamAttrArray, pHeadOfOAMData );
            }
        }
        
        //
        // Unpack offset pointer to the Vram transform data. 
        //
        if( pCellData->pVramTransferData != NULL )
        {
            NNSG2dVramTransferData* pVramTsfmData 
                = NNS_G2D_UNPACK_OFFSET_PTR( pCellData->pVramTransferData, pCellData );
            
            pVramTsfmData->pCellTransferDataArray 
                = NNS_G2D_UNPACK_OFFSET_PTR( pVramTsfmData->pCellTransferDataArray, pVramTsfmData );
                            
            pCellData->pVramTransferData = pVramTsfmData;
        }
        
        // 
        // ユーザ拡張アトリビュートを展開します
        //
        if( pCellData->pExtendedData != NULL )
        {
            pCellData->pExtendedData
               = NNS_G2D_UNPACK_OFFSET_PTR( pCellData->pExtendedData, pCellData );    
            
            UnPackExtendedData_( pCellData->pExtendedData );
        }
        
    }
    NNSI_G2D_DEBUGMSG0("Unpacking NCER file is successful.\n" );
}



#ifndef SDK_FINALROM 

//------------------------------------------------------------------------------
void NNS_G2dPrintOBJAttr( const NNSG2dCellOAMAttrData* pOBJ )
{
    NNS_G2D_NULL_ASSERT( pOBJ );
    OS_Printf( "OBJ_0 = %x\n", pOBJ->attr0 );
    OS_Printf( "OBJ_1 = %x\n", pOBJ->attr1 );
    OS_Printf( "OBJ_2 = %x\n\n", pOBJ->attr2 );
}

//------------------------------------------------------------------------------
static void PrintCellBoundingRect_( const NNSG2dCellBoundingRectS16* pBR )
{
    NNS_G2D_NULL_ASSERT( pBR );
    OS_Printf( "maxX = %d\n", pBR->maxX );
    OS_Printf( "maxY = %d\n", pBR->maxY );
    OS_Printf( "minX = %d\n", pBR->minX );
    OS_Printf( "minY = %d\n", pBR->minY );
}

//------------------------------------------------------------------------------
void NNS_G2dPrintCellInfo( const NNSG2dCellData* pCell )
{
    u16         i;
    NNS_G2D_NULL_ASSERT( pCell );
    
    OS_Printf( "-------------------\n" );
    {
        const NNSG2dCellOAMAttrData*    pOBJ = pCell->pOamAttrArray;
        
        OS_Printf( "numOBJ = %d\n", pCell->numOAMAttrs );
        
        //
        // BoundingRect を 持つ Cell ならば表示する
        //
        if( NNSi_G2dCellHasBR( pCell ) )
        {
            const NNSG2dCellDataWithBR* pCellBR 
                = (const NNSG2dCellDataWithBR*)( pCell );
            PrintCellBoundingRect_( &pCellBR->boundingRect );
        }
        
        for( i = 0; i < pCell->numOAMAttrs; i++ )
        {
            NNS_G2dPrintOBJAttr( &pOBJ[i] );    
        }
    }
    OS_Printf( "-------------------\n" );
}

//------------------------------------------------------------------------------
static void PrintVramTransformData_
( 
    const NNSG2dVramTransferData*   pVramTsfmData, 
    u16                             numCells 
)
{
    u16 i;
    NNS_G2D_NULL_ASSERT( pVramTsfmData );
    
    for( i = 0; i < numCells; i++ )
    {
        const NNSG2dCellVramTransferData* pTsfmData 
            = &pVramTsfmData->pCellTransferDataArray[i];
        
        OS_Printf( "srcDataOffset   = %d\n", pTsfmData->srcDataOffset );
        OS_Printf( "szByte          = %d\n", pTsfmData->szByte );
    }
}

//------------------------------------------------------------------------------
// 拡張情報を表示します
static void PrintCellExtendedData_( const void* pExData )
{
    NNS_G2D_NULL_ASSERT( pExData ); 
    {    
        NNSG2dUserExDataBlock*    pBlk          = (NNSG2dUserExDataBlock*)pExData;
        NNSG2dUserExCellAttrBank* pCellAttrBank = (NNSG2dUserExCellAttrBank*)(pBlk + 1);
        
        NNSi_G2dPrintUserExCellAttrBank( pCellAttrBank );
    }
}

//------------------------------------------------------------------------------
void NNS_G2dPrintCellBank( const NNSG2dCellDataBank*    pCellBank )
{
    u16         i;
    const NNSG2dCellData*   pCell;
    OS_Printf( "---------------------------------------------\n" );
    OS_Printf( "numCell = %d\n", pCellBank->numCells );
    for( i = 0; i < pCellBank->numCells; i++ )
    {
        OS_Printf( "Cell Idx = %d\n", i );
        pCell = NNS_G2dGetCellDataByIdx( pCellBank, i );
        NNS_G2dPrintCellInfo( pCell );
    }
    
    //
    // Vram転送情報
    //
    if( pCellBank->pVramTransferData != NULL )
    {
        OS_Printf( "--------- VramTransform Data\n" );
        {
            const NNSG2dVramTransferData* pVramTsfmData 
                = (const NNSG2dVramTransferData*)pCellBank->pVramTransferData;
            
            OS_Printf( "szByteMax   = %d\n", pVramTsfmData->szByteMax );
            PrintVramTransformData_( pVramTsfmData, pCellBank->numCells );
        }    
    }
    
    //
    // 拡張情報
    //
    if( pCellBank->pExtendedData != NULL )
    { 
        PrintCellExtendedData_( pCellBank->pExtendedData );
    }
    
    OS_Printf( "---------------------------------------------\n" );
}

#endif // SDK_FINALROM 

