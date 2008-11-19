//============================================================================================
/**
 * @file	waza_tool.c
 * @bfief	技パラメータツール群（WB改訂版）
 * @author	HisashiSogabe
 * @date	08.11.13
 */
//============================================================================================
#include	<gflib.h>

#include	"arc_def.h"

#include	"waza_tool/waza_tool.h"
#include	"waza_tool/wazano_def.h"

#include	"waza_tool_def.h"



//============================================================================================
/**
 * 定数宣言
 */
//============================================================================================

typedef struct{
	u16	monsno;
	s16 hp;

	u16	hpmax;
	u8	eqp;
	u8	atc;

	u32 condition;

	u8	speabino;
	u8	sex;
	u8	type1;
	u8	type2;

}WAZA_DAMAGE_CALC_PARAM;

//============================================================================================
/**
 * プロトタイプ宣言
 */
//============================================================================================
void		WT_WazaTableDataGet( void *buffer );
ARCHANDLE	*WT_WazaDataParaHandleOpen( HEAPID heapID );
void		WT_WazaDataParaHandleClose( ARCHANDLE *handle );
u32			WT_WazaDataParaGet( int waza_no, int id );
u32			WT_WazaDataParaGetHandle( ARCHANDLE *handle, int waza_no, int id );
u8			WT_PPMaxGet( u16 wazano, u8 maxupcnt );
u32			WT_WazaDataParaGetAct( WAZA_TABLE_DATA *wtd, int id );

static	void	WT_WazaDataGet( int waza_no, WAZA_TABLE_DATA *wtd );

//============================================================================================
/**
 *	技データテーブルデータをAllocしたメモリに展開
 *
 * @param[in]	heapID	データを展開するメモリヒープID
 */
//============================================================================================
void	WT_WazaTableDataGet( void *buffer )
{
	GFL_ARC_LoadDataOfs( buffer, ARCID_WAZA_TBL, 0, 0, sizeof( WAZA_TABLE_DATA ) * ( WAZANO_MAX + 1 ) );
}

//============================================================================================
/**
 *	技データテーブルファイルハンドルオープン
 *
 * @param[in]	heapID	ハンドルをAllocするヒープID
 */
//============================================================================================
ARCHANDLE	*WT_WazaDataParaHandleOpen( HEAPID heapID )
{
	return GFL_ARC_OpenDataHandle( ARCID_WAZA_TBL, heapID );
}

//============================================================================================
/**
 *	技データテーブルファイルハンドルクローズ
 *
 * @param[in]	handle	クローズするHandle
 */
//============================================================================================
void	WT_WazaDataParaHandleClose( ARCHANDLE *handle )
{
	GFL_ARC_CloseDataHandle( handle );
}

//============================================================================================
/**
 *	技データテーブルから値を取得
 *
 * @param[in]	waza_no		取得したい技ナンバー
 * @param[in]	id			取得したい値の種類
 */
//============================================================================================
u32	WT_WazaDataParaGet( int waza_no, int id )
{
	WAZA_TABLE_DATA wtd;

	WT_WazaDataGet( waza_no, &wtd );

	return	WT_WazaDataParaGetAct( &wtd, id );
}

//============================================================================================
/**
 *	技データテーブルから値を取得（ファイルハンドルを使用する）
 *
 * @param[in]	waza_no		取得したい技ナンバー
 * @param[in]	id			取得したい値の種類
 */
//============================================================================================
u32	WT_WazaDataParaGetHandle( ARCHANDLE *handle, int waza_no, int id )
{
	WAZA_TABLE_DATA wtd;

	GFL_ARC_LoadDataByHandle( handle, waza_no, &wtd );

	return	WT_WazaDataParaGetAct( &wtd, id );
}

//============================================================================================
/**
 *	PPMaxを取得
 *
 * @param[in]	wazano		取得する技ナンバー
 * @param[in]	maxupcnt	マックスアップを使用した回数
 *
 * @retval	PPMAX
 */
//============================================================================================
u8	WT_PPMaxGet( u16 wazano, u8 maxupcnt )
{
	u8	ppmax;

	if( maxupcnt > 3 ){
		maxupcnt = 3;
	}

	ppmax = WT_WazaDataParaGet( wazano, ID_WTD_pp );
	ppmax += ( ppmax * 20 * maxupcnt ) / 100;

	return	ppmax;
}

//============================================================================================
/**
 *	技テーブル構造体データから値を取得
 *
 * @param[in]	wtd		技テーブル構造体データへのポインタ
 * @param[in]	id		取得したい技データのIndex
 */
//============================================================================================
u32	WT_WazaDataParaGetAct( WAZA_TABLE_DATA *wtd, int id )
{
	u32	ret;

	switch( id ){
	case ID_WTD_battleeffect:
		ret = wtd->battleeffect;
		break;
	case ID_WTD_kind:
		ret = wtd->kind;
		break;
	case ID_WTD_damage:
		ret = wtd->damage;
		break;
	case ID_WTD_wazatype:
		ret = wtd->wazatype;
		break;
	case ID_WTD_hitprobability:
		ret = wtd->hitprobability;
		break;
	case ID_WTD_pp:
		ret = wtd->pp;
		break;
	case ID_WTD_addeffect:
		ret = wtd->addeffect;
		break;
	case ID_WTD_attackrange:
		ret = wtd->attackrange;
		break;
	case ID_WTD_attackpri:
		ret = wtd->attackpri;
		break;
	case ID_WTD_flag:
		ret = wtd->flag;
		break;
	case ID_WTD_ap_no:				//コンテスト用パラメータ　アピールNo
		ret = wtd->condata.ap_no;
		break;
	case ID_WTD_contype:			//コンテスト用パラメータ　コンテスト技タイプ
		ret = wtd->condata.contype;
		break;
	}
	return ret;
}

//============================================================================================
/**
 *							外部公開しない関数郡
 */
//============================================================================================
//============================================================================================
/**
 *	技テーブル構造体データを取得
 *
 * @param[in]	waza_no		取得したい技ナンバー
 * @param[out]	wtd			取得した技データの格納先を指定
 */
//============================================================================================
static	void	WT_WazaDataGet( int waza_no, WAZA_TABLE_DATA *wtd )
{
	GFL_ARC_LoadData( wtd, ARCID_WAZA_TBL, waza_no );
}



//=============================================================================================
/**
 * @file	wazadata.c
 * @brief	ポケモンWB  ワザデータアクセサ
 * @author	taya
 *
 * @date	2008.10.14	作成
 */
//=============================================================================================



#include "waza_tool/wazadata.h"


enum {
	HITRATIO_MUST = 101,
};


typedef struct {
	u16  id;

	u8   category;
	u8   type;

	u8   dmgType;
	u8   hitRatio;
	u8   power;
	u8   numMaxHit;

	u8   sick1;
	u8   sick1per;

	u8   influ1;
	u8   influ1per;
	s8   influ1volume;

}WAZA_DATA;

static const WAZA_DATA WazaDataTbl[] = {
	{
		WAZANO_NAMINORI,
		WAZADATA_CATEGORY_SIMPLE_DAMAGE, POKETYPE_WATER,
		WAZADATA_DMG_SPECIAL, 100, 80, 1,
		WAZASICK_NULL, 0,
		WAZA_INFLU_NULL, 0, 0,
	},{
		WAZANO_KOORINOKIBA,
		WAZADATA_CATEGORY_SIMPLE_DAMAGE, POKETYPE_KOORI,
		WAZADATA_DMG_PHYSIC, 100, 65, 1,
		WAZASICK_KOORI, 20,
		WAZA_INFLU_NULL, 0, 0,
	},{
		WAZANO_AMAERU,
		WAZADATA_CATEGORY_SIMPLE_EFFECT, POKETYPE_NORMAL,
		WAZADATA_DMG_NONE, 0, 0, 1,
		WAZASICK_NULL, 0,
		WAZA_INFLU_ATTACK, 0, -2,
	},{
		WAZANO_KAGEBUNSIN,
		WAZADATA_CATEGORY_SIMPLE_EFFECT, POKETYPE_NORMAL,
		WAZADATA_DMG_NONE, 0, 0, 1,
		WAZASICK_NULL, 0,
		WAZA_INFLU_AVOID, 0, 1,
	},{
		WAZANO_HIKARINOKABE,
		WAZADATA_CATEGORY_SIMPLE_EFFECT, POKETYPE_SP,
		WAZADATA_DMG_NONE, 0, 0, 1,
		WAZASICK_NULL, 0,
		WAZA_INFLU_AVOID, 0, 1,
	},{
		0,
		WAZADATA_CATEGORY_SIMPLE_DAMAGE, POKETYPE_WATER,
		WAZADATA_DMG_SPECIAL, 100, 80, 1,
		WAZASICK_NULL, 0,
		WAZA_INFLU_NULL, 0, 0,
	},
};


static const WAZA_DATA* getData( WazaID id )
{
	int i;
	for(i=0; WazaDataTbl[i].id != 0; i++)
	{
		if( WazaDataTbl[i].id == id )
		{
			break;
		}
	}

	return &WazaDataTbl[i];
}



s8   WAZADATA_GetPriority( WazaID id )
{
	return 0;
}


WazaCategory  WAZADATA_GetCategory( WazaID id )
{
	const WAZA_DATA* dat = getData( id );
	return dat->category;
}

PokeType WAZADATA_GetType( WazaID id )
{
	const WAZA_DATA* dat = getData( id );
	return dat->type;
}

u16 WAZADATA_GetPower( WazaID id )
{
	const WAZA_DATA* dat = getData( id );
	return dat->power;
}

WazaDamageType WAZADATA_GetDamageType( WazaID id )
{
	const WAZA_DATA* dat = getData( id );
	return dat->dmgType;
}
// 命中率
u8 WAZADATA_GetHitRatio( WazaID id )
{
	const WAZA_DATA* dat = getData( id );
	return dat->hitRatio;
}

// 必中フラグ。つばめがえしなどがTRUEを返す。
BOOL WAZADATA_IsAleadyHit( WazaID id )
{
	const WAZA_DATA* dat = getData( id );
	return dat->hitRatio == HITRATIO_MUST;
}
// ダメージワザかどうか
BOOL WAZADATA_IsDamage( WazaID id )
{
	const WAZA_DATA* dat = getData( id );
	return dat->dmgType != WAZADATA_DMG_NONE;
}

// クリティカルランク
u8 WAZADATA_GetCriticalRank( WazaID id )
{
	const WAZA_DATA* dat = getData( id );
	return dat->dmgType != WAZADATA_DMG_NONE;
}

// 最高ヒット回数
u8 WAZADATA_GetMaxHitCount( WazaID id )
{
	const WAZA_DATA* dat = getData( id );
	return dat->numMaxHit;
}

#if 0
BOOL      WAZADATA_IsDamage( WazaID id );
WazaDamageType  WAZADATA_GetDamageType( WazaID id );
u16  WAZADATA_GetPower( WazaID id );
u16  WAZADATA_GetHitRatio( WazaID id );


// 複数回連続攻撃ワザの最大回数。
extern u16  WAZADATA_GetContiniusCount( WazaID id );

// きゅうしょに当たるランク
extern u16  WAZADATA_GetCriticalRatio( WazaID id );

// 通常攻撃ワザであり、追加効果を含む場合は追加効果値が返る。
// 状態異常のみを起こすワザ（例えば催眠術など）は、WAZASICK_NULL が返る。
extern WazaSick  WAZADATA_GetAdditionalSick( WazaID id );

// 同上。こちらは追加効果の確率が返る。
extern u8  WAZADATA_GetAdditionalSickRatio( WazaID id );

#endif
