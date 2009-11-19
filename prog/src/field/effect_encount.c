/**
 *  @file   encount_effect.c
 *  @brief  �G���J�E���g�G�t�F�N�g����
 *  @author Miyuki Iwasawa
 *  @date   09.11.17
 */

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "fieldmap.h"
#include "map_attr.h"

#include "field_encount.h"
#include "fldeff_encount.h"

#include "encount_data.h"
#include "field_encount.h"
#include "field_encount_local.h"
#include "effect_encount.h"
#include "map_attr.h"
#include "map_attr_def.h"

#define EFFENC_SEARCH_OX (10)
#define EFFENC_SEARCH_OZ (10)
#define EFFENC_SEARCH_WX  (EFFENC_SEARCH_OX*2+1)
#define EFFENC_SEARCH_WZ  (EFFENC_SEARCH_OZ*2+1)
#define EFFENC_SEARCH_AREA_MAX  (EFFENC_SEARCH_WX*EFFENC_SEARCH_WZ)

#define GRID_MAP_W  (32)

///�G���J�E���g�G�t�F�N�g����
typedef struct _EFFENC_POS{
  u8  valid_f;
  u16 zone_id;
  u8 map_x;
  u8 map_z;
  u16 player_x;
  u16 player_z;
  u16 gx;
  u16 gz;
  EFFENC_TYPE_ID type;

  FLDEFF_TASK* eff_task;
}EFFENC_POS;

typedef struct _EFFENC_ATTR_POS{
  u8  type;
  u16  gx,gz;
  fx32  height;
}EFFENC_ATTR_POS;

typedef struct _EFFENC_SEARCH{
  s16 player_x;
  s16 player_z;
  s16 player_lx;
  s16 player_lz;
  s16 block_x,block_z;
  s16 sx,sz,ex,ez;
}EFFENC_SEARCH;

///�G���J�E���g�G�t�F�N�g�A�g���r���[�g�T���}�b�v
typedef struct _EFFENC_ATTR_MAP{
  u16 count;
  EFFENC_ATTR_POS attr[EFFENC_SEARCH_AREA_MAX];
}EFFENC_ATTR_MAP;

/////////////////////////////////////////////////////
///�G�t�F�N�g�G���J�E���g���䃏�[�N fieldWork�풓
struct _TAG_EFFECT_ENCOUNT{
  EFFENC_POS pos;
  EFFENC_ATTR_MAP attr_map;
  FLDEFF_CTRL* fectrl;
  u32 walk_ct_interval;
};

///////////////////////////////////////////////////////////////////////////////////////////
//�v���g�^�C�v
///////////////////////////////////////////////////////////////////////////////////////////
static BOOL effenc_IsEffectExist( EFFECT_ENCOUNT* eff_wk );
static BOOL effenc_CheckWalkCt( ENCOUNT_WORK* ewk, EFFECT_ENCOUNT* eff_wk );
static void effect_AttributeSearch( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk );
static void effect_AddEffect( EFFECT_ENCOUNT* eff_wk );
static void effect_DelEffect( EFFECT_ENCOUNT* eff_wk );



///////////////////////////////////////////////////////////////////////////////////////////
//�O���[�o���֐��Q
///////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @brief  �G���J�E���g�G�t�F�N�g���[�N����
 */
EFFECT_ENCOUNT* EFFECT_ENC_CreateWork( FIELDMAP_WORK* fwork, HEAPID heapID )
{
  EFFECT_ENCOUNT* eff_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(EFFECT_ENCOUNT));
 
  eff_wk->fectrl = FIELDMAP_GetFldEffCtrl( fwork );
  eff_wk->walk_ct_interval = 10;
  return eff_wk;
}

/*
 *  @brief  �G���J�E���g�G�t�F�N�g���[�N�j��
 */
void EFFECT_ENC_DeleteWork( EFFECT_ENCOUNT* eff_wk )
{
  //�N�����Ȃ�j��
  effect_DelEffect( eff_wk );

  MI_CpuClear8( eff_wk, sizeof(EFFECT_ENCOUNT) );
  GFL_HEAP_FreeMemory( eff_wk );
}

/*
 *  @brief  �G���J�E���g�G�t�F�N�g�N���`�F�b�N
 */
void EFFECT_ENC_CheckEffectEncountStart( FIELD_ENCOUNT* enc )
{
  EFFECT_ENCOUNT* eff_wk = enc->eff_enc;
  ENCOUNT_WORK* ewk = GAMEDATA_GetEncountWork(enc->gdata);

  //�����`�F�b�N
  if( !effenc_CheckWalkCt( ewk, eff_wk ) ){
    return;
  }
  //�O���b�h�x�[�X�}�b�v���`�F�b�N
  if( FIELDMAP_GetBaseSystemType( enc->fwork ) != FLDMAP_BASESYS_GRID ){
    return;
  }

  //�G���J�E���g�f�[�^�`�F�b�N
  if( !enc->encdata->enable_f ){
    return;
  }

  //�A�g���r���[�g�T�[�`
  effect_AttributeSearch( enc, eff_wk );

  //���I
  if( eff_wk->attr_map.count == 0){
    return;
  }
 
  effect_DelEffect( eff_wk );
  effect_AddEffect( eff_wk );
}

/*
 *  @brief  �G�t�F�N�g�G���J�E���g  �����j���@
 */
void EFFECT_ENC_EffectDelete( FIELD_ENCOUNT* enc )
{
  effect_DelEffect( enc->eff_enc );
}

///////////////////////////////////////////////////////////////////////////////////////////
//���[�J���֐��Q
///////////////////////////////////////////////////////////////////////////////////////////
/*
 *  @brief  �������䁕�`�F�b�N
 */
static BOOL effenc_CheckWalkCt( ENCOUNT_WORK* ewk, EFFECT_ENCOUNT* eff_wk )
{
  EWK_EFFECT_ENCOUNT* ewk_eff = &ewk->effect_encount;

  if(ewk_eff->walk_ct < 0xFFFFFFFF){
    ewk_eff->walk_ct++; //�܂��A�I�[�o�[�t���[���邱�Ƃ͂Ȃ����낤���O�̂���max�`�F�b�N
  }

  if( ewk_eff->walk_ct >= eff_wk->walk_ct_interval){
    ewk_eff->walk_ct = 0;
    return TRUE;
  }
  return FALSE;
}

/*
 *  @brief  �G�t�F�N�g�N�������ǂ����`�F�b�N
 */
static BOOL effenc_IsEffectExist( EFFECT_ENCOUNT* eff_wk )
{
  return eff_wk->pos.valid_f;
}

/*
 *  @brief  �G�t�F�N�g�A�g���r���[�g �T�[�`�̈�擾
 */
static void effect_SearchAreaGet( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk, EFFENC_SEARCH* esw )
{
  s16 sx,ex,sz,ez;
  {
    FIELD_PLAYER *fplayer = FIELDMAP_GetFieldPlayer( enc->fwork );
    MMDL* mmdl = FIELD_PLAYER_GetMMdl( fplayer );

    esw->player_x = MMDL_GetGridPosX( mmdl );
    esw->player_z = MMDL_GetGridPosZ( mmdl );
    esw->player_lx = esw->player_x % GRID_MAP_W;
    esw->player_lz = esw->player_z % GRID_MAP_W;
    esw->block_x = esw->player_x / GRID_MAP_W;
    esw->block_z = esw->player_z / GRID_MAP_W;
  }
  sx = esw->player_lx - EFFENC_SEARCH_OX;
  if( sx < 0 ){
    sx = 0;
  }
  ex = esw->player_lx + EFFENC_SEARCH_OX;
  if( ex >= GRID_MAP_W){
    ex = GRID_MAP_W-1;
  }
  sz = esw->player_lz - EFFENC_SEARCH_OZ;
  if( sz < 0 ){
    sz = 0;
  }
  ez = esw->player_lz + EFFENC_SEARCH_OZ;
  if( ez >= GRID_MAP_W){
    ez = GRID_MAP_W-1;
  }

  {
    s16 block;
    block = esw->block_x*32;
    esw->sx = sx + block;
    esw->ex = ex + block;
    block = esw->block_z*32;
    esw->sz = sz + block;
    esw->ez = ez + block;
  }
}

/*
 *  @breif  �A�g���r���[�g�^�C�v�`�F�b�N
 */
static inline EFFENC_TYPE_ID effect_GetAttributeType( MAPATTR attr )
{
  MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( attr );
  MAPATTR_VALUE value = MAPATTR_GetAttrValue( attr );

  if( value == MATTR_BRIDGE_01 ){
    return EFFENC_TYPE_BRIDGE;
  }
  if( !(flag & MAPATTR_FLAGBIT_ENCOUNT)){
    return EFFENC_TYPE_MAX;
  }
  switch(value){
  case MATTR_E_GRASS_LOW:
    return EFFENC_TYPE_SGRASS;

  case MATTR_E_LGRASS_LOW:
    return EFFENC_TYPE_LGRASS;

  case MATTR_E_ZIMEN_01:
    return EFFENC_TYPE_CAVE;

  case MATTR_WATER_01:
  case MATTR_DEEP_MARSH_01:
    return EFFENC_TYPE_WATER;

  case MATTR_SEA_01:
    return EFFENC_TYPE_SEA;
  }
  return EFFENC_TYPE_MAX;
}

/*
 *  @brief  �G�t�F�N�g�A�g���r���[�g�T�[�`
 */
static void effect_AttributeSearch( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk )
{
  int blockIdx;
  s16 i,j;
  VecFx32 vec;
  EFFENC_TYPE_ID id;
  EFFENC_SEARCH esw;
  FLDMAPPER_GRIDINFODATA gridData;
  const FLDMAPPER* g3dMapper = (const FLDMAPPER*)FIELDMAP_GetFieldG3Dmapper( enc->fwork ); 

  //�����̈�擾
  effect_SearchAreaGet( enc, eff_wk, &esw );

  IWASAWA_Printf("EffSearch\n");
  IWASAWA_Printf(" Player(%d,%d), x(%d�`%d), z(%d�`%d)\n",
      esw.player_x,esw.player_z,esw.sx,esw.ex,esw.sz,esw.ez);

  vec.y = 0;
  blockIdx = FLDMAPPER_GetCurrentBlockAccessIdx( g3dMapper );
  for(i = esw.sz; i <= esw.ez;i++){
    vec.z = FX32_CONST(i*16); 
    for(j = esw.sx; j <= esw.ex;j++){
      vec.x = FX32_CONST(j*16); 
      FLDMAPPER_GetGridDataForEffectEncount( g3dMapper, blockIdx, &vec, &gridData );
      IWASAWA_Printf("0x%02x,",gridData.attr&0xFFFF);
      id = effect_GetAttributeType( gridData.attr );
      if(id != EFFENC_TYPE_MAX ){
        eff_wk->attr_map.attr[eff_wk->attr_map.count].type = id;
        eff_wk->attr_map.attr[eff_wk->attr_map.count].gx = j;
        eff_wk->attr_map.attr[eff_wk->attr_map.count].gz = i;
        eff_wk->attr_map.attr[eff_wk->attr_map.count++].height = gridData.height;
      }
    }
    IWASAWA_Printf("\n");
  }
}

/*
 *  @brief  ���I���o�^
 */
static void effect_AddEffect( EFFECT_ENCOUNT* eff_wk )
{
  u16 idx = GFUser_GetPublicRand0( eff_wk->attr_map.count);
  EFFENC_ATTR_POS* attr = &eff_wk->attr_map.attr[idx];
 
  eff_wk->pos.gx = attr->gx;
  eff_wk->pos.gz = attr->gz;
  eff_wk->pos.valid_f = TRUE;
  eff_wk->pos.eff_task = FLDEFF_ENCOUNT_SetEffect( eff_wk->fectrl, attr->gx, attr->gz, attr->height, attr->type );
//  eff_wk->pos.eff_task = FLDEFF_ENCOUNT_SetEffect( eff_wk->fectrl, 788, 715, attr->height, attr->type );  
  IWASAWA_Printf(" EffectAdd( %d, %d, h = 0x%08x) type=%d\n",eff_wk->pos.gx,eff_wk->pos.gz,attr->height, attr->type );
}

/**
 *  @brief  �G�t�F�N�g�폜
 */
static void effect_DelEffect( EFFECT_ENCOUNT* eff_wk )
{
  if( !eff_wk->pos.valid_f ){
    return; //�Ȃɂ����Ȃ�
  }
  FLDEFF_TASK_CallDelete( eff_wk->pos.eff_task );
  MI_CpuClear8( &eff_wk->pos, sizeof(EFFENC_POS));
}

