//============================================================================================
/**
 * @file    itemuse_proc.c
 * @brief   �t�B�[���h�Ɋ֘A�����A�C�e���̎g�p����
 * @author  k.ohno
 * @date    09.08.04
 */
//============================================================================================
#include "gflib.h"
#include "itemuse_event.h"
#include "field/fieldmap.h"
#include "event_mapchange.h"
#include "arc/fieldmap/zone_id.h"
#include "event_fishing.h"
#include "fskill_amaikaori.h"
#include "field/zonedata.h"
#include "app/itemuse.h"

#define ITEMCHECK_NONE  (0xFF)

typedef struct _FLD_ITEM_FUNCTION{
  ItemUseEventFunc useFunc;
  u8  checkID;  //ITEMCHECK_ENABLE�^�A�K�v�Ȃ�����ITEMCHECK_NONE
}FLD_ITEM_FUNCTION;

//////////////////////////////////////////////////
///�v���g�^�C�v
//////////////////////////////////////////////////
static BOOL itemcheck_Cycle( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork );
static BOOL itemcheck_Ananukenohimo( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork );
static BOOL itemcheck_Amaimitu( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork );
static BOOL itemcheck_Turizao( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork );
static BOOL itemcheck_DowsingMachine( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork );

//=============================================================================
/**
 *  ����
 */
//=============================================================================
//-------------------------------------
/// �t�B�[���h�A�C�e���g�p���ʌĂяo���p�e�[�u��
//=====================================
static FLD_ITEM_FUNCTION const DATA_FldItemFunction[ EVENT_ITEMUSE_CALL_MAX ] =
{ 
  { //���]��
    &EVENT_CycleUse,
    ITEMCHECK_CYCLE,
  },
  { //�p���X��Go
    &EVENT_PalaceJumpUse,
    ITEMCHECK_NONE,
  },
  { //�������̕R
    &EVENT_ChangeMapByAnanukenohimo,
    ITEMCHECK_ANANUKENOHIMO,
  },
  { //�Â���
    &EVENT_FieldAmaimitu,
    ITEMCHECK_NONE,
  },
  { //�ފ�
    &EVENT_FieldFishing,
    ITEMCHECK_TURIZAO,
  },
  { //�_�E�W���O�}�V��
    &EVENT_DowsingMachineUse,
    ITEMCHECK_DOWSINGMACHINE,
  },
};

//-------------------------------------
/*
 * �t�B�[���h�A�C�e���g�p�`�F�b�N�֐��e�[�u��
 *
 * ���̃e�[�u���o�^�Ƃ͕ʂɁA�p���X�ł́u��؂Ȃ��́v�͎g���Ȃ��`�F�b�N��
 * �o�b�O��ʂōs���Ă���
 */
//-------------------------------------
static ItemUseCheckFunc const DATA_ItemUseCheckFunc[ITEMCHECK_MAX] = {
  itemcheck_Cycle, // ���]��(����Ă��Ȃ�����TRUE�ŏ���A����Ă��鎞��TRUE�ō~����)
  NULL, // �^�E���}�b�v
  NULL, // �Ƃ������蒠
  itemcheck_Ananukenohimo, // ���Ȃʂ��̃q��
  itemcheck_Amaimitu, // ���܂��~�c
  itemcheck_Turizao, // �肴��
  NULL, // �o�g�����R�[�_�[
  NULL, // ���[��
  NULL, // �X�v���[
  itemcheck_DowsingMachine, // �_�E�W���O�}�V��
};


//----------------------------------------------------------------------------
/**
 *  @brief  �o�b�O��ʂɈ����n���A�C�e���g�p�`�F�b�N
 *
 *  @param  GMEVENT *event  �C�x���g
 *  @param  *seq            �V�[�P���X
 *  @param  *wk_adrs        ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
void ITEMUSE_InitCheckWork( ITEMCHECK_WORK* icwk, GAMESYS_WORK* gsys, FIELDMAP_WORK* field_wk )
{
  int i;
  GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
  PLAYER_WORK *playerWork = GAMEDATA_GetMyPlayerWork( gdata );

  MI_CpuClear8( icwk, sizeof(ITEMCHECK_WORK) );

  for( i = 0;i < ITEMCHECK_MAX; i++){
    ITEMUSE_SetItemUseCheck( icwk, i, ITEMUSE_UseCheckCall( i, gsys, field_wk ));
  }
  //���@�t�H�����`�F�b�N
  icwk->PlayerForm = PLAYERWORK_GetMoveForm( playerWork ); 
  // ( PlayerForm == PLAYER_MOVE_FORM_CYCLE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���ʃA�C�e���g�p�`�F�b�N�֐��Ăяo��
 *
 *  @param  EVENT_ITEMUSE_CALL_TYPE type  �Ăяo���`�F�b�N�^�C�v
 *  @param  *gsys       �Q�[���V�X�e��
 *  @param  *field_wk   �t�B�[���h
 *
 *  @return �C�x���g
 */
//-----------------------------------------------------------------------------
BOOL ITEMUSE_UseCheckCall( ITEMCHECK_ENABLE type, GAMESYS_WORK *gsys, FIELDMAP_WORK *field_wk )
{ 
  GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
  PLAYER_WORK *playerWork = GAMEDATA_GetMyPlayerWork( gdata );

  GF_ASSERT( type < ITEMCHECK_MAX );
  
  if( DATA_ItemUseCheckFunc[type] == NULL ){
    return TRUE;  //�������Ŏg�p���Ă���
  }
  return (DATA_ItemUseCheckFunc[ type ])( gdata, field_wk, playerWork );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���ʃA�C�e���g�p�C�x���g�Ăяo��
 *
 *  @param  EVENT_ITEMUSE_CALL_TYPE type  �Ăяo���A�C�e��
 *  @param  *gsys       �Q�[���V�X�e��
 *  @param  *field_wk   �t�B�[���h
 *
 *  @return �C�x���g
 */
//-----------------------------------------------------------------------------
GMEVENT * EVENT_FieldItemUse( EVENT_ITEMUSE_CALL_TYPE type, GAMESYS_WORK *gsys, FIELDMAP_WORK *field_wk )
{ 
  GF_ASSERT( type < EVENT_ITEMUSE_CALL_MAX );
  OS_Printf("event type = %d\n", type);
  return (DATA_FldItemFunction[ type ].useFunc)( field_wk, gsys );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���ʃA�C�e���g�p�`�F�b�N�Ăяo��
 *
 *  @param  EVENT_ITEMUSE_CALL_TYPE type  �Ăяo���A�C�e��
 *  @param  *gsys       �Q�[���V�X�e��
 *  @param  *field_wk   �t�B�[���h
 *
 *  @retval TRUE�F�g�p��
 *  @retval FALSE�F�g�p�s��
 */
//-----------------------------------------------------------------------------
BOOL FieldItemUseCheck( EVENT_ITEMUSE_CALL_TYPE type, GAMESYS_WORK *gsys, FIELDMAP_WORK *field_wk )
{ 
  GF_ASSERT( type < EVENT_ITEMUSE_CALL_MAX );

  if( DATA_FldItemFunction[ type ].checkID == ITEMCHECK_NONE ){
    return TRUE;  //�g�p��������
  }
  return ITEMUSE_UseCheckCall( DATA_FldItemFunction[type].checkID, gsys, field_wk );
}

//-----------------------------------------------------------------------------
//=============================================================================
///�ʎg�p�`�F�b�N�֐�
//=============================================================================
//-----------------------------------------------------------------------------

/*
 *  @brief  ���]�Ԏg�p�`�F�b�N
 */
static BOOL itemcheck_Cycle( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork )
{
//  PLAYER_WORK *playerWork = GAMEDATA_GetMyPlayerWork( gdata );
  u16 zone_id = PLAYERWORK_getZoneID( playerWork );
  u8 form = PLAYERWORK_GetMoveForm( playerWork ); 
  MAPATTR attr = FIELD_PLAYER_GetMapAttr( FIELDMAP_GetFieldPlayer( field_wk ) );
  BOOL not_use_f = MAPATTR_VALUE_CheckCycleNotUse( MAPATTR_GetAttrValue(attr) );

  if( form == PLAYER_MOVE_FORM_CYCLE ){
    if( not_use_f ){
      return FALSE;
    }
    return TRUE;  //�~����
  }
  //�]�[���`�F�b�N
  if( ZONEDATA_BicycleEnable( zone_id ) == FALSE ){
    return FALSE;
  }

  //�t�H�����`�F�b�N
  if( form != PLAYER_MOVE_FORM_NORMAL ){
    return FALSE;
  }
  //�A�g���r���[�g�ɂ��`�F�b�N
  if( not_use_f ){
    return FALSE;
  }
  return TRUE;  //����
}

/*
 *  @brief  �������̕R�`�F�b�N
 */
static BOOL itemcheck_Ananukenohimo( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork )
{
//  PLAYER_WORK *playerWork = GAMEDATA_GetMyPlayerWork( gdata );
  u16 zone_id = PLAYERWORK_getZoneID( playerWork );
  
  //�t�B�[���h�Z���g���Ȃ��ꏊ�ł͎g���Ȃ�(���j�I�����[���A�ʐM�ΐ핔���A�p���X)
  if( ZONEDATA_CheckFieldSkillUse(zone_id) == FALSE){
    return FALSE;
  }

  if(!ZONEDATA_EscapeEnable( zone_id )){
    return FALSE;
  }
  return TRUE;
}

/*
 *  @brief  ���܂��݂`�F�b�N
 */
static BOOL itemcheck_Amaimitu( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork )
{
  u16 zone_id = PLAYERWORK_getZoneID( playerWork );

  //�t�B�[���h�Z���g���Ȃ��ꏊ�ł͎g���Ȃ�(���j�I�����[���A�ʐM�ΐ핔���A�p���X)
  return ( ZONEDATA_CheckFieldSkillUse(zone_id));
}

/*
 *  @brief  �ފƎg�p�`�F�b�N
 */
static BOOL itemcheck_Turizao( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork )
{
  u16 zone_id = PLAYERWORK_getZoneID( playerWork );

  //�t�B�[���h�Z���g���Ȃ��ꏊ�ł͎g���Ȃ�(���j�I�����[���A�ʐM�ΐ핔���A�p���X)
  if( ZONEDATA_CheckFieldSkillUse(zone_id) == FALSE){
    return FALSE;
  }
  //�ފƃ`�F�b�N
  return FieldFishingCheckPos( gdata, field_wk, NULL );
}

/*
 *  @brief  �_�E�W���O�}�V���g�p�`�F�b�N
 */
static BOOL itemcheck_DowsingMachine( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork )
{
  u16 zone_id = PLAYERWORK_getZoneID( playerWork );

  //�t�B�[���h�Z���g���Ȃ��ꏊ�ł͎g���Ȃ�(���j�I�����[���A�ʐM�ΐ핔���A�p���X)
  return ( ZONEDATA_CheckFieldSkillUse(zone_id));
}

//=============================================================================
/**
 *  ���]�ԌĂяo��
 */
//=============================================================================
typedef struct{
  GAMESYS_WORK *gameSys;
} CYCLEUSE_STRUCT;

//------------------------------------------------------------------------------
/**
 * @brief   ���]�Ԃ��g��
 * @retval  none
 */
//------------------------------------------------------------------------------

static GMEVENT_RESULT CycleEvent(GMEVENT * event, int * seq, void *work)
{
  CYCLEUSE_STRUCT* pCy = work;
  FIELDMAP_WORK *fieldmap = GAMESYSTEM_GetFieldMapWork( pCy->gameSys );
  
  switch( (*seq) ){
  case 0:
    FIELDMAP_SetPlayerItemCycle( fieldmap );
    (*seq)++;
    break;
  case 1:
    {
      FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
      
      if( FIELD_PLAYER_CheckDrawFormWait(fld_player) == TRUE ){
        return( GMEVENT_RES_FINISH );
      }
    }
  }
  
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_CycleUse(FIELDMAP_WORK *fieldWork,GAMESYS_WORK *gsys)
{
  GMEVENT * event = GMEVENT_Create(gsys, NULL, CycleEvent, sizeof(CYCLEUSE_STRUCT));
  CYCLEUSE_STRUCT * pCy = GMEVENT_GetEventWork(event);
  pCy->gameSys = gsys;
  return event;
}


//------------------------------------------------------------------------------
/**
 * @brief   �p���X�ɃW�����v����
 * @retval  none
 */
//------------------------------------------------------------------------------


GMEVENT * EVENT_PalaceJumpUse(FIELDMAP_WORK *fieldWork,GAMESYS_WORK *gsys)
{
  VecFx32 pos;
  ZONEID jump_zone;
  
  if(GFL_UI_KEY_GetCont() & PAD_BUTTON_R){
    pos.x = 184 << FX32_SHIFT;
    pos.y = 0;
    pos.z = 184 << FX32_SHIFT;
    jump_zone = ZONE_ID_UNION;   //������ɔ�Ԃ̂̓f�o�b�O�ł�
  }
  else{
    pos.x = 760 << FX32_SHIFT;
    pos.y = 0;
    pos.z = 234 << FX32_SHIFT;
    jump_zone = ZONE_ID_PALACE01;
  }
  return EVENT_ChangeMapPos(gsys, fieldWork, jump_zone, &pos, 0, FALSE);
}

//=============================================================================
/**
 *  �_�E�W���O�}�V���Ăяo��
 */
//=============================================================================
typedef struct{
  GAMESYS_WORK*      gameSys;
  FIELDMAP_WORK*     fieldWork;
} DOWSINGMACHINEUSE_STRUCT;
//------------------------------------------------------------------------------
/**
 * @brief   �_�E�W���O�}�V�����g��
 * @retval  
 */
//------------------------------------------------------------------------------
static GMEVENT_RESULT DowsingMachineEvent(GMEVENT* event, int* seq, void* work)
{
  DOWSINGMACHINEUSE_STRUCT*   pDMU        = work;
  FIELD_SUBSCREEN_WORK*       subscreen   = FIELDMAP_GetFieldSubscreenWork(pDMU->fieldWork);

  if( FIELD_SUBSCREEN_GetMode( subscreen ) == FIELD_SUBSCREEN_DOWSING )  // �_�E�W���O�}�V�����g���Ă���Ƃ��́A�_�E�W���O�}�V�����I��������
  {
    FIELD_SUBSCREEN_Change( subscreen, FIELD_SUBSCREEN_NORMAL );
  }
  else
  {
    FIELD_SUBSCREEN_Change( subscreen, FIELD_SUBSCREEN_DOWSING );
  }

  return GMEVENT_RES_FINISH;
}
GMEVENT* EVENT_DowsingMachineUse(FIELDMAP_WORK* fieldWork, GAMESYS_WORK* gsys)
{
  GMEVENT*                    event  = GMEVENT_Create(gsys, NULL, DowsingMachineEvent, sizeof(DOWSINGMACHINEUSE_STRUCT));
  DOWSINGMACHINEUSE_STRUCT*   pDMU   = GMEVENT_GetEventWork(event);
  pDMU->gameSys     = gsys;
  pDMU->fieldWork   = fieldWork;
  return event;
}

