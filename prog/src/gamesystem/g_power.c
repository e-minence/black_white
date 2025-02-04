//==============================================================================
/**
 * @file    g_power.c
 * @brief   Gパワー
 * @author  matsuda
 * @date    2010.02.07(日)
 */
//==============================================================================
#include <gflib.h>
#include "field/gpower_id.h"
#include "gamesystem/g_power.h"
#include "waza_tool/wazano_def.h"
#include "arc_def.h"
#include "power.naix"
#include "poke_tool/status_rcv.h"


//==============================================================================
//  定数定義
//==============================================================================
///1秒のフレーム数
#define ONE_SECOND_FRAME    (60)
///1分のフレーム数
#define ONE_MINUTE_FRAME    (ONE_SECOND_FRAME * 60)


//==============================================================================
//  構造体定義
//==============================================================================
typedef struct{
  u32 life[GPOWER_TYPE_MAX];        ///<発生寿命
  u8 occur_power[GPOWER_TYPE_MAX];  ///<発生中のGパワーID(TYPE順に発動中のIDが入っている)
  u16 powerdata_data[GPOWER_TYPE_MAX];  ///<発生中のGパワーのPOWER_CONV_DATA.data
  u8 my_power_type;                 ///<自分が発動しているパワーのタイプ
  u8 padding[3];
}GPOWER_SYSTEM;


//==============================================================================
//  ローカル変数
//==============================================================================
static GPOWER_SYSTEM GPowerSys = {0};


//==============================================================================
//  プロトタイプ宣言
//==============================================================================
static void _OccurPowerClear(GPOWER_TYPE type);
static void _ErrorCheck(void);



//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * Gパワーシステム初期化
 *
 * @param   none		
 */
//==================================================================
void GPower_SYSTEM_Init(void)
{
  GFL_STD_MemClear(&GPowerSys, sizeof(GPOWER_SYSTEM));
  GPOWER_Clear_AllPower();
}

//==================================================================
/**
 * Gパワーシステム更新処理
 *
 * @param   none		
 */
//==================================================================
void GPOWER_SYSTEM_Update(void)
{
  GPOWER_TYPE type;
  
  for(type = 0; type < GPOWER_TYPE_MAX; type++){
    if(GPowerSys.life[type] > 1){
      GPowerSys.life[type]--;
    }
  }
  _ErrorCheck();
}

//==================================================================
/**
 * 発動中のGパワーを全てクリア
 */
//==================================================================
void GPOWER_Clear_AllPower(void)
{
  GPOWER_TYPE type;
  
  for(type = 0; type < GPOWER_TYPE_MAX; type++){
    _OccurPowerClear(type);
  }
  GPowerSys.my_power_type = GPOWER_TYPE_NULL;
}

//==================================================================
/**
 * 発動するGパワーIDをセット
 *
 * @param   gpower_id		
 * @param   powerdata   パワーデータへのポインタ
 * @param   my_power    TRUE:自分のパワーである　FALSE:他人のパワーである
 */
//==================================================================
void GPOWER_Set_OccurID(GPOWER_ID gpower_id, const POWER_CONV_DATA *powerdata, BOOL my_power)
{
  GPOWER_TYPE type;
  
  if(gpower_id >= GPOWER_ID_MAX){
    GF_ASSERT_MSG(0, "gpower_id = %d", gpower_id);
    return;
  }
  
  type = powerdata[gpower_id].type;
  GPowerSys.occur_power[type] = gpower_id;
  GPowerSys.life[type] = powerdata[gpower_id].time * ONE_MINUTE_FRAME;
  GPowerSys.powerdata_data[type] = powerdata[gpower_id].data;
  if(GPowerSys.my_power_type == type){
    GPowerSys.my_power_type = GPOWER_TYPE_NULL;
  }
  if(my_power == TRUE){
    GPowerSys.my_power_type = type;
  }
  
  OS_TPrintf("GPower Set id=%d\n", gpower_id);
  
  //同時に発動しないパワーのチェック
  switch(type){
  case GPOWER_TYPE_ENCOUNT_UP:
    _OccurPowerClear(GPOWER_TYPE_ENCOUNT_DOWN);
    break;
  case GPOWER_TYPE_ENCOUNT_DOWN:
    _OccurPowerClear(GPOWER_TYPE_ENCOUNT_UP);
    break;
  case GPOWER_TYPE_EXP_UP:
    _OccurPowerClear(GPOWER_TYPE_EXP_DOWN);
    break;
  case GPOWER_TYPE_EXP_DOWN:
    _OccurPowerClear(GPOWER_TYPE_EXP_UP);
    break;
  }
}

//==================================================================
/**
 * 指定したGパワーIDが発動しているかチェック
 *
 * @param   gpower_id		GパワーID
 * @param   powerdata		パワーデータへのポインタ
 *
 * @retval  GPOWER_ID		GPOWER_ID_NULLが戻ってきた場合：パワーは発動していない
 * @retval  GPOWER_ID		指定IDと同じIDが戻ってきた場合：指定IDのパワーが発動している
 * @retval  GPOWER_ID		指定IDと違うIDが戻ってきた場合：指定IDと同じタイプの別パワーが発動している
 */
//==================================================================
GPOWER_ID GPOWER_Check_OccurID(GPOWER_ID gpower_id, const POWER_CONV_DATA *powerdata)
{
  return GPowerSys.occur_power[powerdata[gpower_id].type];
}

//==================================================================
/**
 * 指定したタイプのパワーが発動しているかチェック
 *
 * @param   type		    Gパワータイプ
 *
 * @retval  GPOWER_ID		発動しているGパワーID (発動していない場合はGPOWER_ID_NULL)
 */
//==================================================================
GPOWER_ID GPOWER_Check_OccurType(GPOWER_TYPE type)
{
  return GPowerSys.occur_power[type];
}

//==================================================================
/**
 * 指定したタイプのパワーが発動しているかチェック
 *
 * @param   type		    Gパワータイプ
 *
 * @retval  u32		0:発動していない 1以上:残りライフ(秒)
 */
//==================================================================
u32 GPOWER_Check_OccurTypeLife( GPOWER_TYPE type )
{
  u32 sec;
  if( GPowerSys.occur_power[type] == GPOWER_ID_NULL ){
    return 0;
  }
  sec = GPowerSys.life[type]/ONE_SECOND_FRAME;
  if( sec == 0){
    return 1; //1秒以下は1秒と返すことにする
  }
  return sec;
}

//==================================================================
/**
 * 自分のパワーが発動しているかチェック
 *
 * @retval  u32		0:発動していない 1以上:残りライフ(秒)
 */
//==================================================================
u32 GPOWER_Check_MyPower(void)
{
  u32 sec;
  if(GPowerSys.my_power_type == GPOWER_TYPE_NULL){
    return 0;
  }
  sec = GPowerSys.life[GPowerSys.my_power_type]/ONE_SECOND_FRAME;
  if( sec == 0){
    return 1; //1秒以下は1秒と返すことにする
  }
  return sec;
}

//==================================================================
/**
 * Finish待ちになっているGパワーIDを取得する
 *
 * @retval  GPOWER_ID		GパワーID(Finish待ちが1つも無い場合はGPOWER_ID_NULL)
 */
//==================================================================
GPOWER_ID GPOWER_Get_FinishWaitID(void)
{
  GPOWER_TYPE type;
  
  for(type = 0; type < GPOWER_TYPE_MAX; type++){
    if(GPowerSys.life[type] == 1){
      return GPowerSys.occur_power[type];
    }
  }
  
  return GPOWER_ID_NULL;
}

//==================================================================
/**
 * Finish待ちのGパワーIDをFinishさせる
 *
 * @param   gpower_id		
 * @param   powerdata		
 */
//==================================================================
void GPOWER_Set_Finish(GPOWER_ID gpower_id, const POWER_CONV_DATA *powerdata)
{
  GF_ASSERT(GPowerSys.life[powerdata[gpower_id].type] == 1);
  _OccurPowerClear(powerdata[gpower_id].type);
}

//--------------------------------------------------------------
/**
 * 発動中のGパワーをタイプ指定してクリアする
 * @param   type		Gパワータイプ
 */
//--------------------------------------------------------------
static void _OccurPowerClear(GPOWER_TYPE type)
{
  if(type >= GPOWER_TYPE_MAX){
    GF_ASSERT_MSG(0, "type=%d\n", type);
    return;
  }
  GPowerSys.occur_power[type] = GPOWER_ID_NULL;
  GPowerSys.life[type] = 0;
  if(GPowerSys.my_power_type == type){
    GPowerSys.my_power_type = GPOWER_TYPE_NULL;
  }
  OS_TPrintf("GPower Clear type=%d\n", type);
}

//==================================================================
/**
 * パワーデータをAllocしたワークに読み込む
 *
 * @param   heap_id		ヒープID
 * @retval  POWER_CONV_DATA		パワーデータへのポインタ
 */
//==================================================================
POWER_CONV_DATA * GPOWER_PowerData_LoadAlloc(HEAPID heap_id)
{
  return GFL_ARC_LoadDataAlloc(ARCID_POWER, NARC_power_power_data_bin, heap_id);
}

//==================================================================
/**
 * パワーデータをUnloadする
 *
 * @param   powerdata		パワーデータへのポインタ
 */
//==================================================================
void GPOWER_PowerData_Unload(POWER_CONV_DATA *powerdata)
{
  GFL_HEAP_FreeMemory(powerdata);
}

//==================================================================
/**
 * GパワーIDからGパワータイプを取り出す
 *
 * @param   powerdata		パワーデータへのポインタ
 * @param   gpower_id		GパワーID
 *
 * @retval  GPOWER_TYPE		Gパワータイプ
 */
//==================================================================
GPOWER_TYPE GPOWER_ID_to_Type(const POWER_CONV_DATA *powerdata, GPOWER_ID gpower_id)
{
  return powerdata[gpower_id].type;
}

//==================================================================
/**
 * GパワーIDから使用ポイント(デルダマ消費数)を取得する
 *
 * @param   powerdata		パワーデータへのポインタ
 * @param   gpower_id		GパワーID
 *
 * @retval  使用ポイント(デルダマ消費数)
 */
//==================================================================
u16 GPOWER_ID_to_Point(const POWER_CONV_DATA *powerdata, GPOWER_ID gpower_id)
{
  return powerdata[gpower_id].point;
}

//Gパワーがバグっていないか簡易的なチェック
static void _ErrorCheck(void)
{
#ifdef PM_DEBUG
  GPOWER_TYPE type;
  int zero_count = 0;
  
  for(type = 0; type < GPOWER_TYPE_MAX; type++){
    if(GPowerSys.occur_power[type] == 0){
      zero_count++;
    }
  }
  if(zero_count == GPOWER_TYPE_MAX){
    GF_ASSERT(0);
  }
#endif
}


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * Gパワー計算：エンカウント率
 *
 * @param   encount		現在のエンカウント率(上限100%)
 *
 * @retval  u32		Gパワー影響後のエンカウント率
 */
//==================================================================
u32 GPOWER_Calc_Encount(u32 encount)
{
  u32 calc;
  
  if(GPowerSys.occur_power[GPOWER_TYPE_ENCOUNT_UP] != GPOWER_ID_NULL){
    calc = (GPowerSys.powerdata_data[GPOWER_TYPE_ENCOUNT_UP] * encount) / 100;
    if(calc > 100){
      calc = 100;
    }
    return calc;
  }
  if(GPowerSys.occur_power[GPOWER_TYPE_ENCOUNT_DOWN] != GPOWER_ID_NULL){
    calc = (GPowerSys.powerdata_data[GPOWER_TYPE_ENCOUNT_DOWN] * encount) / 100;
    if(calc > 100){
      calc = 100;
    }
    return calc;
  }
  return encount;
}

//==================================================================
/**
 * Gパワー計算：タマゴ孵化
 *
 * @param   add_count		加算する値(下位8ビット小数)
 *
 * @retval  u32		Gパワー影響後の加算値(下位8ビット小数)
 */
//==================================================================
u32 GPOWER_Calc_Hatch(u32 add_count)
{
  if(GPowerSys.occur_power[GPOWER_TYPE_HATCH_UP] != GPOWER_ID_NULL){
    return (GPowerSys.powerdata_data[GPOWER_TYPE_HATCH_UP] * add_count) / 100;
  }
  return add_count;
}

//==================================================================
/**
 * Gパワー計算：なつき度への加算値
 *
 * @param   natsuki		道具などの補正が入った後のなつき度加算値
 *
 * @retval  s32		Gパワー影響後のなつき度加算値
 */
//==================================================================
s32 GPOWER_Calc_Natsuki(s32 natsuki)
{
  if(GPowerSys.occur_power[GPOWER_TYPE_NATSUKI_UP] != GPOWER_ID_NULL){
    return natsuki + GPowerSys.powerdata_data[GPOWER_TYPE_NATSUKI_UP];
  }
  return natsuki;
}

//==================================================================
/**
 * Gパワー計算：商品の値段
 *
 * @param   price		商品の値段
 *
 * @retval  u32		  Gパワー影響後の商品の値段
 */
//==================================================================
u32 GPOWER_Calc_Sale(u32 price)
{
  if(GPowerSys.occur_power[GPOWER_TYPE_SALE] != GPOWER_ID_NULL){
    return (GPowerSys.powerdata_data[GPOWER_TYPE_SALE] * price) / 100;
  }
  return price;
}

//==================================================================
/**
 * Gパワー計算：大量発生
 *
 * @retval  s32		大量発生ID (発生していない場合はGPOWER_OCCUR_NONE)
 */
//==================================================================
s32 GPOWER_Calc_LargeAmountHappen(void)
{
  if(GPowerSys.occur_power[GPOWER_TYPE_HAPPEN] != GPOWER_ID_NULL){
    return GPowerSys.powerdata_data[GPOWER_TYPE_HAPPEN];
  }
  return GPOWER_OCCUR_NONE;
}

//==================================================================
/**
 * Gパワー計算：先頭にいるポケモンのHPを回復
 *
 * @param   party		手持ちパーティへのポインタ
 *
 * @retval  s32		回復したポケモンの手持ち位置(パワーが発動しなかった場合はGPOWER_OCCUR_NONE)
 *                (手持ち全て回復の時は0)
 */
//==================================================================
s32 GPOWER_Calc_HPRestore(POKEPARTY *party)
{
  POKEMON_PARAM *pp;
  int pos, hp_max, hp;
  
  if(GPowerSys.occur_power[GPOWER_TYPE_HP_RESTORE] != GPOWER_ID_NULL){
    if(GPowerSys.powerdata_data[GPOWER_TYPE_HP_RESTORE] == 999){
      STATUS_RCV_PokeParty_RecoverAll(party);
      pos = 0;
    }
    else{
      pos = PokeParty_GetMemberTopIdxBattleEnable( party );
      pp = PokeParty_GetMemberPointer(party, pos);
      {
        BOOL flag = PP_FastModeOn( pp );
        {
          hp_max = PP_Get( pp, ID_PARA_hpmax, NULL );
          hp = PP_Get( pp, ID_PARA_hp, NULL );
          hp += GPowerSys.powerdata_data[GPOWER_TYPE_HP_RESTORE];
          if(hp > hp_max){
            hp = hp_max;
          }
          PP_Put( pp , ID_PARA_hp , hp );
        }
        PP_FastModeOff( pp, flag );
      }
    }
    _OccurPowerClear(GPOWER_TYPE_HP_RESTORE);   //効果は一度なのでクリア
    return pos;
  }
  return GPOWER_OCCUR_NONE;
}

//==================================================================
/**
 * Gパワー計算：先頭にいるポケモンのPPを回復
 *
 * @param   party		手持ちパーティへのポインタ
 *
 * @retval  s32		回復したポケモンの手持ち位置(パワーが発動しなかった場合はGPOWER_OCCUR_NONE)
 */
//==================================================================
s32 GPOWER_Calc_PPRestore(POKEPARTY *party)
{
  int pos;
  POKEMON_PARAM *pp;
  
  if(GPowerSys.occur_power[GPOWER_TYPE_PP_RESTORE] != GPOWER_ID_NULL){
    pos = PokeParty_GetMemberTopIdxBattleEnable( party );
    pp = PokeParty_GetMemberPointer(party, pos);
    {
      BOOL flag = PP_FastModeOn( pp );
      {
        u32 waza, i;
        for(i=0; i<PTL_WAZA_MAX; ++i)
        {
          waza = PP_Get( pp, ID_PARA_waza1+i, NULL );
          if( waza != WAZANO_NULL )
          {
            u32 upsPP = PP_Get( pp, ID_PARA_pp_count1+i, NULL );
            u32 maxPP = WAZADATA_GetMaxPP( waza, upsPP );
            u32 nowPP = PP_Get( pp, ID_PARA_pp1+i, NULL );
            nowPP += GPowerSys.powerdata_data[GPOWER_TYPE_PP_RESTORE];
            if(nowPP > maxPP){
              nowPP = maxPP;
            }
            PP_Put( pp, ID_PARA_pp1+i, nowPP );
          }
        }
      }
      PP_FastModeOff( pp, flag );
    }
    _OccurPowerClear(GPOWER_TYPE_PP_RESTORE);   //効果は一度なのでクリア
    return pos;
  }
  return GPOWER_OCCUR_NONE;
}

//==================================================================
/**
 * Gパワー計算：獲得経験値
 *
 * @param   exp		他人の、外国の、学習装置分配、の補正後の獲得経験値
 *
 * @retval  u32		Gパワー影響後の獲得経験値
 */
//==================================================================
u32 GPOWER_Calc_Exp(u32 exp)
{
  if(GPowerSys.occur_power[GPOWER_TYPE_EXP_UP] != GPOWER_ID_NULL){
    return (GPowerSys.powerdata_data[GPOWER_TYPE_EXP_UP] * exp) / 100;
  }
  if(GPowerSys.occur_power[GPOWER_TYPE_EXP_DOWN] != GPOWER_ID_NULL){
    return (GPowerSys.powerdata_data[GPOWER_TYPE_EXP_DOWN] * exp) / 100;
  }
  return exp;
}

//==================================================================
/**
 * Gパワー計算：獲得お小遣い
 *
 * @param   money		所持道具効果などの補正後のお小遣い
 *
 * @retval  u32		Gパワー影響後のお小遣い
 */
//==================================================================
u32 GPOWER_Calc_Money(u32 money)
{
  if(GPowerSys.occur_power[GPOWER_TYPE_MONEY_UP] != GPOWER_ID_NULL){
    return (GPowerSys.powerdata_data[GPOWER_TYPE_MONEY_UP] * money) / 100;
  }
  return money;
}

//==================================================================
/**
 * Gパワー計算：捕獲率
 *
 * @param   hokakuritu		敵のHPとボールでの計算後の捕獲率
 *
 * @retval  fx32		Gパワー影響後の捕獲率
 */
//==================================================================
fx32 GPOWER_Calc_Capture(fx32 hokakuritu)
{
  fx64 calc;
  
  if(GPowerSys.occur_power[GPOWER_TYPE_CAPTURE_UP] != GPOWER_ID_NULL){
    calc = GPowerSys.powerdata_data[GPOWER_TYPE_CAPTURE_UP];
    calc = (calc * hokakuritu) / 100;
    return (fx32)calc;
  }
  return hokakuritu;
}

//==================================================================
/**
 * Gパワー計算：配布パワー
 */
//==================================================================
void GPOWER_Calc_Distribution(void)
{
  if(GPowerSys.occur_power[GPOWER_TYPE_DISTRIBUTION] != GPOWER_ID_NULL){
    GF_ASSERT(0); //仕様待ち
  }
}
