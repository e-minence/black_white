/*
 *  @file   enc_pokeset.c
 *  @brief  エンカウントする野生ポケモンデータ生成
 *  @author Miyuki Iwasawa
 *  @date   09.10.08
 */
#pragma once

#include "field/weather_no.h"
#include "savedata/encount_sv.h"
#include "field_wfbc.h"

#define FLD_ENCPOKE_NUM_MAX (2) ///<一度にエンカウントするポケモン数max

///ENC_POKE_PARAMの値指定
#define POKE_RARE_SEL_DEF   (0)     //通常レア抽選
#define POKE_RARE_SEL_MUST  (1)     //必ずレア
#define POKE_RARE_SEL_NOT   (2)     //必ずレアではない

#define POKE_SEX_SEL_DEFAULT  (0)   //指定無し
#define POKE_SEX_SEL_MALE     (1)   //オス固定
#define POKE_SEX_SEL_FEMALE   (2)   //メス固定

///ランダムポケモン抽選タイプ
typedef enum{
  ENCPROB_CALCTYPE_NORMAL,  ///<ノーマル
  ENCPROB_CALCTYPE_WATER,   ///<水上
  ENCPROB_CALCTYPE_FISHING, ///<釣り
  ENCPROB_CALCTYPE_EQUAL,  ///<1/指定数
  ENCPROB_CALCTYPE_MAX,
}ENCPROB_CALCTYPE;

//======================================================================
//  struct
//======================================================================
typedef struct _ENC_POKE_PARAM
{
  u16 monsNo;
  u16 item;
  u8  level;
  u8  form;
  u8  rare;       //レア制御    0：制御しない　1：レア抽選　2：レア禁止　※レア抽選する処理はＷＢでは未実装（指定しても通常抽選になる）
  u8  spabi_3rd;  //第3特性適用
  u16 waza[4];
  u8 fixsex;
  u8 dummy[3];
}ENC_POKE_PARAM;

typedef struct _ENCPOKE_FLD_PARAM
{
  const GAMEDATA* gdata;
  ENC_SV_PTR enc_save;

  //ロケーション他
  ENCOUNT_LOCATION  location;
  ENCOUNT_TYPE      enc_type;

  ENCPROB_CALCTYPE  prob_calctype;  ///<確率計算タイプ
  u8                tbl_num;  ///<エンカウトデータテーブルの要素数
  u8                enc_poke_num; //エンカウントさせるポケモン数

  //手持ち先頭ポケモンのパラメータ
  u16 mons_no;  //手持ち先頭モンスターNo
  u16 mons_item;  //アイテムチェック
	u8  mons_egg_f; //タマゴフラグチェック
	u8  mons_spa;  //手持ち先頭特性チェック
  u8  mons_sex; //性別チェック
  u8  mons_chr; //性格チェック
	u8  spray_lv; //むしよけスプレー及び低レベルエンカウント回避チェックに用いるレベル

  u32 myID; //プレイヤーのトレーナーＩＤ
  MYSTATUS* my;

  //各種フラグ
  u32 gameclear_f:1;  ///<ゲームクリアしているかどうか？
  u32 fishing_f:1;    ///<釣りエンカウントフラグ
	u32 spray_f:1;     ///<スプレーチェックするかのフラグ	TRUE:チェックする
	u32 enc_force_f:1;  ///<戦闘回避無効	TRUE:無効	FALSE:有効		(現状では、あまいかおり・あまいミツ用)
  u32 companion_f:1;  ///<連れ歩きチェック
  u32 enc_double_f:1;     ///<ダブルエンカウト

  //発動率100
  u32 spa_rate_up:1;            ///<エンカウント率2倍(ありじごく他)
  u32 spa_rate_down:1;          ///<エンカウント率1/2(あくしゅう他)
  u32 spa_fishing_rate_up:1;    ///<釣りの時だけエンカウント率2倍
  u32 spa_item_rate_up:1;       ///<アイテムが手に入りやすい(ふくがん他)
  u32 spa_chr_fix:1;            ///<手持ちと同じ性格(シンクロ他)

  //発動率2/3
  u32 spa_sex_fix:1;      ///<手持ちと異なる性別(メロメロボディ他)

  //発動率1/2
  u32 spa_hagane_up:1;    ///<ハガネタイプとのエンカウント率アップ
  u32 spa_denki_up:1;     ///<電気タイプとのエンカウント率アップ
  u32 spa_high_lv_hit:1;  ///<レベルの高いポケモンとエンカウントしやすい(やるき他)
  u32 spa_low_lv_rm:1;    ///<レベルの低いポケモンとエンカウントしない(威嚇他)

  u32 dmy:16;
}ENCPOKE_FLD_PARAM;

extern void ENCPOKE_SetEFPStruct(ENCPOKE_FLD_PARAM* outEfp, const GAMEDATA* gdata,
    const ENCOUNT_LOCATION location, const ENCOUNT_TYPE enc_type, const WEATHER_NO weather );
extern u16 ENCPOKE_GetGenerateZone( const GAMEDATA* gdata );

extern u32 ENCPOKE_EncProbManipulation(const ENCPOKE_FLD_PARAM* efp, const GAMEDATA* gdata, const u32 inProb);
extern u32 ENCPOKE_GetEncountPoke( const ENCPOKE_FLD_PARAM *efp, const ENC_COMMON_DATA *enc_tbl, ENC_POKE_PARAM* outPokeTbl );
extern int ENCPOKE_GetNormalEncountPokeData( const ENCOUNT_DATA *inData, ENCPOKE_FLD_PARAM* efp, u16 zone_id, ENC_POKE_PARAM* outPokeTbl);
extern MPD_PTR ENCPOKE_GetMovePokeEncountData( const ENCOUNT_DATA *inData, ENCPOKE_FLD_PARAM* efp, u16 zone_id );
extern u32 ENCPOKE_GetWFBCEncountPoke( const FIELD_WFBC* cp_wfbc, ENCPOKE_FLD_PARAM* efp, ENC_POKE_PARAM* outPokeTbl );

extern POKEMON_PARAM* ENCPOKE_PPCreate(const ENCPOKE_FLD_PARAM* efp, const ENC_POKE_PARAM* poke, int heapID);
extern void ENCPOKE_PPSetup(POKEMON_PARAM* pp,const ENCPOKE_FLD_PARAM* efp, const ENC_POKE_PARAM* poke );
extern void ENCPOKE_PPSetupMovePoke(POKEMON_PARAM* pp,const ENCPOKE_FLD_PARAM* efp, MPD_PTR mpd );


