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

#include "symbol_map.h"
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
//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  u16 move_code;
  u8 limx, limz;
}SYMPOKE_MV_TABLE;

//--------------------------------------------------------------
//--------------------------------------------------------------
static const SYMPOKE_MV_TABLE symPokeMoveCode[] = {
  { MV_DIR_RND, 0, 0 }, // �Œ�i�S���������_���j
  { MV_RND,     1, 1 }, // �����_���ړ��i�����j
  { MV_RND,     2, 2 }, // �����_���ړ��i�L���j
  { MV_RND_V,   0, 2 }, // �㉺�ړ�
  { MV_RND_H,   2, 0 }, // ���E�ړ�
  { MV_RND,     2, 0 }, // ���E�ړ��i�L�����L�����j
  { MV_SPIN_R,  0, 0 }, // �E��]�ړ�
  { MV_SPIN_L,  0, 0 }, // ����]�ړ�
};

//--------------------------------------------------------------
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
//--------------------------------------------------------------
static void SYMBOLPOKE_AdMMdl(
    const POKEADD_WORK * padd,
    const SYMBOL_POKEMON * sympoke, u16 id, u16 grid_x, u16 grid_z, fx32 fx_y )
{
  MMDL_HEADER head;
  MMDL_HEADER_GRIDPOS *grid_pos;
  MMDLSYS *mmdl_sys;
  ZONEID zone_id;
  const SYMPOKE_MV_TABLE * move_data;
  
  move_data = &symPokeMoveCode[ sympoke->move_type ];
  
  mmdl_sys = FIELDMAP_GetMMdlSys( padd->fieldmap );
  zone_id = FIELDMAP_GetZoneID( padd->fieldmap );
  
  head = data_MMdlHeader;
  head.id = id;
  // �����ڔ��f
  head.obj_code = TPOKE_DATA_GetObjCode( padd->tpdata,
      sympoke->monsno, sympoke->sex, sympoke->form_no );
  // �ړ��^�C�v���f
  head.move_code    = move_data->move_code;
  head.move_limit_x = move_data->limx;
  head.move_limit_z = move_data->limz;
  //���샂�f���̃p�����[�^�ɃV���{���|�P�����̏��𖄂ߍ���ł���
  head.param0 = padd->start_no + id;
  head.param1 = *((u32*)sympoke) & 0xffff;
  head.param2 = *((u32*)sympoke) >> 16;

  grid_pos = (MMDL_HEADER_GRIDPOS *)head.pos_buf;
  grid_pos->gx = grid_x;
  grid_pos->gz = grid_z;
  grid_pos->y = fx_y;
  
  MMDLSYS_AddMMdl(mmdl_sys, &head, zone_id);
  TAMADA_Printf("idx=%02d (%2d,%2d)\n", id, grid_x, grid_z );
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
    if ( PP_SetWaza( pp, sympoke->wazano ) == PTL_WAZASET_FAIL )
    {
      PP_SetWazaPush( pp, sympoke->wazano );
    }
  }
  return pp;
}


//--------------------------------------------------------------
//--------------------------------------------------------------
#include "../../../resource/fldmapdata/symbol_map/symbol_map_pos.cdat"
//--------------------------------------------------------------
/**
 * @brief �V���{���|�P�����̓��샂�f����z�u����
 * @param fieldmap
 * @param start_no  �z�u�i���o�[�̊J�n
 * @param sympoke   �z�u����V���{���|�P�����̃f�[�^�z��
 * @param poke_num  �z�u����V���{���|�P�����̃f�[�^��
 *
 * @note
 * KEEP�]�[���̏ꍇ�A�O���P�O�͑傫���|�P�����A�㔼�P�O�͏������|�P������
 * �̈�ŁA�ԂɃ|�P�����������Ă��Ȃ������̃f�[�^�����邱�Ƃ�����̂Œ��ӁB
 *
 * @todo
 * �����鉉�o�����������ǂ��ɂ��ł��Ȃ����H�l����
 */
//--------------------------------------------------------------
void SYMBOLPOKE_Add(
    FIELDMAP_WORK *fieldmap, u32 start_no, const SYMBOL_POKEMON * sympoke, int poke_num,
    SYMBOL_MAP_ID symmap_id )
{
  POKEADD_WORK padd;
  int idx = 0;
  u16 jx, jz;
  MMDL * jiki;

  const u8 * map_pos;

  if ( SYMBOLMAP_IsKeepzoneID( symmap_id ) )
  {
    map_pos = keepMapPos;
  }
  else if ( SYMBOLMAP_IsLargePokeID( symmap_id ) )
  {
    map_pos = largeMapPos;
  }
  else
  {
    map_pos = littleMapPos;
  }

#if 0
  switch ( type )
  {
  case SYMBOL_ZONE_TYPE_KEEP_LARGE:
  case SYMBOL_ZONE_TYPE_KEEP_SMALL:
    map_pos = keepMapPos;
    break;
  case SYMBOL_ZONE_TYPE_FREE_LARGE:
    map_pos = largeMapPos;
    break;
  default:
    GF_ASSERT( 0 );
    /* FALL THROUGH */
  case SYMBOL_ZONE_TYPE_FREE_SMALL:
    map_pos = littleMapPos;
    break;
  }
#endif
  
  padd.tpdata = TPOKE_DATA_Create( FIELDMAP_GetHeapID(fieldmap) );
  padd.fieldmap = fieldmap;
  padd.start_no = start_no;

  jiki = FIELD_PLAYER_GetMMdl( FIELDMAP_GetFieldPlayer( fieldmap ) );
  jx = MMDL_GetGridPosX( jiki );
  jz = MMDL_GetGridPosZ( jiki );

  for ( idx = 0; idx < SYMBOL_MAP_STOCK_MAX; idx ++ )
  {
    if ( sympoke[idx].monsno != 0 )
    {
      int x, z;
      int rnd_x, rnd_z;
      rnd_x = GFUser_GetPublicRand0(2) - 1;
      rnd_z = GFUser_GetPublicRand0(2) - 1;
      x = map_pos[ idx * 2 + 0 ] + rnd_x;
      z = map_pos[ idx * 2 + 1 ] + rnd_z;
      if ( jx == x && jz == z )
      { //���@�Ƃ̏d�Ȃ���������
        if (rnd_x) x -= rnd_x;
        else if (rnd_z) z -= rnd_z;
        else x += 1;
      }
      SYMBOLPOKE_AdMMdl(&padd, &sympoke[idx], idx, x, z, 0 );
      poke_num --;
    }
  }

  TPOKE_DATA_Delete( padd.tpdata );
}



