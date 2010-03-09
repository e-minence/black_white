//======================================================================
/**
 * @file  field_skill.c
 * @brief  �t�B�[���h�Z�����i��`�Z�Ȃǁj
 * @author  Hiroyuki Nakamura
 * @date  2005.12.01
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "system/main.h"  //HEAPID_PROC
#include "fieldmap.h"

#include "field_status_local.h"
#include "field_skill.h"
#include "fskill_amaikaori.h"
#include "fskill_sorawotobu.h"
#include "fieldskill_mapeff.h"
#include "fskill_osyaberi.h"
#include "eventwork.h"
#include "field/zonedata.h"

#include "script.h"
#include "../../../resource/fldmapdata/script/hiden_def.h" //script id

#include "event_mapchange.h"  //EVENT_ChangeMapByTeleport
#include "field_diving_data.h"  //DIVINGSPOT_Check

//======================================================================
//  define
//======================================================================
#define HSW_MAGIC_NUMBER 0x19740205    //�������[�̒a����
#define IDXBIT(idx) (1<<(idx)) //�C���f�b�N�X->�r�b�g

//======================================================================
//  typedef
//======================================================================
//--------------------------------------------------------------
/// �g�p�֐��f�[�^
//--------------------------------------------------------------
typedef struct
{
  FLDSKILL_USE_FUNC use_func;
  FLDSKILL_CHECK_FUNC check_func;
}FLDSKILL_FUNC_DATA;

//--------------------------------------------------------------
///��`�킴�X�N���v�g�N���p���[�N
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
static GMEVENT_RESULT GMEVENT_Flash(GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT GMEVENT_Anawohoru(GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT GMEVENT_Teleport(GMEVENT *event, int *seq, void *wk );

static FLDSKILL_CHECK_FUNC GetCheckFunc( FLDSKILL_IDX idx );
static FLDSKILL_USE_FUNC GetUseFunc( FLDSKILL_IDX idx );

//static HIDEN_SCR_WORK * CreateHSW(
//    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk );
//static void HSW_Delete(HIDEN_SCR_WORK * hsw);
static void InitHSW( HIDEN_SCR_WORK *hsw,
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk );

static BOOL IsEnableSkill(
    const FLDSKILL_CHECK_WORK *scwk, FLDSKILL_IDX skill_idx );
static BOOL CheckPark( const FLDSKILL_CHECK_WORK *scwk );
static BOOL CheckPokePark( const FLDSKILL_CHECK_WORK * scwk );
static BOOL CheckMapModeUse( const FLDSKILL_CHECK_WORK * scwk );

static FLDSKILL_RET SkillCheck_Amaikaori( const FLDSKILL_CHECK_WORK * scwk );
static GMEVENT* SkillUse_Amaikaori( const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk );
static GMEVENT* SkillUse_Sorawotobu( const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk );

static const FLDSKILL_FUNC_DATA SkillFuncTable[FLDSKILL_IDX_MAX];

//======================================================================
//  FLDSKILL
//======================================================================
//--------------------------------------------------------------
/**
 * �X�L���g�p�`�F�b�N���[�N������
 * @param  fsys  �t�B�[���h���[�N
 * @param  id    �`�F�b�N���[�N
 * @return  none
 *
 * @note
 * ���j���[���Ȃǂł̓t�B�[���h�}�b�v���Ȃ����肪����Ȃ��߁A
 * �t�B�[���h�킴���g�p�\�ȏ󋵂��ǂ��������O�Ƀ`�F�b�N���Ă���
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
  
  // �ڂ̑O�̃��f�����擾
  mmdl = FIELD_PLAYER_GetFrontMMdl( fld_player );
  scwk->front_mmdl = mmdl;
  
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
    u16 dir = FIELD_PLAYER_GetDir( fld_player );
    
    //���ݒn�̃A�g���r���[�g
    FIELD_PLAYER_GetPos( fld_player, &pos );
    nattr = MAPATTR_GetAttribute( mapper, &pos );
    
    //���@�O�̃A�g���r���[�g
    fattr = FIELD_PLAYER_GetDirMapAttr( fld_player, dir );
  }
  
  if( FIELD_PLAYER_CheckAttrNaminori(fld_player,nattr,fattr) == TRUE ){
    scwk->enable_skill |= IDXBIT( FLDSKILL_IDX_NAMINORI );
  }

#if 0 //wb null
  if (Player_EventAttrCheck_KabeNobori(fattr, Player_DirGet(fsys->player))) {
    scwk->enable_skill |= (1 << FLD_SKILL_ROCKCLIMB);
  }
#endif
  
  //if (AREADATA_GetInnerOuterSwitch( FIELDMAP_GetAreaData( fieldmap ) ) != 0 )
  if ( ZONEDATA_FlyEnable( scwk->zone_id ) == TRUE )
  {
    scwk->enable_skill |= IDXBIT( FLDSKILL_IDX_SORAWOTOBU );
    scwk->enable_skill |= IDXBIT( FLDSKILL_IDX_TELEPORT );
  }
  if ( ZONEDATA_EscapeEnable( scwk->zone_id ) == TRUE )
  {
    scwk->enable_skill |= IDXBIT( FLDSKILL_IDX_ANAWOHORU );
  }

  {
    MAPATTR_VALUE val = MAPATTR_GetAttrValue( fattr );
    
    if( MAPATTR_VALUE_CheckWaterFall(val) == TRUE ){
      scwk->enable_skill |= IDXBIT( FLDSKILL_IDX_TAKINOBORI );
    }
  }

  {
    GAMEDATA *gdata = GAMESYSTEM_GetGameData( scwk->gsys );
    FIELD_STATUS * fldstatus = GAMEDATA_GetFieldStatus( gdata );
    u32 mapeffmsk = FIELD_STATUS_GetFieldSkillMapEffectMsk( fldstatus );

    // �t���b�V���`�F�b�N
    if( FIELDSKILL_MAPEFF_MSK_IS_ON(mapeffmsk, FIELDSKILL_MAPEFF_MSK_FLASH_NEAR) )
    {
      scwk->enable_skill |= (1 << FLDSKILL_IDX_FLASH);
    }
  }

  { //�����Ƀ_�C�r���O�̎g�p�\���`�F�b�N��ǉ�����
    u16 zone_id;
    if( DIVINGSPOT_Check( fieldmap, &zone_id ) == TRUE )
    {
      scwk->enable_skill |= ( 1 << FLDSKILL_IDX_DIVING );
    }
  }

  //�p���X�����t�B�[���h�ɂ���Ƃ��́A���ׂẴt�B�[���h�Z���g���Ȃ��悤�ɂ���
  if ( GAMEDATA_GetIntrudeReverseArea( GAMESYSTEM_GetGameData( scwk->gsys ) ) == TRUE )
  {
    scwk->enable_skill = 0;
  }
}

//--------------------------------------------------------------
/**
 * �X�L���g�p�`�F�b�N
 * @param idx �`�F�b�N����FLDSKILL_IDX
 * @param scwk FLDSKILL_InitCheckWork()�ŏ������ς݂�FLDSKILL_CHECK_WORK
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
 * �X�L���g�p�w�b�_�[������ 
 * @param head ����������FLDSKILL_USE_HEADER
 * @param poke_pos �Z���g�p����莝���|�P�����ʒu�ԍ�
 * @param use_wazano �g�p����Z�ԍ�
 * @retval
 */
//--------------------------------------------------------------
void FLDSKILL_InitUseHeader( FLDSKILL_USE_HEADER *head,
    u16 poke_pos, u16 use_wazano,
    u32 zoneID, u32 inGridX, u32 inGridY, u32 inGridZ)
{
  head->poke_pos = poke_pos;
  head->use_wazano = use_wazano;
  head->zoneID = zoneID;
  head->GridX = inGridX;    //@todo 20091120���ݖ��g�p
  head->GridY = inGridY;    //@todo 20091120���ݖ��g�p
  head->GridZ = inGridZ;    //@todo 20091120���ݖ��g�p
}

//--------------------------------------------------------------
/**
 * �X�L���g�p
 * @param idx �g�p����FLDSKILL_IDX
 * @param head ���e���Z�b�g���ꂽFLDSKILL_USE_HEADER
 * @param scwk FLDSKILL_InitCheckWork()�ŏ������ς݂�FLDSKILL_CHECK_WORK
 * @retval GMEVENT �X�L���g�p�C�x���g
 */
//--------------------------------------------------------------
GMEVENT * FLDSKILL_UseSkill( FLDSKILL_IDX idx,
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  FLDSKILL_USE_FUNC func = GetUseFunc( idx );
  return func( head, scwk );
}

//======================================================================
//  00 : ����������
//======================================================================
//--------------------------------------------------------------
/**
 * ����������g�p�`�F�b�N
 * @param scwk  FLDSKILL_CHECK_WORK
 * @retval FLDSLILL_RET
 */
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Iaigiri( const FLDSKILL_CHECK_WORK * scwk)
{
  //�R���V�A���E���j�I�����[���`�F�b�N
#if 0 //wb ���󖳎�
  if( CheckMapModeUse(scwk) == TRUE ){
    return FLDSKILL_RET_USE_NG;
  }
#endif

  if( IsEnableSkill(scwk,FLDSKILL_IDX_IAIGIRI) ){
    return FLDSKILL_RET_USE_OK;
  }
  
  return FLDSKILL_RET_USE_NG;
}

//--------------------------------------------------------------
/**
 * ����������g�p
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
 * ����������Ăяo���C�x���g
 * @param event GMEVENT*
 * @parama seq �V�[�P���X
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
//  01 : �Ȃ݂̂�
//======================================================================
//--------------------------------------------------------------
/**
 * �Ȃ݂̂�g�p�`�F�b�N
 * @param scwk  FLDSKILL_CHECK_WORK
 * @retval FLDSLILL_RET
 */
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Naminori( const FLDSKILL_CHECK_WORK * scwk)
{
  //�R���V�A���E���j�I�����[���`�F�b�N
#if 0 //wb ���󖳎�
  if( CheckMapModeUse(scwk) == TRUE ){
    return FLDSKILL_RET_USE_NG;
  }
#endif

  //�g��蒆
  if( scwk->moveform == PLAYER_MOVE_FORM_SWIM ){
    return FLDSKILL_RET_PLAYER_SWIM;
  }
  
  if( IsEnableSkill(scwk,FLDSKILL_IDX_NAMINORI) ){
    return FLDSKILL_RET_USE_OK;
  }
  
  return FLDSKILL_RET_USE_NG;
}

//--------------------------------------------------------------
/**
 * �Ȃ݂̂�g�p
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
 * �Ȃ݂̂�Ăяo���C�x���g
 * @param event GMEVENT*
 * @parama seq �V�[�P���X
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
//  02 : �����̂ڂ�
//======================================================================
//--------------------------------------------------------------
/**
 * �����̂ڂ�g�p�`�F�b�N
 * @param scwk  FLDSKILL_CHECK_WORK
 * @retval FLDSLILL_RET
 */
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Takinobori( const FLDSKILL_CHECK_WORK * scwk)
{
  //�R���V�A���E���j�I�����[���`�F�b�N
#if 0 //wb ���󖳎�
  if( CheckMapModeUse(scwk) == TRUE ){
    return FLDSKILL_RET_USE_NG;
  }
#endif

  if( IsEnableSkill(scwk,FLDSKILL_IDX_TAKINOBORI) ){
    return FLDSKILL_RET_USE_OK;
  }
  
  return FLDSKILL_RET_USE_NG;
}

//--------------------------------------------------------------
/**
 * �����̂ڂ�g�p
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
 * �����̂ڂ�Ăяo���C�x���g
 * @param event GMEVENT*
 * @parama seq �V�[�P���X
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
//  03 : �����肫
//======================================================================
//--------------------------------------------------------------
/**
 * �����肫�g�p�`�F�b�N
 * @param scwk  FLDSKILL_CHECK_WORK
 * @retval FLDSLILL_RET
 */
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Kairiki( const FLDSKILL_CHECK_WORK * scwk)
{
  //�R���V�A���E���j�I�����[���`�F�b�N
#if 0 //wb ���󖳎�
  if( CheckMapModeUse(scwk) == TRUE ){
    return FLDSKILL_RET_USE_NG;
  }
#endif

  if( IsEnableSkill(scwk,FLDSKILL_IDX_KAIRIKI) ){
    return FLDSKILL_RET_USE_OK;
  }
  
  return FLDSKILL_RET_USE_NG;
}

//--------------------------------------------------------------
/**
 * �����肫�g�p
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
 * �����肫�Ăяo���C�x���g
 * @param event GMEVENT*
 * @parama seq �V�[�P���X
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
//  ���܂�������
//======================================================================
//--------------------------------------------------------------
/**
 * ���܂�������g�p�`�F�b�N
 * @param scwk  FLDSKILL_CHECK_WORK
 * @retval FLDSLILL_RET
 */
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Amaikaori( const FLDSKILL_CHECK_WORK * scwk)
{
  //�R���V�A���E���j�I�����[���`�F�b�N
#if 0 //wb ���󖳎�
  if( CheckMapModeUse(scwk) == TRUE ){
    return FLDSKILL_RET_USE_NG;
  }
#endif
  return FLDSKILL_RET_USE_OK;
}

//--------------------------------------------------------------
/**
 * ���܂�������g�p
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

//======================================================================
//  ������Ƃ�
//======================================================================

//--------------------------------------------------------------
/**
 * ������ƂԎg�p
 * @param head FLDSKILL_USE_HEADER
 * @parama  scwk FLDSKILL_CHECK_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT * SkillUse_Sorawotobu(
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  GMEVENT *event;
  HIDEN_SCR_WORK *hsw;
  
  event = EVENT_FieldSkillSorawotobu( scwk->gsys, scwk->fieldmap, head->zoneID);
  return event;
}

//--------------------------------------------------------------
/**
 * ������ƂԎg�p�`�F�b�N
 * @param scwk  FLDSKILL_CHECK_WORK
 * @retval FLDSLILL_RET
 */
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Sorawotobu( const FLDSKILL_CHECK_WORK * scwk)
{
  //�R���V�A���E���j�I�����[���`�F�b�N
#if 0 //wb ���󖳎�
  if( CheckMapModeUse(scwk) == TRUE ){
    return FLDSKILL_RET_USE_NG;
  }
#endif

  if( IsEnableSkill(scwk,FLDSKILL_IDX_SORAWOTOBU) ){
    return FLDSKILL_RET_USE_OK;
  }
  
  return FLDSKILL_RET_USE_NG;
}

//======================================================================
//  �t���b�V��
//======================================================================

//--------------------------------------------------------------
/**
 * �t���b�V���g�p�`�F�b�N
 * @param scwk  FLDSKILL_CHECK_WORK
 * @retval FLDSLILL_RET
 */
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Flash( const FLDSKILL_CHECK_WORK * scwk)
{
#if 0
	// �R���V�A���E���j�I�����[���`�F�b�N
	if( MapModeUseChack( scwk ) == FALSE ){
		return FIELDSKILL_USE_FALSE;
	}
#endif

	if (IsEnableSkill(scwk, FLDSKILL_IDX_FLASH)) {
		return FLDSKILL_RET_USE_OK;
	} else {
		return FLDSKILL_RET_USE_NG;
	}
}

//--------------------------------------------------------------
/**
 * �t���b�V���g�p
 * @param head FLDSKILL_USE_HEADER
 * @parama  scwk FLDSKILL_CHECK_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT * SkillUse_Flash(
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  GMEVENT *event;
  HIDEN_SCR_WORK *hsw;
  
  event = GMEVENT_Create( scwk->gsys, NULL,
      GMEVENT_Flash, sizeof(HIDEN_SCR_WORK) );
  hsw = GMEVENT_GetEventWork( event );
  InitHSW( hsw, head, scwk );
  return event;
}

//--------------------------------------------------------------
/**
 * �t���b�V���Ăяo���C�x���g
 * @param event GMEVENT*
 * @parama seq �V�[�P���X
 * @param wk event work
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_Flash(GMEVENT *event, int *seq, void *wk )
{
  u16 *scwk;
  u16 prm0;
  SCRIPT_WORK *sc;
  HIDEN_SCR_WORK *hsw = wk;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( hsw->gsys );
  EVENTWORK *ev = GAMEDATA_GetEventWork( gdata );
  
/*
  prm0 = hsw->head.poke_pos;
  sc = SCRIPT_ChangeScript( event, SCRID_HIDEN_FLASH, NULL, 0 );
  SCRIPT_SetScriptWorkParam( sc, prm0, 0, 0, 0 );
  return GMEVENT_RES_CONTINUE;
//*/


  {
    FIELD_STATUS * fldstatus = GAMEDATA_GetFieldStatus( gdata );
    FIELDSKILL_MAPEFF* mapeff = FIELDMAP_GetFieldSkillMapEffect( GAMESYSTEM_GetFieldMapWork( hsw->gsys ) );
    FIELD_FLASH* flash = FIELDSKILL_MAPEFF_GetFlash( mapeff );

    switch( *seq )
    {
    case 0:
        {
          u16 prm0;
          SCRIPT_WORK *sc;
          
          prm0 = hsw->head.poke_pos;
          sc = SCRIPT_CallScript( event, SCRID_HIDEN_FLASH_MENU, NULL, NULL, HEAPID_FIELDMAP );
          SCRIPT_SetScriptWorkParam( sc, prm0, 0, 0, 0 );
        }
        (*seq) ++;
        break;
    case 1:
      // ON
      if( !FIELD_STATUS_IsFieldSkillFlash(fldstatus) )
      {
        FIELD_STATUS * p_fldstatus = GAMEDATA_GetFieldStatus( gdata );
        u32 msk;

        FIELD_STATUS_SetFieldSkillFlash( fldstatus, TRUE );
        FIELD_FLASH_Control( flash, FIELD_FLASH_REQ_FADEOUT );

        msk = FIELD_STATUS_GetFieldSkillMapEffectMsk( p_fldstatus );
        FIELDSKILL_MAPEFF_MSK_OFF( msk, FIELDSKILL_MAPEFF_MSK_FLASH_NEAR );
        FIELDSKILL_MAPEFF_MSK_ON( msk, FIELDSKILL_MAPEFF_MSK_FLASH_FAR );
        FIELD_STATUS_SetFieldSkillMapEffectMsk( p_fldstatus, msk );
      }
      else
      {
        // �����L�����Ă���B
        return GMEVENT_RES_FINISH;
      }
      (*seq) ++;
      break;

    case 2:
      if( FIELD_FLASH_GetStatus( flash ) == FIELD_FLASH_STATUS_FAR )
      {
        return GMEVENT_RES_FINISH;  // ����
      }
      break;
    }
  }

  return GMEVENT_RES_CONTINUE;
}

//======================================================================
//  ���Ȃ��ق�
//======================================================================

//--------------------------------------------------------------
/**
 * ���Ȃ��ق�g�p�`�F�b�N
 * @param scwk  FLDSKILL_CHECK_WORK
 * @retval FLDSLILL_RET
 */
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Anawohoru( const FLDSKILL_CHECK_WORK * scwk)
{
#if 0
	// �R���V�A���E���j�I�����[���`�F�b�N
	if( MapModeUseChack( scwk ) == FALSE ){
		return FIELDSKILL_USE_FALSE;
	}
#endif

	if (IsEnableSkill(scwk, FLDSKILL_IDX_ANAWOHORU)) {
		return FLDSKILL_RET_USE_OK;
	} else {
		return FLDSKILL_RET_USE_NG;
	}
}

//--------------------------------------------------------------
/**
 * ���Ȃ��ق�g�p
 * @param head FLDSKILL_USE_HEADER
 * @parama  scwk FLDSKILL_CHECK_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT * SkillUse_Anawohoru(
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  GMEVENT *event;
  HIDEN_SCR_WORK *hsw;
  
  event = GMEVENT_Create( scwk->gsys, NULL,
      GMEVENT_Anawohoru, sizeof(HIDEN_SCR_WORK) );
  hsw = GMEVENT_GetEventWork( event );
  InitHSW( hsw, head, scwk );
  return event;
}

//--------------------------------------------------------------
/**
 * ���Ȃ��ق�Ăяo���C�x���g
 * @param event GMEVENT*
 * @parama seq �V�[�P���X
 * @param wk event work
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_Anawohoru(GMEVENT *event, int *seq, void *wk )
{
  HIDEN_SCR_WORK *hsw = wk;
  switch (*seq)
  {
  case 0:
    {
      u16 prm0;
      SCRIPT_WORK *sc;
      
      prm0 = hsw->head.poke_pos;
      sc = SCRIPT_CallScript( event, SCRID_HIDEN_ANAWOHORU_MENU, NULL, NULL, HEAPID_FIELDMAP );
      SCRIPT_SetScriptWorkParam( sc, prm0, 0, 0, 0 );
    }
    ++(*seq);
    break;
  case 1:
    {
      GMEVENT *next_event = EVENT_ChangeMapByAnawohoru( hsw->gsys );
      GMEVENT_ChangeEvent( event, next_event );
    }
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

//======================================================================
//  �e���|�[�g
//======================================================================

//--------------------------------------------------------------
/**
 * �e���|�[�g�g�p�`�F�b�N
 * @param scwk  FLDSKILL_CHECK_WORK
 * @retval FLDSLILL_RET
 */
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Teleport( const FLDSKILL_CHECK_WORK * scwk)
{
#if 0
	// �R���V�A���E���j�I�����[���`�F�b�N
	if( MapModeUseChack( scwk ) == FALSE ){
		return FIELDSKILL_USE_FALSE;
	}
#endif

	if (IsEnableSkill(scwk, FLDSKILL_IDX_TELEPORT)) {
		return FLDSKILL_RET_USE_OK;
	} else {
		return FLDSKILL_RET_USE_NG;
	}
}

//--------------------------------------------------------------
/**
 * �e���|�[�g�g�p
 * @param head FLDSKILL_USE_HEADER
 * @parama  scwk FLDSKILL_CHECK_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT * SkillUse_Teleport(
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  GMEVENT *event;
  HIDEN_SCR_WORK *hsw;
  
  event = GMEVENT_Create( scwk->gsys, NULL,
      GMEVENT_Teleport, sizeof(HIDEN_SCR_WORK) );
  hsw = GMEVENT_GetEventWork( event );
  InitHSW( hsw, head, scwk );
  return event;
}

//--------------------------------------------------------------
/**
 * �e���|�[�g�Ăяo���C�x���g
 * @param event GMEVENT*
 * @parama seq �V�[�P���X
 * @param wk event work
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_Teleport(GMEVENT *event, int *seq, void *wk )
{
  HIDEN_SCR_WORK *hsw = wk;
  switch (*seq)
  {
  case 0:
    {
      u16 prm0;
      SCRIPT_WORK *sc;
      
      prm0 = hsw->head.poke_pos;
      sc = SCRIPT_CallScript( event, SCRID_HIDEN_TELEPORT_MENU, NULL, NULL, HEAPID_FIELDMAP );
      SCRIPT_SetScriptWorkParam( sc, prm0, 0, 0, 0 );
    }
    ++(*seq);
    break;
  case 1:
    {
      GMEVENT *next_event = EVENT_ChangeMapByTeleport( hsw->gsys );
      GMEVENT_ChangeEvent( event, next_event );
    }
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

//======================================================================
// 13 :�_�C�r���O
//======================================================================
static GMEVENT_RESULT GMEVENT_Diving( GMEVENT *event, int *seq, void *wk );
//--------------------------------------------------------------
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Diving( const FLDSKILL_CHECK_WORK * scwk )
{
	if (IsEnableSkill(scwk, FLDSKILL_IDX_DIVING)) {
		return FLDSKILL_RET_USE_OK;
	} else {
		return FLDSKILL_RET_USE_NG;
  }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT *  SkillUse_Diving(
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK * scwk )
{
  GMEVENT *event;
  HIDEN_SCR_WORK *hsw;
  
  event = GMEVENT_Create( scwk->gsys, NULL, GMEVENT_Diving, sizeof(HIDEN_SCR_WORK) );
  hsw = GMEVENT_GetEventWork( event );
  InitHSW( hsw, head, scwk );
  return event;
}
//--------------------------------------------------------------
/**
 * �_�C�r���O�Ăяo���C�x���g
 * @param event GMEVENT*
 * @parama seq �V�[�P���X
 * @param wk event work
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_Diving( GMEVENT *event, int *seq, void *wk )
{
  u16 *scwk;
  u16 prm0;
  SCRIPT_WORK *sc;
  HIDEN_SCR_WORK *hsw = wk;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( hsw->gsys );
  EVENTWORK *ev = GAMEDATA_GetEventWork( gdata );
  
  prm0 = hsw->head.poke_pos;
  sc = SCRIPT_ChangeScript( event, SCRID_HIDEN_DIVING_MENU, NULL, 0 );
  SCRIPT_SetScriptWorkParam( sc, prm0, 0, 0, 0 );
  return GMEVENT_RES_CONTINUE;
}

//======================================================================
//  ������ׂ�
//======================================================================
//--------------------------------------------------------------
/**
 * ������ׂ�g�p�`�F�b�N
 * @param scwk  FLDSKILL_CHECK_WORK
 * @retval FLDSLILL_RET
 */
//--------------------------------------------------------------
static FLDSKILL_RET SkillCheck_Osyaberi( const FLDSKILL_CHECK_WORK * scwk)
{
  //�R���V�A���E���j�I�����[���`�F�b�N
#if 0 //wb ���󖳎�
  if( CheckMapModeUse(scwk) == TRUE ){
    return FLDSKILL_RET_USE_NG;
  }
#endif
  return FLDSKILL_RET_USE_OK;
}

//--------------------------------------------------------------
/**
 * ������ׂ�g�p
 * @param head FLDSKILL_USE_HEADER
 * @parama  scwk FLDSKILL_CHECK_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT * SkillUse_Osyaberi(
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  GMEVENT *event;
  HIDEN_SCR_WORK *hsw;
  
  event = EVENT_FieldSkillOsyaberi( scwk->gsys, scwk->fieldmap, head->poke_pos );
  return event;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * �_�~�[�g�p�`�F�b�N
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
 * �_�~�[�g�p
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
  
  //�_�~�[�ŊÂ���������ĂĂ܂�
  event = EVENT_FieldSkillAmaikaori( scwk->gsys, scwk->fieldmap, head->poke_pos );
  return event;
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * �X�L���g�p�`�F�b�N�֐��擾
 * @param  id    �֐�ID
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
 * �X�L���g�p�`�F�b�N�֐��擾
 * @param  id    �֐�ID
 * @return FLDSKILL_USE_FUNC
 */
//--------------------------------------------------------------
static FLDSKILL_USE_FUNC GetUseFunc( FLDSKILL_IDX idx )
{
  GF_ASSERT_MSG( idx < FLDSKILL_IDX_MAX,"%d\n",idx );
  return SkillFuncTable[idx].use_func;
}

//--------------------------------------------------------------
/**
 * ��`�킴�X�N���v�g�N���p���[�N����
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
 * ��`�킴�X�N���v�g�N���p���[�N����
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
 * �X�L���g�p�\�`�F�b�N
 * @param scwk FLDSKILL_CHECK_WORK
 * @param skill_idx FLDSKILL_IDX
 * @retval BOOL TRUE=�g�p�\
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
 * �T�t�@���E�|�P�p�[�N�`�F�b�N
 * @param scwk FLDSKILL_CHECK_WORK
 * @retval TRUE=�T�t�@���E�|�P�p�[�N
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
 * �|�P�p�[�N�`�F�b�N
 * @param scwk FLDSKILL_CHECK_WORK
 * @retval TRUE=�|�P�p�[�N
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
 * �R���V�A���E���j�I�����[���`�F�b�N
 * @param scwk FLDSKILL_CHECK_WORK
 * @retval TRUE=�R���V�A���E���j�I�����[��
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
/* �g�p�֐��e�[�u��
 *
 * �����Ƃ��ēn����� FLDSKILL_USE_HEADER *head �� FLDSKILL_CHECK_WORK *scwk��
 * �Ăяo�����Ŏ����ϐ��Ƃ��Ē�`����Ă���̂ŁASkillUse_XXXX�œo�^����
 * �C�x���g���Ń����o�̒l���Q�Ƃ������ꍇ�A�l�̃R�s�[������Ďg�����ƁI
 */
//--------------------------------------------------------------
static const FLDSKILL_FUNC_DATA SkillFuncTable[FLDSKILL_IDX_MAX] =
{
  {SkillUse_Iaigiri,SkillCheck_Iaigiri},    // 00 : ����������
  {SkillUse_Naminori,SkillCheck_Naminori},    // 01 :�Ȃ݂̂� 
  {SkillUse_Takinobori,SkillCheck_Takinobori},    // 02 :�����̂ڂ�
  {SkillUse_Kairiki,SkillCheck_Kairiki},    // 03 :�����肫
  
  {SkillUse_Sorawotobu,SkillCheck_Sorawotobu},    // 04 :������Ƃ�
  {SkillUse_Dummy,SkillCheck_Dummy},    // 05 :����΂炢
  {SkillUse_Dummy,SkillCheck_Dummy},    // 06 :���킭����
  {SkillUse_Dummy,SkillCheck_Dummy},    // 07 :���b�N�N���C��
  {SkillUse_Flash,SkillCheck_Flash},    // 08 :�t���b�V��
  {SkillUse_Teleport,SkillCheck_Teleport},    // 09 :�e���|�[�g
  {SkillUse_Anawohoru,SkillCheck_Anawohoru},    // 10 :���Ȃ��ق�
  {SkillUse_Amaikaori,SkillCheck_Amaikaori},   // 11 :���܂�������
  {SkillUse_Osyaberi,SkillCheck_Osyaberi},    // 12 :������ׂ�
  {SkillUse_Diving, SkillCheck_Diving}, // 13 :�_�C�r���O
};
