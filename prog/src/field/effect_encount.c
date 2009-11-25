/**
 *  @file   encount_effect.c
 *  @brief  �G���J�E���g�G�t�F�N�g����
 *  @author Miyuki Iwasawa
 *  @date   09.11.17
 */

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "item/itemsym.h"
#include "fieldmap.h"
#include "map_attr.h"

#include "field_encount.h"
#include "fldeff_encount.h"
#include "script.h"

#include "encount_data.h"
#include "field_encount.h"
#include "field_encount_local.h"
#include "effect_encount.h"
#include "map_attr.h"
#include "map_attr_def.h"

#include "../../../resource/fldmapdata/script/field_ev_scr_def.h" // for SCRID_FLD_EV_EFFECT_ENC_ITEM_GET

///////////////////////////////////////////////////////////////
//
#define EFFENC_SEARCH_OX (10)
#define EFFENC_SEARCH_OZ (10)
#define EFFENC_SEARCH_WX  (EFFENC_SEARCH_OX*2+1)
#define EFFENC_SEARCH_WZ  (EFFENC_SEARCH_OZ*2+1)
#define EFFENC_SEARCH_AREA_MAX  (EFFENC_SEARCH_WX*EFFENC_SEARCH_WZ)

#define GRID_MAP_W  (32)

typedef struct _EFFENC_ATTR_POS{
  u8  type;
  u16  gx,gy,gz;
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
  FLDEFF_CTRL* fectrl;

  EFFENC_ATTR_MAP attr_map;
  FLDEFF_TASK* eff_task;
  
  u16 walk_ct_interval;
  u16 prob;
  u16 get_item; //�X�N���v�g�Ɏ擾�A�C�e��No���󂯓n�����[�N
};

///////////////////////////////////////////////////////////////////////////////////////////
//�v���g�^�C�v
///////////////////////////////////////////////////////////////////////////////////////////
static void effect_EffectPush( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk );
static void effect_EffectPop( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk );

static BOOL effenc_CheckWalkCt( ENCOUNT_WORK* ewk, EFFECT_ENCOUNT* eff_wk );
static void effenc_WalkCtClear( ENCOUNT_WORK* ewk );


static void effect_AttributeSearch( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk );
static void effect_EffectSetUp( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk );
static BOOL effect_CheckMMdlHit( FIELD_ENCOUNT* enc, s16 x,s16 y, s16 z );
static BOOL effect_DeleteCheck( FIELD_ENCOUNT* enc );
static void effect_EffectDelete( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk );

static void effect_AddFieldEffect( EFFECT_ENCOUNT* eff_wk, EFFENC_PARAM* ep );
static void effect_DelFieldEffect( EFFECT_ENCOUNT* eff_wk );

static u16 effitem_GetItemBridge(void);
static u16 effitem_GetItemCave(void);
static GMEVENT* effitem_ItemGetEventSet( FIELD_ENCOUNT* enc, u16 itemno );

///////////////////////////////////////////////////////////////////////////////////////////
//�O���[�o���֐��Q
///////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @brief  �G���J�E���g�G�t�F�N�g���[�N����
 */
EFFECT_ENCOUNT* EFFECT_ENC_CreateWork( HEAPID heapID )
{
  EFFECT_ENCOUNT* eff_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(EFFECT_ENCOUNT));
  
  return eff_wk;
}

/*
 *  @brief  �G���J�E���g�G�t�F�N�g���[�N�j��
 */
void EFFECT_ENC_DeleteWork( EFFECT_ENCOUNT* eff_wk )
{
  MI_CpuClear8( eff_wk, sizeof(EFFECT_ENCOUNT) );
  GFL_HEAP_FreeMemory( eff_wk );
}

/*
 *  @brief  �G�t�F�N�g�G���J�E���g �V�X�e��������
 */
void EFFECT_ENC_Init( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk )
{
  eff_wk->fectrl = FIELDMAP_GetFldEffCtrl( enc->fwork );
#ifdef DEBUG_ONLY_FOR_iwasawa
  eff_wk->walk_ct_interval = 10;
  eff_wk->prob = 100;
#else
  eff_wk->walk_ct_interval = 200;
  eff_wk->prob = 10;
#endif

  //�p�����[�^��Pop
  effect_EffectPop( enc, eff_wk );
}

/*
 *  @brief  �G�t�F�N�g�G���J�E���g�@�V�X�e���I������
 */
void EFFECT_ENC_End( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk )
{
  //�p�����[�^��Push 
  effect_EffectPush( enc, eff_wk );
  
  //�t�B�[���h�G�t�F�N�g�j��
  effect_DelFieldEffect( eff_wk );
}

/*
 *  @brief  �G���J�E���g�G�t�F�N�g�N���`�F�b�N
 */
void EFFECT_ENC_CheckEffectEncountStart( FIELD_ENCOUNT* enc )
{
  EFFECT_ENCOUNT* eff_wk = enc->eff_enc;
  ENCOUNT_WORK* ewk = GAMEDATA_GetEncountWork(enc->gdata);

  //�O���b�h�x�[�X�}�b�v���`�F�b�N
  if( FIELDMAP_GetBaseSystemType( enc->fwork ) != FLDMAP_BASESYS_GRID ){
    return;
  }
  //�N�����`�F�b�N
  if( ewk->effect_encount.param.valid_f ){
    return;
  }
  //�����`�F�b�N
  if( !effenc_CheckWalkCt( ewk, eff_wk ) ){
    return;
  }
  //�m���`�F�b�N
  if( GFUser_GetPublicRand0( 1000 ) >= (eff_wk->prob*10) ){
    effenc_WalkCtClear( ewk );
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
 
  effect_EffectSetUp( enc, eff_wk );
}

/*
 *  @brief  �G�t�F�N�g�G���J�E���g  �����j���@
 */
void EFFECT_ENC_EffectDelete( FIELD_ENCOUNT* enc )
{
  effect_EffectDelete( enc, enc->eff_enc );
}

/*
 *  @brief  �G�t�F�N�g�G���J�E���g�@OBJ�Ƃ̐ڐG�ɂ��G�t�F�N�g�j���`�F�b�N
 */
void EFFECT_ENC_CheckObjHit( FIELD_ENCOUNT* enc )
{
  effect_DeleteCheck( enc );
}

/**
 *  @brief  �G�t�F�N�g�G���J�E���g�@�}�b�v�`�F���W���̏�ԃN���A
 */
void EFFECT_ENC_MapChangeUpdate( FIELD_ENCOUNT* enc )
{
  ENCOUNT_WORK* ewk = GAMEDATA_GetEncountWork(enc->gdata);

  effenc_WalkCtClear( ewk ); 
}

/*
 *  @brief  �G�t�F�N�g�G���J�E���g�@���W�`�F�b�N
 */
static inline BOOL effect_CheckEffectPos( const EFFENC_PARAM* ep, MMDL_GRIDPOS* pos )
{
  if( ep->gx != pos->gx ||
      ep->gz != pos->gz ||
      ep->gy != pos->gy){
    return FALSE;
  }
  return TRUE;
}

/*
 *  @brief  �G�t�F�N�g�G���J�E���g�@���W�`�F�b�N
 */
BOOL EFFECT_ENC_CheckEffectPos( const FIELD_ENCOUNT* enc, MMDL_GRIDPOS* pos )
{
  ENCOUNT_WORK* ewk = GAMEDATA_GetEncountWork(enc->gdata);

  if( ewk->effect_encount.param.valid_f ){
    return FALSE;
  }
  return effect_CheckEffectPos( &ewk->effect_encount.param, pos );
}

/*
 *  @brief  �G�t�F�N�g�G���J�E���g�@�C�x���g�N���N���`�F�b�N
 */
GMEVENT* EFFECT_ENC_CheckEventApproch( FIELD_ENCOUNT* enc )
{
  EFFECT_ENCOUNT* eff_wk = enc->eff_enc;
  ENCOUNT_WORK* ewk = GAMEDATA_GetEncountWork(enc->gdata);
  EFFENC_PARAM* ep = &ewk->effect_encount.param; 
  GMEVENT* event = NULL;
  u8  ev_no = 0;
  u16 rnd;

  if( !ep->valid_f ){
    return NULL;
  }
  { //���W�`�F�b�N
    MMDL_GRIDPOS pos;
    FIELD_PLAYER *fplayer = FIELDMAP_GetFieldPlayer( enc->fwork );
    MMDL* mmdl = FIELD_PLAYER_GetMMdl( fplayer );
    MMDL_GetGridPos( mmdl, &pos );

    if( !effect_CheckEffectPos( ep, &pos )){
      return NULL;
    }
  }
  //�C�x���g����
  rnd = GFUser_GetPublicRand0( 1000 );
  if( ep->type == EFFENC_TYPE_BRIDGE ){
    if( rnd < 200 ){
      event = FIELD_ENCOUNT_CheckEncount( enc, ENC_TYPE_EFFECT );
    }else{
      event = effitem_ItemGetEventSet( enc, effitem_GetItemBridge() );
      ev_no = 1;
    } 
  }else if( ep->type == EFFENC_TYPE_CAVE ){
    if( rnd < 400){
      event = FIELD_ENCOUNT_CheckEncount( enc, ENC_TYPE_EFFECT );
    }else{
      event = effitem_ItemGetEventSet( enc, effitem_GetItemCave() );
      ev_no = 1;
    }
  }else{
    if( rnd < 800 ){
      event = FIELD_ENCOUNT_CheckEncount( enc, ENC_TYPE_EFFECT );
    }else{
      event = FIELD_ENCOUNT_CheckEncount( enc, ENC_TYPE_FORCE );
    }
  }
  GF_ASSERT( event );

  if(ev_no){
    effect_EffectDelete( enc, eff_wk );
  }else{
    ep->push_cancel_f = TRUE;
  }
  return event;
}

//--------------------------------------------------------------------
/**
 * @brief   �G�t�F�N�g�G���J�E�g�A�C�e���擾�C�x���g�Ŏ�ɓ����ItemNo���擾����
 *
 * @retval itemno
 */
//--------------------------------------------------------------------
u16 EFFECT_ENC_GetEffectEncountItem( FIELD_ENCOUNT* enc )
{
  return enc->eff_enc->get_item;
}

/*
 *  @brief  �G�t�F�N�g�G���J�E���g�v�b�V��
 */
static void effect_EffectPush( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk )
{
  ENCOUNT_WORK* ewk = GAMEDATA_GetEncountWork(enc->gdata);
  EFFENC_PARAM* ep = &ewk->effect_encount.param; 

  if( !ep->valid_f ){
    return;
  }
  ep->push_f = TRUE;
}

/*
 *  @brief  �G�t�F�N�g�G���J�E���g�|�b�v 
 */
static void effect_EffectPop( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk )
{
  ENCOUNT_WORK* ewk = GAMEDATA_GetEncountWork(enc->gdata);
  EFFENC_PARAM* ep = &ewk->effect_encount.param; 

  if( !ep->valid_f || !ep->push_f){
    return;
  }

  ep->push_f = FALSE; //�t���O���Ƃ�
  {
    u16 zone_id = FIELDMAP_GetZoneID( enc->fwork );

    if(zone_id != ep->zone_id || ep->push_cancel_f ){
      MI_CpuClear8( ep, sizeof(EFFENC_PARAM));
      return;
    }
  }
  effect_AddFieldEffect( eff_wk, ep );  //�ۑ����ꂽ�p�����[�^�ŕ��A
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
    return TRUE;
  }
  return FALSE;
}
static void effenc_WalkCtClear( ENCOUNT_WORK* ewk )
{
  ewk->effect_encount.walk_ct = 0;
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

  MI_CpuClear8(&eff_wk->attr_map,sizeof(EFFENC_ATTR_MAP));
  vec.y = 0;
  blockIdx = FLDMAPPER_GetCurrentBlockAccessIdx( g3dMapper );
  for(i = esw.sz; i <= esw.ez;i++){
    vec.z = FX32_CONST(i*16); 
    for(j = esw.sx; j <= esw.ex;j++){
      vec.x = FX32_CONST(j*16); 
      FLDMAPPER_GetGridDataForEffectEncount( g3dMapper, blockIdx, &vec, &gridData );
//    IWASAWA_Printf("0x%02x,",gridData.attr&0xFFFF);
      id = effect_GetAttributeType( gridData.attr );
      if(id != EFFENC_TYPE_MAX ){
        eff_wk->attr_map.attr[eff_wk->attr_map.count].type = id;
        eff_wk->attr_map.attr[eff_wk->attr_map.count].gx = j;
        eff_wk->attr_map.attr[eff_wk->attr_map.count].gz = i;
        eff_wk->attr_map.attr[eff_wk->attr_map.count].gy = SIZE_GRID_FX32( gridData.height );
        eff_wk->attr_map.attr[eff_wk->attr_map.count++].height = gridData.height;
      }
    }
//    IWASAWA_Printf("\n");
  }
}

/*
 *  @brief  �G�t�F�N�g���I���o�^
 */
static void effect_EffectSetUp( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk )
{
  u16 idx = GFUser_GetPublicRand0( eff_wk->attr_map.count);
  EFFENC_ATTR_POS* attr = &eff_wk->attr_map.attr[idx];
  ENCOUNT_WORK* ewk = GAMEDATA_GetEncountWork(enc->gdata);
  EFFENC_PARAM* ep = &ewk->effect_encount.param; 

  //�t�B�[���h���f���̈ʒu���`�F�b�N
  if( effect_CheckMMdlHit( enc, attr->gx, attr->gy, attr->gz )){
    IWASAWA_Printf("HitCancel FieldModelHit\n");
    return;
  }

//  effect_EffectDelete( enc, eff_wk );

  //�p�����[�^�Z�b�g
  ep->gx = attr->gx;
  ep->gy = attr->gy;
  ep->gz = attr->gz;
  ep->height = attr->height;
  ep->type = attr->type;
  ep->zone_id = FIELDMAP_GetZoneID( enc->fwork );
  ep->valid_f = TRUE;

  //�J�E���^�[�N���A
  effenc_WalkCtClear( ewk );

  effect_AddFieldEffect( eff_wk, ep );
}

/*
 *  @brief  �G�t�F�N�g���W�`�F�b�N
 */
static BOOL effect_CheckMMdlHit( FIELD_ENCOUNT* enc, s16 x,s16 y, s16 z )
{
  const MMDLSYS* fos;
  MMDL* fplayer = FIELD_PLAYER_GetMMdl( FIELDMAP_GetFieldPlayer( enc->fwork ) );
  MMDL_GRIDPOS pos;
  MMDL* mmdl;
  u32 i = 0;

  fos = FIELDMAP_GetMMdlSys( enc->fwork );
 
  while(TRUE){
    if(!MMDLSYS_SearchUseMMdl( fos, &mmdl, &i)){
      break;
    }
    if( (mmdl == fplayer) || (MMDL_CheckEndAcmd(mmdl) == FALSE) ){
      continue;
    }
    MMDL_GetGridPos(mmdl,&pos);
    if( pos.gx != x ||
        pos.gy != y ||
        pos.gz != z){
      continue;
    }
    return TRUE;
  }
  return FALSE;
}

/*
 *  @brief  �t�B�[���h���f���Ƃ̐ڐG�ɂ��G�t�F�N�g�j��
 */
static BOOL effect_DeleteCheck( FIELD_ENCOUNT* enc )
{
  ENCOUNT_WORK* ewk = GAMEDATA_GetEncountWork(enc->gdata);
  EFFENC_PARAM* ep = &ewk->effect_encount.param; 
  
  if( !ep->valid_f ){
    return FALSE;
  }
  if( effect_CheckMMdlHit( enc, ep->gx, ep->gy, ep->gz )){
    return TRUE;
  }
  return FALSE;
}

/*
 *  @brief  �G�t�F�N�g�j��
 */
static void effect_EffectDelete( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk )
{
  ENCOUNT_WORK* ewk = GAMEDATA_GetEncountWork(enc->gdata);
  EFFENC_PARAM* ep = &ewk->effect_encount.param; 

  effect_DelFieldEffect( eff_wk );

  MI_CpuClear8( ep, sizeof(EFFENC_PARAM));
}

/*
 *  @brief  �t�B�[���h�G�t�F�N�g�o�^
 */
static void effect_AddFieldEffect( EFFECT_ENCOUNT* eff_wk, EFFENC_PARAM* ep )
{
  eff_wk->eff_task = FLDEFF_ENCOUNT_SetEffect( eff_wk->fectrl, ep->gx, ep->gz, ep->height, ep->type );
//  eff_wk->pos.eff_task = FLDEFF_ENCOUNT_SetEffect( eff_wk->fectrl, 788, 715, ep->height, ep->type );  
  IWASAWA_Printf(" EffectAdd( %d, %d, h = 0x%08x) type=%d <0x%08x\n",ep->gx, ep->gz, ep->height, ep->type,eff_wk->eff_task );
}

/**
 *  @brief  �t�B�[���h�G�t�F�N�g�폜
 */
static void effect_DelFieldEffect( EFFECT_ENCOUNT* eff_wk )
{
  if( eff_wk->eff_task == NULL){
    return; //�Ȃɂ����Ȃ�
  }
  IWASAWA_Printf(" EffectDel <0x%08x\n",eff_wk->eff_task );
  FLDEFF_TASK_CallDelete( eff_wk->eff_task );
  eff_wk->eff_task = NULL;
}


///////////////////////////////////////////////////////////////
//�A�C�e�����X�g�f�[�^

#define ITEM_TBL_STONE_NUM (10)
#define ITEM_TBL_JUWEL_NUM (17)
#define ITEM_TBL_FEATHER_NUM (6)
static const u16 DATA_ItemTableStone[ITEM_TBL_STONE_NUM] = {
 ITEM_TAIYOUNOISI,
 ITEM_TUKINOISI,
 ITEM_HONOONOISI,
 ITEM_KAMINARINOISI,
 ITEM_MIZUNOISI,
 ITEM_RIIHUNOISI,
 ITEM_HIKARINOISI,
 ITEM_YAMINOISI,
 ITEM_MEZAMEISI,
 ITEM_MANMARUISI,
};

static u16 effitem_GetItemCave(void)
{
  u16 rnd = GFUser_GetPublicRand0( 1000 );

  if(rnd < 100){ //��
    rnd = GFUser_GetPublicRand0(ITEM_TBL_STONE_NUM*100) / 100;
    return DATA_ItemTableStone[rnd];
  }else if( rnd < 950 ){
    rnd = GFUser_GetPublicRand0(ITEM_TBL_JUWEL_NUM*100) / 100;
    return ITEM_HONOONOZYUERU+rnd;
  }
  return ITEM_KAWARAZUNOISI;
}

static u16 effitem_GetItemBridge(void)
{
  u16 rnd = GFUser_GetPublicRand0( 1000 );

  if( rnd < 900){
    rnd = GFUser_GetPublicRand0(ITEM_TBL_FEATHER_NUM*100) / 100;
    return ITEM_TAIRYOKUNOHANE+rnd;
  }
  return ITEM_HAATONOHANE;
}

static GMEVENT* effitem_ItemGetEventSet( FIELD_ENCOUNT* enc, u16 itemno )
{
  EFFECT_ENCOUNT* eff_wk = enc->eff_enc;
 
  eff_wk->get_item = itemno;
  return SCRIPT_SetEventScript( enc->gsys, SCRID_FLD_EV_EFFECT_ENC_ITEM_GET, NULL, FIELDMAP_GetHeapID( enc->fwork ) );
}


