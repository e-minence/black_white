//////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_road_sys.h
 * @brief  ���HISS�V�X�e��
 * @author obata_toshihiro
 * @date   2009.07.29
 */
//////////////////////////////////////////////////////////////////////////////////
#include "iss_road_sys.h"
#include "sound/pm_sndsys.h"
#include "../field/field_sound.h"
#include "gamesystem/playerwork.h"


//================================================================================
// ���萔
//================================================================================
#define PRINT_DEST     (2)    // �f�o�b�O���̏o�͐�
#define MIN_VOLUME     (0)	  // �Œቹ��
#define MAX_VOLUME     (127)	// �ő剹��
#define FADE_IN_SPEED  (8)	  // �t�F�[�h �C�����x
#define FADE_OUT_SPEED (8)	  // �t�F�[�h �A�E�g���x
#define TRACKBIT ((1<<(9-1))|(1<<(10-1))) // ����g���b�N(9,10)


//================================================================================
// ������J�֐�
//================================================================================
// �V�X�e������
static void BootSystem( ISS_ROAD_SYS* system );
static void StopSystem( ISS_ROAD_SYS* system );
static void SystemMain( ISS_ROAD_SYS* system ); 
// ���@�̊Ď�
static BOOL CheckPlayerMove( ISS_ROAD_SYS* system ); 
// �{�����[������
static void VolumeUp  ( ISS_ROAD_SYS* system );
static void VolumeDown( ISS_ROAD_SYS* system );
static void ChangeTrackVolume( const ISS_ROAD_SYS* system );


//================================================================================
// �����HISS�V�X�e��
//================================================================================
struct _ISS_ROAD_SYS
{
	BOOL boot;               // �N�����Ă��邩�ǂ���
	int  targetTrackVolume;  // ����Ώۃg���b�N�̃{�����[��

	PLAYER_WORK* player;        // �Ď��Ώۃv���C���[
	VecFx32      prevPlayerPos; // �O��Ď����̃v���C���[���W
};


//================================================================================
// �����J�֐�
//================================================================================

//--------------------------------------------------------------------------------
/**
 * @brief ���HISS�V�X�e�����쐬����
 *
 * @param player �Ď��Ώۂ̃v���C���[
 * @param heapID �g�p����q�[�vID
 * 
 * @return ���HISS�V�X�e��
 */
//--------------------------------------------------------------------------------
ISS_ROAD_SYS* ISS_ROAD_SYS_Create( PLAYER_WORK* player, HEAPID heapID )
{
	ISS_ROAD_SYS* system;

	// ����
	system = (ISS_ROAD_SYS*)GFL_HEAP_AllocMemory( heapID, sizeof( ISS_ROAD_SYS ) );

	// ������
	system->boot              = FALSE;
	system->targetTrackVolume = MIN_VOLUME;
	system->player            = player;
	system->prevPlayerPos     = *( PLAYERWORK_getPosition( player ) );

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-R: create\n" );
	
	return system;
}

//--------------------------------------------------------------------------------
/**
 * @brief ���HISS�V�X�e����j������
 *
 * @param system
 */
//--------------------------------------------------------------------------------
void ISS_ROAD_SYS_Delete( ISS_ROAD_SYS* system )
{
	GFL_HEAP_FreeMemory( system );

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-R: delete\n" );
}

//--------------------------------------------------------------------------------
/**
 * @brief �V�X�e������
 *
 * @param system ����Ώۂ̃V�X�e��
 */
//--------------------------------------------------------------------------------
void ISS_ROAD_SYS_Update( ISS_ROAD_SYS* system )
{ 
  SystemMain( system );
}

//--------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param system
 */
//--------------------------------------------------------------------------------
void ISS_ROAD_SYS_On( ISS_ROAD_SYS* system )
{
  BootSystem( system );
}

//--------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~������
 *
 * @param system
 */
//--------------------------------------------------------------------------------
void ISS_ROAD_SYS_Off( ISS_ROAD_SYS* system )
{
  StopSystem( system );
}


//================================================================================
// ������J�֐�
//================================================================================

//--------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param system
 */
//--------------------------------------------------------------------------------
static void BootSystem( ISS_ROAD_SYS* system )
{
  // �N���ς�
  if( system->boot ){ return; }

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-R: boot\n" );

  // �N��
	system->boot = TRUE;
	system->targetTrackVolume = MIN_VOLUME;
  ChangeTrackVolume( system ); 
}

//--------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~����
 *
 * @param system
 */
//--------------------------------------------------------------------------------
static void StopSystem( ISS_ROAD_SYS* system )
{
  // ��~�ς�
  if( system->boot == FALSE ){ return; }

  // ��~
	system->boot = FALSE; 

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-R: stop\n" );
} 

//--------------------------------------------------------------------------------
/**
 * @brief �V�X�e������
 *
 * @param system
 */
//--------------------------------------------------------------------------------
static void SystemMain( ISS_ROAD_SYS* system )
{
	// �N�����ĂȂ�
	if( system->boot == FALSE ){ return; }

  // �{�����[���X�V
  if( CheckPlayerMove( system ) )
  {
    VolumeUp( system );
  }
  else
  {
    VolumeDown( system );
  }
}

//--------------------------------------------------------------------------------
/**
 * @brief �v���C���[�������Ă��邩�ǂ����𔻒肷��
 *
 * @param system
 */
//--------------------------------------------------------------------------------
static BOOL CheckPlayerMove( ISS_ROAD_SYS* system )
{
  BOOL moving;
	const VecFx32* playerPos;

	// ��l���̍��W���擾
	playerPos = PLAYERWORK_getPosition( system->player );

  // �����Ă��邩�ǂ���
  if( (playerPos->x == system->prevPlayerPos.x) &&
      (playerPos->y == system->prevPlayerPos.y) &&
      (playerPos->z == system->prevPlayerPos.z) )
  {
    moving = FALSE; 
  }
  else
  { 
    moving = TRUE; 
  }

  // ��l���̍��W���L��
  system->prevPlayerPos = *playerPos;

  return moving;
}

//--------------------------------------------------------------------------------
/**
 * @brief �{�����[�����グ��
 *
 * @param system
 */
//--------------------------------------------------------------------------------
static void VolumeUp( ISS_ROAD_SYS* system )
{
  int nextVolume;

  // ���łɍő�
  if( MAX_VOLUME <= system->targetTrackVolume ){ return; }

  // �X�V��̃{�����[��������
  nextVolume = system->targetTrackVolume + FADE_IN_SPEED;
  if( MAX_VOLUME < nextVolume ){ nextVolume = MAX_VOLUME; }

  // �{�����[��UP
  system->targetTrackVolume = nextVolume;
  ChangeTrackVolume( system );
}

//--------------------------------------------------------------------------------
/**
 * @brief �{�����[����������
 *
 * @param system
 */
//--------------------------------------------------------------------------------
static void VolumeDown( ISS_ROAD_SYS* system )
{
  int nextVolume;

  // ���łɍŏ�
  if( system->targetTrackVolume <= MIN_VOLUME ){ return; }

  // �X�V��̃{�����[��������
  nextVolume = system->targetTrackVolume - FADE_OUT_SPEED;
  if( nextVolume < MIN_VOLUME ){ nextVolume = MIN_VOLUME; }

  // �{�����[��DOWN
  system->targetTrackVolume = nextVolume;
  ChangeTrackVolume( system );
}

//--------------------------------------------------------------------------------
/**
 * @brief ����Ώۃg���b�N�̃{�����[�����X�V����
 *
 * @param system
 */
//--------------------------------------------------------------------------------
static void ChangeTrackVolume( const ISS_ROAD_SYS* system )
{
  GF_ASSERT( system->boot );                             // �N�����ĂȂ�
  GF_ASSERT( MIN_VOLUME <= system->targetTrackVolume );  // ���ʂ���������
  GF_ASSERT( system->targetTrackVolume <= MAX_VOLUME );  // ���ʂ���������

  // �{�����[���X�V
  PMSND_ChangeBGMVolume( TRACKBIT, system->targetTrackVolume );

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-R: change volume ==> %d\n", system->targetTrackVolume );
}
