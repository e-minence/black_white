//////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �d���f���� �Վ��j���[�X
 * @file   elboard_spnews_data.c
 * @author obata
 * @date   2009.12.14
 */
//////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "elboard_spnews_data.h"
#include "../../../resource/fldmapdata/zonetable/zone_id.h"


//--------------------------------------------------------------------------------------
/**
 * @brief �Վ��j���[�X�f�[�^��ǂݍ���
 * 
 * @param buf    �ǂݍ��񂾃f�[�^�̊i�[��
 * @param arc_id �ǂݍ��ރf�[�^�̃A�[�J�C�uID
 * @param dat_id �ǂݍ��ރf�[�^�̃A�[�J�C�u���C���f�b�N�X
 *
 * @return �ǂݍ��݂��������s��ꂽ�ꍇ TRUE
 */
//--------------------------------------------------------------------------------------
BOOL ELBOARD_SPNEWS_DATA_Load( ELBOARD_SPNEWS_DATA* buf, ARCID arc_id, ARCDATID dat_id )
{
  // �\���̂փ_�C���N�g�ɕ��荞��
  GFL_ARC_LoadDataOfs( buf, arc_id, dat_id, 0, sizeof(ELBOARD_SPNEWS_DATA) );

  // DEBUG:
  OBATA_Printf( "---------------------------------- ELBOARD_SPNEWS_DATA_Load\n" );
  OBATA_Printf( "flag        = %d\n", buf->flag );
  OBATA_Printf( "flagControl = %d\n", buf->flagControl );
  OBATA_Printf( "msgID       = %d\n", buf->msgID );
  OBATA_Printf( "newsType    = %d\n", buf->newsType );
  OBATA_Printf( "zoneID_1    = %d\n", buf->zoneID_1 );
  OBATA_Printf( "zoneID_2    = %d\n", buf->zoneID_2 );
  OBATA_Printf( "zoneID_3    = %d\n", buf->zoneID_3 );
  OBATA_Printf( "zoneID_4    = %d\n", buf->zoneID_4 );
  OBATA_Printf( "-----------------------------------------------------------\n" );
  return TRUE;
}

//--------------------------------------------------------------------------------------
/**
 * @brief �w�肵���]�[���Ɉ�v���邩�ǂ����𔻒肷��
 *
 * @param news    �Վ��j���[�X
 * @param zone_id ���肷��]�[��ID
 *
 * @return �w��]�[���Ɉ�v����ꍇ TRUE, �����łȂ���� FALSE
 */
//--------------------------------------------------------------------------------------
BOOL ELBOARD_SPNEWS_DATA_CheckZoneHit( const ELBOARD_SPNEWS_DATA* news, u32 zone_id )
{
  // if(�S�������l)
  if( (news->zoneID_1 == ZONE_ID_MAX) &&  
      (news->zoneID_2 == ZONE_ID_MAX) &&
      (news->zoneID_3 == ZONE_ID_MAX) && 
      (news->zoneID_4 == ZONE_ID_MAX) ){ return TRUE; }
  // else if(�ǂꂩ�Ɉ�v)
  else if( (news->zoneID_1 == zone_id) ||  
           (news->zoneID_2 == zone_id) ||
           (news->zoneID_3 == zone_id) || 
           (news->zoneID_4 == zone_id) ){ return TRUE; }
  return FALSE;
}
