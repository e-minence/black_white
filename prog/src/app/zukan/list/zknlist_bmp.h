//============================================================================================
/**
 * @file		zknlist_bmp.h
 * @brief		}ΣXgζΚ aloΦA
 * @author	Hiroyuki Nakamura
 * @date		09.12.14
 *
 *	W[ΌFZKNLISTBMP
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
 * @param		wk		}ΣXg[N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTBMP_Init( ZKNLISTMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		aloΦAν
 *
 * @param		wk		}ΣXg[N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTBMP_Exit( ZKNLISTMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		vgC
 *
 * @param		wk		}ΣXg[N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTBMP_PrintUtilTrans( ZKNLISTMAIN_WORK * wk );

extern void ZKNLISTBMP_PutPokeEntryStr( ZKNLISTMAIN_WORK * wk );

extern void ZKNLISTBMP_PutPokeSearchStr( ZKNLISTMAIN_WORK * wk );

extern void ZKNLISTBMP_PutPokeList( ZKNLISTMAIN_WORK * wk, u32 winIdx, s32 listPos );


extern void ZKNLISTBMP_PutPokeList2( ZKNLISTMAIN_WORK * wk, PRINT_UTIL * util, STRBUF * name, u32 num );

