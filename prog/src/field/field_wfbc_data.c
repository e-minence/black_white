//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_wfbc_data.c
 *	@brief  White Forest  Black City ��{�f�[�^
 *	@author	tomoya takahashi
 *	@date		2009.11.10
 *
 *	���W���[�����FFIELD_WFBC_CORE
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "arc/arc_def.h"
#include "arc/field_wfbc_data.naix"

#include "system/gfl_use.h"

#include "field/field_wfbc_data.h"
#include "field/zonedata.h"

#include "field_status_local.h"

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


//-----------------------------------------------------------------------------
/**
 day*					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static u32 WFBC_CORE_GetNotUseNpcID( const FIELD_WFBC_CORE* cp_wk, int idx );
static u32 WFBC_CORE_GetNotUseNpcIndex( const FIELD_WFBC_CORE* cp_wk, int idx );
static BOOL WFBC_CORE_IsUseNpc( const FIELD_WFBC_CORE* cp_wk, int npc_id );

// �l�f�[�^���\�[�g
static void WFBC_CORE_SortPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array, HEAPID heapID );
// �l�f�[�^���߂�
static void WFBC_CORE_PackPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array );
// �l�f�[�^�������o���ݒ�
static void WFBC_CORE_PushPeople( FIELD_WFBC_CORE* p_wk, const FIELD_WFBC_CORE_PEOPLE* cp_people );
// �z�񂪂����ς����`�F�b�N
static BOOL WFBC_CORE_IsPeopleArrayFull( const FIELD_WFBC_CORE_PEOPLE* cp_array );
// �l������
static void WFBC_CORE_PushPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array, const FIELD_WFBC_CORE_PEOPLE* cp_people );

// �󂢂Ă���l�����[�N���擾
static FIELD_WFBC_CORE_PEOPLE* WFBC_CORE_GetClearPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array );


//-------------------------------------
///	�l�����
//=====================================
static void WFBC_CORE_SetUpPeople( FIELD_WFBC_CORE_PEOPLE* p_people, u8 npc_id, const MYSTATUS* cp_mystatus, u16 mood );
static BOOL WFBC_CORE_People_IsGoBack( const FIELD_WFBC_CORE_PEOPLE* cp_people );
static BOOL WFBC_CORE_People_AddMood( FIELD_WFBC_CORE_PEOPLE* p_people, int add );



//----------------------------------------------------------------------------
/**
 *	@brief  ZONEDATA��WFBC�̐ݒ���s��
 *
 *	@param	cp_wk   ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_SetUpZoneData( const FIELD_WFBC_CORE* cp_wk )
{
  if( cp_wk->type == FIELD_WFBC_CORE_TYPE_WHITE_FOREST )
  {
    ZONEDATA_SetChangeZoneID( ZONEDATA_CHANGE_BC_WF_ID, TRUE );
  }
  if( cp_wk->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
  {
    ZONEDATA_SetChangeZoneID( ZONEDATA_CHANGE_BC_WF_ID, FALSE );
  }
}


//-------------------------------------
///	FIELD_WFBC_CORE�p�֐�
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  WFBC���[�N�̃N���A
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_Clear( FIELD_WFBC_CORE* p_wk )
{
  int i;

  GF_ASSERT( p_wk );
  
  p_wk->data_in = FALSE;
  p_wk->type    = FIELD_WFBC_CORE_TYPE_MAX;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[i] );
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->back_people[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief �m�[�}���}�b�v�̏����N���A
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_ClearNormal( FIELD_WFBC_CORE* p_wk )
{
  int i;

  GF_ASSERT( p_wk );
  
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����E�̐l�����N���A
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_ClearBack( FIELD_WFBC_CORE* p_wk )
{
  int i;

  GF_ASSERT( p_wk );
  
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->back_people[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�̏���ݒ肷��
 *
 *	@param	p_wk          ���[�N
 *	@param  cp_mystatus   �܂��X�e�[�^�X
 *	@param  heapID        �q�[�vID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_SetUp( FIELD_WFBC_CORE* p_wk, const MYSTATUS* cp_mystatus, HEAPID heapID )
{
  int i;
  int pos_idx;
  int npc_idx;
  int set_npc_idx;
  FIELD_WFBC_CORE_PEOPLE* p_people;
  FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader;
  const FIELD_WFBC_PEOPLE_DATA* cp_data;
  u32 mood;
  GF_ASSERT( p_wk );
  
  p_wk->data_in = TRUE;
#if PM_VERSION == VERSION_BLACK
  p_wk->type    = FIELD_WFBC_CORE_TYPE_BLACK_CITY;
#else   // PM_VERSION == VERSION_BLACK
  p_wk->type    = FIELD_WFBC_CORE_TYPE_WHITE_FOREST; 
#endif  // PM_VERSION == VERSION_BLACK

  p_wk->random_no = MyStatus_GetID( cp_mystatus );

  p_loader = FIELD_WFBC_PEOPLE_DATA_Create( 0, GFL_HEAP_LOWID(heapID) );
  cp_data = FIELD_WFBC_PEOPLE_DATA_GetData( p_loader );

  // �l��ݒ肷��
  for( i=0; i<FIELD_WFBC_INIT_PEOPLE_NUM; i++ )
  {
    // �ݒ�ʒu�Ɛl��I��
    npc_idx = GFUser_GetPublicRand( FIELD_WFBC_NPCID_MAX - i );

    p_people  = WFBC_CORE_GetClearPeopleArray( p_wk->people );
    set_npc_idx = WFBC_CORE_GetNotUseNpcID( p_wk, npc_idx );

    // NPC���ǂݍ���
    FIELD_WFBC_PEOPLE_DATA_Load( p_loader, set_npc_idx );
    
    if( p_wk->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY ){
      mood = cp_data->mood_bc;
    }else{
      mood = cp_data->mood_wf;
    }
    WFBC_CORE_SetUpPeople( p_people, set_npc_idx, cp_mystatus, mood );
  }

  FIELD_WFBC_PEOPLE_DATA_Delete( p_loader );
}


//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�f�[�^�̐������`�F�b�N
 *
 *	@param	cp_wk         ���[�N
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �s��
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_IsConfomity( const FIELD_WFBC_CORE* cp_wk )
{
  int i;

  GF_ASSERT( cp_wk );
  
  if( (cp_wk->data_in == TRUE) && (cp_wk->type >= FIELD_WFBC_CORE_TYPE_MAX) )
  {
    return FALSE;
  }
  
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsConfomity( &cp_wk->people[i] ) == FALSE )
    {
      return FALSE;
    }
  }

  // OK
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^����  �s���f�[�^�̏ꍇ�́A����ȏ��ɏ��������܂��B
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_Management( FIELD_WFBC_CORE* p_wk )
{
  int i;

  GF_ASSERT( p_wk );
  
  // �ꉞ�f�[�^���Ȃ���Ԃ��Ǘ����Ă���
  if( (p_wk->data_in == FALSE) && (p_wk->type != FIELD_WFBC_CORE_TYPE_MAX) )
  {
    p_wk->type = FIELD_WFBC_CORE_TYPE_MAX;
  }

  // �X�^�C�v�̒���
  if( (p_wk->data_in == TRUE) && (p_wk->type >= FIELD_WFBC_CORE_TYPE_MAX) )
  {
    // @TODO �������ʐM�����ROM�R�[�h�ɍ��킹��ׂ�
    p_wk->type = FIELD_WFBC_CORE_TYPE_BLACK_CITY;
  }

  // �l�����̒���
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    FIELD_WFBC_CORE_PEOPLE_Management( &p_wk->people[i] );
  }
}

// �f�[�^�̗L��   �s���f�[�^�̏ꍇ�AFALSE��Ԃ��܂��B
//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�̗L���`�F�b�N  �s���f�[�^�̏ꍇ�AFALSE��Ԃ��܂��B
 *
 *	@param	cp_wk   ���[�N
 *
 *	@retval TRUE  �f�[�^����
 *	@retval FALSE �f�[�^�Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_IsInData( const FIELD_WFBC_CORE* cp_wk )
{
  GF_ASSERT( cp_wk );
  
  if( !cp_wk->data_in )
  {
    return FALSE;
  }

  if( FIELD_WFBC_CORE_IsConfomity( cp_wk ) == FALSE )
  {
    return FALSE;
  }

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l�̐���Ԃ�
 *
 *	@param	cp_wk   ���[�N
 *	@param  mapmode MAPMODE �i�����ʏ킩
 *
 *	@return �l�̐�
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_CORE_GetPeopleNum( const FIELD_WFBC_CORE* cp_wk, u32 mapmode )
{
  int i;
  int num;
  const FIELD_WFBC_CORE_PEOPLE* cp_people_array;
  GF_ASSERT( cp_wk );

  if( mapmode == MAPMODE_NORMAL )
  { 
    cp_people_array = cp_wk->people;
  }
  else
  {
    cp_people_array = cp_wk->back_people;
  }

  num = 0;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_people_array[i] ) )
    {
      num ++;
    }
  }
  return num;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �@���l�Ń\�[�g����
 *
 *	@param	p_wk    ���[�N
 *	@param  heapID  �q�[�vID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_SortData( FIELD_WFBC_CORE* p_wk, HEAPID heapID )
{
  WFBC_CORE_SortPeopleArray( p_wk->people, heapID );
  WFBC_CORE_SortPeopleArray( p_wk->back_people, heapID );
}


//----------------------------------------------------------------------------
/**
 *	@brief  1���̐؂�ւ��Ǘ�
 *
 *	@param	p_wk      ���[�N
 *	@param  diff_day  �o�ߓ���
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_CalcOneDataStart( FIELD_WFBC_CORE* p_wk, s32 diff_day )
{
  int i;
  BOOL result;

  // ���ɂ������������Ȃ����`�F�b�N
  if( diff_day <= 0 )
  {
    // ��������
    GF_ASSERT(0);
    diff_day = 1;
  }

  if( diff_day > FIELD_WFBC_MOOD_SUB_DAY_MAX )
  {
    diff_day = FIELD_WFBC_MOOD_SUB_DAY_MAX;
  }
  
  // �S����Mood��FIELD_WFBC_MOOD_SUB���炷
  // �}�X�N�̃N���A
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_wk->people[i] ) )
    {
      // �o�g���A�e����Z���\�Ƃ���B
      p_wk->people[i].one_day_msk = FIELD_WFBC_ONEDAY_MSK_INIT;
      
      result = WFBC_CORE_People_AddMood( &p_wk->people[i], FIELD_WFBC_MOOD_SUB * diff_day );
      if( result )
      {
        // �w�ʐ��E���̃��[�N�Ɋi�[��N���A
        // ���̏��𗠐��E�ɓn���B
        WFBC_CORE_PushPeopleArray( p_wk->back_people, &p_wk->people[i] );
        FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[i] );
      }
    }
  }
  
  
  // �󂢂Ă��郏�[�N���߂�
  WFBC_CORE_PackPeopleArray( p_wk->people );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�ɓ��������̋@���l�v�Z
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_CalcMoodInTown( FIELD_WFBC_CORE* p_wk )
{
  int i;
  
  // �S����Mood��FIELD_WFBC_MOOD_SUB���炷
  // �}�X�N�̃N���A
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_wk->people[i] ) )
    {
      // ���Z�`�F�b�N
      if( FIELD_WFBC_ONEDAY_MSK_CHECK( p_wk->people[i].one_day_msk, FIELD_WFBC_ONEDAY_MSK_INTOWN ) )
      {
        WFBC_CORE_People_AddMood( &p_wk->people[i], FIELD_WFBC_MOOD_ADD_INTOWN );

        // OFF
        p_wk->people[i].one_day_msk = FIELD_WFBC_ONEDAY_MSK_OFF( p_wk->people[i].one_day_msk, FIELD_WFBC_ONEDAY_MSK_INTOWN );
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l�𑫂��@�����o����
 *
 *	@param	p_wk      ���[�N  
 *	@param	cp_people �i�[����l
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_AddPeople( FIELD_WFBC_CORE* p_wk, const FIELD_WFBC_CORE_PEOPLE* cp_people )
{
  WFBC_CORE_PushPeople( p_wk, cp_people );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l��T��  NPCID�̐l
 *
 *	@param	p_wk      ���[�N
 *	@param	npc_id    NPC�@ID
 *
 *	@retval �l���[�N
 *	@retval NULL    ���Ȃ�
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_CORE_PEOPLE* FIELD_WFBC_CORE_GetNpcIDPeople( FIELD_WFBC_CORE* p_wk, u32 npc_id )
{
  int i;

  GF_ASSERT( p_wk );
  GF_ASSERT( npc_id < FIELD_WFBC_NPCID_MAX );

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_wk->people[i] ) )
    {
      if( p_wk->people[i].npc_id == npc_id )
      {
        return &p_wk->people[i];
      }
    }

    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_wk->back_people[i] ) )
    {
      if( p_wk->back_people[i].npc_id == npc_id )
      {
        return &p_wk->back_people[i];
      }
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBC��񂩂�AMMDL�w�b�_�[�𐶐�����
 *
 *	@param	cp_wk     ���[�N
 *	@param	heapID    �q�[�vID
 *
 *	@retval ���샂�f���w�b�_�[�e�[�u��
 *	@retval NULL  �l���͂��Ȃ�
 */
//-----------------------------------------------------------------------------
MMDL_HEADER* FIELD_WFBC_CORE_MMDLHeaderCreateHeapLo( const FIELD_WFBC_CORE* cp_wk, u32 mapmode, HEAPID heapID )
{
  u32 count;
  u32 num;
  MMDL_HEADER* p_buff;
  int i;
  FIELD_WFBC_PEOPLE_DATA_LOAD* p_people_loader;
  const FIELD_WFBC_PEOPLE_DATA* cp_people_data;
  const FIELD_WFBC_CORE_PEOPLE* cp_people_array;
  FIELD_WFBC_CORE_PEOPLE_POS* p_pos;
  
  GF_ASSERT( cp_wk );

  heapID = GFL_HEAP_LOWID( heapID );

  num = FIELD_WFBC_CORE_GetPeopleNum( cp_wk, mapmode );

  if( num == 0 )
  {
    return NULL;
  }

  if( mapmode == MAPMODE_NORMAL )
  {
    cp_people_array = cp_wk->people;
  }
  else
  {
    cp_people_array = cp_wk->back_people;
  }


  p_buff = GFL_HEAP_AllocClearMemory( heapID, sizeof(MMDL_HEADER) * num );

  // ���[�_�[����
  p_people_loader = FIELD_WFBC_PEOPLE_DATA_Create( 0, heapID );

  // �ʒu����ǂݍ���
  p_pos = FIELD_WFBC_PEOPLE_POS_Create( p_people_loader, cp_wk->type, heapID );
  
  count = 0;
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_people_array[i] ) )
    {
      // �l���Ƃ��ēo�^
      p_buff[count] = sc_DEFAULT_HEADER;

      FIELD_WFBC_PEOPLE_DATA_Load( p_people_loader, FIELD_WFBC_CORE_PEOPLE_GetNpcID( &cp_people_array[i] ) );
      cp_people_data = FIELD_WFBC_PEOPLE_DATA_GetData( p_people_loader );

      p_buff[count].id          = FIELD_WFBC_CORE_PEOPLE_GetNpcID( &cp_people_array[i] );
      p_buff[count].obj_code    = cp_people_data->objid;
      p_buff[count].move_code   = MV_RND;
      if( cp_wk->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY ){
        p_buff[count].event_id    = cp_people_data->script_wf;
      }else{
        p_buff[count].event_id    = cp_people_data->script_bc;
      }

      MMDLHEADER_SetGridPos( &p_buff[count], p_pos[i].gx, p_pos[i].gz, 0 );
      

      count ++;
    }
  }


  FIELD_WFBC_PEOPLE_POS_Delete( p_pos );

  GFL_HEAP_FreeMemory( p_people_loader );

  return p_buff;
}





//-------------------------------------
///	FIELD_WFBC_CORE_PEOPLE�p�֐�
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  �l�����[�N�̃N���A
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_Clear( FIELD_WFBC_CORE_PEOPLE* p_wk )
{
  GF_ASSERT( p_wk );

  p_wk->data_in = FALSE;
  p_wk->npc_id  = 0;
  p_wk->mood    = 0;
  p_wk->one_day_msk  = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���̐������`�F�b�N
 *
 *	@param	cp_wk   ���[�N
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �s��
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_PEOPLE_IsConfomity( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^����  �s���f�[�^�̏ꍇ�́A����ȏ��ɏ��������܂��B
 *
 *	@param	p_wk  �l�����[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_Management( FIELD_WFBC_CORE_PEOPLE* p_wk )
{
  GF_ASSERT( p_wk );

  //
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�̗L��   �s���f�[�^�̏ꍇ�AFALSE��Ԃ��܂��B
 *
 *	@param	cp_wk   �l�����[�N
 *
 *	@retval TRUE    �f�[�^����
 *	@retval FALSE   �f�[�^�Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_PEOPLE_IsInData( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );
  
  if( !cp_wk->data_in )
  {
    return FALSE;
  }
  if( FIELD_WFBC_CORE_PEOPLE_IsConfomity( cp_wk ) == FALSE )
  {
    return FALSE;
  }

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �b���������Ƃ��̌v�Z
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_CalcTalk( FIELD_WFBC_CORE_PEOPLE* p_wk )
{
  if( FIELD_WFBC_ONEDAY_MSK_CHECK( p_wk->one_day_msk, FIELD_WFBC_ONEDAY_MSK_TALK ) )
  {
    // �v�Z
    WFBC_CORE_People_AddMood( p_wk, FIELD_WFBC_MOOD_ADD_TALK );

    // OFF
    p_wk->one_day_msk = FIELD_WFBC_ONEDAY_MSK_OFF( p_wk->one_day_msk, FIELD_WFBC_ONEDAY_MSK_TALK );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���������Ƃ�ݒ�
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_SetBattle( FIELD_WFBC_CORE_PEOPLE* p_wk )
{
  // OFF
  p_wk->one_day_msk = FIELD_WFBC_ONEDAY_MSK_OFF( p_wk->one_day_msk, FIELD_WFBC_ONEDAY_MSK_BATTLE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�g�����\�Ȃ̂��`�F�b�N
 *
 *	@param	cp_wk   ���[�N
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_PEOPLE_IsBattle( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  // OFF
  if( FIELD_WFBC_ONEDAY_MSK_CHECK( cp_wk->one_day_msk, FIELD_WFBC_ONEDAY_MSK_BATTLE ) )
  {
    return TRUE;
  }
  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �e�f�[�^�̐ݒ�
 *
 *	@param	p_wk        ���[�N
 *	@param  cp_mystatus �e�̏��
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_SetParentData( FIELD_WFBC_CORE_PEOPLE* p_wk, const MYSTATUS* cp_mystatus )
{
  GF_ASSERT( p_wk );
  GF_ASSERT( cp_mystatus );

  p_wk->parent_id = MyStatus_GetID( cp_mystatus );
  GFL_STD_MemCopy( MyStatus_GetMyName( cp_mystatus ), p_wk->parent, sizeof(STRCODE) * (PERSON_NAME_SIZE + EOM_SIZE) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�̖��O���擾
 *
 *	@param	cp_wk   ���[�N
 *	@param	p_buff  �o�b�t�@
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_GetParentName( const FIELD_WFBC_CORE_PEOPLE* cp_wk, STRCODE* p_buff )
{
  GF_ASSERT( cp_wk );
  GFL_STD_MemCopy( cp_wk->parent, p_buff, sizeof(STRCODE) * (PERSON_NAME_SIZE + EOM_SIZE) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�̃v���C���[ID���擾
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return �v���C���[ID
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_CORE_PEOPLE_GetParentID( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->parent_id;
}

//----------------------------------------------------------------------------
/**
 *	@brief  NPCID���擾
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return NPCID
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_CORE_PEOPLE_GetNpcID( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->npc_id;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �@���l���A�A��čs����Ă��܂������ɂȂ�l���H
 *
 *	@param	cp_wk   ���[�N
 *
 *	@retval TRUE    �͂�
 *	@retval FALSE   ������
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_PEOPLE_IsMoodTakes( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );
  if( cp_wk->mood > FIELD_WFBC_MOOD_TAKES )
  {
    return FALSE;
  }
  return TRUE;
}





//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					WFBC�l�����ւ̃A�N�Z�X
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�l����񃍁[�_�[
//=====================================
struct _FIELD_WFBC_PEOPLE_DATA_LOAD {
  ARCHANDLE* p_handle;
  u32 npc_id;
  FIELD_WFBC_PEOPLE_DATA buff;
};

//----------------------------------------------------------------------------
/**
 *	@brief  �l����񃍁[�_�[����
 *
 *	@param	npc_id    NPCID
 *	@param	heapID 
 *
 *	@return ���[�_�[
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_PEOPLE_DATA_LOAD* FIELD_WFBC_PEOPLE_DATA_Create( u32 npc_id, HEAPID heapID )
{
  FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader;

  p_loader = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_WFBC_PEOPLE_DATA_LOAD) );

  p_loader->p_handle = GFL_ARC_OpenDataHandle( ARCID_WFBC_PEOPLE, heapID );

  // �ǂݍ���
  GF_ASSERT( npc_id < FIELD_WFBC_NPCID_MAX );
  GFL_ARC_LoadDataByHandle( p_loader->p_handle, npc_id, &p_loader->buff );
  p_loader->npc_id = npc_id;
  
  return p_loader;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l����񃍁[�_�[�j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_PEOPLE_DATA_Delete( FIELD_WFBC_PEOPLE_DATA_LOAD* p_wk )
{
  GFL_ARC_CloseDataHandle( p_wk->p_handle );
  GFL_HEAP_FreeMemory( p_wk );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �l�����ēǂ݂���
 *
 *	@param	p_wk      ���[�N
 *	@param	npc_id    NPCID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_PEOPLE_DATA_Load( FIELD_WFBC_PEOPLE_DATA_LOAD* p_wk, u32 npc_id )
{
  GF_ASSERT( npc_id < FIELD_WFBC_NPCID_MAX );
  GFL_ARC_LoadDataByHandle( p_wk->p_handle, npc_id, &p_wk->buff );
  p_wk->npc_id = npc_id;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �l�����  �擾
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return �f�[�^�o�b�t�@
 */
//-----------------------------------------------------------------------------
const FIELD_WFBC_PEOPLE_DATA* FIELD_WFBC_PEOPLE_DATA_GetData( const FIELD_WFBC_PEOPLE_DATA_LOAD* cp_wk )
{
  GF_ASSERT( cp_wk );
  return &cp_wk->buff;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l�����  �ǂݍ��܂�Ă���NPCID���擾
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return NPCID
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_PEOPLE_DATA_GetLoadNpcID( const FIELD_WFBC_PEOPLE_DATA_LOAD* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->npc_id;
}




//----------------------------------------------------------------------------
/**
 *	@brief  �l���ʒu����ǂݍ���
 *
 *  @param  p_loader  �l�����[�_�[���g��
 *  @param  type      �^�C�v
 *	@param	heapID    �q�[�vID
 *
 *	@return �ʒu���
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_CORE_PEOPLE_POS* FIELD_WFBC_PEOPLE_POS_Create( FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader, FIELD_WFBC_CORE_TYPE type, HEAPID heapID )
{
  return GFL_ARC_LoadDataAllocByHandle( p_loader->p_handle, NARC_field_wfbc_data_wb_wfbc_block_bc_pos + type, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l���ʒu���̔j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_PEOPLE_POS_Delete( FIELD_WFBC_CORE_PEOPLE_POS* p_wk )
{
  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �C���f�b�N�X�̈ʒu����Ԃ�
 *
 *	@param	cp_wk ���[�N
 *	@param	index �C���f�b�N�X
 *
 *	@return �ʒu���
 */
//-----------------------------------------------------------------------------
const FIELD_WFBC_CORE_PEOPLE_POS* FIELD_WFBC_PEOPLE_POS_GetIndexData( const FIELD_WFBC_CORE_PEOPLE_POS* cp_wk, u32 index )
{
  GF_ASSERT( cp_wk );
  GF_ASSERT( index < FIELD_WFBC_PEOPLE_MAX );

  return &cp_wk[index];
}





//-----------------------------------------------------------------------------
/**
 *        private�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �܂��A�l���f�[�^�Ƃ��ĕێ����Ă��Ȃ��l����IDX�Ԗڂ��擾����
 *
 *	@param	cp_wk     ���[�N
 *	@param	idx       �C���f�b�N�X
 */ 
//-----------------------------------------------------------------------------
static u32 WFBC_CORE_GetNotUseNpcID( const FIELD_WFBC_CORE* cp_wk, int idx )
{
  int i;
  int count;
  BOOL result;

  count = 0;
  for( i=0; i<FIELD_WFBC_NPCID_MAX; i++ )
  {
    result = WFBC_CORE_IsUseNpc( cp_wk, i );
    if( result == FALSE )
    {
      if( count == idx )
      {
        return i;
      }
      count ++;
    }
  }

  // �S�������Ă�i���肦�Ȃ��j
  GF_ASSERT( 0 );
  return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �܂��A�l���f�[�^�Ƃ��ĕێ����Ă��Ȃ����[�N�C���f�b�N�X�����߂�
 *
 *	@param	cp_wk     ���[�N
 *	@param	idx       �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static u32 WFBC_CORE_GetNotUseNpcIndex( const FIELD_WFBC_CORE* cp_wk, int idx )
{
  int i;
  int count;
  BOOL result;

  count = 0;
  for( i=0; i<FIELD_WFBC_NPCID_MAX; i++ )
  {
    result = FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_wk->people[i] );
    if( result == FALSE )
    {
      if( count == idx )
      {
        return i;
      }
      count ++;
    }
  }

  for( i=0; i<FIELD_WFBC_NPCID_MAX; i++ )
  {
    result = FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_wk->back_people[i] );
    if( result == FALSE )
    {
      if( count == idx )
      {
        return i;
      }
      count ++;
    }
  }

  // �S�������Ă܂�
  GF_ASSERT( 0 );
  return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brieif NPC_ID�����[�N���̐l���ɂ���̂��`�F�b�N
 *
 *	@param	cp_wk     ���[�N
 *	@param	npc_id    NPCID
 *
 *	@retval TRUE  ����
 *	@retval FALSE ���Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_CORE_IsUseNpc( const FIELD_WFBC_CORE* cp_wk, int npc_id )
{
  int i;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_wk->people[i] ) )
    {
      if( cp_wk->people[i].npc_id == npc_id )
      {
        return TRUE;
      }
    }
  }

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_wk->back_people[i] ) )
    {
      if( cp_wk->back_people[i].npc_id == npc_id )
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �l���z��̓��e���@���l�Ń\�[�g����
 *
 *	@param	p_array   �z��
 *	@param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFBC_CORE_SortPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array, HEAPID heapID )
{
  int i, j;
  BOOL flag;
  FIELD_WFBC_CORE_PEOPLE* p_tmp;

  p_tmp = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( heapID ), sizeof(FIELD_WFBC_CORE_PEOPLE) );


  // �@���l�̍������ԂɃo�u���\�[�g
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX-1; i++ )
  {
    flag = 0;
    for(j=0; j<FIELD_WFBC_PEOPLE_MAX-1; j++)
    {
      if( p_array[j].mood < p_array[j+1].mood )
      {
        // ���ꂩ��
        GFL_STD_MemCopy( &p_array[j+1], p_tmp, sizeof(FIELD_WFBC_CORE_PEOPLE) );
        GFL_STD_MemCopy( &p_array[j], &p_array[j+1], sizeof(FIELD_WFBC_CORE_PEOPLE) );
        GFL_STD_MemCopy( p_tmp, &p_array[j], sizeof(FIELD_WFBC_CORE_PEOPLE) );
        flag =1;
      }
    }

    if( flag == 0 )
    {
      // 1�x���ύX���Ȃ���΃\�[�g����
      break;
    }
  }

  GFL_HEAP_FreeMemory( p_tmp );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �l�����[�N�̓��e���߂�
 *
 *	@param	p_array   �z��
 */
//-----------------------------------------------------------------------------
static void WFBC_CORE_PackPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array )
{
  int  i, j;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_array[i] ) == FALSE )
    {
      for( j=i+1; j<FIELD_WFBC_PEOPLE_MAX; j++ )
      {
        GFL_STD_MemCopy( &p_array[j], &p_array[j-1], sizeof(FIELD_WFBC_CORE_PEOPLE) );
      }
      // �Ō�̃��[�N���N���A
      FIELD_WFBC_CORE_PEOPLE_Clear( &p_array[FIELD_WFBC_PEOPLE_MAX-1] );
    }
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �l������@��Ԃ��̏��͔j��
 *	        �ʏ퐢�E�̔z��ɒǉ�  �ʏ�z���ǂ��o���`�ŁA�����E�̔z��ɂ��i�[�����
 *
 *	@param	p_wk          ���[�N
 *	@param	cp_people     �ǉ�����l�f�[�^
 */
//-----------------------------------------------------------------------------
static void WFBC_CORE_PushPeople( FIELD_WFBC_CORE* p_wk, const FIELD_WFBC_CORE_PEOPLE* cp_people )
{
  GF_ASSERT( p_wk );
  GF_ASSERT( cp_people );
  
  if( WFBC_CORE_IsPeopleArrayFull( p_wk->people ) == TRUE )
  {
    // ���̏��𗠐��E�ɓn���B
    WFBC_CORE_PushPeopleArray( p_wk->back_people, &p_wk->people[FIELD_WFBC_PEOPLE_MAX - 1] );
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[FIELD_WFBC_PEOPLE_MAX - 1] );
  }

  // �ʏ퐢�E�̏��ɒǉ�
  WFBC_CORE_PushPeopleArray( p_wk->people, cp_people );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l�̔z�񂪂����ς����`�F�b�N
 *
 *	@param	cp_array  �z��
 *
 *	@retval   TRUE    �����ς�
 *	@retval   FALSE   �܂��]�T������
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_CORE_IsPeopleArrayFull( const FIELD_WFBC_CORE_PEOPLE* cp_array )
{
  int i;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( !FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_array[i] ) )
    {
      return FALSE;
    }
  }
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l�z��ɐl������B  ��Ԃ��̏��͔j��
 *
 *	@param	p_array     �z��
 *	@param	cp_people   �l
 */
//-----------------------------------------------------------------------------
static void WFBC_CORE_PushPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array, const FIELD_WFBC_CORE_PEOPLE* cp_people )
{ 
  int i;
  
  // ���ׂĂP�Â��炵�āA�擪�ɒǉ�
  for( i=FIELD_WFBC_PEOPLE_MAX-1; i>0; i-- )
  {
    GFL_STD_MemCopy( &p_array[i-1], &p_array[i], sizeof(FIELD_WFBC_CORE_PEOPLE) );
  }
  GFL_STD_MemCopy( cp_people, &p_array[0], sizeof(FIELD_WFBC_CORE_PEOPLE) );
}




//----------------------------------------------------------------------------
/**
 *	@brief  �󂢂Ă�l�����[�N��Ԃ�
 *
 *	@param	p_array   �z��
 *
 *	@return �󂢂Ă���l�����[�N
 */
//-----------------------------------------------------------------------------
static FIELD_WFBC_CORE_PEOPLE* WFBC_CORE_GetClearPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array )
{
  int i;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( !FIELD_WFBC_CORE_PEOPLE_IsInData( &p_array[i] ) )
    {
      return &p_array[i];
    }
  }
  // �󂫂��Ȃ�
  GF_ASSERT( 0 );
  return &p_array[0]; // �t���[�Y�͉��
}



//----------------------------------------------------------------------------
/**
 *	@brief  �l�������Z�b�g�A�b�v
 *
 *	@param	p_people  �l�����[�N
 *	@param	npc_id    NPCID
 */
//-----------------------------------------------------------------------------
static void WFBC_CORE_SetUpPeople( FIELD_WFBC_CORE_PEOPLE* p_people, u8 npc_id, const MYSTATUS* cp_mystatus, u16 mood )
{
  p_people->data_in = TRUE;
  p_people->npc_id  = npc_id;
  p_people->mood    = mood;
  p_people->one_day_msk  = FIELD_WFBC_ONEDAY_MSK_INIT;

  // �e�̐ݒ�
  // �����ŏ���������
  FIELD_WFBC_CORE_PEOPLE_SetParentData( p_people, cp_mystatus );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �l�����[�N  �w�ʐ��E�ɍs���ׂ����`�F�b�N
 *
 *	@param	cp_people   �l�����[�N
 *
 *	@retval TRUE  �����ׂ�
 *	@retval FALSE �����Ȃ��ׂ�
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_CORE_People_IsGoBack( const FIELD_WFBC_CORE_PEOPLE* cp_people )
{
  if( cp_people->mood == 0 )
  {
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �@���l�̌v�Z
 *
 *	@param	p_people  �l�����[�N
 *	@param	add       ���Z  �}�C�i�X����
 *
 *	@retval TRUE    �w�ʐ��E�ɂ����ׂ�
 *	@retval FALSE   �w�ʐ��E�ɂ����Ȃ��ׂ�
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_CORE_People_AddMood( FIELD_WFBC_CORE_PEOPLE* p_people, int add )
{
  int mood = p_people->mood;
  BOOL ret = FALSE;
  
  if( (mood + add) > FIELD_WFBC_MOOD_MAX )
  {
    p_people->mood = FIELD_WFBC_MOOD_MAX;
  }
  else if( (mood + add) < 0 )
  {
    p_people->mood = 0;
    ret = TRUE;
  }
  else
  {
    p_people->mood += add;
  }
  return ret;
}


#ifdef PM_DEBUG
u32 FIELD_WFBC_CORE_DEBUG_GetRandomNpcID( const FIELD_WFBC_CORE* cp_wk )
{
  u32 npc_idx;
  u32 npc_num;

  npc_num = FIELD_WFBC_CORE_GetPeopleNum( cp_wk, MAPMODE_NORMAL );

  npc_idx = GFUser_GetPublicRand( FIELD_WFBC_NPCID_MAX - npc_num );


  npc_idx = WFBC_CORE_GetNotUseNpcID( cp_wk, npc_idx );

  return npc_idx;
}
#endif
