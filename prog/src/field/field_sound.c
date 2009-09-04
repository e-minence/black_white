//======================================================================
/**
 * @file	field_sound.c
 * @brief	�t�B�[���h�̃T�E���h�֘A�܂Ƃ�
 * @author	kagaya
 * @data	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "fieldmap.h"
#include "field/zonedata.h"

#include "field_sound.h"

//======================================================================
//  define
//======================================================================
#define TRACKBIT_ALL (0xffff) ///<�Strack ON
#define TRACKBIT_ACTION ((1<<8)|(1<<9)) ///<�A�N�V�����pBGM Track 9,10
#define TRACKBIT_STILL (TRACKBIT_ALL^TRACKBIT_ACTION) ///<Action Track OFF

#define PLAY_NEXTBGM_FADEOUT_FRAME (60)
#define PLAY_NEXTBGM_FADEIN_FRAME (0)

//--------------------------------------------------------------
/// �t�B�[���hBGM�ޔ���
//
// �t�B�[���hBGM�K�w
//                    ____ME(jingle
//          ____EVENT(event,trainer...
//  ____BASE(field
//--------------------------------------------------------------
enum
{
  FSND_PUSHCOUNT_NONE = 0, ///<�ޔ��Ȃ�
  FSND_PUSHCOUNT_BASEBGM = 1, ///<�x�[�XBGM�ޔ�
  FSND_PUSHCOUNT_EVENTBGM, ///<�C�x���gBGM�ޔ�
  FSND_PUSHCOUNT_OVER, ///<�ޔ𐔃I�[�o�[
};

//======================================================================
//  struct  
//======================================================================
//--------------------------------------------------------------
/// FIELD_SOUND
//--------------------------------------------------------------
struct _TAG_FIELD_SOUND
{
  int push_count; //BGM Push�J�E���g
};

//======================================================================
//  proto
//======================================================================
static u32 fsnd_GetMapBGMIndex( GAMEDATA *gdata, u32 zone_id );
static void fsnd_PushBGM( FIELD_SOUND *fsnd, int max );
static void fsnd_PopBGM( FIELD_SOUND *fsnd );

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
 * @param bgmNo �Đ�����BGM�i���o�[
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_SOUND_PlayNextBGM( u32 bgmNo )
{ 
  u32 now;
  
  OS_Printf( "FIELD PLAY BGMNO %d\n", bgmNo );
  
  if( bgmNo == 0 ){
    OS_Printf( "WARNING: FIELD PLAY BGMNO ZERO\n" );
    return;
  }
  
  now = PMSND_GetNextBGMsoundNo();
  
  if( now != bgmNo ){
    PMSND_PlayNextBGM_EX( bgmNo, TRACKBIT_ALL,
        PLAY_NEXTBGM_FADEOUT_FRAME, PLAY_NEXTBGM_FADEIN_FRAME );
  }
}

//--------------------------------------------------------------
/**
 * �]�[���؂�ւ�����BGM�ύX
 * @param gdata GAMEDATA
 * @param form PLAYER_MOVE_FORM
 * @param zone_id 
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_SOUND_ChangePlayZoneBGM(
    GAMEDATA *gdata, PLAYER_MOVE_FORM form, u32 zone_id )
{
  u32 no = FIELD_SOUND_GetFieldBGMNo( gdata, form, zone_id );
  FIELD_SOUND_PlayNextBGM( no );
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
  fsnd_PushBGM( fsnd, FSND_PUSHCOUNT_BASEBGM );
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

//event_debug_menu.c
//event_ircbattle.c
//event_colosseum_battle.c
//scrcmd_trainer.c
//scrcmd_musical.c

//======================================================================
//  �t�B�[���h BGM �t�F�[�h
//======================================================================
//--------------------------------------------------------------
/**
 * @brief BGM�t�F�[�h�C��
 * @param frames �t�F�[�h�C���ɗv����t���[����
 */
//--------------------------------------------------------------
void FIELD_SOUND_FadeInBGM( u16 frames )
{
  PMSND_FadeInBGM( frames );
}

//--------------------------------------------------------------
/**
 * @brief BGM�t�F�[�h�A�E�g
 * @param frames �t�F�[�h�A�E�g�ɗv����t���[����
 */
//--------------------------------------------------------------
void FIELD_SOUND_FadeOutBGM( u16 frames )
{
  PMSND_FadeOutBGM( frames );
}

//--------------------------------------------------------------
/**
 * @brief BGM�t�F�[�h���o
 * @return BGM���t�F�[�h�����ǂ���(TRUE : �t�F�[�h��)
 */
//--------------------------------------------------------------
BOOL FIELD_SOUND_IsBGMFade()
{
  return PMSND_CheckFadeOnBGM();
}

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
    return( SEQ_NAMINORI );
  }
  
  if( form == PLAYER_MOVE_FORM_CYCLE ){
    return( SEQ_BICYCLE );
  }
  
  return( fsnd_GetMapBGMIndex(gdata,zone_id) );
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * ���݂̃t�B�[���hBGM�C���f�b�N�X���擾
 * @param fieldMap FIELDMAP_WORK
 * @retval u16 BGM Index
 */
//--------------------------------------------------------------
static u32 fsnd_GetMapBGMIndex( GAMEDATA *gdata, u32 zone_id )
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
  
  PMSND_PauseBGM( TRUE );
  PMSND_PushBGM();
  fsnd->push_count++;
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
}
