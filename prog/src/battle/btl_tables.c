//=============================================================================================
/**
 * @file  btl_tables.c
 * @brief ポケモンWB てきとうなサイズのテーブルをVRAM_Hに置いてアクセスする
 * @author  taya
 *
 * @date  2010.05.27  作成
 */
//=============================================================================================

#include "poke_tool\poketype.h"
#include "poke_tool\monsno_def.h"
#include "waza_tool\wazano_def.h"
#include "item\itemsym.h"
#include "item\item.h"


#include "btl_common.h"
#include "btl_tables.h"


/**
 *  アンコール除外対象のワザか判定
 */
BOOL BTL_TABLES_IsMatchEncoreFail( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_NULL,    WAZANO_ANKOORU,  WAZANO_OUMUGAESI,
    WAZANO_HENSIN,  WAZANO_MONOMANE, WAZANO_SUKETTI,
  };
  u32 i;

  for(i=0; i<NELEMS(table); ++i)
  {
    if( table[i] == waza ){
      return TRUE;
    }
  }
  return FALSE;
}
/**
 *  ものまね失敗対象のワザ判定
 */
BOOL BTL_TABLES_IsMatchMonomaneFail( WazaID waza )
{
  static const WazaID table[] = {
    WAZANO_NULL,    WAZANO_SUKETTI,   WAZANO_MONOMANE,
    WAZANO_HENSIN,  WAZANO_WARUAGAKI, WAZANO_YUBIWOHURU,
    WAZANO_OSYABERI
  };
  u32 i;

  for(i=0; i<NELEMS(table); ++i)
  {
    if( table[i] == waza ){
      return TRUE;
    }
  }
  return FALSE;
}
/**
 *  プレッシャー対象ワザ判定
 */
BOOL BTL_TABLES_IsPressureEffectiveWaza( WazaID waza )
{
  static const WazaID table[] = {
    WAZANO_YOKODORI, WAZANO_HUUIN,
    WAZANO_MAKIBISI, WAZANO_DOKUBISI, WAZANO_SUTERUSUROKKU,

  };
  u32 i;

  for(i=0; i<NELEMS(table); ++i)
  {
    if( table[i] == waza ){
      return TRUE;
    }
  }

  return FALSE;
}

/**
 *  ゆびをふるで出ないワザテーブル
 */
static const WazaID YubiFuruOmmit[] = {
  WAZANO_YUBIWOHURU,    WAZANO_NEGOTO,      WAZANO_NEKONOTE,
  WAZANO_MANEKKO,       WAZANO_SAKIDORI,    WAZANO_OUMUGAESI,
  WAZANO_SIZENNOTIKARA, WAZANO_OSYABERI,    WAZANO_WARUAGAKI,

  WAZANO_SUKETTI,       WAZANO_MONOMANE,    WAZANO_KAUNTAA,
  WAZANO_MIRAAKOOTO,    WAZANO_MAMORU,      WAZANO_MIKIRI,
  WAZANO_KORAERU,       WAZANO_MITIDURE,    WAZANO_KONOYUBITOMARE,
  WAZANO_YOKODORI,      WAZANO_TEDASUKE,    WAZANO_DOROBOU,
  WAZANO_HOSIGARU,      WAZANO_TORIKKU,     WAZANO_KIAIPANTI,
  WAZANO_FEINTO,
};

//=============================================================================================
/**
 * ゆびをふる出ないワザテーブル情報取得
 *
 * @param   elems   [out] テーブル要素数
 *
 * @retval  const WazaID*   テーブルアドレス
 */
//=============================================================================================
const WazaID* BTL_TABLES_GetYubiFuruOmmitTable( u32* elems )
{
  *elems = NELEMS(YubiFuruOmmit);
  return YubiFuruOmmit;
}
//=============================================================================================
/**
 * ゆびをふるで出ないワザか判定
 *
 * @param   waza
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_TABLES_IsYubiFuruOmmit( WazaID waza )
{
  u32 i;
  for(i=0; i<NELEMS(YubiFuruOmmit); ++i)
  {
    if( YubiFuruOmmit[i] == waza ){
      return TRUE;
    }
  }
  return FALSE;
}

//=============================================================================================
/**
 * アイテムコールで発動してはいけないアイテムか判定
 *
 * @param   itemID
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_TABLES_CheckItemCallNoEffect( u16 itemID )
{
  static const u16 table[] = {
    ITEM_KIAINOHATIMAKI,  ITEM_PAWAHURUHAABU,   ITEM_DASSYUTUBOTAN,
    ITEM_ZYAPONOMI,       ITEM_RENBUNOMI,       ITEM_IBANNOMI,
    ITEM_SENSEINOTUME,    ITEM_KYUUKON,         ITEM_ZYUUDENTI,
  };
  u32 i;

  for(i=0; i<NELEMS(table); ++i)
  {
    if( table[i] == itemID ){
      return TRUE;
    }
  }

  return FALSE;
}



#ifdef PM_DEBUG
int GYubiFuruDebugNumber[ BTL_POS_MAX ];

/**
 *  デバッグ用「ゆびをふるテーブル」リセット
 */
void BTL_TABLES_YubifuruDebugReset( void )
{
  u32 i;
  for(i=0; i<NELEMS(GYubiFuruDebugNumber); ++i){
    GYubiFuruDebugNumber[i] = 0;
  }
}
/**
 *  外部から適当に操作した「ゆびをふるテーブル」の禁止ワザを補正
 */
void BTL_TABLES_YubifuruDebugSetEnd( void )
{
  u32 i;
  for(i=0; i<NELEMS(GYubiFuruDebugNumber); ++i){
    if( BTL_TABLES_IsYubiFuruOmmit(GYubiFuruDebugNumber[i]) )
    {
      BTL_TABLES_YubifuruDebugInc(i);
    }
  }
}
/**
 *  「ゆびをふるテーブル」のナンバーをインクリメント（禁止ワザをスキップ）
 */
void BTL_TABLES_YubifuruDebugInc( u8 pos )
{
  while(1)
  {
    GYubiFuruDebugNumber[pos]++;
    if( GYubiFuruDebugNumber[pos] >= WAZANO_MAX )
    {
      GYubiFuruDebugNumber[pos] = 0;
    }
    if( !BTL_TABLES_IsYubiFuruOmmit( GYubiFuruDebugNumber[pos] ) ){
      break;
    }
  };
}

#endif
