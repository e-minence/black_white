//============================================================================================
/**
 * @file		zknsearch_bmp.h
 * @brief		}ΣυζΚ aloΦA
 * @author	Hiroyuki Nakamura
 * @date		10.02.09
 *
 *	W[ΌFZKNSEARCHBMP
 */
//============================================================================================
#pragma	once


//============================================================================================
//	vg^CvιΎ
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		aloΦAϊ»
 *
 * @param		wk		}ΣυζΚ[N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHBMP_Init( ZKNSEARCHMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		aloΦAν
 *
 * @param		wk		}ΣυζΚ[N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHBMP_Exit( ZKNSEARCHMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		vgC
 *
 * @param		wk		}ΣυζΚ[N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHBMP_PrintUtilTrans( ZKNSEARCHMAIN_WORK * wk );


extern void ZKNSEARCHBMP_PutMainPage( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHBMP_PutRowPage( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHBMP_PutNamePage( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHBMP_PutTypePage( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHBMP_PutColorPage( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHBMP_PutFormPage( ZKNSEARCHMAIN_WORK * wk );

extern void ZKNSEARCHBMP_PutListItem( ZKNSEARCHMAIN_WORK * wk, PRINT_UTIL * util, STRBUF * str );

extern void ZKNSEARCHBMP_PutFormListItem( ZKNSEARCHMAIN_WORK * wk, PRINT_UTIL * util );
