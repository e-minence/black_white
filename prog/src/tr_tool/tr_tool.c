
//============================================================================================
/**
 * @file	tr_tool.c
 * @bfief	トレーナーデータツール郡
 * @author	HisashiSogabe
 * @date	05.12.14
 */
//============================================================================================
#include  <gflib.h>

#include	"arc_def.h"

#include	"waza_tool/wazano_def.h"
#include	"tr_tool/tr_tool.h"
#include	"tr_tool/trtype_def.h"
#include	"tr_tool/trtype_sex.h"
#include	"poke_tool/poke_tool.h"
#include	"poke_tool/pokeparty.h"

#include	"pm_define.h"

#include	"message.naix"

//============================================================================================
/**
 * 定数宣言
 */
//============================================================================================

//============================================================================================
/**
 * プロトタイプ宣言
 */
//============================================================================================
//static	void	TT_EncountTrainerPokeDataMake( BATTLE_SETUP_PARAM *bsp, int num, int heapID );
static	void	TT_EncountTrainerPokeDataMake( TrainerID tr_id, POKEPARTY* pparty, HEAPID heapID );
static	void	RndTmpGet( int mons_no, int form_no, int para, u32* rnd_tmp, HEAPID heapID );
static	void	PokeParaSetFriend( POKEMON_PARAM *pp );
static inline u32 TTL_SETUP_POW_PACK( u8 pow );

//============================================================================================
/**
 *  @brief  エンカウントトレーナーデータを生成する
 *
 *  @param[in/out]  bp      戦闘システムに引き渡す構造体のポインタ
 *  @param[in]      sv      セーブデータ構造体（ライバルの名前を取り出すのに必要）
 *  @param[in]      heapID	メモリ確保するためのヒープID
 */
//============================================================================================
//一応とっておく
#if 0
void	TT_EncountTrainerDataMake( BATTLE_SETUP_PARAM *bsp, const SAVEDATA * sv, HEAPID heapID )
{
	TRAINER_DATA	td;
	int				num;
	MSGDATA_MANAGER *man = MSGMAN_Create(MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_trname_dat, heapID);
	STRBUF			*name;
	const STRCODE * rivalname = MISC_GetRivalName(SaveData_GetMiscReadOnly(sv));

	for(num=0;num<CLIENT_MAX;num++){
		if(bp->trainer_id[num]){
			TT_TrainerDataGet(bp->trainer_id[num],&td);
			bp->trainer_data[num]=td;
			if(td.tr_type==TRTYPE_RIVAL){
				PM_strcpy(&bp->trainer_data[num].name[0],rivalname);			
			}
			else{
				name=MSGMAN_AllocString(man,bp->trainer_id[num]);
				STRBUF_GetStringCode(name,&bp->trainer_data[num].name[0],BUFLEN_PERSON_NAME);
				STRBUF_Delete(name);
			}
			TT_EncountTrainerPokeDataMake(bp,num,heapID);
		}
	}

	bp->fight_type|=td.fight_type;

	MSGMAN_Delete(man);
}
#else
void	TT_EncountTrainerDataMake( BATTLE_SETUP_PARAM *bsp, HEAPID heapID )
{
  //いろいろなトレーナーデータの処理をbspに対してやるはず


  //手持ちポケモンデータ生成
	TT_EncountTrainerPokeDataMake( bsp->trID, bsp->partyEnemy1, heapID );
}
#endif

//============================================================================================
/**
 *  @brief  トレーナーデータを取得する
 *
 *  @param[in]	tr_id		取得したいトレーナーID
 *  @param[in]	id      取得したい値の種類
 *
 *	@retval	取得した値
 */
//============================================================================================
u32 TT_TrainerDataParaGet( TrainerID tr_id, int id )
{
	u32           ret;
	TRAINER_DATA  td;

	TT_TrainerDataGet( tr_id, &td );

	switch( id ){
	case ID_TD_data_type:		//データタイプ
		ret = td.data_type;
		break;
	case ID_TD_tr_type:			//トレーナー分類
		ret = td.tr_type;
		break;
	case ID_TD_tr_gra:			//トレーナーグラフィック
		ret = td.tr_gra;
		break;
	case ID_TD_poke_count:	//所持ポケモン数
		ret = td.poke_count;
		break;
	case ID_TD_use_item1:		//使用道具1
	case ID_TD_use_item2:		//使用道具2
	case ID_TD_use_item3:		//使用道具3
	case ID_TD_use_item4:		//使用道具4
		ret = td.use_item[ id - ID_TD_use_item1 ];
		break;
	case ID_TD_aibit:			  //AIパターン
		ret = td.aibit;
		break;
	case ID_TD_fight_type:	//戦闘タイプ（1vs1or2vs2）
		ret = td.fight_type;
		break;
	}
	return ret;
}

//============================================================================================
/**
 *  @briefトレーナーメッセージデータが存在するかチェック
 *
 *  @param[in]	tr_id   チェックしたいトレーナーID
 *  @param[in]	kindID	チェックしたいメッセージの種類
 *  @param[in]	heapID	テンポラリ領域の生成先ヒープID
 *
 * @retval	TRUE:存在する　FALSE:存在しない
 */
//============================================================================================
BOOL	TT_TrainerMessageCheck( TrainerID tr_id, int kindID, HEAPID heapID )
{
	ARCHANDLE*  handle;
	int         size;
	u16         ofs;
	u16         data[ 2 ];
	BOOL		    ret;

	ret=FALSE;

	size = GFL_ARC_GetDataSize( ARCID_TRTBL, 0 );
	GFL_ARC_LoadDataOfs( &ofs, ARCID_TRTBL_OFS, 0, tr_id * 2, 2 );
	handle = GFL_ARC_OpenDataHandle( ARCID_TRTBL, heapID );

	while( ofs != size ){
		GFL_ARC_LoadDataOfsByHandle( handle, 0, ofs, 4, &data[ 0 ] );
		if( ( data[ 0 ] == tr_id ) && ( data[ 1 ] == kindID ) ){
			ret = TRUE;
			break;
		}
		if( data[ 0 ] != tr_id ){
			break;
		}
		ofs += 4;
	}
	GFL_ARC_CloseDataHandle( handle );

	return ret;
}

//============================================================================================
/**
 *  @brief  トレーナーメッセージデータを取得
 *
 *  @param[in]	tr_id	  取得したいトレーナーID
 *  @param[in]	kindID 	取得したいメッセージの種類
 *  @param[out]	msg			取得したメッセージの格納先へのポインタ
 *  @param[in]	heapID	テンポラリ領域の生成先ヒープID
 */
//============================================================================================
void	TT_TrainerMessageGet( TrainerID tr_id, int kindID, STRBUF* str, HEAPID heapID )
{
	ARCHANDLE*    handle;
	int           size;
	u16           ofs;
	u16           data[ 2 ];
  GFL_MSGDATA*  msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,NARC_message_trmsg_dat, heapID );

	size = GFL_ARC_GetDataSize( ARCID_TRTBL, 0 );
	GFL_ARC_LoadDataOfs( &ofs, ARCID_TRTBL_OFS, 0, tr_id * 2, 2 );
	handle = GFL_ARC_OpenDataHandle( ARCID_TRTBL, heapID );

	while( ofs != size ){
		GFL_ARC_LoadDataOfsByHandle( handle, 0, ofs, 4, &data[ 0 ] );
		if( ( data[ 0 ] == tr_id ) && ( data[ 1 ] == kindID ) ){
			GFL_MSG_GetString( msg, ofs / 4, str );
			break;
		}
		ofs += 4;
	}
	GFL_ARC_CloseDataHandle( handle );

//	GF_ASSERT_MSG(ofs!=size,"TrainerMessage Not Found");
	//メッセージが見つからなかったら、STRBUFをクリアして返す
	if( ofs == size ){
		OS_TPrintf("TrainerMessage Not Found");
		GFL_STR_ClearBuffer( str );
	}
}

//============================================================================================
/**
 *  @briefトレーナー構造体データを取得
 *
 *  @param[in]	tr_id		取得したいトレーナーID
 *  @param[in]	td			取得したトレーナーデータの格納先を指定
 */
//============================================================================================
void	TT_TrainerDataGet( TrainerID tr_id, TRAINER_DATA* td )
{
	GFL_ARC_LoadData( td, ARCID_TRDATA, tr_id );
}

//============================================================================================
/**
 *  @brief  トレーナー持ちポケモン構造体データを取得
 *
 *  @param[in]	tr_id		取得したいトレーナーID
 *  @param[in]	td			取得したトレーナー持ちポケモンデータの格納先を指定
 */
//============================================================================================
void	TT_TrainerPokeDataGet( TrainerID tr_id, void* tpd )
{
	GFL_ARC_LoadData( tpd, ARCID_TRPOKE, tr_id );
}

//============================================================================================
/**
 *  @brief  トレーナータイプから性別を取得する
 *
 *  @param[in]	trtype		取得したいトレーナータイプ
 */
//============================================================================================
u8	TT_TrainerTypeSexGet( int trtype )
{
	return TrTypeSexTable[ trtype ];
}

//============================================================================================
/**
 *							外部公開しない関数郡
 */
//============================================================================================
//============================================================================================
/**
 *  @brief  エンカウントトレーナー持ちポケモンデータを生成する
 *
 *  @param[in/out]	bp    	戦闘システムに引き渡す構造体のポインタ
 *  @param[in]      num   	何番目のトレーナーID？（タッグの時のため）
 *  @param[in]      heapID	メモリ確保するためのヒープID
 */
//============================================================================================
//static	void	TT_EncountTrainerPokeDataMake( BATTLE_PARAM *bp, int num, int heapID )
static	void	TT_EncountTrainerPokeDataMake( TrainerID tr_id, POKEPARTY* pparty, HEAPID heapID )
{
  TRAINER_DATA*   td;
	void*           buf;
	int				      i, j;
	u32				      rnd_tmp, rnd;
	u8				      pow;
	POKEMON_PARAM*  pp;
  GFL_STD_RandContext rand_c;

	PokeParty_Init( pparty, TEMOTI_POKEMAX );

	td = GFL_HEAP_AllocMemory( heapID, sizeof( TRAINER_DATA ) );
	buf = GFL_HEAP_AllocMemory( heapID, sizeof( POKEDATA_TYPE_MULTI ) * TEMOTI_POKEMAX );
	pp = GFL_HEAP_AllocMemory( heapID, POKETOOL_GetWorkSize() );

  TT_TrainerDataGet( tr_id, td );
	TT_TrainerPokeDataGet( tr_id, buf );

	if( TT_TrainerTypeSexGet( td->tr_type ) == PTL_SEX_FEMALE )
  {
		rnd_tmp = 120;
	}
	else
  {
		rnd_tmp = 136;
	}

	switch( td->data_type ){
	case DATATYPE_NORMAL:
		{
			POKEDATA_TYPE_NORMAL* ptn;

			ptn = ( POKEDATA_TYPE_NORMAL * )buf;
			for( i = 0 ; i < td->poke_count ; i++ )
      {
				//持ちポケモンデータのパラメータを使用して、ポケモンの個性乱数を生成
				rnd = ptn[ i ].pow + ptn[ i ].level + ptn[ i ].monsno + tr_id;
        GFL_STD_RandInit( &rand_c, rnd );
				for( j = 0 ; j < td->tr_type ; j++ )
        {
					rnd = GFL_STD_Rand( &rand_c, 0x10000 );
				}
				rnd = ( rnd << 8 ) + rnd_tmp;
				pow = ptn[ i ].pow * 31 / 255;
				PP_SetupEx( pp,
                    ptn[ i ].monsno,
                    ptn[ i ].level,
                    PTL_SETUP_ID_NOT_RARE,
                    TTL_SETUP_POW_PACK( pow ),
                    rnd );
        PokeParaSetFriend( pp );
				PokeParty_Add( pparty, pp );
			}
		}
		break;
	case DATATYPE_WAZA:
		{
			POKEDATA_TYPE_WAZA* ptw;

			ptw = ( POKEDATA_TYPE_WAZA * )buf;
			for( i = 0 ; i < td->poke_count ; i++ )
      {
				//持ちポケモンデータのパラメータを使用して、ポケモンの個性乱数を生成
				rnd = ptw[ i ].pow + ptw[ i ].level + ptw[i].monsno + tr_id;
        GFL_STD_RandInit( &rand_c, rnd );
				for( j = 0 ; j < td->tr_type ; j++ )
        {
					rnd = GFL_STD_Rand( &rand_c, 0x10000 );
				}
				rnd = ( rnd << 8 ) + rnd_tmp;
				pow = ptw[ i ].pow * 31 / 255;
				PP_SetupEx( pp,
                    ptw[ i ].monsno,
                    ptw[ i ].level,
                    PTL_SETUP_ID_NOT_RARE,
                    TTL_SETUP_POW_PACK( pow ),
                    rnd );
				for( j = 0 ; j < 4 ; j++ )
        {
					PP_SetWazaPos( pp, ptw[ i ].waza[ j ], j );
				}
        PokeParaSetFriend( pp );
				PokeParty_Add( pparty, pp );
			}
		}
		break;
	case DATATYPE_ITEM:
		{
			POKEDATA_TYPE_ITEM* pti;

			pti = ( POKEDATA_TYPE_ITEM * )buf;
			for( i = 0 ; i < td->poke_count ; i++ )
      {
				//持ちポケモンデータのパラメータを使用して、ポケモンの個性乱数を生成
				rnd = pti[ i ].pow + pti[ i ].level + pti[ i ].monsno + tr_id;
        GFL_STD_RandInit( &rand_c, rnd );
				for( j = 0 ; j < td->tr_type ; j++ )
        {
					rnd = GFL_STD_Rand( &rand_c, 0x10000 );
				}
				rnd = ( rnd << 8 ) + rnd_tmp;
				pow = pti[ i ].pow * 31 / 255;
				PP_SetupEx( pp,
                    pti[ i ].monsno,
                    pti[ i ].level,
                    PTL_SETUP_ID_NOT_RARE,
                    TTL_SETUP_POW_PACK( pow ),
                    rnd );
				PP_Put( pp, ID_PARA_item, ( u32 )&pti[ i ].itemno );
        PokeParaSetFriend( pp );
				PokeParty_Add( pparty, pp );
			}
		}
		break;
	case DATATYPE_MULTI:
		{
			POKEDATA_TYPE_MULTI*  ptm;

			ptm = ( POKEDATA_TYPE_MULTI * )buf;
			for( i = 0 ; i < td->poke_count ; i++ )
      {
				//持ちポケモンデータのパラメータを使用して、ポケモンの個性乱数を生成
				rnd = ptm[ i ].pow + ptm[ i ].level + ptm[ i ].monsno + tr_id;
        GFL_STD_RandInit( &rand_c, rnd );
				for( j = 0 ; j < td->tr_type ; j++ )
        {
					rnd = GFL_STD_Rand( &rand_c, 0x10000 );
				}
				rnd = ( rnd << 8 ) + rnd_tmp;
				pow = ptm[ i ].pow * 31 / 255;
				PP_SetupEx( pp,
                    ptm[ i ].monsno,
                    ptm[ i ].level,
                    PTL_SETUP_ID_NOT_RARE,
                    TTL_SETUP_POW_PACK( pow ),
                    rnd );
				PP_Put( pp, ID_PARA_item, ( u32 )&ptm[ i ].itemno );
				for( j = 0 ; j < 4 ; j++ )
        {
					PP_SetWazaPos( pp, ptm[ i ].waza[ j ], j );
				}
        PokeParaSetFriend( pp );
				PokeParty_Add( pparty, pp );
			}
		}
		break;
	}
	GFL_HEAP_FreeMemory( td );
	GFL_HEAP_FreeMemory( buf );
	GFL_HEAP_FreeMemory( pp );
}
			
//============================================================================================
/**
 * @brief ポケモンの特性と性別を決定する乱数値を算出する
 *
 * @param[in]   	mons_no	算出するモンスターナンバー
 * @param[in]   	form_no	算出するフォルムナンバー
 * @param[in]   	para    特性と性別パラメータ
 * @param[in/out]	rnd_tmp	求める乱数値
 */
//============================================================================================
static	void	RndTmpGet( int mons_no, int form_no, int para, u32* rnd_tmp, HEAPID heapID )
{
	int sex	    = para & 0x0f;
	int speabi	= ( para & 0xf0 ) >> 4;
  POKEMON_PERSONAL_DATA*  ppd;

	if( para == 0 ){
		return;
	}

  ppd = POKE_PERSONAL_OpenHandle( mons_no, form_no, heapID );

	if( sex ){
		*rnd_tmp = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_sex );
		if( sex == 1 ){
			*rnd_tmp += 2;
		}
		else{
			*rnd_tmp -= 2;
		}
	}

	if( speabi == 1 ){
		*rnd_tmp &= 0xfffffffe;
	}
	else if( speabi == 2 ){
		*rnd_tmp |= 0x00000001;
	}

  POKE_PERSONAL_CloseHandle( ppd );
}

//============================================================================================
/**
 * @brief 所持している技からなつき度を設定する
 *
 * @param[in]	pp	POKEMON_PARAM構造体のポインタ
 */
//============================================================================================
static	void	PokeParaSetFriend( POKEMON_PARAM* pp )
{
	//なつき度の初期値は255
	u8	friend = 255;
	int	no;

	for( no = 0 ; no < PTL_WAZA_MAX ; no++ ){
		//やつあたりを持っているときは、なつき度を0にする
		if( PP_Get( pp, ID_PARA_waza1 + no , NULL ) == WAZANO_YATUATARI ){
			friend = 0;
		}
	}
	PP_Put( pp, ID_PARA_friend, ( u32 )&friend );
}

//============================================================================================
/**
 * @brief 個体値を任意の値に設定するための変数パッキング処理
 *
 * @param[in]	pow	パワー乱数値
 */
//============================================================================================
static inline u32 TTL_SETUP_POW_PACK( u8 pow )
{
  return ( (pow&0x1f)|((pow&0x1f)<<5)|((pow&0x1f)<<10)|((pow&0x1f)<<15)|((pow&0x1f)<<20)|((pow&0x1f)<<25) );
}

