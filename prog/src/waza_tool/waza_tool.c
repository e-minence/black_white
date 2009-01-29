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


s8   WAZADATA_GetPriority( WazaID id )
{
	return WT_WazaDataParaGet( id, ID_WTD_attackpri );
}

WazaCategory  WAZADATA_GetCategory( WazaID id )
{
	return WAZADATA_CATEGORY_SIMPLE_DAMAGE;
}

PokeType WAZADATA_GetType( WazaID id )
{
	return WT_WazaDataParaGet( id, ID_WTD_wazatype );
}

u16 WAZADATA_GetPower( WazaID id )
{
	return WT_WazaDataParaGet( id, ID_WTD_damage );
}

WazaDamageType WAZADATA_GetDamageType( WazaID id )
{
	return WT_WazaDataParaGet( id, ID_WTD_kind );
}

// 命中率
u8 WAZADATA_GetHitRatio( WazaID id )
{
	u8 ret = WT_WazaDataParaGet( id, ID_WTD_hitprobability );
	TAYA_Printf("[WAZATL] GetHitRatio : waza=%d, ratio=%d\n", id, ret);
	if( ret == 0 )
	{
		ret = 100;
	}
	return ret;
}

// 必中フラグ。つばめがえしなどがTRUEを返す。
BOOL WAZADATA_IsAlwaysHit( WazaID id )
{
	return FALSE;
}

// ダメージワザかどうか
BOOL WAZADATA_IsDamage( WazaID id )
{
	return WAZADATA_GetPower(id) != 0;
}

// クリティカルランク
u8 WAZADATA_GetCriticalRank( WazaID id )
{
	return 0;
}

// 最高ヒット回数
u8 WAZADATA_GetMaxHitCount( WazaID id )
{
	return 1;
}

// 効果範囲
WazaTarget WAZADATA_GetTarget( WazaID id )
{
	return 0;
}


#ifdef PM_DEBUG
void WAZADATA_PrintDebug( void )
{
	static const struct {
		const char* name;
		u16 id;
	}tbl[] = {
		{ "みずでっぽう（通常）",		WAZANO_MIZUDEPPOU },
		{ "ねっぷう（相手２体）",				WAZANO_NEPPUU },
		{ "なみのり（３体）",				WAZANO_NAMINORI },
		{ "かげぶんしん（自分）",		WAZANO_KAGEBUNSIN },
		{ "くろいきり（場）",			WAZANO_KUROIKIRI },
		{ "げきりん（ランダム）",				WAZANO_KUROIKIRI },
		{ "アロマセラピー（味方２体）",	WAZANO_AROMASERAPII },
		{ "まきびし（まきびし）",				WAZANO_MAKIBISI },
		{ "てだすけ（てだすけ）",				WAZANO_TEDASUKE },
		{ "つぼつく（つぼつく）",				WAZANO_TUBOWOTUKU },
		{ "さきどり（さきどり）",				WAZANO_SAKIDORI },
		{ "オウムがえし（なし）",		WAZANO_OUMUGAESI },
	};
	u32 i, range;

	OS_TPrintf("***** ワザ効果範囲値チェック *****\n");
	for(i=0; i<NELEMS(tbl); ++i)
	{
		range = WT_WazaDataParaGet( tbl[i].id, ID_WTD_attackrange );
		OS_TPrintf("%s\t=%d\n", tbl[i].name, range);
		
	}
}

#endif
