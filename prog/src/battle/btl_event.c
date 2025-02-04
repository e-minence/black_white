//=============================================================================================
/**
 * @file  btl_event.c
 * @brief ポケモンWB バトルシステム イベント＆ハンドラ処理
 * @author  taya
 *
 * @date  2008.10.23  作成
 */
//=============================================================================================

#include "poke_tool\poketype.h"

#include "btl_common.h"
#include "btl_pokeparam.h"
#include "btl_sideeff.h"
#include "btl_field.h"

#include "handler\hand_waza.h"

#include "btl_event.h"
#include "btl_event_factor.h"




/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  FACTOR_PER_POKE = EVENT_WAZA_STICK_MAX + 3,            ///< ワザ貼り付き最大数+使用ワザ+とくせい+アイテム = 11
  FACTOR_MAX_FOR_POKE = FACTOR_PER_POKE * BTL_POS_MAX,   ///< ポケモンごと最大数(11) * 場所数(6) = 66
  FACTOR_MAX_FOR_SIDEEFF = BTL_SIDEEFF_MAX * 2,          ///< サイドエフェクト最大数(13)×陣営数(2) = 26
  FACTOR_MAX_FOR_POSEFF = BTL_POSEFF_MAX * BTL_POS_MAX,  ///< 位置エフェクト最大数(5) * 場所数(6) = 30
  FACTOR_MAX_FOR_FIELD = BTL_FLDEFF_MAX,                 ///< フィールドエフェクト最大数 = 6

  // 登録できるイベントファクター最大数（66 + 26 + 30 + 6) = 128
  FACTOR_REGISTER_MAX = FACTOR_MAX_FOR_POKE + FACTOR_MAX_FOR_SIDEEFF + FACTOR_MAX_FOR_FIELD + FACTOR_MAX_FOR_POSEFF,
  EVENTVAL_STACK_DEPTH = 96,   ///< イベント変数スタックの容量

  MAINPRI_BIT_WIDTH = 8,
  SUBPRI_BIT_WIDTH  = 24,
  MAINPRI_MAX =  ((1 << MAINPRI_BIT_WIDTH) - 1),
  SUBPRI_MAX  =  ((1 << SUBPRI_BIT_WIDTH) - 1),
  MAINPRI_MASK = MAINPRI_MAX,
  SUBPRI_MASK = SUBPRI_MAX,

};

/**
 * 値の書き換え可否状態
 */

typedef enum {

  REWRITE_FREE = 0,   ///< 自由に可能
  REWRITE_ONCE,       ///< １回だけ可能
  REWRITE_END,        ///< １回書き換えたのでもう終了
  REWRITE_MUL,        ///< 乗算のみ可能
  REWRITE_LOCK,       ///< 不可

}RewriteState;

/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/

/**
* イベントデータ実体
*/
struct _BTL_EVENT_FACTOR {
  BTL_EVENT_FACTOR* prev;
  BTL_EVENT_FACTOR* next;
  const BtlEventHandlerTable* handlerTable;
  BtlEventSkipCheckHandler  skipCheckHandler;
  BtlEventFactorType  factorType;
  u32       priority;               ///< ソートプライオリティ

  u32       currentStackPtr   : 16;  ///< 登録時イベントスタックポインタ
  u32       numHandlers       :  8;  ///< ハンドラテーブル要素数
  u32       callingFlag       :  1;  ///< 呼び出し中を再呼び出ししないためのフラグ
  u32       sleepFlag         :  1;  ///< 休眠フラグ
  u32       tmpItemFlag       :  1;  ///< アイテム用一時利用フラグ
  u32       rmReserveFlag     :  1;  ///< 削除予約フラグ
  u32       recallEnableFlag  :  1;  ///< 再帰呼び出し許可
  u32       existFlag         :  1;  ///< 現在処理中イベントによりAddされた
  u32       rotationSleepFlag :  1;  ///< 現在処理中イベントによりAddされた
  u32       _padd             :  1;
  int       work[ EVENT_HANDLER_WORK_ELEMS ];
  u16       subID;      ///< イベント実体ID。ワザならワザID, とくせいならとくせいIDなど
  u8        dependID;   ///< 依存対象物ID。ワザ・とくせい・アイテムならポケID、場所依存なら場所idなど。
  u8        pokeID;     ///< ポケ依存イベントならポケID／それ以外BTL_POKEID_NULL

};

/*--------------------------------------------------------------------------*/
/* Globals                                                                  */
/*--------------------------------------------------------------------------*/
static BTL_EVENT_FACTOR  Factors[ FACTOR_REGISTER_MAX ];
static BTL_EVENT_FACTOR* FactorStack[ FACTOR_REGISTER_MAX ];
static BTL_EVENT_FACTOR* FirstFactorPtr;
static u32 StackPtr;
static u32 EventStackPtr;

/**
* イベント変数スタック
*/
typedef struct {
  u32   sp;                                 ///< スタックポインタ
  u16   label[ EVENTVAL_STACK_DEPTH ];      ///< 変数ラベル
  int   value[ EVENTVAL_STACK_DEPTH ];      ///< 変数
  fx32  mulMax[ EVENTVAL_STACK_DEPTH ];     ///< 乗算対応実数の最大値
  fx32  mulMin[ EVENTVAL_STACK_DEPTH ];     ///< 乗算対応実数の最小値
  u8    rewriteState[ EVENTVAL_STACK_DEPTH ];///< 書き換え可否状態
}VAR_STACK;

static VAR_STACK VarStack = {0};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static inline BOOL isDependPokeFactorType( BtlEventFactorType factorType );
static void printLinkDebug( void );
static void CallHandlersSub( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID, BtlEventFactorType type, BOOL fSkipCheck );
static void CallHandlersCore( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID, BtlEventFactorType targetType, BOOL fSkipCheck );
static BOOL check_handler_skip( BTL_SVFLOW_WORK* flowWork, BTL_EVENT_FACTOR* factor, BtlEventType eventID );
static BTL_EVENT_FACTOR* pushFactor( void );
static void popFactor( BTL_EVENT_FACTOR* factor );
static void clearFactorWork( BTL_EVENT_FACTOR* factor );
static inline u32 calcFactorPriority( BtlEventPriority mainPri, u32 subPri );
static inline u16 getFactorMainPriority( u32 pri );
static void varStack_Init( void );
static int evar_getNewPoint( const VAR_STACK* stack, BtlEvVarLabel label );
static int evar_getExistPoint( const VAR_STACK* stack, BtlEvVarLabel label );
static fx32 evar_mulValueRound( const VAR_STACK* stack, int ptr, fx32 value );


//=============================================================================================
/**
 * システム初期化 （バトル開始時、サーバマシンから１回呼び出す）
 */
//=============================================================================================
void BTL_EVENT_InitSystem( void )
{
  int i;

  for(i=0; i<NELEMS(Factors); i++)
  {
    clearFactorWork( &Factors[i] );
    FactorStack[i] = &Factors[i];
  }

  FirstFactorPtr = NULL;
  StackPtr = 0;
  EventStackPtr = 0;

  varStack_Init();
}


//=============================================================================================
/**
 * ターンごとの初期化処理
 */
//=============================================================================================
void BTL_EVENT_StartTurn( void )
{
  // スリープフラグを落とす
  BTL_EVENT_FACTOR* factor;

  for(factor=FirstFactorPtr; factor!=NULL; factor=factor->next)
  {
    factor->sleepFlag = FALSE;
  }
}

/**
 *  ファクタータイプがポケ依存するタイプか判定
 */
static inline BOOL isDependPokeFactorType( BtlEventFactorType factorType )
{
  if( (factorType == BTL_EVENT_FACTOR_WAZA)
  ||  (factorType == BTL_EVENT_FACTOR_TOKUSEI)
  ||  (factorType == BTL_EVENT_FACTOR_ITEM)
  ){
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------------
/**
 * リンクリスト情報をPrint（デバッグ用）
 */
//----------------------------------------------------------------------------------
static void printLinkDebug( void )
{
#if 0
  BTL_EVENT_FACTOR* fp;
  u32 cnt = 0;

  BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EV_LinkInfoHeader );
  for(fp=FirstFactorPtr; fp!=NULL; fp=fp->next)
  {
    BTL_N_PrintfSimple( PRINT_CHANNEL_EVENTSYS, DBGSTR_EV_LinkPtr, fp );
    ++cnt;
    if( cnt % 4 == 0 ){
      BTL_N_PrintfSimpleEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_LF );
    }
  }
  if( !cnt ){
    BTL_N_PrintfSimpleEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EV_LinkEmpty );
  }else if( cnt % 4 ){
    BTL_N_PrintfSimpleEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_LF );
  }
  BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EV_LinkInfoFooder );
#endif
}

//=============================================================================================
/**
 * イベント反応要素を追加
 *
 * @param   factorType      ファクタータイプ
 * @param   subID           タイプごとの個別ID（とくせいID，アイテムID等と一致）
 * @param   priority        プライオリティ ( = 数値が高いほど先に呼び出される )
 * @param   pokeID
 * @param   handlerTable
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
BTL_EVENT_FACTOR* BTL_EVENT_AddFactor( BtlEventFactorType factorType, u16 subID,
  BtlEventPriority mainPri, u32 subPri, u8 dependID,
  const BtlEventHandlerTable* handlerTable, u16 numHandlers )
{
  GF_ASSERT(mainPri < MAINPRI_MAX);
  GF_ASSERT(subPri < SUBPRI_MAX);

  {
    BTL_EVENT_FACTOR* newFactor;

    newFactor = pushFactor();
    if( newFactor )
    {
      newFactor->priority = calcFactorPriority( mainPri, subPri );

      BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_AddFactorInfo, dependID, factorType, newFactor->priority, newFactor );

      newFactor->factorType = factorType;
      newFactor->prev = NULL;
      newFactor->next = NULL;
      newFactor->handlerTable = handlerTable;
      newFactor->numHandlers = numHandlers;

      newFactor->subID = subID;
      newFactor->callingFlag = FALSE;
      newFactor->sleepFlag = FALSE;
      newFactor->rotationSleepFlag = FALSE;
      newFactor->tmpItemFlag = FALSE;
      newFactor->skipCheckHandler = NULL;
      newFactor->dependID = dependID;
      newFactor->rmReserveFlag = FALSE;
      newFactor->recallEnableFlag = FALSE;
      newFactor->currentStackPtr = EventStackPtr;
      newFactor->existFlag = TRUE;
      if( isDependPokeFactorType(factorType) ){
        newFactor->pokeID = dependID;
      }else{
        newFactor->pokeID = BTL_POKEID_NULL;
      }

      GFL_STD_MemClear( newFactor->work, sizeof(newFactor->work) );

  //    BTL_Printf("イベント登録 依存ポケID=%d\n", newFactor->pokeID);

      // 最初の登録
      if( FirstFactorPtr == NULL )
      {
        FirstFactorPtr = newFactor;
        BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_AddFactor_Case0 );
      }
      // 現在先頭より高プライオリティ
      else if( newFactor->priority > FirstFactorPtr->priority )
      {
        FirstFactorPtr->prev = newFactor;
        newFactor->next = FirstFactorPtr;
        newFactor->prev = NULL;
        FirstFactorPtr = newFactor;
        BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_AddFactor_Case1,
          FirstFactorPtr->next->dependID, FirstFactorPtr->next->factorType, FirstFactorPtr->next );
      }
      // それ以外はふつうのリンクリスト接続
      else
      {
        BTL_EVENT_FACTOR *find, *last;

        last = FirstFactorPtr;
        for( find=FirstFactorPtr->next; find!=NULL; find=find->next )
        {
          if( newFactor->priority > find->priority )
          {
            newFactor->next = find;
            newFactor->prev = find->prev;
            find->prev->next = newFactor;
            find->prev = newFactor;
            BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_AddFactor_Case2,
                   newFactor->prev->dependID, newFactor->prev->factorType, newFactor->prev,
                   newFactor->dependID, newFactor->factorType, newFactor,
                   find->dependID, find->factorType, find );
            break;
          }
          last = find;
        }
        // 最後まで見つからない場合
        if( find == NULL )
        {
          last->next = newFactor;
          newFactor->prev = last;
          BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_AddFactor_Case3,
                   last->dependID, last->factorType, last,
                   newFactor->dependID, newFactor->factorType, newFactor,
                   newFactor->next, FirstFactorPtr
                 );
        }
      }
      BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EV_AddFactor,
          newFactor->dependID, newFactor->factorType, newFactor, newFactor->priority );
      printLinkDebug();
      return newFactor;
    }
    // スタックから見つからない
    else
    {
      GF_ASSERT(0);
      return NULL;
    }
  }
}

//=============================================================================================
/**
 * 分離ハンドラを全削除
 *
 * @param   none
 */
//=============================================================================================
void BTL_EVENT_RemoveIsolateFactors( void )
{
  BTL_EVENT_FACTOR *factor, *next;

  factor = FirstFactorPtr;
  while( factor != NULL )
  {
    next = factor->next;
    if( factor->factorType == BTL_EVENT_FACTOR_ISOLATE )
    {
      BTL_EVENT_FACTOR_Remove( factor );
    }
    factor = next;
  }
}
//=============================================================================================
/**
 * イベント反応要素を削除
 *
 * @param   factor
 *
 */
//=============================================================================================
void BTL_EVENT_FACTOR_Remove( BTL_EVENT_FACTOR* factor )
{
  if( (factor!=NULL) && (factor->existFlag) )
  {
    if( factor->callingFlag )
    {
      BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EV_DelReserveFactor,
              factor->dependID, factor->factorType, factor );
      factor->rmReserveFlag = TRUE;
      return;
    }

    if( factor == FirstFactorPtr )
    {
      FirstFactorPtr = factor->next;
    }

    if( factor->prev )
    {
      factor->prev->next = factor->next;
    }

    if( factor->next )
    {
      factor->next->prev = factor->prev;
    }

    BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EV_DelFactor, factor->dependID, factor->factorType, factor,
            factor->prev, factor->next );
    printLinkDebug();
    popFactor( factor );
  }
}

//=============================================================================================
/**
 * 登録されている反応要素を、依存ポケモン素早さ順にソート（全く同じ値はランダムで入れ替わる）
 */
//=============================================================================================
void BTL_EVENT_ReOrderFactorsByPokeAgility( BTL_SVFLOW_WORK* flowWork )
{
  BTL_EVENT_FACTOR *fp1, *fp2;
  u32 factorCount = 0;

  // デバッグ出力
  TAYA_Printf( " -------- Before Sort --------\n");
  for( fp1=FirstFactorPtr; fp1!=NULL; fp1=fp1->next )
  {
    TAYA_Printf( "DependID=%2d, Type=%d, Pri=%08x\n", fp1->dependID, fp1->factorType, fp1->priority );
  }


  // 全ポケ依存要素のプライオリティ再設定
  for( fp1=FirstFactorPtr; fp1!=NULL; fp1=fp1->next )
  {
    if( fp1->pokeID != BTL_POKEID_NULL )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWork, fp1->pokeID );
      u16 agility = BTL_SVFTOOL_CalcAgility( flowWork, bpp, TRUE );
      u16 mainPri = getFactorMainPriority( fp1->priority );
      fp1->priority = calcFactorPriority( mainPri, agility );
    }
    ++factorCount;
  }

  // デバッグ出力
  TAYA_Printf( " -------- Renew Priority --------\n");
  for( fp1=FirstFactorPtr; fp1!=NULL; fp1=fp1->next )
  {
    TAYA_Printf( "DependID=%2d, Type=%d, Pri=%08x\n", fp1->dependID, fp1->factorType, fp1->priority );
  }

  // プライオリティ順ソート
  if( factorCount > 1 )
  {
    BTL_EVENT_FACTOR *fp1_next = NULL;
    BOOL fIns;

    fp1 = FirstFactorPtr->next;
    while( fp1 != NULL )
    {
      fp1_next = fp1->next;
      for( fp2=FirstFactorPtr; fp2!=fp1; fp2=fp2->next )
      {
        fIns = FALSE;

        if( fp1->priority > fp2->priority )
        {
          fIns = TRUE;
          TAYA_Printf(" %d-%2d(Pri:%08x) > %d-%2d(Pri:%08x)\n",
                  fp1->factorType, fp1->dependID, fp1->priority,
                  fp2->factorType, fp2->dependID, fp2->priority);
        }
        else if( fp1->priority == fp2->priority ){
          fIns = BTL_CALC_GetRand( 2 )? TRUE : FALSE;
          TAYA_Printf(" %d-%2d(Pri:%08x) = %d-%2d(Pri:%08x) .. %d\n",
                  fp1->factorType, fp1->dependID, fp1->priority,
                  fp2->factorType, fp2->dependID, fp2->priority, fIns);
        }

        if( fIns )
        {
          if( fp1->prev ){
            fp1->prev->next = fp1->next;
          }
          if( fp1->next ){
            fp1->next->prev = fp1->prev;
          }

          fp1->prev = fp2->prev;
          fp1->next = fp2;

          if( fp2->prev ){
            fp2->prev->next = fp1;
          }
          fp2->prev = fp1;

          if( fp2 == FirstFactorPtr ){
            FirstFactorPtr = fp1;
          }

          {
            BTL_EVENT_FACTOR* fp;
            TAYA_Printf( " -------- つなぎ替え経過 --------\n");
            for( fp=FirstFactorPtr; fp!=NULL; fp=fp->next )
            {
              TAYA_Printf( "DependID=%2d, Type=%d, Pri=%08x\n", fp->dependID, fp->factorType, fp->priority );
            }
          }

          break;
        }
      }

      fp1 = fp1->next;
    }
  }

  // デバッグ出力
  TAYA_Printf( " -------- After Sort --------\n");
  for( fp1=FirstFactorPtr; fp1!=NULL; fp1=fp1->next )
  {
    TAYA_Printf( "DependID=%2d, Type=%d, Pri=%08x\n", fp1->dependID, fp1->factorType, fp1->priority );
  }
}

//=============================================================================================
/**
 * ファクターの種類を分離型に変換
 *
 * @param   factor
 */
//=============================================================================================
void BTL_EVENT_FACTOR_ConvertForIsolate( BTL_EVENT_FACTOR* factor )
{
  factor->factorType = BTL_EVENT_FACTOR_ISOLATE;
}
//=============================================================================================
/**
 * 分離型かどうか判定
 *
 * @param   factor
 */
//=============================================================================================
BOOL BTL_EVENT_FACTOR_IsIsolateMode( BTL_EVENT_FACTOR* factor )
{
  return ( factor->factorType == BTL_EVENT_FACTOR_ISOLATE );
}

//=============================================================================================
/**
 * 実体ID（ワザID、とくせいID，アイテムID等、要素タイプに準じる）を返す
 *
 * @param   factor
 *
 * @retval  u16
 */
//=============================================================================================
u16 BTL_EVENT_FACTOR_GetSubID( const BTL_EVENT_FACTOR* factor )
{
  return factor->subID;
}
//=============================================================================================
/**
 * 関連ポケモンIDを返す
 *
 * @param   factor
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_EVENT_FACTOR_GetPokeID( const BTL_EVENT_FACTOR* factor )
{
  return factor->pokeID;
}
//=============================================================================================
/**
 * ワーク内容を取得
 *
 * @param   factor
 * @param   workIdx
 *
 * @retval  int
 */
//=============================================================================================
int BTL_EVENT_FACTOR_GetWorkValue( const BTL_EVENT_FACTOR* factor, u8 workIdx )
{
  GF_ASSERT(workIdx < EVENT_HANDLER_WORK_ELEMS);
  return factor->work[ workIdx ];
}
//=============================================================================================
/**
 * アイテム用一時利用フラグをONにする（BTL_EVENT_USE_ITEM_TMP に反応するようになる）
 *
 * @param   factor
 */
//=============================================================================================
void BTL_EVENT_FACTOR_SetTmpItemFlag( BTL_EVENT_FACTOR* factor )
{
  factor->tmpItemFlag = TRUE;
}
//=============================================================================================
/**
 * 同一ハンドラの再帰呼び出し許可（アイテム用）
 *
 * @param   factor
 */
//=============================================================================================
void BTL_EVENT_FACTOR_SetRecallEnable( BTL_EVENT_FACTOR* factor )
{
  if( factor->callingFlag ){
    factor->recallEnableFlag = TRUE;
  }
}
//=============================================================================================
/**
 * ワーク内容を設定
 *
 * @param   factor
 * @param   workIdx
 * @param   value
 */
//=============================================================================================
void BTL_EVENT_FACTOR_SetWorkValue( BTL_EVENT_FACTOR* factor, u8 workIdx, int value )
{
  GF_ASSERT(workIdx < EVENT_HANDLER_WORK_ELEMS);
  factor->work[ workIdx ] = value;
}


//=============================================================================================
/**
 * 全登録要素の対し、指定イベントの強制通知（スキップ条件を見ない）
 *
 * @param   flowWork
 * @param   eventID
 */
//=============================================================================================
void BTL_EVENT_ForceCallHandlers( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID )
{
  CallHandlersSub( flowWork, eventID, BTL_EVENT_FACTOR_MAX, FALSE );
}
//=============================================================================================
/**
 * 全登録要素に対し、指定イベントの通知（スキップ条件をチェック：通常はこちらを呼ぶ）
 *
 * @param   flowWork
 * @param   eventID
 */
//=============================================================================================
void BTL_EVENT_CallHandlers( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID )
{
  CallHandlersSub( flowWork, eventID, BTL_EVENT_FACTOR_MAX, TRUE );
}
//=============================================================================================
/**
 * 全登録要素に対し、指定イベントの通知（スキップ条件をチェック＆特定タイプのハンドラのみ）
 *
 * @param   flowWork
 * @param   eventID
 * @param   type
 */
//=============================================================================================
void BTL_EVENT_CallHandlersTargetType( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID, BtlEventFactorType type )
{
  CallHandlersSub( flowWork, eventID, type, TRUE );
}

/**
 *  イベント通知共通下請け
 */
static void CallHandlersSub( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID, BtlEventFactorType type, BOOL fSkipCheck )
{
  ++EventStackPtr;
  CallHandlersCore( flowWork, eventID, type, fSkipCheck );
  --EventStackPtr;

  if( EventStackPtr == 0 )
  {
    BTL_EVENT_FACTOR* factor;
    for( factor=FirstFactorPtr; factor!=NULL; factor = factor->next )
    {
      factor->currentStackPtr = 0;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * 指定イベント通知コア
 *
 * @param   flowWork
 * @param   eventID
 * @param   fSkipCheck
 */
//----------------------------------------------------------------------------------
static void CallHandlersCore( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID, BtlEventFactorType targetType, BOOL fSkipCheck )
{
  BTL_EVENT_FACTOR* factor;
  BTL_EVENT_FACTOR* next_factor;


  for( factor=FirstFactorPtr; factor!=NULL; )
  {
    next_factor = factor->next;

    do {

      if( (factor->callingFlag) && (factor->recallEnableFlag == FALSE) ){
         BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_SkipByCallingFlg, factor->dependID, factor->factorType, factor );
         break;
      }
      if( factor->sleepFlag ){
        break;
      }
      if( factor->rotationSleepFlag ){
        break;
      }
      if( (targetType!=BTL_EVENT_FACTOR_MAX) && (factor->factorType != targetType) ){
        break;
      }
      if( (factor->currentStackPtr != 0) && (factor->currentStackPtr >= EventStackPtr) ){
       BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_SkipByStackLayer, factor->dependID, factor->factorType, factor );
        break;
      }
      if( factor->existFlag == FALSE ){
        break;
      }
      if( (eventID == BTL_EVENT_USE_ITEM_TMP) && (factor->tmpItemFlag == FALSE) ){
        break;
      }

      {
        const BtlEventHandlerTable* tbl = factor->handlerTable;
        u32 i;

//        BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_CheckFactorStart, factor->factorType, factor->dependID, factor->numHandlers, eventID );
        for(i=0; i<factor->numHandlers; i++)
        {
//          BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_CheckFactorTbl, i, tbl[i].eventType );
          if( tbl[i].eventType == eventID )
          {
            if( !fSkipCheck || !check_handler_skip(flowWork, factor, eventID) )
            {
              factor->callingFlag = TRUE;
              BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_CallFactorStart, factor, factor->dependID, factor->factorType );
              if( next_factor != NULL ){
                BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_NextFactorInfo, next_factor->dependID, next_factor->factorType, next_factor );
              }
              tbl[i].handler( factor, flowWork, factor->dependID, factor->work );
              BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_CallFactorEnd, factor, factor->dependID, factor->factorType );


              if( factor->recallEnableFlag ){
                factor->recallEnableFlag = FALSE;
              }else{
                factor->callingFlag = FALSE;
              }
              // 呼び出し中に削除された
              if( factor->rmReserveFlag )
              {
                BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_RmvFactorCalling, factor->dependID, factor->factorType, factor );
                if( (next_factor != NULL) && (next_factor->existFlag == FALSE) ){
                  next_factor = factor->next;
                }
                BTL_EVENT_FACTOR_Remove( factor );
              }
            }
            break;
          }
        }
      }
    }while(0);

    if( next_factor != NULL )
    {
      if( next_factor->existFlag )
      {
        factor = next_factor;
      }else{
        factor = factor->next;
      }

      /*
      if( factor ){
        BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_NextFactorInfo2, factor->dependID, factor->factorType, factor );
      }
      */

    }
    else{
      break;
    }
  }
}
/**
 *  特定ハンドラをスキップするかチェック
 */
static BOOL check_handler_skip( BTL_SVFLOW_WORK* flowWork, BTL_EVENT_FACTOR* factor, BtlEventType eventID )
{
  const BTL_POKEPARAM* bpp = NULL;

  if( factor->pokeID != BTL_POKEID_NULL ){
    bpp = BTL_SVFTOOL_GetPokeParam( flowWork, factor->pokeID );
  }

  // とくせいハンドラ
  if( factor->factorType == BTL_EVENT_FACTOR_TOKUSEI )
  {
    if( bpp && BPP_CheckSick(bpp, WAZASICK_IEKI) ){  // 「いえき」状態のポケモンは呼び出さない
      return TRUE;
    }
  }

  // アイテムハンドラ
  if( factor->factorType == BTL_EVENT_FACTOR_ITEM )
  {
    if( BTL_FIELD_CheckEffect(BTL_FLDEFF_MAGICROOM) ){  // 「マジックルーム」発動状態なら呼び出さない
      return TRUE;
    }
    if( bpp && BPP_CheckSick(bpp, WAZASICK_SASIOSAE) ){  // 「さしおさえ」状態のポケモンは呼び出さない
      return TRUE;
    }
    if( bpp && BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_ITEM_CANT_USE) ){  // 装備アイテム使えないフラグONなら呼び出さない
      return TRUE;
    }

  }


  // スキップチェックハンドラが登録されていたら判断させる
  {
    BTL_EVENT_FACTOR* fp;
    for(fp=FirstFactorPtr; fp!=NULL; fp=fp->next)
    {
      if( fp->skipCheckHandler != NULL )
      {
        if( fp->factorType == BTL_EVENT_FACTOR_TOKUSEI )
        {
          const BTL_POKEPARAM* skipBpp = BTL_SVFTOOL_GetPokeParam( flowWork, fp->pokeID );
          if( skipBpp && BPP_CheckSick(skipBpp, WAZASICK_IEKI) ){
            // いえき状態はスキップチェックも行えないようにする
            continue;
          }
        }
        if( fp->rotationSleepFlag ){
          continue;
        }

        if( (fp->skipCheckHandler)( fp, flowWork, factor->factorType, eventID, factor->subID, factor->dependID ) ){
          BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_SkipByCheckHandler, factor->dependID, factor->factorType );
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

//=============================================================================================
/**
 * スキップチェックハンドラをアタッチする
 *
 * @param   factor
 * @param   handler
 */
//=============================================================================================
void BTL_EVENT_FACTOR_AttachSkipCheckHandler( BTL_EVENT_FACTOR* factor, BtlEventSkipCheckHandler handler )
{
  factor->skipCheckHandler = handler;
}

//=============================================================================================
/**
 * スキップチェックハンドラをデタッチする
 *
 * @param   factor
 */
//=============================================================================================
void BTL_EVENT_FACTOR_DettachSkipCheckHandler( BTL_EVENT_FACTOR* factor )
{
  factor->skipCheckHandler = NULL;
}


BTL_EVENT_FACTOR* BTL_EVENT_SeekFactor( BtlEventFactorType factorType, u8 dependID )
{
  BTL_EVENT_FACTOR* factor;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->factorType == factorType) && (factor->dependID == dependID) && (factor->rmReserveFlag==FALSE) )
    {
      return factor;
    }
  }
  return NULL;
}

BTL_EVENT_FACTOR* BTL_EVENT_GetNextFactor( BTL_EVENT_FACTOR* factor )
{
  if( factor ){
    BtlEventFactorType type = factor->factorType;
    u8 dependID = factor->dependID;

    factor = factor->next;
    while( factor ){
      if( (factor->factorType == type) && (factor->dependID == dependID) && (factor->rmReserveFlag==FALSE) ){
        return factor;
      }
      factor = factor->next;
    }
  }
  return NULL;
}

//=============================================================================================
/**
 * 指定ポケモンの貼り付き状態ワザハンドラを休止させる（マジックミラー対処として）
 *
 * @param   pokeID
 *
 */
//=============================================================================================
void BTL_EVENT_SleepFactorMagicMirrorUser( u16 pokeID )
{
  BTL_EVENT_FACTOR* factor;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->factorType == BTL_EVENT_FACTOR_WAZA) && (factor->pokeID == pokeID) )
    {
      if( BTL_HANDLER_Waza_IsStick(factor, factor->work) )
      {
        factor->sleepFlag = TRUE;
      }
    }
  }
}
void BTL_EVENT_WakeFactorMagicMirrorUser( u16 pokeID )
{
  BTL_EVENT_FACTOR* factor;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->factorType == BTL_EVENT_FACTOR_WAZA) && (factor->pokeID == pokeID) )
    {
      factor->sleepFlag = FALSE;
    }
  }
}

//=============================================================================================
/**
 * 指定ポケモンの「とくせい」「どうぐ」ハンドラを休止（ローテーション対処）
 *
 * @param   pokeID
 */
//=============================================================================================
void BTL_EVENT_SleepFactorRotation( u16 pokeID, BtlEventFactorType factorType )
{
  BTL_EVENT_FACTOR* factor;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->pokeID == pokeID)
    &&  (factor->factorType == factorType)
    ){
      factor->rotationSleepFlag = TRUE;
    }
  }
}
//=============================================================================================
/**
 * 指定ポケモンの「とくせい」「どうぐ」ハンドラを休止から復帰（ローテーション対処）
 *
 * @param   pokeID
 * @param   factorType
 *
 * @retval  BOOL    復帰させたハンドラがあればTRUE
 */
//=============================================================================================
BOOL BTL_EVENT_WakeFactorRotation( u16 pokeID, BtlEventFactorType factorType )
{
  BTL_EVENT_FACTOR* factor;
  BOOL result = FALSE;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->pokeID == pokeID)
    &&  (factor->factorType == factorType)
    ){
      factor->rotationSleepFlag = FALSE;
      result = TRUE;
    }
  }

  return result;
}



static BTL_EVENT_FACTOR* pushFactor( void )
{
  if( StackPtr == FACTOR_REGISTER_MAX )
  {
    return NULL;
  }
  return FactorStack[ StackPtr++ ];
}


static void popFactor( BTL_EVENT_FACTOR* factor )
{
  if( StackPtr == 0 )
  {
    GF_ASSERT(0);
    return;
  }
  clearFactorWork( factor );  //値をクリアしてからスタックに積む
  --StackPtr;
  FactorStack[ StackPtr ] = factor;
}

static void clearFactorWork( BTL_EVENT_FACTOR* factor )
{
  GFL_STD_MemClear( factor, sizeof(*factor) );
}

static inline u32 calcFactorPriority( BtlEventPriority mainPri, u32 subPri )
{
  mainPri = BTL_EVPRI_MAX - mainPri - 1;
  return (mainPri << SUBPRI_BIT_WIDTH) | subPri;
}
static inline u16 getFactorMainPriority( u32 pri )
{
  u16 mainPri = (pri >> SUBPRI_BIT_WIDTH) & MAINPRI_MASK;
  mainPri = BTL_EVPRI_MAX - mainPri - 1;
  return mainPri;
}


//======================================================================================================
//  イベント変数スタック構造
//======================================================================================================
enum {
  EVAR_PRINT_FLG = FALSE,
};

static void varStack_Init( void )
{
  VAR_STACK* stack = &VarStack;
  int i;

  stack->sp = 0;
  for(i=0; i<NELEMS(stack->label); ++i)
  {
    stack->label[i] = BTL_EVAR_NULL;
  }
}

//------------------------------------------------------------------------------
/**
 * スタックが空になっているかチェック（毎ターン行う）
 *
 * 空になっていなければ → テスト時はASSERT停止する
 *                       →非テスト時は次善の策としてスタックを空にする
 */
//------------------------------------------------------------------------------
void BTL_EVENTVAR_CheckStackCleared( void )
{
  VAR_STACK* stack = &VarStack;

  if( stack->sp ){
    GF_ASSERT_MSG(0, "sp=%d", stack->sp);
    varStack_Init();
  }
}

//------------------------------------------------------------------------------
/**
 * スタックPush
 *
 * @param   line    Pushを実行した btl_server_flow.c 行番号（デバッグ用）
 */
//------------------------------------------------------------------------------
void BTL_EVENTVAR_PushInpl( u32 line )
{
  VAR_STACK* stack = &VarStack;

  while( stack->sp < NELEMS(stack->label) )
  {
    if( stack->label[stack->sp] == BTL_EVAR_NULL ){
      break;
    }
    stack->sp++;
  }

  if( stack->sp < (NELEMS(stack->label)-1) )
  {
    stack->label[ stack->sp++ ] = BTL_EVAR_SYS_SEPARATE;
    #ifdef PM_DEBUG
    if( stack->sp >= (NELEMS(stack->label)/8*7) ){
      GF_ASSERT_MSG(0, "Event StackPointer =%d 危険水域です！！", stack->sp);
    }
    #endif
    BTL_N_PrintfEx( EVAR_PRINT_FLG, DBGSTR_EVAR_Push, line, stack->sp);
  }
  else
  {
    GF_ASSERT_HEAVY(0);   // stack overflow
  }
}

void BTL_EVENTVAR_PopInpl( u32 line )
{
  VAR_STACK* stack = &VarStack;

  if( stack->sp > 0 )
  {
    u16 p = stack->sp;
    while( (p < NELEMS(stack->label)) && (stack->label[p] != BTL_EVAR_NULL) ){
      stack->label[p++] = BTL_EVAR_NULL;
    }

    stack->sp--;
    stack->label[ stack->sp ] = BTL_EVAR_NULL;

    if( stack->sp )
    {
      while( stack->sp )
      {
        stack->sp--;
        if( stack->label[stack->sp] == BTL_EVAR_SYS_SEPARATE ){
          stack->sp++;
          break;
        }
      }
    }
    BTL_N_PrintfEx( EVAR_PRINT_FLG, DBGSTR_EVAR_Pop, line, stack->sp);

  }
  else
  {
    GF_ASSERT(0);
  }
}

//=============================================================================================
/**
 * ラベルの値を新規セット（サーバフロー用）
 *
 * @param   label
 * @param   value
 */
//=============================================================================================
void BTL_EVENTVAR_SetValue( BtlEvVarLabel label, int value )
{
  GF_ASSERT(label!=BTL_EVAR_NULL);
  GF_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &VarStack;
    int p = evar_getNewPoint( stack, label );
    stack->label[p] = label;
    stack->value[p] = value;
    stack->mulMin[p] = 0;
    stack->mulMax[p] = 0;
    stack->rewriteState[p] = REWRITE_FREE;
  }
}
//=============================================================================================
/**
 * 上書き禁止ラベルの値を新規セット（サーバフロー用）
 *
 * @param   label
 * @param   value
 */
//=============================================================================================
void BTL_EVENTVAR_SetConstValue( BtlEvVarLabel label, int value )
{
  GF_ASSERT(label!=BTL_EVAR_NULL);
  GF_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &VarStack;
    int p = evar_getNewPoint( stack, label );
    stack->label[p] = label;
    stack->value[p] = value;
    stack->mulMin[p] = 0;
    stack->mulMax[p] = 0;
    stack->rewriteState[p] = REWRITE_LOCK;
  }
}
//=============================================================================================
/**
 * 上書き１回のみ有効なラベルの値を新規セット（サーバフロー用）
 *
 * @param   label
 * @param   value
 */
//=============================================================================================
void BTL_EVENTVAR_SetRewriteOnceValue( BtlEvVarLabel label, int value )
{
  GF_ASSERT(label!=BTL_EVAR_NULL);
  GF_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &VarStack;
    int p = evar_getNewPoint( stack, label );
    stack->label[p] = label;
    stack->value[p] = value;
    stack->mulMin[p] = 0;
    stack->mulMax[p] = 0;
    stack->rewriteState[p] = REWRITE_ONCE;
  }
}
//=============================================================================================
/**
 * 乗算対応ラベルの値を新規セット（サーバフロー用）
 *
 * @param   label
 * @param   value
 * @param   mulMin
 * @param   mulMax
 */
//=============================================================================================
void BTL_EVENTVAR_SetMulValue( BtlEvVarLabel label, int value, fx32 mulMin, fx32 mulMax )
{
  GF_ASSERT(label!=BTL_EVAR_NULL);
  GF_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &VarStack;
    int p = evar_getNewPoint( stack, label );
    stack->label[p] = label;
    stack->value[p] = value;
    stack->mulMin[p] = mulMin;
    stack->mulMax[p] = mulMax;
    stack->rewriteState[p] = REWRITE_MUL;
  }
}
//=============================================================================================
/**
 * 既存ラベルの値を上書き（ハンドラ用）
 *
 * @param   label
 * @param   value
 *
 * @retval  BOOL    上書き出来たらTRUE
 */
//=============================================================================================
BOOL BTL_EVENTVAR_RewriteValue( BtlEvVarLabel label, int value )
{
  GF_ASSERT(label!=BTL_EVAR_NULL);
  GF_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &VarStack;
    int p = evar_getExistPoint( stack, label );
    if( p >= 0 )
    {
      if( (stack->rewriteState[p] == REWRITE_FREE)
      ||  (stack->rewriteState[p] == REWRITE_ONCE)
      ){
        stack->label[p] = label;
        stack->value[p] = (int)value;
        if( stack->rewriteState[p] == REWRITE_ONCE ){
          stack->rewriteState[p] = REWRITE_END;
        }
        return TRUE;
      }
      else if(  (stack->rewriteState[p] == REWRITE_LOCK)
      ||        (stack->rewriteState[p] == REWRITE_MUL)
      ){
        GF_ASSERT_MSG(0, "this label(%d) can't rewrite\n", label);
      }
    }
    return FALSE;
  }
}
//=============================================================================================
/**
 * 既存の乗算対応ラベルの値に数値を乗算する
 *
 * @param   label   ラベルID
 * @param   value   乗算する値
 */
//=============================================================================================
void BTL_EVENTVAR_MulValue( BtlEvVarLabel label, fx32 value )
{
  GF_ASSERT(label!=BTL_EVAR_NULL);
  GF_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &VarStack;
    int p = evar_getExistPoint( stack, label );
    if( p >= 0 )
    {
      if( stack->rewriteState[p] == REWRITE_MUL )
      {
        value = FX_Mul( stack->value[p], value );
        value = evar_mulValueRound( stack, p, value );
        stack->value[p] = (int)value;
      }else{
        GF_ASSERT_MSG(0, "this label(%d) can't mul\n", label);
      }
    }
  }
}
//=============================================================================================
/**
 * 既存ラベルの値を取得
 *
 * @param   label
 *
 * @retval  int
 */
//=============================================================================================
int BTL_EVENTVAR_GetValue( BtlEvVarLabel label )
{
  GF_ASSERT(label!=BTL_EVAR_NULL);
  GF_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);
  {
    VAR_STACK* stack = &VarStack;

    int p = stack->sp;
    while( p < NELEMS(stack->label) )
    {
      if( stack->label[p] == label ){
        return stack->value[p];
      }
      if( stack->label[p] == BTL_EVAR_NULL ){
        break;
      }
      ++p;
    }
    GF_ASSERT_MSG(0,"label-%d not found",label);  // label not found
    return 0;
  }
}
//=============================================================================================
/**
 * 指定ラベルが存在すればその値を取得、なければ何もしない
 *
 * @param   label
 * @param   value   [out]
 *
 * @retval  BOOL    ラベルあればTRUE
 */
//=============================================================================================
BOOL BTL_EVENTVAR_GetValueIfExist( BtlEvVarLabel label, int* value )
{
  GF_ASSERT(label!=BTL_EVAR_NULL);
  GF_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);
  {
    VAR_STACK* stack = &VarStack;

    int p = stack->sp;
    while( p < NELEMS(stack->label) )
    {
      if( stack->label[p] == label ){
        *value = stack->value[p];
        return TRUE;
      }
      if( stack->label[p] == BTL_EVAR_NULL ){
        break;
      }
      ++p;
    }
    return FALSE;
  }
}

// 新規ラベル用に確保した位置のポインタを返す
static int evar_getNewPoint( const VAR_STACK* stack, BtlEvVarLabel label )
{
  int p = stack->sp;
  while( p < NELEMS(stack->label) ){
    if( stack->label[p] == BTL_EVAR_NULL ){ break; }
    if( stack->label[p] == label ){
      GF_ASSERT_MSG(0, "Find Same Label ID : %d\n", label);
      break;
    }
    ++p;
  }

  if( p < NELEMS(stack->value) )
  {
    #ifdef PM_DEBUG
    if( p >= (NELEMS(stack->label)/8*7) ){
      BTL_Printf("Var Stack sp=%d 危険水域です！！\n", p);
    }
    #endif
    BTL_PrintfEx( EVAR_PRINT_FLG, "[EVAR]  >regist pos:%d label=%d\n", p, label);
    return p;
  }
  GF_ASSERT(0); // stack overflow
  return 0;
}
// 既存ラベル位置のポインタを返す
static int evar_getExistPoint( const VAR_STACK* stack, BtlEvVarLabel label )
{
  int p = stack->sp;
  while( (p < NELEMS(stack->label)) && (stack->label[p] != BTL_EVAR_NULL) )
  {
    if( stack->label[p] == label ){
      return p;
    }
    ++p;
  }
  GF_ASSERT_MSG(0, "label[%d] not found", label);
  return -1;
}
// 乗算対応変数の上下限を越えていたら丸めて返す
static fx32 evar_mulValueRound( const VAR_STACK* stack, int ptr, fx32 value )
{
  if( stack->mulMax[ptr] || stack->mulMin[ptr] )  // どちらかが0以外なら乗算対応と見なす
  {
    if( stack->mulMin[ptr] > value ){
      value = stack->mulMin[ptr];
    }
    if( stack->mulMax[ptr] < value ){
      value = stack->mulMax[ptr];
    }
  }
  return value;
}

