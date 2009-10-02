//======================================================================
/**
 * @file	eventwork.c
 * @brief	セーブフラグ、ワーク操作
 * @author	Tamada
 * @author	Satoshi Nohara
 * @date	05.10.22
 */
//======================================================================
#include "eventwork.h"
#include "eventwork_def.h"

static u8 * EVENTWORK_GetEventFlagAdrs( EVENTWORK * ev, u16 flag_no );
//======================================================================
//	定義
//======================================================================
//------------------------------------------------------------------
/**
 * @brief	イベントワーク構造体の定義
 */
//------------------------------------------------------------------
struct _EVENTWORK {
	u16 work[EVENT_WORK_AREA_MAX];	//ワーク
	u8	flag[EVENT_FLAG_AREA_MAX];	//フラグ
};

//セーブしないフラグ
static u8 EventCtrlFlag[CTRLFLAG_AREA_MAX] = {};

//システムフラグ範囲内チェック
#define CHECK_SYS_FLAG_RANGE(flag) \
  GF_ASSERT_MSG(flag>=SYS_FLAG_AREA_START && flag<=SYS_FLAG_AREA_END,"ERROR SYS FLAG OUTRANGE")

//======================================================================
//  proto
//======================================================================

//======================================================================
//	EVENTWORK
//======================================================================
//------------------------------------------------------------------
//
//------------------------------------------------------------------
int EVENTWORK_GetWorkSize(void)
{
	return sizeof(EVENTWORK);
}

void EVENTWORK_InitWork( u8 *work )
{
  GFL_STD_MemClear( work, sizeof(EVENTWORK) );
}

//------------------------------------------------------------------
/**
 * @brief	イベントワークを生成する
 *
 * @param	heapID		ヒープID
 *
 * @return	EVENTWORKへのポインタ
 */
//------------------------------------------------------------------
#if 0
EVENTWORK * EVENTWORK_AllocWork( HEAPID heapID )
{
	EVENTWORK * event;
	event = GFL_HEAP_AllocMemory(heapID, sizeof(EVENTWORK));

	EVENTWORK_Init(event);

	return event;
}
#endif

#if 0
//------------------------------------------------------------------
/**
 * @brief	イベントワークを開放する
 *
 * @param	evwk 	EVENTWORK
 *
 * @return	nothing
 */
//------------------------------------------------------------------
void EVENTWORK_FreeWork( EVENTWORK *evwk )
{
	GFL_HEAP_FreeMemory( evwk );
}
#endif

//------------------------------------------------------------------
/**
 * @brief	イベントワークの初期化
 * @param	evwk	EVENTWORKへのポインタ
 */
//------------------------------------------------------------------
#if 0
void EVENTWORK_Init(EVENTWORK * evwk)
{
	//不正な値が入っているのでクリア
	memset( evwk, 0, sizeof(EVENTWORK) );
#if 0
	SVLD_SetCrc(GMDATA_ID_EVENT_WORK);
#endif //CRC_LOADCHECK
}
#endif

//------------------------------------------------------------------
/**
 * @brief	セーブデータからイベントワークへのポインタを取得する
 *
 * @param	sv	セーブデータへのポインタ
 *
 * @return	セーブデータ中のEVENTWORKへのポインタ
 */
//------------------------------------------------------------------
#if 0
EVENTWORK * SaveData_GetEventWork(SAVEDATA * sv)
{
	EVENTWORK * event;
	event = (EVENTWORK *)SaveData_Get(sv, GMDATA_ID_EVENT_WORK);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_EVENT_WORK)
	SVLD_CheckCrc(GMDATA_ID_EVENT_WORK);
#endif //CRC_LOADCHECK
	return event;
}
#endif

//------------------------------------------------------------------
/**
 * @brief	イベントフラグをチェックする
 *
 * @param	ev			イベントワークへのポインタ
 * @param	flag_no		フラグナンバー
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//------------------------------------------------------------------
BOOL EVENTWORK_CheckEventFlag( EVENTWORK * ev, u16 flag_no)
{
	u8 * p = EVENTWORK_GetEventFlagAdrs( ev, flag_no );
	if( p != NULL ){
		if( *p & ( 1 << (flag_no % 8) ) ){ return 1; }
	}
	return 0;
}


//------------------------------------------------------------------
/**
 * @brief	イベントフラグをセットする
 *
 * @param	ev			イベントワークへのポインタ
 * @param	flag_no		フラグナンバー
 *
 * @return	none
 */
//------------------------------------------------------------------
void EVENTWORK_SetEventFlag( EVENTWORK * ev, u16 flag_no)
{
	u8 * p = EVENTWORK_GetEventFlagAdrs( ev, flag_no );
	if( p == NULL ){ return; }
	*p |= 1 << ( flag_no % 8 );
#if 0
	SVLD_SetCrc(GMDATA_ID_EVENT_WORK);
#endif //CRC_LOADCHECK
	return;
}


//------------------------------------------------------------------
/**
 * @brief	イベントフラグをリセットする
 *
 * @param	ev			イベントワークへのポインタ
 * @param	flag_no		フラグナンバー
 *
 * @return	none
 */
//------------------------------------------------------------------
void EVENTWORK_ResetEventFlag( EVENTWORK * ev, u16 flag_no)
{
	u8 * p = EVENTWORK_GetEventFlagAdrs( ev, flag_no );
	if( p == NULL ){ return; }
	*p &= ( 0xff ^ ( 1 << (flag_no % 8) ) );
#if 0
	SVLD_SetCrc(GMDATA_ID_EVENT_WORK);
#endif //CRC_LOADCHECK
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベントフラグのアドレスを取得する
 *
 * @param	flag_no		フラグナンバー
 *
 * @retval	"NULL != フラグアドレス"
 * @retval	"NULL = 未定義"
 */
//--------------------------------------------------------------
static u8 * EVENTWORK_GetEventFlagAdrs( EVENTWORK * ev, u16 flag_no )
{
	if( flag_no == 0 ){
		return NULL;
	}else if( flag_no < SCFLG_START ){
		if( (flag_no / 8) >= EVENT_FLAG_AREA_MAX ){
			GF_ASSERT_MSG(0, "フラグナンバー(0x%04x)が最大数を超えています！\n", flag_no );
      return NULL;
		}
		return &ev->flag[flag_no/8];
	}else{

		if( ((flag_no - SCFLG_START) / 8) >= CTRLFLAG_AREA_MAX ){
			GF_ASSERT_MSG(0, "フラグナンバー(0x%04x)が最大数を超えています！\n", flag_no );
      return NULL;
		}
		return &( EventCtrlFlag[ (flag_no - SCFLG_START) / 8 ] );
	}
}

//--------------------------------------------------------------
/**
 * @brief	イベントワークアドレスを取得
 *
 * @param	ev			イベントワークへのポインタ
 * @param	work_no		ワークナンバー
 *
 * @return	"ワークのアドレス"
 *
 * @li	work_no < 0x8000	通常のセーブワーク
 * @li	work_no >= 0x8000	スクリプト制御ワークの中に確保しているワーク
 */
//--------------------------------------------------------------
u16 * EVENTWORK_GetEventWorkAdrs( EVENTWORK * ev, u16 work_no )
{
	if( (work_no - SVWK_START) >= EVENT_WORK_AREA_MAX ){
		GF_ASSERT_MSG( 0, "ワークナンバー(0x%04x)が最大数を超えています！\n", work_no );
	}
	
	return &ev->work[ work_no - SVWK_START ];
}

//======================================================================
//  イベントワーク　システムフラグ関連
//======================================================================
#if 0
//--------------------------------------------------------------
/**
 * システムフラグ　セット
 * @param ev EVENT_WORK
 * @param flag SYSTEM_FLAG
 * @retval nothing
 */
//--------------------------------------------------------------
static void sysflag_Set( EVENTWORK *ev, u16 flag )
{
  CHECK_SYS_FLAG_RANGE( flag );
  EVENTWORK_SetEventFlag( ev, flag );
}

//--------------------------------------------------------------
/**
 * システムフラグ　リセット
 * @param ev EVENT_WORK
 * @param flag SYSTEM_FLAG
 * @retval nothing
 */
//--------------------------------------------------------------
static void sysflag_Reset( EVENTWORK *ev, u16 flag )
{
  CHECK_SYS_FLAG_RANGE( flag );
  EVENTWORK_ResetEventFlag( ev, flag );
}

//--------------------------------------------------------------
/**
 * システムフラグ　チェック
 * @param ev EVENT_WORK
 * @param flag SYSTEM_FLAG
 * @retval BOOL TRUE=flag on
 */
//--------------------------------------------------------------
static BOOL sysflag_Check( EVENTWORK *ev, u16 flag )
{
  CHECK_SYS_FLAG_RANGE( flag );
  return EVENTWORK_CheckEventFlag( ev, flag );
}

//--------------------------------------------------------------
/**
 * システムフラグ　共通処理
 * @param ev  EVENTWORK
 * @param mode SYS_FLAG_MODE
 * @param flag_id
 * @retval BOOL SYS_FLAG_MODE_CHECK時、TRUEでflag on
 */
//--------------------------------------------------------------
static BOOL sysflag_Common( EVENTWORK *ev, SYS_FLAG_MODE mode, u16 flag )
{
  switch( mode ){
  case SYS_FLAG_MODE_SET:
    sysflag_Set( ev, flag );
    break;
  case SYS_FLAG_MODE_RESET:
    sysflag_Reset( ev, flag );
    break;
  case SYS_FLAG_MODE_CHECK:
    return sysflag_Check( ev, flag );
  default:
 		GF_ASSERT_MSG( 0,"FLAG %d, NOT DEFINED MODE %d\n", flag, mode );
  }
  return FALSE;
}

//======================================================================
//  システムフラグ　秘伝技関連
//======================================================================
//--------------------------------------------------------------
/**
 * システムフラグ　秘伝技　かいりき
 * @param ev EVENTWORK
 * @param mode SYS_FLAG_MODE
 * @retval BOOL
 */
//--------------------------------------------------------------
BOOL EVENTWORK_SYS_FLAG_Kairiki( EVENTWORK *ev, SYS_FLAG_MODE mode )
{
  return sysflag_Common( ev, mode, SYS_FLAG_KAIRIKI );
}
#endif //#if 0
