//////////////////////////////////////////////////////////////////////////
/**
 * @brief  �S�ڑ��`�F�b�N�C�x���g
 * @file   event_debug_all_connect_check.c
 * @author obata
 * @date   2010.04.21
 */
//////////////////////////////////////////////////////////////////////////
#ifdef PM_DEBUG
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "field/eventdata_system.h"
#include "field/eventdata_sxy.h"
#include "sound/wb_sound_data.sadl" // for SEQ_ME_BADGE

#include "fieldmap.h"
#include "event_debug_all_connect_check.h"
#include "event_mapchange.h"
#include "eventdata_local.h"
#include "field_sound.h"

#include "../../../resource/fldmapdata/zonetable/zone_id.h" // for ZONE_ID_xxxx


// �f�o�b�O�o�͐�
#define PRINT_TARGET (3)


//========================================================================
// ���`�F�b�N�ΏۊO�̃]�[��ID
//========================================================================
static const UncheckZoneList[] = 
{
  ZONE_ID_MAX,
};


//========================================================================
// ���C�x���g���[�N
//========================================================================
typedef struct {

  GAMESYS_WORK*     gameSystem;
  GAMEDATA*         gameData;
  EVENTDATA_SYSTEM* eventDataSystem;
  u32 start_zone_id; // �X�^�[�g�n�_�̃]�[��ID

  // �`�F�b�N�f�[�^
  u16 check_zone_id; // �`�F�b�N�Ώۂ̃]�[��ID
  u32 check_exit_id; // �`�F�b�N�Ώۂ̏o�����ID
  u16 check_offset;  // �`�F�b�N�Ώۂ̃I�t�Z�b�g

} CHECK_WORK;


//========================================================================
// ��prototype
//========================================================================
static GMEVENT_RESULT AllConnectCheckEvent( GMEVENT* event, int* seq, void* wk ); // �S�ڑ��`�F�b�N�C�x���g
static u16 GetCurrentZoneID( const CHECK_WORK* work ); // ���݂̃]�[��ID���擾����
static u32 GetConnectNum( const CHECK_WORK* work ); // �ڑ��f�[�^�̐����擾����
static BOOL CheckGridMap( const CHECK_WORK* work ); // �O���b�h�}�b�v���ǂ������`�F�b�N����
// �}�b�v�`�F���W
static GMEVENT* GetMapChangeEvent_Connect( const CHECK_WORK* work ); // �ڑ��ɂ��}�b�v�`�F���W�C�x���g���擾����
static GMEVENT* GetMapChangeEvent_Return( const CHECK_WORK* work ); // �`�F�b�N�Ώۃ]�[���ɖ߂�}�b�v�`�F���W�C�x���g���擾����
// �`�F�b�N�Ώۂ̐ڑ��f�[�^
static const CONNECT_DATA* GetConnectData( const CHECK_WORK* work ); // �`�F�b�N�Ώۂ̐ڑ��f�[�^���擾���� 
static LOC_EXIT_OFS GetExitOffset( const CHECK_WORK* work ); // �`�F�b�N�Ώۂ̃��P�[�V�����I�t�Z�b�g���擾����
static u16 GetConnectOffsetSize( const CHECK_WORK* work ); // �`�F�b�N�Ώۂ̃I�t�Z�b�g�ő�l���擾����
static u16 GetConnectOffsetSize_GRID( const CHECK_WORK* work ); // �`�F�b�N�Ώۂ̃I�t�Z�b�g�ő�l���擾���� ( �O���b�h�}�b�v�p )
static u16 GetConnectOffsetSize_RAIL( const CHECK_WORK* work ); // �`�F�b�N�Ώۂ̃I�t�Z�b�g�ő�l���擾���� ( ���[���}�b�v�p )
static u16 GetConnectZoneID( const CHECK_WORK* work ); // �`�F�b�N�Ώۂ̐ڑ���]�[��ID���擾����
static BOOL CheckSpecialConnect( const CHECK_WORK* work ); // �`�F�b�N�Ώۂ̏o�����������ڑ��o��������ǂ����𔻒肷��
static BOOL CheckClosedConnect( const CHECK_WORK* work ); // �`�F�b�N�Ώۂ̏o������������o��������ǂ������`�F�b�N����
// �`�F�b�N�Ώۂ̍X�V
static void UpdateCheckZoneID( CHECK_WORK* work ); // �`�F�b�N�Ώۂ̃]�[��ID���X�V����
static void UpdateCheckExitID( CHECK_WORK* work ); // �`�F�b�N�Ώۂ̏o�����ID���X�V����
static void UpdateCheckOffset( CHECK_WORK* work ); // �`�F�b�N�Ώۂ̃I�t�Z�b�g���X�V����
static void ResetCheckExitID( CHECK_WORK* work ); // �`�F�b�N�Ώۂ̏o�����ID�����Z�b�g����
static void ResetCheckOffset( CHECK_WORK* work ); // �`�F�b�N�Ώۂ̃I�t�Z�b�g�����Z�b�g����
static u16 GetCheckZoneID( const CHECK_WORK* work ); // �`�F�b�N�Ώۂ̃]�[��ID���擾����
static u32 GetCheckExitID( const CHECK_WORK* work ); // �`�F�b�N�Ώۂ̏o�����ID���擾����
static u16 GetCheckOffset( const CHECK_WORK* work ); // �`�F�b�N�Ώۂ̃I�t�Z�b�g���擾����
// ���[�e�B���e�B
static u16 ConnectData_GetExitDirToDir( const CONNECT_DATA * cp_data ); // EXIT_DIR_xxxx �� DIR_xxxx �ɕϊ�����
static BOOL CheckValidZone( u16 zone_id ); // �`�F�b�N���L���ȃ]�[�����ǂ����𔻒肷��
// �f�o�b�O�o��
static void DebugPrint_Start( const CHECK_WORK* work );
static void DebugPrint_AllConnect( const CHECK_WORK* work );
static void DebugPrint_CheckConnect( const CHECK_WORK* work );
static void DebugPrint_DetectSpecialConnect( const CHECK_WORK* work );
static void DebugPrint_Finish( const CHECK_WORK* work );


//========================================================================
// ��public functions
//========================================================================

//------------------------------------------------------------------------
/**
 * @brief �S�ڑ��`�F�b�N�C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param wk
 *
 * @return ���������C�x���g
 */
//------------------------------------------------------------------------
GMEVENT* EVENT_DEBUG_AllConnectCheck( GAMESYS_WORK* gameSystem, void* wk )
{
  GMEVENT* event;
  CHECK_WORK* work;
  GAMEDATA* gameData;

  gameData = GAMESYSTEM_GetGameData( gameSystem );

  // �C�x���g�𐶐�
  event = GMEVENT_Create( 
      gameSystem, NULL, AllConnectCheckEvent, sizeof(CHECK_WORK) );

  // �C�x���g���[�N��������
  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(CHECK_WORK) );
  work->gameSystem = gameSystem;
  work->gameData = gameData;
	work->eventDataSystem = GAMEDATA_GetEventData( gameData );

  return event;
} 

//========================================================================
// ��private functions
//========================================================================

// �C�x���g�̃V�[�P���X�ԍ�
enum {
  CHECK_SEQ_INIT,           // �C�x���g������
  CHECK_SEQ_SEARCH_CONNECT, // �ڑ��f�[�^����
  CHECK_SEQ_JUMP_NEXT_ZONE, // ���̃]�[���ɔ��
  CHECK_SEQ_NEW_ZONE_START, // �V�����]�[���̃`�F�b�N�J�n
  CHECK_SEQ_CONNECT_TEST,   // �ڑ��e�X�g
  CHECK_SEQ_RETURN,         // �`�F�b�N�Ώۃ]�[���֖߂�
  CHECK_SEQ_TO_NEXT_CHECK,  // ���̃`�F�b�N��
  CHECK_SEQ_FINISH,         // �C�x���g�I��
  CHECK_SEQ_FANFARE,        // �t�@���t�@�[��
  CHECK_SEQ_BGM_RECOVER,    // BGM ���A
};


//------------------------------------------------------------------------
/**
 * @brief �S�ڑ��`�F�b�N�C�x���g
 */
//------------------------------------------------------------------------
static GMEVENT_RESULT AllConnectCheckEvent( GMEVENT* event, int* seq, void* wk )
{
  CHECK_WORK*    work       = wk;
  GAMESYS_WORK*  gameSystem = work->gameSystem;
  GAMEDATA*      gameData   = work->gameData;
  FIELDMAP_WORK* fieldmap   = GAMESYSTEM_GetFieldMapWork( gameSystem );

  switch( *seq ) {
  // �C�x���g������
  case CHECK_SEQ_INIT:
    DebugPrint_Start( work );
    work->start_zone_id = GetCurrentZoneID( work ); // �X�^�[�g�n�_���L��
    work->check_zone_id = work->start_zone_id;
    *seq = CHECK_SEQ_NEW_ZONE_START;
    break;

  // �ڑ��f�[�^����
  case CHECK_SEQ_SEARCH_CONNECT:
    // �S�Ă̏o������̃`�F�b�N������
    if( GetConnectNum(work) <= work->check_exit_id ) {
      *seq = CHECK_SEQ_JUMP_NEXT_ZONE; // ���̃]�[����
    }
    else {
      *seq = CHECK_SEQ_CONNECT_TEST;
    }
    break;

  // ���̃]�[���ɔ��
  case CHECK_SEQ_JUMP_NEXT_ZONE:
    ResetCheckOffset( work );
    ResetCheckExitID( work );
    UpdateCheckZoneID( work ); 

    // ���
    if( GetCheckZoneID(work) == work->start_zone_id ) {
      *seq = CHECK_SEQ_FINISH;
    }
    else {
      GMEVENT_CallEvent( event, GetMapChangeEvent_Return(work) );
      *seq = CHECK_SEQ_NEW_ZONE_START;
    }
    break;

  // �V�����]�[���̃`�F�b�N�J�n
  case CHECK_SEQ_NEW_ZONE_START:
    // �ڑ����Ȃ��ꍇ
    if( GetConnectNum(work) == 0 ) {
      *seq = CHECK_SEQ_JUMP_NEXT_ZONE; // ���̃]�[����
    }
    else {
      *seq = CHECK_SEQ_CONNECT_TEST;
    }
    DebugPrint_AllConnect( work );
    break; 

  // �ڑ��e�X�g
  case CHECK_SEQ_CONNECT_TEST:
    DebugPrint_CheckConnect( work );

    // ����ڑ��o�����̏ꍇ
    if( CheckSpecialConnect(work) ) {
      DebugPrint_DetectSpecialConnect( work );
      GF_ASSERT(0); // WB �ɂ͑��݂��Ȃ��͂�!!
      *seq = CHECK_SEQ_TO_NEXT_CHECK;
    }
    // �����o�����̏ꍇ
    else if( CheckClosedConnect(work) ) {
      *seq = CHECK_SEQ_TO_NEXT_CHECK;
    }
    // �ʏ�̏o������̏ꍇ
    else {
      GMEVENT_CallEvent( event, GetMapChangeEvent_Connect(work) );
      *seq = CHECK_SEQ_RETURN;
    }
    break; 

  // �`�F�b�N�Ώۃ]�[���֖߂�
  case CHECK_SEQ_RETURN:
    GMEVENT_CallEvent( event, GetMapChangeEvent_Return(work) );
    (*seq)++;
    break;

  // �`�F�b�N�ΏۍX�V����
  case CHECK_SEQ_TO_NEXT_CHECK:
    UpdateCheckOffset( work );
    // �S�ẴI�t�Z�b�g�̃`�F�b�N������
    if( GetConnectOffsetSize(work) <= work->check_offset ) {
      // ���̏o�������
      ResetCheckOffset( work );
      UpdateCheckExitID( work ); 
    }
    *seq = CHECK_SEQ_SEARCH_CONNECT;
    break;

  // �C�x���g�I��
  case CHECK_SEQ_FINISH:
    DebugPrint_Finish( work );
    (*seq)++; 
    break;
  // �t�@���t�@�[��
  case CHECK_SEQ_FANFARE:
    GMEVENT_CallEvent( event, 
        EVENT_FSND_PushPlayJingleBGM( gameSystem, SEQ_ME_BADGE ) );
    (*seq)++;
    break; 
  // BGM ���A
  case CHECK_SEQ_BGM_RECOVER:
    GMEVENT_CallEvent( event, 
        EVENT_FSND_PopBGM( gameSystem, FSND_FADE_NONE, FSND_FADE_SHORT ) );
    (*seq)++;
    break;
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}


//------------------------------------------------------------------------
/**
 * @brief ���݂̃]�[��ID���擾����
 *
 * @return ���݂̃}�b�v�̃]�[��ID
 */
//------------------------------------------------------------------------
static u16 GetCurrentZoneID( const CHECK_WORK* work )
{
  GAMESYS_WORK*  gameSystem = work->gameSystem;
  FIELDMAP_WORK* fieldmap   = GAMESYSTEM_GetFieldMapWork( gameSystem );

  return FIELDMAP_GetZoneID( fieldmap );
}

//------------------------------------------------------------------------
/**
 * @brief �ڑ��f�[�^�̐����擾����
 *
 * @param work
 *
 * @return ���݂̃}�b�v�ɑ��݂���ڑ��f�[�^�̐�
 */
//------------------------------------------------------------------------
static u32 GetConnectNum( const CHECK_WORK* work )
{
  return EVENTDATA_GetConnectEventNum( work->eventDataSystem );
}

//------------------------------------------------------------------------
/**
 * @brief �O���b�h�}�b�v���ǂ������`�F�b�N����
 *
 * @param work
 *
 * @return ���݂̃}�b�v���O���b�h�}�b�v�Ȃ� TRUE
 *         �����łȂ��Ȃ� FALSE
 */
//------------------------------------------------------------------------
static BOOL CheckGridMap( const CHECK_WORK* work )
{
  FIELDMAP_WORK* fieldmap;
  FLDMAP_BASESYS_TYPE type;

  fieldmap = GAMESYSTEM_GetFieldMapWork( work->gameSystem );
  type     = FIELDMAP_GetBaseSystemType( fieldmap );

  if( type == FLDMAP_BASESYS_GRID ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//------------------------------------------------------------------------
/**
 * @brief �ڑ��ɂ��}�b�v�`�F���W�C�x���g���擾����
 *
 * @param work
 *
 * @return �`�F�b�N�Ώۂ̐ڑ��f�[�^�ɂ��}�b�v�`�F���W�C�x���g
 */
//------------------------------------------------------------------------
static GMEVENT* GetMapChangeEvent_Connect( const CHECK_WORK* work )
{
  GMEVENT* event;
  FIELDMAP_WORK* fieldmap;
  const CONNECT_DATA* connect;
  LOC_EXIT_OFS offset;

  fieldmap = GAMESYSTEM_GetFieldMapWork( work->gameSystem );
  connect  = GetConnectData( work );
  offset   = GetExitOffset( work );

  event = EVENT_ChangeMapByConnect( work->gameSystem, fieldmap, connect, offset ); 
  return event;
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۃ]�[���ɖ߂�}�b�v�`�F���W�C�x���g���擾����
 *
 * @param work
 *
 * @return �`�F�b�N�Ώۃ]�[���ɖ߂�}�b�v�`�F���W�C�x���g
 */
//------------------------------------------------------------------------
static GMEVENT* GetMapChangeEvent_Return( const CHECK_WORK* work )
{
  GAMESYS_WORK*  gameSystem = work->gameSystem;
  FIELDMAP_WORK* fieldmap   = GAMESYSTEM_GetFieldMapWork( gameSystem );

  GMEVENT* event;
  
  event = DEBUG_EVENT_QuickChangeMapDefaultPos( 
      gameSystem, fieldmap, work->check_zone_id );

  return event;
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۂ̐ڑ��f�[�^���擾���� 
 *
 * @param work
 */
//------------------------------------------------------------------------
static const CONNECT_DATA* GetConnectData( const CHECK_WORK* work )
{
  const CONNECT_DATA* connect;

  connect = EVENTDATA_GetConnectByID( work->eventDataSystem, work->check_exit_id );
  return connect;
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۂ̃��P�[�V�����I�t�Z�b�g���擾����
 *
 * @param work
 *
 * @return �`�F�b�N�Ώۂ̃I�t�Z�b�g�f�[�^
 */
//------------------------------------------------------------------------
static LOC_EXIT_OFS GetExitOffset( const CHECK_WORK* work )
{
  const CONNECT_DATA* connect;
  LOC_EXIT_OFS offset;
  u16 size, ofs, dir;

  connect = GetConnectData( work );
  size    = GetConnectOffsetSize( work );
  ofs     = GetCheckOffset( work );
  dir     = ConnectData_GetExitDirToDir( connect );

  offset = LOC_EXIT_OFS_DEF( size, ofs, dir );
  return offset;
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۂ̃I�t�Z�b�g�ő�l���擾����
 *
 * @param work
 *
 * @return �`�F�b�N�Ώۂ̐ڑ��f�[�^�̍ő�I�t�Z�b�g�l
 */
//------------------------------------------------------------------------
static u16 GetConnectOffsetSize( const CHECK_WORK* work )
{
  // �O���b�h�}�b�v�̏ꍇ
  if( CheckGridMap(work) ) {
    return GetConnectOffsetSize_GRID( work );
  }
  // ���[���}�b�v�̏ꍇ
  else {
    return GetConnectOffsetSize_RAIL( work );
  }
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۂ̃I�t�Z�b�g�ő�l���擾���� ( �O���b�h�}�b�v�p )
 *
 * @param work
 *
 * @return �`�F�b�N�Ώۂ̐ڑ��f�[�^�̍ő�I�t�Z�b�g�l
 */
//------------------------------------------------------------------------
static u16 GetConnectOffsetSize_GRID( const CHECK_WORK* work )
{
  const CONNECT_DATA* connect;
  const CONNECT_DATA_GPOS* gpos;
  u16 size;

  GF_ASSERT( CheckGridMap(work) );

  connect = GetConnectData( work );
  gpos    = (const CONNECT_DATA_GPOS*)(connect->pos_buf);

  if( 1 < gpos->sizex ) {
    size = gpos->sizex; // x �����ɕ�������
  }
  else if( 1 < gpos->sizez ) {
    size = gpos->sizez; // z �����ɕ�������
  }
  else {
    size = 1; // ���Ȃ�
  } 

  return size;
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۂ̃I�t�Z�b�g�ő�l���擾���� ( ���[���}�b�v�p )
 *
 * @param work
 *
 * @return �`�F�b�N�Ώۂ̐ڑ��f�[�^�̍ő�I�t�Z�b�g�l
 */
//------------------------------------------------------------------------
static u16 GetConnectOffsetSize_RAIL( const CHECK_WORK* work )
{ 
  const CONNECT_DATA* connect;
  const CONNECT_DATA_RPOS* rpos;
  u16 size;

  GF_ASSERT( CheckGridMap(work) == FALSE );
  
  connect = GetConnectData( work );
  rpos    = (const CONNECT_DATA_RPOS *)(connect->pos_buf);

  if (rpos->side_grid_size > 1 ) {
    size = rpos->side_grid_size;
  }
  else if (rpos->front_grid_size > 1 ) {
    size = rpos->front_grid_size;
  }
  else {
    size = 1;
  } 

  return size;
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۂ̐ڑ���]�[��ID���擾����
 *
 * @param work
 *
 * @return �`�F�b�N�Ώۂ̐ڑ��łȂ����Ă���ꏊ�̃]�[��ID
 */
//------------------------------------------------------------------------
static u16 GetConnectZoneID( const CHECK_WORK* work )
{
  const CONNECT_DATA* connect;

  connect = GetConnectData( work );
  return connect->link_zone_id;
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۂ̏o�����������ڑ��o��������ǂ����𔻒肷��
 *
 * @param work
 *
 * @return �`�F�b�N�Ώۂ̏o�����������ڑ��o�����Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//------------------------------------------------------------------------
static BOOL CheckSpecialConnect( const CHECK_WORK* work )
{
  const CONNECT_DATA* connect;

  connect = GetConnectData( work );

  return CONNECTDATA_IsSpecialExit( connect );
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۂ̏o������������o��������ǂ������`�F�b�N����
 *
 * @param work
 *
 * @return �`�F�b�N�Ώۂ̏o������������o������Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//------------------------------------------------------------------------
static BOOL CheckClosedConnect( const CHECK_WORK* work )
{
  const CONNECT_DATA* connect;

  connect = GetConnectData( work );

  return CONNECTDATA_IsClosedExit( connect );
}


//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۂ̃]�[��ID���X�V����
 *
 * @param work
 */
//------------------------------------------------------------------------
static void UpdateCheckZoneID( CHECK_WORK* work )
{
  work->check_zone_id++;

  if( ZONE_ID_MAX <= work->check_zone_id ) {
    work->check_zone_id = 0;
  }
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۂ̏o�����ID���X�V����
 *
 * @param work
 */
//------------------------------------------------------------------------
static void UpdateCheckExitID( CHECK_WORK* work )
{
  work->check_exit_id++;
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۂ̃I�t�Z�b�g���X�V����
 *
 * @param work
 */
//------------------------------------------------------------------------
static void UpdateCheckOffset( CHECK_WORK* work )
{
  work->check_offset++;
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۂ̏o�����ID�����Z�b�g����
 *
 * @param work
 */
//------------------------------------------------------------------------
static void ResetCheckExitID( CHECK_WORK* work )
{
  work->check_exit_id = 0;
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۂ̃I�t�Z�b�g�����Z�b�g����
 *
 * @param work
 */
//------------------------------------------------------------------------
static void ResetCheckOffset( CHECK_WORK* work )
{
  work->check_offset = 0;
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۂ̃]�[��ID���擾����
 *
 * @param work
 */
//------------------------------------------------------------------------
static u16 GetCheckZoneID( const CHECK_WORK* work )
{
  return work->check_zone_id;
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۂ̏o�����ID���擾����
 *
 * @param work
 */
//------------------------------------------------------------------------
static u32 GetCheckExitID( const CHECK_WORK* work )
{
  return work->check_exit_id;
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N�Ώۂ̃I�t�Z�b�g���擾����
 *
 * @param work
 */
//------------------------------------------------------------------------
static u16 GetCheckOffset( const CHECK_WORK* work )
{
  return work->check_offset;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ExitDir����Dir���擾
 *
 *	@param	cp_data 
 */
//-----------------------------------------------------------------------------
static u16 ConnectData_GetExitDirToDir( const CONNECT_DATA * cp_data )
{
  u16 dir;

  switch( cp_data->exit_dir ) {
	case EXIT_DIR_UP:    dir = DIR_UP;    break;
	case EXIT_DIR_DOWN:  dir = DIR_DOWN;  break;
	case EXIT_DIR_LEFT:  dir = DIR_LEFT;  break;
	case EXIT_DIR_RIGHT: dir = DIR_RIGHT; break;
  default:             dir = DIR_DOWN;  break;
  } 
  return dir;
}

//------------------------------------------------------------------------
/**
 * @brief �`�F�b�N���L���ȃ]�[�����ǂ����𔻒肷��
 *
 * @param zone_id
 *
 * @return �w�肵���]�[���ɑ΂���`�F�b�N���L���Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//------------------------------------------------------------------------
static BOOL CheckValidZone( u16 zone_id )
{
  int i;

  for( i=0; i<NELEMS(UncheckZoneList); i++ )
  {
    // �`�F�b�N�ΏۊO���X�g���ɔ���
    if( UncheckZoneList[i] == zone_id ) { return FALSE; }
  }

  return TRUE;
}

//------------------------------------------------------------------------
/**
 * @brief �f�o�b�O�o�� ( �S�ڑ��`�F�b�N�J�n )
 */
//------------------------------------------------------------------------
static void DebugPrint_Start( const CHECK_WORK* work )
{
  OS_TFPrintf( PRINT_TARGET, "==========================\n" );
  OS_TFPrintf( PRINT_TARGET, "ALL-CONNECT-CHECK: Start!!\n" );
  OS_TFPrintf( PRINT_TARGET, "==========================\n" );
}
//------------------------------------------------------------------------
/**
 * @brief �f�o�b�O�o�� ( ���ׂĂ̐ڑ��f�[�^ )
 */
//------------------------------------------------------------------------
static void DebugPrint_AllConnect( const CHECK_WORK* work )
{
  OS_TFPrintf( PRINT_TARGET, "---------------------------------------------\n" );
  OS_TFPrintf( PRINT_TARGET, "ALL-CONNECT-CHECK: ZoneID[%d], ConnectNum[%d]\n", GetCheckZoneID(work), GetConnectNum(work) );
  OS_TFPrintf( PRINT_TARGET, "---------------------------------------------\n" );
}
//------------------------------------------------------------------------
/**
 * @brief �f�o�b�O�o�� ( �S�ڑ��`�F�b�N�J�n )
 */
//------------------------------------------------------------------------
static void DebugPrint_CheckConnect( const CHECK_WORK* work )
{
  OS_TFPrintf( PRINT_TARGET, 
      "ALL-CONNECT-CHECK: ZoneID[%d]==>[%d], ExitID[%d], Offset[%d/%d]\n", 
      GetCheckZoneID(work), GetConnectZoneID(work), GetCheckExitID(work), GetCheckOffset(work), GetConnectOffsetSize(work) );
}
//------------------------------------------------------------------------
/**
 * @brief �f�o�b�O�o�� ( ����ڑ��o���������o )
 */
//------------------------------------------------------------------------
static void DebugPrint_DetectSpecialConnect( const CHECK_WORK* work )
{
  OS_TFPrintf( PRINT_TARGET, "********************************************\n" );
  OS_TFPrintf( PRINT_TARGET, "ALL-CONNECT-CHECK: SpecialConnect Detedted!!\n" );
  OS_TFPrintf( PRINT_TARGET, "********************************************\n" );
}
//------------------------------------------------------------------------
/**
 * @brief �f�o�b�O�o�� ( �S�ڑ��`�F�b�N�J�n )
 */
//------------------------------------------------------------------------
static void DebugPrint_Finish( const CHECK_WORK* work )
{
  OS_TFPrintf( PRINT_TARGET, "===========================\n" );
  OS_TFPrintf( PRINT_TARGET, "ALL-CONNECT-CHECK: Finish!!\n" );
  OS_TFPrintf( PRINT_TARGET, "===========================\n" );
}


#endif // PM_DEBUG
