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
#define TRACKBIT_ACTION ((1<<9)|(1<<10)) ///<�A�N�V�����pBGM Track
#define TRACKBIT_STILL (TRACKBIT_ALL^TRACKBIT_ACTION) ///<Action Track OFF

#define PUSH_MAX (1) ///<BGM�ޔ��񐔍ő�

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
static u32 fsnd_GetMapBGMIndex( FIELDMAP_WORK *fieldMap, u32 zone_id );
static void fsnd_PushCount( FIELD_SOUND *fsnd );
static void fsnd_PopCount( FIELD_SOUND *fsnd );

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
 * �t�B�[���hBGM �t�F�[�h�A�E�g �� BGM�Đ�
 * @param bgmNo �Đ�����BGM�i���o�[
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_SOUND_PlayNextBGM( u32 bgmNo )
{ 
  u32 now;
  
  if( bgmNo == 0 ){
    return;
  }
  
//  now = PMSND_GetBGMsoundNo();
  now = PMSND_GetNextBGMsoundNo();
  
  if( now != bgmNo ){
    u8 fadeOutFrame = 60; //kari
    u8 fadeInFrame = 0; //kari
    PMSND_PlayNextBGM_EX(
        bgmNo, TRACKBIT_STILL, fadeOutFrame, fadeInFrame );
  }
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
void FIELD_SOUND_ChgangeBGMTrackAction( void )
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
u32 FIELD_SOUND_GetFieldBGMNo( FIELDMAP_WORK *fieldMap, u32 zone_id )
{
  PLAYER_MOVE_FORM form;
  FIELD_PLAYER *fld_player;
  
  fld_player = FIELDMAP_GetFieldPlayer( fieldMap );
  form = FIELD_PLAYER_GetMoveForm( fld_player );
//  zone_id = FIELDMAP_GetZoneID( fieldMap );
  
  if( form == PLAYER_MOVE_FORM_SWIM ){
    return( SEQ_NAMINORI );
  }

  if( form == PLAYER_MOVE_FORM_CYCLE ){
    return( SEQ_BICYCLE );
  }
  
  return( fsnd_GetMapBGMIndex(fieldMap,zone_id) );
}

//======================================================================
//  BGM�ޔ��A���A
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
  PMSND_PauseBGM(TRUE);
  PMSND_PushBGM();
  fsnd_PushCount( fsnd );
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
  PMSND_PopBGM();
  PMSND_PauseBGM( FALSE );
  fsnd_PopCount( fsnd );
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
static u32 fsnd_GetMapBGMIndex( FIELDMAP_WORK *fieldMap, u32 zone_id )
{
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldMap );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  u16 idx = ZONEDATA_GetBGMID( zone_id, GAMEDATA_GetSeasonID(gdata) );
  return( idx );
}

//--------------------------------------------------------------
/**
 * �ޔ��񐔃J�E���g
 * @param fsnd FIELD_SOUND
 * @retval nothing
 */
//--------------------------------------------------------------
static void fsnd_PushCount( FIELD_SOUND *fsnd )
{
  fsnd->push_count++;
  GF_ASSERT(fsnd->push_count<=PUSH_MAX && "FIELD SOUND PUSH COUNT OVER");
}

//--------------------------------------------------------------
/**
 * ���A�񐔃J�E���g
 * @param fsnd FIELD_SOUND
 * @retval nothing
 */
//--------------------------------------------------------------
static void fsnd_PopCount( FIELD_SOUND *fsnd )
{
  fsnd->push_count--;
  GF_ASSERT(fsnd->push_count>0 && "FIELD SOUND POP COUNT ERROR" );
}

