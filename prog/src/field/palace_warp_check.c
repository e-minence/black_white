//======================================================================
/**
 * @file	palace_warl_check.c
 * @brief�@�p���X���[�v�\�`�F�b�N
 * @author	Saito
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "gamesystem/gamesystem.h"
#include "fieldmap.h"
#include "field/eventdata_system.h"
#include "field/eventdata_sxy.h"
#include "field/zonedata.h"       //for ZONEDATA_EnablePalaceUse

#include "palace_warp_check.h"

//------------------------------------------------------------------
/**
 * @brief �p���X�ւ̃��[�v�ۃ`�F�b�N
 * @param gsys  GAMESYSTEM�ւ̃|�C���^
 * @return  BOOL  TRUE�̎��A�p���X�Ƀ��[�v�\
 *
 * @note
 * �p���X���畜�A�����Ƃ��ANPC�Əd�Ȃ�ʒu�ɏo����������ł���΃��[�v�s�Ƃ���
 * �}�b�v�Ǘ��\�Ńp���X�ւ̃��[�v��������Ă��Ȃ��]�[���ł���΁A���[�v�s�Ƃ���
 */
//------------------------------------------------------------------
BOOL PLC_WP_CHK_Check(GAMESYS_WORK * gsys)
{
  GAMEDATA *gdata =  GAMESYSTEM_GetGameData(gsys);
  EVENTDATA_SYSTEM * evdata = GAMEDATA_GetEventData( gdata );
  const MMDL_HEADER * tbl = EVENTDATA_GetNpcData( evdata );
  u16 max = EVENTDATA_GetNpcCount( evdata );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
  const MMDLSYS *fos = FIELDMAP_GetMMdlSys( fieldWork );
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );

  int i;

  //�}�b�v�Ǘ��\�̓o�^���`�F�b�N
  if (ZONEDATA_EnablePalaceUse( FIELDMAP_GetZoneID( fieldWork ) ) == FALSE) return FALSE;

  { //���@�̌`�Ԃ��`�F�b�N�F�Ȃ݂̂��Ԃł̓p���X�ɂ����Ȃ�
    PLAYER_MOVE_FORM form = FIELD_PLAYER_GetMoveForm( fld_player );
    if ( form == PLAYER_MOVE_FORM_SWIM || form == PLAYER_MOVE_FORM_DIVING )
    {
      return FALSE;
    }
  }

  if (tbl == NULL || max == 0) return TRUE;

  for(i=0;i<max;i++)
  {
    if ( tbl[i].pos_type == MMDL_HEADER_POSTYPE_GRID )
    {
      s16 x,y,z;
      OBJCODE_PARAM param;
      MMDL_HEADER_GRIDPOS * pos = (MMDL_HEADER_GRIDPOS *)(tbl[i].pos_buf);
      MMDLSYS_LoadOBJCodeParam( fos, tbl[i].obj_code, &param );
      
      FIELD_PLAYER_GetGridPos( fld_player, &x, &y, &z );
      //����_�͍���
      if ( (pos->gx<=x) && (x<pos->gx+param.size_width) &&
           (pos->gz-param.size_depth<z) && (z<=pos->gz) )
      {
        NOZOMU_Printf( "Dont warp\n");
        return FALSE;
      }
    }
    else if ( tbl[i].pos_type == MMDL_HEADER_POSTYPE_RAIL )   //���[���ɏ���Ă���l���͎��������[���ɂ���Ƃ��̂݃`�F�b�N����
    {
      RAIL_LOCATION rail_loc;
      MMDL_HEADER_RAILPOS * pos = (MMDL_HEADER_RAILPOS *)(tbl[i].pos_buf);
      //�����������Ă���ʒu�����[���̂Ƃ�
      if( FIELDMAP_GetBaseSystemType( fieldWork ) == FLDMAP_BASESYS_RAIL )
      {
        FIELD_PLAYER_GetNoGridLocation( fld_player, &rail_loc );
        if ( (pos->rail_index == rail_loc.rail_index) &&
             (pos->front_grid == rail_loc.line_grid) &&
             (pos->side_grid == rail_loc.width_grid) )
        {
          NOZOMU_Printf( "Dont warp\n");
          return FALSE;
        }
      }
    }
  }

  //�_�~�[�o�n�r�`�F�b�N(�O���b�h�̂�)
  if( FIELDMAP_GetBaseSystemType( fieldWork ) == FLDMAP_BASESYS_GRID )
  {
    const POS_EVENT_DATA * pos_data;
    VecFx32 pos;
    MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
    EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
    MMDL_GetVectorPos( mmdl, &pos );
    pos_data = EVENTDATA_GetDummyPosEvent( evdata, evwork, &pos );
    if( pos_data != NULL ) //�_�~�[�C�x���g���擾�ł����炻�̏�ł͐N���s��
    {
      NOZOMU_Printf( "Dummy Pos Dont warp\n");
      return FALSE;
    }
  }

  return TRUE;
}
