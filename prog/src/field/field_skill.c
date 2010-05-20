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
#include "gamesystem/game_beacon.h"

#include "fieldmap.h"

#include "field_status_local.h"
#include "field_skill.h"
#include "fskill_amaikaori.h"
#include "fskill_sorawotobu.h"
#include "fieldskill_mapeff.h"
#include "fskill_osyaberi.h"
#include "field/eventwork.h"
#include "field/zonedata.h"
#include "waza_tool/wazano_def.h"

#include "script.h"
#include "../../../resource/fldmapdata/script/hiden_def.h" //script id
#include "../../../resource/fldmapdata/script/seatemple_scr_def.h" //script id

#include "event_mapchange.h"  //EVENT_ChangeMapByTeleport
#include "field_diving_data.h"  //DIVINGSPOT_Check

// �C��_�a�ʒu���
#include "sea_temple.cdat"  //(8byte�̃e�[�u��)

// eventwork
#include "field/eventwork.h"
#include "eventwork_def.h"

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

static FLDSKILL_CHECK_FUNC GetCheckFunc( const FLDSKILL_CHECK_WORK *scwk, FLDSKILL_IDX idx );
static FLDSKILL_USE_FUNC GetUseFunc( const FLDSKILL_CHECK_WORK *scwk, FLDSKILL_IDX idx );

static void InitHSW( HIDEN_SCR_WORK *hsw,
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk );

static BOOL IsEnableSkill(
    const FLDSKILL_CHECK_WORK *scwk, FLDSKILL_IDX skill_idx );

static FLDSKILL_RET SkillCheck_Amaikaori( const FLDSKILL_CHECK_WORK * scwk );
static GMEVENT* SkillUse_Amaikaori( const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk );
static GMEVENT* SkillUse_Sorawotobu( const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk );

static const FLDSKILL_FUNC_DATA SkillFuncTable[FLDSKILL_IDX_MAX];
static const FLDSKILL_FUNC_DATA SeaTempleSkillFuncTable[FLDSKILL_IDX_MAX];

// �C��_�a�G���A����
static BOOL FLDSKILL_SEATEMPLE_IsArea( const SEATEMPLE_SKILL_USE_POS* cp_pos, u16 gx, u16 gz, u16 flag, FIELDMAP_WORK* p_fieldmap );

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
    }
  }
  
  {
    u16 dir = FIELD_PLAYER_GetDir( fld_player );
    
    //���ݒn�̃A�g���r���[�g
    nattr = FIELD_PLAYER_GetMapAttr( fld_player );
    
    //���@�O�̃A�g���r���[�g
    fattr = FIELD_PLAYER_GetDirMapAttr( fld_player, dir );
  }
  
  if( FIELD_PLAYER_CheckNaminoriUse(fld_player,nattr,fattr) == TRUE )
  {
    scwk->enable_skill |= IDXBIT( FLDSKILL_IDX_NAMINORI );
  }
  if( FIELD_PLAYER_CheckTakinoboriUse( fld_player, nattr, fattr ) == TRUE )
  {
    scwk->enable_skill |= IDXBIT( FLDSKILL_IDX_TAKINOBORI );
  }

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



  //�C��_�a�̐ݒ�
  {
    if( ZONEDATA_IsSeaTempleDungeon( scwk->zone_id ) ){
      const FIELD_PLAYER* cp_player = FIELDMAP_GetFieldPlayer( scwk->fieldmap );
      s16 gx, gy, gz;
      FIELD_PLAYER_GetGridPos( cp_player, &gx, &gy, &gz );

      // 2F
      if( ZONEDATA_IsSeaTempleDungeon2F(scwk->zone_id) ){
        if( FLDSKILL_SEATEMPLE_IsArea( &sc_SEATEMPLE_SKILL_USE_POS[ SEATEMPLE_USE_SKILL_2F ], gx, gz, FE_SEA_TEMPLE_FLASH, fieldmap ) ){
          scwk->enable_skill |= (1 << FLDSKILL_IDX_FLASH);
        }
      }
      // 3F
      if( ZONEDATA_IsSeaTempleDungeon3F(scwk->zone_id) ){
        if( FLDSKILL_SEATEMPLE_IsArea( &sc_SEATEMPLE_SKILL_USE_POS[ SEATEMPLE_USE_SKILL_3F ], gx, gz, FE_SEA_TEMPLE_KAIRIKI, fieldmap ) ){
          scwk->enable_skill |= (1 << FLDSKILL_IDX_KAIRIKI);
        }
      }
    }
  }
  //���j�I��/�R���V�A���Ȃǂł̓t�B�[���h�Z�S�ʋ֎~
  if( ZONEDATA_CheckFieldSkillUse( scwk->zone_id ) == FALSE ){
    scwk->enable_skill = 0;
  }else{
    scwk->enable_skill |= ( 1 << FLDSKILL_IDX_AMAIKAORI );
    scwk->enable_skill |= ( 1 << FLDSKILL_IDX_OSYABERI );
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
  FLDSKILL_CHECK_FUNC func = GetCheckFunc( scwk, idx );
  if( func == NULL ){
    return FLDSKILL_RET_USE_NG;
  }
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
    u16 poke_pos, u16 use_wazano, u32 zoneID)
{
  head->poke_pos = poke_pos;
  head->use_wazano = use_wazano;
  head->zoneID = zoneID;
}

//--------------------------------------------------------------
/**
 * �X�L���g�p
 * @param idx �g�p����FLDSKILL_IDX
 * @param head ���e���Z�b�g���ꂽFLDSKILL_USE_HEADER
 * @param scwk FLDSKILL_InitCheckWork()�ŏ������ς݂�FLDSKILL_CHECK_WORK
 * @retval  GMEVENT �X�L���g�p�C�x���g
 * @retval  NULL  �G���[�l
 * @note  
 */
//--------------------------------------------------------------
GMEVENT * FLDSKILL_UseSkill( FLDSKILL_IDX idx,
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  FLDSKILL_USE_FUNC func = GetUseFunc( scwk, idx );
  if( func == NULL ){
    return NULL;
  }
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
  //�g��蒆
  if( scwk->moveform == PLAYER_MOVE_FORM_SWIM ){
    return FLDSKILL_RET_PLAYER_SWIM;
  }

  //�_�C�r���O��
  if( scwk->moveform == PLAYER_MOVE_FORM_DIVING ){
    return FLDSKILL_RET_USE_NG;
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
  if( IsEnableSkill(scwk,FLDSKILL_IDX_AMAIKAORI) ){
    return FLDSKILL_RET_USE_OK;
  }
  return FLDSKILL_RET_USE_NG;
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
  
  GAMEBEACON_Set_FieldSkill( WAZANO_SORAWOTOBU );

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
    FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( hsw->gsys );
    HEAPID heapID = FIELDMAP_GetHeapID( fieldmap );

    switch( *seq )
    {
    case 0:
        {
          u16 prm0;
          SCRIPT_WORK *sc;
          
          prm0 = hsw->head.poke_pos;
          sc = SCRIPT_CallScript( event, SCRID_HIDEN_FLASH_MENU, NULL, NULL, heapID );
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
        GAMEBEACON_Set_FieldSkill( WAZANO_HURASSYU );

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
  
  GAMEBEACON_Set_FieldSkill( WAZANO_ANAWOHORU );
  
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
  
  GAMEBEACON_Set_FieldSkill( WAZANO_TEREPOOTO );
  
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
  if( IsEnableSkill(scwk,FLDSKILL_IDX_OSYABERI) ){
    return FLDSKILL_RET_USE_OK;
  }
  return FLDSKILL_RET_USE_NG;
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
  
  GAMEBEACON_Set_FieldSkill( WAZANO_OSYABERI );
  event = EVENT_FieldSkillOsyaberi( scwk->gsys, scwk->fieldmap, head->poke_pos );
  return event;
}

//======================================================================
//  �C��@�����肫
//======================================================================
//--------------------------------------------------------------
/**
 * �����肫�Ăяo���C�x���g
 * @param event GMEVENT*
 * @parama seq �V�[�P���X
 * @param wk event work
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_SeaTempleKairiki(
    GMEVENT *event, int *seq, void *wk )
{
  u16 *scwk;
  u16 prm0;
  SCRIPT_WORK *sc;
  HIDEN_SCR_WORK *hsw = wk;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( hsw->gsys );
  EVENTWORK *ev = GAMEDATA_GetEventWork( gdata );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( hsw->gsys );
  HEAPID heapID = FIELDMAP_GetHeapID( fieldmap );
  
  switch( *seq )
  {
  case 0:
    prm0 = hsw->head.poke_pos;
    sc = SCRIPT_CallScript( event, SCRID_HIDEN_SEATEMPLE_3F, NULL, NULL, heapID );
    SCRIPT_SetScriptWorkParam( sc, prm0, 0, 0, 0 );
    (*seq) ++;
    break;
  case 1:
    sc = SCRIPT_ChangeScript( event, SCRID_SEATEMPLE_SKILL_USE_3F, NULL, 0 );
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �����肫�g�p
 * @param head FLDSKILL_USE_HEADER
 * @parama  scwk FLDSKILL_CHECK_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT * SkillUse_SeaTempleKairiki(
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  GMEVENT *event;
  HIDEN_SCR_WORK *hsw;
  
  event = GMEVENT_Create( scwk->gsys, NULL,
      GMEVENT_SeaTempleKairiki, sizeof(HIDEN_SCR_WORK) );
  hsw = GMEVENT_GetEventWork( event );
  InitHSW( hsw, head, scwk );
  return event;
}

//======================================================================
//  �C��t���b�V��
//======================================================================
//--------------------------------------------------------------
/**
 * �t���b�V���Ăяo���C�x���g
 * @param event GMEVENT*
 * @parama seq �V�[�P���X
 * @param wk event work
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_SeaTempleFlash(GMEVENT *event, int *seq, void *wk )
{
  u16 *scwk;
  u16 prm0;
  SCRIPT_WORK *sc;
  HIDEN_SCR_WORK *hsw = wk;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( hsw->gsys );
  EVENTWORK *ev = GAMEDATA_GetEventWork( gdata );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( hsw->gsys );
  HEAPID heapID = FIELDMAP_GetHeapID( fieldmap );
  
  switch( *seq )
  {
  case 0:
    prm0 = hsw->head.poke_pos;
    sc = SCRIPT_CallScript( event, SCRID_HIDEN_SEATEMPLE_2F, NULL, NULL, heapID );
    SCRIPT_SetScriptWorkParam( sc, prm0, 0, 0, 0 );
    (*seq) ++;
    break;
  case 1:
    sc = SCRIPT_ChangeScript( event, SCRID_SEATEMPLE_SKILL_USE_2F, NULL, 0 );
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �t���b�V���g�p
 * @param head FLDSKILL_USE_HEADER
 * @parama  scwk FLDSKILL_CHECK_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static GMEVENT * SkillUse_SeaTempleFlash(
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk )
{
  GMEVENT *event;
  HIDEN_SCR_WORK *hsw;
  
  event = GMEVENT_Create( scwk->gsys, NULL,
      GMEVENT_SeaTempleFlash, sizeof(HIDEN_SCR_WORK) );
  hsw = GMEVENT_GetEventWork( event );
  InitHSW( hsw, head, scwk );
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
static FLDSKILL_CHECK_FUNC GetCheckFunc( const FLDSKILL_CHECK_WORK *scwk, FLDSKILL_IDX idx )
{
  if( idx >= FLDSKILL_IDX_MAX ) {
#ifdef PM_DEBUG
    GF_ASSERT( idx < FLDSKILL_IDX_MAX );
#endif
    return NULL;
  }

  // �C��_�a
  if( ZONEDATA_IsSeaTempleDungeon( scwk->zone_id ) ){
    return SeaTempleSkillFuncTable[idx].check_func;
  }
  
  return SkillFuncTable[idx].check_func;
}

//--------------------------------------------------------------
/**
 * �X�L���g�p�`�F�b�N�֐��擾
 * @param  id    �֐�ID
 * @return FLDSKILL_USE_FUNC
 */
//--------------------------------------------------------------
static FLDSKILL_USE_FUNC GetUseFunc( const FLDSKILL_CHECK_WORK *scwk, FLDSKILL_IDX idx )
{
  if( idx >= FLDSKILL_IDX_MAX ){
#ifdef PM_DEBUG
    GF_ASSERT_MSG( idx < FLDSKILL_IDX_MAX,"%d\n",idx );
#endif
    return NULL;
  }

  // �C��_�a
  if( ZONEDATA_IsSeaTempleDungeon( scwk->zone_id ) ){
    return SeaTempleSkillFuncTable[idx].use_func;
  }

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

//----------------------------------------------------------------------------
/**
 *	@brief  �C��_�a�G���A�������`�F�b�N
 *
 *	@param	cp_pos      �G���A���
 *	@param	gx          ���O���b�h
 *	@param	gz          ���O���b�h
 *	@param  flag        �C�x���g�t���O
 *	@param  p_fieldmap  �t�B�[���h�}�b�v
 *
 *	@retval TRUE  �G���A��
 *	@retval FALSE �G���A�O
 */
//-----------------------------------------------------------------------------
static BOOL FLDSKILL_SEATEMPLE_IsArea( const SEATEMPLE_SKILL_USE_POS* cp_pos, u16 gx, u16 gz, u16 flag, FIELDMAP_WORK* p_fieldmap )
{
  GAMESYS_WORK* p_gsys = FIELDMAP_GetGameSysWork( p_fieldmap );
  GAMEDATA* p_gdata = GAMESYSTEM_GetGameData( p_gsys );
  EVENTWORK* p_ev = GAMEDATA_GetEventWork( p_gdata );
  s32 dif_x, dif_z;

  if( EVENTWORK_CheckEventFlag( p_ev, flag ) ){
    return FALSE; //�t���O�������Ă�����g���Ȃ�
  }
  
  dif_x = (s32)cp_pos->gx - (s32)gx;
  dif_z = (s32)cp_pos->gz - (s32)gz;
  
  if( (MATH_ABS(dif_x) < cp_pos->siz_x) && (MATH_ABS(dif_z) < cp_pos->siz_z) ){
    return TRUE;
  }
  return FALSE;
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
  {SkillUse_Flash,SkillCheck_Flash},    // 05 :�t���b�V��
  {SkillUse_Teleport,SkillCheck_Teleport},    // 06 :�e���|�[�g
  {SkillUse_Anawohoru,SkillCheck_Anawohoru},    // 07 :���Ȃ��ق�
  {SkillUse_Amaikaori,SkillCheck_Amaikaori},   // 08 :���܂�������
  {SkillUse_Osyaberi,SkillCheck_Osyaberi},    // 09 :������ׂ�
  {SkillUse_Diving, SkillCheck_Diving}, // 10 :�_�C�r���O
};



//--------------------------------------------------------------
/* �g�p�֐��e�[�u��
 *  �C��_�a��p�e�[�u��
 *
 * �����Ƃ��ēn����� FLDSKILL_USE_HEADER *head �� FLDSKILL_CHECK_WORK *scwk��
 * �Ăяo�����Ŏ����ϐ��Ƃ��Ē�`����Ă���̂ŁASkillUse_XXXX�œo�^����
 * �C�x���g���Ń����o�̒l���Q�Ƃ������ꍇ�A�l�̃R�s�[������Ďg�����ƁI
 */
//--------------------------------------------------------------
static const FLDSKILL_FUNC_DATA SeaTempleSkillFuncTable[FLDSKILL_IDX_MAX] =
{
  {SkillUse_Iaigiri,SkillCheck_Iaigiri},    // 00 : ����������
  {SkillUse_Naminori,SkillCheck_Naminori},    // 01 :�Ȃ݂̂� 
  {SkillUse_Takinobori,SkillCheck_Takinobori},    // 02 :�����̂ڂ�
  {SkillUse_SeaTempleKairiki,SkillCheck_Kairiki},    // �C��_�a�p�����肫
  {SkillUse_Sorawotobu,SkillCheck_Sorawotobu},    // 04 :������Ƃ�
  {SkillUse_SeaTempleFlash,SkillCheck_Flash},    //05 :�C��_�a�p�t���b�V��
  {SkillUse_Teleport,SkillCheck_Teleport},    // 06 :�e���|�[�g
  {SkillUse_Anawohoru,SkillCheck_Anawohoru},    // 07 :���Ȃ��ق�
  {SkillUse_Amaikaori,SkillCheck_Amaikaori},   // 08 :���܂�������
  {SkillUse_Osyaberi,SkillCheck_Osyaberi},    // 09 :������ׂ�
  {SkillUse_Diving, SkillCheck_Diving}, // 10 :�_�C�r���O
};
