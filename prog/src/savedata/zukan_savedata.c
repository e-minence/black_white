//============================================================================================
/**
 * @file  zukan_savedata.c
 * @brief 図鑑セーブデータアクセス処理群ソース
 * @author  mori / tamada GAME FREAK inc.
 * @date  2009.10.26
 */
//============================================================================================

#include <gflib.h>

#include "pm_version.h"
#include "system/gfl_use.h"
#include "savedata/save_tbl.h"
#include "savedata/save_control.h"

//#include "system/gamedata.h"
#include "savedata/zukan_savedata.h"
#include "poke_tool/poke_personal.h"


//#include "include/application/zukanlist/zkn_world_text_data.h"

// 海外版図鑑テキスト処理が存在していたら１、無かったら０
#define FOREIGN_TEXT_FUNC   ( 0 )

// イッシュ図鑑テーブルが無い間は全部の図鑑情報から数えるようにする(1:無い 0:イッシュ図鑑ある)
#define ZUKAN_ISSHU_TABLE_NONE  ( 1 )


//============================================================================================
//============================================================================================
enum {
  POKEZUKAN_ARRAY_LEN = 21, ///<ポケモン図鑑フラグ配列のおおきさ16 * 32 = 512まで大丈夫

  POKEZUKAN_UNKNOWN_NUM = UNK_END,///<アンノーン数

  MAGIC_NUMBER = 0xbeefcafe,

  POKEZUKAN_DEOKISISU_MSK = 0xf,  // bitマスク  POKEZUKAN_ARRAY_LENの後ろ15bit分をデオキシスに使用してます。

  POKEZUKAN_TEXT_POSSIBLE  = 493, // 海外図鑑テキストが読める最大数（＝シンオウポケモンまで）

  POKEZUKAN_TEXTVER_UP_NUM = POKEZUKAN_TEXT_POSSIBLE + 1, //  見た目アップデートバッファ数  +1はパディング

};

//----------------------------------------------------------
/**
 * @brief 自分状態データ型定義
 */
//----------------------------------------------------------
struct ZUKAN_SAVEDATA {
  u32 zukan_magic;                      ///<マジックナンバー
  u32 get_flag[POKEZUKAN_ARRAY_LEN];    ///<捕まえたフラグ用ワーク
  u32 see_flag[POKEZUKAN_ARRAY_LEN];    ///<見つけたフラグ用ワーク
  u32 sex_flag[2][POKEZUKAN_ARRAY_LEN]; ///<オスメスフラグ用ワーク
  u32 PachiRandom;            ///<パッチール用個性乱数保持ワーク
  u8 SiiusiTurn;              ///<シーウシ用見つけた順保持ワーク    2bit必要  1bit*2種類
  u8 SiidorugoTurn;           ///<シードルゴ用見つけた順保持ワーク  2bit必要  1bit*2種類
  u8 MinomuttiTurn;           ///<ミノムッチ用見つけた順保持ワーク  6bit必要  2bit*3種類
  u8 MinomesuTurn;            ///<ミノメス用見つけた順保持ワーク    6bit必要  2bit*3種類
  u8 UnknownTurn[ POKEZUKAN_UNKNOWN_NUM ];///<アンノーンを見つけた番号
  u8 UnknownGetTurn[ POKEZUKAN_UNKNOWN_NUM ];///<アンノーンを捕獲した番号
  u8 TextVersionUp[ POKEZUKAN_TEXTVER_UP_NUM ];///<言語バージョンアップマスク
  u8 GraphicVersionUp;          ///<グラフィックバージョン用機能拡張フラグ
  u8 TextVersionUpMasterFlag;       ///<言語バージョンアップMasterフラグ
  u8 zukan_get;             ///<ずかん取得フラグ
  u8 zenkoku_flag;            ///<全国図鑑保持フラグ
  ///<ロトム用見つけた順番保持ワーク    18bit必要 3bit*6種類
  ///<シェイミ用見つけた順番保持ワーク  2bit必要  1bit*2種類
  ///<ギラティナ用見つけた順番保持ワーク  2bit必要  1bit*2種類
  u32 rotomu_turn;
  u8  syeimi_turn;
  u8  giratyina_turn;
  u8  pityuu_turn;  ///< ピチュー用見つけた順保持ワーク 2bit*3種類( ♂/♀/ギザミミ )
  u8  turn_pad;   ///< 余り
};

//============================================================================================
//============================================================================================
//----------------------------------------------------------
/**
 * @brief ずかんデータ用ワークのサイズ取得
 * @return  int   ZUKAN_SAVEDATAのサイズ
 */
//----------------------------------------------------------
int ZukanSave_GetWorkSize(void)
{
  return sizeof(ZUKAN_SAVEDATA);
}
//----------------------------------------------------------
/**
 * @brief ずかんデータ用ワークの生成処理
 * @param heapID    使用するヒープの指定
 * @return  ZUKAN_SAVEDATA  生成したZUKAN_SAVEDATAへのポインタ
 */
//----------------------------------------------------------
ZUKAN_SAVEDATA * ZukanSave_AllocWork( HEAPID heapID )
{
  ZUKAN_SAVEDATA * zw;
  zw = GFL_HEAP_AllocClearMemory( heapID, sizeof(ZUKAN_SAVEDATA));
  ZukanSave_Init(zw);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc( GMDATA_ID_ZUKAN );
#endif //CRC_LOADCHECK
  return zw;
}

//----------------------------------------------------------
/**
 * @brief ずかんデータ用ワークのコピー
 * @param from  コピー元ZUKAN_SAVEDATAへのポインタ
 * @param to    コピー先ZUKAN_SAVEDATAへのポインタ
 */
//----------------------------------------------------------
void ZukanSave_Copy(const ZUKAN_SAVEDATA * from, ZUKAN_SAVEDATA * to)
{
  MI_CpuCopy8(from, to, sizeof(ZUKAN_SAVEDATA));
}

//============================================================================================
//
//        内部関数
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief ずかんワークの整合性チェック
 */
//----------------------------------------------------------
static inline void zukan_incorrect(const ZUKAN_SAVEDATA * zw)
{
  GF_ASSERT(zw->zukan_magic == MAGIC_NUMBER);
}
//----------------------------------------------------------
/**
 * @brief ポケモンナンバーの範囲チェック
 */
//----------------------------------------------------------
static BOOL monsno_incorrect(u16 monsno)
{
  if (monsno == 0 || monsno > MONSNO_END) {
    GF_ASSERT_MSG(0, "ポケモンナンバー異常：%d\n", monsno);
    return TRUE;
  } else {
    return FALSE;
  }
}

//----------------------------------------------------------
/**
 * @brief 汎用ビットチェック
 */
//----------------------------------------------------------
static inline BOOL check_bit(const u8 * array, u16 flag_id)
{
  flag_id --;
  return 0 != (array[flag_id >> 3] & (1 << (flag_id & 7)));
}
//----------------------------------------------------------
/**
 * @brief 汎用ビットセット
 */
//----------------------------------------------------------
static inline void set_bit(u8 * array, u16 flag_id)
{
  flag_id --;
  array[flag_id >> 3] |= 1 << (flag_id & 7);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc(GMDATA_ID_ZUKAN);
#endif //CRC_LOADCHECK
}
//----------------------------------------------------------
/**
 * @brief 汎用1ビットデータセット
 * そのビットをクリーンにしてから設定します
 */
//----------------------------------------------------------
static inline void setnum_bit(u8 * array, u8 num, u16 flag_id)
{
  GF_ASSERT( num < 2 );
  flag_id --;

  array[flag_id >> 3] &= ~(1 << (flag_id & 7));
  array[flag_id >> 3] |= num << (flag_id & 7);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc(GMDATA_ID_ZUKAN);
#endif //CRC_LOADCHECK
}

#if 0
static inline void reset_bit(u8 * array, u16 flag_id)
{
  array[flag_id >> 3] &= (1 << (flag_id & 7)) ^ 0xff;
}
#endif
//----------------------------------------------------------
/**
 * @brief 汎用ビット数カウント処理
 */
//----------------------------------------------------------
static u16 count_bit(const u32 * array, u32 array_max)
{
  int count, idx;
  u32 item;
  count = 0;
  for (idx = 0; idx < array_max; idx ++) {
    for (item = array[idx]; item != 0; item >>= 1) {
      if (item & 1) {
        count ++;
      }
    }
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc(GMDATA_ID_ZUKAN);
#endif //CRC_LOADCHECK
  return count;
}

//----------------------------------------------------------
/**
 * @brief 汎用2ビットチェック
 * ＊1bit版は1〜xを指定するようになっていたが
 * 2bit版は0〜xを指定するようにする
 */
//----------------------------------------------------------
static inline u32 check_bit2(const u8 * array, u16 flag_id)
{
  return (array[flag_id >> 2] >> ((flag_id & 3)*2)) & 0x3;
}
//----------------------------------------------------------
/**
 * @brief 汎用2ビットデータセット
 * そのビットをクリーンにしてから設定します
 */
//----------------------------------------------------------
static inline void setnum_bit2(u8 * array, u8 num, u16 flag_id)
{
  GF_ASSERT( num < 4 );

  array[flag_id >> 2] &= ~(0x3 << ((flag_id & 3)*2) );
  array[flag_id >> 2] |= num << ((flag_id & 3)*2);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc(GMDATA_ID_ZUKAN);
#endif //CRC_LOADCHECK
}


//----------------------------------------------------------
/**
 * @brief みたビット設定
 */
//----------------------------------------------------------
static inline void set_see_bit(ZUKAN_SAVEDATA * zw, u16 flag_id)
{
  set_bit((u8*)zw->see_flag, flag_id);
}
//----------------------------------------------------------
/**
 * @brief つかまえたビット設定
 */
//----------------------------------------------------------
static inline void set_get_bit(ZUKAN_SAVEDATA * zw, u16 flag_id)
{
  set_bit((u8*)zw->get_flag, flag_id);
}
//----------------------------------------------------------
/**
 * @brief 性別ビット設定  実際に設定
 * 最初の性別設定で2要素目の配列にも同じ値を設定する
 * これにより2要素目が同じ値の時は違う性別のポケモンをみていない
 * ということになる
 */
//----------------------------------------------------------
static void set_sex_bit_first_second(ZUKAN_SAVEDATA * zw, u8 num, u8 no, u16 flag_id)
{
  if( no == ZUKANSAVE_SEX_FIRST ){
    setnum_bit((u8*)zw->sex_flag[ZUKANSAVE_SEX_SECOND], num, flag_id);
  }
  setnum_bit((u8*)zw->sex_flag[no], num, flag_id);
}
//----------------------------------------------------------
/**
 * @brief 性別ビット設定
 */
//----------------------------------------------------------
static void set_sex_bit(ZUKAN_SAVEDATA * zw, u8 num, u8 no, u16 monsno)
{
  GF_ASSERT_MSG( num <= 2, "num = %d", num ); // 男　女　不明以外がきました

  if( num == PTL_SEX_UNKNOWN )   // UNKNOWは男にする
  {
    num = PTL_SEX_MALE;
  }
  set_sex_bit_first_second(zw, num, no, monsno);
}
//----------------------------------------------------------
/**
 * @brief みたビットONOFFチェック
 */
//----------------------------------------------------------
static inline BOOL check_see_bit(const ZUKAN_SAVEDATA * zw, u16 flag_id)
{
  return check_bit((const u8*)zw->see_flag, flag_id);
}
//----------------------------------------------------------
/**
 * @brief つかまえたビットONOFFチェック
 */
//----------------------------------------------------------
static inline BOOL check_get_bit(const ZUKAN_SAVEDATA * zw, u16 flag_id)
{
  return check_bit((const u8*)zw->get_flag, flag_id);
}
//----------------------------------------------------------
/**
 * @brief 性別ビットONOFFチェック
 */
//----------------------------------------------------------
static inline u8 check_sex_bit(const ZUKAN_SAVEDATA * zw, u16 flag_id, u8 first_second)
{
  return check_bit((const u8*)zw->sex_flag[first_second], flag_id);
}

//----------------------------------------------------------
/**
 * @brief 固体乱数を設定
 */
//----------------------------------------------------------
static inline void SetZukanRandom(ZUKAN_SAVEDATA * zw, u16 monsno, u32 rand)
{
  if (monsno == MONSNO_PATTIIRU) {
    zw->PachiRandom = rand;
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc(GMDATA_ID_ZUKAN);
#endif //CRC_LOADCHECK
}

//----------------------------------------------------------
/**
 * @brief 複数フォルムを持つポケモンの見た順保持ワークポインタを返す(アンノーン以外)
 */
//----------------------------------------------------------
static const u8* get_turn_arry(const ZUKAN_SAVEDATA* zw,u32 monsno)
{
  switch( monsno ){
  case MONSNO_KARANAKUSI:
    return &zw->SiiusiTurn;
  case MONSNO_TORITODON:
    return &zw->SiidorugoTurn;
  case MONSNO_SHEIMI:
    return &zw->syeimi_turn;
  case MONSNO_GIRATHINA:
    return &zw->giratyina_turn;
  case MONSNO_MINOMUTTI:
    return &zw->MinomuttiTurn;
  case MONSNO_MINOMADAMU:
    return &zw->MinomesuTurn;
  case MONSNO_PITYUU:
    return &zw->pityuu_turn;
  }
  GF_ASSERT(0);
  return NULL;
}

//----------------------------------------------------------
/**
 * @brief アンノーンの現在設定数を取得
 */
//----------------------------------------------------------
static int get_zukan_unknown_turn_num( const ZUKAN_SAVEDATA * zw ,BOOL get_f)
{
  int i,num = 0;
  u8* data;

  if(get_f){
    data = (u8*)zw->UnknownGetTurn;
  }else{
    data = (u8*)zw->UnknownTurn;
  }
  for( i=0; i<POKEZUKAN_UNKNOWN_NUM; i++ ){
    if( data[i] == 0xff ){
      break;
    }
    num++;
  }
  return num;
}
//----------------------------------------------------------
/**
 * @brief アンノーンフォーム設定済みかチェック
 */
//----------------------------------------------------------
static BOOL check_zukan_unknown_turn_set( const ZUKAN_SAVEDATA * zw, u8 form ,BOOL get_f)
{
  int i;
  u8* data;

  if(get_f){
    data = (u8*)zw->UnknownGetTurn;
  }else{
    data = (u8*)zw->UnknownTurn;
  }
  for( i=0; i<POKEZUKAN_UNKNOWN_NUM; i++ ){
    if( data[i] == form ){
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------
/**
 * @brief アンノーン見つけた(捕まえた)順データの最後に形状IDを設定
 */
//----------------------------------------------------------
static void SetZukanUnknownTurn( ZUKAN_SAVEDATA * zw, int form, BOOL get_f)
{
  int set_idx;
  u8* data;

  if(get_f){
    data = zw->UnknownGetTurn;
  }else{
    data = zw->UnknownTurn;
  }

  // 設定済みかチェック
  if( check_zukan_unknown_turn_set( zw, form, get_f ) ){
    return;
  }

  set_idx = get_zukan_unknown_turn_num( zw, get_f );
  if( set_idx < POKEZUKAN_UNKNOWN_NUM ){
    data[ set_idx ] = form;
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc(GMDATA_ID_ZUKAN);
#endif //CRC_LOADCHECK
}

//----------------------------------------------------------
/**
 * @brief シーウシ　シードルゴ シェイミ　ギラティナの現在設定数を取得
 */
//----------------------------------------------------------
static int get_twoform_poke_turn_num( const ZUKAN_SAVEDATA * zw, u32 monsno )
{
  u32 first_form;
  u32 second_form;
  const u8* arry;
  GF_ASSERT( (monsno == MONSNO_KARANAKUSI) || (monsno == MONSNO_TORITODON) || (monsno == MONSNO_SHEIMI) || (monsno == MONSNO_GIRATHINA) );

  // 見てもいないときは0
  if( ZukanSave_GetPokeSeeFlag( zw, monsno ) == FALSE ){
    return 0;
  }
#if 0
  switch( monsno ){
  case MONSNO_KARANAKUSI:
    arry = &zw->SiiusiTurn;
    break;

  case MONSNO_TORITODON:
    arry = &zw->SiidorugoTurn;
    break;

  case MONSNO_SHEIMI:
    arry = &zw->syeimi_turn;
    break;

  case MONSNO_GIRATHINA:
    arry = &zw->giratyina_turn;
    break;
  }
#else
  arry = get_turn_arry(zw,monsno);
#endif
  first_form = check_bit( arry, 1 );
  second_form = check_bit( arry, 2 );

  if( first_form == second_form ){  // 最初と２個めが一緒なら１回だけ
    return 1;
  }
  return 2;
}
//----------------------------------------------------------
/**
 * @brief ミノムッチ　ミノメス　シェイミ　ギラティナ　フォーム設定済みかチェック
 * @param form  0 or 1
 * *注意 0or1限定で作成しています。
 */
//----------------------------------------------------------
static BOOL check_twoform_poke_turn_set( const ZUKAN_SAVEDATA * zw, u32 monsno, u8 form )
{
  u32 get_form;
  u32 i;
  u32 roop_num;
  const u8* arry;
  GF_ASSERT( (monsno == MONSNO_KARANAKUSI) || (monsno == MONSNO_TORITODON) || (monsno == MONSNO_SHEIMI) || (monsno == MONSNO_GIRATHINA) );

  // 見てもいないときは設定されているわけがない
  if( ZukanSave_GetPokeSeeFlag( zw, monsno ) == FALSE ){
    return FALSE;
  }

#if 0
  switch( monsno ){
  case MONSNO_KARANAKUSI:
    arry = &zw->SiiusiTurn;
    break;

  case MONSNO_TORITODON:
    arry = &zw->SiidorugoTurn;
    break;

  case MONSNO_SHEIMI:
    arry = &zw->syeimi_turn;
    break;

  case MONSNO_GIRATHINA:
    arry = &zw->giratyina_turn;
    break;
  }
#else
  arry = get_turn_arry(zw,monsno);
#endif
  // 設定数分チェック
  // １回しか見ていないときは２つ目のビットは、
  // ダミーデータなので、ちぇっくしない
  roop_num = get_twoform_poke_turn_num( zw, monsno );
  for( i=0; i<roop_num; i++ ){
    get_form = check_bit( arry, i+1 );
    if( get_form == form ){
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------
/**
 * @brief シーウシ　シードラゴ　シェイミ　ギラティナ見つけた順データの最後に形状IDを設定
 * *注意 0or1限定で作成しています。
 */
//----------------------------------------------------------
static void SetZukanTwoFormPokeTurn( ZUKAN_SAVEDATA * zw, u32 monsno, int form)
{
  int set_idx;
  u8* arry;
  GF_ASSERT( (monsno == MONSNO_KARANAKUSI) || (monsno == MONSNO_TORITODON) || (monsno == MONSNO_SHEIMI) || (monsno == MONSNO_GIRATHINA) );

  // 設定済みかチェック
  if( check_twoform_poke_turn_set( zw, monsno, form ) ){
    return;
  }
#if 0
  switch( monsno ){
  case MONSNO_KARANAKUSI:
    arry = &zw->SiiusiTurn;
    break;

  case MONSNO_TORITODON:
    arry = &zw->SiidorugoTurn;
    break;

  case MONSNO_SHEIMI:
    arry = &zw->syeimi_turn;
    break;

  case MONSNO_GIRATHINA:
    arry = &zw->giratyina_turn;
    break;
  }
#else
  arry = (u8*)get_turn_arry(zw,monsno);
#endif
  set_idx = get_twoform_poke_turn_num( zw, monsno );
  if( set_idx < 2 ){  // 0引きなら最初に設定　1なら2つめに設定 2なら設定しない
    setnum_bit( arry, form, set_idx+1 );
    if( set_idx == 0 ){
      setnum_bit( arry, form, set_idx+2 );  // 次も同じ絵で保存する
    }
  }
}

//----------------------------------------------------------
/**
 * @brief ミノムッチ、ミノメス、ピチューの現在設定数を取得
 * ＊形状が0〜2限定
 */
//----------------------------------------------------------
static int get_threeform_poke_turn_num( const ZUKAN_SAVEDATA * zw, u32 monsno )
{
  u32 get_form;
  int i;
  const u8* arry;

  GF_ASSERT( (monsno == MONSNO_MINOMUTTI) || (monsno == MONSNO_MINOMADAMU) || (monsno == MONSNO_PITYUU) );
  // 見てもいないときは0
  if( ZukanSave_GetPokeSeeFlag( zw, monsno ) == FALSE ){
    return 0;
  }

#if 0
  switch( monsno ){
  case MONSNO_MINOMUTTI:
    arry = &zw->MinomuttiTurn;
    break;
  case MONSNO_MINOMADAMU:
    arry = &zw->MinomesuTurn;
    break;
  case MONSNO_PITYUU:
    arry = &zw->pityuu_turn;
    break;
  }
#else
  arry = get_turn_arry(zw,monsno);
#endif
  for( i=0; i<3; i++ ){
    get_form = check_bit2( arry, i );
    if( get_form == 3 ){
      break;
    }
  }
  return i;
}
//----------------------------------------------------------
/**
 * @brief ミノムッチ、ミノメス、ピチューのフォルム設定済みかチェック
 * @param form  0 or 1 or 2
 * ＊形状が0〜2限定
 */
//----------------------------------------------------------
static BOOL check_threeform_poke_turn_set( const ZUKAN_SAVEDATA * zw, u32 monsno, u8 form )
{
  u32 get_form;
  int i;
  const u8* arry;

  GF_ASSERT( (monsno == MONSNO_MINOMUTTI) || (monsno == MONSNO_MINOMADAMU) || (monsno == MONSNO_PITYUU) );
  // 見てもいないときはFALSE
  if( ZukanSave_GetPokeSeeFlag( zw, monsno ) == FALSE ){
    return FALSE;
  }

#if 0
  switch( monsno ){
  case MONSNO_MINOMUTTI:
    arry = &zw->MinomuttiTurn;
    break;
  case MONSNO_MINOMADAMU:
    arry = &zw->MinomesuTurn;
    break;
  case MONSNO_PITYUU:
    arry = &zw->pityuu_turn;
    break;
  }
#else
  arry = get_turn_arry(zw,monsno);
#endif

  for( i=0; i<3; i++ ){
    get_form = check_bit2( arry, i );
    if( get_form == form ){
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------
/**
 * @brief ミノムッチ、ミノメス、ピチューの見つけた順データの最後に形状IDを設定
 * ＊形状が0〜2限定
 */
//----------------------------------------------------------
static void SetZukanThreeFormPokeTurn( ZUKAN_SAVEDATA * zw, u32 monsno, int form)
{
  int set_idx;
  u8* arry;
  GF_ASSERT( (monsno == MONSNO_MINOMUTTI) || (monsno == MONSNO_MINOMADAMU) || (monsno == MONSNO_PITYUU) );

  // 設定済みかチェック
  if( check_threeform_poke_turn_set( zw, monsno, form ) ){
    return;
  }

#if 0
  switch( monsno ){
  case MONSNO_MINOMUTTI:
    arry = &zw->MinomuttiTurn;
    break;
  case MONSNO_MINOMADAMU:
    arry = &zw->MinomesuTurn;
    break;
  case MONSNO_PITYUU:
    arry = &zw->pityuu_turn;
    break;
  }
#else
  arry = (u8*)get_turn_arry(zw,monsno);
#endif
  set_idx = get_threeform_poke_turn_num( zw, monsno );
  if( set_idx < 3 ){
    setnum_bit2( arry, form, set_idx );
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  デオキシスフォルム設定
 *
 *  @param  p_arry  格納領域
 *  @param  no  デオキシスフォルム
 *  @param  num 見つけた順番設定
 */
//-----------------------------------------------------------------------------
static void setDeokisisuFormNoCore( u32* p_arry, u8 no, u8 num )
{
  u32 shift = (24 + (num * 4));
  u32 msk = ~(POKEZUKAN_DEOKISISU_MSK << shift);
  p_arry[ POKEZUKAN_ARRAY_LEN - 1 ] &= msk;
  p_arry[ POKEZUKAN_ARRAY_LEN - 1 ] |= (no << shift);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc(GMDATA_ID_ZUKAN);
#endif //CRC_LOADCHECK
}

//----------------------------------------------------------------------------
/**
 *  @brief  デオキシスフォルム設定
 *
 *  @param  zw  図鑑ワーク
 *  @param  no  デオキシスフォルム
 *  @param  num 見つけた順番設定
 */
//-----------------------------------------------------------------------------
static void setDEOKISISUFormNo( ZUKAN_SAVEDATA * zw, u8 no, u8 num )
{
  GF_ASSERT( num < ZUKANSAVE_DEOKISISU_FORM_NUM );
  GF_ASSERT( no <= POKEZUKAN_DEOKISISU_INIT );
  // デオキシスナンバーを設定
  // 捕まえたフラグの後ろ15bit空いているので
  // 後ろ4bitずつにデータを登録しておく
  if( num < ZUKANSAVE_DEOKISISU_FORM_SAVEDATA_CUTNUM ){
    setDeokisisuFormNoCore( zw->get_flag, no, num );
  }else{
    setDeokisisuFormNoCore( zw->see_flag, no, num-ZUKANSAVE_DEOKISISU_FORM_SAVEDATA_CUTNUM );
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  デオキシスフォルム取得
 *
 *  @param  p_arry  格納領域
 *  @param  num 見つけた順番設定
 *
 *  @retval フォルムナンバー
 */
//-----------------------------------------------------------------------------
static inline u32 getDeokisisuFormNoCore( const u32* p_arry, u8 num )
{
  u32 shift = (24 + (num * 4));
  u32 ret = (p_arry[ POKEZUKAN_ARRAY_LEN - 1 ] >> shift) & POKEZUKAN_DEOKISISU_MSK;
  return ret;
}
//----------------------------------------------------------------------------
/**
 *  @brief  デオキシスフォルム取得
 *
 *  @param  zw  図鑑ワーク
 *  @retval デオキシスフォルム
 */
//-----------------------------------------------------------------------------
static u32 getDEOKISISUFormNo( const ZUKAN_SAVEDATA * zw, u8 num )
{
  u32 form;
  // デオキシスナンバーを設定
  // 捕まえたフラグの後ろ15bit空いているので
  // 後ろ4bitにデータを登録しておく
  if( num < ZUKANSAVE_DEOKISISU_FORM_SAVEDATA_CUTNUM ){
    form = getDeokisisuFormNoCore( zw->get_flag, num );
  }else{
    form = getDeokisisuFormNoCore( zw->see_flag, num - ZUKANSAVE_DEOKISISU_FORM_SAVEDATA_CUTNUM );
  }
  return form;
}

//----------------------------------------------------------------------------
/**
 *  @brief  今までに見たデオキシスの数を返す
 *
 *  @param  zw  図鑑ワーク
 *
 *  @return 今までに見たデオキシスの数を返す
 */
//-----------------------------------------------------------------------------
static u32 nowGetDeokisisuFormNum( const ZUKAN_SAVEDATA * zw )
{
  int i;

  for( i=0; i<ZUKANSAVE_DEOKISISU_FORM_NUM; i++ ){
    if( getDEOKISISUFormNo( zw, i ) == POKEZUKAN_DEOKISISU_INIT  ){
      break;
    }
  }
  return i;
}

//----------------------------------------------------------------------------
/**
 *  @brief  以前にこの形状のデオキシスを見ているかチェック
 *
 *  @param  zw  図鑑ワーク
 *  @param  formno 形状
 *
 *  @retval TRUE  見た
 *  @retval FALSE 見ていない
 */
//-----------------------------------------------------------------------------
static BOOL checkSeeDeokisisuFormNo( const ZUKAN_SAVEDATA * zw, u32 formno )
{
  int i;
  for( i=0; i<ZUKANSAVE_DEOKISISU_FORM_NUM; i++ ){
    if( getDEOKISISUFormNo( zw, i ) == formno  ){
      return TRUE;
    }
  }
  return FALSE;
}



//----------------------------------------------------------------------------
/**
 *  @brief  フォームナンバー設定　パック
 */
//-----------------------------------------------------------------------------
static void SetDeokisisuFormNo( ZUKAN_SAVEDATA * zw, u16 monsno, const POKEMON_PARAM* pp  )
{
  u8 form = PP_Get( pp, ID_PARA_form_no, NULL );
  u32 setno;

  if( monsno == MONSNO_DEOKISISU ){
    // まだ見ていないかチェック
    if( checkSeeDeokisisuFormNo( zw, form ) == FALSE ){
      setno = nowGetDeokisisuFormNum( zw ); // 設定位置
      setDEOKISISUFormNo( zw, form, setno );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  デオキシスフォルムナンバーバッファ初期化
 *
 *  @param  zw  図鑑ワーク
 */
//-----------------------------------------------------------------------------
static void InitDeokisisuFormNo( ZUKAN_SAVEDATA * zw )
{
  int i;
  for( i=0; i<ZUKANSAVE_DEOKISISU_FORM_NUM; i++ ){
    setDEOKISISUFormNo( zw, 0xf, i ); // 0xfで初期化する
  }
}


// u32データ3bitごと参照
#define ZUKAN_3BIT_MAX  (0x7)
static inline u32 get_3bit_data( u32 data, u32 idx )
{
  return ( data >> (idx*3) ) & ZUKAN_3BIT_MAX;
}

static inline void set_3bit_data( u32* p_data, u32 idx, u32 data )
{
  GF_ASSERT( data < ZUKAN_3BIT_MAX );
  (*p_data) &= ~(ZUKAN_3BIT_MAX << (idx*3));
  (*p_data) |= (data << (idx*3));
}

//----------------------------------------------------------
/**
 * @brief ロトムの現在設定数を取得
 */
//----------------------------------------------------------
static int get_rotom_turn_num( const ZUKAN_SAVEDATA * zw, u32 monsno )
{
  u32 form;
  int i, num;

  GF_ASSERT( monsno == MONSNO_ROTOMU );

  // 見てもいないときは0
  if( ZukanSave_GetPokeSeeFlag( zw, monsno ) == FALSE ){
    return 0;
  }

  // 前と後ろが同じ数字になるまで数を数える
  num = 0;
  for( i=0; i<ZUKANSAVE_ROTOM_FORM_NUM; i++ ){
    form = get_3bit_data( zw->rotomu_turn, i );
    if( form != ZUKAN_3BIT_MAX ){
      num++;
    }else{
      break;
    }
  }

  return num;
}
//----------------------------------------------------------
/**
 * @brief ロトム設定済みかチェック
 * @param form  0〜5
 */
//----------------------------------------------------------
static BOOL check_rotom_turn_set( const ZUKAN_SAVEDATA * zw, u32 monsno, u8 form )
{
  int i;
  u32 roop_num;
  u32 get_form;

  GF_ASSERT( (monsno == MONSNO_ROTOMU) );

  // 見てもいないときは設定されているわけがない
  if( ZukanSave_GetPokeSeeFlag( zw, monsno ) == FALSE ){
    return FALSE;
  }

  // 設定数分チェック
  // １回しか見ていないときは２つ目のビットは、
  // ダミーデータなので、ちぇっくしない
  roop_num = get_rotom_turn_num( zw, monsno );
  for( i=0; i<roop_num; i++ ){
    get_form = get_3bit_data( zw->rotomu_turn, i );
    if( get_form == form ){
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------
/**
 * @brief ロトム見つけた順データの最後に形状IDを設定
 */
//----------------------------------------------------------
static void SetZukanRotomTurn( ZUKAN_SAVEDATA * zw, u32 monsno, int form)
{
  int set_idx;
  GF_ASSERT( (monsno == MONSNO_ROTOMU) );

  // 設定済みかチェック
  if( check_rotom_turn_set( zw, monsno, form ) ){
    return;
  }

  set_idx = get_rotom_turn_num( zw, monsno );
  if( set_idx < ZUKANSAVE_ROTOM_FORM_NUM ){
    set_3bit_data( &zw->rotomu_turn, set_idx, form );
  }
}




//----------------------------------------------------------
/**
 * @brief いろんなポケモンの見つけた順データの保存
 */
//----------------------------------------------------------
static void SetZukanSpecialTurn(ZUKAN_SAVEDATA * zw, u16 monsno, const POKEMON_PARAM* pp)
{
  int form;

  form = PP_Get( pp, ID_PARA_form_no, NULL );

  switch( monsno ){
  //  アンノーン
  case MONSNO_ANNOON:
    SetZukanUnknownTurn( zw, form, FALSE );
    break;
  // ミノムッチ・ミノメス
  case MONSNO_MINOMUTTI:
  case MONSNO_MINOMADAMU:
    SetZukanThreeFormPokeTurn( zw, monsno, form );
    break;
  // ピチュー
  case MONSNO_PITYUU:
    // ♂、♀、ギザミミの順にする
    if( form == 0 ){
      if( PP_GetSex(pp) == PTL_SEX_MALE ){
        form = 0;
      }else{
        form = 1;
      }
    }else{
      form = 2;
    }
    SetZukanThreeFormPokeTurn( zw, monsno, form );
    break;
  // ウミウシ・シードルゴ
  case MONSNO_KARANAKUSI:
  case MONSNO_TORITODON:
    SetZukanTwoFormPokeTurn( zw, monsno, form );
    break;
  // デオキシス
  case MONSNO_DEOKISISU:
    SetDeokisisuFormNo( zw, monsno, pp ); // デオキシスのフォルムを保存
    break;
  // シェイミ・ギラティナ
  case MONSNO_SHEIMI:
  case MONSNO_GIRATHINA:
    SetZukanTwoFormPokeTurn( zw, monsno, form );
    break;
  // ロトム
  case MONSNO_ROTOMU:
    SetZukanRotomTurn( zw, monsno, form );
    break;

  default:
    break;
  }
}

//----------------------------------------------------------
/**
 * @brief テキストバージョン配列インデックスをモンスターナンバーから取得
 */
//----------------------------------------------------------
static inline int get_zukan_textversion_idx(u16 monsno)
{

#if FOREIGN_TEXT_FUNC
  return ZKN_WT_GetMonsNo_TEXTVERPokeNum( monsno );
#else
  return 0;
#endif

}
//----------------------------------------------------------
/**
 * @brief テキストバージョンフラグ設定
 */
//----------------------------------------------------------
static void SetZukanTextVersionUp(ZUKAN_SAVEDATA * zw, u16 monsno, u32 lang)
{
  int idx;
  int zkn_world_text_code=0;


//  idx = get_zukan_textversion_idx( monsno );
  idx = monsno;

#if FOREIGN_TEXT_FUNC
  zkn_world_text_code = ZKN_WT_GetLANG_Code_ZKN_WORLD_TEXT_Code( lang );
#endif

  // そのポケモンに外国語データはない
//  if(idx == ZKN_WORLD_TEXT_POKE_NUM){ return; }

#if FOREIGN_TEXT_FUNC
  // その国の外国語データはない
  if(zkn_world_text_code == ZKN_WORLD_TEXT_NUM){
    return;
  }
#endif

  // シンオウ以上のポケモンの海外テキストは無い
  if(monsno > POKEZUKAN_TEXT_POSSIBLE)
  {
    return;
  }

  zw->TextVersionUp[ idx ] |= 1 << zkn_world_text_code;
}
//----------------------------------------------------------
/**
 * @brief 通常のポケモン性別取得
 */
//----------------------------------------------------------
static u32 get_normal_poke_sex( const ZUKAN_SAVEDATA * zw, u16 monsno, int first_second, HEAPID heapId )
{
  u32 sex_first, sex_second;
  u32 ret_sex,sex;
  POKEMON_PERSONAL_DATA *ppd;

  // ポケモンが取りえる性別を取得
  ppd = POKE_PERSONAL_OpenHandle( monsno, 0, heapId );
  sex = PokePersonal_SexGet( ppd, monsno, 0);
  POKE_PERSONAL_CloseHandle( ppd );

  // 絶対に不明なポケモンはfirstは不明をsecondはエラーを返す
  if( sex == PTL_SEX_UNKNOWN ){
    if( first_second == ZUKANSAVE_SEX_FIRST ){
      return PTL_SEX_UNKNOWN;
    }else{
      return ZUKANSAVE_GET_SEX_ERR;
    }
  }

  // 次に見た性別が設定されているのかチェックするために
  // どっちの場合でも最初に見た性別を取得
  sex_first = check_sex_bit( zw, monsno, ZUKANSAVE_SEX_FIRST );

  // 次に見た性別を取得するなら次に見た性別を取得
  if( first_second == ZUKANSAVE_SEX_SECOND ){
    sex_second = check_sex_bit( zw, monsno, ZUKANSAVE_SEX_SECOND );

    // 最初に見た性別と一緒==まだ見ていない
    if( sex_second == sex_first ){
      ret_sex = ZUKANSAVE_GET_SEX_ERR;
    }else{
      ret_sex = sex_second;
    }
  }else{
    ret_sex = sex_first;
  }

  return ret_sex;
}

//----------------------------------------------------------
/**
 * @brief アンノーン見つけた(捕まえた)順番取得
 */
//----------------------------------------------------------
static inline int get_unknown_form( const ZUKAN_SAVEDATA * zw, int idx ,BOOL get_f)
{
  if(get_f){
    return zw->UnknownGetTurn[ idx ];
  }
  return zw->UnknownTurn[ idx ];
}

//----------------------------------------------------------
/**
 * @brief シーウシ　シードラゴ　シェイミ　ギラティナ見つけた順番取得
 *注意 0or1限定で作成しています。
 */
//----------------------------------------------------------
static int get_twoform_poke_see_form( const ZUKAN_SAVEDATA * zw, u32 monsno, int idx )
{
  const u8* arry;
  GF_ASSERT( (monsno == MONSNO_KARANAKUSI) || (monsno == MONSNO_TORITODON) || (monsno == MONSNO_SHEIMI) || (monsno == MONSNO_GIRATHINA) );
  GF_ASSERT( idx < 2 );

#if 0
  switch( monsno ){
  case MONSNO_KARANAKUSI:
    arry = &zw->SiiusiTurn;
    break;

  case MONSNO_TORITODON:
    arry = &zw->SiidorugoTurn;
    break;

  case MONSNO_SHEIMI:
    arry = &zw->syeimi_turn;
    break;

  case MONSNO_GIRATHINA:
    arry = &zw->giratyina_turn;
    break;
  }
#else
  arry = get_turn_arry(zw,monsno);
#endif
  return check_bit( arry, idx+1 );
}

//----------------------------------------------------------
/**
 * @brief ロトム見つけた順番取得
 */
//----------------------------------------------------------
static int get_rotom_see_form( const ZUKAN_SAVEDATA * zw, u32 monsno, int idx )
{
  GF_ASSERT( (monsno == MONSNO_ROTOMU) );
  GF_ASSERT( idx < ZUKANSAVE_ROTOM_FORM_NUM );

  return get_3bit_data( zw->rotomu_turn, idx );
}

//----------------------------------------------------------
/**
 * @brief ミノムッチ、ミノメス、ピチューの見つけた順番取得
 *注意 0〜2限定で作成しています。
 */
//----------------------------------------------------------
static int get_threeform_poke_see_form( const ZUKAN_SAVEDATA * zw, u32 monsno, int idx )
{
  const u8* arry;
  GF_ASSERT( (monsno == MONSNO_MINOMUTTI) || (monsno == MONSNO_MINOMADAMU) || (monsno == MONSNO_PITYUU) );
  GF_ASSERT( idx < 3 );

#if 0
  switch( monsno ){
  case MONSNO_MINOMUTTI:
    arry = &zw->MinomuttiTurn;
    break;
  case MONSNO_MINOMADAMU:
    arry = &zw->MinomesuTurn;
    break;
  case MONSNO_PITYUU:
    arry = &zw->pityuu_turn;
    break;
  }
#else
  arry = get_turn_arry(zw,monsno);
#endif
  return check_bit2( arry, idx );
}


//----------------------------------------------------------------------------
/**
 *  @brief  ゼンコク図鑑完成に必要なポケモンかチェック
 */
//-----------------------------------------------------------------------------
static BOOL check_ZenkokuCompMonsno( u16 monsno )
{
  int i;
  BOOL cut_check;
  static const u16 cut_check_monsno[ ZUKANSAVE_ZENKOKU_COMP_NOPOKE_NUM ] = {
    MONSNO_MYUU,
    MONSNO_SEREBHI,
    MONSNO_ZIRAATI,
    MONSNO_DEOKISISU,
    MONSNO_FIONE,
    MONSNO_MANAFI,
    MONSNO_DAAKURAI,
    MONSNO_SHEIMI,
    MONSNO_ARUSEUSU,
  };

  // チェック除外ポケモンチェック
  cut_check = TRUE;
  for( i=0; i<ZUKANSAVE_ZENKOKU_COMP_NOPOKE_NUM; i++ ){
    if( cut_check_monsno[ i ] == monsno ){
      cut_check = FALSE;
    }
  }
  return cut_check;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ジョウト図鑑完成に必要なポケモンかチェック
 */
//-----------------------------------------------------------------------------
static BOOL check_JohtoCompMonsno( u16 monsno )
{
  int i;
  BOOL cut_check;
  static const u16 cut_check_monsno[ZUKANSAVE_JOHTO_COMP_NOPOKE_NUM] = {
    MONSNO_MYUU,
    MONSNO_SEREBHI,
  };

  // チェック除外ポケモンチェック
  cut_check = TRUE;
  for( i=0; i<ZUKANSAVE_JOHTO_COMP_NOPOKE_NUM; i++ ){
    if( cut_check_monsno[ i ] == monsno ){
      cut_check = FALSE;
    }
  }
  return cut_check;
}

//============================================================================================
//
//      ずかん情報操作用の外部公開関数
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief   ずかんワークの初期化
 * @param zw    ずかんワークへのポインタ
 */
//----------------------------------------------------------
void ZukanSave_Init(ZUKAN_SAVEDATA * zw)
{
  MI_CpuClear( zw, sizeof(ZUKAN_SAVEDATA));
  zw->zukan_magic    = MAGIC_NUMBER;
  zw->zenkoku_flag   = FALSE;

  // 捕まえた順番フラグにダミーデータ設定
  MI_CpuFillFast( zw->UnknownTurn,    0xff, sizeof(u8)*POKEZUKAN_UNKNOWN_NUM );
  MI_CpuFillFast( zw->UnknownGetTurn, 0xff, sizeof(u8)*POKEZUKAN_UNKNOWN_NUM );
  zw->SiiusiTurn     = 0xff;
  zw->SiidorugoTurn  = 0xff;
  zw->MinomuttiTurn  = 0xff;
  zw->MinomesuTurn   = 0xff;

  zw->rotomu_turn    = 0xffffffff;
  zw->syeimi_turn    = 0xff;
  zw->giratyina_turn = 0xff;
  zw->pityuu_turn    = 0xff;

  // デオキシスフォルムタイプ保存領域の初期化
  InitDeokisisuFormNo( zw );
}

//----------------------------------------------------------
/**
 * @brief ポケモンをつかまえた数の取得
 * @param zw    ずかんワークへのポインタ
 * @return  u16   捕まえた数
 */
//----------------------------------------------------------
u16 ZukanSave_GetPokeGetCount(const ZUKAN_SAVEDATA * zw)
{
  int i;
  int count;

  zukan_incorrect(zw);
  count= 0;
  for( i=1; i<=MONSNO_END; i++ ){
    if( ZukanSave_GetPokeGetFlag( zw, i ) == TRUE ){
      count ++;
    }
  }
  return count;

  // 高速ですが、デオキシスフラグをget_flagの後ろに入れたので、使用できません
//  return count_bit(zw->get_flag, POKEZUKAN_ARRAY_LEN);

}

//----------------------------------------------------------
/**
 * @brief ポケモンを見つけた数の取得
 * @param zw    ずかんワークへのポインタ
 * @return  u16   見つけた数
 */
//----------------------------------------------------------
u16 ZukanSave_GetPokeSeeCount(const ZUKAN_SAVEDATA * zw)
{
  int i;
  int count;

  zukan_incorrect(zw);
  count= 0;
  for( i=1; i<=MONSNO_END; i++ ){
    if( ZukanSave_GetPokeSeeFlag( zw, i ) == TRUE ){
      count ++;
    }
  }
  return count;

  // 高速ですが、デオキシスフラグをget_flagの後ろに入れたので、使用できません
//  return count_bit(zw->see_flag, POKEZUKAN_ARRAY_LEN);
}

//----------------------------------------------------------------------------
/**
 *  @brief  現在の図鑑ﾓｰﾄﾞのポケモンを捕まえた数取得
 *  @param  zw    図鑑ワークへのポインタ
 *  @return 捕まえた数
 */
//-----------------------------------------------------------------------------
u16 ZukanSave_GetZukanPokeGetCount(const ZUKAN_SAVEDATA * zw)
{
  if( ZukanSave_GetZenkokuZukanFlag( zw ) ){
    return ZukanSave_GetPokeGetCount( zw );
  }
  return ZukanSave_GetJohtoPokeGetCount( zw );
}

//----------------------------------------------------------------------------
/**
 *  @brief  現在の図鑑ﾓｰﾄﾞのポケモンを見た数取得
 *  @param  zw    図鑑ワークへのポインタ
 *  @return 見た数
 */
//-----------------------------------------------------------------------------
u16 ZukanSave_GetZukanPokeSeeCount(const ZUKAN_SAVEDATA * zw)
{
  if( ZukanSave_GetZenkokuZukanFlag( zw ) ){
    return ZukanSave_GetPokeSeeCount( zw );
  }
  return ZukanSave_GetJohtoPokeSeeCount( zw );
}

//----------------------------------------------------------
/**
 * 【ジョウト】
 * @brief ポケモンをつかまえた数の取得
 * @param zw    ずかんワークへのポインタ
 * @return  u16   捕まえた数
 */
//----------------------------------------------------------
u16 ZukanSave_GetJohtoPokeGetCount(const ZUKAN_SAVEDATA * zw)
{
  u16 * buf;
  u16 i;
  u16 num=0;

  zukan_incorrect(zw);


#if ZUKAN_ISSHU_TABLE_NONE
  for( i=1; i<=MONSNO_END; i++ ){
    if( ZukanSave_GetPokeGetFlag( zw, i ) == TRUE ){
      num++;
    }
  }
#else
  buf = PokeZukanNo2JohtoNoTblLoad();   // @TODO 図鑑NOテーブルがまだ存在しない
  num = 0;

  for( i=1; i<=MONSNO_END; i++ ){
    if( ZukanSave_GetPokeGetFlag( zw, i ) == TRUE ){
      // ジョウト図鑑にいるかチェック
      if( buf[i] != 0 ){
        num++;
      }
    }
  }

  GFL_HEAP_FreeMemory( buf );
#endif

  return num;

}

//----------------------------------------------------------
/**
 * 【ジョウト】
 * @brief ポケモンを見つけた数の取得
 * @param zw    ずかんワークへのポインタ
 * @return  u16   見つけた数
 */
//----------------------------------------------------------
u16 ZukanSave_GetJohtoPokeSeeCount(const ZUKAN_SAVEDATA * zw)
{
  u16 * buf;
  u16 i;
  u16 num=0;

  zukan_incorrect(zw);

#if ZUKAN_ISSHU_TABLE_NONE

  for( i=1; i<=MONSNO_END; i++ ){
    if( ZukanSave_GetPokeSeeFlag( zw, i ) == TRUE ){
       num++;
    }
  }
#else
  buf = PokeZukanNo2JohtoNoTblLoad();
  num = 0;

  for( i=1; i<=MONSNO_END; i++ ){
    if( ZukanSave_GetPokeSeeFlag( zw, i ) == TRUE ){
      // ジョウト図鑑にいるかチェック
      if( buf[i] != 0 ){
        num++;
      }
    }
  }

  GFL_HEAP_FreeMemory( buf );
#endif
  return num;
}

//----------------------------------------------------------------------------
/**
 *  @brief  全国図鑑が完成したかチェックする
 *
 *  @param  zw  図鑑ワーク
 *
 *  @retval TRUE  完成
 *  @retval FALSE 未完成
 */
//-----------------------------------------------------------------------------
BOOL ZukanSave_CheckZenkokuComp(const ZUKAN_SAVEDATA * zw)
{
  u16 num;

  // ゼンコク図鑑完成に必要なポケモンを何匹捕まえたか
  num = ZukanSave_GetZenkokuGetCompCount( zw );

  if( num >= ZUKANSAVE_ZENKOKU_COMP_NUM ){
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ジョウト図鑑が完成したかチェックする
 *
 *  @param  zw  図鑑ワーク
 *
 *  @retval TRUE  完成
 *  @retval FALSE 未完成
 */
//-----------------------------------------------------------------------------
BOOL ZukanSave_CheckJohtoComp(const ZUKAN_SAVEDATA * zw)
{
  u16 num;

  // ジョウト図鑑完成に必要なポケモンを何匹捕まえたか
  num = ZukanSave_GetJohtoGetCompCount( zw );

  if( num >= ZUKANSAVE_JOHTO_COMP_NUM ){
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ゼンコク図鑑　完成に必要なポケモンを捕まえた数
 *
 *  @param  zw    図鑑ワーク
 *  @return 完成に必要なポケモンを捕まえた数
 */
//-----------------------------------------------------------------------------
u16 ZukanSave_GetZenkokuGetCompCount(const ZUKAN_SAVEDATA * zw)
{
  int i;
  u16 num;
  // エルフィ　マナフィ　ダーク　エリウス　アウス
  // 以外を捕まえていたらOK
  num = 0;
  for( i=1; i<=ZUKANSAVE_ZENKOKU_MONSMAX; i++ ){
    if( ZukanSave_GetPokeGetFlag( zw, i ) == TRUE ){
      if( check_ZenkokuCompMonsno( i ) == TRUE ){
        num ++;
      }
    }
  }
  return num;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ジョウト図鑑　完成に必要なポケモンを捕まえた数
 *
 *  @param  zw    図鑑ワーク
 *  @return 完成に必要なポケモンを見つけた数
 */
//-----------------------------------------------------------------------------
u16 ZukanSave_GetJohtoGetCompCount(const ZUKAN_SAVEDATA * zw)
{
  u16 * buf;
  u16 i;
  u16 num=0;


#if ZUKAN_ISSHU_TABLE_NONE

  for( i=1; i<=MONSNO_END; i++ ){
    if( ZukanSave_GetPokeGetFlag( zw, i ) == TRUE ){
      num++;
    }
  }

#else

  buf = PokeZukanNo2JohtoNoTblLoad();
  // ミュウとセレビィ
  // 以外を捕まえていたらOK
  for( i=1; i<=ZUKANSAVE_ZENKOKU_MONSMAX; i++ ){
    if( ZukanSave_GetPokeGetFlag( zw, i ) == TRUE ){
      if( buf[i] != 0 ){
        if( check_JohtoCompMonsno( i ) == TRUE ){
          num++;
        }
      }
    }
  }
  OS_Printf("ジョウトコンプリートチェック num = %d\n",num);
  GFL_HEAP_FreeMemory( buf );

#endif

  return num;
}

//----------------------------------------------------------
/**
 * @brief ポケモンを捕まえたかどうかのフラグ取得
 * @param zw    ずかんワークへのポインタ
 * @param monsno  ポケモンナンバー
 * @return  BOOL  フラグ結果
 */
//----------------------------------------------------------
BOOL ZukanSave_GetPokeGetFlag(const ZUKAN_SAVEDATA * zw, u16 monsno)
{
  zukan_incorrect(zw);
  if (monsno_incorrect(monsno)) {
    return FALSE;
  }
  if (check_get_bit(zw, monsno) && check_see_bit(zw, monsno)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

//----------------------------------------------------------
/**
 * @brief ポケモンを見つけたかどうかのフラグ取得
 * @param zw    ずかんワークへのポインタ
 * @param monsno  ポケモンナンバー
 * @return  BOOL  フラグ結果
 */
//----------------------------------------------------------
BOOL ZukanSave_GetPokeSeeFlag(const ZUKAN_SAVEDATA * zw, u16 monsno)
{
  zukan_incorrect(zw);
  if (monsno_incorrect(monsno)) {
    return FALSE;
  }
  return check_see_bit(zw, monsno);
}

//----------------------------------------------------------------------------
/**
 *  @brief  特殊ポケモンの個性乱数取得
 *
 *  @param  zw      図鑑ワークへのポインタ
 *  @param  random_poke 取得するポケモン個性乱数
 *
 *  @return 個性乱数
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokeRandomFlag(const ZUKAN_SAVEDATA * zw, u8 random_poke)
{
  u32 random;

  zukan_incorrect(zw);

  switch( random_poke ){
  case ZUKANSAVE_RANDOM_PACHI:    // パッチール個性乱数
    random = zw->PachiRandom;
    break;

  default:
    GF_ASSERT_MSG( 0, "random_poke = %d\n", random_poke );
    break;
  }

  return random;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ポケモンを表示するときの性別取得
 *
 *  @param  zw        ずかんワークへのポインタ
 *  @param  monsno      ポケモンナンバー
 *  @param  first_second  最初に見た性別か次に見た性別か
 *
 *  @retval PTL_SEX_MALE    オス
 *  @retval PTL_SEX_FEMALE    メス
 *  @retval PTL_SEX_UNKNOWN   不明
 *  @retval ZUKANSAVE_GET_SEX_ERR 見た性別がまだ無い
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokeSexFlag(const ZUKAN_SAVEDATA * zw, u16 monsno, int first_second, HEAPID heapId )
{
  zukan_incorrect(zw);
  if (monsno_incorrect(monsno)) {
    return ZUKANSAVE_GET_SEX_ERR;
  }
  // 見てないときはエラーコードを返す
  if( !check_see_bit(zw, monsno) ){
    return ZUKANSAVE_GET_SEX_ERR;
  }

  // ノーマルポケモン
  return get_normal_poke_sex( zw, monsno, first_second, heapId );
}

//----------------------------------------------------------------------------
/**
 *  @brief  アンノーン見つけた順設定
 *
 *  @param  zw    図鑑ワーク
 *  @param  count アンノーン順番  (0〜27)
 *  @param  get_f FALSE:見つけただけ,TRUE:捕獲済み
 *
 *  @retval アンノーンフォームナンバー
 *  @retval ZUKANSAVE_GET_SEX_ERR 見つけて(捕獲して)いない
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokeAnnoonForm(const ZUKAN_SAVEDATA * zw, int count, BOOL get_f)
{
  zukan_incorrect(zw);
  // その数のアンノーンフォルムを見ているかチェック
  if( get_zukan_unknown_turn_num( zw, get_f ) <= count ){
    return ZUKANSAVE_GET_SEX_ERR;
  }

  // フォルムナンバーを返す
  return get_unknown_form( zw, count , get_f );
}

//----------------------------------------------------------------------------
/**
 *  @brief  アンノーンを見た(捕まえた)数
 *
 *  @param  zw    図鑑ワーク
 *
 *  @return 見た数
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokeAnnoonNum(const ZUKAN_SAVEDATA * zw, BOOL get_f)
{
  zukan_incorrect(zw);
  return get_zukan_unknown_turn_num( zw, get_f );
}

//----------------------------------------------------------------------------
/**
 *  @brief  シーウシフォルム取得
 *
 *  @param  zw    図鑑ワーク
 *  @param  count 見つけた順番数
 *
 *  @retval その順番に見つけたフォルム
 *  @retval ZUKANSAVE_GET_SEX_ERR 見つけていない
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokeSiiusiForm(const ZUKAN_SAVEDATA * zw, int count)
{
  zukan_incorrect(zw);

  // その数のシーウシフォルムを見ているかチェック
  if( get_twoform_poke_turn_num( zw, MONSNO_KARANAKUSI ) <= count ){
    return ZUKANSAVE_GET_SEX_ERR;
  }
  return get_twoform_poke_see_form( zw, MONSNO_KARANAKUSI, count );
}

//----------------------------------------------------------------------------
/**
 *  @brief  シーウシのフォルムを見た数
 *
 *  @param  zw  図鑑ワーク
 *
 *  @return フォルムを見た数
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokeSiiusiSeeNum(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return get_twoform_poke_turn_num( zw, MONSNO_KARANAKUSI );
}

//----------------------------------------------------------------------------
/**
 *  @brief  シードルゴフォルムナンバー取得
 *
 *  @param  zw    図鑑ワーク
 *  @param  count 見つけた順番数
 *
 *  @retval その順番に見つけたフォルムナンバー
 *  @retval ZUKANSAVE_GET_SEX_ERR 見つけていない
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokeSiidorugoForm(const ZUKAN_SAVEDATA * zw, int count)
{
  zukan_incorrect(zw);

  // その数のシードルゴフォルムを見ているかチェック
  if( get_twoform_poke_turn_num( zw, MONSNO_TORITODON ) <= count ){
    return ZUKANSAVE_GET_SEX_ERR;
  }
  return get_twoform_poke_see_form( zw, MONSNO_TORITODON, count );
}

//----------------------------------------------------------------------------
/**
 *  @brief  シードルゴのフォルムを見た数
 *
 *  @param  zw  図鑑ワーク
 *
 *  @return フォルムを見た数
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokeSiidorugoSeeNum(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return get_twoform_poke_turn_num( zw, MONSNO_TORITODON );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ミノムッチフォルムナンバー取得
 *
 *  @param  zw    図鑑ワーク
 *  @param  count 見つけた順番数
 *
 *  @retval その順番に見つけたフォルムナンバー
 *  @retval ZUKANSAVE_GET_SEX_ERR 見つけていない
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokeMinomuttiForm(const ZUKAN_SAVEDATA * zw, int count)
{
  zukan_incorrect(zw);

  // その数のミノムッチフォルムを見ているかチェック
  if( get_threeform_poke_turn_num( zw, MONSNO_MINOMUTTI ) <= count ){
    return ZUKANSAVE_GET_SEX_ERR;
  }
  return get_threeform_poke_see_form( zw, MONSNO_MINOMUTTI, count );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ミノムッチのフォルムを見た数
 *
 *  @param  zw  図鑑ワーク
 *
 *  @return フォルムを見た数
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokeMinomuttiSeeNum(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return get_threeform_poke_turn_num( zw, MONSNO_MINOMUTTI );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ミノメス地形フォルムナンバー取得
 *
 *  @param  zw    図鑑ワーク
 *  @param  count 見つけた順番数
 *
 *  @retval その順番に見つけたフォルムナンバー
 *  @retval ZUKANSAVE_GET_SEX_ERR 見つけていない
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokeMinomesuForm(const ZUKAN_SAVEDATA * zw, int count)
{
  zukan_incorrect(zw);

  // その数のミノメスフォルムを見ているかチェック
  if( get_threeform_poke_turn_num( zw, MONSNO_MINOMADAMU ) <= count ){
    return ZUKANSAVE_GET_SEX_ERR;
  }
  return get_threeform_poke_see_form( zw, MONSNO_MINOMADAMU, count );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ミノメスのフォルムを見た数
 *
 *  @param  zw  図鑑ワーク
 *
 *  @return フォルムを見た数
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokeMinomesuSeeNum(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return get_threeform_poke_turn_num( zw, MONSNO_MINOMADAMU );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ピチュー地形フォルムナンバー取得
 *
 *  @param  zw    図鑑ワーク
 *  @param  count 見つけた順番数
 *
 *  @retval その順番に見つけたフォルムナンバー
 *  @retval ZUKANSAVE_GET_SEX_ERR 見つけていない
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokePityuuForm(const ZUKAN_SAVEDATA * zw, int count)
{
  zukan_incorrect(zw);

  // その数のミノメスフォルムを見ているかチェック
  if( get_threeform_poke_turn_num( zw, MONSNO_PITYUU ) <= count ){
    return ZUKANSAVE_GET_SEX_ERR;
  }
  return get_threeform_poke_see_form( zw, MONSNO_PITYUU, count );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ピチューのフォルムを見た数
 *
 *  @param  zw  図鑑ワーク
 *
 *  @return フォルムを見た数
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokePityuuSeeNum(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return get_threeform_poke_turn_num( zw, MONSNO_PITYUU );
}

//----------------------------------------------------------------------------
/**
 *  @brief  デオキシスフォルム　を取得する
 *
 *  @param  zw  図鑑ワーク
 *  @param  countカウント
 *
 *  @return デオキシスフォルムナンバー
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokeDeokisisuForm(const ZUKAN_SAVEDATA * zw, int count)
{
  zukan_incorrect(zw);
  return getDEOKISISUFormNo( zw, count );
}

//----------------------------------------------------------------------------
/**
 *  @brief  デオキシスフォルム　を見た数取得
 *
 *  @param  zw  図鑑ワーク
 *
 *  @return 見た数
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokeDeokisisuFormSeeNum(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return nowGetDeokisisuFormNum( zw );
}


//----------------------------------------------------------
/**
 * @brief ずかんデータセット（見つけた）
 * @param zw    ずかんワークへのポインタ
 * @param pp    見つけたポケモンのPOKEMON_PARAMへのポインタ
 */
//----------------------------------------------------------
void ZukanSave_SetPokeSee(ZUKAN_SAVEDATA * zw, const POKEMON_PARAM * pp)
{
  u16 monsno = PP_Get(pp, ID_PARA_monsno, NULL);
  u32 rand = PP_Get(pp, ID_PARA_personal_rnd, NULL);
  u32 sex = PP_GetSex(pp);
  u32 first_sex;  // 最初に見た性別

  zukan_incorrect(zw);
  if (monsno_incorrect(monsno)) {
    return;
  }
  if (!check_see_bit(zw, monsno)) {
    SetZukanRandom(zw, monsno, rand);     // 固体乱数
    set_sex_bit( zw, sex, ZUKANSAVE_SEX_FIRST, monsno );      // 最初に見た性別
  }else{
    // 最初に見た性別と違うときセカンドに登録する
    first_sex = check_sex_bit( zw, monsno, ZUKANSAVE_SEX_FIRST );
    if( first_sex != sex ){
      set_sex_bit( zw, sex, ZUKANSAVE_SEX_SECOND, monsno );     // 次に見た性別
    }
  }
  SetZukanSpecialTurn(zw, monsno, pp);    // 特殊ポケモンの見つけた順番設定
  set_see_bit(zw, monsno);
}

//----------------------------------------------------------
/**
 * @brief ずかんデータセット（捕まえた）
 * @param zw    ずかんワークへのポインタ
 * @param pp    捕まえたポケモンのPOKEMON_PARAMへのポインタ
 */
//----------------------------------------------------------
void ZukanSave_SetPokeGet(ZUKAN_SAVEDATA * zw, const POKEMON_PARAM * pp)
{
  u16 monsno = PP_Get(pp, ID_PARA_monsno, NULL);
  u32 lang = PP_Get(pp, ID_PARA_country_code, NULL);
  u32 rand = PP_Get(pp, ID_PARA_personal_rnd, NULL);
  u32 sex = PP_GetSex(pp);
  u32 first_sex;  // 最初に見た性別

  zukan_incorrect(zw);
  if (monsno_incorrect(monsno)) {
    return;
  }
  if (!check_see_bit(zw, monsno)) {
    SetZukanRandom(zw, monsno, rand);     // 固体乱数
    set_sex_bit( zw, sex, ZUKANSAVE_SEX_FIRST, monsno );      // 表示性別
  }else{
    // 最初に見た性別と違うときセカンドに登録する
    first_sex = check_sex_bit( zw, monsno, ZUKANSAVE_SEX_FIRST );
    if( first_sex != sex ){
      set_sex_bit( zw, sex, ZUKANSAVE_SEX_SECOND, monsno );     // 表示性別
    }
  }
  SetZukanSpecialTurn(zw, monsno, pp);    // 特殊ポケモンの見つけた順番設定
  //アンノーン捕まえた順番設定
  if(monsno == MONSNO_ANNOON){
    SetZukanUnknownTurn( zw, PP_Get( pp, ID_PARA_form_no, NULL), TRUE );
  }

  SetZukanTextVersionUp(zw, monsno, lang);  // 国コード
  // 母国語以外が登録された場合、言語登録マスターフラグをセット
  if( lang != PM_LANG ){
    ZukanSave_SetTextVersionUpMasterFlag( zw );
  }
  set_get_bit(zw, monsno);
  set_see_bit(zw, monsno);
}


//----------------------------------------------------------
/**
 * @brief 全国ずかんモードをセット
 * @param zw    ずかんワークへのポインタ
 */
//----------------------------------------------------------
void ZukanSave_SetZenkokuZukanFlag(ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  zw->zenkoku_flag = TRUE;
}

//----------------------------------------------------------
/**
 * @brief 全国ずかんモードかどうか？の問い合わせ
 * @param zw    ずかんワークへのポインタ
 * @retval  TRUE  全国ずかんモードオープン
 * @retval  FALSE ジョウトずかんモード
 */
//----------------------------------------------------------
BOOL ZukanSave_GetZenkokuZukanFlag(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return zw->zenkoku_flag;
}

//----------------------------------------------------------------------------
/**
 *  @brief  グラフィックバージョン用機能拡張フラグ取得
 *
 *  @param  zw    ずかんワークへのポインタ
 *
 *  @retval TRUE  バージョンアップ完了
 *  @retval FALSE バージョンアップ未完了
 */
//-----------------------------------------------------------------------------
BOOL ZukanSave_GetGraphicVersionUpFlag(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return zw->GraphicVersionUp;
}

//----------------------------------------------------------------------------
/**
 *  @brief  グラフィックバージョン用機能拡張フラグ設定
 *
 *  @param  zw    図鑑ワークへのポインタ
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
void ZukanSave_SetGraphicVersionUpFlag(ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  zw->GraphicVersionUp = 1;
}

//----------------------------------------------------------------------------
/**
 *  @brief  図鑑テキストバージョンチェック
 *
 *  @param  zw
 *  @param  monsno
 *  @param  country_code    国コード
 *
 *  @retval TRUE  その国のバージョンを表示してよい
 *  @retval FALSE その国のバージョンを表示してはいけない
 */
//-----------------------------------------------------------------------------
BOOL ZukanSave_GetTextVersionUpFlag(const ZUKAN_SAVEDATA * zw, u16 monsno, u32 country_code)
{
  int idx;


  GF_ASSERT( country_code < 8 );    // 国コード最大数

  zukan_incorrect(zw);
  // テキストバージョンのあるポケモンかチェック
//  idx = get_zukan_textversion_idx(monsno);
//  if( idx == ZKN_WORLD_TEXT_POKE_NUM ){ return FALSE; }
  idx = monsno;


#if FOREIGN_TEXT_FUNC
  // 外国語テキスト用ナンバーに変換
  country_code = ZKN_WT_GetLANG_Code_ZKN_WORLD_TEXT_Code( country_code );
#endif

  // シンオウ以上のポケモンの海外コードチェックは全てFALSE
  if( monsno > POKEZUKAN_TEXT_POSSIBLE ){
    return FALSE;
  }

  // その言語のフラグがたっているかチェック
  if( (zw->TextVersionUp[idx] & (1<<country_code)) != 0 ){
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  図鑑外国語バージョン　Masterフラグ
 *
 *  @param  zw  図鑑ワーク
 *  このフラグがたっていないと、外国語図鑑はオープンされない
 */
//-----------------------------------------------------------------------------
void ZukanSave_SetTextVersionUpMasterFlag( ZUKAN_SAVEDATA * zw )
{
  zw->TextVersionUpMasterFlag = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  図鑑外国語バージョン　Masterフラグ取得
 *
 *  @param  zw  図鑑ワーク
 *
 *  @retval TRUE  バージョンアップ済み
 *  @retval FALSE バージョンアップされてない
 */
//-----------------------------------------------------------------------------
BOOL ZukanSave_GetTextVersionUpMasterFlag(const ZUKAN_SAVEDATA * zw)
{
  return zw->TextVersionUpMasterFlag;
}

//----------------------------------------------------------------------------
/**
 *  @brief  図鑑保持フラグ取得
 *
 *  @param  zw  図鑑ワークへのポインタ
 *
 *  @retval TRUE  保持
 *  @retval FALSE 保持していない
 */
//-----------------------------------------------------------------------------
BOOL ZukanSave_GetZukanGetFlag(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return zw->zukan_get;
}

//----------------------------------------------------------------------------
/**
 *  @brief  図鑑保持フラグ設定
 *
 *  @param  zw  図鑑ワーク
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
void ZukanSave_SetZukanGetFlag(ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  zw->zukan_get = TRUE;
}

//============================================================================================
//
//  セーブデータ取得のための関数
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief 自分状態データへのポインタ取得
 * @param sv      セーブデータ保持ワークへのポインタ
 * @return  ZUKAN_SAVEDATA  ずかんワークへのポインタ
 */
//----------------------------------------------------------
ZUKAN_SAVEDATA * SaveData_GetZukanSave( SAVE_CONTROL_WORK * sv)
{
  ZUKAN_SAVEDATA * zs;
  zs = SaveControl_DataPtrGet( sv, GMDATA_ID_ZUKAN );
  return zs;
}

//============================================================================================
//============================================================================================
// フォルムありポケモンの見つけた順番せってい
static void set_debag_special_poke_see_no( ZUKAN_SAVEDATA * zw, int monsno )
{
  int i;

  switch( monsno ){
  case MONSNO_ANNOON:
    for( i=0; i<POKEZUKAN_UNKNOWN_NUM; i++ ){
      SetZukanUnknownTurn( zw, i, FALSE );
    }
    break;

  case MONSNO_MINOMUTTI:
  case MONSNO_MINOMADAMU:
  case MONSNO_PITYUU:
    for( i=ZUKANSAVE_MINOMUSHI_FORM_NUM - 1; i>=0; i-- ){
      SetZukanThreeFormPokeTurn( zw, monsno, i );
    }
    break;

  case MONSNO_KARANAKUSI:
  case MONSNO_TORITODON:
  case MONSNO_GIRATHINA:
    for( i=ZUKANSAVE_UMIUSHI_FORM_NUM - 1; i>=0; i-- ){
      SetZukanTwoFormPokeTurn( zw, monsno, i );
    }
    break;

  case MONSNO_SHEIMI:   // シェイミだけ見た目登録順を逆にする
    for( i=0; i<ZUKANSAVE_UMIUSHI_FORM_NUM; i++ ){
      setnum_bit( &zw->syeimi_turn, i, i+1 );
    }
    break;

  case MONSNO_DEOKISISU:
    for( i=ZUKANSAVE_DEOKISISU_FORM_NUM - 1; i>=0; i-- ){
      setDEOKISISUFormNo( zw, i, ZUKANSAVE_DEOKISISU_FORM_NUM-(i+1) );
    }
    break;

  case MONSNO_ROTOMU:
    for( i=0; i<ZUKANSAVE_ROTOM_FORM_NUM; i++ ){
      SetZukanRotomTurn( zw, monsno, i );
    }
    break;
  }
}

// 性別設定
static void set_debug_poke_sex( ZUKAN_SAVEDATA * zw, int monsno, HEAPID heapId )
{
  int sex;
  POKEMON_PERSONAL_DATA *ppd;

  // ポケモンが取りえる性別を取得
  ppd = POKE_PERSONAL_OpenHandle( monsno, 0, heapId );
  sex = PokePersonal_SexGet( ppd, monsno, 0);
  POKE_PERSONAL_CloseHandle( ppd );

  if( sex == PTL_SEX_MALE ){
    // 性別設定
    set_sex_bit( zw, PTL_SEX_MALE, ZUKANSAVE_SEX_FIRST, monsno );
    return;
  }
  if( sex == PTL_SEX_FEMALE ){
    // 性別設定
    set_sex_bit( zw, PTL_SEX_FEMALE, ZUKANSAVE_SEX_FIRST, monsno );
    return;
  }
  if( sex == PTL_SEX_UNKNOWN ){
    // 性別設定
    set_sex_bit( zw, PTL_SEX_UNKNOWN, ZUKANSAVE_SEX_FIRST, monsno );
    return;
  }

  // 性別設定
  if( monsno % 2 ){
    set_sex_bit( zw, PTL_SEX_MALE, ZUKANSAVE_SEX_FIRST, monsno );
    set_sex_bit( zw, PTL_SEX_FEMALE, ZUKANSAVE_SEX_SECOND, monsno );
  }else{
    set_sex_bit( zw, PTL_SEX_FEMALE, ZUKANSAVE_SEX_FIRST, monsno );
    set_sex_bit( zw, PTL_SEX_MALE, ZUKANSAVE_SEX_SECOND, monsno );
  }
}

//----------------------------------------------------------
/**
 * @brief デバッグ用：ずかんデータ生成
 * @param zw      ずかんワークへのポインタ
 * @param option
 */
//----------------------------------------------------------
void Debug_ZukanSave_Make(ZUKAN_SAVEDATA * zw, int start, int end, BOOL see_flg, HEAPID heapId)
{
  int i;
  zukan_incorrect(zw);

  GF_ASSERT( start > 0 );
  GF_ASSERT( end <= MONSNO_END );

  for (i = start; i <= end; i++) {
    if( see_flg == TRUE ){
      set_see_bit(zw, i);
    }else{

      set_get_bit(zw, i);
      set_see_bit(zw, i);

      // 外国図鑑設定
      SetZukanTextVersionUp(zw, i, LANG_JAPAN);
      SetZukanTextVersionUp(zw, i, LANG_ENGLISH);
      SetZukanTextVersionUp(zw, i, LANG_FRANCE);
      SetZukanTextVersionUp(zw, i, LANG_ITALY);
      SetZukanTextVersionUp(zw, i, LANG_GERMANY);
      SetZukanTextVersionUp(zw, i, LANG_SPAIN);
    }

    // 個性乱数設定
    SetZukanRandom( zw, i, 0 );

    // 特殊見つけた順番設定
    set_debag_special_poke_see_no( zw, i );

    // 性別
    set_debug_poke_sex( zw, i, heapId);
  }

  // グラフィックバージョンアップ
  ZukanSave_SetGraphicVersionUpFlag(zw);
  ZukanSave_SetTextVersionUpMasterFlag(zw);
}

void Debug_ZukanSave_Make_PM_LANG(ZUKAN_SAVEDATA * zw, int start, int end, BOOL see_flg, HEAPID heapId)
{
  int i;
  zukan_incorrect(zw);

  GF_ASSERT( start > 0 );
  GF_ASSERT( end <= MONSNO_END );

  for (i = start; i <= end; i++) {
    if( see_flg == TRUE ){
      set_see_bit(zw, i);
    }else{

      set_get_bit(zw, i);
      set_see_bit(zw, i);

      // 外国図鑑設定
      SetZukanTextVersionUp(zw, i, PM_LANG);
    }

    // 個性乱数設定
    SetZukanRandom( zw, i, 0 );

    // 特殊見つけた順番設定
    set_debag_special_poke_see_no( zw, i );

    // 性別
    set_debug_poke_sex( zw, i, heapId );
  }

  // グラフィックバージョンアップ
  ZukanSave_SetGraphicVersionUpFlag(zw);
}

void Debug_ZukanSave_LangSetRand( ZUKAN_SAVEDATA * zw, int start, int end )
{
  int i;

  zukan_incorrect(zw);

  GF_ASSERT( start > 0 );
  GF_ASSERT( end <= MONSNO_END );

  for( i=start; i<=end; i++ ){
    if( check_get_bit( zw, i ) == FALSE ){ continue; }
    if(  GFUser_GetPublicRand(2) ){
      SetZukanTextVersionUp( zw, i, LANG_ENGLISH );
    }
    if( GFUser_GetPublicRand(2) ){
      SetZukanTextVersionUp( zw, i, LANG_FRANCE );
    }
    if( GFUser_GetPublicRand(2) ){
      SetZukanTextVersionUp( zw, i, LANG_ITALY );
    }
    if( GFUser_GetPublicRand(2) ){
      SetZukanTextVersionUp( zw, i, LANG_GERMANY );
    }
    if( GFUser_GetPublicRand(2) ){
      SetZukanTextVersionUp( zw, i, LANG_SPAIN );
    }
  }
  ZukanSave_SetTextVersionUpMasterFlag(zw);
}

void Debug_ZukanSave_LangSetAll( ZUKAN_SAVEDATA * zw, int start, int end )
{
  int i;

  zukan_incorrect(zw);

  GF_ASSERT( start > 0 );
  GF_ASSERT( end <= MONSNO_END );

  for( i=start; i<=end; i++ ){
    if( check_get_bit( zw, i ) == FALSE ){ continue; }
    SetZukanTextVersionUp( zw, i, LANG_ENGLISH );
    SetZukanTextVersionUp( zw, i, LANG_FRANCE );
    SetZukanTextVersionUp( zw, i, LANG_ITALY );
    SetZukanTextVersionUp( zw, i, LANG_GERMANY );
    SetZukanTextVersionUp( zw, i, LANG_SPAIN );
  }
  ZukanSave_SetTextVersionUpMasterFlag(zw);
}

void Debug_ZukanSave_LangSet( ZUKAN_SAVEDATA * zw, int start, int end, u8 lang )
{
  int i;

  zukan_incorrect(zw);

  GF_ASSERT( start > 0 );
  GF_ASSERT( end <= MONSNO_END );

  for( i=start; i<=end; i++ ){
    if( check_get_bit( zw, i ) == FALSE ){ continue; }
    SetZukanTextVersionUp( zw, i, lang );
  }
  ZukanSave_SetTextVersionUpMasterFlag(zw);
}

void Debug_ZukanSave_AnnoonGetSet( ZUKAN_SAVEDATA * zw, int start, int end, HEAPID heapId )
{
  int i;
  zukan_incorrect(zw);

  set_get_bit(zw, MONSNO_ANNOON);
  set_see_bit(zw, MONSNO_ANNOON);

  // 外国図鑑設定
  SetZukanTextVersionUp(zw, MONSNO_ANNOON, PM_LANG);

  // 個性乱数設定
  SetZukanRandom( zw, MONSNO_ANNOON, 0 );

  // 見つけた順・捕まえた順設定
  for( i=start; i<end; i++ ){
    SetZukanUnknownTurn( zw, i, FALSE );
    SetZukanUnknownTurn( zw, i, TRUE );
  }

  // 性別
  set_debug_poke_sex( zw, MONSNO_ANNOON, heapId );

  // グラフィックバージョンアップ
  ZukanSave_SetGraphicVersionUpFlag(zw);
  ZukanSave_SetTextVersionUpMasterFlag(zw);
}


//----------------------------------------------------------------------------
/**
 *  @brief  デオキシスバックアップ
 *
 *  @param  zw  図鑑ワーク
 */
//-----------------------------------------------------------------------------
void Debug_ZukanSave_DeokisisuBuckUp( ZUKAN_SAVEDATA * zw )
{
  // デオキシスフォルムIDバッファ初期化
  InitDeokisisuFormNo( zw );

  // デオキシス保持しているかチェック
  if( ZukanSave_GetPokeSeeFlag( zw, MONSNO_DEOKISISU ) == TRUE ){
    // フォルムナンバー０でセーブ
    setDEOKISISUFormNo( zw, 0, 0 );
  }
}


// 外部参照インデックスを作る時のみ有効(ゲーム中は無効)
#ifdef CREATE_INDEX
void *Index_Get_Zukan_Offset(ZUKAN_SAVEDATA *zw){ return &zw->zukan_get; }
void *Index_Get_Zenkoku_Zukan_Offset(ZUKAN_SAVEDATA *zw){ return &zw->zenkoku_flag; }
void *Index_Get_Get_Flag_Offset(ZUKAN_SAVEDATA *zw){ return &zw->get_flag; }
void *Index_Get_See_Flag_Offset(ZUKAN_SAVEDATA *zw){ return &zw->see_flag; }
void *Index_Get_Sex_Flag_Offset(ZUKAN_SAVEDATA *zw){ return &zw->sex_flag; }
#endif




//------------------------------------------------------------------
/**
 * 全ポケモンの、見つけた順フォルムナンバーを返す
 *
 * @param   zw
 * @param   monsno
 * @param   count
 *
 * @retval  u32
 */
//------------------------------------------------------------------
u32 ZukanSave_GetPokeForm( const ZUKAN_SAVEDATA* zw, int monsno, int count )
{
//  OS_TPrintf("monsno:%d\n", monsno);
  zukan_incorrect(zw);

  switch( monsno ){
  case MONSNO_ANNOON:
//    OS_TPrintf("  Annoon\n");
    if( count < ZukanSave_GetPokeAnnoonNum(zw, FALSE ) )
    {
//      OS_TPrintf("   CheckForm\n");
      return ZukanSave_GetPokeAnnoonForm( zw, count, FALSE );
    }
    break;

  case MONSNO_KARANAKUSI:
    if( count < ZukanSave_GetPokeSiiusiSeeNum(zw) )
    {
      return ZukanSave_GetPokeSiiusiForm( zw, count );
    }
    break;

  case MONSNO_TORITODON:
    if( count < ZukanSave_GetPokeSiidorugoSeeNum(zw) )
    {
      return ZukanSave_GetPokeSiidorugoForm( zw, count );
    }
    break;

  case MONSNO_MINOMUTTI:
    if( count < ZukanSave_GetPokeMinomuttiSeeNum(zw) )
    {
      return ZukanSave_GetPokeMinomuttiForm( zw, count );
    }
    break;

  case MONSNO_MINOMADAMU:
    if( count < ZukanSave_GetPokeMinomesuSeeNum(zw) )
    {
      return ZukanSave_GetPokeMinomesuForm( zw, count );
    }
    break;

  case MONSNO_PITYUU:
    if( count < ZukanSave_GetPokePityuuSeeNum(zw) )
    {
      return ZukanSave_GetPokePityuuForm( zw, count );
    }
    break;

  case MONSNO_DEOKISISU:
    if( count < ZukanSave_GetPokeDeokisisuFormSeeNum(zw) )
    {
      return ZukanSave_GetPokeDeokisisuForm( zw, count );
    }
    break;

  // シェイミ・ギラティナ
  case MONSNO_SHEIMI:
  case MONSNO_GIRATHINA:
    if( count < get_twoform_poke_turn_num( zw, monsno ) ){
      return get_twoform_poke_see_form( zw, monsno, count );
    }
    break;

  // ロトム
  case MONSNO_ROTOMU:
    if( count < get_rotom_turn_num( zw, MONSNO_ROTOMU ) ){
      return get_rotom_see_form( zw, MONSNO_ROTOMU, count );
    }
    break;

  default:
    break;
  }

  return 0;

}

//----------------------------------------------------------------------------
/**
 *  @brief  図鑑  見たことのあるフォルム数を返す
 *
 *  @param  zw      図鑑ワーク
 *  @param  monsno    モンスターナンバー
 *
 *  @return 見たフォルム数
 */
//-----------------------------------------------------------------------------
u32 ZukanSave_GetPokeFormNum( const ZUKAN_SAVEDATA* zw, int monsno )
{
//  OS_TPrintf("monsno:%d\n", monsno);
  zukan_incorrect(zw);

  switch( monsno ){
  case MONSNO_ANNOON:
    return ZukanSave_GetPokeAnnoonNum(zw, FALSE);

  case MONSNO_KARANAKUSI:
    return ZukanSave_GetPokeSiiusiSeeNum(zw);

  case MONSNO_TORITODON:
    return ZukanSave_GetPokeSiidorugoSeeNum(zw);

  case MONSNO_MINOMUTTI:
    return ZukanSave_GetPokeMinomuttiSeeNum(zw);

  case MONSNO_MINOMADAMU:
    return ZukanSave_GetPokeMinomesuSeeNum(zw);

  case MONSNO_PITYUU:
    return ZukanSave_GetPokePityuuSeeNum(zw);

  case MONSNO_DEOKISISU:
    return ZukanSave_GetPokeDeokisisuFormSeeNum(zw);

  // シェイミ
  case MONSNO_SHEIMI:
    return get_twoform_poke_turn_num( zw, MONSNO_SHEIMI );

  // ギラティナ
  case MONSNO_GIRATHINA:
    return get_twoform_poke_turn_num( zw, MONSNO_GIRATHINA );

  // ロトム
  case MONSNO_ROTOMU:
    return get_rotom_turn_num( zw, MONSNO_ROTOMU );

  default:
    break;
  }

  return 1;
}
