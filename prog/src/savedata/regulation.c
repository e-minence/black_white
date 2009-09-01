//============================================================================================
/**
 * @file	regulation.c
 * @brief	バトルレギュレーションデータアクセス用ソース
 * @author	k.ohno
 * @date	2006.1.20
 */
//============================================================================================

#include <gflib.h>

#include "savedata/save_tbl.h"	//SAVEDATA参照のため
#include "buflen.h"
#include "savedata/save_control.h"
#include "savedata/regulation.h"
#include "poke_tool/monsno_def.h"


// 最大数のレギュレーション確保
struct _REGULATION_DATA {
  REGULATION regulation_buff[REGULATION_MAX_NUM];
};


//============================================================================================
//============================================================================================

//============================================================================================
//
//	セーブデータシステムが依存する関数
//
//============================================================================================
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
  return sizeof(REGULATION_DATA);
}

//----------------------------------------------------------
/**
 * @brief	バトルレギュレーションワークの確保
 * @param	heapID		メモリ確保をおこなうヒープ指定
 * @return	REGULATION*	取得したワークへのポインタ
 */
//----------------------------------------------------------
REGULATION* Regulation_AllocWork(u32 heapID)
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
void Regulation_Init(REGULATION* pReg)
{
  GFL_STD_MemClear(pReg, sizeof(REGULATION));
}

//----------------------------------------------------------
/**
 * @brief	バトルレギュレーションデータの初期化
 * @param	pReg		REGULATIONワークへのポインタ
 */
//----------------------------------------------------------
void RegulationData_Init(REGULATION_DATA* pRegData)
{
  GFL_STD_MemClear(pRegData, sizeof(REGULATION_DATA));
}

//----------------------------------------------------------
/**
 * @brief	カップ名セット
 * @param	pReg	REGULATIONワークポインタ
 * @param	pCupBuf	カップ名が入ったバッファ
 */
//----------------------------------------------------------
void Regulation_SetCupName(REGULATION* pReg, const STRBUF *pCupBuf)
{
  GFL_STR_GetStringCode(pCupBuf, pReg->cupName, (REGULATION_CUPNAME_SIZE + EOM_SIZE));
}

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
  GFL_STR_SetStringCodeOrderLength(pReturnCupName, pReg->cupName, (REGULATION_CUPNAME_SIZE + EOM_SIZE));
}

//----------------------------------------------------------
/**
 * @brief	カップ名前取得（STRBUFを生成）
 * @param	pReg	ワークへのポインタ
 * @param	heapID	STRBUFを生成するヒープのID
 * @return	STRBUF	名前を格納したSTRBUFへのポインタ
 */
//----------------------------------------------------------
STRBUF* Regulation_CreateCupName(const REGULATION* pReg, int heapID)
{
  STRBUF* tmpBuf = GFL_STR_CreateBuffer((REGULATION_CUPNAME_SIZE + EOM_SIZE)*GLOBAL_MSGLEN, heapID);
  GFL_STR_SetStringCode( tmpBuf, pReg->cupName );
  return tmpBuf;
}

//----------------------------------------------------------
/**
 * @brief	ルール名セット
 * @param	pReg	REGULATIONワークポインタ
 * @param	pCupBuf	カップ名が入ったバッファ
 */
//----------------------------------------------------------
void Regulation_SetRuleName(REGULATION* pReg, const STRBUF *pRuleBuf)
{
  GFL_STR_GetStringCode(pRuleBuf, pReg->ruleName, (REGULATION_RULENAME_SIZE + EOM_SIZE));
}

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
  GFL_STR_SetStringCodeOrderLength(pReturnRuleName, pReg->ruleName, (REGULATION_RULENAME_SIZE + EOM_SIZE));
}

//----------------------------------------------------------
/**
 * @brief	ルール名前取得（STRBUFを生成）
 * @param	pReg	ワークへのポインタ
 * @param	heapID	STRBUFを生成するヒープのID
 * @return	STRBUF	名前を格納したSTRBUFへのポインタ
 */
//----------------------------------------------------------
STRBUF* Regulation_CreateRuleName(const REGULATION* pReg, int heapID)
{
  STRBUF* tmpBuf = GFL_STR_CreateBuffer((REGULATION_RULENAME_SIZE + EOM_SIZE)*GLOBAL_MSGLEN, heapID);
  GFL_STR_SetStringCode( tmpBuf, pReg->ruleName );
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
  case REGULATION_AGE_LO:  //    #年齢制限以上
    ret = pReg->AGE_LO;
    break;
  case REGULATION_AGE_HI:  //    #年齢制限以下
    ret = pReg->AGE_HI;
    break;
  case REGULATION_SHOW_POKE:  //    #ポケモン見せ合い
    ret = pReg->SHOW_POKE;
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
  case REGULATION_NUM_LO: //    #参加数下限
    if(pReg->NUM_LO > 6){
      GF_ASSERT(0);
      return;
    }
    pReg->NUM_LO = param;
    break;
  case REGULATION_NUM_HI:         //参加数上限
    if(pReg->NUM_LO > 6){
      GF_ASSERT(0);
      return;
    }
    pReg->NUM_HI = param;
    break;
  case REGULATION_LEVEL:  //    #参加レベル
    if(pReg->NUM_LO > 100){
      GF_ASSERT(0);
      return;
    }
    pReg->LEVEL = param;
    break;
  case REGULATION_LEVEL_RANGE: //    #レベル範囲
    if(pReg->LEVEL_RANGE > REGULATION_LEVEL_RANGE_MAX){
      GF_ASSERT(0);
      return;
    }
    pReg->LEVEL_RANGE = param;
    break;
  case REGULATION_LEVEL_TOTAL: //    #レベル合計
    if(pReg->LEVEL_RANGE > 600){
      GF_ASSERT(0);
      return;
    }
    pReg->LEVEL_TOTAL = param;
    break;
  case REGULATION_BOTH_POKE: //    #同じポケモン
    if(pReg->BOTH_POKE > 2){
      GF_ASSERT(0);
      return;
    }
    pReg->BOTH_POKE = param;
    break;
  case REGULATION_BOTH_ITEM:  //    #同じどうぐ
    if(pReg->BOTH_ITEM > 2){
      GF_ASSERT(0);
      return;
    }
    pReg->BOTH_ITEM = param;
    break;
  case REGULATION_MUST_POKE:   //    #必須ポケモン
    if(pReg->MUST_POKE > MONSNO_MAX){ 
      GF_ASSERT(0);
      return;
    }
    pReg->MUST_POKE = param;
    break;
  case REGULATION_MUST_POKE_FORM:   //    #必須ポケモンフォルム
    pReg->MUST_POKE_FORM = param;
    break;
  case REGULATION_SHOOTER:    //    #シューター
    if(pReg->SHOOTER > 2){
      GF_ASSERT(0);
      return;
    }
    pReg->SHOOTER = param;
    break;
  case REGULATION_TIME_VS:     //    #対戦時間
    if(pReg->TIME_VS > REGULATION_VS_TIME){
      GF_ASSERT(0);
      return;
    }
    pReg->TIME_VS = param;
    break;
  case REGULATION_TIME_COMMAND: //    #入力時間
    if(pReg->TIME_COMMAND > REGULATION_COMMAND_TIME){
      GF_ASSERT(0);
      return;
    }
    pReg->TIME_COMMAND = param;
    break;
  case REGULATION_NICKNAME: //    #ニックネーム表示
    if(pReg->NICKNAME > 2){
      GF_ASSERT(0);
      return;
    }
    pReg->NICKNAME = param;
    break;
  case REGULATION_AGE_LO:  //    #年齢制限以上
    pReg->AGE_LO = param;
    break;
  case REGULATION_AGE_HI:  //    #年齢制限以下
    pReg->AGE_HI = param;
    break;
  case REGULATION_SHOW_POKE:  //    #ポケモン見せ合い
    if(pReg->SHOW_POKE > 2){
      GF_ASSERT(0);
      return;
    }
    pReg->SHOW_POKE = param;
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
  }
  return FALSE;
}


//----------------------------------------------------------
/**
 * @brief	レギュレーションデータへのポインタ取得
 * @param	pSave    	セーブデータ保持ワークへのポインタ
 * @param	何本目のレギュレーションデータか
 * @return	REGULATION	ワークへのポインタ  無効データの場合NULL
 */
//----------------------------------------------------------
REGULATION* SaveData_GetRegulation(SAVE_CONTROL_WORK* pSave, int regNo)
{
  REGULATION_DATA* pRegData = NULL;

  GF_ASSERT(regNo < REGULATION_MAX_NUM);
  if(regNo < REGULATION_MAX_NUM){
    pRegData = SaveControl_DataPtrGet(pSave, GMDATA_ID_REGULATION_DATA);
    return &pRegData->regulation_buff[regNo];
  }
  return NULL;
}

//----------------------------------------------------------
/**
 * @brief	レギュレーションデータの保存
 * @param	pSave    	セーブデータ保持ワークへのポインタ
 * @param	const REGULATION  レギュレーションデータ
 * @return	none
 */
//----------------------------------------------------------
void SaveData_SetRegulation(SAVE_CONTROL_WORK* pSave, const REGULATION* pReg, const int regNo)
{
  REGULATION_DATA* pRegData = NULL;

  pRegData = SaveControl_DataPtrGet(pSave, GMDATA_ID_REGULATION_DATA);
  GF_ASSERT(regNo < REGULATION_MAX_NUM);
  if(regNo < REGULATION_MAX_NUM){
    Regulation_Copy(pReg, &pRegData->regulation_buff[regNo]);
  }
}

