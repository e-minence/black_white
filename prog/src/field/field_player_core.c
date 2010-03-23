//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_player_core.c
 *	@brief  �t�B�[���h�v���C���[����@�R�A���
 *	@author	gamefreak
 *	@date		2010.02.05
 *
 *	���W���[�����FFIELD_PLAYER_CORE
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

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

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	FIELD_PLAYER_CORE
//=====================================
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
  int draw_form_wait;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
static void fldplayer_ChangeMoveForm(
    FIELD_PLAYER_CORE *player_core, PLAYER_MOVE_FORM form );

//-------------------------------------
///	�L�[���͏���
//=====================================
static u16 gjiki_GetInputKeyDir(
    const FIELD_PLAYER_CORE *player_core, u16 key_prs );
static void gjiki_SetInputKeyDir( FIELD_PLAYER_CORE *player_core, u16 key_prs );
static u16 getKeyDirX( u16 key_prs );
static u16 getKeyDirZ( u16 key_prs );



//-------------------------------------
///	Req�n
//=====================================
static void gjikiReq_SetNormal( FIELD_PLAYER_CORE *player_core );
static void gjikiReq_SetCycle( FIELD_PLAYER_CORE *player_core );
static void gjikiReq_SetSwim( FIELD_PLAYER_CORE *player_core );
static void gjikiReq_SetMoveFormToDrawForm( FIELD_PLAYER_CORE *player_core );
static void gjikiReq_SetItemGetHero( FIELD_PLAYER_CORE *player_core );
static void gjikiReq_SetReportHero( FIELD_PLAYER_CORE *player_core );
static void gjikiReq_SetPCAzukeHero( FIELD_PLAYER_CORE *player_core );
static void gjikiReq_SetCutInHero( FIELD_PLAYER_CORE *player_core );
static void gjikiReq_SetDiving( FIELD_PLAYER_CORE *player_core );
static void gjiki_PlayBGM( FIELD_PLAYER_CORE *player_core );


//----------------------------------------------------------------------------
/**
 * �t�B�[���h�v���C���[�@�쐬
 * @param playerWork �g�p����PLAYER_WOKR
 * @param *fieldWork �Q�Ƃ���FIELDMAP_WORK
 * @param pos �������W
 * @param sex ����
 * @param heapID HEAPID
 * @retval FIELD_PLAYER_CORE*
 */
//-----------------------------------------------------------------------------
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
	
  //OBJ�R�[�h���瓮��t�H�[����ݒ�
  if( fixcode == OBJCODEMAX ){
    u16 code = MMDL_GetOBJCode( player_core->fldmmdl );
    PLAYER_MOVE_FORM form = FIELD_PLAYER_GetOBJCodeToMoveForm( sex, code );
#if 0
    player_core->move_form = form;
#else
    PLAYERWORK_SetMoveForm( player_core->playerWork, form );
#endif
  }
  
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
  MMDL_ChangeOBJCode( player_core->fldmmdl, code );
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
}

//--------------------------------------------------------------
/**
 * ���N�G�X�g���X�V
 * @param player_core FIELD_PLAYER_CORE
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_CORE_UpdateRequest( FIELD_PLAYER_CORE * player_core )
{
  int i = 0;
  FIELD_PLAYER_REQBIT req_bit = player_core->req_bit;
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

//----------------------------------------------------------------------------
/**
 * FIELD_PLAYER PLAYER_MOVE_STATE�Z�b�g
 * @param player_core FIELD_PLAYER_CORE
 * @param val PLAYER_MOVE_STATE
 */
//-----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
/**
 *	@brief  �v���C���[���[�N�̎擾
 *
 *	@param	player_core  ���[�N
 *
 *	@return �v���C���[���[�N
 */
//-----------------------------------------------------------------------------
PLAYER_WORK * FIELD_PLAYER_CORE_GetPlayerWork( const FIELD_PLAYER_CORE * player_core )
{
  return (player_core->playerWork);
}

//----------------------------------------------------------------------------
/**
 * ���샂�f���������Ă��邩�`�F�b�N
 * @param player_core FIELD_PLAYER_CORE
 * @retval BOOL TRUE=����
 */
//-----------------------------------------------------------------------------
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
    MMDL_ChangeOBJCode( mmdl, code );
  }

  player_core->draw_form_wait = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  DrawForm�ύX�҂�
 *
 *	@param	player_core ���@���[�N
 *
 *	@retval TRUE  ����
 *	@retval FALSE �ύX��
 */
//-----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
/**
 *	@brief  ���@�̃A�j���[�V���������҂�
 *
 *	@param	player_core   ���[�N
 *  
 *	@retval TRUE    ����
 *	@retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
BOOL FIELD_PLAYER_CORE_CheckAnimeEnd( const FIELD_PLAYER_CORE *player_core )
{
  MMDLSYS* p_mmdlsys = MMDL_GetMMdlSys( player_core->fldmmdl );
  MMDL_BLACTCONT * p_mmdl_bbdadct = MMDLSYS_GetBlActCont( p_mmdlsys );
  GFL_BBDACT_SYS* p_bbdact = MMDL_BLACTCONT_GetBbdActSys( p_mmdl_bbdadct );
  u32 actID;
  u16 comm;

  
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




//-----------------------------------------------------------------------------
/**
 *      private�֐�
 */
//-----------------------------------------------------------------------------

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
    MMDL_ChangeOBJCode( mmdl, code );
  }
  FIELD_PLAYER_CORE_SetMoveForm( player_core, form );
}





//-------------------------------------
///	�L�[���͏���
//=====================================
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


//-------------------------------------
///	Req�n
//=====================================
//----------------------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@�ʏ�ړ��ɕύX
 * @param player_core FIELD_PLAYER_CORE
 * @retval nothing
 */
//-----------------------------------------------------------------------------
static void gjikiReq_SetNormal( FIELD_PLAYER_CORE *player_core )
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
    MMDL_ChangeOBJCode( mmdl, code );
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
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@���]�Ԉړ��ɕύX
 * @param player_core FIELD_PLAYER_CORE
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiReq_SetCycle( FIELD_PLAYER_CORE *player_core )
{
  int sex;
  u16 code;
  MMDL *mmdl;
  
  sex = FIELD_PLAYER_CORE_GetSex( player_core );
  mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
  code = FIELD_PLAYER_GetDrawFormToOBJCode( sex, PLAYER_DRAW_FORM_CYCLE );
  
  if( MMDL_GetOBJCode(mmdl) != code ){
    MMDL_ChangeOBJCode( mmdl, code );
  }
   
  FIELD_PLAYER_CORE_SetMoveForm( player_core, PLAYER_MOVE_FORM_CYCLE );
  gjiki_PlayBGM( player_core );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@�g���ړ��ɕύX
 * @param player_core FIELD_PLAYER_CORE
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiReq_SetSwim( FIELD_PLAYER_CORE *player_core )
{
  int sex;
  u16 code;
  MMDL *mmdl;
  
  sex = FIELD_PLAYER_CORE_GetSex( player_core );
  mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
  code = FIELD_PLAYER_GetDrawFormToOBJCode( sex, PLAYER_DRAW_FORM_SWIM );

  if( MMDL_GetOBJCode(mmdl) != code ){
    MMDL_ChangeOBJCode( mmdl, code );
  }
  
  if(FIELD_PLAYER_CORE_GetMoveForm( player_core ) != PLAYER_MOVE_FORM_SWIM ){
    FIELD_PLAYER_CORE_SetMoveForm( player_core, PLAYER_MOVE_FORM_SWIM );
  }
  gjiki_PlayBGM( player_core );
  
  if( FIELD_PLAYER_CORE_GetEffectTaskWork( player_core ) == NULL ){ //�g���|�P����
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

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@����`�Ԃɂ��킹���\���ɂ���
 * @param player_core FIELD_PLAYER_CORE
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiReq_SetMoveFormToDrawForm( FIELD_PLAYER_CORE *player_core )
{
  FIELD_PLAYER_CORE_ResetMoveForm( player_core );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@�A�C�e���Q�b�g���@�ɕύX
 * @param player_core FIELD_PLYER
 * @retval  nothing
 */
//--------------------------------------------------------------
static void gjikiReq_SetItemGetHero( FIELD_PLAYER_CORE *player_core )
{
  FIELD_PLAYER_CORE_ChangeDrawForm( player_core, PLAYER_DRAW_FORM_ITEMGET );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@PC�a�����@�ɕύX
 * @param player_core FIELD_PLYER
 * @retval  nothing
 */
//--------------------------------------------------------------
static void gjikiReq_SetReportHero( FIELD_PLAYER_CORE *player_core )
{
  FIELD_PLAYER_CORE_ChangeDrawForm( player_core, PLAYER_DRAW_FORM_SAVEHERO );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@PC�a�����@�ɕύX
 * @param player_core FIELD_PLYER
 * @retval  nothing
 */
//--------------------------------------------------------------
static void gjikiReq_SetPCAzukeHero( FIELD_PLAYER_CORE *player_core )
{
  FIELD_PLAYER_CORE_ChangeDrawForm( player_core, PLAYER_DRAW_FORM_PCHERO );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@�J�b�g�C�����@�ɕύX
 * @param player_core FIELD_PLYER
 * @retval  nothing
 */
//--------------------------------------------------------------
static void gjikiReq_SetCutInHero( FIELD_PLAYER_CORE *player_core )
{
  FIELD_PLAYER_CORE_ChangeDrawForm( player_core, PLAYER_DRAW_FORM_CUTIN );
}

//----------------------------------------------------------------------------
/**
 *	@brief  DIVING��Ԃ�ݒ�
 */
//-----------------------------------------------------------------------------
static void gjikiReq_SetDiving( FIELD_PLAYER_CORE *player_core )
{
  FLDEFF_CTRL *fectrl;
  fectrl = FIELDMAP_GetFldEffCtrl( player_core->fieldWork );
  
  // �܂��g����Ԃɂ���
  {
    int sex;
    u16 code;
    MMDL *mmdl;
    
    sex = FIELD_PLAYER_CORE_GetSex( player_core );
    mmdl = FIELD_PLAYER_CORE_GetMMdl( player_core );
    code = FIELD_PLAYER_GetDrawFormToOBJCode( sex, PLAYER_DRAW_FORM_DIVING );

    if( MMDL_GetOBJCode(mmdl) != code ){
      MMDL_ChangeOBJCode( mmdl, code );
    }
    
    if(FIELD_PLAYER_CORE_GetMoveForm( player_core ) != PLAYER_MOVE_FORM_DIVING ){
      FIELD_PLAYER_CORE_SetMoveForm( player_core, PLAYER_MOVE_FORM_DIVING );
    }
    // �_�C�r���O�́A��ɉ�ʑJ�ڂƂƂ��ɔ��������B
    // ����ɁABGM�͒ʏ�Ȃł悢�̂ŁA
    // BGM����͍s��Ȃ��B
    //gjiki_PlayBGM( player_core );
    
    if( FIELD_PLAYER_CORE_GetEffectTaskWork( player_core ) == NULL ){ //�g���|�P����
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
}


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


