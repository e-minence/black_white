//==============================================================================
/**
 * @file    symbol_pokemon.c
 * @brief   �V���{���|�P�����z�u����
 * @author  matsuda
 * @date    2009.10.25(��)
 *
 * 2010.03.21 tamada  �~�j���m���X�p��ύX
 *
 * ���샂�f���̃p�����[�^�ɃV���{���|�P�����̏��𖄂ߍ���ł���
 */
//==============================================================================
#include <gflib.h>
#include "field/fieldmap.h"
#include "field/fldmmdl.h"
#include "savedata/mystatus.h"

#include "field/field_const.h"
//#include "intrude_work.h"
//#include "intrude_main.h"
#include "../../../resource/fldmapdata/script/symbol_encount_scr_def.h"

#include "symbol_pokemon.h"

#include "waza_tool/wazano_def.h"  //WAZANO_

///�V���{���|�P�����̃��x��
#define SYMBOL_POKE_LEVEL       (30)

//--------------------------------------------------------------
/// �~�j���m���X���샂�f���w�b�_�[
//--------------------------------------------------------------
static const MMDL_HEADER data_MMdlHeader =
{
	0,	          ///<����ID  fldmmdl_code.h
	                //  �X�N���v�g���X�Ԃ̓��샂�f�����A�j���A�Ƃ��������ʂɎg�p�B
	                //  ev�t�@�C����Ŕ��Ȃ���΂悢�B
	BLACKMONOLITH,	///<�\������OBJ�R�[�h fldmmdl_code.h
	MV_DOWN,	      ///<����R�[�h    
	EV_TYPE_NORMAL,	///<�C�x���g�^�C�v
	0,	            ///<�C�x���g�t���O  �^�C�v���Ɏg�p�����t���O�B�O������擾����
	SCRID_SYMBOL_POKEMON,///<�C�x���gID �b�����������ɋN������X�N���v�gID(*.ev �� _EVENT_DATA�̔ԍ�)
	DIR_DOWN,	      ///<�w�����  �O���b�h�ړ��Ȃ��DIR�n �m���O���b�h�̏ꍇ�͈Ⴄ�l
	0,	            ///<�w��p�����^ 0  �^�C�v���Ɏg�p�����p�����^�B�O������擾����
	0,            	///<�w��p�����^ 1
	0,	            ///<�w��p�����^ 2
	MOVE_LIMIT_NOT,	///<X�����ړ�����
	MOVE_LIMIT_NOT,	///<Z�����ړ�����
  MMDL_HEADER_POSTYPE_GRID,
  {0},            ///<�|�W�V�����o�b�t�@ ��check�@�O���b�h�}�b�v�̏ꍇ�͂�����MMDL_HEADER_GRIDPOS
};


#ifdef PM_DEBUG
//==================================================================
/**
 * �~�j���m���X�̓��샂�f����Add
 *
 * @param   fieldmap		
 * @param   grid_x		�O���b�h���WX
 * @param   grid_z		�O���b�h���WZ
 * @param   fx_y		  Y���W(fx32)
 */
//==================================================================
static void SYMBOLPOKE_AdMMdl(
    FIELDMAP_WORK *fieldmap, const SYMBOL_POKEMON * sympoke, u16 id, u16 grid_x, u16 grid_z, fx32 fx_y )
{
  MMDL_HEADER head;
  MMDL_HEADER_GRIDPOS *grid_pos;
  MMDLSYS *mmdl_sys;
  ZONEID zone_id;
  
  mmdl_sys = FIELDMAP_GetMMdlSys( fieldmap );
  zone_id = FIELDMAP_GetZoneID( fieldmap );
  
  head = data_MMdlHeader;
  head.id = id;
  head.obj_code = TPOKE_0001 + sympoke->monsno;
  head.move_code = GFUser_GetPublicRand0(2) == 0 ? MV_RND_V : MV_RND_H;
  //���샂�f���̃p�����[�^�ɃV���{���|�P�����̏��𖄂ߍ���ł���
  head.param0 = sympoke->monsno;
  head.param1 = sympoke->wazano;
  head.param2 = (sympoke->form_no << 2 ) || sympoke->sex;
  grid_pos = (MMDL_HEADER_GRIDPOS *)head.pos_buf;
  
  grid_pos->gx = grid_x;
  grid_pos->gz = grid_z;
  grid_pos->y = fx_y;
  
  MMDLSYS_AddMMdl(mmdl_sys, &head, zone_id);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MMDL * getPokeMMdl( FIELDMAP_WORK * fieldmap, u16 obj_id )
{
  MMDLSYS *mmdlsys = FIELDMAP_GetMMdlSys( fieldmap );
  MMDL *mmdl = MMDLSYS_SearchOBJID( mmdlsys, obj_id );
  GF_ASSERT( mmdl != NULL );
  return mmdl;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static u16 getMonsno( MMDL * poke )
{
  if ( poke )
  {
    return MMDL_GetParam( poke, MMDL_PARAM_0 );
  }
  return 0;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static u16 getFormno( MMDL * poke )
{
  if ( poke )
  {
    u16 para = MMDL_GetParam( poke, MMDL_PARAM_2 );
    return ( para >> 2 );
  }
  return 0;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static u16 getSex( MMDL * poke )
{
  if ( poke )
  {
    u16 para = MMDL_GetParam( poke, MMDL_PARAM_2 );
    return ( para & 0x3 );
  }
  return PTL_SEX_MALE;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static u16 getWaza( MMDL * poke )
{
  if ( poke )
  {
    return MMDL_GetParam( poke, MMDL_PARAM_1 );
  }
  return WAZANO_HATAKU;  //�G���[�Ώ��A�Ƃ肠����
}

//--------------------------------------------------------------
//--------------------------------------------------------------
u16 SYMBOLPOKE_GetMonsno( FIELDMAP_WORK * fieldmap, u16 obj_id )
{
  MMDL * poke = getPokeMMdl( fieldmap, obj_id );
  return getMonsno( poke );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
u16 SYMBOLPOKE_GetFormno( FIELDMAP_WORK * fieldmap, u16 obj_id )
{
  MMDL * poke = getPokeMMdl( fieldmap, obj_id );
  return getFormno( poke );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
u16 SYMBOLPOKE_GetSex( FIELDMAP_WORK * fieldmap, u16 obj_id )
{
  MMDL * poke = getPokeMMdl( fieldmap, obj_id );
  return getSex( poke );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
u16 SYMBOLPOKE_GetWaza( FIELDMAP_WORK * fieldmap, u16 obj_id )
{
  MMDL * poke = getPokeMMdl( fieldmap, obj_id );
  return getWaza( poke );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static POKEMON_PARAM * SYMBOLPOKE_PP_Create(
    HEAPID heapID, u32 oya_id, u16 monsno, u16 formno, int sex, u16 wazano )
{
  POKEMON_PARAM * pp;
  u32 personal_rnd;
  personal_rnd = POKETOOL_CalcPersonalRandEx( oya_id, monsno, formno, sex, 0, FALSE );

  pp = PP_Create( monsno, SYMBOL_POKE_LEVEL, PTL_SETUP_ID_AUTO, heapID );
  PP_SetupEx( pp, monsno, SYMBOL_POKE_LEVEL, oya_id, PTL_SETUP_ID_AUTO, personal_rnd );
  PP_SetTokusei3( pp, monsno, formno );
  if ( wazano )
  {
    PP_Put( pp, ID_PARA_waza1, wazano );
  }
  return pp;
}

//--------------------------------------------------------------
/**
 * @brief
 * @param heapID    �g�p����q�[�v�w��
 * @param fieldmap
 * @param obj_id    �ΏۂƂȂ�|�P������OBJID
 */
//--------------------------------------------------------------
POKEMON_PARAM * SYMBOLPOKE_PP_CreateByObjID( HEAPID heapID, GAMESYS_WORK * gsys, u16 obj_id )
{
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
  MMDL * poke = getPokeMMdl( fieldmap, obj_id );
  if ( poke )
  {
    POKEMON_PARAM * pp;
    u32 personal_rnd, id;
    u16 monsno, formno, sex, wazano;
    monsno = getMonsno( poke );
    formno = getFormno( poke );
    sex = getSex( poke );
    wazano = getWaza( poke );
    id = MyStatus_GetID( GAMEDATA_GetMyStatus( gamedata ) );
    return SYMBOLPOKE_PP_Create( heapID, id, monsno, formno, sex, wazano );
  }
  return NULL;
}

//--------------------------------------------------------------
/**
 * @brief �V���{���|�P�����̓��샂�f����z�u����
 * @param fieldmap
 * @param sympoke   �z�u����V���{���|�P�����̃f�[�^�z��
 * @param poke_num  �z�u����V���{���|�P�����̃f�[�^��
 */
//--------------------------------------------------------------
void SYMBOLPOKE_Add(FIELDMAP_WORK *fieldmap, const SYMBOL_POKEMON * sympoke, int poke_num )
{
  u32 gx = 9, gz = 12;
  u32 gx_space = 3, gz_space = 1;
  int x, y;
  int rnd_x, rnd_z;
  int idx = 0;
  
  for(y = 0; y < 7; y++){
    for(x = 0; x < 4; x++){
      if (idx < poke_num )
      {
        rnd_x = GFUser_GetPublicRand0(2);
        rnd_z = GFUser_GetPublicRand0(2);
        SYMBOLPOKE_AdMMdl(fieldmap, &sympoke[idx], idx,
            gx + gx_space*x + rnd_x, gz + gz_space*y + rnd_z, 0 );
        idx ++;
      }
    }
  }
}
#if 0
//==================================================================
/**
 * 
 *
 * @param   fieldmap		
 */
//==================================================================
void DEBUG_INTRUDE_Pokemon_Add(FIELDMAP_WORK *fieldmap)
{
  static u16 gx = 9, gz = 12;
  static x_count = 0, y_count = 0;
  u32 gx_space = 3, gz_space = 1;
  int rnd_x, rnd_z;
  int monsno;
  
  //monsno = GFUser_GetPublicRand0(MONSNO_ARUSEUSU) + 1;
  if((GFL_UI_KEY_GetCont() & PAD_BUTTON_R)){
    monsno = TPOKE_0348;
  }
  else{
    monsno = TPOKE_0006 + GFUser_GetPublicRand0(200);
  }
  
  rnd_x = GFUser_GetPublicRand0(2);
  rnd_z = GFUser_GetPublicRand0(2);
  SYMBOLPOKE_AdMMdl(fieldmap, gx + gx_space*x_count + rnd_x, 
    gz + gz_space*y_count + rnd_z, 0,
    monsno);
  x_count++;
  if(x_count >= 4){
    x_count = 0;
    y_count++;
  }
}
#endif


#endif  //-------- PM_DEBUG --------

