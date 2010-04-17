//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fieldmap_ctrl_hybrid.c
 *	@brief  �}�b�v�R���g���[���@�n�C�u���b�h
 *	@author	tomoya takahashi
 *	@date		2009.10.27
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "fieldmap_ctrl_hybrid.h"

#include "field_player.h"

#include "field/zonedata.h"
#include "field/field_const.h"


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	���[���`�F�b�N�̍����͈�
//=====================================
#define FIELDMAP_CTRL_HYBRID_RAILPLANE_CHECK_Y  ( FX32_CONST(4) )

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	FIELDMAP_CTRL_HYBRID_WORK
//=====================================
struct _FIELDMAP_CTRL_HYBRID
{
  FLDMAP_BASESYS_TYPE base_type;

  FIELD_PLAYER*         p_player;
};


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


static void mapCtrlHybrid_Create(
	FIELDMAP_WORK* p_fieldmap, VecFx32* p_pos, u16 dir );
static void mapCtrlHybrid_Delete( FIELDMAP_WORK* p_fieldmap );
static void mapCtrlHybrid_Main( FIELDMAP_WORK* p_fieldmap, VecFx32* p_pos );
static const VecFx32 * mapCtrlHybrid_GetCameraTarget( FIELDMAP_WORK* p_fieldmap );

static void mapCtrlHybrid_Main_Grid( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk );
static void mapCtrlHybrid_Main_Rail( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk );

static void mapCtrlHybrid_ChangeGridToRail( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk, u16 dir, const RAIL_LOCATION* cp_location );
static void mapCtrlHybrid_ChangeRailToGrid( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk, u16 dir, const VecFx32* cp_pos );

static BOOL mapCtrlHybrid_CalcChangePos( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk, VecFx32* p_pos, u16 dir );
static BOOL mapCtrlHybrid_CalcChangeRailLocation( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk, RAIL_LOCATION* p_location, u16 dir );


// �x�[�X�V�X�e���̕ύX
static void mapCtrlHybrid_ChangeBaseSystem( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk, FLDMAP_BASESYS_TYPE type, const void* cp_pos, u16 dir );


//======================================================================
//	�t�B�[���h�}�b�v�@�n�C�u���b�h����
//======================================================================
//--------------------------------------------------------------
///	�}�b�v�R���g���[���@�n�C�u���b�h�ړ�
//--------------------------------------------------------------
const DEPEND_FUNCTIONS FieldMapCtrl_HybridFunctions =
{
	FLDMAP_CTRLTYPE_HYBRID,
	mapCtrlHybrid_Create,
	mapCtrlHybrid_Main,
	mapCtrlHybrid_Delete,
  mapCtrlHybrid_GetCameraTarget,
};


//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h�x�[�X�V�X�e���^�C�v�̎擾
 *
 *	@param	cp_wk   ���[�N
 *  
 *	@return�@�x�[�X�V�X�e���^�C�v
 */
//----------------------------------------------------------------------------- 
FLDMAP_BASESYS_TYPE FIELDMAP_CTRL_HYBRID_GetBaseSystemType( const FIELDMAP_CTRL_HYBRID* cp_wk )
{
  GF_ASSERT( cp_wk );
  return  cp_wk->base_type;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���̏�ԂŃx�[�X�V�X�e����ύX
 *
 *	@param	p_wk          ���[�N
 *	@param	p_fieldmap    �t�B�[���h���[�N
 */
//-----------------------------------------------------------------------------
void FIELDMAP_CTRL_HYBRID_ChangeBaseSystem( FIELDMAP_CTRL_HYBRID* p_wk, FIELDMAP_WORK* p_fieldmap )
{
  MMDL * mmdl;
  u16 dir;
  
  mmdl = FIELD_PLAYER_GetMMdl( p_wk->p_player );
  dir = MMDL_GetDirDisp( mmdl );
  
  if( p_wk->base_type == FLDMAP_BASESYS_GRID )
  {
    BOOL result;
    RAIL_LOCATION location;
    result = mapCtrlHybrid_CalcChangeRailLocation( p_fieldmap, p_wk, &location, dir );
    
    if( result )
    {
      PLAYER_MOVEBIT mbit = PLAYER_MOVEBIT_NON;
      // ����`�F���W
      FIELD_PLAYER_MoveGrid( p_wk->p_player, 0, 0, mbit );
      mapCtrlHybrid_ChangeGridToRail( p_fieldmap, p_wk, dir, &location );
    }
  }
  else
  {
    VecFx32 pos;
    BOOL result;

    result = mapCtrlHybrid_CalcChangePos( p_fieldmap, p_wk, &pos, dir );
    
    if( result )
    {
      // ����`�F���W
      FIELD_PLAYER_MoveNoGrid( p_wk->p_player, 0, 0 );
      mapCtrlHybrid_ChangeRailToGrid( p_fieldmap, p_wk, dir, &pos );
    }
  }
}



#ifdef PM_DEBUG
void FIELDMAP_CTRL_HYBRID_DEBUG_SetBaseSystem( FIELDMAP_CTRL_HYBRID* p_wk, FLDMAP_BASESYS_TYPE type )
{
  p_wk->base_type = type;
}
#endif

//-----------------------------------------------------------------------------
/**
 *        private�֐��S
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  Hybrid�}�b�v�R���g���[��  ����
 */
//-----------------------------------------------------------------------------
static void mapCtrlHybrid_Create( FIELDMAP_WORK* p_fieldmap, VecFx32* p_pos, u16 dir )
{
  HEAPID heapID;
  FIELDMAP_CTRL_HYBRID* p_wk;
  GAMESYS_WORK* p_gsys = FIELDMAP_GetGameSysWork( p_fieldmap );
  GAMEDATA* p_gdata = GAMESYSTEM_GetGameData( p_gsys );
  const PLAYER_WORK* cp_playerwk = GAMEDATA_GetMyPlayerWork( p_gdata );
  u32 base_type;

  heapID  = FIELDMAP_GetHeapID( p_fieldmap );
  
  // ���[�N�쐬���t�B�[���h�}�b�v�ɐݒ�
  p_wk    = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELDMAP_CTRL_HYBRID) ); 
	FIELDMAP_SetMapCtrlWork( p_fieldmap, p_wk );

  // ���[�N������
  {
    p_wk->p_player = FIELDMAP_GetFieldPlayer( p_fieldmap );
    
    // ���[���v���C���[����
    FIELD_PLAYER_SetUpNoGrid( p_wk->p_player, heapID );
    
    // �O���b�h�v���C���[����
    FIELD_PLAYER_SetUpGrid( p_wk->p_player, heapID );

    // ����������ǂ���ɂ��邩PLAYER_WORK����擾
    base_type = PLAYERWORK_getPosType( cp_playerwk );
    p_wk->base_type = FLDMAP_BASESYS_MAX;

    // ���f���̌�����DIR�ɂ���
    FIELD_PLAYER_SetDir( p_wk->p_player, dir );


    // base_type�Ɉړ��V�X�e����J��
    if( base_type == FLDMAP_BASESYS_GRID )
    {
      mapCtrlHybrid_ChangeBaseSystem( p_fieldmap, p_wk, base_type, p_pos, dir );
    }
    else
    {
      const RAIL_LOCATION* cp_location = PLAYERWORK_getRailPosition( cp_playerwk );
      mapCtrlHybrid_ChangeBaseSystem( p_fieldmap, p_wk, base_type, cp_location, dir );
    }
  }

  //�J�������W�Z�b�g
  {
    MMDL* p_mmdl = FIELD_PLAYER_GetMMdl( p_wk->p_player );
    FIELDMAP_SetNowPosTarget( p_fieldmap, MMDL_GetVectorPosAddress( p_mmdl ) );
  }

  FIELD_PLAYER_GetPos( p_wk->p_player, p_pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief�@Hybrid�}�b�v�R���g���[���@�j��
 */
//-----------------------------------------------------------------------------
static void mapCtrlHybrid_Delete( FIELDMAP_WORK* p_fieldmap )
{
	FIELDMAP_CTRL_HYBRID* p_wk;

	p_wk = FIELDMAP_GetMapCtrlWork( p_fieldmap );

	GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief�@Hybrid�}�b�v�R���g���[���@���C��
 */
//-----------------------------------------------------------------------------
static void mapCtrlHybrid_Main( FIELDMAP_WORK* p_fieldmap, VecFx32* p_pos )
{
	FIELDMAP_CTRL_HYBRID* p_wk = FIELDMAP_GetMapCtrlWork( p_fieldmap );
  static void (*pMain[FLDMAP_BASESYS_MAX])( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk ) = 
  {
    mapCtrlHybrid_Main_Grid,
    mapCtrlHybrid_Main_Rail,
  };

  GF_ASSERT( p_wk->base_type < FLDMAP_BASESYS_MAX );
  GF_ASSERT( pMain[ p_wk->base_type ] );

  pMain[ p_wk->base_type ]( p_fieldmap, p_wk );

  FIELD_PLAYER_GetPos( p_wk->p_player, p_pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  Hybrid�}�b�v�R���g���[���@�J�����^�[�Q�b�g�擾
 *
 *	@return �J�����^�[�Q�b�g
 */
//-----------------------------------------------------------------------------
static const VecFx32 * mapCtrlHybrid_GetCameraTarget( FIELDMAP_WORK* p_fieldmap )
{
  FIELD_PLAYER* p_fldplayer = FIELDMAP_GetFieldPlayer( p_fieldmap );
  MMDL* p_mmdl = FIELD_PLAYER_GetMMdl( p_fldplayer );
  const VecFx32 * p_pos = MMDL_GetVectorPosAddress( p_mmdl ); 
  return( p_pos );
}








//-----------------------------------------------------------------------------
/**
 *    Private�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �O���b�h�}�b�v
 */
//-----------------------------------------------------------------------------
static void mapCtrlHybrid_Main_Grid( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk )
{
  int key_trg = GFL_UI_KEY_GetTrg();
  int key_cont = GFL_UI_KEY_GetCont();
  PLAYER_MOVEBIT mbit = PLAYER_MOVEBIT_NON;
  MAPATTR attr;
  MAPATTR front_attr;
  MAPATTR_VALUE value;
  MMDL * mmdl;
  u16 dir;
  u16 set_dir;
  
  mmdl = FIELD_PLAYER_GetMMdl( p_wk->p_player );

  dir = FIELD_PLAYER_GetKeyDir( p_wk->p_player, key_cont );

  
  // �ړ��������Ă��邩�H
  // 1�O�����������A�����瓮�����Ƃ��āA��芷���̏�ɂ������芷��
  if( ((MMDL_CheckPossibleAcmd(mmdl) == TRUE) || (FIELD_PLAYER_IsHitch( p_wk->p_player ) == TRUE)) && 
      (dir != DIR_NOT) )
  {

    // ������HYBRID�A1�O���ړ��s�\�H

    // �A�g���r���[�g�̎擾
    attr = FIELD_PLAYER_GetMapAttr( p_wk->p_player );
    front_attr = FIELD_PLAYER_GetDirMapAttr( p_wk->p_player, dir );
    value = MAPATTR_GetAttrValue( attr );

    if( RAIL_ATTR_VALUE_CheckHybridBaseSystemChange( value ) )
    {
      if( MAPATTR_GetHitchFlag( front_attr ) )
      {
        BOOL result;
        RAIL_LOCATION location;
        result = mapCtrlHybrid_CalcChangeRailLocation( p_fieldmap, p_wk, &location, dir );
        
        if( result )
        {
          PLAYER_MOVEBIT dmy = PLAYER_MOVEBIT_NON;
          // ����`�F���W
          FIELD_PLAYER_MoveGrid( p_wk->p_player, 0, 0, dmy );

          // �`����������߂�
          set_dir = dir;
          
          mapCtrlHybrid_ChangeGridToRail( p_fieldmap, p_wk, set_dir, &location );
          return ;
        }
      }
    }
  }
  
  if( FIELD_PLAYER_CheckPossibleDash(p_wk->p_player) == TRUE ){
    mbit |= PLAYER_MOVEBIT_DASH;
  }
  
	FIELD_PLAYER_MoveGrid( p_wk->p_player, key_trg, key_cont, mbit );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[���}�b�v
 */
//-----------------------------------------------------------------------------
static void mapCtrlHybrid_Main_Rail( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk )
{
  int key_trg = GFL_UI_KEY_GetTrg();
  int key_cont = GFL_UI_KEY_GetCont();
  MAPATTR attr;
  MAPATTR front_attr;
  MAPATTR_VALUE value;
  MMDL * mmdl;
  u16 dir;
  u16 set_dir;
  
  mmdl = FIELD_PLAYER_GetMMdl( p_wk->p_player );

  dir = FIELD_PLAYER_GetKeyDir( p_wk->p_player, key_cont );



  // �ړ��������Ă��邩�H
  // 1�O�����������A�����瓮�����Ƃ��āA��芷���̏�ɂ������芷��
  if( ((MMDL_CheckPossibleAcmd(mmdl) == TRUE) || (FIELD_PLAYER_IsHitch( p_wk->p_player ) == TRUE)) && 
      (dir != DIR_NOT) )
  {
    
    // ������HYBRID�A1�O���ړ��s�\�H
    // �A�g���r���[�g�̎擾
    attr = FIELD_PLAYER_GetMapAttr( p_wk->p_player );
    front_attr = FIELD_PLAYER_GetDirMapAttr( p_wk->p_player, dir );
    value = MAPATTR_GetAttrValue( attr );

    if( RAIL_ATTR_VALUE_CheckHybridBaseSystemChange( value ) )
    {
      if( MAPATTR_GetHitchFlag( front_attr ) )
      {
        VecFx32 pos;
        BOOL result;

        result = mapCtrlHybrid_CalcChangePos( p_fieldmap, p_wk, &pos, dir );

        if( result )
        {
          // ����`�F���W
          FIELD_PLAYER_MoveNoGrid( p_wk->p_player, 0, 0 );

          set_dir = dir;
          
          mapCtrlHybrid_ChangeRailToGrid( p_fieldmap, p_wk, set_dir, &pos );
          return ;
        }
      }
    }
  }

  FIELD_PLAYER_MoveNoGrid( p_wk->p_player, key_trg, key_cont );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �O���b�h�}�b�v���烌�[���}�b�v�Ɉړ�
 *
 *	@param	p_fieldmap
 *	@param	p_wk 
 *	@param  dir 
 */
//-----------------------------------------------------------------------------
static void mapCtrlHybrid_ChangeGridToRail( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk, u16 dir, const RAIL_LOCATION* cp_location )
{
  // ���[���ɕύX
  mapCtrlHybrid_ChangeBaseSystem( p_fieldmap, p_wk, FLDMAP_BASESYS_RAIL, cp_location, dir );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���[���}�b�v����O���b�h�}�b�v�Ɉړ�
 *
 *	@param	p_fieldmap
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void mapCtrlHybrid_ChangeRailToGrid( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk, u16 dir, const VecFx32* cp_pos )
{
  // �O���b�h�ɕύX
  mapCtrlHybrid_ChangeBaseSystem( p_fieldmap, p_wk, FLDMAP_BASESYS_GRID, cp_pos, dir );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ��芷����̂RD���W�̎擾
 *
 *	@param	p_fieldmap
 *	@param	p_wk
 *	@param	p_pos 
 *
 *	@retval TRUE  ��芷��OK 
 *	@retval FALSE ��芷��NG
 */
//-----------------------------------------------------------------------------
static BOOL mapCtrlHybrid_CalcChangePos( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk, VecFx32* p_pos, u16 dir )
{
  VecFx32 front_pos;
  MAPATTR attr;
  FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( p_fieldmap );
  
  FIELD_PLAYER_GetPos( p_wk->p_player, p_pos );
  front_pos = *p_pos;

  // 1���O�̃A�g���r���[�g�擾
  MMDL_TOOL_AddDirVector( dir, &front_pos, GRID_FX32 ); 
  attr = MAPATTR_GetAttribute( mapper, &front_pos );

  if( MAPATTR_GetHitchFlag( attr ) == FALSE )
  {
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��芷����̃��P�[�V�����̎擾����
 *
 *	@param	p_fieldmap      
 *	@param	p_wk
 *	@param	p_location 
 *
 *	@retval TRUE  ��芷��OK 
 *	@retval FALSE ��芷��NG
 */
//-----------------------------------------------------------------------------
static BOOL mapCtrlHybrid_CalcChangeRailLocation( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk, RAIL_LOCATION* p_location, u16 dir )
{
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_fieldmap );
  const FIELD_RAIL_MAN* cp_railman = FLDNOGRID_MAPPER_GetRailMan( p_mapper );
  RAIL_LOCATION location;
  RAIL_LOCATION front_location;
  VecFx32 hitpos;
  VecFx32 startpos, endpos;
  BOOL result;

  // �J�n�|�W�V����
  FIELD_PLAYER_GetPos( p_wk->p_player, &startpos );
  VEC_Set( &endpos, startpos.x, 
      startpos.y + FIELDMAP_CTRL_HYBRID_RAILPLANE_CHECK_Y,
      startpos.z );
  startpos.y -= FIELDMAP_CTRL_HYBRID_RAILPLANE_CHECK_Y;

  // ���[����̈ʒu���擾�A�ݒ肵�A����
  result = FIELD_RAIL_MAN_Calc3DVecRailLocation( cp_railman, &startpos, &endpos, &location, &hitpos );

  // 1���O�ɐi�߂邩�`�F�b�N
  if( result )
  {
    result = FIELD_RAIL_MAN_CalcRailKeyLocation( cp_railman, &location, FIELD_RAIL_TOOL_ConvertDirToRailKey( dir ), &front_location );
  }

  *p_location = location;
  
  return result;
}




//----------------------------------------------------------------------------
/**
 *	@brief  �x�[�X�V�X�e���^�C�v�̕ύX����
 *
 *	@param	p_fieldmap  �t�B�[���h�}�b�v
 *	@param	p_wk        �n�C�u���b�h���[�N
 *	@param	type        �x�[�X�V�X�e���^�C�v
 *	@param  cp_pos       GRID:VecFx32*    RAIL:RAIL_LOCATION*
 */
//-----------------------------------------------------------------------------
static void mapCtrlHybrid_ChangeBaseSystem( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk, FLDMAP_BASESYS_TYPE type, const void* cp_pos, u16 dir )
{
  MMDL* p_mmdl;
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_fieldmap );
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_fieldmap );

  if( p_wk->base_type == type )
  {
    return ;
  }

  
  p_mmdl = FIELD_PLAYER_GetMMdl( p_wk->p_player );

  if( MMDL_CheckMoveBitAcmd( p_mmdl ) ){
    // �A�j���[�V�����̒�~
    MMDL_FreeAcmd( p_mmdl );
  }
  
  if( type == FLDMAP_BASESYS_GRID )
  {
    static MMDL_HEADER data_MMdlHeader =
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
    VecFx32 pos = *((VecFx32*)cp_pos);

    data_MMdlHeader.obj_code = MMDL_GetOBJCode( p_mmdl );
    
    // ����ύX
    FIELD_PLAYER_StopNoGrid( p_wk->p_player );
    MMDL_ChangeMoveParam( p_mmdl, &data_MMdlHeader );

    //  ���W��ݒ�
    {
      MMDL_InitGridPosition( p_mmdl, FX32_TO_GRID( pos.x ), FX32_TO_GRID( pos.y ), FX32_TO_GRID( pos.z ), dir );
      
      MMDL_GetVectorPos( p_mmdl, &pos );
    }
    FIELD_PLAYER_SetPos( p_wk->p_player, &pos );

    // �J�����ݒ�
    FLDNOGRID_MAPPER_UnBindCameraWork( p_mapper );
    FIELD_CAMERA_ChangeMode( p_camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    FIELD_CAMERA_BindDefaultTarget( p_camera );

    // �J������Ԃ��f�t�H���g�l�ɖ߂�
    FIELD_CAMERA_SetDefaultParameter( p_camera );

  }
  else
  {
    FIELD_PLAYER_RestartNoGrid( p_wk->p_player, cp_pos );

    // �J�����ݒ�
    FLDNOGRID_MAPPER_BindCameraWork( p_mapper, FIELD_PLAYER_GetNoGridRailWork( p_wk->p_player ) );

    // �J�����̂��������
    // �J�����g���[�X�̃��Z�b�g
    FIELD_CAMERA_RestartTrace( p_camera );

  }

  // ���������킹��
  MMDL_SetDirDisp( p_mmdl, dir );
  MMDL_SetDirMove( p_mmdl, dir );

  p_wk->base_type = type;
}


