//============================================================================================
/**
 * @file	script_work.c
 * @brief	スクリプトグローバルワーク定義
 * @date  2010.01.26
 * @author  tamada
 */
//============================================================================================
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "script.h"
#include "script_local.h"

#include "scrcmd_work.h"
#include "eventwork_def.h"

#include "field/zonedata.h"

#include "print/wordset.h"    //WORDSET

#include "trainer_eye_data.h"   //SCR_TRAINER_HITDATA
#include "field/eventdata_system.h" //EVENTDATA_GetSpecialScriptData

#include "fieldmap.h"   //FIELDMAP_GetFldMsgBG
#include "battle/battle.h"

#include "musical/musical_event.h"  //MUSICAL_EVENT_WORK

#include "script_trainer.h" //SCRIPT_GetTrainerHitData
#include "scrcmd_work.h"

//============================================================================================
//  define
//============================================================================================
enum
{
	WORDSET_SCRIPT_SETNUM = 32,		///<デフォルトバッファ数
	WORDSET_SCRIPT_BUFLEN = 64,		///<デフォルトバッファ長（文字数）
  //SCR_MSG_BUF_SIZE	= (512),				///<メッセージバッファサイズ
  SCR_MSG_BUF_SIZE =	(1024)				//メッセージバッファサイズ
};


#define SCRIPT_MAGIC_NO		(0x3643f)

#define SCRIPT_MSG_NON (0xffffffff)

#define SCRIPT_NOT_EXIST_ID (0xffff)

enum {
  TEMP_WORK_START = SCWK_AREA_START,
  TEMP_WORK_END = USERWK_AREA_END,
  TEMP_WORK_SIZE = TEMP_WORK_END - TEMP_WORK_START,

  USERWK_SIZE = USERWK_AREA_END - USERWK_AREA_START,
};

//============================================================================================
//	struct
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief スクリプトグローバルワーク定義
 *
 * 各仮想マシンをまたいで存在するスクリプト用ワーク
 */
//--------------------------------------------------------------
struct _TAG_SCRIPT_WORK
{
  //☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆
  //script.c でPrivateなメンバ
  //☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆
	u32 magic_no;			//イベントのワークがスクリプト制御ワークかを判別
	
  //☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆
  //SCRIPT_WORK生成時に引数で初期化され、ReadOnlyで外部公開されるメンバ
  //☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆
	u16 start_scr_id;			//メインのスクリプトID
	MMDL *target_obj;

	HEAPID main_heapID;

	GAMESYS_WORK *gsys;
  GMEVENT *gmevent; //スクリプトを実行しているGMEVENT*
	SCRIPT_FLDPARAM fld_param;

	u16 *ret_script_wk;			//スクリプト結果を代入するワークのポインタ

  BOOL is_sp_flag;    ///<特殊スクリプトかどうか？のフラグ　TRUE=特殊スクリプト
  SCRIPT_TYPE scr_type;   ///<実行するスクリプトの種別

  //☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆
  //script.c内で使用しない、外部公開用メンバ
  //  public R/W
  //☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆
	WORDSET* wordset;				//単語セット
	STRBUF* msg_buf;				//メッセージバッファポインタ
	STRBUF* tmp_buf;				//テンポラリバッファポインタ
	GFL_FONT* font;			    //フォントポインタ　海底神殿暗号フォントに使用


	void * subproc_work; //サブプロセスとのやりとりに使用するワークへの*

  u8 sound_se_flag;   ///< サウンドSE再生チェックフラグ

	//トレーナー視線情報
	SCR_TRAINER_HITDATA trainer_hitdata[TRAINER_EYE_HITMAX];
	
	u16 scrTempWork[TEMP_WORK_SIZE];		//ワーク(ANSWORK,TMPWORKなどの代わり)

  FIELD_SAVEANIME* bg_saveanime;
  
  EV_MOVEPOKE_WORK* movepoke;

  // 再戦トレーナーテンポラリ
  void* rebattle_trainer;

  void* scrcmd_global;
};

//============================================================================================
//	プロトタイプ宣言
//============================================================================================


//イベントワーク、スクリプトワークアクセス関数
static u16 * getTempWork( SCRIPT_WORK * sc, u16 work_no );

static void initFldParam(SCRIPT_FLDPARAM * fparam, GAMESYS_WORK * gsys);

//------------------------------------------------------------------
//------------------------------------------------------------------
static void initFldParam(SCRIPT_FLDPARAM * fparam, GAMESYS_WORK * gsys)
{
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  MI_CpuClear32( fparam, sizeof(SCRIPT_FLDPARAM) );
  fparam->fieldMap = fieldmap;
  if (fieldmap != NULL)
  {
    fparam->msgBG = FIELDMAP_GetFldMsgBG(fieldmap);
  }
}


//============================================================================================
//
//  スクリプトワーク
//
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief スクリプト用グローバルワークの生成
 * @param main_heapID
 * @param gsys
 * @param event
 * @param	scr_id		スクリプトID
 * @param	ret_wk		スクリプト結果を代入するワークのポインタ
 * @param scr_type  スクリプトの種別指定
 * @return	SCRIPT_WORK			SCRIPT型のポインタ
 *
 * スクリプトコマンド全般からグローバルアクセス可能なデータを保持する
 */
//--------------------------------------------------------------
SCRIPT_WORK * SCRIPTWORK_Create( HEAPID main_heapID,
    GAMESYS_WORK * gsys, GMEVENT * event, u16 scr_id, void* ret_wk, SCRIPT_TYPE scr_type )
{
  SCRIPT_WORK * sc;

  GF_ASSERT( scr_type < SCRIPT_TYPE_MAX );

	sc = GFL_HEAP_AllocClearMemory( main_heapID, sizeof(SCRIPT_WORK) );
	sc->magic_no = SCRIPT_MAGIC_NO;
	sc->main_heapID = main_heapID;

	sc->gsys = gsys;
  sc->gmevent = event;
	sc->start_scr_id  = scr_id;	//メインのスクリプトID
	sc->target_obj = NULL;
	sc->ret_script_wk = ret_wk;	//スクリプト結果を代入するワーク
  sc->scr_type = scr_type;
  sc->is_sp_flag = SCRIPT_IsSpecialScriptType( scr_type );
  
  //メッセージ関連
  if ( sc->is_sp_flag == FALSE )
  {
    sc->wordset = WORDSET_CreateEx(
      WORDSET_SCRIPT_SETNUM, WORDSET_SCRIPT_BUFLEN, main_heapID );
    sc->msg_buf = GFL_STR_CreateBuffer( SCR_MSG_BUF_SIZE, main_heapID );
    sc->tmp_buf = GFL_STR_CreateBuffer( SCR_MSG_BUF_SIZE, main_heapID );
  }
  else
  {
    //特殊スクリプトの場合、メッセージ用のワークエリアは不要
    sc->wordset = NULL;
    sc->msg_buf = NULL;
    sc->tmp_buf = NULL;
  }

  sc->scrcmd_global = SCRCMD_GLOBAL_Create( sc, main_heapID );

  //フィールドパラメータ生成
	initFldParam( &sc->fld_param, gsys );

  if ( sc->is_sp_flag == FALSE )
  {
    //チェックビット処理化
    SCREND_CHK_ClearBits();
  }

  return sc;
}

//--------------------------------------------------------------
/**
 * @brief スクリプト用グローバルワークの削除
 */
//--------------------------------------------------------------
void SCRIPTWORK_Delete( SCRIPT_WORK * sc )
{
  sc->magic_no = 0;
  if ( sc->wordset )
  {
    WORDSET_Delete( sc->wordset );
  }
  if ( sc->msg_buf ) {
    GFL_STR_DeleteBuffer( sc->msg_buf );
  }
  if ( sc->tmp_buf ) {
    GFL_STR_DeleteBuffer( sc->tmp_buf );
  }

  SCRCMD_GLOBAL_Delete( sc->scrcmd_global );

  GFL_HEAP_FreeMemory( sc );
}

//============================================================================================
//	スクリプト制御ワークのメンバーアクセス
//============================================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
GMEVENT * SCRIPT_GetEvent( SCRIPT_WORK * sc )
{
  GF_ASSERT(sc->magic_no == SCRIPT_MAGIC_NO);
  return sc->gmevent;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
GAMESYS_WORK * SCRIPT_GetGameSysWork( SCRIPT_WORK * sc )
{
  GF_ASSERT(sc->magic_no == SCRIPT_MAGIC_NO);
  return sc->gsys;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
SCRIPT_FLDPARAM * SCRIPT_GetFieldParam( SCRIPT_WORK * sc )
{
  FIELDMAP_WORK * fieldmap;
  GF_ASSERT( sc->magic_no == SCRIPT_MAGIC_NO );
  //スクリプトコマンド実行中にフィールドへの出入りがあると、
  //条件が変わってポインタが正しい値を指していない可能性がある。
  //なので、アクセスタイミングでの初期化処理を行う。
  initFldParam( &sc->fld_param, sc->gsys );
  GF_ASSERT( sc->fld_param.fieldMap != NULL );
  return &sc->fld_param;
}

//--------------------------------------------------------------
/**
 * @brief スクリプトコマンド用ワークへのポインタ取得
 */
//--------------------------------------------------------------
void * SCRIPT_GetScrCmdGlobal( SCRIPT_WORK * sc )
{
  GF_ASSERT( sc->magic_no );
  return sc->scrcmd_global;
}

//--------------------------------------------------------------
/**
 * @brief 単語セット保持ポインタの取得
 * @param	sc		    SCRIPT_WORKのポインタ
 */
//--------------------------------------------------------------
WORDSET * SCRIPT_GetWordSet( SCRIPT_WORK * sc )
{
  GF_ASSERT( sc->wordset );
  return sc->wordset;
}
//--------------------------------------------------------------
/**
 * @brief MSGバッファの取得
 * @param	sc		    SCRIPT_WORKのポインタ
 */
//--------------------------------------------------------------
STRBUF * SCRIPT_GetMsgBuffer( SCRIPT_WORK * sc )
{
  GF_ASSERT( sc->msg_buf );
  return sc->msg_buf;
}

//--------------------------------------------------------------
/**
 * @brief MSG展開用テンポラリバッファの取得
 * @param	sc		    SCRIPT_WORKのポインタ
 */
//--------------------------------------------------------------
STRBUF * SCRIPT_GetMsgTempBuffer( SCRIPT_WORK * sc )
{
  GF_ASSERT( sc->tmp_buf );
  return sc->tmp_buf;
}

//----------------------------------------------------------------------------
/**
 *	@brief  フォントハンドルを設定    （海底神殿　暗号メッセージに使用）
 *
 *	@param	sc
 *	@param	fontHandle 
 */
//-----------------------------------------------------------------------------
void SCRIPT_SetFontHandle( SCRIPT_WORK * sc, GFL_FONT* fontHandle )
{
  sc->font = fontHandle;
}

//----------------------------------------------------------------------------
/**
 *	@brief  フォントハンドルの取得  （海底神殿　暗号メッセージに使用）
 *
 *	@param	sc 
 *
 *	@return フォントハンドル
 */
//-----------------------------------------------------------------------------
GFL_FONT* SCRIPT_GetFontHandle( const SCRIPT_WORK * sc )
{
  return sc->font;
}


//--------------------------------------------------------------
/**
 * @brief SE制御用フラグの取得
 */
//--------------------------------------------------------------
u8 * SCRIPT_GetSoundSeFlag( SCRIPT_WORK * sc )
{
  return &sc->sound_se_flag;
}
//--------------------------------------------------------------
/**
 * @brief   開始時のスクリプトID取得
 * @param	sc		    SCRIPT_WORKのポインタ
 * @return  u16   スクリプトID
 */
//--------------------------------------------------------------
u16 SCRIPT_GetStartScriptID( const SCRIPT_WORK * sc )
{
  return sc->start_scr_id;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
MMDL * SCRIPT_GetTargetObj( SCRIPT_WORK * sc )
{
  return sc->target_obj;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void SCRIPT_SetTargetObj( SCRIPT_WORK * sc, MMDL * obj )
{
	sc->target_obj = obj;		//話しかけ対象OBJのポインタセット
	if( obj != NULL ){
    u16 *objid;
    objid = getTempWork( sc, SCWK_TARGET_OBJID );
		*objid = MMDL_GetOBJID( obj ); //話しかけ対象OBJIDのセット
	}
}

//--------------------------------------------------------------
/**
 * サブプロセス用ワークのポインターアドレスを取得
 * @param	sc SCRIPT_WORK
 * @retval none
 *
 * 使い終わったら必ずNULLクリアすること！
 */
//--------------------------------------------------------------
void** SCRIPT_SetSubProcWorkPointerAdrs( SCRIPT_WORK *sc )
{
  return &sc->subproc_work;
}

//--------------------------------------------------------------
/**
 * サブプロセス用ワークのポインターを取得
 * @param	sc SCRIPT_WORK
 * @retval none
 *
 * 使い終わったら必ずNULLクリアすること！
 */
//--------------------------------------------------------------
void * SCRIPT_SetSubProcWorkPointer( SCRIPT_WORK *sc )
{
  return sc->subproc_work;
}

//--------------------------------------------------------------
/**
 * サブプロセス用ワーク領域の解放(ポインタがNULLでなければFree)
 * @param	sc SCRIPT_WORK
 * @retval none
 */
//--------------------------------------------------------------
void SCRIPT_FreeSubProcWorkPointer( SCRIPT_WORK *sc )
{
  if(sc->subproc_work != NULL){
    GFL_HEAP_FreeMemory(sc->subproc_work);
    sc->subproc_work = NULL;
  }
}



//============================================================================================
//	スクリプトワーク・フラグ関連
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static u16 * getTempWork( SCRIPT_WORK * sc, u16 work_no )
{
  GF_ASSERT( work_no >= TEMP_WORK_START );
  GF_ASSERT( work_no < TEMP_WORK_END );
  return &(sc->scrTempWork[ work_no - TEMP_WORK_START ]);
}
//------------------------------------------------------------------
/**
 * @brief	イベントワークアドレスを取得
 * @param	ev			イベントワークへのポインタ
 * @param	work_no		ワークナンバー
 *
 * @return	"ワークのアドレス"
 *
 * @li	work_no < 0x8000	通常のセーブワーク
 * @li	work_no >= 0x8000	スクリプト制御ワークの中に確保しているワーク
 */
//------------------------------------------------------------------
u16 * SCRIPT_GetEventWork( SCRIPT_WORK *sc, GAMEDATA *gdata, u16 work_no )
{
	EVENTWORK *ev;
	ev = GAMEDATA_GetEventWork( gdata );
	
	if( work_no < SVWK_START ){
		return NULL;
	}
	
	if( work_no < SCWK_START ){
		return EVENTWORK_GetEventWorkAdrs(ev,work_no);
	}

  if ( work_no < SCWK_END ){
    //スクリプト制御ワークの中で、ANSWORKなどのワークを確保しています
    return getTempWork( sc, work_no );
  }
	
  return NULL;
}

//------------------------------------------------------------------
/**
 * @brief	イベントワークの値を取得
 *
 * @param	work_no		ワークナンバー
 *
 * @return	"ワークの値"
 */
//------------------------------------------------------------------
u16 SCRIPT_GetEventWorkValue( SCRIPT_WORK *sc, GAMEDATA *gdata, u16 work_no )
{
	u16 *res = SCRIPT_GetEventWork( sc, gdata, work_no );
	if( res == NULL ){ return work_no; }
	return *res;
}

//------------------------------------------------------------------
/**
 * @brief	イベントワークの値をセット
 *
 * @param	work_no		ワークナンバー
 * @param	value		セットする値
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//------------------------------------------------------------------
BOOL SCRIPT_SetEventWorkValue(
	SCRIPT_WORK *sc, u16 work_no, u16 value )
{
  GAMEDATA * gdata;
	u16* res;
  GF_ASSERT(sc->magic_no == SCRIPT_MAGIC_NO);
  gdata = GAMESYSTEM_GetGameData( sc->gsys );
	res = SCRIPT_GetEventWork( sc, gdata, work_no );
	if( res == NULL ){ return FALSE; }
	*res = value;
	return TRUE;
}
//--------------------------------------------------------------
/**
 * プログラムからスクリプトへの引数となるパラメータをセット
 * @param	sc SCRIPT_WORK
 * @param	prm0	パラメータ０（SCWK_PARAM0）
 * @param	prm1	パラメータ１（SCWK_PARAM1）
 * @param	prm2	パラメータ２（SCWK_PARAM2）
 * @param	prm3	パラメータ３（SCWK_PARAM3）
 * @retval none
 */
//--------------------------------------------------------------
void SCRIPT_SetScriptWorkParam( SCRIPT_WORK *sc, u16 prm0, u16 prm1, u16 prm2, u16 prm3 )
{
  GF_ASSERT(sc->magic_no == SCRIPT_MAGIC_NO);
  SCRIPT_SetEventWorkValue( sc, SCWK_PARAM0, prm0 );
  SCRIPT_SetEventWorkValue( sc, SCWK_PARAM1, prm1 );
  SCRIPT_SetEventWorkValue( sc, SCWK_PARAM2, prm2 );
  SCRIPT_SetEventWorkValue( sc, SCWK_PARAM3, prm3 );
}

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void * SCRIPT_BackupUserWork( SCRIPT_WORK * sc )
{
  u16 * backup_work = GFL_HEAP_AllocClearMemory( sc->main_heapID, sizeof(u16) * USERWK_SIZE );
  GFL_STD_MemCopy16( getTempWork( sc, USERWK_AREA_START ), backup_work, sizeof(u16) * USERWK_SIZE );
  return backup_work;
}

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void SCRIPT_RestoreUserWork( SCRIPT_WORK * sc, void *backup_work )
{
  GFL_STD_MemCopy16( backup_work, getTempWork( sc, USERWK_AREA_START ), sizeof(u16) * USERWK_SIZE );
  GFL_HEAP_FreeMemory( backup_work );
}

//----------------------------------------------------------------------------
/**
 *	@brief  セーブBGアニメーションワークの管理
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  セーブBGアニメーションワークの保存
 *
 *	@param	sc    ワーク
 *	@param	wk    セーブBGアニメーションワーク
 */
//-----------------------------------------------------------------------------
void SCRIPT_SetSaveAnimeWork( SCRIPT_WORK * sc, FIELD_SAVEANIME * wk )
{
  sc->bg_saveanime = wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  保存しているセーブBGアニメーションワーク取得
 *
 *	@param	sc  ワーク
 */
//-----------------------------------------------------------------------------
FIELD_SAVEANIME * SCRIPT_GetSaveAnimeWork( SCRIPT_WORK * sc )
{
  return sc->bg_saveanime;
}


//----------------------------------------------------------------------------
/**
 *	@brief  移動ポケモンアニメーションワークの管理
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  移動ポケモンアニメーションワークの設定
 */
//-----------------------------------------------------------------------------
void SCRIPT_SetMovePokeWork( SCRIPT_WORK * sc, EV_MOVEPOKE_WORK * wk )
{
  sc->movepoke = wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  移動ポケモンアニメーションワークの取得
 */
//-----------------------------------------------------------------------------
EV_MOVEPOKE_WORK * SCRIPT_GetMovePokeWork( SCRIPT_WORK * sc )
{
  return sc->movepoke;
}


//----------------------------------------------------------------------------
/**
 *	@brief  再戦トレーナーワークの保存
 */
//-----------------------------------------------------------------------------
void SCRIPT_SetReBattleTrainerData( SCRIPT_WORK * sc, void * wk )
{
  sc->rebattle_trainer = wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  再戦トレーナーワーク取得
 *	@return ワーク
 */
//-----------------------------------------------------------------------------
void * SCRIPT_GetReBattleTrainerData( SCRIPT_WORK * sc )
{
  return sc->rebattle_trainer;
}




//--------------------------------------------------------------
/**
 * トレーナー視線情報の取得
 */
//--------------------------------------------------------------
SCR_TRAINER_HITDATA * SCRIPT_GetTrainerHitData( SCRIPT_WORK * sc, u32 tr_no )
{
  GF_ASSERT( tr_no < TRAINER_EYE_HITMAX );
  return &sc->trainer_hitdata[tr_no];
}


