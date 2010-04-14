//==============================================================================
/**
 * @file  symbol_map.c
 * @brief �V���{���G���J�E���g�p�}�b�v�֘A
 * @author  tamada GAME FREAK inc.
 * @date  2010.03.19
 */
//==============================================================================

#include <gflib.h>


#include "savedata/save_control.h"  //SAVE_CONTROL_WORK
#include "gamesystem/gamesystem.h"
#include "savedata/symbol_save.h"   //SYMBOL_POKEMON
#include "savedata/intrude_save.h"
#include "savedata/symbol_save_notwifi.h"

#include "arc/fieldmap/zone_id.h"
#include "field/field_dir.h"

#include "field/intrude_symbol.h"
#include "field_comm/intrude_work.h"  //Intrude_Check_CommConnect

#include "symbol_map.h"
#include "system/main.h"
//==============================================================================
//==============================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
enum {
  SYMMAP_MAX_HEIGHT = 10,
  SYMMAP_MAX_WIDTH = 3,

  SYMMAP_SIZE = SYMMAP_MAX_HEIGHT * SYMMAP_MAX_WIDTH,

  SYMMAP_TOP_ID = 1,    ///<��ԉ���ID
  SYMMAP_ENT_ID = 6,    ///<�������ID

  SYMMAP_IDX_TOP = 1,

  SYMMAP_SMALL_LEVEL_LIMIT = SYMBOL_MAP_LEVEL_SMALL_MAX + 1,
};

//==============================================================================
//==============================================================================
//--------------------------------------------------------------
//  �e�i�K�ł̃}�b�v�\��
//--------------------------------------------------------------
static const u8 map_level1_0[SYMMAP_SIZE] = {
  0,  1,  0,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static const u8 map_level2_0[SYMMAP_SIZE] = {
  0,  1,  0,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static const u8 map_level3_0[SYMMAP_SIZE] = {
  0,  1,  0,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static const u8 map_level4_0[SYMMAP_SIZE] = {
  0,  1,  0,
  17, 18, 19,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static const u8 map_level5_0[SYMMAP_SIZE] = {
  0,  1,  0,
  20, 21, 22,
  17, 18, 19,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static const u8 map_level6_0[SYMMAP_SIZE] = {
  0,  1,  0,
  23, 24, 25,
  20, 21, 22,
  17, 18, 19,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
};
static const u8 map_level7_0[SYMMAP_SIZE] = {
  0,  1,  0,
  26, 27, 28,
  23, 24, 25,
  20, 21, 22,
  17, 18, 19,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
};

static const u8 map_level1_1[SYMMAP_SIZE] = {
  0,  1,  0,
  2,  3,  4,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static const u8 map_level2_1[SYMMAP_SIZE] = {
  0,  1,  0,
  2,  3,  4,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static const u8 map_level3_1[SYMMAP_SIZE] = {
  0,  1,  0,
  2,  3,  4,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static const u8 map_level4_1[SYMMAP_SIZE] = {
  0,  1,  0,
  2,  3,  4,
  17, 18, 19,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
  0,  0,  0,
};
static const u8 map_level5_1[SYMMAP_SIZE] = {
  0,  1,  0,
  2,  3,  4,
  20, 21, 22,
  17, 18, 19,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
  0,  0,  0,
};
static const u8 map_level6_1[SYMMAP_SIZE] = {
  0,  1,  0,
  2,  3,  4,
  23, 24, 25,
  20, 21, 22,
  17, 18, 19,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
  0,  0,  0,
};
static const u8 map_level7_1[SYMMAP_SIZE] = {
  0,  1,  0,
  2,  3,  4,
  26, 27, 28,
  23, 24, 25,
  20, 21, 22,
  17, 18, 19,
  14, 15, 16,
  11, 12, 13,
  8,  9,  10,
  5,  6,  7,
};

static const u8 * const symmap_tables[] = {
  map_level1_0,
  map_level2_0,
  map_level3_0,
  map_level4_0,
  map_level5_0,
  map_level6_0,
  map_level7_0,

  map_level1_1,
  map_level2_1,
  map_level3_1,
  map_level4_1,
  map_level5_1,
  map_level6_1,
  map_level7_1,
};

//--------------------------------------------------------------
//--------------------------------------------------------------

//==============================================================================
//==============================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static inline SYMBOL_MAP_ID LSIDtoSYMMAPID( u8 lsid )
{
  return lsid - 1;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static inline u8 SYMMAPIDtoLSID( SYMBOL_MAP_ID symmap_id )
{
  return symmap_id + 1;
}
//--------------------------------------------------------------
/// �����Ƀ}�b�v�����邩�H
//--------------------------------------------------------------
static inline BOOL hasLeft( const u8 * map, int idx )
{
  return ( (idx % SYMMAP_MAX_WIDTH != 0) && idx != SYMMAP_IDX_TOP );
}
//--------------------------------------------------------------
/// �E���Ƀ}�b�v�����邩�H
//--------------------------------------------------------------
static inline BOOL hasRight( const u8 * map, int idx )
{
  return ( (idx % SYMMAP_MAX_WIDTH != SYMMAP_MAX_WIDTH - 1) && idx != SYMMAP_IDX_TOP );
}
//--------------------------------------------------------------
/// �㑤�Ƀ}�b�v�����邩�H
//--------------------------------------------------------------
static inline BOOL hasUp( const u8 * map, int idx )
{
  return (idx - SYMMAP_MAX_WIDTH >= 0 && map[idx - SYMMAP_MAX_WIDTH] != 0 );
}
//--------------------------------------------------------------
/// �����Ƀ}�b�v�����邩�H
//--------------------------------------------------------------
static inline BOOL hasDown( const u8 * map, int idx )
{
  return ( idx + SYMMAP_MAX_WIDTH < SYMMAP_SIZE && map[idx + SYMMAP_MAX_WIDTH] != 0 );
}
//--------------------------------------------------------------
//  SID����}�b�v�e�[�u���ւ̃C���f�b�N�X���擾����
//--------------------------------------------------------------
static u8 getIndex( const u8 * map, u8 now_lsid )
{
  int i;
#ifdef  PM_DEBUG
  for ( i = 0; i < SYMMAP_SIZE; i+=3 )
  {
    OS_TPrintf( "%2d %2d %2d\n", map[i+0],map[i+1],map[i+2] );
  }
  OS_TPrintf( "search sid == %d\n", now_lsid );
#endif
  for ( i = 0; i < SYMMAP_SIZE; i ++ )
  {
    if ( map[i] == now_lsid ) return i;
  }
  GF_ASSERT(0);
  return SYMMAP_IDX_TOP;
}

//--------------------------------------------------------------
/// SID����Ή�����]�[��ID���擾����
//--------------------------------------------------------------
static u16 getSymbolMapZoneID( const u8 * map, u8 now_lsid )
{
  int idx = getIndex( map, now_lsid );
  BOOL left = hasLeft( map, idx );
  BOOL right = hasRight( map, idx );
  BOOL up = hasUp( map, idx );
  BOOL down = hasDown( map, idx );

  if ( now_lsid == SYMMAP_TOP_ID ) return ZONE_ID_PALACE03;
  if ( now_lsid == SYMMAP_ENT_ID ) return ZONE_ID_PALACE02;

  if ( left && right )
  { //���E���鎞�͏㉺������I
    GF_ASSERT( up && down );
    return ZONE_ID_PALACE07;
  }

  if ( left && !right )
  { //�E�[�̏ꍇ
    GF_ASSERT( up || down );  //�㉺�ǂ��炩�͂���I
    if ( up && down ) return ZONE_ID_PALACE08;
    if ( up )         return ZONE_ID_PALACE10;
    if ( down )       return ZONE_ID_PALACE05;
  }

  if ( !left && right )
  { //���[�̏ꍇ
    GF_ASSERT( up || down );  //�㉺�ǂ��炩�͂���I
    if ( up && down ) return ZONE_ID_PALACE06;
    if ( up )         return ZONE_ID_PALACE09;
    if ( down )       return ZONE_ID_PALACE04;
  }
  GF_ASSERT( 0 );
  return ZONE_ID_PALACE03;  //������̃G���[�����F��Ԑ������ꂽ���̂�Ԃ�
}

//--------------------------------------------------------------
/// ���݂�SID�ƕ������玟�̃}�b�v��SID���擾����
//--------------------------------------------------------------
static u16 getNextMapLSID( const u8 * map, u8 now_lsid, u8 dir_id )
{
  int index = getIndex( map, now_lsid );
  switch ( dir_id )
  {
  case DIR_UP:
    GF_ASSERT( hasUp( map, index ) );
    return map[ index - SYMMAP_MAX_WIDTH ];
  case DIR_DOWN:
    GF_ASSERT( hasDown( map, index ) );
    return map[ index + SYMMAP_MAX_WIDTH ];
  case DIR_LEFT:
    GF_ASSERT( hasLeft( map, index ) );
    return map[ index - 1 ];
  case DIR_RIGHT:
    GF_ASSERT( hasRight( map, index ) );
    return map[ index + 1 ];
  default:
    GF_ASSERT( 0 );
  }
  return now_lsid;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static const u8 * getMapTable( SYMBOL_MAP_LEVEL_LARGE large_lvl, SYMBOL_MAP_LEVEL_SMALL small_lvl )
{
  int index = small_lvl + SYMMAP_SMALL_LEVEL_LIMIT * large_lvl;
  GF_ASSERT( index < NELEMS(symmap_tables) );
  return symmap_tables[index];
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static BOOL getMapLevel( GAMESYS_WORK * gsys, SYMBOL_MAP_LEVEL_SMALL * small, SYMBOL_MAP_LEVEL_LARGE * large )
{
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(gamedata);
  SYMBOL_SAVE_WORK *symbol_save = SymbolSave_GetSymbolData(sv_ctrl);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr( gsys );

  if ( IntrudeSymbol_CheckIntrudeNetID( game_comm, gamedata ) == INTRUDE_NETID_NULL )
  { //�����̃}�b�v
    *small = SymbolSave_GetMapLevelSmall( symbol_save );
    *large = SymbolSave_GetMapLevelLarge( symbol_save );
    return TRUE;
  }
  else
  { //�ʐM����̃}�b�v
    INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect( game_comm );
    INTRUDE_SYMBOL_WORK * isw;
    if ( intcomm == NULL )
    {
      //�ʐM�G���[�Ȃ̂ŁA�Ƃ肠����
      *small = SYMBOL_MAP_LEVEL_SMALL_1;
      *large = SYMBOL_MAP_LEVEL_LARGE_NONE;
      return FALSE;
    }
    isw = IntrudeSymbol_GetSymbolBuffer( intcomm );
    *small = isw->map_level_small;
    *large = isw->map_level_large;
    return TRUE;
  }
}

//--------------------------------------------------------------
/**
 * @brief
 * @param heapID
 * @param gsys
 * @param no
 * @return  INTRUDE_SYMBOL_WORK
 *
 * @todo  �g�����Ȃǂ��l�����INTRUDE_SYMBOL_WORK�ł͂Ȃ��A�Ǝ��̍\���̂�Ԃ��ׂ�
 */
//--------------------------------------------------------------
INTRUDE_SYMBOL_WORK * SYMBOLMAP_AllocSymbolWork( HEAPID heapID, GAMESYS_WORK * gsys, u32 * no )
{
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(gamedata);
  SYMBOL_SAVE_WORK *symbol_save = SymbolSave_GetSymbolData(sv_ctrl);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr( gsys );

  if ( IntrudeSymbol_CheckIntrudeNetID( game_comm, gamedata ) == INTRUDE_NETID_NULL )
  { //�����̃}�b�v
    SYMBOL_MAP_ID symmap_id = GAMEDATA_GetSymbolMapID( gamedata );
    u8 occ_num;
    INTRUDE_SYMBOL_WORK * isw = GFL_HEAP_AllocClearMemory( heapID, sizeof(INTRUDE_SYMBOL_WORK) );
    *no = SymbolSave_GetMapIDSymbolPokemon( symbol_save,
        isw->spoke_array, SYMBOL_MAP_STOCK_MAX, symmap_id, &occ_num );
    isw->num = occ_num;
    isw->map_level_small = SymbolSave_GetMapLevelSmall( symbol_save );
    isw->map_level_large = SymbolSave_GetMapLevelLarge( symbol_save );
    isw->net_id = INTRUDE_NETID_NULL;
    isw->symbol_map_id = symmap_id;
    return isw;
  }
  else
  { //�ʐM����̃}�b�v
    INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect( game_comm );
    if ( intcomm == NULL )
    {
      return NULL; //�ʐM�G���[
    }
    else
    {
      INTRUDE_SYMBOL_WORK * isw = GFL_HEAP_AllocClearMemory( heapID, sizeof(INTRUDE_SYMBOL_WORK) );
      GFL_STD_MemCopy( IntrudeSymbol_GetSymbolBuffer( intcomm ), isw, sizeof( INTRUDE_SYMBOL_WORK ) );
      *no = SYMBOL_POKE_MAX;
      return isw;
    }
  }
}

//==============================================================================
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief �Ή�����]�[����ID���擾
 * @param symmap_id   �V���{���}�b�vID
 * @return  u16   �]�[��ID
 */
//--------------------------------------------------------------
u16 SYMBOLMAP_GetZoneID( GAMESYS_WORK * gsys, SYMBOL_MAP_ID symmap_id )
{
  SYMBOL_MAP_LEVEL_LARGE large_lvl;
  SYMBOL_MAP_LEVEL_SMALL small_lvl;
  const u8 * tbl;
  getMapLevel( gsys, &small_lvl, &large_lvl );
  tbl = getMapTable( large_lvl, small_lvl );

  return getSymbolMapZoneID( tbl, SYMMAPIDtoLSID( symmap_id ) );
}

//--------------------------------------------------------------
/**
 * @brief �ړ���̃V���{���}�b�vID�擾
 * @param now_symmap_id   ���݂̃V���{���}�b�vID
 * @param dir_id          �ړ���}�b�v�̂������
 * @return  SYMBOL_MAP_ID �ړ���̃V���{���}�b�vID
 */
//--------------------------------------------------------------
SYMBOL_MAP_ID SYMBOLMAP_GetNextSymbolMapID(
    GAMESYS_WORK * gsys, SYMBOL_MAP_ID now_symmap_id, u16 dir_id )
{
  SYMBOL_MAP_LEVEL_LARGE large_lvl;
  SYMBOL_MAP_LEVEL_SMALL small_lvl;
  const u8 * tbl;
  u8 next_lsid;

  getMapLevel( gsys, &small_lvl, &large_lvl );
  tbl = getMapTable( large_lvl, small_lvl );

  next_lsid = getNextMapLSID( tbl, SYMMAPIDtoLSID(now_symmap_id), dir_id );
  return LSIDtoSYMMAPID( next_lsid );
}

//--------------------------------------------------------------
/**
 * @brief �L�[�v�]�[�����H�̃`�F�b�N
 * @param symmap_id   �V���{���}�b�vID
 * @return BOOL   TRUE�̂Ƃ��A�L�[�v�]�[���̃}�b�v
 */
//--------------------------------------------------------------
BOOL SYMBOLMAP_IsKeepzoneID( SYMBOL_MAP_ID symmap_id )
{
  return SYMMAPIDtoLSID( symmap_id ) == SYMMAP_TOP_ID;
}

//--------------------------------------------------------------
/**
 * @brief   �p���X�̐X�̓����}�b�v���H�̃`�F�b�N
 * @param symmap_id   �V���{���}�b�vID
 * @return BOOL   TRUE�̎��A�p���X�̐X�̓�����}�b�v
 */
//--------------------------------------------------------------
BOOL SYMBOLMAP_IsEntranceID( SYMBOL_MAP_ID symmap_id )
{
  return SYMMAPIDtoLSID( symmap_id ) == SYMMAP_ENT_ID;
}

