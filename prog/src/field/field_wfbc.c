//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_wfbc.c
 *	@brief  White Forest  Black City 
 *	@author	tomoya takahashi
 *	@date		2009.11.09
 *
 *	���W���[�����FFIELD_WFBC
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "arc/arc_def.h"
#include "arc/field_wfbc_data.naix"
#include "arc/fieldmap/eventdata.naix"

#include "savedata/mystatus.h"

#include "field/field_const.h"

#include "field_wfbc.h"
#include "land_data_patch.h"

#include "eventdata_local.h"




//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�u���b�N�z�u���
//=====================================
#define FIELD_WFBC_BLOCK_PATCH_MAX  (16)
#define FIELD_WFBC_BLOCK_NONE  (0xff)

static const u32 sc_FIELD_WFBC_BLOCK_FILE[ FIELD_WFBC_CORE_TYPE_MAX ] = 
{
  NARC_field_wfbc_data_wb_wfbc_block_bc_block,
  NARC_field_wfbc_data_wb_wfbc_block_wf_block,
};
#define FIELD_WFBC_BLOCK_BUFF_SIZE  ( 0x600 )


//-------------------------------------
///	�C�x���g���
//=====================================
#define FIELD_WFBC_EVENT_WF  (NARC_eventdata_wfblock_bin)
#define FIELD_WFBC_EVENT_BC  (NARC_eventdata_bcblock_bin)


//-------------------------------------
///	���݂̃}�b�v���
//  �s��l
//=====================================
#define FIELD_WFBC_NOW_MAP_ERR  ( 0xffff )




//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	land_data_patch�̏��
//=====================================
typedef struct 
{
  u32 patch[FIELD_WFBC_BLOCK_PATCH_MAX];
} FIELD_WFBC_BLOCK_PATCH;

//-------------------------------------
///	
//=====================================
typedef struct 
{
  u8 pos_x:4;   // �u���b�N���ʒu
  u8 pos_z:4;   // �u���b�N���ʒu
  u8 block_no;
} WFBC_BLOCK_DATA;


//-------------------------------------
///	�u���b�N�z�u���
//=====================================
typedef struct 
{
  // �e�O���b�h�ʒu�̃u���b�N���
  WFBC_BLOCK_DATA block_data[FIELD_WFBC_BLOCK_SIZE_Z][FIELD_WFBC_BLOCK_SIZE_X];

  // �p�b�`���
  FIELD_WFBC_BLOCK_PATCH patch[]; 
} FIELD_WFBC_BLOCK_DATA;


//-------------------------------------
///	�z�u�P���
//=====================================
typedef struct 
{
  WFBC_BLOCK_DATA block_tag;
  u16 block_id;
} WFBC_BLOCK_NOW;

//-------------------------------------
///	���ۂ̔z�u���
//=====================================
typedef struct 
{
   WFBC_BLOCK_NOW map_now[FIELD_WFBC_BLOCK_SIZE_Z][FIELD_WFBC_BLOCK_SIZE_X];
} FIELD_WFBC_NOW_MAPDATA;




//-------------------------------------
///	�C�x���g�N�����
//=====================================
typedef struct 
{
  // ����
  u16 block_tag;
  u16 block_id;

  // �ڑ��C�x���g�C���f�b�N�X
  u32 connect_idx;
} FIELD_WFBC_EVENT_IF;


//-------------------------------------
///	�J�������
//=====================================
typedef struct 
{
  u16 camera_no[ FIELD_WFBC_PEOPLE_MAX+1 ];
} FIELD_WFBC_CAMERA_SETUP;


//-------------------------------------
///	�l�����
//=====================================
struct _FIELD_WFBC_PEOPLE 
{
  u16   status;   // FIELD_WFBC_PEOPLE_STATUS
  FIELD_WFBC_PEOPLE_DATA people_data;
  FIELD_WFBC_CORE_PEOPLE people_local;

  u16   gx;   // �O���b�h�����W
  u16   gz;   // �O���b�h�����W
};

//-------------------------------------
///	�����ڍ\�z�p�̏��S
//  
//  �����ڂ͏�ɕ\�̏����g�p���܂��B
//
//  ����l�́A
//  �@�\�̂Ƃ��������
//    ���̂Ƃ�������܂��B
//=====================================
typedef struct {
  // �����f�[�^
  // �z�u�u���b�N�̌v�Z�Ɏg�p
  // ���̑��̗p�r�ł͎g�p���Ȃ����ƁI�I�I
  GFL_STD_RandContext randData;
  u16 block_rand_max;
  u16 block_pattern_num;

  u32 draw_people_num;
  u32 random_no;
} WFBC_DRAW_PARAM;



//-------------------------------------
///	WFBC���[�N
//=====================================
struct _FIELD_WFBC 
{
  u8    type;     // WF / BC FIELD_WFBC_CORE_TYPE
  u8    mapmode;  // �}�b�v���[�hMAPMODE
  FIELD_WFBC_PEOPLE* p_people;

  // mapmode��MAPMODE_NORMAL�Ȃ�A�����ɕύX���Z�[�u�f�[�^�ɐݒ肳���B
  FIELD_WFBC_CORE* p_core;  

  // ���[�_�[
  FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader;

  // �A�[�J�C�u�n���h��
  ARCHANDLE* p_handle;

  // �u���b�N�z�u���
  FIELD_WFBC_BLOCK_DATA* p_block;

  // ���̃}�b�v���
  // �������ꂽ�u���b�N�̏��
  FIELD_WFBC_NOW_MAPDATA map_data;

  // �l���ʒu���
  FIELD_WFBC_CORE_PEOPLE_POS* p_pos;

  // �`�搶���p�����[�^
  WFBC_DRAW_PARAM draw_param;

  // ���샂�f���V�X�e��
  MMDLSYS* p_mmdlsys;
};


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	FIELD_WFBC
//=====================================
static void WFBC_InitSystem( FIELD_WFBC* p_wk, HEAPID heapID );
static void WFBC_ExitSystem( FIELD_WFBC* p_wk );

static void WFBC_Clear( FIELD_WFBC* p_wk );
static int WFBC_GetClearPeople( const FIELD_WFBC* cp_wk );
static int WFBC_GetPeopleNum( const FIELD_WFBC* cp_wk );

static void WFBC_SetCore( FIELD_WFBC* p_wk, FIELD_WFBC_CORE* p_buff, MAPMODE mapmode );
static void WFBC_GetCore( const FIELD_WFBC* cp_wk, FIELD_WFBC_CORE* p_buff );

static void WFBC_UpdateCore( FIELD_WFBC* p_wk );

static FIELD_WFBC_PEOPLE* WFBC_GetPeople( FIELD_WFBC* p_wk, u32 npc_id );
static const FIELD_WFBC_PEOPLE* WFBC_GetConstPeople( const FIELD_WFBC* cp_wk, u32 npc_id );

//-------------------------------------
///	FIELD_WFBC_PEOPLE
//=====================================
static void WFBC_PEOPLE_Clear( FIELD_WFBC_PEOPLE* p_people );
static void WFBC_PEOPLE_SetUp( FIELD_WFBC_PEOPLE* p_people, const FIELD_WFBC_CORE_PEOPLE* cp_core, const FIELD_WFBC_CORE_PEOPLE_POS* cp_pos, const FIELD_WFBC_PEOPLE_DATA* cp_data );
static void WFBC_PEOPLE_GetCore( const FIELD_WFBC_PEOPLE* cp_people, FIELD_WFBC_CORE_PEOPLE* p_core );


//-------------------------------------
///	FIELD_WFBC_NOW_MAPDATA
//=====================================
static void WFBC_NOW_MAPDATA_Init( FIELD_WFBC_NOW_MAPDATA* p_wk );
static void WFBC_NOW_MAPDATA_SetData( FIELD_WFBC_NOW_MAPDATA* p_wk, u32 block_x, u32 block_z, WFBC_BLOCK_DATA block_tag, u16 block_id );
static WFBC_BLOCK_NOW WFBC_NOW_MAPDATA_GetData( const FIELD_WFBC_NOW_MAPDATA* cp_wk, u32 block_x, u32 block_z );
static BOOL WFBC_NOW_MAPDATA_IsDataIn( const FIELD_WFBC_NOW_MAPDATA* cp_wk, u16 block_tag, u16 block_id );


//-------------------------------------
///	WFBC_DRAW_PARAM
//=====================================
static void WFBC_DRAW_PARAM_Init( WFBC_DRAW_PARAM* p_wk );
static void WFBC_DRAW_PARAM_SetUp( WFBC_DRAW_PARAM* p_wk, const FIELD_WFBC_CORE* cp_data, FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader );
static void WFBC_DRAW_PARAM_MakeMapData( WFBC_DRAW_PARAM* p_wk, const FIELD_WFBC_BLOCK_DATA* cp_block, FIELD_WFBC_NOW_MAPDATA* p_mapdata );



//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�p���[�N�̐���
 *
 *	@param	heapID  �q�[�vID
 *
 *	@return WFBC���[�N
 */
//-----------------------------------------------------------------------------
FIELD_WFBC* FIELD_WFBC_Create( HEAPID heapID )
{
  FIELD_WFBC* p_wk;

  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_WFBC) );

  WFBC_Clear( p_wk );

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�p���[�N�̔j��
 *
 *	@param	p_wk  ���[�N
 */ 
//-----------------------------------------------------------------------------
void FIELD_WFBC_Delete( FIELD_WFBC* p_wk )
{
  GF_ASSERT( p_wk );

  WFBC_ExitSystem( p_wk );

  // ���샂�f���̉e�𕜋A
  if( p_wk->p_mmdlsys ){
    MMDLSYS_SetJoinShadow( p_wk->p_mmdlsys, TRUE );
  }

  GFL_HEAP_FreeMemory( p_wk );
}


//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�p�����e�[�u���C���f�b�N�X��Ԃ�
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return �����e�[�u���̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_GetPeopleNum( const FIELD_WFBC* cp_wk )
{
  GF_ASSERT( cp_wk );

  // �l���𐔂���
  return WFBC_GetPeopleNum( cp_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBCCORE�f�[�^���擾����
 *
 *	@param	cp_wk   ���[�N
 *	@param	p_buff  �i�[�o�b�t�@
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_GetCore( const FIELD_WFBC* cp_wk, FIELD_WFBC_CORE* p_buff )
{
  WFBC_GetCore( cp_wk, p_buff );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l�����̎擾
 *
 *	@param	cp_wk   ���[�N
 *	@param	index   �C���f�b�N�X
 *
 *	@retval �l�����
 *	@retval NULL  ���Ȃ� 
 */
//-----------------------------------------------------------------------------
const FIELD_WFBC_PEOPLE* FIELD_WFBC_GetPeople( const FIELD_WFBC* cp_wk, u32 npc_id )
{
  if( npc_id < FIELD_WFBC_NPCID_MAX )
  {
    return WFBC_GetConstPeople( cp_wk, npc_id );
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�̊X�^�C�v���擾
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return �X�^�C�v
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_CORE_TYPE FIELD_WFBC_GetType( const FIELD_WFBC* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->type;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�̃}�b�v���[�h�̎擾
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return �}�b�v���[�h
 */
//-----------------------------------------------------------------------------
MAPMODE FIELD_WFBC_GetMapMode( const FIELD_WFBC* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->mapmode;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �z�u�u���b�N�̐ݒ�
 *
 *	@param	p_wk     ���[�N
 *	@param  p_attr    �A�g���r���[�g
 *	@param	p_bm      buildmodel
 *	@param	g3Dmap    �}�b�v�u���b�N�Ǘ�
 *	@param  build_count �z�u���f���J�E���g
 *	@param	block_x   �u���b�N���C���f�b�N�X
 *	@param	block_z   �u���b�N���C���f�b�N�X
 *	@param  score     �u���b�N�X�R�A
 *	@param	heapID    �q�[�vID 
 *
 *	@retval build_count �ǉ���
 */
//-----------------------------------------------------------------------------
int FIELD_WFBC_SetUpBlock( FIELD_WFBC* p_wk, NormalVtxFormat* p_attr, FIELD_BMODEL_MAN* p_bm, FLD_G3D_MAP* g3Dmap, int build_count, u32 block_x, u32 block_z, HEAPID heapID )
{
  WFBC_BLOCK_NOW map_data;
  u32 local_grid_x, local_grid_z;
  FIELD_DATA_PATCH* p_patch;
  

  map_data = WFBC_NOW_MAPDATA_GetData( &p_wk->map_data, block_x, block_z );

  
  if( map_data.block_tag.block_no != FIELD_WFBC_BLOCK_NONE )
  {
    TOMOYA_Printf( "block_tag %d\n", map_data.block_tag.block_no );
    TOMOYA_Printf( "block pos x %d  z %d\n", map_data.block_tag.pos_x, map_data.block_tag.pos_z );
    TOMOYA_Printf( "block_x=%d  block_z=%d\n", block_x, block_z );


    // g3Dmap���[�J���̃O���b�h���W�����߂�
    local_grid_x = FIELD_WFBC_BLOCK_TO_GRID_X( block_x ) % 32;
    local_grid_z = FIELD_WFBC_BLOCK_TO_GRID_Z( block_z ) % 32;
    TOMOYA_Printf( "local_grid_x %d\n", local_grid_x );
    TOMOYA_Printf( "local_grid_z %d\n", local_grid_z );

    TOMOYA_Printf( "build_count %d\n", build_count );
    
    // �p�b�`���ǂݍ���
    p_patch = FIELD_DATA_PATCH_Create( map_data.block_id, GFL_HEAP_LOWID(heapID) );

    // �A�g���r���[�g�̃I�[�o�[���C�g
    FIELD_DATA_PATCH_OverWriteAttrEx( p_patch, p_attr, map_data.block_tag.pos_x*8, map_data.block_tag.pos_z*8, local_grid_x, local_grid_z, 8, 8 );

    // �z�u�㏑��
    if( (map_data.block_tag.pos_x == 0) && (map_data.block_tag.pos_z == 0) )
    {
      // ����̃u���b�N���̂Ƃ��ɂ̂ݐݒ�
      build_count = FIELD_LAND_DATA_PATCH_AddBuildModel( p_patch, p_bm, g3Dmap, build_count, local_grid_x, local_grid_z );
    }

    // �p�b�`���j��
    FIELD_DATA_PATCH_Delete( p_patch );
  }

  return build_count;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �C�x���g�̐ݒ�ύX
 *
 *	@param	cp_wk     ���[�N
 *	@param	p_evdata  �C�x���g�V�X�e��
 *	@param	heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
void FILED_WFBC_EventDataOverwrite( const FIELD_WFBC* cp_wk, EVENTDATA_SYSTEM* p_evdata, MAPMODE mapmode, HEAPID heapID )
{
  int i, j;
  FIELD_WFBC_EVENT_IF* p_event_if;
  u32 load_size;
  const CONNECT_DATA* cp_connect;
  static const u32 sc_EVIF[ FIELD_WFBC_CORE_TYPE_MAX ] = 
  {
    NARC_field_wfbc_data_bc_block_event_evp,
    NARC_field_wfbc_data_wf_block_event_evp,
  };
  u32 add_connect_num;
  u32* p_ok_connect_idx_tbl;
  u32 ok_connect_idx_num;

  // �����E�ł͂Ȃɂ����Ȃ�
  if( mapmode != MAPMODE_NORMAL )
  {
    return ;
  }

  // �g���C�x���g�����擾
  load_size = GFL_ARC_GetDataSizeByHandle( cp_wk->p_handle, sc_EVIF[ cp_wk->type ] );
  p_event_if = GFL_ARC_LoadDataAllocByHandle( cp_wk->p_handle, sc_EVIF[ cp_wk->type ], GFL_HEAP_LOWID(heapID) );
  
  // �f�[�^�T�C�Y����A�Ǘ��ڑ��|�C���g�̐����`�F�b�N
  add_connect_num = load_size / sizeof(FIELD_WFBC_EVENT_IF);

  // OK CONNECT IDX �e�[�u���쐬
  p_ok_connect_idx_tbl = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID(heapID), sizeof(u32)*add_connect_num );
  ok_connect_idx_num = 0;

  // ��������
  for( i=0; i<add_connect_num; i++ )
  {
    TOMOYA_Printf( "Event Check blockTag=%d  blockID=%d\n", p_event_if[i].block_tag, p_event_if[i].block_id );
    if( WFBC_NOW_MAPDATA_IsDataIn( &cp_wk->map_data, p_event_if[i].block_tag, p_event_if[i].block_id ) )
    {
      // OK CONNECT_IDX�ɒǉ�
      p_ok_connect_idx_tbl[ ok_connect_idx_num ] = p_event_if[i].connect_idx;
      ok_connect_idx_num ++;
    }
    else
    {
      // OK�����Ȃ��āA�Ō��connect_idx�Ȃ�{���ɔj���`�F�b�N
      for( j=0; j<ok_connect_idx_num; j++ )
      {
        if( p_ok_connect_idx_tbl[j] == p_event_if[i].connect_idx )
        {
          break;
        }
      }
      if( j>=ok_connect_idx_num )
      {
        for( j=i+1; j<add_connect_num; j++ )
        {
          if( p_event_if[i].connect_idx == p_event_if[j].connect_idx )
          {
            break;
          }
        }
        // �Ō��connect_idx�Ȃ�{���ɔj��
        if( j>=add_connect_num ){
          TOMOYA_Printf( "Event Check EventDel %d\n", p_event_if[i].connect_idx );
          // �폜
          EVENTDATA_SYS_DelConnectEventIdx( p_evdata, p_event_if[i].connect_idx );
        }
      }
    }
  }

  GFL_HEAP_FreeMemory( p_ok_connect_idx_tbl );
  GFL_HEAP_FreeMemory( p_event_if );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �J�����̐ݒ�
 *
 *	@param	cp_wk     ���[�N
 *	@param	p_camera  �J����
 *	@param	heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_SetUpCamera( const FIELD_WFBC* cp_wk, FIELD_CAMERA* p_camera, HEAPID heapID )
{
  FIELD_WFBC_CAMERA_SETUP* p_buff;
  u32 people_num;
  
  // �ǂݍ���
  p_buff = GFL_ARCHDL_UTIL_Load( cp_wk->p_handle, NARC_field_wfbc_data_wb_wfbc_camera_cam, FALSE, GFL_HEAP_LOWID(heapID) );  

  // �l���̐����擾
  people_num = FIELD_WFBC_GetPeopleNum( cp_wk );

  GF_ASSERT( people_num <= FIELD_WFBC_PEOPLE_MAX );

  // �J�����ݒ�
  FIELD_CAMERA_SetCameraType( p_camera, p_buff->camera_no[ people_num ] );

  GFL_HEAP_FreeMemory( p_buff );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���샂�f���V�X�e���̐ݒ�
 *
 *	@param	cp_wk       ���[�N
 *	@param	p_mmdlsys   ���샂�f��
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_SetUpMmdlSys( FIELD_WFBC* p_wk, MMDLSYS* p_mmdlsys )
{
  p_wk->p_mmdlsys = p_mmdlsys;
  //@todo �����ǂ����Ă��A�����������y���ł��Ȃ�������A
  //�e���������Ƃ�300 ms�y���Ȃ�B
  //MMDLSYS_SetJoinShadow( p_mmdlsys, FALSE );
}



//----------------------------------------------------------------------------
/**
 *	@brief  �X����ݒ�
 *
 *	@param	p_wk        ���[�N
 *	@param	cp_core     �R�A���
 *	@param  mapmode     �}�b�v���[�h
 *	@param  heapID      �q�[�vID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_SetUp( FIELD_WFBC* p_wk, FIELD_WFBC_CORE* p_core, MAPMODE mapmode, HEAPID heapID )
{
  // �������[�N�̏�����
  WFBC_InitSystem( p_wk, heapID );
  
  WFBC_SetCore( p_wk, p_core, mapmode );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�����N���A
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_Clear( FIELD_WFBC* p_wk )
{
  WFBC_Clear( p_wk );
  WFBC_ExitSystem( p_wk );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �l���̒ǉ�
 *
 *	@param	p_wk    ���[�N
 *
 *	@retval �ǉ������C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_AddPeople( FIELD_WFBC* p_wk, const FIELD_WFBC_CORE_PEOPLE* cp_core )
{
  int index;
  const FIELD_WFBC_CORE_PEOPLE_POS* cp_pos;

  GF_ASSERT( p_wk );
  // �󂢂Ă��郏�[�N���擾
  index = WFBC_GetClearPeople( p_wk );

  // NPC���ǂݍ���
  FIELD_WFBC_PEOPLE_DATA_Load( p_wk->p_loader, cp_core->npc_id );
  
  // �l�����Z�b�g�A�b�v
  cp_pos = FIELD_WFBC_PEOPLE_POS_GetIndexData( p_wk->p_pos, index );
  WFBC_PEOPLE_SetUp( &p_wk->p_people[index], cp_core, cp_pos, FIELD_WFBC_PEOPLE_DATA_GetData( p_wk->p_loader ) );

  // �Z�[�u�f�[�^�ɔ��f
  WFBC_UpdateCore( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l���̔j��
 *
 *	@param	p_wk    ���[�N
 *	@param	npc_id  �j������l����npc_ID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_DeletePeople( FIELD_WFBC* p_wk, u32 npc_id )
{
  FIELD_WFBC_PEOPLE* p_people;
  GF_ASSERT( p_wk );

  p_people = WFBC_GetPeople( p_wk, npc_id );
  // �C���f�b�N�X�̃��[�N���N���[��
  WFBC_PEOPLE_Clear( p_people );

  // �Z�[�u�f�[�^�ɔ��f
  WFBC_UpdateCore( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  npc_id�̐l��A��čs���ꂽ�ݒ�ɂ���
 *
 *	@param	p_wk      ���[�N
 *	@param	npc_id    npc_id
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_SetAwayPeople( FIELD_WFBC* p_wk, u32 npc_id )
{
  FIELD_WFBC_PEOPLE* p_people;
  GF_ASSERT( p_wk );

  p_people = WFBC_GetPeople( p_wk, npc_id );
  p_people->status = FIELD_WFBC_PEOPLE_STATUS_AWAY;
 
  // �Z�[�u�f�[�^�ɔ��f
  WFBC_UpdateCore( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���̐l�Ɖ�b�����A�@���l���Z����
 *
 *	@param	p_wk
 *	@param	npc_id 
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_AddTalkPointPeople( FIELD_WFBC* p_wk, u32 npc_id )
{
  FIELD_WFBC_PEOPLE* p_people;
  GF_ASSERT( p_wk );

  p_people = WFBC_GetPeople( p_wk, npc_id );

  // ��b���Z
  if( p_wk->mapmode == MAPMODE_NORMAL )
  {
    FIELD_WFBC_CORE_PEOPLE_CalcTalk( &p_people->people_local );

    // �Z�[�u�f�[�^�ɔ��f
    WFBC_UpdateCore( p_wk );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�h�Z�b�g�ɗ����̃v���C���[����ݒ�
 *
 *	@param	cp_wk       ���[�N
 *	@param	p_wordset   ���[�h�Z�b�g
 *	@param	npc_id      NPCID
 *	@param  word_set_idx  ���[�h�Z�b�g�̃C���f�b�N�X
 *	@param  heapID      �q�[�vID
 */ 
//-----------------------------------------------------------------------------
void FIELD_WFBC_SetWordSetParentPlayer( const FIELD_WFBC* cp_wk, WORDSET* p_wordset, u32 npc_id, u32 word_set_idx, HEAPID heapID )
{
  MYSTATUS* p_mystatus;
  const FIELD_WFBC_PEOPLE* cp_people;
  GF_ASSERT( cp_wk );

  cp_people = WFBC_GetConstPeople( cp_wk, npc_id );

  p_mystatus = MyStatus_AllocWork( GFL_HEAP_LOWID(heapID) );
  MyStatus_Init( p_mystatus );
  
  MyStatus_SetMyName( p_mystatus, cp_people->people_local.parent );
  MyStatus_SetID( p_mystatus, cp_people->people_local.parent_id );
  

  // ���[�h�Z�b�g�ɐݒ�
  WORDSET_RegisterPlayerName( p_wordset, word_set_idx, p_mystatus );


  GFL_HEAP_FreeMemory( p_mystatus );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���������Ƃ�ݒ�
 *
 *	@param	p_wk      ���[�N
 *	@param	npc_id    NPCID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_SetBattlePeople( FIELD_WFBC* p_wk, u32 npc_id )
{
  FIELD_WFBC_PEOPLE* p_people;
  GF_ASSERT( p_wk );

  p_people = WFBC_GetPeople( p_wk, npc_id );

  FIELD_WFBC_CORE_PEOPLE_SetBattle( &p_people->people_local );

  // �Z�[�u�f�[�^�ɔ��f
  WFBC_UpdateCore( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�g�����邱�Ƃ��o����̂���Ԃ�
 *
 *	@param	p_wk      ���[�N
 *	@param	npc_id    NPCID
 *
 *	@retval TRUE    �o����
 *	@retval FALSE   �o���Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_IsBattlePeople( const FIELD_WFBC* cp_wk, u32 npc_id )
{
  const FIELD_WFBC_PEOPLE* cp_people;
  GF_ASSERT( cp_wk );

  cp_people = WFBC_GetConstPeople( cp_wk, npc_id );

  return FIELD_WFBC_CORE_PEOPLE_IsBattle( &cp_people->people_local );
}



//----------------------------------------------------------------------------
/**
 *	@brief  �l���@�����ڂ̎擾
 *
 *	@param	cp_people   ���[�N
 *
 *	@return ������
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_PEOPLE_GetOBJCode( const FIELD_WFBC_PEOPLE* cp_people )
{
  GF_ASSERT( cp_people );
  return cp_people->people_data.objid;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l�����[�N  ����@�l����Ԃ��擾
 *
 *	@param	cp_people   �l�����[�N
 *
 *	@retval �l�����
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_PEOPLE_STATUS FIELD_WFBC_PEOPLE_GetStatus( const FIELD_WFBC_PEOPLE* cp_people )
{
  GF_ASSERT( cp_people );
  return cp_people->status;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l���f�[�^  ����@�l����{�����擾
 *
 *	@param	cp_people   �l�����[�N
 *
 *	@return ��{���
 */
//-----------------------------------------------------------------------------
const FIELD_WFBC_PEOPLE_DATA* FIELD_WFBC_PEOPLE_GetPeopleData( const FIELD_WFBC_PEOPLE* cp_people )
{
  GF_ASSERT( cp_people );
  return &cp_people->people_data;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l���f�[�^�@����@���[�J���@�̃Z�[�u�l�������擾
 *
 *	@param	cp_people   �l�����[�N
 *
 *	@return ���[�J���̃Z�[�u�l�����
 */
//-----------------------------------------------------------------------------
const FIELD_WFBC_CORE_PEOPLE* FIELD_WFBC_PEOPLE_GetPeopleCore( const FIELD_WFBC_PEOPLE* cp_people )
{
  GF_ASSERT( cp_people );
  return &cp_people->people_local;
}







//-----------------------------------------------------------------------------
/**
 *      private�֐�
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  �K�v�V�X�e���̏�����
 *
 *	@param	p_wk        ���[�N
 *	@param	heapID      �q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFBC_InitSystem( FIELD_WFBC* p_wk, HEAPID heapID )
{
  int i;
  
  if(p_wk->p_loader == NULL)
  {
    p_wk->p_loader = FIELD_WFBC_PEOPLE_DATA_Create( 0, heapID );
    p_wk->p_people = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_WFBC_PEOPLE) * FIELD_WFBC_PEOPLE_MAX );

    //�@�l�����[�N���N���A
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      WFBC_PEOPLE_Clear( &p_wk->p_people[i] );
    }

  
    // �f�[�^�ǂݍ��݂悤�̃n���h������
    p_wk->p_handle = GFL_ARC_OpenDataHandle( ARCID_WFBC_PEOPLE, heapID );

    // �z�u���̃o�b�t�@�𐶐�
    p_wk->p_block = GFL_HEAP_AllocClearMemory( heapID, FIELD_WFBC_BLOCK_BUFF_SIZE );

    // �l���ʒu���𐶐�
    p_wk->p_pos = FIELD_WFBC_PEOPLE_POS_Create( p_wk->p_loader, p_wk->type, heapID );
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief  �K�v�V�X�e���̔j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void WFBC_ExitSystem( FIELD_WFBC* p_wk )
{
  if(p_wk->p_loader)
  {
    // �l���ʒu���𐶐�
    FIELD_WFBC_PEOPLE_POS_Delete( p_wk->p_pos );
    GFL_HEAP_FreeMemory( p_wk->p_block );
    GFL_ARC_CloseDataHandle( p_wk->p_handle );
    FIELD_WFBC_PEOPLE_DATA_Delete( p_wk->p_loader );
    GFL_HEAP_FreeMemory( p_wk->p_people );

    p_wk->p_loader = NULL;
    p_wk->p_people = NULL;
    p_wk->p_handle = NULL;
    p_wk->p_block = NULL;
    p_wk->p_pos = NULL;
  } 
}


//----------------------------------------------------------------------------
/**
 *	@brief  WFBC���[�N���N���A
 */
//-----------------------------------------------------------------------------
static void WFBC_Clear( FIELD_WFBC* p_wk )
{
  int i;

  p_wk->type = 0;
  p_wk->mapmode = 0;
  p_wk->p_core = NULL;

  //�@�l�����[�N���N���A
  if( p_wk->p_people )
  {
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      WFBC_PEOPLE_Clear( &p_wk->p_people[i] );
    }
  }

  // �u���b�N�z�u�����N���A
  if( p_wk->p_block )
  {
    GFL_STD_MemClear( p_wk->p_block, FIELD_WFBC_BLOCK_BUFF_SIZE );
  }

  // �z�u���N���A
  WFBC_NOW_MAPDATA_Init( &p_wk->map_data );

  // �`�搶�����[�N�N���A
  WFBC_DRAW_PARAM_Init( &p_wk->draw_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �󂢂Ă���l�����[�N���擾����
 *
 *	@param	cp_wk     ���[�N
 *
 *	@return �󂢂Ă��郏�[�N�̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static int WFBC_GetClearPeople( const FIELD_WFBC* cp_wk )
{
  int i;

  GF_ASSERT( cp_wk );

  if( cp_wk->p_people )
  {
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( cp_wk->p_people[i].status != FIELD_WFBC_PEOPLE_STATUS_NONE )
      {
        return i;
      }
    }
  }

  GF_ASSERT_MSG( 0, "WFBC People Over\n" );
  return 0; // �������j��Ȃǂ��Ȃ��悤�ɂO��Ԃ�
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l���̐����擾����
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return �l���̐�
 */
//-----------------------------------------------------------------------------
static int WFBC_GetPeopleNum( const FIELD_WFBC* cp_wk )
{
  int num;
  int i;
  
  GF_ASSERT( cp_wk );

  num = 0;
  if( cp_wk->p_people )
  {
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( cp_wk->p_people[i].status != FIELD_WFBC_PEOPLE_STATUS_NONE )
      {
        num ++;
      }
    }
  }

  return num;
}


//----------------------------------------------------------------------------
/**
 *	@brief  CORE����ݒ肷��
 *
 *	@param	p_wk        ���[�N
 *	@param	cp_buff     �o�b�t�@
 */
//-----------------------------------------------------------------------------
static void WFBC_SetCore( FIELD_WFBC* p_wk, FIELD_WFBC_CORE* p_buff, MAPMODE mapmode )
{
  int i;
  BOOL result;
  const FIELD_WFBC_CORE_PEOPLE_POS* cp_pos;
  u32 random_max;
  u32 pattern_num;
  const FIELD_WFBC_PEOPLE_DATA* cp_people_data;

  GF_ASSERT( p_wk );
  GF_ASSERT( p_wk->p_people );
  GF_ASSERT( p_wk->p_loader );
  GF_ASSERT( p_buff );

  result = FIELD_WFBC_CORE_IsInData( p_buff );
  GF_ASSERT( result );
  
  // �����WFBC�����N���[���A�b�v
  WFBC_Clear( p_wk );
  

  // ����ݒ�
  p_wk->type    = p_buff->type;
  p_wk->mapmode = mapmode;
  random_max    = 0;
  pattern_num   = 0;
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    // �l�����̐ݒ�
    // �f�[�^����������ݒ�
    if( mapmode == MAPMODE_NORMAL )
    {
      if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_buff->people[i] ) )
      {
        // NPC���ǂݍ���
        FIELD_WFBC_PEOPLE_DATA_Load( p_wk->p_loader, p_buff->people[i].npc_id );
        cp_pos = FIELD_WFBC_PEOPLE_POS_GetIndexData( p_wk->p_pos, i );
        cp_people_data = FIELD_WFBC_PEOPLE_DATA_GetData( p_wk->p_loader );
        WFBC_PEOPLE_SetUp( &p_wk->p_people[i], &p_buff->people[i], cp_pos, cp_people_data );
      }
    }
    else
    {
      if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_buff->back_people[i] ) )
      {
        // NPC���ǂݍ���
        FIELD_WFBC_PEOPLE_DATA_Load( p_wk->p_loader, p_buff->back_people[i].npc_id );
        cp_pos = FIELD_WFBC_PEOPLE_POS_GetIndexData( p_wk->p_pos, i );
        cp_people_data = FIELD_WFBC_PEOPLE_DATA_GetData( p_wk->p_loader );
        WFBC_PEOPLE_SetUp( &p_wk->p_people[i], &p_buff->back_people[i], cp_pos, FIELD_WFBC_PEOPLE_DATA_GetData( p_wk->p_loader ) );
      }
    }
  }

  // �����ڂɂ������p�����[�^�̐���
  WFBC_DRAW_PARAM_SetUp( &p_wk->draw_param, p_buff, p_wk->p_loader );

  // �R�A���ۑ�
  if( mapmode == MAPMODE_NORMAL )
  {
    p_wk->p_core = p_buff;
  }
  else
  {
    p_wk->p_core = NULL;
  }


  // �u���b�N�z�u����ǂݍ���
  {
    u32 size = GFL_ARC_GetDataSizeByHandle( p_wk->p_handle, sc_FIELD_WFBC_BLOCK_FILE[p_wk->type] );
    GF_ASSERT( size < FIELD_WFBC_BLOCK_BUFF_SIZE );
    GFL_ARC_LoadDataByHandle( p_wk->p_handle, sc_FIELD_WFBC_BLOCK_FILE[p_wk->type], p_wk->p_block );
  }

  // �u���b�N�z�u��񐶐�
  WFBC_DRAW_PARAM_MakeMapData( &p_wk->draw_param, p_wk->p_block, &p_wk->map_data );
}

//----------------------------------------------------------------------------
/**
 *	@brief  CORE�����擾����
 *
 *	@param	cp_wk     ���[�N
 *	@param	p_buff    �f�[�^�i�[��
 */
//-----------------------------------------------------------------------------
static void WFBC_GetCore( const FIELD_WFBC* cp_wk, FIELD_WFBC_CORE* p_buff )
{
  int i;

  GF_ASSERT( cp_wk );
  GF_ASSERT( p_buff );
  GF_ASSERT( cp_wk->p_people );

  // �����i�[
  p_buff->data_in = TRUE;
  p_buff->type    = cp_wk->type;

  if( cp_wk->mapmode == MAPMODE_NORMAL )
  {
    FIELD_WFBC_CORE_ClearNormal( p_buff );
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      // �f�[�^����������ݒ�
      WFBC_PEOPLE_GetCore( &cp_wk->p_people[i], &p_buff->people[i] );
    }
  }
  else
  {
    FIELD_WFBC_CORE_ClearBack( p_buff );
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      // �f�[�^����������ݒ�
      WFBC_PEOPLE_GetCore( &cp_wk->p_people[i], &p_buff->back_people[i] );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�v���[�h���ʏ펞�̃Z�[�u�f�[�^�X�V����
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void WFBC_UpdateCore( FIELD_WFBC* p_wk )
{
  if( (p_wk->mapmode == MAPMODE_NORMAL) && (p_wk->p_core != NULL) )
  {
    // ���̊i�[
    FIELD_WFBC_GetCore( p_wk, p_wk->p_core );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  npc_id�̐l�̎擾
 *
 *	@param	p_wk    ���[�N
 *	@param	npc_id 
 *
 *	@return �l
 */
//-----------------------------------------------------------------------------
static FIELD_WFBC_PEOPLE* WFBC_GetPeople( FIELD_WFBC* p_wk, u32 npc_id )
{
  int i;

  GF_ASSERT( p_wk );
  GF_ASSERT( npc_id < FIELD_WFBC_NPCID_MAX );

  if(p_wk->p_people)
  {
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( p_wk->p_people[i].status != FIELD_WFBC_PEOPLE_STATUS_NONE )
      {
        if( FIELD_WFBC_CORE_PEOPLE_GetNpcID(&p_wk->p_people[i].people_local ) == npc_id )
        {
          return &p_wk->p_people[i];
        }
      }
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  npc_id�̐l�̎擾
 *
 *	@param	cp_wk
 *	@param	npc_id 
 *
 *	@return �l
 */
//-----------------------------------------------------------------------------
static const FIELD_WFBC_PEOPLE* WFBC_GetConstPeople( const FIELD_WFBC* cp_wk, u32 npc_id )
{
  int i;

  GF_ASSERT( cp_wk );
  GF_ASSERT( npc_id < FIELD_WFBC_NPCID_MAX );

  if(cp_wk->p_people)
  {
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( cp_wk->p_people[i].status != FIELD_WFBC_PEOPLE_STATUS_NONE )
      {
        if( FIELD_WFBC_CORE_PEOPLE_GetNpcID(&cp_wk->p_people[i].people_local) == npc_id )
        {
          return &cp_wk->p_people[i];
        }
      }
    }
  }

  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �l�����[�N�̃N���A
 *
 *	@param	p_people  �l��
 */
//-----------------------------------------------------------------------------
static void WFBC_PEOPLE_Clear( FIELD_WFBC_PEOPLE* p_people )
{
  GFL_STD_MemClear( p_people, sizeof(FIELD_WFBC_PEOPLE) );
  p_people->status  = FIELD_WFBC_PEOPLE_STATUS_NONE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �l���̃Z�b�g�A�b�v
 *
 *	@param	p_people    �l�����[�N
 *	@param	cp_core     �l��CORE���
 *	@param  cp_pos      �ʒu���
 *	@param  cp_data     �f�[�^
 */
//-----------------------------------------------------------------------------
static void WFBC_PEOPLE_SetUp( FIELD_WFBC_PEOPLE* p_people, const FIELD_WFBC_CORE_PEOPLE* cp_core, const FIELD_WFBC_CORE_PEOPLE_POS* cp_pos, const FIELD_WFBC_PEOPLE_DATA* cp_data )
{
  BOOL result;
  
  GF_ASSERT( p_people );
  // ����ȏ�񂩃`�F�b�N
  result = FIELD_WFBC_CORE_PEOPLE_IsInData( cp_core );
  GF_ASSERT( result == TRUE );

  p_people->status        = FIELD_WFBC_PEOPLE_STATUS_NORMAL;
  GFL_STD_MemCopy( cp_core, &p_people->people_local, sizeof(FIELD_WFBC_CORE_PEOPLE) );

  // NPCID�̊�{����ǂݍ���
  GFL_STD_MemCopy( cp_data, &p_people->people_data, sizeof(FIELD_WFBC_PEOPLE_DATA) );
  p_people->gx        = cp_pos->gx;
  p_people->gz        = cp_pos->gz;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �l��CORE���̎擾
 *
 *	@param	cp_people   �l�����[�N
 *	@param	p_core      ���i�[��
 */
//-----------------------------------------------------------------------------
static void WFBC_PEOPLE_GetCore( const FIELD_WFBC_PEOPLE* cp_people, FIELD_WFBC_CORE_PEOPLE* p_core )
{
  GF_ASSERT( cp_people );
  GF_ASSERT( p_core );

  if( cp_people->status == FIELD_WFBC_PEOPLE_STATUS_NORMAL )
  {
    // �����R�s�[
    GFL_STD_MemCopy( &cp_people->people_local, p_core, sizeof(FIELD_WFBC_CORE_PEOPLE) );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �����}�b�v���@������
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void WFBC_NOW_MAPDATA_Init( FIELD_WFBC_NOW_MAPDATA* p_wk )
{
  int i, j;

  for( i=0; i<FIELD_WFBC_BLOCK_SIZE_Z; i++ )
  {
    for( j=0; j<FIELD_WFBC_BLOCK_SIZE_X; j++ )
    {
      p_wk->map_now[i][j].block_tag.block_no = FIELD_WFBC_BLOCK_NONE;
      p_wk->map_now[i][j].block_id = FIELD_WFBC_NOW_MAP_ERR;
    }
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�v���̐ݒ�
 *
 *	@param	p_wk        ���[�N
 *	@param	block_x     �u���b�NX
 *	@param	block_z     �u���b�NZ
 *	@param	block_tag   �u���b�N�@�^�OID
 *	@param	block_id    �u���b�N�@LAND_DATA_PATCH�@ID
 */
//-----------------------------------------------------------------------------
static void WFBC_NOW_MAPDATA_SetData( FIELD_WFBC_NOW_MAPDATA* p_wk, u32 block_x, u32 block_z, WFBC_BLOCK_DATA block_tag, u16 block_id )
{
  GF_ASSERT( block_x < FIELD_WFBC_BLOCK_SIZE_X );
  GF_ASSERT( block_z < FIELD_WFBC_BLOCK_SIZE_Z );
  
  p_wk->map_now[block_z][block_x].block_tag = block_tag;
  p_wk->map_now[block_z][block_x].block_id = block_id;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �u���b�N�́@�}�b�v���擾
 *
 *	@param	p_wk      ���[�N
 *	@param	block_x   �u���b�N��
 *	@param	block_z   �u���b�N��
 *
 *	@return �}�b�v���
 */
//-----------------------------------------------------------------------------
static WFBC_BLOCK_NOW WFBC_NOW_MAPDATA_GetData( const FIELD_WFBC_NOW_MAPDATA* cp_wk, u32 block_x, u32 block_z )
{
  GF_ASSERT( block_x < FIELD_WFBC_BLOCK_SIZE_X );
  GF_ASSERT( block_z < FIELD_WFBC_BLOCK_SIZE_Z );
  return cp_wk->map_now[block_z][block_x];
}

//----------------------------------------------------------------------------
/**
 *	@brief  block_tag��block_id�����邩�`�F�b�N
 *
 *	@param	cp_wk       ���[�N
 *	@param	block_tag
 *	@param	block_id 
 *
 *	@retval TRUE  ����
 *	@retval FALSE �Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_NOW_MAPDATA_IsDataIn( const FIELD_WFBC_NOW_MAPDATA* cp_wk, u16 block_tag, u16 block_id )
{
  int i, j;

  for( i=0; i<FIELD_WFBC_BLOCK_SIZE_Z; i++ )
  {
    for( j=0; j<FIELD_WFBC_BLOCK_SIZE_X; j++ )
    {
      if( cp_wk->map_now[i][j].block_tag.block_no == block_tag )
      {
        if( cp_wk->map_now[i][j].block_id == block_id )
        {
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �`��p�p�����[�^�̏�����
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void WFBC_DRAW_PARAM_Init( WFBC_DRAW_PARAM* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(WFBC_DRAW_PARAM) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �`��p�p�����[�^�̐ݒ�
 *
 *	@param	p_wk      ���[�N
 *	@param	cp_data   WFBC�R�A���
 *	@param  p_loader  ���[�_�[
 */
//-----------------------------------------------------------------------------
static void WFBC_DRAW_PARAM_SetUp( WFBC_DRAW_PARAM* p_wk, const FIELD_WFBC_CORE* cp_data, FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader )
{
  int i;
  const FIELD_WFBC_PEOPLE_DATA* cp_people_data;
  u32 random_max;
  u32 pattern_num;

  random_max = 0;
  pattern_num = 0;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    // �����ڂɂ������B
    // �u���b�N�␳�l�n�́A�\���E�̏����g�p����
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_data->people[i] ) )
    {
      // NPC���ǂݍ���
      FIELD_WFBC_PEOPLE_DATA_Load( p_loader, cp_data->people[i].npc_id );
      cp_people_data = FIELD_WFBC_PEOPLE_DATA_GetData( p_loader );
      // �u���b�N�␳�l�����߂�
      random_max += cp_people_data->block_param;
      pattern_num += cp_data->people[i].npc_id;
    }
  }
  
  GFL_STD_RandInit( &p_wk->randData, (u64)((cp_data->random_no << 32) + cp_data->random_no) );
  p_wk->block_rand_max = 2 + random_max;
  p_wk->block_pattern_num = pattern_num;
  p_wk->random_no = cp_data->random_no;


  // �`��l����
  p_wk->draw_people_num = FIELD_WFBC_CORE_GetPeopleNum( cp_data, MAPMODE_NORMAL );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �`��p�����[�^���g�p���āA�u���b�N�z�u���𐶐�
 *
 *	@param	p_wk      ���[�N
 *	@param	p_mapdata �}�b�v���
 */
//-----------------------------------------------------------------------------
#ifdef PM_DEBUG
extern s16 DEBUG_FIELD_WFBC_MAKE_score;
extern u16 DEBUG_FIELD_WFBC_MAKE_flag;
#endif
static void WFBC_DRAW_PARAM_MakeMapData( WFBC_DRAW_PARAM* p_wk, const FIELD_WFBC_BLOCK_DATA* cp_block, FIELD_WFBC_NOW_MAPDATA* p_mapdata )
{
  int i, j;
  WFBC_BLOCK_DATA block_tag;
  const FIELD_WFBC_BLOCK_PATCH* cp_patch_data;
  u8 tbl_index;
  s32 score;
  u32 land_data_patch_id;

  // RANDOM����
  GFL_STD_RandInit( &p_wk->randData, (u64)((p_wk->random_no << 32) + p_wk->random_no) );

  for( i=0; i<FIELD_WFBC_BLOCK_SIZE_Z; i++ )
  {
    for( j=0; j<FIELD_WFBC_BLOCK_SIZE_X; j++ )
    {
      block_tag = cp_block->block_data[ i ][ j ];

      if( block_tag.block_no != FIELD_WFBC_BLOCK_NONE )
      {
        // �u���b�N�̃X�R�A���v�Z
        // �u���b�N�̍���̂݌v�Z
        if( (block_tag.pos_x != 0) || (block_tag.pos_z != 0) )
        {
          // ����u���b�N�̏������̂܂ܕۑ�
          WFBC_NOW_MAPDATA_SetData( p_mapdata, j, i, block_tag, 
              p_mapdata->map_now[i - block_tag.pos_z][j - block_tag.pos_x].block_id );
        }
        else
        {
          u32 block_lefttop_x, block_lefttop_z;
          block_lefttop_x = j - block_tag.pos_x;
          block_lefttop_z = i - block_tag.pos_z;
          
          // �i�iWF�ɂ���S�Ă�OBJ��NO�̍��v�@�{�@�u���b�N����̂w���W�{�@�u���b�N�����Y���W�j / (�u���b�N����̂w���W+1) /(�u���b�N�����Y���W+1) +�Q �j�^�Q
          tbl_index = p_wk->block_pattern_num;
          tbl_index += block_lefttop_x + block_lefttop_z;
          tbl_index /= (block_lefttop_x+1) + (block_lefttop_z+1) + 2;
          tbl_index %= 2;

          // �iOBJ�̐l���~0.8�j�{�����_���i�O�`�P�j
          score = p_wk->draw_people_num;
          score = (score * 8) / 10;
          score += GFL_STD_Rand( &p_wk->randData, p_wk->block_rand_max );

          if( score >= (FIELD_WFBC_BLOCK_PATCH_MAX/2) )
          {
            score = (FIELD_WFBC_BLOCK_PATCH_MAX/2)-1;
          }
          
          score += tbl_index*(FIELD_WFBC_BLOCK_PATCH_MAX/2);
        
          GF_ASSERT( score < FIELD_WFBC_BLOCK_PATCH_MAX );

          // �ݒ�
          cp_patch_data = &cp_block->patch[ block_tag.block_no ];
          land_data_patch_id = cp_patch_data->patch[ score ];

#ifdef FIELD_WFBC_MAKE_MAPDATA_DEBUG
          if( DEBUG_FIELD_WFBC_MAKE_flag )
          {
            int i;
            for( i=0; i<FIELD_WFBC_BLOCK_PATCH_MAX; i++ )
            {
              if( cp_patch_data->patch[ i ] == DEBUG_FIELD_WFBC_MAKE_score )
              {
                land_data_patch_id = DEBUG_FIELD_WFBC_MAKE_score;
              }
            }
          }
#endif

          // ���̕ۑ�
          WFBC_NOW_MAPDATA_SetData( p_mapdata, j, i, block_tag, land_data_patch_id );
        }
      }

    }
  }
}



//�f�o�b�O����
#ifdef PM_DEBUG

#include "gamesystem/game_data.h"
#include "poke_tool/pokeparty.h"
#include "msg/msg_place_name.h"  // for MAPNAME_xxxx
#include "system/main.h"  // 


// �l�����������悤�p�����[�^
static u8 DEBUG_WFBCPeople_mode = 0;
static s8 DEBUG_WFBCPeople_index = 0;
static s8 DEBUG_WFBCPeople_npc_id = 0;


static void DEBWIN_Update_CityLevel( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_CityLevel( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_CityType( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_CityType( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_BCWinNumAdd10( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_BCWinNumAdd10( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_WFPokeGet( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_WFPokeGet( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_WFBCBlockCheck( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_WFBCBlockCheck( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_WFBCPeopleCheck( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_WFBCPeopleCheck( void* userWork , DEBUGWIN_ITEM* item );


void FIELD_FUNC_RANDOM_GENERATE_InitDebug( HEAPID heapId, void* p_gdata )
{
  DEBUGWIN_AddGroupToTop( 10 , "RandomMap" , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_CityLevel ,DEBWIN_Draw_CityLevel , 
                             p_gdata , 10 , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_CityType ,DEBWIN_Draw_CityType , 
                             p_gdata , 10 , heapId );

  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_BCWinNumAdd10 ,DEBWIN_Draw_BCWinNumAdd10 , 
                             p_gdata , 10 , heapId );

  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_WFPokeGet ,DEBWIN_Draw_WFPokeGet , 
                             p_gdata , 10 , heapId );

  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_WFBCBlockCheck,DEBWIN_Draw_WFBCBlockCheck, 
                             p_gdata , 10 , heapId );

  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_WFBCPeopleCheck,DEBWIN_Draw_WFBCPeopleCheck, 
                             p_gdata , 10 , heapId );
}

void FIELD_FUNC_RANDOM_GENERATE_TermDebug( void )
{
  DEBUGWIN_RemoveGroup( 10 );
}

static void DEBWIN_Update_CityLevel( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_CORE* p_wk = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  int  i;

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    // ���₷
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( p_wk->people[i].data_in == FALSE )
      {
        FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[i] );
        p_wk->people[i].npc_id  = FIELD_WFBC_CORE_DEBUG_GetRandomNpcID( p_wk );
        p_wk->people[i].mood  = 50;
        p_wk->people[i].one_day_msk  = FIELD_WFBC_ONEDAY_MSK_INIT;
        p_wk->people[i].data_in = TRUE;
        if( p_wk->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
        {
          FIELD_WFBC_CORE_PEOPLE_SetParentData( &p_wk->people[i], GAMEDATA_GetMyStatus(p_gdata) );
        }
        break;
      }
    }

    // �T�u�ɂ����₷
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( p_wk->back_people[i].data_in == FALSE )
      {
        FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->back_people[i] );
        p_wk->back_people[i].npc_id  = FIELD_WFBC_CORE_DEBUG_GetRandomNpcID( p_wk );
        p_wk->back_people[i].mood  = 0;
        p_wk->back_people[i].one_day_msk  = FIELD_WFBC_ONEDAY_MSK_INIT;
        p_wk->back_people[i].data_in = TRUE;
        if( p_wk->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
        {
          FIELD_WFBC_CORE_PEOPLE_SetParentData( &p_wk->back_people[i], GAMEDATA_GetMyStatus(p_gdata) );
        }
        break;
      }
    }
    
    DEBUGWIN_RefreshScreen();
  }
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT )
  {
    // ���炷
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( p_wk->people[i].data_in )
      {
        FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[i] );
        break;
      }
    }

    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( p_wk->back_people[i].data_in )
      {
        FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->back_people[i] );
        break;
      }
    }
    FIELD_WFBC_CORE_PackPeopleArray( p_wk, MAPMODE_NORMAL );
    FIELD_WFBC_CORE_PackPeopleArray( p_wk, MAPMODE_INTRUDE );
    DEBUGWIN_RefreshScreen();
  }
}

static void DEBWIN_Draw_CityLevel( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_CORE* p_wk = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  u16 level = FIELD_WFBC_CORE_GetPeopleNum( p_wk, MAPMODE_NORMAL );
  DEBUGWIN_ITEM_SetNameV( item , "Level[%d]",level );
}

static void DEBWIN_Update_CityType( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_CORE* p_wk = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  FIELD_WFBC_EVENT* p_event = GAMEDATA_GetWFBCEventData( p_gdata );
  u8 type = p_wk->type;

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT || 
      GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT  ||
      GFL_UI_KEY_GetRepeat() & PAD_BUTTON_A )
  {
    FIELD_WFBC_EVENT_Clear( p_event );

    if( type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
    {
      type = FIELD_WFBC_CORE_TYPE_WHITE_FOREST;
    }
    else
    {
      type = FIELD_WFBC_CORE_TYPE_BLACK_CITY;
    }
    p_wk->type = type;
    DEBUGWIN_RefreshScreen();

    FIELD_WFBC_CORE_SetUpZoneData( p_wk );
    FIELD_WFBC_CORE_CalcOneDataStart( p_gdata, 1, HEAPID_FIELDMAP );
  }
}

static void DEBWIN_Draw_CityType( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_CORE* p_wk = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  u8 type = p_wk->type;

  if( type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
  {
    DEBUGWIN_ITEM_SetName( item , "Type[BLACK CITY]" );
  }
  else
  {
    DEBUGWIN_ITEM_SetName( item , "Type[WHITE FOREST]" );
  }
}

static void DEBWIN_Update_BCWinNumAdd10( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_EVENT* p_wk = GAMEDATA_GetWFBCEventData( p_gdata );
  FIELD_WFBC_CORE* p_core = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  int i;

  if( p_core->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
  {
    if( GFL_UI_KEY_GetTrg() & (PAD_KEY_RIGHT) )
    {
      p_wk->bc_npc_win_count += 10;
      DEBUGWIN_RefreshScreen();
    }
    if( GFL_UI_KEY_GetTrg() & (PAD_KEY_LEFT) )
    {
      p_wk->bc_npc_win_count -= 10;
      DEBUGWIN_RefreshScreen();
    }
  }
}

static void DEBWIN_Draw_BCWinNumAdd10( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_EVENT* p_wk = GAMEDATA_GetWFBCEventData( p_gdata );
  FIELD_WFBC_CORE* p_core = GAMEDATA_GetMyWFBCCoreData( p_gdata );

  if( p_core->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
  {
    DEBUGWIN_ITEM_SetNameV( item , "BC Win[%d]", p_wk->bc_npc_win_count );
  }
  else
  {
    DEBUGWIN_ITEM_SetName( item , "BC Win??" );
  }
}

static void DEBWIN_Update_WFPokeGet( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_EVENT* p_wk = GAMEDATA_GetWFBCEventData( p_gdata );
  FIELD_WFBC_CORE* p_core = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  POKEPARTY* p_party = GAMEDATA_GetMyPokemon( p_gdata );
  POKEMON_PARAM * pp = PokeParty_GetMemberPointer( p_party, 0 );

  if( p_core->type == FIELD_WFBC_CORE_TYPE_WHITE_FOREST )
  {
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      static u16 oyaName[6] = {L'��',L'��',L'��',L'��',L'��',0xFFFF};
      RTCDate now_date;
      GFL_RTC_GetDate( &now_date );
      // �莝���O�Ԗڂɕߊl�ꏊWF�̖ړI�|�P������ݒ�
      PP_Setup( pp, p_wk->wf_poke_catch_monsno, 10, 100 );
      PP_Put( pp, ID_PARA_birth_place, MAPNAME_WC10 );
      PP_Put( pp, ID_PARA_birth_year, now_date.year );
      PP_Put( pp, ID_PARA_birth_month, now_date.month );
      PP_Put( pp, ID_PARA_birth_day, now_date.day );

      PP_Put( pp , ID_PARA_oyaname_raw , (u32)&oyaName[0] );
      PP_Put( pp , ID_PARA_oyasex , PTL_SEX_MALE );
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y )
    {
      static u16 oyaName[6] = {L'��',L'��',L'��',L'��',L'��',0xFFFF};
      RTCDate now_date;
      GFL_RTC_GetDate( &now_date );
      // �莝���O�Ԗڂɕߊl�ꏊWF�̖ړI�|�P������ݒ�
      PP_Setup( pp, p_wk->wf_poke_catch_monsno, 10, 100 );
      PP_Put( pp, ID_PARA_birth_place, MAPNAME_WC10 );
      PP_Put( pp, ID_PARA_birth_year, now_date.year+1 );
      PP_Put( pp, ID_PARA_birth_month, now_date.month );
      PP_Put( pp, ID_PARA_birth_day, now_date.day );

      PP_Put( pp , ID_PARA_oyaname_raw , (u32)&oyaName[0] );
      PP_Put( pp , ID_PARA_oyasex , PTL_SEX_MALE );
    }
  }
}

static void DEBWIN_Draw_WFPokeGet( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUGWIN_ITEM_SetName( item , "WF PokeGet" );
}

static void DEBWIN_Update_WFBCBlockCheck( void* userWork , DEBUGWIN_ITEM* item )
{
  s32 min;
  s32 max;
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_CORE* p_core = GAMEDATA_GetMyWFBCCoreData( p_gdata );

  if( p_core->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
  {
    min = LAND_DATA_PATCH_BC_BUILD_01;
    max = LAND_DATA_PATCH_WF_TREE_01;
  }
  else
  {
    min = LAND_DATA_PATCH_WF_TREE_01;
    max = LAND_DATA_PATCH_MAX;
  }
  
  if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A) )
  {
    if(DEBUG_FIELD_WFBC_MAKE_flag){
      DEBUG_FIELD_WFBC_MAKE_flag = FALSE;
    }else{
      DEBUG_FIELD_WFBC_MAKE_flag = TRUE;
    }
    DEBUGWIN_RefreshScreen();
  }

  if(DEBUG_FIELD_WFBC_MAKE_flag)
  {
    if( (DEBUG_FIELD_WFBC_MAKE_score < min) || (DEBUG_FIELD_WFBC_MAKE_score > max) )
    {
      DEBUG_FIELD_WFBC_MAKE_score = min;
    }
    
    if( GFL_UI_KEY_GetTrg() & (PAD_KEY_LEFT) )
    {
      DEBUG_FIELD_WFBC_MAKE_score --;
      if( DEBUG_FIELD_WFBC_MAKE_score < min )
      {
        DEBUG_FIELD_WFBC_MAKE_score += (max - min);
      }
      DEBUGWIN_RefreshScreen();
    }

    if( GFL_UI_KEY_GetTrg() & (PAD_KEY_RIGHT) )
    {
      DEBUG_FIELD_WFBC_MAKE_score ++;
      if( DEBUG_FIELD_WFBC_MAKE_score >= max )
      {
        DEBUG_FIELD_WFBC_MAKE_score -= (max - min);
      }
      DEBUGWIN_RefreshScreen();
    }
  }
}

static void DEBWIN_Draw_WFBCBlockCheck( void* userWork , DEBUGWIN_ITEM* item )
{
  if( DEBUG_FIELD_WFBC_MAKE_flag )
  {
    DEBUGWIN_ITEM_SetNameV( item , "BlockCheckON next=[%d]", DEBUG_FIELD_WFBC_MAKE_score );
  }
  else
  {
    DEBUGWIN_ITEM_SetName( item , "BlockCheckOFF" );
  }
}

static void DEBWIN_Update_WFBCPeopleCheck( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_CORE* p_wk = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
    // �ݒ�
    FIELD_WFBC_CORE_PEOPLE* p_array;
    
    if( DEBUG_WFBCPeople_mode == 0 ){
      // �\
      p_array = p_wk->people;
    }else{
      // ��
      p_array = p_wk->back_people;
    }

    // �����Ȃ�������ʒu���琶��
    if( p_array[ DEBUG_WFBCPeople_index ].data_in == FALSE ){

      p_array[ DEBUG_WFBCPeople_index ].npc_id  = DEBUG_WFBCPeople_npc_id;
      p_array[ DEBUG_WFBCPeople_index ].mood  = 50;
      p_array[ DEBUG_WFBCPeople_index ].one_day_msk  = FIELD_WFBC_ONEDAY_MSK_INIT;
      p_array[ DEBUG_WFBCPeople_index ].data_in = TRUE;

      if( p_wk->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
      {
        FIELD_WFBC_CORE_PEOPLE_SetParentData( &p_array[ DEBUG_WFBCPeople_index ], GAMEDATA_GetMyStatus(p_gdata) );
      }
      
    }else{

      p_array[ DEBUG_WFBCPeople_index ].npc_id = DEBUG_WFBCPeople_npc_id;

    }

    DEBUGWIN_RefreshScreen();
    
  }else{

    // �\���ݒ�
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L ){
      DEBUG_WFBCPeople_mode ^= 1;
      DEBUGWIN_RefreshScreen();
    }

    // �C���f�b�N�X�ݒ�
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ){
      if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT ){
        DEBUG_WFBCPeople_index = (DEBUG_WFBCPeople_index + 1) % FIELD_WFBC_PEOPLE_MAX;
        DEBUGWIN_RefreshScreen();
      }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT ){
        DEBUG_WFBCPeople_index --;
        if( DEBUG_WFBCPeople_index < 0 ){
          DEBUG_WFBCPeople_index += FIELD_WFBC_PEOPLE_MAX;
        }
        DEBUGWIN_RefreshScreen();
      }
    }
    // NPC_ID
    else{
      if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT ){
        DEBUG_WFBCPeople_npc_id = (DEBUG_WFBCPeople_npc_id + 1) % FIELD_WFBC_NPCID_MAX;
        DEBUGWIN_RefreshScreen();
      }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT ){
        DEBUG_WFBCPeople_npc_id --;
        if( DEBUG_WFBCPeople_npc_id < 0 ){
          DEBUG_WFBCPeople_npc_id += FIELD_WFBC_NPCID_MAX;
        }
        DEBUGWIN_RefreshScreen();
      }
    }
  }
}

static void DEBWIN_Draw_WFBCPeopleCheck( void* userWork , DEBUGWIN_ITEM* item )
{
  if( DEBUG_WFBCPeople_mode == 0 ){
    DEBUGWIN_ITEM_SetNameV( item , "N idx %d npc %d", DEBUG_WFBCPeople_index, DEBUG_WFBCPeople_npc_id );
  }else{
    DEBUGWIN_ITEM_SetNameV( item , "B idx %d npc %d", DEBUG_WFBCPeople_index, DEBUG_WFBCPeople_npc_id );
  }
}
  


#endif //PM_DEBUG


