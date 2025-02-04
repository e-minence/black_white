//======================================================================
/**
 * @file scr_musical.c
 * @brief スクリプトコマンド　ミュージカル関連
 * @author	Satoshi Nohara
 * @date	06.06.26
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/vm_cmd.h"

#include "sound/pm_sndsys.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"
#include "scrcmd_proc.h"

#include "musical/musical_system.h"
#include "musical/musical_event.h"
#include "musical/musical_shot_sys.h"
#include "musical/musical_dressup_sys.h"
#include "savedata/save_control.h"
#include "savedata/musical_save.h"
#include "savedata/musical_dist_save.h"

#include "poke_tool/poke_tool.h"  //ドレスアップデータ用
#include "poke_tool/monsno_def.h" //ドレスアップデータ用
#include "event_fieldmap_control.h"
#include "event_poke_status.h"

#include "field_sound.h"
#include "message.naix"
#include "msg/script/msg_musical_scr.h"
#include "msg/msg_musical_common.h"
#include "msg/msg_mus_item_name.h"
#include "scrcmd_musical.h"

#include "../../../resource/fldmapdata/script/musical_scr_local.h"
#include "fieldmap/fldmmdl_objcode.h"
#include "net_app/irc_match/ircbattlematch.h"

#include "test/ariizumi/ari_debug.h"
#if PM_DEBUG
#include "musical/musical_debug_menu.h"
#endif

//======================================================================
//  define
//======================================================================
FS_EXTERN_OVERLAY(musical);
FS_EXTERN_OVERLAY(ircbattlematch);

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// EVENT_MUSICAL_WORK
//--------------------------------------------------------------
typedef struct
{
  GMEVENT *event;
  MUSICAL_SCRIPT_WORK *musScriptWork;
  BOOL isComm;
}EVENT_MUSICAL_WORK;

typedef struct
{
  MUS_SHOT_INIT_WORK *initWork;
}EV_MUSSHOT_CALL_WORK;

typedef struct
{
  DRESSUP_INIT_WORK *initWork;
}EV_FITTING_CALL_WORK;


typedef struct
{
  SCRCMD_WORK *scWork;
  MUSICAL_SCRIPT_WORK *musScriptWork;
}EV_IRCPROC_CALL_WORK;

//======================================================================
//  proto
//======================================================================
static void EvCmdMusical_InitCommon( SCRIPT_WORK *sc , GAMEDATA *gameData );
static void EvCmdMusical_ExitCommon( MUSICAL_SCRIPT_WORK *musScriptWork , GAMEDATA *gameData );
static void EvCmdMusical_InitCommon_Comm( MUSICAL_SCRIPT_WORK *musScriptWork , GAMESYS_WORK *gsys , GAME_COMM_SYS_PTR gameComm , const BOOL isIrc );
static void EvCmdMusical_ExitCommon_Comm( MUSICAL_SCRIPT_WORK *musScriptWork );

static GMEVENT_RESULT event_Musical( GMEVENT *event, int *seq, void *work );
static GMEVENT_RESULT event_Fitting( GMEVENT *event, int *seq, void *work );
static void EvCmdMusicalShotCallProc_CallBack( void* work );
static void EvCmdFittingCallProc_CallBack( CALL_PROC_WORK* work );
static void EvCmdIrcEntry_CallBack( void* work );

static BOOL EvCmdMusicalEntryParent( VMHANDLE *core, void *wk );
static BOOL EvCmdMusicalEntryChild( VMHANDLE *core, void *wk );
static BOOL EvCmdMusicalCommTimingSync( VMHANDLE *core, void *wk );
static BOOL EvCmdMusicalWaitPostProgram( VMHANDLE *core, void *wk );
static BOOL EvCmdMusicalWaitInitNet( VMHANDLE *core, void *wk );
static BOOL EvCmdMusicalWaitExitNet( VMHANDLE *core, void *wk );
static BOOL EvCmdMusicalWaitPostAllPoke( VMHANDLE *core, void *wk );
//======================================================================
//  スクリプトコマンド　ミュージカル
//======================================================================
//--------------------------------------------------------------
/**
 * ミュージカル：ミュージカル呼び出し
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMusicalCall( VMHANDLE *core, void *wk )
{
  GMEVENT *call_event;
  EVENT_MUSICAL_WORK *ev_musical_work;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );

  u16 pokeIdx = SCRCMD_GetVMWorkValue( core, work );
  u8  mode = VMGetU8(core);

  if( mode == 0 || mode == 1 )
  {
    MUSICAL_SCRIPT_WORK *musScriptWork = GAMEDATA_GetMusicalScrWork( gdata );
    
    const BOOL isComm = ( mode == 0 ? FALSE : TRUE );
    call_event = GMEVENT_Create(
        gsys, NULL, event_Musical, sizeof(EVENT_MUSICAL_WORK) );
    ev_musical_work = GMEVENT_GetEventWork( call_event );
  	MI_CpuClear8( ev_musical_work, sizeof(EVENT_MUSICAL_WORK) );
    
    ev_musical_work->event = MUSICAL_CreateEvent( gsys, gdata , pokeIdx , isComm , musScriptWork );
    ev_musical_work->musScriptWork = musScriptWork;
    ev_musical_work->isComm = isComm;
    //スクリプト終了後、指定したイベントに遷移する
    SCRIPT_EntryNextEvent( sc, call_event );
    
    {
      FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
    //  PMSND_StopBGM();
    //  FIELD_SOUND_PushBGM( fsnd );
    //  PMSND_FadeOutBGM( 30 );
    }
  
    VM_End( core );
    return VMCMD_RESULT_SUSPEND;
  }
  else
  {
    GMEVENT* event;
    MUSICAL_SAVE *musSave = GAMEDATA_GetMusicalSavePtr( gdata );
    EV_MUSSHOT_CALL_WORK *callWork = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(EV_MUSSHOT_CALL_WORK) );
    callWork->initWork = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(MUS_SHOT_INIT_WORK) );
    callWork->initWork->isCheckMode = FALSE;
    callWork->initWork->musShotData = MUSICAL_SAVE_GetMusicalShotData( musSave );
    callWork->initWork->isLoadOverlay = TRUE;
    //ショット呼び出し
    event = EVENT_FieldSubProc_Callback( gsys, fieldmap, 
                                         FS_OVERLAY_ID(musical_shot), &MusicalShot_ProcData, callWork->initWork,
                                         EvCmdMusicalShotCallProc_CallBack, callWork );
    SCRIPT_CallEvent( sc, event );

    return VMCMD_RESULT_SUSPEND;
    
  }
}

//--------------------------------------------------------------
/**
 * ミュージカル：ミュージカル 試着室呼び出し
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMusicalFittingCall( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  u16 pokeIdx = SCRCMD_GetVMWorkValue( core, work );
  
  {
    GMEVENT* event;
    MUSICAL_SAVE *musSave = GAMEDATA_GetMusicalSavePtr( gdata );
    POKEPARTY *party = GAMEDATA_GetMyPokemon(gdata);
    POKEMON_PARAM *pokePara = PokeParty_GetMemberPointer( party , pokeIdx );
    MUSICAL_POKE_PARAM *musPoke = MUSICAL_SYSTEM_InitMusPoke( pokePara , HEAPID_PROC );

    EV_FITTING_CALL_WORK *callWork;

    event = GMEVENT_Create( gsys, NULL, event_Fitting, sizeof(EV_FITTING_CALL_WORK) );
    callWork = GMEVENT_GetEventWork( event );
  	MI_CpuClear8( callWork, sizeof(EV_FITTING_CALL_WORK) );

    callWork->initWork = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(DRESSUP_INIT_WORK) );
    callWork->initWork->musPoke = musPoke;
    callWork->initWork->mus_save = musSave;
    callWork->initWork->commWork = NULL;
    SCRIPT_CallEvent( sc, event );
    
    //試着室呼び出し
//    event = EVENT_FieldSubProc_Callback( gsys, fieldmap, 
//                                         NO_OVERLAY_ID, &DressUp_ProcData, callWork->initWork,
//                                         EvCmdFittingCallProc_CallBack, callWork );
//    SCRIPT_CallEvent( sc, event );

//    EVFUNC_CallSubProc( core, work, NO_OVERLAY_ID, 
//                        &DressUp_ProcData, callWork->initWork, 
//                        EvCmdFittingCallProc_CallBack, callWork );
  }

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * ミュージカル：ミュージカル汎用数値取得
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetMusicalValue( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );

  u8   type = VMGetU8(core);
  u16  val = SCRCMD_GetVMWorkValue( core, work );
  u16* ret_wk = SCRCMD_GetVMWork( core, work );

  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  MUSICAL_SAVE *musSave = GAMEDATA_GetMusicalSavePtr( gdata );

  
  MUSICAL_SCRIPT_WORK *musScriptWork = NULL;
  MUSICAL_EVENT_WORK *evWork = NULL;
  if( type >= MUSICAL_VALUE_WR_SELF_IDX )
  {
    musScriptWork = GAMEDATA_GetMusicalScrWork( gdata );
    if( musScriptWork == NULL )
    {
      GF_ASSERT_MSG(0,"Musical script work is NULL!!");
      *ret_wk = 0;
      return VMCMD_RESULT_CONTINUE;
    }
    evWork = musScriptWork->eventWork;
  }

  switch( type )
  {
  case MUSICAL_VALUE_JOINNUM:       //参加回数
    *ret_wk = MUSICAL_SAVE_GetEntryNum( musSave );
    break;
  case MUSICAL_VALUE_TOPNUM:        //トップ回数
    *ret_wk = MUSICAL_SAVE_GetTopNum( musSave );
    break;
  case MUSICAL_VALUE_LAST_POINT:    //最終評価点
    *ret_wk = MUSICAL_SAVE_GetBefPoint( musSave );
    break;
  case MUSICAL_VALUE_LAST_CONDITION://最終コンディション
    {
      u8 i;
      u8 max = 0;
      u8 maxIdx = MCT_MAX;
      for( i=0;i<MCT_MAX;i++ )
      {
        const u8 val = MUSICAL_SAVE_GetBefCondition( musSave , i );
        if( val > max )
        {
          max = val;
          maxIdx = i;
        }
        else if( val == max )
        {
          //同点があった場合はその他へ
          maxIdx = MCT_MAX;
        }
      }
      *ret_wk = maxIdx;
    }
    break;
  case MUSICAL_VALUE_PROGRAM_NUMBER:  //選択演目番号
    *ret_wk = MUSICAL_SAVE_GetProgramNumber( musSave );
    break;
  case MUSICAL_VALUE_SET_PROGRAM_NUMBER:  //選択演目番号設定
    MUSICAL_SAVE_SetProgramNumber( musSave , val );
    break;
  case MUSICAL_VALUE_IS_EQUIP_ANY:      //何か装備しているか？
    {
      u8 i;
      *ret_wk = 0;
      for( i=0;i<MCT_MAX;i++ )
      {
        const u8 val = MUSICAL_SAVE_GetBefCondition( musSave , i );
        if( val != 0 )
        {
          *ret_wk = 1;
        }
      }
    }
    break;
  case MUSICAL_VALUE_IS_ENABLE_SHOT:    //ミュージカルショットが有効か？
    {
      MUSICAL_SHOT_DATA *shotData = MUSICAL_SAVE_GetMusicalShotData( musSave );
      if( shotData->month == 0 )
      {
        *ret_wk = 0;
      }
      else
      {
        *ret_wk = 1;
      }
    }
    break;
  case MUSICAL_VALUE_LAST_CONDITION_MIN://最終コンディション(最低)
    {
      u8 i;
      u8 min = 0xFF;
      u8 minIdx = MCT_MAX;
      for( i=0;i<MCT_MAX;i++ )
      {
        const u8 val = MUSICAL_SAVE_GetBefCondition( musSave , i );
        //同点があった場合は下から優先するため、ここは以下でチェック
        if( val <= min )
        {
          min = val;
          minIdx = i;
        }
      }
      *ret_wk = minIdx;
    }
    break;

  case MUSICAL_VALUE_LAST_CONDITION_MAX://最終コンディション(最低)
    {
      u8 i;
      u8 sameCnt = 0;
      u8 max = 0;
      u8 maxIdx = MCT_MAX;
      for( i=0;i<MCT_MAX;i++ )
      {
        const u8 val = MUSICAL_SAVE_GetBefCondition( musSave , i );
        //同点があった場合は上から優先するため、ここは"より大きい"でチェック
        if( val > max )
        {
          max = val;
          maxIdx = i;
        }
        else
        if( val == max )
        {
        }
      }
      *ret_wk = maxIdx;
    }
    break;

  case MUSICAL_VALUE_LAST_CONDITION_2ND://最終コンディション(２位)
    {
      u8 i;
      u8 max = 0;
      u8 maxIdx = MCT_MAX;
      u8 second = 0;
      u8 secondIdx = MCT_MAX;
      for( i=0;i<MCT_MAX;i++ )
      {
        const u8 val = MUSICAL_SAVE_GetBefCondition( musSave , i );
        //同点があった場合は上から優先するため、ここは"より大きい"でチェック
        if( val > max )
        {
          if( second == max )
          {
            secondIdx = MCT_MAX;
          }
          else
          {
            second = max;
            secondIdx = maxIdx;
          }
          
          max = val;
          maxIdx = i;
        }
      }
      *ret_wk = secondIdx;
    }
    break;

  case MUSICAL_VALUE_COUNT_MUSICAL_POKE://参加可能なポケモン数
    {
      u8 num = 0;
      u8 i;
      POKEPARTY *party = GAMEDATA_GetMyPokemon(gdata);
      const u8 partyNum = PokeParty_GetPokeCount( party );
      for( i=0;i<partyNum;i++ )
      {
        POKEMON_PARAM *pp = PokeParty_GetMemberPointer( party , i );
        if( MUSICAL_SYSTEM_CheckEntryMusical( pp ) == TRUE )
        {
          num++;
        }
      }
      *ret_wk = num;
    }
  
    break;
  case MUSICAL_VALUE_CHECK_MUSICAL_POKE://ポケモンが参加可能か？
    {
      u8 num = 0;
      u8 i;
      POKEPARTY *party = GAMEDATA_GetMyPokemon(gdata);
      POKEMON_PARAM *pp = PokeParty_GetMemberPointer( party , val );
      if( MUSICAL_SYSTEM_CheckEntryMusical( pp ) == TRUE )
      {
        *ret_wk = 1;
      }
      else
      {
        *ret_wk = 0;
      }
    }
    break;
  case MUSICAL_VALUE_IS_EQUIP_ANY_DRESSUP:      //何か装備しているか？(試着室有効
    {
      u8 i;
      MUSICAL_EQUIP_SAVE *mus_bef_save = MUSICAL_SAVE_GetBefEquipData( musSave );
      *ret_wk = 0;
      for( i=0;i<MUSICAL_ITEM_EQUIP_MAX;i++ )
      {
        if( mus_bef_save->equipData[i].pos != MUS_POKE_EQU_INVALID )
        {
          *ret_wk = 1;
        }
      }
    }
    break;
  case MUSICAL_VALUE_IS_ENABLE_DIST_DATA:
    if( MUSICAL_SAVE_IsEnableDistributData( musSave ) == TRUE )
    {
      *ret_wk = 1;
    }
    else
    {
      *ret_wk = 0;
    }
    break;
  
    //指定グッズを持っているか？
  case MUSICAL_VALUE_CHECK_HAVE_GOODS:
    if( MUSICAL_SAVE_ChackHaveItem( musSave , val ) == TRUE )
    {
      *ret_wk = 1;
    }
    else
    {
      *ret_wk = 0;
    }
    break;
    //--------------------------------------------------------------------
    //以下控え室用
    //--------------------------------------------------------------------
  case MUSICAL_VALUE_WR_SELF_IDX:  //自分の参加番号
    *ret_wk = MUSICAL_EVENT_GetSelfIndex( evWork );
    ARI_TPrintf("MusValWR:SelfIdx[%d]\n",*ret_wk);
    break;
  case MUSICAL_VALUE_WR_MAX_POINT: //最高評価点
    *ret_wk = MUSICAL_EVENT_GetMaxPoint( evWork );
    ARI_TPrintf("MusValWR:MaxPoint[%d]\n",*ret_wk);
    break;
  case MUSICAL_VALUE_WR_MIN_POINT: //最低評価点
    *ret_wk = MUSICAL_EVENT_GetMinPoint( evWork );
    ARI_TPrintf("MusValWR:MinPoint[%d]\n",*ret_wk);
    break;
  case MUSICAL_VALUE_WR_POINT:     //個人得点
    *ret_wk = MUSICAL_EVENT_GetPoint( evWork , val );
    ARI_TPrintf("MusValWR:Point[%d:%d]\n",val,*ret_wk);
    break;
  case MUSICAL_VALUE_WR_GRADE_MSG: //評価メッセージの取得
    break;
  case MUSICAL_VALUE_WR_POS_TO_IDX: //立ち位置に対応した参加番号
    *ret_wk = MUSICAL_EVENT_GetPosIndex( evWork , val );
    ARI_TPrintf("MusValWR:PosIdx[%d:%d]\n",val,*ret_wk);
    break;
  case MUSICAL_VALUE_WR_MAX_CONDITION: //演目の高いコンディション
    *ret_wk = MUSICAL_EVENT_GetMaxCondition( evWork  );
    ARI_TPrintf("MusValWR:MaxCondition[%d]\n",*ret_wk);
    break;
  case MUSICAL_VALUE_WR_POS_TO_RANK://順位に対応した参加番号
    *ret_wk = MUSICAL_EVENT_GetPosToRank( evWork , val );
    ARI_TPrintf("MusValWR:PosRank[%d:%d]\n",val,*ret_wk);
    break;
  case MUSICAL_VALUE_WR_POKE_MAX_POINT_CON: //最も点が高かったコンディション
    *ret_wk = MUSICAL_EVENT_GetMaxPointCondition( evWork , val );
    ARI_TPrintf("MusValWR:MaxPointCon[%d:%d]\n",val,*ret_wk);
    break;
  case MUSICAL_VALUE_WR_POS_TO_OBJ_VIEW:
    *ret_wk = MUSICAL_EVENT_GetPosObjView( evWork , val );
    ARI_TPrintf("MusValWR:ObjView[%d:%d]\n",val,*ret_wk);
    break;
  default:
    GF_ASSERT_MSG( 0 , "タイプ指定が間違ってる[%d]\n",type );
    break;
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ミュージカル：ミュージカルファン用数値取得
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetMusicalFanValue( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;

  u16  pos  = SCRCMD_GetVMWorkValue( core, work );
  u8   type = VMGetU8(core);
  u16* ret_wk = SCRCMD_GetVMWork( core, work );

  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  MUSICAL_SAVE *musSave = GAMEDATA_GetMusicalSavePtr( gdata );
  MUSICAL_FAN_STATE *fanState = MUSICAL_SAVE_GetFanState( musSave , pos );
  
  switch( type )
  {
  case MUSICAL_VALUE_FAN_TYPE:        //見た目
    switch( fanState->type )
    {
    case MCT_COOL:
      *ret_wk = MIDDLEMAN1;
      break;
    case MCT_CUTE:
      *ret_wk = GIRL2;
      break;
    case MCT_ELEGANT:
      *ret_wk = LADY;
      break;
    case MCT_UNIQUE:
      *ret_wk = BOY3;
      break;
    case MCT_MAX:
      *ret_wk = MUSICAL_FAL_TYPE_NONE; // 居ないので0
      break;
    }
    ARI_TPrintf("MusValFan:Type[%d:%d]\n",pos,*ret_wk);
    break;
  case MUSICAL_VALUE_FAN_CHEER_MSG:   //応援メッセージ
    //*ret_wk = msg_musical_fan_cheer_01 + fanState->type;
    *ret_wk = msg_musical_fan_cheer_01 + pos;
    ARI_TPrintf("MusValFan:CheerMsg[%d:%d]\n",pos,*ret_wk);
    break;
  case MUSICAL_VALUE_FAN_GIFT_MSG:    //プレゼントメッセージ
    //*ret_wk = msg_musical_fan_gift_01 + fanState->type;
    *ret_wk = msg_musical_fan_gift_01 + pos;
    ARI_TPrintf("MusValFan:GiftMsg[%d:%d]\n",pos,*ret_wk);
    break;
  case MUSICAL_VALUE_FAN_GIFT_TYPE:   //プレゼント種類
    *ret_wk = fanState->giftType;
    ARI_TPrintf("MusValFan:GiftType[%d:%d]\n",pos,*ret_wk);
    break;
  case MUSICAL_VALUE_FAN_GIFT_NUMBER: //プレゼント番号
    *ret_wk = fanState->giftValue;
    ARI_TPrintf("MusValFan:GiftNumber[%d:%d]\n",pos,*ret_wk);
    break;
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ミュージカル：ミュージカルグッズ追加
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdAddMusicalGoods( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;

  u16  val   = SCRCMD_GetVMWorkValue( core, work );

  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  MUSICAL_SAVE *musSave = GAMEDATA_GetMusicalSavePtr( gdata );

  MUSICAL_SAVE_AddItem( musSave , val );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ミュージカル：ミュージカル系ワードセット
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMusicalWord( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  HEAPID heapId = SCRCMD_WORK_GetHeapID( work );
  WORDSET *wordset    = SCRIPT_GetWordSet( sc );

  u8   type = VMGetU8(core);
  u8   idx  = VMGetU8(core);
  u16  val  = SCRCMD_GetVMWorkValue( core, work );

  switch( type )
  {
  case MUSICAL_WORD_TITLE:        //セーブにある演目
    {
      GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
      MUSICAL_SAVE* musSave = GAMEDATA_GetMusicalSavePtr( gdata );

      STRBUF *tmpBuf = GFL_STR_CreateBuffer( MUSICAL_PROGRAM_NAME_MAX+EOM_SIZE , heapId );

      GFL_STR_SetStringCodeOrderLength( tmpBuf , MUSICAL_SAVE_GetDistributTitle(musSave) , MUSICAL_PROGRAM_NAME_MAX+EOM_SIZE );

      WORDSET_RegisterWord( wordset, idx, tmpBuf, 0, TRUE, PM_LANG );

      GFL_STR_DeleteBuffer( tmpBuf );
    }
    break;
  case MUSICAL_WORD_GOODS:        //グッズ名
    {
      STRBUF * tmpBuf;
      GFL_MSGDATA *msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_mus_item_name_dat , heapId );

      tmpBuf = GFL_MSG_CreateString( msgHandle , ITEM_NAME_000+val );
      WORDSET_RegisterWord( wordset, idx, tmpBuf, 0, TRUE, PM_LANG );

      GFL_STR_DeleteBuffer( tmpBuf );
      GFL_MSG_Delete( msgHandle );
    }
    break;
  case MUSICAL_WORD_TITLE_LOCAL:  //※現在演目
    {
      MUSICAL_SCRIPT_WORK *musScriptWork = GAMEDATA_GetMusicalScrWork( gdata );
      MUSICAL_EVENT_WORK *evWork = musScriptWork->eventWork;
      STRBUF * tmpBuf = MUSICAL_EVENT_CreateStr_ProgramTitle( evWork , heapId );
      WORDSET_RegisterWord( wordset, idx, tmpBuf, 0, TRUE, PM_LANG );
      GFL_STR_DeleteBuffer( tmpBuf );
    }
    break;
  case MUSICAL_WORD_AUDI_TYPE:    //人気客層
    {
      STRBUF * tmpBuf;
      GFL_MSGDATA *msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_musical_common_dat , heapId );

      tmpBuf = GFL_MSG_CreateString( msgHandle , MUSICAL_AUDIENCE_TYPE_01+val );
      WORDSET_RegisterWord( wordset, idx, tmpBuf, 0, TRUE, PM_LANG );

      GFL_STR_DeleteBuffer( tmpBuf );
      GFL_MSG_Delete( msgHandle );
    }
    break;
  case MUSICAL_WORD_CONDITION:  //コンディション名
    {
      STRBUF * tmpBuf;
      GFL_MSGDATA *msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_musical_common_dat , heapId );

      tmpBuf = GFL_MSG_CreateString( msgHandle , MUSICAL_AUDIENCE_CON_01+val );
      WORDSET_RegisterWord( wordset, idx, tmpBuf, 0, TRUE, PM_LANG );

      GFL_STR_DeleteBuffer( tmpBuf );
      GFL_MSG_Delete( msgHandle );
    }
    break;
  case MUSICAL_WORD_NPC_NAME:  //※参加者名前
    {
      MUSICAL_SCRIPT_WORK *musScriptWork = GAMEDATA_GetMusicalScrWork( gdata );
      MUSICAL_EVENT_WORK *evWork = musScriptWork->eventWork;
      
      MUSICAL_EVENT_SetPosCharaName_Wordset( evWork , val , wordset , idx );
    }
    break;
    
  case MUSICAL_WORD_POKE_NAME:
    {
      MUSICAL_SCRIPT_WORK *musScriptWork = GAMEDATA_GetMusicalScrWork( gdata );
      MUSICAL_EVENT_WORK *evWork = musScriptWork->eventWork;
      
      MUSICAL_EVENT_SetPosPokeName_Wordset( evWork , val , wordset , idx );
    }
    break;
  case MUSICAL_WORD_OB_STYLIST:
    {
      GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
      MUSICAL_SAVE* musSave = GAMEDATA_GetMusicalSavePtr( gdata );
      u16 msg[3];
      u8 i,ii,j;
      //ややこしいのでプログラム内処理
      u8 conArr[4];
      u8 pointArr[4];
      
      for( i=0;i<MCT_MAX;i++ )
      {
        conArr[i] = i;
        pointArr[i] = MUSICAL_SAVE_GetBefCondition( musSave , i );
      }
      for( i=1;i<MCT_MAX;i++ )
      {
        for( j=0;j<MCT_MAX-i;j++ )
        {
          const u8 arr1 = j;
          const u8 arr2 = j+1;
          if( pointArr[arr1] < pointArr[arr2] )
          {
            u8 temp = pointArr[arr1];
            pointArr[arr1] = pointArr[arr2];
            pointArr[arr2] = temp;
            temp = conArr[arr1];
            conArr[arr1] = conArr[arr2];
            conArr[arr2] = temp;
          }
        }
      }
      //上位3~4つが一緒
      if( pointArr[0] == pointArr[1] &&
          pointArr[0] == pointArr[2] )
      {
        msg[0] = MUSICAL_AUDIENCE_CON_06;
        msg[1] = MUSICAL_AUDIENCE_CON_05;
      }
      else
      {
        msg[0] = conArr[0]+MUSICAL_AUDIENCE_CON_01;
        //１位が単独で２,３位が一緒
        if( pointArr[1] == pointArr[2] )
        {
          msg[1] = MUSICAL_AUDIENCE_CON_05;
        }
        else
        {
          msg[1] = conArr[1]+MUSICAL_AUDIENCE_CON_01;
        }
      }
      
      //最下位コンディション
      if( pointArr[1] == pointArr[3] )
      {
        msg[2] = conArr[1]+MUSICAL_AUDIENCE_CON_01;
      }
      else
      if( pointArr[2] == pointArr[3] )
      {
        msg[2] = conArr[2]+MUSICAL_AUDIENCE_CON_01;
      }
      else
      {
        msg[2] = conArr[3]+MUSICAL_AUDIENCE_CON_01;
      }
      {
        GFL_MSGDATA *msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_musical_common_dat , heapId );
        for( i=0;i<3;i++ )
        {
          STRBUF * tmpBuf;

          tmpBuf = GFL_MSG_CreateString( msgHandle , msg[i] );
          WORDSET_RegisterWord( wordset, i, tmpBuf, 0, TRUE, PM_LANG );

          GFL_STR_DeleteBuffer( tmpBuf );
        }
        GFL_MSG_Delete( msgHandle );
      }
    }
    break;
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ミュージカル：ミュージカル系ファン贈り物フラグ落とし
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdResetMusicalFanGiftFlg( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;

  u16  pos  = SCRCMD_GetVMWorkValue( core, work );

  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  MUSICAL_SAVE *musSave = GAMEDATA_GetMusicalSavePtr( gdata );
  MUSICAL_FAN_STATE *fanState = MUSICAL_SAVE_GetFanState( musSave , pos );
  
  fanState->giftType = MUSICAL_GIFT_TYPE_NONE;
  return VMCMD_RESULT_CONTINUE;
}

VMCMD_RESULT EvCmdSelectMusicalPoke( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*         ret_decide = SCRCMD_GetVMWork( core, work );       // コマンド第1引数
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );       // コマンド第2引数
  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  
  {
    GMEVENT *event = EVENT_CreatePokeSelectMusical( gsys , ret_decide , ret_wk );
    SCRIPT_CallEvent( scw, event );
  }
  
  return VMCMD_RESULT_SUSPEND;
}

VMCMD_RESULT EvCmdMusicalTools( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  u16  type = SCRCMD_GetVMWorkValue( core, work );
  u16  val1 = SCRCMD_GetVMWorkValue( core, work );
  u16  val2 = SCRCMD_GetVMWorkValue( core, work );
  u16* ret_wk = SCRCMD_GetVMWork( core, work );
  BOOL isNetErr = FALSE;

  MUSICAL_SCRIPT_WORK *musScriptWork = NULL;
  if( type != MUSICAL_TOOL_INIT &&
      type != MUSICAL_TOOL_COMM_CALL_ERROR )
  {
    musScriptWork = GAMEDATA_GetMusicalScrWork( gdata );
    if( musScriptWork->eventWork != NULL )
    {
      MUSICAL_EVENT_CheckNetErr( musScriptWork->eventWork );
      if( MUSICAL_EVENT_IsNetErr( musScriptWork->eventWork ) == TRUE )
      {
        isNetErr = TRUE;
      }
    }
  }

  ARI_TPrintf("ScriptMusTools[%d][%d][%d]\n",type,val1,val2);

  switch( type )
  {
  case MUSICAL_TOOL_INIT: //ミュージカル処理の開始
    {
      EvCmdMusical_InitCommon( sc , gdata );
    }
    break;

  case MUSICAL_TOOL_EXIT: //ミュージカル処理の終了
    {
      EvCmdMusical_ExitCommon( musScriptWork , gdata );
    }
    break;

  case MUSICAL_TOOL_COMM_INIT: //通信開始
    {
      GAME_COMM_SYS_PTR gameComm = GAMESYSTEM_GetGameCommSysPtr( gsys );
      EvCmdMusical_InitCommon_Comm( musScriptWork , gsys , gameComm , val1 );
    }
    break;

  case MUSICAL_TOOL_COMM_EXIT: //通信終了
    {
      EvCmdMusical_ExitCommon_Comm( musScriptWork );
      VMCMD_SetWait( core, EvCmdMusicalWaitExitNet );
      return( VMCMD_RESULT_SUSPEND );
    }
    break;
    
  case MUSICAL_TOOL_COMM_PARENT_CONNECT:
    {
      musScriptWork->scriptRet = ret_wk;
      musScriptWork->entryWork = CommEntryMenu_Setup(
          GAMEDATA_GetMyStatus(gdata),
          fieldmap, 2, 4, HEAPID_PROC,
          COMM_ENTRY_MODE_PARENT,
          COMM_ENTRY_GAMETYPE_MUSICAL, NULL );
      VMCMD_SetWait( core, EvCmdMusicalEntryParent );
      return( VMCMD_RESULT_SUSPEND );
    }
    break;
    
  case MUSICAL_TOOL_COMM_CHILD_CONNECT:
    {
      musScriptWork->scriptRet = ret_wk;
      musScriptWork->entryWork = CommEntryMenu_Setup(
          GAMEDATA_GetMyStatus(gdata),
          fieldmap, 2, 4, HEAPID_PROC, 
          COMM_ENTRY_MODE_CHILD,
          COMM_ENTRY_GAMETYPE_MUSICAL, NULL );
      VMCMD_SetWait( core, EvCmdMusicalEntryChild );
      return( VMCMD_RESULT_SUSPEND );
    }
    break;
  case MUSICAL_TOOL_COMM_TIMESYNC:
    //非通信ならスルー
    if( musScriptWork->commWork != NULL &&
        isNetErr == FALSE )
    {
      musScriptWork->commSyncNo = val1;
      MUS_COMM_SendTimingCommand( musScriptWork->commWork , musScriptWork->commSyncNo );
      VMCMD_SetWait( core, EvCmdMusicalCommTimingSync );
      return( VMCMD_RESULT_SUSPEND );
    }
    break;
  case MUSICAL_TOOL_COMM_WAIT_POST_PROGRAM:
    //非通信ならスルー
    if( musScriptWork->commWork != NULL &&
        isNetErr == FALSE )
    {
      VMCMD_SetWait( core, EvCmdMusicalWaitPostProgram );
      return( VMCMD_RESULT_SUSPEND );
    }
    break;
  case MUSICAL_TOOL_COMM_WAIT_NET_INIT:
    VMCMD_SetWait( core, EvCmdMusicalWaitInitNet );
    return( VMCMD_RESULT_SUSPEND );
    break;
  case MUSICAL_TOOL_COMM_WAIT_POST_ALLPOKE:
    //非通信ならスルー
    if( musScriptWork->commWork != NULL &&
        isNetErr == FALSE )
    {
      VMCMD_SetWait( core, EvCmdMusicalWaitPostAllPoke );
      return( VMCMD_RESULT_SUSPEND );
    }
    
    break;
  case MUSICAL_TOOL_COMM_IR_CONNECT:
    {
      //赤外線受付
      GMEVENT* event;
      EV_IRCPROC_CALL_WORK *cbWork = GFL_HEAP_AllocClearMemoryLo( HEAPID_PROC , sizeof(EV_IRCPROC_CALL_WORK) );
      cbWork->scWork = work;
      cbWork->musScriptWork = musScriptWork;
      musScriptWork->scriptRet = ret_wk;
      musScriptWork->irEntryWork.gamedata = gdata;
      musScriptWork->irEntryWork.netInitWork = MUS_COMM_GetNetInitStruct();
      if( val1 == 0 )
      {
        musScriptWork->irEntryWork.selectType = EVENTIRCBTL_ENTRYMODE_MUSICAL_LEADER;
      }
      else
      {
        musScriptWork->irEntryWork.selectType = EVENTIRCBTL_ENTRYMODE_MUSICAL;
      }
      event = EVENT_FieldSubProc_Callback( gsys, fieldmap, 
                                           FS_OVERLAY_ID(ircbattlematch), 
                                           &IrcBattleMatchProcData, 
                                           &musScriptWork->irEntryWork,
                                           EvCmdIrcEntry_CallBack, 
                                           cbWork );  //←自動開放
      SCRIPT_CallEvent( sc, event );
      return( VMCMD_RESULT_SUSPEND );
    }
    break;
  case MUSICAL_TOOL_COMM_CHECK_ERROR:
    if( musScriptWork->commWork != NULL &&
        isNetErr == TRUE )
    {
      *ret_wk = TRUE;
    }
    else
    {
      *ret_wk = FALSE;
    }
    break;
  case MUSICAL_TOOL_IS_COMM:
    if( musScriptWork->commWork != NULL )
    {
      *ret_wk = TRUE;
    }
    else
    {
      *ret_wk = FALSE;
    }
    break;

  case MUSICAL_TOOL_COMM_CALL_ERROR:
    if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE )
    {
      GAMESYSTEM_SetFieldCommErrorReq( gsys , TRUE );
      //NetErr_App_ReqErrorDisp();
      *ret_wk = TRUE;
    }
    else
    {
      *ret_wk = FALSE;
    }
    break;

  case MUSICAL_TOOL_PRINT:
    ARI_TPrintf("----------------------------\n");
    ARI_TPrintf("ScriptMusTools Print[%d][%d]\n",val1,val2);
    break;

  default:
    ARI_TPrintf("ScriptMusTools InvalidType!!\n");
    break;
  }

  return VMCMD_RESULT_CONTINUE;  
}

#pragma mark [>func
static void EvCmdMusical_InitCommon( SCRIPT_WORK *sc , GAMEDATA *gameData )
{
  MUSICAL_SCRIPT_WORK *musScriptWork = GFL_HEAP_AllocClearMemory( HEAPID_PROC , sizeof( MUSICAL_SCRIPT_WORK ) );
  musScriptWork->eventWork = NULL;
  musScriptWork->commWork = NULL;
  GAMEDATA_SetMusicalScrWork( gameData , musScriptWork );
  GFL_OVERLAY_Load(FS_OVERLAY_ID(musical));
  GFL_OVERLAY_Load(FS_OVERLAY_ID(musical_shot));
#if PM_DEBUG
  MUSICAL_DEBUG_InitDebugMenu( musScriptWork , HEAPID_PROC );
#endif
}
static void EvCmdMusical_ExitCommon( MUSICAL_SCRIPT_WORK *musScriptWork , GAMEDATA *gameData )
{
  GF_ASSERT_MSG( musScriptWork->eventWork == NULL , "イベントワークが開放してない！" );
  GF_ASSERT_MSG( musScriptWork->commWork == NULL , "通信ワークが開放してない！" );
  
#if PM_DEBUG
  MUSICAL_DEBUG_TermDebugMenu();
#endif
  GFL_OVERLAY_Unload(FS_OVERLAY_ID(musical_shot));
  GFL_OVERLAY_Unload(FS_OVERLAY_ID(musical));
  GFL_HEAP_FreeMemory( musScriptWork );
  GAMEDATA_SetMusicalScrWork( gameData , NULL );
}

static void EvCmdMusical_InitCommon_Comm( MUSICAL_SCRIPT_WORK *musScriptWork , GAMESYS_WORK *gsys , GAME_COMM_SYS_PTR gameComm , const BOOL isIrc )
{
  //ここではワークを取れないので、初期化確認でワークをもらう！
  MUS_COMM_InitField( HEAPID_PROC , gsys , gameComm , isIrc );
}
static void EvCmdMusical_ExitCommon_Comm( MUSICAL_SCRIPT_WORK *musScriptWork )
{
  if( musScriptWork->commWork != NULL )
  {
    MUS_COMM_ExitField( musScriptWork->commWork );
    musScriptWork->commWork = NULL;
  }
}


//======================================================================
//  ミュージカル　イベント部分
//======================================================================
//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
static GMEVENT_RESULT event_Musical(
    GMEVENT *event, int *seq, void *work )
{
  EVENT_MUSICAL_WORK *ev_musical_work = work;
  GAMESYS_WORK *gsys =  GMEVENT_GetGameSysWork( event );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  
  switch( (*seq) ){
  case 0:
    GMEVENT_CallEvent( event, ev_musical_work->event );
    (*seq)++;
    break;
  case 1:
    {
      FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
      
      EvCmdMusical_ExitCommon_Comm( ev_musical_work->musScriptWork );

      
      //FIELD_SOUND_PopBGM( fsnd );
      //PMSND_FadeInBGM( 30 );
    }
    (*seq)++;
    break;
  case 2:
    {
      GAMESYS_WORK *gsys =  GMEVENT_GetGameSysWork( event );
      GAME_COMM_SYS_PTR gameComm = GAMESYSTEM_GetGameCommSysPtr( gsys );
      
      if( ev_musical_work->isComm == FALSE ||
          GameCommSys_BootCheck( gameComm ) == GAME_COMM_NO_NULL )
      {
        (*seq)++;
      }
    }
    break;
  case 3:
    if( GFL_NET_IsExit() == TRUE ||
        ev_musical_work->isComm == FALSE )
    {
      if( ev_musical_work->isComm == TRUE )
      {
        //通信復帰処理
        GAMESYS_WORK *gsys =  GMEVENT_GetGameSysWork( event );
        GAMESYSTEM_CommBootAlways(gsys);
      }
      EvCmdMusical_ExitCommon( ev_musical_work->musScriptWork , gdata );
      (*seq)++;
      return( GMEVENT_RES_FINISH );
    }
  }
  return( GMEVENT_RES_CONTINUE );
}
//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
static GMEVENT_RESULT event_Fitting( GMEVENT *event, int *seq, void *work )
{
  EV_FITTING_CALL_WORK *evWork = work;
  GAMESYS_WORK *gsys =  GMEVENT_GetGameSysWork( event );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
  
  switch( (*seq) )
  {
  case 0:
    //GMEVENT_CallEvent(event, EVENT_FSND_PushBGM(gsys, 30));
    (*seq)++;
    break; 
    
  case 1:
    //GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, fieldWork));
    (*seq)++;
    break;
    
  case 2:
    //GMEVENT_CallEvent(event, EVENT_FSND_WaitBGMFade(gsys));
    (*seq)++;
    break;
    
  case 3:
    GAMESYSTEM_CallProc( gsys, NO_OVERLAY_ID, &DressUp_ProcData, evWork->initWork ); 
    (*seq)++;
    break;

  case 4:  // サブプロセス終了待ち
    if( GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL ) break;
    (*seq) ++;
    break;

  case 5:
    //GMEVENT_CallEvent(event, EVENT_FSND_PopBGM(gsys, FSND_FADE_NONE, FSND_FADE_NORMAL));
    (*seq)++;
    break;
  //後はフィールドに任せた！！
  case 6:
    GFL_HEAP_FreeMemory( evWork->initWork->musPoke );
    GFL_HEAP_FreeMemory( evWork->initWork );

    return( GMEVENT_RES_FINISH );
    break;
  }
  return( GMEVENT_RES_CONTINUE );
  
}


static void EvCmdMusicalShotCallProc_CallBack( void* work )
{
  EV_MUSSHOT_CALL_WORK *callWork = work;
  
  // callWorkはPROC呼び出しイベントルーチン内で解放される
  GFL_HEAP_FreeMemory( callWork->initWork );
}

static void EvCmdFittingCallProc_CallBack( CALL_PROC_WORK* cpw )
{
  
  EV_FITTING_CALL_WORK *callWork = (EV_FITTING_CALL_WORK*)cpw->cb_work;
  
  // callWorkはPROC呼び出しイベントルーチン内で解放される
  GFL_HEAP_FreeMemory( callWork->initWork->musPoke );
  //GFL_HEAP_FreeMemory( callWork->initWork );
}


static void EvCmdIrcEntry_CallBack( void* wk )
{
  //ワークは自動開放！
  EV_IRCPROC_CALL_WORK *cbWork = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( cbWork->scWork );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( cbWork->scWork );
  GAME_COMM_SYS_PTR gameComm = GAMESYSTEM_GetGameCommSysPtr( gsys );

  if(( cbWork->musScriptWork->irEntryWork.selectType != EVENTIRCBTL_ENTRYMODE_EXIT )
     &&( cbWork->musScriptWork->irEntryWork.selectType != EVENTIRCBTL_ENTRYMODE_RETRY ))
  {
    *cbWork->musScriptWork->scriptRet = TRUE;
    MUS_COMM_InitAfterIrcConnect( cbWork->musScriptWork->commWork );
  }
  else
  {
    *cbWork->musScriptWork->scriptRet = FALSE;
  }
}

//--------------------------------------------------------------
/**
 * 通信受付メニュー　親
 * @param core VMHANDLE
 * @param wk script work
 * @retval BOOL
 */
//--------------------------------------------------------------
static BOOL EvCmdMusicalEntryParent( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  MUSICAL_SCRIPT_WORK *musScriptWork = GAMEDATA_GetMusicalScrWork( gdata );
  
  const COMM_ENTRY_RESULT entry_ret = CommEntryMenu_Update( musScriptWork->entryWork ); 
  BOOL ret = FALSE;
  
  switch( entry_ret )
  {
  case COMM_ENTRY_RESULT_SUCCESS:      //メンバーが集まった
    (*musScriptWork->scriptRet) = MUSICAL_COMM_ENTRY_PARENT_OK;
    MUS_COMM_InitAfterWirelessConnect( musScriptWork->commWork );
    ret = TRUE;
    break;
  case COMM_ENTRY_RESULT_CANCEL:       //キャンセルして終了
    (*musScriptWork->scriptRet) = MUSICAL_COMM_ENTRY_PARENT_CANCEL;
    ret = TRUE;
    break;
  case COMM_ENTRY_RESULT_ERROR:        //エラーで終了
    (*musScriptWork->scriptRet) = MUSICAL_COMM_ENTRY_PARENT_ERROR;
    ret = TRUE;
    break;
  }

  if( ret == TRUE )
  {
    CommEntryMenu_Exit( musScriptWork->entryWork );
    musScriptWork->entryWork = NULL;
  }
  
  return ret;

}

//--------------------------------------------------------------
/**
 * 通信受付メニュー　子
 * @param core VMHANDLE
 * @param wk script work
 * @retval BOOL
 */
//--------------------------------------------------------------
static BOOL EvCmdMusicalEntryChild( VMHANDLE *core, void *wk )
{
  COMM_ENTRY_RESULT entry_ret;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  MUSICAL_SCRIPT_WORK *musScriptWork = GAMEDATA_GetMusicalScrWork( gdata );

  BOOL ret = FALSE;
  entry_ret = CommEntryMenu_Update( musScriptWork->entryWork ); 
  
  switch( entry_ret )
  {
  case ENTRY_PARENT_ANSWER_OK:      //エントリーOK
    (*musScriptWork->scriptRet) = MUSICAL_COMM_ENTRY_CHILD_OK;
    MUS_COMM_InitAfterWirelessConnect( musScriptWork->commWork );
    ret = TRUE;
    break;
  case ENTRY_PARENT_ANSWER_NG:    //エントリーNG
    (*musScriptWork->scriptRet) = MUSICAL_COMM_ENTRY_CHILD_CANCEL ;
    ret = TRUE;
    break;
  case COMM_ENTRY_RESULT_ERROR:        //エラーで終了
    (*musScriptWork->scriptRet) = MUSICAL_COMM_ENTRY_PARENT_ERROR;
    ret = TRUE;
    break;
  }
  
  if( ret == TRUE )
  {
    CommEntryMenu_Exit( musScriptWork->entryWork );
    musScriptWork->entryWork = NULL;
  }
  return ret;
}

//--------------------------------------------------------------
/**
 * 通信同期待ち
 * @param core VMHANDLE
 * @param wk script work
 * @retval BOOL
 */
//--------------------------------------------------------------
static BOOL EvCmdMusicalCommTimingSync( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  MUSICAL_SCRIPT_WORK *musScriptWork = GAMEDATA_GetMusicalScrWork( gdata );
  
  if( musScriptWork->eventWork != NULL )
  {
    MUSICAL_EVENT_CheckNetErr( musScriptWork->eventWork );
    if( MUSICAL_EVENT_IsNetErr( musScriptWork->eventWork ) == TRUE )
    {
      return TRUE;
    }
  }

  if( MUS_COMM_CheckTimingCommand( musScriptWork->commWork , musScriptWork->commSyncNo ) == TRUE )
  {
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * プログラム受信待ち(音以外
 * @param core VMHANDLE
 * @param wk script work
 * @retval BOOL
 */
//--------------------------------------------------------------
static BOOL EvCmdMusicalWaitPostProgram( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  MUSICAL_SCRIPT_WORK *musScriptWork = GAMEDATA_GetMusicalScrWork( gdata );
  
  MUSICAL_EVENT_CheckNetErr( musScriptWork->eventWork );
  if( MUSICAL_EVENT_IsNetErr( musScriptWork->eventWork ) == TRUE )
  {
    return TRUE;
  }

  if( MUS_COMM_IsPostScriptData( musScriptWork->commWork ) == TRUE )
  {
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ネット初期化待ち
 * @param core VMHANDLE
 * @param wk script work
 * @retval BOOL
 */
//--------------------------------------------------------------
static BOOL EvCmdMusicalWaitInitNet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  GAME_COMM_SYS_PTR gameComm = GAMESYSTEM_GetGameCommSysPtr( gsys );
  MUSICAL_SCRIPT_WORK *musScriptWork = GAMEDATA_GetMusicalScrWork( gdata );

  if( GameCommSys_BootCheck(gameComm) == GAME_COMM_NO_MUSICAL )
  {
    musScriptWork->commWork = GameCommSys_GetAppWork(gameComm);
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ネット終了待ち
 * @param core VMHANDLE
 * @param wk script work
 * @retval BOOL
 */
//--------------------------------------------------------------
static BOOL EvCmdMusicalWaitExitNet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  GAME_COMM_SYS_PTR gameComm = GAMESYSTEM_GetGameCommSysPtr( gsys );
  MUSICAL_SCRIPT_WORK *musScriptWork = GAMEDATA_GetMusicalScrWork( gdata );

  if( musScriptWork->eventWork != NULL )
  {
    MUSICAL_EVENT_CheckNetErr( musScriptWork->eventWork );
    if( MUSICAL_EVENT_IsNetErr( musScriptWork->eventWork ) == TRUE )
    {
      return TRUE;
    }
  }

  if( GameCommSys_BootCheck( gameComm ) == GAME_COMM_NO_NULL )
  {
    return TRUE;
  }
  return FALSE;
}
//--------------------------------------------------------------
/**
 * ぽけ受信待ち
 * @param core VMHANDLE
 * @param wk script work
 * @retval BOOL
 */
//--------------------------------------------------------------
static BOOL EvCmdMusicalWaitPostAllPoke( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  MUSICAL_SCRIPT_WORK *musScriptWork = GAMEDATA_GetMusicalScrWork( gdata );
  
  MUSICAL_EVENT_CheckNetErr( musScriptWork->eventWork );
  if( MUSICAL_EVENT_IsNetErr( musScriptWork->eventWork ) == TRUE )
  {
    return TRUE;
  }

  if( MUS_COMM_IsPostAllPoke( musScriptWork->commWork ) == TRUE )
  {
    return TRUE;
  }
  return FALSE;
}
