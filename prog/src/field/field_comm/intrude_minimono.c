//==============================================================================
/**
 * @file    intrude_minimono.c
 * @brief   �~�j���m���X����  ��field�I�[�o�[���C�ɔz�u
 * @author  matsuda
 * @date    2009.10.25(��)
 */
//==============================================================================
#include <gflib.h>
#include "field/fieldmap.h"
#include "field/fldmmdl.h"
#include "intrude_minimono.h"
#include "field/field_const.h"
#include "intrude_work.h"
#include "intrude_main.h"
#include "../../../resource/fldmapdata/script/plc04_def.h"  //SCRID_�`


//--------------------------------------------------------------
/// �~�j���m���X���샂�f���w�b�_�[
//--------------------------------------------------------------
static const MMDL_HEADER data_MMdlHeader =
{
	100,	          ///<����ID  fldmmdl_code.h
	                //  �X�N���v�g���X�Ԃ̓��샂�f�����A�j���A�Ƃ��������ʂɎg�p�B
	                //  ev�t�@�C����Ŕ��Ȃ���΂悢�B
	BLACKMONOLITH,	///<�\������OBJ�R�[�h fldmmdl_code.h
	MV_DOWN,	      ///<����R�[�h    
	EV_TYPE_NORMAL,	///<�C�x���g�^�C�v
	0,	            ///<�C�x���g�t���O  �^�C�v���Ɏg�p�����t���O�B�O������擾����
	SCRID_PLC04_MINIMONO,///<�C�x���gID �b�����������ɋN������X�N���v�gID(*.ev �� _EVENT_DATA�̔ԍ�)
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
 * �~�j���m���X�̓��샂�f����Add
 *
 * @param   fieldWork		
 * @param   grid_x		�O���b�h���WX
 * @param   grid_z		�O���b�h���WZ
 * @param   fx_y		  Y���W(fx32)
 */
//==================================================================
void MINIMONO_AddMMdl(FIELDMAP_WORK *fieldWork, u16 grid_x, u16 grid_z, fx32 fx_y)
{
  MMDL_HEADER head;
  MMDL_HEADER_GRIDPOS *grid_pos;
  MMDLSYS *mmdl_sys;
  ZONEID zone_id;
  
  mmdl_sys = FIELDMAP_GetMMdlSys( fieldWork );
  zone_id = FIELDMAP_GetZoneID( fieldWork );
  
  head = data_MMdlHeader;
  if(GFUser_GetPublicRand(1) == 0){
    head.obj_code = WHITEMONOLITH;
  }
  grid_pos = (MMDL_HEADER_GRIDPOS *)head.pos_buf;
  
  grid_pos->gx = grid_x;
  grid_pos->gz = grid_z;
  grid_pos->y = fx_y;
  
  MMDLSYS_AddMMdl(mmdl_sys, &head, zone_id);
  OS_TPrintf("minimono add\n");
}

//==================================================================
/**
 * �~�j���m���X�̓��샂�f�����ʒu�����_����Add
 *
 * @param   fieldWork		
 */
//==================================================================
void MINIMONO_AddPosRand(GAME_COMM_SYS_PTR game_comm, FIELDMAP_WORK *fieldWork)
{
  VecFx32 vec;
  int town_tblno;
  
  town_tblno = Intrude_GetWarpTown(game_comm);
  if(town_tblno == PALACE_TOWN_DATA_NULL || town_tblno == PALACE_TOWN_DATA_PALACE){
    return;
  }
  
  Intrude_GetPalaceTownRandPos(town_tblno, &vec);
  vec.z -= GRID_TO_FX32( 1 );  //���@�̃��[�v�ꏊ�Ɣ��Ȃ��悤��1grid��ɂ���
  MINIMONO_AddMMdl(fieldWork, FX32_TO_GRID( vec.x ), FX32_TO_GRID( vec.z ), vec.y);
}

#ifdef PM_DEBUG
//==================================================================
/**
 * �~�j���m���X�̓��샂�f����Add
 *
 * @param   fieldWork		
 * @param   grid_x		�O���b�h���WX
 * @param   grid_z		�O���b�h���WZ
 * @param   fx_y		  Y���W(fx32)
 */
//==================================================================
static void DEBUG_INTRUDE_Pokemon_AddMMdl(FIELDMAP_WORK *fieldWork, u16 grid_x, u16 grid_z, fx32 fx_y)
{
  MMDL_HEADER head;
  MMDL_HEADER_GRIDPOS *grid_pos;
  const MMDLSYS *mmdl_sys;
  ZONEID zone_id;
  
  mmdl_sys = FIELDMAP_GetMMdlSys( fieldWork );
  zone_id = FIELDMAP_GetZoneID( fieldWork );
  
  head = data_MMdlHeader;
  if(GFUser_GetPublicRand(1) == 0){
    head.obj_code = TPOKE_0001 + GFUser_GetPublicRand0(MONSNO_ARUSEUSU) + 1;
  }
  grid_pos = (MMDL_HEADER_GRIDPOS *)head.pos_buf;
  
  grid_pos->gx = grid_x;
  grid_pos->gz = grid_z;
  grid_pos->y = fx_y;
  
  MMDLSYS_AddMMdl(mmdl_sys, &head, zone_id);
}

//==================================================================
/**
 * 
 *
 * @param   fieldWork		
 */
//==================================================================
void DEBUG_INTRUDE_Pokemon_Add(FIELDMAP_WORK *fieldWork)
{
  static u16 gx = 20;
  static u16 gz = 30;
  
  DEBUG_INTRUDE_Pokemon_AddMMdl(fieldWork, gx, gz, 0);
  gx += 1;
  if(gx % 10 == 0){
    gz += 2;
    gx -= 10;
  }
}
#endif  //-------- PM_DEBUG --------

