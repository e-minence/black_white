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
#define MIN_VOLUME     (0)	// �Œቹ��
#define MAX_VOLUME   (127)	// �ő剹��
#define FADE_IN_SPEED  (8)	// �t�F�[�h�E�C�����x
#define FADE_OUT_SPEED (8)	// �t�F�[�h�E�A�E�g���x
#define TRACKBIT ((1<<(9-1))|(1<<(10-1))) // ����g���b�N(9,10)


//================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//================================================================================
static BOOL IsVecEqual( const VecFx32* p_vec1, const VecFx32* p_vec2 );
static void BootSystem( ISS_ROAD_SYS* sys );
static void StopSystem( ISS_ROAD_SYS* sys );
static void UpdateVolume( ISS_ROAD_SYS* sys );
static void AddVolume( ISS_ROAD_SYS* sys, int val );


//================================================================================
// �����HISS�V�X�e���\����
//================================================================================
struct _ISS_ROAD_SYS
{
	// �g�p����q�[�vID
	HEAPID heapID;

	// �N�����
	BOOL isActive;

	// ����
	int volume;

	// �Ď��Ώۃv���C���[
	PLAYER_WORK* pPlayer;

	// �O��Ď����̃v���C���[���W
	VecFx32 prevPlayerPos;
};


//================================================================================
// �����J�֐�
//================================================================================

//--------------------------------------------------------------------------------
/**
 * @brief  ���HISS�V�X�e�����쐬����
 *
 * @param  p_player �Ď��Ώۂ̃v���C���[
 * @param  heap_id  �g�p����q�[�vID
 * 
 * @return ���HISS�V�X�e��
 */
//--------------------------------------------------------------------------------
ISS_ROAD_SYS* ISS_ROAD_SYS_Create( PLAYER_WORK* p_player, HEAPID heap_id )
{
	ISS_ROAD_SYS* sys;

	// �������m��
	sys = (ISS_ROAD_SYS*)GFL_HEAP_AllocMemory( heap_id, sizeof( ISS_ROAD_SYS ) );

	// �����ݒ�
	sys->heapID        = heap_id;
	sys->isActive      = FALSE;
	sys->volume        = MIN_VOLUME;
	sys->pPlayer       = p_player;
	sys->prevPlayerPos = *( PLAYERWORK_getPosition( p_player ) );

  // DEBUG:
  OBATA_Printf( "ISS-R: create\n" );
	
	// �쐬�������HISS�V�X�e����Ԃ�
	return sys;
}

//--------------------------------------------------------------------------------
/**
 * @brief  ���HISS�V�X�e����j������
 *
 * @param sys �j�����铹�HISS�V�X�e�� 
 */
//--------------------------------------------------------------------------------
void ISS_ROAD_SYS_Delete( ISS_ROAD_SYS* sys )
{
	GFL_HEAP_FreeMemory( sys );

  // DEBUG:
  OBATA_Printf( "ISS-R: delete\n" );
}

//--------------------------------------------------------------------------------
/**
 * @brief �v���C���[���Ď���, ���ʂ𒲐�����
 *
 * @param sys ����Ώۂ̃V�X�e��
 */
//--------------------------------------------------------------------------------
void ISS_ROAD_SYS_Update( ISS_ROAD_SYS* sys )
{ 
	// �N�����ĂȂ�
	if( sys->isActive != TRUE ){ return; }

  // ���ʍX�V
  UpdateVolume( sys );
}

//--------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param sys �N������V�X�e��
 */
//--------------------------------------------------------------------------------
void ISS_ROAD_SYS_On( ISS_ROAD_SYS* sys )
{
  BootSystem( sys );
}

//--------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~������
 *
 * @param sys ��~������V�X�e��
 */
//--------------------------------------------------------------------------------
void ISS_ROAD_SYS_Off( ISS_ROAD_SYS* sys )
{
  StopSystem( sys );
}

//--------------------------------------------------------------------------------
/**
 * @breif �����Ԃ��擾����
 *
 * @param sys ��Ԃ𒲂ׂ�ISS�V�X�e��
 * 
 * @return ���쒆���ǂ���
 */
//--------------------------------------------------------------------------------
BOOL ISS_ROAD_SYS_IsOn( const ISS_ROAD_SYS* sys )
{
	return sys->isActive;
}



//================================================================================
// ������J�֐�
//================================================================================

//--------------------------------------------------------------------------------
/**
 * @brief 2�̃x�N�g�������������ǂ����𔻒肷��
 *
 * @param p_vec1 ���肷��x�N�g��1
 * @param p_vec2 ���肷��x�N�g��2
 *
 * @return �x�N�g�����������Ȃ� TRUE
 */
//--------------------------------------------------------------------------------
static BOOL IsVecEqual( const VecFx32* p_vec1, const VecFx32* p_vec2 )
{
	return ( ( p_vec1->x == p_vec2->x ) &&
			     ( p_vec1->y == p_vec2->y ) &&
			     ( p_vec1->z == p_vec2->z ) );
}

//--------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param sys ���HISS�V�X�e��
 */
//--------------------------------------------------------------------------------
static void BootSystem( ISS_ROAD_SYS* sys )
{
  // �N����
  if( sys->isActive ){ return; }

  // �N��
	sys->isActive = TRUE;
	sys->volume   = MIN_VOLUME;	// ���ʂ��ŏ��ɐݒ�
  PMSND_ChangeBGMVolume( TRACKBIT, MIN_VOLUME );

  // DEBUG:
  OBATA_Printf( "ISS-R: boot\n" );
}

//--------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~����
 *
 * @param sys ���HISS�V�X�e��
 */
//--------------------------------------------------------------------------------
static void StopSystem( ISS_ROAD_SYS* sys )
{
  // ��~��
  if( !sys->isActive ){ return; }

  // ��~
	sys->isActive = FALSE; 
  // ���삵�Ă����g���b�N�����ɖ߂�
  //PMSND_ChangeBGMVolume( TRACKBIT, 127 ); 

  // DEBUG:
  OBATA_Printf( "ISS-R: stop\n" );
} 

//--------------------------------------------------------------------------------
/**
 * @brief ���ʂ��X�V����
 *
 * @param sys ���HISS�V�X�e��
 */
//--------------------------------------------------------------------------------
static void UpdateVolume( ISS_ROAD_SYS* sys )
{
	const VecFx32* pos = NULL;
  int         volume = 0; // �{�����[���ω���

  GF_ASSERT( sys->isActive );
  GF_ASSERT( sys->pPlayer );

	// ��l���̍��W���擾
	pos = PLAYERWORK_getPosition( sys->pPlayer );

	// ���ʕω��ʂ�����
	if( IsVecEqual( pos, &sys->prevPlayerPos ) )
	{
		volume = -FADE_OUT_SPEED;
	}
	else
	{ 
		volume = FADE_IN_SPEED;
	}

  // ���ʂ�ύX
  AddVolume( sys, volume );

	// �L����l�����W���X�V
	sys->prevPlayerPos = *pos; 
}

//--------------------------------------------------------------------------------
/**
 * @brief ���ʂ�ύX����
 *
 * @param sys ISS-R�V�X�e��
 * @param val �ύX��
 */
//--------------------------------------------------------------------------------
static void AddVolume( ISS_ROAD_SYS* sys, int val )
{
  int prev_volume; // �ω��O�̃{�����[��

  // �N�����Ă��Ȃ�
  GF_ASSERT( sys->isActive );

  // �ω��ʃ[��
  if( val == 0 ){ return; } 
  
  // ���ʂ��X�V
  prev_volume  = sys->volume;
  sys->volume += val;
  if( sys->volume < MIN_VOLUME ){ sys->volume = MIN_VOLUME; }  // �ŏ��l�␳
  if( sys->volume > MAX_VOLUME ){ sys->volume = MAX_VOLUME; }  // �ő�l�␳
  if( sys->volume == prev_volume ){ return; }                  // �ω�����
  PMSND_ChangeBGMVolume( TRACKBIT, sys->volume );

  // DEBUG: 
  OBATA_Printf( "ISS-R: change volume ==> %d\n", sys->volume );
}
