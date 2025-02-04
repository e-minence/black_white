//============================================================================================
/**
 * @file  wordset.c
 * @brief 単語モジュール処理
 * @author  taya
 * @date  2005.09.28
 */
//============================================================================================
#include <gflib.h>

#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_msg.h"
#include "str_tool_local.h"
#include "tr_tool/trtype_def.h" // for TRTYPE_xxxx
#include "savedata/th_rank_def.h" // for TH_RANK_xxxx
#include "../../../resource/research_radar/data/hobby_id.h"  // for HOBBY_ID_xxxx
#include "../../../resource/research_radar/data/job_id.h"  // for JOB_ID_xxxx
#include "../../../resource/research_radar/data/answer_id.h"  // for ANSWER_ID_xxxx
#include "msg/script/msg_trialhouse.h"

#include "arc_def.h"
#include "message.naix"
#include "script_message.naix"


/*----------------------------------*/
/** 文法性                          */
/*----------------------------------*/
enum {
  WORDSET_GRAMMER_NONE,   ///< 使用しない
  WORDSET_GRAMMER_MALE,   ///< 男性
  WORDSET_GRAMMER_FEMALE,   ///< 女性
  WORDSET_GRAMMER_NEUTRAL,  ///< 中性
};


/*----------------------------------*/
/** 単語パラメータ                  */
/*----------------------------------*/
typedef struct {
  u8  def_article;    ///< 定冠詞ＩＤ
  u8  indef_article;  ///< 不定冠詞ＩＤ
  u8  preposition;    ///< 前置詞ＩＤ
  u8  grammer : 7;    ///< 文法性
  u8  form : 1;       ///< 単数・複数（0 = 単数）
  u8  deco_id;        ///< デコ文字ID
}WORDSET_PARAM;


typedef struct {
  WORDSET_PARAM param;
  STRBUF*     str;
}WORD;



struct _WORDSET{

  u32     max;
  u32     heapID;
  WORD*   word;
  STRBUF*   tmpBuf;

};



/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void InitParam(WORDSET_PARAM* param);
static void RegisterWord( WORDSET* wordset, u32 bufID, const STRBUF* str, const WORDSET_PARAM* param );
static void RegisterWordFormMsgArc( WORDSET* wordset, u32 bufID, u32 arc_id, u32 msg_id );
static void ClearBuffer( WORDSET* wordset, u32 bufID );



//======================================================================================================
// システム初期化・終了
//======================================================================================================

//------------------------------------------------------------------
/**
 * 単語セットモジュール作成
 *
 * @param   heapID      作成先ヒープＩＤ
 *
 * @retval  WORDSET*    単語セット
 */
//------------------------------------------------------------------
WORDSET*  WORDSET_Create( HEAPID heapID )
{
  return WORDSET_CreateEx( WORDSET_DEFAULT_SETNUM, WORDSET_DEFAULT_BUFLEN, heapID );
}

//------------------------------------------------------------------
/**
 * 単語セットモジュール作成（登録単語数・文字列長の指定版）
 *
 * @param   word_max
 * @param   buflen
 * @param   heapID
 *
 * @retval  WORDSET*
 */
//------------------------------------------------------------------
WORDSET* WORDSET_CreateEx( u32 word_max, u32 buflen, HEAPID heapID )
{
  GF_ASSERT(word_max);
  GF_ASSERT(buflen);

  {
    WORDSET* wordset = NULL;
    u32 i;

    do {

      WORDSET* wordset = GFL_HEAP_AllocMemory( heapID, sizeof(WORDSET) );
      if( wordset == NULL ){ break; }

      wordset->max = word_max;
      wordset->heapID = heapID;

      wordset->tmpBuf = GFL_STR_CreateBuffer( buflen, heapID );
      if( wordset->tmpBuf == NULL ){ break; }

      wordset->word = GFL_HEAP_AllocMemory( heapID, sizeof(WORD)*word_max );
      if( wordset->word == NULL ){ break; }

      for(i=0; i<word_max; i++)
      {
        InitParam(&(wordset->word[i].param));
        wordset->word[i].str = GFL_STR_CreateBuffer( buflen, heapID );
        if( wordset->word[i].str == NULL ){
          break;
        }
      }

      if( i == word_max )
      {
        return wordset;
      }

    }while(0);

    if( wordset )
    {
      WORDSET_Delete(wordset);
    }
    return NULL;
  }
}

//------------------------------------------------------------------
/**
 * 単語セット破棄
 *
 * @param   wordset
 *
 */
//------------------------------------------------------------------
void WORDSET_Delete( WORDSET* wordset )
{
  u32 i;

  GF_ASSERT(wordset->max);

  if( wordset->word )
  {
    for(i=0; i<wordset->max; i++)
    {
      if( wordset->word[i].str )
      {
        GFL_STR_DeleteBuffer( wordset->word[i].str );
      }
      else
      {
        break;
      }
    }
    GFL_HEAP_FreeMemory( wordset->word );
  }

  if( wordset->tmpBuf )
  {
    GFL_STR_DeleteBuffer(wordset->tmpBuf);
  }

  wordset->max = 0;
  GFL_HEAP_FreeMemory(wordset);
}


//------------------------------------------------------------------
/**
 * 単語パラメータ初期化
 *
 * @param   param   パラメータ構造体へのポインタ
 *
 */
//------------------------------------------------------------------
static void InitParam(WORDSET_PARAM* param)
{
  GFL_STD_MemClear( param, sizeof(*param) );
  param->deco_id = PMS_DECOID_NULL;
}

//------------------------------------------------------------------
/**
 * 指定バッファをクリア(エラー対策)
 *
 * @param   param   パラメータ構造体へのポインタ
 *
 */
//------------------------------------------------------------------
static void ClearBuffer( WORDSET* wordset, u32 bufID )
{
  InitParam( &wordset->word[bufID].param );
  GFL_STR_ClearBuffer( wordset->word[bufID].str );
}



//======================================================================================================
// 単語セットモジュールに各種単語を登録する
//======================================================================================================

//------------------------------------------------------------------
/**
 * 単語文字列＆パラメータを指定バッファに登録
 *
 * @param   wordset   単語セットモジュール
 * @param   bufID   バッファＩＤ
 * @param   str     文字列
 * @param   param   文字列に付随するパラメータ
 *
 */
//------------------------------------------------------------------
static void RegisterWord( WORDSET* wordset, u32 bufID, const STRBUF* str, const WORDSET_PARAM* param )
{
  GF_ASSERT_MSG( bufID < wordset->max, "bufID=%d, wordmax=%d", bufID, wordset->max );

  if( bufID < wordset->max )
  {
    if( param != NULL )
    {
      wordset->word[bufID].param = *param;
    }
    else
    {
      InitParam( &wordset->word[bufID].param );
    }
    GFL_STR_CopyBuffer( wordset->word[bufID].str, str );
  }
}

//------------------------------------------------------------------
/**
 * 指定アーカイブ内の、指定ID文字列をバッファ展開
 *
 * @param   wordset   単語セットモジュール
 * @param   bufID   バッファＩＤ
 * @param   str     文字列
 * @param   param   文字列に付随するパラメータ
 *
 */
//------------------------------------------------------------------
static void RegisterWordFormMsgArc( WORDSET* wordset, u32 bufID, u32 arc_id, u32 msg_id )
{
  GFL_MSGDATA *man = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, arc_id, wordset->heapID);
  if( man )
  {
    GFL_MSG_GetString( man, msg_id, wordset->tmpBuf );
    RegisterWord( wordset, bufID, wordset->tmpBuf, NULL );
    GFL_MSG_Delete(man);
  }
}

//------------------------------------------------------------------
/**
 * 任意文字列をパラメータ付きで登録
 *
 * @param   wordset     単語セット
 * @param   bufID     バッファID
 * @param   word      文字列
 * @param   sex       性別コード
 * @param   singular_flag 単／複（TRUEで単数）
 * @param   lang      言語コード
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterWord( WORDSET* wordset, u32 bufID, const STRBUF* word, u32 sex, BOOL singular_flag, u32 lang )
{
  WORDSET_PARAM  param;

  InitParam( &param );
  param.grammer = sex;
  param.form = !singular_flag;

  RegisterWord( wordset, bufID, word, &param );
}

//------------------------------------------------------------------
/**
 * 指定バッファにポケモン種族名を登録 (MonsNo)
 *
 * @param   bufID   バッファID
 * @param   pp      ポケモンパラメータ
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeMonsNameNo( WORDSET* wordset, u32 bufID, u16 monsno )
{
  // [[[ここでポケモンの性別等をチェックできる]]]
  GFL_MSG_GetString( GlobalMsg_PokeName, monsno, wordset->tmpBuf );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
}

//------------------------------------------------------------------
/**
 * 指定バッファにポケモン種族名を登録 (POKEMON_MARAM)
 *
 * @param   bufID   バッファID
 * @param   pp      ポケモンパラメータ
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeMonsName( WORDSET* wordset, u32 bufID, const POKEMON_PARAM* pp )
{
  u32 monsno;

  // [[[ここでポケモンの性別等をチェックできる]]]
  monsno = PP_Get( pp, ID_PARA_monsno, NULL );
  WORDSET_RegisterPokeMonsNameNo( wordset, bufID, monsno );
}
//------------------------------------------------------------------
/**
 * 指定バッファにポケモン種族名を登録 (POKEMON_PASO_PARAM)
 *
 * @param   bufID   バッファID
 * @param   ppp     ポケモンパラメータ
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeMonsNamePPP( WORDSET* wordset, u32 bufID, const POKEMON_PASO_PARAM* ppp )
{
  u32 monsno;

  // [[[ここでポケモンの性別等をチェックできる]]]
  monsno = PPP_Get( ppp, ID_PARA_monsno, NULL );
  WORDSET_RegisterPokeMonsNameNo( wordset, bufID, monsno );
}

//------------------------------------------------------------------
/**
 * 指定バッファにポケモンのニックネームを登録 (POKEMON_PARAM)
 *
 * @param   bufID   バッファID
 * @param   pp      ポケモンパラメータ
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeNickName( WORDSET* wordset, u32 bufID, const POKEMON_PARAM* pp )
{
  // [[[ここでポケモンの性別等をチェックできる]]]
  PP_Get( pp, ID_PARA_nickname, wordset->tmpBuf );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
}
//------------------------------------------------------------------
/**
 * 指定バッファにポケモンのニックネームを登録 (POKEMON_PASO_PARAM)
 *
 * @param   bufID   バッファID
 * @param   ppp     ポケモンパラメータ
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeNickNamePPP( WORDSET* wordset, u32 bufID, const POKEMON_PASO_PARAM* ppp )
{
  // [[[ここでポケモンの性別等をチェックできる]]]
  PPP_Get( ppp, ID_PARA_nickname, wordset->tmpBuf );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
}
//------------------------------------------------------------------
/**
 * 指定バッファにポケモンの親名を登録
 *
 * @param   bufID   バッファID
 * @param   ppp   ポケモンパラメータ
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeOyaName( WORDSET* wordset,  u32 bufID, const POKEMON_PARAM* pp )
{
  // [[[ここで親の性別もチェックできる]]]
  PP_Get( pp, ID_PARA_oyaname, wordset->tmpBuf );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);

}
//------------------------------------------------------------------
/**
 * 指定バッファにワザ名を登録
 *
 * @param   bufID     バッファID
 * @param   wazaID      ワザID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterWazaName( WORDSET* wordset, u32 bufID, u32 wazaID )
{
  RegisterWordFormMsgArc( wordset, bufID, NARC_message_wazaname_dat, wazaID );
}
//------------------------------------------------------------------
/**
 * 指定バッファにアイテム名
 *
 * @param   bufID     バッファID
 * @param   itemID    アイテムID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterItemName( WORDSET* wordset, u32 bufID, u32 itemID )
{
  RegisterWordFormMsgArc( wordset, bufID, NARC_message_itemname_dat, itemID );
}
//------------------------------------------------------------------
/**
 * 指定バッファにポケモンのとくせい名を登録
 *
 * @param   bufID     バッファID
 * @param   tokuseiID   とくせいID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterTokuseiName( WORDSET* wordset, u32 bufID, u32 tokuseiID )
{
  RegisterWordFormMsgArc( wordset, bufID, NARC_message_tokusei_dat, tokuseiID );
}
//------------------------------------------------------------------
/**
 * 指定バッファにポケモンのせいかくを登録
 *
 * @param   bufID     バッファID
 * @param   seikakuID   せいかくID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterSeikaku( WORDSET* wordset, u32 bufID, u32 seikakuID )
{
  RegisterWordFormMsgArc( wordset, bufID, NARC_message_chr_dat, seikakuID );
}
//------------------------------------------------------------------
/**
 * 指定バッファに数字を登録
 *
 * @param   bufID   バッファID
 * @param   number    数値
 * @param   keta    桁数
 * @param   dispType  整形タイプ
 * @param   codeType  文字コードタイプ
 *
 * dispType :  STR_NUM_DISP_LEFT    左詰め
 *             STR_NUM_DISP_RIGHT   右詰め（スペース埋め）
 *             STR_NUM_DISP_ZERO    右詰め（ゼロ埋め）
 *
 *
 * codeType :  STR_NUM_CODE_DEFAULT  ローカライズ時のデフォルト幅（日本＝全角幅／海外＝半角幅）になる
 *             STR_NUM_CODE_ZENKAKU  全角幅指定
 *             STR_NUM_CODE_HANKAKU  半角幅指定
 */
//------------------------------------------------------------------
void WORDSET_RegisterNumber( WORDSET* wordset, u32 bufID, s32 number, u32 keta, StrNumberDispType dispType, StrNumberCodeType codeType )
{
  STRTOOL_SetNumber( wordset->tmpBuf, number, keta, dispType, codeType );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
}
//------------------------------------------------------------------
/**
 * 指定バッファに16進数数字を登録
 *
 * @param   bufID   バッファID
 * @param   number    数値
 * @param   keta    桁数
 * @param   dispType  整形タイプ
 * @param   codeType  文字コードタイプ
 *
 * dispType :  STR_NUM_DISP_LEFT    左詰め
 *             STR_NUM_DISP_RIGHT   右詰め（スペース埋め）
 *             STR_NUM_DISP_ZERO    右詰め（ゼロ埋め）
 *
 *
 * codeType :  STR_NUM_CODE_DEFAULT  ローカライズ時のデフォルト幅（日本＝全角幅／海外＝半角幅）になる
 *             STR_NUM_CODE_ZENKAKU  全角幅指定
 *             STR_NUM_CODE_HANKAKU  半角幅指定
 */
//------------------------------------------------------------------
void WORDSET_RegisterHexNumber( WORDSET* wordset, u32 bufID, u32 number, u32 keta, StrNumberDispType dispType, StrNumberCodeType codeType )
{
  STRTOOL_SetHexNumber( wordset->tmpBuf, number, keta, dispType, codeType );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
}


//------------------------------------------------------------------
/**
 * 指定バッファに簡易会話単語を登録
 *
 * @param   wordset   ワードセットオブジェクト
 * @param   bufID   何番のバッファに登録するか
 * @param   word    簡易会話単語ナンバー
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPMSWord( WORDSET* wordset, u32 bufID, PMS_WORD word )
{
  // データの不正チェックを行い、不正だった場合は正しいデータに強制的に変更する
  PMS_WORD correct_word = word;
  PMSDAT_CorrectWord( &correct_word, FALSE, FALSE );

  //PMSW_GetStr( word, wordset->tmpBuf , wordset->heapID );
  PMSW_GetStr( correct_word, wordset->tmpBuf , wordset->heapID );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL );
}
//------------------------------------------------------------------
/**
 * 指定バッファに簡易会話デコ文字を登録
 *
 * @param   wordset   ワードセットオブジェクト
 * @param   bufID     何番のバッファに登録するか
 * @param   decoID    デコ文字
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPMSDeco( WORDSET* wordset, u32 bufID, PMS_DECO_ID decoID )
{
  // データの不正チェックを行い、不正だった場合は正しいデータに強制的に変更する
  PMS_DECO_ID correct_deco_id = decoID;
  PMSDAT_CorrectDecoId( &correct_deco_id, FALSE );

  GF_ASSERT_MSG( bufID < wordset->max, "bufID=%d, wordmax=%d", bufID, wordset->max );

  if( bufID < wordset->max )
  {
    //wordset->word[bufID].param.deco_id = decoID;
    wordset->word[bufID].param.deco_id = correct_deco_id;
  }
}

//------------------------------------------------------------------
/**
 * 指定バッファにポケモン（ワザ）タイプ名を登録
 *
 * @param   bufID     バッファID
 * @param   typeID      タイプID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeTypeName( WORDSET* wordset, u32 bufID, u32 typeID )
{
  RegisterWordFormMsgArc( wordset, bufID, NARC_message_typename_dat, typeID );
}

//------------------------------------------------------------------
/**
 * プレイヤー名を登録
 *
 * @param   wordset   単語セットオブジェクト
 * @param   bufID   バッファID
 * @param   status
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPlayerName( WORDSET* wordset, u32 bufID, const MYSTATUS* my )
{
  MyStatus_CopyNameString( my, wordset->tmpBuf );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL );
}

//------------------------------------------------------------------
/**
 * 指定バッファに国名を登録
 *
 * @param   wordset   ワードセットオブジェクト
 * @param   bufID   何番のバッファに登録するか
 * @param   strID   国ID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterCountryName( WORDSET* wordset, u32 bufID, u32 countryID )
{
  GFL_MSGDATA *man = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_place_msg_world_dat, wordset->heapID);
  if( man )
  {
    int str_count = GFL_MSG_GetStrCount( man );
    if( countryID < str_count ){
      GFL_MSG_GetString( man, countryID, wordset->tmpBuf );
      RegisterWord( wordset, bufID, wordset->tmpBuf, NULL );
    }else{
      GF_ASSERT_MSG( countryID < str_count, "countryID=%d, countryID Max=%d", countryID, str_count );
      ClearBuffer( wordset, bufID );
    }
    GFL_MSG_Delete(man);
  }
}

//------------------------------------------------------------------
/**
 * 指定バッファに地域名を登録
 *
 * @param   wordset   ワードセットオブジェクト
 * @param   bufID   何番のバッファに登録するか
 * @param   countryID 国ID
 * @param   placeID   地域ID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterLocalPlaceName( WORDSET* wordset, u32 bufID, u32 countryID, u32 placeID )
{
  u32  datID;

  datID = WIFI_COUNTRY_CountryCodeToPlaceMsgDataID(countryID);
  if( datID )
  {
    if( placeID )
    {
      GFL_MSGDATA *man = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, datID, wordset->heapID);
      if( man )
      {
        int str_count = GFL_MSG_GetStrCount( man );
        if( placeID < str_count ){
          GFL_MSG_GetString( man, placeID, wordset->tmpBuf );
          RegisterWord( wordset, bufID, wordset->tmpBuf, NULL );
          GFL_MSG_Delete(man);
          return;
        }
        GF_ASSERT_MSG( placeID < str_count, "palceID=%d, placeID max=%d", placeID, str_count );
        GFL_MSG_Delete(man);
      }
    }
  }
  ClearBuffer( wordset, bufID );
}

//------------------------------------------------------------------
/**
 * 指定バッファにトレーナー種別を登録
 *
 * @param wordset ワードセットオブジェクト
 * @param bufID   何番のバッファに登録するか
 * @param trID    トレーナータイプ ( TRTYPE_xxxx )
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterTrTypeName( WORDSET* wordset, u32 bufID, u8 trType )
{
  GF_ASSERT( trType < TRTYPE_MAX );
  RegisterWordFormMsgArc( wordset, bufID, NARC_message_trtype_dat, trType );
}

//------------------------------------------------------------------
/**
 * 指定バッファにトレーナー種別を登録 ( トレーナーIDを指定 )
 *
 * @param   wordset ワードセットオブジェクト
 * @param   bufID   何番のバッファに登録するか
 * @param   trID    トレーナーID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterTrTypeName_byTrID( WORDSET* wordset, u32 bufID, TrainerID trID )
{
  //tr_tool/tr_tool.hをインクルードして
  //TT_TrainerTypeSexGet(trID)とすることでトレーナーの性別が取得できます
  int tr_type = TT_TrainerDataParaGet( trID, ID_TD_tr_type );
  WORDSET_RegisterTrTypeName( wordset, bufID, tr_type );
}

//------------------------------------------------------------------
/**
 * 指定バッファにトレーナー名を登録
 *
 * @param   wordset   ワードセットオブジェクト
 * @param   bufID     何番のバッファに登録するか
 * @param   trID      トレーナー種別ID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterTrainerName( WORDSET* wordset, u32 bufID, TrainerID trID )
{
  RegisterWordFormMsgArc( wordset, bufID, NARC_message_trname_dat, trID );
}

//------------------------------------------------------------------
/**
 * 指定バッファにステータス名を登録
 *
 * @param   wordset   ワードセットオブジェクト
 * @param   bufID     何番のバッファに登録するか
 * @param   trID      ステータス(体力・攻撃・防御・素早さ・特攻・特防・命中・回避・最大HP
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeStatusName( WORDSET* wordset, u32 bufID, u8 statusID )
{
  RegisterWordFormMsgArc( wordset, bufID, NARC_message_status_dat, statusID );
}

//------------------------------------------------------------------
/**
 * 指定バッファにアイテムを入れるポケット名を登録
 *
 * @param   bufID     バッファID
 * @param   wazaID      ポケットID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterItemPocketName( WORDSET* wordset, u32 bufID, u32 pocketID )
{
  RegisterWordFormMsgArc( wordset, bufID, NARC_message_itempocket_dat, pocketID );
}

//------------------------------------------------------------------
/**
 * @brief 指定バッファに地名を登録
 *
 * @param bufID  バッファID
 * @param placeNameID 地名ID
 *
 * ゾーンIDから地名IDを取得するには ZONEDATA_GetPlaceNameID(zone_id)を用いてください
 */
//------------------------------------------------------------------
void WORDSET_RegisterPlaceName( WORDSET* wordset, u32 bufID, u32 placeNameID )
{
  RegisterWordFormMsgArc( wordset, bufID, NARC_message_place_name_dat, placeNameID );
}

//------------------------------------------------------------------
/**
 * @brief 指定バッファにボックス名を登録
 *
 * @param bufID         バッファID
 * @param box           ボックスデータポインタ
 * @param trayNumber    トレイナンバー
 */
//------------------------------------------------------------------
void WORDSET_RegisterBoxName( WORDSET* wordset, u32 bufID, const BOX_MANAGER* box, u32 trayNumber )
{
  BOXDAT_GetBoxName( box, trayNumber, wordset->tmpBuf );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL );
}

//------------------------------------------------------------------
/**
 * @brief 指定バッファにGパワー名を登録
 *
 * @param bufID  バッファID
 * @param zoneID ゾーンID
 */
//------------------------------------------------------------------
void WORDSET_RegisterGPowerName( WORDSET* wordset, u32 bufID, GPOWER_ID g_power )
{
  if( g_power >= GPOWER_ID_MAX ){
    GF_ASSERT( g_power < GPOWER_ID_MAX );
    g_power = 0;
  }
  RegisterWordFormMsgArc( wordset, bufID, NARC_message_power_dat, g_power );
}

//------------------------------------------------------------------
/**
 * @brief 指定バッファに趣味名を登録
 *
 * @param bufID   バッファID
 * @param hobbyID 趣味ID
 */
//------------------------------------------------------------------
void WORDSET_RegisterHobbyName( WORDSET* wordset, u32 bufID, u8 hobbyID )
{
  // 趣味IDエラー
  if( HOBBY_ID_MAX < hobbyID )
  {
    GF_ASSERT(0);
    ClearBuffer( wordset, bufID );
    return;
  }
  RegisterWordFormMsgArc( wordset, bufID, NARC_message_hobby_dat, hobbyID );
}

//------------------------------------------------------------------
/**
 * @brief 指定バッファに回答名を登録
 *
 * @param bufID    バッファID
 * @param answerID 回答ID ( ANSWER_ID_xxxx )
 */
//------------------------------------------------------------------
void WORDSET_RegisterAnswerName( WORDSET* wordset, u32 bufID, u8 answerID )
{
  // 回答IDエラー
  if( ANSWER_ID_MAX < answerID )
  {
    GF_ASSERT(0);
    ClearBuffer( wordset, bufID );
    return;
  }
  RegisterWordFormMsgArc( wordset, bufID, NARC_message_answer_dat, answerID );
}

//------------------------------------------------------------------
/**
 * @brief 指定バッファに仕事名を登録
 *
 * @param bufID   バッファID
 * @param jobID   仕事ID
 */
//------------------------------------------------------------------
void WORDSET_RegisterJobName( WORDSET* wordset, u32 bufID, u8 jobID )
{
  // 仕事IDエラー
  if( JOB_ID_MAX < jobID )
  {
    GF_ASSERT(0);
    ClearBuffer( wordset, bufID );
    return;
  }
  RegisterWordFormMsgArc( wordset, bufID, NARC_message_job_dat, jobID );
}

//------------------------------------------------------------------
/**
 * @brief 指定バッファにトライアルハウスランク名を展開
 *
 * @param bufID   バッファID
 * @param rank    トライアルハウスのランク
 */
//------------------------------------------------------------------
//マルチブート用きり分け  (MB子機ではARCID_SCRIPT_MESSAGE無いので通らないようにします Ariizumi100326
#ifndef MULTI_BOOT_MAKE  //通常時処理
void WORDSET_RegisterTrialHouseRank( WORDSET* wordset, u32 bufID, u8 rank )
{
  GFL_MSGDATA *msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
      ARCID_SCRIPT_MESSAGE, NARC_script_message_trialhouse_dat, wordset->heapID );

  ClearBuffer( wordset, bufID );

  // ランクエラー
  if( TH_RANK_MASTER < rank )
  {
    GF_ASSERT(0);
    return;
  }

  if( msg )
  {
    GFL_MSG_GetString( msg, rank+msg_trialhouse_rank0, wordset->tmpBuf );
    RegisterWord( wordset, bufID, wordset->tmpBuf, NULL );
    GFL_MSG_Delete( msg );
  }
}
#endif //MULTI_BOOT_MAKE




//======================================================================================================
// 文字列展開
//======================================================================================================

//------------------------------------------------------------------
/**
 * 登録された単語を使って文字列展開する
 *
 * @param   dst   展開先バッファ
 * @param   src   展開元文字列
 *
 */
//------------------------------------------------------------------
void WORDSET_ExpandStr( const WORDSET* wordset, STRBUF* dstbuf, const STRBUF* srcbuf )
{
  const STRCODE* src;
  STRCODE *dst;
  STRCODE EOM_Code, TAG_Code;

  src = GFL_STR_GetStringCodePointer( srcbuf );
  GFL_STR_ClearBuffer( dstbuf );
  EOM_Code = GFL_STR_GetEOMCode();
  TAG_Code = PRINTSYS_GetTagStartCode();

  while( *src != EOM_Code )
  {
    if( *src == TAG_Code )
    {
      if( PRINTSYS_IsWordSetTagGroup(src) )
      {
        u32 word_id;

        word_id = PRINTSYS_GetTagParam( src, 0 );
        GF_ASSERT( word_id < wordset->max );

        // デコキャラが出てきたらＸ加算タグを埋め込んでおく
        if( wordset->word[ word_id ].param.deco_id != PMS_DECOID_NULL ){
          u16 add_x = PMS_DECO_WIDTH;
          PRINTSYS_CreateTagCode( wordset->word[word_id].str, PRINTSYS_TAGGROUP_CTRL_GEN, PRINTSYS_CTRL_GENERAL_X_ADD, 1, &add_x );
        }

        // [[[海外版ではパラメータの解釈が必要]]]
        GFL_STR_AddString( dstbuf, wordset->word[ word_id ].str );
        src = PRINTSYS_SkipTag( src );
      }
      else
      {
        const STRCODE* p = src;

        src = PRINTSYS_SkipTag( src );
        while(p < src)
        {
          GFL_STR_AddCode( dstbuf, *p++ );
        }
      }
    }
    else
    {
      GFL_STR_AddCode( dstbuf, *src++ );
    }
  }
}




//======================================================================================================
// バッファクリア
//======================================================================================================

//------------------------------------------------------------------
/**
 * 内部バッファを全て空文字列でクリアする
 *
 * @param   wordset   WORDSETへのポインタ
 *
 */
//------------------------------------------------------------------
void WORDSET_ClearAllBuffer( WORDSET* wordset )
{
  u32 i;
  for(i=0; i<wordset->max; i++)
  {
    GFL_STR_ClearBuffer( wordset->word[i].str );
  }
}




