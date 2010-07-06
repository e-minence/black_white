//======================================================================
/**
 *	GAME FREAK inc.
 *	@file		field_player_core.c
 *	@brief  �t�B�[���h�v���C���[����@�R�A���
 *	@author	gamefreak
 *	@date		2010.02.05
 *	���W���[�����FFIELD_PLAYER_CORE
 */
//======================================================================
#include "gflib.h"

#include "field/field_const.h"

#include "fieldmap.h"
#include "field_g3d_mapper.h"
#include "fldmmdl.h"
#include "field_sound.h"
#include "fldeff_namipoke.h"
#include "fldeff_kemuri.h"
#include "fldeff_bubble.h"

#include "field_player_core.h"

//======================================================================
//  define
//======================================================================
#ifdef ADDFIX100703_REGU_TEX_HERO
#ifdef DEBUG_ONLY_FOR_kagaya
#define PRINTHEAP_TEX_HERO
#endif
#endif

#define REQ_CHG_DRAW_MIDDLE_WAIT (0)

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///	FIELD_PLAYER_CORE
//--------------------------------------------------------------
struct _FIELD_PLAYER_CORE 
{
	HEAPID heapID;
  GAMESYS_WORK *gsys;
	FIELDMAP_WORK *fieldWork;
  PLAYER_WORK *playerWork;
   
  #if 0 //PLAYER_WORK�ֈړ�
  PLAYER_MOVE_FORM move_form;
  #endif
  
  PLAYER_MOVE_STATE move_state;
  PLAYER_MOVE_VALUE move_value;
	
  int sex; //����

	MMDL *fldmmdl;
  
  // �L�[���͏���
  u16 input_key_dir_x; //�L�[���͉�����
  u16 input_key_dir_z; //�L�[���͏c����
  
  // REQ�n�����o
  FIELD_PLAYER_REQBIT req_bit;
  
  // EffectTask
  FLDEFF_TASK *fldeff_joint;
  
  // �����ڍX�V�E�G�C�g�p�@�҂�����
  u16 draw_form_wait;
  u16 chg_draw_form_middle_wait;
  
  // REQ SEQ�����o
  s16 req_seq_no;
  u16 req_seq_end_flag;
};

//--------------------------------------------------------------
/// REQPROC
//--------------------------------------------------------------
typedef int (*REQPROC)(FIELD_PLAYER_CORE *);

//======================================================================
//  proto
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

static void fldplayer_ChangeOBJCode(
    FIELD_PLAYER_CORE *player_core, u16 code );
static void fldplayer_ChangeMoveForm(
    FIELD_PLAYER_CORE *player_core, PLAYER_MOVE_FORM form );
static BOOL fldplayer_CheckOBJCodeHero( FIELD_PLAYER_CORE *player_core );
static BOOL fldplayer_CheckBBDAct( FIELD_PLAYER_CORE *player_core );

///	�L�[���͏���
static u16 gjiki_GetInputKeyDir(
    const FIELD_PLAYER_CORE *player_core, u16 key_prs );
static void gjiki_SetInputKeyDir( FIELD_PLAYER_CORE *player_core, u16 key_prs );
static u16 getKeyDirX( u16 key_prs );
static u16 getKeyDirZ( u16 key_prs );

///	Req�n
static const REQPROC * const data_RequestProcTbl[FIELD_PLAYER_REQBIT_MAX];

//parts
static void gjiki_PlayBGM( FIELD_PLAYER_CORE *player_core );

//======================================================================
//  field player core
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�v���C���[�@�쐬
 * @param playerWork �g�p����PLAYER_WOKR
 * @param *fieldWork �Q�Ƃ���FIELDMAP_WORK
 * @param pos �������W
 * @param sex ����
 * @param heapID HEAPID
 * @retval FIELD_PLAYER_CORE*
 */
//---------------------------------------------------------------
FIELD_PLAYER_CORE * FIELD_PLAYER_CORE_Create(
    PLAYER_WORK *playerWork, FIELDMAP_WORK *fieldWork,
		const VecFx32 *pos, int sex, HEAPID heapID )
{
  u16 fixcode;
	MMDLSYS *fmmdlsys;
	FIELD_PLAYER_CORE *player_core;
	
	player_core = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_PLAYER_CORE) );
  player_core->playerWork = playerWork;
	player_core->fieldWork = fieldWork;
  player_core->gsys = FIELDMAP_GetGameSysWork( fieldWork );

  player_core->input_key_dir_x = DIR_NOT;
  player_core->input_key_dir_z = DIR_NOT;

	//MMDL�Z�b�g�A�b�v
	fmmdlsys = FIELDMAP_GetMMdlSys( fieldWork );
	
	player_core->fldmmdl =
		MMDLSYS_SearchOBJID( fmmdlsys, MMDL_ID_PLAYER );
  
  player_core->sex = sex;
  
  fixcode = PLAYERWORK_GetOBJCodeFix( player_core->playerWork );

	if( player_core->fldmmdl == NULL )	//�V�K
	{
		MMDL_HEADER head;
    MMDL_HEADER_GRIDPOS *gridpos;
		head = data_MMdlHeader;
    gridpos = (MMDL_HEADER_GRIDPOS *)head.pos_buf;
		gridpos->gx = SIZE_GRID_FX32( pos->x );
		gridpos->gz = SIZE_GRID_FX32( pos->z );
		gridpos->y = pos->y;
    
    if( fixcode == OBJCODEMAX )
    {
      PLAYER_MOVE_FORM form = FIELD_PLAYER_CORE_GetMoveForm( player_core );
      head.obj_code = FIELD_PLAYER_GetMoveFormToOBJCode( sex, form );
    }
    else
    {
      head.obj_code = fixcode;
    }
    
		player_core->fldmmdl = MMDLSYS_AddMMdl( fmmdlsys, &head, 0 );
	}
	else //���A
	{
		int gx = SIZE_GRID_FX32( pos->x );
		int gy = SIZE_GRID_FX32( pos->y );
		int gz = SIZE_GRID_FX32( pos->z );
		MMDL *fmmdl = player_core->fldmmdl;
		
		MMDL_SetGridPosX( fmmdl, gx );
		MMDL_SetGridPosY( fmmdl, gy );
		MMDL_SetGridPosZ( fmmdl, gz );
		MMDL_SetVectorPos( fmmdl, pos );
    
    //���|�[�g���̃C�x���g�pOBJ�̏ꍇ�A����t�H�[���ɍ��킹�ĕ��A
    if( fixcode == OBJCODEMAX ){
      FIELD_PLAYER_CORE_ResetMoveForm( player_core );
    }
	}
	
#if 0 // MoveForm���Z�[�u�����悤�ɂȂ������߁A�s�v�Ȃ͂��B2010.04.14
  //OBJ�R�[�h���瓮��t�H�[����ݒ�
  if( fixcode == OBJCODEMAX ){
    u16 code = MMDL_GetOBJCode( player_core->fldmmdl );
    PLAYER_MOVE_FORM form = FIELD_PLAYER_GetOBJCodeToMoveForm( sex, code );
    PLAYERWORK_SetMoveForm( player_core->playerWork, form );
  }
#endif
  
	MMDL_SetStatusBitNotZoneDelete( player_core->fldmmdl, TRUE );
	return( player_core );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�v���C���[�@�폜
 * @param player_core FIELD_PLAYER_CORE
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CORE_Delete( FIELD_PLAYER_CORE *player_core )
{
	//���샂�f���̍폜�̓t�B�[���h���C������
	GFL_HEAP_FreeMemory( player_core );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�v���C���[�@����X�e�[�^�X�X�V
 * @param player_core
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CORE_UpdateMoveStatus( FIELD_PLAYER_CORE *player_core )
{
  MMDL *fmmdl = player_core->fldmmdl;
  PLAYER_MOVE_VALUE value = player_core->move_value;
  PLAYER_MOVE_STATE state = player_core->move_state;
  
  player_core->move_state = PLAYER_MOVE_STATE_OFF;
  
  if( MMDL_CheckPossibleAcmd(fmmdl) == FALSE ) //���쒆
  {
    switch( value )
    {
    case PLAYER_MOVE_VALUE_STOP:
      break;
    case PLAYER_MOVE_VALUE_WALK:
      if( state == PLAYER_MOVE_STATE_OFF || state == PLAYER_MOVE_STATE_END )
      {
        player_core->move_state = PLAYER_MOVE_STATE_START;
      }
      else
      {
        player_core->move_state = PLAYER_MOVE_STATE_ON;
      }
      break;
    case PLAYER_MOVE_VALUE_TURN:
      player_core->move_state = PLAYER_MOVE_STATE_ON;
      break;
    }
    return;
  }
  
  if( MMDL_CheckEndAcmd(fmmdl) == TRUE ) //����I��
  {
    switch( value )
    {
    case PLAYER_MOVE_VALUE_STOP:
      break;
    case PLAYER_MOVE_VALUE_WALK:
      switch( state )
      {
      case PLAYER_MOVE_STATE_OFF:
        break;
      case PLAYER_MOVE_STATE_END:
        player_core->move_state = PLAYER_MOVE_STATE_OFF;
        break;
      default:
        player_core->move_state = PLAYER_MOVE_STATE_END;
      }
      break;
    case PLAYER_MOVE_VALUE_TURN:
      switch( state )
      {
      case PLAYER_MOVE_STATE_OFF:
        break;
      case PLAYER_MOVE_STATE_END:
        player_core->move_state = PLAYER_MOVE_STATE_OFF;
        break;
      default:
        player_core->move_state = PLAYER_MOVE_STATE_END;
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
 * @param player_core FIELD_PLAYER_CORE
 * @param pos ���W�i�[��
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CORE_GetPos(
		const FIELD_PLAYER_CORE *player_core, VecFx32 *pos )
{
  MMDL_GetVectorPos( player_core->fldmmdl, pos );
}

//--------------------------------------------------------------
/**
 * FILED_PLAYER�@���W�Z�b�g
 * @param player_core FIELD_PLAYER_CORE
 * @param pos �Z�b�g������W
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CORE_SetPos(
		FIELD_PLAYER_CORE *player_core, const VecFx32 *pos )
{
	int gx = SIZE_GRID_FX32( pos->x );
	int gy = SIZE_GRID_FX32( pos->y );
	int gz = SIZE_GRID_FX32( pos->z );
  MMDL* fmmdl = player_core->fldmmdl;
	
	MMDL_SetOldGridPosX( fmmdl, MMDL_GetGridPosX(fmmdl) );
	MMDL_SetOldGridPosY( fmmdl, MMDL_GetGridPosY(fmmdl) );
	MMDL_SetOldGridPosZ( fmmdl, MMDL_GetGridPosZ(fmmdl) );
	MMDL_SetGridPosX( fmmdl, gx );
	MMDL_SetGridPosY( fmmdl, gy );
	MMDL_SetGridPosZ( fmmdl, gz );
	MMDL_SetVectorPos( fmmdl, pos );
}

//--------------------------------------------------------------
/**
 * FILED_PLAYER�@�����擾
 * @param player_core FIELD_PLAYER_CORE
 * @retval u16 ����
 */
//--------------------------------------------------------------
u16 FIELD_PLAYER_CORE_GetDir( const FIELD_PLAYER_CORE *player_core )
{
  return MMDL_GetDirDisp(player_core->fldmmdl);
}

//--------------------------------------------------------------
/**
 * FILED_PLAYER�@�����Z�b�g
 * @param player_core FIELD_PLAYER_CORE
 * @param dir ����
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CORE_SetDir( FIELD_PLAYER_CORE *player_core, u16 dir )
{
  MMDL_SetDirDisp( player_core->fldmmdl, dir );
}

//--------------------------------------------------------------
/**
 * ���@�̕\���R�[�h�𒼐ڕύX
 * @param player_core FIELD_PLAYER_CORE
 * @param code �\���R�[�h HERO��
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CORE_ChangeOBJCode( FIELD_PLAYER_CORE *player_core, u16 code )
{
  fldplayer_ChangeOBJCode( player_core, code );
  PLAYERWORK_SetOBJCodeFix( player_core->playerWork, code );
}


//--------------------------------------------------------------
/**
 * ���@�̕\���R�[�h�Œ������
 * @param player_core FIELD_PLAYER_CORE
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CORE_ClearOBJCodeFix( FIELD_PLAYER_CORE *player_core )
{
  PLAYERWORK_SetOBJCodeFix( player_core->playerWork, OBJCODEMAX );
}

//--------------------------------------------------------------
/**
 * FILED_PLAYER_CORE�@FIELDMAP_WORK�擾
 * @param player_core FIELD_PLAYER_CORE
 * @retval FIELDMAP_WORK*
 */
//--------------------------------------------------------------
FIELDMAP_WORK * FIELD_PLAYER_CORE_GetFieldMapWork(
		FIELD_PLAYER_CORE *player_core )
{
	return( player_core->fieldWork );
}

//--------------------------------------------------------------
/**
 * FILED_PLAYER_CORE�@MMDL�擾
 * @param player_core FIELD_PLAYER_CORE
 * @retval MMDL*
 */
//--------------------------------------------------------------
MMDL * FIELD_PLAYER_CORE_GetMMdl( const FIELD_PLAYER_CORE *player_core )
{
	return( player_core->fldmmdl );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER PLAYER_MOVE_VALUE�Z�b�g
 * @param player_core FIELD_PLAYER_CORE
 * @retval PLAYER_MOVE_STATE
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CORE_SetMoveValue(
    FIELD_PLAYER_CORE *player_core, PLAYER_MOVE_VALUE val )
{
  player_core->move_value = val;
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_CORE PLAYER_MOVE_VALUE�擾
 * @param player_core FIELD_PLAYER_CORE
 * @retval PLAYER_MOVE_VALUE
 */
//--------------------------------------------------------------
PLAYER_MOVE_VALUE FIELD_PLAYER_CORE_GetMoveValue(
    const FIELD_PLAYER_CORE *player_core )
{
  return( player_core->move_value );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER PLAYER_MOVE_STATE�Z�b�g
 * @param player_core FIELD_PLAYER_CORE
 * @param val PLAYER_MOVE_STATE
 */
//---------------------------------------------------------------
void FIELD_PLAYER_CORE_SetMoveState(
    FIELD_PLAYER_CORE *player_core, PLAYER_MOVE_STATE val )
{
  player_core->move_state = val;
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_CORE PLAYER_MOVE_STATE�擾
 * @param player_core FIELD_PLAYER_CORE
 * @retval PLAYER_MOVE_STATE_CORE
 */
//--------------------------------------------------------------
PLAYER_MOVE_STATE FIELD_PLAYER_CORE_GetMoveState(
    const FIELD_PLAYER_CORE *player_core )
{
  return( player_core->move_state );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_CORE PLAYER_MOVE_FORM�擾
 * @param player_core FIELD_PLAYER_CORE
 * @retval PLAYER_MOVE_FORM
 */
//--------------------------------------------------------------
PLAYER_MOVE_FORM FIELD_PLAYER_CORE_GetMoveForm(
    const FIELD_PLAYER_CORE *player_core )
{
  PLAYER_MOVE_FORM form = PLAYERWORK_GetMoveForm( player_core->playerWork );
  return( form );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_CORE PLAYER_MOVE_FORM�Z�b�g
 * @param player_core FIELD_PLAYER_CORE
 * @retval PLAYER_MOVE_FORM
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CORE_SetMoveForm(
    FIELD_PLAYER_CORE *player_core, PLAYER_MOVE_FORM form )
{
  PLAYERWORK_SetMoveForm( player_core->playerWork, form );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_CORE FIELD_PLAYER�ɃZ�b�g����Ă��鐫�ʂ��擾
 * @param player_core FIELD_PLAYER_CORE
 * @retval int PM_MALE,PM_FEMALE
 */
//--------------------------------------------------------------
int FIELD_PLAYER_CORE_GetSex( const FIELD_PLAYER_CORE *player_core )
{
  return( player_core->sex );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_CORE GAMESYS_WORK�擾
 * @param player_core FIELD_PLAYER_CORE
 * @retval GAMESYS_WORK*
 */
//--------------------------------------------------------------
GAMESYS_WORK * FIELD_PLAYER_CORE_GetGameSysWork( FIELD_PLAYER_CORE *player_core )
{
  return( player_core->gsys );
}

//--------------------------------------------------------------
/**
 *	@brief  �v���C���[���[�N�̎擾
 *
 *	@param	player_core  ���[�N
 *
 *	@return �v���C���[���[�N
 */
//---------------------------------------------------------------
PLAYER_WORK * FIELD_PLAYER_CORE_GetPlayerWork( const FIELD_PLAYER_CORE * player_core )
{
  return (player_core->playerWork);
}

//--------------------------------------------------------------
/**
 * ���샂�f���������Ă��邩�`�F�b�N
 * @param player_core FIELD_PLAYER_CORE
 * @retval BOOL TRUE=����
 */
//---------------------------------------------------------------
BOOL FIELD_PLAYER_CORE_CheckLiveMMdl( const FIELD_PLAYER_CORE *player_core )
{
  const MMDL *fmmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
  
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
 * ���@�̓���`�Ԃ�ύX BGM�ύX����
 * @param player_core FIELD_PLAYER_CORE
 * @param form PLAYER_MOVE_FORM
 * @retval nothing
 */
//--------------------------------------------------------------
#if 0
void FIELD_PLAYER_CORE_ChangeMoveForm(
    FIELD_PLAYER_CORE *player_core, PLAYER_MOVE_FORM form )
{
  fldplayer_ChangeMoveForm( player_core, form );
  
  {
    FIELDMAP_WORK *fieldWork = player_core->fieldWork;
    GAMEDATA *gdata = GAMESYSTEM_GetGameData( player_core->gsys );
    FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( gdata );
    u32 zone_id = FIELDMAP_GetZoneID( fieldWork );
    FSND_ChangeBGM_byPlayerFormChange( fsnd, gdata, zone_id );
  }
}
#endif

//--------------------------------------------------------------
/**
 * ���@�̌��݂̓����ԂōăZ�b�g
 * @param player_core FIELD_PLAYER_CORE
 * @param form PLAYER_MOVE_FORM
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CORE_ResetMoveForm( FIELD_PLAYER_CORE *player_core )
{
  PLAYER_MOVE_FORM form = FIELD_PLAYER_CORE_GetMoveForm( player_core );
  fldplayer_ChangeMoveForm( player_core, form );
}

//--------------------------------------------------------------
/**
 * PLAYER_DRAW_FORM���玩�@��OBJ�R�[�h�̂ݕύX
 * @param player_core FIELD_PLAYER_CORE
 * @param form PLAYER_MOVE_FORM
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CORE_ChangeDrawForm(
    FIELD_PLAYER_CORE *player_core, PLAYER_DRAW_FORM form )
{
  int sex = FIELD_PLAYER_CORE_GetSex( player_core );
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
  u16 code = FIELD_PLAYER_GetDrawFormToOBJCode( sex, form );
  
  if( MMDL_GetOBJCode(mmdl) != code ){
    fldplayer_ChangeOBJCode( player_core, code );
  }
  
  player_core->draw_form_wait = 0;
}

//--------------------------------------------------------------
/**
 *	@brief  DrawForm�ύX�҂�
 *
 *	@param	player_core ���@���[�N
 *
 *	@retval TRUE  ����
 *	@retval FALSE �ύX��
 */
//---------------------------------------------------------------
BOOL FIELD_PLAYER_CORE_CheckDrawFormWait( FIELD_PLAYER_CORE *player_core )
{
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
  u32 actID;
  actID = MMDL_CallDrawGetProc( mmdl, 0 );
  
  if(actID != MMDL_BLACTID_NULL){

    // �`��ł����Ԃ���1�t���܂�
    if(player_core->draw_form_wait == 1){
      return TRUE;
    }

    player_core->draw_form_wait ++;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ���@��DrawForm���擾
 * @param player_core FIELD_PLAYER_CORE
 * @retval PLAYER_DRAW_FORM
 */
//--------------------------------------------------------------
PLAYER_DRAW_FORM FIELD_PLAYER_CORE_GetDrawForm( FIELD_PLAYER_CORE *player_core )
{
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
  return FIELD_PLAYER_CheckOBJCodeToDrawForm( MMDL_GetOBJCode( mmdl ));
}

//--------------------------------------------------------------
/**
 * ���@���w��t�H�[���ɕύX���N�G�X�g(�A�j���R�[������)
 * @param player_core FIELD_PLAYER_CORE
 * @param form PLAYER_MOVE_FORM
 * @retval nothing
 *
 * FIELD_PLAYER_ChangeFormWait()�ő҂���
 */
//--------------------------------------------------------------
#if 0
void FIELD_PLAYER_CORE_ChangeFormRequest( FIELD_PLAYER_CORE *player_core, PLAYER_DRAW_FORM form )
{
  u16 code;
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );

  FIELD_PLAYER_CORE_ChangeDrawForm( player_core, form );

  switch(form){
  case PLAYER_DRAW_FORM_CUTIN:
    code = AC_HERO_CUTIN;
    break;
  default:
    return;
  }
  MMDL_SetAcmd( mmdl, code );
}
#endif

//--------------------------------------------------------------
/**
 * ���@�̎w��t�H�[���ύX��҂�
 * @param player_core FIELD_PLAYER_CORE
 * @param form PLAYER_MOVE_FORM
 * @retval nothing
 *
 * FIELD_PLAYER_ChangeFormRequest()�ƃZ�b�g
 */
//--------------------------------------------------------------
#if 0
BOOL FIELD_PLAYER_CORE_ChangeFormWait( FIELD_PLAYER_CORE *player_core )
{
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
  BOOL result1, result2;
  result1 = FIELD_PLAYER_CORE_CheckDrawFormWait( player_core );
  result2 = MMDL_CheckEndAcmd( mmdl );
  
  // �����ڂ̍X�V�ƁA�A�N�V�����R�}���h�̏I����҂�
  if( (result1 == TRUE) && (result2 == result1) ){
    return TRUE;
  }
  return FALSE;
}
#endif

//--------------------------------------------------------------
/**
 * ���@�̕\���R�[�h�����@��p�̂��̂��ǂ���
 * @param player_core FIELD_PLAYER
 * @retval BOOL TRUE=���@��p FALSE=���@�ȊO�̃R�[�h
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_CORE_CheckIllegalOBJCode( FIELD_PLAYER_CORE *player_core )
{
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
  u16 code = MMDL_GetOBJCode( mmdl );
  if( FIELD_PLAYER_CheckOBJCodeToDrawForm(code) != PLAYER_DRAW_FORM_MAX ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 *	@brief  ���@�̃A�j���[�V���������҂�
 *
 *	@param	player_core   ���[�N
 *  
 *	@retval TRUE    ����
 *	@retval FALSE   �r��
 */
//---------------------------------------------------------------
BOOL FIELD_PLAYER_CORE_CheckAnimeEnd( const FIELD_PLAYER_CORE *player_core )
{
  MMDLSYS* p_mmdlsys = MMDL_GetMMdlSys( player_core->fldmmdl );
  MMDL_BLACTCONT * p_mmdl_bbdadct = MMDLSYS_GetBlActCont( p_mmdlsys );
  GFL_BBDACT_SYS* p_bbdact = MMDL_BLACTCONT_GetBbdActSys( p_mmdl_bbdadct );
  u32 actID;
  u16 comm;

  if( (MMDL_CheckDrawPause(player_core->fldmmdl) == TRUE) && //�`��|�[�YON
      (MMDL_BLACTCONT_CheckUpdateBBD(player_core->fldmmdl) == TRUE) ){ //�A�N�^�[�X�V�ς�
    return TRUE;
  }
  
  actID = MMDL_CallDrawGetProc( player_core->fldmmdl, 0 );
  if( actID != MMDL_BLACTID_NULL ){

    if( GFL_BBDACT_GetAnimeLastCommand( p_bbdact, actID, &comm ) ){

      if( comm == GFL_BBDACT_ANMCOM_END ){
        return TRUE;
      }
    }
  }
  
  return FALSE;
}

//======================================================================
//  �ړ��`�F�b�N
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�I�[�_�[�`�F�b�N
 * @param player_core FIELD_PLAYER_GRID
 * @param dir �ړ������BDIR_UP��
 * @retval BOOL TRUE=�ړ��\ FALSE=�ړ��s��
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_CORE_CheckStartMove(
    const FIELD_PLAYER_CORE * player_core, u16 dir )
{
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
  
  if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
    return( TRUE );
  }
  
  if( dir == DIR_NOT ){
    return( FALSE );
  }

#if 0  
  if( Jiki_WallHitAcmdCodeCheck(code) == TRUE ){ //�ǃq�b�g�̓L�����Z���\
  }
#endif

  return( FALSE );
}


//--------------------------------------------------------------
/**
 * ���@�@���̓L�[����ړ����������Ԃ�
 * @param player_core FIELD_PLAYER_CORE
 * @param key �L�[���
 * @retval u16 �ړ����� DIR_UP��
 */
//--------------------------------------------------------------
u16 FIELD_PLAYER_CORE_GetKeyDir( const FIELD_PLAYER_CORE* player_core, int key )
{
  u16 dir = gjiki_GetInputKeyDir( player_core, key );
  return dir;
}

//--------------------------------------------------------------
/**
 * �ړ��J�n���̃L�[���͏��Z�b�g
 * @param player_core FIELD_PLAYER_GRID
 * @param key_prs �L�[���͏��
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CORE_SetMoveStartKeyDir( FIELD_PLAYER_CORE* player_core, int key )
{
  gjiki_SetInputKeyDir( player_core, key );
}

//--------------------------------------------------------------
/**
 * ���@�ɔg���|�P�����̃^�X�N�|�C���^���Z�b�g
 * @param player_core FIELD_PLAYER_CORE
 * @param task �Z�b�g����FLDEFF_TASK
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CORE_SetEffectTaskWork(
    FIELD_PLAYER_CORE *player_core, FLDEFF_TASK *task )
{
  player_core->fldeff_joint = task;
}

//--------------------------------------------------------------
/**
 * ���@�������Ă���G�t�F�N�g�^�X�N�̃|�C���^���擾
 * @param   player_core FIELD_PLAYER_CORE
 * @retval FLDEFF_TASK*
 */
//--------------------------------------------------------------
FLDEFF_TASK * FIELD_PLAYER_CORE_GetEffectTaskWork(
    FIELD_PLAYER_CORE *player_core )
{
  return( player_core->fldeff_joint );
}

//======================================================================
//  private�֐�
//======================================================================
//--------------------------------------------------------------
//  ADDFIX100703_REGU_TEX_HERO 
//--------------------------------------------------------------
#ifdef PRINTHEAP_TEX_HERO
#include "system/main.h"  //HEAPID_FIELDMAP
static void printHeap( int num, u16 code )
{
  switch( FIELD_PLAYER_CheckOBJCodeToDrawForm(code) ){
  case PLAYER_DRAW_FORM_NORMAL:
    OS_TPrintf( "HERO " ); break;
  case PLAYER_DRAW_FORM_CYCLE:
    OS_TPrintf( "CYCLE " ); break;
  case PLAYER_DRAW_FORM_SWIM:
    OS_TPrintf( "SWIM " ); break;
  case PLAYER_DRAW_FORM_DIVING:
    OS_TPrintf( "DIVING " ); break;
  case PLAYER_DRAW_FORM_ITEMGET:
    OS_TPrintf( "ITEMGET " ); break;
  case PLAYER_DRAW_FORM_SAVEHERO:
    OS_TPrintf( "SAVE " ); break;
  case PLAYER_DRAW_FORM_PCHERO:
    OS_TPrintf( "PC " ); break;
  case PLAYER_DRAW_FORM_YURE:
    OS_TPrintf( "YURE " ); break;
  case PLAYER_DRAW_FORM_FISHING:
    OS_TPrintf( "FISHING " ); break;
  case PLAYER_DRAW_FORM_CUTIN:
    OS_TPrintf( "CUTIN " ); break;
  }

  OS_TPrintf( "%d HEAP_FIELD %6x(%6x)\n",
      num,
      GFI_HEAP_GetHeapAllocatableSize( HEAPID_FIELDMAP ),
      GFL_HEAP_GetHeapFreeSize( HEAPID_FIELDMAP ) );
}
#endif

//--------------------------------------------------------------
/**
 * ���@�A���샂�f����OBJ�R�[�h��ύX����
 * @param player_core FIELD_PLAYER�QCORE
 * @param code OBJ�R�[�h
 * @retval nothing
 */
//--------------------------------------------------------------
//----
#ifdef ADDFIX100703_REGU_TEX_HERO
//----
static void fldplayer_ChangeOBJCode(
    FIELD_PLAYER_CORE *player_core, u16 code )
{
  u16 now_code;
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
  MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  
#if 0  
  OS_TPrintf( "*****HEAPID_FIELD****\n" );
  GFL_HEAP_DEBUG_PrintExistMemoryBlocks( HEAPID_FIELDMAP );
  OS_TPrintf( "****HEAPID_FIELD****\n" );
#endif
  
  now_code = MMDL_GetOBJCode( mmdl );

#ifdef PRINTHEAP_TEX_HERO  
  OS_TPrintf( "---- ���@�ύX�J�n\n" );
  printHeap( 0, now_code );
#endif
  
  //���݂�HERO,HEROINE�A�������͕ύX��HERO,HEROINE�ł����
  //�P��̐؂�ւ��̂�
  if( code == HERO || code == HEROINE ||
      now_code == HERO || now_code == HEROINE )
  {
    MMDL_BLACTCONT_ChangeOBJCodeWithDummy( mmdl, code );
    MMDLSYS_ForceWaitVBlankProc( mmdlsys );
  }
  else //HERO,HEROINE����Ȃ��Ă͂Ȃ�Ȃ����̐؂�ւ�
  {
    int sex = FIELD_PLAYER_CORE_GetSex( player_core );
    u16 hero_code = FIELD_PLAYER_GetMoveFormToOBJCode(
        sex, PLAYER_DRAW_FORM_NORMAL );
    
    MMDL_ChangeOBJCode( mmdl, hero_code ); //HERO�ւ̒u�������̓_�~�[�s�v
    MMDLSYS_ForceWaitVBlankProc( mmdlsys );
    
#ifdef PRINTHEAP_TEX_HERO  
    OS_TPrintf( "��U���@�߂��� " );
    printHeap( 1, hero_code );
#endif
    
    MMDL_BLACTCONT_ChangeOBJCodeWithDummy( mmdl, code );
    MMDLSYS_ForceWaitVBlankProc( mmdlsys );
  }
  
#ifdef PRINTHEAP_TEX_HERO  
  printHeap( 99, code );
  OS_TPrintf( "----���@�ύX����\n" );
#endif
}
//----
#else //old
//----
static void fldplayer_ChangeOBJCode(
    FIELD_PLAYER_CORE *player_core, u16 code )
{
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
//  MMDL_ChangeOBJCode( mmdl, code );
  MMDL_BLACTCONT_ChangeOBJCodeWithDummy( mmdl, code );
}
//----
#endif
//----

//--------------------------------------------------------------
/**
 * ���@�̓���`�Ԃ�ύX
 * @param player_core FIELD_PLAYER�QCORE
 * @param form PLAYER_MOVE_FORM
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldplayer_ChangeMoveForm(
    FIELD_PLAYER_CORE *player_core, PLAYER_MOVE_FORM form )
{
  int sex = FIELD_PLAYER_CORE_GetSex( player_core );
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
  u16 code = FIELD_PLAYER_GetMoveFormToOBJCode( sex, form );
  
  if( MMDL_GetOBJCode(mmdl) != code ){
    fldplayer_ChangeOBJCode( player_core, code );
  }
  FIELD_PLAYER_CORE_SetMoveForm( player_core, form );
}

//--------------------------------------------------------------
/**
 * ���@�̕\���R�[�h��HERO,HEROINE���ǂ���
 * @param player_core FIELD_PLAYER_CORE
 * @retval BOOL TRUE=HERO,HEROINE�ł���
 */
//--------------------------------------------------------------
static BOOL fldplayer_CheckOBJCodeHero( FIELD_PLAYER_CORE *player_core )
{
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
  u16 code = MMDL_GetOBJCode( mmdl );

  if( code == HERO || code == HEROINE ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���@�̃r���{�[�h�A�N�^�[�`�F�b�N
 * @param player_core FIELD_PLAYER_CORE
 * @retval BOOL TRUE=�A�N�^�[���݂��Ă���
 */
//--------------------------------------------------------------
static BOOL fldplayer_CheckBBDAct( FIELD_PLAYER_CORE *player_core )
{
  MMDL *mmdl = player_core->fldmmdl;
  u16 actID = MMDL_CallDrawGetProc( mmdl, 0 );
  
  if( actID != MMDL_BLACTID_NULL ){
    return( TRUE );
  }
  
  return( FALSE );
}

//======================================================================
///	�L�[���͏���
//======================================================================
//--------------------------------------------------------------
/**
 * �L�[���͕����擾
 * @param player_core
 * @param key_prs �L�[���͏��
 * @retval u16 �L�[���͕���
 */
//--------------------------------------------------------------
static u16 gjiki_GetInputKeyDir(
    const FIELD_PLAYER_CORE *player_core, u16 key_prs )
{
  int key_dir_x = getKeyDirX( key_prs );
  int key_dir_z = getKeyDirZ( key_prs );
  
  if( key_dir_x == DIR_NOT ){   //����������̏ꍇ��Z�D��
    return( key_dir_z );
  }
  
  if( key_dir_z == DIR_NOT ){   //Z�L�[���� X�L�[�Ԃ�
    return( key_dir_x );
  }
  
  { //�΂߉���
    MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
    u16 move_dir = MMDL_GetDirMove( mmdl );
    u16 input_x = player_core->input_key_dir_x;
    u16 input_z = player_core->input_key_dir_z;
    
    //�ړ������ƈ�v���Ȃ�������Z�D��ŕԂ�
    if( move_dir != DIR_NOT ){
      //�ߋ��ɉ������������p��
      if( key_dir_x == input_x && key_dir_z == input_z ){
        return( move_dir ); //�ړ����ƈ�v���������Ԃ�
      }
      
      if( key_dir_z != input_z ){  //�V�K�΂߉�����Z�D��ŕԂ�
        return( key_dir_z );
      }
      
      return( key_dir_x );
    }
    
    return( key_dir_z ); //Z�D��ŕԂ�
  }
  
  return( DIR_NOT );
}

//--------------------------------------------------------------
/**
 * �L�[���͏��Z�b�g
 * @param player_core FIELD_PLAYER_CORE
 * @param key_prs �L�[���͏��
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetInputKeyDir( FIELD_PLAYER_CORE *player_core, u16 key_prs )
{
  player_core->input_key_dir_x = getKeyDirX( key_prs );
  player_core->input_key_dir_z = getKeyDirZ( key_prs );
}

//--------------------------------------------------------------
/**
 * �L�[�v���X���牟����Ă���X�������擾
 * @param key_prs �L�[���͏��
 * @retval u16 ������Ă������ DIR_UP��
 */
//--------------------------------------------------------------
static u16 getKeyDirX( u16 key_prs )
{
  if( (key_prs & PAD_KEY_LEFT) ){
    return( DIR_LEFT );
  }
  
  if( (key_prs & PAD_KEY_RIGHT) ){
    return( DIR_RIGHT );
  }
  
  return( DIR_NOT );
}

//--------------------------------------------------------------
/**
 * �L�[�v���X���牟����Ă���Z�������擾
 * @param key_prs �L�[���͏��
 * @retval u16 ������Ă������ DIR_UP��
 */
//--------------------------------------------------------------
static u16 getKeyDirZ( u16 key_prs )
{
  if( (key_prs & PAD_KEY_UP) ){
    return( DIR_UP );
  }
  
  if( (key_prs & PAD_KEY_DOWN) ){
    return( DIR_DOWN );
  }
  
  return( DIR_NOT );
}

//======================================================================
//  ���@���N�G�X�g
//======================================================================
//--------------------------------------------------------------
/**
 * �w�胊�N�G�X�g�����s
 */
//--------------------------------------------------------------
static BOOL updateRequest( FIELD_PLAYER_CORE *player_core, int no )
{
  if( player_core->req_seq_end_flag == 0 ){
    int ret;
    
    do{
      ret = data_RequestProcTbl[no][player_core->req_seq_no]( player_core );
    }while( ret == TRUE );
  }
  
  return( player_core->req_seq_end_flag );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_REQBIT -> �ԍ�
 */
//--------------------------------------------------------------
static int reqBit_to_Number( u32 bit )
{
  int i = 0;
  
  do{
    if( (bit&0x01) ){
      break;
    }
    bit >>= 1;
    i++;
  }while( i < FIELD_PLAYER_REQBIT_MAX );
  
  return( i );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g 
 * @param player_core FIELD_PLAYER_CORE
 * @param reqbit FIELD_PLAYER_REQBIT
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CORE_SetRequest(
  FIELD_PLAYER_CORE * player_core, FIELD_PLAYER_REQBIT req_bit )
{
  player_core->req_bit = req_bit;
  player_core->req_seq_no = 0;
  player_core->req_seq_end_flag = 0;
  player_core->draw_form_wait = 0;
  player_core->chg_draw_form_middle_wait = 0;
}

//--------------------------------------------------------------
/**
 * ���N�G�X�g���X�V
 * @param player_core FIELD_PLAYER_CORE
 * @retval BOOL TRUE=���N�G�X�g��������
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_CORE_UpdateRequest( FIELD_PLAYER_CORE *player_core )
{
  FIELD_PLAYER_REQBIT bit = player_core->req_bit;
  int no = reqBit_to_Number( bit );
  
  if( no >= FIELD_PLAYER_REQBIT_MAX ){
    return( TRUE );
  }
  
  if( updateRequest(player_core,no) == TRUE ){
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���N�G�X�g�������X�V
 * @param player_core FIELD_PLAYER_CORE
 * @retval nothing
 * @attention ���N�G�X�g���������I���܂ŋ������s���܂��B
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CORE_ForceUpdateRequest( FIELD_PLAYER_CORE *player_core )
{
  FIELD_PLAYER_REQBIT bit = player_core->req_bit;
  int no = reqBit_to_Number( bit );
  
  if( no >= FIELD_PLAYER_REQBIT_MAX ){
    GF_ASSERT( 0 );
    return;
  }
  
  while( updateRequest(player_core,no) == FALSE ){};
}

#if 0 //old �r�b�g�P�ʂŕ����������Ă���
void FIELD_PLAYER_CORE_SetRequest(
  FIELD_PLAYER_CORE * player_core, FIELD_PLAYER_REQBIT req_bit )
{
  player_core->req_bit = req_bit;
}

void FIELD_PLAYER_CORE_UpdateRequest( FIELD_PLAYER_CORE *player_core )
{
  int i = 0;
  FIELD_PLAYER_REQBIT req_bit  = player_core->req_bit;
  
  static void (* const data_RequestProcTbl[FIELD_PLAYER_REQBIT_MAX])( FIELD_PLAYER_CORE *player_core ) =
  {
    gjikiReq_SetNormal, //FIELD_PLAYER_REQBIT_NORMAL
    gjikiReq_SetCycle, //FIELD_PLAYER_REQBIT_CYCLE
    gjikiReq_SetSwim, //FIELD_PLAYER_REQBIT_SWIM
    gjikiReq_SetMoveFormToDrawForm,
    gjikiReq_SetItemGetHero,
    gjikiReq_SetReportHero,
    gjikiReq_SetPCAzukeHero,
    gjikiReq_SetCutInHero,
    gjikiReq_SetDiving, //FIELD_PLAYER_REQBIT_SWIM
  };
  
  while( i < FIELD_PLAYER_REQBIT_MAX ){
    if( (req_bit&0x01) ){
      data_RequestProcTbl[i]( player_core );
    }
    req_bit >>= 1;
    i++;
  }
   
  player_core->req_bit = 0;
}
#endif

//======================================================================
//  ���@���N�G�X�g�@�ėp
//======================================================================
//--------------------------------------------------------------
//  ���N�G�X�g req_seq_no����
//--------------------------------------------------------------
#define req_NextSeqNo( player_core ) player_core->req_seq_no++;

//--------------------------------------------------------------
//  ���N�G�X�g req_seq_end_flag�𗧂Ă�
//--------------------------------------------------------------
#define req_SetEndFlag( player_core ) player_core->req_seq_end_flag = TRUE;

//--------------------------------------------------------------
/**
 * ���N�G�X�g�@���@��OBJ�R�[�h��ύX����
 * @param
 * @retval BOOL TRUE=�ύX�������͓���BFALSE=HERO,HEROINE�ɕύX����
 */
//--------------------------------------------------------------
static BOOL reqCommon_SetOBJCode(
    FIELD_PLAYER_CORE *player_core, u16 code )
{
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
  
  if( MMDL_GetOBJCode(mmdl) == code ){
    req_SetEndFlag( player_core );
    return( TRUE );
  }
  
  if( fldplayer_CheckOBJCodeHero(player_core) == TRUE ){
    fldplayer_ChangeOBJCode( player_core, code );
    req_SetEndFlag( player_core );
    return( TRUE );
  }
  
  {
    int sex = FIELD_PLAYER_CORE_GetSex( player_core );
    code = FIELD_PLAYER_GetDrawFormToOBJCode( sex, PLAYER_DRAW_FORM_NORMAL );
    fldplayer_ChangeOBJCode( player_core, code );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���N�G�X�g�@PLAYER_DRAW_FORM�Z�b�g
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL reqCommon_SetDrawForm(
    FIELD_PLAYER_CORE *player_core, PLAYER_DRAW_FORM form )
{
  int sex = FIELD_PLAYER_CORE_GetSex( player_core );
  u16 code = FIELD_PLAYER_GetDrawFormToOBJCode( sex, form );
  return( reqCommon_SetOBJCode(player_core,code) );
}

//--------------------------------------------------------------
/**
 * ���N�G�X�g�@���@�̕\���R�[�h�ύX��҂��A�w��PLAYER_DRAW_FORM�ɕύX
 * @param 
 * @retval
 */
//--------------------------------------------------------------
static BOOL reqCommon_WaitSetDrawForm(
    FIELD_PLAYER_CORE *player_core, PLAYER_DRAW_FORM form )
{
  if( fldplayer_CheckBBDAct(player_core) == TRUE ){
    player_core->chg_draw_form_middle_wait++;
    
    if( player_core->chg_draw_form_middle_wait >= REQ_CHG_DRAW_MIDDLE_WAIT ){
      int sex = FIELD_PLAYER_CORE_GetSex( player_core );
      u16 code = FIELD_PLAYER_GetDrawFormToOBJCode( sex, form );
      fldplayer_ChangeOBJCode( player_core, code );
      req_SetEndFlag( player_core );
      return( TRUE );
    }
  }
  
  return( FALSE );
}

//======================================================================
//  ���@���N�G�X�g�@�ʏ�ړ��ɕύX
//======================================================================
//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@�ʏ�ړ��ɕύX 0
 * @param player_core FIELD_PLAYER_CORE
 * @retval nothing
 */
//---------------------------------------------------------------
static int req_SetNormal0( FIELD_PLAYER_CORE *player_core )
{
  VecFx32 offs = {0,0,0};
  int sex;
  u16 code;
  MMDL *mmdl;
  FLDEFF_TASK * task;
  
  sex = FIELD_PLAYER_CORE_GetSex( player_core );
  mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
  code = FIELD_PLAYER_GetDrawFormToOBJCode( sex, PLAYER_DRAW_FORM_NORMAL );
  
  if( MMDL_GetOBJCode(mmdl) != code ){
    fldplayer_ChangeOBJCode( player_core, code );
  }
  
  FIELD_PLAYER_CORE_SetMoveForm( player_core, PLAYER_MOVE_FORM_NORMAL );
  gjiki_PlayBGM( player_core );
  
  // JoinEffect������΍폜
  task = FIELD_PLAYER_CORE_GetEffectTaskWork( player_core );
  
  if( task != NULL ){
    FLDEFF_TASK_CallDelete( task );
    FIELD_PLAYER_CORE_SetEffectTaskWork( player_core, NULL );
  }
  
  MMDL_SetVectorOuterDrawOffsetPos( mmdl, &offs );
  
  req_SetEndFlag( player_core );
  return( FALSE );
}

//--------------------------------------------------------------
//  ���@���N�G�X�g�@�ʏ�ړ��ɕύX�@�܂Ƃ�
//--------------------------------------------------------------
static const REQPROC data_req_SetNormalTbl[] =
{
  req_SetNormal0,
};

//======================================================================
//  ���@���N�G�X�g�@���]�Ԉړ��ɕύX
//======================================================================
//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@���]�Ԉړ��ɕύX 0
 * @param player_core FIELD_PLAYER_CORE
 * @retval int TRUE=�ċA�Ăяo��
 */
//--------------------------------------------------------------
static int req_SetCycle0( FIELD_PLAYER_CORE *player_core )
{
  int sex;
  u16 code;
  MMDL *mmdl;
  
  sex = FIELD_PLAYER_CORE_GetSex( player_core );
  mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
  
  FIELD_PLAYER_CORE_SetMoveForm( player_core, PLAYER_MOVE_FORM_CYCLE );
  gjiki_PlayBGM( player_core );
  
  if( reqCommon_SetDrawForm(player_core,PLAYER_DRAW_FORM_CYCLE) != TRUE ){
    req_NextSeqNo( player_core );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@���]�Ԉړ��ɕύX 1
 * @param player_core FIELD_PLAYER_CORE
 * @retval int TRUE=�ċA�Ăяo��
 */
//--------------------------------------------------------------
static int req_SetCycle1( FIELD_PLAYER_CORE *player_core )
{
  reqCommon_WaitSetDrawForm( player_core, PLAYER_DRAW_FORM_CYCLE );
  return( FALSE );
}

//--------------------------------------------------------------
//  ���@���N�G�X�g�@���]�Ԉړ��ɕύX�@�܂Ƃ�
//--------------------------------------------------------------
static const REQPROC data_req_SetCycleTbl[] =
{
  req_SetCycle0,
  req_SetCycle1,
};

//======================================================================
//  ���@���N�G�X�g�@�g���ړ��ɕύX
//======================================================================
//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@�g���ړ��ɕύX 0
 * @param player_core FIELD_PLAYER_CORE
 * @retval nothing
 */
//--------------------------------------------------------------
static int req_SetSwim0( FIELD_PLAYER_CORE *player_core )
{
  int sex;
  u16 code;
  MMDL *mmdl;
  
  sex = FIELD_PLAYER_CORE_GetSex( player_core );
  mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
  code = FIELD_PLAYER_GetDrawFormToOBJCode( sex, PLAYER_DRAW_FORM_SWIM );
  
  if( MMDL_GetOBJCode(mmdl) != code ){
    fldplayer_ChangeOBJCode( player_core, code );
  }
  
  if(FIELD_PLAYER_CORE_GetMoveForm(player_core) != PLAYER_MOVE_FORM_SWIM ){
    FIELD_PLAYER_CORE_SetMoveForm( player_core, PLAYER_MOVE_FORM_SWIM );
  }
  
  gjiki_PlayBGM( player_core );
  
  //�g���|�P����
  if( FIELD_PLAYER_CORE_GetEffectTaskWork(player_core) == NULL ){
    u16 dir;
    VecFx32 pos;
    FLDEFF_CTRL *fectrl;
    FLDEFF_TASK* task;
    
    fectrl = FIELDMAP_GetFldEffCtrl( player_core->fieldWork );
    dir = MMDL_GetDirDisp( mmdl );
    MMDL_GetVectorPos( mmdl, &pos );
    
    task = FLDEFF_NAMIPOKE_SetMMdl(
        fectrl, dir, &pos, mmdl, NAMIPOKE_JOINT_ON );

    FIELD_PLAYER_CORE_SetEffectTaskWork( player_core, task );
  }
  
  //�����Ŕg���ɒu�������郊�N�G�X�g�Ȃ̂�
  //�\���ύX�҂��͗v��Ȃ�
  req_SetEndFlag( player_core );
  return( FALSE );
}

//--------------------------------------------------------------
//  ���@���N�G�X�g�@�g���ړ��ɕύX�@�܂Ƃ�
//--------------------------------------------------------------
static const REQPROC data_req_SetNaminoriTbl[] =
{
  req_SetSwim0,
};

//======================================================================
//  ���@���N�G�X�g�@����`�Ԃɂ��킹���\���ɂ���
//======================================================================
//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@����`�Ԃɂ��킹���\���ɂ��� 0
 * @param player_core FIELD_PLAYER_CORE
 * @retval nothing
 */
//--------------------------------------------------------------
static int req_SetMoveFormToDrawForm0( FIELD_PLAYER_CORE *player_core )
{
  PLAYER_MOVE_FORM form = FIELD_PLAYER_CORE_GetMoveForm( player_core );
  int sex = FIELD_PLAYER_CORE_GetSex( player_core );
  u16 code = FIELD_PLAYER_GetMoveFormToOBJCode( sex, form );
  
  if( reqCommon_SetOBJCode(player_core,code) != TRUE ){
    req_NextSeqNo( player_core );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@����`�Ԃɂ��킹���\���ɂ��� 1
 * @param player_core FIELD_PLAYER_CORE
 * @retval nothing
 */
//--------------------------------------------------------------
static int req_SetMoveFormToDrawForm1( FIELD_PLAYER_CORE *player_core )
{
  if( fldplayer_CheckBBDAct(player_core) == TRUE ){
    player_core->chg_draw_form_middle_wait++;
    
    if( player_core->chg_draw_form_middle_wait >= REQ_CHG_DRAW_MIDDLE_WAIT ){
      PLAYER_MOVE_FORM form = FIELD_PLAYER_CORE_GetMoveForm( player_core );
      int sex = FIELD_PLAYER_CORE_GetSex( player_core );
      u16 code = FIELD_PLAYER_GetMoveFormToOBJCode( sex, form );
      fldplayer_ChangeOBJCode( player_core, code );
      req_SetEndFlag( player_core );
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
// ���@���N�G�X�g�����@����`�Ԃɂ��킹���\���ɂ���@�܂Ƃ�
//--------------------------------------------------------------
static const REQPROC data_req_SetMoveFormToDrawFormTbl[] =
{
  req_SetMoveFormToDrawForm0,
  req_SetMoveFormToDrawForm1,
};

//======================================================================
//  ���@���N�G�X�g  �A�C�e���Q�b�g���@�ɕύX
//======================================================================
//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@�A�C�e���Q�b�g���@�ɕύX 0
 * @param player_core FIELD_PLYER
 * @retval int TRUE=�ċA�Ăяo��
 */
//--------------------------------------------------------------
static int req_SetItemGetHero0( FIELD_PLAYER_CORE *player_core )
{
  if( reqCommon_SetDrawForm(player_core,PLAYER_DRAW_FORM_ITEMGET) != TRUE ){
    req_NextSeqNo( player_core );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@�A�C�e���Q�b�g���@�ɕύX 1
 * @param player_core FIELD_PLAYER_CORE
 * @retval int TRUE=�ċA�Ăяo��
 */
//--------------------------------------------------------------
static int req_SetItemGetHero1( FIELD_PLAYER_CORE *player_core )
{
  reqCommon_WaitSetDrawForm( player_core, PLAYER_DRAW_FORM_ITEMGET );
  return( FALSE );
}

//--------------------------------------------------------------
//  ���@���N�G�X�g�@�A�C�e���Q�b�g���@�ɕύX�@�܂Ƃ�
//--------------------------------------------------------------
static const REQPROC data_req_SetItemGetHeroTbl[] =
{
  req_SetItemGetHero0,
  req_SetItemGetHero1,
};

//======================================================================
//  ���@���N�G�X�g  ���|�[�g���@�ɕύX
//======================================================================
//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@���|�[�g���@�ɕύX 0
 * @param player_core FIELD_PLYER
 * @retval int TRUE=�ċA�Ăяo��
 */
//--------------------------------------------------------------
static int req_SetReportHero0( FIELD_PLAYER_CORE *player_core )
{
  if( reqCommon_SetDrawForm(player_core,PLAYER_DRAW_FORM_SAVEHERO) != TRUE ){
    req_NextSeqNo( player_core );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@���|�[�g���@�ɕύX 1
 * @param player_core FIELD_PLAYER_CORE
 * @retval int TRUE=�ċA�Ăяo��
 */
//--------------------------------------------------------------
static int req_SetReportHero1( FIELD_PLAYER_CORE *player_core )
{
  reqCommon_WaitSetDrawForm( player_core, PLAYER_DRAW_FORM_SAVEHERO );
  return( FALSE );
}

//--------------------------------------------------------------
//  ���@���N�G�X�g�@���|�[�g���@�ɕύX�@�܂Ƃ�
//--------------------------------------------------------------
static const REQPROC data_req_SetReportHeroTbl[] =
{
  req_SetReportHero0,
  req_SetReportHero1,
};

//======================================================================
//  ���@���N�G�X�g  PC�a�����@�ɕύX
//======================================================================
//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@PC�a�����@�ɕύX 0
 * @param player_core FIELD_PLYER
 * @retval int TRUE=�ċA�Ăяo��
 */
//--------------------------------------------------------------
static int req_SetPcAzukeHero0( FIELD_PLAYER_CORE *player_core )
{
  if( reqCommon_SetDrawForm(player_core,PLAYER_DRAW_FORM_PCHERO) != TRUE ){
    req_NextSeqNo( player_core );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@PC�a�����@�ɕύX 1
 * @param player_core FIELD_PLAYER_CORE
 * @retval int TRUE=�ċA�Ăяo��
 */
//--------------------------------------------------------------
static int req_SetPcAzukeHero1( FIELD_PLAYER_CORE *player_core )
{
  reqCommon_WaitSetDrawForm( player_core, PLAYER_DRAW_FORM_PCHERO );
  return( FALSE );
}

//--------------------------------------------------------------
//  ���@���N�G�X�g�@PC�a�����@�ɕύX�@�܂Ƃ�
//--------------------------------------------------------------
static const REQPROC data_req_SetPcAzukeHeroTbl[] =
{
  req_SetPcAzukeHero0,
  req_SetPcAzukeHero1,
};

//======================================================================
//  ���@���N�G�X�g  �J�b�g�C�����@�ɕύX
//======================================================================
//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@�J�b�g�C�����@�ɕύX 0
 * @param player_core FIELD_PLYER
 * @retval int TRUE=�ċA�Ăяo��
 */
//--------------------------------------------------------------
static int req_SetCutinHero0( FIELD_PLAYER_CORE *player_core )
{
  if( reqCommon_SetDrawForm(player_core,PLAYER_DRAW_FORM_CUTIN) != TRUE ){
    req_NextSeqNo( player_core );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@�J�b�g�C�����@�ɕύX 1
 * @param player_core FIELD_PLAYER_CORE
 * @retval int TRUE=�ċA�Ăяo��
 */
//--------------------------------------------------------------
static int req_SetCutinHero1( FIELD_PLAYER_CORE *player_core )
{
  reqCommon_WaitSetDrawForm( player_core, PLAYER_DRAW_FORM_CUTIN);
  return( FALSE );
}

//--------------------------------------------------------------
//  ���@���N�G�X�g�@�J�b�g�C�����@�ɕύX�@�܂Ƃ�
//--------------------------------------------------------------
static const REQPROC data_req_SetCutinHeroTbl[] =
{
  req_SetCutinHero0,
  req_SetCutinHero1,
};

//======================================================================
//  ���@���N�G�X�g  �_�C�r���O���@�ɕύX
//======================================================================
//--------------------------------------------------------------
/**
 *	@brief  DIVING��Ԃ�ݒ�
 */
//--------------------------------------------------------------
static int req_SetDiving0( FIELD_PLAYER_CORE *player_core )
{
  FLDEFF_CTRL *fectrl = FIELDMAP_GetFldEffCtrl( player_core->fieldWork );
  
  // �܂��g����Ԃɂ���
  {
    int sex;
    u16 code;
    MMDL *mmdl;
    
    sex = FIELD_PLAYER_CORE_GetSex( player_core );
    mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
    code = FIELD_PLAYER_GetDrawFormToOBJCode( sex, PLAYER_DRAW_FORM_DIVING );
    
    if( MMDL_GetOBJCode(mmdl) != code ){
      fldplayer_ChangeOBJCode( player_core, code );
    }
    
    if( FIELD_PLAYER_CORE_GetMoveForm(player_core) != PLAYER_MOVE_FORM_DIVING ){
      FIELD_PLAYER_CORE_SetMoveForm( player_core, PLAYER_MOVE_FORM_DIVING );
    }
      
    // �_�C�r���O�́A��ɉ�ʑJ�ڂƂƂ��ɔ��������B
    // ����ɁABGM�͒ʏ�Ȃł悢�̂ŁA
    // BGM����͍s��Ȃ��B
    //gjiki_PlayBGM( player_core );
    
    //�g���|�P����
    if( FIELD_PLAYER_CORE_GetEffectTaskWork(player_core) == NULL ){
      u16 dir;
      VecFx32 pos;
      FLDEFF_CTRL *fectrl;
      FLDEFF_TASK* task;
      
      fectrl = FIELDMAP_GetFldEffCtrl( player_core->fieldWork );
      dir = MMDL_GetDirDisp( mmdl );
      MMDL_GetVectorPos( mmdl, &pos );
      
      task = FLDEFF_NAMIPOKE_SetMMdl(
          fectrl, dir, &pos, mmdl, NAMIPOKE_JOINT_ON );

      FIELD_PLAYER_CORE_SetEffectTaskWork( player_core, task );
    }
  }
  
  //�C�A��ݒ�
  FLDEFF_BUBBLE_SetMMdl( player_core->fldmmdl, fectrl );

  // ���򖗃G�t�F�N�gOFF
  if( player_core->fldeff_joint ){
    FLDEFF_NAMIPOKE_SetRippleEffect( player_core->fldeff_joint, FALSE );
  }

  //�����Ń_�C�r���O�ɒu�������郊�N�G�X�g�Ȃ̂�
  //�\���ύX�҂��͗v��Ȃ�
  req_SetEndFlag( player_core );
  return( FALSE );
}

//--------------------------------------------------------------
//  ���@���N�G�X�g�@�_�C�r���O�ɕύX�@�܂Ƃ�
//--------------------------------------------------------------
static const REQPROC data_req_SetDivingTbl[] =
{
  req_SetDiving0,
};

//======================================================================
//  ���@���N�G�X�g  �ނ莩�@�ɕύX
//======================================================================
//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@�ނ莩�@�ɕύX 0
 * @param player_core FIELD_PLYER
 * @retval int TRUE=�ċA�Ăяo��
 */
//--------------------------------------------------------------
static int req_SetFishing0( FIELD_PLAYER_CORE *player_core )
{
  if( reqCommon_SetDrawForm(player_core,PLAYER_DRAW_FORM_FISHING) != TRUE ){
    req_NextSeqNo( player_core );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@�ނ莩�@�ɕύX 1
 * @param player_core FIELD_PLAYER_CORE
 * @retval int TRUE=�ċA�Ăяo��
 */
//--------------------------------------------------------------
static int req_SetFishing1( FIELD_PLAYER_CORE *player_core )
{
  reqCommon_WaitSetDrawForm( player_core, PLAYER_DRAW_FORM_FISHING);
  
  /*
   * add 100617 BUGFIX BTS �Г��o�ONo.1568
   * �g��蒆�ŉ���OBJ������Ȃ�΃I�t�Z�b�g��ς���
  */
  if( player_core->req_seq_end_flag == TRUE ){
    if( FIELD_PLAYER_CORE_GetMoveForm(player_core) == PLAYER_MOVE_FORM_SWIM ){
      int ret = FALSE;
      MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
      MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
      s16 gx = MMDL_GetGridPosX( mmdl );
      s16 gz = MMDL_GetGridPosZ( mmdl );
      MMDL_TOOL_AddDirGrid( DIR_DOWN, &gx, &gz, 1 );
      
      if( MMDLSYS_SearchGridPos(mmdlsys,gx,gz,TRUE) != NULL ){
        ret = TRUE; //�]���̕\����
      }

      MMDL_DrawFishingHero_SetOffsetType( mmdl, ret );
    }  
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
//  ���@���N�G�X�g�@�ނ莩�@�ɕύX�@�܂Ƃ�
//--------------------------------------------------------------
static const REQPROC data_req_SetFishingTbl[] =
{
  req_SetFishing0,
  req_SetFishing1,
};

//======================================================================
//  ���@���N�G�X�g�@�܂Ƃ�
//======================================================================
//--------------------------------------------------------------
/// ���N�G�X�g�e�[�u��
//--------------------------------------------------------------
static const REQPROC * const data_RequestProcTbl[FIELD_PLAYER_REQBIT_MAX] =
{
  data_req_SetNormalTbl, //FIELD_PLAYER_REQBIT_NORMAL 
  data_req_SetCycleTbl, //FIELD_PLAYER_REQBIT_CYCLE
  data_req_SetNaminoriTbl, //FIELD_PLAYER_REQBIT_SWIM
  data_req_SetMoveFormToDrawFormTbl, //FIELD_PLAYER_REQBIT_MOVE_FORM_TO_DRAW_FORM
  data_req_SetItemGetHeroTbl, //FIELD_PLAYER_REQBIT_ITEMGET
  data_req_SetReportHeroTbl, //FIELD_PLAYER_REQBIT_REPORT
  data_req_SetPcAzukeHeroTbl, //FIELD_PLAYER_REQBIT_PC_AZUKE
  data_req_SetCutinHeroTbl, //FIELD_PLAYER_REQBIT_CUTIN
  data_req_SetDivingTbl, //FIELD_PLAYER_REQBIT_DIVING
  data_req_SetFishingTbl, //FIELD_PLAYER_REQBIT_MAX
};

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�̌`�Ԃɍ��킹��BGM���Đ�
 * @param player_core FIELD_PLAYER_CORE
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_PlayBGM( FIELD_PLAYER_CORE *player_core )
{
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( player_core->fieldWork );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( gdata );
  u32 zone_id = FIELDMAP_GetZoneID( player_core->fieldWork );
  FSND_ChangeBGM_byPlayerFormChange( fsnd, gdata, zone_id );
}
