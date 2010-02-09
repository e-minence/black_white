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

#include "field_player_core.h"
#include "field_player_grid.h"
#include "field_player_nogrid.h"

#include "field/field_const.h"


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
  FIELDMAP_WORK * fieldWork;
  FIELD_PLAYER_CORE * corewk;   // field_player_core
  FIELD_PLAYER_GRID * gridwk;   // field_player_grid
  FIELD_PLAYER_NOGRID * nogridwk; // field_player_nogrid
};

//======================================================================
//	proto
//======================================================================
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
		const VecFx32 *pos, int sex, HEAPID heapID )
{
  FIELD_PLAYER* fld_player;

  fld_player = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_PLAYER) );

  fld_player->fieldWork = fieldWork;

  // COREWK����
  fld_player->corewk = FIELD_PLAYER_CORE_Create( playerWork, fieldWork, pos, sex, heapID );

  return fld_player;
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
  // GRID
  if( fld_player->gridwk ){
    FIELD_PLAYER_GRID_Delete( fld_player->gridwk );
  }
  

  // NOGRID
  if( fld_player->nogridwk ){
    FIELD_PLAYER_NOGRID_Delete( fld_player->nogridwk );
  }
  
  // CORE
  FIELD_PLAYER_CORE_Delete( fld_player->corewk );
  
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
  
  // �R�A�A�b�v�f�[�g
  FIELD_PLAYER_CORE_Update( fld_player->corewk );
  { 
    //�v���C���[���W��PLAYERWORK�֔��f
    VecFx32 pos;
    PLAYER_WORK* playerWork = FIELD_PLAYER_CORE_GetPlayerWork( fld_player->corewk );
    MMDL* fldmmdl = FIELD_PLAYER_CORE_GetMMdl( fld_player->corewk );
    
    MMDL_GetVectorPos( fldmmdl, &pos );
    PLAYERWORK_setPosition( playerWork, &pos );
    
    //������PLAYERWORK�֔��f
    {
      u16 dir = MMDL_GetDirDisp(fldmmdl);
      PLAYERWORK_setDirection_Type( playerWork, dir );
    }
    
    // ���[���Ǝ��̍X�V����
    if( FIELDMAP_GetBaseSystemType(fld_player->fieldWork) == FLDMAP_BASESYS_GRID )
    {
      PLAYERWORK_setPosType( playerWork, LOCATION_POS_TYPE_3D );
    }
    else
    {
      RAIL_LOCATION location;
      MMDL_GetRailLocation( fldmmdl, &location );
      PLAYERWORK_setRailPosition( playerWork, &location );
      PLAYERWORK_setPosType( playerWork, LOCATION_POS_TYPE_RAIL );
    }
  }
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
  
  if( FIELDMAP_GetBaseSystemType(fld_player->fieldWork) == FLDMAP_BASESYS_GRID )
  {
    event = FIELD_PLAYER_GRID_CheckMoveEvent(
        fld_player->gridwk, key_trg, key_cont, evbit );
  }
  else
  {
    OS_TPrintf( "Check Move Event rail not support\n" );
  }
  
  return( event );
}


//--------------------------------------------------------------
/**
 * ���@���N�G�X�g 
 * @param fld_player FIELD_PLAYER
 * @param reqbit FIELD_PLAYER_REQBIT
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_SetRequest(
  FIELD_PLAYER * fld_player, FIELD_PLAYER_REQBIT req_bit )
{
  FIELD_PLAYER_CORE_SetRequest( fld_player->corewk, req_bit );
}

//--------------------------------------------------------------
/**
 * ���N�G�X�g���X�V
 * @param fld_player FIELD_PLAYER
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_UpdateRequest( FIELD_PLAYER * fld_player )
{
  FIELD_PLAYER_CORE_UpdateRequest( fld_player->corewk );
}


//======================================================================
//  �ړ��`�F�b�N
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�I�[�_�[�`�F�b�N
 * @param gjiki FIELD_PLAYER_GRID
 * @param dir �ړ������BDIR_UP��
 * @retval BOOL TRUE=�ړ��\ FALSE=�ړ��s��
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_CheckStartMove(
    FIELD_PLAYER * fld_player, u16 dir )
{
  if( FIELDMAP_GetBaseSystemType(fld_player->fieldWork) == FLDMAP_BASESYS_GRID ){
    return FIELD_PLAYER_GRID_CheckStartMove( fld_player->gridwk, dir );
  }
  return FIELD_PLAYER_CORE_CheckStartMove( fld_player->corewk, dir );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���@��������~������
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_ForceStop( FIELD_PLAYER * fld_player )
{
  if( FIELDMAP_GetBaseSystemType(fld_player->fieldWork) == FLDMAP_BASESYS_GRID ){
    FIELD_PLAYER_GRID_ForceStop( fld_player->gridwk );
  }else{
    FIELD_PLAYER_NOGRID_ForceStop( fld_player->nogridwk );
  }
}

//--------------------------------------------------------------
/**
 * ���@�@�����~
 * @param fld_player
 * @retval BOOL TRUE=��~�����BFALSE=�ړ����ɂ���~�o���Ȃ��B
 */
//--------------------------------------------------------------
void FIELD_PLAYER_SetMoveStop( FIELD_PLAYER * fld_player )
{
  if( FIELDMAP_GetBaseSystemType(fld_player->fieldWork) == FLDMAP_BASESYS_GRID ){
    FIELD_PLAYER_GRID_SetMoveStop( fld_player->gridwk );
  }else{
    //FIELD_PLAYER_NOGRID_ForceStop( fld_player->nogridwk );
    OS_Printf( "rail not support\n" );
    GF_ASSERT( 0 );
  }
}


//--------------------------------------------------------------
/**
 * ���@�ɔg���|�P�����̃^�X�N�|�C���^���Z�b�g
 * @param gjiki FIELD_PLAYER
 * @param task �Z�b�g����FLDEFF_TASK
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_SetEffectTaskWork(
    FIELD_PLAYER * fld_player, FLDEFF_TASK *task )
{
  FIELD_PLAYER_CORE_SetEffectTaskWork( fld_player->corewk, task );
}

//--------------------------------------------------------------
/**
 * ���@�������Ă���G�t�F�N�g�^�X�N�̃|�C���^���擾
 * @param   gjiki FIELD_PLAYER
 * @retval FLDEFF_TASK*
 */
//--------------------------------------------------------------
FLDEFF_TASK * FIELD_PLAYER_GetEffectTaskWork(
    FIELD_PLAYER * fld_player )
{
  return FIELD_PLAYER_CORE_GetEffectTaskWork( fld_player->corewk );
}

//--------------------------------------------------------------
/**
 * �w����������@�ɓn����PLAYER_MOVE_VALUE�͂ǂ��Ȃ邩�`�F�b�N
 * @param fld_player FIELD_PLAYER
 * @param dir �L�[���͕���
 * @retval PLAYER_MOVE_VALUE
 */
//--------------------------------------------------------------
PLAYER_MOVE_VALUE FIELD_PLAYER_GetDirMoveValue(
    FIELD_PLAYER * fld_player, u16 dir )
{
  if( FIELDMAP_GetBaseSystemType(fld_player->fieldWork) == FLDMAP_BASESYS_GRID ){
    return FIELD_PLAYER_GRID_GetMoveValue( fld_player->gridwk, dir );
  }else{
    //FIELD_PLAYER_NOGRID_ForceStop( fld_player->nogridwk );
    OS_Printf( "rail not support\n" );
    GF_ASSERT( 0 );
  }
  return 0;
}

//--------------------------------------------------------------
/**
 * ���@�@���̓L�[����ړ����������Ԃ�
 * @param gjiki FIELD_PLAYER
 * @param key �L�[���
 * @retval u16 �ړ����� DIR_UP��
 */
//--------------------------------------------------------------
u16 FIELD_PLAYER_GetKeyDir( const FIELD_PLAYER* fld_player, int key )
{
  return FIELD_PLAYER_CORE_GetKeyDir( fld_player->corewk, key );
}


//--------------------------------------------------------------
/**
 * ���@��g����Ԃɂ���
 * @param gjiki FIELD_PLAYER
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_SetNaminori( FIELD_PLAYER * fld_player )
{
  FIELD_PLAYER_CORE_SetRequest( fld_player->corewk, FIELD_PLAYER_REQBIT_SWIM );
  FIELD_PLAYER_CORE_UpdateRequest( fld_player->corewk );
}

//--------------------------------------------------------------
/**
 * ���@�g����Ԃ��I������
 * @param gjiki FIELD_PLAYER
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_SetNaminoriEnd( FIELD_PLAYER * fld_player )
{
  FIELD_PLAYER_CORE_SetRequest( fld_player->corewk, FIELD_PLAYER_REQBIT_NORMAL );
  FIELD_PLAYER_CORE_UpdateRequest( fld_player->corewk );
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
  
  //�O���b�h����@�e�틭���ړ��`�F�b�N
  if( FIELDMAP_GetBaseSystemType(
        fld_player->fieldWork) == FLDMAP_BASESYS_GRID )
  {
    //���������ړ��`�F�b�N
    if( FIELD_PLAYER_GRID_CheckUnderForceMove(fld_player->gridwk) == TRUE )
    {
      FIELD_PLAYER_CORE_SetMoveState( fld_player->corewk, PLAYER_MOVE_STATE_ON );
      return;
    }
    
    //���������`�F�b�N
    if( FIELD_PLAYER_GRID_CheckOzeFallOut(fld_player->gridwk) == TRUE )
    {
      FIELD_PLAYER_CORE_SetMoveState( fld_player->corewk, PLAYER_MOVE_STATE_ON );
      return;
    }
  }

  FIELD_PLAYER_CORE_UpdateMoveStatus( fld_player->corewk );
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
  FIELD_PLAYER_CORE_GetPos( fld_player->corewk, pos );
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
  FIELD_PLAYER_CORE_SetPos( fld_player->corewk, pos );
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
  return FIELD_PLAYER_CORE_GetDir( fld_player->corewk );
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
  FIELD_PLAYER_CORE_SetDir( fld_player->corewk, dir );
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
	return FIELD_PLAYER_CORE_GetFieldMapWork( fld_player->corewk );
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
	return FIELD_PLAYER_CORE_GetMMdl( fld_player->corewk );
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
  FIELD_PLAYER_CORE_SetMoveValue( fld_player->corewk, val );
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
  return FIELD_PLAYER_CORE_GetMoveValue( fld_player->corewk );
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
  return FIELD_PLAYER_CORE_GetMoveState( fld_player->corewk );
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
  return FIELD_PLAYER_CORE_GetMoveForm( fld_player->corewk );
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
  FIELD_PLAYER_CORE_SetMoveForm( fld_player->corewk, form );
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
  return FIELD_PLAYER_CORE_GetSex( fld_player->corewk );
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
  return FIELD_PLAYER_CORE_GetGameSysWork( fld_player->corewk );
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
  
  if( FIELDMAP_GetBaseSystemType(fld_player->fieldWork) == FLDMAP_BASESYS_GRID )
  {
    VecFx32 pos;  
    FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( fld_player->fieldWork );

    FIELD_PLAYER_GetPos( fld_player, &pos );
    attr = MAPATTR_GetAttribute( mapper, &pos );
  }
  else if( FIELDMAP_GetBaseSystemType(fld_player->fieldWork) == FLDMAP_BASESYS_RAIL )
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
  
  if( FIELDMAP_GetBaseSystemType(fld_player->fieldWork) == FLDMAP_BASESYS_GRID )
  {
    VecFx32 pos;  
    FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( fld_player->fieldWork );

    FIELD_PLAYER_GetDirPos( fld_player, dir, &pos );
    attr = MAPATTR_GetAttribute( mapper, &pos );
  }
  else if( FIELDMAP_GetBaseSystemType(fld_player->fieldWork) == FLDMAP_BASESYS_RAIL )
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
      if( tbl->move_form != PLAYER_MOVE_FORM_MAX ){
        return(tbl->move_form);
      }
      //PLAYER_MOVE_FORM�����Ă͂܂�Ȃ��R�[�h�ɑ΂���
      //���̊֐����Ă΂�Ă���(�{�����肦�Ȃ�
       GF_ASSERT( tbl->move_form != PLAYER_MOVE_FORM_MAX );
       break;
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
 * BJ�R�[�h������PLAYER_DRAW_FORM�擾
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


//--------------------------------------------------------------
/**
 * OBJ�R�[�h����PLAYER_DRAW_FORM�擾�@�`�F�b�N�p
 * @param sex PM_MALE,PM_FEMALE
 * @param code OBJ�R�[�h
 * @retval PLAYER_DRAW_FORM
 */
//--------------------------------------------------------------
PLAYER_DRAW_FORM FIELD_PLAYER_CheckOBJCodeToDrawForm( u16 code )
{
  int sex,i;

  for( sex = 0; sex < 2; sex++ )
  {
    const OBJCODE_FORM *tbl = dataOBJCodeForm[sex];
    
    for( i = 0; i < PLAYER_DRAW_FORM_MAX; i++, tbl++ )
    {
      if( tbl->code == code )
      {
        return( i );
      }
    }
  }
  
  return( PLAYER_DRAW_FORM_MAX );
}


//--------------------------------------------------------------
/**
 * ���@�����|�[�g��A�C�e���Q�b�g�ȂǃC�x���g�p�\���ɕς��Ă����Ȃ���
 * @param fld_player FIELD_PLAYER
 * @retval  BOOL TRUE=OK
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_CheckChangeEventDrawForm( FIELD_PLAYER *fld_player )
{
  PLAYER_MOVE_FORM form = FIELD_PLAYER_GetMoveForm( fld_player );
  
  switch( form ){
  case PLAYER_MOVE_FORM_SWIM:
    return( FALSE );
  }
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���@�̕\���R�[�h�𒼐ڕύX
 * @param fld_player FIELD_PLAYER
 * @param code �\���R�[�h HERO��
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_ChangeOBJCode( FIELD_PLAYER *fld_player, u16 code )
{
  FIELD_PLAYER_CORE_ChangeOBJCode( fld_player->corewk, code );
}

//--------------------------------------------------------------
/**
 * ���@�̕\���R�[�h�Œ������
 * @param fld_player FIELD_PLAYER
 * @param code �\���R�[�h HERO��
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_ClearOBJCodeFix( FIELD_PLAYER *fld_player )
{
  FIELD_PLAYER_CORE_ClearOBJCodeFix( fld_player->corewk );
}

//--------------------------------------------------------------
/**
 * ���@�̕\���R�[�h�����@��p�̂��̂��ǂ���
 * @param fld_player FIELD_PLAYER
 * @retval BOOL TRUE=���@��p FALSE=���@�ȊO�̃R�[�h
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_CheckIllegalOBJCode( FIELD_PLAYER *fld_player )
{
  return FIELD_PLAYER_CORE_CheckIllegalOBJCode( fld_player->corewk );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �ڂ̑O�ɂ��郂�f��������
 *
 *	@param	fld_player  �t�B�[���h�v���C���[
 *
 *	@return �ڂ̑O�ɂ��郂�f��
 */
//-----------------------------------------------------------------------------
MMDL * FIELD_PLAYER_GetFrontMMdl( const FIELD_PLAYER *fld_player )
{
  MMDL* mmdl = NULL;
  MMDLSYS* mmdlsys = FIELDMAP_GetMMdlSys( fld_player->fieldWork );
  
  if( FIELDMAP_GetBaseSystemType( fld_player->fieldWork ) == FLDMAP_BASESYS_GRID )
  {
    s16 gx,gy,gz;
    
    FIELD_PLAYER_GetFrontGridPos( fld_player, &gx, &gy, &gz );
    mmdl = MMDLSYS_SearchGridPos( mmdlsys, gx, gz, FALSE );
  }
  else
  {
    MMDL* mymmdl = FIELD_PLAYER_CORE_GetMMdl( fld_player->corewk );
    RAIL_LOCATION location;
    
    MMDL_GetRailFrontLocation( mymmdl, &location );
    mmdl = MMDLSYS_SearchRailLocation( mmdlsys, &location, TRUE );
  }
  
  return mmdl;
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
	
  if( FIELDMAP_GetBaseSystemType(fld_player->fieldWork) == FLDMAP_BASESYS_GRID )
  {
    *gx = MMDL_GetGridPosX( fmmdl );
    *gy = MMDL_GetGridPosY( fmmdl );
    *gz = MMDL_GetGridPosZ( fmmdl );
    *gx += MMDL_TOOL_GetDirAddValueGridX( dir );
    *gz += MMDL_TOOL_GetDirAddValueGridZ( dir );
  }
  else
  {
    GF_ASSERT_MSG( 0, "FIELD_PLAYER_GetDirGridPos BaseSystem Rail\n" );
  }
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
  MMDL* fldmmdl = FIELD_PLAYER_CORE_GetMMdl( fld_player->corewk );

  if( FIELDMAP_GetBaseSystemType(fld_player->fieldWork) == FLDMAP_BASESYS_GRID )
  {
    s16 gx,gy,gz;
    FIELD_PLAYER_GetDirGridPos( fld_player, dir, &gx, &gy, &gz );
    MMDL_TOOL_GetCenterGridPos( gx, gz, pos );
    pos->y = GRID_SIZE_FX32( gy );
  }
  else
  {
    fx32 grid_size;
    VecFx16 way;
    RAIL_LOCATION location;
    VecFx32 now_pos;
    FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( fld_player->fieldWork );
    FIELD_RAIL_MAN* p_railman = FLDNOGRID_MAPPER_DEBUG_GetRailMan( p_mapper );

    // ���̈ʒu�ƁADIR�L�[�̂RD�����擾
    MMDL_GetRailLocation( fldmmdl, &location );
    FIELD_RAIL_MAN_GetLocationPosition( p_railman, &location, &now_pos );
    MMDL_Rail_GetDirLineWay( fldmmdl, dir, &way );

    // now_pos����way�ɂP�O���b�h���i�񂾐悪���̃|�W�V����
    grid_size = FIELD_RAIL_MAN_GetRailGridSize( p_railman );
    pos->x = now_pos.x + FX_Mul( way.x, grid_size );
    pos->y = now_pos.y + FX_Mul( way.x, grid_size );
    pos->z = now_pos.z + FX_Mul( way.x, grid_size );
  }
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

//----------------------------------------------------------------------------
/**
 *	@brief  ���@��DIR�̈ړ��������擾
 *
 *	@param	fld_player    FIELD_PLAYER
 *	@param	dir           DIR_�`
 *	@param	way           �RD�ړ�����
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_GetDirWay( 
    const FIELD_PLAYER *fld_player, u16 dir, VecFx32* way )
{
  MMDL* fldmmdl = FIELD_PLAYER_CORE_GetMMdl( fld_player->corewk );
  if( FIELDMAP_GetBaseSystemType(fld_player->fieldWork) == FLDMAP_BASESYS_GRID )
  {
    way->y = 0;
    way->x = MMDL_TOOL_GetDirAddValueGridX( dir );
    way->z = MMDL_TOOL_GetDirAddValueGridZ( dir );

    way->x = GRID_TO_FX32( way->x );
    way->z = GRID_TO_FX32( way->z );
    VEC_Normalize( way, way );
  }
  else
  {
    VecFx16 way16;

    MMDL_Rail_GetDirLineWay( fldmmdl, dir, &way16 );
    VEC_Set( way, way16.x, way16.y, way16.z );
  }
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
  return FIELD_PLAYER_CORE_CheckLiveMMdl( fld_player->corewk );
}
//--------------------------------------------------------------
/**
 * ���@�̓���`�Ԃ�ύX BGM�ύX����
 * @param fld_player FIELD_PLAYER
 * @param form PLAYER_MOVE_FORM
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_ChangeMoveForm(
    FIELD_PLAYER *fld_player, PLAYER_MOVE_FORM form )
{
  FIELD_PLAYER_CORE_ChangeMoveForm( fld_player->corewk, form );
}

//--------------------------------------------------------------
/**
 * ���@�̌��݂̓����ԂōăZ�b�g
 * @param fld_player FIELD_PLAYER
 * @param form PLAYER_MOVE_FORM
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_ResetMoveForm( FIELD_PLAYER *fld_player )
{
  FIELD_PLAYER_CORE_ResetMoveForm( fld_player->corewk );
}

//--------------------------------------------------------------
/**
 * PLAYER_DRAW_FORM���玩�@��OBJ�R�[�h�̂ݕύX
 * @param fld_player FIELD_PLAYER
 * @param form PLAYER_MOVE_FORM
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_ChangeDrawForm(
    FIELD_PLAYER *fld_player, PLAYER_DRAW_FORM form )
{
  FIELD_PLAYER_CORE_ChangeDrawForm( fld_player->corewk, form );
}

//--------------------------------------------------------------
/**
 * ���@��DrawForm���擾
 * @param fld_player FIELD_PLAYER
 * @retval PLAYER_DRAW_FORM
 */
//--------------------------------------------------------------
PLAYER_DRAW_FORM FIELD_PLAYER_GetDrawForm( FIELD_PLAYER *fld_player )
{
  return FIELD_PLAYER_CORE_GetDrawForm( fld_player->corewk );
}

//--------------------------------------------------------------
/**
 * ���@���w��t�H�[���ɕύX���N�G�X�g(�A�j���R�[������)
 * @param fld_player FIELD_PLAYER
 * @param form PLAYER_MOVE_FORM
 * @retval nothing
 *
 * FIELD_PLAYER_ChangeFormWait()�ő҂���
 */
//--------------------------------------------------------------
void FIELD_PLAYER_ChangeFormRequest( FIELD_PLAYER *fld_player, PLAYER_DRAW_FORM form )
{
  FIELD_PLAYER_CORE_ChangeFormRequest( fld_player->corewk, form );
}

//--------------------------------------------------------------
/**
 * ���@�̎w��t�H�[���ύX��҂�
 * @param fld_player FIELD_PLAYER
 * @param form PLAYER_MOVE_FORM
 * @retval nothing
 *
 * FIELD_PLAYER_ChangeFormRequest()�ƃZ�b�g
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_ChangeFormWait( FIELD_PLAYER *fld_player )
{
  return FIELD_PLAYER_CORE_ChangeFormWait( fld_player->corewk );
}

//--------------------------------------------------------------
/**
 * ���@�g���A�g���r���[�g�`�F�b�N
 * @param fld_player
 * @param nattr
 * @param fattr
 * @retval BOOL TRUE=�g���\�A�g���r���[�g�ł���B
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_CheckAttrNaminori(
    FIELD_PLAYER *fld_player, MAPATTR nattr, MAPATTR fattr )
{
  MAPATTR_FLAG attr_flag = MAPATTR_GetAttrFlag( fattr );
  MAPATTR_VALUE f_val = MAPATTR_GetAttrValue( fattr );
  
  if( ((attr_flag&MAPATTR_FLAGBIT_WATER) &&
      MAPATTR_GetHitchFlag(fattr) == FALSE) ||
    MAPATTR_VALUE_CheckShore(f_val) == TRUE ){
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���@������\���̏ꍇ�͌��ɖ߂��B
 * @param fld_player
 * @retval nothing
 * @note ���̏��A�Y���ӏ��͔�������Ԃ̂�
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CheckSpecialDrawForm(
    FIELD_PLAYER *fld_player, BOOL menu_open_flag )
{
  if( FIELDMAP_GetBaseSystemType(
        fld_player->fieldWork) == FLDMAP_BASESYS_GRID ){
    FIELD_PLAYER_GRID_CheckSpecialDrawForm( fld_player->gridwk, menu_open_flag );
  }
  else
  {
    OS_Printf( "rail not support\n" );
    GF_ASSERT(0);
  }
}


//======================================================================
//	Grid ��p����
//======================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  GRID���[�N���Z�b�g�A�b�v����
 *
 *	@param	fld_player  �t�B�[���h���[�N
 *	@param	heapID      �q�[�vID
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_SetUpGrid( FIELD_PLAYER *fld_player, HEAPID heapID )
{
  GF_ASSERT( fld_player->gridwk == NULL );
  fld_player->gridwk = FIELD_PLAYER_GRID_Init( fld_player->corewk, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief  GRID���[�N�̍X�V����
 *
 *	@param	fld_player  �t�B�[���h���[�N
 *	@param	key_trg     �g���K
 *	@param	key_cont    �R���g
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_MoveGrid( FIELD_PLAYER *fld_player, int key_trg, int key_cont )
{
  GF_ASSERT( fld_player->gridwk );
  FIELD_PLAYER_GRID_Move( fld_player->gridwk, key_trg, key_cont );
}


//======================================================================
//	NOGrid ��p����
//======================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  NOGRID���[�N���Z�b�g�A�b�v����
 *
 *	@param	fld_player  �t�B�[���h���[�N
 *	@param	heapID      �q�[�vID
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_SetUpNoGrid( FIELD_PLAYER *fld_player, HEAPID heapID )
{
  GF_ASSERT( fld_player->nogridwk == NULL );
  fld_player->nogridwk = FIELD_PLAYER_NOGRID_Create( fld_player->corewk, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief  NOGRID���[�N�̍X�V����
 *
 *	@param	fld_player  �t�B�[���h���[�N
 *	@param	key_trg     �g���K
 *	@param	key_cont    �R���g
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_MoveNoGrid( FIELD_PLAYER *fld_player, int key_trg, int key_cont )
{
  GF_ASSERT( fld_player->nogridwk );
  FIELD_PLAYER_NOGRID_Move( fld_player->nogridwk, key_trg, key_cont );
}

//----------------------------------------------------------------------------
/**
 *	@brief  NOGRID���[�N����Ďn��
 *
 *	@param	fld_player  �t�B�[���h���[�N
 *	@param	heapID      �q�[�vID
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_RestartNoGrid( FIELD_PLAYER *fld_player, const RAIL_LOCATION* cp_pos )
{
  GF_ASSERT( fld_player->nogridwk );
  FIELD_PLAYER_NOGRID_Restart( fld_player->nogridwk, cp_pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  NOGRID���[�N�����~
 *
 *	@param	fld_player    �t�B�[���h���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_StopNoGrid( FIELD_PLAYER *fld_player )
{
  GF_ASSERT( fld_player->nogridwk );
  FIELD_PLAYER_NOGRID_Stop( fld_player->nogridwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  NOGRID���[�N�@���[�����[�N�̎擾
 *
 *	@param	fld_player  �t�B�[���h�v���C���[
 *
 *	@return ���[�����[�N
 */
//-----------------------------------------------------------------------------
FIELD_RAIL_WORK* FIELD_PLAYER_GetNoGridRailWork( const FIELD_PLAYER *fld_player )
{
  GF_ASSERT( fld_player->nogridwk );
  return FIELD_PLAYER_NOGRID_GetRailWork( fld_player->nogridwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  NOGRID�@���[�����P�[�V������ݒ�
 *
 *	@param	p_player
 *	@param	cp_location 
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_SetNoGridLocation( FIELD_PLAYER* p_player, const RAIL_LOCATION* cp_location )
{
  GF_ASSERT( p_player->nogridwk );
  FIELD_PLAYER_NOGRID_SetLocation( p_player->nogridwk, cp_location );
}

//----------------------------------------------------------------------------
/**
 *	@brief  NOGRID�@���[�����P�[�V�������擾
 *
 *	@param	cp_player     �v���C���[
 *	@param	p_location    ���P�[�V�����i�[��
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_GetNoGridLocation( const FIELD_PLAYER* cp_player, RAIL_LOCATION* p_location )
{
  GF_ASSERT( cp_player->nogridwk );
  FIELD_PLAYER_NOGRID_GetLocation( cp_player->nogridwk, p_location );
}

//----------------------------------------------------------------------------
/**
 *	@brief  NOGRID���[�N  ���[�����[�N�̍��W���擾
 *
 *	@param	cp_player �v���C���[���[�N
 *	@param	p_pos     ���W�i�[��
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_GetNoGridPos( const FIELD_PLAYER* cp_player, VecFx32* p_pos )
{
  GF_ASSERT( cp_player->nogridwk );
  FIELD_PLAYER_NOGRID_GetPos( cp_player->nogridwk, p_pos );
}

//-----------------------------------------------------------------------------
// �R�A���[�N�̎擾
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h�v���C���[����R�A���[�N���擾
 *
 *	@param	p_player    �t�B�[���h�v���C���[
 *
 *	@return �R�A���[�N
 */
//-----------------------------------------------------------------------------
FIELD_PLAYER_CORE * FIELD_PLAYER_GetCoreWk( FIELD_PLAYER* p_player )
{
  return p_player->corewk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h�v���C���[����O���b�h�v���C���[���[�N
 *
 *	@param	fld_player  �t�B�[���h�v���C���[ 
 *
 *	@return �O���b�h�v���C���[���[�N
 */
//-----------------------------------------------------------------------------
FIELD_PLAYER_GRID* FIELD_PLAYER_GetGridWk( FIELD_PLAYER* fld_player )
{
  return fld_player->gridwk;
}






//======================================================================
//	data
//======================================================================

//--------------------------------------------------------------
/// ���ʁAOBJ�R�[�h�A�e�t�H�[��
//--------------------------------------------------------------
static const OBJCODE_FORM dataOBJCodeForm[2][PLAYER_DRAW_FORM_MAX] =
{
  { //�j��l��
    {HERO,PLAYER_MOVE_FORM_NORMAL},
    {CYCLEHERO,PLAYER_MOVE_FORM_CYCLE},
    {SWIMHERO,PLAYER_MOVE_FORM_SWIM},
    {GETHERO,PLAYER_MOVE_FORM_MAX},
    {REPORTHERO,PLAYER_MOVE_FORM_MAX},
    {PCHERO,PLAYER_MOVE_FORM_MAX},
    {HURAHERO,PLAYER_MOVE_FORM_MAX},
    {FISHHERO,PLAYER_MOVE_FORM_MAX},
    {CUTINHERO,PLAYER_MOVE_FORM_MAX},
  },
  { //����l��
    {HEROINE,PLAYER_MOVE_FORM_NORMAL},
    {CYCLEHEROINE,PLAYER_MOVE_FORM_CYCLE},
    {SWIMHEROINE,PLAYER_MOVE_FORM_SWIM},
    {GETHEROINE,PLAYER_MOVE_FORM_MAX},
    {REPORTHEROINE,PLAYER_MOVE_FORM_MAX},
    {PCHEROINE,PLAYER_MOVE_FORM_MAX},
    {HURAHEROINE,PLAYER_MOVE_FORM_MAX},
    {FISHHEROINE,PLAYER_MOVE_FORM_MAX},
    {CUTINHEROINE,PLAYER_MOVE_FORM_MAX},
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
