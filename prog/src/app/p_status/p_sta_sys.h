//======================================================================
/**
 * @file	plist_sys.h
 * @brief	�|�P�����X�e�[�^�X ���C������
 * @author	ariizumi
 * @data	09/06/10
 *
 * ���W���[�����FPSTATUS
 */
//======================================================================
#pragma once

#include "print/printsys.h"
#include "print/wordset.h"
#include "p_sta_local_def.h"


extern const BOOL PSTATUS_InitPokeStatus( PSTATUS_WORK *work );
extern const BOOL PSTATUS_TermPokeStatus( PSTATUS_WORK *work );
extern const PSTATUS_RETURN_TYPE PSTATUS_UpdatePokeStatus( PSTATUS_WORK *work );
extern void PSTATUS_SetActiveBarButton( PSTATUS_WORK *work , const BOOL isLock );

extern const POKEMON_PASO_PARAM* PSTATUS_UTIL_GetCurrentPPP( PSTATUS_WORK *work );
extern POKEMON_PARAM* PSTATUS_UTIL_GetCurrentPP( PSTATUS_WORK *work );
extern void PSTATUS_UTIL_SetCurrentPPPFast( PSTATUS_WORK *work , const BOOL isFast );

//�����`��
extern void PSTATUS_UTIL_DrawStrFuncBmp( PSTATUS_WORK *work , GFL_BMP_DATA *bmpData , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col );
extern void PSTATUS_UTIL_DrawStrFunc( PSTATUS_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col );
extern void PSTATUS_UTIL_DrawStrFuncRight( PSTATUS_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col );
extern void PSTATUS_UTIL_DrawValueStrFuncBmp( PSTATUS_WORK *work , GFL_BMP_DATA *bmpData , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col );
extern void PSTATUS_UTIL_DrawValueStrFunc( PSTATUS_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col );
extern void PSTATUS_UTIL_DrawValueStrFuncRightBmp( PSTATUS_WORK *work , GFL_BMP_DATA *bmpData ,
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col );
extern void PSTATUS_UTIL_DrawValueStrFuncRight( PSTATUS_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col );

