//======================================================================
/**
 * @file	field_sound.c
 * @brief	�t�B�[���h�̃T�E���h�֘A�܂Ƃ�
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "sound/pm_sndsys.h"

#include "fieldmap.h"
#include "field/zonedata.h"

#include "field_sound.h"

//======================================================================
//  define
//======================================================================
#define TRACKBIT_ALL (0xffff) ///<�Strack ON
//#define TRACKBIT_ACTION ((1<<8)|(1<<9)) ///<�A�N�V�����pBGM Track 9,10
//#define TRACKBIT_STILL (TRACKBIT_ALL^TRACKBIT_ACTION) ///<Action Track OFF


// FIELD_SOUND�t�F�[�h�Ď����
typedef enum{
  FS_STATE_PLAY,      // �Đ���
  FS_STATE_STOP,      // ��~��
  FS_STATE_FADE_IN,   // �t�F�[�h�C����
  FS_STATE_FADE_OUT,  // �t�F�[�h�A�E�g��
} FS_STATE;


//======================================================================
//  struct  
//======================================================================
//--------------------------------------------------------------
/// FIELD_SOUND
//--------------------------------------------------------------
struct _TAG_FIELD_SOUND
{
  u16 play_event_bgm; // �C�x���gBGM�Đ���
  s16 push_count; // BGM Push�J�E���g

  FS_STATE   state;  // ���݂̏��
  u32      nextBGM;  // ���ɍĐ�����BGM
  u16  fadeInFrame;  // �t�F�[�h�C���t���[���� 
  u16 fadeOutFrame;  // �t�F�[�h�A�E�g�t���[����
  BOOL   reqFadeIn;  // ���N�G�X�g(�t�F�[�h�C��) 
  BOOL  reqFadeOut;  // ���N�G�X�g(�t�F�[�h�A�E�g) 
  BOOL      reqPop;  // ���N�G�X�g(�|�b�v)
  BOOL     reqPush;  // ���N�G�X�g(�v�b�V��)
};

//======================================================================
//  proto
//======================================================================
static u32 fsnd_GetMapBGM( GAMEDATA *gdata, u32 zone_id );
static void fsnd_PushBGM( FIELD_SOUND *fsnd, int max );
static void fsnd_PopBGM( FIELD_SOUND *fsnd );
static void fsnd_UpdateBGM_PLAY( FIELD_SOUND* fsnd );
static void fsnd_UpdateBGM_STOP( FIELD_SOUND* fsnd );
static void fsnd_UpdateBGM_FADE_IN( FIELD_SOUND* fsnd );
static void fsnd_UpdateBGM_FADE_OUT( FIELD_SOUND* fsnd );

//======================================================================
//  FILED_SOUND
//======================================================================
//--------------------------------------------------------------
/**
 * FIELD_SOUND FIELD_SOUND�쐬
 * @param heapID HEAPID
 * @retval FIELD_SOUND*
 */
//--------------------------------------------------------------
FIELD_SOUND * FIELD_SOUND_Create( HEAPID heapID )
{
  FIELD_SOUND *fsnd;
  fsnd = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_SOUND) );
  fsnd->state      = FS_STATE_STOP;
  fsnd->nextBGM    = 0;
  fsnd->reqFadeIn  = FALSE;
  fsnd->reqFadeOut = FALSE;
  fsnd->reqPop     = FALSE;
  fsnd->reqPush    = FALSE;
  return( fsnd );
}

//--------------------------------------------------------------
/**
 * FIELD_SOUND FIELDSOUND�폜
 * @param heapID HEAPID
 * @retval FIELD_SOUND*
 */
//--------------------------------------------------------------
void FIELD_SOUND_Delete( FIELD_SOUND *fsnd )
{
  GFL_HEAP_FreeMemory( fsnd );
}

//======================================================================
//  �t�B�[���h BGM �Đ�
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���hBGM  BGM�Đ�
 * @param bgmNo �Đ�����BGM�i���o�[
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_SOUND_PlayBGM( u32 bgmNo )
{ 
  u32 now;
  
  OS_Printf( "FIELD PLAY BGMNO %d\n", bgmNo );
  
  if( bgmNo == 0 ){
    OS_Printf( "WARNING: FIELD PLAY BGMNO ZERO\n" );
    return;
  }
  
  now = PMSND_GetNextBGMsoundNo();
  
  if( now != bgmNo ){
    PMSND_PlayBGM_EX( bgmNo, TRACKBIT_ALL );
  }
}

//--------------------------------------------------------------
/**
 * �t�B�[���hBGM �t�F�[�h�A�E�g �� BGM�Đ�
 * @param fsnd         �t�B�[���h�T�E���h
 * @param bgmNo        �Đ�����BGM�i���o�[
 * @param fadeOutFrame �t�F�[�h�A�E�g�t���[����
 * @param fadeInFrame  �t�F�[�h�C���t���[����
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_SOUND_PlayNextBGM_Ex( FIELD_SOUND* fsnd, u32 bgmNo, 
                                 u16 fadeOutFrame, u16 fadeInFrame )
{ 
  // ���炩�̃��N�G�X�g������ꍇ, BGM�ύX���N�G�X�g�͖����Ƃ���
  if( fsnd->reqFadeIn || fsnd->reqFadeOut || fsnd->reqPop || fsnd->reqPush )
  {
    return;
  } 
  // ���N�G�X�g���X�V
  fsnd->nextBGM      = bgmNo;
  fsnd->fadeInFrame  = fadeInFrame; 
  fsnd->fadeOutFrame = fadeOutFrame; 
}

//--------------------------------------------------------------
/**
 * �]�[���؂�ւ�����BGM�ύX
 * @param fsnd FIELD_SOUND
 * @param gdata GAMEDATA
 * @param form PLAYER_MOVE_FORM
 * @param zone_id 
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_SOUND_ChangePlayZoneBGM( FIELD_SOUND* fsnd, GAMEDATA* gdata, 
                                    PLAYER_MOVE_FORM form, u32 zone_id )
{
  // ���@�t�H�[���Ȃǂ��l����, �w��]�[���ōĐ����ׂ�BGM������
  u32 no = FIELD_SOUND_GetFieldBGMNo( gdata, form, zone_id );
  FIELD_SOUND_PlayNextBGM( fsnd, no );
}

//--------------------------------------------------------------
/**
 * ���݂�BGM��ޔ����A�C�x���g�p��BGM���Đ�
 * @param fsnd FIELD_SOUND
 * @param bgmno BGM�ԍ�
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_SOUND_PushPlayEventBGM( FIELD_SOUND *fsnd, u32 bgmno )
{
  if( fsnd->push_count <= FSND_PUSHCOUNT_BASEBGM ){ //EVENT BGM�Ɉڍs�ς݂�
    fsnd_PushBGM( fsnd, FSND_PUSHCOUNT_BASEBGM );
  }
  FIELD_SOUND_PlayBGM( bgmno );
}

//--------------------------------------------------------------
/**
 * ���݂�BGM��ޔ����AME�p��BGM���Đ�
 * @param fsnd FIELD_SOUND
 * @param bgmno BGM�ԍ�
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_SOUND_PushPlayJingleBGM( FIELD_SOUND *fsnd, u32 bgmno )
{
  fsnd_PushBGM( fsnd, FSND_PUSHCOUNT_EVENTBGM );
  FIELD_SOUND_PlayBGM( bgmno );
}

//======================================================================
//  �t�B�[���hBGM �ޔ��A���A
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���hBGM �ޔ�
 * @param FIELD_SOUND *fsnd
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_SOUND_PushBGM( FIELD_SOUND *fsnd )
{
  fsnd_PushBGM( fsnd, FSND_PUSHCOUNT_BASEBGM );
}

//--------------------------------------------------------------
/**
 * �t�B�[���hBGM ���A
 * @param fsnd FIELD_SOUND
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_SOUND_PopBGM( FIELD_SOUND *fsnd )
{
  fsnd_PopBGM( fsnd );
}

//--------------------------------------------------------------
/**
 * �t�B�[���hBGM �ޔ�����Ă���΋����J���B
 * @param fsnd FIELD_SOUND
 * @retval nothing
 * @note �G���[���p�Ȃ̂Ŋ�{�͎g�p�֎~�B
 */
//--------------------------------------------------------------
void FIELD_SOUND_ForcePopBGM( FIELD_SOUND *fsnd )
{
  if( fsnd->push_count ){
    GF_ASSERT( 0 && "ERROR: FSOUND FORGET POP\n" );
    
    while( fsnd->push_count ){
      fsnd_PopBGM( fsnd );
      fsnd->push_count--;
    }
  }
}

//event_debug_menu.c
//event_ircbattle.c
//event_colosseum_battle.c
//scrcmd_trainer.c
//scrcmd_musical.c


//======================================================================
// �t�B�[���hBGM �t�F�[�h�Ǘ�
//======================================================================

//----------------------------------------------------------------------
/**
 * @brief BGM�̃t�F�[�h��Ԃ��Ǘ���, BGM�̕ύX���s��
 *
 * @param fsnd FIELD_SOUND
 */
//----------------------------------------------------------------------
void FIELD_SOUND_UpdateBGM( FIELD_SOUND* fsnd )
{
  switch( fsnd->state )
  {
  case FS_STATE_PLAY:     fsnd_UpdateBGM_PLAY( fsnd );     break;
  case FS_STATE_FADE_OUT: fsnd_UpdateBGM_FADE_OUT( fsnd ); break;
  case FS_STATE_STOP:     fsnd_UpdateBGM_STOP( fsnd );     break;
  case FS_STATE_FADE_IN:  fsnd_UpdateBGM_FADE_IN( fsnd );  break;
  }
}

//----------------------------------------------------------------------
/**
 * @brief BGM�t�F�[�h�̊Ǘ�(�Đ���)
 */
//----------------------------------------------------------------------
static void fsnd_UpdateBGM_PLAY( FIELD_SOUND* fsnd )
{
  u32 now_bgm = PMSND_GetNextBGMsoundNo();

  // BGM�ύX or �t�F�[�h�A�E�g
  if( (now_bgm != fsnd->nextBGM) || ( fsnd->reqFadeOut ) )
  {
    PMSND_FadeOutBGM( fsnd->fadeOutFrame );
    fsnd->state = FS_STATE_FADE_OUT; 
    // DEBUG:
    OBATA_Printf( "FIELD_SOUND: fade out\n" );
  } 
}
//----------------------------------------------------------------------
/**
 * @brief BGM�t�F�[�h�̊Ǘ�(�t�F�[�h�A�E�g��)
 */
//----------------------------------------------------------------------
static void fsnd_UpdateBGM_FADE_OUT( FIELD_SOUND* fsnd )
{
  if( PMSND_CheckFadeOnBGM() != TRUE )  // if(�t�F�[�h�I��)
  { 
    if( fsnd->reqPush )  // if(�v�b�V�����N�G�X�g)
    { 
      fsnd_PushBGM( fsnd, FSND_PUSHCOUNT_BASEBGM );
    } 
    fsnd->reqFadeOut = FALSE;
    fsnd->state      = FS_STATE_STOP;
    // DEBUG:
    OBATA_Printf( "FIELD_SOUND: stop\n" );
  }
}
//----------------------------------------------------------------------
/**
 * @brief BGM�t�F�[�h�̊Ǘ�(��~��)
 */
//----------------------------------------------------------------------
static void fsnd_UpdateBGM_STOP( FIELD_SOUND* fsnd )
{
  BOOL fade_start = FALSE;
  u32  now_bgm    = PMSND_GetNextBGMsoundNo();

  // Pop���N�G�X�g�L ==> �t�F�[�h�C���J�n
  if( fsnd->reqPop )
  { // BGM���A
    fsnd_PopBGM( fsnd );
    PMSND_FadeInBGM( fsnd->fadeInFrame );
    fade_start = TRUE;
  }
  // BGM�ύX���N�G�X�g�L
  else if( fsnd->nextBGM && fsnd->nextBGM != now_bgm )
  { 
    PMSND_PlayNextBGM( fsnd->nextBGM, 0, fsnd->fadeInFrame );
    fade_start = TRUE;
  }
  // �t�F�[�h�C�����N�G�X�g�L ==> �t�F�[�h�C���J�n
  else if( fsnd->reqFadeIn )
  { 
    PMSND_FadeInBGM( fsnd->fadeInFrame );
    fade_start = TRUE;
  }

  // �t�F�[�h�C���J�n
  if( fade_start )
  {
    fsnd->state = FS_STATE_FADE_IN;
    // DEBUG:
    OBATA_Printf( "FIELD_SOUND: fade in\n" );
  }
}
//----------------------------------------------------------------------
/**
 * @brief BGM�t�F�[�h�̊Ǘ�(�t�F�[�h�C����)
 */
//----------------------------------------------------------------------
static void fsnd_UpdateBGM_FADE_IN( FIELD_SOUND* fsnd )
{
  // �t�F�[�h�I�� ==> �Đ���
  if( PMSND_CheckFadeOnBGM() != TRUE )
  {
    fsnd->reqFadeIn = FALSE;
    fsnd->state     = FS_STATE_PLAY;
    // DEBUG:
    OBATA_Printf( "FIELD_SOUND: play\n" );
  }
}


//======================================================================
//  �t�B�[���h BGM �t�F�[�h
//======================================================================
//--------------------------------------------------------------
/**
 * @brief BGM�t�F�[�h�C��
 * @param fsnd   FIELD_SOUND
 * @param bgmNo  �Đ�����BGM
 * @param frames �t�F�[�h�C���ɗv����t���[����
 */
//--------------------------------------------------------------
void FIELD_SOUND_FadeInBGM( FIELD_SOUND* fsnd, u32 bgmNo, u16 frames )
{
  fsnd->state       = FS_STATE_STOP;
  fsnd->nextBGM     = bgmNo;
  fsnd->fadeInFrame = frames;
  fsnd->reqFadeIn   = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief BGM Pop ==> �t�F�[�h�C��
 * @param fsnd   FIELD_SOUND
 * @param frames �t�F�[�h�C���ɗv����t���[����
 */
//--------------------------------------------------------------
void FIELD_SOUND_FadeInPopBGM( FIELD_SOUND* fsnd, u16 frames )
{
  FIELD_SOUND_FadeInBGM( fsnd, 0, frames );
  fsnd->reqPop = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief BGM�t�F�[�h�A�E�g
 * @param fsnd   FIELD_SOUND
 * @param frames �t�F�[�h�A�E�g�ɗv����t���[����
 */
//--------------------------------------------------------------
void FIELD_SOUND_FadeOutBGM( FIELD_SOUND* fsnd, u16 frames )
{
  fsnd->state        = FS_STATE_PLAY;
  fsnd->fadeOutFrame = frames;
  fsnd->reqFadeOut   = TRUE;
  //FIELD_SOUND_PlayNextBGM_Ex( fsnd, 0, frames, 0 );
}

//--------------------------------------------------------------
/**
 * @brief BGM�t�F�[�h�A�E�g ==> BGM Push
 * @param fsnd   FIELD_SOUND
 * @param frames �t�F�[�h�A�E�g�ɗv����t���[����
 */
//--------------------------------------------------------------
void FIELD_SOUND_FadeOutPushBGM( FIELD_SOUND* fsnd, u16 frames )
{
  FIELD_SOUND_FadeOutBGM( fsnd, frames );
  fsnd->reqPush = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief BGM�t�F�[�h���o
 * @return BGM���t�F�[�h�����ǂ���(TRUE : �t�F�[�h��)
 */
//--------------------------------------------------------------
BOOL FIELD_SOUND_IsBGMFade( FIELD_SOUND* fsnd )
{
  if( (fsnd->state == FS_STATE_FADE_IN) || 
      (fsnd->state == FS_STATE_FADE_OUT) )
  { 
    return TRUE;
  }
  return FALSE;
}

#if 0
//======================================================================
//  �t�B�[���h BGM �g���b�N�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h BGM ���@�ړ����̃g���b�N�ɕύX
 * @param nothing
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_SOUND_ChangeBGMTrackAction( void )
{
	u16 trackBit = TRACKBIT_ALL;
	PMSND_ChangeBGMtrack( trackBit );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h BGM ���@��~���̃g���b�N�ɕύX
 * @param nothing
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_SOUND_ChangeBGMTrackStill( void )
{
	u16 trackBit = TRACKBIT_STILL;
	PMSND_ChangeBGMtrack( trackBit );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h BGM �{�����[���ύX
 * @param vol BGM�{�����[�� 0-127
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_SOUND_ChangeBGMVolume( int vol )
{
	PMSND_ChangeBGMVolume( TRACKBIT_ALL, vol );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h BGM �A�N�V�����p�g���b�N�{�����[���ύX
 * @param vol BGM�{�����[�� 0-127
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_SOUND_ChangeBGMActionVolume( int vol )
{
	PMSND_ChangeBGMVolume( TRACKBIT_ACTION, vol );
}
#endif


//======================================================================
//  �t�B�[���h BGM BGM�i���o�[
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h BGM �Đ�����BGM�i���o�[�擾
 * @param fieldMap FIELDMAP_WORK *
 * @param zone_id �]�[��ID
 * @retval u16 �t�B�[���hBGM�i���o�[
 */
//--------------------------------------------------------------
u32 FIELD_SOUND_GetFieldBGMNo(
    GAMEDATA *gdata, PLAYER_MOVE_FORM form, u32 zone_id )
{
  if( form == PLAYER_MOVE_FORM_SWIM ){
    return( SEQ_BGM_NAMINORI );
  }
  
  if( form == PLAYER_MOVE_FORM_CYCLE ){
    return( SEQ_BGM_BICYCLE );
  }
  
  return( fsnd_GetMapBGM(gdata,zone_id) );
}

//======================================================================
//  �g���[�i�[����BGM
//======================================================================
//--------------------------------------------------------------
/// ����BGM�Ή��f�[�^
//--------------------------------------------------------------
#include "tr_tool/trtype_def.h"
#include "../../../resource/trtype_bgm/trtype_bgm.cdat"

//--------------------------------------------------------------
/**
 * �g���[�i�[�����Ȃ��Đ����܂��B
 * @param trtype �g���[�i�[�^�C�v
 * @retval u32 BGM�ԍ�
 */
//--------------------------------------------------------------
u32 FIELD_SOUND_GetTrainerEyeBgmNo( u32 trtype )
{
  int i = 0;
  while( DATA_TrainerTypeToEyeBGMNo[i][0] != TRTYPE_MAX ){
    if( DATA_TrainerTypeToEyeBGMNo[i][0] == trtype ){
      return( DATA_TrainerTypeToEyeBGMNo[i][1]  );
    }
    i++;
  }
  
  OS_Printf( "ERROR:UNKNOW TRAINER TYPE BGM\n" );
  GF_ASSERT( 0 );
  return( SEQ_BGM_EYE_01 );
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * BGM�v�b�V���񐔂��擾
 * @param fsnd FIELD_SOUND
 * @retval FSND_PUSHCOUNT
 */
//--------------------------------------------------------------
FSND_PUSHCOUNT FIELD_SOUND_GetBGMPushCount( const FIELD_SOUND *fsnd )
{
  return( fsnd->push_count );
}

//--------------------------------------------------------------
/**
 * ���݂̃t�B�[���hBGM�C���f�b�N�X���擾
 * @param fieldMap FIELDMAP_WORK
 * @retval u16 BGM Index
 */
//--------------------------------------------------------------
static u32 fsnd_GetMapBGM( GAMEDATA *gdata, u32 zone_id )
{
  u16 idx = ZONEDATA_GetBGMID( zone_id, GAMEDATA_GetSeasonID(gdata) );
  return( idx );
}

//--------------------------------------------------------------
/**
 * BGM�ޔ�
 * @param fsnd FIELD_SOUND
 * @param max ������ƃG���[�ƂȂ�ޔ𐔍ő�
 * @retval nothing
 */
//--------------------------------------------------------------
static void fsnd_PushBGM( FIELD_SOUND *fsnd, int max )
{
  int count = fsnd->push_count + 1;
  
  if( count > max || count >= FSND_PUSHCOUNT_OVER ){
    GF_ASSERT( 0 && "ERROR:FSOUND PUSH COUNT OVER" );
    return;
  }
  
  if( fsnd->state == FS_STATE_FADE_OUT )
  {
    PMSND_PauseBGM( FALSE );
  }
  else
  {
    PMSND_PauseBGM( TRUE );
  } 
  PMSND_PushBGM();
  fsnd->push_count++;
  // �Ď���Ԃ��~
  fsnd->state      = FS_STATE_STOP;
  fsnd->nextBGM    = 0;
  fsnd->reqFadeIn  = FALSE;
  fsnd->reqFadeOut = FALSE;
  fsnd->reqPush    = FALSE;
  fsnd->reqPop     = FALSE;

  // DEBUG:
  OBATA_Printf( "FIELD_SOUND: push BGM\n" );
}

//--------------------------------------------------------------
/**
 * BGM���A
 * @param fsnd FIELD_SOUND
 * @retval nothing
 */
//--------------------------------------------------------------
static void fsnd_PopBGM( FIELD_SOUND *fsnd )
{
  int count = fsnd->push_count - 1;
  
  if( count < 0 ){
    GF_ASSERT( 0 && "ERROR:FSOUND POP COUNT ERROR" );
    return;
  }
  
  PMSND_PopBGM();
  PMSND_PauseBGM( FALSE );
  fsnd->push_count--;
  // �Ď���Ԃ𕜋A
  fsnd->state      = FS_STATE_PLAY;
  fsnd->nextBGM    = PMSND_GetBGMsoundNo();
  fsnd->reqFadeIn  = FALSE;
  fsnd->reqFadeOut = FALSE;
  fsnd->reqPush    = FALSE;
  fsnd->reqPop     = FALSE;

  // DEBUG:
  OBATA_Printf( "FIELD_SOUND: pop BGM\n" );
}
