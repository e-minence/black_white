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

#include "field_wfbc.h"

#include "eventdata_local.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�l����{���
//=====================================
static const MMDL_HEADER sc_DEFAULT_HEADER = 
{
	0,		///<����ID
	0,	  ///<�\������OBJ�R�[�h
	0,	  ///<����R�[�h
	0,	  ///<�C�x���g�^�C�v
	0,	  ///<�C�x���g�t���O
	0,	  ///<�C�x���gID
	0,		///<�w�����
	0,		///<�w��p�����^ 0
	0,		///<�w��p�����^ 1
	0,		///<�w��p�����^ 2
	0,		///<X�����ړ�����
	0,		///<Z�����ړ�����
};


//-------------------------------------
///	�l���z�u���
//=====================================
typedef struct
{
  u16 gx;
  u16 gz;
} WFBC_PEOPLE_POS;
static const WFBC_PEOPLE_POS  sc_WFBC_PEOPLE_POS[ FIELD_WFBC_CORE_TYPE_MAX ][ FIELD_WFBC_PEOPLE_MAX ] = 
{
  // BLACK CITY
  {
    { 23,12 },
    { 27,32 },
    { 31,24 },
    { 35,29 },
    { 39,17 },
    { 43,16 },
    { 47,28 },
    { 51,12 },
    { 55,20 },
    { 59,30 },

    { 24,13 },
    { 28,33 },
    { 32,25 },
    { 36,30 },
    { 40,18 },
    { 44,17 },
    { 48,29 },
    { 52,13 },
    { 56,21 },
    { 60,31 },
  },
  

  // White Forest
  {
    { 23,12 },
    { 27,32 },
    { 31,24 },
    { 35,29 },
    { 39,17 },
    { 43,16 },
    { 47,28 },
    { 51,12 },
    { 55,20 },
    { 59,30 },

    { 24,13 },
    { 28,33 },
    { 32,25 },
    { 36,30 },
    { 40,18 },
    { 44,17 },
    { 48,29 },
    { 52,13 },
    { 56,21 },
    { 60,31 },
  },
};


//-------------------------------------
///	�u���b�N�z�u���
//=====================================
#define FIELD_WFBC_BLOCK_SIZE_X     (8)
#define FIELD_WFBC_BLOCK_SIZE_Z     (8)
#define FIELD_WFBC_BLOCK_PATCH_MAX  (20)
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
#define FIELD_WFBC_EVENT_WF  (NARC_eventdata_wf_block_event_bin)
#define FIELD_WFBC_EVENT_BC  (NARC_eventdata_bc_block_event_bin)


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
///	�u���b�N�z�u���
//=====================================
typedef struct 
{
  // �e�O���b�h�ʒu�̃u���b�N���
  u8 block_data[FIELD_WFBC_BLOCK_SIZE_X][FIELD_WFBC_BLOCK_SIZE_Z];

  // �p�b�`���
  FIELD_WFBC_BLOCK_PATCH patch[]; 
} FIELD_WFBC_BLOCK_DATA;


#if 0
//-------------------------------------
///	�C�x���g�N�����
//=====================================
typedef struct 
{
  // ����
  u32 if_para[];
} FIELD_WFBC_EVENT_IF;
#endif


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
static void WFBC_PEOPLE_SetUp( FIELD_WFBC_PEOPLE* p_people, const FIELD_WFBC_CORE_PEOPLE* cp_core, const WFBC_PEOPLE_POS* cp_pos, const FIELD_WFBC_PEOPLE_DATA* cp_data );
static void WFBC_PEOPLE_GetCore( const FIELD_WFBC_PEOPLE* cp_people, FIELD_WFBC_CORE_PEOPLE* p_core );


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
  return WFBC_GetConstPeople( cp_wk, npc_id );
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
 *	@brief  ���샂�f�������擾
 *
 *	@param	cp_wk     ���[�N
 *	@param	heapID    �q�[�vID
 *
 *	@retval ���샂�f���w�b�_�[�e�[�u��
 *	@retval NULL  �l���͂��Ȃ�
 */
//-----------------------------------------------------------------------------
MMDL_HEADER* FIELD_WFBC_MMDLHeaderCreateHeapLo( const FIELD_WFBC* cp_wk, HEAPID heapID )
{
  u32 count;
  u32 num;
  MMDL_HEADER* p_buff;
  int i;
  
  GF_ASSERT( cp_wk );

  num = WFBC_GetPeopleNum( cp_wk );

  if( num == 0 )
  {
    return NULL;
  }

  p_buff = GFL_HEAP_AllocClearMemoryLo( heapID, sizeof(MMDL_HEADER) * num );
  
  count = 0;
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( cp_wk->p_people[i].status == FIELD_WFBC_PEOPLE_STATUS_NORMAL )
    {
      // �l���Ƃ��ēo�^
      p_buff[count] = sc_DEFAULT_HEADER;

      p_buff[count].id          = FIELD_WFBC_CORE_PEOPLE_GetNpcID(&cp_wk->p_people[i].people_local);
      p_buff[count].obj_code    = cp_wk->p_people[i].people_data.objid;
      p_buff[count].move_code   = MV_RND;
      if( cp_wk->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY ){
        p_buff[count].event_id    = cp_wk->p_people[i].people_data.script_wf;
      }else{
        p_buff[count].event_id    = cp_wk->p_people[i].people_data.script_bc;
      }
      
      MMDLHEADER_SetGridPos( &p_buff[count], cp_wk->p_people[i].gx, cp_wk->p_people[i].gz, 0 );

      count ++;
    }
  }

  return p_buff;
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
void FILED_WFBC_EventDataOverwrite( const FIELD_WFBC* cp_wk, EVENTDATA_SYSTEM* p_evdata, HEAPID heapID )
{
  u16 bg_num, npc_num, connect_num, pos_num;

  bg_num      = EVENTDATA_GetBgEventNum(p_evdata);
  npc_num     = EVENTDATA_GetNpcEventNum(p_evdata);
  connect_num = EVENTDATA_GetConnectEventNum(p_evdata);
  pos_num     = EVENTDATA_GetPosEventNum(p_evdata);

  bg_num      ++;
  npc_num     ++;
  connect_num ++;
  pos_num     ++;
  
  EVENTDATA_SYS_ReloadEventDataEx( p_evdata, bg_num, npc_num, connect_num, pos_num );
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

  GF_ASSERT( p_wk );
  // �󂢂Ă��郏�[�N���擾
  index = WFBC_GetClearPeople( p_wk );

  // NPC���ǂݍ���
  FIELD_WFBC_PEOPLE_DATA_Load( p_wk->p_loader, cp_core->npc_id );
  
  // �l�����Z�b�g�A�b�v
  WFBC_PEOPLE_SetUp( &p_wk->p_people[index], cp_core, &sc_WFBC_PEOPLE_POS[ p_wk->type ][ index ], FIELD_WFBC_PEOPLE_DATA_GetData( p_wk->p_loader ) );

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
    GFL_HEAP_FreeMemory( p_wk->p_block );
    GFL_ARC_CloseDataHandle( p_wk->p_handle );
    FIELD_WFBC_PEOPLE_DATA_Delete( p_wk->p_loader );
    GFL_HEAP_FreeMemory( p_wk->p_people );

    p_wk->p_loader = NULL;
    p_wk->p_people = NULL;
    p_wk->p_handle = NULL;
    p_wk->p_block = NULL;
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
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    // �f�[�^����������ݒ�
    if( mapmode == MAPMODE_NORMAL )
    {
      if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_buff->people[i] ) )
      {
        // NPC���ǂݍ���
        FIELD_WFBC_PEOPLE_DATA_Load( p_wk->p_loader, p_buff->people[i].npc_id );
        WFBC_PEOPLE_SetUp( &p_wk->p_people[i], &p_buff->people[i], &sc_WFBC_PEOPLE_POS[ p_wk->type ][ i ], FIELD_WFBC_PEOPLE_DATA_GetData( p_wk->p_loader ) );
      }
    }
    else
    {
      if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_buff->back_people[i] ) )
      {
        // NPC���ǂݍ���
        FIELD_WFBC_PEOPLE_DATA_Load( p_wk->p_loader, p_buff->back_people[i].npc_id );
        WFBC_PEOPLE_SetUp( &p_wk->p_people[i], &p_buff->back_people[i], &sc_WFBC_PEOPLE_POS[ p_wk->type ][ i ], FIELD_WFBC_PEOPLE_DATA_GetData( p_wk->p_loader ) );
      }
    }
  }

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
static void WFBC_PEOPLE_SetUp( FIELD_WFBC_PEOPLE* p_people, const FIELD_WFBC_CORE_PEOPLE* cp_core, const WFBC_PEOPLE_POS* cp_pos, const FIELD_WFBC_PEOPLE_DATA* cp_data )
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





//�f�o�b�O����
#ifdef PM_DEBUG
static void DEBWIN_Update_CityLevel( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_CityLevel( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_CityType( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_CityType( void* userWork , DEBUGWIN_ITEM* item );

void FIELD_FUNC_RANDOM_GENERATE_InitDebug( HEAPID heapId, FIELD_WFBC_CORE* p_core )
{
  DEBUGWIN_AddGroupToTop( 10 , "RandomMap" , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_CityLevel ,DEBWIN_Draw_CityLevel , 
                             p_core , 10 , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_CityType ,DEBWIN_Draw_CityType , 
                             p_core , 10 , heapId );
}

void FIELD_FUNC_RANDOM_GENERATE_TermDebug( void )
{
  DEBUGWIN_RemoveGroup( 10 );
}

static void DEBWIN_Update_CityLevel( void* userWork , DEBUGWIN_ITEM* item )
{
  FIELD_WFBC_CORE* p_wk = userWork;
  int  i;

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    // ���₷
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( p_wk->people[i].data_in == FALSE )
      {
        p_wk->people[i].data_in = TRUE;
        p_wk->people[i].npc_id  = 0;
        p_wk->people[i].mood  = 50;
        p_wk->people[i].one_day_msk  = FIELD_WFBC_ONEDAY_MSK_INIT;
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
    DEBUGWIN_RefreshScreen();
  }
}

static void DEBWIN_Draw_CityLevel( void* userWork , DEBUGWIN_ITEM* item )
{
  FIELD_WFBC_CORE* p_wk = userWork;
  u16 level = FIELD_WFBC_CORE_GetPeopleNum( p_wk );
  DEBUGWIN_ITEM_SetNameV( item , "Level[%d]",level );
}

static void DEBWIN_Update_CityType( void* userWork , DEBUGWIN_ITEM* item )
{
  FIELD_WFBC_CORE* p_wk = userWork;
  u8 type = p_wk->type;

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT || 
      GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT  ||
      GFL_UI_KEY_GetRepeat() & PAD_BUTTON_A )
  {
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
  }
}

static void DEBWIN_Draw_CityType( void* userWork , DEBUGWIN_ITEM* item )
{
  FIELD_WFBC_CORE* p_wk = userWork;
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
#endif
