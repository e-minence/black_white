/**
 * @file  tr_ai.c
 * @brief トレーナーAIプログラム
 * @author  HisashiSogabe
 * @date  06.04.25
 */
//============================================================================================
#include <gflib.h>

#include "system/vm_cmd.h"

#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"
#include "poke_tool/tokusyu_def.h"
#include "waza_tool/wazano_def.h"
#include "waza_tool/waza_tool.h"

#include "battle/battle.h"
#include "battle/btl_calc.h"
#include "battle/btl_field.h"
#include "battle/btl_pokeparam.h"
#include "battle/btl_sideeff.h"
#include "battle/btl_util.h"

#include "item/item.h"

#include "tr_ai.h"
#include "tr_ai_def.h"
#include "tr_ai.cdat"

#include "arc_def.h"

#ifdef PM_DEBUG
#if defined DEBUG_ONLY_FOR_sogabe | defined DEBUG_ONLY_FOR_morimoto
#define POINT_VIEW
#endif
#endif

//=========================================================================
//
//=========================================================================
#define TR_AI_WAZATBL_MAX ( 48 )

//=========================================================================
//  AI用の構造体宣言
//=========================================================================

typedef struct{
  u8          seq_no;
  u8          waza_pos;
  WazaID      waza_no;

  int         waza_point[ PTL_WAZA_MAX ];

  int         calc_work;
  u32         ai_bit;
  u32         ai_bit_temp;

  u8          status_flag;
  u8          ai_no;
  u8          select_waza_pos;    //選択した技のポジション
  u8          select_waza_dir;    //選択した技を当てる相手

  u8          damage_loss[ PTL_WAZA_MAX ];
                    //ここから上は、AIチェックのたびにクリアする
  u16         use_waza[ BTL_POS_MAX ][ PTL_WAZA_MAX ];  //使用した技をストックしていく
                                                      //（基本見た技しか知らないようにするため）
  u8          look_tokusei[ BTL_FRONT_POKE_MAX ];       //見た特性を保持

  u16         have_item[2][4];      //トレーナーの持ちアイテム
  u8          have_item_cnt[2];     //トレーナーの持ちアイテム数
  BtlPokePos  atk_btl_poke_pos;   //攻撃側ポケモンのクライアントID
  BtlPokePos  def_btl_poke_pos;   //防御側ポケモンのクライアントID

  BtlRule       rule;
  BtlCompetitor competitor;

  WazaID        wazaID[TR_AI_WAZATBL_MAX ];
  WAZA_DATA     *wd[ TR_AI_WAZATBL_MAX ];

  HEAPID        heapID;
  ARCHANDLE*    handle;
  ARCHANDLE*    waza_handle;
  VM_CODE*      sequence;

  const BTL_MAIN_MODULE*    wk;
  BTL_SVFLOW_WORK*          svfWork;
  const BTL_POKE_CONTAINER* pokeCon;
  const BTL_POKEPARAM*      atk_bpp;
  const BTL_POKEPARAM*      def_bpp;

  VMCMD_RESULT  vmcmd_result;   //スクリプト制御ワーク

#if 0
  u8  AI_ITEM_TYPE[2];
  u8  AI_ITEM_CONDITION[2];

  u16 AI_ITEM_NO[2];

  u8  AI_DirSelectClient[CLIENT_MAX];     //AIで選択した攻撃対象

  ITEMDATA    *item;                      //アイテムテーブル

  u16       AI_CALC_COUNT[CLIENT_MAX];    //計算実行した回数
  u16       AI_CALC_CONTINUE[CLIENT_MAX]; //計算途中かどうか
#endif
}TR_AI_WORK;

enum
{
  TR_AI_VM_STACK_SIZE = 16, ///<使用するスタックのサイズ
  TR_AI_VM_REG_SIZE   = 8,    ///<使用するレジスタの数

  WAZAPOINT_FLAT = 100,     //技繰り出し判定用点数の初期値
};

typedef enum{
  COND_UNDER = 0,   //<
  COND_OVER,        //>
  COND_EQUAL,       //==
  COND_NOTEQUAL,    //!=
  COND_BIT,         //
  COND_NBIT,        //
  COND_OR_UNDER,    //<=
  COND_OR_OVER,     //>=
}BranchCond;

//=========================================================================
//  AI_THINK_BIT宣言
//=========================================================================
enum
{
  AI_THINK_NO_BASIC = 0,    //基本AI
  AI_THINK_NO_STRONG,       //攻撃型AI
  AI_THINK_NO_EXPERT,       //エキスパートAI
  AI_THINK_NO_HOJO,         //補助技AI
  AI_THINK_NO_GAMBLE,       //ギャンブルAI
  AI_THINK_NO_DAMAGE,       //ダメージ重視AI
  AI_THINK_NO_RELAY,        //リレー型AI
  AI_THINK_NO_DOUBLE,       //ダブルバトルAI
  AI_THINK_NO_HP_CHECK,     //HPで判断AI
  AI_THINK_NO_WEATHER,      //天候技AI
  AI_THINK_NO_HARASS,       //いやがらせAI

  AI_THINK_NO_MV_POKE,      //移動ポケモン
  AI_THINK_NO_SAFARI,       //サファリゾーン
  AI_THINK_NO_GUIDE,        //ガイドバトル

  AI_THINK_NO_MAX,          //
};

enum
{
  AI_THINK_BIT_BASIC    = 1 << AI_THINK_NO_BASIC,     //基本AI
  AI_THINK_BIT_STRONG   = 1 << AI_THINK_NO_STRONG,    //攻撃型AI
  AI_THINK_BIT_EXPERT   = 1 << AI_THINK_NO_EXPERT,    //エキスパートAI
  AI_THINK_BIT_HOJO     = 1 << AI_THINK_NO_HOJO,      //補助技AI
  AI_THINK_BIT_GAMBLE   = 1 << AI_THINK_NO_GAMBLE,    //ギャンブルAI
  AI_THINK_BIT_DAMAGE   = 1 << AI_THINK_NO_DAMAGE,    //ダメージ重視AI
  AI_THINK_BIT_RELAY    = 1 << AI_THINK_NO_RELAY,     //リレー型AI
  AI_THINK_BIT_DOUBLE   = 1 << AI_THINK_NO_DOUBLE,    //ダブルバトルAI
  AI_THINK_BIT_HP_CHECK = 1 << AI_THINK_NO_HP_CHECK,  //HPで判断AI
  AI_THINK_BIT_WEATHER  = 1 << AI_THINK_NO_WEATHER,   //天候技AI
  AI_THINK_BIT_HARASS   = 1 << AI_THINK_NO_HARASS,    //いやがらせAI
  AI_THINK_BIT_MV_POKE  = 1 << AI_THINK_NO_MV_POKE,   //移動ポケモン
  AI_THINK_BIT_SAFARI   = 1 << AI_THINK_NO_SAFARI,    //サファリゾーン
  AI_THINK_BIT_GUIDE    = 1 << AI_THINK_NO_GUIDE,     //ガイドバトル
};


//-----------------------------------------------------------------------------
//          プロトタイプ宣言
//-----------------------------------------------------------------------------
static  BOOL  waza_ai_single( VMHANDLE* vmh, TR_AI_WORK* taw );
static  BOOL  waza_ai_double( VMHANDLE* vmh, TR_AI_WORK* taw );
static  void  tr_ai_sequence( VMHANDLE* vmh, TR_AI_WORK* taw );

static  VMCMD_RESULT  AI_IF_RND_UNDER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_RND_OVER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_RND_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_RND_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_INCDEC( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HP_UNDER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HP_OVER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HP_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_HP_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_POKESICK( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_POKESICK( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_WAZASICK( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_WAZASICK( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_DOKUDOKU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_DOKUDOKU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_CONTFLG( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_CONTFLG( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_SIDEEFF( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_SIDEEFF( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_UNDER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_OVER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_BIT( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_BIT( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_WAZANO( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_WAZANO( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_TABLE_JUMP( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_TABLE_JUMP( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HAVE_DAMAGE_WAZA( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_HAVE_DAMAGE_WAZA( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_TURN( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_TYPE( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_IRYOKU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_COMP_POWER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_LAST_WAZA( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_WAZA_TYPE( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_WAZA_TYPE( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_FIRST( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_BENCH_COUNT( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_WAZANO( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_WAZASEQNO( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_TOKUSEI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_AISYOU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_WAZA_AISYOU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_BENCH_COND( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_BENCH_COND( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_WEATHER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_WAZA_SEQNO_JUMP( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_WAZA_SEQNO_JUMP( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_PARA_UNDER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_PARA_OVER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_PARA_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_PARA_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_WAZA_HINSHI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_WAZA_HINSHI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HAVE_WAZA( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_HAVE_WAZA( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HAVE_WAZA_SEQNO( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_HAVE_WAZA_SEQNO( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_WAZA_CHECK_STATE( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_ESCAPE( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_SAFARI_ESCAPE_JUMP( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_SAFARI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_SOUBI_ITEM( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_SOUBI_EQUIP( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_POKESEX( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_NEKODAMASI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_TAKUWAERU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_BTL_RULE( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_BTL_COMPETITOR( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_RECYCLE_ITEM( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_WORKWAZA_TYPE( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_WORKWAZA_POW( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_WORKWAZA_SEQNO( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_MAMORU_COUNT( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_JUMP( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_AIEND( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_LEVEL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_CHOUHATSU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_CHOUHATSU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_MIKATA_ATTACK( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_HAVE_TYPE( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_HAVE_TOKUSEI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_ALREADY_MORAIBI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HAVE_ITEM( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_FLDEFF_CHECK( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_SIDEEFF_COUNT( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_BENCH_HPDEC( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_BENCH_PPDEC( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_NAGETSUKERU_IRYOKU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_PP_REMAIN( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_TOTTEOKI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_WAZA_KIND( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_LAST_WAZA_KIND( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_AGI_RANK( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_SLOWSTART_TURN( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_BENCH_DAMAGE_MAX( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HAVE_BATSUGUN( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_LAST_WAZA_DAMAGE_CHECK( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_STATUS_UP( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_STATUS_DIFF( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_CHECK_STATUS_UNDER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_CHECK_STATUS_OVER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_CHECK_STATUS_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_COMP_POWER_WITH_PARTNER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HINSHI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_HINSHI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_GET_TOKUSEI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_MIGAWARI( VMHANDLE* vmh, void* context_work );

static  void  ai_if_rnd( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_hp( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_pokesick( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_wazasick( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_moudoku( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_contflg( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_sideeff( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_wazano( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_bench_cond( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_waza_seq_no_jump( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_para( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_waza_hinshi( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_have_waza( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_have_waza_seqno( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_chouhatsu( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_hinshi( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  BOOL  branch_act( VMHANDLE* vmh, BranchCond cond, int src, int dst, int adrs );
static  BtlPokePos get_poke_pos( TR_AI_WORK* taw, int side );
static  u32   get_table_data( int* adrs );
static  int   get_tokusei( TR_AI_WORK* taw, int side, BtlPokePos pos );
static  const BTL_POKEPARAM*  get_bpp( TR_AI_WORK* taw, BtlPokePos pos );
static  const BTL_POKEPARAM*  get_bpp_from_party( const BTL_PARTY* pty, u8 idx );
static  void  waza_no_stock( TR_AI_WORK* taw );
static  int   get_waza_param( TR_AI_WORK* taw, WazaID waza_no, WazaDataParam param );

//============================================================================================
/**
 *  スクリプトテーブル
 */
//============================================================================================
static const VMCMD_FUNC tr_ai_command_table[]={
  AI_IF_RND_UNDER,
  AI_IF_RND_OVER,
  AI_IF_RND_EQUAL,
  AI_IFN_RND_EQUAL,
  AI_INCDEC,
  AI_IF_HP_UNDER,
  AI_IF_HP_OVER,
  AI_IF_HP_EQUAL,
  AI_IFN_HP_EQUAL,
  AI_IF_POKESICK,
  AI_IFN_POKESICK,
  AI_IF_WAZASICK,
  AI_IFN_WAZASICK,
  AI_IF_DOKUDOKU,
  AI_IFN_DOKUDOKU,
  AI_IF_CONTFLG,
  AI_IFN_CONTFLG,
  AI_IF_SIDEEFF,
  AI_IFN_SIDEEFF,
  AI_IF_UNDER,
  AI_IF_OVER,
  AI_IF_EQUAL,
  AI_IFN_EQUAL,
  AI_IF_BIT,
  AI_IFN_BIT,
  AI_IF_WAZANO,
  AI_IFN_WAZANO,
  AI_IF_TABLE_JUMP,
  AI_IFN_TABLE_JUMP,
  AI_IF_HAVE_DAMAGE_WAZA,
  AI_IFN_HAVE_DAMAGE_WAZA,
  AI_CHECK_TURN,
  AI_CHECK_TYPE,
  AI_CHECK_IRYOKU,
  AI_COMP_POWER,
  AI_CHECK_LAST_WAZA,
  AI_IF_EQUAL,          //旧AI_IF_WAZA_TYPE　内容的には一緒なので、まとめた
  AI_IFN_EQUAL,         //旧AI_IFN_WAZA_TYPE　内容的には一緒なので、まとめた
  AI_IF_FIRST,
  AI_CHECK_BENCH_COUNT,
  AI_CHECK_WAZANO,
  AI_CHECK_WAZASEQNO,
  AI_CHECK_TOKUSEI,
  AI_CHECK_AISYOU,
  AI_CHECK_WAZA_AISYOU,
  AI_IF_BENCH_COND,
  AI_IFN_BENCH_COND,
  AI_CHECK_WEATHER,
  AI_IF_WAZA_SEQNO_JUMP,
  AI_IFN_WAZA_SEQNO_JUMP,
  AI_IF_PARA_UNDER,
  AI_IF_PARA_OVER,
  AI_IF_PARA_EQUAL,
  AI_IFN_PARA_EQUAL,
  AI_IF_WAZA_HINSHI,
  AI_IFN_WAZA_HINSHI,
  AI_IF_HAVE_WAZA,
  AI_IFN_HAVE_WAZA,
  AI_IF_HAVE_WAZA_SEQNO,
  AI_IFN_HAVE_WAZA_SEQNO,
  AI_IF_WAZA_CHECK_STATE,
  AI_ESCAPE,
  AI_SAFARI_ESCAPE_JUMP,
  AI_SAFARI,
  AI_CHECK_SOUBI_ITEM,
  AI_CHECK_SOUBI_EQUIP,
  AI_CHECK_POKESEX,
  AI_CHECK_NEKODAMASI,
  AI_CHECK_TAKUWAERU,
  AI_CHECK_BTL_RULE,
  AI_CHECK_BTL_COMPETITOR,
  AI_CHECK_RECYCLE_ITEM,
  AI_CHECK_WORKWAZA_TYPE,
  AI_CHECK_WORKWAZA_POW,
  AI_CHECK_WORKWAZA_SEQNO,
  AI_CHECK_MAMORU_COUNT,
  AI_JUMP,
  AI_AIEND,
  AI_IF_LEVEL,
  AI_IF_CHOUHATSU,
  AI_IFN_CHOUHATSU,
  AI_IF_MIKATA_ATTACK,
  AI_CHECK_HAVE_TYPE,
  AI_CHECK_HAVE_TOKUSEI,
  AI_IF_ALREADY_MORAIBI,
  AI_IF_HAVE_ITEM,
  AI_FLDEFF_CHECK,
  AI_CHECK_SIDEEFF_COUNT,
  AI_IF_BENCH_HPDEC,
  AI_IF_BENCH_PPDEC,
  AI_CHECK_NAGETSUKERU_IRYOKU,
  AI_CHECK_PP_REMAIN,
  AI_IF_TOTTEOKI,
  AI_CHECK_WAZA_KIND,
  AI_CHECK_LAST_WAZA_KIND,
  AI_CHECK_AGI_RANK,
  AI_CHECK_SLOWSTART_TURN,
  AI_IF_BENCH_DAMAGE_MAX,
  AI_IF_HAVE_BATSUGUN,
  AI_IF_LAST_WAZA_DAMAGE_CHECK,
  AI_CHECK_STATUS_UP,
  AI_CHECK_STATUS_DIFF,
  AI_IF_CHECK_STATUS_UNDER,
  AI_IF_CHECK_STATUS_OVER,
  AI_IF_CHECK_STATUS_EQUAL,
  AI_COMP_POWER_WITH_PARTNER,
  AI_IF_HINSHI,
  AI_IFN_HINSHI,
  AI_GET_TOKUSEI,
  AI_IF_MIGAWARI,
};

enum{
  AI_SEQ_THINK_INIT=0,
  AI_SEQ_THINK,
  AI_SEQ_END,
};

//============================================================================================
/**
 *  VMイニシャライザテーブル
 */
//============================================================================================
static  const VM_INITIALIZER  vm_init={
  TR_AI_VM_STACK_SIZE,                 //u16 stack_size; ///<使用するスタックのサイズ
  TR_AI_VM_REG_SIZE,                   //u16 reg_size;   ///<使用するレジスタの数
  tr_ai_command_table,                 //const VMCMD_FUNC * command_table; ///<使用する仮想マシン命令の関数テーブル
  NELEMS( tr_ai_command_table ),       //const u32 command_max;      ///<使用する仮想マシン命令定義の最大数
};

//============================================================================================
/**
 * @brief VM初期化
 *
 * @param[in] wk
 * @param[in] pokeCon
 * @param[in] ai_bit  起動するAIをビットで指定
 * @param[in] heapID  ヒープID
 */
//============================================================================================
VMHANDLE* TR_AI_Init( const BTL_MAIN_MODULE* wk, BTL_SVFLOW_WORK* svfWork, const BTL_POKE_CONTAINER* pokeCon, u32 ai_bit, HEAPID heapID )
{
  VMHANDLE* vmh;
  TR_AI_WORK*  taw = GFL_HEAP_AllocClearMemory( heapID, sizeof( TR_AI_WORK ) );

  //AIBITの最大値チェック
  GF_ASSERT( AI_THINK_NO_MAX < 32 );

  taw->heapID = heapID;
  taw->vmcmd_result = VMCMD_RESULT_CONTINUE;
  taw->ai_bit_temp = ai_bit;

  taw->handle = GFL_ARC_OpenDataHandle( ARCID_TR_AI, heapID );
  taw->waza_handle = WAZADATA_OpenDataHandle( heapID );

  {
    int i;

    for( i = 0 ; i < TR_AI_WAZATBL_MAX ; i++ )
    {
      taw->wd[ i ] = GFL_HEAP_AllocMemory( heapID, WAZADATA_GetWorkSize() );
    }
  }

  taw->wk = wk;
  taw->svfWork = svfWork;
  taw->pokeCon = pokeCon;

  taw->rule        = BTL_MAIN_GetRule( wk );
  taw->competitor  = BTL_MAIN_GetCompetitor( wk );

  vmh = VM_Create( heapID, &vm_init );
  VM_Init( vmh, taw );

  return vmh;
}

//============================================================================================
/**
 * @brief VMメイン
 *
 * @param[in] vmh 仮想マシン制御構造体へのポインタ
 *
 * @retval  TRUE:計算中 FALSE:計算終了
 */
//============================================================================================
BOOL  TR_AI_Main( VMHANDLE* vmh )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)VM_GetContext( vmh );
  BOOL  ret = FALSE;

#if 0 //@todo ダブル未作成
  if( taw->rule == BTL_RULE_SINGLE )
  {
    taw->def_btl_poke_pos = taw->atk_btl_poke_pos ^ 1;
    ret = waza_ai_single( vmh, taw );
  }
  else if( taw->rule == BTL_RULE_DOUBLE )
  {
    ret = waza_ai_double( vmh, taw );
  }
#else
  taw->def_btl_poke_pos = taw->atk_btl_poke_pos ^ 1;
  ret = waza_ai_single( vmh, taw );
#endif

  return ret;
}

//============================================================================================
/**
 * @brief VM終了
 *
 * @param[in] vmh 仮想マシン制御構造体へのポインタ
 */
//============================================================================================
void  TR_AI_Exit( VMHANDLE* vmh )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)VM_GetContext( vmh );
  int i;

  GFL_ARC_CloseDataHandle( taw->handle );
  GFL_ARC_CloseDataHandle( taw->waza_handle );

  for( i = 0 ; i < TR_AI_WAZATBL_MAX ; i++ )
  {
    GFL_HEAP_FreeMemory ( taw->wd[ i ] );
  }
  GFL_HEAP_FreeMemory ( taw );
  VM_Delete( vmh );
}

//============================================================================================
/**
 * @brief VM起動
 *
 * @param[in] vmh       仮想マシン制御構造体へのポインタ
 * @param[in] unselect  選べない技
 * @param[in] pos       起動するポケモンクライアントID
 */
//============================================================================================
void  TR_AI_Start( VMHANDLE* vmh, u8* unselect, BtlPokePos pos )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)VM_GetContext( vmh );
  int i;

  GF_ASSERT_MSG( taw->ai_bit == 0, "AIが計算中です" );

  taw->seq_no = 0;
  taw->waza_no = 0;
  taw->waza_pos = 0;
  taw->calc_work = 0;
  taw->status_flag = 0;
  taw->ai_no = 0;
  taw->atk_btl_poke_pos = pos;
  taw->ai_bit = taw->ai_bit_temp;

  for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
  {
    taw->waza_point[ i ] = WAZAPOINT_FLAT;
  }

  {
    //繰り出せない技のポイントは０にして除外
    for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
    {
      if( unselect[ i ] == 0 )
      {
        taw->waza_point[ i ] = 0;
      }
      taw->damage_loss[ i ] = 100 - GFL_STD_MtRand( 16 );
    }
  }
}

//============================================================================================
/**
 * @brief 選択した技のポジションを返す
 *
 * @param[in] vmh       仮想マシン制御構造体へのポインタ
 */
//============================================================================================
u8  TR_AI_GetSelectWazaPos( VMHANDLE* vmh )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)VM_GetContext( vmh );

  return taw->select_waza_pos;    //選択した技のポジション
}

//============================================================================================
/**
 * @brief 選択した技の攻撃対象を返す
 *
 * @param[in] vmh       仮想マシン制御構造体へのポインタ
 */
//============================================================================================
u8  TR_AI_GetSelectWazaDir( VMHANDLE* vmh )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)VM_GetContext( vmh );

  return taw->select_waza_dir;    //選択した技を当てる相手
}

//============================================================================================
/**
 * @brief AIシーケンス（シングル）
 *
 * @param[in] taw
 *
 * @retval  TRUE:計算中 FALSE:計算終了
 */
//============================================================================================
static  BOOL  waza_ai_single( VMHANDLE* vmh, TR_AI_WORK* taw )
{
  taw->atk_bpp = get_bpp( taw, taw->atk_btl_poke_pos );
  taw->def_bpp = get_bpp( taw, taw->def_btl_poke_pos );

  waza_no_stock( taw );

  while( taw->ai_bit )
  {
    if( taw->ai_bit & 1 )
    {
      if( ( taw->status_flag & AI_STATUSFLAG_CONTINUE ) == 0 )
      {
        taw->seq_no = AI_SEQ_THINK_INIT;
      }
      tr_ai_sequence( vmh, taw );
    }
    taw->ai_bit >>= 1;
    taw->ai_no++;
    taw->waza_pos = 0;
  }
  if( taw->status_flag & AI_STATUSFLAG_ESCAPE )
  {
    taw->select_waza_pos = AI_ENEMY_ESCAPE;
  }
  //@todo 現状サファリはないのでコメント
//  else if(sp->AIWT.AI_STATUSFLAG&AI_STATUSFLAG_SAFARI){
//    taw->select_waza_pos = AI_ENEMY_SAFARI;
//  }
  else{
    int i;
    int poscnt = 1;
    u8  point[ PTL_WAZA_MAX ];
    u8  poswork[ PTL_WAZA_MAX ];

    point[ 0 ] = taw->waza_point[ 0 ];
    poswork[ 0 ] = 0;

    for( i = 1 ; i < PTL_WAZA_MAX ; i++ )
    {
      //技がないところは、無視
      if( i < BPP_WAZA_GetCount( taw->atk_bpp ) )
      {
        if( point[ 0 ] == taw->waza_point[ i ]){
          point[ poscnt ] = taw->waza_point[ i ];
          poswork[ poscnt++ ] = i;
        }
        if( point[ 0 ] < taw->waza_point[ i ] )
        {
          poscnt = 1;
          point[ 0 ] = taw->waza_point[ i ];
          poswork[ 0 ] = i;
        }
      }
    }
    taw->select_waza_pos = poswork[ GFL_STD_MtRand( poscnt ) ];
  }

#ifdef POINT_VIEW
  {
    int i;
    for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
    {
      OS_TPrintf( "waza%d:%d\n", i+1, taw->waza_point[ i ] );
    }
  }
#endif

  taw->select_waza_dir = taw->def_btl_poke_pos;

  return FALSE;
}

//============================================================================================
/**
 * @brief AIシーケンス（ダブル）
 *
 * @param[in] taw
 *
 * @retval  TRUE:計算中 FALSE:計算終了
 */
//============================================================================================
static  BOOL  waza_ai_double( VMHANDLE* vmh, TR_AI_WORK* taw )
{
  //@todo ダブルは未作成
  return FALSE;
}

//============================================================================================
/**
 * @brief AIシーケンスメイン
 *
 * @param[in] taw
 *
 * @retval  TRUE:計算中 FALSE:計算終了
 */
//============================================================================================
static  void  tr_ai_sequence( VMHANDLE* vmh, TR_AI_WORK* taw )
{
  while( taw->seq_no != AI_SEQ_END )
  {
    switch( taw->seq_no ){
    case AI_SEQ_THINK_INIT:
      {
        //PPがない場合は技なし @todo PPがない状態ならwaza_pointが0になっているはずなので、そっちでチェック
        if( BPP_WAZA_GetPP( taw->atk_bpp, taw->waza_pos ) == 0 )
        {
          taw->waza_no = 0;
        }
        else{
          taw->waza_no = BPP_WAZA_GetID( taw->atk_bpp, taw->waza_pos );
          taw->sequence = GFL_ARC_LoadDataAllocByHandle( taw->handle, taw->ai_no, taw->heapID );
          VM_Start( vmh, taw->sequence );
        }
      }
      taw->seq_no++;
      /*fallthru*/
    case AI_SEQ_THINK:
      if( taw->waza_no != 0 )
      {
        BOOL  ret = VM_Control( vmh );
        if( taw->status_flag & AI_STATUSFLAG_END )
        {
          GFL_HEAP_FreeMemory( taw->sequence );
          taw->sequence = NULL;
        }
      }
      else{
        taw->waza_point[ taw->waza_pos ] = 0;
        taw->status_flag |= AI_STATUSFLAG_END;
      }
      if( taw->status_flag & AI_STATUSFLAG_END )
      {
        taw->waza_pos++;
        if( ( taw->waza_pos < BPP_WAZA_GetCount( taw->atk_bpp ) ) &&
          ( ( taw->status_flag & AI_STATUSFLAG_FINISH ) == 0 ) )
        {
          taw->seq_no = AI_SEQ_THINK_INIT;
        }
        else
        {
          taw->seq_no++;
        }
        taw->status_flag &= AI_STATUSFLAG_END_OFF;
      }
      break;
    default:
    case AI_SEQ_END:
      taw->seq_no = AI_SEQ_END;
      break;
    }
  }
}

//------------------------------------------------------------
//  ランダム分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_RND_UNDER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_rnd( vmh, taw, COND_UNDER );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_RND_OVER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_rnd( vmh, taw, COND_OVER );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_RND_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_rnd( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_RND_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_rnd( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  ランダム分岐メイン
//------------------------------------------------------------
static  void  ai_if_rnd( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int value = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  int rnd = GFL_STD_MtRand( 256 );

  branch_act( vmh, cond, rnd, value, adrs );
}

//------------------------------------------------------------
//  ポイント加減算
//------------------------------------------------------------
static  VMCMD_RESULT  AI_INCDEC( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int value = ( int )VMGetU32( vmh );

  taw->waza_point[ taw->waza_pos ] += value;

  if( taw->waza_point[ taw->waza_pos ] < 0 )
  {
    taw->waza_point[ taw->waza_pos ] = 0;
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  HPでの分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_HP_UNDER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_hp( vmh, taw, COND_UNDER );

  return taw->vmcmd_result;
}

static  VMCMD_RESULT  AI_IF_HP_OVER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_hp( vmh, taw, COND_OVER );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_HP_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_hp( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_HP_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_hp( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  HPでの分岐処理メイン
//------------------------------------------------------------
static  void  ai_if_hp( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  int value = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  int hpratio = BPP_GetHPRatio( get_bpp( taw, pos ) );

  branch_act( vmh, cond, hpratio >> FX32_SHIFT, value, adrs );
}

//------------------------------------------------------------
//  ポケモンが何らかの状態異常にかかっているかチェックして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_POKESICK( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_pokesick( vmh, context_work, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_POKESICK( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_pokesick( vmh, context_work, COND_EQUAL );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ポケモンが何らかの状態異常にかかっているかチェックして分岐処理メイン
//------------------------------------------------------------
static  void  ai_if_pokesick( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  PokeSick  sick  = BPP_GetPokeSick( get_bpp( taw, pos ) );

  branch_act( vmh, cond, sick, POKESICK_NULL, adrs );
}

//------------------------------------------------------------
//  ポケモンが何らかの状態異常(WAZASICK)にかかっているかチェックして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_WAZASICK( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_wazasick( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_WAZASICK( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_wazasick( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ポケモンが何らかの状態異常(WAZASICK)にかかっているかチェックして分岐処理メイン
//------------------------------------------------------------
static  void  ai_if_wazasick( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  WazaSick  value = ( WazaSick )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  BOOL  sick = BPP_CheckSick( get_bpp( taw, pos ), value );

  branch_act( vmh, cond, sick, TRUE, adrs );
}

//------------------------------------------------------------
//  ポケモンがどくどくにかかっているかチェックして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_DOKUDOKU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_moudoku( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_DOKUDOKU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_moudoku( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ポケモンが「もうどく」にかかっているかをチェックして分岐
//------------------------------------------------------------
static  void  ai_if_moudoku( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  const BTL_POKEPARAM* bpp = get_bpp( taw, pos );
  BOOL result = FALSE;

  if( BPP_CheckSick(bpp, WAZASICK_DOKU) )
  {
     BPP_SICK_CONT cont = BPP_GetSickCont( bpp, WAZASICK_DOKU );
     if( BPP_SICKCONT_IsMoudokuCont(cont) ){
      result = TRUE;
    }
  }

  branch_act( vmh, cond, result, TRUE, adrs );
}

//------------------------------------------------------------
//  ポケモンが何らかの状態異常(CONTFLG)にかかっているかチェックして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_CONTFLG( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_contflg( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_CONTFLG( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_contflg( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  ポケモンが何らかの状態異常(CONTFLG)にかかっているかチェックして分岐処理メイン
//------------------------------------------------------------
static  void  ai_if_contflg( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  BppContFlag value = ( BppContFlag )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  BOOL contflg = BPP_CONTFLAG_Get( get_bpp( taw, pos ), value );

  branch_act( vmh, cond, contflg, TRUE, adrs );
}

//------------------------------------------------------------
//  場に対して何らかの状態変化がかかっているかチェックして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_SIDEEFF( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_sideeff( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_SIDEEFF( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_sideeff( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  場に対して何らかの状態変化がかかっているかチェックして分岐処理メイン
//------------------------------------------------------------
static  void  ai_if_sideeff( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  BtlSideEffect value = ( BtlSideEffect )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  BtlSideEffect sideeff = 0; //@todo 取得関数に置き換える必要がある

  branch_act( vmh, cond, sideeff, value, adrs );
}

//------------------------------------------------------------
//  ワークと比較して分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_UNDER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if( vmh, taw, COND_UNDER );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_OVER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if( vmh, taw, COND_OVER );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_BIT( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if( vmh, taw, COND_BIT );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_BIT( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if( vmh, taw, COND_NBIT );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  ワークと比較して分岐処理メイン
//------------------------------------------------------------
static  void  ai_if( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int value = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );

  //OS_TPrintf("ai_if:\nsrc:%d dst:%d cond:%d\n",taw->calc_work,value,cond);

  branch_act( vmh, cond, taw->calc_work, value, adrs );
}

//------------------------------------------------------------
//  今計算している技ナンバーと比較して分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_WAZANO( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_wazano( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_WAZANO( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_wazano( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  今計算している技ナンバーと比較して分岐処理メイン
//------------------------------------------------------------
static  void  ai_if_wazano( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int value = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );

  branch_act( vmh, cond, taw->waza_no, value, adrs );
}

//------------------------------------------------------------
//  指定されたテーブルを参照して分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_TABLE_JUMP( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int tbl_adrs  = ( int )VMGetU32( vmh );
  int jump_adrs = ( int )VMGetU32( vmh );
  int data;

  while( ( data = get_table_data( &tbl_adrs ) ) != TR_AI_TABLE_END )
  {
    if( taw->calc_work == data )
    {
      VMCMD_Jump( vmh, vmh->adrs + jump_adrs );
      break;
    }
  }

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_TABLE_JUMP( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int tbl_adrs  = ( int )VMGetU32( vmh );
  int jump_adrs = ( int )VMGetU32( vmh );
  int data;

  while( ( data = get_table_data( &tbl_adrs ) ) != TR_AI_TABLE_END )
  {
    if( taw->calc_work == data )
    {
      return taw->vmcmd_result;
    }
  }
  VMCMD_Jump( vmh, vmh->adrs + jump_adrs );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  自分がダメージ技をもっているかチェックして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_HAVE_DAMAGE_WAZA( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int adrs = ( int )VMGetU32( vmh );
  int i;

  for( i = 0 ; i < BPP_WAZA_GetCount( taw->atk_bpp ) ; i++ )
  {
    if( get_waza_param( taw, BPP_WAZA_GetID( taw->atk_bpp, i ), WAZAPARAM_POWER ) != 0 )
    {
      break;
    }
  }

  if( i < BPP_WAZA_GetCount( taw->atk_bpp ) )
  {
    VMCMD_Jump( vmh, vmh->adrs + adrs );
  }

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_HAVE_DAMAGE_WAZA( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int adrs = ( int )VMGetU32( vmh );
  int i;

  for( i = 0 ; i < BPP_WAZA_GetCount( taw->atk_bpp ) ; i++ )
  {
    if( get_waza_param( taw, BPP_WAZA_GetID( taw->atk_bpp, i ), WAZAPARAM_POWER ) != 0 )
    {
      break;
    }
  }

  if( i == BPP_WAZA_GetCount( taw->atk_bpp ) )
  {
    VMCMD_Jump( vmh, vmh->adrs + adrs );
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ターンのチェック（今何ターン目かをワークに入れる）
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_TURN( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = 1;  //@todo 取得関数に置き換える必要があります

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  タイプのチェック(ポケモンあるいは技のタイプをワークに入れる）
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_TYPE( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  PokeTypePair  atk_type = BPP_GetPokeType( taw->atk_bpp );
  PokeTypePair  def_type = BPP_GetPokeType( taw->def_bpp );

  switch( side ){
  case CHECK_ATTACK_TYPE1:
    taw->calc_work = PokeTypePair_GetType1( atk_type );
    break;
  case CHECK_DEFENCE_TYPE1:
    taw->calc_work = PokeTypePair_GetType1( def_type );
    break;
  case CHECK_ATTACK_TYPE2:
    taw->calc_work = PokeTypePair_GetType2( atk_type );
    break;
  case CHECK_DEFENCE_TYPE2:
    taw->calc_work = PokeTypePair_GetType2( def_type );
    break;
  case CHECK_WAZA:
    taw->calc_work = get_waza_param( taw, taw->waza_no, WAZAPARAM_TYPE );
    break;
  case CHECK_ATTACK_FRIEND_TYPE1:
    {
      PokeTypePair  atk_type_f = BPP_GetPokeType( get_bpp( taw, get_poke_pos( taw, CHECK_ATTACK_FRIEND ) ) );
      taw->calc_work = PokeTypePair_GetType1( atk_type_f );
    }
    break;
  case CHECK_DEFENCE_FRIEND_TYPE1:
    {
      PokeTypePair  def_type_f = BPP_GetPokeType( get_bpp( taw, get_poke_pos( taw, CHECK_DEFENCE_FRIEND ) ) );
      taw->calc_work = PokeTypePair_GetType1( def_type_f );
    }
    break;
  case CHECK_ATTACK_FRIEND_TYPE2:
    {
      PokeTypePair  atk_type_f = BPP_GetPokeType( get_bpp( taw, get_poke_pos( taw, CHECK_ATTACK_FRIEND ) ) );
      taw->calc_work = PokeTypePair_GetType2( atk_type_f );
    }
    break;
  case CHECK_DEFENCE_FRIEND_TYPE2:
    {
      PokeTypePair  def_type_f = BPP_GetPokeType( get_bpp( taw, get_poke_pos( taw, CHECK_DEFENCE_FRIEND ) ) );
      taw->calc_work = PokeTypePair_GetType2( def_type_f );
    }
    break;
  default:
    GF_ASSERT(0);
    break;
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  攻撃技かどうかのチェック（技の威力をワークに入れる)（HGSS時点で未使用コマンド）
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_IRYOKU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = WAZADATA_GetPower( taw->waza_no );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  威力が一番高いかのチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_COMP_POWER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int loss_flag = ( int )VMGetU32( vmh );

  //@todo 技のダメージを取得する関数を作る必要があります　とりあえず技の威力で判定
  {
    int i;
    int src_pow = get_waza_param( taw, taw->waza_no, WAZAPARAM_POWER );

    if( src_pow == 0 )
    {
      taw->calc_work = COMP_POWER_NONE;
    }
    else
    {
      taw->calc_work = COMP_POWER_TOP;

      for( i = 0 ; i < BPP_WAZA_GetCount( taw->atk_bpp ) ; i++ )
      {
        int pow = get_waza_param( taw, BPP_WAZA_GetID( taw->atk_bpp, i ), WAZAPARAM_POWER );

        if( i == taw->waza_pos ) continue;
        if( pow > src_pow )
        {
          taw->calc_work = COMP_POWER_NOTOP;
          break;
        }
      }
    }
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  前のターンに使った技のチェック（技のナンバーをワークに入れる）
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_LAST_WAZA( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = BPP_GetPrevWazaID( get_bpp( taw, pos ) );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  どちらが先行かチェックして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_FIRST( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int cond  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );

  //@todo どちらが先攻かを取得する関数を作る必要があります

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  控えが何体いるかチェック（数をワークに入れる）
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_BENCH_COUNT( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  u8  clientID = BTL_MAIN_BtlPosToClientID( taw->wk, pos );
  const BTL_PARTY*  pty = BTL_POKECON_GetPartyDataConst( taw->pokeCon, clientID );
  //前衛の数
  u8  front_count = BTL_MAIN_GetClientCoverPosCount( taw->wk, clientID );
  int i;

  taw->calc_work = 0;

  for( i = front_count ; i < BTL_PARTY_GetMemberCount( pty ) ; i++ )
  {
    const BTL_POKEPARAM* bpp = get_bpp_from_party( pty, i );
    //@todo タマゴの場合も考慮しなければならない（田谷君の話だとタマゴならそもそもBTL_PARTYに入れなければいいということみたい）
    if( !BPP_IsDead( bpp ) )
    {
      taw->calc_work++;
    }
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  現在の技ナンバーのチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_WAZANO( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = taw->waza_no;

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  現在の技ナンバーのシーケンスナンバーのチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_WAZASEQNO( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = get_waza_param( taw, taw->waza_no, WAZAPARAM_AI_SEQNO );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  とくせいのチェック（とくせいナンバーをワークに入れる）
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_TOKUSEI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = get_tokusei( taw, side, pos );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  自分と相手の相性チェック（HGSS時点で未使用コマンド）
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_AISYOU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "未作成" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  技のタイプと相手の相性をチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_WAZA_AISYOU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  BtlTypeAff  aisyou  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  PokeTypePair  def_type = BPP_GetPokeType( taw->def_bpp );
  PokeType  waza_type = get_waza_param( taw, taw->waza_no, WAZAPARAM_TYPE );
  BtlTypeAff  aff1 = BTL_CALC_TypeAff( waza_type, PokeTypePair_GetType1( def_type ) );
  BtlTypeAff  aff2 = BTL_CALC_TypeAff( waza_type, PokeTypePair_GetType2( def_type ) );
  BtlTypeAff  aff = BTL_CALC_TypeAffMul( aff1, aff2 );

  //@todo とりあえず特性とかなにも考えずに単純に相性チェック

  branch_act( vmh, COND_EQUAL, aff, aisyou, adrs );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  控えの状態チェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_BENCH_COND( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_bench_cond( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_BENCH_COND( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_bench_cond( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  控えの状態チェックして分岐処理メイン
//------------------------------------------------------------
static  void  ai_if_bench_cond( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  u8  clientID = BTL_MAIN_BtlPosToClientID( taw->wk, pos );
  const BTL_PARTY*  pty = BTL_POKECON_GetPartyDataConst( taw->pokeCon, clientID );
  //前衛の数
  u8  front_count = BTL_MAIN_GetClientCoverPosCount( taw->wk, clientID );
  int i;

  for( i = front_count ; i < BTL_PARTY_GetMemberCount( pty ) ; i++ )
  {
    const BTL_POKEPARAM* bpp = get_bpp_from_party( pty, i );

    if( !BPP_IsDead( bpp ) )
    {
      PokeSick  bench_sick = BPP_GetPokeSick( bpp );
      if( branch_act( vmh, cond, bench_sick, POKESICK_NULL, adrs ) )
      {
        break;
      }
    }
  }
}

//------------------------------------------------------------
//  天候チェック（天候ナンバーをワークに入れる）
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_WEATHER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = BTL_FIELD_GetWeather();

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  技のシーケンスナンバーをチェックして、分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_WAZA_SEQNO_JUMP( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_waza_seq_no_jump( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_WAZA_SEQNO_JUMP( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_waza_seq_no_jump( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  技のシーケンスナンバーをチェックして、分岐処理メイン
//------------------------------------------------------------
static  void  ai_if_waza_seq_no_jump( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int seqno = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  int data  = get_waza_param( taw, taw->waza_no, WAZAPARAM_AI_SEQNO );

  branch_act( vmh, cond, data, seqno, adrs );
}

//------------------------------------------------------------
//  自分や相手のパラメータ変化値を参照して分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_PARA_UNDER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_para( vmh, taw, COND_UNDER );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_PARA_OVER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_para( vmh, taw, COND_OVER );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_PARA_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_para( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_PARA_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_para( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  自分や相手のパラメータ変化値を参照して分岐処理メイン
//------------------------------------------------------------
static  void  ai_if_para( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  BppValueID para  = ( int )VMGetU32( vmh );
  int value = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  int data  = BPP_GetValue( get_bpp( taw, pos ), para );

  branch_act( vmh, cond, data, value, adrs );
}

//------------------------------------------------------------
//  技のダメージ計算をして相手が瀕死になるかチェックして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_WAZA_HINSHI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_waza_hinshi( vmh, taw, COND_OR_UNDER );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_WAZA_HINSHI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_waza_hinshi( vmh, taw, COND_OVER );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  技のダメージ計算をして相手が瀕死になるかチェックして分岐処理メイン
//------------------------------------------------------------
static  void  ai_if_waza_hinshi( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int loss_flag = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  int hp = BPP_GetValue( taw->def_bpp, BPP_HP );
  int damage = 1; //@todo ダメージ取得処理が必要

  branch_act( vmh, cond, hp, damage, adrs );
}

//------------------------------------------------------------
//  特定の技を持っているかのチェックをして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_HAVE_WAZA( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_have_waza( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_HAVE_WAZA( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_have_waza( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  特定の技を持っているかのチェックをして分岐処理メイン
//------------------------------------------------------------
static  void  ai_if_have_waza( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  WazaID waza  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  int i;
  BOOL  ret = FALSE;

  switch( side ){
  case CHECK_ATTACK:
    for( i = 0 ; i < BPP_WAZA_GetCount( taw->atk_bpp ) ; i++ )
    {
      WazaID have_waza = BPP_WAZA_GetID( taw->atk_bpp, i );
      if( have_waza == waza )
      {
        ret = TRUE;
        break;
      }
    }
    break;
  case CHECK_ATTACK_FRIEND:
    {
      const BTL_POKEPARAM* bpp = get_bpp( taw, get_poke_pos( taw, CHECK_ATTACK_FRIEND ) );
      if( BPP_IsDead( bpp ) )
      {
        break;
      }
      for( i = 0 ; i < BPP_WAZA_GetCount( bpp ) ; i++ )
      {
        WazaID have_waza = BPP_WAZA_GetID( bpp, i );
        if( have_waza == waza )
        {
          ret = TRUE;
          break;
        }
      }
    }
    break;
  case CHECK_DEFENCE:
    for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
    {
      if( taw->use_waza[ pos ][ i ] == waza )
      {
        ret = TRUE;
        break;
      }
    }
    break;
  default:
    OS_Printf("ここにくるのは、おかしい\n");
    break;
  }

  branch_act( vmh, cond, ret, TRUE, adrs );
}

//------------------------------------------------------------
//  特定の技シーケンスを持っているかのチェックをして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_HAVE_WAZA_SEQNO( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_have_waza_seqno( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_HAVE_WAZA_SEQNO( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_have_waza_seqno( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  特定の技シーケンスを持っているかのチェックをして分岐処理メイン
//------------------------------------------------------------
static  void  ai_if_have_waza_seqno( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  int seqno = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  int i;
  BOOL  ret = FALSE;

  switch( side ){
  case CHECK_ATTACK:
    for( i = 0 ; i < BPP_WAZA_GetCount( taw->atk_bpp ) ; i++ )
    {
      int have_seqno = get_waza_param( taw, BPP_WAZA_GetID( taw->atk_bpp, i ), WAZAPARAM_AI_SEQNO );
      if( have_seqno == seqno )
      {
        ret = TRUE;
        break;
      }
    }
    break;
  case CHECK_DEFENCE:
    for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
    {
      int have_seqno = get_waza_param( taw, taw->use_waza[ pos ][ i ], WAZAPARAM_AI_SEQNO );
      if( have_seqno == seqno )
      {
        ret = TRUE;
        break;
      }
    }
    break;
  default:
    OS_Printf("ここにくるのは、おかしい\n");
    break;
  }

  branch_act( vmh, cond, ret, TRUE, adrs );
}


//------------------------------------------------------------
//  技の状態をチェックをして分岐（HGSS時点で未使用コマンド）
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_WAZA_CHECK_STATE( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "未作成" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  にげるをせんたく
//------------------------------------------------------------
static  VMCMD_RESULT  AI_ESCAPE( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "未作成" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  サファリゾーンでの逃げる確率を計算して逃げるときのアドレスを指定
//------------------------------------------------------------
static  VMCMD_RESULT  AI_SAFARI_ESCAPE_JUMP( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "未作成" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  サファリゾーンでの特殊アクションを選択
//------------------------------------------------------------
static  VMCMD_RESULT  AI_SAFARI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "未作成" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//    装備アイテムのチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_SOUBI_ITEM( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = BPP_GetItem( get_bpp( taw, pos ) );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//    装備効果のチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_SOUBI_EQUIP( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  u32 item = BPP_GetItem( get_bpp( taw, pos ) );

  taw->calc_work = ITEM_GetParam( item, ITEM_PRM_EQUIP, taw->heapID );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//    ポケモンの性別のチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_POKESEX( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = BPP_GetValue( get_bpp( taw, pos ), BPP_SEX );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//    ねこだましできるかチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_NEKODAMASI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  //@todo のちほど変更
  taw->calc_work = ( BPP_GetTurnCount( get_bpp( taw, pos ) ) == 0 );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//    たくわえるカウンタのチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_TAKUWAERU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = BPP_COUNTER_Get( get_bpp( taw, pos ), BPP_COUNTER_TAKUWAERU );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//    BTL_RULEのチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_BTL_RULE( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = taw->rule;

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//    BTL_COMPETITORのチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_BTL_COMPETITOR( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = taw->competitor;

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//    リサイクルできるアイテムのチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_RECYCLE_ITEM( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = BPP_GetConsumedItem( get_bpp( taw, pos ) );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ワークに入っている技ナンバーのタイプをチェック（HGSS時点で未使用コマンド）
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_WORKWAZA_TYPE( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "未作成" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ワークに入っている技ナンバーの威力をチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_WORKWAZA_POW( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT( taw->calc_work < WAZANO_MAX );

  taw->calc_work = get_waza_param( taw, taw->calc_work, WAZAPARAM_POWER );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ワークに入っている技ナンバーのシーケンスナンバーをチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_WORKWAZA_SEQNO( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = get_waza_param( taw, taw->calc_work, WAZAPARAM_AI_SEQNO );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  まもるカウンタをチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_MAMORU_COUNT( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  const BTL_POKEPARAM* bpp = get_bpp( taw, pos );

  if( ( BPP_GetPrevWazaID( bpp ) != WAZANO_MAMORU ) &&
      ( BPP_GetPrevWazaID( bpp ) != WAZANO_MIKIRI ) &&
      ( BPP_GetPrevWazaID( bpp ) != WAZANO_KORAERU ) )
  {
    taw->calc_work = 0;
  }
  else
  {
    taw->calc_work = BPP_GetWazaContCounter( bpp );
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  シーケンスジャンプ
//------------------------------------------------------------
static  VMCMD_RESULT  AI_JUMP( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int adrs  = ( int )VMGetU32( vmh );

  VMCMD_Jump( vmh, vmh->adrs + adrs );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  シーケンス終了
//------------------------------------------------------------
static  VMCMD_RESULT  AI_AIEND( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->status_flag |= AI_STATUSFLAG_END;

  //仮想マシン停止
  VM_End( vmh );

  return VMCMD_RESULT_SUSPEND;
}

//------------------------------------------------------------
//  お互いのレベルをチェックして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_LEVEL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int value  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  int cond[ 3 ] = { COND_OVER, COND_UNDER, COND_EQUAL };
  int atk_level = BPP_GetValue( taw->atk_bpp, BPP_LEVEL );
  int def_level = BPP_GetValue( taw->def_bpp, BPP_LEVEL );

  GF_ASSERT( value < LEVEL_EQUAL + 1 );

  branch_act( vmh, cond[ value ], atk_level, def_level, adrs );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  挑発状態かチェックして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_CHOUHATSU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_chouhatsu( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_CHOUHATSU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_chouhatsu( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  挑発状態かチェックして分岐処理メイン
//------------------------------------------------------------
static  void  ai_if_chouhatsu( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int adrs  = ( int )VMGetU32( vmh );
  BOOL  value = BPP_CheckSick( taw->def_bpp, WAZASICK_TYOUHATSU );

  branch_act( vmh, cond, value, TRUE, adrs );
}

//------------------------------------------------------------
//  攻撃対象が味方がどうかチェックして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_MIKATA_ATTACK( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int adrs  = ( int )VMGetU32( vmh );

  branch_act( vmh, COND_EQUAL, ( taw->atk_btl_poke_pos & 1 ), ( taw->def_btl_poke_pos & 1 ), adrs );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  タイプをもっているかチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_HAVE_TYPE( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  PokeType  type  = ( PokeType )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  PokeTypePair  poke_type = BPP_GetPokeType( get_bpp( taw, pos ) );

  if( ( PokeTypePair_GetType1( poke_type ) == type ) ||
      ( PokeTypePair_GetType2( poke_type ) == type ) )
  {
    taw->calc_work = HAVE_YES;
  }
  else
  {
    taw->calc_work = HAVE_NO;
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  とくせい持っているかチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_HAVE_TOKUSEI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side    = ( int )VMGetU32( vmh );
  int tokusei = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  int have_tokusei = get_tokusei( taw, side, pos );

  if( have_tokusei == tokusei )
  {
    taw->calc_work = HAVE_YES;
  }
  else
  {
    taw->calc_work = HAVE_NO;
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  すでに「もらいび」でパワーアップ状態にあるかチェックして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_ALREADY_MORAIBI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  //@todo
  GF_ASSERT_MSG( 0, "未作成" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  アイテムを持っているかチェックして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_HAVE_ITEM( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  int item = ( int )VMGetU32( vmh );
  int adrs = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  u32 have_item = BPP_GetItem( get_bpp( taw, pos ) );

  //@todo
  GF_ASSERT_MSG( 0, "未作成" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  FLDEFFチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_FLDEFF_CHECK( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  BtlFieldEffect  fldeff = ( BtlFieldEffect )VMGetU32( vmh );
  int adrs = ( int )VMGetU32( vmh );
  BOOL data = BTL_FIELD_CheckEffect( fldeff );

  branch_act( vmh, COND_EQUAL, data, TRUE, adrs );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  SIDEEFFのカウントを取得
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_SIDEEFF_COUNT( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  int flag = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  //flagでまきびし、どくびしを分岐
  taw->calc_work = 0;//@todo 取得関数

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  控えポケモンのHP減少をチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_BENCH_HPDEC( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  u8  clientID = BTL_MAIN_BtlPosToClientID( taw->wk, pos );
  const BTL_PARTY*  pty = BTL_POKECON_GetPartyDataConst( taw->pokeCon, clientID );
  //前衛の数
  u8  front_count = BTL_MAIN_GetClientCoverPosCount( taw->wk, clientID );
  int i;

  for( i = front_count ; i < BTL_PARTY_GetMemberCount( pty ) ; i++ )
  {
    const BTL_POKEPARAM* bpp = get_bpp_from_party( pty, i );

    if( !BPP_IsDead( bpp ) )
    {
      int hpratio = BPP_GetHPRatio( get_bpp( taw, pos ) );
      if( branch_act( vmh, COND_UNDER, hpratio, 100, adrs ) )
      {
        break;
      }
    }
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  控えポケモンのPP減少をチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_BENCH_PPDEC( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  u8  clientID = BTL_MAIN_BtlPosToClientID( taw->wk, pos );
  const BTL_PARTY*  pty = BTL_POKECON_GetPartyDataConst( taw->pokeCon, clientID );
  //前衛の数
  u8  front_count = BTL_MAIN_GetClientCoverPosCount( taw->wk, clientID );
  int i, j;

  for( i = front_count ; i < BTL_PARTY_GetMemberCount( pty ) ; i++ )
  {
    const BTL_POKEPARAM* bpp = get_bpp_from_party( pty, i );

    if( !BPP_IsDead( bpp ) )
    {
      for( j = 0 ; j < BPP_WAZA_GetCount( get_bpp( taw, pos ) ) ; j++ )
      {
        int ppshort = BPP_WAZA_GetPPShort( get_bpp( taw, pos ), j );
        if( branch_act( vmh, COND_NOTEQUAL, ppshort, 0, adrs ) )
        {
          break;
        }
      }
      if( j != BPP_WAZA_GetCount( get_bpp( taw, pos ) ) )
      {
        break;
      }
    }
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  装備アイテムのなげつける威力を取得
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_NAGETSUKERU_IRYOKU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = 0;

  //差し押さえされているかチェック
  if( !BPP_CheckSick( get_bpp( taw, pos ), WAZASICK_SASIOSAE ) )
  {
    u32 item = BPP_GetItem( get_bpp( taw, pos ) );
    taw->calc_work = ITEM_GetParam( item, ITEM_PRM_NAGETUKERU_ATC, taw->heapID );
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  残りPPを取得
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_PP_REMAIN( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = BPP_WAZA_GetPP( taw->atk_bpp, taw->waza_pos );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  とっておきチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_TOTTEOKI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  int adrs = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  const BTL_POKEPARAM* bpp = get_bpp( taw, pos );

  //持っている技を出し切っていないか、持っている技が2以上ない場合は失敗
  if( ( BPP_WAZA_GetUsedCount( bpp ) >= BPP_WAZA_GetCount( bpp ) ) && (BPP_WAZA_GetCount( bpp ) > 1) )
  {
    VMCMD_Jump( vmh, vmh->adrs + adrs );
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  技の分類チェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_WAZA_KIND( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = get_waza_param( taw, taw->waza_no, WAZAPARAM_DAMAGE_TYPE );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  相手が最後に出した技の分類チェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_LAST_WAZA_KIND( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = get_waza_param( taw, BPP_GetPrevWazaID( taw->def_bpp ), WAZAPARAM_DAMAGE_TYPE );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  素早さで指定した側が何位かチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_AGI_RANK( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = 0;  //@todo 取得関数

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  スロースタート何ターン目か
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_SLOWSTART_TURN( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = BPP_GetTurnCount( get_bpp( taw, pos ) );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  控えにいる方がダメージを与えるかどうかチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_BENCH_DAMAGE_MAX( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  //@todo
  GF_ASSERT_MSG( 0, "未作成" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  抜群の技を持っているかチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_HAVE_BATSUGUN( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  //@todo
  GF_ASSERT_MSG( 0, "未作成" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  指定した相手の最後に出した技と自分の技とのダメージをチェック
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_LAST_WAZA_DAMAGE_CHECK( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  //@todo
  GF_ASSERT_MSG( 0, "未作成" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  指定した相手のステータス上昇分の値を取得
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_STATUS_UP( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  int i;
  BppValueID  id_tbl[]={
    BPP_ATTACK_RANK,
    BPP_DEFENCE_RANK,
    BPP_SP_ATTACK_RANK,
    BPP_SP_DEFENCE_RANK,
    BPP_AGILITY_RANK,
    BPP_HIT_RATIO,
    BPP_AVOID_RATIO,
  };

  taw->calc_work = 0;

  for( i = 0 ; i < NELEMS( id_tbl ) ; i++ )
  {
    int rank = BPP_GetValue( get_bpp( taw, pos ), id_tbl[ i ] );
    if( rank > 6 )
    {
      taw->calc_work += ( rank - 6 );
    }
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  指定した相手とのステータス差分を取得
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_STATUS_DIFF( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  int flag = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = BPP_GetValue( get_bpp( taw, pos ), flag ) - BPP_GetValue( taw->atk_bpp, flag );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  指定した相手とのステータスをチェックして分岐（HGSS時点で未使用コマンド）
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_CHECK_STATUS_UNDER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "未作成" );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_CHECK_STATUS_OVER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "未作成" );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_CHECK_STATUS_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "未作成" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  威力が一番高いかのチェック（パートナーも含む）
//------------------------------------------------------------
static  VMCMD_RESULT  AI_COMP_POWER_WITH_PARTNER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  //@todo
  GF_ASSERT_MSG( 0, "未作成" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  指定した相手が瀕死かチェックして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_HINSHI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_hinshi( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}

static  VMCMD_RESULT  AI_IFN_HINSHI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_hinshi( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  指定した相手が瀕死かチェックして分岐処理メイン
//------------------------------------------------------------
static  void  ai_if_hinshi( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side = ( int )VMGetU32( vmh );
  int adrs = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  BOOL  hinshi = BPP_IsDead( get_bpp( taw, pos ) );

  branch_act( vmh, cond, hinshi, TRUE, adrs );
}

//------------------------------------------------------------
//  技効果を考慮して特性を取得（移動ポケモン専用）
//------------------------------------------------------------
static  VMCMD_RESULT  AI_GET_TOKUSEI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  //@todo
  GF_ASSERT_MSG( 0, "未作成" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  みがわり中かチェックして分岐
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_MIGAWARI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  int adrs = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  if( BPP_MIGAWARI_IsExist( get_bpp( taw, pos ) ) )
  {
    VMCMD_Jump( vmh, vmh->adrs + adrs );
  }

  return taw->vmcmd_result;
}

//============================================================================================
/**
 *  条件によってブランチ処理
 *
 * @param[in] vmh
 * @param[in] cond  ブランチ条件
 * @param[in] src   比較対象１
 * @param[in] dst   比較対象２
 * @param[in] adrs  飛び先
 *
 * @retval  TRUE:分岐した　FALSE:分岐しない
 */
//============================================================================================
static  BOOL  branch_act( VMHANDLE* vmh, BranchCond cond, int src, int dst, int adrs )
{
  BOOL  branch = FALSE;

  switch( cond ){
  case COND_UNDER:
    if( src < dst )
    {
      branch = TRUE;
    }
    break;
  case COND_OVER:
    if( src > dst )
    {
      branch = TRUE;
    }
    break;
  case COND_EQUAL:
    if( src == dst )
    {
      branch = TRUE;
    }
    break;
  case COND_NOTEQUAL:
    if( src != dst )
    {
      branch = TRUE;
    }
    break;
  case COND_BIT:
    if( src & dst )
    {
      branch = TRUE;
    }
    break;
  case COND_NBIT:
    if( ( src & dst ) == 0 )
    {
      branch = TRUE;
    }
    break;
  case COND_OR_UNDER:
    if( src <= dst )
    {
      branch = TRUE;
    }
    break;
  case COND_OR_OVER:
    if( src >= dst )
    {
      branch = TRUE;
    }
    break;
  }
  if( branch == TRUE )
  {
    VMCMD_Jump( vmh, vmh->adrs + adrs );
  }

  return branch;
}

//============================================================================================
/**
 *  チェックするBtlPokePosを取得
 */
//============================================================================================
static  BtlPokePos  get_poke_pos( TR_AI_WORK* taw, int side )
{
  BtlPokePos btl_poke_pos;

  switch( side ){
  case CHECK_ATTACK:
    btl_poke_pos = taw->atk_btl_poke_pos;
    break;
  case CHECK_DEFENCE:
  default:
    btl_poke_pos = taw->def_btl_poke_pos;
    break;
  case CHECK_ATTACK_FRIEND:
    btl_poke_pos = taw->atk_btl_poke_pos ^ 2;
    break;
  case CHECK_DEFENCE_FRIEND:
    btl_poke_pos = taw->def_btl_poke_pos ^ 2;
    break;
  }
  return btl_poke_pos;
}

//============================================================================================
/**
 *  テーブルデータをu8型で取得
 */
//============================================================================================
static  inline  u8  get_u8_data( int* adrs )
{
  u8 ret = *(adrs);
  ++ adrs;
  return ret;
}

//============================================================================================
/**
 *  テーブルデータをu32型で取得
 */
//============================================================================================
static  u32 get_table_data( int* adrs )
{
  u32 val;
  u8  a,b,c,d;

  a = get_u8_data( adrs );
  b = get_u8_data( adrs );
  c = get_u8_data( adrs );
  d = get_u8_data( adrs );

  val = 0;
  val += (u32)d;
  val <<= 8;
  val += (u32)c;
  val <<= 8;
  val += (u32)b;
  val <<= 8;
  val += (u32)a;

  return val;
}

//============================================================================================
/**
 *  立ち位置を指定して特性を取得
 */
//============================================================================================
static  int get_tokusei( TR_AI_WORK* taw, int side, BtlPokePos pos )
{
  int have_tokusei;
  const BTL_POKEPARAM*  bpp = get_bpp( taw, pos );

  //いえきを受けているかチェック
  if( BPP_CheckSick( bpp, WAZASICK_IEKI ) )
  {
    have_tokusei = 0;
  }
  //防御側は持っている可能性を考慮した判定をする
  else if( ( side == CHECK_DEFENCE ) || ( side == CHECK_DEFENCE_FRIEND ) )
  {
    //特性の発現を見ている場合はそれを取得
    if( taw->look_tokusei[ pos ] )
    {
      have_tokusei = taw->look_tokusei[ pos ];
    }
    else
    {
      have_tokusei = BPP_GetValue( bpp, BPP_TOKUSEI ); //素直に取得
      //かげふみ、じりょく、ありじごくは、自分が逃げられないことで察知できているはずなので、除外する
      if( ( have_tokusei != TOKUSYU_KAGEHUMI ) &&
          ( have_tokusei != TOKUSYU_ZIRYOKU ) &&
          ( have_tokusei != TOKUSYU_ARIZIGOKU ) )
      {
        //特性の発現を見ていない場合はパーソナルで持ちうる特性から取得
        int monsno = BPP_GetMonsNo( bpp );
        int formno = BPP_GetValue( bpp, BPP_TOKUSEI );
        int tokusei1;
        int tokusei2;

        tokusei1 = POKETOOL_GetPersonalParam( monsno, formno, POKEPER_ID_speabi1 );
        tokusei2 = POKETOOL_GetPersonalParam( monsno, formno, POKEPER_ID_speabi2 );

        //特性を2種類持っているポケモンは、可能性を探る
        if( ( tokusei1 ) && ( tokusei2 ) )
        {
          //どちらにも一致しないなら持っていないことにする
          if( ( tokusei1 != have_tokusei ) && ( tokusei2 != have_tokusei ) )
          {
            have_tokusei = 0;
          }
        }
        //1種類しかもっていないなら決めうち
        else if( tokusei1 )
        {
          have_tokusei = tokusei1;
        }
        else
        {
          have_tokusei = tokusei2;
        }
      }
    }
  }
  else
  {
    have_tokusei = BPP_GetValue( bpp, BPP_TOKUSEI ); //自分自身の特性は知っているので、素直に取得
  }

  return have_tokusei;
}

//============================================================================================
/**
 *  BTL_POKEPARAMを取得
 */
//============================================================================================
static  const BTL_POKEPARAM*  get_bpp( TR_AI_WORK* taw, BtlPokePos pos )
{
  return  BTL_POKECON_GetFrontPokeDataConst( taw->pokeCon, pos );
}

static  const BTL_POKEPARAM*  get_bpp_from_party( const BTL_PARTY* pty, u8 idx )
{
  return  BTL_PARTY_GetMemberDataConst( pty, idx );
}


//============================================================================================
/**
 *  1個前に繰り出した技をワークに格納しておく
 */
//============================================================================================
static  void  waza_no_stock( TR_AI_WORK* taw )
{
  int i;
  const BTL_POKEPARAM*  bpp = get_bpp( taw, taw->def_btl_poke_pos );

  for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
  {
    if( taw->use_waza[ taw->def_btl_poke_pos ][ i ] == BPP_GetPrevWazaID( bpp ) )
    {
      break;
    }
    if( taw->use_waza[ taw->def_btl_poke_pos ][ i ] == 0 )
    {
      taw->use_waza[ taw->def_btl_poke_pos ][ i ] = BPP_GetPrevWazaID( bpp );
      break;
    }
  }
}

//============================================================================================
/**
 *  技のパラメータを読み出し
 */
//============================================================================================
static  int   get_waza_param( TR_AI_WORK* taw, WazaID waza_no, WazaDataParam param )
{
  int i;

  for( i = 0 ; i < TR_AI_WAZATBL_MAX ; i++ )
  {
    if( taw->wazaID[ i ] == 0 )
    {
      taw->wazaID[ i ] = waza_no;
      GFL_ARC_LoadDataByHandle( taw->waza_handle, waza_no, taw->wd[ i ] );
      break;
    }
    if( taw->wazaID[ i ] == waza_no )
    {
      if( i != 0 )
      {
        WAZA_DATA*  wd;
        WazaID      waza_no_temp;

        wd = taw->wd[ i - 1 ];
        waza_no_temp = taw->wazaID[ i - 1 ];
        taw->wd[ i - 1 ] =  taw->wd[ i ];
        taw->wazaID[ i - 1 ] = taw->wazaID[ i ];
        taw->wd[ i ] =  wd;
        taw->wazaID[ i ] = waza_no_temp;
      }
      break;
    }
  }
  if( i == TR_AI_WAZATBL_MAX )
  {
    i--;
    taw->wazaID[ i ] = waza_no;
    GFL_ARC_LoadDataByHandle( taw->waza_handle, waza_no, taw->wd[ i ] );
  }
  return WAZADATA_PTR_GetParam( taw->wd[ i ], param );
}

