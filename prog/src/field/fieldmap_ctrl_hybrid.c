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
#include "field_player_grid.h"
#include "field_player_nogrid.h"

#include "field/zonedata.h"


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
	FIELD_PLAYER_GRID*    p_player_grid;
	FIELD_PLAYER_NOGRID*  p_player_nogrid;
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


// �x�[�X�V�X�e���̕ύX
static void mapCtrlHybrid_ChangeBaseSystem( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk, FLDMAP_BASESYS_TYPE type, const void* cp_pos );


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
 *	@brief  �t�B�[���h�O���b�h�v���C���[�̎擾
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return �t�B�[���h�O���b�h�v���C���[���[�N
 */
//-----------------------------------------------------------------------------
FIELD_PLAYER_GRID* FIELDMAP_CTRL_HYBRID_GetFieldPlayerGrid( const FIELDMAP_CTRL_HYBRID* cp_wk )
{
  GF_ASSERT( cp_wk );

  return cp_wk->p_player_grid;
}

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
 *	@brief  �t�B�[���h�m�[�O���b�h�v���C���[�̎擾
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return �t�B�[���h�m�[�O���b�h�v���C���[���[�N
 */
//-----------------------------------------------------------------------------
FIELD_PLAYER_NOGRID* FIELDMAP_CTRL_HYBRID_GetFieldPlayerNoGrid( const FIELDMAP_CTRL_HYBRID* cp_wk )
{
  GF_ASSERT( cp_wk );

  return cp_wk->p_player_nogrid;
}






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
		p_wk->p_player_nogrid = FIELD_PLAYER_NOGRID_Create( p_wk->p_player, heapID );
    
    // �O���b�h�v���C���[����
		p_wk->p_player_grid = FIELD_PLAYER_GRID_Init( p_wk->p_player, heapID );

    // ����������ǂ���ɂ��邩PLAYER_WORK����擾
    base_type = PLAYERWORK_getPosType( cp_playerwk );
    p_wk->base_type = FLDMAP_BASESYS_MAX;


    // base_type�Ɉړ��V�X�e����J��
    if( base_type == FLDMAP_BASESYS_GRID )
    {
      mapCtrlHybrid_ChangeBaseSystem( p_fieldmap, p_wk, base_type, p_pos );
    }
    else
    {
      const RAIL_LOCATION* cp_location = PLAYERWORK_getRailPosition( cp_playerwk );
      mapCtrlHybrid_ChangeBaseSystem( p_fieldmap, p_wk, base_type, cp_location );
    }

    FIELD_PLAYER_SetDir( p_wk->p_player, dir );
  }

  //�J�������W�Z�b�g
  {
    MMDL* p_mmdl = FIELD_PLAYER_GetMMdl( p_wk->p_player );
    FIELDMAP_SetNowPosTarget( p_fieldmap, MMDL_GetVectorPosAddress( p_mmdl ) );
  }
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
	FIELD_PLAYER_GRID_Delete( p_wk->p_player_grid );
	FIELD_PLAYER_NOGRID_Delete( p_wk->p_player_nogrid );

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

	FIELD_PLAYER_GRID_Move( p_wk->p_player_grid, key_trg, key_cont );

  // ����ł��Ȃ��Ƃ��A���[������ւ̑J�ڂ����݂�
  if( FIELD_PLAYER_GetMoveValue( p_wk->p_player ) == PLAYER_MOVE_VALUE_STOP )
  {
    FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_fieldmap );
    const FIELD_RAIL_MAN* cp_railman = FLDNOGRID_MAPPER_GetRailMan( p_mapper );
    RAIL_LOCATION location;
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

    if( result == TRUE )
    {
      // ���[���ɕύX
      mapCtrlHybrid_ChangeBaseSystem( p_fieldmap, p_wk, FLDMAP_BASESYS_RAIL, &location );
    }
  }
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

  FIELD_PLAYER_NOGRID_Move( p_wk->p_player_nogrid, key_trg, key_cont );

  // ����ł��Ȃ��Ƃ��A�O���b�h����ւ̑J�ڂ����݂�
  if( FIELD_PLAYER_NOGRID_IsHitch( p_wk->p_player_nogrid ) )
  {
    VecFx32 pos;

    FIELD_PLAYER_GetPos( p_wk->p_player, &pos );
    
    // �ړ��\�ȃO���b�h���H
    
    // �O���b�h�ɕύX
    mapCtrlHybrid_ChangeBaseSystem( p_fieldmap, p_wk, FLDMAP_BASESYS_GRID, &pos );
  }
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
static void mapCtrlHybrid_ChangeBaseSystem( FIELDMAP_WORK* p_fieldmap, FIELDMAP_CTRL_HYBRID* p_wk, FLDMAP_BASESYS_TYPE type, const void* cp_pos )
{
  MMDL* p_mmdl;
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_fieldmap );
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_fieldmap );

  if( p_wk->base_type == type )
  {
    return ;
  }
  
  p_mmdl = FIELD_PLAYER_GetMMdl( p_wk->p_player );
  
  // �A�j���[�V�����̒�~
  MMDL_FreeAcmd( p_mmdl );
  
  if( type == FLDMAP_BASESYS_GRID )
  {
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
    
    // ����ύX
    FIELD_PLAYER_NOGRID_Stop( p_wk->p_player_nogrid );
    MMDL_ChangeMoveParam( p_mmdl, &data_MMdlHeader );

    FIELD_PLAYER_SetPos( p_wk->p_player, cp_pos );

    // �J�����ݒ�
    FLDNOGRID_MAPPER_UnBindCameraWork( p_mapper );
    FIELD_CAMERA_ChangeMode( p_camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    FIELD_CAMERA_BindDefaultTarget( p_camera );
  }
  else
  {
    FIELD_PLAYER_NOGRID_Restart( p_wk->p_player_nogrid, cp_pos );

    // �J�����ݒ�
    FLDNOGRID_MAPPER_BindCameraWork( p_mapper, FIELD_PLAYER_NOGRID_GetRailWork( p_wk->p_player_nogrid ) );
  }

  p_wk->base_type = type;
}

