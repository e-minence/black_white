/////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �v���C���[�{�����[���̃t�F�[�h�Ǘ�
 * @file   player_volume_fader.c
 * @author obata
 * @date   2010.01.27
 */
///////////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h>
#include "player_volume_fader.h"

#include "sound/wb_sound_data.sadl" // PLAYER_


#ifdef PM_DEBUG
BOOL BGMPlayerVolumeFadeEnable = TRUE; 
#endif

#define MAX_VOLUME (127)


//===================================================================================
// ���V�X�e�����[�N
//===================================================================================
struct _PLAYER_VOLUME_FADER
{
  u8  playerNo;     // �v���C���[�ԍ�
  u8  startVolume;  // �t�F�[�h�����{�����[��
  u8  endVolume;    // �t�F�[�h�ŏI�{�����[��
  u16 fadeFrame;    // �t�F�[�h �t���[����
  u16 fadeCount;    // �t�F�[�h �t���[���J�E���^
  u8 muteFlag;
};


//===================================================================================
// ������J�֐�
//===================================================================================
// �V�X�e������
static void InitFader( PLAYER_VOLUME_FADER* fader );
static void SetFadeParam( PLAYER_VOLUME_FADER* fader, u8 endVolume, u16 fadeFrame );
static void SetPlayerVolume( PLAYER_VOLUME_FADER* fader, u8 volume );
static void FaderMain( PLAYER_VOLUME_FADER* fader ); 
// �{�����[������
static int  CalcNowVolume( const PLAYER_VOLUME_FADER* fader );
static void UpdatePlayerVolume( const PLAYER_VOLUME_FADER* fader );


//===================================================================================
// ���O�����J�֐�
//===================================================================================

//-----------------------------------------------------------------------------------
/**
 * @brief �V�X�e���𐶐�����
 *
 * @param heapID   �g�p����q�[�vID
 * @param playerNo �V�X�e�������삷��v���C���[�ԍ�
 */
//-----------------------------------------------------------------------------------
PLAYER_VOLUME_FADER* PLAYER_VOLUME_FADER_Create( HEAPID heapID, u8 playerNo )
{
  PLAYER_VOLUME_FADER* fader;

  // ����
  fader = GFL_HEAP_AllocMemory( heapID, sizeof(PLAYER_VOLUME_FADER) );

  // ������
  InitFader( fader );
  fader->playerNo = playerNo;

  return fader;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �V�X�e����j������
 *
 * @param fader
 */
//-----------------------------------------------------------------------------------
void PLAYER_VOLUME_FADER_Delete( PLAYER_VOLUME_FADER* fader )
{
  GFL_HEAP_FreeMemory( fader );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �V�X�e������
 *
 * @parma fader
 */
//-----------------------------------------------------------------------------------
void PLAYER_VOLUME_FADER_Main( PLAYER_VOLUME_FADER* fader )
{
  FaderMain( fader );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �{�����[����ύX����
 *
 * @param fader
 * @param volume �ݒ肷��{�����[�� [0, 127]
 * @param frames �t�F�[�h�t���[����
 */
//-----------------------------------------------------------------------------------
void PLAYER_VOLUME_FADER_SetVolume( PLAYER_VOLUME_FADER* fader, u8 volume, u16 frames )
{
  // �����ݒ�
  if( frames == 0)
  { 
    SetPlayerVolume( fader, volume ); 
    return;
  }

  // �t�F�[�h�ݒ�
  SetFadeParam( fader, volume, frames );
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void PLAYER_VOLUME_FADER_SetMute( PLAYER_VOLUME_FADER* fader, BOOL flag )
{
  fader->muteFlag = flag;
  UpdatePlayerVolume( fader );
}

//==================================================================================
// ���V�X�e������
//==================================================================================

//----------------------------------------------------------------------------------
/**
 * @brief �V�X�e��������������
 *
 * @param fader
 */
//----------------------------------------------------------------------------------
static void InitFader( PLAYER_VOLUME_FADER* fader )
{
  fader->playerNo    = 0;
  fader->startVolume = MAX_VOLUME;
  fader->endVolume   = MAX_VOLUME;
  fader->fadeFrame   = 0;
  fader->fadeCount   = 0;
  fader->muteFlag    = FALSE;

  UpdatePlayerVolume( fader ); 
}

//----------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h �p�����[�^��ݒ肷��
 *
 * @param fader
 * @param endVolume �t�F�[�h�ŏI�l
 * @param fadeFrame �t�F�[�h �t���[����
 */
//----------------------------------------------------------------------------------
static void SetFadeParam( PLAYER_VOLUME_FADER* fader, u8 endVolume, u16 fadeFrame )
{
  fader->startVolume = CalcNowVolume( fader );
  fader->endVolume   = endVolume;
  fader->fadeFrame   = fadeFrame;
  fader->fadeCount   = 0; 
}

//----------------------------------------------------------------------------------
/**
 * @brief �v���C���[�{�����[���𑦎��ݒ肷��
 *
 * @param fader
 * @parma volume �ݒ肷��{�����[��
 */
//----------------------------------------------------------------------------------
static void SetPlayerVolume( PLAYER_VOLUME_FADER* fader, u8 volume )
{
  fader->startVolume = volume;
  fader->endVolume   = volume;
  fader->fadeFrame   = 0;
  fader->fadeCount   = 0;

  UpdatePlayerVolume( fader );
}

//----------------------------------------------------------------------------------
/**
 * @brief �V�X�e�� ���C������
 *
 * @param fader
 */
//----------------------------------------------------------------------------------
static void FaderMain( PLAYER_VOLUME_FADER* fader )
{
  if( fader->fadeCount < fader->fadeFrame )
  {
    fader->fadeCount++;
    UpdatePlayerVolume( fader );
  } 
}

//==================================================================================
// ���{�����[������
//==================================================================================

//----------------------------------------------------------------------------------
/**
 * @brief ���݂̃{�����[�����v�Z����
 *
 * @param fader
 */
//---------------------------------------------------------------------------------- 
static int CalcNowVolume( const PLAYER_VOLUME_FADER* fader )
{
  int volume;
  int startVolume, endVolume;
  int fadeFrame, fadeCount;

  startVolume = fader->startVolume;
  endVolume   = fader->endVolume;
  fadeFrame   = fader->fadeFrame;
  fadeCount   = fader->fadeCount;

  if( fadeCount == 0 )
  {
    volume = startVolume;
  }
  else
  { 
    volume = (endVolume - startVolume) * fadeCount / fadeFrame + startVolume;
  }

  GF_ASSERT( 0 <= volume );
  GF_ASSERT( volume <= MAX_VOLUME ); 
  return volume;
}

//----------------------------------------------------------------------------------
/**
 * @brief �v���C���[�̃{�����[�����X�V����
 * 
 * @param fader
 */
//----------------------------------------------------------------------------------
static void UpdatePlayerVolume( const PLAYER_VOLUME_FADER* fader )
{
  int volume;

#ifdef PM_DEBUG
  if( (fader->playerNo == PLAYER_BGM) && (BGMPlayerVolumeFadeEnable == FALSE) ) {
    return; // BGM�{�����[���̍X�V�𖳌���
  }
#endif

  volume = CalcNowVolume( fader );
  if ( fader->muteFlag ) {
    volume = 0;
  }
  NNS_SndPlayerSetPlayerVolume( fader->playerNo, volume );
}
