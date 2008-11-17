//=============================================================================================
/**
 * @file	btl_calc.c
 * @brief	ポケモンWB バトル -システム-	各種、数値の計算ルーチン
 * @author	taya
 *
 * @date	2008.09.06	作成
 */
//=============================================================================================
#include <gflib.h>

#include "btl_common.h"
#include "btl_calc.h"

#include "btl_util.h"

//--------------------------------------------------------------
/**
 *	ステータスランク補正テーブル
 */
//--------------------------------------------------------------
static const struct {
	u8  num;
	u8  denom;
}StatusRankTable[] = {
	{ 2, 8 },	// x(2/8) = 25%
	{ 2, 7 },	// x(2/7) = 29%
	{ 2, 6 },	// x(2/6) = 33%
	{ 2, 5 },	// x(2/5) = 40%
	{ 2, 4 },	// x(2/4) = 50%
	{ 2, 3 },	// x(2/3) = 67%
	{ 2, 2 },	// x(2/2) = 100%
	{ 3, 2 },	// x(3/2) = 150%
	{ 4, 2 },	// x(4/2) = 200%
	{ 5, 2 },	// x(5/2) = 250%
	{ 6, 2 },	// x(6/2) = 300%
	{ 7, 2 },	// x(7/2) = 350%
	{ 8, 2 },	// x(8/2) = 400%
};

u16 BTL_CALC_StatusRank( u16 defaultVal, u8 rank )
{
	GF_ASSERT(rank < NELEMS(StatusRankTable));

	{
		defaultVal = defaultVal * StatusRankTable[rank].num;
		defaultVal /= StatusRankTable[rank].denom;
		return defaultVal;
	}
}


//--------------------------------------------------------------
/**
 *	的中率補正テーブル
 */
//--------------------------------------------------------------
static const struct {
	u8  num;
	u8  denom;
}HitRankTable[] = {
	{   6,  18 },
	{   6,  16 },
	{   6,  14 },
	{   6,  12 },
	{   6,  10 },
	{   6,   8 },
	{   6,   6 },
	{   8,   6 },
	{  10,   6 },
	{  12,   6 },
	{  14,   6 },
	{  16,   6 },
	{  18,   6 },
};

u8 BTL_CALC_HitPer( u8 defPer, u8 rank )
{
	GF_ASSERT(rank < NELEMS(HitRankTable));

	{
		u32 per = defPer * HitRankTable[rank].num / HitRankTable[rank].denom;
		if( per > 100 )
		{
			per = 100;
		}
		return per;
	}
}

//--------------------------------------------------------------
/**
 *	クリティカル率テーブル
 */
//--------------------------------------------------------------
static const u8 CriticalRankTable[] = {
	16, 8, 4, 3, 2,
};

BOOL BTL_CALC_CheckCritical( u8 rank )
{
	GF_ASSERT(rank < NELEMS(CriticalRankTable));

	return (GFL_STD_MtRand( CriticalRankTable[rank] ) == 0);
}

//--------------------------------------------------------------
/**
 *	タイプ相性テーブル
 */
//--------------------------------------------------------------
BtlTypeAff BTL_CALC_TypeAff( PokeType wazaType, PokeTypePair defenderType )
{
	enum {
		x0 = 0,
		xH = 2,
		x1 = 4,
		x2 = 8,

		RESULT_0   = 0,
		RESULT_25  = 1,
		RESULT_50  = 2,
		RESULT_100 = 4,
		RESULT_200 = 8,
		RESULT_400 = 16,
	};

	static const u8 affTbl[ POKETYPE_NUMS ][ POKETYPE_NUMS ] = {
//			  NML, BTL, HIK, POI, JIM, IWA, MUS, GHO, MET, HAT, FIR, WAT, KUS, ELE, ESP, KOR, DRA, AKU,
/*NORMAL*/	{  x1,  x1,  x1,  x1,  x1,  xH,  x1,  x0,  xH,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1, },
/*BATTLE*/	{  x2,  x1,  xH,  xH,  x1,  x2,  xH,  x0,  x2,  x1,  x1,  x1,  x1,  x1,  xH,  x2,  x1,  x2, },
/*HIKOU*/	{  x1,  x2,  x1,  x1,  x1,  xH,  x2,  x1,  xH,  x1,  x1,  x1,  x2,  xH,  x1,  x1,  x1,  x1, },
/*POISON*/	{  x1,  x1,  x1,  xH,  xH,  xH,  x1,  xH,  x0,  x1,  x1,  x1,  x2,  x1,  x1,  x1,  x1,  x1, },
/*JIMEN*/	{  x1,  x1,  x0,  x2,  x1,  x2,  xH,  x1,  x2,  x1,  x2,  x1,  xH,  x2,  x1,  x1,  x1,  x1, },
/*IWA*/		{  x1,  xH,  x2,  x1,  xH,  x1,  x2,  x1,  xH,  x1,  x2,  x1,  x1,  x1,  x1,  x2,  x1,  x1, },
/*MUSHI*/	{  x1,  xH,  xH,  xH,  x1,  x1,  x1,  xH,  xH,  x1,  xH,  x1,  x2,  x1,  x2,  x1,  x1,  x2, },
/*GHOST*/	{  x0,  x1,  x1,  x1,  x1,  x1,  x1,  x2,  xH,  x1,  x1,  x1,  x1,  x1,  x2,  x1,  x1,  xH, },
/*METAL*/	{  x1,  x1,  x1,  x1,  x1,  x2,  x1,  x1,  xH,  x1,  xH,  xH,  x1,  xH,  x1,  x2,  x1,  x1, },
/*HATE*/	{  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1, },
/*FIRE*/	{  x1,  x1,  x1,  x1,  x1,  xH,  x2,  x1,  x2,  x1,  xH,  xH,  x2,  x1,  x1,  x2,  xH,  x1, },
/*WATER*/	{  x1,  x1,  x1,  x1,  x2,  x2,  x1,  x1,  x1,  x1,  x2,  xH,  xH,  x1,  x1,  x1,  xH,  x1, },
/*KUSA*/	{  x1,  x1,  xH,  xH,  x2,  x2,  xH,  x1,  xH,  x1,  xH,  x2,  xH,  x1,  x1,  x1,  xH,  x1, },
/*ELEC*/	{  x1,  x1,  x2,  x1,  x0,  x1,  x1,  x1,  x1,  x1,  x1,  x2,  xH,  xH,  x1,  x1,  xH,  x1, },
/*ESPER*/	{  x1,  x2,  x1,  x2,  x1,  x1,  x1,  x1,  xH,  x1,  x1,  x1,  x1,  x1,  xH,  x1,  x1,  x0, },
/*KOORI*/	{  x1,  x1,  x2,  x1,  x2,  x1,  x1,  x1,  xH,  x1,  xH,  xH,  x2,  x1,  x1,  xH,  x2,  x1, },
/*DRAGON*/	{  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  xH,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x2,  x1, },
/*AKU*/		{  x1,  xH,  x1,  x1,  x1,  x1,  x1,  x2,  xH,  x1,  x1,  x1,  x1,  x1,  x2,  x1,  x1,  xH, },
	};

	PokeType defType1, defType2;
	u8 result;

	PokeTypePair_Split( defenderType, &defType1, &defType2 );
	result = affTbl[ wazaType ][ defType1 ];
	if( defType2 != defType1 )
	{
		result = (result * affTbl[ wazaType ][ defType2 ]) / x1;
	}

	switch( result ){
	case RESULT_0:		return BTL_TYPEAFF_0;
	case RESULT_25:		return BTL_TYPEAFF_25;
	case RESULT_50:		return BTL_TYPEAFF_50;
	case RESULT_100:	return BTL_TYPEAFF_100;
	case RESULT_200:	return BTL_TYPEAFF_200;
	case RESULT_400:	return BTL_TYPEAFF_400;
	}

	{
		BTL_Printf("wazaType=%d, pokeType1=%d, pokeType2=%d, defType=%04x\n", wazaType, defType1, defType2, defenderType);
		GF_ASSERT(0);
	}
	return BTL_TYPEAFF_100;
}
u32 BTL_CALC_AffDamage( u32 rawDamage, BtlTypeAff aff )
{
	switch( aff ){
	case BTL_TYPEAFF_0:		return 0;
	case BTL_TYPEAFF_25:	return (rawDamage / 4);
	case BTL_TYPEAFF_50:	return (rawDamage / 2);
	case BTL_TYPEAFF_100:	return rawDamage;
	case BTL_TYPEAFF_200:	return rawDamage * 2;
	case BTL_TYPEAFF_400:	return rawDamage * 4;
	default:
		GF_ASSERT(0);
		return rawDamage;
	}
}



//=============================================================================================
/**
 * 最大ヒット回数を受け取り、実際にヒットする回数（ランダム補正）を返す
 *
 * @param   numHitMax		
 *
 * @retval  u8		
 */
//=============================================================================================
u8 BTL_CALC_HitCountMax( u8 numHitMax )
{
	enum {
		HIT_COUNT_MIN = 2,
		HIT_COUNT_MAX = 5,

		HIT_COUNT_RANGE = HIT_COUNT_MAX - HIT_COUNT_MIN + 1,
	};

	GF_ASSERT(numHitMax>=HIT_COUNT_MIN);
	GF_ASSERT(numHitMax<=HIT_COUNT_MAX);

	{
		static const u8 perTbl[ HIT_COUNT_RANGE ][ HIT_COUNT_RANGE ] = {
/* 2 */			{  100, 100, 100, 100 },
/* 3 */			{    0, 100, 100, 100 },
/* 4 */			{   35,  70, 100, 100 },
/* 5 */			{   30,  60,  80, 100 },
		};

		u8  max, p, i;

		p = GFL_STD_MtRand( 100 );
		max = numHitMax - HIT_COUNT_MIN;
		for(i=0; i<HIT_COUNT_RANGE; i++)
		{
			if( p < perTbl[max][i] )
			{
				break;
			}
		}
		return i + HIT_COUNT_MIN;
	}
}

