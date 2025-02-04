//=============================================================================================
/**
 * @file  btl_string.c
 * @brief ポケモンWB バトル 文字列生成処理
 * @author  taya
 *
 * @date  2008.10.06  作成
 */
//=============================================================================================
#include <gflib.h>

#include "print\wordset.h"
#include "waza_tool\wazadata.h"

#include "btl_common.h"
#include "btl_string.h"
#include "btl_main.h"

#include "arc_def.h"
#include "message.naix"
#include "msg\msg_btl_std.h"
#include "msg\msg_btl_set.h"
#include "msg\msg_btl_attack.h"
#include "msg\msg_btl_ui.h"
#include "msg\msg_wazaname.h"

#ifdef PM_DEBUG
#include "script_message.naix"
#include "msg\script\msg_n01r0502.h"
#endif

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/

enum {
  TMP_STRBUF_SIZE = 256,
  MSG_ARG_MAX = 8,
};

//------------------------------------------------------
/**
 *  WORDSETバッファ用途
 */
//------------------------------------------------------
typedef enum {

  BUFIDX_POKE_1ST = 0,
  BUFIDX_POKE_2ND,
  BUFIDX_TOKUSEI,

}WordBufID;

//------------------------------------------------------
/**
 *  同内容メッセージセットの分類
 */
//------------------------------------------------------
typedef enum {

  SETTYPE_MINE = 0, ///< 自分のポケ
  SETTYPE_WILD,     ///< 野生のポケ
  SETTYPE_ENEMY,    ///< 相手のポケ

  SETTYPE_MAX,

}SetStrFormat;

//------------------------------------------------------
/**
 *  文字列リソースID
 */
//------------------------------------------------------
typedef enum {
  MSGSRC_STD = 0,
  MSGSRC_SET,
  MSGSRC_ATK,
  MSGSRC_UI,
  MSGSRC_TOKUSEI,
  MSGSRC_WAZAOBOE,
  MSGSRC_YESNO,
  MSGSRC_SUBWAY,

  MSGDATA_MAX,
}MsgSrcID;


//------------------------------------------------------------------------------
/**
 *
 *  システムワーク
 *
 */
//------------------------------------------------------------------------------
static struct {

  const BTL_MAIN_MODULE*  mainModule;   ///< メインモジュール
  const BTL_POKE_CONTAINER* pokeCon;    ///< ポケモンコンテナ
  WORDSET*          wset;               ///< WORDSET
  STRBUF*           tmpBuf;             ///< 文字列一時展開用バッファ
  GFL_MSGDATA*      msg[ MSGDATA_MAX ]; ///< メッセージデータハンドル
  int               args[ BTL_STR_ARG_MAX ];  ///< 引数保存配列

  HEAPID          heapID;               ///< ヒープID
  u32             clientID;             ///< UIクライアントID
  u16             fIgnoreFormat;        ///< 対象による文字列フォーマット変更をオフ（常に自分用=接頭詞なし）
  u16             fForceTrueName;       ///< ポケモンニックネームタグで強制的に正体名を利用（外部gmm対応）

}SysWork;


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void register_PokeNickname( u8 pokeID, WordBufID bufID );
static void register_PokeNicknameTruth( u8 pokeID, WordBufID bufID );
static void register_PokeName( u8 pokeID, u8 bufID );
static void register_TrainerType( WORDSET* wset, u8 bufIdx, u8 clientID );
static void register_TrainerName( WORDSET* wset, u8 bufIdx, u8 clientID );
static void ms_std_simple( STRBUF* dst, BtlStrID_STD strID, const int* args );
static inline SetStrFormat get_strFormat( u8 pokeID );
static inline u16 get_setStrID( u8 pokeID, u16 defaultStrID );
static inline u16 get_setStrID_Poke2( u8 pokeID1, u8 pokeID2, u16 defaultStrID );
static inline u16 get_setPtnStrID( u8 pokeID, u16 originStrID, u8 ptnNum );
static const STRCODE* seekNextTag( const STRCODE* sp, PrintSysTagGroup* tagGroup, u16* tagIndex, u8* argIndex );
static u8 searchPokeTagCount( const STRBUF* buf );
static void registerWords( const STRBUF* buf, const int* args, WORDSET* wset );
static void ms_set_auto( STRBUF* dst, u16 strID, const int* args );
static void ms_set_default( STRBUF* dst, u16 strID, const int* args );
static void ms_set_poke2( STRBUF* dst, u16 strID, const int* args );
static void ms_set_rankup_item( STRBUF* dst, u16 strID, const int* args );
static void ms_set_rankup( STRBUF* dst, u16 strID, const int* args );
static void ms_set_rankdown( STRBUF* dst, u16 strID, const int* args );
static void ms_set_rank_limit( STRBUF* dst, u16 strID, const int* args );
static void ms_set_useitem( STRBUF* dst, u16 strID, const int* args );
static void ms_set_poke2poke( STRBUF* dst, u16 strID, const int* args );
static void ms_wazaoboe_simple( STRBUF* dst, BtlStrID_WAZAOBOE strID, const int* args );
static void ms_yesno_simple( STRBUF* dst, BtlStrID_WAZAOBOE strID, const int* args );




//=============================================================================================
/**
 * 文字列生成システム初期化（バトル起動直後に１回だけ呼ぶ）
 *
 * @param   mainModule
 * @param   heapID
 *
 */
//=============================================================================================
void BTL_STR_InitSystem( const BTL_MAIN_MODULE* mainModule, u8 playerClientID, const BTL_POKE_CONTAINER* pokeCon, HEAPID heapID )
{
  static const u16 msgDataID[] = {
    NARC_message_btl_std_dat,
    NARC_message_btl_set_dat,
    NARC_message_btl_attack_dat,
    NARC_message_btl_ui_dat,
    NARC_message_tokusei_dat,
    NARC_message_waza_oboe_dat,
    NARC_message_yesnomenu_dat,
    NARC_message_tower_trainer_dat,
  };

  int i;

  SysWork.mainModule = mainModule;
  SysWork.pokeCon = pokeCon;
  SysWork.heapID = heapID;
  SysWork.clientID = playerClientID;
  SysWork.fIgnoreFormat = BTL_MAIN_GetSetupStatusFlag( mainModule, BTL_STATUS_FLAG_LEGEND_EX );
  SysWork.fForceTrueName = FALSE;

  SysWork.wset = WORDSET_Create( heapID );
  SysWork.tmpBuf = GFL_STR_CreateBuffer( TMP_STRBUF_SIZE, heapID );

  for(i=0; i<MSGDATA_MAX; i++)
  {
    SysWork.msg[i] = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, msgDataID[i], heapID );
  }
}

//=============================================================================================
/**
 * 文字列生成システム終了
 */
//=============================================================================================
void BTL_STR_QuitSystem( void )
{
  WORDSET_Delete( SysWork.wset );
  GFL_STR_DeleteBuffer( SysWork.tmpBuf );

  {
    int i;

    for(i=0; i<MSGDATA_MAX; i++)
    {
      GFL_MSG_Delete( SysWork.msg[i] );
    }
  }
}

//--------------------------------------------------------------------------------------

static  void register_PokeNickname( u8 pokeID, WordBufID bufID )
{
  const BTL_POKEPARAM* bpp;
  const POKEMON_PARAM* pp;

  bpp = BTL_POKECON_GetPokeParamConst( SysWork.pokeCon, pokeID );
  pp = BPP_GetViewSrcData( bpp );

  WORDSET_RegisterPokeNickName( SysWork.wset, bufID, pp );
}

static void register_PokeNicknameTruth( u8 pokeID, WordBufID bufID )
{
  const BTL_POKEPARAM* bpp;
  const POKEMON_PARAM* pp;

  bpp = BTL_POKECON_GetPokeParamConst( SysWork.pokeCon, pokeID );
  pp = BPP_GetSrcData( bpp );

  WORDSET_RegisterPokeNickName( SysWork.wset, bufID, pp );
}
static void register_PokeName( u8 pokeID, u8 bufID )
{
  const BTL_POKEPARAM* bpp;
  const POKEMON_PARAM* pp;

  bpp = BTL_POKECON_GetPokeParamConst( SysWork.pokeCon, pokeID );
  pp = BPP_GetViewSrcData( bpp );

  WORDSET_RegisterPokeMonsName( SysWork.wset, bufID, pp );
}

static void register_TrainerType( WORDSET* wset, u8 bufIdx, u8 clientID )
{
  u32 trType = BTL_MAIN_GetClientTrainerType( SysWork.mainModule, clientID );
  WORDSET_RegisterTrTypeName( wset, bufIdx, trType );
}
static void register_TrainerName( WORDSET* wset, u8 bufIdx, u8 clientID )
{
  if( clientID != BTL_CLIENTID_COMM_SUPPORT )
  {
    if( BTL_MAIN_IsClientNPC(SysWork.mainModule, clientID) )
    {
      if( BTL_MAIN_GetCompetitor(SysWork.mainModule) == BTL_COMPETITOR_TRAINER )
      {
        u32 trainerID = BTL_MAIN_GetClientTrainerID( SysWork.mainModule, clientID );
        if( trainerID != TRID_NULL ){
          WORDSET_RegisterTrainerName( wset, bufIdx, trainerID );
        }
      }
      else{
        const STRBUF* name;
        u32 tr_type;
        name = BTL_MAIN_GetClientTrainerName( SysWork.mainModule, clientID, &tr_type );
        WORDSET_RegisterWord( wset, bufIdx, name, TT_TrainerTypeSexGet(tr_type), TRUE, PM_LANG );
      }
    }
    else{
      const MYSTATUS* status = BTL_MAIN_GetClientPlayerData( SysWork.mainModule, clientID );
      WORDSET_RegisterPlayerName( wset, bufIdx, status );
    }
  }
  else
  {
    const MYSTATUS* status = BTL_MAIN_GetCommSuppoortPlayerData( SysWork.mainModule );
    WORDSET_RegisterPlayerName( wset, bufIdx, status );
  }
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

//=============================================================================================
/**
 * 標準メッセージの生成
 * ※標準メッセージ：対象者なし、あるいは対象が誰であっても一定のフォーマットで生成される文字列
 *
 * @param   buf       [out] 生成文字列格納バッファ
 * @param   strID     文字列ID
 * @param   numArgs   可変個引数の数
 * @param   ...       引数（可変）
 *
 */
//=============================================================================================
void BTL_STR_MakeStringStd( STRBUF* buf, BtlStrID_STD strID, u32 numArgs, ... )
{
  GF_ASSERT(numArgs<=MSG_ARG_MAX);
  {
    va_list   list;
    u32 i;

    BTL_N_Printf( DBGSTR_STR_StdStrInfo, strID, numArgs);

    va_start( list, numArgs );
    for(i=0; i<numArgs; ++i)
    {
      SysWork.args[i] = va_arg( list, int );
      BTL_N_PrintfSimple( DBGSTR_STR_Arg, i, SysWork.args[i]);
    }
    va_end( list );

    // ゴミ除去ねんのため
    while( i < NELEMS(SysWork.args) ){
      SysWork.args[i++] = 0;
    }
    BTL_STR_MakeStringStdWithArgArray( buf, strID, SysWork.args );
  }
}
//=============================================================================================
/**
 * 標準メッセージの生成（引数配列渡し版）
 *
 * @param   buf
 * @param   strID
 * @param   args
 *
 */
//=============================================================================================
void BTL_STR_MakeStringStdWithArgArray( STRBUF* buf, BtlStrID_STD strID, const int* args )
{
  /**
   *  対象陣営に応じてIDを補正する文字列群
   */
  static const u16 sideConvStrID[] = {
    BTL_STRID_STD_Makibisi,
    BTL_STRID_STD_MakibisiOff,
    BTL_STRID_STD_Dokubisi,
    BTL_STRID_STD_DokubisiOff,
    BTL_STRID_STD_StealthRock,
    BTL_STRID_STD_StealthRockOff,
    BTL_STRID_STD_WideGuard,
    BTL_STRID_STD_FastGuard,
    BTL_STRID_STD_Kinchoukan,
    BTL_STRID_STD_SinpiNoMamori,
    BTL_STRID_STD_SinpiNoMamoriOff,
    BTL_STRID_STD_HikariNoKabe,
    BTL_STRID_STD_HikariNoKabeOff,
    BTL_STRID_STD_Reflector,
    BTL_STRID_STD_ReflectorOff,
    BTL_STRID_STD_SiroiKiri,
    BTL_STRID_STD_SiroiKiriOff,
    BTL_STRID_STD_Oikaze,
    BTL_STRID_STD_OikazeOff,
    BTL_STRID_STD_Omajinai,
    BTL_STRID_STD_OmajinaiOff,
    BTL_STRID_STD_Rainbow,
    BTL_STRID_STD_RainbowOff,
    BTL_STRID_STD_Burning,
    BTL_STRID_STD_BurningOff,
    BTL_STRID_STD_Moor,
    BTL_STRID_STD_MoorOff,
  };
  int i;

  BTL_N_Printf( DBGSTR_STR_StdStrID, strID);

  // 対象陣営による補正：対象陣営ID（BtlSide）は args[0] に入れておくこと
  for(i=0; i<NELEMS(sideConvStrID); ++i)
  {
    if( strID == sideConvStrID[i] )
    {
      if( !BTL_MAIN_IsPlayerSide(SysWork.mainModule, args[0]) ){
        ++strID;
      }
      break;
    }
  }

  ms_std_simple( buf, strID, args );
}

static void ms_std_simple( STRBUF* dst, BtlStrID_STD strID, const int* args )
{
  GFL_MSG_GetString( SysWork.msg[MSGSRC_STD], strID, SysWork.tmpBuf );

  registerWords( SysWork.tmpBuf, args, SysWork.wset );
  WORDSET_ExpandStr( SysWork.wset, dst, SysWork.tmpBuf );
}


//----------------------------------------------------------------------
//--------------------------------------------------------------------------
/**
 * ポケモンIDから、「自分の・野生の・相手の」いずれのパターンか判定
 *
 * @param   pokeID
 *
 * @retval  SetStrFormat
 */
//--------------------------------------------------------------------------
static inline SetStrFormat get_strFormat( u8 pokeID )
{
  if( SysWork.fIgnoreFormat == FALSE )
  {
    u8 targetClientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );

    if( BTL_MAIN_IsOpponentClientID(SysWork.mainModule, SysWork.clientID, targetClientID) )
    {
      if( BTL_MAIN_GetCompetitor(SysWork.mainModule) == BTL_COMPETITOR_WILD )
      {
        return SETTYPE_WILD;
      }
      else
      {
        return SETTYPE_ENEMY;
      }
    }
  }

  return SETTYPE_MINE;
}
/**
 *  セット文字列ID変換（自分の・野生の・相手の３パターン）
 */
static inline u16 get_setStrID( u8 pokeID, u16 defaultStrID )
{
  return defaultStrID + get_strFormat( pokeID );
}
/**
 *  セット文字列ID変換（自分→自分、自分→野生、野生→自分など、ポケ名２つを含む７パターン）
 */
static inline u16 get_setStrID_Poke2( u8 pokeID1, u8 pokeID2, u16 defaultStrID )
{
  SetStrFormat  fmt = get_strFormat( pokeID1 );
  switch( fmt ){
  case SETTYPE_MINE:
    return defaultStrID + get_strFormat( pokeID2 );
  case SETTYPE_WILD:
    defaultStrID += SETTYPE_MAX;
    break;
  case SETTYPE_ENEMY:
    defaultStrID += (SETTYPE_MAX + 2);
    break;
  }
  if( get_strFormat(pokeID2) == fmt ){
    defaultStrID++;
  }
  return defaultStrID;
}
/**
 *  セット文字列ID変換（パターン数指定）
 */
static inline u16 get_setPtnStrID( u8 pokeID, u16 originStrID, u8 ptnNum )
{
  return originStrID + (ptnNum * SETTYPE_MAX) + get_strFormat( pokeID );
}

//=============================================================================================
/**
 * セットメッセージの生成
 * ※セットメッセージ：自分側，敵側（やせいの），敵側（あいての）が一定の法則でセットになった文字列
 *
 * @param   buf
 * @param   strID
 * @param   args
 *
 */
//=============================================================================================
void BTL_STR_MakeStringSet( STRBUF* buf, BtlStrID_SET strID, const int* args )
{
  static const struct {
    u16   strID;
    void  (* func)( STRBUF*, u16, const int* );
  }funcTbl[] = {
    { BTL_STRID_SET_Rankup_ATK,           ms_set_rankup           },
    { BTL_STRID_SET_Rankdown_ATK,         ms_set_rankup           },
    { BTL_STRID_SET_RankupMax_ATK,        ms_set_rank_limit       },
    { BTL_STRID_SET_RankdownMin_ATK,      ms_set_rank_limit       },
    { BTL_STRID_SET_UseItem_Rankup_ATK,   ms_set_rankup_item      },
    { BTL_STRID_STD_UseItem_Self,         ms_set_useitem          },

    { BTL_STRID_SET_Tedasuke,             ms_set_default          },  // てだすけはポケモン名２体入り文字列だが標準（３セット）
    { BTL_STRID_SET_JikoAnji,             ms_set_default          },  // じこあんじも同様
    { BTL_STRID_SET_MeromeroAct,          ms_set_default          },  // メロメロ
    { BTL_STRID_SET_SideChange,           ms_set_default          },  // サイドチェンジ
    { BTL_STRID_SET_AffGood_2,            ms_set_default          },  // 効果バツグン２体同時
    { BTL_STRID_SET_AffGood_3,            ms_set_default          },  // 効果バツグン３体同時
    { BTL_STRID_SET_AffBad_2,             ms_set_default          },  // 効果イマイチ２体同時
    { BTL_STRID_SET_AffBad_3,             ms_set_default          },  // 効果イマイチ３体同時

  };

  int i;

  BTL_N_Printf(DBGSTR_STR_SetStrID, strID);

  for(i=0; i<NELEMS(funcTbl); i++)
  {
    if( funcTbl[i].strID == strID )
    {
      funcTbl[i].func( buf, strID, args );
      return;
    }
  }


  ms_set_auto( buf, strID, args );
}

//--------------------------------------------------------------
/**
 *  タグ種類（gmm）
 */
//--------------------------------------------------------------
typedef enum {
  TAGIDX_TRAINER_NAME  = 0,
  TAGIDX_POKE_NAME     = 1,
  TAGIDX_POKE_NICKNAME = 2,
  TAGIDX_POKE_TYPE     = 3,
  TAGIDX_TOKUSEI_NAME  = 6,
  TAGIDX_WAZA_NAME     = 7,
  TAGIDX_ITEM_NAME     = 9,
  TAGIDX_POKE_NICKNAME_TRUTH = 12,
  TAGIDX_TRAINER_TYPE  = 14,
  TAGIDX_POKETYPE_NAME  = 15,
}TagIndexType;

static const STRCODE* seekNextTag( const STRCODE* sp, PrintSysTagGroup* tagGroup, u16* tagIndex, u8* argIndex )
{
  if( sp != NULL )
  {
    STRCODE EOMCode, TAGCode;

    EOMCode = GFL_STR_GetEOMCode();
    TAGCode = PRINTSYS_GetTagStartCode();
    while( (*sp) != EOMCode )
    {
        if( *sp == TAGCode )
        {
          if( PRINTSYS_IsWordSetTagGroup(sp) )
          {
            *argIndex = PRINTSYS_GetTagParam( sp, 0 );
            *tagGroup = PRINTSYS_GetTagGroup( sp );
            *tagIndex = PRINTSYS_GetTagIndex( sp );
            return PRINTSYS_SkipTag( sp );
          }
          else{
            sp = PRINTSYS_SkipTag( sp );
          }
        }
        else{
          ++sp;
        }
    }
    return NULL;
  }
  return NULL;
}

//----------------------------------------------------------------------------------
/**
 * 文字列中に含まれるポケモン名タグの数を返す
 *
 * @param   buf
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
static u8 searchPokeTagCount( const STRBUF* buf )
{
  const STRCODE* sp = GFL_STR_GetStringCodePointer( buf );
  PrintSysTagGroup tagGroup;
  u16 tagIdx;
  u8  argIdx;
  u8 count = 0;

  while( sp )
  {
    sp = seekNextTag( sp, &tagGroup, &tagIdx, &argIdx );
    if( sp )
    {
      if( tagGroup == PRINTSYS_TAGGROUP_WORD )
      {
        if( (tagIdx == TAGIDX_POKE_NAME)
        ||  (tagIdx == TAGIDX_POKE_NICKNAME)
        ||  (tagIdx == TAGIDX_POKE_NICKNAME_TRUTH)
        ){
          ++count;
        }
      }
    }
  }
  return count;
}

//----------------------------------------------------------------------------------
/**
 * 文字列中のタブIDから必要な情報を判別してWORDSETに登録する
 *
 * @param   buf
 * @param   args
 * @param   wset
 */
//----------------------------------------------------------------------------------
static void registerWords( const STRBUF* buf, const int* args, WORDSET* wset )
{
  const STRCODE* sp = GFL_STR_GetStringCodePointer( buf );
  STRCODE eomCode, tagCode;
  u8  argIdxDec = 0;
  u8  clientID = BTL_CLIENT_MAX;

  eomCode = GFL_STR_GetEOMCode();
  tagCode = PRINTSYS_GetTagStartCode();

  while( *sp != eomCode )
  {
    if( *sp == tagCode )
    {
      if( PRINTSYS_IsWordSetTagGroup(sp) )
      {
        u32 bufIdx = PRINTSYS_GetTagParam( sp, 0 );
        u32 argIdx = bufIdx;
        u8  tagGrp = PRINTSYS_GetTagGroup( sp );
        if( argIdx >= argIdxDec ){
          argIdx -= argIdxDec;
        }
        BTL_N_Printf( DBGSTR_PRINT_BufIdx, bufIdx);

        if( tagGrp == PRINTSYS_TAGGROUP_NUM )
        {
          u8 keta = PRINTSYS_GetTagIndex( sp ) + 1;
          BTL_N_Printf( DBGSTR_STR_TagSetNumber, keta, args[argIdx] );
          WORDSET_RegisterNumber( wset, bufIdx, args[argIdx], keta, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
        }
        else
        {
          u8 tagIdx = PRINTSYS_GetTagIndex( sp );
          switch( tagIdx ){
          case TAGIDX_TRAINER_TYPE:
            clientID = args[ argIdx ];
            register_TrainerType( wset, bufIdx, clientID );
            ++argIdxDec;
            break;
          case TAGIDX_TRAINER_NAME:
            if( clientID == BTL_CLIENT_MAX ){
              clientID = args[argIdx];
            }
            register_TrainerName( wset, bufIdx, clientID );
            clientID = BTL_CLIENT_MAX;
            break;
          case TAGIDX_POKE_NICKNAME:
            BTL_N_Printf( DBGSTR_STR_SetPokeNickname, args[argIdx]);
            if( SysWork.fForceTrueName == FALSE ){
              register_PokeNickname( args[argIdx], bufIdx );
            }else{
              register_PokeNicknameTruth( args[argIdx], bufIdx );
            }
            break;
          case TAGIDX_POKE_NICKNAME_TRUTH:
            BTL_N_Printf( DBGSTR_STR_SetPokeNicknameTruth, args[argIdx]);
            register_PokeNicknameTruth( args[argIdx], bufIdx );
            break;
          case TAGIDX_POKE_NAME:
            register_PokeName( args[argIdx], bufIdx );
            break;
          case TAGIDX_POKE_TYPE:
            WORDSET_RegisterPokeTypeName( wset, bufIdx, args[argIdx] );
            break;
          case TAGIDX_TOKUSEI_NAME:
            WORDSET_RegisterTokuseiName( wset, bufIdx, args[argIdx] );
            break;
          case TAGIDX_WAZA_NAME:
            BTL_N_Printf( DBGSTR_STR_SetTagWazaName, args[argIdx] );
            WORDSET_RegisterWazaName( wset, bufIdx, args[argIdx] );
            break;
          case TAGIDX_ITEM_NAME:
            BTL_N_Printf( DBGSTR_STR_SetItemName, argIdx, args[argIdx]);
            WORDSET_RegisterItemName( wset, bufIdx, args[argIdx] );
            break;
          case TAGIDX_POKETYPE_NAME:
            WORDSET_RegisterPokeTypeName( wset, bufIdx, args[argIdx] );
            break;
          default:
            GF_ASSERT_MSG(0, "unknown tagIdx = %d\n", tagIdx);
            break;
          }
        }
      }
      sp = PRINTSYS_SkipTag( sp );
    }
    else{
      ++sp;
    }
  }
}
//--------------------------------------------------------------
/**
 *  args[0]~[1] にポケモンIDが入っているのを自動判別
 */
//--------------------------------------------------------------
static void ms_set_auto( STRBUF* dst, u16 strID, const int* args )
{
  GFL_MSG_GetString( SysWork.msg[MSGSRC_SET], strID, SysWork.tmpBuf );

  if( searchPokeTagCount(SysWork.tmpBuf) == 2 ){
    ms_set_poke2( dst, strID, args );
  }else{
    ms_set_default( dst, strID, args );
  }
}
//--------------------------------------------------------------
/**
 *  標準処理（args[0] にポケモンIDが入っている）
 */
//--------------------------------------------------------------
static void ms_set_default( STRBUF* dst, u16 strID, const int* args )
{
  BTL_N_Printf( DBGSTR_STR_SetPoke1, strID, args[0], args[1]);

  strID = get_setStrID( args[0], strID );

  GFL_MSG_GetString( SysWork.msg[MSGSRC_SET], strID, SysWork.tmpBuf );
  registerWords( SysWork.tmpBuf, args, SysWork.wset );
  WORDSET_ExpandStr( SysWork.wset, dst, SysWork.tmpBuf );
}
//--------------------------------------------------------------
/**
 *  標準処理（args[0], args[1] にポケモンIDが入っている）
 */
//--------------------------------------------------------------
static void ms_set_poke2( STRBUF* dst, u16 strID, const int* args )
{
  BTL_N_Printf( DBGSTR_STR_SetPoke2, strID, args[0], args[1]);

  strID = get_setStrID_Poke2( args[0], args[1], strID );

  GFL_MSG_GetString( SysWork.msg[MSGSRC_SET], strID, SysWork.tmpBuf );
  registerWords( SysWork.tmpBuf, args, SysWork.wset );
  WORDSET_ExpandStr( SysWork.wset, dst, SysWork.tmpBuf );
}
//--------------------------------------------------------------
/**
 *  ○○は△△で××が（ぐーんと）あがった！
 *  args... [0]:pokeID,  [1]:itemID,  [2]:statusType,  [3]:volume
 */
//--------------------------------------------------------------
static void ms_set_rankup_item( STRBUF* dst, u16 strID, const int* args )
{
  u8 statusType = args[2] - WAZA_RANKEFF_ORIGIN;

  u16 leap = 0;
  if( args[3] >= 3 ){ leap = 2; }
  if( args[3] == 2 ){ leap = 1; }

  strID += (leap * SETTYPE_MAX * WAZA_RANKEFF_NUMS);
  strID = get_setPtnStrID( args[0], strID, statusType );

  register_PokeNickname( args[0], BUFIDX_POKE_1ST );
  WORDSET_RegisterItemName( SysWork.wset, 1, args[1] );

  GFL_MSG_GetString( SysWork.msg[MSGSRC_SET], strID, SysWork.tmpBuf );
  WORDSET_ExpandStr( SysWork.wset, dst, SysWork.tmpBuf );
}

//--------------------------------------------------------------
/**
 *  ○○の××が（ぐーんと）あがった！
 *  args... [0]:pokeID,  [1]:statusType, [2]:volume
 */
//--------------------------------------------------------------
static void ms_set_rankup( STRBUF* dst, u16 strID, const int* args )
{
  u8 statusType = args[1] - WAZA_RANKEFF_ORIGIN;
  u16 leap = 0;
  if( args[2] >= 3 ){ leap = 2; }
  if( args[2] == 2 ){ leap = 1; }

  strID += (leap * SETTYPE_MAX * WAZA_RANKEFF_NUMS);

  register_PokeNickname( args[0], BUFIDX_POKE_1ST );
  strID = get_setPtnStrID( args[0], strID, statusType );
  GFL_MSG_GetString( SysWork.msg[MSGSRC_SET], strID, SysWork.tmpBuf );
  WORDSET_ExpandStr( SysWork.wset, dst, SysWork.tmpBuf );
}

//--------------------------------------------------------------
/**
 *  ○○の××が（がくっと）さがった！
 *  args... [0]:pokeID,  [1]:statusType, [2]:volume
 */
//--------------------------------------------------------------
static void ms_set_rankdown( STRBUF* dst, u16 strID, const int* args )
{
  u8 statusType = args[1] - WAZA_RANKEFF_ORIGIN;
  if( args[2] > 1 )
  {
    strID += (SETTYPE_MAX * WAZA_RANKEFF_NUMS);
  }
  register_PokeNickname( args[0], BUFIDX_POKE_1ST );
  strID = get_setPtnStrID( args[0], strID, statusType );
  GFL_MSG_GetString( SysWork.msg[MSGSRC_SET], strID, SysWork.tmpBuf );
  WORDSET_ExpandStr( SysWork.wset, dst, SysWork.tmpBuf );
}
//--------------------------------------------------------------
/**
 *  ○○の××は　もうあがらない（さがらない）！
 *  args... [0]:pokeID,  [1]:statusType
 */
//--------------------------------------------------------------
static void ms_set_rank_limit( STRBUF* dst, u16 strID, const int* args )
{
  u8 statusType = args[1] - WAZA_RANKEFF_ORIGIN;

  register_PokeNickname( args[0], BUFIDX_POKE_1ST );
  strID = get_setPtnStrID( args[0], strID, statusType );
  GFL_MSG_GetString( SysWork.msg[MSGSRC_SET], strID, SysWork.tmpBuf );
  WORDSET_ExpandStr( SysWork.wset, dst, SysWork.tmpBuf );
}
static void ms_set_useitem( STRBUF* dst, u16 strID, const int* args )
{
  u8 clientID = args[0];

  if( BTL_MAIN_GetPlayerClientID(SysWork.mainModule) != clientID )
  {
    if( BTL_MAIN_IsClientNPC(SysWork.mainModule, clientID) ){
      strID = BTL_STRID_STD_UseItem_NPC;
    }else{
      strID = BTL_STRID_STD_UseItem_Player;
    }
  }
  else{
    strID = BTL_STRID_STD_UseItem_Self;
  }

  GFL_MSG_GetString( SysWork.msg[MSGSRC_STD], strID, SysWork.tmpBuf );
  registerWords( SysWork.tmpBuf, args, SysWork.wset );
  WORDSET_ExpandStr( SysWork.wset, dst, SysWork.tmpBuf );
}

//--------------------------------------------------------------
/**
 *  ○○は××にねらいをさだめた！　等（野生→野生など含む７パターン）
 *  args... [0]:pokeID,  [1]:targetPokeID
 */
//--------------------------------------------------------------
static void ms_set_poke2poke( STRBUF* dst, u16 strID, const int* args )
{
  strID = get_setStrID_Poke2( args[0], args[1], strID );
  GFL_MSG_GetString( SysWork.msg[MSGSRC_SET], strID, SysWork.tmpBuf );
  registerWords( SysWork.tmpBuf, args, SysWork.wset );
  WORDSET_ExpandStr( SysWork.wset, dst, SysWork.tmpBuf );

}

//=============================================================================================
/**
 * ワザメッセージの生成
 * ※ワザメッセージ：○○の××こうげき！とか。セットメッセージと同様、必ず３つセット。
 *
 * @param   buf
 * @param   strID
 * @param   args
 *
 */
//=============================================================================================
void BTL_STR_MakeStringWaza( STRBUF* dst, u8 pokeID, u16 waza )
{
  u16 strID = get_setStrID( pokeID, waza * SETTYPE_MAX );;

  register_PokeNickname( pokeID, BUFIDX_POKE_1ST );
  GFL_MSG_GetString( SysWork.msg[MSGSRC_ATK], strID, SysWork.tmpBuf );
  WORDSET_ExpandStr( SysWork.wset, dst, SysWork.tmpBuf );
}

//=============================================================================================
/**
 * ROM内サブウェイトレーナーの勝敗メッセージを生成
 *
 * @param   dst
 * @param   subwayTRID
 * @param   playerResult
 */
//=============================================================================================
void BTL_STR_MakeStringSubway( STRBUF* dst, u16 subwayTRID, BtlResult playerResult )
{
  if( subwayTRID )
  {
    u16 strID = (subwayTRID - 1) * 3;

    if( playerResult == BTL_RESULT_LOSE ){
      strID += 1;
    }else{
      strID += 2;
    }

    GFL_MSG_GetString( SysWork.msg[MSGSRC_SUBWAY], strID, dst );
  }
}



//=============================================================================================
/**
 *
 *
 * @param   dst
 * @param   strID
 *
 */
//=============================================================================================
void BTL_STR_GetUIString( STRBUF* dst, u16 strID )
{
  GFL_MSG_GetString( SysWork.msg[MSGSRC_UI], strID, dst );
}

//--------------------------------------------------------------------

void BTL_STR_MakeWazaUIString( STRBUF* dst, u16 wazaID, u8 wazaPP, u8 wazaPPMax )
{
  WORDSET_RegisterWazaName( SysWork.wset, 0, wazaID );
  WORDSET_RegisterNumber( SysWork.wset, 1, wazaPP, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( SysWork.wset, 2, wazaPPMax, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );

  GFL_MSG_GetString( SysWork.msg[MSGSRC_UI], BTLMSG_UI_SEL_WAZA, SysWork.tmpBuf );
  WORDSET_ExpandStr( SysWork.wset, dst, SysWork.tmpBuf );
}

//=============================================================================================
/**
 * 技覚えメッセージの生成（引数配列渡し版）
 *
 * @param   buf
 * @param   strID
 * @param   args
 *
 */
//=============================================================================================
void BTL_STR_MakeStringWazaOboeWithArgArray( STRBUF* buf, BtlStrID_WAZAOBOE strID, const int* args )
{
  ms_wazaoboe_simple( buf, strID, args );
}

static void ms_wazaoboe_simple( STRBUF* dst, BtlStrID_WAZAOBOE strID, const int* args )
{
  GFL_MSG_GetString( SysWork.msg[MSGSRC_WAZAOBOE], strID, SysWork.tmpBuf );
  SysWork.fForceTrueName = TRUE;
  registerWords( SysWork.tmpBuf, args, SysWork.wset );
  SysWork.fForceTrueName = FALSE;
  WORDSET_ExpandStr( SysWork.wset, dst, SysWork.tmpBuf );
}

//=============================================================================================
/**
 * 技覚えメッセージの生成（引数配列渡し版）
 *
 * @param   buf
 * @param   strID
 * @param   args
 *
 */
//=============================================================================================
void BTL_STR_MakeStringYesNoWithArgArray( STRBUF* buf, BtlStrID_WAZAOBOE strID, const int* args )
{
  ms_yesno_simple( buf, strID, args );
}

static void ms_yesno_simple( STRBUF* dst, BtlStrID_WAZAOBOE strID, const int* args )
{
  GFL_MSG_GetString( SysWork.msg[MSGSRC_YESNO], strID, SysWork.tmpBuf );
  registerWords( SysWork.tmpBuf, args, SysWork.wset );
  WORDSET_ExpandStr( SysWork.wset, dst, SysWork.tmpBuf );
}



//=============================================================================================
/**
 * レベルアップウィンドウテキスト（前レベルとの差分）生成
 *
 * @param   buf
 * @param   hp
 * @param   atk
 * @param   def
 * @param   sp_atk
 * @param   sp_def
 * @param   agi
 */
//=============================================================================================
void BTL_STR_MakeString_LvupInfo_Diff( STRBUF* dst, u32 hp, u32 atk, u32 def, u32 sp_atk, u32 sp_def, u32 agi )
{
  GFL_MSG_GetString( SysWork.msg[MSGSRC_UI], BTLSTR_UI_LVUP_DIFF, SysWork.tmpBuf );

  WORDSET_RegisterNumber( SysWork.wset, 0, hp,     2,  STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( SysWork.wset, 1, atk,    2,  STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( SysWork.wset, 2, def,    2,  STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( SysWork.wset, 3, sp_atk, 2,  STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( SysWork.wset, 4, sp_def, 2,  STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( SysWork.wset, 5, agi,    2,  STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );

  WORDSET_ExpandStr( SysWork.wset, dst, SysWork.tmpBuf );
}
//=============================================================================================
/**
 * レベルアップウィンドウテキスト（新レベルパラメータ）生成
 *
 * @param   dst
 * @param   hp
 * @param   atk
 * @param   def
 * @param   sp_atk
 * @param   sp_def
 * @param   agi
 */
//=============================================================================================
void BTL_STR_MakeString_LvupInfo_Param( STRBUF* dst, u32 hp, u32 atk, u32 def, u32 sp_atk, u32 sp_def, u32 agi )
{
  GFL_MSG_GetString( SysWork.msg[MSGSRC_UI], BTLSTR_UI_LVUP_PARAM, SysWork.tmpBuf );

  WORDSET_RegisterNumber( SysWork.wset, 0, hp,     3,  STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( SysWork.wset, 1, atk,    3,  STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( SysWork.wset, 2, def,    3,  STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( SysWork.wset, 3, sp_atk, 3,  STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( SysWork.wset, 4, sp_def, 3,  STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( SysWork.wset, 5, agi,    3,  STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );

  WORDSET_ExpandStr( SysWork.wset, dst, SysWork.tmpBuf );
}





