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
#include "system/main.h"      //for HEAPID_
#include "arc/fieldmap/zone_id.h" //for WC10 BC10

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
  s16 x,y,z;
  int i;

  int zone_id = FIELDMAP_GetZoneID( fieldWork );

  //�}�b�v�Ǘ��\�̓o�^���`�F�b�N
  if (ZONEDATA_EnablePalaceUse( zone_id ) == FALSE) return FALSE;

  { //���@�̌`�Ԃ��`�F�b�N�F�Ȃ݂̂��Ԃł̓p���X�ɂ����Ȃ�
    PLAYER_MOVE_FORM form = FIELD_PLAYER_GetMoveForm( fld_player );
    if ( form == PLAYER_MOVE_FORM_SWIM || form == PLAYER_MOVE_FORM_DIVING )
    {
      return FALSE;
    }
  }

  if (tbl == NULL || max == 0)
  {
    if ( (zone_id != ZONE_ID_BC10)&&(zone_id != ZONE_ID_WC10) )
    {
      return TRUE;
    }
  }

  for(i=0;i<max;i++)
  {
    if ( tbl[i].pos_type == MMDL_HEADER_POSTYPE_GRID )
    {
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

  //�z���C�g�t�H���X�g�E�u���b�N�V�e�B�̂Ƃ��̃`�F�b�N
  if ( (zone_id == ZONE_ID_BC10)||(zone_id == ZONE_ID_WC10)  )
  {
    int i;
    FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader;
    FIELD_WFBC_CORE_TYPE type;
    HEAPID heap_id;
    FIELD_WFBC_CORE_PEOPLE_POS* ppos;
    
    heap_id = HEAPID_FIELDMAP;
    FIELD_PLAYER_GetGridPos( fld_player, &x, &y, &z );
    //�ǂݍ���
    //type =   FIELD_WFBC_CORE_TYPE_BLACK_CITY,    // �u���b�N�V�e�B
    //         FIELD_WFBC_CORE_TYPE_WHITE_FOREST,  // �z���C�g�t�H���X�g
    if (zone_id == ZONE_ID_BC10) type = FIELD_WFBC_CORE_TYPE_BLACK_CITY;
    else type = FIELD_WFBC_CORE_TYPE_WHITE_FOREST;
    ppos = FIELD_WFBC_PEOPLE_POS_Create( p_loader, type, GFL_HEAP_LOWID(heap_id) );

    //�f�[�^�擾 index�̍ő�l��FIELD_WFBC_PEOPLE_MAX
    for (i=0;i<FIELD_WFBC_PEOPLE_MAX;i++)
    {
      const FIELD_WFBC_CORE_PEOPLE_POS* pos;
      pos = FIELD_WFBC_PEOPLE_POS_GetIndexData( ppos, i );
      if ( (pos->gx == x)&&(pos->gz == z) )
      {
        NOZOMU_Printf( "Dont warp\n");
        return FALSE;
      }
    }
    for (i=0;i<FIELD_WFBC_PEOPLE_MAX;i++)
    {
      const FIELD_WFBC_CORE_PEOPLE_POS* pos;
      pos = FIELD_WFBC_PEOPLE_POS_GetIndexItemPos( ppos, i );
      if ( (pos->gx == x)&&(pos->gz == z) )
      {
        NOZOMU_Printf( "Dont warp\n");
        return FALSE;
      }
    }
    //�j��
    FIELD_WFBC_PEOPLE_POS_Delete( ppos );
  }

  return TRUE;
}

#ifdef PM_DEBUG

#include "eventdata_local.h"

extern const POS_EVENT_DATA * D_EVENTDATA_GetPosEventData( const EVENTDATA_SYSTEM *evdata );


void PLC_WP_CHK_CheckDebugPrint(GAMESYS_WORK * gsys)
{
  GAMEDATA *gdata =  GAMESYSTEM_GetGameData(gsys);
  EVENTDATA_SYSTEM * evdata = GAMEDATA_GetEventData( gdata );
  const MMDL_HEADER * tbl = EVENTDATA_GetNpcData( evdata );
  u16 max = EVENTDATA_GetNpcCount( evdata );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
  const MMDLSYS *fos = FIELDMAP_GetMMdlSys( fieldWork );
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );

  int i;
  u32 debug_rail_obj;
  s16 x,y,z, w,h;
  int zone_id = FIELDMAP_GetZoneID( fieldWork );

  //�}�b�v�Ǘ��\�̓o�^���`�F�b�N
  if (ZONEDATA_EnablePalaceUse( zone_id ) == FALSE) return;

  if (tbl == NULL || max == 0)
  {
    if ( (zone_id != ZONE_ID_BC10)&&(zone_id != ZONE_ID_WC10) )
    {
      return;
    }
  }


  OS_FPrintf( 2,"===========ZONE_ID %d==============\n",FIELDMAP_GetZoneID( fieldWork ));
  debug_rail_obj = 0;
  for(i=0;i<max;i++)
  {
    if ( tbl[i].pos_type == MMDL_HEADER_POSTYPE_GRID )
    {
      OBJCODE_PARAM param;
      MMDL_HEADER_GRIDPOS * pos = (MMDL_HEADER_GRIDPOS *)(tbl[i].pos_buf);
      MMDLSYS_LoadOBJCodeParam( fos, tbl[i].obj_code, &param );
      
      //����_�͍���
      for(w=0;w<param.size_width;w++){
        x = pos->gx+w;
        for(h=0;h<param.size_depth;h++){
          z = pos->gz-h;
          OS_FPrintf( 2,"OBJ_Grid %d,%d\n",x, z);
        }
      }
    }
    else if ( tbl[i].pos_type == MMDL_HEADER_POSTYPE_RAIL )   //���[���ɏ���Ă���l���͎��������[���ɂ���Ƃ��̂݃`�F�b�N����
    {
      RAIL_LOCATION rail_loc;
      MMDL_HEADER_RAILPOS * pos = (MMDL_HEADER_RAILPOS *)(tbl[i].pos_buf);
      {
        OS_FPrintf( 2,"RAIL %d,%d,%d\n",pos->rail_index,pos->front_grid,pos->side_grid);
        debug_rail_obj++;
      }
    }
  }

  OS_FPrintf( 2,"RAIL_OBJ_NUM = %d\n",debug_rail_obj);

  //�_�~�[�o�n�r�`�F�b�N(�O���b�h�̂�)
  if( FIELDMAP_GetBaseSystemType( fieldWork ) == FLDMAP_BASESYS_GRID )
  {
    const POS_EVENT_DATA * pos_data;
    VecFx32 pos;
    MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
    EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
    MMDL_GetVectorPos( mmdl, &pos );
    pos_data = EVENTDATA_GetDummyPosEvent( evdata, evwork, &pos );
    {
      const POS_EVENT_DATA *data = D_EVENTDATA_GetPosEventData( evdata );
      if( data != NULL )
      {
        u16 i;
        u16 *work_val;
        u16 max = EVENTDATA_GetPosEventNum(evdata);
        for( i=0; i < max; i++, data++ )
        {
          BOOL valid = FALSE;
          if ( data->check_type != POS_CHECK_TYPE_DUMMY ) continue;

          //���[�N�w�肪�O�̂Ƃ��A��������POS����
          if ( data->workID == 0 ) valid = TRUE;
          else OS_FPrintf( 2,"�_�~�[�Ȃ̂ɒl���ݒ肳��Ă���\n");

          work_val = EVENTWORK_GetEventWorkAdrs( evwork, data->workID );
          if( (*work_val) == data->param ) valid = TRUE;

          if (valid)
          {
            //���W�\��
            const POS_EVENT_DATA_GPOS* cp_gpos;
            int gx, gz, y;

            if( data->pos_type == EVENTDATA_POSTYPE_RAIL )
            {
              OS_FPrintf( 2,"�C�x���g�f�[�^�����[���ݒ�\n");
              continue;
            }
            cp_gpos = (const POS_EVENT_DATA_GPOS*)data->pos_buf; 

            OS_FPrintf( 2,"EVT %d,%d,%d\n",cp_gpos->gx, cp_gpos->height, cp_gpos->gz);
          }
        }
      }
    }
  }

  //�z���C�g�t�H���X�g�E�u���b�N�V�e�B�̂Ƃ��̃`�F�b�N
  if ( (zone_id == ZONE_ID_BC10)||(zone_id == ZONE_ID_WC10)  )
  {
    int i;
    FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader;
    FIELD_WFBC_CORE_TYPE type;
    HEAPID heap_id;
    FIELD_WFBC_CORE_PEOPLE_POS* ppos;
    
    heap_id = HEAPID_FIELDMAP;
    FIELD_PLAYER_GetGridPos( fld_player, &x, &y, &z );
    //�ǂݍ���
    // ���[�_�[����
    p_loader = FIELD_WFBC_PEOPLE_DATA_Create( 0, GFL_HEAP_LOWID(heap_id) );
    //type =   FIELD_WFBC_CORE_TYPE_BLACK_CITY,    // �u���b�N�V�e�B
    //         FIELD_WFBC_CORE_TYPE_WHITE_FOREST,  // �z���C�g�t�H���X�g
    if (zone_id == ZONE_ID_BC10) type = FIELD_WFBC_CORE_TYPE_BLACK_CITY;
    else type = FIELD_WFBC_CORE_TYPE_WHITE_FOREST;
    ppos = FIELD_WFBC_PEOPLE_POS_Create( p_loader, type, GFL_HEAP_LOWID(heap_id) );

    //�f�[�^�擾 index�̍ő�l��FIELD_WFBC_PEOPLE_MAX
    for (i=0;i<FIELD_WFBC_PEOPLE_MAX;i++)
    {
      const FIELD_WFBC_CORE_PEOPLE_POS* pos;
      pos = FIELD_WFBC_PEOPLE_POS_GetIndexData( ppos, i );
      OS_FPrintf( 2,"WB %d,%d\n",pos->gx, pos->gz);
    }
    for (i=0;i<FIELD_WFBC_PEOPLE_MAX;i++)
    {
      const FIELD_WFBC_CORE_PEOPLE_POS* pos;
      pos = FIELD_WFBC_PEOPLE_POS_GetIndexItemPos( ppos, i );
      OS_FPrintf( 2,"WB %d,%d\n",pos->gx, pos->gz);
    }
    //�j��
    FIELD_WFBC_PEOPLE_POS_Delete( ppos );

    FIELD_WFBC_PEOPLE_DATA_Delete( p_loader );
  }
}
#endif
