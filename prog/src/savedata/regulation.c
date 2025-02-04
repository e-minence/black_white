//============================================================================================
/**
 * @file	regulation.c
 * @brief	バトルレギュレーションデータアクセス用ソース
 * @author	k.ohno
 * @date	2006.1.20
 */
//============================================================================================

#include <gflib.h>

#include "pm_define.h"
#include "arc_def.h"
#include "system/main.h"
#include "message.naix"
#include "savedata/save_tbl.h"	//SAVEDATA参照のため
#include "buflen.h"
#include "savedata/save_control.h"
#include "savedata/regulation.h"
#include "poke_tool/monsno_def.h"

#include "msg/msg_regulation_cup.h"
#include "msg/msg_regulation_rule.h"

#define HEAPID_SAVE_TEMP        (GFL_HEAPID_APP)

#if PM_DEBUG
#define REGPRINT (0)
#else
#define REGPRINT (0)
#endif

#ifdef DEBUG_ONLY_FOR_toru_nagihashi
//#undef REGPRINT
//#define REGPRINT  (1)
#endif //def

//============================================================================================
//============================================================================================
//-------------------------------------
///	  デジタル選手証見た目情報
//=====================================
struct _REGULATION_VIEWDATA
{
  u16 mons_no[ TEMOTI_POKEMAX ];
  u8  form_no[ TEMOTI_POKEMAX ];
  u8  item_flag;  //6体分のビット
  u8  sex_flag;  //6対分のビット  ポケモンアイコン用の性別なので♀が１その他が０
  u8  dummy[ 10 ];
  /* 下記の節約
   *
  u8  item_flag[ TEMOTI_POKEMAX ];
  u8  sex[ TEMOTI_POKEMAX ];*/
} ;

//-------------------------------------
///	  セーブデータ本体
//=====================================
struct _REGULATION_SAVEDATA
{ 
  REGULATION_CARDDATA card_data[ REGULATION_CARD_TYPE_MAX ];
  REGULATION_VIEWDATA view_data[ REGULATION_CARD_TYPE_MAX ];
};


//============================================================================================
//
//	セーブデータシステムが依存する関数
//
//============================================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  レギュレーションセーブデータのサイズ
 *
 *	@return サイズ
 */
//-----------------------------------------------------------------------------
int RegulationSaveData_GetWorkSize(void)
{ 
  return sizeof(REGULATION_SAVEDATA);
}
//----------------------------------------------------------
/**
 * @brief	ワークサイズ取得
 * @return	int		サイズ（バイト単位）
 */
//----------------------------------------------------------
int Regulation_GetWorkSize(void)
{
  return sizeof(REGULATION);
}

//----------------------------------------------------------
/**
 * @brief	ワークサイズ取得
 * @return	int		サイズ（バイト単位）
 */
//----------------------------------------------------------
int RegulationData_GetWorkSize(void)
{
  return sizeof(REGULATION_CARDDATA);
}

//----------------------------------------------------------
/**
 * @brief	バトルレギュレーションワークの確保
 * @param	heapID		メモリ確保をおこなうヒープ指定
 * @return	REGULATION*	取得したワークへのポインタ
 */
//----------------------------------------------------------
REGULATION* Regulation_AllocWork(HEAPID heapID)
{
  REGULATION* reg;
  reg = GFL_HEAP_AllocClearMemory(heapID, sizeof(REGULATION));
  return reg;
}

//----------------------------------------------------------
/**
 * @brief	REGULATIONのコピー
 * @param	from	コピー元REGULATIONへのポインタ
 * @param	to		コピー先REGULATIONへのポインタ
 */
//----------------------------------------------------------
void Regulation_Copy(const REGULATION *pFrom, REGULATION* pTo)
{
  GFL_STD_MemCopy(pFrom, pTo, sizeof(REGULATION));
}

//----------------------------------------------------------
/**
 * @brief	REGULATIONの比較
 * @param	cmp1   比較するREGULATIONへのポインタ
 * @param	cmp2   比較されるREGULATIONへのポインタ
 * @return  一致していたらTRUE
 */
//----------------------------------------------------------
int Regulation_Cmp(const REGULATION* pCmp1,const REGULATION* pCmp2)
{
  if(0 == GFL_STD_MemComp(pCmp1,pCmp2,sizeof(REGULATION)))
  {
    return TRUE;
  }
  return FALSE;
}

//============================================================================================
//
//	REGULATION操作のための関数
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	バトルレギュレーションデータの初期化
 * @param	pReg		REGULATIONワークへのポインタ
 */
//----------------------------------------------------------
void Regulation_Init(REGULATION * my)
{
  GFL_STD_MemClear(my, sizeof(REGULATION));
}

//----------------------------------------------------------
/**
 * @brief	デジタル選手証レギュレーションデータの初期化
 * @param	pReg		REGULATIONワークへのポインタ
 */
//----------------------------------------------------------
void RegulationData_Init(REGULATION_CARDDATA* pRegData)
{
  GFL_STD_MemClear(pRegData, sizeof(REGULATION_CARDDATA));
}

//----------------------------------------------------------------------------
/**
 *	@brief  レギュレーションをカードデータから取得
 *
 *	@param	REGULATION_CARDDATA *pRegData カードデータ
 *
 *	@return レギュレーション
 */
//-----------------------------------------------------------------------------
REGULATION* RegulationData_GetRegulation(REGULATION_CARDDATA *pRegData)
{ 
  return &pRegData->regulation_buff;
}

//----------------------------------------------------------------------------
/**
 *	@brief  カードデータへレギュレーションを設定
 *
 *	@param	*pRegData           カードデータ
 *	@param	REGULATION* pReg    レギュレーション
 */
//-----------------------------------------------------------------------------
void RegulationData_SetRegulation(REGULATION_CARDDATA *pRegData, const REGULATION* pReg)
{ 

}
//----------------------------------------------------------
/**
 * @brief	カップ名セット
 * @param	pReg	REGULATIONワークポインタ
 * @param	pCupBuf	カップ名が入ったバッファ
 */
//----------------------------------------------------------
//void Regulation_SetCupName(REGULATION* pReg, const STRBUF *pCupBuf)
//{
//  GFL_STR_GetStringCode(pCupBuf, pReg->cupName, (REGULATION_CUPNAME_SIZE + EOM_SIZE));
//}

//----------------------------------------------------------
/**
 * @brief	カップ名取得
 * @param	pReg		    REGULATIONワークポインタ
 * @param	pReturnCupName	カップ名を入れるSTRBUFポインタ
 * @return	none
 */
//----------------------------------------------------------
void Regulation_GetCupName(const REGULATION* pReg,STRBUF* pReturnCupName)
{
 // GFL_STR_SetStringCodeOrderLength(pReturnCupName, pReg->cupName, (REGULATION_CUPNAME_SIZE + EOM_SIZE));
  GFL_MSGDATA* pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                                          NARC_message_regulation_cup_dat, HEAPID_SAVE_TEMP );
  GFL_MSG_GetString( pMsgData, pReg->cupNo, pReturnCupName );
  GFL_MSG_Delete(pMsgData);
}

//----------------------------------------------------------
/**
 * @brief	カップ名前取得（STRBUFを生成）
 * @param	pReg	ワークへのポインタ
 * @param	heapID	STRBUFを生成するヒープのID
 * @return	STRBUF	名前を格納したSTRBUFへのポインタ
 */
//----------------------------------------------------------
STRBUF* Regulation_CreateCupName(const REGULATION* pReg, HEAPID heapID)
{
  STRBUF* tmpBuf = GFL_STR_CreateBuffer((REGULATION_CUPNAME_SIZE + EOM_SIZE)*GLOBAL_MSGLEN, heapID);
  Regulation_GetCupName(pReg,tmpBuf);
//  GFL_STR_SetStringCode( tmpBuf, pReg->cupName );
  return tmpBuf;

}

//----------------------------------------------------------
/**
 * @brief	ルール名セット
 * @param	pReg	REGULATIONワークポインタ
 * @param	pCupBuf	カップ名が入ったバッファ
 */
//----------------------------------------------------------
//void Regulation_SetRuleName(REGULATION* pReg, const STRBUF *pRuleBuf)
//{
//  GFL_STR_GetStringCode(pRuleBuf, pReg->ruleName, (REGULATION_RULENAME_SIZE + EOM_SIZE));
//}

//----------------------------------------------------------
/**
 * @brief	ルール名取得
 * @param	pReg		    REGULATIONワークポインタ
 * @param	pReturnCupName	ルール名を入れるSTRBUFポインタ
 * @return	none
 */
//----------------------------------------------------------
void Regulation_GetRuleName(const REGULATION* pReg,STRBUF* pReturnRuleName)
{
//  GFL_STR_SetStringCodeOrderLength(pReturnRuleName, pReg->ruleName, (REGULATION_RULENAME_SIZE + EOM_SIZE));

  GFL_MSGDATA* pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                                          NARC_message_regulation_cup_dat, HEAPID_SAVE_TEMP );
  GFL_MSG_GetString( pMsgData, pReg->ruleNo, pReturnRuleName );
  GFL_MSG_Delete(pMsgData);

}

//----------------------------------------------------------
/**
 * @brief	ルール名前取得（STRBUFを生成）
 * @param	pReg	ワークへのポインタ
 * @param	heapID	STRBUFを生成するヒープのID
 * @return	STRBUF	名前を格納したSTRBUFへのポインタ
 */
//----------------------------------------------------------
STRBUF* Regulation_CreateRuleName(const REGULATION* pReg, HEAPID heapID)
{
  STRBUF* tmpBuf = GFL_STR_CreateBuffer((REGULATION_RULENAME_SIZE + EOM_SIZE)*GLOBAL_MSGLEN, heapID);
  Regulation_GetRuleName(pReg,tmpBuf);

  //  GFL_STR_SetStringCode( tmpBuf, pReg->ruleName );
  return tmpBuf;
}

//----------------------------------------------------------
/**
 * @brief	パラメーターを取得する
 * @param	pReg	REGULATIONワークポインタ
 * @param	type	REGULATION_PARAM_TYPE enum のどれか
 */
//----------------------------------------------------------
int Regulation_GetParam(const REGULATION* pReg, REGULATION_PARAM_TYPE type)
{
  int ret = 0;

  switch(type){
  case REGULATION_CUPNUM:
    ret = pReg->cupNo;
    break;
  case REGULATION_RULENUM:
    ret = pReg->ruleNo;
    break;
  case REGULATION_NUM_LO: //    #参加数下限
    ret = pReg->NUM_LO;
    break;
  case REGULATION_NUM_HI:         //参加数上限
    ret = pReg->NUM_HI;
    break;
  case REGULATION_LEVEL:  //    #参加レベル
    ret = pReg->LEVEL;
    break;
  case REGULATION_LEVEL_RANGE: //    #レベル範囲
    ret = pReg->LEVEL_RANGE;
    break;
  case REGULATION_LEVEL_TOTAL: //    #レベル合計
    ret = pReg->LEVEL_TOTAL;
    break;
  case REGULATION_BOTH_POKE: //    #同じポケモン
    ret = pReg->BOTH_POKE;
    break;
  case REGULATION_BOTH_ITEM:  //    #同じどうぐ
    ret = pReg->BOTH_ITEM;
    break;
  case REGULATION_MUST_POKE:   //    #必須ポケモン
    ret = pReg->MUST_POKE;
    break;
  case REGULATION_MUST_POKE_FORM:    //    #必須フォルム
    ret = pReg->MUST_POKE_FORM;
    break;
  case REGULATION_SHOOTER:    //    #シューター
    ret = pReg->SHOOTER;
    break;
  case REGULATION_TIME_VS:     //    #対戦時間
    ret = pReg->TIME_VS;
    break;
  case REGULATION_TIME_COMMAND: //    #入力時間
    ret = pReg->TIME_COMMAND;
    break;
  case REGULATION_NICKNAME: //    #ニックネーム表示
    ret = pReg->NICKNAME;
    break;
  case REGULATION_STATE:
    ret = pReg->CAMERA;
    break;
  case REGULATION_SHOW_POKE:  //    #ポケモン見せ合い
    ret = pReg->SHOW_POKE;
    break;
  case REGULATION_SHOW_POKE_TIME:  //    #ポケモン見せ合い制限時間
    ret = pReg->SHOW_POKE_TIME;
    break;
  case REGULATION_BATTLETYPE:  //   バトルタイプ
    ret = pReg->BATTLE_TYPE;
    break;  
  case REGULATION_BTLCOUNT:  ///<  戦闘規定回数
    ret = pReg->BTL_COUNT;
    break;
  }
  return ret;
}

//----------------------------------------------------------
/**
 * @brief	パラメーターを書きこむ
 * @param	pReg	REGULATIONワークポインタ
 * @param	type	REGULATION_PARAM_TYPE enum のどれか
 * @param	param	書きこむ値
 * @return  正しく書き込んだらTRUE  不正な値の場合FALSE
 */
//----------------------------------------------------------
void Regulation_SetParam(REGULATION* pReg, REGULATION_PARAM_TYPE type, int param)
{

  switch(type){
  case REGULATION_CUPNUM:
    if(REGULATION_CUP_MAX > param){
      pReg->cupNo = param;
    }
    else{
      GF_ASSERT(0);
    }
    break;
  case REGULATION_RULENUM:
    if(REGULATION_RULE_MAX > param){
      pReg->ruleNo = param;
    }
    else{
      GF_ASSERT(0);
    }
    break;
  case REGULATION_NUM_LO: //    #参加数下限
    if(param >= 6){
      GF_ASSERT(0);
      return;
    }
    pReg->NUM_LO = param;
    break;
  case REGULATION_NUM_HI:         //参加数上限
    if(param >= 6){
      GF_ASSERT(0);
      return;
    }
    pReg->NUM_HI = param;
    break;
  case REGULATION_LEVEL:  //    #参加レベル
    if(param > 100){
      GF_ASSERT(0);
      return;
    }
    pReg->LEVEL = param;
    break;
  case REGULATION_LEVEL_RANGE: //    #レベル範囲
    if(param > REGULATION_LEVEL_RANGE_MAX){
      GF_ASSERT(0);
      return;
    }
    pReg->LEVEL_RANGE = param;
    break;
  case REGULATION_LEVEL_TOTAL: //    #レベル合計
    if(param > 600){
      GF_ASSERT(0);
      return;
    }
    pReg->LEVEL_TOTAL = param;
    break;
  case REGULATION_BOTH_POKE: //    #同じポケモン
    if(param >= 2){
      GF_ASSERT(0);
      return;
    }
    pReg->BOTH_POKE = param;
    break;
  case REGULATION_BOTH_ITEM:  //    #同じどうぐ
    if(param >= 2){
      GF_ASSERT(0);
      return;
    }
    pReg->BOTH_ITEM = param;
    break;
  case REGULATION_MUST_POKE:   //    #必須ポケモン
    if(param >= MONSNO_MAX){ 
      GF_ASSERT(0);
      return;
    }
    pReg->MUST_POKE = param;
    break;
  case REGULATION_MUST_POKE_FORM:   //    #必須ポケモンフォルム
    pReg->MUST_POKE_FORM = param;
    break;
  case REGULATION_SHOOTER:    //    #シューター
    if(param >= 2){
      GF_ASSERT(0);
      return;
    }
    pReg->SHOOTER = param;
    break;
  case REGULATION_TIME_VS:     //    #対戦時間
    pReg->TIME_VS = param;
    break;
  case REGULATION_TIME_COMMAND: //    #入力時間
    pReg->TIME_COMMAND = param;
    break;
  case REGULATION_NICKNAME: //    #ニックネーム表示
    if(param >= 2){
      GF_ASSERT(0);
      return;
    }
    pReg->NICKNAME = param;
    break;

  case REGULATION_STATE:
    pReg->CAMERA  = param;
    break;

  case REGULATION_SHOW_POKE:  //    #ポケモン見せ合い
    if(param >= 2){
      GF_ASSERT(0);
      return;
    }
    pReg->SHOW_POKE = param;
    break;
  case REGULATION_SHOW_POKE_TIME:  //    #ポケモン見せ合い時間
    if(param > REGULATION_SHOW_POKE_TIME_MAX){
      GF_ASSERT(0);
      return;
    }
    pReg->SHOW_POKE_TIME = param;
    break;
  case REGULATION_BATTLETYPE:  //  バトルタイプ
    if(param >= REGULATION_BATTLE_MAX){
      GF_ASSERT(0);
      return;
    }
    pReg->BATTLE_TYPE = param;
    break;
  case REGULATION_BTLCOUNT:  ///<  戦闘規定回数
    pReg->BTL_COUNT = param;
    break;
  }
}

//----------------------------------------------------------
/**
 * @brief	パラメータのBITが立っているかどうかを検査
 * @param	pReg	REGULATIONワークポインタ
 * @param	type	REGULATION_PARAM_TYPE enum のどれか
 * @param	no	  ポケモン番号やアイテム番号
 * @return  BITがONでTRUE
 */
//----------------------------------------------------------
BOOL Regulation_CheckParamBit(const REGULATION* pReg, REGULATION_PARAM_TYPE type, int no)
{
  int buffno = no / 8;
  int bitno = no % 8;
  
  switch(type){
  case REGULATION_VETO_POKE_BIT:
    if(pReg->VETO_POKE_BIT[buffno] & (0x1<<bitno)){
      return TRUE;
    }
    break;
  case REGULATION_VETO_ITEM:
    if(pReg->VETO_ITEM[buffno] & (0x1<<bitno)){
      return TRUE;
    }
    break;
  case REGULATION_VETO_SHOOTER_ITEM:  ///<  シューター禁止道具
    if(pReg->VETO_SHOOTER_ITEM[buffno] & (0x1<<bitno)){
      return TRUE;
    }
    break;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  レギュレーションからシューター設定
 *
 *	@param	REGULATION* cp_reg          レギュレーション
 *	@param	SHOOTER_ITEM_BIT_WORK *p_wk シューターワーク
 */
//-----------------------------------------------------------------------------
void Regulation_GetShooterItem( const REGULATION* cp_reg, SHOOTER_ITEM_BIT_WORK *p_wk )
{ 
  GFL_STD_MemClear( p_wk, sizeof(SHOOTER_ITEM_BIT_WORK) );
  p_wk->shooter_use = Regulation_GetParam( cp_reg , REGULATION_SHOOTER );

  if(p_wk->shooter_use >= REGULATION_SHOOTER_MANUAL)
  {
    p_wk->shooter_use = REGULATION_SHOOTER_VALID;
  }

  GFL_STD_MemCopy( cp_reg->VETO_SHOOTER_ITEM, p_wk->bit_tbl, SHOOTER_ITEM_BIT_TBL_MAX );
}

//----------------------------------------------------------
/**
 * @brief	カップ名取得
 * @param	pReg		    REGULATION_CARDDATAワークポインタ
 * @param	pReturnCupName	カップ名を入れるSTRBUFポインタ
 * @return	none
 */
//----------------------------------------------------------

void Regulation_GetCardCupName(const REGULATION_CARDDATA* pReg,STRBUF* pReturnCupName)
{
  GFL_STR_SetStringCodeOrderLength(pReturnCupName, pReg->cupname, (WIFI_PLAYER_TIX_CUPNAME_MOJINUM + EOM_SIZE));
}

//----------------------------------------------------------
/**
 * @brief	カップ名前取得（STRBUFを生成）
 * @param	pReg	REGULATION_CARDDATAワークへのポインタ
 * @param	heapID	STRBUFを生成するヒープのID
 * @return	STRBUF	名前を格納したSTRBUFへのポインタ
 */
//----------------------------------------------------------

STRBUF* Regulation_CreateCardCupName(const REGULATION_CARDDATA* pReg, HEAPID heapID)
{
  STRBUF* tmpBuf = GFL_STR_CreateBuffer((WIFI_PLAYER_TIX_CUPNAME_MOJINUM + EOM_SIZE)*GLOBAL_MSGLEN, heapID);
  GFL_STR_SetStringCode( tmpBuf, pReg->cupname );
  return tmpBuf;
}

//----------------------------------------------------------------------------
/**
 *	@brief  カップ名取得
 *
 *	@param	const REGULATION_CARDDATA* pReg   ワークへのポインタ
 *
 *	@return カップ名が入ったSTRCODEのポインタ
 */
//-----------------------------------------------------------------------------
const STRCODE *Regulation_GetCardCupNamePointer( const REGULATION_CARDDATA* pReg )
{ 
  return pReg->cupname;
}


//----------------------------------------------------------
/**
 * @brief	デジタル選手証パラメーターを引き出す
 * @param	pReg	REGULATION_CARDDATAワークポインタ
 * @param	type	REGULATION_CARD_PARAM_TYPE enum のどれか
 * @return  パラメータ
 */
//----------------------------------------------------------
int Regulation_GetCardParam(const REGULATION_CARDDATA* pReg, REGULATION_CARD_PARAM_TYPE type)
{
  int ret;
  
  switch(type){
  case REGULATION_CARD_ROMVER:
    ret = pReg->ver;
    break;
  case REGULATION_CARD_CUPNO:
    ret = pReg->no;
    break;
  case REGULATION_CARD_LANGCODE:
    ret = pReg->code;
    break;
  case REGULATION_CARD_START_YEAR:  ///< 開始年：00-99
    ret = pReg->start_year;
    break;
  case REGULATION_CARD_START_MONTH: ///< 開始月：01-12
    ret = pReg->start_month;
    break;
  case REGULATION_CARD_START_DAY:   ///< 開始日：01-31
    ret = pReg->start_day;
    break;
  case REGULATION_CARD_END_YEAR:    ///< 終了年：00-99
    ret = pReg->end_year;
    break;
  case REGULATION_CARD_END_MONTH:   ///< 終了月：01-12
    ret = pReg->end_month;
    break;
  case REGULATION_CARD_END_DAY:     ///< 終了日：01-31
    ret = pReg->end_day;
    break;
  case REGULATION_CARD_STATUS:      ///< 大会状態：０未開催／１開催中／２終了
    ret = pReg->status;
    break;
  case REGULATION_CARD_BGM:         ///< 大会で使用する曲（シーケンス番号がそのまま入っているわけではない
    ret = pReg->bgm_no;
    break;

  case REGULATION_CARD_SAMEMATCH:         ///< 同じ人との対戦を許可するかどうか（ライブ大会でしか使わない→対戦した人のmacaddresを保存していないから）
    ret = pReg->same_match;
    break;
  }
  return ret;
}


//----------------------------------------------------------
/**
 * @brief	パラメーターを書きこむ
 * @param	pReg	REGULATION_CARDDATAワークポインタ
 * @param	type	REGULATION_CARD_PARAM_TYPE enum のどれか
 * @param	param	書きこむ値
 * @return  none
 */
//----------------------------------------------------------
void Regulation_SetCardParam(REGULATION_CARDDATA* pReg, REGULATION_CARD_PARAM_TYPE type, int param)
{
  switch(type){
  case REGULATION_CARD_ROMVER:
    pReg->ver = param;
    break;
  case REGULATION_CARD_CUPNO:
    pReg->no = param;
    break;
  case REGULATION_CARD_LANGCODE:
    pReg->code = param;
    break;
  case REGULATION_CARD_START_YEAR:  ///< 開始年：00-99
    pReg->start_year = param;
    break;
  case REGULATION_CARD_START_MONTH: ///< 開始月：01-12
    pReg->start_month = param;
    break;
  case REGULATION_CARD_START_DAY:   ///< 開始日：01-31
    pReg->start_day = param;
    break;
  case REGULATION_CARD_END_YEAR:    ///< 終了年：00-99
    pReg->end_year = param;
    break;
  case REGULATION_CARD_END_MONTH:   ///< 終了月：01-12
    pReg->end_month = param;
    break;
  case REGULATION_CARD_END_DAY:     ///< 終了日：01-31
    pReg->end_day = param;
    break;
  case REGULATION_CARD_STATUS:      ///< 大会状態：０未開催／１開催中／２終了
    pReg->status = param;
    break;
  case REGULATION_CARD_BGM:         ///< 大会で使用する曲（シーケンス番号がそのまま入っているわけではない
    pReg->bgm_no  = param;
    break;

  case REGULATION_CARD_SAMEMATCH:   ///< 同じ人との対戦を許可するかどうか
    pReg->same_match  = param;
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  レギュレーションカードのCRCチェック (開催ワーク statusに値が書かれているとチェックできません)
 *
 *	@param	const REGULATION* pReg  チェックするもの
 *
 *	@return TRUEで正常  FALSEで不正
 */
//-----------------------------------------------------------------------------
BOOL Regulation_CheckCrc( const REGULATION_CARDDATA* pReg )
{ 
#if REGPRINT
  OS_TPrintf( "CRCチェック buffer[0x%x]== calc[0x%x]\n", pReg->crc, GFL_STD_CrcCalc( pReg, sizeof(REGULATION_CARDDATA) - 2 ));
#endif
  return pReg->crc == Regulation_GetCrc( pReg );
}

//----------------------------------------------------------------------------
/**
 *	@brief  CRCを返す
 *
 *	@param	const REGULATION_CARDDATA* pReg レギュレーション
 *
 *	@return CRC
 */
//-----------------------------------------------------------------------------
u32 Regulation_GetCrc( const REGULATION_CARDDATA* pReg )
{
  return GFL_STD_CrcCalc( pReg, sizeof(REGULATION_CARDDATA) - 2 );
}

//----------------------------------------------------------
/**
 * @brief	レギュレーションデータへのポインタ取得
 * @param	pSave    	セーブデータ保持ワークへのポインタ
 * @param	何本目のレギュレーションデータか
 * @return	REGULATION	ワークへのポインタ  無効データの場合NULL
 */
//----------------------------------------------------------
REGULATION_CARDDATA* RegulationSaveData_GetRegulationCard( REGULATION_SAVEDATA *p_save, const REGULATION_CARD_TYPE type )
{
  GF_ASSERT( type < REGULATION_CARD_TYPE_MAX );
  return &p_save->card_data[ type ];
}

//----------------------------------------------------------
/**
 * @brief	レギュレーションデータの保存
 * @param	pSave    	セーブデータ保持ワークへのポインタ
 * @param	const REGULATION  レギュレーションデータ
 * @return	none
 */
//----------------------------------------------------------
void RegulationSaveData_SetRegulation(REGULATION_SAVEDATA *p_save, const REGULATION_CARD_TYPE type, const REGULATION_CARDDATA* pReg)
{
  GF_ASSERT( type < REGULATION_CARD_TYPE_MAX );
  p_save->card_data[ type ] = *pReg;
}

//----------------------------------------------------------------------------
/**
 *	@brief  初期化
 *
 *	@param	* my  ワーク
 */
//-----------------------------------------------------------------------------
void RegulationView_Init(REGULATION_VIEWDATA * my)
{ 
  GFL_STD_MemClear( my, sizeof(REGULATION_VIEWDATA) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  レギュレーション見た目データ取得
 *
 *	@param	REGULATION_SAVEDATA *p_save セーブデータ
 *	@param	REGULATION_CARD_TYPE type   タイプ
 *
 *	@return 見た目データ
 */
//-----------------------------------------------------------------------------
REGULATION_VIEWDATA* RegulationSaveData_GetRegulationView( REGULATION_SAVEDATA *p_save, const REGULATION_CARD_TYPE type )
{ 
  GF_ASSERT( type < REGULATION_CARD_TYPE_MAX );
  return &p_save->view_data[ type ];
}

//----------------------------------------------------------------------------
/**
 *	@brief  データ取得
 *
 *	@param	const REGULATION_VIEWDATA *pView  ワーク
 *	@param	type                              取得タイプ
 *	@param	idx                               インデックス（６まで）
 *
 *	@return データ
 */
//-----------------------------------------------------------------------------
int RegulationView_GetParam( const REGULATION_VIEWDATA *pView, REGULATION_VIEW_PARAM_TYPE type, u8 idx )
{ 
  GF_ASSERT( idx < TEMOTI_POKEMAX );

  switch( type )
  { 
  case REGULATION_VIEW_MONS_NO:
    return pView->mons_no[ idx ];

  case REGULATION_VIEW_FROM_NO:
    return pView->form_no[ idx ];

  case REGULATION_VIEW_ITEM_FLAG:
    return (pView->item_flag >> idx) & 0x1;

  case REGULATION_VIEW_SEX:
    return (pView->sex_flag >> idx) & 0x1;

  default:
    GF_ASSERT(0);
    return 0;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  データ設定
 *
 *	@param	REGULATION_VIEWDATA *pView  ワーク
 *	@param	type                        取得タイプ
 *	@param	idx                         インデックス
 *	@param	param                       設定するデータ
 */
//-----------------------------------------------------------------------------
void RegulationView_SetParam( REGULATION_VIEWDATA *pView, REGULATION_VIEW_PARAM_TYPE type, u8 idx, int param )
{ 
  GF_ASSERT( idx < TEMOTI_POKEMAX );

  switch( type )
  { 
  case REGULATION_VIEW_MONS_NO:
    pView->mons_no[ idx ]   = param;
    break;

  case REGULATION_VIEW_FROM_NO:
    pView->form_no[ idx ]   = param;
    break;

  case REGULATION_VIEW_ITEM_FLAG:
    pView->item_flag |= ((param == TRUE)<<idx);
    break;

  case REGULATION_VIEW_SEX:
    //ポケモンアイコン用の性別なので♀のみオン
    pView->sex_flag |= ((param == PTL_SEX_FEMALE)<<idx);
    break;

  default:
    GF_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  簡単データ設定  （バトルボックスのポケモンパーティから設定する）
 *
 *	@param	REGULATION_VIEWDATA *pView  ワーク
 *	@param	POKEPARTY *cp_party         ポケモンパーティ
 */
//-----------------------------------------------------------------------------
void RegulationView_SetEazy( REGULATION_VIEWDATA *pView, const POKEPARTY *cp_party )
{ 
  int i;

  RegulationView_Init( pView );

  for( i = 0; i < PokeParty_GetPokeCount( cp_party);i ++ )
  { 
    POKEMON_PARAM *p_pp = PokeParty_GetMemberPointer( cp_party, i );
    if( PP_Get( p_pp, ID_PARA_poke_exist, NULL ) )
    { 
      RegulationView_SetParam( pView, REGULATION_VIEW_MONS_NO, i, PP_Get( p_pp, ID_PARA_monsno, NULL ) );
      RegulationView_SetParam( pView, REGULATION_VIEW_FROM_NO, i, PP_Get( p_pp, ID_PARA_form_no, NULL ) );
      RegulationView_SetParam( pView, REGULATION_VIEW_ITEM_FLAG, i, PP_Get( p_pp, ID_PARA_item, NULL ) != 0 );
      RegulationView_SetParam( pView, REGULATION_VIEW_SEX, i, PP_Get( p_pp, ID_PARA_sex, NULL ) );

    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  レギュレーションデータへのポインタ
 *
 *	@param	* my  ポインタ
 */
//-----------------------------------------------------------------------------
void RegulationSaveData_Init(REGULATION_SAVEDATA * my)
{ 
  GFL_STD_MemClear(my, sizeof(REGULATION_SAVEDATA));
}

//----------------------------------------------------------
/**
 * @brief	レギュレーションデータへのポインタ取得
 * @param	pSave    	セーブデータ保持ワークへのポインタ
 * @return	REGULATION	ワークへのポインタ  無効データの場合NULL
 */
//----------------------------------------------------------
REGULATION_SAVEDATA* SaveData_GetRegulationSaveData(SAVE_CONTROL_WORK* pSave)
{
  return SaveControl_DataPtrGet(pSave, GMDATA_ID_REGULATION_DATA);
}

#ifdef PM_DEBUG

#include "debug/debug_str_conv.h"
//----------------------------------------------------------------------------
/**
 *	@brief  デバッグプリント
 *
 *	@param	const REGULATION_CARDDATA* cpReg ワーク
 */
//-----------------------------------------------------------------------------
void Regulation_PrintDebug( const REGULATION_CARDDATA* cpReg )
{ 
  const REGULATION  *cp_data = &cpReg->regulation_buff;

#if REGPRINT
  OS_TPrintf( "reg size =%d\n", sizeof(REGULATION));
  OS_TPrintf( "card size=%d\n", sizeof(REGULATION_CARDDATA));
  OS_TPrintf( "cupno    =%d\n", cp_data->cupNo);
  OS_TPrintf( "rule_num =%d\n", cp_data->ruleNo); ///< 大会ルール番号
  OS_TPrintf( "num_lo   =%d\n", cp_data->NUM_LO); ///<    #参加数下限
  OS_TPrintf( "num_hi   =%d\n", cp_data->NUM_HI); ///<    #参加数上限
  OS_TPrintf( "level    =%d\n", cp_data->LEVEL);  ///<    #参加レベル
  OS_TPrintf( "lv_range =%d\n", cp_data->LEVEL_RANGE); ///<    #レベル範囲
  OS_TPrintf( "lv_total =%d\n", cp_data->LEVEL_TOTAL); ///<    #レベル合計
  OS_TPrintf( "both_poke=%d\n", cp_data->BOTH_POKE); ///<    #同じポケモン
  OS_TPrintf( "both_item=%d\n", cp_data->BOTH_ITEM);  ///<    #同じどうぐ             60
  OS_TPrintf( "veto_poke=0x%x\n", cp_data->VETO_POKE_BIT[REG_POKENUM_MAX_BYTE]);  ///<    #参加禁止ポケモン 142
  OS_TPrintf( "veto_item=0x%d\n", cp_data->VETO_ITEM[REG_ITEMNUM_MAX_BYTE]); ///<    #持ち込み禁止道具 218
  OS_TPrintf( "must_poke=%d\n", cp_data->MUST_POKE);   ///<    #必須ポケモン
  OS_TPrintf( "must_form=%d\n", cp_data->MUST_POKE_FORM);   ///<    #必須ポケモンフォルム
  OS_TPrintf( "shooter  =%d\n", cp_data->SHOOTER);    ///<    #シューター
  OS_TPrintf( "time_vs  =%d\n", cp_data->TIME_VS);     ///<    #対戦時間
  OS_TPrintf( "time_cmd =%d\n", cp_data->TIME_COMMAND); ///<    #入力時間
  OS_TPrintf( "nick_view=%d\n", cp_data->NICKNAME); ///<    #ニックネーム表示
  OS_TPrintf( "camera   =%d\n", cp_data->CAMERA);  ///<     #カメラモード
  OS_TPrintf( "show_poke=%d\n", cp_data->SHOW_POKE);  ///<    #ポケモン見せ合い
  OS_TPrintf( "show_time=%d\n", cp_data->SHOW_POKE_TIME);  ///<ポケモン見せ合い時間
  OS_TPrintf( "btl_type =%d\n", cp_data->BATTLE_TYPE);    ///< バトルタイプ  230

  { 
    char str[WIFI_PLAYER_TIX_CUPNAME_MOJINUM + EOM_SIZE];
    DEB_STR_CONV_StrcodeToSjis( cpReg->cupname, str, WIFI_PLAYER_TIX_CUPNAME_MOJINUM + EOM_SIZE ); 
    OS_TPrintf( "cupname =[%s]\n", str );
  }
  OS_TPrintf( "rom_ver  =%d\n", cpReg->ver);//カセットバージョン：
  OS_TPrintf( "cup_no   =%d\n", cpReg->no);//大会No.
  OS_TPrintf( "countory =%d\n", cpReg->code);//国コード：
  OS_TPrintf( "start y  =%d\n", cpReg->start_year);//開始年：00-99
  OS_TPrintf( "start m  =%d\n", cpReg->start_month);//開始月：01-12
  OS_TPrintf( "start d  =%d\n", cpReg->start_day);//開始日：01-31
  OS_TPrintf( "end   y  =%d\n", cpReg->end_year);//終了年：00-99
  OS_TPrintf( "end   m  =%d\n", cpReg->end_month);//終了月：01-12
  OS_TPrintf( "endy  d  =%d\n", cpReg->end_day);//終了日：01-31
  OS_TPrintf( "status   =%d\n", cpReg->status);  //大会状態： net/dreamworld_netdata.hのDREAM_WORLD_MATCHUP_TYPEの値
  OS_TPrintf( "crc      =%d\n", cpReg->crc);  //整合性検査
#endif
}
#endif //PM_DEBUG
