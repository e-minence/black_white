//==============================================================================
/**
 * @file    intrude_monolith.c
 * @brief   ���m���X����  ��field�I�[�o�[���C�ɔz�u
 * @author  matsuda
 * @date    2009.10.29(��)
 */
//==============================================================================
#include <gflib.h>
#include "field/fieldmap.h"
#include "field/fldmmdl.h"
#include "intrude_minimono.h"
#include "field/field_const.h"
#include "intrude_work.h"
#include "intrude_main.h"
#include "intrude_field.h"
#include "intrude_monolith.h"
#include "field/zonedata.h"
#include "../../../resource/fldmapdata/script/palace01_def.h"  //SCRID_�`


//--------------------------------------------------------------
/// ���m���X���샂�f���w�b�_�[
//--------------------------------------------------------------
static const MMDL_HEADER data_MMdlHeader =
{
	99, 	          ///<����ID  fldmmdl_code.h
	                //  �X�N���v�g���X�Ԃ̓��샂�f�����A�j���A�Ƃ��������ʂɎg�p�B
	                //  ev�t�@�C����Ŕ��Ȃ���΂悢�B
	NONDRAW,	      ///<�\������OBJ�R�[�h fldmmdl_code.h
	MV_DOWN,	      ///<����R�[�h    
	EV_TYPE_NORMAL,	///<�C�x���g�^�C�v
	0,	            ///<�C�x���g�t���O  �^�C�v���Ɏg�p�����t���O�B�O������擾����
	SCRID_PALACE01_MONOLITH,///<�C�x���gID �b�����������ɋN������X�N���v�gID(*.ev �� _EVENT_DATA�̔ԍ�)
	DIR_DOWN,	      ///<�w�����  �O���b�h�ړ��Ȃ��DIR�n �m���O���b�h�̏ꍇ�͈Ⴄ�l
	0,	            ///<�w��p�����^ 0  �^�C�v���Ɏg�p�����p�����^�B�O������擾����
	0,            	///<�w��p�����^ 1
	0,	            ///<�w��p�����^ 2
	MOVE_LIMIT_NOT,	///<X�����ړ�����
	MOVE_LIMIT_NOT,	///<Z�����ړ�����
  MMDL_HEADER_POSTYPE_GRID,
  {0},            ///<�|�W�V�����o�b�t�@ ��check�@�O���b�h�}�b�v�̏ꍇ�͂�����MMDL_HEADER_GRIDPOS
};


//==================================================================
/**
 * ���m���X�̓��샂�f����Add
 *
 * @param   fieldWork		
 * @param   grid_x		�O���b�h���WX
 * @param   grid_z		�O���b�h���WZ
 * @param   fx_y		  Y���W(fx32)
 */
//==================================================================
void MONOLITH_AddMMdl(FIELDMAP_WORK *fieldWork, u16 grid_x, u16 grid_z, fx32 fx_y)
{
  MMDL_HEADER head;
  MMDL_HEADER_GRIDPOS *grid_pos;
  const MMDLSYS *mmdl_sys;
  ZONEID zone_id;
  
  mmdl_sys = FIELDMAP_GetMMdlSys( fieldWork );
  zone_id = FIELDMAP_GetZoneID( fieldWork );
  
  head = data_MMdlHeader;
  grid_pos = (MMDL_HEADER_GRIDPOS *)head.pos_buf;
  
  grid_pos->gx = grid_x;
  grid_pos->gz = grid_z;
  grid_pos->y = fx_y;
  
  MMDLSYS_AddMMdl(mmdl_sys, &head, zone_id);
  OS_TPrintf("monolith add\n");
}

//==================================================================
/**
 * ���m���X�̓��샂�f�����ʒu�����_����Add
 *
 * @param   fieldWork		
 */
//==================================================================
void MONOLITH_AddConnectAllMap(FIELDMAP_WORK *fieldWork)
{
  VecFx32 vec;
  int i, net_id;
  
  if(ZONEDATA_IsPalace( FIELDMAP_GetZoneID( fieldWork ) ) == FALSE){
    return;
  }
  
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    for(i = 0; i < PALACE_MAP_MONOLITH_NUM; i++){
      vec.x = PALACE_MAP_MONOLITH_X + FIELD_CONST_GRID_FX32_SIZE * i + PALACE_MAP_LEN * net_id;//(net_id+1);
      vec.y = PALACE_MAP_MONOLITH_Y;
      vec.z = PALACE_MAP_MONOLITH_Z;
      MONOLITH_AddMMdl(fieldWork, FX32_TO_GRID( vec.x ), FX32_TO_GRID( vec.z ), vec.y);
    }
  }
  OS_TPrintf("���m���X�o�^\n");
}
