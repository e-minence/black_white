//============================================================================================
/**
 * @file	regulation.h
 * @brief	バトルレギュレーションデータアクセス用ヘッダ
 * @author	k.ohno
 * @date	2006.1.20
 */
//============================================================================================
#pragma once


#include <gflib.h>

//============================================================================================
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	バトルレギュレーションデータ型定義
 */
//----------------------------------------------------------
typedef struct _REGULATION_DATA REGULATION_DATA;


typedef enum {
  REGULATION_CUPNAME_SIZE = 12,      // カップ名の長さ 12文字24バイト +EOM2byte
  REGULATION_RULENAME_SIZE = 12,      // ルール名の長さ 12文字24バイト +EOM2byte
  REGULATION_MAX_NUM = 1,   // １本保存可能
  REGULATION_VS_TIME = 99,        ///< 対戦時間MAX 99分
  REGULATION_COMMAND_TIME = 99,   ///< コマンド入力時間 99秒
  REGULATION_AGE_NONE = 0,   ///< 年齢無制限

} REGULATION_ETC_PARAM_TYPE;

typedef enum  {
  REGULATION_NUM_LO, //    #参加数下限
  REGULATION_NUM_HI, //    #参加数上限
  REGULATION_LEVEL,  //    #参加レベル
  REGULATION_LEVEL_RANGE, //    #レベル範囲
  REGULATION_LEVEL_TOTAL, //    #レベル合計
  REGULATION_BOTH_POKE, //    #同じポケモン
  REGULATION_BOTH_ITEM,  //    #同じどうぐ
  REGULATION_VETO_POKE_BIT,  //    #参加禁止ポケモン
  REGULATION_VETO_ITEM, //    #持ち込み禁止道具
  REGULATION_MUST_POKE,   //    #必須ポケモン
  REGULATION_MUST_POKE_FORM,  //  #必須ポケモンフォルム
  REGULATION_SHOOTER,    //    #シューター
  REGULATION_TIME_VS,     //    #対戦時間
  REGULATION_TIME_COMMAND, //    #入力時間
  REGULATION_NICKNAME, //    #ニックネーム表示
  REGULATION_AGE_LO,  //    #年齢制限以上
  REGULATION_AGE_HI,  //    #年齢制限以下
  REGULATION_SHOW_POKE,  //    #ポケモン見せ合い
} REGULATION_PARAM_TYPE;

typedef enum  {
  REGULATION_LEVEL_RANGE_NORMAL,   ///<なし
  REGULATION_LEVEL_RANGE_OVER,     ///<以上
  REGULATION_LEVEL_RANGE_LESS,     ///<以下
  REGULATION_LEVEL_RANGE_DRAG_DOWN,  ///<引き下げ
  REGULATION_LEVEL_RANGE_SAME,         ///<全補正
  REGULATION_LEVEL_RANGE_PULL_UP,     ///<引き上げ
  REGULATION_LEVEL_RANGE_MAX   //
} REGULATION_LEVEL_RANGE_TYPE;


//----------------------------------------------------------
/**
 * @brief	バトルレギュレーションデータ型定義  fushigi_data.h参照の為外部公開に
 */
//----------------------------------------------------------

#define REG_POKENUM_MAX_BYTE (656/8)  //##このくらいに増えるかも ８２バイト
#define REG_ITEMNUM_MAX_BYTE (608/8)  //##このくらいにふえるかも


typedef struct{
  STRCODE cupName[REGULATION_CUPNAME_SIZE + EOM_SIZE];
  STRCODE ruleName[REGULATION_RULENAME_SIZE + EOM_SIZE];
  u8 NUM_LO; //    #参加数下限
  u8 NUM_HI; //    #参加数上限
  u8 LEVEL;  //    #参加レベル
  u8 LEVEL_RANGE; //    #レベル範囲
  u16 LEVEL_TOTAL; //    #レベル合計
  u8 BOTH_POKE; //    #同じポケモン
  u8 BOTH_ITEM;  //    #同じどうぐ
  u8 VETO_POKE_BIT[REG_POKENUM_MAX_BYTE];  //    #参加禁止ポケモン
  u8 VETO_ITEM[REG_ITEMNUM_MAX_BYTE]; //    #持ち込み禁止道具
  u16 MUST_POKE;   //    #必須ポケモン
  u8 MUST_POKE_FORM;   //    #必須ポケモンフォルム
  u8 SHOOTER;    //    #シューター
  u8 TIME_VS;     //    #対戦時間
  u8 TIME_COMMAND; //    #入力時間
  u8 NICKNAME; //    #ニックネーム表示
  u8 AGE_LO;  //    #年齢制限以上
  u8 AGE_HI;  //    #年齢制限以下
  u8 SHOW_POKE;  //    #ポケモン見せ合い
} REGULATION;

//============================================================================================
//============================================================================================
//----------------------------------------------------------
//	セーブデータシステムや通信で使用する関数
//----------------------------------------------------------
extern int Regulation_GetWorkSize(void);
extern int RegulationData_GetWorkSize(void);
extern REGULATION * Regulation_AllocWork(u32 heapID);
extern void Regulation_Copy(const REGULATION * from, REGULATION * to);
extern int Regulation_Cmp(const REGULATION* pCmp1,const REGULATION* pCmp2);

//----------------------------------------------------------
//	REGULATION操作のための関数
//----------------------------------------------------------
extern void Regulation_Init(REGULATION * my);
extern void RegulationData_Init(REGULATION_DATA * my);

//カップ名
extern void Regulation_SetCupName(REGULATION * pReg, const STRBUF* pCupName);
extern void Regulation_GetCupName(const REGULATION* pReg,STRBUF* pReturnCupName);
extern STRBUF* Regulation_CreateCupName(const REGULATION* pReg, int heapID);

//ルール名
extern void Regulation_SetRuleName(REGULATION * pReg, const STRBUF* pRuleName);
extern void Regulation_GetRuleName(const REGULATION* pReg,STRBUF* pReturnRuleName);
extern STRBUF* Regulation_CreateRuleName(const REGULATION* pReg, int heapID);

extern int Regulation_GetParam(const REGULATION* pReg, REGULATION_PARAM_TYPE type);
extern void Regulation_SetParam(REGULATION * pReg, REGULATION_PARAM_TYPE type, int param);

extern BOOL Regulation_CheckParamBit(const REGULATION* pReg, REGULATION_PARAM_TYPE type, int no);

//----------------------------------------------------------
//	セーブデータ取得のための関数
//----------------------------------------------------------
extern REGULATION* SaveData_GetRegulation(SAVE_CONTROL_WORK* pSave,int regNo);
extern void SaveData_SetRegulation(SAVE_CONTROL_WORK* pSave, const REGULATION* pReg, const int regNo);



