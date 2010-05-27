//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		debug_wfbc.c
 *	@brief  WFBC�I�[�g�f�o�b�N�@�\
 *	@author	tomoya takahashi
 *	@date		2010.05.24
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifdef PM_DEBUG

#include "debug_wfbc.h"

#include "field/field_wfbc_data.h" 

#include "fieldmap/zone_id.h"
#include "event_mapchange.h"

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
///	�S�l��PATTERN�`�F�b�N
//=====================================
typedef struct {

  FIELD_WFBC_CORE* p_wfbc_data;
  GAMESYS_WORK* p_gamesys;
  FIELDMAP_WORK* p_fieldmap;

  int p[10];

  int wait;

  int pattern_count;
  
} EVENT_DEBUG_WFBC_PEOPLE;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static GMEVENT_RESULT ev_DEBUG_WFBC_PeopleCheck( GMEVENT* p_event, int* p_seq, void* p_work );

static BOOL ev_DEBUG_WFBC_PeopleCheckNextID( u32 index, const EVENT_DEBUG_WFBC_PEOPLE* cp_wk, int* p_nextID );

static GMEVENT_RESULT ev_DEBUG_WFBC_100SetUp( GMEVENT* p_event, int* p_seq, void* p_work );

//----------------------------------------------------------------------------
/**
 *	@brief  �l���SPATTERN�`�F�b�N
 */
//-----------------------------------------------------------------------------
GMEVENT * DEBUG_WFBC_PeopleCheck(
    GAMESYS_WORK *gsys )
{
  EVENT_DEBUG_WFBC_PEOPLE* p_wk;
  GMEVENT* p_event;
  GAMEDATA* p_gdata = GAMESYSTEM_GetGameData( gsys );

  p_event = GMEVENT_Create( gsys, NULL,
      ev_DEBUG_WFBC_PeopleCheck, sizeof(EVENT_DEBUG_WFBC_PEOPLE) );

  p_wk = GMEVENT_GetEventWork( p_event );
  p_wk->p_wfbc_data = GAMEDATA_GetMyWFBCCoreData( p_gdata );

  p_wk->p_gamesys = gsys;
  p_wk->p_fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );


  return p_event;
}


//----------------------------------------------------------------------------
/**
 *	@brief  100��SetUp��ǂ�ł݂�B
 */
//-----------------------------------------------------------------------------
GMEVENT * DEBUG_WFBC_100SetUp(
    GAMESYS_WORK *gsys )
{
  EVENT_DEBUG_WFBC_PEOPLE* p_wk;
  GMEVENT* p_event;
  GAMEDATA* p_gdata = GAMESYSTEM_GetGameData( gsys );

  p_event = GMEVENT_Create( gsys, NULL,
      ev_DEBUG_WFBC_100SetUp, sizeof(EVENT_DEBUG_WFBC_PEOPLE) );

  p_wk = GMEVENT_GetEventWork( p_event );
  p_wk->p_wfbc_data = GAMEDATA_GetMyWFBCCoreData( p_gdata );

  p_wk->p_gamesys = gsys;
  p_wk->p_fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );


  return p_event;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT ev_DEBUG_WFBC_100SetUp( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_DEBUG_WFBC_PEOPLE* p_wk = p_work;
  int i;
  HEAPID heapID = FIELDMAP_GetHeapID( p_wk->p_fieldmap );
  GAMEDATA* p_gdata = GAMESYSTEM_GetGameData( p_wk->p_gamesys );
  const MYSTATUS* cp_mystatus = GAMEDATA_GetMyStatus(p_gdata);

  for( i=0; i<100; i++ ){
    FIELD_WFBC_CORE_Clear( p_wk->p_wfbc_data );
    FIELD_WFBC_CORE_SetUp( p_wk->p_wfbc_data, cp_mystatus, heapID );
  }

  return GMEVENT_RES_FINISH;
}


//-----------------------------------------------------------------------------
/**
 *      private�֐�
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  �l��PATTERN���`�F�b�N
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT ev_DEBUG_WFBC_PeopleCheck( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_DEBUG_WFBC_PEOPLE* p_wk = p_work;

  switch( *p_seq ){
  // ������
  case 0:
    {
      int i;
      for( i=0; i<10; i++ ){
        p_wk->p[i]  = i;
      }
    }
    p_wk->pattern_count = 1;
    (*p_seq)++;
    break;

  // �W�����v
  case 1:

    {
      int i;
      OS_SetPrintOutput_Arm9( 2 );
      TOMOYA_Printf( "wfbcData[%d]:", p_wk->pattern_count );
      // �f�[�^�ݒ�
      for( i=0; i<10; i++ ){
        p_wk->p_wfbc_data->people[i].data_in = TRUE;
        p_wk->p_wfbc_data->people[i].npc_id  = p_wk->p[i];
        p_wk->p_wfbc_data->people[i].mood  = 50;
        p_wk->p_wfbc_data->people[i].one_day_msk  = FIELD_WFBC_ONEDAY_MSK_INIT;
        TOMOYA_Printf( "%d ", p_wk->p[i] );
      }
      TOMOYA_Printf( "\n" );
      OS_SetPrintOutput_Arm9( 0 );
    }
    
    p_wk->wait = 60;
    {
      GMEVENT* p_subevent;
      p_subevent = DEBUG_EVENT_QuickChangeMapDefaultPos( p_wk->p_gamesys, p_wk->p_fieldmap, ZONE_ID_BC10 );
      GMEVENT_CallEvent( p_event, p_subevent );
    }
    
    (*p_seq)++;
    break;

  // �E�G�C�g
  case 2:
    p_wk->wait --;
    if( p_wk->wait <= 0 ){
      (*p_seq)++;
    }
    break;
    
  // ����PATTERN
  case 3:
    {

      int check_index;
      int next_id;
      
      // �J�E���g�A�b�v
      for( check_index = 9; check_index>=0; check_index-- ){

        if( ev_DEBUG_WFBC_PeopleCheckNextID( check_index, p_wk, &next_id ) == FALSE ){
          p_wk->p[check_index] = next_id;
          break;
        }

        // check_index�̌��ł͂��ׂă`�F�b�N�����̂�LOOP�N���A
        p_wk->p[check_index] = -1;
      }

      // check_index���}�C�i�X�ɂȂ�����I���
      if( check_index<0 ){

        OS_SetPrintOutput_Arm9( 2 );
        TOMOYA_Printf( "ALL WFBC PEOPLE CHECK END\n" );
        OS_SetPrintOutput_Arm9( 0 );
        return( GMEVENT_RES_FINISH );
      }

      // check_index���傫���C���f�b�N�X�̃��[�N���Đݒ�
      for( check_index += 1; check_index<10; check_index++ ){
        GF_ASSERT( p_wk->p[check_index] == -1 );
        p_wk->p[check_index] = 0;
        if( ev_DEBUG_WFBC_PeopleCheckNextID( check_index, p_wk, &next_id ) == FALSE ){
          p_wk->p[check_index] = next_id;
        }else{
          GF_ASSERT( 0 ); // ���������B
        }
      }
    }
    (*p_seq) = 1;
    p_wk->pattern_count++;
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���̃`�F�b�NID�𒲂ׂ�
 *
 *	@param	index �C���f�b�N�X�@�O�`�X
 *	@param	p_wk      ���[�N
 *	@param	p_nextID  ����ID�\��
 *
 *	@retval p_nextID��FIELD_WFBC_NPCID_MAX�𒴂���
 */
//-----------------------------------------------------------------------------
static BOOL ev_DEBUG_WFBC_PeopleCheckNextID( u32 index, const EVENT_DEBUG_WFBC_PEOPLE* cp_wk, int* p_nextID )
{
  int i;
  int check;
  BOOL not_hit;
  BOOL ret = FALSE;

  
  check = cp_wk->p[index];

  do{
    check ++;
    not_hit = TRUE;

    for( i=0; i<10; i++ ){

      if( i != index ){
        if( cp_wk->p[i] == check ){
          not_hit = FALSE;
        }
      }
    } 
    
  }while( not_hit == FALSE );

  *p_nextID = check;
  
  if( check >= FIELD_WFBC_NPCID_MAX ){
    return TRUE;
  }
  return FALSE;
  
}



#endif //PM_DEBUG

