//=============================================================================================
/**
 * @file  btl_action.h
 * @brief ポケモンWBバトル プレイヤーが選択した動作内容の受け渡し構造体定義
 * @author  taya
 *
 * @date  2008.10.06  作成
 */
//=============================================================================================
#ifndef __BTL_ACTION_H__
#define __BTL_ACTION_H__

//--------------------------------------------------------------
/**
 *  コマンド選択
 */
//--------------------------------------------------------------
typedef enum {
  BTL_ACTION_NULL=0,  ///< 何もしない（相手のポケモン選択待ち中など）

  BTL_ACTION_FIGHT,
  BTL_ACTION_ITEM,
  BTL_ACTION_CHANGE,
  BTL_ACTION_ESCAPE,

  BTL_ACTION_SKIP,    ///< 反動などで動けない
}BtlAction;


typedef union {

  struct {
    u32 cmd   : 3;
    u32 param : 29;
  }gen;

  struct {
    u32 cmd     : 3;
    u32 targetPos : 3;
    u32 waza      : 16;
  }fight;

  struct {
    u32 cmd       : 3;
    u32 number    : 16;
    u32 targetIdx : 3;
  }item;

  struct {
    u32 cmd       : 3;
    u32 posIdx    : 3;  // 入れ替え対象位置ID
    u32 memberIdx : 3;  // 選ばれたポケモンのパーティ内インデックス
    u32 depleteFlag : 1;
  }change;

  struct {
    u32 cmd     : 3;
  }escape;


}BTL_ACTION_PARAM;

// たたかうアクション
static void BTL_ACTION_SetFightParam( BTL_ACTION_PARAM* p, WazaID waza, u8 targetPos )
{
  p->fight.cmd = BTL_ACTION_FIGHT;
  p->fight.targetPos = targetPos;
  p->fight.waza = waza;
}
// アイテムつかうアクション
static void BTL_ACTION_SetItemParam( BTL_ACTION_PARAM* p, u16 itemNumber, u8 targetIdx )
{
  p->item.cmd = BTL_ACTION_ITEM;
  p->item.number = itemNumber;
  p->item.targetIdx = targetIdx;
}
// 入れ替えポケモン選択アクション（選択対象は未定）
static inline void BTL_ACTION_SetChangeBegin( BTL_ACTION_PARAM* p )
{
  p->change.cmd = BTL_ACTION_CHANGE;
  p->change.posIdx = 0;
  p->change.memberIdx = 0;
  p->change.depleteFlag = 0;
}

// 入れ替えポケモン選択アクション（通常）
static void BTL_ACTION_SetChangeParam( BTL_ACTION_PARAM* p, u8 posIdx, u8 memberIdx )
{
  p->change.cmd = BTL_ACTION_CHANGE;
  p->change.posIdx = posIdx;
  p->change.memberIdx = memberIdx;
  p->change.depleteFlag = 0;
}
// 入れ替えポケモン選択アクション（もう戦えるポケモンがいない）
static void BTL_ACTION_SetChangeDepleteParam( BTL_ACTION_PARAM* p )
{
  p->change.cmd = BTL_ACTION_CHANGE;
  p->change.memberIdx = 0;
  p->change.depleteFlag = 1;
}

static void BTL_ACTION_SetEscapeParam( BTL_ACTION_PARAM* p )
{
  p->escape.cmd = BTL_ACTION_ESCAPE;
}

static void BTL_ACTION_SetNULL( BTL_ACTION_PARAM* p )
{
  p->gen.cmd = BTL_ACTION_NULL;
}

static void BTL_ACTION_SetSkip( BTL_ACTION_PARAM* p )
{
  p->gen.cmd = BTL_ACTION_SKIP;
}

static inline BtlAction BTL_ACTION_GetAction( const BTL_ACTION_PARAM* p )
{
  return p->gen.cmd;
}

static inline WazaID BTL_ACTION_GetWazaID( const BTL_ACTION_PARAM* act )
{
  if( act->gen.cmd == BTL_ACTION_FIGHT ){
    return act->fight.waza;
  }
  return WAZANO_NULL;
}

#endif

