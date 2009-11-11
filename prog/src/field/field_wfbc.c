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

#include "field_wfbc.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�l�����
//=====================================
struct _FIELD_WFBC_PEOPLE 
{
  u16   status;   // FIELD_WFBC_PEOPLE_STATUS
  u16   npcid;    // �l��ID
  u16   objcode;  // ������
  u16   pad;      // 
};

//-------------------------------------
///	WFBC���[�N
//=====================================
struct _FIELD_WFBC 
{
  FIELD_WFBC_CORE_TYPE type;
  FIELD_WFBC_PEOPLE people[ FIELD_WFBC_PEOPLE_MAX ];
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	FIELD_WFBC
//=====================================
static void WFBC_Clear( FIELD_WFBC* p_wk );
static int WFBC_GetClearPeople( const FIELD_WFBC* cp_wk );
static int WFBC_GetPeopleNum( const FIELD_WFBC* cp_wk );

static void WFBC_SetCore( FIELD_WFBC* p_wk, const FIELD_WFBC_CORE* cp_buff );
static void WFBC_GetCore( const FIELD_WFBC* cp_wk, FIELD_WFBC_CORE* p_buff );

//-------------------------------------
///	FIELD_WFBC_PEOPLE
//=====================================
static void WFBC_PEOPLE_Clear( FIELD_WFBC_PEOPLE* p_people );
static void WFBC_PEOPLE_SetUp( FIELD_WFBC_PEOPLE* p_people, const FIELD_WFBC_CORE_PEOPLE* cp_core );
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
 *	@return �l�����
 */
//-----------------------------------------------------------------------------
const FIELD_WFBC_PEOPLE* FIELD_WFBC_GetPeople( const FIELD_WFBC* cp_wk, int index )
{
  GF_ASSERT( index < FIELD_WFBC_PEOPLE_MAX );
  GF_ASSERT( cp_wk );
  
  return &cp_wk->people[index];
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
 *	@brief  �X����ݒ�
 *
 *	@param	p_wk        ���[�N
 *	@param	cp_core     �R�A���
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_SetUp( FIELD_WFBC* p_wk, const FIELD_WFBC_CORE* cp_core )
{
  WFBC_SetCore( p_wk, cp_core );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l���̒ǉ�
 *
 *	@param	p_wk    ���[�N
 *	@param	objcode �I�u�W�F�R�[�h
 *
 *	@retval �ǉ������C���f�b�N�X
 */
//-----------------------------------------------------------------------------
int FIELD_WFBC_AddPeople( FIELD_WFBC* p_wk, const FIELD_WFBC_CORE_PEOPLE* cp_core )
{
  int index;

  GF_ASSERT( p_wk );
  // �󂢂Ă��郏�[�N���擾
  index = WFBC_GetClearPeople( p_wk );

  // �l�����Z�b�g�A�b�v
  WFBC_PEOPLE_SetUp( &p_wk->people[index], cp_core );

  return index;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l���̔j��
 *
 *	@param	p_wk    ���[�N
 *	@param	index   �j������l���̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_DeletePeople( FIELD_WFBC* p_wk, int index )
{
  GF_ASSERT( p_wk );
  GF_ASSERT( index < FIELD_WFBC_PEOPLE_MAX );
  
  // �C���f�b�N�X�̃��[�N���N���[��
  WFBC_PEOPLE_Clear( &p_wk->people[index] );
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
  return cp_people->objcode;
}





//-----------------------------------------------------------------------------
/**
 *      private�֐�
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  WFBC���[�N���N���A
 */
//-----------------------------------------------------------------------------
static void WFBC_Clear( FIELD_WFBC* p_wk )
{
  int i;

  GFL_STD_MemClear( p_wk, sizeof(FIELD_WFBC) );

  //�@�l�����[�N���N���A
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    WFBC_PEOPLE_Clear( &p_wk->people[i] );
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

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( cp_wk->people[i].status != FIELD_WFBC_PEOPLE_STATUS_NONE )
    {
      return i;
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
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( cp_wk->people[i].status != FIELD_WFBC_PEOPLE_STATUS_NONE )
    {
      num ++;
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
static void WFBC_SetCore( FIELD_WFBC* p_wk, const FIELD_WFBC_CORE* cp_buff )
{
  int i;
  BOOL result;

  GF_ASSERT( p_wk );
  GF_ASSERT( cp_buff );

  result = FIELD_WFBC_CORE_IsInData( cp_buff );
  GF_ASSERT( result );
  
  // �����WFBC�����N���[���A�b�v
  WFBC_Clear( p_wk );

  // ����ݒ�
  p_wk->type = cp_buff->type;
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    // �f�[�^����������ݒ�
    if( cp_buff->people[i].data_in )
    {
      WFBC_PEOPLE_SetUp( &p_wk->people[i], &cp_buff->people[i] );
    }
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

  // ���̃N���A
  FIELD_WFBC_CORE_Crear( p_buff );
  
  // �����i�[
  p_buff->type = cp_wk->type;
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    // �f�[�^����������ݒ�
    WFBC_PEOPLE_GetCore( &cp_wk->people[i], &p_buff->people[i] );
  }
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
  p_people->status  = FIELD_WFBC_PEOPLE_STATUS_NONE;
  p_people->objcode = 0;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �l���̃Z�b�g�A�b�v
 *
 *	@param	p_people    �l�����[�N
 *	@param	cp_core     �l��CORE���
 */
//-----------------------------------------------------------------------------
static void WFBC_PEOPLE_SetUp( FIELD_WFBC_PEOPLE* p_people, const FIELD_WFBC_CORE_PEOPLE* cp_core )
{
  BOOL result;
  
  GF_ASSERT( p_people );
  // ����ȏ�񂩃`�F�b�N
  result = FIELD_WFBC_CORE_PEOPLE_IsInData( cp_core );
  GF_ASSERT( result == TRUE );

  p_people->status  = FIELD_WFBC_PEOPLE_STATUS_NORMAL;
  p_people->npcid   = cp_core->npc_id;

  // NPCID�̊�{����ǂݍ���
  p_people->objcode = 0;

  // �g���f�[�^��ݒ肷��
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

  FIELD_WFBC_CORE_PEOPLE_Crear( p_core );

  if( cp_people->status == FIELD_WFBC_PEOPLE_STATUS_NONE )
  {
    // ���Ȃ�
    return ;
  }

  p_core->data_in = TRUE;
  p_core->npc_id  = cp_people->npcid;
}





//�f�o�b�O����
#ifdef PM_DEBUG
static void DEBWIN_Update_CityLevel( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_CityLevel( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_CityType( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_CityType( void* userWork , DEBUGWIN_ITEM* item );


void FIELD_FUNC_RANDOM_GENERATE_InitDebug( HEAPID heapId )
{
  DEBUGWIN_AddGroupToTop( 10 , "RandomMap" , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_CityLevel ,DEBWIN_Draw_CityLevel , 
                             NULL , 10 , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_CityType ,DEBWIN_Draw_CityType , 
                             NULL , 10 , heapId );
}

void FIELD_FUNC_RANDOM_GENERATE_TermDebug( void )
{
  DEBUGWIN_RemoveGroup( 10 );
}

static void DEBWIN_Update_CityLevel( void* userWork , DEBUGWIN_ITEM* item )
{
#if 0
  SAVE_CONTROL_WORK *saveWork = SaveControl_GetPointer();
  RANDOMMAP_SAVE* mapSave = RANDOMMAP_SAVE_GetRandomMapSave( saveWork );
  u16 level = RANDOMMAP_SAVE_GetCityLevel( mapSave );
  
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    if( level < 20 )
    {
      level++;
      RANDOMMAP_SAVE_SetCityLevel( mapSave , level );
      DEBUGWIN_RefreshScreen();
    }
  }
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT )
  {
    if( level > 0 )
    {
      level--;
      RANDOMMAP_SAVE_SetCityLevel( mapSave , level );
      DEBUGWIN_RefreshScreen();
    }
  }
#endif
}

static void DEBWIN_Draw_CityLevel( void* userWork , DEBUGWIN_ITEM* item )
{
#if 0
  SAVE_CONTROL_WORK *saveWork = SaveControl_GetPointer();
  RANDOMMAP_SAVE* mapSave = RANDOMMAP_SAVE_GetRandomMapSave( saveWork );
  u16 level = RANDOMMAP_SAVE_GetCityLevel( mapSave );
  DEBUGWIN_ITEM_SetNameV( item , "Level[%d]",level );
#endif
}

static void DEBWIN_Update_CityType( void* userWork , DEBUGWIN_ITEM* item )
{
#if 0
  SAVE_CONTROL_WORK *saveWork = SaveControl_GetPointer();
  RANDOMMAP_SAVE* mapSave = RANDOMMAP_SAVE_GetRandomMapSave( saveWork );
  u8 type = RANDOMMAP_SAVE_GetCityType( mapSave );
  
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT || 
      GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT  ||
      GFL_UI_KEY_GetRepeat() & PAD_BUTTON_A )
  {
    if( type == RMT_BLACK_CITY )
    {
      type = RMT_WHITE_FOREST;
    }
    else
    {
      type = RMT_BLACK_CITY;
    }
    RANDOMMAP_SAVE_SetCityType( mapSave , type );
    DEBUGWIN_RefreshScreen();
  }
#endif 
}

static void DEBWIN_Draw_CityType( void* userWork , DEBUGWIN_ITEM* item )
{
#if 0
  SAVE_CONTROL_WORK *saveWork = SaveControl_GetPointer();
  RANDOMMAP_SAVE* mapSave = RANDOMMAP_SAVE_GetRandomMapSave( saveWork );
  u8 type = RANDOMMAP_SAVE_GetCityType( mapSave );
  if( type == RMT_BLACK_CITY )
  {
    DEBUGWIN_ITEM_SetName( item , "Type[BLACK CITY]" );
  }
  else
  {
    DEBUGWIN_ITEM_SetName( item , "Type[WHITE FOREST]" );
  }
#endif
}
#endif
