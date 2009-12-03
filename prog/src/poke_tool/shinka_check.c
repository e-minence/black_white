//============================================================================================
/**
 * @file    shinka_check.c
 * @bfief   進化チェック関連関数群
 * @author  HisashiSogabe
 * @date    09.12.02
 */
//============================================================================================
#include <gflib.h>
#include "item/item.h"
#include "poke_tool/monsno_def.h"
#include "system/rtc_tool.h"

#include "poke_tool/shinka_check.h"
#include "shinka_def.h"

#include "arc_def.h"

//============================================================================================
/**
 *	構造体宣言
 */
//============================================================================================
struct pokemon_shinka_data{
	u16	ShinkaCond;
	u16	ShinkaData;
	u16	ShinkaMons;
};

struct pokemon_shinka_table{
	POKEMON_SHINKA_DATA	psd[7];
};

static	void	PokeShinkaDataGet( int mons_no, POKEMON_SHINKA_TABLE *pst );
static  BOOL  PokeWazaHaveCheck( POKEMON_PARAM* pp, WazaID waza_no );

//============================================================================================
/**
 *	進化チェック
 *
 *	@param[in]	ppt			    チェックするポケパーティ構造体のポインタ
 *	@param[in]	pp			    チェックするポケモンの構造体のポインタ
 *	@param[in]	cond		    進化条件
 *	@param[in]	param		    アイテム進化チェック時は使用アイテム
 *	                        場所進化の場合は、shinka_place_mode
 *	@param[out]	shinka_cond 進化条件格納ワークへのポインタ
 *	@param[in]	heapID      ヒープID
 *
 *	@retval	0 進化せず 0以外 進化したポケモンナンバー
 */
//============================================================================================
u16	SHINKA_Check( POKEPARTY *ppt, POKEMON_PARAM *pp, SHINKA_COND cond, u16 param, SHINKA_COND *shinka_cond, HEAPID heapID )
{
	POKEMON_SHINKA_TABLE	*pst;
	u16	mons_no;
	u16	item_no;
	u8	level;
	int	i;
	u16	ret=0;
	u16	friend;
	u32	personal_rnd;
	u8	eqp;
	u8	beautiful;
	u16	rnd = 0;
	SHINKA_COND	dummy;

	mons_no       = PP_Get( pp, ID_PARA_monsno, NULL );
	item_no       = PP_Get( pp, ID_PARA_item, NULL );
	personal_rnd  = PP_Get( pp, ID_PARA_personal_rnd, NULL );
	beautiful     = PP_Get( pp, ID_PARA_beautiful, NULL );
	rnd = ( personal_rnd & 0xffff0000 ) >> 16;

	eqp = ITEM_GetParam( item_no, ITEM_PRM_EQUIP, heapID );

	//ユンゲラーは絶対進化
	if( mons_no != MONSNO_YUNGERAA )
  {
		//アイテムで進化しない装備効果は進化しない
		if( ( eqp == SOUBI_SINKASINAI ) && ( cond != SHINKA_COND_ITEM_CHECK ) )
    {
			return 0;
		}
	}
	//ぎざみみピチューは進化しない
#if 0
	if( ( mons_no == MONSNO_PITYUU ) && ( PP_Get( pp, ID_PARA_form_no, NULL ) == FORMNO_PITYUU_GIZAMIMI ) )
  {
		return 0;
	}		
#endif

	if( shinka_cond == NULL )
  {
		shinka_cond = &dummy;
	}

	pst = GFL_HEAP_AllocMemory( heapID, sizeof( POKEMON_SHINKA_TABLE ) );
	PokeShinkaDataGet( mons_no, pst );

	switch( cond ){
	case SHINKA_COND_LEVELUP:
		level   = PP_Get( pp, ID_PARA_level, NULL );
		friend  = PP_Get( pp, ID_PARA_friend, NULL );
		for( i = 0 ; i < SHINKA_COND_MAX ; i++ )
    {
			switch( pst->psd[ i ].ShinkaCond ){
			case SHINKA_FRIEND_HIGH:
				if( SHINKA_FRIEND <= friend )
        {
					ret = pst->psd[ i ].ShinkaMons;
					(*shinka_cond) = SHINKA_FRIEND_HIGH;
				}
				break;
			case SHINKA_FRIEND_HIGH_NOON:
				if( ( GFL_RTC_IsNightTime() == FALSE ) && ( SHINKA_FRIEND <= friend ) )
        {
					ret = pst->psd[ i ].ShinkaMons;
					(*shinka_cond) = SHINKA_FRIEND_HIGH_NOON;
				}
				break;
			case SHINKA_FRIEND_HIGH_NIGHT:
				if( ( GFL_RTC_IsNightTime() == TRUE ) && ( SHINKA_FRIEND <= friend ) )
        {
					ret = pst->psd[ i ].ShinkaMons;
					(*shinka_cond) = SHINKA_FRIEND_HIGH_NIGHT;
				}
				break;
			case SHINKA_LEVELUP:
				if( pst->psd[ i ].ShinkaData <= level )
        {
					ret = pst->psd[ i ].ShinkaMons;
					(*shinka_cond) = SHINKA_LEVELUP;
				}
				break;
			case SHINKA_SPECIAL_POW:
				if( pst->psd[ i ].ShinkaData <= level )
        {
					if( ( PP_Get( pp, ID_PARA_pow, NULL ) ) > ( PP_Get( pp, ID_PARA_def, NULL ) ) )
          {
						ret = pst->psd[ i ].ShinkaMons;
						(*shinka_cond) = SHINKA_SPECIAL_POW;
					}
				}
				break;
			case SHINKA_SPECIAL_EVEN:
				if( pst->psd[ i ].ShinkaData <= level )
        {
					if( ( PP_Get( pp, ID_PARA_pow, NULL ) ) == ( PP_Get ( pp, ID_PARA_def, NULL ) ) )
          {
						ret = pst->psd[ i ].ShinkaMons;
						(*shinka_cond) = SHINKA_SPECIAL_EVEN;
					}
				}
				break;
			case SHINKA_SPECIAL_DEF:
				if( pst->psd[ i ].ShinkaData <= level )
        {
					if( ( PP_Get( pp, ID_PARA_pow, NULL ) ) < ( PP_Get( pp, ID_PARA_def, NULL ) ) )
          {
						ret = pst->psd[ i ].ShinkaMons;
						(*shinka_cond) = SHINKA_SPECIAL_DEF;
					}
				}
				break;
			case SHINKA_SPECIAL_RND_EVEN:		//個性乱数を10で割った余りが５未満
				if( pst->psd[ i ].ShinkaData <= level)
        {
					if( ( rnd % 10 ) < 5 )
          {
						ret = pst->psd[ i ].ShinkaMons;
						(*shinka_cond) = SHINKA_SPECIAL_RND_EVEN;
					}
				}
				break;
			case SHINKA_SPECIAL_RND_ODD:		//個性乱数を10で割った余りが５以上
				if( pst->psd[ i ].ShinkaData <= level )
        {
					if( ( rnd % 10 ) >= 5 )
          {
						ret = pst->psd[ i ].ShinkaMons;
						(*shinka_cond) = SHINKA_SPECIAL_RND_ODD;
					}
				}
				break;
			case SHINKA_SPECIAL_LEVELUP:		//手持ちに空きがあれば、進化しつつ、カラを残す
				if( pst->psd[ i ].ShinkaData <= level )
        {
					ret = pst->psd[ i ].ShinkaMons;
					(*shinka_cond) = SHINKA_SPECIAL_LEVELUP;
				}
				break;
			case SHINKA_SPECIAL_NUKENIN:		//手持ちに空きがあれば
				(*shinka_cond) = SHINKA_SPECIAL_NUKENIN;
				break;
			case SHINKA_SPECIAL_BEAUTIFUL:		//美しさで進化
				if( pst->psd[ i ].ShinkaData <= beautiful )
        {
					ret = pst->psd[ i ].ShinkaMons;
					(*shinka_cond) = SHINKA_SPECIAL_BEAUTIFUL;
				}
				break;
			case SHINKA_SOUBI_NOON:
				if( ( GFL_RTC_IsNightTime() == FALSE ) && ( pst->psd[ i ].ShinkaData == item_no ) )
        {
					ret = pst->psd[ i ].ShinkaMons;
					(*shinka_cond) = SHINKA_SOUBI_NOON;
				}
				break;
			case SHINKA_SOUBI_NIGHT:
				if( ( GFL_RTC_IsNightTime() == TRUE ) && ( pst->psd[ i ].ShinkaData == item_no ) )
        {
					ret = pst->psd[ i ].ShinkaMons;
					(*shinka_cond) = SHINKA_SOUBI_NIGHT;
				}
				break;
			case SHINKA_WAZA:
				if( PokeWazaHaveCheck( pp, pst->psd[ i ].ShinkaData ) == TRUE )
        {
					ret = pst->psd[ i ].ShinkaMons;
					(*shinka_cond) = SHINKA_WAZA;
				}
				break;
			case SHINKA_POKEMON:
				if( ppt != NULL )
        {
					if( PokeParty_PokemonCheck( ppt, pst->psd[ i ].ShinkaData ) == TRUE )
          {
						ret = pst->psd[ i ].ShinkaMons;
						(*shinka_cond) = SHINKA_POKEMON;
					}
				}
				break;
			case SHINKA_MALE:
				if( ( PP_Get( pp, ID_PARA_sex, NULL ) == PTL_SEX_MALE ) && ( pst->psd[ i ].ShinkaData <= level ) )
        {
					ret = pst->psd[ i ].ShinkaMons;
					(*shinka_cond) = SHINKA_MALE;
				}
				break;
			case SHINKA_FEMALE:
				if( ( PP_Get( pp, ID_PARA_sex, NULL ) == PTL_SEX_FEMALE ) && ( pst->psd[ i ].ShinkaData <= level ) )
        {
					ret = pst->psd[ i ].ShinkaMons;
					(*shinka_cond) = SHINKA_FEMALE;
				}
				break;
			case SHINKA_PLACE_TENGANZAN:
				if( pst->psd[ i ].ShinkaCond ==param)
        {
					ret = pst->psd[ i ].ShinkaMons;
					(*shinka_cond) = SHINKA_PLACE_TENGANZAN;
				}
				break;
			case SHINKA_PLACE_KOKE:
				if( pst->psd[ i ].ShinkaCond == param )
        {
					ret = pst->psd[ i ].ShinkaMons;
					(*shinka_cond) = SHINKA_PLACE_KOKE;
				}
				break;
			case SHINKA_PLACE_ICE:
				if( pst->psd[ i ].ShinkaCond == param )
        {
					ret = pst->psd[ i ].ShinkaMons;
					(*shinka_cond) = SHINKA_PLACE_ICE;
				}
				break;
			}
			//進化条件が見つかったら、抜ける
			if(ret){
				break;
			}
		}
		break;
	case SHINKA_COND_TUUSHIN:
		for( i = 0 ; i < SHINKA_COND_MAX ; i++ )
    {
			switch( pst->psd[ i ].ShinkaCond ){
			case SHINKA_TUUSHIN:
				ret = pst->psd[ i ].ShinkaMons;
				(*shinka_cond) = SHINKA_TUUSHIN;
				break;
			case SHINKA_TUUSHIN_ITEM:
				if( pst->psd[ i ].ShinkaData == item_no )
        {
					ret = pst->psd[ i ].ShinkaMons;
					(*shinka_cond) = SHINKA_TUUSHIN_ITEM;
				}
				break;
			}
			//進化条件が見つかったら、抜ける
			if( ret )
      {
				break;
			}
		}
		break;
	case SHINKA_COND_ITEM:
	case SHINKA_COND_ITEM_CHECK:
		for( i = 0 ; i < SHINKA_COND_MAX ; i++ )
    {
			if( ( pst->psd[ i ].ShinkaCond == SHINKA_ITEM ) && ( pst->psd[ i ].ShinkaData == param ) )
      {
				ret = pst->psd[ i ].ShinkaMons;
				(*shinka_cond) = 0;
				break;
			}
			if( ( pst->psd[ i ].ShinkaCond == SHINKA_ITEM_MALE ) &&
			    ( PP_Get( pp, ID_PARA_sex, NULL ) == PTL_SEX_MALE ) &&
			    ( pst->psd[ i ].ShinkaData == param ) )
      {
				ret = pst->psd[ i ].ShinkaMons;
				(*shinka_cond) = 0;
				break;
			}
			if( ( pst->psd[ i ].ShinkaCond == SHINKA_ITEM_FEMALE ) &&
			    ( PP_Get( pp, ID_PARA_sex, NULL ) == PTL_SEX_FEMALE ) &&
			    ( pst->psd[ i ].ShinkaData == param ) )
      {
				ret = pst->psd[ i ].ShinkaMons;
				(*shinka_cond) = 0;
				break;
			}
			//進化条件が見つかったら、抜ける
			if( ret )
      {
				break;
			}
		}
		break;
	}
	GFL_HEAP_FreeMemory( pst );
	return ret;
}

//============================================================================================
/**
 *	進化テーブルのArcHandleを取得
 *
 * @param[in]	heapID	ArcHandleを確保するためのheapID
 */
//============================================================================================
ARCHANDLE*  SHINKA_GetArcHandle( HEAPID heapID )
{ 
  return GFL_ARC_OpenDataHandle( ARCID_EVOTBL, heapID );
}

//============================================================================================
/**
 *	進化テーブルのパラメータをArcHandleを介して取得
 *
 * @param[in]	handle	ArcHandle
 * @param[in]	mons_no 取得するポケモンナンバー
 * @param[in]	index	  何番目のデータを取得するか（最大７）
 * @param[in]	param	  取得するパラメータ
 */
//============================================================================================
u16 SHINKA_GetParamByHandle( ARCHANDLE* handle, int mons_no, int index, SHINKA_PARAM param )
{ 
  POKEMON_SHINKA_TABLE  pst;

  GF_ASSERT( index < SHINKA_COND_MAX );

  GFL_ARC_LoadDataByHandle( handle, mons_no, &pst );

  switch( param ){ 
  case SHINKA_PARAM_COND:
    return pst.psd[ index ].ShinkaCond;
  case SHINKA_PARAM_DATA:
    return pst.psd[ index ].ShinkaData;
  case SHINKA_PARAM_MONS:
    return pst.psd[ index ].ShinkaMons;
  }

  return 0;
}

//============================================================================================
/**
 *	進化テーブル構造体データを取得
 *
 * @param[in]	mons_no	取得したいポケモンナンバー
 * @param[out]	psd		取得した進化データの格納先を指定
 */
//============================================================================================
static	void	PokeShinkaDataGet( int mons_no, POKEMON_SHINKA_TABLE *pst )
{
	GFL_ARC_LoadData( pst, ARCID_EVOTBL, mons_no );
}

//============================================================================================
/**
 *	ポケモンが指定された技をもっているかチェック
 *
 * @param[in]	pp	      チェックするPOKEMON_PAMAM構造体
 * @param[in]	waza_no		チェックする技
 *
 * @retval  FALSE:もっていない　TRUE:もっている
 */
//============================================================================================
static  BOOL  PokeWazaHaveCheck( POKEMON_PARAM* pp, WazaID waza_no )
{ 
  int i;
  BOOL  ret = FALSE;

  for( i = ID_PARA_waza1 ; i < ID_PARA_waza4 + 1 ; i++ )
  { 
    if( PP_Get( pp, i, NULL ) == waza_no )
    {
      ret = TRUE;
      break;
    }
  }
  
  return ret;
}

