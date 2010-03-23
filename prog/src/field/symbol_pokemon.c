//==============================================================================
/**
 * @file    symbol_pokemon.c
 * @brief   �V���{���|�P�����z�u����
 * @author  matsuda --> tamada
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
#include "field/tpoke_data.h"


//==============================================================================
//==============================================================================
///�V���{���|�P�����̃��x��
#define SYMBOL_POKE_LEVEL       (30)


typedef struct {
  TPOKE_DATA * tpdata;
  FIELDMAP_WORK * fieldmap;
  u32 start_no;
}POKEADD_WORK;

//==============================================================================
//==============================================================================
//--------------------------------------------------------------
/// ���샂�f���w�b�_�[
//--------------------------------------------------------------
static const MMDL_HEADER data_MMdlHeader =
{
	0,	          ///<����ID  fldmmdl_code.h
	                //  �X�N���v�g���X�Ԃ̓��샂�f�����A�j���A�Ƃ��������ʂɎg�p�B
	                //  ev�t�@�C����Ŕ��Ȃ���΂悢�B
	TPOKE_0001,	///<�\������OBJ�R�[�h fldmmdl_code.h
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


//==============================================================================
//==============================================================================

//==================================================================
/**
 * @brief
 *
 * @param   fieldmap		
 * @param   start_no  �V���{���|�P�����z�u�i���o�[�̊J�nIndex
 * @param   sympoke   ������SYMBOL_POKEMON�f�[�^
 * @param   id        obj_id
 * @param   grid_x		�O���b�h���WX
 * @param   grid_z		�O���b�h���WZ
 * @param   fx_y		  Y���W(fx32)
 */
//==================================================================
static void SYMBOLPOKE_AdMMdl(
    const POKEADD_WORK * padd,
    const SYMBOL_POKEMON * sympoke, u16 id, u16 grid_x, u16 grid_z, fx32 fx_y )
{
  MMDL_HEADER head;
  MMDL_HEADER_GRIDPOS *grid_pos;
  MMDLSYS *mmdl_sys;
  ZONEID zone_id;
  
  mmdl_sys = FIELDMAP_GetMMdlSys( padd->fieldmap );
  zone_id = FIELDMAP_GetZoneID( padd->fieldmap );
  
  head = data_MMdlHeader;
  head.id = id;
  head.obj_code = TPOKE_DATA_GetObjCode( padd->tpdata,
      sympoke->monsno, sympoke->sex, sympoke->form_no );
  head.move_code = GFUser_GetPublicRand0(2) == 0 ? MV_RND_V : MV_RND_H;
  //���샂�f���̃p�����[�^�ɃV���{���|�P�����̏��𖄂ߍ���ł���
  head.param0 = padd->start_no + id;
  head.param1 = *((u32*)sympoke) & 0xffff;
  head.param2 = *((u32*)sympoke) >> 16;
  grid_pos = (MMDL_HEADER_GRIDPOS *)head.pos_buf;
  
  grid_pos->gx = grid_x;
  grid_pos->gz = grid_z;
  grid_pos->y = fx_y;
  
  MMDLSYS_AddMMdl(mmdl_sys, &head, zone_id);
}

//--------------------------------------------------------------
/**
 * @brief �V���{���|�P�����F�p�����[�^�̎擾
 * @param sympoke �p�����[�^���󂯎��|�C���^
 * @param mmdl  �ΏۂƂȂ�|�P�������샂�f���ւ̃|�C���^
 */
//--------------------------------------------------------------
void SYMBOLPOKE_GetParam( SYMBOL_POKEMON * sympoke, const MMDL * mmdl )
{
  u16 param1 = MMDL_GetParam( mmdl, MMDL_PARAM_1 );
  u16 param2 = MMDL_GetParam( mmdl, MMDL_PARAM_2 );
  *( (u32*)sympoke ) = ( ((u32)param1) | ((u32)param2 << 16) );
}

//--------------------------------------------------------------
/**
 * @brief �V���{���|�P�����F�z�u�i���o�[�̎擾
 * @param   mmdl  �ΏۂƂȂ�|�P�������샂�f���ւ̃|�C���^
 * @return  u32   �V���{���|�P�����̃i���o�[
 * SYMBOL_POKE_MAX�ȏ�̏ꍇ�A�ʐM����̃|�P����
 */
//--------------------------------------------------------------
u32 SYMBOLPOKE_GetSymbolNo( const MMDL * mmdl )
{
  return MMDL_GetParam( mmdl, MMDL_PARAM_0 );
}

//--------------------------------------------------------------
/**
 * @brief �V���{���|�P�����p�����[�^����|�P��������
 * @param heapID    �g�p����q�[�v�̎w��
 * @param gamedata  �Q�[���f�[�^�iMyStatus�Q�Ɨp�j
 * @param sympoke   ��������V���{���|�P�����̃p�����[�^
 * @return  POKEMON_PARAM ���������|�P�����f�[�^
 */
//--------------------------------------------------------------
POKEMON_PARAM * SYMBOLPOKE_PP_Create(
    HEAPID heapID, GAMEDATA * gamedata, const SYMBOL_POKEMON * sympoke )
{
  POKEMON_PARAM * pp;
  u32 personal_rnd;
  u32 oya_id;
  oya_id = MyStatus_GetID( GAMEDATA_GetMyStatus( gamedata ) );
  personal_rnd = POKETOOL_CalcPersonalRandEx(
      oya_id, sympoke->monsno, sympoke->form_no, sympoke->sex, 0, FALSE );

  pp = PP_Create( sympoke->monsno, SYMBOL_POKE_LEVEL, PTL_SETUP_ID_AUTO, heapID );
  PP_SetupEx( pp, sympoke->monsno, SYMBOL_POKE_LEVEL, oya_id, PTL_SETUP_ID_AUTO, personal_rnd );
  PP_SetTokusei3( pp, sympoke->monsno, sympoke->form_no );
  if ( sympoke->wazano )
  {
    PP_Put( pp, ID_PARA_waza1, sympoke->wazano );
  }
  return pp;
}

//--------------------------------------------------------------
/**
 * @brief �V���{���|�P�����̓��샂�f����z�u����
 * @param fieldmap
 * @param start_no  �z�u�i���o�[�̊J�n
 * @param sympoke   �z�u����V���{���|�P�����̃f�[�^�z��
 * @param poke_num  �z�u����V���{���|�P�����̃f�[�^��
 */
//--------------------------------------------------------------
void SYMBOLPOKE_Add(
    FIELDMAP_WORK *fieldmap, u32 start_no, const SYMBOL_POKEMON * sympoke, int poke_num )
{
  enum {
    gx = 9,
    gz = 12,
    gx_space = 3,
    gz_space = 1
  };

  POKEADD_WORK padd;
  int x, y;
  int rnd_x, rnd_z;
  int idx = 0;
  
  padd.tpdata = TPOKE_DATA_Create( FIELDMAP_GetHeapID(fieldmap) );
  padd.fieldmap = fieldmap;
  padd.start_no = start_no;

  for(y = 0; y < 7; y++){
    for(x = 0; x < 4; x++){
      if ( sympoke[idx].monsno != 0 )
      {
        rnd_x = GFUser_GetPublicRand0(2);
        rnd_z = GFUser_GetPublicRand0(2);
        SYMBOLPOKE_AdMMdl(&padd, &sympoke[idx], idx,
            gx + gx_space*x + rnd_x, gz + gz_space*y + rnd_z, 0 );
        poke_num --;
      }
      idx ++;
      if (idx >= SYMBOL_MAP_STOCK_MAX ) goto END;
    }
  }
END:

  TPOKE_DATA_Delete( padd.tpdata );
}



