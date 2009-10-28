//======================================================================
/**
 * @file  field_skill.c
 * @brief  フィールド技処理（秘伝技など）
 * @author  Hiroyuki Nakamura
 * @date  2005.12.01
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "system/main.h"  //HEAPID_PROC
#include "fieldmap.h"

#include "field_skill.h"
#include "fskill_amaikaori.h"
#include "eventwork.h"

#include "script.h"
#include "../../../resource/fldmapdata/script/hiden_def.h" //script id

//======================================================================
//  define
//======================================================================
#define HSW_MAGIC_NUMBER 0x19740205    //あっきーの誕生日
#define IDXBIT(idx) (1<<(idx)) //インデックス->ビット

//======================================================================
//  typedef
//======================================================================
//--------------------------------------------------------------
/// 使用関数データ
//--------------------------------------------------------------
typedef struct
{
  FLDSKILL_USE_FUNC use_func;
  FLDSKILL_CHECK_FUNC check_func;
}FLDSKILL_FUNC_DATA;

//--------------------------------------------------------------
///秘伝わざスクリプト起動用ワーク
//--------------------------------------------------------------
typedef struct
{
  u32 magic;
  MMDL *mmdl;
  GAMESYS_WORK *gsys;
  FLDSKILL_USE_HEADER head;
}HIDEN_SCR_WORK;

//======================================================================
//  struct
//======================================================================
static GMEVENT_RESULT GMEVENT_Iaigiri(GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT GMEVENT_Naminori(GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT GMEVENT_Takinobori(GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT GMEVENT_Kairiki(GMEVENT *event, int *seq, void *wk );

static FLDSKILL_CHECK_FUNC GetCheckFunc( FLDSKILL_IDX idx );
static FLDSKILL_USE_FUNC GetUseFunc( FLDSKILL_IDX idx );

//static HIDEN_SCR_WORK * CreateHSW(
//    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk );
//static void HSW_Delete(HIDEN_SCR_WORK * hsw);
static void InitHSW( HIDEN_SCR_WORK *hsw,
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk );

static BOOL IsEnableSkill(
    const FLDSKILL_CHECK_WORK *scwk, FLDSKILL_IDX skill_idx );
static BOOL CheckBadge(
    const FLDSKILL_CHECK_WORK *scwk, int badge_id );
static BOOL CheckCompanion( const FLDSKILL_CHECK_WORK *scwk );
static BOOL CheckPark( const FLDSKILL_CHECK_WORK *scwk );
static BOOL CheckPokePark( const FLDSKILL_CHECK_WORK * scwk );
static BOOL CheckMapModeUse( const FLDSKILL_CHECK_WORK * scwk );

static FLDSKILL_RET SkillCheck_Amaikaori( const FLDSKILL_CHECK_WORK * scwk );
static GMEVENT* SkillUse_Amaikaori( const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk );

static const FLDSKILL_FUNC_DATA SkillFuncTable[FLDSKILL_IDX_MAX];

//======================================================================
//  FLDSKILL
//======================================================================
//--------------------------------------------------------------
/**
 * スキル使用チェックワーク初期化
 * @param  fsys  フィールドワーク
 * @param  id    チェックワーク
 * @return  none
 */
//--------------------------------------------------------------
void FLDSKILL_InitCheckWork(
    FIELDMAP_WORK *fieldmap, FLDSKILL_CHECK_WORK *scwk )
{
  MMDL *mmdl;
  FIELD_PLAYER *fld_player;
  MAPATTR fattr,nattr;
  s16 gx,gy,gz;
  VecFx32 pos;
  
  MI_CpuClear8( scwk, sizeof(FLDSKILL_CHECK_WORK) );
  scwk->zone_id = FIELDMAP_GetZoneID( fieldmap );
  scwk->gsys = FIELDMAP_GetGameSysWork( fieldmap );
  scwk->fieldmap = fieldmap;

  fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
  scwk->moveform = FIELD_PLAYER_GetMoveForm( fld_player );
  FIELD_PLAYER_GetFrontGridPos( fld_player, &gx, &gy, &gz );
  
  {
    GAMEDATA *gdata = GAMESYSTEM_GetGameData( scwk->gsys );
    MMDLSYS *mmdlsys = GAMEDATA_GetMMdlSys( gdata );
    mmdl = MMDLSYS_SearchGridPos( mmdlsys, gx, gz, FALSE );
    scwk->front_mmdl = mmdl;
  }
  
  if( mmdl != NULL ){
    switch( MMDL_GetOBJCode(mmdl) ){
    case ROCK:
      scwk->enable_skill |= IDXBIT( FLDSKILL_IDX_KAIRIKI );
      break;
    case TREE:
      scwk->enable_skill |= IDXBIT( FLDSKILL_IDX_IAIGIRI );
      break;
#if 0 //wb null
    case BREAKROCK:
      scwk->enable_skill |= IDXBIT( FLDSKILL_IDX_IWAKUDAKI );
      break;
#endif
    }
  }
  
  {
    FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( fieldmap );
    
    //現在地のアトリビュート
    FIELD_PLAYER_GetPos( fld_player, &pos );
    nattr = MAPATTR_GetAttribute( mapper, &pos );
    
    //自機前のアトリビュート
    MMDL_TOOL_GridPosToVectorPos( gx, gy, gz, &pos );
    fattr = MAPATTR_GetAttribute( mapper, &pos );
  }
  
  if( FIELD_PLAYER_CheckAttrNaminori(fld_player,nattr,fattr) == TRUE ){
    scwk->enable_skill |= IDXBIT( FLDSKILL_IDX_NAMINORI );
  }

#if 0 //wb null
  if (Player_EventAttrCheck_KabeNobori(fattr, Player_DirGet(fsys->player))) {
    scwk->enable_skill |= (1 << FLD_SKILL_ROCKCLIMB);
  }
#endif
  
  {
    MAPATTR_VALUE val = MAPATTR_GetAttrValue( fattr );
    
    if( MAPATTR_VALUE_CheckWaterFall(val) == TRUE ){
      scwk->enable_skill |= IDXBIT( FLDSKILL_IDX_TAKINOBORI );
    }
  }

#if 0 //wb null
  switch (Situation_GetWeatherID(SaveData_GetSituation(fsys->savedata))) {
  case WEATHER_SYS_MIST1:
    scwk->enable_skill |= (1 << FLD_SKILL_KIRIBARAI);
    break;
  case WEATHER_SYS_FLASH:
    scwk->enable_skill |= (1 << FLD_SKILL_FLASH);
    break;
  }
#endif
}

//--------------------------------------------------------------
/**
 * スキル使用チェック
 * @param idx チェックするFLDSKILL_IDX
 * @param scwk FLDSKILL_InitCheckWork()で初期化済みのFLDSKILL_CHECK_WORK
 * @retval FLDSKILL_CHECK_FUNC
 */
//--------------------------------------------------------------
FLDSKILL_RET FLDSKILL_CheckUseSkill(
    FLDSKILL_IDX idx, FLDSKILL_CHECK_WORK *scwk )
{
  FLDSKILL_CHECK_FUNC func = GetCheckFunc( idx );
  return func( scwk );
}

//--------------------------------------------------------------
/**
 * スキル使用ヘッダー初期化 
 * @param head 初期化するFLDSKILL_USE_HEADER
 * @param poke_pos 技を使用する手持ちポケモン位置番号
 * @param use_wazano 使用する技番号
 * @retval
 */
//--------------------------------------------------------------
void FLDSKILL_InitUseHeader( FLDSKILL_USE_HEADER *head,
    u16 poke_pos, u16 use_wazano )
{
  head->poke_pos = poke_pos;
  head->use_wazano = use_wazano;
}

//--------------------------------------------------------------
/**
 * スキル使用
 * @param idx 使用するFLDSKILL_IDX
 * @param head 内容がセットされたFLDSKILL_USE_HEADER
 * @param scwk FLDSKILL_InitCheckWork()で初期化済みのFLDSKILL_CHECK_WORK
 * @retval GMEVENT スキル使用イベント
 */
//--------------------------------------------------------------
GMEVENT * FLDSKILL_UseSkill( FLDSKILL_IDX idx,
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  FLDSKILL_USE_FUNC func = GetUseFunc( idx );
  return func( head, scwk );
}

//======================================================================
//  00 : いあいぎり
//======================================================================
//--------------------------------------------------------------
/**
 * いあいぎり使用チェック
 * @param scwk  FLDSKILL_CHECK_WORK
 * @retval FLDSLILL_RET
 */
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Iaigiri( const FLDSKILL_CHECK_WORK * scwk)
{
  //コロシアム・ユニオンルームチェック
#if 0 //wb 現状無視
  if( CheckMapModeUse(scwk) == TRUE ){
    return FLDSKILL_RET_USE_NG;
  }
#endif

  // バッジチェック
#if 0 //wb 現状無視
  if( CheckBadge(scwk,0) == FALSE ){
    return FLDSKILL_RET_NO_BADGE;
  }
#endif

  if( IsEnableSkill(scwk,FLDSKILL_IDX_IAIGIRI) ){
    return FLDSKILL_RET_USE_OK;
  }
  
  return FLDSKILL_RET_USE_NG;
}

//--------------------------------------------------------------
/**
 * いあいぎり使用
 * @param head FLDSKILL_USE_HEADER
 * @parama  scwk FLDSKILL_CHECK_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT * SkillUse_Iaigiri(
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  GMEVENT *event;
  HIDEN_SCR_WORK *hsw;
  
  event = GMEVENT_Create( scwk->gsys, NULL,
      GMEVENT_Iaigiri, sizeof(HIDEN_SCR_WORK) );
  hsw = GMEVENT_GetEventWork( event );
  InitHSW( hsw, head, scwk );
  return event;
}

//--------------------------------------------------------------
/**
 * いあいぎり呼び出しイベント
 * @param event GMEVENT*
 * @parama seq シーケンス
 * @param wk event work
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_Iaigiri(GMEVENT *event, int *seq, void *wk )
{
  u16 *scwk;
  u16 prm0;
  SCRIPT_WORK *sc;
  HIDEN_SCR_WORK *hsw = wk;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( hsw->gsys );
  EVENTWORK *ev = GAMEDATA_GetEventWork( gdata );
  
  prm0 = hsw->head.poke_pos;
  sc = SCRIPT_ChangeScript( event, SCRID_HIDEN_IAIGIRI_MENU, NULL, 0 );
  SCRIPT_SetScriptWorkParam( sc, prm0, 0, 0, 0 );
  return GMEVENT_RES_CONTINUE;
}

//======================================================================
//  01 : なみのり
//======================================================================
//--------------------------------------------------------------
/**
 * なみのり使用チェック
 * @param scwk  FLDSKILL_CHECK_WORK
 * @retval FLDSLILL_RET
 */
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Naminori( const FLDSKILL_CHECK_WORK * scwk)
{
  //コロシアム・ユニオンルームチェック
#if 0 //wb 現状無視
  if( CheckMapModeUse(scwk) == TRUE ){
    return FLDSKILL_RET_USE_NG;
  }
#endif

  // バッジチェック
#if 0 //wb 現状無視
  if( CheckBadge(scwk,0) == FALSE ){
    return FLDSKILL_RET_NO_BADGE;
  }
#endif
  
  //波乗り中
  if( scwk->moveform == PLAYER_MOVE_FORM_SWIM ){
    return FLDSKILL_RET_PLAYER_SWIM;
  }
  
	// 連れ歩き
#if 0 //wb 現状無視
	if( CompanionCheck( scwk ) == TRUE ){
		return FIELDSKILL_COMPANION;
	}
#endif
  
  if( IsEnableSkill(scwk,FLDSKILL_IDX_NAMINORI) ){
    return FLDSKILL_RET_USE_OK;
  }
  
  return FLDSKILL_RET_USE_NG;
}

//--------------------------------------------------------------
/**
 * なみのり使用
 * @param head FLDSKILL_USE_HEADER
 * @parama  scwk FLDSKILL_CHECK_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT * SkillUse_Naminori(
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  GMEVENT *event;
  HIDEN_SCR_WORK *hsw;
  
  event = GMEVENT_Create( scwk->gsys, NULL,
      GMEVENT_Naminori, sizeof(HIDEN_SCR_WORK) );
  hsw = GMEVENT_GetEventWork( event );
  InitHSW( hsw, head, scwk );
  return event;
}

//--------------------------------------------------------------
/**
 * なみのり呼び出しイベント
 * @param event GMEVENT*
 * @parama seq シーケンス
 * @param wk event work
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_Naminori(GMEVENT *event, int *seq, void *wk )
{
  u16 *scwk;
  u16 prm0;
  SCRIPT_WORK *sc;
  HIDEN_SCR_WORK *hsw = wk;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( hsw->gsys );
  EVENTWORK *ev = GAMEDATA_GetEventWork( gdata );
  
  prm0 = hsw->head.poke_pos;
  sc = SCRIPT_ChangeScript( event, SCRID_HIDEN_NAMINORI_MENU, NULL, 0 );
  SCRIPT_SetScriptWorkParam( sc, prm0, 0, 0, 0 );
  return GMEVENT_RES_CONTINUE;
}

//======================================================================
//  02 : たきのぼり
//======================================================================
//--------------------------------------------------------------
/**
 * たきのぼり使用チェック
 * @param scwk  FLDSKILL_CHECK_WORK
 * @retval FLDSLILL_RET
 */
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Takinobori( const FLDSKILL_CHECK_WORK * scwk)
{
  //コロシアム・ユニオンルームチェック
#if 0 //wb 現状無視
  if( CheckMapModeUse(scwk) == TRUE ){
    return FLDSKILL_RET_USE_NG;
  }
#endif

  // バッジチェック
#if 0 //wb 現状無視
  if( CheckBadge(scwk,0) == FALSE ){
    return FLDSKILL_RET_NO_BADGE;
  }
#endif

  if( IsEnableSkill(scwk,FLDSKILL_IDX_TAKINOBORI) ){
    return FLDSKILL_RET_USE_OK;
  }
  
  return FLDSKILL_RET_USE_NG;
}

//--------------------------------------------------------------
/**
 * たきのぼり使用
 * @param head FLDSKILL_USE_HEADER
 * @parama  scwk FLDSKILL_CHECK_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT * SkillUse_Takinobori(
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  GMEVENT *event;
  HIDEN_SCR_WORK *hsw;
  
  event = GMEVENT_Create( scwk->gsys, NULL,
      GMEVENT_Takinobori, sizeof(HIDEN_SCR_WORK) );
  hsw = GMEVENT_GetEventWork( event );
  InitHSW( hsw, head, scwk );
  return event;
}

//--------------------------------------------------------------
/**
 * たきのぼり呼び出しイベント
 * @param event GMEVENT*
 * @parama seq シーケンス
 * @param wk event work
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_Takinobori(
    GMEVENT *event, int *seq, void *wk )
{
  u16 *scwk;
  u16 prm0;
  SCRIPT_WORK *sc;
  HIDEN_SCR_WORK *hsw = wk;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( hsw->gsys );
  EVENTWORK *ev = GAMEDATA_GetEventWork( gdata );
  
  prm0 = hsw->head.poke_pos;
  sc = SCRIPT_ChangeScript( event, SCRID_HIDEN_TAKINOBORI_MENU, NULL, 0 );
  SCRIPT_SetScriptWorkParam( sc, prm0, 0, 0, 0 );
  return GMEVENT_RES_CONTINUE;
}

//======================================================================
//  03 : かいりき
//======================================================================
//--------------------------------------------------------------
/**
 * かいりき使用チェック
 * @param scwk  FLDSKILL_CHECK_WORK
 * @retval FLDSLILL_RET
 */
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Kairiki( const FLDSKILL_CHECK_WORK * scwk)
{
  //コロシアム・ユニオンルームチェック
#if 0 //wb 現状無視
  if( CheckMapModeUse(scwk) == TRUE ){
    return FLDSKILL_RET_USE_NG;
  }
#endif

  // バッジチェック
#if 0 //wb 現状無視
  if( CheckBadge(scwk,0) == FALSE ){
    return FLDSKILL_RET_NO_BADGE;
  }
#endif

  if( IsEnableSkill(scwk,FLDSKILL_IDX_KAIRIKI) ){
    return FLDSKILL_RET_USE_OK;
  }
  
  return FLDSKILL_RET_USE_NG;
}

//--------------------------------------------------------------
/**
 * かいりき使用
 * @param head FLDSKILL_USE_HEADER
 * @parama  scwk FLDSKILL_CHECK_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT * SkillUse_Kairiki(
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  GMEVENT *event;
  HIDEN_SCR_WORK *hsw;
  
  event = GMEVENT_Create( scwk->gsys, NULL,
      GMEVENT_Kairiki, sizeof(HIDEN_SCR_WORK) );
  hsw = GMEVENT_GetEventWork( event );
  InitHSW( hsw, head, scwk );
  return event;
}

//--------------------------------------------------------------
/**
 * かいりき呼び出しイベント
 * @param event GMEVENT*
 * @parama seq シーケンス
 * @param wk event work
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_Kairiki(
    GMEVENT *event, int *seq, void *wk )
{
  u16 *scwk;
  u16 prm0;
  SCRIPT_WORK *sc;
  HIDEN_SCR_WORK *hsw = wk;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( hsw->gsys );
  EVENTWORK *ev = GAMEDATA_GetEventWork( gdata );
  
  prm0 = hsw->head.poke_pos;
  sc = SCRIPT_ChangeScript( event, SCRID_HIDEN_KAIRIKI_MENU, NULL, 0 );
  SCRIPT_SetScriptWorkParam( sc, prm0, 0, 0, 0 );
  return GMEVENT_RES_CONTINUE;
}

//======================================================================
//  あまいかおり
//======================================================================
//--------------------------------------------------------------
/**
 * あまいかおり使用チェック
 * @param scwk  FLDSKILL_CHECK_WORK
 * @retval FLDSLILL_RET
 */
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Amaikaori( const FLDSKILL_CHECK_WORK * scwk)
{
  //コロシアム・ユニオンルームチェック
#if 0 //wb 現状無視
  if( CheckMapModeUse(scwk) == TRUE ){
    return FLDSKILL_RET_USE_NG;
  }
#endif
  return FLDSKILL_RET_USE_OK;
}

//--------------------------------------------------------------
/**
 * あまいかおり使用
 * @param head FLDSKILL_USE_HEADER
 * @parama  scwk FLDSKILL_CHECK_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT * SkillUse_Amaikaori(
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  GMEVENT *event;
  HIDEN_SCR_WORK *hsw;
  
  event = EVENT_FieldSkillAmaikaori( scwk->gsys, scwk->fieldmap, head->poke_pos );
  return event;
}

//--------------------------------------------------------------
/**
 * ダミー使用チェック
 * @param scwk  FLDSKILL_CHECK_WORK
 * @retval FLDSLILL_RET
 */
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Dummy( const FLDSKILL_CHECK_WORK * scwk)
{
  return FLDSKILL_RET_USE_NG;
}

//--------------------------------------------------------------
/**
 * ダミー使用
 * @param head FLDSKILL_USE_HEADER
 * @parama  scwk FLDSKILL_CHECK_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT * SkillUse_Dummy(
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  GMEVENT *event;
  HIDEN_SCR_WORK *hsw;
  
  //ダミーで甘い香りをあててます
  event = EVENT_FieldSkillAmaikaori( scwk->gsys, scwk->fieldmap, head->poke_pos );
  return event;
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * スキル使用チェック関数取得
 * @param  id    関数ID
 * @return  FLDSKILL_CHECK_FUNC
 */
//--------------------------------------------------------------
static FLDSKILL_CHECK_FUNC GetCheckFunc( FLDSKILL_IDX idx )
{
  GF_ASSERT( idx < FLDSKILL_IDX_MAX );
  return SkillFuncTable[idx].check_func;
}

//--------------------------------------------------------------
/**
 * スキル使用チェック関数取得
 * @param  id    関数ID
 * @return FLDSKILL_USE_FUNC
 */
//--------------------------------------------------------------
static FLDSKILL_USE_FUNC GetUseFunc( FLDSKILL_IDX idx )
{
  GF_ASSERT( idx < FLDSKILL_IDX_MAX );
  return SkillFuncTable[idx].use_func;
}

//--------------------------------------------------------------
/**
 * 秘伝わざスクリプト起動用ワーク生成
 * @param head FLDSKILL_USE_HEADER
 * @param scwk FLDSKILL_CHECK_WORK
 * @retval HIDEN_SCR_WORK*
 */
//--------------------------------------------------------------
static void InitHSW( HIDEN_SCR_WORK *hsw,
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  hsw->magic = HSW_MAGIC_NUMBER;
  hsw->mmdl = scwk->front_mmdl;
  hsw->head = *head;
  hsw->gsys = scwk->gsys;
}

//--------------------------------------------------------------
/**
 * 秘伝わざスクリプト起動用ワーク消去
 * @param hsw HIDEN_SCR_WORK
 * @retval none
 */
//--------------------------------------------------------------
#if 0 //wb null
static void HSW_Delete(HIDEN_SCR_WORK * hsw)
{
  GF_ASSERT( hsw->magic == HSW_MAGIC_NUMBER );
  GFL_HEAP_FreeMemory( hsw );
}
#endif

//--------------------------------------------------------------
/**
 * スキル使用可能チェック
 * @param scwk FLDSKILL_CHECK_WORK
 * @param skill_idx FLDSKILL_IDX
 * @retval BOOL TRUE=使用可能
 */
//--------------------------------------------------------------
static BOOL IsEnableSkill(
    const FLDSKILL_CHECK_WORK *scwk, FLDSKILL_IDX skill_idx )
{
  if( (scwk->enable_skill & IDXBIT(skill_idx)) != 0 ){
    return TRUE;
  }else{
    return FALSE;
  }
}

//--------------------------------------------------------------
/**
 * バッジチェック
 * @param scwk FLDSKILL_CHECK_WORK
 * @param badge バッジID
 * @retval BOOL TRUE=バッジあり
 */
//--------------------------------------------------------------
static BOOL CheckBadge(
    const FLDSKILL_CHECK_WORK *scwk, int badge_id )
{
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( scwk->gsys );
  MYSTATUS *mystatus = GAMEDATA_GetMyStatus( gdata );
  return MyStatus_GetBadgeFlag( mystatus, badge_id );
}

//--------------------------------------------------------------
/**
 * 連れ歩きチェック
 * @param scwk FLDSKILL_CHECK_WORK
 * @retval TRUE=連れ歩き
 */
//--------------------------------------------------------------
static BOOL CheckCompanion( const FLDSKILL_CHECK_WORK *scwk )
{
#if 0 //wb null
  return SysFlag_PairCheck( SaveData_GetEventWork(scwk->fsys->savedata) );
#else
  return FALSE;
#endif
}

//--------------------------------------------------------------
/**
 * サファリ・ポケパークチェック
 * @param scwk FLDSKILL_CHECK_WORK
 * @retval TRUE=サファリ・ポケパーク
 */
//--------------------------------------------------------------
static BOOL CheckPark( const FLDSKILL_CHECK_WORK *scwk )
{
#if 0 //wb null
  if( SysFlag_SafariCheck(
        SaveData_GetEventWork(scwk->fsys->savedata) ) == TRUE ||
    SysFlag_PokeParkCheck(
      SaveData_GetEventWork(scwk->fsys->savedata)) == TRUE ){
    return TRUE;
  }
  return FALSE;
#else
  return FALSE;
#endif
}

//--------------------------------------------------------------
/**
 * ポケパークチェック
 * @param scwk FLDSKILL_CHECK_WORK
 * @retval TRUE=ポケパーク
 */
//--------------------------------------------------------------
static BOOL CheckPokePark( const FLDSKILL_CHECK_WORK * scwk )
{
#if 0 //wb null
  if( SysFlag_PokeParkCheck(
        SaveData_GetEventWork(scwk->fsys->savedata)) == TRUE ){
    return TRUE;
  }
  return FALSE;
#else
  return FALSE;
#endif
}

//--------------------------------------------------------------
/**
 * コロシアム・ユニオンルームチェック
 * @param scwk FLDSKILL_CHECK_WORK
 * @retval TRUE=コロシアム・ユニオンルーム
 */
//--------------------------------------------------------------
static BOOL CheckMapModeUse( const FLDSKILL_CHECK_WORK * scwk )
{
#if 0 //wb null
  if( scwk->fsys->MapMode == MAP_MODE_COLOSSEUM || scwk->fsys->MapMode == MAP_MODE_UNION ){
    return FALSE;
  }
  return TRUE;
#else
  return FALSE;
#endif
}

//======================================================================
//  data
//======================================================================
//--------------------------------------------------------------
/* 使用関数テーブル
 *
 * 引数として渡される FLDSKILL_USE_HEADER *head と FLDSKILL_CHECK_WORK *scwkは
 * 呼び出し側で自動変数として定義されているので、SkillUse_XXXXで登録する
 * イベント中でメンバの値を参照したい場合、値のコピーを取って使うこと！
 */
//--------------------------------------------------------------
static const FLDSKILL_FUNC_DATA SkillFuncTable[FLDSKILL_IDX_MAX] =
{
  {SkillUse_Iaigiri,SkillCheck_Iaigiri},    // 00 : いあいぎり
  {SkillUse_Naminori,SkillCheck_Naminori},    // 01 :なみのり 
  {SkillUse_Takinobori,SkillCheck_Takinobori},    // 02 :たきのぼり
  {SkillUse_Kairiki,SkillCheck_Kairiki},    // 03 :かいりき
  
  {SkillUse_Dummy,SkillCheck_Dummy},    // 04 :そらをとぶ
  {SkillUse_Dummy,SkillCheck_Dummy},    // 05 :きりばらい
  {SkillUse_Dummy,SkillCheck_Dummy},    // 06 :いわくだき
  {SkillUse_Dummy,SkillCheck_Dummy},    // 07 :ロッククライム
  {SkillUse_Dummy,SkillCheck_Dummy},    // 08 :フラッシュ
  {SkillUse_Dummy,SkillCheck_Dummy},    // 09 :テレポート
  {SkillUse_Dummy,SkillCheck_Dummy},    // 10 :あなをほる
  {SkillUse_Amaikaori,SkillCheck_Amaikaori},   // 11 :あまいかおり
  {SkillUse_Dummy,SkillCheck_Dummy},    // 12 :おしゃべり
};
