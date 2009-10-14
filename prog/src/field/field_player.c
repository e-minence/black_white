//======================================================================
/**
 * @file	field_player.c
 * @date	2008.9.29
 * @brief	�t�B�[���h�v���C���[����
 */
//======================================================================
#include <gflib.h>

#include "fieldmap.h"
#include "field_g3d_mapper.h"
#include "fldmmdl.h"
#include "field_sound.h"
#include "field_player.h"

#include "field_player_grid.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
/// OBJCODE_FORM
//--------------------------------------------------------------
typedef struct
{
  u16 code;
  u16 move_form;
}OBJCODE_FORM;

//--------------------------------------------------------------
///	FIELD_PLAYER
//--------------------------------------------------------------
struct _FIELD_PLAYER
{
	HEAPID heapID;
  GAMESYS_WORK *gsys;
	FIELDMAP_WORK *fieldWork;
  PLAYER_WORK *playerWork;
   
#if 0 //PLAYER_WORK�ֈړ�
  PLAYER_MOVE_FORM move_form;
#endif
  
  FLDMAP_CTRLTYPE map_type;

  PLAYER_MOVE_STATE move_state;
  PLAYER_MOVE_VALUE move_value;
	
  int sex; //����
  
	u16 dir;
	u16 padding0;
	
  VecFx32 pos;
  
	MMDL *fldmmdl;
	FLDMAPPER_GRIDINFODATA gridInfoData;
};

//======================================================================
//	proto
//======================================================================
static const MMDL_HEADER data_MMdlHeader;
static const OBJCODE_FORM dataOBJCodeForm[2][PLAYER_DRAW_FORM_MAX];

//======================================================================
//	�t�B�[���h�v���C���[
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�v���C���[�@�쐬
 * @param playerWork �g�p����PLAYER_WOKR
 * @param *fieldWork �Q�Ƃ���FIELDMAP_WORK
 * @param pos �������W
 * @param sex ����
 * @param heapID HEAPID
 * @retval FIELD_PLAYER*
 */
//--------------------------------------------------------------
FIELD_PLAYER * FIELD_PLAYER_Create(
    PLAYER_WORK *playerWork, FIELDMAP_WORK *fieldWork,
		const VecFx32 *pos, int sex, FLDMAP_CTRLTYPE type, HEAPID heapID )
{
	MMDLSYS *fmmdlsys;
	FIELD_PLAYER *fld_player;
	
	fld_player = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_PLAYER) );
  fld_player->playerWork = playerWork;
	fld_player->fieldWork = fieldWork;
  fld_player->gsys = FIELDMAP_GetGameSysWork( fieldWork );
	fld_player->pos = *pos;
  fld_player->map_type = type;

	FLDMAPPER_GRIDINFODATA_Init( &fld_player->gridInfoData );
	
	//MMDL�Z�b�g�A�b�v
	fmmdlsys = FIELDMAP_GetMMdlSys( fieldWork );
	
	fld_player->fldmmdl =
		MMDLSYS_SearchOBJID( fmmdlsys, MMDL_ID_PLAYER );
  
  fld_player->sex = sex;
  
	if( fld_player->fldmmdl == NULL )	//�V�K
	{
		MMDL_HEADER head;
    MMDL_HEADER_GRIDPOS *gridpos;
		head = data_MMdlHeader;
    gridpos = (MMDL_HEADER_GRIDPOS *)head.pos_buf;
		gridpos->gx = SIZE_GRID_FX32( pos->x );
		gridpos->gz = SIZE_GRID_FX32( pos->z );
		gridpos->y = pos->y;
    head.obj_code = FIELD_PLAYER_GetMoveFormToOBJCode(
        sex, PLAYER_MOVE_FORM_NORMAL ); 
		fld_player->fldmmdl = MMDLSYS_AddMMdl( fmmdlsys, &head, 0 );
	}
	else //���A
	{
		int gx = SIZE_GRID_FX32( pos->x );
		int gy = SIZE_GRID_FX32( pos->y );
		int gz = SIZE_GRID_FX32( pos->z );
		MMDL *fmmdl = fld_player->fldmmdl;
		
		MMDL_SetGridPosX( fmmdl, gx );
		MMDL_SetGridPosY( fmmdl, gy );
		MMDL_SetGridPosZ( fmmdl, gz );
		MMDL_SetVectorPos( fmmdl, pos );
	}
	
  { //OBJ�R�[�h���瓮��t�H�[����ݒ�
    u16 code = MMDL_GetOBJCode( fld_player->fldmmdl );
    PLAYER_MOVE_FORM form = FIELD_PLAYER_GetOBJCodeToMoveForm( sex, code );
#if 0
    fld_player->move_form = form;
#else
    PLAYERWORK_SetMoveForm( fld_player->playerWork, form );
#endif
  }
  
	MMDL_SetStatusBitNotZoneDelete( fld_player->fldmmdl, TRUE );
	return( fld_player );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�v���C���[�@�폜
 * @param fld_player FIELD_PLAYER
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_Delete( FIELD_PLAYER *fld_player )
{
	//���샂�f���̍폜�̓t�B�[���h���C������
	GFL_HEAP_FreeMemory( fld_player );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�v���C���[�@�X�V
 * @param fld_player FIELD_PLAYER
 * @param dir ����
 * @param pos ���W
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_Update( FIELD_PLAYER *fld_player )
{
  //�v���C���[���W��PLAYERWORK�֔��f
  VecFx32 pos;
  FIELD_PLAYER_GetPos( fld_player, &pos );
	PLAYERWORK_setPosition( fld_player->playerWork, &pos );
}

//--------------------------------------------------------------
/**
 * ���@�C�x���g�ړ��`�F�b�N
 * @param fld_player FIELD_PLAYER
 * @param key_trg ���̓L�[�g���K���
 * @param key_cont ���̓L�[�R���e�j���[���
 * @param evbit PLAYER_EVENTBIT
 * @retval BOOL TRUE=���@�C�x���g�ړ�����
 */
//--------------------------------------------------------------
GMEVENT * FIELD_PLAYER_CheckMoveEvent( FIELD_PLAYER *fld_player,
    int key_trg, int key_cont, PLAYER_EVENTBIT evbit )
{
  GMEVENT *event = NULL;
  
  if( fld_player->map_type == FLDMAP_CTRLTYPE_GRID ){
    event = FIELD_PLAYER_GRID_CheckMoveEvent(
        fld_player, key_trg, key_cont, evbit );
  }
  
  return( event );
}

//======================================================================
//  FIELD_PLAYER ����X�e�[�^�X
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�v���C���[�@����X�e�[�^�X�X�V
 * @param fld_player
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_UpdateMoveStatus( FIELD_PLAYER *fld_player )
{
  MMDL *fmmdl = fld_player->fldmmdl;
  PLAYER_MOVE_VALUE value = fld_player->move_value;
  PLAYER_MOVE_STATE state = fld_player->move_state;
  
  fld_player->move_state = PLAYER_MOVE_STATE_OFF;
  
  if( MMDL_CheckPossibleAcmd(fmmdl) == FALSE ){ //���쒆
    switch( value ){
    case PLAYER_MOVE_VALUE_STOP:
      break;
    case PLAYER_MOVE_VALUE_WALK:
      if( state == PLAYER_MOVE_STATE_OFF ||
          state == PLAYER_MOVE_STATE_END ){
        fld_player->move_state = PLAYER_MOVE_STATE_START;
      }else{
        fld_player->move_state = PLAYER_MOVE_STATE_ON;
      }
      break;
    case PLAYER_MOVE_VALUE_TURN:
      fld_player->move_state = PLAYER_MOVE_STATE_ON;
      break;
    }
    
    return;
  }
  
  if( MMDL_CheckEndAcmd(fmmdl) == TRUE ){ //����I��
    switch( value ){
    case PLAYER_MOVE_VALUE_STOP:
      break;
    case PLAYER_MOVE_VALUE_WALK:
      switch( state ){
      case PLAYER_MOVE_STATE_OFF:
        break;
      case PLAYER_MOVE_STATE_END:
        fld_player->move_state = PLAYER_MOVE_STATE_OFF;
        break;
      default:
        fld_player->move_state = PLAYER_MOVE_STATE_END;
      }
      break;
    case PLAYER_MOVE_VALUE_TURN:
      switch( state ){
      case PLAYER_MOVE_STATE_OFF:
        break;
      case PLAYER_MOVE_STATE_END:
        fld_player->move_state = PLAYER_MOVE_STATE_OFF;
        break;
      default:
        fld_player->move_state = PLAYER_MOVE_STATE_END;
      }
      break;
    }

    return;
  }
}

//======================================================================
//	FILED_PLAYER�@�Q�ƁA�ݒ�
//======================================================================
//--------------------------------------------------------------
/**
 * FILED_PLAYER�@���W�擾
 * @param fld_player FIELD_PLAYER
 * @param pos ���W�i�[��
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GetPos( const FIELD_PLAYER *fld_player, VecFx32 *pos )
{
#if 0
	*pos = fld_player->pos;
#else //�\�����W�ƂȂ�A�N�^�[���璼�Ɏ擾
  MMDL_GetVectorPos( fld_player->fldmmdl, pos );
#endif
}

//--------------------------------------------------------------
/**
 * FILED_PLAYER�@���W�Z�b�g
 * @param fld_player FIELD_PLAYER
 * @param pos �Z�b�g������W
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_SetPos( FIELD_PLAYER *fld_player, const VecFx32 *pos )
{
	int gx = SIZE_GRID_FX32( pos->x );
	int gy = SIZE_GRID_FX32( pos->y );
	int gz = SIZE_GRID_FX32( pos->z );
	MMDL *fmmdl = fld_player->fldmmdl;
	
	MMDL_SetOldGridPosX( fmmdl, MMDL_GetGridPosX(fmmdl) );
	MMDL_SetOldGridPosY( fmmdl, MMDL_GetGridPosY(fmmdl) );
	MMDL_SetOldGridPosZ( fmmdl, MMDL_GetGridPosZ(fmmdl) );
	MMDL_SetGridPosX( fmmdl, gx );
	MMDL_SetGridPosY( fmmdl, gy );
	MMDL_SetGridPosZ( fmmdl, gz );
	MMDL_SetVectorPos( fmmdl, pos );
	
	fld_player->pos = *pos;
}

//--------------------------------------------------------------
/**
 * FILED_PLAYER�@�����擾
 * @param fld_player FIELD_PLAYER
 * @retval u16 ����
 */
//--------------------------------------------------------------
u16 FIELD_PLAYER_GetDir( const FIELD_PLAYER *fld_player )
{
#if 0
	return( fld_player->dir );
#else //�\������A�N�^�[���璼�Ɏ擾
  MMDL_GetDirDisp( fld_player->fldmmdl );
#endif
}

//--------------------------------------------------------------
/**
 * FILED_PLAYER�@�����Z�b�g
 * @param fld_player FIELD_PLAYER
 * @param dir ����
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_SetDir( FIELD_PLAYER *fld_player, u16 dir )
{
#if 0
	fld_player->dir = dir;
#else //�\������A�N�^�[�֒��ɃZ�b�g
  MMDL_SetDirDisp( fld_player->fldmmdl, dir );
#endif
}

//--------------------------------------------------------------
/**
 * FILED_PLAYER�@FIELDMAP_WORK�擾
 * @param fld_player FIELD_PLAYER
 * @retval FIELDMAP_WORK*
 */
//--------------------------------------------------------------
FIELDMAP_WORK * FIELD_PLAYER_GetFieldMapWork( FIELD_PLAYER *fld_player )
{
	return( fld_player->fieldWork );
}

//--------------------------------------------------------------
/**
 * FILED_PLAYER�@MMDL�擾
 * @param fld_player FIELD_PLAYER
 * @retval MMDL*
 */
//--------------------------------------------------------------
MMDL * FIELD_PLAYER_GetMMdl( const FIELD_PLAYER *fld_player )
{
	return( fld_player->fldmmdl );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER FLDMAPPER_GRIDINFODATA�擾
 * @param fld_player* FIELD_PLAYER
 * @retval FLDMAPPER_GRIDINFODATA*
 */
//--------------------------------------------------------------
FLDMAPPER_GRIDINFODATA * FIELD_PLAYER_GetGridInfoData(
		FIELD_PLAYER *fld_player )
{
	return( &fld_player->gridInfoData );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER PLAYER_MOVE_VALUE�Z�b�g
 * @param fld_player FIELD_PLAYER
 * @retval PLAYER_MOVE_STATE
 */
//--------------------------------------------------------------
void FIELD_PLAYER_SetMoveValue(
    FIELD_PLAYER *fld_player, PLAYER_MOVE_VALUE val )
{
  fld_player->move_value = val;
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER PLAYER_MOVE_VALUE�擾
 * @param fld_player FIELD_PLAYER
 * @retval PLAYER_MOVE_STATE
 */
//--------------------------------------------------------------
PLAYER_MOVE_VALUE FIELD_PLAYER_GetMoveValue(
    const FIELD_PLAYER *fld_player )
{
  return( fld_player->move_value );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER PLAYER_MOVE_STATE�擾
 * @param fld_player FIELD_PLAYER
 * @retval PLAYER_MOVE_STATE
 */
//--------------------------------------------------------------
PLAYER_MOVE_STATE FIELD_PLAYER_GetMoveState(
    const FIELD_PLAYER *fld_player )
{
  return( fld_player->move_state );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER PLAYER_MOVE_FORM�擾
 * @param fld_player FIELD_PLAYER
 * @retval PLAYER_MOVE_FORM
 */
//--------------------------------------------------------------
PLAYER_MOVE_FORM FIELD_PLAYER_GetMoveForm(
    const FIELD_PLAYER *fld_player )
{
  PLAYER_MOVE_FORM form = PLAYERWORK_GetMoveForm( fld_player->playerWork );
  return( form );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER PLAYER_MOVE_FORM�Z�b�g
 * @param fld_player FIELD_PLAYER
 * @retval PLAYER_MOVE_FORM
 */
//--------------------------------------------------------------
void FIELD_PLAYER_SetMoveForm(
    FIELD_PLAYER *fld_player, PLAYER_MOVE_FORM form )
{
  PLAYERWORK_SetMoveForm( fld_player->playerWork, form );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER FIELD_PLAYER�ɃZ�b�g����Ă��鐫�ʂ��擾
 * @param fld_player FIELD_PLAYER
 * @retval int PM_MALE,PM_FEMALE
 */
//--------------------------------------------------------------
int FIELD_PLAYER_GetSex( const FIELD_PLAYER *fld_player )
{
  return( fld_player->sex );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER GAMESYS_WORK�擾
 * @param fld_player FIELD_PLAYER
 * @retval GAMESYS_WORK*
 */
//--------------------------------------------------------------
GAMESYS_WORK * FIELD_PLAYER_GetGameSysWork( FIELD_PLAYER *fld_player )
{
  return( fld_player->gsys );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �v���C���[�ʒu�̃A�g���r���[�g�̎擾
 *
 *	@param	fld_player  FIELD_PLAYER
 *
 *	@return �}�b�v�A�g���r���[�g
 */
//-----------------------------------------------------------------------------
MAPATTR FIELD_PLAYER_GetMapAttr( const FIELD_PLAYER *fld_player )
{
  MAPATTR attr;
  
  if( fld_player->map_type == FLDMAP_CTRLTYPE_GRID )
  {
    VecFx32 pos;  
    FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( fld_player->fieldWork );

    FIELD_PLAYER_GetPos( fld_player, &pos );
    attr = MAPATTR_GetAttribute( mapper, &pos );
  }
  else if( fld_player->map_type == FLDMAP_CTRLTYPE_NOGRID )
  {
    RAIL_LOCATION location;
    const MMDL * mmdl = FIELD_PLAYER_GetMMdl( (FIELD_PLAYER*)fld_player );
    const FLDNOGRID_MAPPER* mapper = FIELDMAP_GetFldNoGridMapper( fld_player->fieldWork );

    MMDL_GetRailLocation( mmdl, &location );
    attr = FLDNOGRID_MAPPER_GetAttr( mapper, &location );
  }
  
  return( attr );
}

//----------------------------------------------------------------------------
/**
 *	@brief  dir�����̃A�g���r���[�g�̎擾
 *
 *	@param	fld_player  FIELD_PLAYER
 *	@param	dir         ����
 *
 *	@return �}�b�v�A�g���r���[�g
 */
//-----------------------------------------------------------------------------
MAPATTR FIELD_PLAYER_GetDirMapAttr( const FIELD_PLAYER *fld_player, u16 dir )
{
  MAPATTR attr;
  
  if( fld_player->map_type == FLDMAP_CTRLTYPE_GRID )
  {
    VecFx32 pos;  
    FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( fld_player->fieldWork );

    FIELD_PLAYER_GetDirPos( fld_player, dir, &pos );
    attr = MAPATTR_GetAttribute( mapper, &pos );
  }
  else if( fld_player->map_type == FLDMAP_CTRLTYPE_NOGRID )
  {
    RAIL_LOCATION location;
    MMDL * mmdl = FIELD_PLAYER_GetMMdl( fld_player );
    FLDNOGRID_MAPPER* mapper = FIELDMAP_GetFldNoGridMapper( fld_player->fieldWork );

    MMDL_GetRailDirLocation( mmdl, dir, &location );
    attr = FLDNOGRID_MAPPER_GetAttr( mapper, &location );
  }
  
  return( attr );
}


//======================================================================
/// ���ʁAOBJ�R�[�h�A�e�t�H�[��
//======================================================================
//--------------------------------------------------------------
/**
 * ���ʂ�PLAYER_DRAW_FORM����OBJ�R�[�h�擾
 * @param sex PM_MALE,PM_FEMALE
 * @param form PLAYER_DRAW_FORM
 * @retval OBJ�R�[�h
 */
//--------------------------------------------------------------
u16 FIELD_PLAYER_GetDrawFormToOBJCode( int sex, PLAYER_DRAW_FORM form )
{
  const OBJCODE_FORM *tbl = &dataOBJCodeForm[sex][form];
  GF_ASSERT( sex < 2 ); //PM_FEMALE
  GF_ASSERT( form < PLAYER_DRAW_FORM_MAX );
  return( tbl->code );
}

//--------------------------------------------------------------
/**
 * ���ʂ�OBJ�R�[�h���炩��PLAYER_MOVE_FORM�擾
 * @param sex PM_MALE,PM_FEMALE
 * @param code OBJ�R�[�h
 * @retval PLAYER_MOVE_FORM
 * @note PLAYER_MOVE_FORM_MAX���̓A�T�[�g
 */
//--------------------------------------------------------------
PLAYER_MOVE_FORM FIELD_PLAYER_GetOBJCodeToMoveForm( int sex, u16 code )
{
  int i = 0;
  const OBJCODE_FORM *tbl = dataOBJCodeForm[sex];
  GF_ASSERT( sex < 2 ); //PM_FEMALE
  for( ; i < PLAYER_DRAW_FORM_MAX; i++,tbl++ ){
    if( tbl->code == code ){
      GF_ASSERT( tbl->move_form != PLAYER_MOVE_FORM_MAX );
      return(tbl->move_form);
    }
  }
  GF_ASSERT( 0 );
  return( PLAYER_MOVE_FORM_NORMAL );
}

//--------------------------------------------------------------
/**
 * ���ʂ�PLAYER_MOVE_FORM����OBJ�R�[�h�擾
 * @param sex PM_MALE,PM_FEMALE
 * @param form PLAYER_MOVE_FORM
 * @retval u16 OBJ�R�[�h
 */
//--------------------------------------------------------------
u16 FIELD_PLAYER_GetMoveFormToOBJCode( int sex, PLAYER_MOVE_FORM form )
{
  int i = 0;
  const OBJCODE_FORM *tbl = dataOBJCodeForm[sex];
  GF_ASSERT( sex < 2 ); //PM_FEMALE
  GF_ASSERT( form < PLAYER_MOVE_FORM_MAX );
  for( ; i < PLAYER_DRAW_FORM_MAX; i++,tbl++ ){
    if( tbl->move_form == form ){
      return(tbl->code);
    }
  }
  GF_ASSERT( 0 );
  return( HERO );
}

//--------------------------------------------------------------
/**
 * ���ʂ�OBJ�R�[�h����PLAYER_DRAW_FORM�擾
 * @param sex PM_MALE,PM_FEMALE
 * @param code OBJ�R�[�h
 * @retval PLAYER_DRAW_FORM
 */
//--------------------------------------------------------------
PLAYER_DRAW_FORM FIELD_PLAYER_GetOBJCodeToDrawForm( int sex, u16 code )
{
  int i = 0;
  const OBJCODE_FORM *tbl = dataOBJCodeForm[sex];
  GF_ASSERT( sex < 2 ); //PM_FEMALE
  for( ; i < PLAYER_DRAW_FORM_MAX; i++,tbl++ ){
    if( tbl->code == code ){
      return( i );
    }
  }
  GF_ASSERT( 0 );
  return( PLAYER_DRAW_FORM_NORMAL );
}

//======================================================================
//	FILED_PLAYER�@�c�[��
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�̎w�������̈ʒu���O���b�h�P�ʂŎ擾
 * @param fld_player FIELD_PLAYER
 * @param gx X���W�i�[��
 * @param gy Y���W�i�[��
 * @param gz Z���W�i�[��
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GetDirGridPos(
		const FIELD_PLAYER *fld_player, u16 dir, s16 *gx, s16 *gy, s16 *gz )
{
	const MMDL *fmmdl = FIELD_PLAYER_GetMMdl( fld_player );
	
	*gx = MMDL_GetGridPosX( fmmdl );
	*gy = MMDL_GetGridPosY( fmmdl );
	*gz = MMDL_GetGridPosZ( fmmdl );
	*gx += MMDL_TOOL_GetDirAddValueGridX( dir );
	*gz += MMDL_TOOL_GetDirAddValueGridZ( dir );
}

//--------------------------------------------------------------
/**
 * ���@�̎w�������̍��W���擾
 * @param fld_player FIELD_PLAYER
 * @param pos ���W�i�[��
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GetDirPos(
		const FIELD_PLAYER *fld_player, u16 dir, VecFx32 *pos )
{
  s16 gx,gy,gz;
  FIELD_PLAYER_GetDirGridPos( fld_player, dir, &gx, &gy, &gz );
  MMDL_TOOL_GetCenterGridPos( gx, gz, pos );
  pos->y = GRID_SIZE_FX32( gy );
}

//--------------------------------------------------------------
/**
 * ���@�̑O���ʒu���O���b�h�P�ʂŎ擾
 * @param fld_player FIELD_PLAYER
 * @param gx X���W�i�[��
 * @param gy Y���W�i�[��
 * @param gz Z���W�i�[��
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GetFrontGridPos(
		const FIELD_PLAYER *fld_player, s16 *gx, s16 *gy, s16 *gz )
{
	const MMDL *fmmdl = FIELD_PLAYER_GetMMdl( fld_player );
	u16 dir = MMDL_GetDirDisp( fmmdl );
  FIELD_PLAYER_GetDirGridPos( fld_player, dir, gx, gy, gz );
}

//--------------------------------------------------------------
/**
 * ���샂�f���������Ă��邩�`�F�b�N
 * @param fld_player FIELD_PLAYER
 * @retval BOOL TRUE=����
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_CheckLiveMMdl( const FIELD_PLAYER *fld_player )
{
  const MMDL *fmmdl = FIELD_PLAYER_GetMMdl( fld_player );
  
  if( fmmdl == NULL ){
    return( FALSE );
  }
   
  if( MMDL_CheckStatusBitUse(fmmdl) == FALSE ){
    return( FALSE );
  }
  
  if( MMDL_GetOBJID(fmmdl) != MMDL_ID_PLAYER ){
    return( FALSE );
  }
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���@�̓���`�Ԃ�ύX
 * @param fld_player FIELD_PLAYER
 * @param form PLAYER_MOVE_FORM
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_ChangeMoveForm(
    FIELD_PLAYER *fld_player, PLAYER_MOVE_FORM form )
{
  int sex = FIELD_PLAYER_GetSex( fld_player );
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
  u16 code = FIELD_PLAYER_GetMoveFormToOBJCode( sex, form );
  
  if( MMDL_GetOBJCode(mmdl) != code ){
    MMDL_ChangeOBJCode( mmdl, code );
  }
  
  FIELD_PLAYER_SetMoveForm( fld_player, form );
  
  {
    FIELDMAP_WORK *fieldWork = FIELD_PLAYER_GetFieldMapWork( fld_player );
    GAMEDATA *gdata = GAMESYSTEM_GetGameData( fld_player->gsys );
    u32 zone_id = FIELDMAP_GetZoneID( fieldWork );
    FIELD_SOUND_ChangePlayZoneBGM( gdata, form, zone_id );
  }
}

//======================================================================
//	data
//======================================================================
//--------------------------------------------------------------
/// ���@���샂�f���w�b�_�[
//--------------------------------------------------------------
static const MMDL_HEADER data_MMdlHeader =
{
	MMDL_ID_PLAYER,	///<����ID
	HERO,	///<�\������OBJ�R�[�h
	MV_DMY,	///<����R�[�h
	0,	///<�C�x���g�^�C�v
	0,	///<�C�x���g�t���O
	0,	///<�C�x���gID
	0,	///<�w�����
	0,	///<�w��p�����^ 0
	0,	///<�w��p�����^ 1
	0,	///<�w��p�����^ 2
	MOVE_LIMIT_NOT,	///<X�����ړ�����
	MOVE_LIMIT_NOT,	///<Z�����ړ�����
  MMDL_HEADER_POSTYPE_GRID,
};

//--------------------------------------------------------------
/// ���ʁAOBJ�R�[�h�A�e�t�H�[��
//--------------------------------------------------------------
static const OBJCODE_FORM dataOBJCodeForm[2][PLAYER_DRAW_FORM_MAX] =
{
  { //�j��l��
    {HERO,PLAYER_MOVE_FORM_NORMAL},
    {CYCLEHERO,PLAYER_MOVE_FORM_CYCLE},
    {SWIMHERO,PLAYER_MOVE_FORM_SWIM},
  },
  { //����l��
    {HEROINE,PLAYER_MOVE_FORM_NORMAL},
    {CYCLEHEROINE,PLAYER_MOVE_FORM_CYCLE},
    {SWIMHEROINE,PLAYER_MOVE_FORM_SWIM},
  },
};

//======================================================================
//	�ȉ������܂�
//======================================================================
#if 0
GFL_BBDACT_RESUNIT_ID GetPlayerBBdActResUnitID( FIELD_PLAYER *fld_player )
{
	return( fld_player->bbdActResUnitID );
}
#endif

#if 0
void PlayerActGrid_Update(
	FIELD_PLAYER *fld_player, u16 dir, const VecFx32 *pos )
{
	VecFx32 trans;
	fld_player->pos = *pos;
	
	SetGridPlayerActTrans( fld_player, pos );
	MMDL_SetForceDirDisp( fld_player->fldmmdl, dir );
}
#endif

#if 0
void SetGridPlayerActTrans( FIELD_PLAYER* fld_player, const VecFx32* trans )
{
	int gx = SIZE_GRID_FX32( trans->x );
	int gy = SIZE_GRID_FX32( trans->y );
	int gz = SIZE_GRID_FX32( trans->z );
	
	MMDL_SetOldGridPosX( fld_player->fldmmdl,
		MMDL_GetGridPosX(fld_player->fldmmdl) );
	MMDL_SetOldGridPosY( fld_player->fldmmdl,
		MMDL_GetGridPosY(fld_player->fldmmdl) );
	MMDL_SetOldGridPosZ( fld_player->fldmmdl,
		MMDL_GetGridPosZ(fld_player->fldmmdl) );
	
	MMDL_SetGridPosX( fld_player->fldmmdl, gx );
	MMDL_SetGridPosY( fld_player->fldmmdl, gy );
	MMDL_SetGridPosZ( fld_player->fldmmdl, gz );
	MMDL_SetVectorPos( fld_player->fldmmdl, trans );
	
	VEC_Set( &fld_player->pos, trans->x, trans->y, trans->z );
}
#endif

#if 0
void PlayerActGrid_AnimeSet(
	FIELD_PLAYER *fld_player, int dir, PLAYER_ANIME_FLAG flag )
{
	switch( flag ){
	case PLAYER_ANIME_FLAG_STOP:
		MMDL_SetDrawStatus( fld_player->fldmmdl, DRAW_STA_STOP );
		break;
	case PLAYER_ANIME_FLAG_WALK:
		MMDL_SetDrawStatus( fld_player->fldmmdl, DRAW_STA_WALK_8F );
		break;
	default:
		MMDL_SetDrawStatus( fld_player->fldmmdl, DRAW_STA_WALK_4F );
		break;
	}
}
#endif

#if 0
FLDMAPPER_GRIDINFODATA * PlayerActGrid_GridInfoGet( FIELD_PLAYER *fld_player )
{
	return( &fld_player->gridInfoData );
}
#endif

#if 0
void PlayerActGrid_ScaleSizeSet(
	FIELD_PLAYER *fld_player, fx16 sizeX, fx16 sizeY )
{
	GFL_BBDACT_SYS *bbdActSys = FIELDMAP_GetBbdActSys( fld_player->fieldWork );
	int idx = GFL_BBDACT_GetBBDActIdxResIdx(
			bbdActSys, fld_player->bbdActActUnitID );
	GFL_BBD_SetObjectSiz(
		GFL_BBDACT_GetBBDSystem(bbdActSys),
		idx, &sizeX, &sizeY );
}
#endif

#if 0
void PLAYER_GRID_GetFrontGridPos(
	FIELD_PLAYER *fld_player, int *gx, int *gy, int *gz )
{
	int dir;
	MMDL *fmmdl = FIELD_PLAYER_GetMMdl( fld_player );
	
	*gx = MMDL_GetGridPosX( fmmdl );
	*gy = MMDL_GetGridPosY( fmmdl );
	*gz = MMDL_GetGridPosZ( fmmdl );
	dir = MMDL_GetDirDisp( fmmdl );
	
	*gx += MMDL_TOOL_GetDirAddValueGridX( dir );
	*gz += MMDL_TOOL_GetDirAddValueGridZ( dir );
}
#endif
